/*
 * $Id: poolmgr.c,v 1.116 2001/03/22 10:59:22 jdurand Exp $
 */

/*
 * Copyright (C) 1993-2000 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: poolmgr.c,v $ $Revision: 1.116 $ $Date: 2001/03/22 10:59:22 $ CERN IT-PDP/DM Jean-Philippe Baud Jean-Damien Durand";
#endif /* not lint */

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef _WIN32
#include <unistd.h>
#include <signal.h>
#endif
#include "osdep.h"
#ifndef _WIN32
#include <netinet/in.h>
#endif
#include <errno.h>
#include "rfio_api.h"
#include "stage.h"
#include "stage_api.h"
#include "u64subr.h"
#include "serrno.h"
#include "marshall.h"
#include "osdep.h"
#ifdef USECDB
#include "stgdb_Cdb_ifce.h"
#endif
#include "Cns_api.h"
#include "Castor_limits.h"
#include "Cpwd.h"
#include "Cgrp.h"

#undef  unmarshall_STRING
#define unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#if defined(_WIN32)
static char strftime_format[] = "%b %d %H:%M:%S";
#else /* _WIN32 */
static char strftime_format[] = "%b %e %H:%M:%S";
#endif /* _WIN32 */

#if (defined(IRIX5) || defined(IRIX6) || defined(IRIX64))
/* Surpringly, on Silicon Graphics, strdup declaration depends on non-obvious macros */
extern char *strdup _PROTO((CONST char *));
#endif

extern char *getconfent();
extern char defpoolname[];
extern char defpoolname_in[];
extern char defpoolname_out[];
extern char currentpool_out[];
extern int rpfd;
extern int req2argv _PROTO((char *, char ***));
extern struct stgcat_entry *stce;	/* end of stage catalog */
extern struct stgcat_entry *stcs;	/* start of stage catalog */
extern struct stgpath_entry *stps;	/* start of stage path catalog */
extern int maxfds;
extern int reqid;
extern int stglogit _PROTO(());
#if (defined(IRIX64) || defined(IRIX5) || defined(IRIX6))
extern int sendrep _PROTO((int, int, ...));
#else
extern int sendrep _PROTO(());
#endif
extern struct stgcat_entry *newreq _PROTO(());
extern int delfile _PROTO((struct stgcat_entry *, int, int, int, char *, uid_t, gid_t, int, int));
extern int nextreqid _PROTO(());

#if !defined(linux)
extern char *sys_errlist[];
#endif
static struct migrator *migrators;
struct fileclass *fileclasses;
static int nbmigrator;
static int nbpool;
static char *nfsroot;
static char **poolc;
static struct pool *pools;
#ifndef _WIN32
struct sigaction sa_poolmgr;
#endif
static int nbfileclasses;
#ifdef USECDB
extern struct stgdb_fd dbfd;
#endif
extern int savereqs _PROTO(());
extern char localhost[CA_MAXHOSTNAMELEN+1];

struct files_per_stream {
  struct stgcat_entry *stcp;
  struct stgpath_entry *stpp;
  int nstcp;
  u_signed64 size;
  uid_t euid;
  gid_t egid;
  char tppool[CA_MAXPOOLNAMELEN+1];
  int nb_substreams;
};

/* This structure is used for the qsort on pool elements */
struct pool_element_ext {
  u_signed64 free;
  int nbreadaccess;
  int nbwriteaccess;
  int mode;
  int nbreadserver;
  int nbwriteserver;
  int poolmigrating;
  char server[CA_MAXHOSTNAMELEN+1];
  char dirpath[MAXPATH];
};

void print_pool_utilization _PROTO((int, char *, char *, char *, char *, int, int, int, int));
int update_migpool _PROTO((struct stgcat_entry **, int, int));
int insert_in_migpool _PROTO((struct stgcat_entry *));
void checkfile2mig _PROTO(());
int migrate_files _PROTO((struct pool *));
int migpoolfiles _PROTO((struct pool *));
int iscleanovl _PROTO((int, int));
void killcleanovl _PROTO((int));
int ismigovl _PROTO((int, int));
void killmigovl _PROTO((int));
int isvalidpool _PROTO((char *));
void migpoolfiles_log_callback _PROTO((int, char *));
int isuserlevel _PROTO((char *));
void poolmgr_wait4child _PROTO(());
int selectfs _PROTO((char *, int *, char *, int));
void rwcountersfs _PROTO((char *, char *, int, int));
void getdefsize _PROTO((char *, int *));
int updfreespace _PROTO((char *, char *, signed64));
void redomigpool _PROTO(());
int updpoolconf _PROTO((char *, char *, char *));
int getpoolconf _PROTO((char *, char *, char *));
int checkpoolcleaned _PROTO((char ***));
void checkpoolspace _PROTO(());
int cleanpool _PROTO((char *));
int get_create_file_option _PROTO((char *));
int poolalloc _PROTO((struct pool *, int));
int checklastaccess _PROTO((char *, time_t));
int enoughfreespace _PROTO((char *, int));
int upd_fileclass _PROTO((struct pool *, struct stgcat_entry *));
int upd_fileclasses _PROTO(());
void upd_last_tppool_used _PROTO((struct fileclass *));
char *next_tppool _PROTO((struct fileclass *));
int euid_egid _PROTO((uid_t *, gid_t *, char *, struct migrator *, struct stgcat_entry *, struct stgcat_entry *, char **, int));
extern int verif_euid_egid _PROTO((uid_t, gid_t, char *, char *));
void stglogfileclass _PROTO((struct Cns_fileclass *));
void printfileclass _PROTO((int, struct fileclass *));
int retenp_on_disk _PROTO((int));
void check_retenp_on_disk _PROTO(());
int ispool_out _PROTO((char *));
void nextpool_out _PROTO((char *));
void bestnextpool_out _PROTO((char *, int));
int ispoolmigrating _PROTO((char *));
struct pool_element *betterfs_vs_pool _PROTO((char *, int, u_signed64, int *));
int pool_elements_cmp _PROTO((CONST void *, CONST void *));
void get_global_stream_count _PROTO((char *, int *, int *));
char *findpoolname _PROTO((char *));

#if hpux
/* On HP-UX seteuid() and setegid() do not exist and have to be wrapped */
/* calls to setresuid().                                                */
#define seteuid(euid) setresuid(-1,euid,-1)
#define setegid(egid) setresgid(-1,egid,-1)
#endif

#if defined(_REENTRANT) || defined(_THREAD_SAFE)
#define strtok(X,Y) strtok_r(X,Y,&last)
#endif /* _REENTRANT || _THREAD_SAFE */

int getpoolconf(defpoolname,defpoolname_in,defpoolname_out)
     char *defpoolname;
     char *defpoolname_in;
     char *defpoolname_out;
{
  char buf[4096];
  int defsize;
  char *dp;
  struct pool_element *elemp;
  int errflg = 0;
  FILE *fopen(), *s;
  char func[16];
  int i, j;
  struct migrator *migr_p;
  int nbpool_ent;
  char *p;
  char path[MAXPATH];
  struct pool *pool_p;
  struct rfstatfs st;
  time_t thistime;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
  char *last = NULL;
#endif /* _REENTRANT || _THREAD_SAFE */
  int nbmigrator_real;

  strcpy (func, "getpoolconf");
  if ((s = fopen (STGCONFIG, "r")) == NULL) {
    stglogit (func, STG23, STGCONFIG);
    return (CONFERR);
  }
	
  /* 1st pass: count number of pools and migrators  */

  migrators = NULL;
  nbmigrator_real = 0;
  nbmigrator = 0;
  nbpool = 0;
  *defpoolname = '\0';
  *defpoolname_in = '\0';
  *defpoolname_out = '\0';
  defsize = 0;
  while (fgets (buf, sizeof(buf), s) != NULL) {
    int gotit;
    if (buf[0] == '#') continue;	/* comment line */
    if ((p = strtok (buf, " \t\n")) == NULL) continue; 
    if (strcmp (p, "POOL") == 0) {
      nbpool++;
    }
    gotit = 0;
    while ((p = strtok (NULL, " \t\n")) != NULL) {
      if (strcmp (p, "MIGRATOR") == 0) {
        if (gotit != 0) {
          stglogit (func, STG29);
          fclose (s);
          return (0);
        }
        nbmigrator++;
        gotit = 1;
      }
    }
  }
  if (nbpool == 0) {
    stglogit (func, STG29);
    fclose (s);
    return (0);
  }
  pools = (struct pool *) calloc (nbpool, sizeof(struct pool));
  if (nbmigrator > 0)
    migrators = (struct migrator *)
      calloc (nbmigrator, sizeof(struct migrator));
  /* Although there is no pid for any migration yet, the element migreqtime */
  /* has to be initialized to current time... */
  thistime = time(NULL);
  for (i = 0; i < nbmigrator; i++) {
    migrators[i].migreqtime_last_end = thistime;
  }

  /* 2nd pass: count number of members in each pool and
     store migration parameters */

  rewind (s);
  nbpool_ent = -1;
  pool_p = pools - 1;
  while (fgets (buf, sizeof(buf), s) != NULL) {
    if (buf[0] == '#') continue;	/* comment line */
    if ((p = strtok (buf, " \t\n")) == NULL) continue;
    if (strcmp (p, "POOL") == 0) {
      if (poolalloc (pool_p, nbpool_ent) < 0) {
        errflg++;
        goto reply;
      }
      nbpool_ent = 0;
      pool_p++;
      if ((p = strtok (NULL, " \t\n")) == NULL) {
        stglogit (func, STG25, "pool");	/* name missing */
        errflg++;
        goto reply;
      }
      if ((int) strlen (p) > CA_MAXPOOLNAMELEN) {
        stglogit (func, STG27, "pool", p);
        errflg++;
        goto reply;
      }
      strcpy (pool_p->name, p);
      while ((p = strtok (NULL, " \t\n")) != NULL) {
        if (strcmp (p, "DEFSIZE") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          pool_p->defsize = strtol (p, &dp, 10);
          if (*dp != '\0' || pool_p->defsize <= 0) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
        } else if ((strcmp (p, "MINFREE") == 0) || (strcmp (p, "GC_STOP_THRESH") == 0)) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          pool_p->gc_stop_thresh = strtol (p, &dp, 10);
          if (*dp != '\0' || pool_p->gc_stop_thresh < 0) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
        } else if (strcmp (p, "GC_START_THRESH") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          pool_p->gc_start_thresh = strtol (p, &dp, 10);
          if (*dp != '\0' || pool_p->gc_start_thresh < 0) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
        } else if (strcmp(p, "GC") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL ||
              strchr (p, ':') == NULL ) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          strcpy (pool_p->gc, p);
        } else if (strcmp(p, "NO_FILE_CREATION") == 0) {
          pool_p->no_file_creation = 1;
        } else if (strcmp (p, "MIGRATOR") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG25, "migrator");	/* name missing */
            errflg++;
            goto reply;
          }
          if ((int) strlen (p) > CA_MAXMIGRNAMELEN) {
            stglogit (func, STG27, "migrator", pool_p->name);
            errflg++;
            goto reply;
          }
          strcpy (pool_p->migr_name, p);
          for (j = 0, migr_p = migrators; j < nbmigrator; j++, migr_p++) {
            if (strcmp (pool_p->migr_name, migr_p->name) == 0) {
              /* Yet defined */
              pool_p->migr = migr_p;
              break;
            }
            if (migr_p->name[0] != '\0') continue; /* Place yet taken by another migrator name... */
            /* migrator name was not yet registered */
            strcpy(migr_p->name, p);
            /* Got a new one */
            pool_p->migr = migr_p;
            nbmigrator_real++;
            break;
          }
        } else if (strcmp (p, "MIG_START_THRESH") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          pool_p->mig_start_thresh = strtol (p, &dp, 10);
          if (*dp != '\0' || pool_p->mig_start_thresh < 0) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
        } else if (strcmp (p, "MIG_STOP_THRESH") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          pool_p->mig_stop_thresh = strtol (p, &dp, 10);
          if (*dp != '\0' || pool_p->mig_stop_thresh < 0) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
        } else if (strcmp (p, "MIG_DATA_THRESH") == 0) {
          if ((p = strtok (NULL, " \t\n")) == NULL) {
            stglogit (func, STG26, "pool", pool_p->name);
            errflg++;
            goto reply;
          }
          pool_p->mig_data_thresh = strutou64 (p);
        } else {
          stglogit (func, STG26, "pool", pool_p->name);
          errflg++;
          goto reply;
        }
      }
    } else if (strcmp (p, "DEFPOOL") == 0) {
      if (poolalloc (pool_p, nbpool_ent) < 0) {
        errflg++;
        goto reply;
      }
      nbpool_ent = -1;
      if ((p = strtok (NULL, " \t\n")) == NULL) {
        stglogit (func, STG30);	/* name missing */
        errflg++;
        goto reply;
      }
      if ((int) strlen (p) > CA_MAXPOOLNAMELEN) {
        stglogit (func, STG27, "pool", p);
        errflg++;
        goto reply;
      }
      strcpy (defpoolname, p);
    } else if (strcmp (p, "DEFPOOL_IN") == 0) {
      if (poolalloc (pool_p, nbpool_ent) < 0) {
        errflg++;
        goto reply;
      }
      nbpool_ent = -1;
      if ((p = strtok (NULL, " \t\n")) == NULL) {
        stglogit (func, STG30);	/* name missing */
        errflg++;
        goto reply;
      }
      if ((int) strlen (p) > CA_MAXPOOLNAMELEN) {
        stglogit (func, STG27, "pool", p);
        errflg++;
        goto reply;
      }
      strcpy (defpoolname_in, p);
    } else if (strcmp (p, "DEFPOOL_OUT") == 0) {
      if (poolalloc (pool_p, nbpool_ent) < 0) {
        errflg++;
        goto reply;
      }
      nbpool_ent = -1;
      if ((p = strtok (NULL, " \t\n")) == NULL) {
        stglogit (func, STG30);	/* name missing */
        errflg++;
        goto reply;
      }
      if ((int) strlen (p) > ((10*(CA_MAXPOOLNAMELEN + 1)) - 1)) {
        stglogit (func, STG27, "pool", p);
        errflg++;
        goto reply;
      }
      strcpy (defpoolname_out, p);
      /* Please note that defpoolname_out can contain multiple entries, separated with a ':' */
    } else if (strcmp (p, "DEFSIZE") == 0) {
      if (poolalloc (pool_p, nbpool_ent) < 0) {
        errflg++;
        goto reply;
      }
      nbpool_ent = -1;
      if ((p = strtok (NULL, " \t\n")) == NULL) {
        stglogit (func, STG28);
        errflg++;
        goto reply;
      }
      defsize = strtol (p, &dp, 10);
      if (*dp != '\0' || defsize <= 0) {
        stglogit (func, STG28);
        errflg++;
        goto reply;
      }
    } else {
      if (nbpool_ent < 0) {
        stglogit (func, STG26, "pool", "");
        errflg++;
        goto reply;
      }
      nbpool_ent++;
    }
  }
  if (poolalloc (pool_p, nbpool_ent) < 0) {
    errflg++;
    goto reply;
  }
  if (*defpoolname == '\0') {
    if (nbpool == 1) {
      strcpy (defpoolname, pools->name);
    } else {
      stglogit (func, STG30);
      errflg++;
      goto reply;
    }
  } else {
    if (! isvalidpool (defpoolname)) {
      stglogit (func, STG32, defpoolname);
      errflg++;
      goto reply;
    }
  }
  if (*defpoolname_in == '\0') {
      strcpy (defpoolname_in, defpoolname);
  } else {
    if (! isvalidpool (defpoolname_in)) {
      stglogit (func, STG32, defpoolname_in);
      errflg++;
      goto reply;
    }
  }
  if (*defpoolname_out == '\0') {
      strcpy (defpoolname_out, defpoolname);
  } else {
    /* We loop all entried separated by the ':' to verify poolnames validity */
    char savdefpoolname_out[10*(CA_MAXPOOLNAMELEN + 1)];
    strcpy(savdefpoolname_out, defpoolname_out);
    if ((p = strtok(defpoolname_out, ":")) != NULL) {
      while (1) {
        if (! isvalidpool (p)) {
          stglogit (func, STG32, p);
          errflg++;
          strcpy(defpoolname_out, savdefpoolname_out);
          goto reply;
        }
        if ((p = strtok(NULL, ":")) == NULL) break;
      }
    }
    strcpy(defpoolname_out, savdefpoolname_out);
  }

  /* Reduce number of migrator in case of pools sharing the same one */
  if ((nbmigrator_real > 0) && (nbmigrator > 0) && (nbmigrator_real < nbmigrator)) {
    struct migrator *dummy;
    
    dummy = (struct migrator *) realloc(migrators, nbmigrator_real * sizeof(struct migrator));
    migrators = dummy;
    nbmigrator = nbmigrator_real;
  }

  /* associate pools with migrators */

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (! *pool_p->migr_name) continue;
    for (j = 0, migr_p = migrators; j < nbmigrator; j++, migr_p++)
      if (strcmp (pool_p->migr_name, migr_p->name) == 0) break;
    if (nbmigrator == 0 || j >= nbmigrator) {
      stglogit (func, STG55, pool_p->migr_name);
      errflg++;
      goto reply;
    }
    pool_p->migr = migr_p;
  }

  /* 3rd pass: store pool elements */

  rewind (s);
  pool_p = pools - 1;
  migr_p = migrators - 1;
  while (fgets (buf, sizeof(buf), s) != NULL) {
    if (buf[0] == '#') continue;    /* comment line */
    if ((p = strtok (buf, " \t\n")) == NULL) continue;
    if (strcmp (p, "POOL") == 0) {
      pool_p++;
      elemp = pool_p->elemp;
      if (pool_p->defsize == 0) pool_p->defsize = defsize;
      if (pool_p->defsize <= 0) {
        stglogit(func,
                 "### CONFIGURATION ERROR : POOL %s with DEFSIZE <= 0\n",
                 pool_p->name);
        errflg++;
        goto reply;
      }
      stglogit (func,"POOL %s DEFSIZE %d GC_STOP_THRESH %d GC_START_THRESH %d\n",
				pool_p->name, pool_p->defsize, pool_p->gc_stop_thresh, pool_p->gc_start_thresh);
      stglogit (func,".... GC %s\n",
				*(pool_p->gc) != '\0' ? pool_p->gc : "<none>");
      stglogit (func,".... NO_FILE_CREATION %d\n",
				pool_p->no_file_creation);
      if (pool_p->migr_name[0] != '\0') {
        stglogit (func,".... MIGRATOR %s\n", pool_p->migr_name);
        stglogit (func,".... MIG_STOP_THRESH %d MIG_START_THRESH %d\n",
                  pool_p->mig_stop_thresh, pool_p->mig_start_thresh);
      }
    } else if (strcmp (p, "DEFPOOL") == 0) continue;
    else if (strcmp (p, "DEFPOOL_IN") == 0) continue;
    else if (strcmp (p, "DEFPOOL_OUT") == 0) continue;
    else if (strcmp (p, "DEFSIZE") == 0) continue;
    else {
      if ((int) strlen (p) > CA_MAXHOSTNAMELEN) {
        stglogit (func, STG26, "pool element [host too long]", p);
        errflg++;
        goto reply;
      }
      strcpy (elemp->server, p);
      if ((p = strtok (NULL, " \t\n")) == NULL) {
        stglogit (func, STG26, "pool element [no path]", elemp->server);
        errflg++;
        goto reply;
      }
      if ((int) strlen (p) >= MAXPATH) {
        stglogit (func, STG26, "pool element [path too long]", p);
        errflg++;
        goto reply;
      }
      strcpy (elemp->dirpath, p);
      nfsroot = getconfent ("RFIO", "NFS_ROOT", 0);
#ifdef NFSROOT
      if (nfsroot == NULL) nfsroot = NFSROOT;
#endif
      if (nfsroot != NULL &&
          strncmp (elemp->dirpath, nfsroot, strlen (nfsroot)) == 0 &&
          *(elemp->dirpath + strlen(nfsroot)) == '/')	/* /shift syntax */
        strcpy (path, elemp->dirpath);
      else
        sprintf (path, "%s:%s", elemp->server, elemp->dirpath);
      if (rfio_statfs (path, &st) < 0) {
        stglogit (func, STG02, path, "rfio_statfs",
                  rfio_serror());
      } else {
        char tmpbuf1[21];
        char tmpbuf2[21];
        elemp->capacity = (u_signed64) ((u_signed64) st.totblks * (u_signed64) st.bsize);
        elemp->free = (u_signed64) ((u_signed64) st.freeblks * (u_signed64) st.bsize);
        pool_p->capacity += elemp->capacity;
        pool_p->free += elemp->free;
        stglogit (func,
                  "... %s capacity=%s, free=%s\n",
                  path, u64tostru(elemp->capacity, tmpbuf1, 0), u64tostru(elemp->free, tmpbuf2, 0));
      }
      elemp++;
    }
  }
 reply:
  fclose (s);
  if (errflg) {
    for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
      if (pool_p->elemp != NULL) free (pool_p->elemp);
    }
    free (pools);
    if (migrators) free (migrators);
    return (CONFERR);
  } else {
    /* This pointer will maintain a list of pools on which a gc have finished since last call */
    poolc = (char **) calloc (nbpool, sizeof(char *));
    return (0);
  }
}

int checklastaccess(poolname, atime)
     char *poolname;
     time_t atime;
{
  int i;
  struct pool *pool_p;

  /*	return -1 if the file has been accessed after garbage collector startup,
   *		  the file should not be removed
   *	return 0  otherwise
   */
  if (*poolname == '\0')
    return (0);
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  if (i == nbpool) return (0);    /* old entry; pool does not exist */
  if (pool_p->cleanreqtime == 0) return (0);	/* cleaner not active */
  if (atime < pool_p->cleanreqtime) return (0);
  return (-1);
}

