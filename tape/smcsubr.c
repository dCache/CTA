/*
 * Copyright (C) 1998-2001 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
#ifndef lint
static char sccsid[] = "@(#)$RCSfile: smcsubr.c,v $ $Revision: 1.4 $ $Date: 2001/09/19 12:57:09 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "Ctape.h"
#include "scsictl.h"
#include "serrno.h"
#include "smc.h"
#define	RBT_XTRA_PROC 10
static struct smc_status smc_status;
static char *smc_msgaddr;

static void
save_error(rc, nb_sense, sense, msgaddr)
int rc;
int nb_sense;
unsigned char *sense;
char *msgaddr;
{
	smc_msgaddr = msgaddr;
	smc_status.rc = rc;
	smc_status.skvalid = 0;
	smc_status.save_errno = serrno;
	if (rc == -4 && nb_sense >= 14) {
		smc_status.asc = sense[12];
		smc_status.ascq = sense[13];
		smc_status.sensekey = sense[2] &0xF;
		smc_status.skvalid = 1;
	} else {
		smc_status.asc = 0;
		smc_status.ascq = 0;
		smc_status.sensekey = 0;
	}
}

static int
get_element_size(fd, rbtdev, type)
int fd;
char *rbtdev;
int type;
{
	unsigned char buf[16];
	unsigned char cdb[12];
	char *msgaddr;
	int nb_sense_ret;
	int rc;
	char sense[MAXSENSE];
 
 	memset (cdb, 0, sizeof(cdb));
 	cdb[0] = 0xB8;		/* read element status */
 	cdb[1] = 0x10 + type;
 	cdb[5] = 0;
 	cdb[9] = 16;
 	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 12, buf, 16,
 		sense, 38, 60000, SCSI_IN, &nb_sense_ret, &msgaddr);
 	if (rc < 0) {
 		save_error (rc, nb_sense_ret, sense, msgaddr);
 		return (-1);
 	}
	return (buf[10] * 256 + buf[11]);
}

static int
get_element_info(opcode, fd, rbtdev, type, start, nbelem, element_info)
char opcode;
int fd;
char *rbtdev;
int type;
int start;
int nbelem;
struct smc_element_info element_info[];
{
	int avail_elem;
	unsigned char cdb[12];
	unsigned char *data;
	int edl;
	int element_size;
	char func[16];
	int i;
	int len;
	char *msgaddr;
	int nb_sense_ret;
	unsigned char *p;
	unsigned char *page_end, *page_start;
	unsigned char *q;
	int rc;
	char sense[MAXSENSE];

