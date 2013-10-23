/*
 * rfio_calls.c,v 1.3 2004/03/22 12:11:24 jdurand Exp
 */

/*
 * Copyright (C) 1990-2002 by CERN/IT/PDP/DM
 * All rights reserved
 */

#if defined(linux)
#define _GNU_SOURCE                /* O_DIRECT */
#endif

/*
 * Remote file I/O flags and declarations.
 */
#define RFIO_KERNEL     1
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <common.h>
#include <stdlib.h>
#include <zlib.h>
#include <sys/xattr.h>
#include <sys/param.h>
#include <syslog.h>                     /* System logger                */
#include <sys/time.h>
#if defined(linux)
#include <signal.h>
#endif

#if USE_XFSPREALLOC
#include "rfio_xfsprealloc.h"
#endif

#include "rfio.h"
#include "rfcntl.h"
#include "log.h"
#include "u64subr.h"
#include "Castor_limits.h"
#include "getconfent.h"

#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>

#ifdef linux
#include <sys/uio.h>
#include "rfio_alignedbuf.h"
#endif

#include "rfio_callhandlers.h"
#include "checkkey.h"
#include "alrm.h"
#include "rfio_calls.h"

extern int forced_umask;
#define CORRECT_UMASK(this) (forced_umask > 0 ? forced_umask : this)
extern int ignore_uid_gid;

extern int Csec_service_type;
extern int peer_uid;
extern int peer_gid;

#include <fcntl.h>

/* For multithreading stuff, only tested under Linux at present */
static int daemonv3_rdmt, daemonv3_wrmt;
#include <Cthread_api.h>
#include "Csemaphore.h"

/* If DAEMONV3_RDMT is true, reading from disk will be multithreaded
   The circular buffer will have in this case DAEMONV3_RDMT_NBUF buffers of
   size DAEMONV3_RDMT_BUFSIZE. Defaults values are defined below */
#define DAEMONV3_RDMT (1)
#define DAEMONV3_RDMT_NBUF (4)
#define DAEMONV3_RDMT_BUFSIZE (2097152)

static int daemonv3_rdmt_nbuf, daemonv3_rdmt_bufsize;

/* If DAEMONV3_WRMT is true, reading from disk will be multithreaded
   The circular buffer will have in this case DAEMONV3_WRMT_NBUF buffers of
   size DAEMONV3_WRMT_BUFSIZE. Defaults values are defined below */
#define DAEMONV3_WRMT (1)
#define DAEMONV3_WRMT_NBUF (4)
#define DAEMONV3_WRMT_BUFSIZE (2097152)

static int daemonv3_wrmt_nbuf, daemonv3_wrmt_bufsize;

/* The circular buffer definition */
static struct element {
  char *p;
  int len;
} *array;

/* The two semaphores to synchonize accesses to the circular buffer */
CSemaphore empty,full;
/* Number of buffers produced and consumed */
int produced = 0,consumed = 0;

/* Variable used for error reporting between disk writer thread
   and main thread reading from the network */
static int write_error = 0;
/* Variable set by the main thread reading from the network
   to tell the disk reader thread to stop */
static int stop_read;

extern char *getconfent();
int check_user_perm();                /* Forward declaration                 */
static int chksuser();                /* Forward declaration                 */

/*
 * Buffer declarations
 */
char         rqstbuf[BUFSIZ];                /* Request buffer                */
char    filename[MAXFILENAMSIZE];       /* file name                     */

static char     *iobuffer;              /* Data communication buffer    */
static int      iobufsiz= 0;            /* Current io buffer size       */

/*
 * Communication limits.
 */
#define SO_BUFSIZE      20*1024         /* Default socket buffer size   */
#define MAXXFERSIZE     200*1024        /* Maximum transfer size        */

extern char *forced_filename;
#define CORRECT_FILENAME(filename) (forced_filename != NULL ? forced_filename : filename)


/* Warning : the new sequential transfer mode cannot be used with
   several files open at a time (because of those global variables)*/
static int data_sock;   /* Data socket */
static int ctrl_sock;  /* the control socket */
static int first_write;
static int first_read;
static int byte_read_from_network;
static struct rfiostat myinfo;
/* Context for the open/firstwrite/close handlers */
extern void *handler_context;

const char *rfio_all_perms[] = { "FTRUST", "WTRUST", "RTRUST", "XTRUST", "OPENTRUST", "STATTRUST",
                                 "POPENTRUST", "LINKTRUST", "CHMODTRUST", "CHOWNTRUST", "MKDIRTRUST",
                                 "RMDIRTRUST", "RENAMETRUST", NULL };

/* free elements in a NULL terminated array of character pointers
 * and the array itself
 */
static int l_free_strlist(char **pathelements)
{
  int n = 0;

  if (pathelements == NULL) {
    errno = EFAULT; return -1;
  }

  while(pathelements[n] != NULL) {
    free(pathelements[n]);
    ++n;
  }

  free(pathelements);
  return 0;
}


/* array of file path elements to
 * a path string.
 * pathelements: array of character pointers of the elements
 * type: 0 for unix style, 1 for windows style
 * prefix: prefix for the path, used for windows style drive or hostname
 */
static char *l_path_elements_to_path(const char **pathelements,
                                     int type,
                                     const char *prefix)
{
  size_t len;
  int n;
  char *result;
  const char *separator;

  if (pathelements == NULL) {
    errno = EFAULT; return NULL;
  }

  n = 0; len = 0;
  while(pathelements[n] != NULL) {
    len += strlen(pathelements[n])+1;
    ++n;
  }
  if (len==0) len=1;
  if (prefix != NULL) len += strlen(prefix);

  switch(type) {
  case 0:
    separator = "/";
    break;
  default:
    separator = "\\";
    break;
  }

  result = (char*)malloc(len+1);
  if (result == NULL) return NULL;
  *result = '\0';
  if (prefix != NULL) strcat(result, prefix);

  n=0;
  while(pathelements[n] != NULL) {
    strcat(result,separator);
    strcat(result, pathelements[n]);
    ++n;
  }
  if (n==0) strcat(result,separator);

  return result;
}

/* create a canonical representation of a path
 * used in order to compare paths for path white listing.
 * Similar to realpath() functionality, but without the
 * PATH_MAX dependency and some provision for windows paths
 * and avoiding some portability problems with realpath()
 */
static char **l_canonicalize_path(inpath,sym_depth,effective_cwd,path_typep,prefixp,travel_sym)
     const char *inpath;
     int sym_depth;
     const char *effective_cwd;
     int *path_typep;
     char **prefixp;
     int travel_sym;
{
  char buffer[4096];
  const char *delineators;
  char *cp;
  char **elements = NULL;
  int n;
  char *ptr = NULL;
  char **result = NULL;
  int save_errno;

  if (inpath == NULL) {
    errno = EFAULT; goto error;
  }

  if (*inpath == '\0') {
    errno = ENOENT; goto error;
  }

  if (sym_depth>8) {
    errno = ELOOP; goto error;
  }

  if (prefixp == NULL) {
    errno = EINVAL; goto error;
  }

  if (path_typep == NULL) {
    errno = EINVAL; goto error;
  }

  if (*prefixp != NULL) {
    free(*prefixp);
    *prefixp = NULL;
  }

  /* set path_type and prefix */
  *path_typep = 0;
  *prefixp = NULL;

  if (*inpath != '/') {
    if (effective_cwd != NULL) {
      if (strlen(effective_cwd)>=sizeof(buffer)) {
        errno = ENAMETOOLONG;
        goto error;
      } else {
        strcpy(buffer, effective_cwd);
      }
    } else {
      if (getcwd(buffer, sizeof(buffer))==NULL) goto error;
    }

    if (*buffer != '/') {
      errno = EINVAL; goto error;
    }

    ptr = (char*)malloc(strlen(buffer)+1+strlen(inpath)+1);
    if (ptr == NULL) goto error;

    strcpy(ptr, buffer);
    strcat(ptr, "/");
    strcat(ptr, inpath);

    result = l_canonicalize_path(ptr,sym_depth,buffer,path_typep,prefixp,travel_sym);
    goto success;
  }

  elements = (char**)calloc(strlen(inpath), sizeof(char *));
  if (elements == NULL) goto error;

  ptr = (char*)malloc(strlen(inpath)+1);
  if (ptr == NULL) goto error;
  strcpy(ptr, inpath);

  if (*path_typep == 0) delineators = "/";
  else delineators = "\\";

  for (n=0,cp=strtok(ptr,delineators);cp!=NULL;cp=strtok(NULL,delineators)) {
    if (!strcmp(cp,".")) continue;
    if (!strcmp(cp,"..")) {
      if (n>0) {
        free(elements[--n]);
        elements[n] = NULL;
      }
      continue;
    }
    elements[n++] = strdup(cp);
  }
  free(ptr); ptr=NULL;

  /* check through for symbolic links */
  n=0;
  do {
    char *ptr2,**e2;
    int rc,count,count2,n2;

    cp = elements[n];
    elements[n] = NULL;

    ptr2 = l_path_elements_to_path((const char**)elements, *path_typep, *prefixp);
    elements[n] = cp;

    if (ptr2 == NULL) goto error;

    rc = readlink(ptr2, buffer, sizeof(buffer)-1);
    if (n==0 || rc<0 || (cp == NULL && !travel_sym)) {
      if (ptr != NULL) free(ptr);
      ptr = ptr2;
      ++n;
      continue;
    }
    free(ptr2);
    buffer[rc] = '\0';

    result = l_canonicalize_path(buffer, sym_depth+1, ptr, path_typep, prefixp,travel_sym);
    if (result == NULL) goto error;

    free(ptr); ptr = NULL;

    count=0; while(result[count] != NULL) ++count;
    count2=0; while(elements[n+count2] != NULL) ++count2;

    e2 = (char**)realloc(result, sizeof(char*)*(count+count2+1));
    if (e2 == NULL) goto error;
    result = e2;

    for(n2=0;n2<count2;n2++) {
      result[count+n2] = elements[n+n2];
      elements[n+n2] = NULL;
    }
    result[count+count2] = NULL;
    l_free_strlist(elements);

    elements = result; result = NULL;
    n = 0; cp = elements[0];
  } while(cp != NULL);

  result = elements;
  elements = NULL;

 success:
  if (ptr != NULL) free(ptr);
  return result;

 error:
  save_errno = errno;
  if (ptr!=NULL) free(ptr);
  if (elements!=NULL) l_free_strlist(elements);
  if (result!=NULL) l_free_strlist(result);
  if (prefixp != NULL && *prefixp != NULL) {
    free(*prefixp);
    *prefixp = NULL;
  }
  errno = save_errno;
  return NULL;
}

/* return the canonical path as string rather than elements
 * outpath: if set is filled with a pointer to the canonical path
 *
 * the following are equivalent to the canonical path
 * outtype: if set is filled with the classification type of the path
 * outprefix: if set is filled with the path prefix
 * outelements: if set is set to an array of pointers to the path elements
 */
static int rfio_canonicalize_path(const char *inpath,
                                  char **outpath,
                                  int *outtype,
                                  char **outprefix,
                                  char ***outelements,
                                  int travel_sym)
{
  char *result_path;
  char **elements;
  int p_type;
  char *prefix=NULL;

  if (outelements != NULL) *outelements = NULL;
  if (outpath != NULL) *outpath = NULL;
  if (outprefix != NULL) *outprefix = NULL;

  elements = l_canonicalize_path(inpath, 0, NULL, &p_type, &prefix, travel_sym);
  if (elements==NULL) return -1;
  result_path = l_path_elements_to_path((const char**)elements, p_type, prefix);

  /* makes it easier to compare the prefixes */
  if (prefix == NULL) prefix=strdup("");

  if (result_path == NULL || prefix == NULL) {
    l_free_strlist(elements);
    if (prefix != NULL) free(prefix);
    if (result_path != NULL) free(result_path);
    return -1;
  }

  if (outelements != NULL) {
    *outelements = elements;
  } else {
    l_free_strlist(elements);
  }

  if (outpath != NULL) {
    *outpath = result_path;
  } else {
    free(result_path);
  }

  if (outprefix != NULL) {
    *outprefix = prefix;
  } else {
    if (prefix != NULL) free(prefix);
  }

  if (outtype != NULL) {
    *outtype = p_type;
  }

  return 0;
}

/* perform path whitelist logic, including checking of
 * hostname for possible whitelist bypass:
 *
 * hostname which may be used to check for bypass
 * path to be tested
 * rfiod_permstrs possible list of permission strings
 *
 * opath: if not null is filled with a canonicalized version
 * opathsize: space available in opath
 */
int check_path_whitelist(const char *hostname,
                         const char *path,
                         const char **rfiod_permstrs,
                         char *opath,
                         size_t opathsize,
                         int travel_sym)
{
  int found = 0;
  int n;
  int save_errno = errno;
  char **test_path_elements;
  char *test_path_prefix;
  char *test_path;
  int test_path_type;
  char **white_list = NULL, **additional = NULL;
  int count = 0, count2 = 0;

  if (path == NULL || (hostname == NULL && rfiod_permstrs != NULL)) {
    (*logfunc)(LOG_ERR, "check_path_whitelist: Invalid host or path, disallowing access\n");
    errno = EFAULT;
    return -1;
  }

  if (rfiod_permstrs != NULL) {
    const char *permstr, *cp;
    char *p = NULL;
    for(n = 0; !found && (permstr = rfiod_permstrs[n]) != NULL; ++n) {
      if ( (cp=getconfent("RFIOD", permstr, 1)) != NULL ) {
        p = (char*)malloc(strlen(cp)+1);
        strcpy(p, cp);
        for (cp=strtok(p,"\t ");cp!=NULL;cp=strtok(NULL,"\t ")) {
          if ( !strcmp(hostname,cp) ) {
            found ++;
            break;
          }
        }
        free(p);
        p = NULL;
      }
    }
  }

  if (found) {
    if (opath == NULL) {
      (*logfunc)(LOG_ERR, "check_path_whitelist: Host trusted but output path is null\n");
      errno = EINVAL;
      return -1;
    }
    if (opath != NULL && strlen(path)>=opathsize) {
      (*logfunc)(LOG_ERR, "check_path_whitelist: Host trusted but path (%s) too long to return in output\n", path);
      errno = ENAMETOOLONG;
      return -1;
    }
    strcpy(opath, path);
    errno = save_errno;
    return 0;
  }

  if (rfio_canonicalize_path(path,&test_path,&test_path_type,&test_path_prefix,&test_path_elements,travel_sym)<0) {
    (*logfunc)(LOG_ERR, "check_path_whitelist: Could not canonicalize the path in the request (%s), disallowing access\n", path);
    /* keep errno from rfio_canonicalize_path() */
    return -1;
  }

  if (getconfent_multi("DiskManager", "MountPoints", 1, &white_list, &count)<0) {
    white_list = NULL; count = 0;
  } else if (count==0) {
    l_free_strlist(white_list); white_list = NULL;
  }

  if (getconfent_multi("RFIOD", "PathWhiteList", 1, &additional, &count2)<0) {
    additional = NULL; count2 = 0;
  } else if (count2==0) {
    l_free_strlist(additional); additional = NULL;
  } else {
    if (count==0) {
      count = count2;
      white_list = additional;
    } else {
      char **p = (char**)realloc(white_list, sizeof(char *) * (count+count2+1));
      if (p == NULL) {
        save_errno = errno;
        l_free_strlist(white_list);
        l_free_strlist(additional);
        l_free_strlist(test_path_elements);
        free(test_path_prefix);
        free(test_path);
        (*logfunc)(LOG_ERR, "check_path_whitelist: Problem making the white list, disallowing access\n");
        errno = save_errno;
        return -1;
      }
      white_list = p;
      for(n=0;n<count2;++n) {
        white_list[count+n] = additional[n];
        additional[n] = NULL;
      }
      white_list[count+count2] = NULL;
      count += count2;
      l_free_strlist(additional);
    }
    additional = NULL;
  }

  for(n=0;!found && n<count;++n) {
    int i,match=1;
    char **p_elem,*p_prefix;
    int p_type;
    if (!rfio_canonicalize_path(white_list[n],NULL,&p_type,&p_prefix,&p_elem,1)) {
      if (p_type == test_path_type && !strcmp(p_prefix,test_path_prefix)) {
        for(i=0;1;i++) {
          if (p_elem[i] == NULL) break;
          if (test_path_elements[i] == NULL || strcmp(p_elem[i], test_path_elements[i])) {
            match = 0;
            break;
          }
        }
        if (match) { found=1; }
      }
      l_free_strlist(p_elem);
      free(p_prefix);
    }
  }

  l_free_strlist(white_list);
  l_free_strlist(test_path_elements);
  free(test_path_prefix);

  if (!found) {
    (*logfunc)(LOG_ERR, "check_path_whitelist: Could not match path %s (canonicalized to %s) to white list\n", path, test_path);
    free(test_path);
    errno = EACCES;
    return -1;
  }

  if (opath != NULL && strlen(test_path)>=opathsize) {
    (*logfunc)(LOG_ERR, "check_path_whitelist: Canonicalized path %s too long to return\n",test_path);
    free(test_path);
    errno = ENAMETOOLONG;
    return -1;
  }

  (*logfunc)(LOG_INFO, "check_path_whitelist: Granting %s\n", path);

  if (opath != NULL) {
    strcpy(opath, test_path);
  }

  free(test_path);

  errno = save_errno;
  return 0;
}

/************************************************************************/
/*                                                                      */
/*                              IO HANDLERS                             */
/*                                                                      */
/************************************************************************/

int     srrequest(int     s,
                  int     *bet)
{
  char    * p;
  WORD  magic;
  WORD   code;
  int       n;
  fd_set fds;

  FD_ZERO(&fds);
  FD_SET(s,&fds);
  if ( select(s+1,&fds,(fd_set *)0,(fd_set *)0,NULL) == -1 ) {
    (*logfunc)(LOG_ERR,"rrequest(): select(): %s\n",strerror(errno));
    return -1;
  }
  if ( ! FD_ISSET(s,&fds) ) {
    (*logfunc)(LOG_ERR,"rrequest(): select() returns OK but FD_ISSET not\n");
    return -1;
  }
  (*logfunc)(LOG_DEBUG, "rrequest: reading %d bytes\n",RQSTSIZE);
  if ((n = netread_timeout(s,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT)) != RQSTSIZE) {
    if (n == 0)      {
      return 0;
    }
    else {
      (*logfunc)(LOG_ERR, "rrequest: read(): %s\n", strerror(errno));
      return -1;
    }
  }
  p= rqstbuf;
  unmarshall_WORD(p,magic);
  unmarshall_WORD(p,code);
  (*logfunc)(LOG_DEBUG,"rrequest:  magic: %x code: %x\n",magic,code);
  *bet= ( magic == RFIO_MAGIC ? 0 : 1 );
  return code;
}

