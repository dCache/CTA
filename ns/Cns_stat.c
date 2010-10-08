/*
 * Copyright (C) 1999-2005 by CERN/IT/PDP/DM
 * All rights reserved
 */

/* Cns_statx - get information about a file or a directory and
   return unique fileid */

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "marshall.h"
#include "Cns_api.h"
#include "Cns.h"
#include "serrno.h"

/* Cns_lstat - get information about a symbolic link */

int
Cns_lstat(const char *path, struct Cns_filestat *statbuf)
{
  char *actual_path;
  int c;
  char func[16];
  gid_t gid;
  int msglen;
  char *q;
  char *rbp;
  char repbuf[57];
  char *sbp;
  char sendbuf[REQBUFSZ];
  char server[CA_MAXHOSTNAMELEN+1];
  struct Cns_api_thread_info *thip;
  uid_t uid;
  u_signed64 zero = 0;

  strncpy (func, "Cns_lstat", 16);
  if (Cns_apiinit (&thip))
    return (-1);
  Cns_getid(&uid, &gid);

  if (! path || ! statbuf) {
    serrno = EFAULT;
    return (-1);
  }

  if (strlen (path) > CA_MAXPATHLEN) {
    serrno = ENAMETOOLONG;
    return (-1);
  }

  if (Cns_selectsrvr (path, thip->server, server, &actual_path))
    return (-1);

  /* Build request header */

  sbp = sendbuf;
  marshall_LONG (sbp, CNS_MAGIC2);
  marshall_LONG (sbp, CNS_LSTAT);
  q = sbp;        /* save pointer. The next field will be updated */
  msglen = 3 * LONGSIZE;
  marshall_LONG (sbp, msglen);

  /* Build request body */

  marshall_LONG (sbp, uid);
  marshall_LONG (sbp, gid);
  marshall_HYPER (sbp, thip->cwd);
  marshall_HYPER (sbp, zero);
  marshall_STRING (sbp, actual_path);

  msglen = sbp - sendbuf;
  marshall_LONG (q, msglen); /* update length field */

  c = send2nsd (NULL, server, sendbuf, msglen, repbuf, sizeof(repbuf));
  if (c == 0) {
    rbp = repbuf;
    unmarshall_HYPER (rbp, statbuf->fileid);
    unmarshall_WORD (rbp, statbuf->filemode);
    unmarshall_LONG (rbp, statbuf->nlink);
    unmarshall_LONG (rbp, statbuf->uid);
    unmarshall_LONG (rbp, statbuf->gid);
    unmarshall_HYPER (rbp, statbuf->filesize);
    unmarshall_TIME_T (rbp, statbuf->atime);
    unmarshall_TIME_T (rbp, statbuf->mtime);
    unmarshall_TIME_T (rbp, statbuf->ctime);
    unmarshall_WORD (rbp, statbuf->fileclass);
    unmarshall_BYTE (rbp, statbuf->status);
  }
  if (c && serrno == SENAMETOOLONG) serrno = ENAMETOOLONG;
  return (c);
}

