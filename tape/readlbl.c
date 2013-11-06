/*
 * $Id: readlbl.c,v 1.19 2009/03/13 14:20:24 wiebalck Exp $
 */

/*	readlbl - read one possible label record 
 *
 *	return	-1 and serrno set in case of error
 *		 0 if a 80 characters record was read
 *		 1 if the record length was different
 *		 2 for EOF
 *		 3 for blank tape
 */

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mtio.h>
#include <sys/utsname.h>
#include "Ctape.h"
#include "Ctape_api.h"
#include "serrno.h"

int skiptprf(int, const char *const, int, int);
char *getconfent();

int readlbl(const int tapefd,
            const char *const path,
            char *const lblbuf)
{
	/* int errcat; */
	char func[16];
	char *msgaddr;
	int n;
        struct mtget mt_info;
        static struct utsname un;
        int major = 0;
        int minor = 0;
        int patch = 0;
        int nr = 0;

	ENTRY (readlbl);
	if ((n = read (tapefd, lblbuf, LBLBUFSZ)) < 0) {
		if (errno != ENOMEM) {	/* not large block */
			if (errno == ENOSPC) {
				msgaddr = "";
				serrno = ETPARIT;
			} else if (errno == EIO) {
                                /*
                                  errcat = gettperror (tapefd, path, &msgaddr);
                                  if (errcat == ETBLANK) RETURN (3);
                                  serrno = (errcat > 0) ? errcat : EIO;
                                */

                                /*
                                  The following logic is used to detect a blank tape:
                                  
                                  . I/O error on read
                                  . rewind (must succeed)
                                  . try to skip 1 record forward (must fail) 
                                  . check if BOT and EOD are set

                                  Instead of skipping one record forward, it is possible to skip to the
                                  end of data using skip2eod() and do the same BOT/EOD check. This would 
                                  avoid the error generated by the failing skip. However, since this
                                  check is done on every I/O error when reading a label, rewinding and 
                                  skipping to the EOD may take of the order of minutes before the code 
                                  recognizes that the tape is not empty. Skipping one block was chosen
                                  as it should save time. 

                                  Using the RELAX_BLANKTAPE_CHECK the checking can be relaxed (the check
                                  for BOT and EOD is not done). Use with care!
                                  
                                  Using MTIOCPOS to determine the position before taking any action does
                                  not seem to work on an empty tape. 

                                  Once the sense bytes are available, checking for BLANK_CHECK will
                                  immediately reveal blank tapes. 
                                */

                                if ((0 == rwndtape(tapefd, path)) && (0 != skiptprf(tapefd, path, 1, 1))) {
                                        char *p = NULL;
                                        p = getconfent( "TAPE", "RELAX_BLANKTAPE_CHECK", 0 );
                                        if ((NULL != p) && (0 == strcasecmp(p, "YES"))) {
                                                usrmsg(func, "IO error on read, rewind OK, skip fsr1 failed ==> tape is blank\n");
                                                RETURN(3);
                                        } else {
                                                if (ioctl (tapefd, MTIOCGET, &mt_info) >= 0) {
                                                        if (GMT_EOD(mt_info.mt_gstat) && GMT_BOT(mt_info.mt_gstat)) {
                                                                usrmsg(func, "blank tape detected (BOT and EOD are set)\n");
                                                                RETURN (3);
                                                        } else {
                                                                usrmsg(func, "tape not blank\n");
                                                        }
                                                } else {
                                                        usrmsg(func, "MTIOCGET failed\n");
                                                }                                                
                                        }
                                } else {
                                        usrmsg(func, "rwndtape failed or skiptprf succeeded\n");
                                }
                                msgaddr = strerror(errno);
				serrno = errno;
			} else {
				msgaddr = strerror(errno);
				serrno = errno;
			}
			usrmsg (func, TP042, path, "read", msgaddr);
			RETURN (-1);
		}
	}
	if (n == 0) {
                /* try first to determine blank tape via st macro for 2.6 kernels */
                uname(&un);
                nr = sscanf(un.release, "%d.%d.%d", &major, &minor, &patch);
                if (nr == 3 && major == 2) {
                        if (minor >= 6) {
                                if (ioctl (tapefd, MTIOCGET, &mt_info) >= 0) {
                                        if (GMT_EOD(mt_info.mt_gstat)) {
                                                /* blank tape - end of data */
                                                usrmsg (func, "read 0 bytes, EOD detected\n");
                                                RETURN(3);	
                                        } else {
                                                usrmsg (func, "read 0 bytes, but not at EOD\n");
                                        }
                                } else {
                                        usrmsg (func, "MTIOCGET failed\n");   
                                }
                        }
                } 

		if (gettperror (tapefd, path, &msgaddr) == ETBLANK) {
			RETURN (3);	/* blank tape */
                }
		RETURN (2);	/* tapemark */
	}

	lblbuf[80] = '\0';
	if (n != 80) {
                RETURN (1); 
        }
	RETURN (0);
}