int srchkreqsize(int     s,
                 char *p,
                 int len)
{
  char tmpbuf[1024];
  int templen = 1024;
  if ( p == NULL ) {
    errno = EINVAL;
    return(-1);
  }
  if ( len <=0 ) return(0);
  if ( p+len > rqstbuf+sizeof(rqstbuf) ) {
    (*logfunc)(LOG_ERR,"rchkreqsize() request too long (%d > %d)\n",len,
        (int)(sizeof(rqstbuf)-(p-rqstbuf)));
    errno = E2BIG;
    /* empty the buffer */
    while (netread_timeout(s, tmpbuf, templen, RFIO_CTRL_TIMEOUT) == templen);
    return(-1);
  }
  return(0);
}

int srrename(int     s,
             char    *host,
             int     rt)
{
  char    *p;
  LONG    status = 0;
  LONG    len;
  int     uid,gid;
  char    filenameo[MAXFILENAMSIZE];
  char    filenamen[MAXFILENAMSIZE];
  int     rcode = 0;

  p = rqstbuf + (2*WORDSIZE);
  unmarshall_WORD(p, uid);
  unmarshall_WORD(p, gid);
  unmarshall_LONG(p, len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    /*
     * Reading rename request.
     */
    (*logfunc)(LOG_DEBUG, "srrename for (%d,%d): reading %d bytes\n", uid,gid,len);
    /* rename() is not for remote users */
    if ( rt ) {
      status = -1;
      rcode = EACCES;
      (*logfunc)(LOG_ERR,"Attempt to srrename() from %s denied\n",host);
    }
    else {
      if ( ((status=check_user_perm(&uid,&gid,host,&rcode,"WTRUST")) < 0) && ((status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0) ) {
        if (status == -1)
          (*logfunc)(LOG_ERR,"srrename(): UID %d not allowed to rename()\n",uid);
        else
          (*logfunc)(LOG_ERR,"srrename(): failed, rcode = %d\n",rcode);
        status = -1;
      }
      if ( !status && forced_filename != NULL) {
        (*logfunc)(LOG_ERR,"Disallowing rename in forced filename mode\n");
        status= -1;
        errno=EACCES;
        rcode=errno;
      }
      if (netread_timeout(s, rqstbuf, len, RFIO_CTRL_TIMEOUT) != len) {
        (*logfunc)(LOG_ERR, "srrename(): read(): %s\n", strerror(errno));
        return -1;
      }
      p = rqstbuf;
      *filenameo = *filenamen = '\0';
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, filenameo,MAXFILENAMSIZE)) == -1)
        rcode = SENAMETOOLONG;
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, filenamen,MAXFILENAMSIZE)) == -1)
        rcode = SENAMETOOLONG;
      (*logfunc)(LOG_INFO,"srrename: filenameo %s, filenamen %s\n", filenameo, filenamen);
      if (status == 0 ) {
        const char *perm_array[] = { "WTRUST", "RENAMETRUST", NULL };
        char ofilenameo[MAXFILENAMSIZE], ofilenamen[MAXFILENAMSIZE];
        if (!check_path_whitelist(host, filenameo, perm_array, ofilenameo, sizeof(ofilenameo),0) &&
            !check_path_whitelist(host, filenamen, perm_array, ofilenamen, sizeof(ofilenamen),0)) {
          if ( (status =  rename(ofilenameo, ofilenamen)) < 0 )
            rcode = errno;
          else
            status = 0;
        } else {
          status = -1;
          rcode = errno;
        }
      }
    }
  }

  p = rqstbuf;
  marshall_LONG(p, status);
  marshall_LONG(p, rcode);
  (*logfunc)(LOG_DEBUG, "srrename: sending back status %d rcode %d\n", status,rcode);
  if (netwrite_timeout(s, rqstbuf, 2*LONGSIZE, RFIO_CTRL_TIMEOUT) != (2*LONGSIZE))  {
    (*logfunc)(LOG_ERR, "srrename(): netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return status;
}

int   srerrmsg(int     s)
{
  int   code = 0;
  int    len = 0;
  char * msg = NULL;
  char *   p = NULL;

  p = rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p, code);
  (*logfunc)(LOG_INFO, "rerrmsg: code: %d\n",code);
  {
    const char *const_msg = (code > 0) ? sstrerror(code) : "Invalid error code";
    msg = strdup(const_msg);
  }
  (*logfunc)(LOG_DEBUG, "rerrmsg: errmsg: %s\n",msg);
  len = strlen(msg)+1;
  p = rqstbuf;
  marshall_LONG(p,len);
  marshall_STRING(p,msg);
  (*logfunc)(LOG_DEBUG, "rerrmsg: sending back %d bytes\n",len+LONGSIZE);
  free(msg);
  if (netwrite_timeout(s,rqstbuf,len+LONGSIZE,RFIO_CTRL_TIMEOUT) != (len+LONGSIZE)) {
    (*logfunc)(LOG_ERR, "rerrmsg: netwrite_timeout(): %s\n", strerror(errno));
    return(-1);
  }
  return(0);
}

int     srlstat(int     s,
                int     rt, /* Is it a remote site call ?   */
                char *host, /* Where the request comes from */
                int   bet) /* Version indicator: 0(old) or 1(new) */
{
  char *   p;
  int status = 0, rcode = 0;
  int    len;
  struct stat statbuf;
  char user[CA_MAXUSRNAMELEN+1];
  int uid,gid;

  memset(&statbuf,'\0',sizeof(statbuf));
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    /*
     * Reading stat request.
     */
    (*logfunc)(LOG_DEBUG,"rlstat: reading %d bytes\n",len);
    if ((status = netread_timeout(s,rqstbuf,len,RFIO_CTRL_TIMEOUT)) != len) {
      (*logfunc)(LOG_ERR,"rlstat: read(): %s\n",strerror(errno));
      return -1;
    }
    p= rqstbuf;
    status = uid = gid = 0;
    if (bet) {
      unmarshall_WORD(p,uid);
      unmarshall_WORD(p,gid);
      *user = '\0';
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p,user,CA_MAXUSRNAMELEN+1)) == -1 )
        rcode = E2BIG;
    }
    *filename = '\0';
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p,filename,MAXFILENAMSIZE)) == -1 )
      rcode = SENAMETOOLONG;

    if ( (status == 0) && bet && rt ) {
      char to[100];
      int rcd;
      int to_uid, to_gid;

      if ( (rcd = get_user(host,user,uid,gid,to,&to_uid,&to_gid)) == -ENOENT ) {
        (*logfunc)(LOG_ERR,"rlstat: get_user(): Error opening mapping file\n");
        status= -1;
        errno = EINVAL;
        rcode = errno;
      }

      if ( !status && abs(rcd) == 1 ) {
        (*logfunc)(LOG_ERR,"No entry in mapping file for (%s,%s,%d,%d)\n", host,user,uid,gid);
        status= -1;
        errno=EACCES;
        rcode=errno;
      }
      else {
        (*logfunc)(LOG_DEBUG,"(%s,%s,%d,%d) mapped to %s(%d,%d)\n",
            host,user,uid,gid,to,to_uid,to_gid);
        uid = to_uid;
        gid = to_gid;
      }
    }
    if ( !status ) {
      if (bet &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"FTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"WTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"RTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"XTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"STATTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"POPENTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"LINKTRUST")) < 0  &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"CHMODTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"CHOWNTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"MKDIRTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"RMDIRTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0 ) {
        if (status == -2)
          (*logfunc)(LOG_ERR,"rlstat(): uid %d not allowed to stat()\n",uid);
        else
          (*logfunc)(LOG_ERR,"rlstat(): failed at check_user_perm(), rcode %d\n",rcode);
        status = rcode;
      } else {
        char ofilename[MAXFILENAMSIZE];
        strcpy(ofilename, filename);
        if (forced_filename != NULL || !check_path_whitelist(host, filename, rfio_all_perms, ofilename, sizeof(ofilename),0)) {
          status= ( lstat(CORRECT_FILENAME(ofilename), &statbuf) < 0 ) ? errno : 0;
        } else {
          status = errno;
        }
        (*logfunc)(LOG_INFO,"rlstat: file: %s , status %d\n",CORRECT_FILENAME(filename),status);
      }
    }
  }

  p = rqstbuf;
  marshall_WORD(p, statbuf.st_dev);
  marshall_LONG(p, statbuf.st_ino);
  marshall_WORD(p, statbuf.st_mode);
  marshall_WORD(p, statbuf.st_nlink);
  marshall_WORD(p, statbuf.st_uid);
  marshall_WORD(p, statbuf.st_gid);
  marshall_LONG(p, statbuf.st_size);
  marshall_LONG(p, statbuf.st_atime);
  marshall_LONG(p, statbuf.st_mtime);
  marshall_LONG(p, statbuf.st_ctime);
  /*
   * Bug #2646. This is one of the rare cases when the errno
   * is returned in the status parameter.
   */
  if ( status == -1 && rcode > 0 ) status = rcode;
  marshall_LONG(p, status);
  (*logfunc)(LOG_DEBUG, "rlstat: sending back %d\n", status);
  if (netwrite_timeout(s,rqstbuf,6*LONGSIZE+5*WORDSIZE,RFIO_CTRL_TIMEOUT) != (6*LONGSIZE+5*WORDSIZE))  {
    (*logfunc)(LOG_ERR, "rlstat: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

int     srstat(int     s,
               int   rt, /* Is it a remote site call ?   */
               char *host, /* Where the request comes from */
               int   bet) /* Version indicator: 0(old) or 1(new) */
{
  char *   p;
  int status = 0, rcode = 0;
  int    len;
  struct stat statbuf;
  char user[CA_MAXUSRNAMELEN+1];
  int uid,gid;

  memset(&statbuf, 0, sizeof(statbuf));  
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    /*
     * Reading stat request.
     */
    (*logfunc)(LOG_DEBUG,"rstat: reading %d bytes\n",len);
    if ((status = netread_timeout(s,rqstbuf,len,RFIO_CTRL_TIMEOUT)) != len) {
      (*logfunc)(LOG_ERR,"rstat: read(): %s\n",strerror(errno));
      return -1;
    }
    p= rqstbuf;
    status = uid = gid = 0;
    if (bet) {
      unmarshall_WORD(p,uid);
      unmarshall_WORD(p,gid);
      *user = '\0';
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p,user,CA_MAXUSRNAMELEN+1)) == -1 )
        rcode = E2BIG;
    }
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p,filename,MAXFILENAMSIZE)) == -1 )
      rcode = SENAMETOOLONG;

    if ( (status == 0) && bet && rt ) {
      char to[100];
      int rcd;
      int to_uid, to_gid;

      if ( (rcd = get_user(host,user,uid,gid,to,&to_uid,&to_gid)) == -ENOENT ) {
        (*logfunc)(LOG_ERR,"rstat: get_user(): Error opening mapping file\n");
        status= -1;
        errno = EINVAL;
        rcode = errno;
      }

      if ( !status && abs(rcd) == 1 ) {
        (*logfunc)(LOG_ERR,"No entry in mapping file for (%s,%s,%d,%d)\n", host,user,uid,gid);
        status= -1;
        errno=EACCES;
        rcode=errno;
      }
      else {
        (*logfunc)(LOG_DEBUG,"(%s,%s,%d,%d) mapped to %s(%d,%d)\n",
            host,user,uid,gid,to,to_uid,to_gid);
        uid = to_uid;
        gid = to_gid;
      }
    }
    if ( !status ) {
      /*
       * Trust root for stat() if trusted for any other privileged operation
       */
      rcode = 0;
      if (bet &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"FTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"WTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"RTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"XTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"STATTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"POPENTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"LINKTRUST")) < 0  &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"CHMODTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"CHOWNTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"MKDIRTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"RMDIRTRUST")) < 0 &&
          (status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0 ) {
        if (status == -2)
          (*logfunc)(LOG_ERR,"rstat(): uid %d not allowed to stat()\n",uid);
        else
          (*logfunc)(LOG_ERR,"rstat(): failed at check_user_perm(), rcode %d\n",rcode);
        memset(&statbuf,'\0',sizeof(statbuf));
        status = rcode;
      } else  {
        char ofilename[MAXFILENAMSIZE];
        strcpy(ofilename, filename);
        if (forced_filename != NULL || !check_path_whitelist(host, filename, rfio_all_perms, ofilename, sizeof(ofilename),1)) {
          status= ( stat(CORRECT_FILENAME(ofilename), &statbuf) < 0 ) ? errno : 0;
        } else {
          status = errno;
        }

        (*logfunc)(LOG_INFO,"rstat: stat(): file: %s for (%d,%d) status %d\n",CORRECT_FILENAME(filename),uid,gid,status);
      }
    }
  }

  p = rqstbuf;
  marshall_WORD(p, statbuf.st_dev);
  marshall_LONG(p, statbuf.st_ino);
  marshall_WORD(p, statbuf.st_mode);
  marshall_WORD(p, statbuf.st_nlink);
  marshall_WORD(p, statbuf.st_uid);
  marshall_WORD(p, statbuf.st_gid);
  marshall_LONG(p, statbuf.st_size);
  marshall_LONG(p, statbuf.st_atime);
  marshall_LONG(p, statbuf.st_mtime);
  marshall_LONG(p, statbuf.st_ctime);
  /*
   * Bug #2646. This is one of the rare cases when the errno
   * is returned in the status parameter.
   */
  if ( status == -1 && rcode > 0 ) status = rcode;
  marshall_LONG(p, status);
  marshall_LONG(p, statbuf.st_blksize);
  marshall_LONG(p, statbuf.st_blocks);
  if (netwrite_timeout(s,rqstbuf,8*LONGSIZE+5*WORDSIZE,RFIO_CTRL_TIMEOUT) != (8*LONGSIZE+5*WORDSIZE))  {
    (*logfunc)(LOG_ERR, "rstat: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

int     sraccess(int     s,
                 char    *host,
                 int     rt)
{
  char *p;
  int  status = 0;
  int  len ,uid, gid;
  int  mode;
  int rcode = 0;
  const char *perm_array[14] = { "OPENTRUST", "STATTRUST", "POPENTRUST", "LINKTRUST", "CHMODTRUST",
                                 "CHOWNTRUST", "MKDIRTRUST", "RMDIRTRUST", "RENAMETRUST",
                                 NULL, NULL, NULL, NULL, NULL };

  (void)rt;
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    /*
     * Reading stat request.
     */
    (*logfunc)(LOG_DEBUG,"raccess: reading %d bytes\n",len);
    if (netread_timeout(s,rqstbuf,len,RFIO_CTRL_TIMEOUT) != len) {
      (*logfunc)(LOG_ERR,"raccess: read(): %s\n",strerror(errno));
      return -1;
    }
    p= rqstbuf;
    *filename = '\0';
    status = 0;
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p,filename,MAXFILENAMSIZE)) == -1)
      rcode = SENAMETOOLONG;
    unmarshall_LONG(p,uid);
    unmarshall_LONG(p,gid);
    unmarshall_LONG(p,mode);

    if (((mode & ~(R_OK | W_OK | X_OK)) != 0) && (mode != F_OK)) {
      status = EINVAL;
      (*logfunc)(LOG_ERR,"raccess: wrong mode 0x%x\n", mode);
    }

    /* While performing tape operations multiple calls to change uid are issued. As
     * a result we ignore errors from setgroups if we are not running as the super-
     * user because it is a super-user only command. If we do not do this all tape
     * related activity will FAIL!!!
     */
    if ( (! status) && (((getuid() == 0) && (setgroups(0, NULL)<0)) || (setgid(gid)<0) || (setuid(uid)<0)) )  {
      status= errno;
      (*logfunc)(LOG_ERR,"raccess: unable to setuid,gid(%d,%d): %s, we are (uid=%d,gid=%d,euid=%d,egid=%d)\n",uid,gid,strerror(errno),(int) getuid(),(int) getgid(),(int) geteuid(),(int) getegid());
    }

    if (!status) {
      if ((mode & (R_OK | W_OK | X_OK)) != 0) {
        if ((mode & R_OK) == R_OK) {
          perm_array[9] = "RTRUST";
          if (
              (status=check_user_perm(&uid,&gid,host,&rcode,"RTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"STATTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"POPENTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"LINKTRUST")) < 0  &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"CHMODTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"CHOWNTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"MKDIRTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"RMDIRTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0
              ) {
            if (status == -2)
              (*logfunc)(LOG_ERR,"raccess(): uid %d not allowed to do access(R_OK)\n",uid);
            else
              (*logfunc)(LOG_ERR,"raccess(): failed at check_user_perm(), rcode %d\n",rcode);
          }
        }
        if ((! status) && ((mode & W_OK) == W_OK)) {
          perm_array[9] = "WTRUST";
          if (
              (status=check_user_perm(&uid,&gid,host,&rcode,"WTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"STATTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"POPENTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"LINKTRUST")) < 0  &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"CHMODTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"CHOWNTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"MKDIRTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"RMDIRTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0
              ) {
            if (status == -2)
              (*logfunc)(LOG_ERR,"raccess(): uid %d not allowed to do access(W_OK)\n",uid);
            else
              (*logfunc)(LOG_ERR,"raccess(): failed at check_user_perm(), rcode %d\n",rcode);
          }
        }
        if ((! status) && ((mode & X_OK) == X_OK)) {
          perm_array[9] = "XTRUST";
          if (
              (status=check_user_perm(&uid,&gid,host,&rcode,"XTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"STATTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"POPENTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"LINKTRUST")) < 0  &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"CHMODTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"CHOWNTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"MKDIRTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"RMDIRTRUST")) < 0 &&
              (status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0
              ) {
            if (status == -2)
              (*logfunc)(LOG_ERR,"raccess(): uid %d not allowed to do access(X_OK)\n",uid);
            else
              (*logfunc)(LOG_ERR,"raccess(): failed at check_user_perm(), rcode %d\n",rcode);
          }
        }
      } else {
        perm_array[9] = "FTRUST";
        perm_array[10] = "WTRUST";
        perm_array[11] = "RTRUST";
        perm_array[12] = "XTRUST";
        if (
            (status=check_user_perm(&uid,&gid,host,&rcode,"FTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"WTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"RTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"XTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"STATTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"POPENTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"LINKTRUST")) < 0  &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"CHMODTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"CHOWNTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"MKDIRTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"RMDIRTRUST")) < 0 &&
            (status=check_user_perm(&uid,&gid,host,&rcode,"RENAMETRUST")) < 0
            ) {
          if (status == -2)
            (*logfunc)(LOG_ERR,"raccess(): uid %d not allowed to do access(F_OK)\n",uid);
          else
            (*logfunc)(LOG_ERR,"raccess(): failed at check_user_perm(), rcode %d\n",rcode);
        }
      }
      if ( !status && forced_filename != NULL) {
        (*logfunc)(LOG_ERR,"Disallowing access() in forced filename mode\n");
        status= -1;
        errno=EACCES;
        rcode=errno;
      }
      if (status) {
        status = rcode;
      } else {
        char ofilename[MAXFILENAMSIZE];
        if (!check_path_whitelist(host, filename, perm_array, ofilename, sizeof(ofilename),1)) {
          status= ( access(ofilename, mode) < 0 ) ? errno : 0;
        } else {
          status = errno;
        }
        (*logfunc)(LOG_INFO,"raccess: filen: %s, mode %d for (%d,%d) status %d\n",filename,mode,uid,gid,status);
      }
    }
  }

  p = rqstbuf;
  /*
   * Bug #2646. This is one of the rare cases when the errno
   * is returned in the status parameter.
   */
  if ( status == -1 && rcode > 0 ) status = rcode;
  marshall_LONG(p, status);
  (*logfunc)(LOG_DEBUG, "raccess: sending back %d\n", status);
  if (netwrite_timeout(s,rqstbuf,LONGSIZE,RFIO_CTRL_TIMEOUT) != LONGSIZE)  {
    (*logfunc)(LOG_ERR, "raccess: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

int  srstatfs(int     s)
{
  int status = 0;
  int rcode = 0;
  int len;
  char *p;
  char path[MAXFILENAMSIZE];
  struct rfstatfs statfsbuf;

  memset(&statfsbuf, 0, sizeof(statfsbuf));  
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    (*logfunc)(LOG_DEBUG,"srstatfs(): reading %d bytes\n",len);
    if ((status = netread_timeout(s,rqstbuf,len,RFIO_CTRL_TIMEOUT)) != len) {
      (*logfunc)(LOG_ERR,"srstatfs(): read(): %s\n",strerror(errno));
      return -1;
    }

    p= rqstbuf;
    status = 0;
    *path = '\0';
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p,path,MAXFILENAMSIZE)) == -1) {
      rcode = SENAMETOOLONG;
      (*logfunc)(LOG_ERR,"srstatfs: path too long\n");
    } else {
      status = rfstatfs(path,&statfsbuf);
      rcode = errno;
      (*logfunc)(LOG_INFO,"srrstatfs: path : %s , status %d\n",path,status );
    }

    /*
     * Shipping the results
     */
  }

  p = rqstbuf;
  marshall_LONG( p, statfsbuf.bsize );
  marshall_LONG( p, statfsbuf.totblks );
  marshall_LONG( p, statfsbuf.freeblks );
  marshall_LONG( p, statfsbuf.totnods );
  marshall_LONG( p, statfsbuf.freenods );
  marshall_LONG( p, status );
  marshall_LONG( p, rcode );

  (*logfunc)(LOG_DEBUG, "srstatfs: sending back %d\n", status);
  if (netwrite_timeout(s,rqstbuf,7*LONGSIZE,RFIO_CTRL_TIMEOUT) != (7*LONGSIZE))  {
    (*logfunc)(LOG_ERR, "srstatfs: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return status;


}

int  sropen(int     s,
            int   rt,   /* Is it a remote site call ?   */
            char *host, /* Where the request comes from */
            int   bet) /* Version indicator: 0(old) or 1(new) */
{
  int   status;
  int   rcode = 0;
  char       *p;
  int        len;
  int         fd = -1;
  LONG    flags = 0;
  LONG    mode;
  int     uid,gid;
  WORD    mask, ftype, passwd = 0;
  WORD    mapping = 0;
  char    account[MAXACCTSIZE];           /* account string       */
  char    user[CA_MAXUSRNAMELEN+1];                       /* User name            */
  char    reqhost[MAXHOSTNAMELEN];
  char    vmstr[MAXVMSTRING];
  int  sock;
  int rc;
  char *pfn = NULL;

  p = rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p, len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    /*
     * Reading open request.
     */
    (*logfunc)(LOG_DEBUG,"ropen: reading %d bytes\n",len);
    memset(rqstbuf,'\0',BUFSIZ);
    if ((status = netread_timeout(s,rqstbuf,len,RFIO_CTRL_TIMEOUT)) != len) {
      (*logfunc)(LOG_ERR,"ropen: read(): %s\n",strerror(errno));
      return -1;
    }
    status = 0;
    p= rqstbuf;
    unmarshall_WORD(p, uid);
    unmarshall_WORD(p, gid);
    unmarshall_WORD(p, mask);
    unmarshall_WORD(p, ftype);
    unmarshall_LONG(p, flags);
    unmarshall_LONG(p, mode);
    *account = *filename = *user = *reqhost = *vmstr = '\0';
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p, account, MAXACCTSIZE)) == -1 )
      rcode = E2BIG;
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p, filename, MAXFILENAMSIZE)) == -1 )
      rcode = SENAMETOOLONG;
    if ((status == 0) && bet) {
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, user, CA_MAXUSRNAMELEN+1)) == -1 )
        rcode = E2BIG;
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, reqhost, MAXHOSTNAMELEN)) == -1 )
        rcode = E2BIG;
      unmarshall_LONG(p, passwd);
      unmarshall_WORD(p, mapping);
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, vmstr, sizeof(vmstr))) == -1 )
        rcode = E2BIG;
    }

    (*logfunc)(LOG_DEBUG,"vms string is %s\n", vmstr);
    if (bet)
      (*logfunc)(LOG_DEBUG,"Opening file %s for remote user: %s\n",CORRECT_FILENAME(filename),user);
    if (rt)
      (*logfunc)(LOG_DEBUG,"Mapping : %s\n",mapping ? "yes" : "no" );
    if (rt && !mapping) {
      (*logfunc)(LOG_DEBUG,"passwd : %d\n",passwd);
      (*logfunc)(LOG_DEBUG,"uid: %d\n",uid );
      (*logfunc)(LOG_DEBUG,"gid: %d\n",gid );
    }

    /*
     * Someone in the site has tried to specify (uid,gid) directly !
     */
    if ( (status == 0) && bet && !mapping && !rt) {
      (*logfunc)(LOG_INFO,"attempt to make non-mapped I/O and modify uid or gid !\n");
      errno=EACCES;
      rcode=errno;
      status= -1;
    }

    if ( rt ) {
      (*logfunc)(LOG_ALERT, "rfio: connection %s mapping by %s(%d,%d) from %s",(mapping ? "with" : "without"),user,uid,gid,host);
    }

    /*
     * MAPPED mode: user will be mapped to user "to"
     */
    if ( !status && rt && mapping ) {
      char to[100];
      int rcd,to_uid,to_gid;

      (*logfunc)(LOG_DEBUG,"Mapping (%s, %d, %d) \n",user, uid, gid );
      if ( (rcd = get_user(host,user,uid,gid,to,&to_uid,&to_gid)) == -ENOENT ) {
        (*logfunc)(LOG_ERR,"sropen(): get_user() error opening mapping file\n");
        status = -1;
        errno = EINVAL;
        rcode = SEHOSTREFUSED;
      }

      else if ( abs(rcd) == 1 ) {
        (*logfunc)(LOG_ERR,"No entry found in mapping file for (%s,%s,%d,%d)\n", host,user,uid,gid);
        status = -1;
        errno = EACCES;
        rcode = SEHOSTREFUSED;
      }
      else {
        (*logfunc)(LOG_DEBUG,"(%s,%s,%d,%d) mapped to %s(%d,%d)\n",
            host,user,uid,gid,to,to_uid,to_gid);
        uid = to_uid;
        gid = to_gid;
        if ( uid < 100 || gid < 100 ) {
          errno = EACCES;
          status = -1;
          rcode = SEHOSTREFUSED;
        }
      }
    }
    /*
     * DIRECT access: the user specifies uid & gid by himself
     */
    if ( !status && rt && !mapping ){
      char * rtuser;
      if ( (rtuser=getconfent ("RTUSER","CHECK",0) ) == NULL || ! strcmp (rtuser,"YES") )
        {
          /* Port is also passwd */
            if( (sock = connecttpread(reqhost, passwd)) >= 0 && !checkkey(sock, passwd) )
              {
                status= -1;
                errno = EACCES;
                rcode= errno;
                (*logfunc)(LOG_ERR,"ropen: DIRECT mapping : permission denied\n");
              }
            if( sock < 0 )
              {
                status= -1;
                (*logfunc)(LOG_ERR,"ropen: DIRECT mapping failed: Couldn't connect %s\n", reqhost);
                rcode = EACCES;
              }
        }
      else
        (*logfunc)(LOG_INFO ,"Any DIRECT rfio request from out of site is authorized\n");
    }
    if ( !status ) {
      int need_user_check = 1;

      (*logfunc)(LOG_DEBUG, "ropen: uid %d gid %d mask %o ftype %d flags %d mode %d\n",uid, gid, mask, ftype, flags, mode);
      (*logfunc)(LOG_DEBUG, "ropen: account: %s\n", account);
      (*logfunc)(LOG_DEBUG, "ropen: filename: %s\n", CORRECT_FILENAME(filename));
      (*logfunc)(LOG_INFO, "ropen(%s,0X%X,0X%X) for (%d,%d)\n",CORRECT_FILENAME(filename),flags,mode,uid,gid);
      (void) umask((mode_t) CORRECT_UMASK(mask));

      rc = rfio_handle_open(CORRECT_FILENAME(filename),
                            ntohopnflg(flags),
                            mode,
                            uid,
                            gid,
                            &pfn,
                            &handler_context,
                            &need_user_check);
      if (rc < 0) {
        char alarmbuf[1024];
        sprintf(alarmbuf,"sropen(): %s",CORRECT_FILENAME(filename));
        (*logfunc)(LOG_DEBUG, "sropen: rfio_handler_open refused open: %s\n", sstrerror(serrno));
        rcode = serrno;
        rfio_alrm(rcode,alarmbuf);
      }

      if (need_user_check && ((status=check_user_perm(&uid,&gid,host,&rcode,(((ntohopnflg(flags)) & (O_WRONLY|O_RDWR)) != 0) ? "WTRUST" : "RTRUST")) < 0) &&
          ((status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0) ) {
        if (status == -2)
          (*logfunc)(LOG_ERR,"ropen(): uid %d not allowed to open()\n",uid);
        else
          (*logfunc)(LOG_ERR,"ropen(): failed at check_user_perm(), rcode %d\n",rcode);
        status = -1;
      }
      else
        {
          const char *perm_array[3];
          char ofilename[MAXFILENAMSIZE];
          perm_array[0] = (((ntohopnflg(flags)) & (O_WRONLY|O_RDWR)) != 0) ? "WTRUST" : "RTRUST";
          perm_array[1] = "OPENTRUST";
          perm_array[2] = NULL;

          strcpy(ofilename, filename);
          fd = -1;
          if (forced_filename!=NULL || !check_path_whitelist(host, filename, perm_array, ofilename, sizeof(ofilename),1)) {
            fd = open(CORRECT_FILENAME(ofilename), ntohopnflg(flags),
                      ((forced_filename != NULL) && (((ntohopnflg(flags)) & (O_WRONLY|O_RDWR)) != 0)) ? 0644 : mode);
            (*logfunc)(LOG_DEBUG, "ropen: open(%s,%d,%d) returned %x (hex)\n", CORRECT_FILENAME(ofilename), flags, mode, fd);
          }
          if (fd < 0) {
            char alarmbuf[1024];
            sprintf(alarmbuf,"sropen(): %s",CORRECT_FILENAME(filename));
            status= -1;
            rcode= errno;
            (*logfunc)(LOG_DEBUG,"ropen: open: %s\n",strerror(errno));
            rfio_alrm(rcode,alarmbuf);
          }
          else {
            /*
             * Getting current offset
             */
            status= lseek(fd,0L,SEEK_CUR);
            (*logfunc)(LOG_DEBUG,"ropen: lseek(%d,0,SEEK_CUR) returned %x (hex)\n",fd,status);
            if ( status < 0 ) rcode= errno;
          }
        }
    }
  }

  if(pfn != NULL) free (pfn);

  /*
   * Sending back status.
   */
  p= rqstbuf;
  marshall_WORD(p,RQST_OPEN);
  marshall_LONG(p,status);
  marshall_LONG(p,rcode);
  marshall_LONG(p,0);
  (*logfunc)(LOG_DEBUG, "ropen: sending back status(%d) and errno(%d)\n",status,rcode);
  if (netwrite_timeout(s,rqstbuf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+3*LONGSIZE))  {
    (*logfunc)(LOG_ERR,"ropen: netwrite_timeout(): %s\n",strerror(errno));
    if (fd >=0) close(fd);
    return -1;
  }
  return fd;
}

