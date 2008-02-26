/*
 * Copyright (C) 2000-2004 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*      nslisttape - list the file segments residing on a volume */
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h> 
#if defined(_WIN32)
#include <winsock2.h>
#endif
#include "Cgetopt.h"
#include "Cns.h"
#include "Cns_api.h"
#include "serrno.h"
#include "u64subr.h"

int main(int argc,char **argv)
{

	struct Cns_direntape *dtp;
	int c;
	static int dsflag = 0;
        static int checksumflag = 0;
	static int sumflag = 0;
	int errflg = 0;
	int iflag = 0;
	int humanflag = 0;
	int flags;
	Cns_list list;
	static struct Coptions longopts[] = {
		{"display_side", NO_ARGUMENT, &dsflag, 1},
		{"checksum", NO_ARGUMENT, &checksumflag, 1},
		{"ds", NO_ARGUMENT, &dsflag, 1},
		{"summarize", NO_ARGUMENT, &sumflag, 1},
		{0, 0, 0, 0}
	};
	signed64 parent_fileid = -1;
	char path[CA_MAXPATHLEN+1];
	char *server = NULL;
	char tmpbuf[21];
	char *vid = NULL;
	int fseq = 0;
	u_signed64 count = 0;
	u_signed64 size = 0;

#if defined(_WIN32)
	WSADATA wsadata;
#endif

	Copterr = 1;
	Coptind = 1;
	while ((c = Cgetopt_long (argc, argv, "Hh:V:isf:", longopts, NULL)) != EOF) {
		switch (c) {
		case 'h':
			server = Coptarg;
			break;
		case 'H':
			humanflag++;
			break;
		case 'V':
			vid = Coptarg;
			break;
		case 'i':
			iflag++;
			break;
		case 's':
			sumflag++;
			break;
		case 'f':
			fseq = (int) strtol(Coptarg, (char **)NULL, 10);
			if ((errno != 0) || (fseq == 0)) {
				fprintf (stderr, "invalid file sequence number: %s\n", Coptarg);
				exit (USERR);
			}
			break;
		case '?':
			errflg++;
			break;
		default:
			break;
		}
	}
	if (Coptind < argc || ! vid) {
		errflg++;
	}
	if (errflg) {
		fprintf (stderr, "usage: %s [-h name_server] [--display_side] [--checksum] [--summarize] [-f fseq] [-H] -V vid\n", argv[0]);
		exit (USERR);
	}
#if defined(_WIN32)
	if (WSAStartup (MAKEWORD (2, 0), &wsadata)) {
		fprintf (stderr, NS052);
		exit (SYERR);
	}
#endif

	if (sumflag) {
		c = Cns_tapesum(vid, &count, &size, 1);
		if (c < 0) {
			fprintf (stderr, "%s: %s\n", vid, (serrno == ENOENT) ? "No such volume" : sstrerror(serrno));
#if defined(_WIN32)
			WSACleanup();
#endif
			exit (USERR);
		}
		if (humanflag) {
			printf("%llu %s\n", count, u64tostru (size, tmpbuf, 0));
		} else {
			printf("%llu %llu\n", count, size);
		}
#if defined(_WIN32)
		WSACleanup();
#endif
		exit (0);		
	}

	flags = CNS_LIST_BEGIN;
	serrno = 0;
	while ((dtp = Cns_listtape (server, vid, flags, &list, fseq)) != NULL) {
		if (dtp->parent_fileid != parent_fileid) {
			if (Cns_getpath (server, dtp->parent_fileid, path) < 0) {
				fprintf (stderr, "%s\n", sstrerror(serrno));
#if defined(_WIN32)
				WSACleanup();
#endif
				exit (USERR);
			}
			parent_fileid = dtp->parent_fileid;
		}
		if (dsflag || dtp->side > 0)
			printf ("%c %d %3d %-6.6s/%d %5d %02x%02x%02x%02x %s %3d",
				dtp->s_status, dtp->copyno, dtp->fsec, dtp->vid,
				dtp->side, dtp->fseq, dtp->blockid[0], dtp->blockid[1],
				dtp->blockid[2], dtp->blockid[3],
				u64tostr (dtp->segsize, tmpbuf, 20), dtp->compression);
		else
			printf ("%c %d %3d %-6.6s   %5d %02x%02x%02x%02x %s %3d",
				dtp->s_status, dtp->copyno, dtp->fsec, dtp->vid,
				dtp->fseq, dtp->blockid[0], dtp->blockid[1],
				dtp->blockid[2], dtp->blockid[3],
				u64tostr (dtp->segsize, tmpbuf, 20), dtp->compression);

		if (checksumflag) {
			if (dtp->checksum_name != NULL &&  strlen(dtp->checksum_name)>0) {
				printf (" %*s %08lx", CA_MAXCKSUMNAMELEN, dtp->checksum_name, dtp->checksum);
			} else {
				printf (" %*s %08x", CA_MAXCKSUMNAMELEN, "-", 0);
			}
		}

		if (iflag) {
			printf (" %s %s/%s\n", u64tostr (dtp->fileid, tmpbuf, 20), path, dtp->d_name);
		} else {
			printf (" %s/%s\n", path, dtp->d_name);  
		}
		flags = CNS_LIST_CONTINUE;
	}
	if (serrno != 0) {
		fprintf (stderr, "%s: %s\n", vid, (serrno == ENOENT) ? "No such volume" : sstrerror(serrno)); 
		exit(USERR);
	} 
	(void) Cns_listtape (server, vid, CNS_LIST_END, &list, fseq);
	
#if defined(_WIN32)
	WSACleanup();
#endif
	exit (0);
}