	strcpy (func, "get_elem_info");
	if (type) {
		element_size = get_element_size (fd, rbtdev, type);
		if (element_size < 0) return (-1);
	} else {
		element_size = get_element_size (fd, rbtdev, 1); /* transport */
		if (element_size < 0) return (-1);
		i = get_element_size (fd, rbtdev, 2);	/* slot */
		if (i < 0) return (-1);
		if (i > element_size) element_size = i;
		i = get_element_size (fd, rbtdev, 3);	/* port */
		if (i < 0) return (-1);
		if (i > element_size) element_size = i;
		i = get_element_size (fd, rbtdev, 4);	/* device */
		if (i < 0) return (-1);
		if (i > element_size) element_size = i;
	}
	len = nbelem * element_size + 8;
	if (type != 0 || nbelem == 1)
		len += 8;	/* one element header */
	else
		len += 32;	/* possibly four element headers */
	data = malloc (len);
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = opcode;	/* read element status or request volume element address */
	cdb[1] = 0x10 + type;
	cdb[2] = start >> 8;
	cdb[3] = start & 0xFF;
	cdb[4] = nbelem >> 8;
	cdb[5] = nbelem & 0xFF;
	cdb[7] = len >> 16;
	cdb[8] = (len >> 8) & 0xFF;
	cdb[9] = len & 0xFF;
	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 12, data, len,
		sense, 38, 60000, SCSI_IN, &nb_sense_ret, &msgaddr);
	if (rc < 0) {
		save_error (rc, nb_sense_ret, sense, msgaddr);
		free (data);
		return (-1);
	}
	avail_elem = *(data+2) * 256 + *(data+3);
	i = 0;
	p = data + 8;			/* point after data header */
	while (i < avail_elem) {
		edl = *(p+2) * 256 + *(p+3);
		page_start = p + 8;	/* point after page header */
		page_end = page_start +
			(((*(p+5) * 256 + *(p+6)) * 256) + *(p+7));
		if (page_end > (data + len)) page_end = data + len;
		for (p = page_start; p < page_end && i < avail_elem; p += edl, i++) {
			element_info[i].element_address = *p * 256 + *(p+1);
			element_info[i].element_type = *(page_start-8);
			element_info[i].state = *(p+2);
			element_info[i].asc = *(p+4);
			element_info[i].ascq = *(p+5);
			element_info[i].source_address = *(p+10) * 256 + *(p+11);
			if (*(p+12) == '\0')
				element_info[i].name[0] = '\0';
			else {
				q = (unsigned char *) strchr ((char *)p+12, ' ');
				strncpy (element_info[i].name, (char *)p+12, q-p-12);
				element_info[i].name[q-p-12] = '\0';
			}
		}
	}
	free (data);
	return (avail_elem);
}

smc_find_cartridge(fd, rbtdev, template, type, start, nbelem, element_info)
int fd;
char *rbtdev;
int type;
char *template;
int start;
int nbelem;
struct smc_element_info element_info[];
{
	unsigned char cdb[12];
	char func[16];
	char *msgaddr;
	int nb_sense_ret;
	char plist[40];
	int rc;
	char sense[MAXSENSE];

	ENTRY (find_cartridge);
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = 0xB6;		/* send volume tag */
	cdb[1] = type;
	cdb[2] = start >> 8;
	cdb[3] = start & 0xFF;
	cdb[5] = 5;
	cdb[9] = 40;
	memset (plist, 0, sizeof(plist));
	strcpy (plist, template);
	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 12, plist, 40,
		sense, 38, 60000, SCSI_OUT, &nb_sense_ret, &msgaddr);
	if (rc < 0) {
		save_error (rc, nb_sense_ret, sense, msgaddr);
		RETURN (-1);
	}
	rc = get_element_info (0xB5, fd, rbtdev, type, start, nbelem, element_info);
	RETURN (rc);
}

smc_get_geometry(fd, rbtdev, robot_info)
int fd;
char *rbtdev;
struct robot_info *robot_info;
{
	unsigned char buf[36];
	unsigned char cdb[6];
	char func[16];
	char *msgaddr;
	int nb_sense_ret;
	int rc;
	char sense[MAXSENSE];

	ENTRY (get_geometry);
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = 0x12;		/* inquiry */
	cdb[4] = 36;
	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 6, buf, 36,
		sense, 38, 30000, SCSI_IN, &nb_sense_ret, &msgaddr);
	if (rc < 0) {
		save_error (rc, nb_sense_ret, sense, msgaddr);
		RETURN (-1);
	}
	memcpy (robot_info->inquiry, buf+8, 28);
	robot_info->inquiry[28] = '\0';
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = 0x1A;		/* mode sense */
	cdb[2] = 0x1D;		/* element address assignment page */
	cdb[4] = 24;
	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 6, buf, 24,
		sense, 38, 180000, SCSI_IN, &nb_sense_ret, &msgaddr);
	if (rc < 0) {
		save_error (rc, nb_sense_ret, sense, msgaddr);
		RETURN (-1);
	}
	robot_info->transport_start = buf[6] * 256 + buf[7];
	robot_info->transport_count = buf[8] * 256 + buf[9];
	robot_info->slot_start = buf[10] * 256 + buf[11];
	robot_info->slot_count = buf[12] * 256 + buf[13];
	robot_info->port_start = buf[14] * 256 + buf[15];
	robot_info->port_count = buf[16] * 256 + buf[17];
	robot_info->device_start = buf[18] * 256 + buf[19];
	robot_info->device_count = buf[20] * 256 + buf[21];
	RETURN (0);
}