/* useful function to answer the client */
int rfio_calls_answer_client_internal
(char* rqstbuf, int code, int status, int s) {
  char* p;
  p = rqstbuf;
  marshall_WORD(p,RQST_WRITE);
  marshall_LONG(p,status);
  marshall_LONG(p,code);
  marshall_LONG(p,0);
  (*logfunc)(LOG_DEBUG, "srwrite: status %d, rcode %d\n", status, code);
  if ( netwrite_timeout(s,rqstbuf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != WORDSIZE+3*LONGSIZE ) {
    (*logfunc)(LOG_ERR, "srwrite: netwrite(): %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

int srwrite(int     s,
            struct rfiostat * infop,
            int     fd)
{
  int  status;         /* Return code               */
  int  rcode;          /* To send back errno        */
  int  how;            /* lseek mode                */
  int  offset;         /* lseek offset              */
  int  size;           /* Requeste write size       */
  char *p;             /* Pointer to buffer         */

  if (first_write) {
    first_write = 0;
    status = rfio_handle_firstwrite(handler_context);
    if (status != 0) {
      (*logfunc)(LOG_ERR, "srwrite: rfio_handle_firstwrite(): %s\n", strerror(serrno));
      rfio_calls_answer_client_internal(rqstbuf, serrno, status, s);
      return -1;
    }
  }

  /*
   * Receiving request,
   */
  (*logfunc)(LOG_DEBUG, "rwrite(%d, %d)\n",s, fd);
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p, size);
  unmarshall_LONG(p,how);
  unmarshall_LONG(p,offset);
  (*logfunc)(LOG_DEBUG, "rwrite(%d, %d): size %d, how %d offset %d\n",s,fd,size,how,offset);
  /*
   * Checking if buffer is large enough.
   */
  if (iobufsiz < size)     {
    int     optval;        /* setsockopt opt value */

    if (iobufsiz > 0)       {
      (*logfunc)(LOG_DEBUG, "rwrite(): freeing %x\n",iobuffer);
      (void) free(iobuffer);
    }
    if ((iobuffer = malloc(size)) == NULL)    {
      rfio_calls_answer_client_internal(rqstbuf, errno, -1, s);
      return -1;
    }
    iobufsiz = size;
    optval = (iobufsiz > 64 * 1024) ? iobufsiz : (64 * 1024);
    (*logfunc)(LOG_DEBUG, "rwrite(): allocated %d bytes at %x\n",size,iobuffer);
    if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&optval, sizeof(optval)) == -1)
      (*logfunc)(LOG_ERR, "rwrite(): setsockopt(SO_RCVBUF): %s\n",strerror(errno));
    else
      (*logfunc)(LOG_DEBUG, "rwrite(): setsockopt(SO_RCVBUF): %d\n",optval);
  }
  /*
   * Reading data on the network.
   */
  p= iobuffer;
  if (netread_timeout(s,p,size,RFIO_DATA_TIMEOUT) != size) {
    (*logfunc)(LOG_ERR, "rwrite: read(): %s\n", strerror(errno));
    return -1;
  }
  /*
   * lseek() if needed.
   */
  if ( how != -1 ) {
    (*logfunc)(LOG_DEBUG,"rwrite(%d,%d): lseek(%d,%d,%d)\n",s,fd,fd,offset,how);
    infop->seekop++;
    if ( (status= lseek(fd,offset,how)) == -1 ) {
      rfio_calls_answer_client_internal(rqstbuf, errno, status, s);
      return -1;
    }
  }
  /*
   * Writing data on disk.
   */
  infop->wnbr+= size;
  status = write(fd,p,size);
  rcode= ( status < 0 ) ? errno : 0;

  if ( status < 0 ) {
    char alarmbuf[1024];
    sprintf(alarmbuf,"srwrite(): %s",filename);
    rfio_alrm(rcode,alarmbuf);
  }
  if (rfio_calls_answer_client_internal(rqstbuf, rcode, status, s) < 0) {
    return -1;
  }
  return status;
}

int srread(int     s,
           struct rfiostat * infop,
           int     fd)
{
  int  status;        /* Return code                */
  int  rcode;         /* To send back errno         */
  int  how;           /* lseek mode                 */
  int  offset;        /* lseek offset               */
  int  size;          /* Requested read size        */
  char *p;            /* Pointer to buffer          */
  int  msgsiz;        /* Message size               */

  /*
   * Receiving request.
   */
  (*logfunc)(LOG_DEBUG, "rread(%d, %d)\n",s, fd);
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p, size);
  unmarshall_LONG(p,how);
  unmarshall_LONG(p,offset);
  /*
   * lseek() if needed.
   */
  if ( how != -1 ) {
    (*logfunc)(LOG_DEBUG,"rread(%d,%d): lseek(%d,%d,%d)\n",s,fd,fd,offset,how);
    infop->seekop++;
    if ( (status= lseek(fd,offset,how)) == -1 ) {
      rcode= errno;
      p= rqstbuf;
      marshall_WORD(p,RQST_READ);
      marshall_LONG(p,status);
      marshall_LONG(p,rcode);
      marshall_LONG(p,0);
      if ( netwrite_timeout(s,rqstbuf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+3*LONGSIZE) ) {
        (*logfunc)(LOG_ERR, "rread: netwrite_timeout(): %s\n", strerror(errno));
        return -1;
      }
      return -1;
    }
  }
  /*
   * Allocating buffer if not large enough.
   */
  (*logfunc)(LOG_DEBUG, "rread(%d, %d): checking buffer size %d\n", s, fd, size);
  if (iobufsiz < (size+WORDSIZE+3*LONGSIZE))     {
    int     optval;        /* setsockopt opt value        */

    if (iobufsiz > 0)       {
      (*logfunc)(LOG_DEBUG, "rread(): freeing %x\n",iobuffer);
      (void) free(iobuffer);
    }
    if ((iobuffer = malloc(size+WORDSIZE+3*LONGSIZE)) == NULL)    {
      status= -1;
      rcode= errno;
      (*logfunc)(LOG_ERR, "rread: malloc(): %s\n", strerror(errno));
      p= rqstbuf;
      marshall_WORD(p,RQST_READ);
      marshall_LONG(p,status);
      marshall_LONG(p,rcode);
      marshall_LONG(p,0);
      if ( netwrite_timeout(s,rqstbuf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+3*LONGSIZE) ) {
        (*logfunc)(LOG_ERR, "rread: netwrite_timeout(): %s\n", strerror(errno));
        return -1;
      }
      return -1;
    }
    iobufsiz = size + WORDSIZE + 3*LONGSIZE;
    (*logfunc)(LOG_DEBUG, "rread(): allocated %d bytes at %x\n",size,iobuffer);
    optval = (iobufsiz > 64 * 1024) ? iobufsiz : (64 * 1024);
    if( setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval)) == -1 )
      (*logfunc)(LOG_ERR, "rread(): setsockopt(SO_SNDBUF): %s\n",strerror(errno));
    (*logfunc)(LOG_DEBUG, "rread(): setsockopt(SO_SNDBUF): %d\n",optval);
  }
  p = iobuffer + WORDSIZE + 3*LONGSIZE;
  status = read(fd, p, size);
  if ( status < 0 ) {
    char alarmbuf[1024];
    sprintf(alarmbuf,"srread(): %s",filename);
    rcode= errno;
    msgsiz= WORDSIZE+3*LONGSIZE;
    rfio_alrm(rcode,alarmbuf);
  }  else  {
    rcode= 0;
    infop->rnbr+= status;
    msgsiz= status+WORDSIZE+3*LONGSIZE;
  }
  p= iobuffer;
  marshall_WORD(p,RQST_READ);
  marshall_LONG(p,status);
  marshall_LONG(p,rcode);
  marshall_LONG(p,status);
  (*logfunc)(LOG_DEBUG, "rread: returning status %d, rcode %d\n", status, rcode);
  if (netwrite_timeout(s,iobuffer,msgsiz,RFIO_CTRL_TIMEOUT) != msgsiz)  {
    (*logfunc)(LOG_ERR, "rread: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return status;
}

int srreadahead(int     s,
                struct rfiostat *infop,
                int     fd)
{
  int  status;         /* Return code                 */
  int  rcode;          /* To send back errno          */
  int  how;            /* lseek mode                  */
  int  offset;         /* lseek offset                */
  int  size;           /* Requested read size         */
  int  first;          /* First block sent            */
  char *p;             /* Pointer to buffer           */

  /*
   * Receiving request.
   */
  (*logfunc)(LOG_DEBUG, "rreadahead(%d, %d)\n",s, fd);
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,size);
  unmarshall_LONG(p,how);
  unmarshall_LONG(p,offset);
  /*
   * lseek() if needed.
   */
  if ( how != -1 ) {
    (*logfunc)(LOG_DEBUG,"rread(%d,%d): lseek(%d,%d,%d)\n",s,fd,fd,offset,how);
    infop->seekop++;
    if ( (status= lseek(fd,offset,how)) == -1 ) {
      rcode= errno;
      p= iobuffer;
      marshall_WORD(p,RQST_FIRSTREAD);
      marshall_LONG(p,status);
      marshall_LONG(p,rcode);
      if ( netwrite_timeout(s,iobuffer,iobufsiz,RFIO_CTRL_TIMEOUT) != iobufsiz ) {
        (*logfunc)(LOG_ERR, "rreadahead(): netwrite_timeout(): %s\n", strerror(errno));
        return -1;
      }
      return status;
    }
  }
  /*
   * Allocating buffer if not large enough.
   */
  (*logfunc)(LOG_DEBUG, "rreadahead(%d, %d): checking buffer size %d\n", s, fd, size);
  if (iobufsiz < (size+WORDSIZE+3*LONGSIZE))     {
    int     optval;        /* setsockopt opt value */

    if (iobufsiz > 0)       {
      (*logfunc)(LOG_DEBUG, "rreadahead(): freeing %x\n",iobuffer);
      (void) free(iobuffer);
    }
    if ((iobuffer = malloc(size+WORDSIZE+3*LONGSIZE)) == NULL)    {
      (*logfunc)(LOG_ERR, "rreadahead: malloc(): %s\n", strerror(errno));
      (void) close(s);
      return -1;
    }
    iobufsiz = size+WORDSIZE+3*LONGSIZE;
    optval = (iobufsiz > 64 * 1024) ? iobufsiz : (64 * 1024);
    (*logfunc)(LOG_DEBUG, "rreadahead(): allocated %d bytes at %x\n",iobufsiz,iobuffer);
    if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&optval, sizeof(optval)) == -1)
      (*logfunc)(LOG_ERR, "rreadahead(): setsockopt(SO_SNDBUF): %s\n",strerror(errno));
    else
      (*logfunc)(LOG_DEBUG, "rreadahead(): setsockopt(SO_SNDBUF): %d\n",optval);
  }
  /*
   * Reading data and sending it.
   */
  for(first= 1;;first= 0) {
    fd_set fds;
    struct timeval timeout;

    /*
     * Has a new request arrived ?
     */
    FD_ZERO(&fds);
    FD_SET(s,&fds);
    timeout.tv_sec = 0;
    timeout.tv_usec= 0;
    if ( select(FD_SETSIZE,&fds,(fd_set *)0,(fd_set *)0,&timeout) == -1 ) {
      (*logfunc)(LOG_ERR,"rreadahead(): select(): %s\n",strerror(errno));
      return -1;
    }
    if ( FD_ISSET(s,&fds) ) {
      (*logfunc)(LOG_DEBUG,"rreadahead(): returns because of new request\n");
      return 0;
    }
    /*
     * Reading disk ...
     */
    p= iobuffer + WORDSIZE + 3*LONGSIZE;
    status = read(fd,p,size);
    if (status < 0)        {
      rcode= errno;
      iobufsiz= WORDSIZE+3*LONGSIZE;
    }
    else {
      rcode= 0;
      infop->rnbr+= status;
      iobufsiz = status+WORDSIZE+3*LONGSIZE;
    }
    (*logfunc)(LOG_DEBUG, "rreadahead: status %d, rcode %d\n", status, rcode);
    /*
     * Sending data.
     */
    p= iobuffer;
    marshall_WORD(p,(first)?RQST_FIRSTREAD:RQST_READAHEAD);
    marshall_LONG(p,status);
    marshall_LONG(p, rcode);
    marshall_LONG(p, status);
    if ( netwrite_timeout(s, iobuffer, iobufsiz, RFIO_CTRL_TIMEOUT) != iobufsiz ) {
      (*logfunc)(LOG_ERR, "rreadahead(): netwrite_timeout(): %s\n", strerror(errno));
      return -1;
    }
    /*
     * The end of file has been reached
     * or an error was encountered.
     */
    if ( status != size ) {
      return 0;
    }
  }
}

