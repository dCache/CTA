
/*
 * Copyright (C) 1999-2003 by CERN/IT/PDP/DM
 * All rights reserved
 */


/*
 * Cnetdb.c - CASTOR MT-safe wrappers on netdb routines.
 */ 

#include <sys/types.h>
#include <stddef.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <Cglobals.h>
#include <Cnetdb.h>

struct hostent *Cgethostbyname(name)
const char *name;
{
#if defined(__APPLE__)
    /*
     * If single-thread compilation we don't do anything.
     * Also: Windows, MACOSX and Digital UNIX v4 and higher already
     *       provide thread safe version.
     */
    return(gethostbyname(name));
#elif defined(__linux__)
    static int hostent_key = -1;
    static int hostdata_key = -1;
    int rc;
    struct hostent *hp;
    void *result = NULL;
    void *buffer = NULL;
    int bufsize = 1024;
    int h_errnoop = 0;

    Cglobals_get(&hostent_key,&result,sizeof(struct hostent));
    Cglobals_get(&hostdata_key,&buffer,bufsize);

    if ( result == NULL || buffer == NULL ) {
        h_errno = NO_RECOVERY;
        return(NULL);
    }
    rc = gethostbyname_r(name,result,buffer,bufsize,&hp,&h_errnoop);
    h_errno = h_errnoop;
    if ( rc == -1 ) hp = NULL;
    return(hp);
#else
    /*
     * Not supported
     */
    return(NULL);
#endif
}

struct hostent *Cgethostbyaddr(addr,len,type)
const void *addr;
size_t len;
int type;
{
#if defined(__APPLE__)
    /*
     * If single-thread compilation we don't do anything.
     * Also: Windows, MACOSX and Digital UNIX v4 and higher already
     *       provide thread safe version.
     */
    return(gethostbyaddr(addr,len,type));
#elif defined(__linux__)
    static int hostent_key = -1;
    static int hostdata_key = -1;
    int rc;
    struct hostent *hp;
    void *result = NULL;
    void *buffer = NULL;
    int bufsize = 1024;
    int h_errnoop = 0;

    Cglobals_get(&hostent_key,&result,sizeof(struct hostent));
    Cglobals_get(&hostdata_key,&buffer,bufsize);

    if ( result == NULL || buffer == NULL ) {
        h_errno = NO_RECOVERY;
        return(NULL);
    }
    rc = gethostbyaddr_r(addr,len,type,result,buffer,bufsize,&hp,&h_errnoop);
    h_errno = h_errnoop;
    if ( rc == -1 ) hp = NULL;
    return(hp);
#else
    /*
     * Not supported
     */
    return(NULL);
#endif
}

struct servent *Cgetservbyname(name,proto)
const char *name;
const char *proto;
{
#if defined(__APPLE__)
    /*
     * If single-thread compilation we don't do anything.
     * Also: Windows, MACOSX and Digital UNIX v4 and higher already
     *       provide thread safe version.
     */
    return(getservbyname(name,proto));
#elif defined(__linux__)
    static int servent_key = -1;
    static int servdata_key = -1;
    int rc;
    struct servent *sp;
    void *result = NULL;
    void *buffer = NULL;
    int bufsize = 1024;

    Cglobals_get(&servent_key,&result,sizeof(struct servent));
    Cglobals_get(&servdata_key,&buffer,bufsize);

    if ( result == NULL || buffer == NULL ) {
        return(NULL);
    }
    rc = getservbyname_r(name,proto,result,buffer,bufsize,&sp);
    if ( rc == -1 ) sp = NULL;
    return(sp);
#else
    /*
     * Not supported
     */
    return(NULL);
#endif
}
