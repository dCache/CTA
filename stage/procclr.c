/*
 * $Id: procclr.c,v 1.34 2001/07/12 11:07:30 jdurand Exp $
 */

/*
 * Copyright (C) 1993-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: procclr.c,v $ $Revision: 1.34 $ $Date: 2001/07/12 11:07:30 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <grp.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include "marshall.h"
#undef  unmarshall_STRING
#define unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#include "stage.h"
#include "stage_api.h"
#if SACCT
#include "../h/sacct.h"
#endif
#include "osdep.h"
#include "Cgrp.h"
#include "Cgetopt.h"
#include "rfio_api.h"

#if hpux
/* On HP-UX seteuid() and setegid() do not exist and have to be wrapped */
/* calls to setresuid().                                                */
#define seteuid(euid) setresuid(-1,euid,-1)
#define setegid(egid) setresgid(-1,egid,-1)
#endif

void procclrreq _PROTO((int, int, char *, char *));

extern char func[16];
extern int nbcat_ent;
extern int reqid;
extern int rpfd;
extern struct stgcat_entry *stce;	/* end of stage catalog */
extern struct stgcat_entry *stcs;	/* start of stage catalog */
extern struct stgpath_entry *stpe;	/* end of stage path catalog */
extern struct stgpath_entry *stps;	/* start of stage path catalog */
extern struct waitq *waitqp;
extern int req2argv _PROTO((char *, char ***));
#if (defined(IRIX64) || defined(IRIX5) || defined(IRIX6))
extern int sendrep _PROTO((int, int, ...));
#else
extern int sendrep _PROTO(());
#endif
extern int stglogit _PROTO(());
extern int isvalidpool _PROTO((char *));
extern void dellink _PROTO((struct stgpath_entry *));
extern int enoughfreespace _PROTO((char *, int));
extern int checklastaccess _PROTO((char *, time_t));
extern int delfile _PROTO((struct stgcat_entry *, int, int, int, char *, uid_t, gid_t, int, int));
extern int savepath _PROTO(());
extern void stageacct _PROTO((int, uid_t, gid_t, char *, int, int, int, int, struct stgcat_entry *, char *));
extern void rwcountersfs _PROTO((char *, char *, int, int));
extern int stglogflags _PROTO(());

int check_delete _PROTO((struct stgcat_entry *, gid_t, uid_t, char *, char *, int, int));

#if defined(_REENTRANT) || defined(_THREAD_SAFE)
#define strtok(X,Y) strtok_r(X,Y,&last)
#endif /* _REENTRANT || _THREAD_SAFE */

extern u_signed64 stage_uniqueid;

