/*
 * $Id: getacct.c,v 1.4 1999/07/22 12:31:14 obarring Exp $
 * $Log: getacct.c,v $
 * Revision 1.4  1999/07/22 12:31:14  obarring
 * Add _THREAD_SAFE for strtok_r on AIX
 *
 * Revision 1.3  1999/07/21 20:07:46  jdurand
 * *** empty log message ***
 *
 * Revision 1.2  1999/07/21 16:25:41  obarring
 * Make MT safe
 *
 */

/*
 * Copyright (C) 1990-1999 by CERN IT-PDP/DM
 * All rights reserved
 */

#ifndef lint
static char cvsId[] = "$Id: getacct.c,v 1.4 1999/07/22 12:31:14 obarring Exp $";
#endif /* not lint */

/*  getacct() - Getting the current account id  */

/*
 * If the environment variable ACCOUNT is set
 *      Check if it is a valid account id for the user
 * Else
 *      Get the primary account id of the user
 * Endif
 *
 * The look-up policy is the same as for passwd.
 */

#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#if !defined(_WIN32)
#include <unistd.h>
/*
 * _WIN32 strtok() is already MT safe where as others wait
 * for next POXIS release
 */
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
#define strtok(X,Y) strtok_r(X,Y,&last)
#endif /* _REENTRANT || _THREAD_SAFE */
#else
extern uid_t getuid();
#endif
#include <stdlib.h>

#include <Cglobals.h>
#include "getacct.h"

extern char *getacctent();


char    *getacct_r(resbuf,resbufsiz) 
char *resbuf;
size_t resbufsiz;
{ 
    char      *account = NULL;    /* Pointer to the account env variable  */
    struct passwd *pwd = NULL;        /* Pointer to the password entry    */
    char      buf[BUFSIZ];
    char      *cprv, *rv;
#if !defined(_WIN32) && (defined(_REENTRANT) || defined(_THREAD_SAFE))
    char      *last = NULL;
#endif /* !_WIN32 && (_REENTRANT || _THREAD_SAFE) */ 

    cprv = rv = NULL;

    /*
     * Get environment variable.
     */

    account = getenv(ACCOUNT_VAR);

    if (account != NULL) {
        if (strcmp(account, EMPTY_STR) == 0) account = NULL;
    }

    /*
     * Get password entry.
     */

    if ((pwd = getpwuid(getuid())) == NULL) return(NULL);

    /*
     * Get account file entry
     */

    cprv = getacctent(pwd, account, buf, (int)sizeof(buf));

    /*
     * Extract account id
     */

    if (cprv != NULL) {
        if ((rv = strtok(cprv, COLON_STR)) != NULL) {
            if ((rv = strtok((char *)NULL, COLON_STR)) != NULL) {
                strcpy(resbuf, rv);
                return(resbuf);
            }
        }
    }
    return(NULL);    
}

static int getacct_key = -1;

char *getacct()
{
    char *resbuf = NULL;

    Cglobals_get(&getacct_key,(void **) &resbuf,BUFSIZ+1);
    if ( resbuf == NULL ) return(NULL);

    return(getacct_r(resbuf,BUFSIZ+1));
}


/*
 * Fortran wrapper
 */

/*FH*   to be done      RC = XYACCT()           */
