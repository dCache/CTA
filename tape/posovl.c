/*
 * Copyright (C) 1990-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include "Ctape.h"
#include "Ctape_api.h"
#include "marshall.h"
#include "serrno.h"
#include "tplogger_api.h"
char *devtype;
char errbuf[512];
char func[16];
gid_t gid;
char hostname[CA_MAXHOSTNAMELEN+1];
int jid;
char *path;
fd_set readmask;
int rpfd;
int tapefd;
uid_t uid;
int main(int	argc,
         char	**argv)
{
	char actual_hdr1[81];
	int blksize;
	unsigned char blockid[4];
	unsigned int blockid_tmp[4];
	int c;
	int cfseq;
	int den;
	char *domainname;
	char *drive;
	unsigned char drive_serial_no[13];
	char fid[CA_MAXFIDLEN+1];
	int filstat;
	int flags;
	int fsec;
	int fseq;
	char *fsid;
	char hdr1[LBLBUFSZ];
	char hdr2[LBLBUFSZ];
	int i;
	unsigned char inq_data[29];
	int lblcode;
	int lrecl;
	int method;
	int mode;
	int msglen;
	char *name;
	char *p;
	char *q;
	int Qfirst;
	int Qlast;
	char repbuf[REPBUFSZ];
	int retentd;
	char *sbp;
	int scsi;
	char sendbuf[REQBUFSZ];
	char tpfid[CA_MAXFIDLEN+1];
	char uhl1[LBLBUFSZ];
	int ux;
	char *vid;
	char vol1[LBLBUFSZ];
	char *vsn;

	void cleanup();
	void positkilled();
        void configdown( char* );
        char *getconfent();

	ENTRY (posovl);

        p = getconfent ("TAPE", "TPLOGGER", 0);
        if (p && (0 == strcasecmp(p, "SYSLOG"))) {
                tl_init_handle( &tl_tpdaemon, "syslog" ); 
        } else {
                tl_init_handle( &tl_tpdaemon, "dlf" );  
        }
        tl_tpdaemon.tl_init( &tl_tpdaemon, 0 );

  if (32 != argc) {
    printf("Wrong number of arguments\n");
    exit(-1);
  }

	drive = argv[1];
	vid = argv[2];
	rpfd = atoi(argv[3]);
	uid = atoi (argv[4]);
	gid = atoi (argv[5]);
	name = argv[6];
	jid = atoi (argv[7]);
	ux = atoi(argv[8]);
	/* dgn = argv[9]; not used */
	devtype = argv[10];
	/* The driver name in argv[11] is no longer used */
	mode = atoi (argv[12]);
	lblcode = atoi (argv[13]);
	vsn = argv[14];

	sscanf (argv[15], "%02x%02x%02x%02x", &blockid_tmp[0], &blockid_tmp[1],
	    &blockid_tmp[2], &blockid_tmp[3]);
	for (i = 0; i < 4; i++)
	  blockid[i] = blockid_tmp[i];

	cfseq = atoi (argv[16]);
	strcpy (fid, argv[17]);
	filstat = atoi (argv[18]);
	fsec = atoi (argv[19]);
	fseq = atoi (argv[20]);
	method = atoi (argv[21]);
	path = argv[22];
	Qfirst = atoi (argv[23]);
	Qlast = atoi (argv[24]);
	retentd = atoi (argv[25]);

	blksize = atoi (argv[26]);
	lrecl = atoi (argv[27]);
	den = atoi (argv[28]);
	flags = atoi (argv[29]);
	fsid = argv[30];
	domainname = argv[31];
 
	scsi = 1;

	c = 0;
	(void) Ctape_seterrbuf (errbuf, sizeof(errbuf));
	gethostname (hostname, CA_MAXHOSTNAMELEN+1);

	signal (SIGINT, positkilled);

	/* open device and check drive ready */

		if (!scsi)
			tapefd = open (path, O_RDONLY);
		else
			tapefd = open (path, O_RDONLY|O_NDELAY);
		if (tapefd < 0) {
			c = errno;
			if (errno == ENXIO)	/* drive not operational */
				configdown (drive);
			else {
				usrmsg (func, TP042, path, "open",
					strerror(errno));
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 42, 6,
                                                    "func"   , TL_MSG_PARAM_STR  , func,
                                                    "path"   , TL_MSG_PARAM_STR  , path,
                                                    "Message", TL_MSG_PARAM_STR  , "open",
                                                    "JobID"  , TL_MSG_PARAM_INT  , jid,
                                                    "Drive"  , TL_MSG_PARAM_STR  , drive, 
                                                    "TPVID"  , TL_MSG_PARAM_TPVID, vid );
                        }
			goto reply;
		}
		if (chkdriveready (tapefd) <= 0) {
			usrmsg (func, TP054);
                        tl_tpdaemon.tl_log( &tl_tpdaemon, 54, 2,
                                            "func" , TL_MSG_PARAM_STR  , func,
                                            "TPVID", TL_MSG_PARAM_TPVID, vid );                        
			c = ETNRDY;
			goto reply;
		}
		if (method == TPPOSIT_BLKID) {
			tplogit (func, "locating to blockid %02x%02x%02x%02x\n",
			    blockid[0], blockid[1], blockid[2], blockid[3]);
                        {
                                char msg[32];
                                sprintf( msg, "locating to blockid %02x%02x%02x%02x\n",
                                         blockid[0], blockid[1], blockid[2], blockid[3] );
                                tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 7,
                                                    "func",       TL_MSG_PARAM_STR  , func,
                                                    "Message",    TL_MSG_PARAM_STR  , msg,
                                                    "Block ID 0", TL_MSG_PARAM_INT  , blockid[0],
                                                    "Block ID 1", TL_MSG_PARAM_INT  , blockid[1],
                                                    "Block ID 2", TL_MSG_PARAM_INT  , blockid[2],
                                                    "Block ID 3", TL_MSG_PARAM_INT  , blockid[3],
                                                    "TPVID"     , TL_MSG_PARAM_TPVID, vid );
                        }
			if ((c = locate (tapefd, path, blockid))) goto reply;
			flags |= LOCATE_DONE;
		}
		if ((c = posittape (tapefd, path, devtype, lblcode, mode,
		    &cfseq, fid, filstat, fsec, fseq, den, flags, Qfirst, Qlast,
		    vol1, hdr1, hdr2, uhl1)))
			goto reply;
		if (mode == WRITE_ENABLE)
			if ((c = read_pos (tapefd, path, blockid)))
				goto reply;

	/* tape is positionned */
	if (lblcode == AUL ) {
		if (filstat != NEW_FILE) {	/* set defaults from label */
			if (fid[0] == '\0') {
				strncpy (tpfid, hdr1 + 4, 17);
				tpfid [17] = '\0';
				if ((p = strchr (tpfid, ' ')) != NULL) *p = '\0';
				strcpy (fid, tpfid);
			}
			if (hdr2[0]) {
				if (blksize == 0) {
					if (*uhl1) {
						sscanf (uhl1 + 14, "%10d", &blksize);
					} else {
						sscanf (hdr2 + 5, "%5d", &blksize);
                                        }
                                }
				if (lrecl == 0) {
					if (*uhl1) {
						sscanf (uhl1 + 24, "%10d", &lrecl);
					} else {
						sscanf (hdr2 + 10, "%5d", &lrecl);
                                        }
                                }
			}
		} else {
			if (fid[0] == '\0') {
				p = strrchr (path, '/') + 1;
				if ((i = strlen (p) - 17) > 0) p += i;
				UPPER (p);
				strcpy (fid, p);
			}
		}
	}

	/* set default values to the fields which have not been set yet */

	if (blksize == 0) {
			if (lrecl == 0) {
				blksize = DEFAULTMIGRATIONBLOCKSIZE;
			} else {
				blksize = lrecl;
                        }
        }
	if (lrecl == 0) lrecl = blksize;

	/* Build UPDFIL request header */

	sbp = sendbuf;
	marshall_LONG (sbp, TPMAGIC);
	marshall_LONG (sbp, UPDFIL);
	q = sbp;        /* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build UPDFIL request body */

	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	marshall_LONG (sbp, jid);
	marshall_WORD (sbp, ux);
	marshall_LONG (sbp, blksize);
	marshall_OPAQUE (sbp, blockid, 4);
	marshall_LONG (sbp, cfseq);
	marshall_STRING (sbp, fid);
	marshall_LONG (sbp, lrecl);
	marshall_STRING (sbp, "F"); /* recfm is no longer used */

	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);      /* update length field */

	if ((c = send2tpd (NULL, sendbuf, msglen, NULL, 0)) == 0) {
		sbp = repbuf;
		marshall_LONG (sbp, cfseq);
		if (lblcode == AUL) {
			buildvollbl (vol1, vsn, lblcode, name);
			if (mode == WRITE_DISABLE || filstat == APPEND)
				for (i = 0; i < 80; i++)
					actual_hdr1[i] = hdr1[i] ? hdr1[i] : ' ';
			buildhdrlbl (hdr1, hdr2,
				fid, fsid, fsec, cfseq, retentd,
				'F', blksize, lrecl, den, lblcode);
			if (mode == WRITE_ENABLE && filstat != APPEND)
				memcpy (actual_hdr1, hdr1, 80);
			vol1[80] = '\0';
			actual_hdr1[80] = '\0';
			hdr2[80] = '\0';
			if (lblcode == AUL) {
				inq_data[0] = '\0';
				(void) inquiry (tapefd, path, inq_data);
				drive_serial_no[0] = '\0';
				(void) inquiry80 (tapefd, path, drive_serial_no);
				builduhl (uhl1, cfseq, blksize, lrecl, domainname,
                  hostname, (char*)inq_data, (char*)drive_serial_no);
				uhl1[80] = '\0';
			} else
				uhl1[0] = '\0';
		} else {
			vol1[0] = '\0';
			actual_hdr1[0] = '\0';
			hdr2[0] = '\0';
			uhl1[0] = '\0';
		}
                marshall_STRING (sbp, vol1);
                marshall_STRING (sbp, actual_hdr1);
                marshall_STRING (sbp, hdr2);
		marshall_STRING (sbp, uhl1);
		sendrep (rpfd, MSG_DATA, sbp - repbuf, repbuf);
	} else {
		usrmsg (func, "%s", errbuf);
                tl_tpdaemon.tl_log( &tl_tpdaemon, 103, 5,
                                    "func"   , TL_MSG_PARAM_STR  , func,
                                    "Message", TL_MSG_PARAM_STR  , errbuf,
                                    "JobID"  , TL_MSG_PARAM_INT  , jid,
                                    "vid"    , TL_MSG_PARAM_STR  , vid,
                                    "TPVID"  , TL_MSG_PARAM_TPVID, vid );                        
        }
