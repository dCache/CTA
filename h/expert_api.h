/*
 * 
 * Copyright (C) 2004 by CERN/IT/ADC
 * All rights reserved
 *
 * @(#)$RCSfile: expert_api.h,v $ $Revision: 1.2 $ $Date: 2008/07/28 16:55:05 $ CERN IT-ADC/CA Vitaly Motyakov";
 *
 */

#ifndef EXPERT_API_H
#define EXPERT_API_H

#include "expert.h"

/* Function prototypes */

EXTERN_C int expert_send_request _PROTO((int*, int));
EXTERN_C int expert_send_data _PROTO((int, const char*, int));
EXTERN_C int expert_receive_data _PROTO((int, char*, int, int));
EXTERN_C int send2expert _PROTO((int*, char*, int));
EXTERN_C int getexpertrep _PROTO((int, int*, int*, int*));

#endif
