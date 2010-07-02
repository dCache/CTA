/*
 * $Id: popen.c,v 1.15 2009/06/03 13:47:56 sponcec3 Exp $
 */

/*
 * Copyright (C) 1994-2002 by CERN/IT/PDP/DM
 * All rights reserved
 */

/* popen.c       Remote pipe I/O - open file a file                      */

/*
 * System remote file I/O
 */
#define RFIO_KERNEL     1
#include <fcntl.h>
#include <sys/param.h>          /* For MAXHOSTNAMELEN definition  */
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "rfio.h"
#include "rfio_rfilefdt.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
extern RFILE *rfilefdt[MAXRFD] ;

RFILE *rfio_popen( rcom , type )
     char * rcom  ;
     char *type  ;
{

  char *host  ;
  RFILE *rfp  ;
  int rfp_index;
  char *p , *cp, *cp2    ;
  char command[MAXCOMSIZ]; /* command with remote syntax */
  struct passwd *pwuid ;
  char *pcom  ;
  int rt   ; /* daemon is in the site or remote ? */
  int rcode, status = 0 ;
  int len  ;
  FILE *file, *popen()  ;
  char localhost[MAXHOSTNAMELEN];
  char buf[BUFSIZ] ;

  INIT_TRACE("RFIO_TRACE");

  if ( (int)strlen(rcom) > MAXCOMSIZ -5 ) {
    serrno = SEUMSG2LONG ;
    return NULL ;
  }

  /*
   * get the stderr stream if any
   */
  strcpy(command, rcom) ;
  strcat (command, " 2>&1");
  /*
   * Allocate and initialize a remote file descriptor.
   */
  if ((rfp = (RFILE *)malloc(sizeof(RFILE))) == NULL)        {
    TRACE(2, "rfio", "rfio_popen: malloc(): ERROR occured (errno=%d)", errno);
    END_TRACE();
    return NULL ;
  }
  rfio_setup(rfp) ;
  TRACE(3,"rfio","Allocated buffer at %x",rfp);
  cp = strchr(command,':') ;
  cp2 = strchr(command,' ') ;

  /* Bug fix when having a : in the command line but the command is local */
  /* If the first space is before the ':', the command is really local */
  if (cp2 < cp)
    cp = NULL;

  if (cp != NULL) {
    *cp = '\0' ;
    host = command ;
    pcom =  cp + 1 ;
  }
  if ( gethostname(localhost, MAXHOSTNAMELEN) < 0) {
    TRACE(2,"rfio","gethostname() failed");
    TRACE(2,"rfio","freeing RFIO descriptor at 0X%X", rfp);
    (void) free((char *)rfp);
    END_TRACE();
    return NULL;
  }

  /*
   * file is local
   */
  if ( (cp == NULL) || !strcmp( host, localhost) || !strcmp(host,"localhost") ) {
    TRACE(3,"rfio","popen(%s,%s): local mode",command,type) ;
    if (cp == NULL)
    file = popen(command,type);
    else
    file = popen(pcom, type) ;
    rfio_errno = 0;
    if ( file == NULL ) {
      TRACE(1,"rfio","popen() failed ,error %d", errno) ;
      TRACE(2,"rfio","freeing RFIO descriptor at 0X%X", rfp);
      serrno = 0;
      (void) free((char *)rfp);
      END_TRACE();
      return (NULL) ;
    }
    rfp->fp_save = file;
    memcpy( &(rfp->fp), file, sizeof(FILE))  ;
    return ( rfp ) ;
  }
  /*
   * Parsing The command
   */

  TRACE(2,"rfio", "RFIO descriptor allocated");
  TRACE( 3, "rfio","rfio_popen(): host <%s>, command <%s>",host, pcom);
  if ( (rfp->s = rfio_connect(host , &rt)) < 0) {
    TRACE(2,"rfio","freeing RFIO descriptor at 0X%X", rfp);
    (void) free((char *)rfp);
    END_TRACE();
    return NULL ;
  }

  /*
   * Remote file table is not large enough.
   */
  if ((rfp_index = rfio_rfilefdt_allocentry(rfp->s)) == -1) {
    TRACE(2, "rfio", "freeing RFIO descriptor at 0X%X", rfp);
    (void) close(rfp->s);
    (void) free((char *)rfp);
    END_TRACE();
    errno= EMFILE ;
    return NULL ;
  }
  rfilefdt[rfp_index]=rfp;


  p= buf ;
  if ( (pwuid=getpwuid(geteuid())) == NULL) {
    TRACE(2, "rfio" ,"rfio_popen: cuserid error %s",strerror(errno));
    (void) free((char *)rfp);
    END_TRACE();
    return NULL ;
  }


  len = 2*WORDSIZE+strlen(type)+strlen(pcom)+strlen(pwuid->pw_name)+3 ;
  marshall_WORD(p,B_RFIO_MAGIC)  ;
  marshall_WORD(p,RQST_POPEN)  ;
  marshall_LONG(p,len)   ;
  if (netwrite_timeout(rfp->s,buf, RQSTSIZE, RFIO_CTRL_TIMEOUT) != RQSTSIZE ) {
    TRACE(2,"rfio","rfio_popen: write(): ERROR occured (errno=%d)",errno);
    free((char *)rfp) ;
    END_TRACE() ;
    return NULL ;
  }
  p = buf ;
  marshall_WORD(p,rfp->uid)  ;
  marshall_WORD(p,rfp->gid)  ;
  marshall_STRING(p,type)  ;
  marshall_STRING(p,pcom)  ;
  marshall_STRING(p,pwuid->pw_name) ;
  if (netwrite_timeout(rfp->s,buf, len, RFIO_CTRL_TIMEOUT) != len ) {
    TRACE(2,"rfio","rfio_popen: write(): ERROR occured (errno=%d)",errno);
    free((char *)rfp) ;
    END_TRACE() ;
    return NULL ;
  }

  /*
   * Getting status and current offset.
   */
  if (netread_timeout(rfp->s,buf, WORDSIZE+LONGSIZE, RFIO_CTRL_TIMEOUT) != (WORDSIZE+LONGSIZE)) {
    TRACE(2, "rfio","rfio_popen: read(): ERROR occured (errno=%d)", errno);
    free((char *)rfp);
    END_TRACE();
    return NULL ;
  }
  p = buf ;
  unmarshall_LONG(p, status) ;
  unmarshall_WORD(p, rcode) ;
  TRACE(1,"rfio","rfio_popen: return status(%d), rcode(%d) for fd(%d)",status,rcode, rfp->s) ;

  if (status < 0) {
    rfio_errno= rcode ;
    free((char *)rfp) ;
    END_TRACE() ;
    return NULL ;
  }
  else
    return rfilefdt[rfp_index] ;

}
