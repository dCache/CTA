/*
 * $Id: stgdaemon.c,v 1.12 1999/12/15 08:20:02 jdurand Exp $
 */

/*
 * Copyright (C) 1993-1999 by CERN/IT/PDP/DM
 * All rights reserved
 */

/*
 * Nota : restore tab after emacs global identation:
 * perl -pi -e 'while (s/^(\t*)(  )/$1\t/) {} ' /tmp/stgdaemon.c
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stgdaemon.c,v $ $Revision: 1.12 $ $Date: 1999/12/15 08:20:02 $ CERN IT-PDP/DM Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <fcntl.h>
#if sgi || sun || ultrix
#include <sys/termio.h>
#endif
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#if _AIX
#include <sys/un.h>
#endif
#if _AIX && _IBMR2
#include <sys/select.h>
#endif
#include <sys/wait.h>
#if defined(ultrix) || (defined(sun) && !defined(SOLARIS))
#include <sys/resource.h>
#endif
#include "marshall.h"
#undef  unmarshall_STRING
#define unmarshall_STRING(ptr,str)  { str = ptr ; INC_PTR(ptr,strlen(str)+1) ; }
#include "rfio.h"
#include "net.h"
#include "stage.h"
#if SACCT
#include "../h/sacct.h"
#endif
#include "stgdb_Cdb_ifce.h"
#include <serrno.h>
#include "osdep.h"

extern char *optarg;
extern int optind;
extern int rfio_errno;
extern char *rfio_serror();
#if (defined(IRIX64) || defined(IRIX5) || defined(IRIX6))
extern int sendrep (int, int, ...);
#endif
#if !defined(linux)
extern char *sys_errlist[];
#endif
char defpoolname[MAXPOOLNAMELEN];
int force_init;
char func[16];
int initreq_reqid;
int initreq_rpfd;
int last_reqid;			/* last reqid used */
char logbuf[PRTBUFSZ];
int maxfds;
int nbcat_ent;
int nbpath_ent;
fd_set readfd, readmask;
int reqid;
int rpfd;
#if (defined(_AIX) && defined(_IBMR2)) || defined(SOLARIS) || defined(IRIX5) || (defined(__osf__) && defined(__alpha)) || defined(linux)
struct sigaction sa;
#endif
size_t stgcat_bufsz;
struct stgcat_entry *stce;	/* end of stage catalog */
struct stgcat_entry *stcs;	/* start of stage catalog */
size_t stgpath_bufsz;
struct stgpath_entry *stpe;	/* end of stage path catalog */
struct stgpath_entry *stps;	/* start of stage path catalog */
struct waitq *waitqp;
char *Default_db_user = "Cstg_username";
char *Default_db_pwd  = "Cstg_password";
struct stgdb_fd dbfd;
extern char *optarg;
extern int optind;

void prockilreq _PROTO((char *, char *));
void procinireq _PROTO((char *, char *));
void checkpoolstatus _PROTO(());
void checkwaitq _PROTO(());
void create_link _PROTO((struct stgcat_entry *, char *));
void dellink _PROTO((struct stgpath_entry *));
void delreq _PROTO((struct stgcat_entry *, int));
void rmfromwq _PROTO((struct waitq *));
void sendinfo2cptape _PROTO((int, struct stgcat_entry *));
void stgdaemon_usage _PROTO(());

