/*
 * $Id: stat.c,v 1.7 2002/09/20 06:59:36 baud Exp $
 */

/*
 * Copyright (C) 1990-2002 by CERN/IT/PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: stat.c,v $ $Revision: 1.7 $ $Date: 2002/09/20 06:59:36 $ CERN/IT/PDP/DM Frederic Hemmer";
#endif /* not lint */

/* stat.c       Remote File I/O - get file status                       */

#define RFIO_KERNEL 1
#include <rfio.h>


EXTERN_C int DLL_DECL rfio_smstat _PROTO((int, char *, struct stat *, int));

int  DLL_DECL rfio_stat(filepath, statbuf)       /* Remote file stat    */
char    *filepath;              /* remote file path                     */
struct stat *statbuf;           /* status buffer (subset of local used) */
{
	register int    s;              /* socket descriptor            */
	int       status ;
	char    *host, *filename;
	int 	rt ;

	INIT_TRACE("RFIO_TRACE");
	TRACE(1, "rfio", "rfio_stat(%s, %x)", filepath, statbuf);

	if (!rfio_parseln(filepath,&host,&filename,RDLINKS)) {
                /* if not a remote file, must be local or HSM  */
                if ( host != NULL ) {
                    /*
                     * HSM file
                     */
                    TRACE(1,"rfio","rfio_stat: %s is an HSM path",
                          filename);
                    END_TRACE();
                    rfio_errno = 0;
                    return(rfio_HsmIf_stat(filename,statbuf));
                }
#if LOCAL_IO
		TRACE(1, "rfio", "rfio_stat: using local stat(%s, %x)",
			filename, statbuf);

		END_TRACE();
		rfio_errno = 0;
		status = stat(filename,statbuf);
		if ( status < 0 ) serrno = 0;
		return(status);
#else
		END_TRACE();
		rfio_errno = SENOTRFILE;
		return(-1);
#endif /* LOCAL_IO */
	}

	s = rfio_connect(host,&rt);
	if (s < 0)      {
		return(-1);
	}
	END_TRACE();
	status = rfio_smstat(s,filename,statbuf,RQST_STAT_SEC) ;
	if ( status == -1 && serrno == SEPROTONOTSUP ) {
	  s = rfio_connect(host,&rt);
	  if (s < 0)      {
	    return(-1);
	  }
	  status = rfio_smstat(s,filename,statbuf,RQST_STAT) ;
	}
	(void) close(s);
	return (status);
}
