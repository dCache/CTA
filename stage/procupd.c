/*
 * $Id: procupd.c,v 1.3 1999/07/20 17:29:18 jdurand Exp $
 *
 * $Log: procupd.c,v $
 * Revision 1.3  1999/07/20 17:29:18  jdurand
 * Added Id and Log CVS's directives
 *
 */

/*
 * Copyright (C) 1993-1998 by CERN/CN/PDP/DH
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)procupd.c	1.21 08/26/98 CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#if defined(_WIN32)
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif
#include <sys/stat.h>
#include "marshall.h"
#undef  unmarshall_STRING
#define unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#include "stage.h"
#if SACCT
#include "../h/sacct.h"
#endif
#ifdef DB
#include <Cdb_api.h>
#include "wrapdb.h"
#endif /* DB */

extern char *optarg;
extern int optind;
extern char *rfio_serror();
extern char func[16];
extern int reqid;
extern int rpfd;
extern struct stgcat_entry *stce;	/* end of stage catalog */
extern struct stgcat_entry *stcs;	/* start of stage catalog */
extern struct waitq *waitqp;
#ifdef DB
extern db_fd **db_stgcat;
extern db_fd **db_stgpath;
#endif /* DB */

procupdreq(req_data, clienthost)
char *req_data;
char *clienthost;
{
	char **argv;
	int blksize = -1;
	int c, i, n;
	struct stgcat_entry *cur;
	char *dp;
	char dsksrvr[MAXHOSTNAMELEN];
	char *dvn;
	int errflg = 0;
	char *fid = NULL;
	int found;
	char *fseq = NULL;
	gid_t gid;
	char *ifce;
	int j;
	int key;
	int lrecl = -1;
	int nargs;
	char *p_cmd;
	char *p_stat;
	char *p, *q;
	char prevfseq[MAXFSEQ];
	char *rbp;
	int rc = -1;
	char *recfm = NULL;
	static char s_cmd[5][9] = {"", "stagein", "stageout", "stagewrt", "stageput"};
	int size = 0;
	struct stat st;
	struct stgcat_entry *stcp;
	int subreqid;
	int transfer_time = 0;
	uid_t uid;
	static char unknown[2] = {"*"};
	int upd_reqid;
	int upd_rpfd;
	char *user;
	int waiting_time = 0;
	struct waitf *wfp;
	struct waitq *wqp;
	int Zflag = 0;
#ifdef DB
  int changed;
#endif /* DB */

	rbp = req_data;
	unmarshall_STRING (rbp, user);	/* login name */
	unmarshall_WORD (rbp, uid);
	unmarshall_WORD (rbp, gid);
	nargs = req2argv (rbp, &argv);
#if SACCT
	stageacct (STGCMDR, uid, gid, clienthost,
		reqid, STAGEUPDC, 0, 0, NULL, "");
#endif

	dvn = unknown;
	ifce = unknown;
	upd_reqid = reqid;
	upd_rpfd = rpfd;
	optind = 1;
	while ((c = getopt (nargs, argv, "b:D:F:f:h:I:L:q:R:s:T:U:W:Z:")) != EOF) {
		switch (c) {
		case 'b':
			blksize = strtol (optarg, &dp, 10);
			if (*dp != '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-b");
				errflg++;
			}
			break;
		case 'D':
			dvn = optarg;
			break;
		case 'F':
			recfm = optarg;
			break;
		case 'f':
			fid = optarg;
			break;
		case 'h':
			break;
		case 'I':
			ifce = optarg;
			break;
		case 'L':
			lrecl = strtol (optarg, &dp, 10);
			if (*dp != '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-L");
				errflg++;
			}
			break;
		case 'q':
			fseq = optarg;
			break;
		case 'R':
			rc = strtol (optarg, &dp, 10);
			if (*dp != '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-R");
				errflg++;
			}
			break;
		case 's':
			size = strtol (optarg, &dp, 10);
			if (*dp != '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-s");
				errflg++;
			}
			break;
		case 'T':
			transfer_time = strtol (optarg, &dp, 10);
			if (*dp != '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-T");
				errflg++;
			}
			break;
		case 'W':
			waiting_time = strtol (optarg, &dp, 10);
			if (*dp != '\0') {
				sendrep (rpfd, MSG_ERR, STG06, "-W");
				errflg++;
			}
			break;
		case 'Z':
			Zflag++;
			p = strtok (optarg, ".");
			reqid = strtol (p, &dp, 10);
			p = strtok (NULL, "@");
			key = strtol (p, &dp, 10);
			break;
		}
	}
	if (Zflag && nargs > optind) {
		sendrep (rpfd, MSG_ERR, STG16);
		errflg++;
	}
	if (errflg) {
		c = USERR;
		goto reply;
	}
	if (nargs > optind) {
		for  (i = optind; i < nargs; i++)
			if (c = upd_stageout (STAGEUPDC, argv[i], &subreqid))
				goto reply;
		goto reply;
	}
	found = 0;
	wqp = waitqp;
	while (wqp) {
		if (wqp->reqid == reqid &&
		    wqp->key == key &&
		    wqp->uid == uid &&
		    wqp->gid == gid) {
			found = 1;
			break;
		}
		wqp = wqp->next;
	}
	if (! found) {
		sendrep (rpfd, MSG_ERR, STG22);
		c = USERR;
		goto reply;
	}
	c = 0;
	for (i = 0, wfp = wqp->wf; i < wqp->nbdskf; i++, wfp++)
		if (! wfp->waiting_on_req) break;
	subreqid = wfp->subreqid;
	found = 0;
#ifdef DB
    if (wrapCdb_fetch(db_stgcat, subreqid, (void **) &stcp, NULL, 0) == 0)
      found = 1;
#else /* DB */
	for (stcp = stcs; stcp < stce; stcp++) {
		if (stcp->reqid == 0) break;
		if (stcp->reqid == subreqid) {
			found = 1;
			break;
		}
	}
#endif /* DB */
	if (! found) {
		sendrep (rpfd, MSG_ERR, STG22);
		c = USERR;
		goto reply;
	}
	if ( rc < 0) {	/* -R not specified, i.e. tape mounted and positionned */
		if (stcp->ipath[0] == '\0') {	/* deferred allocation */
			c = build_ipath (wfp->upath, stcp, wqp->pool_user);
			if (c > 0) goto reply;
			if (c < 0) {
				wqp->clnreq_reqid = upd_reqid;
				wqp->clnreq_rpfd = rpfd;
				stcp->status |= WAITING_SPC;
#ifdef DB
                if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
                  sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
                  c = SYERR;
                  goto reply;
                }
#endif
				strcpy (wqp->waiting_pool, stcp->poolname);
				if (c = cleanpool (stcp->poolname)) goto reply;
				return;
			}
		}
		if (fseq) {
			if (*stcp->poolname &&
			     stcp->u1.t.fseq[0] == 'u') {
				p = strrchr (stcp->ipath, '/') + 1;
				sprintf (p, "%s.%s.%s",
				    stcp->u1.t.vid[0], fseq, stcp->u1.t.lbl);
#ifdef DB
                if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
                  sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
                  c = SYERR;
                  goto reply;
                }
#else
				savereqs ();
#endif /* DB */
			}
		}
		sendrep (rpfd, MSG_OUT, stcp->ipath);
		goto reply;
	}
	if (p = strchr (stcp->ipath, ':')) {
		q = stcp->ipath;
	} else {
		q = strchr (stcp->ipath + 1, '/') + 1;
		p = strchr (q, '/');
	}
	strncpy (dsksrvr, q, p - q);
	dsksrvr[p - q] = '\0';
	stglogit (func, STG97, dsksrvr, strrchr (stcp->ipath, '/')+1,
		stcp->user, stcp->group,
		clienthost, dvn, ifce, size, waiting_time, transfer_time, rc);
	if (stcp->status == STAGEIN && rfio_stat (stcp->ipath, &st) == 0) {
		stcp->actual_size = st.st_size;
#ifdef DB
        if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
          sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
          c = SYERR;
          goto reply;
        }
