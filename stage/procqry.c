/*
 * $Id: procqry.c,v 1.13 1999/12/22 07:23:40 jdurand Exp $
 */

/*
 * Copyright (C) 1993-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: procqry.c,v $ $Revision: 1.13 $ $Date: 1999/12/22 07:23:40 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <grp.h>
#include <math.h>
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
#include <regex.h>
#else
#define INIT       register char *sp = instring;
#define GETC()     (*sp++)
#define PEEKC()    (*sp)
#define UNGETC(c)  (--sp)
#define RETURN(c)  return(c)
#define ERROR(c)   return(0)
#include <regexp.h>
#endif
#include <string.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/stat.h>
#include "marshall.h"
#undef  unmarshall_STRING
#define unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#include "stage.h"
#if SACCT
#include "../h/sacct.h"
#endif
#include "stgdb_Cdb_ifce.h"
#include <serrno.h>
#include "osdep.h"

void procqryreq _PROTO((char *, char *));
void print_link_list _PROTO((char *, int, char *, int, char *, int, char (*)[7], char *, char *, char *, char *));
void print_tape_info _PROTO((char *, int, char *, int, char *, int, char (*)[7], char *));

extern char *optarg;
extern int optind;
#if (defined(IRIX64) || defined(IRIX5) || defined(IRIX6))
extern int sendrep (int, int, ...);
#endif
#if !defined(linux)
extern char *sys_errlist[];
#endif
extern char defpoolname[MAXPOOLNAMELEN];
extern char func[16];
extern int nbcat_ent;
extern int reqid;
extern int rpfd;
extern struct stgcat_entry *stce;	/* end of stage catalog */
extern struct stgcat_entry *stcs;	/* start of stage catalog */
extern struct stgpath_entry *stpe;	/* end of stage path catalog */
extern struct stgpath_entry *stps;	/* start of stage path catalog */
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
static regmatch_t pmatch;
static regex_t preg;
#else
static char expbuf[256];
#endif
extern struct stgdb_fd dbfd;

struct stgdb_fd dbfd_in_fork;
struct stgdb_fd *dbfd_query;