int checkpoolcleaned(pool_list)
     char ***pool_list;
{
  int i, n;
  struct pool *pool_p;

  n = 0;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (pool_p->cleanstatus == 0) continue;
    poolc[n++] = pool_p->name;
    pool_p->cleanstatus = 0;
  }
  if (n)
    *pool_list = poolc;
  return (n);	/* number of pools cleaned since last call */
}

void checkpoolspace()
{
  int i;
  struct pool *pool_p;
  extern int shutdownreq_reqid;

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if ((pool_p->free * 100) < (pool_p->capacity * pool_p->gc_start_thresh)) {
      /* Not enough minimum space */
      /* Note that cleanpool() not launch a cleaner if another is already running */
      if (shutdownreq_reqid == 0) cleanpool(pool_p->name);
    }
  }
}

int cleanpool(poolname)
     char *poolname;
{
  char func[16];
  int i;
  int pid;
  struct pool *pool_p;
  char progfullpath[MAXPATH];
  char hostname[CA_MAXHOSTNAMELEN + 1];

  strcpy (func, "cleanpool");
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  if (pool_p->ovl_pid != 0) return (0);   /* Another cleaner on same pool is already running */
  pool_p->ovl_pid = fork ();
  pid = pool_p->ovl_pid;
  if (pid < 0) {
    stglogit (func, STG02, "", "fork", sys_errlist[errno]);
    return (SYERR);
  } else if (pid == 0) {  /* we are in the child */
    gethostname (hostname, CA_MAXHOSTNAMELEN + 1);
    sprintf (progfullpath, "%s/cleaner", BIN);
    stglogit (func, "execing cleaner, pid=%d\n", getpid());
    execl (progfullpath, "cleaner", pool_p->gc, poolname, hostname, 0);
    stglogit (func, STG02, "cleaner", "execl", sys_errlist[errno]);
    exit (SYERR);
  } else
    pool_p->cleanreqtime = time(NULL);
  return (0);
}

int enoughfreespace(poolname, minf)
     char *poolname;
     int minf;
{
  int i;
  int gc_stop_thresh;
  struct pool *pool_p;

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  if (i == nbpool) return (0);	/* old entry; pool does not exist */
  if (minf)
    gc_stop_thresh = minf;
  else
    gc_stop_thresh = pool_p->gc_stop_thresh;
  return((pool_p->free * 100) > (pool_p->capacity * gc_stop_thresh));
}

char *
findpoolname(path)
     char *path;
{
  struct pool_element *elemp;
  int i, j;
  char *p;
  struct pool *pool_p;
  char server[CA_MAXHOSTNAMELEN + 1];

  if ((p = strchr (path, ':')) != NULL) {
    strncpy (server, path, p - path);
    server[p - path] = '\0';
  }
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    for (j = 0, elemp = pool_p->elemp; j < pool_p->nbelem; j++, elemp++)
      if (p) {
        if (strcmp (server, elemp->server) == 0 &&
            strncmp (p + 1, elemp->dirpath, strlen (elemp->dirpath)) == 0 &&
            *(p + 1 + strlen (elemp->dirpath)) == '/')
          return (pool_p->name);
      } else {
        if (strncmp (path, elemp->dirpath, strlen (elemp->dirpath)) == 0 &&
            *(path + strlen (elemp->dirpath)) == '/')
          return (pool_p->name);
      }
  return (NULL);
}

int iscleanovl(pid, status)
     int pid;
     int status;
{
  int found;
  int i;
  struct pool *pool_p;

  if (nbpool == 0) return (0);
  found = 0;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (pool_p->ovl_pid == pid) {
      found = 1;
      break;
    }
  if (! found) return (0);
  pool_p->ovl_pid = 0;
  pool_p->cleanreqtime = 0;
  if (status == 0)
    pool_p->cleanstatus = 1;
  return (1);
}

void killcleanovl(sig)
     int sig;
{
  int i;
  struct pool *pool_p;
  char *func = "killcleanovl";

  if (nbpool == 0) return;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (pool_p->ovl_pid != 0) {
      stglogit (func, "killing process %d\n", pool_p->ovl_pid);
      kill (pool_p->ovl_pid, sig);
    }
}

int ismigovl(pid, status)
     int pid;
     int status;
{
  int found;
  int i;
  struct pool *pool_p;
  struct stgcat_entry *stcp;
  char *func = "ismigovl";
  if (nbpool == 0) return (0);
  found = 0;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (pool_p->migr == NULL) continue;
    if (pool_p->migr->mig_pid == pid) {
      found = 1;
      break;
    }
  }
  if (! found) return (0);
  pool_p->migr->mig_pid = 0;
  pool_p->migr->migreqtime = 0;
  pool_p->migr->migreqtime_last_end = time(NULL);
  /* We check if there are remaining entries in WAITING_MIG status in this pool */
  /* If so, this is the migrator that failed. */
  for (stcp = stcs; stcp < stce; stcp++) {
    if (stcp->reqid == 0) break;
    if (strcmp(stcp->poolname, pool_p->name) != 0) continue;
    if ((stcp->status & WAITING_MIGR) == WAITING_MIGR) {
      /* This entry had a problem */
      stglogit(func, "STG02 - %s still in WAITING_MIGR, changed to PUT_FAILED\n", stcp->u1.h.xfile);
      stcp->status &= ~WAITING_MIGR;
      /* The following will force update_migpool() to correctly update the being_migr counters */
      stcp->status |= BEING_MIGR;
      update_migpool(&stcp,-1,0);
      stcp->status |= PUT_FAILED;
#ifdef USECDB
      if (stgdb_upd_stgcat(&dbfd,stcp) != 0) {
        stglogit (func, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
      }
#endif
      savereqs ();
    }
  }
  return (1);
}

void killmigovl(sig)
     int sig;
{
  int i;
  struct pool *pool_p;
  char *func = "killmigovl";

  if (nbpool == 0) return;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (pool_p->migr == NULL) continue;
    if (pool_p->migr->mig_pid != 0) {
      stglogit (func, "killing process %d\n", pool_p->migr->mig_pid);
      kill (pool_p->migr->mig_pid, sig);
    }
  }
}

int isvalidpool(poolname)
     char *poolname;
{
  int i;
  struct pool *pool_p;

  if (nbpool == 0) return (0);
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  return (i == nbpool ? 0 : 1);
}

int poolalloc(pool_p, nbpool_ent)
     struct pool *pool_p;
     int nbpool_ent;
{
  char func[16];

  strcpy (func, "poolalloc");
  switch (nbpool_ent) {
  case -1:
    break;
  case 0:		/* pool is empty */
    stglogit (func, STG24, pool_p->name);
    return (-1);
  default:
    pool_p->elemp = (struct pool_element *)
      calloc (nbpool_ent, sizeof(struct pool_element));
    pool_p->nbelem = nbpool_ent;
  }
  return (0);
}

void print_pool_utilization(rpfd, poolname, defpoolname, defpoolname_in, defpoolname_out, migrator_flag, class_flag, queue_flag, counters_flag)
     int rpfd;
     char *poolname, *defpoolname, *defpoolname_in, *defpoolname_out;
     int migrator_flag;
     int class_flag;
     int queue_flag;
{
  struct pool_element *elemp;
  int i, j;
  int migr_newline = 1;
  struct pool *pool_p;
  struct migrator *migr_p;
  struct migrator *pool_p_migr = NULL;
  char tmpbuf[21];
  char timestr[64] ;   /* Time in its ASCII format             */
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
  struct tm tmstruc;
#endif /* _REENTRANT || _THREAD_SAFE */
  struct tm *tp;
  char tmpbuf1[21];
  char tmpbuf2[21];
  char tmpbuf3[21];
  char tmpbuf4[21];
  char tmpbuf5[21];
  char tmpbuf6[21];
  u_signed64 before_fraction, after_fraction;
  int is_poolout;

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (*poolname && strcmp (poolname, pool_p->name)) continue;
    if (*poolname) pool_p_migr = pool_p->migr;
    sendrep (rpfd, MSG_OUT, "POOL %s%s DEFSIZE %d GC_START_THRESH %d GC_STOP_THRESH %d GC %s%s%s%s%s%s%s%s%s\n",
             pool_p->name,
             pool_p->no_file_creation ? " NO_FILE_CREATION" : "",
             pool_p->defsize,
             pool_p->gc_start_thresh,
             pool_p->gc_stop_thresh,
             pool_p->gc,
             (pool_p->migr_name[0] != '\0') ? " MIGRATOR " : "",
             (pool_p->migr_name[0] != '\0') ? pool_p->migr_name : "",
             (pool_p->migr_name[0] != '\0') ? " MIG_START_THRESH " : "",
             (pool_p->migr_name[0] != '\0') ? u64tostr(pool_p->mig_start_thresh, tmpbuf1, 0) : "",
             (pool_p->migr_name[0] != '\0') ? " MIG_STOP_THRESH " : "",
             (pool_p->migr_name[0] != '\0') ? u64tostr(pool_p->mig_stop_thresh, tmpbuf2, 0) : "",
             (pool_p->migr_name[0] != '\0') ? " MIG_DATA_THRESH " : "",
             (pool_p->migr_name[0] != '\0') ? u64tostru(pool_p->mig_data_thresh, tmpbuf3, 0) : ""
             );
    if (pool_p->cleanreqtime > 0) {
#if ((defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32))
      localtime_r(&(pool_p->cleanreqtime),&tmstruc);
      tp = &tmstruc;
#else
      tp = localtime(&(pool_p->cleanreqtime));
#endif /* _REENTRANT || _THREAD_SAFE */
      strftime(timestr,64,strftime_format,tp);
      sendrep (rpfd, MSG_OUT, "\tLAST GARBAGE COLLECTION STARTED %s%s\n", timestr, pool_p->ovl_pid > 0 ? " STILL ACTIVE" : "");
    } else {
      sendrep (rpfd, MSG_OUT, "\tLAST GARBAGE COLLECTION STARTED <none>\n");
    }
    before_fraction = pool_p->capacity ? (100 * pool_p->free) / pool_p->capacity : 0;
    after_fraction = pool_p->capacity ?
      (10 * (pool_p->free * 100 - pool_p->capacity * before_fraction)) / pool_p->capacity :
      0;
    sendrep (rpfd, MSG_OUT,"                              CAPACITY %s FREE %s (%s.%s%%)\n",
             u64tostru(pool_p->capacity, tmpbuf1, 0),
             u64tostru(pool_p->free, tmpbuf2, 0),
             u64tostr(before_fraction, tmpbuf3, 0),
             u64tostr(after_fraction, tmpbuf4, 0));
    is_poolout = (ispool_out(pool_p->name) == 0) ? 1 : 0;
    for (j = 0, elemp = pool_p->elemp; j < pool_p->nbelem; j++, elemp++) {
      before_fraction = elemp->capacity ? (100 * elemp->free) / elemp->capacity : 0;
      after_fraction = elemp->capacity ?
        (10 * (elemp->free * 100 - elemp->capacity * before_fraction)) / elemp->capacity :
        0;
      sendrep (rpfd, MSG_OUT, "  %s %s CAPACITY %s FREE %s (%s.%s%%)%s%s%s%s%s%s\n",
               elemp->server,
               elemp->dirpath,
               u64tostru(elemp->capacity, tmpbuf1, 0),
               u64tostru(elemp->free, tmpbuf2, 0),
               u64tostr(before_fraction, tmpbuf3, 0),
               u64tostr(after_fraction, tmpbuf4, 0),
               (counters_flag && is_poolout) ?       " " : "",
               (counters_flag && is_poolout) ?  "nread " : "",
               (counters_flag && is_poolout) ? u64tostr((u_signed64) elemp->nbreadaccess, tmpbuf5, 0) : "",
               (counters_flag && is_poolout) ?       " " : "",
               (counters_flag && is_poolout) ? "nwrite " : "",
               (counters_flag && is_poolout) ? u64tostr((u_signed64) elemp->nbwriteaccess, tmpbuf6, 0) : ""
               );
    }
  }
  if (*poolname == '\0') {
    sendrep (rpfd, MSG_OUT, "DEFPOOL     %s\n", defpoolname);
    sendrep (rpfd, MSG_OUT, "DEFPOOL_IN  %s\n", defpoolname_in);
    sendrep (rpfd, MSG_OUT, "DEFPOOL_OUT %s\n", defpoolname_out);
  }
  if ((migrator_flag != 0) && (migrators != NULL)) {
    for (i = 0, migr_p = migrators; i < nbmigrator; i++, migr_p++) {
      if (*poolname && pool_p_migr != migr_p) continue;
      if (migr_newline) {
        sendrep (rpfd, MSG_OUT, "\n");
        migr_newline = 0;
      }
      sendrep (rpfd, MSG_OUT, "MIGRATOR %s\n",migr_p->name);
      sendrep (rpfd, MSG_OUT, "\tNBFILES_CAN_BE_MIGR    %d\n", migr_p->global_predicates.nbfiles_canbemig);
      sendrep (rpfd, MSG_OUT, "\tSPACE_CAN_BE_MIGR      %s\n", u64tostru(migr_p->global_predicates.space_canbemig, tmpbuf, 0));
      sendrep (rpfd, MSG_OUT, "\tNBFILES_BEING_MIGR     %d\n", migr_p->global_predicates.nbfiles_beingmig);
      sendrep (rpfd, MSG_OUT, "\tSPACE_BEING_MIGR       %s\n", u64tostru(migr_p->global_predicates.space_beingmig, tmpbuf, 0));
      if (migr_p->migreqtime > 0) {
#if ((defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32))
        localtime_r(&(migr_p->migreqtime),&tmstruc);
        tp = &tmstruc;
#else
        tp = localtime(&(migr_p->migreqtime));
#endif /* _REENTRANT || _THREAD_SAFE */
        strftime(timestr,64,strftime_format,tp);
        sendrep (rpfd, MSG_OUT, "\tLAST MIGRATION STARTED %s%s\n", timestr, migr_p->mig_pid > 0 ? " STILL ACTIVE" : "");
      } else {
        sendrep (rpfd, MSG_OUT, "\tLAST MIGRATION STARTED <none>\n");
      }
      for (j = 0; j < migr_p->nfileclass; j++) {
        sendrep (rpfd, MSG_OUT, "\tFILECLASS %s@%s (classid=%d)\n",
                 migr_p->fileclass[j]->Cnsfileclass.name,
                 migr_p->fileclass[j]->server,
                 migr_p->fileclass[j]->Cnsfileclass.classid);
        sendrep (rpfd, MSG_OUT, "\t\tNBFILES_CAN_BE_MIGR    %d\n", migr_p->fileclass_predicates[j].nbfiles_canbemig);
        sendrep (rpfd, MSG_OUT, "\t\tSPACE_CAN_BE_MIGR      %s\n", u64tostru(migr_p->fileclass_predicates[j].space_canbemig, tmpbuf, 0));
        sendrep (rpfd, MSG_OUT, "\t\tNBFILES_BEING_MIGR     %d\n", migr_p->fileclass_predicates[j].nbfiles_beingmig);
        sendrep (rpfd, MSG_OUT, "\t\tSPACE_BEING_MIGR       %s\n", u64tostru(migr_p->fileclass_predicates[j].space_beingmig, tmpbuf, 0));
      }
    }
  }
  if ((class_flag != 0) && (fileclasses != NULL)) {
    sendrep (rpfd, MSG_OUT, "\n");
    for (i = 0; i < nbfileclasses; i++) {
      printfileclass(rpfd, &(fileclasses[i]));
    }
  }
  if (queue_flag != 0) {
	struct waitq *wqp = NULL;
	struct waitf *wfp;
    extern struct waitq *waitqp;
    int iwaitq;

	for (wqp = waitqp, iwaitq = 1; wqp; wqp = wqp->next, iwaitq++) {
      struct stgcat_entry *stcp;
      sendrep (rpfd, MSG_OUT, "\n--------------------\nWaiting Queue No %d\n--------------------\n", iwaitq);
      sendrep (rpfd, MSG_OUT, "pool_user            %s\n", wqp->pool_user);
      sendrep (rpfd, MSG_OUT, "clienthost           %s\n", wqp->clienthost);
      sendrep (rpfd, MSG_OUT, "req_user             %s\n", wqp->req_user);
      sendrep (rpfd, MSG_OUT, "req_uid              %ld\n", (unsigned long) wqp->req_uid);
      sendrep (rpfd, MSG_OUT, "req_gid              %ld\n", (unsigned long) wqp->req_gid);
      sendrep (rpfd, MSG_OUT, "rtcp_user            %s\n", wqp->rtcp_user);
      sendrep (rpfd, MSG_OUT, "rtcp_group           %s\n", wqp->rtcp_group);
      sendrep (rpfd, MSG_OUT, "rtcp_uid             %ld\n", (unsigned long) wqp->rtcp_uid);
      sendrep (rpfd, MSG_OUT, "rtcp_gid             %ld\n", (unsigned long) wqp->rtcp_gid);
      sendrep (rpfd, MSG_OUT, "clientpid            %ld\n", (unsigned long) wqp->clientpid);
      sendrep (rpfd, MSG_OUT, "uniqueid gives       Pid=0x%lx (%d) CthreadId+1=0x%lx (-> Tid=%d)\n",
               (unsigned long) (wqp->uniqueid / 0xFFFFFFFF),
               (int) (wqp->uniqueid / 0xFFFFFFFF),
               (unsigned long) (wqp->uniqueid - (wqp->uniqueid / 0xFFFFFFFF) * 0xFFFFFFFF),
               (int) (wqp->uniqueid - (wqp->uniqueid / 0xFFFFFFFF) * 0xFFFFFFFF) - 1
               );
      sendrep (rpfd, MSG_OUT, "copytape             %d\n", wqp->copytape);
      sendrep (rpfd, MSG_OUT, "Pflag                %d\n", wqp->Pflag);
      sendrep (rpfd, MSG_OUT, "Upluspath            %d\n", wqp->Upluspath);
      sendrep (rpfd, MSG_OUT, "reqid                %d\n", wqp->reqid);
      sendrep (rpfd, MSG_OUT, "key                  %d\n", wqp->key);
      sendrep (rpfd, MSG_OUT, "rpfd                 %d\n", wqp->rpfd);
      sendrep (rpfd, MSG_OUT, "ovl_pid              %d\n", wqp->ovl_pid);
      sendrep (rpfd, MSG_OUT, "nb_subreqs           %d\n", wqp->nb_subreqs);
      sendrep (rpfd, MSG_OUT, "nbdskf               %d\n", wqp->nbdskf);
      sendrep (rpfd, MSG_OUT, "nb_waiting_on_req    %d\n", wqp->nb_waiting_on_req);
      sendrep (rpfd, MSG_OUT, "nb_clnreq            %d\n", wqp->nb_clnreq);
      sendrep (rpfd, MSG_OUT, "waiting_pool         %d\n", wqp->waiting_pool);
      sendrep (rpfd, MSG_OUT, "clnreq_reqid         %d\n", wqp->clnreq_reqid);
      sendrep (rpfd, MSG_OUT, "clnreq_rpfd          %d\n", wqp->clnreq_rpfd);
      sendrep (rpfd, MSG_OUT, "status               0x%lx\n", (unsigned long) wqp->status);
      sendrep (rpfd, MSG_OUT, "nretry               %d\n", wqp->nretry);
      sendrep (rpfd, MSG_OUT, "Aflag                %d\n", wqp->Aflag);
      sendrep (rpfd, MSG_OUT, "concat_off_fseq      %d\n", wqp->concat_off_fseq);
      sendrep (rpfd, MSG_OUT, "api_out              %d\n", wqp->api_out);
      sendrep (rpfd, MSG_OUT, "openmode             %d\n", (int) wqp->openmode);
      sendrep (rpfd, MSG_OUT, "openflags            0x%lx\n", (unsigned long) wqp->openflags);
      sendrep (rpfd, MSG_OUT, "silent               %d\n", wqp->silent);
      sendrep (rpfd, MSG_OUT, "use_subreqid         %d\n", wqp->use_subreqid);
      sendrep (rpfd, MSG_OUT, "save_nbsubreqid      %d\n", wqp->save_nbsubreqid);
      if (wqp->save_subreqid != NULL) {
        for (i = 0, wfp = wqp->wf; i < wqp->save_nbsubreqid; i++, wfp++) {
          sendrep (rpfd, MSG_OUT, "\tsave_subreqid[%d] = %d\n", i, wqp->save_subreqid[i]);
        }
      }
      sendrep (rpfd, MSG_OUT, "last_rwcounterfs_vs_R %d\n", wqp->last_rwcounterfs_vs_R);
      for (i = 0, wfp = wqp->wf; i < wqp->nb_subreqs; i++, wfp++) {
        for (stcp = stcs; stcp < stce; stcp++) {
          if (wfp->subreqid == stcp->reqid) {
            char tmpbuf1[21];
            char tmpbuf2[22];
            switch (stcp->t_or_d) {
            case 't':
              sendrep(rpfd, MSG_OUT,
                      "\tWaiting File No %3d\n"
                      "\t\tVID.FSEQ=%s.%s\n"
                      "\t\tsubreqid=%d\n"
                      "\t\twaiting_on_req=%d\n"
                      "\t\tupath=%s\n"
                      "\t\tsize_to_recall=%s\n"
                      "\t\tnb_segments=%d\n"
                      "\t\tsize_yet_recalled=%s\n",
                      i,
                      stcp->u1.t.vid[0],
                      stcp->u1.t.fseq,
                      wfp->subreqid,
                      wfp->waiting_on_req,
                      wfp->upath,
                      u64tostr(wfp->size_to_recall, tmpbuf1, 0),
                      wfp->nb_segments,
                      u64tostr(wfp->size_yet_recalled, tmpbuf2, 0)
                      );
              break;
            case 'd':
            case 'a':
              sendrep(rpfd, MSG_OUT,
                      "\tWaiting File No %3d\n"
                      "\t\tu1.d.xfile=%s\n"
                      "\t\tsubreqid=%d\n"
                      "\t\twaiting_on_req=%d\n"
                      "\t\tupath=%s\n"
                      "\t\tsize_to_recall=%s\n"
                      "\t\tnb_segments=%d\n"
                      "\t\tsize_yet_recalled=%s\n",
                      i,
                      stcp->u1.d.xfile,
                      wfp->subreqid,
                      wfp->waiting_on_req,
                      wfp->upath,
                      u64tostr(wfp->size_to_recall, tmpbuf1, 0),
                      wfp->nb_segments,
                      u64tostr(wfp->size_yet_recalled, tmpbuf2, 0)
                      );
              break;
            case 'm':
              sendrep(rpfd, MSG_OUT,
                      "\tWaiting File No %3d\n"
                      "\t\tu1.m.xfile=%s\n"
                      "\t\tsubreqid=%d\n"
                      "\t\twaiting_on_req=%d\n"
                      "\t\tupath=%s\n"
                      "\t\tsize_to_recall=%s\n"
                      "\t\tnb_segments=%d\n"
                      "\t\tsize_yet_recalled=%s\n",
                      i,
                      stcp->u1.m.xfile,
                      wfp->subreqid,
                      wfp->waiting_on_req,
                      wfp->upath,
                      u64tostr(wfp->size_to_recall, tmpbuf1, 0),
                      wfp->nb_segments,
                      u64tostr(wfp->size_yet_recalled, tmpbuf2, 0)
                      );
              break;
            case 'h':
              sendrep(rpfd, MSG_OUT,
                      "\tWaiting File No %3d\n"
                      "\t\tu1.h.xfile=%s\n"
                      "\t\tsubreqid=%d\n"
                      "\t\twaiting_on_req=%d\n"
                      "\t\tupath=%s\n"
                      "\t\tsize_to_recall=%s\n"
                      "\t\tnb_segments=%d\n"
                      "\t\tsize_yet_recalled=%s\n",
                      i,
                      stcp->u1.h.xfile,
                      wfp->subreqid,
                      wfp->waiting_on_req,
                      wfp->upath,
                      u64tostr(wfp->size_to_recall, tmpbuf1, 0),
                      wfp->nb_segments,
                      u64tostr(wfp->size_yet_recalled, tmpbuf2, 0)
                      );
              break;
            default:
              sendrep(rpfd, MSG_OUT,
                      "\tWaiting File No %3d\n"
                      "\t\t<unknown_type>\n"
                      "\t\tsubreqid=%d\n"
                      "\t\twaiting_on_req=%d\n"
                      "\t\tupath=%s\n"
                      "\t\tsize_to_recall=%s\n"
                      "\t\tnb_segments=%d\n"
                      "\t\tsize_yet_recalled=%s\n",
                      i,
                      wfp->subreqid,
                      wfp->waiting_on_req,
                      wfp->upath,
                      u64tostr(wfp->size_to_recall, tmpbuf1, 0),
                      wfp->nb_segments,
                      u64tostr(wfp->size_yet_recalled, tmpbuf2, 0)
                      );
              break;
            }
            break;
          }
        }
      }
    }
  }
}

