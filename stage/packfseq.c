/*
 * $Id: packfseq.c,v 1.12 2001/02/02 12:14:33 jdurand Exp $
 */

/*
 * Copyright (C) 1993-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: packfseq.c,v $ $Revision: 1.12 $ $Date: 2001/02/02 12:14:33 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "stage.h"
#include "osdep.h"
#include <stdlib.h>

#ifdef hpux
/* What the hell does hpux does not like this prototype ??? */
int packfseq _PROTO(());
#else
int packfseq _PROTO((fseq_elem *, int, int, int, char, char *, int));
#endif

int packfseq(fseq_list, i, nbdskf, nbtpf, trailing, fseq, fseqsz)
		 fseq_elem *fseq_list;
		 int i;
		 int nbdskf;
		 int nbtpf;
		 char trailing;
		 char *fseq;
		 int fseqsz;
{
	char *dp;
	char fseq_tmp[CA_MAXFSEQLEN + 1];
	int j;
	int n1, n2;
	int prev;

	if (i < nbtpf)
		strcpy (fseq, (char *) (fseq_list + i));
	else
		strcpy (fseq, (char *) (fseq_list + nbtpf - 1));
	if ((i == nbdskf - 1) && (nbdskf < nbtpf)) {
		n1 = strtol ((char *) (fseq_list + i), &dp, 10);
		prev = n1;
		for (j = i + 1; j < nbtpf; j++) {
			n2 = strtol ((char *) (fseq_list + j), &dp, 10);
			if (n2 != prev && n2 != prev + 1) {
				if (prev > n1) {
					sprintf (fseq_tmp, "-%d", prev);
					if (strlen (fseq) + strlen (fseq_tmp) >= fseqsz)
						return (1);
					strcat (fseq, fseq_tmp);
				}
				sprintf (fseq_tmp, ",%d", n2);
				if (strlen (fseq) + strlen (fseq_tmp) >= fseqsz)
					return (1);
				strcat (fseq, fseq_tmp);
				n1 = n2;
			}
			prev = n2;
		}
		if (prev > n1 && trailing != '-') {
			sprintf (fseq_tmp, "-%d", prev);
			if (strlen (fseq) + strlen (fseq_tmp) >= fseqsz)
				return (1);
			strcat (fseq, fseq_tmp);
		}
	}
	if ((i == nbdskf - 1) && (trailing == '-')) {
		if (strlen (fseq) + 1 >= fseqsz)
			return (1);
		strcat (fseq, "-");
	}
	return (0);
}
