/*
 * Copyright (C) 1990-2001 by CERN CN-PDP/CS
 * All rights reserved
 */

/*
 */

/* log.h        generalized logging facilities                          */

#pragma once

#define LOG_NOLOG       -1      /* Don't log                            */
#ifndef _SHIFT_H_INCLUDED_
#include <osdep.h>
#endif

EXTERN_C void (*logfunc) (int, char *, ...);  
                                /* logging function to use */
EXTERN_C void initlog (const char *const , const int, char *const);
EXTERN_C void logit (int, char *, ...);
EXTERN_C void setlogbits (int);
EXTERN_C int getloglv (void);

#ifndef _SYSLOG_WIN32_H
#include <syslog.h>             /* system logger definitions            */
#endif
#define LOG_EMERG       0       /* system is unusable                   */
#define LOG_ALERT       1       /* action must be taken immediately     */
#define LOG_CRIT        2       /* critical conditions                  */
#define LOG_ERR         3       /* error conditions                     */
#define LOG_WARNING     4       /* warning conditions                   */
#define LOG_NOTICE      5       /* normal but signification condition   */
#define LOG_INFO        6       /* informational                        */
#define LOG_DEBUG       7       /* debug-level messages                 */