/* SCSI 3 additional sense code and additional sense qualifier */
struct scsierr_codact {
	unsigned char sensekey;
	unsigned char asc;
	unsigned char ascq;
	short action;
	char *txt;
};
struct scsierr_codact scsierr_acttbl[] = {
    {0x02, 0x04, 0x00, RBT_FAST_RETRY, "Logical Unit Not Ready, Cause Not Reportable"},
    {0x02, 0x04, 0x01, RBT_FAST_RETRY, "Logical Unit Is In Process of Becoming Ready"},
    {0x02, 0x04, 0x02, RBT_OMSGR, "Logical Unit Not Ready, initialization required"},
    {0x02, 0x04, 0x03, RBT_OMSGR, "Logical Unit Not Ready, Manual Intervention Required"},
    {0x0B, 0x08, 0x00, RBT_OMSGR, "Logical Unit Communication Failure"},
    {0x0B, 0x08, 0x01, RBT_OMSGR, "Logical Unit Communication Time-out"},
    {0x05, 0x1A, 0x00, RBT_NORETRY, "Parameter List Length Error"},
    {0x05, 0x20, 0x00, RBT_NORETRY, "Invalid Command Operation Code"},
    {0x05, 0x21, 0x01, RBT_NORETRY, "Invalid Element Address"},
    {0x05, 0x24, 0x00, RBT_NORETRY, "Invalid field in CDB"},
    {0x05, 0x25, 0x00, RBT_NORETRY, "Logical Unit Not Supported"},
    {0x05, 0x26, 0x00, RBT_NORETRY, "Invalid field in Parameter List"},
    {0x05, 0x26, 0x01, RBT_NORETRY, "Parameter Not Supported"},
    {0x05, 0x26, 0x02, RBT_NORETRY, "Parameter Value Invalid"},
    {0x06, 0x28, 0x00, RBT_FAST_RETRY, "Not Ready to Ready Transition"},
    {0x06, 0x28, 0x01, RBT_FAST_RETRY, "Import or Export Element Accessed"},
    {0x06, 0x29, 0x00, RBT_FAST_RETRY, "Power On, Reset, or Bus Device Reset Occurred"},
    {0x06, 0x2A, 0x01, RBT_FAST_RETRY, "Mode Parameters Changed"},
    {0x05, 0x30, 0x00, RBT_NORETRY, "Incompatible Medium Installed"},
    {0x00, 0x30, 0x03, RBT_FAST_RETRY, "Cleaning Cartridge Installed"},
    {0x05, 0x39, 0x00, RBT_NORETRY, "Saving Parameters Not Supported"},
    {0x05, 0x3A, 0x00, RBT_XTRA_PROC, "Medium Not Present"},
    {0x05, 0x3B, 0x0D, RBT_XTRA_PROC, "Medium Destination Element Full"},
    {0x05, 0x3B, 0x0E, RBT_XTRA_PROC, "Medium Source Element Empty"},
    {0x04, 0x40, 0x01, RBT_OMSGR, "Hardware Error, General"},
    {0x04, 0x40, 0x02, RBT_OMSGR, "Hardware Error, Tape Transport"},
    {0x04, 0x40, 0x03, RBT_OMSGR, "Hardware Error, CAP"},
    {0x0B, 0x43, 0x00, RBT_NORETRY, "Message Error"},
    {0x02, 0x44, 0x00, RBT_OMSGR, "Internal Target Failure"},
    {0x0B, 0x45, 0x00, RBT_OMSGR, "Select or Reselect Failure"},
    {0x0B, 0x47, 0x00, RBT_OMSGR, "SCSI Parity Error"},
    {0x0B, 0x48, 0x00, RBT_OMSGR, "Initiator Detected Error"},
    {0x02, 0x4C, 0x00, RBT_OMSGR, "Logical Unit Failed Self-Configuration"},
    {0x05, 0x4E, 0x00, RBT_NORETRY, "Overlapped Commands Attempted"},
    {0x05, 0x53, 0x02, RBT_OMSGR, "Medium Removal Prevented"},
    {0x06, 0x54, 0x00, RBT_OMSGR, "SCSI To Host System Interface Failure"},
    {0x02, 0x5A, 0x01, RBT_OMSGR, "Operator Medium Removal Request"}
};