#endif
    }
#if SACCT
	stageacct (STGFILS, wqp->uid, wqp->gid, wqp->clienthost,
		wqp->reqid, wqp->req_type, wqp->nretry, rc, stcp, clienthost);
#endif
	if (rc == 211) {	/* no more file on tape */
		sprintf (prevfseq, "%d", atoi (stcp->u1.t.fseq) - 1);
#ifdef DB
        if ((cur = (struct stgcat_entry *) calloc(1,sizeof(struct stgcat_entry))) == NULL) {
          stglogit (func, STG05);
          exit (SYERR);
        }
        {
          /* We loop on the "status" database */
          size_t db_size;
          char *db_key = NULL;
          char *db_data = NULL;
          Cdb_altkey_rewind(db_stgcat, "status");
          while (Cdb_altkey_nextrec(db_stgcat, "status", &db_key, (void **) &db_data, &db_size) != 0) {
            char *ptr, *ptrmax;
            int thisstatus = atoi(db_key);
            
            if ((stcp->status & 0xF0) != STAGED) continue;
            ptrmax = ptr = db_data;
            ptrmax += db_size;
            do {
              size_t stcp_size;
              int reqid;
              
              reqid = atoi(ptr);
              /* In case of a string, sizeof() == strlen() + 1 */
              /* ptr points to the reqid (as a string, e.g. a master key) */
              if (wrapCdb_fetch(db_stgcat,reqid,(void **) &stcp, NULL, 0) == 0) {
                if (stcp->t_or_d != 't') goto docontinue;
                for (j = 0; j < MAXVSN; j++)
                  if (strcmp (cur->u1.t.vid[j], stcp->u1.t.vid[j])) break;
                if (j < MAXVSN) goto docontinue;
                for (j = 0; j < MAXVSN; j++)
                  if (strcmp (cur->u1.t.vsn[j], stcp->u1.t.vsn[j])) break;
                if (j < MAXVSN) goto docontinue;
                if (strcmp (cur->u1.t.lbl, stcp->u1.t.lbl)) goto docontinue;
                if (strcmp (prevfseq, stcp->u1.t.fseq)) goto docontinue;
                stcp->status |= LAST_TPFILE;
                if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
                  sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
                }
                break;
              }
            docontinue:
              ptr += (strlen(ptr) + 1);
            } while (ptr < ptrmax);
          }
          if (stcp != NULL)
            free(stcp);
          if (db_key != NULL)
            free(db_key);
          if (db_data != NULL)
            free(db_data);
        }