void procclrreq(req_type, magic, req_data, clienthost)
		 int req_type;
		 int magic;
		 char *req_data;
		 char *clienthost;
{
	char **argv = NULL;
	int c, i, j;
	int Fflag = 0;
	int cflag = 0;
	char *dp;
	int errflg = 0;
	int found;
	char *fseq = NULL;
	gid_t gid;
	char group[CA_MAXGRPNAMELEN + 1];
	struct group *gr;
	char *lbl = NULL;
	char *linkname = NULL;
	char *mfile = NULL;
	int gc_stop_thresh = 0;
	int nargs;
	int numvid = 0;
	char *path = NULL;
	int poolflag = 0;
	char poolname[CA_MAXPOOLNAMELEN + 1];
	char *q;
	char *rbp;
	int rflag = 0;
	int silentflag = 0;
	struct stgcat_entry *stcp;
	struct stgpath_entry *stpp;
	uid_t uid;
	char *user;
	char vid[MAXVSN][7];
	char *xfile = NULL;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
	char *last = NULL;
#endif /* _REENTRANT || _THREAD_SAFE */
	int t_or_d, nstcp_input, nstpp_input;
	extern struct passwd start_passwd;             /* Start uid/gid stage:st */
	int save_rpfd;

	c = 0;
	save_rpfd = rpfd;
	poolname[0] = '\0';
	rbp = req_data;
	unmarshall_STRING (rbp, user);	/* login name */
	if (req_type > STAGE_00) {
		unmarshall_LONG (rbp, uid);
		unmarshall_LONG (rbp, gid);
	} else {
		unmarshall_WORD (rbp, uid);
		unmarshall_WORD (rbp, gid);
	}
#ifndef __INSURE__
	stglogit (func, STG92, (req_type > STAGE_00) ? "stage_clr" : "stageclr", user, uid, gid, clienthost);
#endif
#if SACCT
	stageacct (STGCMDR, uid, gid, clienthost,
						 reqid, req_type, 0, 0, NULL, "");
#endif
	if ((gr = Cgetgrgid (gid)) == NULL) {
		sendrep (rpfd, MSG_ERR, STG36, gid);
		c = SYERR;
		goto reply;
	}
	strncpy (group, gr->gr_name, CA_MAXGRPNAMELEN);
	group[CA_MAXGRPNAMELEN] = '\0';

	if (req_type > STAGE_00) {
		u_signed64 flags;
		struct stgcat_entry stcp_input;
		struct stgpath_entry stpp_input;

		unmarshall_HYPER (rbp, stage_uniqueid);
		unmarshall_HYPER (rbp, flags);
		{
			char tmpbyte;
			unmarshall_BYTE(rbp, tmpbyte);
			t_or_d = tmpbyte;
		}
		unmarshall_LONG (rbp, nstcp_input);
		unmarshall_LONG (rbp, nstpp_input);
		/* We support one nstcp_input == 1 xor nstpp_input == 1 */
		if (((nstcp_input <= 0) && (nstpp_input <= 0)) ||
			((nstcp_input >  0) && (nstpp_input >  0)) ||
			((nstcp_input >  0) && (nstcp_input != 1)) ||
            ((nstpp_input >  0) && (nstpp_input != 1))) {
			sendrep(rpfd, MSG_ERR, "STG02 - Only exactly one stcp (%d here) or one stpp (%d here) input is supported\n", nstcp_input, nstpp_input);
			c = USERR;
			goto reply;
        }
		/* We makes sure there is only one entry input, of any type, so no memory allocation needed */
		if (nstcp_input > 0) {
			memset((void *) &stcp_input, (int) 0, sizeof(struct stgcat_entry));
			{
				char logit[BUFSIZ + 1];
				int struct_status = 0;

				stcp_input.reqid = -1;
				unmarshall_STAGE_CAT(magic, STAGE_INPUT_MODE, struct_status, rbp, &(stcp_input));
				if (struct_status != 0) {
					sendrep(rpfd, MSG_ERR, "STG02 - Bad catalog entry input\n");
					c = SYERR;
					goto reply;
				}
				logit[0] = '\0';
				stcp_input.t_or_d = t_or_d;
				if (stage_stcp2buf(logit,BUFSIZ,&(stcp_input)) == 0 || serrno == SEUMSG2LONG) {
					logit[BUFSIZ] = '\0';
					stglogit("stage_clr","stcp[1/1] : %s\n",logit);
	 			}
			}
			/* We set the flags */
			switch (t_or_d) {
			case 'd':
				xfile = stcp_input.ipath;
				break;
			case 'm':
			case 'h':
				mfile = stcp_input.u1.m.xfile;
				break;
			case 't':
				if (stcp_input.u1.t.lbl[0] != '\0') lbl = stcp_input.u1.t.lbl;
				if (stcp_input.u1.t.fseq[0] != '\0') fseq = stcp_input.u1.t.fseq;
				for (i = 0; i < MAXVSN; i++) {
					if (stcp_input.u1.t.vid[i][0] != '\0') {
						strcpy (vid[numvid], stcp_input.u1.t.vid[i]);
						UPPER (vid[numvid]);
						numvid++;
					}
				}
				break;
			}
			if (stcp_input.poolname[0] != '\0') {
				if (strcmp (stcp_input.poolname, "NOPOOL") == 0 ||
						isvalidpool (stcp_input.poolname)) {
					strcpy (poolname, stcp_input.poolname);
				} else {
					sendrep (rpfd, MSG_ERR, STG32, stcp_input.poolname);
					c = USERR;
					goto reply;
				}
			}
        } else {
			int path_status = 0;
			unmarshall_STAGE_PATH(magic, STAGE_INPUT_MODE, path_status, rbp, &(stpp_input));
			if (path_status != 0) {
				sendrep(rpfd, MSG_ERR, "STG02 - Bad input (path input structure)\n");
				c = USERR;
				goto reply;
			}
			stglogit(func,"stpp[1/1] : %s\n",stpp_input.upath);
			/* We set the flags */
			if ((flags & STAGE_LINKNAME) == STAGE_LINKNAME) linkname = stpp_input.upath;
			if ((flags & STAGE_PATHNAME) == STAGE_PATHNAME) path = stpp_input.upath;
			if ((linkname != NULL) && (path != NULL)) {
				sendrep(rpfd, MSG_ERR, "STG02 - Both STAGE_LINKNAME and STAGE_PATHNAME is not allowed\n");
				c = USERR;
				goto reply;
			}
		}
		if ((flags & STAGE_CONDITIONAL) == STAGE_CONDITIONAL) cflag = 1;
		if ((flags & STAGE_FORCE) == STAGE_FORCE) Fflag = 1;
		if ((flags & STAGE_REMOVEHSM) == STAGE_REMOVEHSM) rflag = 1;
		if ((flags & STAGE_SILENT) == STAGE_SILENT) {
			silentflag = 1;
			rpfd = -1;
		}
		/* Print the flags */
		stglogflags(func,flags);
	} else {
		nargs = req2argv (rbp, &argv);

		Coptind = 1;
		Copterr = 0;
		while ((c = Cgetopt (nargs, argv, "cFGh:I:iL:l:M:m:P:p:q:r:V:")) != -1) {
			switch (c) {
			case 'c':
				cflag++;
				break;
			case 'F':
				Fflag++;
				break;
			case 'G':
				break;
			case 'h':
				break;
			case 'I':
				xfile = Coptarg;
				break;
			case 'i':
				break;
			case 'L':
				linkname = Coptarg;
				break;
			case 'l':	/* label type (al, nl, sl or blp) */
				lbl = Coptarg;
				break;
			case 'M':
				mfile = Coptarg;
				break;
			case 'm':
				gc_stop_thresh = strtol (Coptarg, &dp, 10);
				if (*dp != '\0' || gc_stop_thresh > 100) {
					sendrep (rpfd, MSG_ERR, STG06, "-m");
					errflg++;
				}
				break;
			case 'P':
				path = Coptarg;
				if (*path == '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-P");
					errflg++;
				}
				break;
			case 'p':
				if (strcmp (Coptarg, "NOPOOL") == 0 ||
						isvalidpool (Coptarg)) {
					strcpy (poolname, Coptarg);
				} else {
					sendrep (rpfd, MSG_ERR, STG32, Coptarg);
					errflg++;
				}
				break;
			case 'q':	/* file sequence number(s) */
				fseq = Coptarg;
				break;
			case 'r':
				/* Coptarg is equal to emove_from_hsm */
				/* because we only allows this in the stageclr command line */
				rflag = 1;
				break;
			case 'V':	/* visual identifier(s) */
				q = strtok (Coptarg, ":");
				while (q != NULL) {
					strcpy (vid[numvid], q);
					UPPER (vid[numvid]);
					numvid++;
					q = strtok (NULL, ":");
				}
				break;
			}
		}
	}
	/* -L linkname and -remove_from_hsm is not allowed */
	if (linkname && rflag)
		errflg++;

	if (errflg) {
		c = USERR;
		goto reply;
	}
	c = 0;
	if (strcmp (poolname, "NOPOOL") == 0)
		poolflag = -1;
	found = 0;
	if (linkname) {
		for (stpp = stps; stpp < stpe; stpp++) {
			if (stpp->reqid == 0) break;
			if (strcmp (linkname, stpp->upath) == 0) {
				found = 1;
				break;
			}
		}
		if (found) {
			dellink (stpp);
			savepath ();
		} else {
			sendrep (rpfd, MSG_ERR, STG33, linkname, "file not found");
			c = USERR;
			goto reply;
		}
	} else if (path) {
		for (stpp = stps; stpp < stpe; stpp++) {
			if (stpp->reqid == 0) break;
			if (strcmp (path, stpp->upath) == 0) {
				found = 1;
				break;
			}
		}
		if (found) {
			for (stcp = stcs; stcp < stce; stcp++) {
				if (stpp->reqid == stcp->reqid) break;
			}
			if (cflag && stcp->poolname[0] &&
					enoughfreespace (stcp->poolname, gc_stop_thresh)) {
				c = ENOUGHF;
				goto reply;
			}
			if (cflag && uid == 0 &&	/* probably garbage collector */
					checklastaccess (stcp->poolname, stcp->a_time)) {
				c = EBUSY;
				goto reply;
			}
			if ((i = check_delete (stcp, gid, uid, group, user, rflag, Fflag)) > 0) {
				c = i;
				goto reply;
			}
		} else {
		stageclr_redo:
			for (stcp = stcs; stcp < stce; stcp++) {
				int thisnextreqid;
				if (stcp->reqid == 0) break;
                /* We keep in mind what would be the next reqid */
				thisnextreqid = (stcp+1)->reqid;
				if (strcmp (path, stcp->ipath) == 0) {
					found = 1;
					if (cflag && stcp->poolname[0] &&
							enoughfreespace (stcp->poolname, gc_stop_thresh)) {
						c = ENOUGHF;
						goto reply;
					}
					if (cflag && uid == 0 &&
							checklastaccess (stcp->poolname, stcp->a_time)) {
						c = EBUSY;
						goto reply;
					}
					if ((i = check_delete (stcp, gid, uid, group, user, rflag, Fflag)) > 0) {
						c = i;
						goto reply;
					}
				}
				if (thisnextreqid == (stcp+1)->reqid) {
					/* The catalog has not been shifted - we can continue unless end of the catalog */
					if (thisnextreqid == 0) break;
					continue;
				}
				if (thisnextreqid == stcp->reqid) {
					/* The catalog has been shifted by one - we lye to the for() loop unless end of the catalog */
					if (thisnextreqid == 0) break;
					stcp--;
					/* And we continue */
					continue;
                }
				/* Here the catalog has been shifted by more than one, this is a bit too much */
				/* for us so we restart the whole loop */
				goto stageclr_redo;
			}
		}
		if (! found) {
			sendrep (rpfd, MSG_ERR, STG33, path, "file not found");
			c = USERR;
			goto reply;
		}
	} else {
		for (stcp = stcs; stcp < stce; stcp++) {
			if (stcp->reqid == 0) break;
			if (poolflag < 0) {	/* -p NOPOOL */
				if (stcp->poolname[0]) continue;
			} else if (*poolname && strcmp (poolname, stcp->poolname)) continue;
			if (numvid) {
				if (stcp->t_or_d != 't') continue;
				for (j = 0; j < numvid; j++)
					if (strcmp (stcp->u1.t.vid[0], vid[j]) == 0) break;
				if (j == numvid) continue;
			}
			if (lbl) {
				if (stcp->t_or_d != 't') continue;
				if (strcmp (lbl, stcp->u1.t.lbl)) continue;
			}
			if (fseq) {
				if (stcp->t_or_d != 't') continue;
				if (strcmp (fseq, stcp->u1.t.fseq)) continue;
			}
			if (xfile) {
				if (stcp->t_or_d != 'd') continue;
				if (strcmp (xfile, stcp->u1.d.xfile)) continue;
			}
			if (mfile) {
				if (stcp->t_or_d != 'm' && stcp->t_or_d != 'h') continue;
				if (stcp->t_or_d == 'm' && strcmp (mfile, stcp->u1.m.xfile)) continue;
				if (stcp->t_or_d == 'h' && strcmp (mfile, stcp->u1.h.xfile)) continue;
			}
			found = 1;
			if (cflag && stcp->poolname[0] &&
					enoughfreespace (stcp->poolname, gc_stop_thresh)) {
				c = ENOUGHF;
				goto reply;
			}
			if ((i = check_delete (stcp, gid, uid, group, user, rflag, Fflag)) > 0) {
				c = i;
				goto reply;
			}
			stcp += i;
		}
		if (! found) {
			if (mfile && rflag && Fflag) {
				/* User wanted anyway to delete the HSM file */
				/* RFIO is anyway already interfaced to the name server - we use immediately its interface */
				setegid(gid);
				seteuid(uid);
				if (ISCASTOR(mfile)) {
					if (Cns_unlink (mfile) == 0) {
						stglogit (func, STG95, mfile, user);
					} else {
						sendrep (rpfd, MSG_ERR, STG02, mfile, "Cns_unlink", sstrerror(serrno));
						if (req_type > STAGE_00) {
							c = serrno;
						} else {
							c = ((serrno == EINVAL) || (serrno == EPERM) || (serrno == EACCES)) ? USERR : SYERR;
						}
					}
				} else {
					if (rfio_unlink (mfile) == 0) {
						stglogit (func, STG95, mfile, user);
					} else {
						int save_serrno = rfio_serrno();
						sendrep (rpfd, MSG_ERR, STG02, mfile, "rfio_unlink", rfio_serror());
						if (req_type > STAGE_00) {
							c = save_serrno;
						} else {
							c = ((save_serrno == EINVAL) || (save_serrno == EPERM) || (save_serrno == EACCES)) ? USERR : SYERR;
						}
					}
				}
				setegid(start_passwd.pw_gid);
				seteuid(start_passwd.pw_uid);
			} else {
				sendrep (rpfd, MSG_ERR, STG33, (mfile ? mfile : (xfile ? xfile : "")), "file not found");
				c = USERR;
			}
		}
	}
 reply:
	if (argv != NULL) free (argv);
	rpfd = save_rpfd;
	sendrep (rpfd, STAGERC, STAGECLR, c);
}

