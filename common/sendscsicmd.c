/*
 * Copyright (C) 1996-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*	send_scsi_cmd - Send a SCSI command to a device */
/*	return	-5	if not supported on this platform (serrno = SEOPNOTSUP)
 *		-4	if SCSI error (serrno = EIO)
 *		-3	if CAM error (serrno = EIO)
 *		-2	if ioctl fails with errno (serrno = errno)
 *		-1	if open/stat fails with errno (message fully formatted)
 *		 0	if successful with no data transfer
 *		>0	number of bytes transferred
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/version.h>
#include <sys/param.h>
/* Impossible unless very very old kernels: */
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
#include SCSIINC
#include <sys/stat.h>
#include "scsictl.h"
#include "serrno.h"
#if defined(TAPE)
#include "Ctape.h"
#define USRMSG(fmt,p,f,msg) \
	{ \
	sprintf (tp_err_msgbuf, fmt, p, f, msg); \
	*msgaddr = tp_err_msgbuf; \
	}
static char tp_err_msgbuf[132];
#else
#define USRMSG(fmt,p,f,msg) {}
#endif
/*static char nosensekey[] = "no sense key available";*/
static char *sk_msg[] = {
        "No sense",
        "Recovered error",
        "Not ready",
        "Medium error",
        "Hardware error",
        "Illegal request",
        "Unit attention",
        "Data protect",
        "Blank check",
        "Vendor unique",
        "Copy aborted",
        "Aborted command",
        "Equal",
        "Volume overflow",
        "Miscompare",
        "Reserved",
};
struct scsi_info {
        int status;
        char *text;
};
struct scsi_info scsi_codmsg[] = {
	{ SCSI_STATUS_CHECK_CONDITION,	    "Check condition"      },
	{ SCSI_STATUS_BUSY,		    "Target busy"          },
	{ SCSI_STATUS_RESERVATION_CONFLICT, "Reservation conflict" },
	{ 0xFF,				    NULL }
};
static char err_msgbuf[132];
#define PROCBUFSZ 80

void find_sgpath(char *sgpath, int maj, int min) {
        
        /*
          Find the sg device for a pair of major and minor device IDs
          of a tape device. The match is done by
          
          . identifying the tape's st device node
          . getting the device's unique ID from sysfs
          . searching the sg device with the same ID (in sysfs)
          
          If no match is found, the returned sg path will be an empty
          string.
        */

        char systape[] = "/sys/class/scsi_tape";
        char sysgen[]  = "/sys/class/scsi_generic";
        char syspath[256];

        char tlink[256];
        char glink[256];

        int match = 0;        
        DIR *dir_tape, *dir_gen;
        struct dirent *dirent;
        char st_dev[64];

        struct stat sbuf;

        sgpath[0] = '\0';

        /* find the st sysfs entry */
        if (!(dir_tape = opendir(systape))) return;  
        while ((dirent = readdir(dir_tape))) {
                
                if (0 == strcmp(".", dirent->d_name)) continue;
                if (0 == strcmp("..", dirent->d_name)) continue;

                sprintf(st_dev, "/dev/%s", dirent->d_name);                
                stat(st_dev, &sbuf);
                if (maj == major(sbuf.st_rdev) && min == minor(sbuf.st_rdev)) {
                        sprintf(syspath, "%s/%s/device", systape, dirent->d_name);
                        match = 1;
                        break;
                }
        }
        closedir(dir_tape);

        if (0 == match) return;

        memset(tlink, 0, 256);
        readlink(syspath, tlink, 256);

        /* find the corresponding sg sysfs entry */
        if (!(dir_gen = opendir(sysgen))) return;
        while ((dirent = readdir(dir_gen))) {
                
                if (0 == strcmp(".", dirent->d_name)) continue;
                if (0 == strcmp("..", dirent->d_name)) continue;

                sprintf(syspath, "%s/%s/device", sysgen, dirent->d_name);
                
                memset(glink, 0, 256);
                readlink(syspath, glink, 256);
                
                if (0 == strcmp(glink, tlink)) {
                        sprintf(sgpath, "/dev/%s", dirent->d_name);
                        goto out;
                }
        }
 out:
        closedir(dir_gen);
        return;
}


