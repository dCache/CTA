/*
 * $Id: Cpool.c,v 1.7 1999/10/08 16:58:23 jdurand Exp $
 */

#include <Cpool_api.h>
#ifndef _WIN32
/* All that stuff is for CTHREAD_MULTI_PROCESS support on */
/* Unix-like systems.                                     */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <serrno.h>
#endif /* _WIN32 */
#ifdef _AIX
/* Otherwise cc will not know about fd_set on */
/* old aix versions.                          */
#include <sys/select.h>
#endif
#ifdef DEBUG
#ifndef CPOOL_DEBUG
#define CPOOL_DEBUG
#endif
#endif /* DEBUG */

#ifdef CPOOL_DEBUG
#include <log.h>
#endif

int Cpool_debug = 0;

/* ------------------------------------ */
/* For the what command                 */
/* ------------------------------------ */
static char sccsid[] = "@(#)$RCSfile: Cpool.c,v $ $Revision: 1.7 $ $Date: 1999/10/08 16:58:23 $ CERN IT-PDP/DM Jean-Damien Durand";

/* ------------------------------------ */
/* Mutex static variables a-la-Cthread  */
/* ------------------------------------ */
#ifdef CPOOL_DEBUG
/* For mutex on CPOOL_DEBUG printouts */
static int lock_cpool_debug;
#endif
/* For child and parent protocol in a CTHREAD_TRUE_THREAD */
/* environment                                            */
static int lock_child;
static int lock_parent;

#ifndef _WIN32
/* ------------------------------------ */
/* Undefinition of memory wrappers      */
/* ------------------------------------ */
/* We don't want to recursively call   */
/* ourselves...                        */
#undef calloc
#undef malloc
#undef realloc
#undef free
#endif /* _WIN32 */

/* ------------------------------------ */
/* Linked list of pools                 */
/* ------------------------------------ */
struct Cpool_t {
  int                     poolnb;   /* Pool number                        */
  int                     nbelem;   /* Nb of elems                        */
  int                    *cid;      /* Elements Cthread ID                */
  int                     forceid;  /* Index forcing the assignment to    */
#ifndef _WIN32
  /* If CTHREAD_MULTI_PROCESS */
  int                    *writefd;  /* Parent->Child (only on unix)       */
  int                    *readfd;   /* Child->Parent (only on unix)       */
#endif
  /* If CTHREAD_TRUE_THREAD */
  int                    *state;    /* Elements current status (0=READY)  */
                                    /*                        (1=RUNNING) */
                                    /*                       (-1=STARTED) */
  int                     flag;     /* Parent flag (-1=WAITING)           */
#if defined(__STDC__)
  void                  *(**start)(void *); /* Start routines             */
#else
  void                  *(**start)();       /* Start routines             */
#endif
  void                  **arg;

  struct Cpool_t         *next;     /* Next pool                          */
};

#ifndef _WIN32
static struct Cpool_t Cpool = { 0, 0, NULL, -1, NULL, NULL, NULL, 0, NULL, NULL, NULL};
#else
static struct Cpool_t Cpool = { 0, 0, NULL, -1, NULL, 0, NULL, NULL, NULL};
#endif

#ifndef _WIN32
/* ------------------------------------ */
/* Linked list of memory allocation     */
/* ------------------------------------ */
struct Cmalloc_t {
  void *start;
  void *end;
  struct Cmalloc_t *next;
};
static struct Cmalloc_t Cmalloc = { NULL, NULL, NULL};
#endif /* _WIN32 */

#ifndef _WIN32
/* ------------------------------------ */
/* Non-thread environment pipe protocol */
/* ------------------------------------ */
static int  tubes[5];
#endif /* _WIN32 */

#ifndef _WIN32
/* ------------------------------------ */
/* Typedefs                             */
/* ------------------------------------ */
#if defined(__STDC__)
typedef void    Sigfunc(int);
#else
typedef void    Sigfunc();
#endif

#endif /* _WIN32 */
#if (defined(hpux) || defined(_INCLUDE_HPUX_SOURCE) || defined(_HPUX_SOURCE))
/* hpux wants int instead of fd_set */
typedef int _cpool_fd_set;
/* typedef fd_set _cpool_fd_set; */
#else
typedef fd_set _cpool_fd_set;
#endif /* hpux || _INCLUDE_HPUX_SOURCE || _HPUX_SOURCE */

/* ------------------------------------ */
/* Prototypes                           */
/* ------------------------------------ */
#if defined(__STDC__)
void     *_Cpool_starter(void *);
#ifndef _WIN32
size_t   _Cpool_writen(int, void *, size_t);
size_t   _Cpool_readn(int, void *, size_t);
size_t   _Cpool_writen_timeout(int, void *, size_t, int);
size_t   _Cpool_readn_timeout(int, void *, size_t, int);
void     _Cpool_alarm(int);
Sigfunc *_Cpool_signal(int, Sigfunc *);
#endif /* _WIN32 */
int      _Cpool_self();
#else /* __STDC__ */
void    *_Cpool_starter();
#ifndef _WIN32
size_t   _Cpool_writen();
size_t   _Cpool_readn();
size_t   _Cpool_writen_timeout();
size_t   _Cpool_readn_timeout();
void     _Cpool_alarm();
Sigfunc *_Cpool_signal();
#endif /* _WIN32 */
int      _Cpool_self();
#endif /* __STDC__ */

#ifndef _WIN32
/* ------------------------------------ */
/* Constants used in the fork() model   */
/* ------------------------------------ */
#ifndef _CPOOL_STARTER_TIMEOUT
#define _CPOOL_STARTER_TIMEOUT 10
#endif
#ifndef _CPOOL_SLEEP_FLAG
#define _CPOOL_SLEEP_FLAG       -1
#endif
static void *_cpool_sleep_flag = (void *) _CPOOL_SLEEP_FLAG;
#endif /* _WIN32 */

/* ============================================ */
/* Routine  : Cpool_create                      */
/* Arguments: Number of requested processes     */
/*            &Number of created processses     */
/* -------------------------------------------- */
/* Output   : >= 0 (PoolNumber) -1 (ERROR)      */
/* -------------------------------------------- */
/* History:                                     */
/* 17-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
CTHREAD_DECL Cpool_create(nbreq,nbget)
     int nbreq;
     int *nbget;
{
  struct Cpool_t         *current          = &Cpool;
  struct Cpool_t         *previous         = &Cpool;
  int                     i;
  int                     j;
  int                     k;
  int                     poolnb, nbcreated;
#ifndef _WIN32
  /* If CTHREAD_MULTI_PROCESS */
  int                     p_to_c[2];
  int                     c_to_p[2];
  int                    *to_close = NULL;
  int                     pid = 0;
#endif
  /* If CTHREAD_TRUE_THREAD */
  void                   *cpool_arg = NULL;

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_create(%d,0x%lx)\n",
        _Cpool_self(),_Cthread_self(),nbreq,(unsigned long) nbget);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  if (nbreq <= 0) {
    serrno = EINVAL;
    return(-1);
  }

  /* We makes sure that Cthread pakage is initalized */
  Cthread_init();
  
  /* We search the last available pool */
  if (Cthread_mutex_lock(&Cpool) != 0) {
    return(-1);
  }
  previous = &Cpool;
  if (current->next != NULL) {
    while (current->next != NULL) {
      previous = current;
      current  = current->next;
    }
    previous = current;
    poolnb = current->poolnb;
  } else {
    /* Make sure that ++poolnb will return a number >= 0 */
    poolnb = -1;
  }