#else /* DB */
		/* flag previous file as last file on tape */
        cur = stcp;
		for (stcp = stcs; stcp < stce; stcp++) {
			if (stcp->reqid == 0) break;
			if (stcp->t_or_d != 't') continue;
			if ((stcp->status & 0xF0) != STAGED) continue;
			for (j = 0; j < MAXVSN; j++)
				if (strcmp (cur->u1.t.vid[j], stcp->u1.t.vid[j])) break;
			if (j < MAXVSN) continue;
			for (j = 0; j < MAXVSN; j++)
				if (strcmp (cur->u1.t.vsn[j], stcp->u1.t.vsn[j])) break;
			if (j < MAXVSN) continue;
			if (strcmp (cur->u1.t.lbl, stcp->u1.t.lbl)) continue;
			if (strcmp (prevfseq, stcp->u1.t.fseq)) continue;
			stcp->status |= LAST_TPFILE;
			break;
		}
#endif /* DB */
#ifdef DB
        if (cur != NULL)
          free(cur);
#endif /* DB */
		wqp->nb_subreqs = i;
		for ( ; i < wqp->nbdskf; i++, wfp++) {
			for (stcp = stcs; stcp < stce; stcp++) {
#ifdef DB
              if (wrapCdb_fetch(db_stgcat, wfp->subreqid, (void **) &stcp, NULL, 0)) {
                sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
                continue;
              }
#else /* DB */
				if (wfp->subreqid == stcp->reqid)
					break;
#endif /* DB */
			}
			if (wfp->waiting_on_req) {
				delreq (stcp);
				continue;
			}
			if (delfile (stcp, 1, 0, 1, "no more file", uid, gid, 0) < 0)
				sendrep (wqp->rpfd, MSG_ERR, STG02, stcp->ipath,
				    "rfio_unlink", rfio_serror());
			check_waiting_on_req (wfp->subreqid, FAILED);
		}
		wqp->nbdskf = wqp->nb_subreqs;
		goto reply;
	}
	p_cmd = s_cmd[stcp->status & 0xF];
	if (rc == 0)
		p_stat = "succeeded";
	else if (rc == LIMBYSZ || rc == BLKSKPD || rc == TPE_LSZ ||
	    (rc == MNYPARI && (stcp->u1.t.E_Tflags & KEEPFILE)))
		p_stat = "warning";
	else
		p_stat = "failed";
	if (stcp->t_or_d == 't')
		sendrep (wqp->rpfd, MSG_ERR, STG41, p_cmd, p_stat,
			fseq ? fseq : stcp->u1.t.fseq, stcp->u1.t.vid[0], rc);
	else if (stcp->t_or_d == 'd')
		sendrep (wqp->rpfd, MSG_ERR, STG42, p_cmd, p_stat, stcp->u1.d.xfile, rc);
	else if (stcp->t_or_d == 'm')
		sendrep (wqp->rpfd, MSG_ERR, STG42, p_cmd, p_stat, stcp->u1.m.xfile, rc);
	if ((rc != 0 && rc != LIMBYSZ &&
	    rc != BLKSKPD && rc != TPE_LSZ && rc != MNYPARI) ||
	    (rc == MNYPARI && (stcp->u1.t.E_Tflags & KEEPFILE) == 0)) {
		wqp->status = rc;
		if (rc != ENOSPC) goto reply;
		if (*stcp->poolname == '\0' ||
		    stcp->status == STAGEWRT || stcp->status == STAGEPUT ||
		    wqp->nb_clnreq++ > MAXRETRY) {
			c = ENOSPC;
			goto reply;
		}
		if (delfile (stcp, 1, 0, 0, "nospace retry", uid, gid, 0) < 0)
			sendrep (wqp->rpfd, MSG_ERR, STG02, stcp->ipath,
				"rfio_unlink", rfio_serror());
		wqp->status = 0;
		wqp->clnreq_reqid = upd_reqid;
		wqp->clnreq_rpfd = rpfd;
		stcp->status |= WAITING_SPC;
#ifdef DB
        if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
          sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
        }