int
selectfs(poolname, size, path, status)
     char *poolname;
     int *size;
     char *path;
     int status;
{
  struct pool_element *elemp;
  int found = 0;
  int i;
  struct pool *pool_p;
  u_signed64 reqsize;
  char tmpbuf0[21];
  char tmpbuf1[21];
  char tmpbuf2[21];
  struct pool_element *this_element;
  struct stgcat_entry stcx;
  struct stgcat_entry *stcp;

  stcx.status = status;
  stcp = &stcx;

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  if (*size == 0) *size = pool_p->defsize;
  reqsize = (u_signed64) ((u_signed64) *size * (u_signed64) ONE_MB);	/* size in bytes */

  i = pool_p->next_pool_elem;
  do {
    elemp = pool_p->elemp + i;
    if (elemp->free >= reqsize) {
      found = 1;
      break;
    } else if (ISSTAGEOUT(stcp) || ISSTAGEALLOC(stcp)) {
      /* We do not dare to do a simple turnaround when allocating space. */
      /* It can happens that the qsort() was ALREADY called just before but */
      /* the best fs selected does not have enough space neverthless */
      /* This sounds a bit redundant but until I find a better solution I do again */
      /* a qsort() here, excluding the fs'es that do not have enough space! */
      if ((this_element = betterfs_vs_pool(poolname,WRITE_MODE,reqsize,&i)) == NULL) {
        /* Oups... Tant-pis, return what will provocate the ENOENT */
        break;
      } else {
        elemp = this_element;
        found = 1;
        break;
      }
    }
    i++;
    if (i >= pool_p->nbelem) i = 0;
  } while (i != pool_p->next_pool_elem);

  /* We can already reply now if we have found NO candidate ! */
  if (!found)
    return (-1);

  pool_p->next_pool_elem = i + 1;
  if (pool_p->next_pool_elem >= pool_p->nbelem) pool_p->next_pool_elem = 0;
  pool_p->free -= reqsize;
  elemp->free -= reqsize;
  if (pool_p->free > pool_p->capacity) {
    /* This is impossible. In theory it can happen only if reqsize > previous pool_p->free and */
    /* if pool_p->capacity < max_u_signed64 */
    stglogit ("selectfs", "### Warning, pool_p->free > pool_p->capacity. pool_p->free set to 0\n");
    pool_p->free = 0;
  }
  if (elemp->free > elemp->capacity) {
    /* This is impossible. In theory it can happen only if reqsize > previous elemp->free and */
    /* if elemp->capacity < max_u_signed64 */
    stglogit ("selectfs", "### Warning, elemp->free > elemp->capacity. elemp->free set to 0\n");
    elemp->free = 0;
  }
  nfsroot = getconfent ("RFIO", "NFS_ROOT", 0);
#ifdef NFSROOT
  if (nfsroot == NULL) nfsroot = NFSROOT;
#endif
  if (nfsroot != NULL &&
      strncmp (elemp->dirpath, nfsroot, strlen (nfsroot)) == 0 &&
      *(elemp->dirpath + strlen(nfsroot)) == '/')	/* /shift syntax */
    strcpy (path, elemp->dirpath);
  else
    sprintf (path, "%s:%s", elemp->server, elemp->dirpath);
  stglogit ("selectfs", "%s reqsize=%s, elemp->free=%s, pool_p->free=%s\n",
            path, u64tostr(reqsize, tmpbuf0, 0), u64tostr(elemp->free, tmpbuf1, 0), u64tostr(pool_p->free, tmpbuf2, 0));
  return (1);
}

void
rwcountersfs(poolname, ipath, status, req_type)
     char *poolname;
     char *ipath;
     int status;
     int req_type;
{
  struct pool_element *elemp;
  int i, j;
  char *p;
  char path[MAXPATH];
  struct pool *pool_p;
  char server[CA_MAXHOSTNAMELEN + 1];
  int read_incr = 0;
  int write_incr = 0;

  if ((poolname == NULL) || (*poolname == '\0') || (ipath == NULL) || (*ipath == '\0')) {
    return;
  }

  /* Not supported on pool other but defpoolname_out */
  if (ispool_out(poolname) != 0) {
    return;
  }

  switch (req_type) {
  case STAGEOUT:
  case STAGEALLOC:
    write_incr = 1;
    break;
  case STAGEWRT:
  case STAGEPUT:
    read_incr = 1;
    break;
  case STAGEUPDC:
    if (((status & 0xF) == STAGEOUT) ||
        ((status & 0xF) == STAGEALLOC)) {
      write_incr = -1;
    } else if (((status & 0xF) == STAGEWRT) ||
               ((status & 0xf) == STAGEPUT)) {
      read_incr = -1;
    } else if (((status & 0xF) == STAGEIN)) {
      /* Not supported for the moment */
      /* write_incr = -1; */
    } else {
      stglogit ("rwcountersfs", "### Warning, called in STAGEUPDC mode for an unsupported stcp->status = 0x%lx\n", (unsigned long) status);
      return;      
    }
    break;
  default:
    /*
    stglogit ("rwcountersfs", "### Warning, called in unsupported mode %d (%s)\n", req_type,
              req_type == STAGEIN ? "STAGEIN" :
              (req_type == STAGEQRY ? "STAGEQRY" :
               (req_type == STAGECLR ? "STAGECLR" :
                (req_type == STAGEKILL ? "STAGEKILL" :
                 (req_type == STAGEINIT ? "STAGEINIT" :
                  (req_type == STAGECAT ? "STAGECAT" :
                   (req_type == STAGEGET ? "STAGEGET" :
                    (req_type == STAGEMIGPOOL ? "STAGEMIGPOOL" :
                     (req_type == STAGEFILCHG ? "STAGEFILCHG" :
                      (req_type == STAGESHUTDOWN ? "STAGESHUTDOWN" : "<unknown>"
                       )
                      )
                     )
                    )
                   )
                  )
                 )
                )
               )
              );
    */
    return;      
  }
  if ((p = strchr (ipath, ':')) != NULL) {
    strncpy (server, ipath, p - ipath);
    *(server + (p - ipath)) = '\0';
  }
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  if (i == nbpool) return;	/* old entry; pool does not exist */
  for (j = 0, elemp = pool_p->elemp; j < pool_p->nbelem; j++, elemp++)
    if (p) {
      if (strcmp (server, elemp->server) ||
          strncmp (p + 1, elemp->dirpath, strlen (elemp->dirpath)) ||
          *(p + 1 + strlen (elemp->dirpath)) != '/') continue;
      sprintf (path, "%s:%s", elemp->server, elemp->dirpath);
      break;
    } else {
      if (strncmp (ipath, elemp->dirpath, strlen (elemp->dirpath)) ||
          *(ipath + strlen (elemp->dirpath)) != '/') continue;
      strcpy (path, elemp->dirpath);
      break;
    }
  if (j < pool_p->nbelem) {
    if ((write_incr < 0) && (elemp->nbwriteaccess <= 0)) {
      stglogit ("rwcountersfs", "### %s Warning, write_incr=-1 on nbwriteaccess <= 0. All reseted to 0.\n", path);
      write_incr = 0;
      elemp->nbwriteaccess = 0;
    }
    if ((read_incr < 0) && (elemp->nbreadaccess <= 0)) {
      stglogit ("rwcountersfs", "### %s Warning, read_incr=-1 on nbreadaccess <= 0. All reseted to 0.\n", path);
      read_incr = 0;
      elemp->nbreadaccess = 0;
    }
    elemp->nbreadaccess += read_incr;
    elemp->nbwriteaccess += write_incr;
    stglogit ("rwcountersfs", "%s read[%s%d]/write[%s%d]=%2d/%2d\n",
              path,
              read_incr >= 0 ? "+" : "-",
              read_incr >= 0 ? read_incr : -read_incr,
              write_incr >= 0 ? "+" : "-",
              write_incr >= 0 ? write_incr : -write_incr,
              elemp->nbreadaccess,
              elemp->nbwriteaccess);
  }
}

void
getdefsize(poolname, size)
     char *poolname;
     int *size;
{
  int i;
  struct pool *pool_p;

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  *size = pool_p->defsize;
}
int
updfreespace(poolname, ipath, incr)
     char *poolname;
     char *ipath;
     signed64 incr;
{
  struct pool_element *elemp;
  int i, j;
  char *p;
  char path[MAXPATH];
  struct pool *pool_p;
  char server[CA_MAXHOSTNAMELEN + 1];
  char tmpbuf1[21];
  char tmpbuf2[21];
  char tmpbuf3[21];

  if (*poolname == '\0')
    return (0);
  if ((p = strchr (ipath, ':')) != NULL) {
    strncpy (server, ipath, p - ipath);
    *(server + (p - ipath)) = '\0';
  }
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) break;
  if (i == nbpool) return (0);	/* old entry; pool does not exist */
  for (j = 0, elemp = pool_p->elemp; j < pool_p->nbelem; j++, elemp++)
    if (p) {
      if (strcmp (server, elemp->server) ||
          strncmp (p + 1, elemp->dirpath, strlen (elemp->dirpath)) ||
          *(p + 1 + strlen (elemp->dirpath)) != '/') continue;
      sprintf (path, "%s:%s", elemp->server, elemp->dirpath);
      break;
    } else {
      if (strncmp (ipath, elemp->dirpath, strlen (elemp->dirpath)) ||
          *(ipath + strlen (elemp->dirpath)) != '/') continue;
      strcpy (path, elemp->dirpath);
      break;
    }
  if (j < pool_p->nbelem) {
    elemp->free += incr;
    pool_p->free += incr;
    if (pool_p->free > pool_p->capacity) {
      if (incr >= 0) {
        stglogit ("selectfs", "### Warning, pool_p->free > pool_p->capacity. pool_p->free set to pool_p->capacity\n");
        pool_p->free = pool_p->capacity;
      } else {
        stglogit ("selectfs", "### Warning, pool_p->free > pool_p->capacity. pool_p->free set to 0\n");
        pool_p->free = 0;
      }
    }
    if (elemp->free > elemp->capacity) {
      if (incr >= 0) {
        stglogit ("selectfs", "### Warning, elemp->free > elemp->capacity. elemp->free set to elemp->capacity\n");
        elemp->free = elemp->capacity;
      } else {
        stglogit ("selectfs", "### Warning, elemp->free > elemp->capacity. elemp->free set to 0\n");
        elemp->free = 0;
      }
    }
    stglogit ("updfreespace", "%s incr=%s%s, elemp->free=%s, pool_p->free=%s\n",
              path, (incr < 0 ? "-" : ""),
              u64tostr((u_signed64) (incr < 0 ? -incr : incr), tmpbuf1, 0),
              u64tostr(elemp->free, tmpbuf2, 0),
              u64tostr(pool_p->free, tmpbuf3, 0));
  }
  return (0);
}

int updpoolconf(defpoolname,defpoolname_in,defpoolname_out)
     char *defpoolname;
     char *defpoolname_in;
     char *defpoolname_out;
{
  int c, i, j;
  struct pool *pool_n, *pool_p;
  char sav_defpoolname[CA_MAXPOOLNAMELEN + 1];
  char sav_defpoolname_in[CA_MAXPOOLNAMELEN + 1];
  char sav_defpoolname_out[10 * (CA_MAXPOOLNAMELEN + 1)];
  struct migrator *sav_migrator;
  int sav_nbmigrator;
  int sav_nbpool;
  char **sav_poolc;
  struct pool *sav_pools;
  extern int migr_init;
  struct stgcat_entry *stcp;
  struct waitq *wqp = NULL;
  struct waitf *wfp;
  extern struct waitq *waitqp;

  /* save the current configuration */
  strcpy (sav_defpoolname, defpoolname);
  strcpy (sav_defpoolname_in, defpoolname_in);
  strcpy (sav_defpoolname_out, defpoolname_out);
  sav_migrator = migrators;
  sav_nbmigrator = nbmigrator;
  sav_nbpool = nbpool;
  sav_poolc = poolc;
  sav_pools = pools;

  if (migr_init == 0) {
    /* We check if we have to force migr_init value or not */
    /* If there is any MIGR counter different than zero, so we do */
    for (j = 0, pool_n = pools; j < nbpool; j++, pool_n++) {
      if (pool_n->migr != NULL) {
        int k;

        if ((pool_n->migr->global_predicates.nbfiles_canbemig != 0) ||
            (pool_n->migr->global_predicates.space_canbemig   != 0) ||
            (pool_n->migr->global_predicates.nbfiles_beingmig != 0) ||
            (pool_n->migr->global_predicates.space_beingmig   != 0)) {
          migr_init = 1;
          break;
        }
        for (k = 0; k < pool_n->migr->nfileclass; k++) {
          if ((pool_n->migr->fileclass_predicates[k].nbfiles_canbemig != 0) ||
              (pool_n->migr->fileclass_predicates[k].space_canbemig   != 0) ||
              (pool_n->migr->fileclass_predicates[k].nbfiles_beingmig != 0) ||
              (pool_n->migr->fileclass_predicates[k].space_beingmig   != 0)) {
            migr_init = 1;
            break;
          }
        }
        if (migr_init != 0) break;
      }
    }
  }

  if ((c = getpoolconf (defpoolname,defpoolname_in,defpoolname_out))) {	/* new configuration is incorrect */
    /* restore the previous configuration */
    strcpy (defpoolname, sav_defpoolname);
    strcpy (defpoolname_in, sav_defpoolname_in);
    strcpy (defpoolname_out, sav_defpoolname_out);
    migrators = sav_migrator;
    nbmigrator = sav_nbmigrator;
    nbpool = sav_nbpool;
    pools = sav_pools;
  } else {			/* free the old configuration */
    /* but keep pids of cleaner/migrator as well as started time if any */
    free (sav_poolc);
    for (i = 0, pool_p = sav_pools; i < sav_nbpool; i++, pool_p++) {
      if ((pool_p->ovl_pid != 0) || (pool_p->migr != NULL && pool_p->migr->mig_pid)) {
        for (j = 0, pool_n = pools; j < nbpool; j++, pool_n++) {
          if (strcmp (pool_n->name, pool_p->name) == 0) {
            pool_n->ovl_pid = pool_p->ovl_pid;
            pool_n->cleanreqtime = pool_p->cleanreqtime;
            if (pool_n->migr != NULL && pool_p->migr != NULL) {
              pool_n->migr->mig_pid = pool_p->migr->mig_pid;
              pool_n->migr->migreqtime = pool_p->migr->migreqtime;
              pool_n->migr->migreqtime_last_end = pool_p->migr->migreqtime_last_end;
            }
            break;
          }
        }
      }
      free (pool_p->elemp);
    }
    free (sav_pools);
    if (sav_migrator) free (sav_migrator);
    /* And restore rw counters */
    for (stcp = stcs; stcp < stce; stcp++) {
      if (stcp->reqid == 0) break;
      if (ISSTAGEOUT(stcp) || ISSTAGEOUT(stcp)) {
        rwcountersfs(stcp->poolname, stcp->ipath, stcp->status, stcp->status);
      }
    }
	for (wqp = waitqp; wqp; wqp = wqp->next) {
      if (wqp->last_rwcounterfs_vs_R == LAST_RWCOUNTERSFS_TPPOS) {
        wfp = wqp->wf;
        for (stcp = stcs; stcp < stce; stcp++) {
          if (stcp->reqid == 0) break;
          if (stcp->reqid == wfp->subreqid) {
            if (ISSTAGEWRT(stcp) || ISSTAGEPUT(stcp)) {
              rwcountersfs(stcp->poolname, stcp->ipath, STAGEWRT, STAGEWRT); /* Could be STAGEPUT */
            }
            break;
          }
        }
      }
    }
  }
  if (migr_init != 0) {
    /* Update the fileclasses */
    upd_fileclasses();
    /* Update the migrators */
    for (j = 0, pool_n = pools; j < nbpool; j++, pool_n++) {
      if (pool_n->migr != NULL) {
        int k;

        pool_n->migr->global_predicates.nbfiles_canbemig = 0;
        pool_n->migr->global_predicates.space_canbemig = 0;
        pool_n->migr->global_predicates.nbfiles_beingmig = 0;
        pool_n->migr->global_predicates.space_beingmig = 0;
        for (k = 0; k < pool_n->migr->nfileclass; k++) {
          pool_n->migr->fileclass_predicates[k].nbfiles_canbemig = 0;
          pool_n->migr->fileclass_predicates[k].space_canbemig = 0;
          pool_n->migr->fileclass_predicates[k].nbfiles_beingmig = 0;
          pool_n->migr->fileclass_predicates[k].space_beingmig = 0;
        }
      }
    }
    redomigpool();
  }
  return (c);
}

void redomigpool()
{
  struct stgcat_entry *stcp;

  for (stcp = stcs; stcp < stce; stcp++) {
    if (stcp->reqid == 0) break;
    if ((stcp->status & CAN_BE_MIGR) != CAN_BE_MIGR) continue;
    if ((stcp->status & PUT_FAILED) == PUT_FAILED) continue;
    insert_in_migpool(stcp);
  }
}

int get_create_file_option(poolname)
     char *poolname;
{
  int i;
  struct pool *pool_p;

  if (nbpool == 0) return (0);
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) 
      return(pool_p->no_file_creation);
  return (0);
}