#ifndef _WIN32
  if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
    /* To let know the child which parent is the creator */
    /* and to die in case of non-parent existence        */
    /* (on unix only, this is to prevent zombies)        */
    pid = getpid();
    
    /* We prepare the list of fd's to close */
    if ((to_close = malloc(2 * nbreq * sizeof(int))) == NULL) {
      Cthread_mutex_unlock(&Cpool);
      serrno = SEINTERNAL;
      return(-1);
    }
  }
#endif
  
  /* We create a new pool element */
  if ((current = malloc(sizeof(struct Cpool_t))) == NULL) {
    Cthread_mutex_unlock(&Cpool);
    serrno = SEINTERNAL;
    return(-1);
  }

  /* Allocation for Cthread ID's */
  if ((current->cid = malloc(nbreq * sizeof(int))) == NULL) {
#ifndef _WIN32
    if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
      free(to_close);
    }
#endif
    free(current);
    Cthread_mutex_unlock(&Cpool);
    serrno = SEINTERNAL;
    return(-1);
  }
#ifndef _WIN32
  if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
    /* Allocation for writing pipes (unix only) */
    if ((current->writefd = malloc(nbreq * sizeof(int))) == NULL) {
      free(to_close);
      free(current->cid);
      free(current);
      Cthread_mutex_unlock(&Cpool);
      serrno = SEINTERNAL;
      return(-1);
    }
    /* Allocation for reading pipes (unix only) */
    if ((current->readfd = malloc(nbreq * sizeof(int))) == NULL) {
      free(to_close);
      free(current->writefd);
      free(current->cid);
      free(current);
      Cthread_mutex_unlock(&Cpool);
      serrno = SEINTERNAL;
      return(-1);
    }
  } else {
#endif /* _WIN32 */
    /* Allocation for threads status */
    if ((current->state = malloc(nbreq * sizeof(int))) == NULL) {
      free(current->cid);
      free(current);
      Cthread_mutex_unlock(&Cpool);
      serrno = SEINTERNAL;
      return(-1);
    }
    /* Allocation for threads starting routines */
    if ((current->start = malloc(nbreq * sizeof(void *))) == NULL) {
      free(current->state);
      free(current->cid);
      free(current);
      Cthread_mutex_unlock(&Cpool);
      serrno = SEINTERNAL;
      return(-1);
    }
    /* Allocation for threads arguments addresses */
    if ((current->arg = malloc(nbreq * sizeof(void *))) == NULL) {
      free(current->start);
      free(current->state);
      free(current->cid);
      free(current);
      Cthread_mutex_unlock(&Cpool);
      serrno = SEINTERNAL;
      return(-1);
    }
#ifndef _WIN32
  }
#endif
  current->next = NULL;
  if (Cthread_environment() != CTHREAD_MULTI_PROCESS) {
    /* We tell that there is no dispatch at this time */
    current->flag = 0;
  }
    
  nbcreated = j = k = 0;
  /* We create the pools */
  for (i = 0; i < nbreq; i++) {
#ifndef _WIN32
    if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
      /* The pipes */
      if (pipe(p_to_c))
        continue;
      if (pipe(c_to_p)) {
        close(p_to_c[0]);
        close(p_to_c[1]);
        continue;
      }
      /* We prepare the argument */
      tubes[0] = p_to_c[0]; /* For reading */
      tubes[1] = p_to_c[1]; /* For writing */
      tubes[2] = c_to_p[0]; /* For reading */
      tubes[3] = c_to_p[1]; /* For writing */
      tubes[4] = pid;
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_create : pipes give [%d,%d,%d,%d]\n",
            _Cpool_self(),_Cthread_self(),tubes[0],tubes[1],tubes[2],tubes[3]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      /* Non thread environment : everything will be done with pipes */
      current->cid[i] = Cthread_create(_Cpool_starter,NULL);
    } else {
#endif /* _WIN32 */
      /* Thread environment : everything will be done with shared mem. and cond. vars. */
      /* We send as argument the address of the current pool structure as well as the  */
      /* index of this thread in this structure                                        */
      if ((cpool_arg = malloc(sizeof(struct Cpool_t *) + sizeof(int))) != NULL) {
        char *dummy = cpool_arg;
        memcpy(dummy,&current,sizeof(struct Cpool_t *));
        dummy += sizeof(struct Cpool_t *);
        memcpy(dummy,&i,sizeof(int));
        /* And we don't forget to initialize the state of this thread to be zero... */
        current->state[i] = -1;
        if ((current->cid[i] = Cthread_create(_Cpool_starter,cpool_arg)) < 0) {
          free(cpool_arg);
        }
        /* And we wait for this thread to have really started */
        Cthread_mutex_lock(&(current->state[i]));
        while (current->state[i] != 0) {
          Cthread_cond_wait(&(current->state[i]));
        }
        Cthread_mutex_unlock(&(current->state[i]));
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_create : Thread No %d started\n",
              _Cpool_self(),_Cthread_self(),i);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      } else {
        serrno = SEINTERNAL;
        current->cid[i] = -1;
      }
#ifndef _WIN32
    }
#endif /* _WIN32 */
    if (current->cid[i] < 0) {
      /* Error at cthread_create : we clean fd created an try the */
      /* next iteration                                           */
#ifndef _WIN32
      if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
        for (j=0; j <= 3; j++) {
          close(tubes[j]);
        }
      }
#endif /* _WIN32 */
    } else {
#ifndef _WIN32
      if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
        current->writefd[i] = p_to_c[1];
        current->readfd[i] = c_to_p[0];
        to_close[k++] = p_to_c[0];
        to_close[k++] = c_to_p[1];
      }
#endif /* _WIN32 */
      /* We count number of created processes */
      ++nbcreated;
    }
  }

#ifndef _WIN32
  if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
    for (j=0; j < k; j++) {
      close(to_close[j]);
    }
    free(to_close);
  }
#endif /* _WIN32 */

  /* We update the return value */
  if (nbget != NULL)
    *nbget = nbcreated;

  /* We initialize the element structure */
  current->poolnb = ++poolnb;                  /* Pool number */
  current->nbelem = nbcreated;                 /* Number of threads in it */
  current->forceid = -1;                       /* Force thread index assignment */
  previous->next = current;                    /* Next pool */
  Cthread_mutex_unlock(&Cpool);

  return(current->poolnb);
}

/* ============================================ */
/* Routine  : _Cpool_starter                    */
/* Arguments: address of arguments              */
/*            (used only within threads)        */
/* -------------------------------------------- */
/* Output   : dummy (not used)                  */
/* -------------------------------------------- */
/* History:                                     */
/* 14-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
/* Nota: the prototype of this function is like */
/* this just to agree with the prototype of     */
/* Cthread_create and al.                       */
/* ============================================ */
void *_Cpool_starter(arg)
     void *arg;
{
#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter(0x%lx)\n",
        _Cpool_self(),_Cthread_self(),(unsigned long) arg);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

#ifndef _WIN32
  if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
    /*----------------------- */
    /* Non-Thread only        */
    /*----------------------- */
    int         p_to_c[2];
    int         c_to_p[2];
    int         ppid;
    void       *thisarg;
    int         ready = 1;
    size_t      thislength;
#if defined(__STDC__)
    void     *(*routine)(void *);
#else
    void     *(*routine)();