int   srclose(int     s,
              struct rfiostat * infop,
              int     fd)
{
  int status;
  int  rcode;
  char   * p;
  char tmpbuf[21], tmpbuf2[21];
  struct stat filestat;
  int ret;

  (*logfunc)(LOG_INFO,"rclose(%d,%d): %d read, %d readahead, %d write, %d flush, %d stat, %d lseek and %d preseek\n",
      s, fd, infop->readop, infop->aheadop, infop->writop, infop->flusop, infop->statop,
      infop->seekop, infop->presop);
  (*logfunc)(LOG_INFO,"rclose(%d,%d): %s bytes read and %s bytes written\n",
      s, fd, u64tostr(infop->rnbr,tmpbuf,0), u64tostr(infop->wnbr,tmpbuf2,0));

  /* sync the file to be sure that filesize in correct in following stats.
     this is needed by some ext3 bug/feature
     Still ignore the output of fsync */
  fsync(fd);

  /* Stat the file to be able to provide that information
     to the close handler */
  memset(&filestat,0,sizeof(struct stat));
  fstat(fd, &filestat);

  status = close(fd);
  rcode = ( status < 0 ) ? errno : 0;
  if (iobufsiz > 0)       {
    (*logfunc)(LOG_DEBUG,"rclose(): freeing %x\n",iobuffer);
    (void) free(iobuffer);
  }
  iobufsiz= 0;
  ret=rfio_handle_close(handler_context, &filestat, rcode);
  if (ret<0){
    (*logfunc)(LOG_ERR, "srclose: rfio_handle_close failed\n");
    if (status>=0) {
      /* we have to set status = -1 and fill rcode with serrno, that should be filled by rfio_handle_close */
      status=-1;
      rcode=serrno;
    }
    /* we already have status<0 in error case here and will send a reply for client with rcode */
  }
  p= rqstbuf;
  marshall_WORD(p,RQST_CLOSE);
  marshall_LONG(p,status);
  marshall_LONG(p,rcode);
  marshall_LONG(p,0);
  if (netwrite_timeout(s,rqstbuf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+3*LONGSIZE))  {
    (*logfunc)(LOG_ERR, "rclose: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return status;
}

int  srpclose(int     s,
              FILE    *fs)
{
  int  status;
  char *p;

  errno = 0;
  status = pclose(fs);
  (*logfunc)(LOG_DEBUG,"rpclose(%x) returns %d\n",fs,status);
  /*
   * status returns the command's error code
   */
  p= rqstbuf;
  marshall_LONG(p,status);
  marshall_LONG(p,errno);
  if (netwrite_timeout(s,rqstbuf,2*LONGSIZE,RFIO_CTRL_TIMEOUT) != (2*LONGSIZE))  {
    (*logfunc)(LOG_ERR, "rpclose: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return status;
}

int srfread(int     s,
            FILE    *fp)
{
  int  status = 0;
  int  rcode = 0;
  int  size, items;
  char *p;

  p = rqstbuf +2*WORDSIZE;
  (*logfunc)(LOG_DEBUG, "rfread(%x)\n",fp);
  unmarshall_LONG(p, size);
  unmarshall_LONG(p, items);
  (*logfunc)(LOG_DEBUG,"rfread(%d, %x): size %d items %d\n", s, fp, size, items);
  if (iobufsiz < items*size) {
    if (iobufsiz > 0) {
      (*logfunc)(LOG_DEBUG, "rfread(): freeing %x\n",iobuffer);
      (void) free(iobuffer);
    }
    if ((iobuffer = malloc(items*size)) == NULL)    {
      (*logfunc)(LOG_ERR, "rfread: malloc(): %s\n", strerror(errno));
      return(-1);
    }
    iobufsiz = items*size;
    (*logfunc)(LOG_DEBUG, "rfread(): allocated %d bytes at %x\n",items*size,iobuffer);
  }
  errno = 0;
  status = fread(iobuffer, size, items, fp);
  if ( status == 0 ) {
    rcode= errno;
  }
  (*logfunc)(LOG_DEBUG, "rfread : status %d, rcode %d\n", status, rcode);
  p = rqstbuf;
  marshall_LONG(p, status);
  marshall_LONG(p, rcode);
  if (netwrite_timeout(s, rqstbuf, 2*LONGSIZE, RFIO_CTRL_TIMEOUT) != (2*LONGSIZE))  {
    (*logfunc)(LOG_ERR, "rfread : netwrite_timeout(): %s\n", strerror(errno));
    return(-1);
  }
  if ( status > 0 ) {
    if (netwrite_timeout(s,iobuffer, status*size, RFIO_CTRL_TIMEOUT) != (status*size))  {
      (*logfunc)(LOG_ERR, "rfread: netwrite_timeout(): %s\n",strerror(errno));
      return(-1);
    }
  }
  return(status);
}

int srfwrite(int     s,
             FILE    *fp)
{
  int     status = 0;
  int     rcode = 0;
  int     size, items;
  char    *ptr;
  char    *p = rqstbuf;

  p =  rqstbuf +2*WORDSIZE;
  (*logfunc)(LOG_DEBUG, "rfwrite(%x)\n",fp);
  unmarshall_LONG(p, size);
  unmarshall_LONG(p, items);
  (*logfunc)(LOG_DEBUG, "rfwrite(%d,%x): size %d items %d\n",s,fp,size,items);
  if (iobufsiz < items*size)     {
    if (iobufsiz > 0)       {
      (*logfunc)(LOG_DEBUG, "rfwrite(): freeing %x\n",iobuffer);
      (void) free(iobuffer);
    }
    if ((iobuffer = malloc(items*size)) == NULL)    {
      (*logfunc)(LOG_ERR, "rfwrite: malloc(): %s\n", strerror(errno));
      return(-1);
    }
    iobufsiz = items*size;
    (*logfunc)(LOG_DEBUG, "rfwrite(): allocated %d bytes at %x\n",items*size,iobuffer);
  }
  ptr = iobuffer;
  (*logfunc)(LOG_DEBUG, "rfwrite: reading %d bytes\n",items*size);
  if (netread_timeout(s, iobuffer, items*size, RFIO_CTRL_TIMEOUT) != (items*size))       {
    (*logfunc)(LOG_ERR, "rfwrite: read(): %s\n", strerror(errno));
    return(-1);
  }
  if ( (status = fwrite( ptr, size, items, fp)) == 0 )
    rcode= errno;
  (*logfunc)(LOG_DEBUG, "rfwrite: status %d, rcode %d\n", status, rcode);
  p = rqstbuf;
  marshall_LONG(p, status);
  marshall_LONG(p, rcode);
  if (netwrite_timeout(s, rqstbuf, 2*LONGSIZE, RFIO_CTRL_TIMEOUT) != (2*LONGSIZE))  {
    (*logfunc)(LOG_ERR, "rfwrite: netwrite_timeout(): %s\n", strerror(errno));
    return(-1);
  }
  return(status);
}

int     srfstat(int     s,
                struct rfiostat *infop,
                int     fd)
{
  int  status;
  int  rcode;
  int  msgsiz;
  int  how;
  int  offset;
  char *p;
  struct stat  statbuf;

  (void)infop;

  (*logfunc)(LOG_DEBUG, "rfstat(%d, %d)\n",s,fd);
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,offset);
  unmarshall_LONG(p,how);
  /*
   * lseek() if needed.
   */
  if ( how != -1 ) {
    (*logfunc)(LOG_DEBUG,"rread(%d,%d): lseek(%d,%d,%d)\n",s,fd,fd,offset,how);
    if ( (status= lseek(fd,offset,how)) == -1 ) {
      rcode= errno;
      p= rqstbuf;
      marshall_WORD(p,RQST_FSTAT);
      marshall_LONG(p,status);
      marshall_LONG(p,rcode);
      if ( netwrite_timeout(s,rqstbuf,6*LONGSIZE+6*WORDSIZE,RFIO_CTRL_TIMEOUT) != (6*LONGSIZE+6*WORDSIZE) ) {
        (*logfunc)(LOG_ERR, "rreadahead(): netwrite_timeout(): %s\n", strerror(errno));
        return -1;
      }
      return status;
    }
  }
  /*
   * Issuing the fstat()
   */
  status= fstat(fd, &statbuf);
  rcode= errno;
  msgsiz= 5*WORDSIZE + 5*LONGSIZE;
  p = rqstbuf;
  marshall_WORD(p, RQST_FSTAT);
  marshall_LONG(p, status);
  marshall_LONG(p,  rcode);
  marshall_LONG(p, msgsiz);
  marshall_WORD(p, statbuf.st_dev);
  marshall_LONG(p, statbuf.st_ino);
  marshall_WORD(p, statbuf.st_mode);
  marshall_WORD(p, statbuf.st_nlink);
  marshall_WORD(p, statbuf.st_uid);
  marshall_WORD(p, statbuf.st_gid);
  marshall_LONG(p, statbuf.st_size);
  marshall_LONG(p, statbuf.st_atime);
  marshall_LONG(p, statbuf.st_mtime);
  marshall_LONG(p, statbuf.st_ctime);
  (*logfunc)(LOG_DEBUG, "rfstat: sending back %d\n",status);
  if (netwrite_timeout(s,rqstbuf,8*LONGSIZE+6*WORDSIZE,RFIO_CTRL_TIMEOUT) != (8*LONGSIZE+6*WORDSIZE))  {
    (*logfunc)(LOG_ERR,"rfstat: netwrite_timeout(): %s\n",strerror(errno));
    return -1;
  }
  return 0;
}

int srlseek(int     s,
            struct rfiostat *infop,
            int     fd)
{
  int  status;
  int  rcode;
  int  offset;
  int  how;
  char *p;

  (void)infop;
  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,offset);
  unmarshall_LONG(p,how);
  (*logfunc)(LOG_DEBUG,"rlseek(%d, %d): offset %d, how: %x\n",s,fd,offset,how);
  status = lseek(fd, offset, how);
  rcode= ( status < 0 ) ? errno : 0;
  (*logfunc)(LOG_DEBUG,"rlseek: status %d, rcode %d\n",status,rcode);
  p= rqstbuf;
  marshall_WORD(p,RQST_LSEEK);
  marshall_LONG(p,status);
  marshall_LONG(p,rcode);
  marshall_LONG(p,0);
  if (netwrite_timeout(s,rqstbuf,WORDSIZE+3*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+3*LONGSIZE))  {
    (*logfunc)(LOG_ERR,"rlseek: netwrite_timeout(): %s\n",strerror(errno));
    return -1;
  }
  return status;
}

/*
 * check if trusted host.
 */
static int chksuser(int uid,                 /* uid of caller */
                    int gid,                /* gid of caller */
                    char *hostname,        /* caller's host name */
                    int *ptrcode,                /* Return code */
                    char *permstr)                /* permission string for the request */
{

  char ptr[BUFSIZ];
  int found = 0;
  char *cp , *p;

  (void)gid;
  if ( uid < 100  ) {
    if ( permstr != NULL && hostname != NULL && (p=getconfent("RFIOD", permstr, 1)) != NULL ) {
      strcpy(ptr,p);
      for (cp=strtok(ptr,"\t ");cp!=NULL;cp=strtok(NULL,"\t ")) {
        if ( !strcmp(hostname,cp) ) {
          found ++;
          break;
        }
      }
    }
    if (!found) {
      *ptrcode = EACCES;
      (*logfunc)(LOG_ERR,"chksuser():uid < 100: No %s.\n",permstr);
      return -1;
    }
    else
      (*logfunc)(LOG_INFO, "chksuser():root authorized from %s\n",hostname);
  }
  return 0;
}


static int chk_newacct(struct passwd *pwd,
                       gid_t gid)
{
  char buf[BUFSIZ];
  char acct[MAXACCTSIZE];
  char * def_acct;
  struct group * gr;
  char * getacctent();

  /* get default account */
  if ( getacctent(pwd,NULL,buf,sizeof(buf)) == NULL )
    return -1;
  if ( strtok(buf,":") == NULL || (def_acct= strtok(NULL,":")) == NULL )
    return -1;
  if ( strlen(def_acct) == 6 && *(def_acct+3) == '$' &&   /* uuu$gg */
       (gr= getgrgid(gid))
       ) {
    strncpy(acct,def_acct,4);
    strcpy(acct+4,gr->gr_name);    /* new uuu$gg */
    if ( getacctent(pwd,acct,buf,sizeof(buf)) )
      return 0;      /* newacct was executed */
  }
  acct[0]= '\0';
  return -1;
}

int chsgroup(struct passwd *pw,
             int gid)
{
  struct group *gr;
  char **membername;
  int found;

  if ( pw != NULL && pw->pw_gid != (gid_t)gid ) {
    found = 0;
    (void) setgrent();
    while ((gr = getgrent()))
      {
        if ( gr->gr_gid != (gid_t)gid ) continue;
        for ( membername = gr->gr_mem; membername && *membername; membername++ )
          if ( !strcmp(*membername,pw->pw_name) ) {
            found = 1;
            break;
          }
        if ( found ) break;
      }
    endgrent();
    if ( !found && chk_newacct(pw,(gid_t)gid) ) return(-1);
  }
  else if ( pw == NULL ) return(-1);
  return(0);
}

/*
 * makes the setgid() and setuid(). Returns -1 if error , -2 if unauthorized.
 */
int chsuser(int uid,                /* uid of caller                     */
            int gid,                /* gid of caller                     */
            char *hostname,         /* caller's host name                */
            int *ptrcode,           /* Return code                       */
            char *permstr)          /* permission string for the request */
{

  struct passwd *pw;

  if (Csec_service_type < 0) {
    uid = peer_uid;
    gid = peer_gid;
  }

  if ( chksuser(uid,gid,hostname,ptrcode,permstr) < 0 )
    return -2;
  if ( uid >=100 && ( (pw = getpwuid((uid_t)uid)) == NULL
                      || chsgroup(pw,gid)
                      ))
    {
      *ptrcode = EACCES;
      (*logfunc)(LOG_ERR,"chsuser(): user (%d,%d) does not exist at local host\n",uid,gid);
      return -2;
    }
  /* While performing tape operations multiple calls to change uid are issued. As
   * a result we ignore errors from setgroups if we are not running as the super-
   * user because it is a super-user only command. If we do not do this all tape
   * related activity will FAIL!!!
   */
  if ( ((getuid() == 0) && (setgroups(0, NULL)<0)) || (setgid((gid_t)gid)<0) || (setuid((uid_t)uid)<0) )  {
    *ptrcode = errno;
    (*logfunc)(LOG_ERR,"chsuser(): unable to setuid,gid(%d,%d): %s, we are (uid=%d,gid=%d,euid=%d,egid=%d)\n",uid,gid,strerror(errno),(int) getuid(),(int) getgid(),(int) geteuid(),(int) getegid());
    return -2;
  }
  return 0;
}

/*
 * makes the setgid() and setuid(). Returns -1 if error , -2 if unauthorized.
 */
int check_user_perm(int *uid,               /* uid of caller                     */
                    int *gid,               /* gid of caller                     */
                    char *hostname,         /* caller's host name                */
                    int *ptrcode,           /* Return code                       */
                    char *permstr)          /* permission string for the request */
{
  if (Csec_service_type < 0) {
    *uid = peer_uid;
    *gid = peer_gid;
  }

  return(ignore_uid_gid != 0 ? 0 : chsuser(*uid,*gid,hostname,ptrcode,permstr));
}


int  sropen_v3(int     s,
               int     rt,             /* Is it a remote site call ?          */
               char    *host,          /* Where the request comes from        */
               int     bet)            /* Version indicator: 0(old) or 1(new) */
{
  int  status;
  int  rcode = 0;
  char  *p;
  int  len;
  int  fd = -1;
  LONG flags, mode;
  int uid,gid;
  WORD mask, ftype, passwd = 0;
  WORD mapping = 0;
  char account[MAXACCTSIZE];           /* account string       */
  char user[CA_MAXUSRNAMELEN+1];       /* User name            */
  char reqhost[MAXHOSTNAMELEN];
  char vmstr[MAXVMSTRING];
  const char *value = NULL;
  char *buf = NULL;
  char *dp  = NULL;
  char *dp2 = NULL;
  long low_port  = RFIO_LOW_PORT_RANGE;
  long high_port = RFIO_HIGH_PORT_RANGE;
  int sock, data_s = -1;
  int port;
  socklen_t fromlen;
  socklen_t size_sin;
  struct sockaddr_in sin, from;
  extern int max_rcvbuf;
  extern int max_sndbuf;
  int yes;
  struct timeval tv;

  /* Initialization of global variables */
  ctrl_sock = s;
  first_write = 1;
  first_read = 1;
  memset(&sin, 0, sizeof(sin));
  /* Init myinfo to zeros */
  myinfo.readop = myinfo.writop = myinfo.flusop = myinfo.statop = myinfo.seekop
    = myinfo.presop = 0;
  myinfo.rnbr = myinfo.wnbr = 0;
  /* Will remain at this value (indicates that the new sequential transfer mode has been used) */
  myinfo.aheadop = 1;
  byte_read_from_network = 0;

  p= rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p, len);
  if ( (status = srchkreqsize(s,p,len)) == -1 ) {
    rcode = errno;
  } else {
    /*
     * Reading open request.
     */
    (*logfunc)(LOG_DEBUG,"ropen_v3: reading %d bytes\n",len);
    if ((status = netread_timeout(s,rqstbuf,len,RFIO_CTRL_TIMEOUT)) != len) {
      (*logfunc)(LOG_ERR,"ropen_v3: read(): %s\n",strerror(errno));
      return -1;
    }
    status = 0;
    *account = *filename = *user = *reqhost = *vmstr = '\0';
    p= rqstbuf;
    unmarshall_WORD(p, uid);
    unmarshall_WORD(p, gid);
    unmarshall_WORD(p, mask);
    unmarshall_WORD(p, ftype);
    unmarshall_LONG(p, flags);
    unmarshall_LONG(p, mode);
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p, account, MAXACCTSIZE)) == -1)
      rcode = E2BIG;
    if ( (status == 0) &&
         (status = unmarshall_STRINGN(p, filename, MAXFILENAMSIZE)) == -1)
      rcode = SENAMETOOLONG;
    if (bet) {
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, user, CA_MAXUSRNAMELEN+1)) == -1)
        rcode = E2BIG;
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, reqhost, MAXHOSTNAMELEN)) == -1)
        rcode = E2BIG;
      unmarshall_LONG(p, passwd);
      unmarshall_WORD(p, mapping);
      if ( (status == 0) &&
           (status = unmarshall_STRINGN(p, vmstr, sizeof(vmstr))) == -1)
        rcode = E2BIG;
    }

    (*logfunc)(LOG_DEBUG,"vms string is %s\n", vmstr);
    if (bet)
      (*logfunc)(LOG_DEBUG,"Opening file %s for remote user: %s\n",CORRECT_FILENAME(filename),user);
    if (rt)
      (*logfunc)(LOG_DEBUG,"Mapping : %s\n",mapping ? "yes" : "no" );
    if (rt && !mapping) {
      (*logfunc)(LOG_DEBUG,"passwd : %d\n",passwd);
      (*logfunc)(LOG_DEBUG,"uid: %d\n",uid );
      (*logfunc)(LOG_DEBUG,"gid: %d\n",gid );
    }

    /*
     * Someone in the site has tried to specify (uid,gid) directly !
     */
    if (bet && !mapping && !rt) {
      (*logfunc)(LOG_INFO,"attempt to make non-mapped I/O and modify uid or gid !\n");
      errno=EACCES;
      rcode=errno;
      status= -1;
    }

    if ( rt ) {
      (*logfunc)(LOG_ALERT, "rfio: connection %s mapping by %s(%d,%d) from %s",(mapping ? "with" : "without"),user,uid,gid,host);
    }

    /*
     * MAPPED mode: user will be mapped to user "to"
     */
    if ( !status && rt && mapping ) {
      char to[100];
      int rcd,to_uid,to_gid;

      (*logfunc)(LOG_DEBUG,"Mapping (%s, %d, %d) \n",user, uid, gid );
      if ( (rcd = get_user(host,user,uid,gid,to,&to_uid,&to_gid)) == -ENOENT ) {
        (*logfunc)(LOG_ERR,"sropen_v3(): get_user() error opening mapping file\n");
        status = -1;
        errno = EINVAL;
        rcode = SEHOSTREFUSED;
      }

      else if ( abs(rcd) == 1 ) {
        (*logfunc)(LOG_ERR,"No entry found in mapping file for (%s,%s,%d,%d)\n", host,user,uid,gid);
        status = -1;
        errno = EACCES;
        rcode = SEHOSTREFUSED;
      }
      else {
        (*logfunc)(LOG_DEBUG,"(%s,%s,%d,%d) mapped to %s(%d,%d)\n",
            host,user,uid,gid,to,to_uid,to_gid);
        uid = to_uid;
        gid = to_gid;
        if ( uid < 100 || gid < 100 ) {
          errno = EACCES;
          status = -1;
          rcode = SEHOSTREFUSED;
        }
      }
    }
    /*
     * DIRECT access: the user specifies uid & gid by himself
     */
    if( !status && rt && !mapping ) {
      char *rtuser;
      if( (rtuser = getconfent("RTUSER","CHECK",0) ) == NULL || ! strcmp (rtuser,"YES") )
        {
          /* Port is also passwd */
          sock = connecttpread(reqhost, passwd);
            if( (sock >= 0) && !checkkey(sock, passwd) )
              {
                status= -1;
                errno = EACCES;
                rcode= errno;
                (*logfunc)(LOG_ERR,"ropen_v3: DIRECT mapping : permission denied\n");
              }
            if (sock < 0)
              {
                status= -1;
                (*logfunc)(LOG_ERR,"ropen_v3: DIRECT mapping failed: Couldn't connect %s\n",reqhost);
                rcode = EACCES;
              }
        }
      else
        (*logfunc)(LOG_INFO ,"Any DIRECT rfio request from out of site is authorized\n");
    }
    if ( !status ) {
      int need_user_check = 1;
      int rc;
      char *pfn = NULL;

      (*logfunc)(LOG_DEBUG, "ropen_v3: uid %d gid %d mask %o ftype %d flags %d mode %d\n",uid, gid, mask, ftype, flags, mode);
      (*logfunc)(LOG_DEBUG, "ropen_v3: account: %s\n", account);
      (*logfunc)(LOG_DEBUG, "ropen_v3: filename: %s\n", CORRECT_FILENAME(filename));
      (*logfunc)(LOG_INFO, "ropen_v3(%s,0X%X,0X%X) for (%d,%d)\n",CORRECT_FILENAME(filename),flags,mode,uid,gid);
      (void) umask((mode_t) CORRECT_UMASK(mask));

      rc = rfio_handle_open(CORRECT_FILENAME(filename),
                            ntohopnflg(flags),
                            mode,
                            uid,
                            gid,
                            &pfn,
                            &handler_context,
                            &need_user_check);

      if (rc < 0) {
        char alarmbuf[1024];
        sprintf(alarmbuf,"sropen_v3(): %s",CORRECT_FILENAME(filename));
        (*logfunc)(LOG_DEBUG, "ropen_v3: rfio_handler_open refused open: %s\n", sstrerror(serrno));
        rcode = serrno;
        rfio_alrm(rcode,alarmbuf);
      }

      /* NOTE(fuji): from now on, flags is in host byte-order... */
      flags = ntohopnflg(flags);
      if ( getconfent("RFIOD","DIRECTIO",0) ) {
#if defined(linux)
        (*logfunc)(LOG_DEBUG, "%s: O_DIRECT requested\n", __func__);
        flags |= O_DIRECT;
#else
        (*logfunc)(LOG_DEBUG, "%s: O_DIRECT requested but ignored.");
#endif
      }

      if (need_user_check && ((status=check_user_perm(&uid,&gid,host,&rcode,((flags & (O_WRONLY|O_RDWR)) != 0) ? "WTRUST" : "RTRUST")) < 0) &&
          ((status=check_user_perm(&uid,&gid,host,&rcode,"OPENTRUST")) < 0) ) {
        if (status == -2)
          (*logfunc)(LOG_ERR,"ropen_v3: uid %d not allowed to open()\n",uid);
        else
          (*logfunc)(LOG_ERR,"ropen_v3: failed at check_user_perm(), rcode %d\n",rcode);
        status = -1;
      }  else
        {
          const char *perm_array[3];
          char ofilename[MAXFILENAMSIZE];
          perm_array[0] = ((flags & (O_WRONLY|O_RDWR)) != 0) ? "WTRUST" : "RTRUST";
          perm_array[1] = "OPENTRUST";
          perm_array[2] = NULL;

          strcpy(ofilename, filename);
          fd = -1;
          if (forced_filename!=NULL || !check_path_whitelist(host, filename, perm_array, ofilename, sizeof(ofilename),1)) {
            fd = open(CORRECT_FILENAME(ofilename), flags,
                      ((forced_filename != NULL) && ((flags & (O_WRONLY|O_RDWR)) != 0)) ? 0644 : mode);
            (*logfunc)(LOG_DEBUG,"ropen_v3: open(%s,%d,%d) returned %x (hex)\n",CORRECT_FILENAME(ofilename),flags,mode,fd);
          }
          if (fd < 0)  {
            char alarmbuf[1024];
            sprintf(alarmbuf,"sropen_v3: %s",CORRECT_FILENAME(filename));
            status= -1;
            rcode= errno;
            (*logfunc)(LOG_DEBUG,"ropen_v3: open: %s\n",strerror(errno));
            /* rfio_alrm(rcode,alarmbuf); */
          }
          else  {
            /*
             * Getting current offset
             */
            status= lseek(fd,0L,SEEK_CUR);
            (*logfunc)(LOG_DEBUG,"ropen_v3: lseek(%d,0,SEEK_CUR) returned %x (hex)\n",fd,status);
            if ( status < 0 ) rcode= errno;
          }
        }
      if (pfn != NULL) free(pfn);
    }

    if (! status && fd >= 0)  {
      data_s = socket(AF_INET, SOCK_STREAM, 0);
      if( data_s < 0 )  {
        (*logfunc)(LOG_ERR, "datasocket(): %s\n", strerror(errno));
        exit(1);
      }
      (*logfunc)(LOG_DEBUG, "data socket created fd=%d\n", data_s);

      sin.sin_addr.s_addr = htonl(INADDR_ANY);
      sin.sin_family = AF_INET;

      /* Check to see if there is a user defined RFIOD/PORT_RANGE configured */
      if ((value = getconfent("RFIOD", "PORT_RANGE", 0)) != NULL) {
        if ((buf = strdup(value)) == NULL) {
          (*logfunc)(LOG_ERR, "ropen_v3: strdup: %s\n", strerror(errno));
          exit(1);
        }
        if ((p = strchr(buf, ',')) != NULL) {
          *p = '\0';
          p++;
          /* Check that the values are valid */
          if ((((low_port = strtol(buf, &dp, 10)) <= 0) || (*dp != '\0')) ||
              (((high_port = strtol(p, &dp2, 10)) <= 0) || (*dp2 != '\0')) ||
              (high_port <= low_port) ||
              ((low_port < 1024) || (low_port > 65535)) ||
              ((high_port < 1024) || (high_port > 65535))) {
            (*logfunc)(LOG_ERR, "ropen_v3: invalid port range: %s, using default %d,%d\n",
                value, RFIO_LOW_PORT_RANGE, RFIO_HIGH_PORT_RANGE);
            low_port  = RFIO_LOW_PORT_RANGE;
            high_port = RFIO_HIGH_PORT_RANGE;
          } else {
            (*logfunc)(LOG_DEBUG, "ropen64_v3: using port range: %d,%d\n", low_port, high_port);
          }
        } else {
          (*logfunc)(LOG_ERR, "ropen_v3: invalid port range: %s, using default %d,%d\n",
              value, RFIO_LOW_PORT_RANGE, RFIO_HIGH_PORT_RANGE);
        }
        (void) free(buf);
      }

      /* Set random seed */
      gettimeofday(&tv, NULL);
      srand(tv.tv_usec * tv.tv_sec);

      /* Loop over all the ports in the specified range starting at a random
       * offset
       */
      port = (rand() % (high_port - (low_port + 1))) + low_port;
      while (port++) {
        /* If we reach the maximum allowed port, reset it! */
        if (port > high_port) {
          port = low_port;
          sleep(5);  /* sleep between complete loops, prevents CPU thrashing */
          continue;
        }

        /* Attempt to bind to the port */
        sin.sin_port = htons(port);
        if (bind(data_s, (struct sockaddr*)&sin, sizeof(sin)) == 0) {
          /* Just because the bind was successfull, doesn't mean the listen
           * will succeed!
           */
          if (listen(data_s, 5) < 0) {
            if (errno == EADDRINUSE) {
              (*logfunc)(LOG_DEBUG, "ropen_v3: listen(%d): %s, attempting another port\n", data_s, strerror(errno));
              /* close and recreate the socket */
              close(data_s);
              data_s = socket(AF_INET, SOCK_STREAM, 0);
              if( data_s < 0 )  {
                (*logfunc)(LOG_ERR, "datasocket(): %s\n", strerror(errno));
                exit(1);
              }
              sleep(1);
              continue;
            } else {
              (*logfunc)(LOG_ERR, "ropen_v3: listen(%d): %s\n", data_s, strerror(errno));
              exit(1);
            }
          }
          break;
        } else {
          (*logfunc)(LOG_DEBUG, "ropen_v3: bind(%d:%d): %s, trying again\n", data_s, port, strerror(errno));
        }
      }

      size_sin = sizeof(sin);
      if (getsockname(data_s, (struct sockaddr*)&sin, &size_sin) < 0 )  {
        (*logfunc)(LOG_ERR, "ropen_v3: getsockname: %s\n", strerror(errno));
        exit(1);
      }

      (*logfunc)(LOG_DEBUG, "ropen_v3: assigning data port %d\n", htons(sin.sin_port));
    }
  }

  /*
   * Sending back status to the client
   */
  p= rqstbuf;
  marshall_WORD(p,RQST_OPEN_V3);
  marshall_LONG(p,status);
  marshall_LONG(p,rcode);
  marshall_LONG(p,ntohs(sin.sin_port));
  (*logfunc)(LOG_DEBUG, "ropen_v3: sending back status(%d) and errno(%d)\n",status,rcode);
  errno = ECONNRESET;
  if (netwrite_timeout(s,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT) != RQSTSIZE)  {
    (*logfunc)(LOG_ERR,"ropen_v3: netwrite_timeout(): %s\n",strerror(errno));
    if (data_s >= 0) close(data_s);
    if (fd >= 0) close(fd);
    return -1;
  }

  if (! status && fd >= 0)
    {
      /*
       * The rcvbuf on the data socket must be set _before_
       * the connection is accepted! Otherwise the receiver will
       * only offer the default window, which for large MTU devices
       * (such as HIPPI) is far too small and performance goes down
       * the drain.
       *
       * The sndbuf must be set on the socket returned by accept()
       * as it is not inherited (at least not on SGI).
       * 98/08/05 - Jes
       */
      (*logfunc)(LOG_DEBUG, "doing setsockopt rcv\n");
      if (setsockopt(data_s,SOL_SOCKET,SO_RCVBUF,(char *)&max_rcvbuf,
                     sizeof(max_rcvbuf)) < 0) {
        (*logfunc)(LOG_ERR, "setsockopt open rcvbuf(%d bytes): %s\n", max_rcvbuf, strerror(errno));
      }
      (*logfunc)(LOG_DEBUG, "setsockopt rcvbuf on data socket (%d bytes)\n", max_rcvbuf);
      for (;;) {
        fromlen = sizeof(from);
        (*logfunc)(LOG_DEBUG, "ropen_v3: wait for accept to complete\n");
        data_sock = accept(data_s, (struct sockaddr*)&from, &fromlen);
        if( data_sock < 0 )  {
          (*logfunc)(LOG_ERR, "data accept(): %s\n",strerror(errno));
          exit(1);
        }
        else
          break;
      }
      (*logfunc)(LOG_DEBUG, "data accept is ok, fildesc=%d\n",data_sock);

      /*
       * Set the send socket buffer on the data socket (see comment
       * above before accept())
       */
      (*logfunc)(LOG_DEBUG, "doing setsockopt snd\n");
      if (setsockopt(data_sock,SOL_SOCKET,SO_SNDBUF,(char *)&max_sndbuf,
                     sizeof(max_sndbuf)) < 0) {
        (*logfunc)(LOG_ERR, "setsockopt open sndbuf(%d bytes): %s\n",
            max_sndbuf, strerror(errno));
      }
      (*logfunc)(LOG_DEBUG, "setsockopt sndbuf on data socket (%d bytes)\n",
          max_sndbuf);

      /* Set the keepalive option on both sockets */
      yes = 1;
      if (setsockopt(data_sock,SOL_SOCKET,SO_KEEPALIVE,(char *)&yes, sizeof(yes)) < 0) {
        (*logfunc)(LOG_ERR, "setsockopt keepalive on data: %s\n",strerror(errno));
      }
      (*logfunc)(LOG_DEBUG, "setsockopt keepalive on data done\n");

      yes = 1;
      if (setsockopt(ctrl_sock,SOL_SOCKET,SO_KEEPALIVE,(char *)&yes, sizeof(yes)) < 0) {
        (*logfunc)(LOG_ERR, "setsockopt keepalive on ctrl: %s\n",strerror(errno));
      }
      (*logfunc)(LOG_DEBUG, "setsockopt keepalive on ctrl done\n");

      yes = 1;
      if (setsockopt(data_sock,IPPROTO_TCP,TCP_NODELAY,(char *)&yes,sizeof(yes)) < 0) {
        (*logfunc)(LOG_ERR, "setsockopt nodelay on data: %s\n",strerror(errno));
      }
      (*logfunc)(LOG_DEBUG,"setsockopt nodelay option set on data socket\n");

      yes = 1;
      if (setsockopt(ctrl_sock,IPPROTO_TCP,TCP_NODELAY,(char *)&yes,sizeof(yes)) < 0) {
        (*logfunc)(LOG_ERR, "setsockopt nodelay on ctrl: %s\n",strerror(errno));
      }
      (*logfunc)(LOG_DEBUG,"setsockopt nodelay option set on ctrl socket\n");
    }
  if (data_s >= 0) close(data_s);
  return fd;
}