void procqryreq(req_data, clienthost)
char *req_data;
char *clienthost;
{
	char *afile = NULL;
	char **argv;
	int aflag = 0;
	int c, j;
	int errflg = 0;
	int fflag = 0;
	char *fseq = NULL;
	gid_t gid;
	char group[MAXGRPNAMELEN];
	struct group *gr;
	int hdrprinted = 0;
	int Lflag = 0;
	static char l_stat[3][11] = {"", "STAGED_LSZ", "STAGED_TPE"};
	int lflag = 0;
	char *mfile = NULL;
	int nargs;
	int numvid = 0;
	char *p;
	char p_lrecl[7];
	char p_recfm[6];
	char p_size[6];
	char p_stat[12];
	int Pflag = 0;
	int pid = -1;
	int poolflag = 0;
	char poolname[MAXPOOLNAMELEN];
	char *q;
	static char s_stat[5][9] = {"", "STAGEIN", "STAGEOUT", "STAGEWRT", "STAGEWRT"};
	char *rbp;
	int Sflag = 0;
	int sflag = 0;
	struct stat st;
	struct stgcat_entry *stcp;
	int Tflag = 0;
	static char title[] = 
		"Vid      Fseq Lbl Recfm Lrecl Blksiz State      Nbacc.     Size    Pool\n";
	static char title_A[] = 
		"File name                            State      Nbacc.     Size    Pool\n";
	static char title_I[] = 
		"File name         Recfm Lrecl Blksiz State      Nbacc.     Size    Pool\n";
	struct tm *tm;
	int uflag = 0;
	char *user;
	char vid[MAXVSN][7];
	static char x_stat[7][12] = {"", "WAITING_SPC", "WAITING_REQ", "STAGED","KILLED", "FAILED", "PUT_FAILED"};
	char *xfile = NULL;
	int xflag = 0;

	poolname[0] = '\0';
	rbp = req_data;
	unmarshall_STRING (rbp, user);	/* login name */
	unmarshall_WORD (rbp, gid);
	nargs = req2argv (rbp, &argv);
#if SACCT
	stageacct (STGCMDR, -1, gid, clienthost,
		reqid, STAGEQRY, 0, 0, NULL, "");
#endif

	if ((gr = getgrgid (gid)) == NULL) {
		sendrep (rpfd, MSG_ERR, STG36, gid);
		c = SYERR;
		goto reply;
	}
	strcpy (group, gr->gr_name);
#ifdef linux
	optind = 0;
#else
	optind = 1;
#endif
	while ((c = getopt (nargs, argv, "A:afGh:I:LlM:Pp:q:SsTuV:x")) != EOF) {
		switch (c) {
		case 'A':
			afile = optarg;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regcomp (&preg, afile, 0)) {
#else
			if (! compile (afile, expbuf, expbuf+sizeof(expbuf), '\0')) {
#endif
				sendrep (rpfd, MSG_ERR, STG06, "-A");
				errflg++;
			}
			break;
		case 'a':	/* all groups */
			aflag++;
			break;
		case 'f':	/* full external pathnames */
			fflag++;
			break;
		case 'G':
			break;
		case 'h':
			break;
		case 'I':
			xfile = optarg;
			break;
		case 'L':	/* print linkname */
			Lflag++;
			break;
		case 'l':	/* long format (more details) */
			lflag++;
			break;
		case 'M':
			mfile = optarg;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regcomp (&preg, mfile, 0)) {
#else
			if (! compile (mfile, expbuf, expbuf+sizeof(expbuf), '\0')) {
#endif
				sendrep (rpfd, MSG_ERR, STG06, "-M");
				errflg++;
			}
			break;
		case 'P':	/* print pathname */
			Pflag++;
			break;
		case 'p':
			if (strcmp (optarg, "NOPOOL") == 0 ||
			    isvalidpool (optarg)) {
				strcpy (poolname, optarg);
			} else {
				sendrep (rpfd, MSG_ERR, STG32, optarg);
				errflg++;
			}
			break;
		case 'q':	/* file sequence number(s) */
			fseq = optarg;
			break;
		case 'S':	/* sort requests for cleaner */
			Sflag++;
			break;
		case 's':	/* statistics on pool utilization */
			sflag++;
			break;
		case 'T':	/* print tape info */
			Tflag++;
			break;
		case 'u':	/* only requests belonging to user */
			uflag++;
			break;
		case 'V':	/* visual identifier(s) */
			q = strtok (optarg, ":");
			while (q) {
				strcpy (vid[numvid], q);
				UPPER (vid[numvid]);
				numvid++;
				q = strtok (NULL, ":");
			}
			break;
		case 'x':	/* debug: reqid + internal pathname */
			xflag++;
			break;
		}
	}
	if (sflag && strcmp (poolname, "NOPOOL") == 0) {
		sendrep (rpfd, MSG_ERR, STG17, "-s", "-p NOPOOL");
		errflg++;
	}
	if (errflg) {
		c = USERR;
		goto reply;
	}
	c = 0;
	if (strcmp (poolname, "NOPOOL") == 0)
		poolflag = -1;
	if (aflag || lflag || Sflag) {	/* run long requests with rfio_stat calls in a child */
		if ((pid = fork ()) < 0) {
			sendrep (rpfd, MSG_ERR, STG02, "", "fork", sys_errlist[errno]);
			c = SYERR;
			goto reply;
		}
		if (pid) {
			stglogit (func, "forking procqry, pid=%d\n", pid);
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (afile || mfile)
				regfree (&preg);
#endif
			free (argv);
			close (rpfd);
			return;
		} else {
			/* We are in the child : we open a new connection to the Database Server so that   */
			/* it will not clash with current one owned by the main process.                   */

            strcpy(dbfd_in_fork.username,dbfd.username);
            strcpy(dbfd_in_fork.password,dbfd.password);

			if (stgdb_login(&dbfd_in_fork) != 0) {
                sendrep(rpfd, MSG_ERR, STG100, "login", sstrerror(serrno), __FILE__, __LINE__);
				stglogit(func, "Error loging to database server (%s)\n",sstrerror(serrno));
				exit(SYERR);
			}

			/* Open the database */
			if (stgdb_open(&dbfd_in_fork,"stage") != 0) {
                sendrep(rpfd, MSG_ERR, STG100, "open", sstrerror(serrno), __FILE__, __LINE__);
				stglogit(func, "Error opening \"stage\" database (%s)\n",sstrerror(serrno));
				exit(SYERR);
			}

			/* There is no need to ask for a dump of the catalog : we use the one that is */
			/* already in memory.                                                         */

			/* We set the pointer to use to the correct dbfd structure */
			dbfd_query = &dbfd_in_fork;
		}
	} else {
		/* No fork : the dbfd to use is the one of the main process */
		dbfd_query = &dbfd;
	}

	if (Lflag) {
		print_link_list (poolname, aflag, group, uflag, user,
		    numvid, vid, fseq, xfile, afile, mfile);
		goto reply;
	}
	if (sflag) {
		print_pool_utilization (rpfd, poolname, defpoolname);
		goto reply;
	}
	if (Sflag) {
		if (print_sorted_list (poolname, aflag, group, uflag, user,
		    numvid, vid, fseq, xfile, afile, mfile) < 0)
			c = SYERR;
		goto reply;
	}
	if (Tflag) {
		print_tape_info (poolname, aflag, group, uflag, user,
		    numvid, vid, fseq);
		goto reply;
	}
	for (stcp = stcs; stcp < stce; stcp++) {
		if (stcp->reqid == 0) break;
		if ((stcp->status & 0xF0) == WAITING_REQ) continue;
		if (poolflag < 0) {	/* -p NOPOOL */
			if (stcp->poolname[0]) continue;
		} else if (*poolname && strcmp (poolname, stcp->poolname)) continue;
		if (!aflag && strcmp (group, stcp->group)) continue;
		if (uflag && strcmp (user, stcp->user)) continue;
		if (numvid) {
			if (stcp->t_or_d != 't') continue;
			for (j = 0; j < numvid; j++)
				if (strcmp (stcp->u1.t.vid[0], vid[j]) == 0) break;
			if (j == numvid) continue;
		}
		if (fseq) {
			if (stcp->t_or_d != 't') continue;
			if (strcmp (fseq, stcp->u1.t.fseq)) continue;
		}
		if (xfile) {
			if (stcp->t_or_d != 'd') continue;
			if (strcmp (xfile, stcp->u1.d.xfile)) continue;
		}
		if (afile) {
			if (stcp->t_or_d != 'a') continue;
			if ((p = strrchr (stcp->u1.d.xfile, '/')) == NULL)
				p = stcp->u1.d.xfile;
			else
				p++;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regexec (&preg, p, 1, &pmatch, 0)) continue;
#else
			if (step (p, expbuf) == 0) continue;
#endif
		}
		if (mfile) {
			if (stcp->t_or_d != 'm') continue;
			p = stcp->u1.m.xfile;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regexec (&preg, p, 1, &pmatch, 0)) continue;
#else
			if (step (p, expbuf) == 0) continue;
#endif
		}
		if (Pflag) {
			sendrep (rpfd, MSG_OUT, "%s\n", stcp->ipath);
			continue;
		}
		if (hdrprinted++ == 0) {
			if (xfile)
				sendrep (rpfd, MSG_OUT, title_I);
			else if (afile || mfile)
				sendrep (rpfd, MSG_OUT, title_A);
			else
				sendrep (rpfd, MSG_OUT, title);
		}
		if (strcmp (stcp->recfm, "U,b") == 0)
			strcpy (p_recfm, "U,bin");
		else if (strcmp (stcp->recfm, "U,f") == 0)
			strcpy (p_recfm, "U,f77");
		else
			strcpy (p_recfm, stcp->recfm);
		if (stcp->lrecl > 0)
			sprintf (p_lrecl, "%6d", stcp->lrecl);
		else
			strcpy (p_lrecl, "     *");
		if (stcp->size > 0)
			sprintf (p_size, "%d", stcp->size);
		else
			strcpy (p_size, "*");
		if (stcp->status & 0xF00)
			strcpy (p_stat, l_stat[(stcp->status & 0xF00) >> 8]);
		else if (stcp->status & 0xF0)
			strcpy (p_stat, x_stat[(stcp->status & 0xF0) >> 4]);
		else if (stcp->status == STAGEALLOC)
			strcpy (p_stat, "STAGEALLOC");
		else
			strcpy (p_stat, s_stat[stcp->status]);
		if ((lflag || ((stcp->status & 0xFF0) == 0)) &&
		    rfio_mstat (stcp->ipath, &st) == 0) {
			int has_been_updated = 0;

			if (st.st_size > stcp->actual_size) {
				stcp->actual_size = st.st_size;
                has_been_updated = 1;
          }
          if (st.st_atime > stcp->a_time) {
            stcp->a_time = st.st_atime;
            has_been_updated = 1;
          }
          if (st.st_mtime > stcp->a_time) {
            stcp->a_time = st.st_mtime;
            has_been_updated = 1;
          }
          if (has_been_updated != 0) {
            if (stgdb_upd_stgcat(dbfd_query,stcp) != 0) {
              sendrep(rpfd, MSG_ERR, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
            }
          }
		}
		if (stcp->t_or_d == 't') {
		    if (xflag) {
			if (sendrep (rpfd, MSG_OUT,
			    "%-6s %6s %-3s %-5s%s %6d %-11s %5d %6.1f/%-4s %-14s %6d %s\n",
			    stcp->u1.t.vid[0], stcp->u1.t.fseq, stcp->u1.t.lbl,
			    p_recfm, p_lrecl, stcp->blksize, p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname, stcp->reqid, stcp->ipath) < 0) {
				c = SYERR;
				goto reply;
			}
		    } else {
			if (sendrep (rpfd, MSG_OUT,
			    "%-6s %6s %-3s %-5s%s %6d %-11s %5d %6.1f/%-4s %s\n",
			    stcp->u1.t.vid[0], stcp->u1.t.fseq, stcp->u1.t.lbl,
			    p_recfm, p_lrecl, stcp->blksize, p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname) < 0) {
				c = SYERR;
				goto reply;
			}
		    }
		} else if ((stcp->t_or_d == 'd') || (stcp->t_or_d == 'a')) {
		    if ((q = strrchr (stcp->u1.d.xfile, '/')) == NULL)
			q = stcp->u1.d.xfile;
		    else
			q++;
		    if (xflag) {
			if ((stcp->t_or_d == 'd' && sendrep (rpfd, MSG_OUT,
			    "%-17s %-3s  %s %6d %-11s %5d %6.1f/%-4s %-14s %6d %s\n", q,
			    stcp->recfm, p_lrecl, stcp->blksize, p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname, stcp->reqid, stcp->ipath) < 0) ||
			    (stcp->t_or_d == 'a' && sendrep (rpfd, MSG_OUT,
			    "%-36s %-11s %5d %6.1f/%-4s %-14s %6d %s\n", q,
			    p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname, stcp->reqid, stcp->ipath) < 0)) {
				c = SYERR;
				goto reply;
			}
		    } else {
			if ((stcp->t_or_d == 'd' && sendrep (rpfd, MSG_OUT,
			    "%-17s %-3s  %s %6d %-11s %5d %6.1f/%-4s %s\n", q,
			    stcp->recfm, p_lrecl, stcp->blksize, p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname) < 0) ||
			    (stcp->t_or_d == 'a' && sendrep (rpfd, MSG_OUT,
			    "%-36s %-11s %5d %6.1f/%-4s %s\n", q,
			    p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname) < 0)) {
				c = SYERR;
				goto reply;
			}
		    }
		} else {	/* stcp->t_or_d == 'm' */
		    if ((q = strrchr (stcp->u1.m.xfile, '/')) == NULL)
			q = stcp->u1.m.xfile;
		    else
			q++;
		    if (xflag) {
			if (sendrep (rpfd, MSG_OUT,
			    "%-36s %-11s %5d %6.1f/%-4s %-14s %6d %s\n", q,
			    p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname, stcp->reqid, stcp->ipath) < 0) {
				c = SYERR;
				goto reply;
			}
		    } else {
			if (sendrep (rpfd, MSG_OUT,
			    "%-36s %-11s %5d %6.1f/%-4s %s\n", q,
			    p_stat, stcp->nbaccesses,
			    (float)(stcp->actual_size)/(1024.*1024.), p_size,
			    stcp->poolname) < 0) {
				c = SYERR;
				goto reply;
			}
		    }
		}
		if (fflag) {
			if ((stcp->t_or_d == 'a') || (stcp->t_or_d == 'd')) {
				if (sendrep (rpfd, MSG_OUT, " %s\n",
				    stcp->u1.d.xfile) < 0) {
					c = SYERR;
					goto reply;
				}
			} else if (stcp->t_or_d == 'm') {
				if (sendrep (rpfd, MSG_OUT, " %s\n",
				    stcp->u1.m.xfile) < 0) {
					c = SYERR;
					goto reply;
				}
			}
		}
		if (lflag) {
			tm = localtime (&stcp->c_time);
			if (sendrep (rpfd, MSG_OUT,
			    "\t\t\tcreated by  %-8.8s  %s  %04d/%02d/%02d %02d:%02d:%02d\n",
			    stcp->user, stcp->group,
			    tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			    tm->tm_hour, tm->tm_min, tm->tm_sec) < 0) {
				c = SYERR;
				goto reply;
			}
			tm = localtime (&stcp->a_time);
			if (sendrep (rpfd, MSG_OUT,
			    "\t\t\tlast access               %04d/%02d/%02d %02d:%02d:%02d\n",
			    tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			    tm->tm_hour, tm->tm_min, tm->tm_sec) < 0) {
				c = SYERR;
				goto reply;
			}
		}
	}
	rfio_end();