#endif /* DB */
		strcpy (wqp->waiting_pool, stcp->poolname);
		if (c = cleanpool (stcp->poolname)) goto reply;
		return;
	}
	if (blksize > 0) {
      stcp->blksize = blksize;
#ifdef DB
      ++changed;
#endif /* DB */
    }
	if (lrecl > 0) {
      stcp->lrecl = lrecl;
#ifdef DB
      ++changed;
#endif /* DB */
    }
	if (recfm) {
      strncpy (stcp->recfm, recfm, 3);
#ifdef DB
      ++changed;
#endif /* DB */
    }
	if (stcp->recfm[0] == 'U') {
      stcp->lrecl = 0;
#ifdef DB
      ++changed;
#endif /* DB */
    }
	else if (stcp->lrecl == 0) {
      stcp->lrecl = stcp->blksize;
#ifdef DB
      ++changed;
#endif /* DB */
    }
	if (fid) {
      strcpy (stcp->u1.t.fid, fid);
#ifdef DB
      ++changed;
#endif /* DB */
    }
	if (fseq &&
	    (stcp->u1.t.fseq[0] == 'u' || stcp->u1.t.fseq[0] == 'n')) {
      strcpy (stcp->u1.t.fseq, fseq);
#ifdef DB
      ++changed;
#endif /* DB */
    }
