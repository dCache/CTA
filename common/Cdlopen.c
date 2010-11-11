/*
 * $Id: Cdlopen.c,v 1.5 2008/12/04 15:05:20 itglp Exp $
 */

/*
 * $RCSfile: Cdlopen.c,v $ $Revision: 1.5 $ $Date: 2008/12/04 15:05:20 $ CERN IT-ADC/CA Jean-Damien Durand
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Cdlopen_api.h"
#include "Cglobals.h"     /* dlerror() is not required to be reentrant, i.e. thread-safe */
#include "Csnprintf.h"
#include "Cmutex.h"

static char *C__Cdlopen_buffer();

#ifndef _MSC_VER
static int Cdlopen_mutex = -1;
#endif

static int Cdlopen_buffer_key = -1; /* Our static key, integer, init value -1 */
#define CDLOPEN_BUFLEN 1024
#define Cdlopen_buffer (C__Cdlopen_buffer())

static char my_Cdlopen_buffer[CDLOPEN_BUFLEN+1]; /* If Cglobals_get error in order not to crash */

static char *C__Cdlopen_buffer()
{
  char *var;
  /* Call Cglobals_get */
  Cglobals_get(&Cdlopen_buffer_key,
	       (void **) &var,
	       (size_t) (CDLOPEN_BUFLEN+1)
	       );
  /* If error, var will be NULL */
  if (var == NULL) {
    return(my_Cdlopen_buffer);
  }
  return(var);
}

#ifdef _MSC_VER
#include <windows.h>

/* Note: flag is not used on Windows */
void *Cdlopen(const char *filename,
              int flag)
{
  return((void *)LoadLibrary(filename));
}

/* Note: GetLastError() is thread-safe */
char *Cdlerror(void) {
  DWORD getlasterror = GetLastError();
  Csnprintf(Cdlopen_buffer, CDLOPEN_BUFLEN, "GetLastError return code: %u", getlasterror);
  Cdlopen_buffer[CDLOPEN_BUFLEN] = '\0'; /* Who knows */
  return(Cdlopen_buffer);
}

void *Cdlsym(void *handler, const char *symbol) {
  return((void *)GetProcAddress((HINSTANCE)handler, symbol));
}
int Cdlclose(void *handler) {
  return(!FreeLibrary((HINSTANCE)handler));
}
#else
/* Assuming standard dlopen() interface */
void *Cdlopen(const char *filename,
              int flag)
{
  return(dlopen(filename, flag));
}

/* Note: dlerror() is NOT required to be reentrant, i.e. thread-safe */
/* so we have to protect with a mutex */
char *Cdlerror(void) {
  int rc;

  rc = Cmutex_lock(&Cdlopen_mutex,10); /* Oupss */
  char* buf = dlerror();
  if (buf != NULL) {
    Csnprintf(Cdlopen_buffer, CDLOPEN_BUFLEN, "%s", buf);
    Cdlopen_buffer[CDLOPEN_BUFLEN] = '\0'; /* Who knows */
    buf = Cdlopen_buffer;
  }
  if (rc == 0) {
    Cmutex_unlock(&Cdlopen_mutex);
  }
  return buf;
}

void *Cdlsym(void *handler, const char *symbol) {
  return(dlsym(handler, symbol));
}
int Cdlclose(void *handler) {
  return(dlclose(handler));
}
#endif
