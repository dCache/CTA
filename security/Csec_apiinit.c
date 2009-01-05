/*
 * Copyright (C) 2003 by CERN/IT/ADC/CA
 * All rights reserved
 */
 
/*	Csec_apiinit - allocate thread specific or global structures */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include "Cglobals.h"
#include "serrno.h"

#include "Csec.h"

static int Csec_api_key = -1;

int DLL_DECL
Csec_apiinit(thip)
     struct Csec_api_thread_info **thip;
{
  Cglobals_get (&Csec_api_key,
		(void **) thip, sizeof(struct Csec_api_thread_info));

  if (*thip == NULL) {
    fprintf(stderr, "ENOMEM\n");
    serrno = ENOMEM;
    return (-1);
  }

  if ((*thip)->init_done == 0) {
    (*thip)->init_done = 1;
    /* BEWARE the init_done must be done BEFORE the call to
       setup_trace(), because setup_trace calls apiinit itself ! */
    Csec_setup_trace();
  }
    
  return (0);
}