main(argc,argv)
     int argc;
     char **argv;
{
  int c, i, l;
  char *clienthost;
  struct sockaddr_in from;
  int fromlen = sizeof(from);
  struct hostent *hp;
  int magic;
  int msglen;
  int on = 1;	/* for REUSEADDR */
  char *rbp;
  char req_data[REQBUFSZ-3*LONGSIZE];
  char req_hdr[3*LONGSIZE];
  int req_type;
  int rqfd;
  int scfd;
  struct sockaddr_in sin;		/* internet address */
  int spfd;
  struct servent *sp;
  struct stat st;
  struct stgcat_entry *stcp;
  int stg_s;
  struct stgpath_entry *stpp;
  struct timeval timeval;
  void wait4child();
  int foreground = 0;
  int errflg = 0;

#ifdef linux
  optind = 0;
#else
  optind = 1;
#endif
  while ((c = getopt (argc, argv, "fhv")) != EOF) {
    switch (c) {
    case 'f':
      foreground = 1;
      break;
    case 'h':
      stgdaemon_usage();
      exit(0);
    case 'v':
      printf("%s\n",sccsid);
      exit(0);
    case '?':
      ++errflg;
      break;
    default:
      ++errflg;
      printf("?? getopt returned character code 0%o (octal) 0x%lx (hex) %d (int) '%c' (char) ?\n"
          ,c,(unsigned long) c,c,(char) c);
      break;
    }
  }

  if (errflg != 0) {
    printf("### getopt error\n");
    exit(1);
  }

#if defined(ultrix) || (defined(sun) && (!defined(SOLARIS) || defined(SOLARIS25))) || (defined(__osf__) && defined(__alpha)) || defined(linux) || defined(IRIX6)
  maxfds = getdtablesize();
#else
  maxfds = _NFILE;
#endif
#ifndef TEST
  if (foreground == 0) {
    /* Background */
    if ((c = fork()) < 0) {
      fprintf (stderr, "stgdaemon: cannot fork\n");
      exit (1);
    } else
      if (c > 0) exit (0);
#if (defined(sun) && !defined(SOLARIS)) || defined(ultrix) || defined(_IBMESA)
    c = setpgrp(0, getpid());
#else
#if (defined(__osf__) && defined(__alpha)) || defined(linux)
    c = setsid();
#else
#if HPUX10
    c = setpgrp3();
#else
    c = setpgrp();
#endif
#endif
#endif
  for (c = 0; c < maxfds; c++)
    close (c);
#if ultrix || sun || sgi
  c = open ("/dev/tty", O_RDWR);
  if (c >= 0) {
    ioctl (c, TIOCNOTTY, 0);
    (void) close(c);
  }
#endif
  }
#endif /* not TEST */
#if defined(ultrix) || (defined(sun) && !defined(SOLARIS)) || (defined(_AIX) && defined(_IBMESA))
  signal (SIGCHLD, wait4child);
#else
#if (defined(sgi) && !defined(IRIX5)) || defined(hpux)
  signal (SIGCLD, wait4child);
#else
#if (defined(_AIX) && defined(_IBMR2)) || defined(SOLARIS) || defined(IRIX5) || (defined(__osf__) && defined(__alpha)) || defined(linux)
  sa.sa_handler = wait4child;
  sa.sa_flags = SA_RESTART;
  sigaction (SIGCHLD, &sa, NULL);
#endif
#endif
#endif
  strcpy (func, "stgdaemon");
  stglogit (func, "started\n");
#if SACCT
  stageacct (STGSTART, 0, 0, "", 0, 0, 0, 0, NULL, "");
#endif

  FD_ZERO (&readmask);
  FD_ZERO (&readfd);
  signal (SIGPIPE,SIG_IGN);

  umask (0);
  c = RFIO_NONET;
  rfiosetopt (RFIO_NETOPT, &c, 4);

  /* Open request socket */

  if ((stg_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    stglogit (func, STG02, "", "socket", sys_errlist[errno]);
    exit (CONFERR);
  }
  if ((sp = getservbyname (STG, "tcp")) == NULL) {
    stglogit (func, STG09, STG, "not defined in /etc/services");
    exit (CONFERR);
  }
  memset ((char *)&sin, 0, sizeof(struct sockaddr_in)) ;
  sin.sin_family = AF_INET ;
  sin.sin_port = sp->s_port;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  if (setsockopt (stg_s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
    stglogit (func, STG02, "", "setsockopt", sys_errlist[errno]);
  if (setsockopt (stg_s, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(on)) < 0)
    stglogit (func, STG02, "", "setsockopt", sys_errlist[errno]);
  if (bind (stg_s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
    stglogit (func, STG02, "", "bind", sys_errlist[errno]);
    exit (CONFERR);
  }
  listen (stg_s, 1) ;
	
  FD_SET (stg_s, &readmask);

  /* get pool configuration */

  if (c = getpoolconf (defpoolname)) exit (c);

#ifdef USECDB
  /* Get stager/database login:password */
  {
    FILE *configfd;
    char cfbuf[80];
    char *p_p, *p_u;

    if ((configfd = fopen(STGDBCONFIG,"r")) == NULL) {
      stglogit (func, "Cannot open Db Configuration file %s\n", STGDBCONFIG);
      stglogit (func, "Using default Db Username/Password = \"%s\"/\"<not printed>\"\n",Default_db_user);
      p_u = Default_db_user;
      p_p  = Default_db_pwd;
    } else {
      if (fgets (cfbuf, sizeof(cfbuf), configfd) &&
          strlen (cfbuf) >= 5 && (p_u = strtok (cfbuf, "/\n")) &&
          (p_p = strtok (NULL, "@\n"))) {
      } else {
        stglogit(func, "Error reading Db Configuration file %s\n",STGDBCONFIG);
        exit (CONFERR);
      }
      fclose(configfd);
    }

    if (strlen(p_u) > CA_MAXUSRNAMELEN || strlen(p_p) > CA_MAXUSRNAMELEN) {
      stglogit("func", 
               "Database username and/or password exceeds maximum length of %d characters !\n",
               CA_MAXUSRNAMELEN);
      exit (CONFERR);
    }

    /* Remember it for the future */
    strcpy(dbfd.username,p_u);
    strcpy(dbfd.password,p_p);
  }

  /* Log to the database server */
  if (stgdb_login(&dbfd) != 0) {
    stglogit(func, "Error loging to database server (%s)\n",sstrerror(serrno));
    exit(SYERR);
  }

  /* Open the database */
  if (stgdb_open(&dbfd,"stage") != 0) {
    stglogit(func, "Error opening \"stage\" database (%s)\n",sstrerror(serrno));
    exit(SYERR);
  }

  /* Ask for a dump of the catalog */
  if (stgdb_load(&dbfd,&stcs,&stce,&stgcat_bufsz,&stps,&stpe,&stgpath_bufsz) != 0) {
    stglogit(func, "Error loading \"stage\" databases into memory\n");
    exit(SYERR);
  }

  /* We compute the number of entries as well as the last used reqid */
  nbcat_ent = 0;
  for (stcp = stcs; stcp < stce; stcp++) {
    if (stcp->reqid == 0) {
      break;
    }
    nbcat_ent++;
  }
  if (nbcat_ent == 0) {
    last_reqid = 0;
  } else {
    last_reqid = (stcs + nbcat_ent - 1)->reqid;
  }

  nbpath_ent = 0;
  for (stpp = stps; stpp < stpe; stpp++) {
    if (stpp->reqid == 0) {
      break;
    }
    nbpath_ent++;
  }

#else /* USECDB */
        /* read stage catalog */

        scfd = open (STGCAT, O_RDWR | O_CREAT, 0664);
        fstat (scfd, &st);
        if (st.st_size == 0) {
                stgcat_bufsz = BUFSIZ;
                stcs = (struct stgcat_entry *) calloc (1, stgcat_bufsz);
                stce = stcs + (stgcat_bufsz/sizeof(struct stgcat_entry));
                nbcat_ent = 0;
                close (scfd);
                last_reqid = 0;
        } else {
                stgcat_bufsz = (st.st_size + BUFSIZ -1) / BUFSIZ * BUFSIZ;
                stcs = (struct stgcat_entry *) calloc (1, stgcat_bufsz);
                stce = stcs + (stgcat_bufsz/sizeof(struct stgcat_entry));
                nbcat_ent = st.st_size / sizeof(struct stgcat_entry);
                read (scfd, (char *) stcs, st.st_size);
                close (scfd);
                if (st.st_size != nbcat_ent * sizeof(struct stgcat_entry)) {
                        stglogit (func, STG48, STGCAT);
                        exit (SYERR);
                }
                for (stcp = stcs, i = 0; i < nbcat_ent; i++, stcp++)
                        if (stcp->reqid == 0) {
                                stglogit (func, STG48, STGCAT);
                                exit (SYERR);
                }
                last_reqid = (stcs + nbcat_ent - 1)->reqid;
        }
        spfd = open (STGPATH, O_RDWR | O_CREAT, 0664);
        fstat (spfd, &st);
        if (st.st_size == 0) {
                stgpath_bufsz = BUFSIZ;
                stps = (struct stgpath_entry *) calloc (1, stgpath_bufsz);
                stpe = stps + (stgpath_bufsz/sizeof(struct stgpath_entry));
                nbpath_ent = 0;
                close (spfd);
        } else {
                stgpath_bufsz = (st.st_size + BUFSIZ -1) / BUFSIZ * BUFSIZ;
                stps = (struct stgpath_entry *) calloc (1, stgpath_bufsz);
                stpe = stps + (stgpath_bufsz/sizeof(struct stgpath_entry));
                nbpath_ent = st.st_size / sizeof(struct stgpath_entry);
                read (spfd, (char *) stps, st.st_size);
                close (spfd);
                if (st.st_size != nbpath_ent * sizeof(struct stgpath_entry)) {
                        stglogit (func, STG48, STGPATH);
                        exit (SYERR);
                }
                for (stpp = stps, i = 0; i < nbpath_ent; i++, stpp++)
                        if (stpp->reqid == 0) {
                                stglogit (func, STG48, STGPATH);
                                exit (SYERR);
                }
        }

#endif

  /* remove uncompleted requests */
  for (stcp = stcs; stcp < stce; ) {
    if (stcp->reqid == 0) {
      break;
    }
    if (((stcp->status & 0xF) == STAGEIN &&
         (stcp->status & 0xF0) != STAGED) ||
        stcp->status == (STAGEOUT | WAITING_SPC) ||
        stcp->status == (STAGEALLOC | WAITING_SPC)) {
      if (delfile (stcp, 0, 0, 1, "startup", 0, 0, 0) < 0) { /* remove incomplete file */
        stglogit (func, STG02, stcp->ipath, "rfio_unlink",
                  rfio_serror());
        stcp++;
      }
    } else if (stcp->status == STAGEOUT ||
               stcp->status == STAGEALLOC) {
      if (rfio_stat (stcp->ipath, &st) == 0) {
        stcp->actual_size = st.st_size;
        stgdb_upd_stgcat(&dbfd,stcp);
      }
      updfreespace (stcp->poolname, stcp->ipath,
                    (int)stcp->actual_size - stcp->size*1024*1024);
      stcp++;
    } else if (stcp->status == STAGEWRT) {
      delreq (stcp,0);
    } else if (stcp->status == STAGEPUT) {
      stcp->status = STAGEOUT|PUT_FAILED;
      stgdb_upd_stgcat(&dbfd,stcp);
      stcp++;
    } else stcp++;
  }

  /* main loop */

  while (1) {
    checkpoolstatus ();	/* check if any pool just cleaned */
    checkwaitq ();	/* scan the wait queue */
    if (initreq_reqid && (waitqp == NULL || force_init)) {
      /* reread pool configuration + adjust space */
      reqid = initreq_reqid;
      rpfd = initreq_rpfd;
      c = updpoolconf (defpoolname);
      for (stcp = stcs; stcp < stce; stcp++) {
        if (stcp->status == STAGEIN ||
            stcp->status == STAGEOUT ||
            stcp->status == STAGEALLOC) {
          if (rfio_stat (stcp->ipath, &st) == 0) {
            stcp->actual_size = st.st_size;
            stgdb_upd_stgcat(&dbfd,stcp);
          }
          updfreespace (stcp->poolname, stcp->ipath,
					    (int)stcp->actual_size - stcp->size*1024*1024);
        }
      }
      if (c != 0) sendrep (rpfd, MSG_ERR, STG09, STGCONFIG, "incorrect");
      sendrep (rpfd, STAGERC, STAGEINIT, c);
      force_init = 0;
      initreq_reqid = 0;
    }

    if (FD_ISSET (stg_s, &readfd)) {
      rqfd = accept (stg_s, (struct sockaddr *) &from, &fromlen);
      reqid = nextreqid();
      l = netread_timeout (rqfd, req_hdr, sizeof(req_hdr), STGTIMEOUT);
      if (l == sizeof(req_hdr)) {
        size_t read_size;

        rbp = req_hdr;
        unmarshall_LONG (rbp, magic);
        unmarshall_LONG (rbp, req_type);
        unmarshall_LONG (rbp, msglen);
        rpfd = rqfd;
        l = msglen - sizeof(req_hdr);
        if ((read_size = netread_timeout (rqfd, req_data, l, STGTIMEOUT))) {
          if (req_type == STAGEIN || req_type == STAGEOUT || req_type == STAGEALLOC ||
              req_type == STAGEWRT || req_type == STAGEPUT)
            if (initreq_reqid ||
                stat (NOMORESTAGE, &st) == 0) {
              sendrep (rpfd, STAGERC, req_type,
                       ESTNACT);
              goto endreq;
            }
          if (getpeername (rqfd, (struct sockaddr*)&from,
                           &fromlen) < 0) {
            stglogit (func, STG02, "", "getpeername",
                      sys_errlist[errno]);
          }
          hp = gethostbyaddr ((char *)(&from.sin_addr),
                              sizeof(struct in_addr),from.sin_family);
          if (hp == NULL)
            clienthost = inet_ntoa (from.sin_addr);
          else
            clienthost = hp->h_name ;
          switch (req_type) {
          case STAGEIN:
          case STAGEOUT:
          case STAGEWRT:
          case STAGECAT:
            procioreq (req_type, req_data, clienthost);
            break;
          case STAGEPUT:
            procputreq (req_data, clienthost);
            break;
          case STAGEQRY:
            procqryreq (req_data, clienthost);
            break;
          case STAGECLR:
            procclrreq (req_data, clienthost);
            break;
          case STAGEKILL:
            prockilreq (req_data, clienthost);
            break;
          case STAGEUPDC:
            procupdreq (req_data, clienthost);
            break;
          case STAGEINIT:
            procinireq (req_data, clienthost);
            break;
          case STAGEALLOC:
            procallocreq (req_data, clienthost);
            break;
          case STAGEGET:
            procgetreq (req_data, clienthost);
            break;
          default:
            sendrep (rpfd, MSG_ERR, STG03, req_type);
          sendrep (rpfd, STAGERC, req_type, USERR);
          }
        } else {
          close (rqfd);
          if (l != 0)
            stglogit (func, "Network read error\n");
        }
      } else {
        close (rqfd);
        if (l != 0)
          stglogit (func, STG04, l);
      }
    endreq:
      FD_CLR (rqfd, &readfd);
    }
    memcpy (&readfd, &readmask, sizeof(readmask));
    timeval.tv_sec = CHECKI;	/* must set each time for linux */
    timeval.tv_usec = 0;
    if (select (maxfds, &readfd, (fd_set *)0, (fd_set *)0, &timeval) < 0) {
      FD_ZERO (&readfd);
    }
  }
}

void prockilreq(req_data, clienthost)
     char *req_data;
     char *clienthost;
{
  int clientpid;
  gid_t gid;
  char *rbp;
  char *user;
  struct waitq *wqp;

  rbp = req_data;
  unmarshall_STRING (rbp, user);	/* login name */
  unmarshall_WORD (rbp, gid);
  unmarshall_WORD (rbp, clientpid);

  wqp = waitqp;
  while (wqp) {
    if (wqp->clientpid == clientpid &&
        strcmp (wqp->clienthost, clienthost) == 0 &&
        wqp->gid == gid &&
        strcmp (wqp->user, user) == 0) {
      stglogit (func, "kill received for request %d\n", wqp->reqid);
      if (wqp->ovl_pid) {
        stglogit (func, "killing process %d\n", wqp->ovl_pid);
        kill (wqp->ovl_pid, SIGINT);
      }
      wqp->status = REQKILD;
      break;
    } else {
      wqp = wqp->next;
    }
  }
  close (rpfd);
}

void procinireq(req_data, clienthost)
     char *req_data;
     char *clienthost;
{
  char **argv;
  int c, i;
  gid_t gid;
  int nargs;
  char *rbp;
  char *user;

  rbp = req_data;
  unmarshall_STRING (rbp, user);	/* login name */
  unmarshall_WORD (rbp, gid);
  nargs = req2argv (rbp, &argv);
#if SACCT
  stageacct (STGCMDR, -1, gid, clienthost,
             reqid, STAGEINIT, 0, 0, NULL, "");
#endif
  if (initreq_reqid) {
    free (argv);
    sendrep (rpfd, MSG_ERR, STG39);
    sendrep (rpfd, STAGERC, STAGEINIT, USERR);
    return;
  }

#ifdef linux
  optind = 0;
#else
  optind = 1;
#endif
  while ((c = getopt (nargs, argv, "Fh:")) != EOF) {
    switch (c) {
    case 'F':
      force_init = 1;
    }
  }
  initreq_reqid = reqid;
  initreq_rpfd = rpfd;
  free (argv);
}

struct waitq *
add2wq (clienthost, user, uid, gid, clientpid, Upluspath, reqid, req_type, nbwf, wfp)
     char *clienthost;
     char *user;
     uid_t uid;
     gid_t gid;
     int clientpid;
     int Upluspath;
     int reqid;
     int req_type;
     int nbwf;
     struct waitf **wfp;
{
  /* add request to the wait queue */
  struct waitq *prev, *wqp;

  wqp = waitqp;
  while (wqp) {
    prev = wqp;
    wqp = wqp->next;
  }
  wqp = (struct waitq *) calloc (1, sizeof(struct waitq));
  if (!waitqp) {	/* queue is empty */
    waitqp = wqp;
  } else {
    prev->next = wqp;
    wqp->prev = prev;
  }
  strcpy (wqp->clienthost, clienthost);
  strcpy (wqp->user, user);
  wqp->uid = uid;
  wqp->gid = gid;
  wqp->clientpid = clientpid;
  wqp->Upluspath = Upluspath;
  wqp->reqid = reqid;
  wqp->req_type = req_type;
  wqp->key = time (0) & 0xFFFF;
  wqp->rpfd = rpfd;
  wqp->wf = (struct waitf *) calloc (nbwf, sizeof(struct waitf));
  *wfp = wqp->wf;
  return (wqp);
}

build_ipath(upath, stcp, pool_user)
     char *upath;
     struct stgcat_entry *stcp;
     char *pool_user;
{
  int c;
  int fd;
  char *p;
  char *p_f, *p_u;
  struct passwd *pw;

  if (*stcp->poolname == '\0') {
    strcpy (stcp->ipath, upath);
    return (0);
  }

  /* allocate space */

  if (selectfs (stcp->poolname, &stcp->size, stcp->ipath) < 0)
    return (-1);	/* not enough space */

  /* build full internal path name */

  sprintf (stcp->ipath+strlen(stcp->ipath), "/%s", stcp->group);

  p_u = stcp->ipath + strlen (stcp->ipath);
  sprintf (p_u, "/%s", pool_user);

  p_f = stcp->ipath + strlen (stcp->ipath);
  if (stcp->t_or_d == 't') {
    if (*(stcp->u1.t.fseq) != 'n')
      sprintf (p_f, "/%s.%s.%s",
               stcp->u1.t.vid[0], stcp->u1.t.fseq, stcp->u1.t.lbl);
    else
      sprintf (p_f, "/%s.%s.%s.%d",
               stcp->u1.t.vid[0], stcp->u1.t.fseq, stcp->u1.t.lbl,
               stcp->reqid);
  } else {
    if ((p = strrchr (stcp->u1.d.xfile, '/')) == NULL)
      p = stcp->u1.d.xfile;
    else
      p++;
    sprintf (p_f, "/%s.%d", p, stcp->reqid);
  }

  /* Do not create empty file on stagein/out (for Objectivity DB) */

  if (get_create_file_option (stcp->poolname))
    return(0);

  /* try to create file */

  (void) umask (stcp->mask);
  fd = rfio_open (stcp->ipath, O_WRONLY | O_CREAT, 0777);
  (void) umask (0);
  if (fd < 0) {
    if (errno != ENOENT && rfio_errno != ENOENT) {
      sendrep (rpfd, MSG_ERR, STG02, stcp->ipath, "rfio_open",
               rfio_serror());
      return (SYERR);
    }

    /* create group and user directories */

    *p_u = '\0';
    c = create_dir (stcp->ipath, 0, stcp->gid, 0755);
    *p_u = '/';
    if (c) {
      return (c);
    }
    if ((pw = getpwnam (pool_user)) == NULL) {
      sendrep (rpfd, MSG_ERR, STG11, pool_user);
      return (SYERR);
    }
    *p_f = '\0';
    c = create_dir (stcp->ipath, pw->pw_uid, stcp->gid, 0775);
    *p_f = '/';
    if (c) return (c);

    /* try again to create file */

    (void) umask (stcp->mask);
    fd = rfio_open (stcp->ipath, O_WRONLY | O_CREAT, 0777);
    (void) umask (0);
    if (fd < 0) {
      sendrep (rpfd, MSG_ERR, STG02, stcp->ipath, "rfio_open",
               rfio_serror());
      return (SYERR);
    }
  }
  close (fd);
  if (rfio_chown (stcp->ipath, stcp->uid, stcp->gid) < 0) {
    sendrep (rpfd, MSG_ERR, STG02, stcp->ipath, "rfio_chown",
             rfio_serror());
    return (SYERR);
  }
  return (0);
}

void
checkovlstatus(pid, status)
     int pid;
     int status;
{
  int found;
  int savereqid;
  struct waitq *wqp;

  savereqid = reqid;

  /* was it a "cleaner" overlay ? */
  if (iscleanovl (pid, status)) {
    reqid = 0;
    stglogit (func, "cleaner process %d exiting with status %x\n",
              pid, status & 0xFFFF);
  } else {	/* it was a "stager" or a "stageqry" overlay */
    found =  0;
    wqp = waitqp;
    while (wqp) {
      if (wqp->ovl_pid == pid) {
        found = 1;
        break;
      }
      wqp = wqp->next;
    }
    if (! found) {		/* entry already discarded from waitq or stageqry */
      reqid = 0;
      stglogit (func, "process %d exiting with status %x\n",
				pid, status & 0xFFFF);
    } else {
      reqid = wqp->reqid;
      stglogit (func, "stager process %d exiting with status %x\n",
				pid, status & 0xFFFF);
      wqp->ovl_pid = 0;
      if (wqp->status == 0)
        wqp->status = (status & 0xFF) ?
          SYERR : ((status >> 8) & 0xFF);
      wqp->nretry++;
    }
  }
  reqid = savereqid;
}

void checkpoolstatus()
{
  int c, i, j, n;
  char **poolc;
  struct stgcat_entry *stcp;
  struct waitf *wfp;
  struct waitq *wqp;

  n = checkpoolcleaned (&poolc);
  for (j = 0; j < n; j++) {
    for (wqp = waitqp; wqp; wqp = wqp->next) {
      if (strcmp (wqp->waiting_pool, poolc[j]) == 0) {
        reqid = wqp->reqid;
        rpfd = wqp->rpfd;
        for (i = 0, wfp = wqp->wf; i < wqp->nbdskf; i++, wfp++) {
          if (wfp->waiting_on_req > 0) continue;
          for (stcp = stcs; stcp < stce; stcp++) {
            if (wfp->subreqid == stcp->reqid)
              break;
          }
          if ((stcp->status & 0xF0) == WAITING_SPC) {
            if ((c = build_ipath (wfp->upath,
                                  stcp, wqp->pool_user)) < 0) {
              if (wqp->nb_clnreq++ > MAXRETRY) {
                sendrep (rpfd, MSG_ERR, STG45);
                wqp->status = ENOSPC;
              } else {
                strcpy (wqp->waiting_pool, stcp->poolname);
                cleanpool (stcp->poolname);
              }
              break;
            } else if (c) {
              wqp->status = c;
            } else {
              stcp->status &= 0xF;
              stgdb_upd_stgcat(&dbfd,stcp);
              *wqp->waiting_pool = '\0';
              if (stcp->status == STAGEOUT ||
                  stcp->status == STAGEALLOC) {
                if (wqp->Pflag)
                  sendrep (rpfd, MSG_OUT,
                           "%s\n", stcp->ipath);
                if (strcmp (stcp->ipath, wfp->upath))
                  create_link (stcp, wfp->upath);
                if (wqp->Upluspath &&
                    strcmp (stcp->ipath, (wfp+1)->upath))
                  create_link (stcp, (wfp+1)->upath);
              }
            }
          }
        }
      }
    }
  }
}

check_waiting_on_req(subreqid, state)
     int subreqid;
     int state;
{
  int c;
  int firstreqid;
  int found;
  int i;
  int savereqid;
  int saverpfd;
  struct stgcat_entry *stcp;
  struct waitf *wfp;
  struct waitq *wqp;

  found = 0;
  firstreqid = 0;
  savereqid = reqid;
  saverpfd = rpfd;
  for (wqp = waitqp; wqp; wqp = wqp->next) {
    if (! wqp->nb_waiting_on_req) continue;
    reqid = wqp->reqid;
    rpfd = wqp->rpfd;
    for (i = 0, wfp = wqp->wf; i < wqp->nb_subreqs; i++, wfp++) {
      if (wfp->waiting_on_req != subreqid) continue;
      found++;
      if (state == STAGED) {
        for (stcp = stcs; stcp < stce; stcp++) {
          if (stcp->reqid == 0) break;
          if (stcp->reqid == subreqid) break;
        }
#if SACCT
        stageacct (STGFILS, wqp->uid, wqp->gid, wqp->clienthost,
                   wqp->reqid, wqp->req_type, 0, 0, stcp, "");
#endif
        sendrep (rpfd, MSG_ERR, STG96,
                 strrchr (stcp->ipath, '/')+1,
                 stcp->actual_size,
                 (float)(stcp->actual_size)/(1024.*1024.),
                 stcp->nbaccesses);
        if (wqp->copytape)
          sendinfo2cptape (rpfd, stcp);
        if (*(wfp->upath) && strcmp (stcp->ipath, wfp->upath))
          create_link (stcp, wfp->upath);
        if (wqp->Upluspath &&
            strcmp (stcp->ipath, (wfp+1)->upath))
          create_link (stcp, (wfp+1)->upath);
        for (stcp = stcs; stcp < stce; stcp++) {
          if (wfp->subreqid == stcp->reqid)
            break;
        }
        delreq (stcp,0);
        wqp->nb_waiting_on_req--;
        wqp->nb_subreqs--;
        wqp->nbdskf--;
        for ( ; i < wqp->nb_subreqs; i++, wfp++) {
          wfp->subreqid = (wfp+1)->subreqid;
          wfp->waiting_on_req = (wfp+1)->waiting_on_req;
          strcpy (wfp->upath, (wfp+1)->upath);
        }
      } else {	/* FAILED or KILLED */
        if (firstreqid == 0) {
          for (stcp = stcs; stcp < stce; stcp++) {
            if (wfp->subreqid == stcp->reqid)
              break;
          }
          stcp->status &= 0xF;
          if (! wqp->Aflag) {
            if ((c = build_ipath (wfp->upath, stcp,
                                  wqp->pool_user)) < 0) {
              stcp->status |= WAITING_SPC;
              stgdb_upd_stgcat(&dbfd,stcp);
              strcpy (wqp->waiting_pool, stcp->poolname);
              wqp->nb_clnreq++;
              cleanpool (stcp->poolname);
            } else if (c) {
              wqp->status = c;
            } else {
              stgdb_upd_stgcat(&dbfd,stcp);
            }
          } else {
            stgdb_upd_stgcat(&dbfd,stcp);
          }
          wqp->nb_waiting_on_req--;
          wfp->waiting_on_req = -1;
          firstreqid = wfp->subreqid;
        } else {
          wfp->waiting_on_req = firstreqid;
        }
      }
      break;
    }
  }
  reqid = savereqid;
  rpfd = saverpfd;
  return (found);
}

void checkwaitq()
{
  int i;
  struct stgcat_entry *stcp;
  struct waitf *wfp;
  struct waitq *wqp, *wqp1;

  wqp = waitqp;
  while (wqp) {
    reqid = wqp->reqid;
    if (wqp->nb_subreqs == 0 && wqp->nb_waiting_on_req == 0 &&
        ! *(wqp->waiting_pool) && ! wqp->ovl_pid) {
      /* stage request completed successfully */
#if SACCT
      stageacct (STGCMDC, wqp->uid, wqp->gid, wqp->clienthost,
                 reqid, wqp->req_type, wqp->nretry, wqp->status,
                 NULL, "");
#endif
      sendrep (wqp->rpfd, STAGERC, wqp->req_type, wqp->status);
      wqp1 = wqp;
      wqp = wqp->next;
      rmfromwq (wqp1);
    } else if (wqp->nb_subreqs > wqp->nb_waiting_on_req &&
               ! wqp->ovl_pid &&
               ! *(wqp->waiting_pool) && wqp->status != ENOSPC &&
               wqp->status != USERR && wqp->status != CLEARED &&
               wqp->status != REQKILD && wqp->nretry < MAXRETRY) {
      if (wqp->nretry)
        sendrep (wqp->rpfd, MSG_ERR, STG43, wqp->nretry);
      fork_exec_stager (wqp);
      wqp = wqp->next;
    } else if (wqp->clnreq_reqid &&	/* space requested by rtcopy */
               (! *(wqp->waiting_pool) ||	/* has been freed or */
                wqp->status)) {		/* could not be found */
      reqid = wqp->clnreq_reqid;
      rpfd = wqp->clnreq_rpfd;
      wqp->clnreq_reqid = 0;
      wqp->clnreq_rpfd = 0;
      for (i = 0, wfp = wqp->wf; i < wqp->nbdskf; i++, wfp++)
        if (! wfp->waiting_on_req) break;
      for (stcp = stcs; stcp < stce; stcp++) {
        if (wfp->subreqid == stcp->reqid)
          break;
      }
      sendrep (rpfd, MSG_OUT, stcp->ipath);
      sendrep (rpfd, STAGERC, STAGEUPDC, wqp->status);
      wqp->status = 0;
      wqp = wqp->next;
    } else if (wqp->status) {
      /* request failed */
#if SACCT
      stageacct (STGCMDC, wqp->uid, wqp->gid, wqp->clienthost,
                 reqid, wqp->req_type, wqp->nretry, wqp->status,
                 NULL, "");
#endif
      if (wqp->status != REQKILD)
        sendrep (wqp->rpfd, STAGERC, wqp->req_type,
                 wqp->status);
      else
        close (wqp->rpfd);
      for (i = 0, wfp = wqp->wf; i < wqp->nbdskf; i++, wfp++) {
        for (stcp = stcs; stcp < stce; stcp++) {
          if (wfp->subreqid == stcp->reqid)
            break;
        }
        switch (stcp->status & 0xF) {
        case STAGEIN:
          if (wfp->waiting_on_req > 0) {
            delreq (stcp,0);
            continue;
          }
          if (delfile (stcp, 1, 0, 1, (wqp->status == REQKILD) ?
                       "req killed" : "failing req", wqp->uid, wqp->gid, 0) < 0)
            stglogit (func, STG02, stcp->ipath,
                      "rfio_unlink", rfio_serror());
          check_waiting_on_req (wfp->subreqid,
                                (wqp->status == REQKILD) ? KILLED : FAILED);
          break;
        case STAGEOUT:
        case STAGEWRT:
        case STAGEALLOC:
          delreq (stcp,0);
          break;
        case STAGEPUT:
          stcp->status = STAGEOUT | PUT_FAILED;
          stgdb_upd_stgcat(&dbfd,stcp);
        }
      }
      wqp1 = wqp;
      wqp = wqp->next;
      rmfromwq (wqp1);
    } else
      wqp = wqp->next;
  }
}

create_dir(dirname, uid, gid, mask)
     char *dirname;
     uid_t uid;
     gid_t gid;
#if (defined(sun) && !defined(SOLARIS)) || defined(ultrix)
     int mask;
#else
     mode_t mask;
#endif
{
  if (rfio_mkdir (dirname, mask) < 0) {
    if (errno == EEXIST || rfio_errno == EEXIST) {
      return (0);
    } else {
      sendrep (rpfd, MSG_ERR, STG02, dirname, "rfio_mkdir",
               rfio_serror());
      return (SYERR);
    }
  }
  if (rfio_chown (dirname, uid, gid) < 0) {
    sendrep (rpfd, MSG_ERR, STG02, dirname, "rfio_chown",
             rfio_serror());
    return (SYERR);
  }
  return (0);
}

void create_link(stcp, upath)
     struct stgcat_entry *stcp;
     char *upath;
{
  int c;
  int found;
  char lpath[MAXHOSTNAMELEN+MAXPATH];
  struct stgpath_entry *newpath();
  struct stgpath_entry *stpp;

  found = 0;
  for (stpp = stps; stpp < stpe; stpp++) {
    if (stpp->reqid == 0) break;
    if (strcmp (upath, stpp->upath)) continue;
    found = 1;
    break;
  }
  if (! found) {
    stpp = newpath ();
    strcpy (stpp->upath, upath);
  }
  stpp->reqid = stcp->reqid;
  if ((c = rfio_readlink (upath, lpath, sizeof(lpath))) > 0) {
    lpath[c] = '\0';
    if (strcmp (lpath, stcp->ipath) == 0) return;
  }
  if (c > 0 || errno == EINVAL || rfio_errno == EINVAL)
    sendrep (rpfd, RMSYMLINK, upath);
  stglogit (func, STG94, upath);
  sendrep (rpfd, SYMLINK, stcp->ipath, upath);
  savepath ();
  stgdb_ins_stgpath(&dbfd,stpp);
}

delfile(stcp, freersv, dellinks, delreqflg, by, byuid, bygid, remove_hsm)
     struct stgcat_entry *stcp;
     int freersv;
     int dellinks;
     int delreqflg;
     char *by;
     uid_t byuid;
     gid_t bygid;
     int remove_hsm;
{
  int actual_size = 0;
  struct stat st;
  struct stgpath_entry *stpp;

  if (stcp->ipath[0]) {
    /* Is there any other entry pointing at the same disk file?
       It could be the case if stagein+stagewrt or several stagewrt */
    int found = 0;
    struct stgcat_entry *stclp;
    for (stclp = stcs; stclp < stce; stclp++) {
      if (stclp->reqid == 0) break;
      if (stclp == stcp) continue;
      if (strcmp (stcp->ipath, stclp->ipath) == 0) {
        found = 1;
        break;
      }
    }
    if (! found) {
      /* it's the last entry;
         we can remove the file and release the space */
      if (! freersv) {
        if ((stcp->status & 0xF0) != STAGED &&
            rfio_stat (stcp->ipath, &st) == 0)
          actual_size = st.st_size;
        else
          actual_size = stcp->actual_size;

        stgdb_upd_stgcat(&dbfd,stcp);
      }
      if (rfio_unlink (stcp->ipath) == 0) {
#if SACCT
        stageacct (STGFILC, byuid, bygid, "",
                   reqid, 0, 0, 0, stcp, "");
#endif
        stglogit (func, STG95, stcp->ipath, by);
      } else if (errno != ENOENT && rfio_errno != ENOENT) {
        if (errno != EIO && rfio_errno != EIO)
          return (-1);
        else
          stglogit (func, STG02, stcp->ipath, "rfio_unlink",
                    rfio_serror());
      }
      updfreespace (stcp->poolname, stcp->ipath, (freersv) ?
                    (stcp->size*1024*1024) : actual_size);
    }
  }
  if (remove_hsm) {
    if (rfio_unlink (stcp->u1.m.xfile) == 0)
      stglogit (func, STG95, stcp->u1.m.xfile, by);
    else
      sendrep (rpfd, RTCOPY_OUT, STG02, stcp->u1.m.xfile,
               "rfio_unlink", rfio_serror());
  }
  if (dellinks) {
    for (stpp = stps; stpp < stpe; ) {
      if (stpp->reqid == 0) break;
      if (stcp->reqid == stpp->reqid)
        dellink (stpp);
      else
        stpp++;
    }
  }
  if (delreqflg)
    delreq (stcp,0);
  savepath ();
  return (0);
}

void dellink(stpp)
     struct stgpath_entry *stpp;
{
  int n;
  char *p1, *p2;

  stglogit (func, STG93, stpp->upath);
  stgdb_del_stgpath(&dbfd,stpp);
  sendrep (rpfd, RMSYMLINK, stpp->upath);
  nbpath_ent--;
  p2 = (char *)stps + (nbpath_ent * sizeof(struct stgpath_entry));
  if ((char *)stpp != p2) {	/* not last path in the list */
    p1 = (char *)stpp + sizeof(struct stgpath_entry);
    n = p2 + sizeof(struct stgpath_entry) - p1;
    memcpy ((char *)stpp, p1, n);
  }
  memset (p2, 0, sizeof(struct stgpath_entry));
}

/* If nodb_delete_flag is != 0 then only update in memory is performed */
void delreq(stcp,nodb_delete_flag)
     struct stgcat_entry *stcp;
     int nodb_delete_flag;
{
  int n;
  char *p1, *p2;

  if (nodb_delete_flag == 0) {
    stgdb_del_stgcat(&dbfd,stcp);
  }

  nbcat_ent--;
  p2 = (char *)stcs + (nbcat_ent * sizeof(struct stgcat_entry));
  if ((char *)stcp != p2) {	/* not last request in the list */
    p1 = (char *)stcp + sizeof(struct stgcat_entry);
    n = p2 + sizeof(struct stgcat_entry) - p1;
    memcpy ((char *)stcp, p1, n);
  }
  memset (p2, 0, sizeof(struct stgcat_entry));
  savereqs ();
}

fork_exec_stager(wqp)
     struct waitq *wqp;
{
  char arg_Aflag[2];
  char arg_key[6], arg_nbsubreqs[4], arg_reqid[7], arg_nretry[3], arg_rpfd[3];
  int c, i;
  static int pfd[2];
  int pid;
  char progfullpath[MAXPATH];
  struct stgcat_entry *stcp;
  struct waitf *wfp;

  if (pipe (pfd) < 0) {
    sendrep (wqp->rpfd, MSG_ERR, STG02, "", "pipe", sys_errlist[errno]);
    return (SYERR);
  }
  wqp->ovl_pid = fork ();
  pid = wqp->ovl_pid;
  if (pid < 0) {
    sendrep (wqp->rpfd, MSG_ERR, STG02, "", "fork", sys_errlist[errno]);
    return (SYERR);
  } else if (pid == 0) {	/* we are in the child */
    for (c = 0; c < maxfds; c++)
      if (c != pfd[0] && c != wqp->rpfd) close (c);
    dup2 (pfd[0], 0);
    sprintf (progfullpath, "%s/stager", BIN);
    sprintf (arg_reqid, "%d", wqp->reqid);
    sprintf (arg_key, "%d", wqp->key);
    sprintf (arg_rpfd, "%d", wqp->rpfd);
    sprintf (arg_nbsubreqs, "%d", wqp->nbdskf - wqp->nb_waiting_on_req);
    sprintf (arg_nretry, "%d", wqp->nretry);
    sprintf (arg_Aflag, "%d", wqp->Aflag);

    stglogit (func, "execing stager, pid=%d\n", getpid());
    execl (progfullpath, "stager", arg_reqid, arg_key, arg_rpfd,
           arg_nbsubreqs, arg_nretry, arg_Aflag, 0);
    stglogit (func, STG02, "stager", "execl", sys_errlist[errno]);
    exit (SYERR);
  } else {
    wqp->status = 0;
    close (pfd[0]);
#if SACCT
    stageacct (STGCMDS, wqp->uid, wqp->gid, wqp->clienthost,
               wqp->reqid, wqp->req_type, wqp->nretry, wqp->status,
               NULL, "");
#endif
    for (i = 0, wfp = wqp->wf; i < wqp->nbdskf; i++, wfp++) {
      if (wfp->waiting_on_req > 0) continue;
      if (wfp->waiting_on_req < 0) wfp->waiting_on_req = 0;
      for (stcp = stcs; stcp < stce; stcp++)
        if (stcp->reqid == wfp->subreqid) break;
      write (pfd[1], stcp, sizeof(struct stgcat_entry));
    }
    close (pfd[1]);
  }
  return (0);
}

struct stgpath_entry *
newpath()
{
  struct stgpath_entry *stpp;

  nbpath_ent++;
  if (nbpath_ent > stgpath_bufsz/sizeof(struct stgpath_entry)) {
    stps = (struct stgpath_entry *) realloc (stps, stgpath_bufsz+BUFSIZ);
    memset ((char *)stps+stgpath_bufsz, 0, BUFSIZ);
    stgpath_bufsz += BUFSIZ;
    stpe = stps + (stgpath_bufsz/sizeof(struct stgpath_entry));
  }
  stpp = stps + (nbpath_ent - 1);
  return (stpp);
}

struct stgcat_entry *
newreq()
{
  struct stgcat_entry *stcp;

  nbcat_ent++;
  if (nbcat_ent > stgcat_bufsz/sizeof(struct stgcat_entry)) {
    stcs = (struct stgcat_entry *) realloc (stcs, stgcat_bufsz+BUFSIZ);
    memset ((char *)stcs+stgcat_bufsz, 0, BUFSIZ);
    stgcat_bufsz += BUFSIZ;
    stce = stcs + (stgcat_bufsz/sizeof(struct stgcat_entry));
  }
  stcp = stcs + (nbcat_ent - 1);
  return (stcp);
}

nextreqid()
{
  int found;
  struct stgcat_entry *stcp;

  while (1) {
    found = 0;
    if (++last_reqid > MAXREQID) last_reqid = 1;
    for (stcp = stcs; stcp < stce; stcp++) {
      if (stcp->reqid == 0) break;
      if (stcp->reqid == last_reqid) {
        found = 1;
        break;
      }
    }
    if (! found) return (last_reqid);
  }
}

req2argv(rbp, argvp)
     char *rbp;
     char ***argvp;
{
  char **argv;
  int i, n;
  int nargs;
  char *p;

  unmarshall_WORD (rbp, nargs);
  argv = (char **) malloc ((nargs + 1) * sizeof(char *));
  unmarshall_STRING (rbp, argv[0]);
  strcpy (logbuf, argv[0]);
  for (i = 1; i < nargs; i++) {
    unmarshall_STRING (rbp, argv[i]);
    strcat (logbuf, " ");
    n = strlen (argv[i]);
    p = argv[i];
    if ((int) (strlen (logbuf) + n) > (PRTBUFSZ - 43)) {
      strcat (logbuf, "\\");
      stglogit (func, STG98, logbuf);
      strcpy (logbuf, "+ ");
      while (n > (PRTBUFSZ - 45)) {
        strncat (logbuf, p, PRTBUFSZ - 45);
        strcat (logbuf, "\\");
        stglogit (func, STG98, logbuf);
        strcpy (logbuf, "+ ");
        n -= PRTBUFSZ - 45;
        p += PRTBUFSZ - 45;
      }
    }
    strcat (logbuf, p);
  }
  argv[nargs] = NULL;
  stglogit (func, STG98, logbuf);
  *argvp = argv;
  return (nargs);
}

void rmfromwq(wqp)
     struct waitq *wqp;
{
  if (wqp->prev) (wqp->prev)->next = wqp->next;
  else waitqp = wqp->next;
  if (wqp->next) (wqp->next)->prev = wqp->prev;
  free (wqp->wf);
  free (wqp);
}

savepath()
{
  int c, n;
  int spfd;

#ifdef USECDB
  /* This function is now dummy with the DB interface */
#else
  if ((spfd = open (STGPATH, O_WRONLY)) < 0) {
    stglogit (func, STG02, STGPATH, "open", sys_errlist[errno]);
    return (-1);
  }
  n = nbpath_ent * sizeof(struct stgpath_entry);
  if (nbpath_ent != 0) {
    if ((c = write (spfd, stps, n)) != n) {
      stglogit (func, STG02, STGPATH, "write", sys_errlist[errno]);
      close (spfd);
      return (-1);
    }
  }
#if cray
  trunc (spfd);
#else
  ftruncate (spfd, n);
#endif
  close (spfd);
#endif

  return (0);
}

savereqs()
{
  int c, n;
  int scfd;

#ifdef USECDB
  /* This function is now dummy with the DB interface */
#else
  if ((scfd = open (STGCAT, O_WRONLY)) < 0) {
    stglogit (func, STG02, STGCAT, "open", sys_errlist[errno]);
    return (-1);
  }
  n = nbcat_ent * sizeof(struct stgcat_entry);
  if (nbcat_ent != 0) {
    if ((c = write (scfd, stcs, n)) != n) {
      stglogit (func, STG02, STGCAT, "write", sys_errlist[errno]);
      close (scfd);
      return (-1);
    }
  }
#if cray
  trunc (scfd);
#else
  ftruncate (scfd, n);
#endif
  close (scfd);
#endif

  return (0);
}

void sendinfo2cptape(rpfd, stcp)
     int rpfd;
     struct stgcat_entry *stcp;
{
  char buf[256];

  if (strcmp (stcp->u1.t.lbl, "sl") == 0 || strcmp (stcp->u1.t.lbl, "al") == 0)
    sprintf (buf, "-P %s -q %s -b %d -F %s -f %s -L %d",
             stcp->ipath, stcp->u1.t.fseq, stcp->blksize,
             stcp->recfm, stcp->u1.t.fid, stcp->lrecl);
  else
    sprintf (buf, "-P %s -q %s", stcp->ipath, stcp->u1.t.fseq);
  sendrep (rpfd, MSG_ERR, STG47, buf);
}

upd_stageout(req_type, upath, subreqid)
     int req_type;
     char *upath;
     int *subreqid;
{
  int found;
  struct stat st;
  struct stgcat_entry *stcp;
  struct stgpath_entry *stpp;

  found = 0;
  /* first lets assume that internal and user path are different */
  for (stpp = stps; stpp < stpe; stpp++) {
    stglogit(func, "stpp->reqid=%d...\n",stpp->reqid);
    if (stpp->reqid == 0) break;
    stglogit(func, "stpp->reqid=%d, Comparing upath=\"%s\" and stpp->upath=\"%s\"\n",stpp->reqid,upath,stpp->upath);
    if (strcmp (upath, stpp->upath)) continue;
    found = 1;
    break;
  }
  if (found) {
    for (stcp = stcs; stcp < stce; stcp++) {
      stglogit(func, "stcp->reqid=%d...\n",stcp->reqid);
      stglogit(func, "stpp->reqid=%d == stcp->reqid=%d ?\n",stpp->reqid,stcp->reqid);
      if (stpp->reqid == stcp->reqid) break;
    }
  } else {
    for (stcp = stcs; stcp < stce; stcp++) {
      stglogit(func, "stcp->reqid=%d...\n",stcp->reqid);
      if (stcp->reqid == 0) break;
      stglogit(func, "stcp->reqid=%d, Comparing upath=\"%s\" and stcp->upath=\"%s\"\n",stcp->reqid,upath,stcp->ipath);
      if (strcmp (upath, stcp->ipath)) continue;
      found = 1;
      break;
    }
  }
  if (found == 0 ||
      (req_type == STAGEUPDC &&
       stcp->status != STAGEOUT && stcp->status != STAGEALLOC) ||
      (req_type == STAGEPUT &&
       stcp->status != STAGEOUT && stcp->status != (STAGEOUT|PUT_FAILED))) {
    sendrep (rpfd, MSG_ERR, STG22);
    return (USERR);
  }
  if (stcp->status == STAGEOUT || stcp->status == STAGEALLOC) {
    if (rfio_stat (stcp->ipath, &st) == 0)
      stcp->actual_size = st.st_size;
    updfreespace (stcp->poolname, stcp->ipath,
                  stcp->size*1024*1024 - (int)stcp->actual_size);
  }
  if (req_type == STAGEPUT)
    stcp->status = STAGEPUT;
  else if (req_type == STAGEUPDC)
    stcp->status |= STAGED;
  stcp->a_time = time (0);
  *subreqid = stcp->reqid;

  stgdb_upd_stgcat(&dbfd,stcp);

  return (0);
}

#if defined(ultrix) || (defined(sun) && !defined(SOLARIS))
void wait4child()
{
  int pid;
  union wait status;

  while ((pid = wait3 (&status, WNOHANG, (struct rusage *) 0)) > 0)
    checkovlstatus (pid, status.w_status);
}
#else
void wait4child()
{
  int pid;
  int status;

#if defined(_IBMR2) || defined(SOLARIS) || defined(IRIX5) || (defined(__osf__) && defined(__alpha)) || defined(linux)
  while ((pid = waitpid (-1, &status, WNOHANG)) > 0)
    checkovlstatus (pid, status);
#else
  pid = wait (&status);
  checkovlstatus (pid, status);
#if _IBMESA
  signal (SIGCHLD, wait4child);
#else
  signal (SIGCLD, wait4child);
#endif
#endif
}
#endif

void stgdaemon_usage() {
  printf("\nUsage : stgdaemon [options]\n"
         "  where options can be\n"
         "  -f      Foreground\n"
         "  -h      This help\n"
         "  -v      Print version\n"
         "\n"
         );
}