reply:
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
	if (afile || mfile)
		regfree (&preg);
#endif
	free (argv);
	sendrep (rpfd, STAGERC, STAGEQRY, c);
	if (pid == 0) {	/* we are in the child */
      if (stgdb_close(dbfd_query) != 0) {
        sendrep(rpfd, MSG_ERR, STG100, "close", sstrerror(serrno), __FILE__, __LINE__);
      }
      if (stgdb_logout(dbfd_query) != 0) {
        sendrep(rpfd, MSG_ERR, STG100, "logout", sstrerror(serrno), __FILE__, __LINE__);
      }
      exit (c);
    }
}

void print_link_list(poolname, aflag, group, uflag, user, numvid, vid, fseq, xfile, afile, mfile)
char *poolname;
int aflag;
char *group;
int uflag;
char *user;
int numvid;
char vid[MAXVSN][7];
char *fseq;
char *xfile;
char *afile;
char *mfile;
{
	int j;
	char *p;
	int poolflag = 0;
	struct stgcat_entry *stcp;
	struct stgpath_entry *stpp;

	if (strcmp (poolname, "NOPOOL") == 0)
		poolflag = -1;
	if (numvid == 0 && fseq == NULL && afile == NULL && mfile == NULL && xfile == NULL) {
		for (stpp = stps; stpp < stpe; stpp++) {
			if (stpp->reqid == 0) break;
			sendrep (rpfd, MSG_OUT, "%s\n", stpp->upath);
		}
		return;
	}
	for (stcp = stcs; stcp < stce; stcp++) {
		if (stcp->reqid == 0) break;
		if ((stcp->status & 0xF0) != STAGED) continue;
		if (poolflag < 0) {	/* -p NOPOOL */
			if (stcp->poolname[0]) continue;
		} else if (*poolname && strcmp (poolname, stcp->poolname)) continue;
		if (!aflag && strcmp (group, stcp->group)) continue;
		if (uflag && strcmp (user, stcp->user)) continue;
		if (numvid) {
			if (stcp->t_or_d != 't') continue;
			for (j = 0; j < numvid; j++)
				if (strcmp (stcp->u1.t.vid[0], vid[j]) == 0) break;
			if (j == numvid) continue;
		}
		if (fseq) {
			if (stcp->t_or_d != 't') continue;
			if (strcmp (fseq, stcp->u1.t.fseq)) continue;
		}
		if (xfile) {
			if (stcp->t_or_d != 'd') continue;
			if (strcmp (xfile, stcp->u1.d.xfile)) continue;
		}
		if (afile) {
			if (stcp->t_or_d != 'a') continue;
			if ((p = strrchr (stcp->u1.d.xfile, '/')) == NULL)
				p = stcp->u1.d.xfile;
			else
				p++;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regexec (&preg, p, 1, &pmatch, 0)) continue;
#else
			if (step (p, expbuf) == 0) continue;
#endif
		}
		if (mfile) {
			if (stcp->t_or_d != 'm') continue;
			p = stcp->u1.m.xfile;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regexec (&preg, p, 1, &pmatch, 0)) continue;
#else
			if (step (p, expbuf) == 0) continue;
#endif
		}
		for (stpp = stps; stpp < stpe; stpp++) {
			if (stpp->reqid == 0) break;
			if (stcp->reqid == stpp->reqid)
				sendrep (rpfd, MSG_OUT, "%s\n", stpp->upath);
		}
	}
}