#endif
    int         sleep_flag;
    
    /* We get the argument */
    p_to_c[0] = tubes[0];
    p_to_c[1] = tubes[1];
    c_to_p[0] = tubes[2];
    c_to_p[1] = tubes[3];
    ppid      = tubes[4];
    
  /* We close unnecessary fd's */
    
    close(p_to_c[1]);
    close(c_to_p[0]);
    
    while (1) {
      /* We send a flag in c_to_p[1] to say that we are ready */
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : Write ready flag with timeout\n",
            _Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    _cpool_multi_process_again:
      sleep_flag = 0;
      while (1) {
        if (_Cpool_writen_timeout(c_to_p[1],&ready,sizeof(int),_CPOOL_STARTER_TIMEOUT) != 
            sizeof(int)) {
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : error No %d\n",
                _Cpool_self(),_Cthread_self(),errno);
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          if (serrno == SETIMEDOUT) {
            /* Timeout */
            /* We check that the parent is still there */
#ifdef CPOOL_DEBUG
            /* Cthread_mutex_lock(&lock_cpool_debug); */
            if (Cpool_debug != 0)
              log(LOG_INFO,"[Cpool  [%2d][%2d]] timeout : kill(%d,0)\n",_Cpool_self(),_Cthread_self(),ppid);
            /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
            if (kill(ppid,0))
              /* Nope... */
              return(NULL);
            continue;
          } else {
            /* Error */
            return(NULL);
          }
        } else {
          break;
        }
      }
      /* And now we wait for something in the read tube */
      /* We read the address of the start routine, unless it is _CPOOL_SLEEP_FLAG */
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : Read startroutine address with timeout on fd = %d\n",
            _Cpool_self(),_Cthread_self(),p_to_c[0]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      while (1) {
        if (_Cpool_readn_timeout(p_to_c[0],&routine,sizeof(void *),_CPOOL_STARTER_TIMEOUT) !=
            sizeof(void *)) {
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : error No %d\n",_Cpool_self(),_Cthread_self(),errno);
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          if (serrno == SETIMEDOUT) {
            /* Timeout */
            /* We check that the parent is still there */
#ifdef CPOOL_DEBUG
            /* Cthread_mutex_lock(&lock_cpool_debug); */
            if (Cpool_debug != 0)
              log(LOG_INFO,"[Cpool  [%2d][%2d]] timeout : kill(%d,0)\n",_Cpool_self(),_Cthread_self(),ppid);
            /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
            if (kill(ppid,0))
              /* Nope... */
              return(NULL);
            continue;
          } else {
            /* Error */
            return(NULL);
          }
        } else {
#if defined(__STDC__)
          if (routine == (void *(*)(void *)) _CPOOL_SLEEP_FLAG) {
            /* We just had a hit from Cpool_next_index, that sent us the sleep flag */
            sleep_flag = 1;
          }
#else
          if (routine == (void *(*)()) _CPOOL_SLEEP_FLAG) {
            /* We just had a hit from Cpool_next_index, that sent us the sleep flag */
            sleep_flag = 1;
          }
#endif
          break;
        }
      }

      if (sleep_flag == 1) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] -> Sleep flag received. Looping again\n",_Cpool_self(),_Cthread_self());
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        goto _cpool_multi_process_again;
      }

#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] -> startroutine is at 0x%lx\n",_Cpool_self(),_Cthread_self(),(unsigned long) routine);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      /* In a non-thread environment we read the argument length and content */
      if (_Cpool_readn_timeout(p_to_c[0],&thislength,sizeof(size_t),_CPOOL_STARTER_TIMEOUT) != 
          sizeof(size_t)) 
        return(NULL);
      if (thislength > 0) {
        if ((thisarg = malloc(thislength)) == NULL)
          exit(EXIT_FAILURE);
        if (_Cpool_readn_timeout(p_to_c[0],thisarg,thislength,_CPOOL_STARTER_TIMEOUT) != 
            thislength)
          return(NULL);
      } else {
        thisarg = NULL;
      }
      /* We execute the routine */
      routine(thisarg);
      /* We free memory if needed */
      if (thisarg != NULL)
        free(thisarg);
    }
  } else {
#endif /* _WIN32 */
    /*----------------------- */
    /* Thread only            */
    /*----------------------- */
    struct Cpool_t *current;
    int             index;
    char           *dummy;
#if defined(__STDC__)
    void          *(*start)(void *);
#else
    void          *(*start)();
#endif
    void           *startarg;

    /* We receive in the argument the address of the pool structure */
    /* And the index of our thread in this structure                */
    current = (struct Cpool_t *) * (struct Cpool_t **) arg;
    dummy = arg;
    dummy  += sizeof(struct Cpool_t *);
    index   = (int) * (int *) dummy;
    /* And we free this memory */
    free(arg);

#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : Telling that we (index %d) are starting\n",
          _Cpool_self(),_Cthread_self(),index);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We tell the Cpool_create that we are started */
    Cthread_mutex_lock(&(current->state[index]));
    current->state[index] = 0;
    Cthread_cond_signal(&(current->state[index]));
    Cthread_mutex_unlock(&(current->state[index]));
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : We (index %d) signalled it\n",
          _Cpool_self(),_Cthread_self(),index);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We loop indefinitely */
    while (1) {
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : lock on &lock_parent\n",
            _Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We check current->flag */
      if (Cthread_mutex_lock(&lock_parent) != 0) {
        /* Error */
        return(NULL);
      }
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : current->flag = %d\n",
            _Cpool_self(),_Cthread_self(),current->flag);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      if (current->flag == -1) {
        /* Either the parent is waiting for any of us, or for us only */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : Setting current->flag to our index %d and signaling on &lock_parent\n",
              _Cpool_self(),_Cthread_self(),index);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        if (current->forceid == -1 || current->forceid == index) {
          /* The parent is waiting for: */
          /* -1   : any of us           */
          /* index: us, exactly         */
          /* We tell what is our index  */
          current->flag = index;
          /* And we signal              */
          if (Cthread_cond_signal(&lock_parent)) {
            /* Error */
            Cthread_mutex_unlock(&lock_parent);
            return(NULL);
          }
        }
      }
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : un-lock on &lock_parent\n",
            _Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      Cthread_mutex_unlock(&lock_parent);
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : lock on lock_child\n",_Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We put a lock on the state index */
      if (Cthread_mutex_lock(&lock_child) != 0) {
        return(NULL);
      }

      /* We wait until there is something new      */
      /* The lock on "current->state[index]" is    */
      /* then released                             */
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : waiting condition on &lock_child, with predicate: current->state[%d] == 0 (current value is: %d)\n",
            _Cpool_self(),_Cthread_self(),index,current->state[index]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      while ( current->state[index] == 0 ) {
        if (Cthread_cond_wait(&lock_child) != 0) {
          Cthread_mutex_unlock(&lock_child);
          return(NULL);
        }
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : out of the condition wait, current->state[%d] = %d\n",
              _Cpool_self(),_Cthread_self(),index,current->state[index]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      }
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : out of the condition loop, current->state[%d] = %d\n",
            _Cpool_self(),_Cthread_self(),index,current->state[index]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : un-lock on lock_child\n",_Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We release the lock on the state index    */
      Cthread_mutex_unlock(&lock_child);
      
      /* We are waked up: the routine and its arguments */
      /* address are put in current->start[index] and   */
      /* current->arg[index]                            */
#if defined(__STDC__)
      start    = (void *(*)(void *)) current->start[index];
#else
      start    = (void *(*)())       current->start[index];
