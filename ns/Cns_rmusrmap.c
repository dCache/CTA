/*
 * Copyright (C) 2005 by CERN/IT/GD/SC
 * All rights reserved
 */

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "marshall.h"
#include "Cns_api.h"
#include "Cns.h"
#include "serrno.h"

/* Cns_rmusrmap - suppress user entry corresponding to a given uid/name */

int
Cns_rmusrmap(uid_t uid, char *username)
{
  int c;
  char func[16];
  int msglen;
  char *q;
  char *sbp;
  char sendbuf[REQBUFSZ];
  struct Cns_api_thread_info *thip;

  strcpy (func, "Cns_rmgrpmap");
  if (Cns_apiinit (&thip))
    return (-1);

  /* Build request header */

  sbp = sendbuf;
  marshall_LONG (sbp, CNS_MAGIC);
  marshall_LONG (sbp, CNS_RMUSRMAP);
  q = sbp; /* save pointer. The next field will be updated */
  msglen = 3 * LONGSIZE;
  marshall_LONG (sbp, msglen);

  /* Build request body */

  marshall_LONG (sbp, uid);
  if (username) {
    marshall_STRING (sbp, username);
  } else {
    marshall_STRING (sbp, "");
  }

  msglen = sbp - sendbuf;
  marshall_LONG (q, msglen); /* update length field */

  c = send2nsd (NULL, NULL, sendbuf, msglen, NULL, 0);
  return (c);
}