/* flag means :                                           */
/*  1             file put in stack can_be_migr           */
/* -1             file removed from stack can_be_migr     */
/* immediate parameter: used only if flag == 1;           */
/* If 0           set only canbemigr stack                */
/* If 1           set both canbemigr and beingmigr stacks */
/* If 2           set only being migr stack               */
/* Returns: 0 (OK) or -1 (NOT OK)                         */
int update_migpool(stcp,flag,immediate)
     struct stgcat_entry **stcp;
     int flag;
     int immediate;
{
  int i, ipool;
  struct pool *pool_p;
  int ifileclass;
  struct stgcat_entry savestcp = **stcp;
  char *func = "update_migpool";
  int savereqid = reqid;
  int rc = 0;
  int ngen = 0;

  if (((flag != 1) && (flag != -1)) || ((immediate != 0) && (immediate != 1) && (immediate != 2))) {
    stglogit (func, STG105, func,  "flag should be 1 or -1, and immediate should be 1, 2 or 3");
    serrno = EINVAL;
    rc = -1;
    goto update_migpool_return;
  }

  /* We check that this poolname exist */
  ipool = -1;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (strcmp(pool_p->name,(*stcp)->poolname) == 0) {
      ipool = i;
      break;
    }
  }
  if (ipool < 0) {
    stglogit (func, STG32, (*stcp)->poolname);
    serrno = EINVAL;
    rc = -1;
    goto update_migpool_return;
  }
  /* We check that this pool have a migrator */
  if (pool_p->migr == NULL) {
    return(0);
  }
  /* We check that this stcp have a known fileclass v.s. its pool migrator */
  if ((ifileclass = upd_fileclass(pool_p,*stcp)) < 0) {
    rc = -1;
    goto update_migpool_return;
  }

  /* If this fileclass specifies nbcopies == 0 or nbtppools == 0 (in practice, both) */
  /* [it is protected to restrict to this - see routine upd_fileclass() in this source] */
  /* this stcp cannot be a candidate for migration - we put it is STAGED status */
  /* immediately */
  if ((pool_p->migr->fileclass[ifileclass]->Cnsfileclass.nbcopies == 0) || 
      (pool_p->migr->fileclass[ifileclass]->Cnsfileclass.nbtppools == 0)) {
    sendrep(rpfd, MSG_ERR, STG139,
            (*stcp)->u1.h.xfile, 
            pool_p->migr->fileclass[ifileclass]->Cnsfileclass.name,
            pool_p->migr->fileclass[ifileclass]->server,
            pool_p->migr->fileclass[ifileclass]->Cnsfileclass.classid,
            pool_p->migr->fileclass[ifileclass]->Cnsfileclass.nbcopies,
            pool_p->migr->fileclass[ifileclass]->Cnsfileclass.nbtppools);
    /* We mark it as staged */
    (*stcp)->status |= STAGED;
    if (((*stcp)->status & CAN_BE_MIGR) == CAN_BE_MIGR) {
      /* And not as a migration candidate */
      (*stcp)->status &= ~CAN_BE_MIGR;
    }
#ifdef USECDB
    if (stgdb_upd_stgcat(&dbfd,*stcp) != 0) {
      stglogit(func, STG100, "insert", sstrerror(serrno), __FILE__, __LINE__);
    }
#endif
    savereqs();
    /* But we still returns ok... */
    return(0);
  }

  switch (flag) {
  case -1:
    if (((*stcp)->status & CAN_BE_MIGR) != CAN_BE_MIGR) {
      /* This is a not a return from automatic migration */
      return(0);
    }
    /* This is a return from automatic migration */
    /* We update global migrator variables */
    if (--pool_p->migr->global_predicates.nbfiles_canbemig < 0) {
      stglogit (func, STG106, func,
              (*stcp)->poolname,
              "nbfiles_canbemig < 0 after automatic migration OK (resetted to 0)");
      pool_p->migr->global_predicates.nbfiles_canbemig = 0;
    }
    if (pool_p->migr->global_predicates.space_canbemig < (*stcp)->actual_size) {
      stglogit (func, STG106,
              func,
              (*stcp)->poolname,
              "space_canbemig < (*stcp)->actual_size after automatic migration OK (resetted to 0)");
      pool_p->migr->global_predicates.space_canbemig = 0;
    } else {
      pool_p->migr->global_predicates.space_canbemig -= (*stcp)->actual_size;
    }
    if (((*stcp)->status == (STAGEPUT|CAN_BE_MIGR)) || (((*stcp)->status & BEING_MIGR) == BEING_MIGR)) {
      if (--pool_p->migr->global_predicates.nbfiles_beingmig < 0) {
        stglogit (func, STG106,
                func,
                (*stcp)->poolname,
                "nbfiles_beingmig < 0 after automatic migration OK (resetted to 0)");
        pool_p->migr->global_predicates.nbfiles_beingmig = 0;
      }
      if (pool_p->migr->global_predicates.space_beingmig < (*stcp)->actual_size) {
        stglogit (func, STG106,
                func,
                (*stcp)->poolname,
                "space_beingmig < (*stcp)->actual_size after automatic migration OK (resetted to 0)");
        pool_p->migr->global_predicates.space_beingmig = 0;
      } else {
        pool_p->migr->global_predicates.space_beingmig -= (*stcp)->actual_size;
      }
    }
    /* We update fileclass_vs_migrator variables */
    if (--pool_p->migr->fileclass_predicates[ifileclass].nbfiles_canbemig < 0) {
      stglogit (func, STG110,
              func,
              (*stcp)->poolname,
              pool_p->migr->fileclass[ifileclass]->Cnsfileclass.name,
              pool_p->migr->fileclass[ifileclass]->server,
              "nbfiles_canbemig < 0 after automatic migration OK (resetted to 0)");
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_canbemig = 0;
    }
    if (pool_p->migr->fileclass_predicates[ifileclass].space_canbemig < (*stcp)->actual_size) {
      stglogit (func, STG110,
              func,
              (*stcp)->poolname,
              pool_p->migr->fileclass[ifileclass]->Cnsfileclass.name,
              pool_p->migr->fileclass[ifileclass]->server,
              "space_canbemig < (*stcp)->actual_size after automatic migration OK (resetted to 0)");
      pool_p->migr->fileclass_predicates[ifileclass].space_canbemig = 0;
    } else {
      pool_p->migr->fileclass_predicates[ifileclass].space_canbemig -= (*stcp)->actual_size;
    }
    if (((*stcp)->status == (STAGEPUT|CAN_BE_MIGR)) || (((*stcp)->status & BEING_MIGR) == BEING_MIGR)) {
      if (--pool_p->migr->fileclass_predicates[ifileclass].nbfiles_beingmig < 0) {
        stglogit (func, STG110,
                func,
                (*stcp)->poolname,
                pool_p->migr->fileclass[ifileclass]->Cnsfileclass.name,
                pool_p->migr->fileclass[ifileclass]->server,
                "nbfiles_beingmig < 0 after automatic migration OK (resetted to 0)");
        pool_p->migr->fileclass_predicates[ifileclass].nbfiles_beingmig = 0;
      }
      if (pool_p->migr->fileclass_predicates[ifileclass].space_beingmig < (*stcp)->actual_size) {
        stglogit (func, STG110,
                func,
                (*stcp)->poolname,
                pool_p->migr->fileclass[ifileclass]->Cnsfileclass.name,
                pool_p->migr->fileclass[ifileclass]->server,
                "space_beingmig < (*stcp)->actual_size after automatic migration OK (resetted to 0)");
        pool_p->migr->fileclass_predicates[ifileclass].space_beingmig = 0;
      } else {
        pool_p->migr->fileclass_predicates[ifileclass].space_beingmig -= (*stcp)->actual_size;
      }
    }
    if (((*stcp)->status & BEING_MIGR) == BEING_MIGR) {
      (*stcp)->status &= ~BEING_MIGR;
#ifdef USECDB
      if (stgdb_upd_stgcat(&dbfd,*stcp) != 0) {
        stglogit(func, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
      }
#endif
      savereqs();
    }
    break;
  case 1:
    /* This is to add an entry for the next automatic migration */

    if ((*stcp)->actual_size <= 0) {
      sendrep(rpfd, MSG_ERR, STG105, "update_migpool", "(*stcp)->actual_size <= 0");
      /* No point */
      serrno = EINVAL;
      rc = -1;
      break;
    }

    /* If this entry has an empty member (*stcp)->u1.h.tppool this means that this */
    /* is a new entry that we will have to migrate in conformity with its fileclass */
    /* Otherwise this means that this is an entry that is a survivor of a previous */
    /* migration that failed - this can happen in particular at a start of the stgdaemon */
    /* when it loads and check the full catalog before starting processing requests... */

    if ((*stcp)->u1.h.tppool[0] == '\0') {
      struct stgcat_entry *stcp_ok;
      int index_found;

      for (i = 0; i < pool_p->migr->fileclass[ifileclass]->Cnsfileclass.nbcopies; i++) {
        /* We decide right now to which tape pool this (*stcp) will go */
        /* We have the list of available tape pools in */
        if (i > 0) {
          /* We need to create a new entry in the catalog for this file */
          /* Because this may generate a realloc we need to restore afterwhile */
          /* the correct original *stcp */
          index_found = (*stcp) - stcs;
          stcp_ok = newreq();
          memcpy(stcp_ok, &savestcp, sizeof(struct stgcat_entry));
          stcp_ok->reqid = nextreqid();
          reqid = savereqid;
          /* We flag this stcp_ok as a candidate for migration */
          stcp_ok->status |= CAN_BE_MIGR;
          strcpy(stcp_ok->u1.h.tppool,next_tppool(pool_p->migr->fileclass[ifileclass]));
          stglogit(func, "STG98 - %s %s (copy No %d) with tape pool %s\n",
                   i > 0 ? "Extended" : "Modified",
                   stcp_ok->u1.h.xfile,
                   i,
                   stcp_ok->u1.h.tppool);
          /* We prepare the 'immediate' switch below because we can do this now and only now */
          if (immediate >= 1) {
            if (stcp_ok->status != (STAGEPUT|CAN_BE_MIGR)) stcp_ok->status |= BEING_MIGR;
          }
#ifdef USECDB
          if (stgdb_ins_stgcat(&dbfd,stcp_ok) != 0) {
            stglogit(func, STG100, "insert", sstrerror(serrno), __FILE__, __LINE__);
          }
#endif
          /* We restore original stcp */
          *stcp = stcs + index_found;
          savereqs();
        } else {
          /* We flag this (*stcp) as a candidate for migration */
          (*stcp)->status |= CAN_BE_MIGR;
          strcpy((*stcp)->u1.h.tppool,next_tppool(pool_p->migr->fileclass[ifileclass]));
          stglogit(func, "STG98 - %s %s (copy No %d) with tape pool %s\n",
                   i > 0 ? "Extended" : "Modified",
                   (*stcp)->u1.h.xfile,
                   i,
                   (*stcp)->u1.h.tppool);
          /* We prepare the 'immediate' switch below because we can do this now and only now */
          if (immediate >= 1) {
            if ((*stcp)->status != (STAGEPUT|CAN_BE_MIGR)) (*stcp)->status |= BEING_MIGR;
          }
#ifdef USECDB
          if (stgdb_upd_stgcat(&dbfd,(*stcp)) != 0) {
            stglogit(func, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
          }
#endif
          savereqs();
        }
      }
      ngen = pool_p->migr->fileclass[ifileclass]->Cnsfileclass.nbcopies;
    } else {
      stglogit(func, STG120,
               (*stcp)->u1.h.xfile,
               (*stcp)->u1.h.tppool,
               pool_p->migr->fileclass[ifileclass]->Cnsfileclass.name,
               pool_p->migr->fileclass[ifileclass]->server,
               pool_p->migr->fileclass[ifileclass]->Cnsfileclass.classid);
      /* We flag this (*stcp) as a candidate for migration */
      (*stcp)->status |= CAN_BE_MIGR;
      /* We prepare the 'immediate' switch below because we can do this now and only now */
      if (immediate >= 1) {
        if ((*stcp)->status != (STAGEPUT|CAN_BE_MIGR)) (*stcp)->status |= BEING_MIGR;
      }
#ifdef USECDB
      if (stgdb_upd_stgcat(&dbfd,(*stcp)) != 0) {
        stglogit(func, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
      }
#endif
      savereqs();
      ngen = 1;
    }
    switch (immediate) {
    case 0:
      /* We udpate global migrator variables */
      pool_p->migr->global_predicates.nbfiles_canbemig += ngen;
      pool_p->migr->global_predicates.space_canbemig += ((*stcp)->actual_size * ngen);
      /* We udpate global fileclass variables */
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_canbemig += ngen;
      pool_p->migr->fileclass_predicates[ifileclass].space_canbemig += ((*stcp)->actual_size * ngen);
      break;
    case 1:
      /* We udpate global migrator variables */
      pool_p->migr->global_predicates.nbfiles_canbemig += ngen;
      pool_p->migr->global_predicates.space_canbemig += ((*stcp)->actual_size * ngen);
      /* We udpate global fileclass variables */
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_canbemig += ngen;
      pool_p->migr->fileclass_predicates[ifileclass].space_canbemig += ((*stcp)->actual_size * ngen);
      /* No break here - intentionnally */
    case 2:
      /* We udpate global migrator variables */
      pool_p->migr->global_predicates.nbfiles_beingmig += ngen;
      pool_p->migr->global_predicates.space_beingmig += ((*stcp)->actual_size * ngen);
      /* We udpate global fileclass variables */
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_beingmig += ngen;
      pool_p->migr->fileclass_predicates[ifileclass].space_beingmig += ((*stcp)->actual_size * ngen);
      break;
    default:
      break;
    }
    break;
  default:
    sendrep(rpfd, MSG_ERR, STG105, "update_migpool", "flag != 1 && flag != -1");
    serrno = EINVAL;
    rc = -1;
    break;
  }
 update_migpool_return:
  if (rc != 0) {
    /* Oups... Error... We add PUT_FAILED to the the eventual CAN_BE_MIGR state */
    if (((*stcp)->status & CAN_BE_MIGR) == CAN_BE_MIGR) {
      (*stcp)->status |= PUT_FAILED;
#ifdef USECDB
      if (stgdb_upd_stgcat(&dbfd,*stcp) != 0) {
        stglogit (func, STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
      }
#endif
      savereqs();
    }
  }
  return(rc);
}

/* Returns: 0 (OK) or -1 (NOT OK) */
int insert_in_migpool(stcp)
     struct stgcat_entry *stcp;
{
  int i, ifileclass, ipool;
  struct pool *pool_p;
  char *func = "insert_in_migpool";

  /* We check that this poolname exist */
  ipool = -1;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    if (strcmp(pool_p->name,stcp->poolname) == 0) {
      ipool = i;
      break;
    }
  }
  if (ipool < 0) {
    stglogit(func, STG32, stcp->poolname); /* Not in the list of pools */
    serrno = EINVAL;
    return(-1);
  }
  /* We check that this pool have a migrator */
  if (pool_p->migr == NULL) {
    stglogit(func, STG33, stcp->ipath, "New configuration makes it orphan of migrator");
    return(0);
  }
  pool_p->migr->global_predicates.nbfiles_canbemig++;
  pool_p->migr->global_predicates.space_canbemig += stcp->actual_size;
  if ((stcp->status == (STAGEPUT|CAN_BE_MIGR)) || ((stcp->status & BEING_MIGR) == BEING_MIGR)) {
    pool_p->migr->global_predicates.nbfiles_beingmig++;
    pool_p->migr->global_predicates.space_beingmig += stcp->actual_size;
  }
  if ((ifileclass = upd_fileclass(pool_p,stcp)) >= 0) {
    pool_p->migr->fileclass_predicates[ifileclass].nbfiles_canbemig++;
    pool_p->migr->fileclass_predicates[ifileclass].space_canbemig += stcp->actual_size;
    if ((stcp->status == (STAGEPUT|CAN_BE_MIGR)) || ((stcp->status & BEING_MIGR) == BEING_MIGR)) {
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_beingmig++;
      pool_p->migr->fileclass_predicates[ifileclass].space_beingmig += stcp->actual_size;
    }
  }
  return(0);
}

void checkfile2mig()
{
  int i, j;
  struct pool *pool_p;

  /* The migration will be started if one of the 4 following criteria is met:
     - space occupied by files in state CAN_BE_MIGR execeeds a given value
     - amount of free space in the disk pool is below a given threshold
     - time elapsed since last migration is bigger than a given value.

     The number of parallel migration streams is set for each pool
  */

  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    int global_or;

    global_or = 0;
    if (pool_p->migr == NULL)	/* migration not wanted in this pool */
      continue;
    if (pool_p->migr->mig_pid != 0)	/* migration already running */
      continue;

    if ((pool_p->migr->global_predicates.nbfiles_canbemig - pool_p->migr->global_predicates.nbfiles_beingmig) <= 0)	/* No point anyway */
      continue;

    /* (Note that because of explicit migration there can be files in BEING_MIGR without going through this routine) */

    /* We check global predicates that are the sum of every fileclasses predicates */
    if (((pool_p->mig_data_thresh > 0) && ((pool_p->migr->global_predicates.space_canbemig - pool_p->migr->global_predicates.space_beingmig) >= pool_p->mig_data_thresh)) ||
        ((pool_p->mig_start_thresh > 0) && ((pool_p->free * 100) <= (pool_p->capacity * pool_p->mig_start_thresh)))) {
      global_or = 1;
    }
    if (global_or != 0) {
      char tmpbuf1[21];
      char tmpbuf2[21];
      char tmpbuf3[21];
      char tmpbuf4[21];

      stglogit("checkfile2mig", "STG98 - Migrator %s@%s - Global Predicate returns true:\n",
               pool_p->migr->name,
               pool_p->name);
      stglogit("checkfile2mig", "STG98 - 1) (%s) (space_canbemig=%s - space_beingmig=%s)=%s > data_mig_threshold=%s ?\n",
               pool_p->mig_data_thresh > 0 ? "ON" : "OFF",
               u64tostru(pool_p->migr->global_predicates.space_canbemig, tmpbuf1, 0),
               u64tostru(pool_p->migr->global_predicates.space_beingmig, tmpbuf2, 0),
               u64tostru(pool_p->migr->global_predicates.space_canbemig - pool_p->migr->global_predicates.space_beingmig, tmpbuf3, 0),
               u64tostru(pool_p->mig_data_thresh, tmpbuf4, 0)
               );
      stglogit("checkfile2mig", "STG98 - 2) (%s) free=%s < (capacity=%s * mig_start_thresh=%d%%)=%s ?\n",
               pool_p->mig_start_thresh > 0 ? "ON" : "OFF",
               u64tostru(pool_p->free, tmpbuf1, 0),
               u64tostru(pool_p->capacity, tmpbuf2, 0),
               pool_p->mig_start_thresh,
               u64tostru((pool_p->capacity * pool_p->mig_start_thresh) /
                         ((u_signed64) 100), tmpbuf3, 0)
               );
    }

    if (global_or == 0) {
      /* Global predicate is not enough - we go through each fileclass predicate */
      
      for (j = 0; j < pool_p->migr->nfileclass; j++) {
        if ((pool_p->migr->fileclass_predicates[j].nbfiles_canbemig - pool_p->migr->fileclass_predicates[j].nbfiles_beingmig) <= 0)	/* No point anyway */
          continue;
        if (((pool_p->mig_data_thresh > 0) && ((pool_p->migr->fileclass_predicates[j].space_canbemig - pool_p->migr->fileclass_predicates[j].space_beingmig) >= pool_p->mig_data_thresh)) ||
            ((pool_p->migr->fileclass[j]->Cnsfileclass.migr_time_interval > 0) && ((time(NULL) - pool_p->migr->migreqtime_last_end) >= pool_p->migr->fileclass[j]->Cnsfileclass.migr_time_interval))) {
          global_or = 1;
        }
        if (global_or != 0) {
          char tmpbuf1[21];
          char tmpbuf2[21];
          char tmpbuf3[21];
          char tmpbuf4[21];

          stglogit("checkfile2mig", "STG98 - Migrator %s@%s - Fileclass %s@%s (classid %d) - %d cop%s - Predicates returns true:\n",
                   pool_p->migr->name,
                   pool_p->name,
                   pool_p->migr->fileclass[j]->Cnsfileclass.name,
                   pool_p->migr->fileclass[j]->server,
                   pool_p->migr->fileclass[j]->Cnsfileclass.classid,
                   pool_p->migr->fileclass[j]->Cnsfileclass.nbcopies,
                   pool_p->migr->fileclass[j]->Cnsfileclass.nbcopies > 1 ? "ies" : "y");
          stglogit("checkfile2mig", "STG98 - 1) (%s) (space_canbemig=%s - space_beingmig=%s)=%s > data_mig_threshold=%s ?\n",
                   pool_p->mig_data_thresh > 0 ? "ON" : "OFF",
                   u64tostru(pool_p->migr->fileclass_predicates[j].space_canbemig, tmpbuf1, 0),
                   u64tostru(pool_p->migr->fileclass_predicates[j].space_beingmig, tmpbuf2, 0),
                   u64tostru(pool_p->migr->fileclass_predicates[j].space_canbemig - pool_p->migr->fileclass_predicates[j].space_beingmig, tmpbuf3, 0),
                   u64tostru(pool_p->mig_data_thresh, tmpbuf4, 0)
                   );
          stglogit("checkfile2mig", "STG98 - 2) (%s) last migrator ended %d seconds ago > %d seconds ?\n",
                   pool_p->migr->fileclass[j]->Cnsfileclass.migr_time_interval > 0 ? "ON" : "OFF",
                   (int) (time(NULL) - pool_p->migr->migreqtime_last_end),
                   pool_p->migr->fileclass[j]->Cnsfileclass.migr_time_interval);
          break;
        }
      }
    }
    if (global_or != 0) {
      migrate_files(pool_p);
      return;
    }
  }
}


int migrate_files(pool_p)
     struct pool *pool_p;
{
  char func[16];
  struct stgcat_entry *stcp;
  int pid, ifileclass;

  strcpy (func, "migrate_files");
  pool_p->migr->mig_pid = fork ();
  pid = pool_p->migr->mig_pid;

  if (pid < 0) {
    stglogit (func, STG02, "", "fork", sys_errlist[errno]);
    pool_p->migr->mig_pid = 0;
    return (SYERR);
  } else if (pid == 0) {  /* we are in the child */
    /* @@@@ EXCEPTIONNAL @@@@ */
    /* exit(2); */
    /* @@@@ END OF EXCEPTIONNAL @@@@ */
    exit(migpoolfiles(pool_p));
  } else {  /* we are in the parent */
    struct pool *pool_n;
    int okpoolname;
    int ipoolname;
    int j;
    char tmpbuf[21];

    /* For logging purpose we reset these variables, used only in this routine */
    pool_p->migr->global_predicates.nbfiles_to_mig = 0;
    pool_p->migr->global_predicates.space_to_mig = 0;
    for (j = 0; j < pool_p->migr->nfileclass; j++) {
      pool_p->migr->fileclass_predicates[j].nbfiles_to_mig = 0;
      pool_p->migr->fileclass_predicates[j].space_to_mig = 0;
    }

    /* We remember all the entries that will be treated in this migration */
    for (stcp = stcs; stcp < stce; stcp++) {
      if (stcp->reqid == 0) break;
      if (! ISCASTORCANBEMIG(stcp)) continue;       /* Not a candidate for migration */
      okpoolname = 0;
      /* Does it belong to a pool managed by this migrator ? */
      for (ipoolname = 0, pool_n = pools; ipoolname < nbpool; ipoolname++, pool_n++) {
        /* We search the pool structure for this stcp */
        if (strcmp(stcp->poolname,pool_n->name) != 0) continue;
        /* And we check if it has the same migrator pointer */
        if (pool_n->migr != pool_p->migr) continue;
        /* We found it : this stcp is a candidate for migration */
        okpoolname = 1;
        break;
      }
      if (okpoolname == 0) continue;    /* This stcp is not a candidate for migration */
      if ((ifileclass = upd_fileclass(pool_p,stcp)) < 0) continue;
      /* We update the fileclass within this migrator watch variables */
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_beingmig++;
      pool_p->migr->fileclass_predicates[ifileclass].nbfiles_to_mig++;
      pool_p->migr->fileclass_predicates[ifileclass].space_beingmig += stcp->actual_size;
      pool_p->migr->fileclass_predicates[ifileclass].space_to_mig += stcp->actual_size;
      /* We flag this stcp a WAITING_MIGR (before being in STAGEOUT|CAN_BE_MIGR|BEING_MIGR) */
      stcp->status |= WAITING_MIGR;
      /* We update the migrator global watch variables */
      pool_p->migr->global_predicates.nbfiles_beingmig++;
      pool_p->migr->global_predicates.nbfiles_to_mig++;
      pool_p->migr->global_predicates.space_beingmig += stcp->actual_size;
      pool_p->migr->global_predicates.space_to_mig += stcp->actual_size;
    }
      
    stglogit (func, "execing migrator %s@%s for %d HSM files (total of %s), pid=%d\n",
              pool_p->migr->name,
              pool_p->name,
              pool_p->migr->global_predicates.nbfiles_to_mig,
              u64tostru(pool_p->migr->global_predicates.space_to_mig, tmpbuf, 0),
              pid);

    for (j = 0; j < pool_p->migr->nfileclass; j++) {
      if (pool_p->migr->fileclass_predicates[j].nbfiles_to_mig <= 0) continue;
      /* We log how many files per fileclass are concerned for this migration */
      stglogit (func, "detail> migrator %s@%s, Fileclass %s@%s (classid %d) : %d HSM files (total of %s)\n",
                pool_p->migr->name,
                pool_p->name,
                pool_p->migr->fileclass[j]->Cnsfileclass.name,
                pool_p->migr->fileclass[j]->server,
                pool_p->migr->fileclass[j]->Cnsfileclass.classid,
                pool_p->migr->fileclass_predicates[j].nbfiles_to_mig,
                u64tostru(pool_p->migr->fileclass_predicates[j].space_to_mig, tmpbuf, 0));
    }
    /* We keep track of last fork time with the associated pid */
    pool_p->migr->migreqtime = time(NULL);
    if ((pool_p->migr->global_predicates.nbfiles_beingmig == 0) ||
        (pool_p->migr->global_predicates.space_beingmig == 0)) {
      int j;
      struct pool *pool_n;

      stglogit (func, "### Executing recovery procedure - migrator should not have been executed\n");
      /* Update the migrators */
      for (j = 0, pool_n = pools; j < nbpool; j++, pool_n++) {
        if (pool_n->migr != NULL) {
          int k;
          pool_n->migr->global_predicates.nbfiles_canbemig = 0;
          pool_n->migr->global_predicates.space_canbemig = 0;
          pool_n->migr->global_predicates.nbfiles_beingmig = 0;
          pool_n->migr->global_predicates.space_beingmig = 0;
          for (k = 0; k < pool_n->migr->nfileclass; k++) {
            pool_n->migr->fileclass_predicates[k].nbfiles_canbemig = 0;
            pool_n->migr->fileclass_predicates[k].space_canbemig = 0;
            pool_n->migr->fileclass_predicates[k].nbfiles_beingmig = 0;
            pool_n->migr->fileclass_predicates[k].space_beingmig = 0;
          }
        }
      }
      redomigpool();
    }
  }
  return (0);
}

int migpoolfiles(pool_p)
     struct pool *pool_p;
{
  /* We use the weight algorithm defined by Fabrizio Cane for DPM */

  int c;
  struct sorted_ent *prev, *scc, *sci, *scf, *scs, *scc_found;
  struct stgcat_entry *stcp;
  struct stgpath_entry *stpp;
  int found_nbfiles = 0;
  char func[16];
  int okpoolname;
  int ipoolname;
  int term_status;
  pid_t child_pid;
  int i, j, k, l, nfiles_per_tppool;
  struct pool *pool_n;
  int found_not_scanned = -1;
  int fork_pid;
  extern struct passwd start_passwd;         /* Generic uid/gid at startup (admin) */
  extern struct passwd stage_passwd;             /* Generic uid/gid stage:st */
  char *minsize_per_stream;
  u_signed64 minsize;
  u_signed64 defminsize = (u_signed64) ((u_signed64) 2 * (u_signed64) ONE_GB);
  struct files_per_stream *tppool_vs_stcp = NULL;
  int ntppool_vs_stcp = 0;
  char tmpbuf[21];
  char remember_tppool[CA_MAXPOOLNAMELEN+1];
  u_signed64 ideal_minsize;
  int nideal_minsize;
  int itppool, found_tppool;
  int npoolname_out;
  int original_nb_of_stream;

  /* We get the minimum size to be transfered */
  minsize = defminsize;
  if ((minsize_per_stream = getconfent ("STG", "MINSIZE_PER_STREAM", 0)) != NULL) {
    minsize = defminsize;
    if ((minsize = strutou64(minsize_per_stream)) <= 0) {
      minsize = defminsize;
    }
  }


  strcpy (func, "migpoolfiles");
  /* We reset the reqid (we are in a child) */
  reqid = 0;

  stglogit(func, STG33, "minsize per stream current value", u64tostr((u_signed64) minsize, tmpbuf, 0));

#ifdef STAGER_DEBUG
  stglogit(func, "### Please gdb /usr/local/bin/stgdaemon %d, then break %d\n",getpid(),__LINE__+2);
  sleep(9);
  sleep(1);
#endif

  if (stage_setlog((void (*) _PROTO((int, char *))) &migpoolfiles_log_callback) != 0) {
    stglogit(func, "### stage_setlog error (%s)\n",sstrerror(serrno));
    return(SYERR);
  }

#ifndef __INSURE__
  /* We do not want any connection but the one to the stgdaemon */
  for (c = 0; c < maxfds; c++)
    close (c);
#endif

#ifndef _WIN32
  signal(SIGCHLD, poolmgr_wait4child);
#endif

  /* build a sorted list of stage catalog entries for the specified pool */
  scf = NULL;
  if ((scs = (struct sorted_ent *) calloc ((pool_p->migr->global_predicates.nbfiles_canbemig - pool_p->migr->global_predicates.nbfiles_beingmig), sizeof(struct sorted_ent))) == NULL) {
    stglogit(func, "### calloc error (%s)\n",strerror(errno));
    return(SYERR);
  }

  for (i = 0; i < pool_p->migr->nfileclass; i++) {
    pool_p->migr->fileclass[i]->streams = 0;
  }

  sci = scs;
  for (stcp = stcs; stcp < stce; stcp++) {
    if (stcp->reqid == 0) break;
    if (! ISCASTORCANBEMIG(stcp)) continue;       /* Not a being migrated candidate */
    okpoolname = 0;
    /* Does it belong to a pool managed by this migrator ? */
    for (ipoolname = 0, pool_n = pools; ipoolname < nbpool; ipoolname++, pool_n++) {
      /* We search the pool structure for this stcp */
      if (strcmp(stcp->poolname,pool_n->name) != 0) continue;
      /* And we check if it has the same migrator pointer */
      if (pool_n->migr != pool_p->migr) continue;
      /* We found it : this stcp is a candidate for migration */
      okpoolname = 1;
      break;
    }
    if (okpoolname == 0) continue;    /* This stcp is not one of our candidates for migration */
    if (stcp->u1.h.tppool[0] == '\0') {
      /* This should never be */
      stglogit(func, STG115, stcp->reqid, stcp->u1.h.xfile);
      free(scs);
      return(SEINTERNAL);
    }
    sci->weight = (double)stcp->a_time;
    if (stcp->actual_size > ONE_KB)
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
    sci->stcp = stcp;
    ++found_nbfiles;
    sci++;
  }

  if (found_nbfiles == 0) {
    stglogit(func, "### Internal error - Found 0 files to migrate !??\n");
    exit(0);
  }

  /* All entries that are to be migrated have a not-empty tape pool in member  */
  /* stcp->u1.h.tppool.                                                        */

  /* Concerning fileclasses uid/gid filters :                                  */
  /* If (uid/gid) filter is NOT set : migration is done under stage:st account */
  /* If (   /gid) filter is     set : migration is done under the first stcp   */
  /* If (uid/gid) filter is     set : migration is done under this account     */
  /* If (uid/   ) filter is     set : migration is done under this account     */

  /* We reset the internal counter on streams per fileclass */
  /* Note that we are already in a forked process, so this cannot clash with another */

  /* We group the stcp entries per tppool */
  while (1) {
    /* We search at least one entry not yet scanned */
    found_not_scanned = 0;
    i = -1;
    for (scc = scf; scc; scc = scc->next) {
      ++i;
      if (scc->scanned == 0) {
        found_not_scanned = 1;
        scc_found = scc;
        break;
      }
    }
    if (found_not_scanned == 0) {
      break;
    }
    /* We want to know all migration requests before we execute them */

    /* Is it a known poolname ? */
    found_tppool = 0;
    for (itppool = 0; itppool < ntppool_vs_stcp; itppool++) {
      if (strcmp(scc_found->stcp->u1.h.tppool, tppool_vs_stcp[itppool].tppool) == 0) {
        found_tppool = 1;
        break;
      }
    }
    if (found_tppool == 0) {
      if (ntppool_vs_stcp == 0) {
        if ((tppool_vs_stcp = (struct files_per_stream *) malloc(sizeof(struct files_per_stream))) == NULL) {
          stglogit(func, "### malloc error (%s)\n",strerror(errno));
          free(scs);
          return(SYERR);
        }
      } else {
        struct files_per_stream *dummy;
        
        if ((dummy = (struct files_per_stream *) realloc(tppool_vs_stcp,(ntppool_vs_stcp + 1) * sizeof(struct files_per_stream))) == NULL) {
          stglogit(func, "### realloc error (%s)\n",strerror(errno));
          free(scs);
          for (i = 0; i < ntppool_vs_stcp; i++) {
            if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
            if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
          }
          free(tppool_vs_stcp);
          return(SYERR);
        }
        tppool_vs_stcp = dummy;
      }
      found_tppool = ntppool_vs_stcp++;
      tppool_vs_stcp[ntppool_vs_stcp - 1].stcp = NULL;
      tppool_vs_stcp[ntppool_vs_stcp - 1].stpp = NULL;
      tppool_vs_stcp[ntppool_vs_stcp - 1].nstcp = 0;
      tppool_vs_stcp[ntppool_vs_stcp - 1].size = 0;       /* In BYTES - not MBytes */
      tppool_vs_stcp[ntppool_vs_stcp - 1].euid = 0;
      tppool_vs_stcp[ntppool_vs_stcp - 1].egid = 0;
      tppool_vs_stcp[ntppool_vs_stcp - 1].nb_substreams = 0;
      strcpy(tppool_vs_stcp[ntppool_vs_stcp - 1].tppool,scc_found->stcp->u1.h.tppool);
    }

    /* We grab the most restrictive (euid/egid) pair based on our known fileclasses */
    if (euid_egid(&(tppool_vs_stcp[found_tppool].euid),
                  &(tppool_vs_stcp[found_tppool].egid),
                  scc_found->stcp->u1.h.tppool,
                  pool_p->migr, scc_found->stcp, NULL, NULL, 1) != 0) {
      free(scs);
      for (i = 0; i < ntppool_vs_stcp; i++) {
        if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
        if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
      }
      free(tppool_vs_stcp);
      return(SYERR);
    }
    if (tppool_vs_stcp[found_tppool].egid == 0) {      /* No gid filter */
      tppool_vs_stcp[found_tppool].egid = stage_passwd.pw_gid; /* Put default gid */
      if (tppool_vs_stcp[found_tppool].euid == 0) {   /* No uid filter */
        tppool_vs_stcp[found_tppool].euid = stage_passwd.pw_uid; /* Put default uid */
      }
    } else {
      if (tppool_vs_stcp[found_tppool].euid == 0) {   /* No uid filter */
        tppool_vs_stcp[found_tppool].euid = scc_found->stcp->uid; /* Put current uid */
      }
    }
    if (verif_euid_egid(tppool_vs_stcp[found_tppool].euid,tppool_vs_stcp[found_tppool].egid, NULL, NULL) != 0) {
      free(scs);
      for (i = 0; i < ntppool_vs_stcp; i++) {
        if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
        if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
      }
      free(tppool_vs_stcp);
      return(SYERR);
    }

    /* We search all other entries sharing the same tape pool */
    j = -1;
    nfiles_per_tppool = 0;
    /* We reset internal flags to not do double count */
    for (i = 0; i < pool_p->migr->nfileclass; i++) {
      pool_p->migr->fileclass[i]->flag = 0;
    }
    for (scc = scf; scc; scc = scc->next) {
      if (scc->scanned != 0) continue;
      if ((scc == scc_found) || (strcmp(scc->stcp->u1.h.tppool,scc_found->stcp->u1.h.tppool) == 0)) {
        int ifileclass;

        /* And we count how of them we got */
        ++nfiles_per_tppool;
        if ((ifileclass = upd_fileclass(pool_p,scc->stcp)) >= 0) {
          if (pool_p->migr->fileclass[ifileclass]->flag == 0) {
            /* This pool was not yet counted */
            pool_p->migr->fileclass[ifileclass]->streams++;
            pool_p->migr->fileclass[ifileclass]->flag = 1;
          }
        }
      }
    }
    /* We group them into a single entity for API compliance */
    if (((stcp = tppool_vs_stcp[found_tppool].stcp = (struct stgcat_entry *) calloc(nfiles_per_tppool,sizeof(struct stgcat_entry))) == NULL) ||
        ((stpp = tppool_vs_stcp[found_tppool].stpp = (struct stgpath_entry *) calloc(nfiles_per_tppool,sizeof(struct stgpath_entry))) == NULL)) {
      stglogit(func, "### calloc error (%s)\n",strerror(errno));
      free(scs);
      for (i = 0; i < ntppool_vs_stcp; i++) {
        if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
        if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
      }
      free(tppool_vs_stcp);
      return(SYERR);
    }
    tppool_vs_stcp[found_tppool].nstcp = nfiles_per_tppool;
    /* We store them in this area */
    j = -1;
    for (scc = scf; scc; scc = scc->next) {
      if (scc->scanned != 0) continue;
      if ((scc == scc_found) || (strcmp(scc->stcp->u1.h.tppool,scc_found->stcp->u1.h.tppool) == 0)) {
        ++j;
        memcpy(stcp +j,scc->stcp,sizeof(struct stgcat_entry));
        /* We makes sure that the -K option is disabled... This means that when this copy will be migrated */
        /* The corresponding STAGEWRT entry in the catalog will disappear magically !... */
        (stcp+j)->keep = 0;
        /* We also makes sure that the -s parameter is disabled as well */
        (stcp+j)->size = 0;
        /* We remove the poolname argument that we will sent via stagewrt_hsm */
        (stcp+j)->poolname[0] = '\0';
        /* Please note that the (stcp+j)->ipath will be our user path (stpp) */
        strcpy((stpp+j)->upath, (stcp+j)->ipath);
        scc->scanned = 1;
        tppool_vs_stcp[found_tppool].size += scc->stcp->actual_size;
      }
    }
    /* Next round */
  }

  /* Here we have all the ntppool_vs_stcp streams to launch, each of them composed of */
  /* tppool_vs_stcp[0..ntppool_vs_stcp].nstcp entries */

  /* We check if the size of the migration does fit the minimum size per stream - default to 2GB */
  if (minsize > 0) {
    for (i = 0; i < ntppool_vs_stcp; i++) {
      if (tppool_vs_stcp[i].size < minsize) {
        char tmpbuf1[21];
        char tmpbuf2[21];
        
        /* One of the stream is of less size than the minimum - we give up */
        stglogit(func, "### Warning - stream on tape pool %s have size to be migrated %s < %s\n",
                 tppool_vs_stcp[i].stcp[0].u1.h.tppool,
                 u64tostr(tppool_vs_stcp[i].size, tmpbuf1, 0),
                 u64tostr(minsize, tmpbuf2, 0));
      }
    }
  }

  /* No need to waste memory inherited by parent anymore */
  if (stcs != NULL) free(stcs); stcs = NULL;
  if (stps != NULL) free(stps); stps = NULL;
  
  /* At first found we do not want to remember about previous tape pool because, per definition, */
  /* there is exactly one stream per tape pool */
  remember_tppool[0] = '\0';
  {
    /* We count the number of stageout pools */
    char savdefpoolname_out[10*(CA_MAXPOOLNAMELEN + 1)];
    char *p;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
    char *last = NULL;
#endif /* _REENTRANT || _THREAD_SAFE */

    strcpy(savdefpoolname_out, defpoolname_out);
    npoolname_out = 0;
    if ((p = strtok(defpoolname_out, ":")) != NULL) {
      while (1) {
        npoolname_out++;
        if ((p = strtok(NULL, ":")) == NULL) break;
      }
    }
    strcpy(defpoolname_out, savdefpoolname_out);
  }

  if (npoolname_out <= 0) {
    stglogit(func, "### Warning - npoolname_out <= 0 - Forced to be 1\n");
    npoolname_out = 1;
  }

  /* For all tape pools we determine up to how many streams it can be expanded */
  original_nb_of_stream = ntppool_vs_stcp;
  for (j = 0; j < original_nb_of_stream; j++) {
    int max_nfree_stream;
    int sav_ntppool_vs_stcp;
    int nstcp;
    int found_nideal_minsize;
    int created_new_streams;

    /* This stream has already been subject to expansion */
    if (tppool_vs_stcp[j].nb_substreams > 0) continue;

    /* We loop on all the fileclasses to determine which ones are concerned by this stream and up to */
    /* how many streams it allows it to be expanded */
    ideal_minsize = 0;
    nideal_minsize = 0;
    nstcp = 0;
    max_nfree_stream = 0;
    for (i = 0; i < pool_p->migr->nfileclass; i++) {
      int nfree_stream;
      int has_fileclass_i;
      
      if (pool_p->migr->fileclass[i]->streams <= 0) continue; /* This fileclass is not concerned */

      /* We start with a number of virtual stream equal to the real number of streams */
      pool_p->migr->fileclass[i]->nfree_stream = 0;

      /* We adapt the number of streams available v.s. number of stageout pools and current number of streams */
      if ((nfree_stream = (pool_p->migr->fileclass[i]->Cnsfileclass.maxdrives - pool_p->migr->fileclass[i]->streams)) <= 0) {
        nfree_stream = 1;
      } else {
        nfree_stream++;             /* Because we will expand one of them - we count it also */
        if ((nfree_stream /= npoolname_out) <= 0) {
          nfree_stream = 1;
        } else {
          if ((nfree_stream * npoolname_out) != (pool_p->migr->fileclass[i]->Cnsfileclass.maxdrives - pool_p->migr->fileclass[i]->streams + 1)) {
            if ((pool_p->migr->fileclass[i]->streams + nfree_stream) < pool_p->migr->fileclass[i]->Cnsfileclass.maxdrives) {
              nfree_stream++;
            }
          }
        }
      }

      if (nfree_stream <= 1) continue;       /* Cannot create any stream ? */

      /* We check if stream No j references the class No i */
      has_fileclass_i = 0;
      for (k = 0; k < tppool_vs_stcp[j].nstcp; k++) {
        if ((strcmp(tppool_vs_stcp[j].stcp[k].u1.h.server,pool_p->migr->fileclass[i]->server) == 0) &&
            (tppool_vs_stcp[j].stcp[k].u1.h.fileclass == pool_p->migr->fileclass[i]->Cnsfileclass.classid)) {
          has_fileclass_i = 1;
          break;
        }
      }
      if (has_fileclass_i == 0) continue;        /* No reference to fileclass No i in this stream */

      /* We keep in mind the grand total size of this stream and the number of entries in it */
      if (tppool_vs_stcp[j].size > minsize) {
        ideal_minsize += tppool_vs_stcp[j].size;
      } else {
        ideal_minsize += minsize;
      }
      nstcp += tppool_vs_stcp[j].nstcp;

      if (nfree_stream > max_nfree_stream) max_nfree_stream = nfree_stream;

      pool_p->migr->fileclass[i]->nfree_stream = nfree_stream;

    }

    if (ideal_minsize <= 0) continue;        /* No expansion for stream No j */

    /* We determine how many streams can be done by estimating the ideal_minsize */
    found_nideal_minsize = 0;
    for (nideal_minsize = 1; nideal_minsize <= max_nfree_stream; nideal_minsize++) {
      if ((ideal_minsize / nideal_minsize) < minsize) {
        found_nideal_minsize = --nideal_minsize;
        break;
      }
    }

    if (nideal_minsize == (max_nfree_stream + 1)) {
      /* We've done the whole for() loop upper */
      --nideal_minsize;
    }

    if (nideal_minsize <= 1) continue; /* No point to create another stream */

    if ((ideal_minsize /= nideal_minsize) < minsize) {
      ideal_minsize = minsize;
    }

    stglogit(func, "ideal minsize per stream for tape pool %s is %s, splitted in %d streams\n",
             tppool_vs_stcp[j].tppool,
             u64tostr((u_signed64) minsize, tmpbuf, 0),
             nideal_minsize
             );

    /* We know that we can create up to nideal_minsize streams        */
    /* We create room for them, assuming that memory is not a pb here */
    /* Please note that, by construction here, we have already */
    /* ntppool_vs_stcp streams, the nideal_minsize ones that we are */
    /* going to create are going to the end */
    created_new_streams = 1;
    sav_ntppool_vs_stcp = ntppool_vs_stcp;
    {
      struct files_per_stream *dummy;
        
      if ((dummy = (struct files_per_stream *) realloc(tppool_vs_stcp,(ntppool_vs_stcp + nideal_minsize) * sizeof(struct files_per_stream))) == NULL) {
        stglogit(func, "### realloc error (%s)\n",strerror(errno));
        /* But this is NOT fatal in the sense that we already have working streams - we just log this */
        /* and we do as if we found no need to create another stream... */
        created_new_streams = 0;
      } else {
        tppool_vs_stcp = dummy;
        for (k = 0; k < nideal_minsize; k++) {
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].stcp = NULL;
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].stpp = NULL;
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].tppool[0] = '\0';
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].nstcp = 0;
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].size = 0;
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].euid = tppool_vs_stcp[j].euid;
          tppool_vs_stcp[sav_ntppool_vs_stcp + k].egid = tppool_vs_stcp[j].egid;
          /* We know try to create enough room for our new stream */
          if (((stcp = tppool_vs_stcp[sav_ntppool_vs_stcp + k].stcp = (struct stgcat_entry *) calloc(nstcp,sizeof(struct stgcat_entry))) == NULL) ||
              ((stpp = tppool_vs_stcp[sav_ntppool_vs_stcp + k].stpp = (struct stgpath_entry *) calloc(nstcp,sizeof(struct stgpath_entry))) == NULL)) {
            stglogit(func, "### calloc error (%s)\n",strerror(errno));
            /* But this is NOT fatal in the sense that we already have working streams - we just log this */
            if (stcp != NULL) free(stcp);
            if (stpp != NULL) free(stpp);
            /* We left here nideal_minsize streams that will not be used - tant pis */
            created_new_streams = 0;
            break;
          } else {
            ntppool_vs_stcp++;
          }
        }
      }
    }

    if (created_new_streams == 0) continue;       /* Could not create the streams ! */

    /* We fill the streams with respect to fileclass */
    /* We have a sort of conceptual problem here when multiple fileclasses are present in stream No j */
    /* but when they said that they do not have equal number of free streams */

    /* We reset internal indexes */
    for (i = 0; i < pool_p->migr->nfileclass; i++) {
      /* Will range from 0 to (pool_p->migr->fileclass[i]->nfree_stream - 1) */
      pool_p->migr->fileclass[i]->ifree_stream = 0;
    }
    /* l variable will loop in range [sav_ntppool_vs_stcp,ntppool_vs_stcp] */
    for (k = 0; k < tppool_vs_stcp[j].nstcp; k++) {
      struct stgcat_entry *stcp;
      struct stgpath_entry *stpp;
      int ifileclass;

      if ((ifileclass = upd_fileclass(pool_p,&(tppool_vs_stcp[j].stcp[k]))) < 0) {
        stglogit(func, "### cannot determine fileclass of %s\n", tppool_vs_stcp[j].stcp[k].u1.h.xfile);
        /* Should never happen - by convention we put this stcp in first stream */
        l = 0;
      } else {
        if ((l = pool_p->migr->fileclass[ifileclass]->ifree_stream++) > (pool_p->migr->fileclass[ifileclass]->nfree_stream - 1)) {
          l = 0;
          pool_p->migr->fileclass[ifileclass]->ifree_stream = 1; /* So that next round l will be set to 1 */
        }
      }

      if (tppool_vs_stcp[sav_ntppool_vs_stcp + l].size > ideal_minsize) {
        int sav_l = l;
        /* We look if there is another stream that have enough room for us */

        while (1) {
          if ((ifileclass = upd_fileclass(pool_p,&(tppool_vs_stcp[j].stcp[k]))) < 0) {
            stglogit(func, "### cannot determine fileclass of %s\n", tppool_vs_stcp[j].stcp[k].u1.h.xfile);
            /* Should never happen - by convention we put this stcp in first stream */
            l = 0;
          } else {
            if ((l = pool_p->migr->fileclass[ifileclass]->ifree_stream++) > (pool_p->migr->fileclass[ifileclass]->nfree_stream - 1)) {
              l = 0;
              pool_p->migr->fileclass[ifileclass]->ifree_stream = 1; /* So that next round l will be set to 1 */
            }
          }
          if (l == sav_l) break;       /* We made a turnaround */
          if (tppool_vs_stcp[sav_ntppool_vs_stcp + l].size > ideal_minsize) continue; /* Not a good candidate */
        }
      }

      /* We copy entry No k of stream No j into entry No tppool_vs_stcp[sav_ntppool_vs_stcp + l].nstcp */
      /* of pool No l */
      stcp = tppool_vs_stcp[sav_ntppool_vs_stcp + l].stcp;
      stcp += tppool_vs_stcp[sav_ntppool_vs_stcp + l].nstcp;
      stpp = tppool_vs_stcp[sav_ntppool_vs_stcp + l].stpp;
      stpp += tppool_vs_stcp[sav_ntppool_vs_stcp + l].nstcp;
      *stcp = tppool_vs_stcp[j].stcp[k];
      strcpy(stpp->upath,tppool_vs_stcp[j].stpp[k].upath);
      tppool_vs_stcp[sav_ntppool_vs_stcp + l].size += tppool_vs_stcp[j].stcp[k].actual_size;
      tppool_vs_stcp[sav_ntppool_vs_stcp + l].nstcp++;
    }

    /* By definition stream No j have been completely emptied */
    tppool_vs_stcp[j].size = 0;
    tppool_vs_stcp[j].nstcp = 0;

    /* We flag it so */
    tppool_vs_stcp[j].nb_substreams = nideal_minsize;

    /* And we flag also the new streams */
    for (l = sav_ntppool_vs_stcp; l < (sav_ntppool_vs_stcp + nideal_minsize); l++) {
      tppool_vs_stcp[l].nb_substreams = nideal_minsize;
    }
  }


  /* Streams are created - By construction if a stream have nb_substreams > 0 but size == 0, this means */
  /* that it has been expanded */

  {
    int iprint = 0;

    for (j = 0; j < ntppool_vs_stcp; j++) {
      if ((tppool_vs_stcp[j].nb_substreams > 0) && (tppool_vs_stcp[j].size <= 0)) continue;
      stglogit(func, STG135,
               ++iprint,
               tppool_vs_stcp[j].nstcp,
               u64tostr(tppool_vs_stcp[j].size, tmpbuf, 0),
               tppool_vs_stcp[j].stcp[0].u1.h.tppool);
    }
  }

  /* We fork and execute the stagewrt request(s) */
  for (j = 0; j < ntppool_vs_stcp; j++) {
    if ((tppool_vs_stcp[j].nb_substreams > 0) && (tppool_vs_stcp[j].size <= 0)) continue;
    if ((fork_pid= fork()) < 0) {
      stglogit(func, "### Cannot fork (%s)\n",strerror(errno));
      free(scs);
      for (i = 0; i < ntppool_vs_stcp; i++) {
        if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
        if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
      }
      free(tppool_vs_stcp);
      return(SYERR);
    } else if (fork_pid == 0) {
      /* We are in the child */
      int rc;
      
#ifdef STAGER_DEBUG
      stglogit(func, "### stagewrt_hsm request : Please gdb /usr/local/bin/stgdaemon %d, then break %d\n",getpid(),__LINE__+2);
      sleep(9);
      sleep(1);
#endif

      free(scs);                                      /* Neither for the sorted entries */
      setegid(start_passwd.pw_gid);                   /* Move to admin (who knows) */
      seteuid(start_passwd.pw_uid);
      setegid(tppool_vs_stcp[j].egid);                /* Move to requestor (from fileclasses) */
      seteuid(tppool_vs_stcp[j].euid);
    stagewrt_hsm_retry:
      if ((rc = stagewrt_hsm((u_signed64) STAGE_SILENT|STAGE_NOHSMCREAT, /* Flags */
                             0,                            /* open flags - disabled */
                             localhost,                    /* Hostname */
                             NULL,                         /* Pooluser */
                             tppool_vs_stcp[j].nstcp,      /* nstcp_input */
                             tppool_vs_stcp[j].stcp,       /* records */
                             0,                            /* nstcp_output - none wanted */
                             NULL,                         /* stcp_output - none wanted */
                             tppool_vs_stcp[j].nstcp,      /* nstpp_input */
                             tppool_vs_stcp[j].stpp        /* stpp_input */
                             )) != 0) {
        stglogit(func, "### stagewrt_hsm request error No %d (%s)\n", serrno, sstrerror(serrno));
        if ((serrno == SECOMERR) || (serrno == SECONNDROP)) {
          /* There was a communication error */
          stglogit(func, "### retrying in 1 second\n");
          stage_sleep(1);
          goto stagewrt_hsm_retry;
        }
      }
      for (i = 0; i < ntppool_vs_stcp; i++) {
        if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
        if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
      }
      free(tppool_vs_stcp);
      setegid(start_passwd.pw_gid);                   /* Go back to admin */
      seteuid(start_passwd.pw_uid);
#ifdef STAGER_DEBUG
      stglogit(func, "### stagewrt_hsm request exiting with status %d\n", rc);
      if (rc != 0) {
        stglogit(func, "### ... serrno=%d (%s), errno=%d (%s)\n", serrno, sstrerror(serrno), errno, strerror(errno));
      }
#endif
      exit((rc != 0) ? SYERR : 0);
    }
  }

  free(scs);
  for (i = 0; i < ntppool_vs_stcp; i++) {
    if (tppool_vs_stcp[i].stcp != NULL) free(tppool_vs_stcp[i].stcp);
    if (tppool_vs_stcp[i].stpp != NULL) free(tppool_vs_stcp[i].stpp);
  }
  free(tppool_vs_stcp);

  /* No need to waste memory inherited by parent anymore */

  /* Wait for all child to exit */
  while (1) {
    if ((child_pid = waitpid(-1, &term_status, WNOHANG)) < 0) {
      break;
    }
    if (child_pid > 0) {
      if (WIFEXITED(term_status)) {
        stglogit("migpoolfiles","Migration child pid=%d exited, status %d\n",
                 child_pid, WEXITSTATUS(term_status));
      } else if (WIFSIGNALED(term_status)) {
        stglogit("migpoolfiles","Migration child pid=%d exited due to uncaught signal %d\n",
                 child_pid, WTERMSIG(term_status));
      } else {
        stglogit("migpoolfiles","Migration child pid=%d was stopped\n",
                 child_pid);
      }
    }
    sleep(1);
  }

  return(0);
}