int check_delete(stcp, gid, uid, group, user, rflag, Fflag)
		 struct stgcat_entry *stcp;
		 gid_t gid;
		 uid_t uid;
		 char *group;
		 char *user;
		 int rflag; /* True if HSM source file has to be removed */
		 int Fflag; /* Forces deletion of request in memory instead of internal error */
{
	int found;
	int i;
	int savereqid;
	struct waitf *wfp;
	struct waitq *wqp;

	/*	return	<0	request deleted
	 *		 0	running request (status set to CLEARED and req signalled)
	 *		>0	in case of error
	 */

	if (strcmp (group, STGGRP) && strcmp (group, stcp->group) && uid != 0) {
		sendrep (rpfd, MSG_ERR, STG33, "", "permission denied");
		return (USERR);
	}

    /* Special case of intermediate state */
	if (ISCASTORWAITINGMIG(stcp)) {
		sendrep (rpfd, MSG_ERR, STG33, stcp->u1.h.xfile, strerror(EBUSY));
		return (EBUSY);
	}

	if ((stcp->status & 0xF0) == STAGED ||
		(stcp->status & (STAGEOUT | PUT_FAILED)) == (STAGEOUT | PUT_FAILED)) {
		if (delfile (stcp, 0, 1, 1, user, uid, gid, rflag, 1) < 0) {
			sendrep (rpfd, MSG_ERR, STG02, stcp->ipath, "rfio_unlink",
							 rfio_serror());
			return (USERR);
		}
	} else if ((stcp->status & 0xF) == STAGEOUT || (stcp->status & 0xF) == STAGEALLOC) {
		if ((ISSTAGEOUT(stcp) && (! ISCASTORMIG(stcp))) || (ISSTAGEALLOC(stcp) && ((stcp->status | STAGED) != STAGED))) {
			rwcountersfs(stcp->poolname, stcp->ipath, stcp->status, STAGEUPDC);
		}
		if (delfile (stcp, 1, 1, 1, user, uid, gid, rflag, 1) < 0) {
			sendrep (rpfd, MSG_ERR, STG02, stcp->ipath, "rfio_unlink",
							 rfio_serror());
			return (USERR);
		}
	} else {	/* the request should be in the active/wait queue */
		found = 0;
		for (wqp = waitqp; wqp; wqp = wqp->next) {
			for (i = 0, wfp = wqp->wf; i < wqp->nbdskf; i++, wfp++) {
				if (wfp->subreqid == stcp->reqid) {
					found = 1;
					break;
				}
			}
			if (found) {
				savereqid = reqid;
				reqid = wqp->reqid;
				sendrep (wqp->rpfd, MSG_ERR, STG95, "request", user);
				reqid = savereqid;
				wqp->status = CLEARED;
				/* is there an active stager overlay for this file? */
				if (wqp->ovl_pid) {
					stglogit (func, "killing process %d\n",
										wqp->ovl_pid);
					kill (wqp->ovl_pid, SIGINT);
					wqp->ovl_pid = 0;
				}
				return (0);
			}
		}
		if (Fflag && ISROOT(uid,gid)) {
			sendrep (rpfd, MSG_ERR,
					 "Status=0x%x but req not in waitq - Deleting reqid %d\n",
					 stcp->status, stcp->reqid);
			if ((stcp->status & 0xF) == STAGEOUT || (stcp->status & 0xF) == STAGEALLOC) {
				if ((ISSTAGEOUT(stcp) && (! ISCASTORMIG(stcp))) || (ISSTAGEALLOC(stcp) && ((stcp->status | STAGED) != STAGED))) {
					rwcountersfs(stcp->poolname, stcp->ipath, stcp->status, STAGEUPDC);
				}
			}
			if (delfile (stcp, 1, 1, 1, user, uid, gid, rflag, 1) < 0) {
				sendrep (rpfd, MSG_ERR, STG02, stcp->ipath, "rfio_unlink",
						 rfio_serror());
				return (USERR);
			}
			return (0);
		} else {
			sendrep (rpfd, MSG_ERR, STG104, stcp->status);
			return (USERR);
		}
	}
	return (-1);
}
