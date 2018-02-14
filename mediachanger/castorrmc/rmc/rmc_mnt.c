/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/*      rmc_mnt - mount a cartridge into a drive that maybe in either */
/*                a SCSI compatible or an ACS compatible tape library */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "marshall.h"
#include "rmc_api.h"
#include "rmc_constants.h"
#include "serrno.h"

#include <errno.h>
#include <string.h>

int rmc_mnt(
	const char *const server,
	const char *const vid,
	const char *const loader)
{
	const gid_t gid = getgid();
	const uid_t uid = getuid();

	/* The total length of the fixed size members of the request message */
	/* is Magic (4 bytes) + request ID (4 bytes) + length (4 bytes) +    */
	/* uid (4 bytes) + gid (4 bytes) = 20 bytes                          */
	const int msglen = 20 + strlen(vid) + 1 + strlen(loader) + 1;

	char repbuf[1];
	char *sbp = NULL;
	char sendbuf[RMC_REQBUFSZ];

	/* Consider the function arguments invalid if the total size of the */
	/* request message would be greater than RMC_REQBUFSZ               */
	if(msglen > RMC_REQBUFSZ) {
		errno = ERMCUNREC;
		serrno = errno;
		return -1;
	}

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, RMC_MAGIC);
	marshall_LONG (sbp, RMC_MOUNT);
	marshall_LONG (sbp, msglen);

	/* Build request body */

	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	marshall_STRING (sbp, vid);
	marshall_STRING (sbp, loader);

	/* Being paranoid; checking the calculated message length against */
	/* the number of bytes marshalled                                 */
	if(sbp - sendbuf != msglen) {
		errno = SEINTERNAL;
		serrno = errno;
		return -1;
	}

        return send2rmc (server, sendbuf, msglen, repbuf, sizeof(repbuf));
}