#endif
      startarg = (void *)            current->arg[index];
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : Execute 0x%lx(0x%lx)\n",
            _Cpool_self(),_Cthread_self(),(unsigned long) start,(unsigned long) (startarg != NULL ? startarg : 0));
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We execute the routine                         */
      (*start)(startarg);
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : lock on lock_child\n",_Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We put a lock on the state index */
      if (Cthread_mutex_lock(&lock_child) != 0) {
        return(NULL);
      }
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : current->state[%d] = %d\n",
            _Cpool_self(),_Cthread_self(),index,current->state[index]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We flag us in the non-running state */
      current->state[index] = 0;
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_starter : un-lock on lock_child\n",_Cpool_self(),_Cthread_self());
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We release the lock on the state index */
      Cthread_mutex_unlock(&lock_child);
      
    }
#ifndef _WIN32
  }
#endif
}

#ifndef _WIN32
/* ============================================ */
/* Routine  : _Cpool_writen_timeout             */
/* Arguments: file des., pointer, size, timeout */
/* -------------------------------------------- */
/* Output   : Number of bytes writen            */
/* -------------------------------------------- */
/* History:                                     */
/* 17-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
size_t _Cpool_writen_timeout(fd,vptr,n,timeout)
     int fd;
     void *vptr;
     size_t n;
     int timeout;
{
  size_t		nleft;
  size_t		nwritten;
  char         *ptr;
  
  /* We use the signal alarm */
  Sigfunc  *sigfunc;
  
#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_writen_timeout(%d,0x%lx,0x%x,%d)\n",
        _Cpool_self(),_Cthread_self(),fd,(unsigned long) vptr, (unsigned int) n, timeout);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* Get previous handler */
  if ((sigfunc = _Cpool_signal(SIGALRM, _Cpool_alarm)) == SIG_ERR) {
    serrno = SEINTERNAL;
    return(0);
  }
  
  /* In any case we catch trap SIGPIPE */
  _Cpool_signal(SIGPIPE, SIG_IGN);
  
  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    alarm(timeout);
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (errno == EINTR) {
        errno = ETIMEDOUT;
        serrno = SETIMEDOUT;
        goto doreturn;
      } else {
        goto doreturn;
      }
    }
    
    nleft -= nwritten;
    ptr   += nwritten;
  }
 doreturn:
  /* Disable alarm            */
  alarm(0);
  /* Restore previous handler */
  _Cpool_signal(SIGALRM, sigfunc);
  if (nleft == -1) {
    /* Nothing done... */
    nleft = n;
  }
  return(n - nleft);
}

/* ============================================ */
/* Routine  : _Cpool_writen                     */
/* Arguments: file des., pointer, size          */
/* -------------------------------------------- */
/* Output   : Number of bytes writen            */
/* -------------------------------------------- */
/* History:                                     */
/* 14-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
size_t _Cpool_writen(fd,vptr,n)
     int fd;
     void *vptr;
     size_t n;
{
  size_t		nleft;
  size_t		nwritten;
  char         *ptr;
  Sigfunc          *handler;

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_writen(%d,0x%lx,0x%x)\n",
        _Cpool_self(),_Cthread_self(),fd,(unsigned long) vptr, (unsigned int) n);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* In any case we catch trap SIGPIPE */
  handler = _Cpool_signal(SIGPIPE, SIG_IGN);

  ptr = vptr;
  nleft = n;
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (errno == EINTR) {
        nwritten = 0;		/* and call write() again */
      } else {
        _Cpool_signal(SIGPIPE, handler);
        return(n - nleft);
      }
    }
    nleft -= nwritten;
    ptr   += nwritten;
  }
  _Cpool_signal(SIGPIPE, handler);
  return(n - nleft);
}

/* ============================================ */
/* Routine  : _Cpool_readn                      */
/* Arguments: file des., pointer, size          */
/* -------------------------------------------- */
/* Output   : Number of bytes read              */
/* -------------------------------------------- */
/* History:                                     */
/* 14-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
size_t _Cpool_readn(fd,vptr,n)
     int fd;
     void *vptr;
     size_t n;
{
  size_t	nleft;
  size_t	nread;
  char	*ptr;
  Sigfunc          *handler;
  
#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_readn(%d,0x%lx,0x%x)\n",
        _Cpool_self(),_Cthread_self(),fd,(unsigned long) vptr, (unsigned int) n);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* In any case we catch trap SIGPIPE */
  if ((handler = _Cpool_signal(SIGPIPE, SIG_IGN)) == SIG_ERR) {
    return(0);
  }
  _Cpool_signal(SIGPIPE, SIG_IGN);

  ptr = vptr;
  nleft = n;
  nread = 0;
  while (nleft > 0) {
    if ( (nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        nread = 0;		/* and call read() again */
      } else {
        _Cpool_signal(SIGPIPE, handler);
        return(n - nleft);
      }
    } else if (nread == 0)
      break;				/* EOF */
    nleft -= nread;
    ptr   += nread;
  }
  _Cpool_signal(SIGPIPE, handler);
  return(n - nleft);		/* return >= 0 */
}

/* ============================================ */
/* Routine  : _Cpool_readn_timeout              */
/* Arguments: file des., pointer, size, timeout */
/* -------------------------------------------- */
/* Output   : Number of bytes read              */
/* -------------------------------------------- */
/* History:                                     */
/* 14-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
size_t _Cpool_readn_timeout(fd,vptr,n,timeout)
     int fd;
     void *vptr;
     size_t n;
     int timeout;
{
  size_t	nleft;
  size_t	nread;
  char	*ptr;
  
  /* We use the signal alarm */
  Sigfunc  *sigfunc;
  
#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_readn_timeout(%d,0x%lx,0x%x,%d)\n",
        _Cpool_self(),_Cthread_self(),fd,(unsigned long) vptr, (unsigned int) n, timeout);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* Get previous handler */
  if ((sigfunc = _Cpool_signal(SIGALRM, _Cpool_alarm)) == SIG_ERR) {
    serrno = SEINTERNAL;
    return(0);
  }

  /* In any case we catch trap SIGPIPE */
  _Cpool_signal(SIGPIPE, SIG_IGN);
  
  ptr = vptr;
  nleft = n;
  nread = 0;
  while (nleft > 0) {
    alarm(timeout);
    if ( (nread = read(fd, ptr, nleft)) < 0) {
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In _Cpool_readn_timeout : errno = %d [EINTR=%d]\n",
            _Cpool_self(),_Cthread_self(),errno,EINTR);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (errno == EINTR) {
        errno = ETIMEDOUT;
        serrno = SETIMEDOUT;
        goto doreturn;
      } else {
        goto doreturn;
      }
    } else if (nread == 0)
      break;				/* EOF */
    nleft -= nread;
    ptr   += nread;
  }
 doreturn:
  /* Disable alarm            */
  alarm(0);
  /* Restore previous handler */
  _Cpool_signal(SIGALRM, sigfunc);
  if (nleft == -1) {
    /* Nothing done... */
    nleft = n;
  }
  return(n - nleft);		/* return >= 0 */
}

/* ============================================ */
/* Routine  : _Cpool_alarm                      */
/* Arguments: signal to trap                    */
/* -------------------------------------------- */
/* Output   :                                   */
/* -------------------------------------------- */
/* History:                                     */
/* 17-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
void _Cpool_alarm(signo)
     int signo;
{
#ifdef CPOOL_DEBUG
  /* Better not to do any mutex/cond etc... in a signal handler... */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool[...[...]]] ### SIGALRM catched\n");
#endif
  /* We just trap the signal and return */
  return;
}