void migpoolfiles_log_callback(level,message)
     int level;
     char *message;
{
  /*
    char func[16];

    if (level == MSG_OUT) {
    strcpy (func, "migpoolfiles");
    stglogit(func,"%s",message);
    }
  */
  return;
}

int isuserlevel(path)
     char *path;
{
  char *p[4];
  int c, rc;

  /* A user-level files begins like this: /xxx/yyy.zz/user/ */
  /*                                      1   2      3    4 */
  /* So we search up to 4 '/' and requires that string between */
  /* 3rd and 4th ones is "user" */

  /* Search No 1 */
  if ((p[0] = strchr(path,'/')) == NULL)
    return(0);
  /* Search No 2 */
  if ((p[1] = strchr(++p[0],'/')) == NULL)
    return(0);
  /* Search No 3 */
  if ((p[2] = strchr(++p[1],'/')) == NULL)
    return(0);
  /* Search No 4 */
  if ((p[3] = strchr(++p[2],'/')) == NULL)
    return(0);

  c = p[3][0];
  p[3][0] = '\0';

  rc = (strcmp(p[2],"user") == 0 ? 1 : 0);

  p[3][0] = c;
  return(rc);
}

#ifndef _WIN32
void poolmgr_wait4child(signo)
     int signo;
{
}
#endif

