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

#include "castor/messages/Constants.hpp"

//------------------------------------------------------------------------------
// msgTypeToString
//------------------------------------------------------------------------------
const char *castor::messages::msgTypeToString(const MsgType msgType) throw() {
  switch(msgType) {
  case MSG_TYPE_NONE:
    return "None";
  case MSG_TYPE_EXCEPTION:
    return "Exception";
  case MSG_TYPE_FORKCLEANER:
    return "ForkCleaner";
  case MSG_TYPE_FORKDATATRANSFER:
    return "ForkDataTransfer";
  case MSG_TYPE_FORKLABEL:
    return "ForkLabel";
  case MSG_TYPE_FORKSUCCEEDED:
    return "ForkSucceeded";
  case MSG_TYPE_HEARTBEAT:
    return "Heartbeat";
  case MSG_TYPE_MIGRATIONJOBFROMTAPEGATEWAY:
    return "MigrationJobFromTapeGateway";
  case MSG_TYPE_MIGRATIONJOBFROMWRITETP:
    return "MigrationJobFromWriteTp";
  case MSG_TYPE_NBFILESONTAPE:
    return "NbFilesOnTape";
  case MSG_TYPE_PROCESSCRASHED:
    return "ProcessCrashed";
  case MSG_TYPE_PROCESSEXITED:
    return "ProcessExited";
  case MSG_TYPE_RECALLJOBFROMREADTP:
    return "RecallJobFromReadTp";
  case MSG_TYPE_RECALLJOBFROMTAPEGATEWAY:
    return "RecallJobFromTapeGAteway";
  case MSG_TYPE_RETURNVALUE:
    return "ReturnValue";
  case MSG_TYPE_STOPPROCESSFORKER:
    return "StopProcessForker";
  case MSG_TYPE_TAPEMOUNTEDFORMIGRATION:
    return "TapeMountedForMigration";
  case MSG_TYPE_TAPEMOUNTEDFORRECALL:
    return "TapeMountedForRecall";
  case MSG_TYPE_LABELERROR:
    return "LabelError";
  case MSG_TYPE_ACSMOUNTTAPEREADONLY:
    return "AcsMountTapeReadOnly";
  case MSG_TYPE_ACSMOUNTTAPEREADWRITE:
    return "AcsMountTapeReadWrite";        
  case MSG_TYPE_ACSDISMOUNTTAPE:
    return "AcsDismountTape";
  case MSG_TYPE_ACSFORCEDISMOUNTTAPE:
    return "AcsForceDismountTape";
  case MSG_TYPE_ADDLOGPARAMS:
    return "AddLogParams";
  case MSG_TYPE_DELETELOGPARAMS:
    return "DeleteLogParams";
  default:
    return "Unknown";
  }
} // msgTypeToString()