/* ============================================ */
/* Routine  : _Cpool_signal                     */
/* Arguments: signal to trap, signal routine    */
/* -------------------------------------------- */
/* Output   : status (SIG_ERR if error)         */
/* -------------------------------------------- */
/* History:                                     */
/* 11-MAY-1999       First implementation       */
/*                   [with R.W.Stevens example] */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
Sigfunc *_Cpool_signal(signo,func)
     int signo;
     Sigfunc *func;
{
  struct sigaction	act, oact;
  
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
  } else {
#ifdef	SA_RESTART
    act.sa_flags |= SA_RESTART;		/* SVR4, 44BSD */
#endif
  }
  if (sigaction(signo, &act, &oact) < 0)
    return(SIG_ERR);
  return(oact.sa_handler);
}

/* ============================================ */
/* Set of wrappers around memory allocation     */
/* to allow passing of arguments between        */
/* forked child in case of pools processes      */
/* assignment.                                  */
/* This has also the advantage of beeing a      */
/* run-time checker for memory allocation       */
/* and use.                                     */
/* ============================================ */

/* ============================================ */
/* Routine  : Cpool_calloc                      */
/* Arguments: nb of members, size of each       */
/* -------------------------------------------- */
/* Output   : address allocated                 */
/* -------------------------------------------- */
/* History:                                     */
/* 11-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
/* Notes:                                       */
/* This routine is a wrapper around calloc      */
/* ============================================ */
void *Cpool_calloc(file,line,nmemb,size)
     char *file;
     int line;
     size_t nmemb;
     size_t size;
{
  struct Cmalloc_t *current  = &Cmalloc;
  struct Cmalloc_t *previous = &Cmalloc;
  void             *result;
  char             *dummy;

  if (Cthread_environment() != CTHREAD_MULTI_PROCESS) {
    return(calloc(nmemb,size));
  }

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_calloc(0x%x,0x%x) called at %s:%d\n",
        _Cpool_self(),_Cthread_self(),(unsigned int) nmemb, (unsigned int) size, file, line);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* We search the last element */
  while (current->next != NULL) {
    previous = current;
    current = current->next;
  }

  /* We create an element */
  if ((current = malloc(sizeof(struct Cmalloc_t))) == NULL) {
    return(NULL);
  }
  /* We create the requested memory */
  if ((result = calloc(nmemb,size)) == NULL) {
    free(current);
    return(NULL);
  }
  /* We fill the new element */
  current->start = result;
  /* Unfortunately "current->end += (nmemb * size)" is not ANSI-C and */
  /* will be rejected by a lot of compilers (exception I know is gcc) */
  dummy  = (char *) result;
  dummy += (nmemb * size);
  current->end   = dummy;
  current->next  = NULL;

  /* We update internal pointer */
  previous->next = current;

  /* We return the result of _calloc */
  return(result);
}

/* ============================================ */
/* Routine  : Cpool_malloc                      */
/* Arguments: size to allocate                  */
/* -------------------------------------------- */
/* Output   : address allocated                 */
/* -------------------------------------------- */
/* History:                                     */
/* 11-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
/* Notes:                                       */
/* This routine is a wrapper around malloc      */
/* ============================================ */
void *Cpool_malloc(file,line,size)
     char *file;
     int line;
     size_t size;
{
  struct Cmalloc_t *current  = &Cmalloc;
  struct Cmalloc_t *previous = &Cmalloc;
  void             *result;
  char             *dummy;

  if (Cthread_environment() != CTHREAD_MULTI_PROCESS) {
    return(malloc(size));
  }

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_malloc(%d) called at %s:%d\n",
        _Cpool_self(),_Cthread_self(),(int) size, file, line);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* We search the last element */
  while (current->next != NULL) {
    previous = current;
    current = current->next;
  }
  /* We create an element */
  if ((current = malloc(sizeof(struct Cmalloc_t))) == NULL) {
    return(NULL);
  }
  /* We create the requested memory */
  if ((result = malloc(size)) == NULL) {
    free(current);
    return(NULL);
  }
  /* We fill the new element */
  current->start   = result;
  /* Unfortunately "current->end += (nmemb * size)" is not ANSI-C and */
  /* will be rejected by a lot of compilers (exception I know is gcc) */
  dummy            = (char *) result;
  dummy           += (size-1);
  current->end     = dummy;
  current->next    = NULL;

  /* We update internal pointer */
  previous->next = current;

  /* We return the result of _calloc */
  return(result);
}

/* ============================================ */
/* Routine  : Cpool_free                        */
/* Arguments: address to free                   */
/* -------------------------------------------- */
/* Output   :                                   */
/* -------------------------------------------- */
/* History:                                     */
/* 11-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
/* Notes:                                       */
/* This routine is a wrapper around free        */
/* ============================================ */
void Cpool_free(file,line,ptr)
     char *file;
     int line;
     void *ptr;
{
  struct Cmalloc_t *current  = &Cmalloc;
  struct Cmalloc_t *previous = &Cmalloc;
  int               n = 1;
  /* We test to see if the user wants to */
  /* to free something really allocated  */

  if (Cthread_environment() != CTHREAD_MULTI_PROCESS) {
    free(ptr);
    return;
  }

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_free(0x%lx) called at %s:%d\n",
        _Cpool_self(),_Cthread_self(),(unsigned long) ptr, file, line);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  while (current->next != NULL) {
    previous = current;
    current = current->next;

    if (current->start == ptr) {
      n = 0;
      break;
    }
  }

  if (n) {
    /* ERROR */
    errno = EINVAL;
    return;
  }

  free(ptr);

  /* We update pointers */
  previous->next = current->next;
  free(current);
  return;
}

/* ============================================ */
/* Routine  : Cpool_realloc                     */
/* Arguments: address to realloc, its size      */
/* -------------------------------------------- */
/* Output   :                                   */
/* -------------------------------------------- */
/* History:                                     */
/* 11-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
/* Notes:                                       */
/* This routine is a wrapper around realloc     */
/* ============================================ */
void *Cpool_realloc(file,line,ptr,size)
     char *file;
     int line;
     void *ptr;
     size_t size;
{
  struct Cmalloc_t *current  = &Cmalloc;
  struct Cmalloc_t *previous = &Cmalloc;
  void             *result;
  int               n = 1;
  char             *dummy;

  if (Cthread_environment() != CTHREAD_MULTI_PROCESS) {
    return(realloc(ptr,size));
  }

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_realloc(0x%lx,%d) called at %s:%d\n",
        _Cpool_self(),_Cthread_self(),(unsigned long) ptr, (int) size, file, line);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

  /* We search the correct element */
  while (current->next != NULL) {
    previous = current;
    current = current->next;
    if (current->start == ptr) {
      n = 0;
      break;
    }
  }

  if (n) {
    /* Error */
    errno = EINVAL;
    return(NULL);
  }

  if ((result = realloc(ptr,size)) == NULL) {
    return(NULL);
  }

  /* We update the pointer */
  current->start = result;
  /* Unfortunately "current->end += (nmemb * size)" is not ANSI-C and */
  /* will be rejected by a lot of compilers (exception I know is gcc) */
  dummy          = result;
  dummy         += size;
  current->end   = dummy;

  /* We return the result */
  return(result);
}
#endif /* _WIN32 */