print_sorted_list(poolname, aflag, group, uflag, user, numvid, vid, fseq, xfile, afile, mfile)
char *poolname;
int aflag;
char *group;
int uflag;
char *user;
int numvid;
char vid[MAXVSN][7];
char *fseq;
char *xfile;
char *afile;
char *mfile;
{
	/* We use the weight algorithm defined by Fabrizio Cane for DPM */

	int found;
	int j;
	char *p;
	int poolflag = 0;
	struct sorted_ent *prev, *scc, *sci, *scf, *scs;
	struct stat st;
	struct stgcat_entry *stcp;
	struct stgpath_entry *stpp;
	struct tm *tm;

	if (strcmp (poolname, "NOPOOL") == 0)
		poolflag = -1;

	/* build a sorted list of stage catalog entries for the specified pool */
	scf = NULL;
	scs = (struct sorted_ent *) calloc (nbcat_ent, sizeof(struct sorted_ent));
	sci = scs;
	for (stcp = stcs; stcp < stce; stcp++) {
		if (stcp->reqid == 0) break;
		if ((stcp->status & 0xF0) != STAGED) continue;
		if (poolflag < 0) {	/* -p NOPOOL */
			 if (stcp->poolname[0]) continue;
		} else if (*poolname && strcmp (poolname, stcp->poolname)) continue;
		if (!aflag && strcmp (group, stcp->group)) continue;
		if (uflag && strcmp (user, stcp->user)) continue;
		if (numvid) {
			if (stcp->t_or_d != 't') continue;
			for (j = 0; j < numvid; j++)
				if (strcmp (stcp->u1.t.vid[0], vid[j]) == 0) break;
			if (j == numvid) continue;
		}
		if (fseq) {
			if (stcp->t_or_d != 't') continue;
			if (strcmp (fseq, stcp->u1.t.fseq)) continue;
		}
		if (xfile) {
			if (stcp->t_or_d != 'd') continue;
			if (strcmp (xfile, stcp->u1.d.xfile)) continue;
		}
		if (afile) {
			if (stcp->t_or_d != 'a') continue;
			if ((p = strrchr (stcp->u1.d.xfile, '/')) == NULL)
				p = stcp->u1.d.xfile;
			else
				p++;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regexec (&preg, p, 1, &pmatch, 0)) continue;
#else
			if (step (p, expbuf) == 0) continue;
#endif
		}
		if (mfile) {
			if (stcp->t_or_d != 'm') continue;
			p = stcp->u1.m.xfile;
#if defined(_IBMR2) || defined(hpux) || (defined(__osf__) && defined(__alpha)) || defined(linux)
			if (regexec (&preg, p, 1, &pmatch, 0)) continue;
#else
			if (step (p, expbuf) == 0) continue;
#endif
		}
		if (rfio_mstat (stcp->ipath, &st) == 0) {
			int has_been_updated = 0;

          if (st.st_size > stcp->actual_size) {
            stcp->actual_size = st.st_size;
            has_been_updated = 1;
          }
          if (st.st_atime > stcp->a_time) {
            stcp->a_time = st.st_atime;
            has_been_updated = 1;
          }
          if (st.st_mtime > stcp->a_time) {
            stcp->a_time = st.st_mtime;
            has_been_updated = 1;
          }
          if (has_been_updated != 0) {
            if (stgdb_upd_stgcat(dbfd_query,stcp) != 0) {
              sendrep(rpfd, MSG_ERR, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
            }
          }
		}
		sci->weight = (double)stcp->a_time;
		if (stcp->actual_size > 1024)
			sci->weight -=
				(86400.0 * log((double)stcp->actual_size/1024.0));
		if (scf == NULL) {
			scf = sci;
		} else {
			prev = NULL;
			scc = scf;
			while (scc && scc->weight <= sci->weight) {
				prev = scc;
				scc = scc->next;
			}
			if (! prev) {
				sci->next = scf;
				scf = sci;
			} else {
				prev->next = sci;
				sci->next = scc;
			}
		}
		found = 0;
		for (stpp = stps; stpp < stpe; stpp++) {
			if (stpp->reqid == 0) break;
			if (stcp->reqid == stpp->reqid) {
				found = 1;
				break;
			}
		}
		if (found)
			sci->stpp = stpp;
		sci->stcp = stcp;
		sci++;
	}
	rfio_end();

	/* output the sorted list */

	for (scc = scf; scc; scc = scc->next) {
		tm = localtime (&scc->stcp->a_time);
		if (sendrep (rpfd, MSG_OUT,
			"%04d/%02d/%02d %02d:%02d:%02d %6.1f %4d %s %s\n",
			tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec,
			((float)(scc->stcp->actual_size))/(1024.*1024.),
			scc->stcp->nbaccesses, scc->stcp->ipath,
			(scc->stpp) ? scc->stpp->upath : scc->stcp->ipath) < 0) {
				free (scs);
				return (-1);
		}
	}
	free (scs);
	return (0);
}