int
Cns_statx(const char *path, struct Cns_fileid *file_uniqueid, struct Cns_filestat *statbuf)
{
  char *actual_path;
  int c;
  char func[16];
  gid_t gid;
  int msglen;
  char *q;
  char *rbp;
  char repbuf[57];
  char *sbp;
  char sendbuf[REQBUFSZ];
  char server[CA_MAXHOSTNAMELEN+1];
  struct Cns_api_thread_info *thip;
  uid_t uid;
  u_signed64 zero = 0;

  strncpy (func, "Cns_stat", 16);
  if (Cns_apiinit (&thip))
    return (-1);
  Cns_getid(&uid, &gid);

  if (! path || ! statbuf || ! file_uniqueid) {
    serrno = EFAULT;
    return (-1);
  }

  if (strlen (path) > CA_MAXPATHLEN) {
    serrno = ENAMETOOLONG;
    return (-1);
  }

  if (file_uniqueid && *file_uniqueid->server)
    strcpy (server, file_uniqueid->server);
  else
    if (Cns_selectsrvr (path, thip->server, server, &actual_path))
      return (-1);

  /* Build request header */

  sbp = sendbuf;
  marshall_LONG (sbp, CNS_MAGIC);
  marshall_LONG (sbp, CNS_STAT);
  q = sbp;        /* save pointer. The next field will be updated */
  msglen = 3 * LONGSIZE;
  marshall_LONG (sbp, msglen);

  /* Build request body */

  marshall_LONG (sbp, uid);
  marshall_LONG (sbp, gid);
  marshall_HYPER (sbp, thip->cwd);
  if (*file_uniqueid->server) {
    marshall_HYPER (sbp, file_uniqueid->fileid);
    marshall_STRING (sbp, "");
  } else {
    marshall_HYPER (sbp, zero);
    marshall_STRING (sbp, actual_path);
  }

  msglen = sbp - sendbuf;
  marshall_LONG (q, msglen); /* update length field */

  c = send2nsd (NULL, server, sendbuf, msglen, repbuf, sizeof(repbuf));
  if (c == 0) {
    rbp = repbuf;
    unmarshall_HYPER (rbp, statbuf->fileid);
    unmarshall_WORD (rbp, statbuf->filemode);
    unmarshall_LONG (rbp, statbuf->nlink);
    unmarshall_LONG (rbp, statbuf->uid);
    unmarshall_LONG (rbp, statbuf->gid);
    unmarshall_HYPER (rbp, statbuf->filesize);
    unmarshall_TIME_T (rbp, statbuf->atime);
    unmarshall_TIME_T (rbp, statbuf->mtime);
    unmarshall_TIME_T (rbp, statbuf->ctime);
    unmarshall_WORD (rbp, statbuf->fileclass);
    unmarshall_BYTE (rbp, statbuf->status);

    strcpy (file_uniqueid->server, server);
    file_uniqueid->fileid = statbuf->fileid;
  }
  if (c && serrno == SENAMETOOLONG) serrno = ENAMETOOLONG;
  return (c);
}

/* Cns_stat - get information about a file or a directory */

int
Cns_stat(const char *path, struct Cns_filestat *statbuf)
{
  struct Cns_fileid file_uniqueid;

  memset ((void *) &file_uniqueid, 0, sizeof(struct Cns_fileid));
  return (Cns_statx (path, &file_uniqueid, statbuf));
}

/* Cns_statcs - get information about a file or a directory as Cns_stat*/

int
Cns_statcs(const char *path, struct Cns_filestatcs *statbuf)
{
  struct Cns_fileid file_uniqueid;

  memset ((void *) &file_uniqueid, 0, sizeof(struct Cns_fileid));
  return (Cns_statcsx (path, &file_uniqueid, statbuf));
}

/*
 * This is the same function as Cns_statx, but has Cns_filestatcs structure as parameter.
 * Cns_filestatcs has additional fields:  csumtype, csumvalue
 */
int
Cns_statcsx(const char *path, struct Cns_fileid *file_uniqueid, struct Cns_filestatcs *statbuf)
{
  char *actual_path;
  int c;
  char func[16];
  gid_t gid;
  int msglen;
  char *q;
  char *rbp;
  char repbuf[93];
  char *sbp;
  char sendbuf[REQBUFSZ];
  char server[CA_MAXHOSTNAMELEN+1];
  struct Cns_api_thread_info *thip;
  uid_t uid;
  u_signed64 zero = 0;

  strncpy (func, "Cns_statcs", 16);
  if (Cns_apiinit (&thip))
    return (-1);
  Cns_getid(&uid, &gid);

  if (! path || ! statbuf || ! file_uniqueid) {
    serrno = EFAULT;
    return (-1);
  }

  if (strlen (path) > CA_MAXPATHLEN) {
    serrno = ENAMETOOLONG;
    return (-1);
  }

  if (file_uniqueid && *file_uniqueid->server)
    strcpy (server, file_uniqueid->server);
  else
    if (Cns_selectsrvr (path, thip->server, server, &actual_path))
      return (-1);

  /* Build request header */

  sbp = sendbuf;
  marshall_LONG (sbp, CNS_MAGIC);
  marshall_LONG (sbp, CNS_STATCS);
  q = sbp;        /* save pointer. The next field will be updated */
  msglen = 3 * LONGSIZE;
  marshall_LONG (sbp, msglen);

  /* Build request body */

  marshall_LONG (sbp, uid);
  marshall_LONG (sbp, gid);
  marshall_HYPER (sbp, thip->cwd);
  if (*file_uniqueid->server) {
    marshall_HYPER (sbp, file_uniqueid->fileid);
    marshall_STRING (sbp, "");
  } else {
    marshall_HYPER (sbp, zero);
    marshall_STRING (sbp, actual_path);
  }

  msglen = sbp - sendbuf;
  marshall_LONG (q, msglen); /* update length field */

  c = send2nsd (NULL, server, sendbuf, msglen, repbuf, sizeof(repbuf));
  if (c == 0) {
    rbp = repbuf;
    unmarshall_HYPER (rbp, statbuf->fileid);
    unmarshall_WORD (rbp, statbuf->filemode);
    unmarshall_LONG (rbp, statbuf->nlink);
    unmarshall_LONG (rbp, statbuf->uid);
    unmarshall_LONG (rbp, statbuf->gid);
    unmarshall_HYPER (rbp, statbuf->filesize);
    unmarshall_TIME_T (rbp, statbuf->atime);
    unmarshall_TIME_T (rbp, statbuf->mtime);
    unmarshall_TIME_T (rbp, statbuf->ctime);
    unmarshall_WORD (rbp, statbuf->fileclass);
    unmarshall_BYTE (rbp, statbuf->status);
    unmarshall_STRING (rbp, statbuf->csumtype);
    unmarshall_STRING (rbp, statbuf->csumvalue);

    strcpy (file_uniqueid->server, server);
    file_uniqueid->fileid = statbuf->fileid;
  }
  if (c && serrno == SENAMETOOLONG) serrno = ENAMETOOLONG;
  return (c);
}

