/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2002-2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*      rmc_read_elem_status - read element status in a remote SCSI robot */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include "marshall.h"
#include "rmc_api.h"
#include "rmc_constants.h"
#include "serrno.h"
int rmc_read_elem_status(
	const char *const server,
	const int type,
	const int startaddr,
	const int nbelem,
	struct smc_element_info *const element_info)
{
	int c;
	struct smc_element_info *elemp;
	gid_t gid;
	int i;
	int msglen;
	char *q;
	char *rbp;
	char repbuf[RMC_REPBUFSZ];
	char *sbp;
	char sendbuf[RMC_REQBUFSZ];
	uid_t uid;

	uid = getuid();
	gid = getgid();

	/* Build request header */

	sbp = sendbuf;
	marshall_LONG (sbp, RMC_MAGIC);
	marshall_LONG (sbp, RMC_READELEM);
	q = sbp;	/* save pointer. The next field will be updated */
	msglen = 3 * LONGSIZE;
	marshall_LONG (sbp, msglen);

	/* Build request body */

	marshall_LONG (sbp, uid);
	marshall_LONG (sbp, gid);
	marshall_STRING (sbp, ""); /* loader field is no longer used */
	marshall_LONG (sbp, type);
	marshall_LONG (sbp, startaddr);
	marshall_LONG (sbp, nbelem);

	msglen = sbp - sendbuf;
	marshall_LONG (q, msglen);	/* update length field */

        while ((c = send2rmc (server, sendbuf, msglen, repbuf, sizeof(repbuf))) &&
            serrno == ERMCNACT)
                sleep (RMC_RETRYI);
	if (c == 0) {
		rbp = repbuf;
		unmarshall_LONG (rbp, c);
		for (i = 0, elemp = element_info; i < c; i++, elemp++) {
			unmarshall_WORD (rbp, elemp->element_address);
			unmarshall_BYTE (rbp, elemp->element_type);
			unmarshall_BYTE (rbp, elemp->state);
			unmarshall_BYTE (rbp, elemp->asc);
			unmarshall_BYTE (rbp, elemp->ascq);
			unmarshall_BYTE (rbp, elemp->flags);
			unmarshall_WORD (rbp, elemp->source_address);
			unmarshall_STRING (rbp, elemp->name);
		}
	}
	return (c);
}