/* ============================================ */
/* Routine  : Cpool_assign                      */
/* Arguments: pool number                       */
/*            starting routine                  */
/*            arguments address                 */
/*            timeout                           */
/* -------------------------------------------- */
/* Output   : 0 (OK) -1 (ERROR)                 */
/* -------------------------------------------- */
/* History:                                     */
/* 17-MAY-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
CTHREAD_DECL Cpool_assign(poolnb,startroutine,arg,timeout)
     int poolnb;
#if defined(__STDC__)
     void *(*startroutine)(void *);
#else
     void *(*startroutine)();
#endif
     void *arg;
     int timeout;
{

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign(%d,0x%lx,0x%lx,%d)\n",
        _Cpool_self(),_Cthread_self(),poolnb, (unsigned long) startroutine, (unsigned long) arg, timeout);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
#ifndef _WIN32
  /* THIS ROUTINE IS EXPLICITELY SPLITTED IN TWO PARTS    */
  /* - The _NOCTHREAD one (using pipes)                   */
  /* - The _CTHREAD one (using shared mem. and cond. var. */
  if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
    /*----------------------- */
    /* Non-Thread only        */
    /*----------------------- */
    struct Cpool_t *current = &Cpool;
    int    found;
    size_t length_vars;
    struct timeval          tv;
    int                     maxfd;
    int                     i;
    fd_set                  readlist;
    int                     ready;
    
    if (poolnb < 0) {
      serrno = EINVAL;
      return(-1);
    }
    
    /* In case of a non-thread environment we check that */
    /* the argument is in a windows allocated memory or  */
    /* not.                                              */
    found = 0;
    if (arg != NULL) {
      struct Cmalloc_t *current  = &Cmalloc;
      struct Cmalloc_t *previous = NULL;
      
      while (current->next != NULL) {
        previous = current;
        current = current->next;
        
        if (current->start <= arg && arg <= current->end) {
          found = 1;
          length_vars  = (size_t) current->end;
          length_vars -= (size_t) arg;
          length_vars++;
          break;
        }
      }
      if (! found) {
        /* Not in a known window : we assume it is not longer   */
        /* than the maximum size of any typedef, itself assumed */
        /* to be 8 bytes.                                       */
        length_vars = 8;
      }
    } else {
      length_vars = 0;
    }
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : length_vars set to %d\n",
          _Cpool_self(),_Cthread_self(),(int) length_vars);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    found = 0;
    if (Cthread_mutex_lock(&Cpool) != 0) {
      return(-1);
    }
    /* We search the corresponding pool */
    current = &Cpool;
    while ((current = current->next) != NULL) {
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Wanted pool is %d, Got pool No %d\n",
            _Cpool_self(),_Cthread_self(),poolnb,current->poolnb);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (current->poolnb == poolnb) {
        found = 1;
        break;
      }
    }
    Cthread_mutex_unlock(&Cpool);
    
    if (found == 0) {
      serrno = EINVAL;
      return(-1);
    }
    
    /* We collect all the fd's to read from */
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] Wait flag on ",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    maxfd = 0;
    FD_ZERO(&readlist);
    for (i = 0; i < current->nbelem; i++) {
      /* We take care of any previous call to Cpool_next_index */
      if (current->forceid != -1) {
        if (i != current->forceid) {
          continue;
        }
      }
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"%d ",current->readfd[i]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (current->readfd[i] > maxfd)
        maxfd = current->readfd[i];
      FD_SET(current->readfd[i],&readlist);
    }
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"\n");
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We wait for a flag of any of the child */
    if (timeout > 0) {
      tv.tv_sec  = timeout;
      tv.tv_usec = 0;
      /* Possible warning on hpux 10 because cma_select expects "int" */
      /* instead of "fd_set"                                          */
      if (select(maxfd+1, (_cpool_fd_set *) &readlist, NULL, NULL, &tv) <= 0) {
        /* Error or timeout */
        /* ... We reset any previous call to Cpool_next_index */
        current->forceid = -1;
        serrno = SETIMEDOUT;
        return(-1);
      }
    } else {
      /* Possible warning on hpux 10 because cma_select expects "int" */
      /* instead of "fd_set"                                          */
      if (select(maxfd+1, (_cpool_fd_set *) &readlist, NULL, NULL, NULL) < 0) {
        /* Error */
        /* ... We reset any previous call to Cpool_next_index */
        current->forceid = -1;
        serrno = SEINTERNAL;
        return(-1);
      }
    }
    
    /* We got one, let's know which one it is */
    i = 0;
    for (i = 0; i <= current->nbelem; i++) {
      if (FD_ISSET(current->readfd[i],&readlist)) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] Got flag on %d\n",
              _Cpool_self(),_Cthread_self(),current->readfd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        /* We read the flag */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] Read the flag on %d\n",
              _Cpool_self(),_Cthread_self(),current->readfd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        _Cpool_readn(current->readfd[i], &ready, sizeof(int));
        /* And we send the arguments */
        /* ... address of the start routine */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] Write routine address = 0x%lx on %d\n",
              _Cpool_self(),_Cthread_self(),(unsigned long) startroutine,current->writefd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        _Cpool_writen(current->writefd[i],&startroutine,sizeof(void *));
        /* In a non-thread environment we write the argument length and content */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] Write argument length = %d on %d\n",
              _Cpool_self(),_Cthread_self(),(int) length_vars,current->writefd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        _Cpool_writen(current->writefd[i],&length_vars,sizeof(size_t));
        /* And the arguments themselves */
        if (length_vars > 0) {
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] Write argument content of size %d on %d\n",
                _Cpool_self(),_Cthread_self(),(int) length_vars,current->writefd[i]);
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          _Cpool_writen(current->writefd[i],arg,length_vars);
        }
        break;
      }
    }
    
    /* ... We reset any previous call to Cpool_next_index */
    current->forceid = -1;

    /* Return OK */
    return(0);

  } else {
#endif /* _WIN32 */
    /*----------------------- */
    /* Thread only            */
    /*----------------------- */
    struct Cpool_t *current = &Cpool;
    int    found;
    int    i;
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign(%d,0x%lx,0x%lx,%d)\n",
          _Cpool_self(),_Cthread_self(),poolnb, (unsigned long) startroutine, (unsigned long) arg, timeout);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (poolnb < 0) {
      errno = EINVAL;
      return(-1);
    }
    
    found = 0;
    if (Cthread_mutex_lock(&Cpool) != 0) {
      return(-1);
    }
    /* We search the corresponding pool */
    current = &Cpool;
    while ((current = current->next) != NULL) {
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Wanted pool is %d, Got pool No %d\n",
            _Cpool_self(),_Cthread_self(),poolnb,current->poolnb);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (current->poolnb == poolnb) {
        found = 1;
        break;
      }
    }
    Cthread_mutex_unlock(&Cpool);
    
    if (found == 0) {
      serrno = EINVAL;
      return(-1);
    }
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We assume that we will not have to wait   */
    if (Cthread_mutex_lock(&lock_parent) != 0) {
      return(-1);
    }
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Setting current->flag to -1\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    current->flag = 0;
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    Cthread_mutex_unlock(&lock_parent);
    
    /* We check if there is one thread available */
    found = 0;
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : lock on &lock_child\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (Cthread_mutex_lock(&lock_child) != 0) {
      return(-1);
    }
    for (i = 0; i < current->nbelem; i++) {
      if (current->state[i] == 0) {
        /* We take care of any previous call to Cpool_next_index */
        if (current->forceid != -1) {
          if (i != current->forceid) {
#ifdef CPOOL_DEBUG
            /* Cthread_mutex_lock(&lock_cpool_debug); */
            if (Cpool_debug != 0)
              log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Found thread at index %d, but expected %d instead\n",
                  _Cpool_self(),_Cthread_self(),i,current->forceid);
            /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
            continue;
          }
        }
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Found thread at index %d\n",
              _Cpool_self(),_Cthread_self(),i);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        /* Okay: We change the predicate */
        /* value on which the thread is  */
        /* waiting for to be changed     */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Setting current->state[%d] to 1\n",
              _Cpool_self(),_Cthread_self(),i);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        current->state[i] = 1;
        /* We put the routine and its    */
        /* arguments                     */
