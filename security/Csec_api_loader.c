/*
 * $id$
 * Copyright (C) 2003 by CERN/IT/ADC/CA Benjamin Couturier
 * All rights reserved
 */

/*
 * Csec_api_loader.c - API function used for authentication in CASTOR
 */

#include <osdep.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "marshall.h"
#include "serrno.h"
#include "Cnetdb.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#ifndef lint
static char sccsid[] = "@(#)Csec_api_loader.c,v 1.1 2004/01/12 10:31:39 CERN IT/ADC/CA Benjamin Couturier";
#endif

#include <Cmutex.h>
#include <Cglobals.h>
#include <Cthread_api.h>
#include <net.h>
#include <serrno.h>
#include <dlfcn.h>

#include <Csec.h>

/* Macro to initialize one symbol in the context structure */
#define DLSETFUNC(CTX, HDL, SYM) if ((CTX->SYM = dlsym(HDL, #SYM "_impl")) == NULL) { \
    serrno =  ESEC_NO_SECMECH;                                  \
    Csec_errmsg(func, "Error finding symbol %s: %s\n",		\
		#SYM, dlerror());					\
    CTX->shhandle = NULL;						\
    return NULL; }

/**
 * Gets the shared library corresponding to the context !
 */
void *Csec_get_shlib(Csec_context_t *ctx) {
  char filename[CA_MAXNAMELEN];
  void *handle;
  char *func = "Csec_get_shlib";
 
 /* Checking input */
  if (ctx == NULL) {
    serrno = EINVAL;
    Csec_errmsg(func, "Context is NULL !");
    return NULL;
  }

  /* Creating the library name */
  snprintf(filename, CA_MAXNAMELEN, "libCsec_plugin_%s.so", ctx->protocols[ctx->current_protocol].id);
  Csec_trace(func, "Using shared library <%s> for mechanism <%s>\n",
	     filename,
	     ctx->protocols[ctx->current_protocol].id);
    
  handle = dlopen(filename, RTLD_NOW);
  
  if (handle == NULL) {
    char dlerrmsg[ERRBUFSIZE+1];
    serrno =  ESEC_NO_SECMECH;
    strncpy(dlerrmsg, dlerror(), ERRBUFSIZE);
    
    ctx->shhandle = NULL;
    Csec_trace(func, "Error opening shared library %s: %s\n", filename,
	       dlerrmsg);
    Csec_errmsg(func, "Error opening shared library %s: %s\n", filename,
		dlerrmsg);
    
    return NULL;
  }

  ctx->shhandle = handle;

  DLSETFUNC(ctx, handle, Csec_init_context);
  DLSETFUNC(ctx, handle, Csec_reinit_context);
  DLSETFUNC(ctx, handle, Csec_delete_connection_context);
  DLSETFUNC(ctx, handle, Csec_delete_creds);
  DLSETFUNC(ctx, handle, Csec_server_acquire_creds);
  DLSETFUNC(ctx, handle, Csec_server_establish_context_ext);
  DLSETFUNC(ctx, handle, Csec_client_establish_context);
  DLSETFUNC(ctx, handle, Csec_map2name);
  DLSETFUNC(ctx, handle, Csec_get_service_name);

  ctx->flags |= CSEC_CTX_SHLIB_LOADED;

  return handle;
    
}






