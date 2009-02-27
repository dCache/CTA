/*
 * Copyright (C) 2007-2008 by CERN/IT/GD/ITR
 * All rights reserved
 */

/* nsping - check name server alive and return version number */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <winsock2.h>
#endif
#include "Cns.h"
#include "Cns_api.h"
#include "Cgetopt.h"
#include "serrno.h"

void usage(int status, char *name) {
  if (status != 0) {
    fprintf (stderr, "Try `%s --help` for more information.\n", name);
  } else {
    printf ("Usage: %s [OPTION]\n", name);
    printf ("Check if the name server is alive.\n\n");
    printf ("  -h, --host=HOSTNAME  the name server to ping\n");
    printf ("      --help           display this help and exit\n\n");
    printf ("Report bugs to <castor.support@cern.ch>.\n");
  }
#if defined(_WIN32)
  WSACleanup();
#endif
  exit (status);
}

int main(argc, argv)
     int argc;
     char **argv;
{
  int c;
  int errflg = 0;
  int hflg = 0;
  char info[256];
  static char retryenv[16];
  char *server = NULL;
  char *p = NULL;

#if defined(_WIN32)
  WSADATA wsadata;
#endif

  Coptions_t longopts[] = {
    { "host", REQUIRED_ARGUMENT, NULL, 'h' },
    { "help", NO_ARGUMENT,       &hflg, 1  },
    { NULL,   0,                 NULL,  0  }
  };

  Coptind = 1;
  Copterr = 1;
  while ((c = Cgetopt_long (argc, argv, "h:", longopts, NULL)) != EOF) {
    switch (c) {
    case 'h':
      server = Coptarg;
      if ((p = getenv (CNS_HOST_ENV)) ||
	  (p = getconfent (CNS_SCE, "HOST", 0))) {
	if (strcmp(p, server) != 0) {
	  fprintf (stderr,
		   "--host option is not permitted when CNS/HOST is defined\n");
	  errflg++;
	}
      }
      break;
    case '?':
    case ':':
      errflg++;
      break;
    default:
      break;
    }
  }
  if (hflg) {
    usage (0, argv[0]);
  }
  if (Coptind < argc) {
    errflg++;
  }
  if (errflg) {
    usage (USERR, argv[0]);
  }

#if defined(_WIN32)
  if (WSAStartup (MAKEWORD (2, 0), &wsadata)) {
    fprintf (stderr, NS052);
    exit (SYERR);
  }
#endif
  sprintf (retryenv, "%s=0", CNS_CONRETRY_ENV);
  putenv (retryenv);
  if (Cns_ping (server, info) < 0) {
    fprintf (stderr, "nsping: %s\n", sstrerror(serrno));
#if defined(_WIN32)
    WSACleanup();
#endif
    exit (USERR);
  }
  printf ("%s\n", info);
#if defined(_WIN32)
  WSACleanup();
#endif
  exit (0);
}