#if defined(__STDC__)
        current->start[i] = (void *(*)(void *)) startroutine;
#else
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d] current->start = 0x%lx, i = %d, stored at 0x%lx\n",
              _Cpool_self(),_Cthread_self(),
                (unsigned long) current->start,i,
                (unsigned long) (current->start + (i * sizeof(void *))));
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        current->start[i] = (void *(*)())       startroutine;
#endif
        current->arg[i] = (void *) arg;
        /* We signal the thread          */
        
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Setting condition on &lock_child\n",
              _Cpool_self(),_Cthread_self());
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        /* ... We reset any call to Cpool_next_index */
        current->forceid = -1;

        if (Cthread_cond_signal(&lock_child) != 0) {
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Oooups... Cthread_cond_signal(&lock_child) error (%s)\n",
                _Cpool_self(),_Cthread_self(),sstrerror(serrno));
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          Cthread_mutex_unlock(&lock_child);
          return(-1);
        }
        
        /* We remember for the immediate out of this loop... */
        found = 1;
        
        /* And we exit of the loop       */
        break;
      }
    }
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_child\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    Cthread_mutex_unlock(&lock_child);
    
    if (found != 0) {
      /* We found at least one thread available: */
      return(0);
    }
    
    /* We did not found one thread available */
    /* We say that we are waiting            */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (timeout == 0) {
      /* No thread immediately available, and timeout == 0 */
      /* So we exit immediately                            */
      serrno = SETIMEDOUT;
      return(-1);
    }

    if (Cthread_mutex_lock(&lock_parent) != 0) {
      return(-1);
    }
    current->flag = -1;
    
    /* And we wait on our predicate          */
    while (current->flag == -1) {
      if (timeout > 0) {
        
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Waiting condition on &lock_parent with timeout = %d seconds\n",
              _Cpool_self(),_Cthread_self(),timeout);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        if (Cthread_cond_timedwait(&lock_parent,timeout) != 0) {
          
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Waiting condition failed, reset current->flag to zero\n",
                _Cpool_self(),_Cthread_self());
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          
          current->flag = 0;
          
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_parent\n",
                _Cpool_self(),_Cthread_self());
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          
          Cthread_mutex_unlock(&lock_parent);

          /* ... We reset any call to Cpool_next_index */
          current->forceid = -1;

          return(-1);
        }
      } else {
        /* timeout < 0 : we wait, wait, wait... */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Waiting condition on &lock_parent with no timeout\n",
              _Cpool_self(),_Cthread_self());
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        if (Cthread_cond_wait(&lock_parent) != 0) {
          
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Waiting condition failed, reset current->flag to zero\n",
                _Cpool_self(),_Cthread_self());
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          
          current->flag = 0;
          
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_parent\n",
                _Cpool_self(),_Cthread_self());
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
                  
          Cthread_mutex_unlock(&lock_parent);

          /* ... We reset any call to Cpool_next_index */
          current->forceid = -1;

          return(-1);
        }
      }
    }

    /* Yes... */
    /* current->flag contains the index */
    /* of the thread that tell us it is */
    /* available                        */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Waiting condition okay, current->flag=%d\n",
          _Cpool_self(),_Cthread_self(),current->flag);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We check vs. any previous call to Cpool_next_index */
    if (current->forceid != -1) {
      if (current->flag != current->forceid) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Waiting condition okay, but current->flag=%d != current->forceid=%d\n",
              _Cpool_self(),_Cthread_self(),current->flag,current->forceid);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        current->flag = 0;
          
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_parent\n",
              _Cpool_self(),_Cthread_self());
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        
        Cthread_mutex_unlock(&lock_parent);
        
        /* ... We reset any call to Cpool_next_index */
        current->forceid = -1;
        
        return(-1);
      }
    }

#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : lock on &lock_child\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (Cthread_mutex_lock(&lock_child) != 0) {
      Cthread_mutex_unlock(&lock_parent);
      return(-1);
    }
    /* We change child predicate        */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Thread that answered has index %d, Setting predicate current->state[%d] to 1\n",
          _Cpool_self(),_Cthread_self(),current->flag,current->flag);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    current->state[current->flag] = 1;
    /* We put arguments                 */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Setting current->start[%d] to 0x%lx and current->arg[%d] to 0x%lx\n",
          _Cpool_self(),_Cthread_self(),current->flag,(unsigned long) startroutine,current->flag,
            arg != NULL ? (unsigned long) arg : 0);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We put the routine and its    */
    /* arguments                     */
#if defined(__STDC__)
    current->start[current->flag] = (void *(*)(void *)) startroutine;
#else
    current->start[current->flag] = (void *(*)())       startroutine;
#endif
    current->arg[current->flag] = (void *) arg;
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : reset current->flag\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    current->flag = 0;
    
    /* We signal him the child       */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : Signalling on &lock_child\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (Cthread_cond_signal(&lock_child) != 0) {
      Cthread_mutex_unlock(&lock_child);
      Cthread_mutex_unlock(&lock_parent);
      return(-1);
    }
    /* We release our lock on his predicate */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_child\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    Cthread_mutex_unlock(&lock_child);
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_assign : un-lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self());
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    Cthread_mutex_unlock(&lock_parent);
    return(0);
#ifndef _WIN32
  }
#endif
}

/* ============================================ */
/* Routine  : Cpool_next_index                  */
/* Arguments: pool number                       */
/* -------------------------------------------- */
/* Output   : index >= 0 (OK) -1 (ERROR)        */
/* -------------------------------------------- */
/* History:                                     */
/* 08-JUN-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
CTHREAD_DECL Cpool_next_index(poolnb)
     int poolnb;
{

#ifdef CPOOL_DEBUG
  /* Cthread_mutex_lock(&lock_cpool_debug); */
  if (Cpool_debug != 0)
    log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d)\n",
        _Cpool_self(),_Cthread_self(),poolnb);
  /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
