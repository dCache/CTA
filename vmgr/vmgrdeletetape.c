/*
 * Copyright (C) 2000 by CERN/IT/PDP/DM
 * All rights reserved
 */
 
#ifndef lint
static char sccsid[] = "@(#)$RCSfile: vmgrdeletetape.c,v $ $Revision: 1.3 $ $Date: 2000/03/02 08:38:21 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

/*	vmgrdeletetape - delete a tape volume */
#include <stdio.h>
#include <sys/types.h>
#include "serrno.h"
#include "vmgr.h"
extern	char	*optarg;
extern	int	optind;
main(argc, argv)
int argc;
char **argv;
{
	int c;
	int errflg = 0;
	char *vid = NULL;

        while ((c = getopt (argc, argv, "V")) != EOF) {
                switch (c) {
                case 'V':
			vid = optarg;
                        break;
                case '?':
                        errflg++;
                        break;
                default:
                        break;
                }
        }
        if (optind < argc) {
                errflg++;
        }
        if (errflg) {
                fprintf (stderr, "usage: %s -V vid\n", argv[0]);
                exit (USERR);
        }
 
	if (vmgr_deletetape (vid) < 0) {
		fprintf (stderr, "vmgrdeletetape %s: %s\n", vid, sstrerror(serrno));
		exit (USERR);
	}
	exit (0);
}