/* This routine created if necessary a new fileclass element in the fileclasses array */
int upd_fileclass(pool_p,stcp)
     struct pool *pool_p;
     struct stgcat_entry *stcp;
{
  struct Cns_fileid Cnsfileid;
  struct Cns_filestat Cnsfilestat;
  struct Cns_fileclass Cnsfileclass;
  int ifileclass = -1;
  int ifileclass_vs_migrator = -1;
  int i;

  if ((stcp == NULL) || (stcp->t_or_d != 'h')) {
    serrno = SEINTERNAL;
    return(-1);
  }

  /* We grab fileclass if necessary */
  if (stcp->u1.h.fileclass <= 0) {
    strcpy(Cnsfileid.server,stcp->u1.h.server);
    Cnsfileid.fileid = stcp->u1.h.fileid;
    if (Cns_statx(stcp->u1.h.xfile, &Cnsfileid, &Cnsfilestat) != 0) {
      sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "Cns_statx", sstrerror(serrno));
      return(-1);
    }
    if (stcp->u1.h.fileclass != Cnsfilestat.fileclass) {
      stcp->u1.h.fileclass = Cnsfilestat.fileclass;
#ifdef USECDB
      if (stgdb_upd_stgcat(&dbfd,stcp) != 0) {
        stglogit ("upd_fileclass", STG100, "update", sstrerror(serrno), __FILE__, __LINE__);
      }
#endif
      savereqs();
    }
  }

  /* We query the class if necessary */
  for (i = 0; i < nbfileclasses; i++) {
    if (fileclasses[i].Cnsfileclass.classid == stcp->u1.h.fileclass &&
        strcmp(fileclasses[i].server, stcp->u1.h.server) == 0) {
      ifileclass = i;
      break;
    }
  }
  if (ifileclass < 0) {
    int sav_nbcopies;
    char *pi, *pj, *pk;
    int j, k, have_duplicate, nb_duplicate, new_nbtppools;

    if (Cns_queryclass(stcp->u1.h.server, stcp->u1.h.fileclass, NULL, &Cnsfileclass) != 0) {
      sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "Cns_queryclass", sstrerror(serrno));
      return(-1);
    }

    /* @@@@ EXCEPTIONNAL @@@@ */
    /* Cnsfileclass.migr_time_interval = 60; */
    /* Cnsfileclass.maxdrives = 1; */
    /* Cnsfileclass.retenp_on_disk = 0; */
    /* @@@@ END OF EXCEPTIONNAL @@@@ */

    /* We check that this fileclass does not contain values that we cannot sustain */
    if ((sav_nbcopies = Cnsfileclass.nbcopies) < 0) {
      sendrep (rpfd, MSG_ERR, STG126, Cnsfileclass.name, stcp->u1.h.server, Cnsfileclass.classid, Cnsfileclass.nbcopies);
      return(-1);
    }
    /* We check that the number of tape pools is valid */
    if (Cnsfileclass.nbtppools < 0) {
      sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "Cns_queryclass", "returns invalid number of tape pool - invalid fileclass - Please contact your admin");
      return(-1);
    }

    /* We allow (nbcopies == 0 && nbtppool == 0) only */
    if ((sav_nbcopies == 0) || (Cnsfileclass.nbtppools == 0)) {
      if ((sav_nbcopies > 0) || (Cnsfileclass.nbtppools > 0)) {
        sendrep (rpfd, MSG_ERR, STG138, Cnsfileclass.name, stcp->u1.h.server, Cnsfileclass.classid, sav_nbcopies, Cnsfileclass.nbtppools);
        return(-1);
      }
      /* Here we are sure that both nbcopies and nbtpools are equal to zero */
      /* There is no need to check anything - we immediately add this fileclass */
      /* into the main array */
      goto upd_fileclass_add;
    }

    pi = Cnsfileclass.tppools;
    i = 0;
    while (*pi != '\0') {
      pi += (CA_MAXPOOLNAMELEN+1);
      if (++i >= Cnsfileclass.nbtppools) {
        /* We already have count as many tape pools as the fileclass specifies */
        break;
      }
    }
    if (i != Cnsfileclass.nbtppools) {
      sendrep (rpfd, MSG_ERR, STG127, Cnsfileclass.name, stcp->u1.h.server, Cnsfileclass.classid, Cnsfileclass.nbtppools, i);
      Cnsfileclass.nbtppools = i;
    }
    /* We reduce eventual list of duplicate tape pool in tppools element */
    nb_duplicate = 0;
    new_nbtppools = Cnsfileclass.nbtppools;
    while (1) {
      pi = Cnsfileclass.tppools;
      have_duplicate = 0;
      for (i = 0; i < new_nbtppools; i++) {
        pj = pi + (CA_MAXPOOLNAMELEN+1);
        for (j = i + 1; j < new_nbtppools; j++) {
          if (strcmp(pi,pj) == 0) {
            sendrep (rpfd, MSG_ERR, STG128, Cnsfileclass.name, stcp->u1.h.server, Cnsfileclass.classid, pi);
            have_duplicate = j;
            nb_duplicate++;
            break;
          }
          pj += (CA_MAXPOOLNAMELEN+1);
        }
        if (have_duplicate > 0) {   /* Because it starts a j+1, i starting at zero */
          pk = pj;
          for (k = j + 1; j < new_nbtppools; j++) {
            strcpy(pk - (CA_MAXPOOLNAMELEN+1), pk); 
            pk += (CA_MAXPOOLNAMELEN+1);
          }
          new_nbtppools--;
          break;
        }
        pi += (CA_MAXPOOLNAMELEN+1);
      }
      if (have_duplicate == 0) break;
    }
    if (new_nbtppools != Cnsfileclass.nbtppools) {
      /* We found duplicate(s) - so we rescan number of tape pools */
      sendrep (rpfd, MSG_ERR, STG129, Cnsfileclass.name, stcp->u1.h.server, Cnsfileclass.classid, Cnsfileclass.nbtppools, new_nbtppools);
      Cnsfileclass.nbtppools = new_nbtppools;
    }
    /* We check the number of copies vs. number of tape pools */
    if (Cnsfileclass.nbcopies > Cnsfileclass.nbtppools) {
      sendrep (rpfd, MSG_ERR, STG130, Cnsfileclass.name, stcp->u1.h.server, Cnsfileclass.classid, Cnsfileclass.nbcopies, Cnsfileclass.nbtppools, Cnsfileclass.nbtppools);
      Cnsfileclass.nbcopies = new_nbtppools;
    }

  upd_fileclass_add:
    /* We add this fileclass to the list of known fileclasses */
    if (nbfileclasses <= 0) {
      if ((fileclasses = (struct fileclass *) malloc(sizeof(struct fileclass))) == NULL) {
        sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "malloc", strerror(errno));
        return(-1);
      }
      nbfileclasses = 0;
    } else {
      struct fileclass *dummy;
      struct pool *pool_n;

      if ((dummy = (struct fileclass *)
           malloc((nbfileclasses + 1) * sizeof(struct fileclass))) == NULL) {
        sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "malloc", strerror(errno));
        return(-1);
      }
      /* We copy old content */
      memcpy(dummy, fileclasses, nbfileclasses * sizeof(struct fileclass));
      /* The realloc has been successful but fileclasses may have been moved - so we update all */
      /* references to previous indexes within fileclasses to indexes within dummy */
      /* This is simpler for me rather than playing with deltas in the memory */
      for (j = 0, pool_n = pools; j < nbpool; j++, pool_n++) {
        if (pool_n->migr == NULL) continue;
        for (i = 0; i < pool_n->migr->nfileclass; i++) {
          for (k = 0; k < nbfileclasses; k++) {               /* Nota - nbfileclasses have not been yet changed */
            if (pool_n->migr->fileclass[i] == &(fileclasses[k])) {
              pool_n->migr->fileclass[i] = &(dummy[k]);
              break;
            }
          }
        }
      }
      /* Update finished */
      free(fileclasses);
      fileclasses = dummy;
    }
    /* We update last element */
    fileclasses[nbfileclasses].Cnsfileclass = Cnsfileclass;
    strcpy(fileclasses[nbfileclasses].server,stcp->u1.h.server);
    fileclasses[nbfileclasses].last_tppool_used[0] = '\0';
    stglogit ("upd_fileclass", STG109,
              fileclasses[nbfileclasses].Cnsfileclass.name,
              fileclasses[nbfileclasses].server,
              fileclasses[nbfileclasses].Cnsfileclass.classid,
              nbfileclasses,
              fileclasses[nbfileclasses].Cnsfileclass.tppools);
    stglogfileclass(&Cnsfileclass);
    ifileclass = nbfileclasses++;
	/* File class at index (nbfileclasses - 1) has been created */
  }
  
  if (pool_p != NULL) {
    /* We check that this pool's migrator is aware about this fileclass */
    if (pool_p->migr != NULL) {
      for (i = 0; i < pool_p->migr->nfileclass; i++) {
        if (pool_p->migr->fileclass[i] == &(fileclasses[ifileclass])) {
          ifileclass_vs_migrator = i;
        }
      }
      if (ifileclass_vs_migrator < 0) {
        if (pool_p->migr->nfileclass <= 0) {
          if (((pool_p->migr->fileclass            = (struct fileclass **) malloc(sizeof(struct fileclass *))) == NULL) || 
              ((pool_p->migr->fileclass_predicates = (struct predicates *) malloc(sizeof(struct predicates))) == NULL)) {
            sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "malloc", strerror(errno));
            return(-1);
          }
          pool_p->migr->nfileclass = 0;
        } else {
          struct fileclass **dummy;
          struct predicates *dummy2;
          
          if ((dummy = (struct fileclass **) 
                realloc(pool_p->migr->fileclass,(pool_p->migr->nfileclass + 1) * sizeof(struct fileclass *))) == NULL) {
            sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "realloc", strerror(errno));
            return(-1);
          } else {
            if ((dummy2 = (struct predicates *)
                realloc(pool_p->migr->fileclass_predicates,(pool_p->migr->nfileclass + 1) * sizeof(struct predicates))) == NULL) {
              free(dummy);
              sendrep (rpfd, MSG_ERR, STG02, stcp->u1.h.xfile, "realloc", strerror(errno));
              return(-1);
            }
          }
          pool_p->migr->fileclass = dummy;
          pool_p->migr->fileclass_predicates = dummy2;
        }
        pool_p->migr->fileclass[pool_p->migr->nfileclass] = &(fileclasses[ifileclass]);
        memset(&(pool_p->migr->fileclass_predicates[pool_p->migr->nfileclass]),0,sizeof(struct predicates));
        ifileclass_vs_migrator = pool_p->migr->nfileclass++;
      }
    }
    return(ifileclass_vs_migrator);
  } else {
    /* No pool_p specified : we return the global index */
    return(ifileclass);
  }
}