int send_scsi_cmd (tapefd, path, do_not_open, cdb, cdblen, buffer, buflen, sense, senselen, timeout, flags, nb_sense_ret, msgaddr)
int tapefd;
char *path;
int do_not_open;
unsigned char *cdb;
int cdblen;
unsigned char *buffer;
int buflen;
char *sense;
int senselen;
int timeout;	/* in milliseconds */
int flags;
int *nb_sense_ret;
char **msgaddr;
{
	/* int i;*/
	int fd;
	FILE *fopen();
	int n;
	int resid = 0;
	struct stat sbuf;
	struct stat sbufa;
	static char *sg_buffer;
	static int sg_bufsiz = 0;
	struct sg_header *sg_hd;
	char sgpath[80];
	int timeout_in_jiffies = 0;
	int sg_big_buff_val =  SG_BIG_BUFF;
	int procfd, nbread;
	char procbuf[PROCBUFSZ];
                                                                                                                                                            
	/* First the value in /proc of the max buffer size for the sg driver */
	procfd = open("/proc/scsi/sg/def_reserved_size", O_RDONLY);
	if (procfd > 0) {
	  memset(procbuf, 0, PROCBUFSZ);
	  nbread = read(procfd, procbuf, PROCBUFSZ -1);
	  if (nbread > 0) {
	    long int tmp;
	    char *endptr = NULL;
	    tmp = strtol(procbuf, &endptr, 10);
	    if (endptr == NULL || *endptr == '\n') {
	      sg_big_buff_val = (int) tmp;
	    }
	  }
	  close(procfd);
	}

	if (sizeof(struct sg_header) + cdblen + buflen > sg_big_buff_val) {
#if defined(TAPE)
		sprintf (tp_err_msgbuf, "blocksize too large (max %ld)\n",
		    sg_big_buff_val - sizeof(struct sg_header) - cdblen);
		*msgaddr = tp_err_msgbuf;
#else
		sprintf (err_msgbuf, "blocksize too large (max %lu)",
		    sg_big_buff_val - sizeof(struct sg_header) - cdblen);
		*msgaddr = err_msgbuf;
#endif
		serrno = EINVAL;
		return (-1);
	}
	if (sizeof(struct sg_header)+cdblen+buflen > sg_bufsiz) {
		if (sg_bufsiz > 0) free (sg_buffer);
		if ((sg_buffer = malloc (sizeof(struct sg_header)+cdblen+buflen)) == NULL) {
			serrno = errno;
#if defined(TAPE)
			sprintf (tp_err_msgbuf, TP005);
			*msgaddr = tp_err_msgbuf;
#else
			sprintf (err_msgbuf, "malloc error: %s", strerror(errno));
			*msgaddr = err_msgbuf;
#endif
			return (-1);
		}
		sg_bufsiz = sizeof(struct sg_header) + cdblen + buflen;
	}
	if (do_not_open) {
		fd = tapefd;
		strcpy (sgpath, path);
	} else {
		if (stat (path, &sbuf) < 0) {
			serrno = errno;
#if defined(TAPE)
			USRMSG (TP042, path, "stat", strerror(errno));
#else
			sprintf (err_msgbuf, "stat error: %s", strerror(errno));
			*msgaddr = err_msgbuf;
#endif
			return (-1);
		}

                /* get the major device ID of the sg devices ... */
		if (stat ("/dev/sg0", &sbufa) < 0) {
			serrno = errno;
#if defined(TAPE)
			USRMSG (TP042, "/dev/sg0", "stat", strerror(errno));
#else
			sprintf (err_msgbuf, "stat error: %s", strerror(errno));
			*msgaddr = err_msgbuf;
#endif
			return (-1);
		}
                /* ... to detect links and use the path directly! */
		if (major(sbuf.st_rdev) == major(sbufa.st_rdev)) {
			strcpy (sgpath, path);
		} else {
                        find_sgpath(sgpath, major(sbuf.st_rdev), minor(sbuf.st_rdev));  
		}

		if ((fd = open (sgpath, O_RDWR)) < 0) {
			serrno = errno;
#if defined(TAPE)
			USRMSG (TP042, sgpath, "open", strerror(errno));
#else
			sprintf (err_msgbuf, "open error: %s", strerror(errno));
			*msgaddr = err_msgbuf;
#endif
			return (-1);
		}
	}

        /* set the sg timeout (in jiffies) */
        timeout_in_jiffies = timeout * HZ / 1000;
        ioctl (fd, SG_SET_TIMEOUT, &timeout_in_jiffies);

	memset (sg_buffer, 0, sizeof(struct sg_header));
	sg_hd = (struct sg_header *) sg_buffer;
	sg_hd->reply_len = sizeof(struct sg_header) + ((flags & SCSI_IN) ? buflen : 0);
	sg_hd->twelve_byte = cdblen == 12;
	memcpy (sg_buffer+sizeof(struct sg_header), cdb, cdblen);
	n = sizeof(struct sg_header) + cdblen;
	if (buflen && (flags & SCSI_OUT)) {
		memcpy (sg_buffer+n, buffer, buflen);
		n+= buflen;
	}
	if (write (fd, sg_buffer, n) < 0) {
		*msgaddr = (char *) strerror(errno);
		serrno = errno;
		USRMSG (TP042, sgpath, "write", *msgaddr);
		if (! do_not_open) close (fd);
		return (-2);
	}
	if ((n = read (fd, sg_buffer, sizeof(struct sg_header) +
	    ((flags & SCSI_IN) ? buflen : 0))) < 0) {
		*msgaddr = (char *) strerror(errno);
		serrno = errno;
		USRMSG (TP042, sgpath, "read", *msgaddr);
		if (! do_not_open) close (fd);
		return (-2);
	}
	if (! do_not_open) close (fd);
	if (sg_hd->sense_buffer[0]) {
		memcpy (sense, sg_hd->sense_buffer, sizeof(sg_hd->sense_buffer));
		*nb_sense_ret = sizeof(sg_hd->sense_buffer);
	}
	if (sg_hd->sense_buffer[0] & 0x80) {	/* valid */
		resid = sg_hd->sense_buffer[3] << 24 | sg_hd->sense_buffer[4] << 16 |
		    sg_hd->sense_buffer[5] << 8 | sg_hd->sense_buffer[6];
	}
	if ((sg_hd->sense_buffer[0] & 0x70) &&
	    ((sg_hd->sense_buffer[2] & 0xE0) == 0 ||
	    (sg_hd->sense_buffer[2] & 0xF) != 0)) {
		sprintf (err_msgbuf, "%s ASC=%X ASCQ=%X",
		    sk_msg[*(sense+2) & 0xF], *(sense+12), *(sense+13));
		*msgaddr = err_msgbuf;
		serrno = EIO;
		USRMSG (TP042, sgpath, "scsi", *msgaddr);
		return (-4);
	} else if (sg_hd->result) {
		*msgaddr = (char *) strerror(sg_hd->result);
		serrno = sg_hd->result;
		USRMSG (TP042, sgpath, "read", *msgaddr);
		return (-2);
	}
	if (n)
		n -= sizeof(struct sg_header) + resid;
	if (n && (flags & SCSI_IN))
		memcpy (buffer, sg_buffer+sizeof(struct sg_header), n);
	return ((flags & SCSI_IN) ? n : buflen - resid);
}


void get_ss_msg(scsi_status, msgaddr)
int scsi_status;
char **msgaddr;
{
	int i;

	for (i = 0; i < 256; i++) {
		if (scsi_codmsg[i].status == 0xFF) {
			sprintf (err_msgbuf,
			    "Undefined SCSI status %02X", scsi_status);
			*msgaddr = err_msgbuf;
			break;
		} else if (scsi_status == scsi_codmsg[i].status) {
			*msgaddr = scsi_codmsg[i].text;
			break;
		}
	}
}