#ifndef _WIN32
  /* THIS ROUTINE IS EXPLICITELY SPLITTED IN TWO PARTS    */
  /* - The _NOCTHREAD one (using pipes)                   */
  /* - The _CTHREAD one (using shared mem. and cond. var. */
  if (Cthread_environment() == CTHREAD_MULTI_PROCESS) {
    /*----------------------- */
    /* Non-Thread only        */
    /*----------------------- */
    struct Cpool_t *current = &Cpool;
    int    found;
    int                     maxfd;
    int                     i;
    fd_set                  readlist;
    int                     ready;
    
    if (poolnb < 0) {
      serrno = EINVAL;
      return(-1);
    }
    
    found = 0;
    if (Cthread_mutex_lock(&Cpool) != 0) {
      return(-1);
    }
    /* We search the corresponding pool */
    current = &Cpool;
    while ((current = current->next) != NULL) {
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index : Wanted pool is %d, Got pool No %d\n",
            _Cpool_self(),_Cthread_self(),poolnb,current->poolnb);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (current->poolnb == poolnb) {
        found = 1;
        break;
      }
    }
    Cthread_mutex_unlock(&Cpool);
    
    if (found == 0) {
      errno = EINVAL;
      return(-1);
    }
    
    /* We collect all the fd's to read from */
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Wait flag on ",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    maxfd = 0;
    FD_ZERO(&readlist);
    for (i = 0; i < current->nbelem; i++) {
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"%d ",current->readfd[i]);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (current->readfd[i] > maxfd)
        maxfd = current->readfd[i];
      FD_SET(current->readfd[i],&readlist);
    }
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"\n");
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We wait for a flag of any of the child */
    if (select(maxfd+1, (_cpool_fd_set *) &readlist, NULL, NULL, NULL) < 0) {
      /* Error */
      serrno = SEINTERNAL;
      return(-1);
    }
    
    /* We got one, let's know which one it is */
    i = 0;
    for (i = 0; i <= current->nbelem; i++) {
      if (FD_ISSET(current->readfd[i],&readlist)) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Got flag on %d\n",
              _Cpool_self(),_Cthread_self(),poolnb,current->readfd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        /* We read the flag */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Read the flag on %d\n",
              _Cpool_self(),_Cthread_self(),poolnb,current->readfd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        _Cpool_readn(current->readfd[i], &ready, sizeof(int));
        /* And we send a wait flag */
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Write sleep flag on %d\n",
              _Cpool_self(),_Cthread_self(),poolnb,current->writefd[i]);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        _Cpool_writen(current->writefd[i],&_cpool_sleep_flag,sizeof(void *));
        /* We remember it for the next assignment */
        current->forceid = i;
        return(i);
        break;
      }
    }
    
    /* We should not be there */
    errno = SEINTERNAL;
    return(-1);
    
  } else {
#endif /* _WIN32 */
    /*----------------------- */
    /* Thread only            */
    /*----------------------- */
    struct Cpool_t *current = &Cpool;
    int    found;
    int    i;
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d)\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (poolnb < 0) {
      errno = EINVAL;
      return(-1);
    }
    
    found = 0;
    if (Cthread_mutex_lock(&Cpool) != 0) {
      return(-1);
    }
    /* We search the corresponding pool */
    current = &Cpool;
    while ((current = current->next) != NULL) {
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Got pool No %d\n",
            _Cpool_self(),_Cthread_self(),poolnb,current->poolnb);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      if (current->poolnb == poolnb) {
        found = 1;
        break;
      }
    }
    Cthread_mutex_unlock(&Cpool);
    
    if (found == 0) {
      errno = EINVAL;
      return(-1);
    }
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    /* We assume that we will not have to wait   */
    if (Cthread_mutex_lock(&lock_parent) != 0) {
      return(-1);
    }
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Setting current->flag to 0\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif

#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : un-lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    Cthread_mutex_unlock(&lock_parent);
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : lock on &lock_child\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (Cthread_mutex_lock(&lock_child) != 0) {
      return(-1);
    }
    for (i = 0; i < current->nbelem; i++) {
      if (current->state[i] == 0) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Found thread at index %d\n",
              _Cpool_self(),_Cthread_self(),poolnb,i);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        if (current->flag == -1) {
          if (current->forceid == i) {
            /* We found again the same thread as before */
#ifdef CPOOL_DEBUG
            /* Cthread_mutex_lock(&lock_cpool_debug); */
            if (Cpool_debug != 0)
              log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Return again %d\n",
                  _Cpool_self(),_Cthread_self(),poolnb,i);
            /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
            Cthread_mutex_unlock(&lock_child);
            current->forceid = i;
            return(i);
          } else {
            /* We should have find the same ! */
#ifdef CPOOL_DEBUG
            /* Cthread_mutex_lock(&lock_cpool_debug); */
            if (Cpool_debug != 0)
              log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Got %d before, still valid it seems, and now got %d !\n",
                  _Cpool_self(),_Cthread_self(),poolnb,current->forceid,i);
            /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
            Cthread_mutex_unlock(&lock_child);
            serrno = SEINTERNAL;
            return(-1);
          }
        } else {
        /* Okay: We will return this index */
          Cthread_mutex_unlock(&lock_child);
          current->forceid = i;
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Return %d\n",
                _Cpool_self(),_Cthread_self(),poolnb,i);
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          return(i);
        }
      }
    }
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : un-lock on &lock_child\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    Cthread_mutex_unlock(&lock_child);
    
    /* We did not found one thread available */
    /* We say that we are waiting            */
    
#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : lock on &lock_parent\n",
          _Cpool_self(),_Cthread_self(),poolnb);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    if (Cthread_mutex_lock(&lock_parent) != 0) {
      return(-1);
    }

    while (1) {
      current->flag = -1;
      
      /* And we wait on our predicate          */
      while (current->flag == -1) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Waiting condition on &lock_parent with no timeout\n",
              _Cpool_self(),_Cthread_self(),poolnb);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        
        if (Cthread_cond_wait(&lock_parent) != 0) {
          
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Waiting condition failed, reset current->flag to zero\n",
                _Cpool_self(),_Cthread_self(),poolnb);
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          
          current->flag = 0;
          
#ifdef CPOOL_DEBUG
          /* Cthread_mutex_lock(&lock_cpool_debug); */
          if (Cpool_debug != 0)
            log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : un-lock on &lock_parent\n",
                _Cpool_self(),_Cthread_self(),poolnb);
          /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
          
          
          Cthread_mutex_unlock(&lock_parent);
          return(-1);
        }
      }
      
      /* Yes... */
      /* current->flag contains the index */
      /* of the thread that tell us it is */
      /* available                        */
      
#ifdef CPOOL_DEBUG
      /* Cthread_mutex_lock(&lock_cpool_debug); */
      if (Cpool_debug != 0)
        log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Waiting condition okay, returns current->flag=%d\n",
            _Cpool_self(),_Cthread_self(),poolnb,current->flag);
      /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
      
      /* We check that this thread is REALLY in a READY status */
      if (current->state[current->flag] != 0) {
#ifdef CPOOL_DEBUG
        /* Cthread_mutex_lock(&lock_cpool_debug); */
        if (Cpool_debug != 0)
          log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Waiting condition okay, returns current->flag=%d, but this thread state is not READY. Continue.\n",
              _Cpool_self(),_Cthread_self(),poolnb,current->flag);
        /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
        continue;
      }
      break;
    }

    
    /* ... And remember it to force assignment */
    i = current->forceid = current->flag;

    Cthread_mutex_unlock(&lock_parent);

#ifdef CPOOL_DEBUG
    /* Cthread_mutex_lock(&lock_cpool_debug); */
    if (Cpool_debug != 0)
      log(LOG_INFO,"[Cpool  [%2d][%2d]] In Cpool_next_index(%d) : Return %d\n",
          _Cpool_self(),_Cthread_self(),poolnb,current->flag);
    /* Cthread_mutex_unlock(&lock_cpool_debug); */
#endif
    
    return(i);

#ifndef _WIN32
  }
#endif
}

/* ============================================ */
/* Routine  : _Cpool_self                       */
/* Arguments:                                   */
/* -------------------------------------------- */
/* Output   : pool number >= 0 (OK) -1 (ERROR)  */
/* -------------------------------------------- */
/* History:                                     */
/* 12-JUL-1999       First implementation       */
/*                   Jean-Damien.Durand@cern.ch */
/* ============================================ */
CTHREAD_DECL _Cpool_self() {
  struct Cpool_t *current = NULL;
  int             i;
  int             cid;

  /* Nota: There is no call to Cthread_mutex_lock
     nor Cthread_mutex_unlock on &Cpool because
     this would block Cpool execution. Neverthless
     all debug printout in Cpool are wrapped into
     mutex lock/unlock on variable lock_cpool_debug
  */

  /* We get current Cthread ID */
  if ((cid = _Cthread_self()) < 0) {
    return(-1);
  }
  
  /* We search the corresponding pool */
  current = &Cpool;
  while ((current = current->next) != NULL) {
    for (i = 0; i < current->nbelem; i++) {
      if (current->cid[i] == cid) {
        return(current->poolnb);
      }
    }
  }
  
  serrno = EINVAL;
  return(-1);
}