/* This routine updates all the known fileclasses */
int upd_fileclasses()
{
  struct Cns_fileclass Cnsfileclass;
  int i;

  for (i = 0; i < nbfileclasses; i++) {
    if (Cns_queryclass(fileclasses[i].server, fileclasses[i].Cnsfileclass.classid, NULL, &Cnsfileclass) != 0) {
      stglogit ("upd_fileclasses", STG57,
                fileclasses[i].Cnsfileclass.name,
                fileclasses[i].Cnsfileclass.classid,
                fileclasses[i].server,
                i,
                "Cns_queryclass",
                sstrerror(serrno));
      /* We do not want to stop the update of other fileclasses, if any */
      /* return(-1); */
    }
    if (fileclasses[i].Cnsfileclass.tppools != NULL) {
      free(fileclasses[i].Cnsfileclass.tppools);
    }
    fileclasses[i].Cnsfileclass = Cnsfileclass;
	/* File class at index (nbfileclasses - 1) has been created */
    /* We check if the last tape pool used still belongs to this fileclass */
    /* If not, we reset last_tppool_used variable */
    upd_last_tppool_used(&(fileclasses[i]));
    stglogfileclass(&Cnsfileclass);
  }
  return(0);
}

void upd_last_tppool_used(fileclass)
     struct fileclass *fileclass;
{
  int j;
  int found = 0;
  char *p;

  if (fileclass->last_tppool_used[0] != '\0') {
    p = fileclass->Cnsfileclass.tppools;
    for (j = 0; j < fileclass->Cnsfileclass.nbtppools; j++) {
      if (strcmp(p,fileclass->last_tppool_used) == 0) {
        found = 1;
        break;
      }
      /* Tape pools are separated by '\0' */
      p += (CA_MAXPOOLNAMELEN+1);
    }
    if (found == 0) {
      /* Not found : we reset this knowledge... */
      stglogit("upd_last_tppool_used", STG111,
               fileclass->last_tppool_used,
               fileclass->Cnsfileclass.name,
               fileclass->server,
               fileclass->Cnsfileclass.classid);
      fileclass->last_tppool_used[0] = '\0';
    }
  }
}

char *next_tppool(fileclass)
     struct fileclass *fileclass;
{
  int i;
  int found_last_tppool = -1;
  char *p;

  /* The array tppool_used[0..(nbtppools_used - 1)] is a filter to tppools[0..(nbtppools - 1)] */

  p = fileclass->Cnsfileclass.tppools;
  if ((fileclass->last_tppool_used[0] == '\0') || (fileclass->Cnsfileclass.nbtppools == 1)) {
    /* No last_tppool_used yet - or the answer is always the same... */
    strcpy(fileclass->last_tppool_used,p);
    return(p);
  }
  for (i = 0; i < fileclass->Cnsfileclass.nbtppools; i++) {
    found_last_tppool = 0;
    if (strcmp(p,fileclass->last_tppool_used) == 0) {
      found_last_tppool = i;
      break;
    }
    p += (CA_MAXPOOLNAMELEN+1);
  }
  if (found_last_tppool == -1) {
    /* Not found : we reset this knowledge... */
    stglogit("next_tppool", STG113);
    p = fileclass->Cnsfileclass.tppools;
    strcpy(fileclass->last_tppool_used,p);
    return(p);
  }
  /* Found : either there is something after, either we have to turnaround */
  if (found_last_tppool == (fileclass->Cnsfileclass.nbtppools - 1)) {
    /* Was at the end of the list */
    p = fileclass->Cnsfileclass.tppools;
  } else {
    /* Got to next entry */
    p += (CA_MAXPOOLNAMELEN+1);
  }
  /* Update persistent knowledge for further selection */
  strcpy(fileclass->last_tppool_used,p);
  /* Please note that p is a pointer to a global array (declared static in this source) */
  return(p);
}

int euid_egid(euid,egid,tppool,migr,stcp,stcp_check,tppool_out,being_migr)
     uid_t *euid;
     gid_t *egid;
     char *tppool;
     struct migrator *migr;   /* Migrator - case of all automatic migration calls */
     struct stgcat_entry *stcp;   /* Must be non-NULL if migr == NULL */
     struct stgcat_entry *stcp_check;   /* Used to check requestor compatible uid/gid */
     char **tppool_out;
     int being_migr;              /* We check CAN_BE_MIGR, BEING_MIGR otherwise - only if migr != NULL */
{
  int i, j;
  char *p;
  int found_tppool;
  int found_fileclass;
  uid_t last_fileclass_euid = 0;
  gid_t last_fileclass_egid = 0;
  extern struct passwd start_passwd;             /* Start uid/gid at startup (admin) */
  extern struct passwd stage_passwd;             /* Generic uid/gid stage:st */

  /* At first call - application have to set them to zero - default is then "stage" uid/gid */
  if (*euid != (uid_t) 0) {
    last_fileclass_euid = *euid;  /* We simulate a virtual previous explicit filter */
  }
  if (*egid != (gid_t) 0) {
    last_fileclass_egid = *egid;  /* We simulate a virtual previous explicit filter */
  }

  /* If migrator is not specified, then stcp have to be speficied */
  if (migr == NULL) {
    if (stcp == NULL) {
      serrno = SEINTERNAL;
      return(-1);
    }
    /* We need to know what is the fileclass for this stcp */
    if ((i = upd_fileclass(NULL,stcp)) < 0) {
      return(-1);
    }
    p = fileclasses[i].Cnsfileclass.tppools;
    if ((tppool != NULL) && (tppool[0] != '\0')) {
      /* We check that this tppool really belongs to this fileclass */
      found_tppool = 0;
      for (j = 0; j < fileclasses[i].Cnsfileclass.nbtppools; j++) {
        if (strcmp(tppool,p) == 0) {
          found_tppool = 1;
          break;
        }
        p += (CA_MAXPOOLNAMELEN+1);
      }
      if (found_tppool == 0) {
        sendrep(rpfd, MSG_ERR, STG122, tppool);
        serrno = EINVAL;
        return(-1);
      }
      if (tppool_out != NULL) *tppool_out = p;
    } else {
      tppool = next_tppool(&(fileclasses[i]));
      if (tppool_out != NULL) *tppool_out = tppool;       /* Already allocated on the heap */
    }
    goto update_euid_egid;
  } else {
    /* If migrator is non-NULL, then pool have to be specified */
    if (tppool == NULL) {
      serrno = SEINTERNAL;
      return(-1);
    }
    if (tppool_out != NULL) *tppool_out = tppool;
  }

  for (i = 0; i < nbfileclasses; i++) {
    /* We check if this fileclass belongs to thoses that emerge for our migrator predicates */
    found_fileclass = 0;
    for (j = 0; j < migr->nfileclass; j++) {
      if (migr->fileclass[j] == &(fileclasses[i])) {
        found_fileclass = 1;
        break;
      }
    }
    if (found_fileclass == 0) continue;
    /*
    if (being_migr != 0) {
      if (migr->fileclass[j]->being_migr <= 0) continue;
    } else {
      if ((migr->fileclass_predicates[j].nbfiles_canbemig - migr->fileclass_predicates[j].nbfiles_beingmig) <= 0) continue;
    }
    if (found_fileclass == 0) {
      sendrep(rpfd, MSG_ERR, "STG02 - Cannot find fileclass that can trigger your migration\n");
      return(-1);
    }
    */
    /* From now on, we are sure that this fileclass contains files that are migration candidates */
    p = fileclasses[i].Cnsfileclass.tppools;
    found_tppool = 0;
    for (j = 0; j < fileclasses[i].Cnsfileclass.nbtppools; j++) {
      if (strcmp(tppool,p) == 0) {
        found_tppool = 1;
        break;
      }
      p += (CA_MAXPOOLNAMELEN+1);
    }
    if (found_tppool == 0) continue;
    if (tppool_out != NULL) *tppool_out = p;
  update_euid_egid:
    /* If uid in this fileclass is set and we did not yet set it ourself - we do so */
    if (fileclasses[i].Cnsfileclass.uid != (uid_t) 0) {
      /* If euid was not already set by a previous call or if euid was not already set by */
      /* a previous loop iteration */
      if (last_fileclass_euid == 0) {
        last_fileclass_euid = *euid = fileclasses[i].Cnsfileclass.uid;
      } else {
        /* If we already updated it - we check consistency */
        if (*euid != fileclasses[i].Cnsfileclass.uid) {
          sendrep(rpfd, MSG_ERR, STG118, last_fileclass_euid, fileclasses[i].Cnsfileclass.name, tppool, "uid", *euid, fileclasses[i].Cnsfileclass.uid);
          return(-1);
        }
      }
    }
    /* If gid in this fileclass is set and we did not yet set it ourself - we do so */
    if (fileclasses[i].Cnsfileclass.gid != (gid_t) 0) {
      /* If egid was not already set by a previous call or if egid was not already set by */
      /* a previous loop iteration */
      if (last_fileclass_egid == 0) {
        last_fileclass_egid = *egid = fileclasses[i].Cnsfileclass.gid;
      } else {
        /* If we already updated it - we check consistency */
        if (*egid != fileclasses[i].Cnsfileclass.gid) {
          sendrep(rpfd, MSG_ERR, STG118, last_fileclass_egid, fileclasses[i].Cnsfileclass.name, tppool, "gid", *egid, fileclasses[i].Cnsfileclass.gid);
          return(-1);
        }
      }
    }
    if (migr == NULL) {
      /* Case where we went there because of the goto upper */
      goto euid_egid_return;
    }
  }
 euid_egid_return:
  /* So here we have decided about euid and egid - If there is a stcp_check != NULL in the arguments, we also checks */
  /* validity of the requestor stcp_check */
  if (stcp_check != NULL) {
    /* We check the found group id in priority */
    if (last_fileclass_egid != 0) {
      /* There is an explicit filter on group id - current's stcp_check->gid have to match it */
      if ((*egid != stcp_check->gid) && (stcp_check->gid != start_passwd.pw_gid)) {
        sendrep(rpfd, MSG_ERR, STG125, stcp_check->user, "gid", stcp_check->gid, "group", *egid, stcp_check->u1.h.xfile);
        return(-1);
      }
      /* We check compatibility of uid */
      if (last_fileclass_euid != 0) {
        /* There is an explicit filter on user id - current's stcp_check->uid have to match it */
        if ((*euid != stcp_check->uid) && (stcp_check->uid != start_passwd.pw_uid)) {
          sendrep(rpfd, MSG_ERR, STG125, stcp_check->user, "uid", stcp_check->uid, "user", *euid, stcp_check->u1.h.xfile);
          return(-1);
        } else {
          /* Current's stcp_check's uid is matching ok */
          *euid = (stcp_check->uid != start_passwd.pw_uid) ? stcp_check->uid : (last_fileclass_euid != 0 ? last_fileclass_euid : stcp_check->uid);
          /* And it is also matching gid per def in this branch */
          *egid = (stcp_check->gid != start_passwd.pw_gid) ? stcp_check->gid : (last_fileclass_egid != 0 ? last_fileclass_egid : stcp_check->gid);
        }
      } else {
        /* Current's stcp_check's uid is matching since there is no filter on this */
        *euid = (stcp_check->uid != start_passwd.pw_uid) ? stcp_check->uid : (last_fileclass_euid != 0 ? last_fileclass_euid : stcp_check->uid);
        /* And it is also matching gid per def in this branch */
        *egid = (stcp_check->gid != start_passwd.pw_gid) ? stcp_check->gid : (last_fileclass_egid != 0 ? last_fileclass_egid : stcp_check->gid);
      }
    } else {
      /* There is no explicit filter on group id - Is there explicit filter on user id ? */
      if (last_fileclass_euid != 0) {
        /* There is an explicit filter on user id - current's stcp_check->uid have to match it */
        if ((*euid != stcp_check->uid) && (stcp_check->uid != start_passwd.pw_uid)) {
          sendrep(rpfd, MSG_ERR, STG125, stcp_check->user, "uid", stcp_check->uid, "user", *euid, stcp_check->u1.h.xfile);
          return(-1);
        } else {
          /* Current's stcp_check's uid is matching ok */
          *euid = (stcp_check->uid != start_passwd.pw_uid) ? stcp_check->uid : (last_fileclass_euid != 0 ? last_fileclass_euid : stcp_check->uid);
          /* Current's stcp_check's gid is matching since there is no filter on this */
          *egid = (stcp_check->gid != start_passwd.pw_gid) ? stcp_check->gid : (last_fileclass_egid != 0 ? last_fileclass_egid : stcp_check->gid);
        }
      } else {
        /* There is no filter at all so the default is "stage" account */
        /* Current's stcp_check's uid is matching since there is no filter on this */
        *euid = stage_passwd.pw_uid;
        /* Current's stcp_check's gid is matching since there is no filter on this */
        *egid = stage_passwd.pw_gid;
      }
    }
  }
  /* OK */
  return(0);
}

void stglogfileclass(Cnsfileclass)
     struct Cns_fileclass *Cnsfileclass;
{
  struct group *gr;
  int i;
  char *p;
  struct passwd *pw;
  gid_t sav_gid = -1;
  char sav_gidstr[7];
  uid_t sav_uid = -1;
  char sav_uidstr[CA_MAXUSRNAMELEN+1];
  char *func = "stglogfileclass";
  int verif_nbtppools = 0;

  if (Cnsfileclass->uid != sav_uid) {
    sav_uid = Cnsfileclass->uid;
		if (sav_uid == 0)
          strcpy (sav_uidstr, "-");
		else if ((pw = Cgetpwuid (sav_uid)) != NULL)
          strcpy (sav_uidstr, pw->pw_name);
		else
          sprintf (sav_uidstr, "%-8u", sav_uid);
  }
  if (Cnsfileclass->gid != sav_gid) {
    sav_gid = Cnsfileclass->gid;
    if (sav_gid == 0)
      strcpy (sav_gidstr, "-");
    else if ((gr = Cgetgrgid (sav_gid)) != NULL)
      strcpy (sav_gidstr, gr->gr_name);
    else
      sprintf (sav_gidstr, "%-6u", sav_gid);
  }
  p = Cnsfileclass->tppools;

  stglogit(func,"CLASS_ID       %d\n", Cnsfileclass->classid);
  stglogit(func,"CLASS_NAME     %s\n", Cnsfileclass->name);
  stglogit(func,"CLASS_UID      %d (%s)\n", (int) sav_uid, sav_uidstr);
  stglogit(func,"CLASS_GID      %d (%s)\n", (int) sav_gid, sav_gidstr);
  stglogit(func,"FLAGS          0x%x\n", Cnsfileclass->flags);
  stglogit(func,"MAXDRIVES      %d\n", Cnsfileclass->maxdrives);
  stglogit(func,"MIN FILESIZE   %d\n", Cnsfileclass->min_filesize);
  stglogit(func,"MAX FILESIZE   %d\n", Cnsfileclass->max_filesize);
  stglogit(func,"MAX SEGSIZE    %d\n", Cnsfileclass->max_segsize);
  stglogit(func,"MIGR INTERVAL  %d\n", Cnsfileclass->migr_time_interval);
  stglogit(func,"MIN TIME       %d\n", Cnsfileclass->mintime_beforemigr);
  stglogit(func,"NBCOPIES       %d\n", Cnsfileclass->nbcopies);
  stglogit(func,"RETENP_ON_DISK %d\n", Cnsfileclass->retenp_on_disk);
  stglogit(func,"NBTPPOOLS      %d\n", Cnsfileclass->nbtppools);
  if (*p != '\0') {
    verif_nbtppools++;
    stglogit(func,"TAPE POOL No %d %s\n", verif_nbtppools, p);
    for (i = 1; i < Cnsfileclass->nbtppools; i++) {
      p += (CA_MAXPOOLNAMELEN+1);
      if (*p != '\0') {
        verif_nbtppools++;
        stglogit(func,"TAPE POOL No %d %s\n", verif_nbtppools, p);
      } else {
        break;
      }
    }
  }

  if (verif_nbtppools != Cnsfileclass->nbtppools) {
    stglogit(func,"### Warning : Found tape pools does not match fileclass - forced to %d\n", verif_nbtppools);
    Cnsfileclass->nbtppools = verif_nbtppools;
  }
  if (Cnsfileclass->nbcopies > Cnsfileclass->nbtppools) {
    stglogit(func,"### Warning : Found number of copies is higher than number of tape pools - forced to %d\n", Cnsfileclass->nbtppools);
    Cnsfileclass->nbcopies = Cnsfileclass->nbtppools;
  }
}

void printfileclass(rpfd,fileclass)
     int rpfd;
     struct fileclass *fileclass;
{
  struct group *gr;
  int i;
  char *p;
  struct passwd *pw;
  gid_t sav_gid = -1;
  char sav_gidstr[7];
  uid_t sav_uid = -1;
  char sav_uidstr[CA_MAXUSRNAMELEN+1];
  int verif_nbtppools = 0;

  if (rpfd < 0) return;

  if (fileclass->Cnsfileclass.uid != sav_uid) {
    sav_uid = fileclass->Cnsfileclass.uid;
		if (sav_uid == 0)
          strcpy (sav_uidstr, "-");
		else if ((pw = Cgetpwuid (sav_uid)) != NULL)
          strcpy (sav_uidstr, pw->pw_name);
		else
          sprintf (sav_uidstr, "%-8u", sav_uid);
  }
  if (fileclass->Cnsfileclass.gid != sav_gid) {
    sav_gid = fileclass->Cnsfileclass.gid;
    if (sav_gid == 0)
      strcpy (sav_gidstr, "-");
    else if ((gr = Cgetgrgid (sav_gid)) != NULL)
      strcpy (sav_gidstr, gr->gr_name);
    else
      sprintf (sav_gidstr, "%-6u", sav_gid);
  }
  p = fileclass->Cnsfileclass.tppools;

  sendrep(rpfd,MSG_OUT,"FILECLASS %s@%s (classid=%d)\n", fileclass->Cnsfileclass.name, fileclass->server, fileclass->Cnsfileclass.classid);
  sendrep(rpfd,MSG_OUT,"\tCLASS_UID      %d (%s)\n", (int) sav_uid, sav_uidstr);
  sendrep(rpfd,MSG_OUT,"\tCLASS_GID      %d (%s)\n", (int) sav_gid, sav_gidstr);
  sendrep(rpfd,MSG_OUT,"\tFLAGS          0x%x\n", fileclass->Cnsfileclass.flags);
  sendrep(rpfd,MSG_OUT,"\tMAXDRIVES      %d\n", fileclass->Cnsfileclass.maxdrives);
  sendrep(rpfd,MSG_OUT,"\tMIN FILESIZE   %d\n", fileclass->Cnsfileclass.min_filesize);
  sendrep(rpfd,MSG_OUT,"\tMAX FILESIZE   %d\n", fileclass->Cnsfileclass.max_filesize);
  sendrep(rpfd,MSG_OUT,"\tMAX SEGSIZE    %d\n", fileclass->Cnsfileclass.max_segsize);
  sendrep(rpfd,MSG_OUT,"\tMIGR INTERVAL  %d\n", fileclass->Cnsfileclass.migr_time_interval);
  sendrep(rpfd,MSG_OUT,"\tMIN TIME       %d\n", fileclass->Cnsfileclass.mintime_beforemigr);
  sendrep(rpfd,MSG_OUT,"\tNBCOPIES       %d\n", fileclass->Cnsfileclass.nbcopies);
  sendrep(rpfd,MSG_OUT,"\tRETENP_ON_DISK %d\n", fileclass->Cnsfileclass.retenp_on_disk);
  sendrep(rpfd,MSG_OUT,"\tNBTPPOOLS      %d\n", fileclass->Cnsfileclass.nbtppools);
  if (*p != '\0') {
    verif_nbtppools++;
    sendrep(rpfd,MSG_OUT,"\tTAPE POOL No %d %s\n", verif_nbtppools, p);
    for (i = 1; i < fileclass->Cnsfileclass.nbtppools; i++) {
      p += (CA_MAXPOOLNAMELEN+1);
      if (*p != '\0') {
        verif_nbtppools++;
        sendrep(rpfd,MSG_OUT,"\tTAPE POOL No %d %s\n", verif_nbtppools, p);
      } else {
        break;
      }
    }
  }

