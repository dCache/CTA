#include <unistd.h>
#include <stdio.h>
#include <signal.h> 
#include <errno.h>
#include <osdep.h>

int Cinitdaemon(name,wait4child)
char *name;
void (*wait4child) _PROTO((int));
{
        int c, _jid;
        int maxfds;
        struct sigaction sa;

#if defined(SOLARIS) || (defined(__osf__) && defined(__alpha)) || defined(linux) || defined(sgi)
        maxfds = getdtablesize();
#else
        maxfds = _NFILE;
#endif
        /* Background */
        if ((c = fork()) < 0) {
                fprintf (stderr, "%s: cannot fork: %s\n",name,sstrerror(errno));
                exit(1);
        } else
                if (c > 0) exit (0);
#if (defined(__osf__) && defined(__alpha)) || defined(linux)
        c = setsid();
#else
#if hpux
        c = setpgrp3();
#else
        c = setpgrp();
#endif
#endif
        for (c = 0; c < maxfds; c++)
                close (c);
        _jid = getpid();
#if ! defined(_WIN32)
        if ( wait4child != NULL ) {
            sa.sa_handler = wait4child;
        } else {
            sa.sa_handler = SIG_IGN;
        }
        sa.sa_flags = SA_RESTART;
        sigaction (SIGCHLD, &sa, NULL);
#endif
        return(_jid);
}