reply:
	if (c < 0) c = serrno;
	if (c) {
		cleanup();
	} else {
		close (tapefd);
	}
	sendrep (rpfd, TAPERC, c);
        tl_tpdaemon.tl_exit( &tl_tpdaemon, 0 );
	exit (0);
}

void cleanup()
{
	int flags;
	int msglen;
	char *q;
	char repbuf[1];
	char *sbp;
	char sendbuf[REQBUFSZ];

	tplogit (func, "cleanup started\n");
        tl_tpdaemon.tl_log( &tl_tpdaemon, 110, 2,
                            "func",       TL_MSG_PARAM_STR, func,
                            "Message",    TL_MSG_PARAM_STR, "cleanup started" );
	if (tapefd >= 0)
		close (tapefd);

	/* must unload and deassign */

	flags = TPRLS_KEEP_RSV|TPRLS_UNLOAD|TPRLS_NOWAIT;
 
	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, TPMAGIC);
	marshall_LONG (sbp, TPRLS);
	q = sbp;        /* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);
 
	/* Build request body */

	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	marshall_LONG (sbp, jid);
	marshall_WORD (sbp, flags);
	marshall_STRING (sbp, path);

	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

	(void) send2tpd (NULL, sendbuf, msglen, repbuf, sizeof(repbuf));
}

void configdown(char *drive)
{
	char msg[OPRMSGSZ];

	sprintf (msg, TP033, drive, hostname); /* ops msg */
	usrmsg ("posovl", "%s\n", msg);
        tl_tpdaemon.tl_log( &tl_tpdaemon, 33, 4,
                            "func",     TL_MSG_PARAM_STR, "posovl",
                            "Message",  TL_MSG_PARAM_STR, msg,
                            "Drive",    TL_MSG_PARAM_STR, drive, 
                            "Hostname", TL_MSG_PARAM_STR, hostname );

	(void) Ctape_config (drive, CONF_DOWN);
}

void positkilled()
{
	cleanup();
        
        /* called before each exit() */
        tl_tpdaemon.tl_exit( &tl_tpdaemon, 0 );

	exit (2);
}