#ifdef DB
  if (changed) {
    if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
      sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
    }
  }
#endif /* DB */
	wqp->nb_subreqs--;
	wqp->nbdskf--;
	rpfd = wqp->rpfd;
	if (stcp->status == STAGEWRT || stcp->status == STAGEPUT) {
		n = 1;
		if (wqp->nb_subreqs == 0 && wqp->nbdskf > 0) {
			n += wqp->nbdskf;
			wqp->nbdskf = 0;
		}
		for (i = 0; i < n; i++, wfp++) {
#ifdef DB
          if (wrapCdb_fetch(db_stgcat, wfp->subreqid, (void **) &stcp, NULL, 0) != 0) {
            sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
            continue;
          }
#else /* DB */
			for (stcp = stcs; stcp < stce; stcp++)
				if (stcp->reqid == wfp->subreqid) break;
#endif
			if (wqp->copytape)
				sendinfo2cptape (rpfd, stcp);
			if (! stcp->keep && stcp->nbaccesses <= 1) {
				if (stcp->status == STAGEWRT && stcp->poolname[0] == '\0')
					delreq (stcp);
				else if (delfile (stcp, 0, 1, 1, "stagewrt ok", uid, gid, 0) < 0) {
					sendrep (rpfd, MSG_ERR, STG02, stcp->ipath,
						"rfio_unlink", rfio_serror());
					stcp->status |= STAGED;
#ifdef DB
                    if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0)) {
                      sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
                      continue;
                    }
#endif /* DB */
				}
			} else {
				stcp->status |= STAGED;
#ifdef DB
            if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
              sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
              continue;
            }
#endif /* DB */
            }
		}
		i = 0;		/* reset these variables changed by the above loop */
		wfp = wqp->wf;	/* and needed in the loop below */
	} else {	/* STAGEIN */
		stcp->status |= STAGED;
		if (rc == BLKSKPD || rc == TPE_LSZ || rc == MNYPARI)
			stcp->status |= STAGED_TPE;
		if (rc == LIMBYSZ || rc == TPE_LSZ)
			stcp->status |= STAGED_LSZ;
#ifdef DB
        if (wrapCdb_store(db_stgcat,stcp->reqid,stcp,sizeof(struct stgcat_entry),DB_ALWAYS,0) != 0) {
          sendrep (rpfd, MSG_ERR, STG102, __FILE__, __LINE__, errno, db_strerror(errno));
          goto reply;
        }
#endif /* DB */
		if (wqp->copytape)
			sendinfo2cptape (rpfd, stcp);
		if (*(wfp->upath) && strcmp (stcp->ipath, wfp->upath))
			create_link (stcp, wfp->upath);
		if (wqp->Upluspath &&
		    strcmp (stcp->ipath, (wfp+1)->upath))
			create_link (stcp, (wfp+1)->upath);
		updfreespace (stcp->poolname, stcp->ipath,
			stcp->size*1024*1024 - (int)stcp->actual_size);
		check_waiting_on_req (subreqid, STAGED);
	}
	savereqs  ();
	for ( ; i < wqp->nbdskf; i++, wfp++) {
		wfp->subreqid = (wfp+1)->subreqid;
		wfp->waiting_on_req = (wfp+1)->waiting_on_req;
		strcpy (wfp->upath, (wfp+1)->upath);
	}
	if (rc == MNYPARI) wqp->status = rc;
reply:
#ifdef DB
    if (stcp != NULL)
      free(stcp);
#endif /* DB */
	free (argv);
	reqid = upd_reqid;
	rpfd = upd_rpfd;
	sendrep (rpfd, STAGERC, STAGEUPDC, c);
}