smc_lasterror(smc_stat, msgaddr)
struct smc_status *smc_stat;
char **msgaddr;
{
	int i;

	smc_stat->rc = smc_status.rc;
	smc_stat->skvalid = smc_status.skvalid;
	*msgaddr = smc_msgaddr;
	if ((smc_status.rc == -1 || smc_status.rc == -2) &&
	    smc_status.save_errno == EBUSY)
		return (EBUSY);
	if (! smc_status.skvalid)
		return (RBT_NORETRY);
	smc_stat->sensekey = smc_status.sensekey;
	smc_stat->asc = smc_status.asc;
	smc_stat->ascq = smc_status.ascq;
	for (i = 0; i < sizeof(scsierr_acttbl)/sizeof(struct scsierr_codact); i++) {
		if (smc_status.asc == scsierr_acttbl[i].asc &&
		    smc_status.ascq == scsierr_acttbl[i].ascq &&
		    smc_status.sensekey == scsierr_acttbl[i].sensekey) {
			*msgaddr = scsierr_acttbl[i].txt;
			return (scsierr_acttbl[i].action);
		}
	}
	return (RBT_NORETRY);
}

smc_move_medium(fd, rbtdev, from, to)
int fd;
char *rbtdev;
int from;
int to;
{
	unsigned char cdb[12];
	char func[16];
	char *msgaddr;
	int nb_sense_ret;
	int rc;
	char sense[MAXSENSE];

	ENTRY (move_medium);
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = 0xA5;		/* move medium */
	cdb[4] = from >> 8;
	cdb[5] = from & 0xFF;
	cdb[6] = to >> 8;
	cdb[7] = to & 0xFF;
	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 12, NULL, 0,
		sense, 38, 300000, SCSI_NONE, &nb_sense_ret, &msgaddr);
	if (rc < 0) {
		save_error (rc, nb_sense_ret, sense, msgaddr);
		RETURN (-1);
	}
	RETURN (0);
}

smc_read_elem_status(fd, rbtdev, type, start, nbelem, element_info)
int fd;
char *rbtdev;
int type;
int start;
int nbelem;
struct smc_element_info element_info[];
{
	char func[16];
	int rc;

	ENTRY (read_elem_statu);
	rc = get_element_info (0xB8, fd, rbtdev, type, start, nbelem, element_info);
	RETURN (rc);
}

smc_ready_inport(fd, rbtdev, port)
int fd;
char *rbtdev;
int port;
{
	unsigned char cdb[6];
	char func[16];
	char *msgaddr;
	int nb_sense_ret;
	int rc;
	char sense[MAXSENSE];

	ENTRY (ready_inport);
	memset (cdb, 0, sizeof(cdb));
	cdb[0] = 0xDE;		/* ready inport */
	cdb[2] = port >> 8;
	cdb[3] = port & 0xFF;
	rc = send_scsi_cmd (fd, rbtdev, 0, cdb, 6, NULL, 0,
		sense, 38, 30000, SCSI_NONE, &nb_sense_ret, &msgaddr);
	if (rc < 0) {
		save_error (rc, nb_sense_ret, sense, msgaddr);
		RETURN (-1);
	}
	RETURN (0);
}