  if (verif_nbtppools != fileclass->Cnsfileclass.nbtppools) {
    sendrep(rpfd,MSG_OUT,"### Warning : Found tape pools does not match fileclass - forced to %d\n", verif_nbtppools);
    fileclass->Cnsfileclass.nbtppools = verif_nbtppools;
  }
  if (fileclass->Cnsfileclass.nbcopies > fileclass->Cnsfileclass.nbtppools) {
    sendrep(rpfd,MSG_OUT,"### Warning : Found number of copies is higher than number of tape pools - forced to %d\n", fileclass->Cnsfileclass.nbtppools);
    fileclass->Cnsfileclass.nbcopies = fileclass->Cnsfileclass.nbtppools;
  }
}

int retenp_on_disk(ifileclass)
     int ifileclass;
{
  return(fileclasses[ifileclass].Cnsfileclass.retenp_on_disk);
}

void check_retenp_on_disk() {
	time_t this_time = time(NULL);
	struct stgcat_entry *stcp;
	char *func = "check_retenp_on_disk";
	extern struct fileclass *fileclasses;
	extern struct passwd start_passwd;             /* Start uid/gid at startup (admin) */

	for (stcp = stcs; stcp < stce; stcp++) {
		int ifileclass;
		int thisretenp;

		if (stcp->t_or_d != 'h') continue;      /* Not a CASTOR file */
		if (stcp->keep) continue;               /* Has -K option */
		if ((! ISSTAGEOUT(stcp)) || (! ISSTAGEWRT(stcp))) continue; /* Not coming from a STAGEOUT or a STAGEWRT request */
		if ((stcp->status & STAGED) != STAGED) continue; /* Not STAGEd */
		if ((ifileclass = upd_fileclass(NULL,stcp)) < 0) continue; /* Unknown fileclass */
		thisretenp = retenp_on_disk(ifileclass);
		if ((thisretenp == INFINITE_LIFETIME) || (thisretenp == AS_LONG_AS_POSSIBLE)) continue;
		if (((int) (this_time - stcp->a_time)) > thisretenp) { /* Lifetime exceeds ? */
			/* Candidate for garbage */
			stglogit (func, STG133, stcp->u1.h.xfile, fileclasses[ifileclass].Cnsfileclass.name, stcp->u1.h.server, fileclasses[ifileclass].Cnsfileclass.classid, thisretenp, (int) (this_time - stcp->a_time));
			if (delfile (stcp, 0, 1, 1, "check_retenp_on_disk", start_passwd.pw_uid, start_passwd.pw_gid, 0, 0) < 0) {
				stglogit (STG02, stcp->ipath, "rfio_unlink", rfio_serror());
			}
		}
	}
}

int ispool_out(poolname)
     char *poolname;
{
  char *p;

  if (poolname[0] == '\0') {
    /* No point */
    return(-1);
  }

  /* defpoolname_out could countain more than one entry, all separated with a ':' */
  /* We search poolname */
  if ((p = strstr(defpoolname_out, poolname)) == NULL) {
    return(-1);
  }

  /* We accept it only if it is: */
  /* - at the start of defpoolname_out and ends with an ':' or an '\0' */
  /* - not at the start and is preceded with an ':' and ends with an ':' or an '\0' */
  if ((p == defpoolname_out) && ((p[strlen(p)] == ':') || (p[strlen(p)] == '\0'))) return(0);
  if ((p != defpoolname_out) && (*(p - 1) == ':') && ((p[strlen(p)] == ':') || (p[strlen(p)] == '\0'))) return(0);

  /* No... */
  return(-1);
}

/* We return with this routine the best stageout pool we could select v.s. migration activity or not */
/* and v.s. estimated contention */
/* mode is READ_MODE for reading, WRITE_MODE for writing */

void bestnextpool_out(nextout,mode)
     char *nextout;
     int mode;
{
  char firstpool_out[CA_MAXPOOLNAMELEN+1];
  char thispool_out[CA_MAXPOOLNAMELEN+1];
  struct pool_element *this_element;
  struct pool_element_ext *best_elements = NULL;
  struct pool_element_ext *this_best_element;
  struct pool_element *found_element = NULL;
  int nbest_elements = 0;
  int i, j;
  struct pool *pool_p;
  struct pool_element *elemp;
  int qsort_all_poolout = 0;
  char *func = "bestnextpool_out";
  char tmpbuf[21];

  /* Do we support qsort v.s. multiples poolout ? */
  if (getconfent ("STG", "QSORT_ALL_POOLOUT", 0) != NULL) {
    /* Yes */
    qsort_all_poolout = 1;
  } else {
    /* We FORCE qsort on multiples poolouts only in one case : the very beginning */
    if (currentpool_out[0] == '\0') {
      stglogit(func, "First stageout round - forcing qsort on all outpools candidates\n");
      qsort_all_poolout = 1;
    }
  }

  /* We loop until we find a poolout that have no migrator, the bigger space available, the less */
  /* contention possible */

  nextpool_out(firstpool_out);
  strcpy(thispool_out, firstpool_out);

  while (1) {
    if ((this_element = betterfs_vs_pool(thispool_out,mode,0,NULL)) == NULL) {
      /* Oups... Tant-pis, return current pool */
      if (best_elements != NULL) free(best_elements);
      strcpy(nextout, thispool_out);
      return;
    }

    if (nbest_elements == 0) {
      if ((this_best_element = best_elements = (struct pool_element_ext *) malloc(sizeof(struct pool_element))) == NULL) {
        /* Oups... Tant-pis, return current pool */
        strcpy(nextout, thispool_out);
        return;
      }
    } else {
      struct pool_element_ext *dummy;

      if ((dummy = (struct pool_element_ext *) realloc(best_elements, (nbest_elements + 1) * sizeof(struct pool_element))) == NULL) {
        /* Oups... Tant-pis, return current pool */
        free(best_elements);
        strcpy(nextout, thispool_out);
        return;
      }
      best_elements = dummy;
      this_best_element = best_elements;
      this_best_element += nbest_elements;
    }
    this_best_element->free = this_element->free;
    this_best_element->nbreadaccess = this_element->nbreadaccess;
    this_best_element->nbwriteaccess = this_element->nbwriteaccess;
    this_best_element->mode = mode;
    this_best_element->nbreadserver = 0;
    this_best_element->nbwriteserver = 0;
    this_best_element->poolmigrating = ispoolmigrating(currentpool_out);
    this_best_element->server[0] = '\0';
    strcpy(this_best_element->server,this_element->server);
    this_best_element->dirpath[0] = '\0';
    strcpy(this_best_element->dirpath,this_element->dirpath);
    nbest_elements++;
    /* Do we support qsort v.s. multiples poolout ? */
    if (qsort_all_poolout == 0) {
      /* No */
      goto bestnextpool_out_getit;
    }

    /* Go to the next pool unless round about */

    nextpool_out(thispool_out);
    if (strcmp(thispool_out, firstpool_out) == 0) {
      stglogit(func, "Turnaround reached at outpool %s\n", thispool_out);
      break;
    }
  }

  if (nbest_elements > 1) {
    /* We are doing again the qsort using the best fs v.s. all poolouts */

    /* We fill the global number of streams per server, used in the qsort to optimize server location */
    for (i = 0, this_best_element = best_elements; i < nbest_elements; i++, this_best_element++) {
      get_global_stream_count(this_best_element->server, &(this_best_element->nbreadserver), &(this_best_element->nbwriteserver));
    }
    
    /* Sort them */
    qsort((void *) best_elements, nbest_elements, sizeof(struct pool_element_ext), &pool_elements_cmp);

#ifdef STAGER_DEBUG
    for (j = 0, this_best_element = best_elements; j < nbest_elements; j++, this_best_element++) {
      stglogit(func, "rank %2d: %s %s read=%d write=%d readserver=%d writeserver=%d poolmigrating=%d free=%s\n",
               j,
               this_best_element->server,
               this_best_element->dirpath,
               this_best_element->nbreadaccess,
               this_best_element->nbwriteaccess,
               this_best_element->nbreadserver,
               this_best_element->nbwriteserver,
               this_best_element->poolmigrating,
               u64tostru(this_best_element->free, tmpbuf, 0)
               );
    }
#endif
  } else {
#ifdef STAGER_DEBUG
    stglogit(func, "only one element: %10s %30s read=%d write=%d readserver=%d writeserver=%d poolmigrating=%d free=%s\n",
             this_best_element->server,
             this_best_element->dirpath,
             this_best_element->nbreadaccess,
             this_best_element->nbwriteaccess,
             this_best_element->nbreadserver,
             this_best_element->nbwriteserver,
             this_best_element->poolmigrating,
             u64tostru(this_best_element->free, tmpbuf, 0)
             );
#endif
  }

 bestnextpool_out_getit:
  /* So we have found here the best fs */
  /* We anticipate the coming call to selectfs() with the following trick: */
  /* - we know that selectfs() takes poolname as first argument and pool_p->next_pool_elem */
  /* as the fs to start the scanning with */
  /* We thus ly to pool_p->next_pool_elem, simply */

  /* Grab the result - this is the one on the top of the list */
  this_best_element = best_elements;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    for (j = 0, elemp = pool_p->elemp; j < pool_p->nbelem; j++, elemp++) {
      if ((strcmp(this_best_element->server ,elemp->server) == 0) &&
          (strcmp(this_best_element->dirpath,elemp->dirpath) == 0)) {
        found_element = elemp;
        /* Here is the trick */
        strcpy(nextout, pool_p->name);
        pool_p->next_pool_elem = j;
        break;
      }
    }
  }

  if (found_element == NULL) {
    /* Something fishy has happened ? */
    stglogit(func, "something fishy happened - using %s\n", thispool_out);
    strcpy(nextout, thispool_out);
  } else {
    stglogit(func, "selected element: %s %s read=%d write=%d readserver=%d writeserver=%d poolmigrating=%d free=%s\n",
             this_best_element->server,
             this_best_element->dirpath,
             this_best_element->nbreadaccess,
             this_best_element->nbwriteaccess,
             this_best_element->nbreadserver,
             this_best_element->nbwriteserver,
             this_best_element->poolmigrating,
             u64tostru(this_best_element->free, tmpbuf, 0)
             );
  }

  if (best_elements != NULL) free(best_elements);
}

void nextpool_out(nextout)
     char *nextout;
{
  char *p;
  int first_time = 0;

  /* We check if current pool out belongs to the poolout list */
  if (ispool_out(currentpool_out) != 0) {
    /* Either very first call, either list of poolout has changed and current poolout from */
    /* last call does not anymore belong to this list */
    currentpool_out[0] = '\0';
  }

  if (currentpool_out[0] == '\0') {
    first_time = 1;
  first_nextpool:
    /* We return the first one in the list up to first possible ':' delimiter */
    if ((p = strchr(defpoolname_out, ':')) != NULL) {
      *p = '\0';
      strcpy(nextout, defpoolname_out);
      *p = ':';
    } else {
      strcpy(nextout, defpoolname_out);
    }
    strcpy (currentpool_out, nextout);
    return;
  } else {
    char *p2;

    /* If curent pool is not migrating, we still return it */
    if (ispoolmigrating(currentpool_out) == 0) {
      strcpy(nextout, currentpool_out);
      return;
    }
    /* We return the next one in the list */
    if ((p = strstr(defpoolname_out, currentpool_out)) == NULL) {
      /* Current pool not in the list ? This should not happen because of ispool_out() call before */
      stglogit("nextpool_out", STG32, currentpool_out);
      goto first_nextpool;
    }
    /* We check if there is another one after */
    p += strlen(currentpool_out);
    if (p[0] == '\0') {
      /* We are the end - we go back to the first one */
      goto first_nextpool;
    }
    if ((p2 = strchr(++p, ':')) != NULL) {
      /* We are not at the end */
      *p2 = '\0';
      strcpy(nextout, p);
      strcpy (currentpool_out, nextout);
      *p2 = ':';
    } else {
      /* We are at the end... */
      strcpy(nextout, p);
      strcpy (currentpool_out, nextout);
    }
  }
  return;
}

int ispoolmigrating(poolname)
     char *poolname;
{
  int i;
  int found;
  struct pool *pool_p;

  found = 0;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) {
      found = 1;
      break;
    }
  
  if ((found != 0) && (pool_p->migr != NULL)) {
    return(pool_p->migr->mig_pid != 0 ? 1 : 0);       /* Will be != 0 if there is a migrator running */
  } else {
    return(0);
  }
}

/* mode is READ_MODE for reading, WRITE_MODE for writing, if req_size is specified (like in a selectfs() retry) */
/* it used as a threshold filter */

struct pool_element *betterfs_vs_pool(poolname,mode,reqsize,index)
     char *poolname;
     int mode;
     u_signed64 reqsize;
     int *index;
{
  int i, j, jfound;
  int found;
  struct pool *pool_p;
  struct pool_element *rc = NULL;
  struct pool_element_ext *elements;
  char *func = "betterfs_vs_pool";
#ifdef STAGER_DEBUG
  char tmpbuf[21];
  char tmpbufreqsize[21];
#endif

  found = 0;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++)
    if (strcmp (poolname, pool_p->name) == 0) {
      found = 1;
      break;
    }

  if (found == 0) {
    /* Hmmm... Pool not found! */
    stglogit(func, STG32, poolname);
    return(NULL);
  }

  /* We prepare the coming qsort */
  if ((elements = (struct pool_element_ext *) malloc(pool_p->nbelem * sizeof(struct pool_element_ext))) == NULL) {
    stglogit(func, STG02, poolname, "malloc", strerror(errno));
    return(NULL);
  }

  for (i = 0; i < pool_p->nbelem; i++) {
    elements[i].free = pool_p->elemp[i].free;
    elements[i].nbreadaccess = pool_p->elemp[i].nbreadaccess;
    elements[i].nbwriteaccess = pool_p->elemp[i].nbwriteaccess;
    elements[i].mode = mode;
    elements[i].nbreadserver = 0;
    elements[i].nbwriteserver = 0;
    elements[i].poolmigrating = ispoolmigrating(poolname); /* Not usefull in this sort, but pretty for output */
    strcpy(elements[i].server,pool_p->elemp[i].server);
    strcpy(elements[i].dirpath,pool_p->elemp[i].dirpath);
  }

  /* We fill the global number of streams per server, used in the qsort to optimize server location */
  for (i = 0; i < pool_p->nbelem; i++) {
    get_global_stream_count(elements[i].server, &(elements[i].nbreadserver), &(elements[i].nbwriteserver));
  }
    
  /* Sort them */
  qsort((void *) elements, pool_p->nbelem, sizeof(struct pool_element_ext), &pool_elements_cmp);

  jfound = -1;
  for (j = 0; j < pool_p->nbelem; j++) {
    if (reqsize <= 0) {
#ifdef STAGER_DEBUG
      stglogit(func, "rank %2d: %s %s read=%d write=%d readserver=%d writeserver=%d poolmigrating=%d free=%s\n",
               j,
               elements[j].server,
               elements[j].dirpath,
               elements[j].nbreadaccess,
               elements[j].nbwriteaccess,
               elements[j].nbreadserver,
               elements[j].nbwriteserver,
               elements[j].poolmigrating,
               u64tostru(elements[j].free, tmpbuf, 0)
               );
#endif
      if (jfound < 0) jfound = j;
    } else {
      if (elements[j].free < reqsize) {
#ifdef STAGER_DEBUG
        stglogit(func, "[reqsize=%s => rejected] rank %2d: %s %s read=%d write=%d readserver=%d writeserver=%d poolmigrating=%d free=%s\n",
                 u64tostru(reqsize, tmpbufreqsize, 0),
                 j,
                 elements[j].server,
                 elements[j].dirpath,
                 elements[j].nbreadaccess,
                 elements[j].nbwriteaccess,
                 elements[j].nbreadserver,
                 elements[j].nbwriteserver,
                 elements[j].poolmigrating,
                 u64tostru(elements[j].free, tmpbuf, 0)
                 );
#endif
      } else {
#ifdef STAGER_DEBUG
        stglogit(func, "[reqsize=%s => accepted] rank %2d: %s %s read=%d write=%d readserver=%d writeserver=%d poolmigrating=%d free=%s\n",
                 u64tostru(reqsize, tmpbufreqsize, 0),
                 j,
                 elements[j].server,
                 elements[j].dirpath,
                 elements[j].nbreadaccess,
                 elements[j].nbwriteaccess,
                 elements[j].nbreadserver,
                 elements[j].nbwriteserver,
                 elements[j].poolmigrating,
                 u64tostru(elements[j].free, tmpbuf, 0)
                 );
#endif
        if (jfound < 0) jfound = j;
      }
    }
  }

  if (jfound >= 0) {
    /* Grab the result */
    for (i = 0; i < pool_p->nbelem; i++) {
      if ((strcmp(elements[jfound].server ,pool_p->elemp[i].server) == 0) &&
          (strcmp(elements[jfound].dirpath,pool_p->elemp[i].dirpath) == 0)) {
        rc = &(pool_p->elemp[i]);
        if (index != NULL) {
          *index = i;
        }
        break;
      }
    }
  }

  /* Free space */
  free(elements);

  /* And return the element address */
  return(rc);
}

int pool_elements_cmp(p1,p2)
     CONST void *p1;
     CONST void *p2;
{
  struct pool_element_ext *fp1 = (struct pool_element_ext *) p1;
  struct pool_element_ext *fp2 = (struct pool_element_ext *) p2;

  /* Versus mode, we first compare nbreadaccess and nbwriteaccess */
  /* We sort with reverse order v.s. size */
  /* mode is READ_MODE for reading, WRITE_MODE for writing */
  /* Note that by construction all the elements shares the same mode in the qsort() */

  /* If one has a associated migration (where it is accessed or not) and not the other, no doubt */
  if (fp1->poolmigrating != fp2->poolmigrating) {
    if (fp2->poolmigrating != 0) {
      return(-1);
    } else {
      return(1);
    }
  }

  if ((strcmp(fp1->server,fp2->server) == 0) ||
      ((fp1->nbreadserver + fp1->nbwriteserver) == (fp2->nbreadserver + fp2->nbwriteserver))) {

    if (fp1->mode == WRITE_MODE) {

      /* Write mode */

      if ((fp1->nbreadaccess == 0) && (fp2->nbreadaccess == 0)) {

        if (fp1->nbwriteaccess == fp2->nbwriteaccess) {

        pool_elements_cmp_vs_free:
          if (fp1->free > fp2->free) {
            return(-1);
          } else if (fp1->free < fp2->free) {
            return(1);
          } else {
            return(0);
          }

        } else {

          if (fp1->nbwriteaccess < fp2->nbwriteaccess) {
            return(-1);
          } else {
            return(1);
          }

        }

      } else if (fp1->nbreadaccess == fp2->nbreadaccess) {

        if ((fp1->nbwriteaccess - fp1->nbreadaccess) == (fp2->nbwriteaccess - fp2->nbreadaccess)) {

          goto pool_elements_cmp_vs_free;

        } else if ((fp1->nbwriteaccess - fp1->nbreadaccess) < (fp2->nbwriteaccess - fp2->nbreadaccess)) {

          return(-1);

        } else {

          return(1);
        }

      } else {

        if ((fp1->nbwriteaccess + fp1->nbreadaccess) == (fp2->nbwriteaccess + fp2->nbreadaccess)) {

          goto pool_elements_cmp_vs_free;

        } else if ((fp1->nbwriteaccess + fp1->nbreadaccess) < (fp2->nbwriteaccess + fp2->nbreadaccess)) {

          return(-1);

        } else {

          return(1);
        }

      }

    } else {

      /* Read mode */

      if ((fp1->nbwriteaccess == 0) && (fp2->nbwriteaccess == 0)) {

        if (fp1->nbreadaccess == fp2->nbreadaccess) {

          goto pool_elements_cmp_vs_free;

        } else {

          if (fp1->nbreadaccess < fp2->nbreadaccess) {
            return(-1);
          } else {
            return(1);
          }

        }

      } else if (fp1->nbwriteaccess == fp2->nbwriteaccess) {

        if ((fp1->nbreadaccess - fp1->nbwriteaccess) == (fp2->nbreadaccess - fp2->nbwriteaccess)) {

          goto pool_elements_cmp_vs_free;

        } else if ((fp1->nbreadaccess - fp1->nbwriteaccess) < (fp2->nbreadaccess - fp2->nbwriteaccess)) {

          return(-1);

        } else {

          return(1);
        }

      } else {

        if ((fp1->nbreadaccess + fp1->nbwriteaccess) == (fp2->nbreadaccess + fp2->nbwriteaccess)) {

          goto pool_elements_cmp_vs_free;

        } else if ((fp1->nbreadaccess + fp1->nbwriteaccess) < (fp2->nbreadaccess + fp2->nbwriteaccess)) {

          return(-1);

        } else {

          return(1);
        }

      }

    }

  } else {

    /* Not the same host */

    if ((fp1->nbreadserver + fp1->nbwriteserver) == (fp2->nbreadserver + fp2->nbwriteserver)) {
      
      goto pool_elements_cmp_vs_free;
      
    } else if ((fp1->nbreadserver + fp1->nbwriteserver) < (fp2->nbreadserver + fp2->nbwriteserver)) {
      
      return(-1);
      
    } else {
      
      return(1);
    }

  }

}

void get_global_stream_count(server,nbreadserver,nbwriteserver)
     char *server;
     int *nbreadserver;
     int *nbwriteserver;
{
  struct pool_element *elemp;
  struct pool *pool_p;
  int i, j;

  *nbreadserver = 0;
  *nbwriteserver = 0;
  for (i = 0, pool_p = pools; i < nbpool; i++, pool_p++) {
    for (j = 0, elemp = pool_p->elemp; j < pool_p->nbelem; j++, elemp++) {
      if (strcmp(elemp->server, server) != 0) continue;
      *nbreadserver += elemp->nbreadaccess;
      *nbwriteserver += elemp->nbwriteaccess;
    }
  }
}