void print_tape_info(poolname, aflag, group, uflag, user, numvid, vid, fseq)
char *poolname;
int aflag;
char *group;
int uflag;
char *user;
int numvid;
char vid[MAXVSN][7];
char *fseq;
{
	int j;
	int poolflag = 0;
	struct stgcat_entry *stcp;

	if (strcmp (poolname, "NOPOOL") == 0)
		poolflag = -1;
	for (stcp = stcs; stcp < stce; stcp++) {
		if (stcp->reqid == 0) break;
		if ((stcp->status & 0xF0) != STAGED) continue;
		if (poolflag < 0) {	/* -p NOPOOL */
			if (stcp->poolname[0]) continue;
		} else if (*poolname && strcmp (poolname, stcp->poolname)) continue;
		if (!aflag && strcmp (group, stcp->group)) continue;
		if (uflag && strcmp (user, stcp->user)) continue;
		if (stcp->t_or_d != 't') continue;
		if (numvid) {
			for (j = 0; j < numvid; j++)
				if (strcmp (stcp->u1.t.vid[0], vid[j]) == 0) break;
			if (j == numvid) continue;
			if (fseq && strcmp (fseq, stcp->u1.t.fseq)) continue;
		}
		if (strcmp (stcp->u1.t.lbl, "al") &&
		    strcmp (stcp->u1.t.lbl, "sl")) continue;
		sendrep (rpfd, MSG_OUT, "-b %d -F %s -f %s -L %d\n",
			stcp->blksize, stcp->recfm, stcp->u1.t.fid, stcp->lrecl);
	}
}