int
Cns_statg(const char *path, const char *guid, struct Cns_filestatg *statbuf)
{
  char *actual_path;
  int c;
  char func[16];
  gid_t gid;
  int msglen;
  char *q;
  char *rbp;
  char repbuf[130];
  char *sbp;
  char sendbuf[REQBUFSZ];
  char server[CA_MAXHOSTNAMELEN+1];
  struct Cns_api_thread_info *thip;
  uid_t uid;

  strncpy (func, "Cns_statg", 16);
  if (Cns_apiinit (&thip))
    return (-1);
  Cns_getid(&uid, &gid);

  if ((! path && ! guid) || ! statbuf) {
    serrno = EFAULT;
    return (-1);
  }

  if (path && strlen (path) > CA_MAXPATHLEN) {
    serrno = ENAMETOOLONG;
    return (-1);
  }
  if (guid && strlen (guid) > CA_MAXGUIDLEN) {
    serrno = EINVAL;
    return (-1);
  }

  if (path && Cns_selectsrvr (path, thip->server, server, &actual_path))
    return (-1);

  /* Build request header */

  sbp = sendbuf;
  marshall_LONG (sbp, CNS_MAGIC);
  marshall_LONG (sbp, CNS_STATG);
  q = sbp;        /* save pointer. The next field will be updated */
  msglen = 3 * LONGSIZE;
  marshall_LONG (sbp, msglen);

  /* Build request body */

  marshall_LONG (sbp, uid);
  marshall_LONG (sbp, gid);
  marshall_HYPER (sbp, thip->cwd);
  if (path) {
    marshall_STRING (sbp, actual_path);
  } else {
    marshall_STRING (sbp, "");
  }
  if (guid) {
    marshall_STRING (sbp, guid);
  } else {
    marshall_STRING (sbp, "");
  }

  msglen = sbp - sendbuf;
  marshall_LONG (q, msglen); /* update length field */

  c = send2nsd (NULL, path ? server : NULL, sendbuf, msglen, repbuf, sizeof(repbuf));
  if (c == 0) {
    rbp = repbuf;
    unmarshall_HYPER (rbp, statbuf->fileid);
    unmarshall_STRING (rbp, statbuf->guid);
    unmarshall_WORD (rbp, statbuf->filemode);
    unmarshall_LONG (rbp, statbuf->nlink);
    unmarshall_LONG (rbp, statbuf->uid);
    unmarshall_LONG (rbp, statbuf->gid);
    unmarshall_HYPER (rbp, statbuf->filesize);
    unmarshall_TIME_T (rbp, statbuf->atime);
    unmarshall_TIME_T (rbp, statbuf->mtime);
    unmarshall_TIME_T (rbp, statbuf->ctime);
    unmarshall_WORD (rbp, statbuf->fileclass);
    unmarshall_BYTE (rbp, statbuf->status);
    unmarshall_STRING (rbp, statbuf->csumtype);
    unmarshall_STRING (rbp, statbuf->csumvalue);
  }
  if (c && serrno == SENAMETOOLONG) serrno = ENAMETOOLONG;
  return (c);
}
