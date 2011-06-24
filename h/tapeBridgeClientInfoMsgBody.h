/******************************************************************************
 *                h/tapeBridgeClientInfoMsgBody.h
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#ifndef H_TAPEBRIDGECLIENTINFOMSGBODY_H
#define H_TAPEBRIDGECLIENTINFOMSGBODY_H 1

#include "h/Castor_limits.h"
#include "h/osdep.h"

#include <stdint.h>

/**
 * The body of a TAPEBRIDGE_CLIENTINFO message.
 */
typedef struct {
  uint32_t volReqId;
  uint32_t bridgeCallbackPort;
  uint32_t bridgeClientCallbackPort;
  uint32_t clientUID;
  uint32_t clientGID;
  uint32_t useBufferedTapeMarksOverMultipleFiles;
  char    bridgeHost[CA_MAXHOSTNAMELEN+1];
  char    bridgeClientHost[CA_MAXHOSTNAMELEN+1];
  char    dgn[CA_MAXDGNLEN+1];
  char    drive[CA_MAXUNMLEN+1];
  char    clientName[CA_MAXUSRNAMELEN+1];
} tapeBridgeClientInfoMsgBody_t;

#define TAPEBRIDGECLIENTINFOMSGBODY_MAXSIZE (                           \
    LONGSIZE              + /* volReqID                              */ \
    LONGSIZE              + /* bridgeCallbackPort                    */ \
    LONGSIZE              + /* bridgeClientCallbackPort              */ \
    LONGSIZE              + /* clientUID                             */ \
    LONGSIZE              + /* clientGID                             */ \
    LONGSIZE              + /* useBufferedTapeMarksOverMultipleFiles */ \
    CA_MAXHOSTNAMELEN + 1 + /* bridgeHost                            */ \
    CA_MAXHOSTNAMELEN + 1 + /* bridgeClientHost                      */ \
    CA_MAXDGNLEN      + 1 + /* dgn                                   */ \
    CA_MAXUNMLEN      + 1 + /* drive                                 */ \
    CA_MAXUSRNAMELEN  + 1   /* clientName                            */)

#define TAPEBRIDGECLIENTINFOMSGBODY_MINSIZE (                           \
    LONGSIZE              + /* volReqID                              */ \
    LONGSIZE              + /* bridgeCallbackPort                    */ \
    LONGSIZE              + /* bridgeClientCallbackPort              */ \
    LONGSIZE              + /* clientUID                             */ \
    LONGSIZE              + /* clientGID                             */ \
    LONGSIZE              + /* useBufferedTapeMarksOverMultipleFiles */ \
    1                     + /* bridgeHost                            */ \
    1                     + /* bridgeClientHost                      */ \
    1                     + /* dgn                                   */ \
    1                     + /* drive                                 */ \
    1                       /* clientName                            */)

#endif /* H_TAPEBRIDGECLIENTINFOMSGBODY_H */
