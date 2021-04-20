/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace cta {
namespace mediachanger     {
  	
const size_t HOSTNAMEBUFLEN    = 256;
const int    LISTENBACKLOG     = 2;
const size_t SERVICENAMEBUFLEN = 256;

enum ProtocolType {
  PROTOCOL_TYPE_NONE,
  PROTOCOL_TYPE_TAPE
};

enum ProtocolVersion {
  PROTOCOL_VERSION_NONE,
  PROTOCOL_VERSION_1
};

                        /* Request types */

#define RMC_GETGEOM        1 /* Get robot geometry */
#define RMC_FINDCART       2 /* Find cartridge(s) */
#define RMC_READELEM       3 /* Read element status */
#define RMC_MOUNT          4 /* Mount request */
#define RMC_UNMOUNT        5 /* Unmount request */
#define RMC_EXPORT         6 /* Export tape request */
#define RMC_IMPORT         7 /* Import tape request */
#define RMC_GENERICMOUNT   8 /* Generic (SCSI or ACS) mount request */
#define RMC_GENERICUNMOUNT 9 /* Generic (SCSI or ACS) mount request */

                        /* SCSI media changer server reply types */

#define MSG_ERR         1
#define MSG_DATA        2
#define RMC_RC          3

/*
 *------------------------------------------------------------------------
 * RMC (Remote SCSI media changer server) errors
 *------------------------------------------------------------------------
 */
#define ERMBASEOFF      2200            /* RMC error base offset        */
#define        ERMCNACT        ERMBASEOFF+1    /* Remote SCSI media changer server not active or service being drained */
#define        ERMCRBTERR      (ERMBASEOFF+2)  /* Remote SCSI media changer error */
#define        ERMCUNREC       ERMCRBTERR+1    /* Remote SCSI media changer unrec. error */
#define        ERMCSLOWR       ERMCRBTERR+2    /* Remote SCSI media changer error (slow retry) */
#define        ERMCFASTR       ERMCRBTERR+3    /* Remote SCSI media changer error (fast retry) */
#define        ERMCDFORCE      ERMCRBTERR+4    /* Remote SCSI media changer error (demount force) */
#define        ERMCDDOWN       ERMCRBTERR+5    /* Remote SCSI media changer error (drive down) */
#define        ERMCOMSGN       ERMCRBTERR+6    /* Remote SCSI media changer error (ops message) */
#define        ERMCOMSGS       ERMCRBTERR+7    /* Remote SCSI media changer error (ops message + retry) */
#define        ERMCOMSGR       ERMCRBTERR+8    /* Remote SCSI media changer error (ops message + wait) */
#define        ERMCUNLOAD      ERMCRBTERR+9    /* Remote SCSI media changer error (unload + demount) */
#define ERMMAXERR       ERMBASEOFF+11

/**
 * The default TCP/IP port on which the CASTOR rmcd daemon listens for incoming
 * TCP/IP connections from the tape server.
 */
const unsigned short RMC_PORT = 5014;

/**
 * The network timeout of rmc communications should be several minutes due
 * to the time it takes to mount and unmount tapes.
 */
const int RMC_NET_TIMEOUT = 600; // Timeout in seconds

/**
 * The maximum number of attempts a retriable RMC request should be issued.
 */
const int RMC_MAX_RQST_ATTEMPTS = 10;

/**
 * The magic number to identify rmcd messages.
 */
const uint32_t RMC_MAGIC = 0x120D0301;

} // namespace mediachanger
} // namespace cta