int   srclose_v3(int     s,
                 struct rfiostat *infop,
                 int     fd)
{
  int  status;
  int  rcode;
  char *p;
  char tmpbuf[21], tmpbuf2[21];
  struct stat filestat;
  int ret;

  (void)infop;
  (*logfunc)(LOG_INFO,"%d read, %d readahead, %d write, %d flush, %d stat, %d lseek and %d preseek\n",
      myinfo.readop, myinfo.aheadop, myinfo.writop, myinfo.flusop, myinfo.statop,
      myinfo.seekop, myinfo.presop);
  (*logfunc)(LOG_INFO,"%s bytes read and %s bytes written\n",
      u64tostr(myinfo.rnbr,tmpbuf,0), u64tostr(myinfo.wnbr,tmpbuf2,0));
  (*logfunc)(LOG_INFO, "rclose_v3(%d, %d)\n",s, fd);

  /* sync the file to be sure that filesize in correct in following stats.
     this is needed by some ext3 bug/feature
     Still ignore the output of fsync */
  fsync(fd);

  /* Stat the file to be able to provide that information
     to the close handler */
  memset(&filestat,0,sizeof(struct stat));
  fstat(fd, &filestat);

  status = close(fd);
  rcode = ( status < 0 ) ? errno : 0;

  ret=rfio_handle_close(handler_context, &filestat, rcode);
  if (ret<0){
    (*logfunc)(LOG_ERR, "srclose_v3: rfio_handle_close failed\n");
    return -1;
  }

  /* Close data socket */
  if( close(data_sock) < 0 )
    (*logfunc)(LOG_DEBUG, "rclose_v3 : Error closing data socket fildesc=%d,errno=%d\n",
        data_sock, errno);
  else
    (*logfunc)(LOG_DEBUG, "rclose_v3 : closing data socket fildesc=%d\n", data_sock);


  p= rqstbuf;
  marshall_WORD(p, RQST_CLOSE_V3);
  marshall_LONG(p, status);
  marshall_LONG(p, rcode);

  errno = ECONNRESET;
  if (netwrite_timeout(s, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT) != RQSTSIZE)  {
    (*logfunc)(LOG_ERR, "rclose_v3: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  if( close(s) < 0 )
    (*logfunc)(LOG_DEBUG, "rclose_v3 : Error closing control socket fildesc=%d,errno=%d\n",
        s, errno);
  else
    (*logfunc)(LOG_DEBUG, "rclose_v3 : closing ctrl socket fildesc=%d\n", s);

  return status;
}

void *produce_thread(int *ptr)
{
  int      fd = *ptr;
  int      byte_read = -1;
  int      error = 0;
  int      total_produced = 0;
  unsigned int ckSum = 0;
  char     ckSumbuf[CA_MAXCKSUMLEN+1]; /* max check sum 256bit 32x8+'\0'*/
  char     ckSumbufdisk[CA_MAXCKSUMLEN+1];
  char     useCksum;
  char     *conf_ent;
  char     *ckSumalg="ADLER32";
  int      xattr_len;

  /* Check if checksum support is enabled */
  useCksum = 1;
  if ((conf_ent = getconfent("RFIOD","USE_CKSUM",0)) != NULL)
    if (!strncasecmp(conf_ent,"no",2)) useCksum = 0;

  if (useCksum) {
    if ((xattr_len = fgetxattr(fd,"user.castor.checksum.value",ckSumbufdisk,CA_MAXCKSUMLEN)) == -1) {
      (*logfunc)(LOG_ERR,"produce_thread: fgetxattr failed for user.castor.checksum.value, error=%d\n",errno);
      (*logfunc)(LOG_ERR,"produce_thread: skipping checksums check\n");
      useCksum = 0;
    } else {
      ckSumbufdisk[xattr_len] = '\0';
      ckSum = adler32(0L,Z_NULL,0);
      (*logfunc)(LOG_DEBUG,"produce_thread: checksum init for %s\n",ckSumalg);
      (*logfunc)(LOG_DEBUG,"produce_thread: disk file checksum=0x%s\n",ckSumbufdisk);
    }
  }

  while ((! error) && (byte_read != 0)) {
    if (Cthread_mutex_lock(&stop_read)) {
      (*logfunc)(LOG_ERR,"produce_thread: Cannot get mutex : serrno=%d\n", serrno);
      return(NULL);
    }
    if (stop_read)
      return (NULL);
    if (Cthread_mutex_unlock(&stop_read)) {
      (*logfunc)(LOG_ERR,"produce_thread: Cannot release mutex : serrno=%d\n", serrno);
      return(NULL);
    }
    Csemaphore_down(&empty);

    byte_read = read(fd,array[produced % daemonv3_rdmt_nbuf].p,daemonv3_rdmt_bufsize);

    if (byte_read > 0) {
      total_produced += byte_read;
      array[produced % daemonv3_rdmt_nbuf].len = byte_read;
      if (useCksum) {
        ckSum = adler32(ckSum,(unsigned char*)array[produced % daemonv3_rdmt_nbuf].p,(unsigned int)byte_read);
        (*logfunc)(LOG_DEBUG,"produce_thread: current checksum=0x%x\n",ckSum);
      }
    }
    else {
      if (byte_read == 0) {
        (*logfunc)(LOG_DEBUG,"End of reading : total produced = %d,buffers=%d\n",total_produced,produced);
        array[produced % daemonv3_rdmt_nbuf].len = 0;
        /* Check for checksum mismatch. */
        if (useCksum) {
          sprintf(ckSumbuf,"%x", ckSum);
          (*logfunc)(LOG_DEBUG,"produce_thread: file checksum=0x%s\n",ckSumbuf);
          if (strncmp(ckSumbufdisk,ckSumbuf,CA_MAXCKSUMLEN)==0) {
            (*logfunc)(LOG_DEBUG,"produce_thread: checksums OK!\n");
          }
          else {
            (*logfunc)(LOG_ERR,"produce_thread: checksum error detected reading file: %s (recorded checksum: %s calculated checksum: %s)\n",
                CORRECT_FILENAME(filename), ckSumbufdisk, ckSumbuf);
            array[produced % daemonv3_rdmt_nbuf].len = -(EREMOTEIO); /* setting errno=Remote I/O error */
            error = -1;
          }
        }
      }
      else {
        if (byte_read < 0) {
          array[produced % daemonv3_rdmt_nbuf].len = -errno;
          error = -1;
        }
      }
    }
    produced++;
    Csemaphore_up(&full);
  }
  return(NULL);
}

void *consume_thread(int *ptr)
{
  int      fd = *ptr;
  int      byte_written = -1;
  int      error = 0, end = 0;
  int      total_consumed = 0;
  char     *buffer_to_write;
  int      len_to_write;
  int      saved_errno;
  unsigned int ckSum = 0;
  char     ckSumbuf[CA_MAXCKSUMLEN+1]; /* max check sum 256bit 32x8+'\0'*/
  char     ckSumbufdisk[CA_MAXCKSUMLEN+1];
  char     useCksum;
  char     *conf_ent;
  char     *ckSumalg="ADLER32";
  int      mode;

  /* Check if checksum support is enabled */
  useCksum = 1;
  if ((conf_ent=getconfent("RFIOD","USE_CKSUM",0)) != NULL)
    if (!strncasecmp(conf_ent,"no",2)) useCksum = 0;

  /* Deal with cases where checksums should not be calculated */
  if (useCksum) {
    mode = fcntl(fd,F_GETFL);
    if (mode == -1) {
      (*logfunc)(LOG_ERR,"consume_thread: fcntl (F_GETFL) failed, error=%d\n",errno);
      useCksum = 0;
    }
    /* Checksums on updates are not supported */
    else if (mode & O_RDWR) {
      (*logfunc)(LOG_INFO,"consume_thread: file opened in O_RDWR, skipping checksums\n");
      useCksum = 0;
    }
    /* If we are writing to the file and a checksum already exists, we
     * remove the checksum value but leave the type.
     */
    else if ((mode & O_WRONLY) &&
             (fgetxattr(fd,"user.castor.checksum.type",ckSumbufdisk,CA_MAXCKSUMLEN) != -1)) {
      (*logfunc)(LOG_INFO,"consume_thread: file opened in O_WRONLY and checksum already exists, removing checksum\n");
      useCksum = 0;
    } else {
      ckSum = adler32(0L,Z_NULL,0);
      (*logfunc)(LOG_DEBUG,"consume_thread: checksum init for %s\n",ckSumalg);
    }
  }
  /* Always remove the checksum value */
  fremovexattr(fd,"user.castor.checksum.value");

  while ((! error) && (! end)) {
    Csemaphore_down(&full);

    buffer_to_write = array[consumed % daemonv3_wrmt_nbuf].p;
    len_to_write = array[consumed % daemonv3_wrmt_nbuf].len;

    if (len_to_write > 0) {
      (*logfunc)(LOG_DEBUG,"Trying to write %d bytes from %X\n",len_to_write,buffer_to_write);

      byte_written = write(fd, buffer_to_write, len_to_write);
      /* If the write is successfull but incomplete (fs is full) we
         report the ENOSPC error immediately in order to simplify the code
      */
      if ((byte_written >= 0) && (byte_written != len_to_write)) {
        byte_written = -1;
        errno = ENOSPC;
      }

      /* The following Cthread_mutex_lock call may modify this value */
      saved_errno = errno;

      /* Error reporting to global var */
      if (byte_written == -1) {
        error = 1;
        if (Cthread_mutex_lock(&write_error)) {
          (*logfunc)(LOG_ERR,"Cannot get mutex : serrno=%d",serrno);
          return(NULL);
        }

        write_error = saved_errno;

        if (Cthread_mutex_unlock(&write_error)) {
          (*logfunc)(LOG_ERR,"Cannot release mutex : serrno=%d",serrno);
          return(NULL);
        }

        (*logfunc)(LOG_DEBUG,"Error when writing : buffers=%d, error=%d\n",consumed,errno);
      }
      else {
        /* All bytes written to disks */
        total_consumed += byte_written;
        (*logfunc)(LOG_DEBUG,"Has written buf %d to disk (len %d)\n",consumed % daemonv3_wrmt_nbuf,byte_written);
        if(useCksum) {
          ckSum = adler32(ckSum,(unsigned char*)buffer_to_write,(unsigned int)byte_written);
          (*logfunc)(LOG_DEBUG,"consume_thread: current checksum=0x%x\n",ckSum);
        }
      }
    }
    else
      if (len_to_write == 0) {
        (*logfunc)(LOG_DEBUG,"End of writing : total consumed = %d,buffers=%d\n",total_consumed,consumed);
        end = 1;
      }
      else
        if (len_to_write < 0) {
          /* Error indicated by the thread reading from network, this thread just terminates */
          error = 1;
        }
    consumed++;
    Csemaphore_up(&empty);
  }

  /* Record the checksum value and type in the extended attributes of the file.
   * This is done in all cases, including errors!
   */
  if (useCksum) {
    sprintf(ckSumbuf,"%x",ckSum);
    (*logfunc)(LOG_DEBUG,"consume_thread: file checksum=0x%s\n",ckSumbuf);
    /* Double check whether the checksum is set on disk. If yes, it means
       it appeared while we were writing. this means that concurrent writing
       is taking place. Thus we reset the checksum rather than setting it */
    if (fgetxattr(fd,"user.castor.checksum.value",ckSumbufdisk,CA_MAXCKSUMLEN) != -1) {
      (*logfunc)(LOG_INFO,"consume64_thread: concurrent writing detected, removing checksum\n");
      fremovexattr(fd,"user.castor.checksum.value");
    } else {
      /* Always try and set the type first! */
      if (fsetxattr(fd,"user.castor.checksum.type",ckSumalg,strlen(ckSumalg),0))
        (*logfunc)(LOG_ERR,"consume64_thread: fsetxattr failed for user.castor.checksum.type, error=%d\n",errno);
      else if (fsetxattr(fd,"user.castor.checksum.value",ckSumbuf,strlen(ckSumbuf),0))
        (*logfunc)(LOG_ERR,"consume64_thread: fsetxattr failed for user.castor.checksum.value, error=%d\n",errno);
    }
  }
  return(NULL);
}

void wait_consumer_thread(int cid)
{
  (*logfunc)(LOG_DEBUG,"Entering wait_consumer_thread\n");
  /* if no write error */
  /* Indicate to the consumer thread that an error has occured */
  /* The consumer thread will then terminate */
  /* In case of write_error, the consumer thread has already exited */
  /* and waiting on empty64 could lead to a dead lock */
  if (!write_error) {
    Csemaphore_down(&empty);
    array[produced % daemonv3_wrmt_nbuf].len = -1;
    produced++;
    Csemaphore_up(&full);
  }
  (*logfunc)(LOG_INFO,"Joining thread\n");
  (*logfunc)(LOG_DEBUG,"Joining consumer thread after error in main thread\n");
  if (Cthread_join(cid,NULL) < 0)
    {
      (*logfunc)(LOG_ERR,"Error joining consumer thread after error in main thread, serrno=%d\n",serrno);
      return;
    }
}

     int srread_v3(int     ctrl_sock,
                   struct  rfiostat* infop,
                   int     fd)
{
  int  status = 0;         /* Return code          */
  int  rcode;          /* To send back errno   */
  int  offset;         /* lseek offset         */
  char *p;             /* Pointer to buffer    */
  char *iobuffer = 0;
  off_t bytes2send;
  fd_set fdvar, fdvar2;
  extern int max_sndbuf;
  struct stat st;
  char rfio_buf[BUFSIZ];
  int eof_met = 0;
  int join_done = 0;
  int DISKBUFSIZE_READ = 2097152;
  int n;
  int cid1 = 0;
  int el;

  (void)infop;
  /*
   * Receiving request,
   */
  (*logfunc)(LOG_DEBUG, "rread_v3(%d, %d)\n",ctrl_sock, fd);

  if (first_read)
    {
      char *p;
      first_read = 0;
      eof_met = 0;
      join_done = 0;

      if( (p = getconfent("RFIO", "DAEMONV3_RDSIZE", 0)) != NULL ) {
        if (atoi(p) > 0)
          DISKBUFSIZE_READ = atoi(p);
      }

      daemonv3_rdmt = DAEMONV3_RDMT;
      if( (p = getconfent("RFIO", "DAEMONV3_RDMT", 0)) != NULL ) {
        if (*p == '0')
          daemonv3_rdmt = 0;
        else
          daemonv3_rdmt = 1;
      }

      daemonv3_rdmt_nbuf = DAEMONV3_RDMT_NBUF;
      if( (p = getconfent("RFIO", "DAEMONV3_RDMT_NBUF", 0)) != NULL )
        if (atoi(p) > 0)
          daemonv3_rdmt_nbuf = atoi(p);

      daemonv3_rdmt_bufsize = DAEMONV3_RDMT_BUFSIZE;
      if( (p = getconfent("RFIO", "DAEMONV3_RDMT_BUFSIZE", 0)) != NULL )
        if (atoi(p) > 0)
          daemonv3_rdmt_bufsize = atoi(p);

      (*logfunc)(LOG_DEBUG,"rread_v3 : daemonv3_rdmt=%d,daemonv3_rdmt_nbuf=%d,daemonv3_rdmt_bufsize=%d\n",
          daemonv3_rdmt,daemonv3_rdmt_nbuf,daemonv3_rdmt_bufsize);

      if (daemonv3_rdmt) {
        /* Indicates we are using RFIO V3 and multithreadding while reading */
        myinfo.aheadop = 3;
        /* Allocating circular buffer itself */
        (*logfunc)(LOG_DEBUG, "rread_v3 allocating circular buffer : %d bytes\n",sizeof(struct element) * daemonv3_rdmt_nbuf);
        if ((array = (struct element *)malloc(sizeof(struct element) * daemonv3_rdmt_nbuf)) == NULL)  {
          (*logfunc)(LOG_ERR, "rread_v3: malloc array: ERROR occured (errno=%d)", errno);
          return -1;
        }
        (*logfunc)(LOG_DEBUG, "rread_v3 malloc array allocated : 0X%X\n",array);

        /* Allocating memory for each element of circular buffer */
        for (el=0; el < daemonv3_rdmt_nbuf; el++) {
          (*logfunc)(LOG_DEBUG, "rread_v3 allocating circular buffer element %d: %d bytes\n",el,daemonv3_rdmt_bufsize);
          if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
            array[el].p = (char *)malloc_page_aligned(daemonv3_rdmt_bufsize);
          } else {
            array[el].p = (char *)malloc(daemonv3_rdmt_bufsize);
          }
          if ( array[el].p == NULL)  {
            (*logfunc)(LOG_ERR, "rread_v3: malloc array element %d: ERROR occured (errno=%d)", el, errno);
            return -1;
          }
          (*logfunc)(LOG_DEBUG, "rread_v3 malloc array element %d allocated : 0X%X\n",el, array[el].p);
        }
      }
      else {
        (*logfunc)(LOG_DEBUG, "rread_v3 allocating malloc buffer : %d bytes\n",DISKBUFSIZE_READ);
        if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
          iobuffer = (char *)malloc_page_aligned(DISKBUFSIZE_READ);
        } else {
          iobuffer = (char *)malloc(DISKBUFSIZE_READ);
        }
        if ( iobuffer == NULL)  {
          (*logfunc)(LOG_ERR, "rread_v3: malloc: ERROR occured (errno=%d)", errno);
          return -1;
        }
        (*logfunc)(LOG_DEBUG, "rread_v3 malloc buffer allocated : 0X%X\n",iobuffer);
      }

      if (fstat(fd,&st) < 0) {
        (*logfunc)(LOG_ERR, "rread_v3: fstat(): ERROR occured (errno=%d)", errno);
        return -1;
      }

      (*logfunc)(LOG_DEBUG, "rread_v3 filesize : %d bytes\n",st.st_size);
      if ((offset = lseek(fd,0L,SEEK_CUR)) < 0) {
        (*logfunc)(LOG_ERR, "rread_v3: lseek offset(): ERROR occured (errno=%d)", errno);
        return -1;
      }
      bytes2send = st.st_size - offset;
      if (bytes2send < 0) bytes2send = 0;
      (*logfunc)(LOG_DEBUG, "rread_v3: %d bytes to send (offset taken into account)\n",bytes2send);
      p = rfio_buf;
      marshall_WORD(p,RQST_READ_V3);
      marshall_LONG(p,bytes2send);

      (*logfunc)(LOG_DEBUG, "rread_v3: sending %d bytes", RQSTSIZE);
      errno = ECONNRESET;
      if ((n = netwrite_timeout(ctrl_sock, rfio_buf, RQSTSIZE, RFIO_CTRL_TIMEOUT)) != RQSTSIZE) {
        (*logfunc)(LOG_ERR, "rread_v3: netwrite_timeout(): %s\n", strerror(errno));
        return -1;
      }

      if (daemonv3_rdmt) {
        Csemaphore_init(&empty,daemonv3_rdmt_nbuf);
        Csemaphore_init(&full,0);

        if ((cid1 = Cthread_create((void *(*)(void *))produce_thread,(void *)&fd)) < 0) {
          (*logfunc)(LOG_ERR,"Cannot create producer thread : serrno=%d,errno=%d\n",serrno,errno);
          return(-1);
        }
      }
    }

  /*
   * Reading data from the network.
   */
  while (1)
    {
      struct timeval t;
      fd_set *write_fdset;

      FD_ZERO(&fdvar);
      FD_SET(ctrl_sock,&fdvar);

      FD_ZERO(&fdvar2);
      FD_SET(data_sock,&fdvar2);

      t.tv_sec = 10;
      t.tv_usec = 0;

      if (eof_met)
        write_fdset = NULL;
      else
        write_fdset = &fdvar2;

      (*logfunc)(LOG_DEBUG,"srread: doing select\n");
      if( select(FD_SETSIZE, &fdvar, write_fdset, NULL, &t) < 0 )  {
        (*logfunc)(LOG_ERR, "rfio","srread_v3: select failed (errno=%d)", errno);
        return -1;
      }
      if( FD_ISSET(ctrl_sock, &fdvar) )  {
        int n, magic, code;

        /* Something received on the control socket */
        (*logfunc)(LOG_DEBUG, "ctrl socket: reading %d bytes\n", RQSTSIZE);
        errno = ECONNRESET;
        if ((n = netread_timeout(ctrl_sock,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT)) != RQSTSIZE) {
          (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
          return -1;
        }
        p = rqstbuf;
        unmarshall_WORD(p,magic);
        unmarshall_WORD(p,code);

        /* what to do ? */
        if (code == RQST_CLOSE_V3)  {
          (*logfunc)(LOG_DEBUG,"close request: magic: %x code: %x\n", magic, code);
          if (!daemonv3_rdmt) {
            (*logfunc)(LOG_DEBUG,"freeing iobuffer at 0X%X\n",iobuffer);
            if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
              free_page_aligned(iobuffer);
            } else {
              free(iobuffer);
            }
          }
          else {
            if(!join_done) {
              if (Cthread_mutex_lock(&stop_read)) {
                (*logfunc)(LOG_ERR,"srread_v3: Cannot get mutex : serrno=%d\n", serrno);
                return(-1);
              }
              stop_read = 1;
              if (Cthread_mutex_unlock(&stop_read)) {
                (*logfunc)(LOG_ERR,"srread_v3: Cannot release mutex : serrno=%d\n", serrno);
                return(-1);
              }
              Csemaphore_up(&empty);
              if (Cthread_join(cid1,NULL) < 0) {
                (*logfunc)(LOG_ERR,"srread_v3: Error joining producer, serrno=%d\n", serrno);
                return(-1);
              }
            }
            for (el=0; el < daemonv3_rdmt_nbuf; el++) {
              (*logfunc)(LOG_DEBUG,"freeing array element %d at 0X%X\n",el,array[el].p);
              if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
                free_page_aligned(array[el].p);
              } else {
                free(array[el].p);
              }
            }
            (*logfunc)(LOG_DEBUG,"freeing array at 0X%X\n",array);
            free(array);
          }
          srclose_v3(ctrl_sock,&myinfo,fd);
          return 0;
        }
        else  {
          (*logfunc)(LOG_ERR,"unknown request:  magic: %x code: %x\n",magic,code);
          return(-1);
        }
      }

      /*
       * Reading data on disk.
       */

      if( !eof_met && (FD_ISSET(data_sock, &fdvar2)) )  {
        if (daemonv3_rdmt) {
          Csemaphore_down(&full);

          if (array[consumed % daemonv3_rdmt_nbuf].len > 0) {
            iobuffer = array[consumed % daemonv3_rdmt_nbuf].p;
            status = array[consumed % daemonv3_rdmt_nbuf].len;
          }
          else if (array[consumed % daemonv3_rdmt_nbuf].len == 0) {
            status = 0;
            iobuffer = NULL;
            (*logfunc)(LOG_DEBUG,"Waiting for producer thread\n");
            if (Cthread_join(cid1,NULL) < 0) {
              (*logfunc)(LOG_ERR,"Error joining producer, serrno=%d\n",serrno);
              return(-1);
            }
            join_done = 1;
          }
          else if (array[consumed % daemonv3_rdmt_nbuf].len < 0) {
            status = -1;
            errno = -(array[consumed % daemonv3_rdmt_nbuf].len);
          }
          consumed++;
        }
        else
          status = read(fd,iobuffer,DISKBUFSIZE_READ);

        /* To simulate a read I/O error
           status = -1;
           errno = 5; */

        rcode = (status < 0) ? errno:0;
        (*logfunc)(LOG_DEBUG, "%d bytes have been read on disk\n",status);

        if (status == 0)  {
          if (daemonv3_rdmt)
            Csemaphore_up(&empty);
          eof_met = 1;
          p = rqstbuf;
          marshall_WORD(p,REP_EOF);
          (*logfunc)(LOG_DEBUG, "rread_v3: eof\n");
          errno = ECONNRESET;
          if ((n = netwrite_timeout(ctrl_sock, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT)) != RQSTSIZE)  {
            (*logfunc)(LOG_ERR,"rread_v3: netwrite_timeout(): %s\n", strerror(errno));
            return -1;
          }
        } /*  status == 0 */
        else
          if (status < 0)  {
            if (daemonv3_rdmt)
              Csemaphore_up(&empty);
            p = rqstbuf;
            marshall_WORD(p, REP_ERROR);
            marshall_LONG(p, status);
            marshall_LONG(p, rcode);
            (*logfunc)(LOG_DEBUG, "rread_v3: status %d, rcode %d\n", status, rcode);
            errno = ECONNRESET;
            if ((n = netwrite_timeout(ctrl_sock, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT)) != RQSTSIZE)  {
              (*logfunc)(LOG_ERR, "rread_v3: netwrite_timeout(): %s\n", strerror(errno));
              return -1;
            }
            (*logfunc)(LOG_DEBUG, "read_v3: waiting ack for error\n");
            if ((n = netread_timeout(ctrl_sock,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT)) != RQSTSIZE) {
              if (n == 0)  {
                errno = ECONNRESET;
                (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
                return -1;
              }  else  {
                (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
                return -1;
              }
            }
            return(-1);
          }  else  {
            (*logfunc)(LOG_DEBUG, "rread_v3: writing %d bytes to data socket %d\n",status, data_sock);
            errno = ECONNRESET;
            if( (n = netwrite(data_sock, iobuffer, status)) != status ) {
              (*logfunc)(LOG_ERR, "rread_v3: netwrite(): %s\n", strerror(errno));
              return -1;
            }
            if (daemonv3_rdmt)
              Csemaphore_up(&empty);
            myinfo.rnbr += status;
            myinfo.readop++;
          }
      }
    }
}

int srwrite_v3(int     s,
               struct rfiostat *infop,
               int     fd)
{
  int  status = 0;         /* Return code         */
  int  rcode;          /* To send back errno  */
  char *p;             /* Pointer to buffer   */
  char *iobuffer = 0;
  fd_set fdvar, fdvar2;
  int    byte_written_by_client;
  extern int max_rcvbuf;
  int    maxseg;
  socklen_t optlen;
  int    byte_in_diskbuffer = 0;
  char   *iobuffer_p = 0;
  struct timeval t;
  int    sizeofdummy;
  /*
   * Put dummy on heap to avoid large arrays in thread stack
   */
  unsigned char *dummy;
  int DISKBUFSIZE_WRITE = 2097152;
  int el;
  int cid2 = 0;
  int saved_errno = 0;

  (void)infop;
  /*
   * Receiving request,
   */
  (*logfunc)(LOG_DEBUG, "rwrite_v3(%d, %d)\n",s, fd);
  if( first_write )  {
    char *p;

#ifdef USE_XFSPREALLOC
    if ( (p = getconfent("RFIOD","XFSPREALLOC",0)) ) {
      rfio_xfs_resvsp64(fd, atoi(p));
    }
#endif

    first_write = 0;
    status = rfio_handle_firstwrite(handler_context);
    if (status != 0)  {
      (*logfunc)(LOG_ERR, "srwrite64_v3: rfio_handle_firstwrite: %s\n", strerror(serrno));
      p = rqstbuf;
      marshall_WORD(p,REP_ERROR);
      marshall_LONG(p,status);
      marshall_LONG(p,serrno);
      if( netwrite_timeout(s, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT) != RQSTSIZE )  {
        (*logfunc)(LOG_ERR, "rwrite_v3: netwrite_timeout(): %s\n", strerror(errno));
      }
      return -1;
    }
    if ((p = getconfent("RFIO","DAEMONV3_WRSIZE",0)) != NULL)  {
      if (atoi(p) > 0)
        DISKBUFSIZE_WRITE = atoi(p);
    }

    daemonv3_wrmt = DAEMONV3_WRMT;
    if( (p = getconfent("RFIO", "DAEMONV3_WRMT", 0)) != NULL ) {
      if (*p == '0')
        daemonv3_wrmt = 0;
      else
        daemonv3_wrmt = 1;
    }

    daemonv3_wrmt_nbuf = DAEMONV3_WRMT_NBUF;
    if( (p = getconfent("RFIO", "DAEMONV3_WRMT_NBUF", 0)) != NULL )
      if (atoi(p) > 0)
        daemonv3_wrmt_nbuf = atoi(p);

    daemonv3_wrmt_bufsize = DAEMONV3_WRMT_BUFSIZE;
    DISKBUFSIZE_WRITE = DAEMONV3_WRMT_BUFSIZE;
    if( (p = getconfent("RFIO", "DAEMONV3_WRMT_BUFSIZE", 0)) != NULL )
      if (atoi(p) > 0)  {
        daemonv3_wrmt_bufsize = atoi(p);
        DISKBUFSIZE_WRITE = atoi(p);
      }

    (*logfunc)(LOG_DEBUG,"rwrite_v3 : daemonv3_wrmt=%d,daemonv3_wrmt_nbuf=%d,daemonv3_wrmt_bufsize=%d\n",
        daemonv3_wrmt,daemonv3_wrmt_nbuf,daemonv3_wrmt_bufsize);

    if (daemonv3_wrmt) {
      /* Indicates we are using RFIO V3 and multithreading while writing */
      myinfo.aheadop = 3;
      /* Allocating circular buffer itself */
      (*logfunc)(LOG_DEBUG, "rwrite_v3 allocating circular buffer : %d bytes\n",sizeof(struct element) * daemonv3_wrmt_nbuf);
      if ((array = (struct element *)malloc(sizeof(struct element) * daemonv3_wrmt_nbuf)) == NULL)  {
        (*logfunc)(LOG_ERR, "rwrite_v3: malloc array: ERROR occured (errno=%d)", errno);
        return -1;
      }
      (*logfunc)(LOG_DEBUG, "rwrite_v3 malloc array allocated : 0X%X\n",array);

      /* Allocating memory for each element of circular buffer */
      for (el=0; el < daemonv3_wrmt_nbuf; el++) {
        (*logfunc)(LOG_DEBUG, "rwrite_v3 allocating circular buffer element %d: %d bytes\n",el,daemonv3_wrmt_bufsize);
        if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
          array[el].p = (char *)malloc_page_aligned(daemonv3_wrmt_bufsize);
        } else {
          array[el].p = (char *)malloc(daemonv3_wrmt_bufsize);
        }
        if ( array[el].p == NULL)  {
          (*logfunc)(LOG_ERR, "rwrite_v3: malloc array element %d: ERROR occured (errno=%d)", el, errno);
          return -1;
        }
        (*logfunc)(LOG_DEBUG, "rwrite_v3 malloc array element %d allocated : 0X%X\n",el, array[el].p);
      }
    }
    else {
      (*logfunc)(LOG_DEBUG, "rwrite_v3 allocating malloc buffer : %d bytes\n",DISKBUFSIZE_WRITE);
      if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
        iobuffer = (char *)malloc_page_aligned(DISKBUFSIZE_WRITE);
      } else {
        iobuffer = (char *)malloc(DISKBUFSIZE_WRITE);
      }
      if ( iobuffer == NULL)  {
        (*logfunc)(LOG_ERR, "rwrite_v3: malloc: ERROR occured (errno=%d)", errno);
        return -1;
      }
      (*logfunc)(LOG_DEBUG, "rwrite_v3 malloc buffer allocated : 0X%X\n",iobuffer);
    }

    byte_in_diskbuffer = 0;
    if (daemonv3_wrmt)
      iobuffer_p = NULL; /* For safety */
    else
      iobuffer_p = iobuffer;

    optlen = sizeof(maxseg);
    if (getsockopt(data_sock,IPPROTO_TCP,TCP_MAXSEG,(char *)&maxseg,&optlen) < 0) {
      (*logfunc)(LOG_ERR,"rfio","rwrite_v3: getsockopt: ERROR occured (errno=%d)",errno);
      return -1;
    }
    (*logfunc)(LOG_DEBUG,"rwrite_v3: max TCP segment: %d\n",maxseg);

    if (daemonv3_wrmt) {
      Csemaphore_init(&empty,daemonv3_wrmt_nbuf);
      Csemaphore_init(&full,0);

      if ((cid2 = Cthread_create((void *(*)(void *))&consume_thread,(void *)&fd)) < 0) {
        (*logfunc)(LOG_ERR,"Cannot create consumer thread : serrno=%d,errno=%d\n",serrno,errno);
        return(-1);
      }
    }

  }  /* End of if( first_write ) */

  /*
   * Reading data from the network.
   */

  while (1)  {
    FD_ZERO(&fdvar);
    FD_SET(ctrl_sock, &fdvar);
    FD_SET(data_sock, &fdvar);

    t.tv_sec = 10;
    t.tv_usec = 0;

    (*logfunc)(LOG_DEBUG,"rwrite: doing select\n");
    if( select(FD_SETSIZE, &fdvar, NULL, NULL, &t) < 0 )  {
      (*logfunc)(LOG_ERR, "rfio", "rwrite_v3: select failed (errno=%d)", errno);
      if (daemonv3_wrmt)
        wait_consumer_thread(cid2);
      return -1;
    }

    if( FD_ISSET(ctrl_sock, &fdvar) )  {
      int n, magic, code;

      /* Something received on the control socket */
      (*logfunc)(LOG_DEBUG, "ctrl socket: reading %d bytes\n",RQSTSIZE);
      if( (n = netread_timeout(ctrl_sock, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT)) != RQSTSIZE )  {
        if (n == 0)  {
          errno = ECONNRESET;
          (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
          if (daemonv3_wrmt)
            wait_consumer_thread(cid2);
          return -1;
        }
        else {
          (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
          if (daemonv3_wrmt)
            wait_consumer_thread(cid2);
          return -1;
        }
      }
      p = rqstbuf;
      unmarshall_WORD(p,magic);
      unmarshall_WORD(p,code);
      unmarshall_LONG(p,byte_written_by_client);

      if (code == RQST_CLOSE_V3)
        (*logfunc)(LOG_DEBUG,"close request:  magic: %x code: %x\n",magic,code);
      else
        (*logfunc)(LOG_DEBUG,"unknown request:  magic: %x code: %x\n",magic,code);

      (*logfunc)(LOG_DEBUG, "data socket: read_from_net=%d, written_by_client=%d\n",
          byte_read_from_network, byte_written_by_client);

      if( byte_read_from_network == byte_written_by_client )  {
        /*
         * Writing last buffered data on disk if necessary
         */

        if( byte_in_diskbuffer )  {
          (*logfunc)(LOG_DEBUG, "writing last %d bytes on disk\n",byte_in_diskbuffer);
          if (daemonv3_wrmt) {
            array[produced % daemonv3_wrmt_nbuf].len = byte_in_diskbuffer;
            produced++;
            Csemaphore_up(&full);

            /* Indicate to the consumer thread that writing is finished */
            Csemaphore_down(&empty);
            array[produced % daemonv3_wrmt_nbuf].len = 0;
            produced++;
            Csemaphore_up(&full);
          }
          else {
            status = write(fd,iobuffer,byte_in_diskbuffer);
            /* If the write is successfull but incomplete (fs is full) we
               report the ENOSPC error immediately in order to simplify the
               code */
            if ((status > 0) && (status != byte_in_diskbuffer)) {
              status = -1;
              errno = ENOSPC;
            }
          }
        }  /* if( byte_in_diskbuffer ) */
        else {
          if (daemonv3_wrmt) {
            /* Indicate to the consumer thread that writing is finished */
            Csemaphore_down(&empty);
            array[produced % daemonv3_wrmt_nbuf].len = 0;
            produced++;
            Csemaphore_up(&full);
          }
        }
        if (daemonv3_wrmt) {
          (*logfunc)(LOG_INFO,"Joining thread\n");
          /* Wait for consumer thread */
          /* We can then safely catch deferred disk write errors */
          if (Cthread_join(cid2,NULL) < 0) {
            (*logfunc)(LOG_ERR,"Error joining consumer, serrno=%d\n",serrno);
            return(-1);
          }

          /* Catch deferred disk errors, if any */
          if (Cthread_mutex_lock(&write_error)) {
            (*logfunc)(LOG_ERR,"Cannot get mutex : serrno=%d",serrno);
            return(-1);
          }

          if (write_error) {
            status = -1;
            saved_errno = write_error;
          }
          else
            status = byte_in_diskbuffer;

          if (Cthread_mutex_unlock(&write_error)) {
            (*logfunc)(LOG_ERR,"Cannot release mutex : serrno=%d",serrno);
            return(-1);
          }
        }
        if ((daemonv3_wrmt) && (status == -1))
          errno = saved_errno;

        rcode = (status < 0) ? errno:0;

        if (status < 0)  {
          p = rqstbuf;
          marshall_WORD(p,REP_ERROR);
          marshall_LONG(p,status);
          marshall_LONG(p,rcode);
          (*logfunc)(LOG_ERR, "rwrite_v3: status %d (%s), rcode %d\n", status,
              strerror(errno), rcode);
          errno = ECONNRESET;
          if( (n = netwrite_timeout(s, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT)) != RQSTSIZE )  {
            (*logfunc)(LOG_ERR, "rwrite_v3: netwrite_timeout(): %s\n", strerror(errno));
            /* No need to wait consumer thread here since it already exited after error */
            return -1;
          }

          /* No deadlock here since the client has already sent a CLOSE request
             (thus no data is still in transit) */
          (*logfunc)(LOG_DEBUG, "rwrite_v3: waiting ack for error\n");
          if ((n = netread_timeout(ctrl_sock,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT)) != RQSTSIZE) {
            if (n == 0)  {
              errno = ECONNRESET;
              (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
              /* No need to wait consumer thread here since it already exited after error */
              return -1;
            }
            else    {
              (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
              /* No need to wait consumer thread here since it already exited after error */
              return -1;
            }
          }
        }
        else   {
          myinfo.wnbr += byte_in_diskbuffer;
          if (byte_in_diskbuffer)
            myinfo.writop++;
          byte_in_diskbuffer = 0;
          if (daemonv3_wrmt)
            iobuffer_p = NULL; /* For safety */
          else
            iobuffer_p = iobuffer;
        }

        if (!daemonv3_wrmt) {
          (*logfunc)(LOG_DEBUG,"freeing iobuffer at 0X%X\n",iobuffer);
          if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
            free_page_aligned(iobuffer);
          } else {
            free(iobuffer);
          }
        }
        else {
          for (el=0; el < daemonv3_wrmt_nbuf; el++) {
            (*logfunc)(LOG_DEBUG,"freeing array element %d at 0X%X\n",el,array[el].p);
            if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
              free_page_aligned(array[el].p);
            } else {
              free(array[el].p);
            }
          }
          (*logfunc)(LOG_DEBUG,"freeing array at 0X%X\n",array);
          free(array);
        }
#ifdef USE_XFSPREALLOC
        if ( (p = getconfent("RFIOD","XFSPREALLOC",0)) ) {
          rfio_xfs_unresvsp64(fd, atoi(p), myinfo.wnbr);
        }
#endif
        srclose_v3(ctrl_sock, &myinfo, fd);
        return 0;
      } /*  if( byte_read_from_network == byte_written_by_client ) */
      else  {
        int diff;

        diff = byte_written_by_client - byte_read_from_network;
        if (byte_in_diskbuffer + diff > DISKBUFSIZE_WRITE)  {
          /* If previous buffer is empty then we must take a new one */
          if ((daemonv3_wrmt) && (byte_in_diskbuffer == 0)) {
            Csemaphore_down(&empty);
            iobuffer = array[produced % daemonv3_wrmt_nbuf].p;
          }
          iobuffer = (char*)realloc(iobuffer, byte_in_diskbuffer + diff);

          (*logfunc)(LOG_DEBUG, "data socket: realloc done to get %d additional bytes, buffer 0X%X\n", diff, iobuffer);
          if (iobuffer == NULL)  {
            (*logfunc)(LOG_ERR, "realloc failed: %s\n", strerror(errno));
            if (daemonv3_wrmt)
              wait_consumer_thread(cid2);
            return -1;
          }
          iobuffer_p = iobuffer + byte_in_diskbuffer;
          if (daemonv3_wrmt) {
            (*logfunc)(LOG_DEBUG,"Updating circular elem %d to address %X\n",produced % daemonv3_wrmt_nbuf,iobuffer);
            /* Update circular array element to take reallocation into account */
            array[produced % daemonv3_wrmt_nbuf].p = iobuffer;
            array[produced % daemonv3_wrmt_nbuf].len = byte_in_diskbuffer + diff;
          }

        } /* buffer reallocation was necessary */

        /* If previous buffer is empty then we must take a new one */
        if ((daemonv3_wrmt) && (byte_in_diskbuffer == 0)) {
          Csemaphore_down(&empty);
          iobuffer = iobuffer_p = array[produced % daemonv3_wrmt_nbuf].p;
        }
        (*logfunc)(LOG_DEBUG, "data socket: reading residu %d bytes\n", diff);
        if( (n = netread_timeout(data_sock, iobuffer_p, diff, RFIO_DATA_TIMEOUT)) != diff )  {
          if (n == 0)   {
            errno = ECONNRESET;
            (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
            if (daemonv3_wrmt)
              wait_consumer_thread(cid2);
            return -1;
          }
          else  {
            (*logfunc)(LOG_ERR, "read data residu socket: read(): %s\n", strerror(errno));
            if (daemonv3_wrmt)
              wait_consumer_thread(cid2);
            return -1;
          }
        }
        byte_read_from_network += diff;
        byte_in_diskbuffer += diff;
        /*
         * Writing data on disk.
         */

        (*logfunc)(LOG_DEBUG, "writing %d bytes on disk\n",byte_in_diskbuffer);
        if (daemonv3_wrmt) {
          array[produced % daemonv3_wrmt_nbuf].len = byte_in_diskbuffer;
          produced++;
          Csemaphore_up(&full);

          /* Indicate to the consumer thread that writing is finished */
          Csemaphore_down(&empty);
          array[produced % daemonv3_wrmt_nbuf].len = 0;
          produced++;
          Csemaphore_up(&full);
        }
        else {
          status = write(fd, iobuffer, byte_in_diskbuffer);
          /* If the write is successfull but incomplete (fs is full) we
             report the ENOSPC error immediately in order to simplify the
             code */
          if ((status > 0) && (status != byte_in_diskbuffer)) {
            status = -1;
            errno = ENOSPC;
          }
        }

        if (daemonv3_wrmt) {
          (*logfunc)(LOG_INFO,"Joining thread\n");
          /* Wait for consumer thread */
          /* We can then safely catch deferred disk write errors */
          if (Cthread_join(cid2,NULL) < 0) {
            (*logfunc)(LOG_ERR,"Error joining consumer, serrno=%d\n",serrno);
            return(-1);
          }

          /* Catch deferred disk write errors, if any */
          if (Cthread_mutex_lock(&write_error)) {
            (*logfunc)(LOG_ERR,"Cannot get mutex : serrno=%d",serrno);
            return(-1);
          }

          if (write_error) {
            status = -1;
            saved_errno = write_error;
          }
          else
            status = byte_in_diskbuffer;

          if (Cthread_mutex_unlock(&write_error)) {
            (*logfunc)(LOG_ERR,"Cannot release mutex : serrno=%d",serrno);
            return(-1);
          }
        }
        if ((daemonv3_wrmt) && (status == -1))
          errno = saved_errno;
        rcode = (status<0) ? errno:0;

        if (status < 0)  {
          p = rqstbuf;
          marshall_WORD(p, REP_ERROR);
          marshall_LONG(p, status);
          marshall_LONG(p, rcode);
          (*logfunc)(LOG_DEBUG, "rwrite_v3: status %d, rcode %d\n", status, rcode);
          errno = ECONNRESET;
          if( (n = netwrite_timeout(s, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT)) != RQSTSIZE )  {
            (*logfunc)(LOG_ERR, "rwrite_v3: netwrite_timeout(): %s\n", strerror(errno));
            /* Consumer thread already exited */
            return -1;
          }
          /* No deadlock possible here since all the data sent by the client
             using the data socket has been read at this point */

          (*logfunc)(LOG_DEBUG, "rwrite_v3: waiting ack for error\n");
          if ((n = netread_timeout(ctrl_sock,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT)) != RQSTSIZE) {
            if (n == 0)  {
              errno = ECONNRESET;
              (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
              /* Consumer thread already exited */
              return -1;
            }
            else  {
              (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
              /* Consumer thread already exited */
              return -1;
            }
          }
        }
        else   {
          myinfo.wnbr += byte_in_diskbuffer;
          myinfo.writop++;
          byte_in_diskbuffer = 0;
          if (daemonv3_wrmt)
            iobuffer_p = NULL; /* For safety */
          else
            iobuffer_p = iobuffer;
        }
        if (!daemonv3_wrmt) {
          (*logfunc)(LOG_DEBUG,"freeing iobuffer at 0X%X\n",iobuffer);
          if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
            free_page_aligned(iobuffer);
          } else {
            free(iobuffer);
          }
        }
        else {
          for (el=0; el < daemonv3_wrmt_nbuf; el++) {
            (*logfunc)(LOG_DEBUG,"freeing array element %d at 0X%X\n",el,array[el].p);
            if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
              free_page_aligned(array[el].p);
            } else {
              free(array[el].p);
            }
          }
          (*logfunc)(LOG_DEBUG,"freeing array at 0X%X\n",array);
          free(array);
        }
        srclose_v3(ctrl_sock, &myinfo, fd);
        return 0;
      }
    }


    if (FD_ISSET(data_sock,&fdvar)) {
      int n,can_be_read;

      if ((daemonv3_wrmt) && (byte_in_diskbuffer == 0)) {
        (*logfunc)(LOG_DEBUG, "Data received on data socket, new buffer %d requested\n",produced % daemonv3_wrmt_nbuf);
        Csemaphore_down(&empty);
        iobuffer = iobuffer_p = array[produced % daemonv3_wrmt_nbuf].p;
      }

      (*logfunc)(LOG_DEBUG,"iobuffer_p = %X,DISKBUFSIZE_WRITE = %d\n",iobuffer_p,DISKBUFSIZE_WRITE);
        if( (n = read(data_sock, iobuffer_p, DISKBUFSIZE_WRITE-byte_in_diskbuffer)) <= 0 )
          {
            if (n == 0)  {
              errno = ECONNRESET;
              (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
              if (daemonv3_wrmt)
                wait_consumer_thread(cid2);
              return -1;
            }
            else  {
              (*logfunc)(LOG_DEBUG, "read data socket: read(): %s\n", strerror(errno));
              if (daemonv3_wrmt)
                wait_consumer_thread(cid2);
              return -1;
            }
          }
        else  {
          can_be_read = n;
          (*logfunc)(LOG_DEBUG, "read data socket : %d bytes\n", can_be_read);
        }

      byte_read_from_network += can_be_read;
      byte_in_diskbuffer += can_be_read;
      iobuffer_p += can_be_read;

      /*
       * Writing data on disk.
       */

      if (byte_in_diskbuffer == DISKBUFSIZE_WRITE)  {
        (*logfunc)(LOG_DEBUG, "writing %d bytes on disk\n", byte_in_diskbuffer);
        if (daemonv3_wrmt) {
          array[produced % daemonv3_wrmt_nbuf].len = byte_in_diskbuffer;
          produced++;
          Csemaphore_up(&full);
        }
        else {
          status = write(fd, iobuffer, byte_in_diskbuffer);
          /* If the write is successfull but incomplete (fs is full) we
             report the ENOSPC error immediately in order to simplify the
             code */
          if ((status > 0) && (status != byte_in_diskbuffer)) {
            status = -1;
            errno = ENOSPC;
          }
        }

        if (daemonv3_wrmt) {
          if (Cthread_mutex_lock(&write_error)) {
            (*logfunc)(LOG_ERR,"Cannot get mutex : serrno=%d",serrno);
            return(-1);
          }

          if (write_error) {
            status = -1;
            saved_errno = write_error;
          }
          else
            status = byte_in_diskbuffer;

          if (Cthread_mutex_unlock(&write_error)) {
            (*logfunc)(LOG_ERR,"Cannot release mutex : serrno=%d",serrno);
            return(-1);
          }
        }
        if ((daemonv3_wrmt) && (status == -1))
          errno = saved_errno;

        rcode = (status < 0) ? errno:0;

        if (status < 0)  {
          p = rqstbuf;
          marshall_WORD(p, REP_ERROR);
          marshall_LONG(p, status);
          marshall_LONG(p, rcode);
          (*logfunc)(LOG_ERR, "rwrite_v3: status %d (%s), rcode %d\n", status,strerror(errno), rcode);
          errno = ECONNRESET;
          if ((n = netwrite_timeout(s,rqstbuf,RQSTSIZE,RFIO_CTRL_TIMEOUT)) != RQSTSIZE)  {
            (*logfunc)(LOG_ERR, "rwrite_v3: netwrite_timeout(): %s\n", strerror(errno));
            /* Consumer thread already exited after error */
            return -1;
          }

          /*
           * To avoid overflowing the local thread stack we must
           * put dummy on heap
           */
          sizeofdummy = 256 * 1024;
          dummy = (unsigned char *)malloc(sizeof(unsigned char) * sizeofdummy);
          if (dummy == NULL)
            (*logfunc)(LOG_ERR, "rwrite_v3: malloc(): %s\n", strerror(errno));

          /*
            There is a potential deadlock here since the client may be stuck
            in netwrite (cf rfio_write_v3), trying to write data on the data
            socket while both socket buffers (client + server) are full.
            To avoid this problem, we empty the data socket while waiting
            for the ack to be received on the control socket
          */

          while (1)  {
            FD_ZERO(&fdvar2);
            FD_SET(ctrl_sock,&fdvar2);
            FD_SET(data_sock,&fdvar2);

            t.tv_sec = 1;
            t.tv_usec = 0;

            (*logfunc)(LOG_DEBUG,"rwrite_v3: doing select after error writing on disk\n");
              if( select(FD_SETSIZE, &fdvar2, NULL, NULL, &t) < 0 )
                {
                  (*logfunc)(LOG_ERR,"rfio","rwrite_v3: select fdvar2 failed (errno=%d)",errno);
                  /* Consumer thread already exited after error */
                  return -1;
                }

            if( FD_ISSET(ctrl_sock, &fdvar2) )  {
              /* The ack has been received on the control socket */

              (*logfunc)(LOG_DEBUG, "rwrite_v3: waiting ack for error\n");
              n = netread_timeout(ctrl_sock, rqstbuf, RQSTSIZE, RFIO_CTRL_TIMEOUT);
              if (n != RQSTSIZE)  {
                if (n == 0)  {
                  errno = ECONNRESET;
                  (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
                  /* Consumer thread already exited after error */
                  return -1;
                }
                else  {
                  (*logfunc)(LOG_ERR, "read ctrl socket: read(): %s\n", strerror(errno));
                  /* Consumer thread already exited after error */
                  return -1;
                }
              }
              else  {
                if (!daemonv3_wrmt) {
                  (*logfunc)(LOG_DEBUG,"freeing iobuffer at 0X%X\n",iobuffer);
                  if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
                    free_page_aligned(iobuffer);
                  } else {
                    free(iobuffer);
                  }
                }
                else {
                  for (el=0; el < daemonv3_wrmt_nbuf; el++) {
                    (*logfunc)(LOG_DEBUG,"freeing array element %d at 0X%X\n",el,array[el].p);
                    if ( getconfent("RFIOD", "DIRECTIO", 0) ) {
                      free_page_aligned(array[el].p);
                    } else {
                      free(array[el].p);
                    }
                  }
                  (*logfunc)(LOG_DEBUG,"freeing array at 0X%X\n",array);
                  free(array);
                }
                /* srclose_v3(ctrl_sock,&myinfo,fd); */
                return 0;
              }
            }

            if (FD_ISSET(data_sock,&fdvar2))  {
              /* Read as much data as possible from the data socket */

              (*logfunc)(LOG_DEBUG, "rwrite_v3: emptying data socket (last disk write)\n");
                n = read(data_sock, dummy, sizeofdummy);
              if( n <= 0 )
                {
                  (void) free(dummy);

                  if (n == 0)  {
                    errno = ECONNRESET;
                    (*logfunc)(LOG_ERR, "read emptying data socket: read(): %s\n", strerror(errno));
                    /* Consumer thread already exited after error */
                    return -1;
                  }
                  else {
                    (*logfunc)(LOG_ERR, "read emptying data socket: read(): %s\n", strerror(errno));
                    /* Consumer thread already exited after error */
                    return -1;
                  }
                }
              (*logfunc)(LOG_DEBUG, "rwrite_v3: emptying data socket, %d bytes read\n",n);
            }
          }
          free(dummy);
        }
        else  {
          myinfo.wnbr += byte_in_diskbuffer;
          myinfo.writop++;
          byte_in_diskbuffer = 0;
          iobuffer_p = iobuffer;
        }
      }
    }
  }
}

int srlseek_v3(int s,
               struct rfiostat *infop,
               int fd)
{
  int  status;
  int  rcode;
  int  offset;
  int  how;
  char *p;

  (void)infop;
  p = rqstbuf + 2*WORDSIZE;
  unmarshall_LONG(p,offset);
  unmarshall_LONG(p,how);
  (*logfunc)(LOG_DEBUG,"rlseek_v3(%d, %d): offset %d, how: %d\n",s,fd,offset,how);
  status = lseek(fd, offset, how);
  rcode = (status < 0) ? errno:0;
  (*logfunc)(LOG_DEBUG,"rlseek_v3: status %d, rcode %d\n",status,rcode);
  p = rqstbuf;
  marshall_WORD(p,RQST_LSEEK_V3);
  marshall_LONG(p,status);
  marshall_LONG(p,rcode);

  if (netwrite_timeout(s,rqstbuf,WORDSIZE+2*LONGSIZE,RFIO_CTRL_TIMEOUT) != (WORDSIZE+2*LONGSIZE))  {
    (*logfunc)(LOG_ERR,"rlseek: netwrite_timeout(): %s\n", strerror(errno));
    return -1;
  }
  return status;
}
