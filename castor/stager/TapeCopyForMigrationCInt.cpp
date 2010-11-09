/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/TapeCopyForMigrationCInt.cpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "castor/IObject.hpp"
#include "castor/stager/CastorFile.hpp"
#include "castor/stager/DiskCopy.hpp"
#include "castor/stager/Segment.hpp"
#include "castor/stager/Stream.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapeCopyForMigration.hpp"
#include "castor/stager/TapeCopyStatusCodes.hpp"
#include "osdep.h"
#include <stdlib.h>
#include <string.h>
#include <vector>

extern "C" {

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_create
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_create(castor::stager::TapeCopyForMigration** obj) {
    *obj = new castor::stager::TapeCopyForMigration();
    return 0;
  }
  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_delete
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_delete(castor::stager::TapeCopyForMigration* obj) {
    delete obj;
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_getTapeCopy
  //----------------------------------------------------------------------------
  castor::stager::TapeCopy* Cstager_TapeCopyForMigration_getTapeCopy(castor::stager::TapeCopyForMigration* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_fromTapeCopy
  //----------------------------------------------------------------------------
  castor::stager::TapeCopyForMigration* Cstager_TapeCopyForMigration_fromTapeCopy(castor::stager::TapeCopy* obj) {
    return dynamic_cast<castor::stager::TapeCopyForMigration*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_getIObject
  //----------------------------------------------------------------------------
  castor::IObject* Cstager_TapeCopyForMigration_getIObject(castor::stager::TapeCopyForMigration* obj) {
    return obj;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_fromIObject
  //----------------------------------------------------------------------------
  castor::stager::TapeCopyForMigration* Cstager_TapeCopyForMigration_fromIObject(castor::IObject* obj) {
    return dynamic_cast<castor::stager::TapeCopyForMigration*>(obj);
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_print
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_print(castor::stager::TapeCopyForMigration* instance) {
    instance->print();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_TYPE
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_TYPE(int* ret) {
    *ret = castor::stager::TapeCopyForMigration::TYPE();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_copyNb
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_copyNb(castor::stager::TapeCopyForMigration* instance, unsigned int* var) {
    *var = instance->copyNb();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setCopyNb
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setCopyNb(castor::stager::TapeCopyForMigration* instance, unsigned int new_var) {
    instance->setCopyNb(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_errorCode
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_errorCode(castor::stager::TapeCopyForMigration* instance, int* var) {
    *var = instance->errorCode();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setErrorCode
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setErrorCode(castor::stager::TapeCopyForMigration* instance, int new_var) {
    instance->setErrorCode(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_nbRetry
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_nbRetry(castor::stager::TapeCopyForMigration* instance, int* var) {
    *var = instance->nbRetry();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setNbRetry
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setNbRetry(castor::stager::TapeCopyForMigration* instance, int new_var) {
    instance->setNbRetry(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_missingCopies
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_missingCopies(castor::stager::TapeCopyForMigration* instance, int* var) {
    *var = instance->missingCopies();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setMissingCopies
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setMissingCopies(castor::stager::TapeCopyForMigration* instance, int new_var) {
    instance->setMissingCopies(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_fseq
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_fseq(castor::stager::TapeCopyForMigration* instance, int* var) {
    *var = instance->fseq();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setFseq
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setFseq(castor::stager::TapeCopyForMigration* instance, int new_var) {
    instance->setFseq(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_tapeGatewayRequestId
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_tapeGatewayRequestId(castor::stager::TapeCopyForMigration* instance, int* var) {
    *var = instance->tapeGatewayRequestId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setTapeGatewayRequestId
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setTapeGatewayRequestId(castor::stager::TapeCopyForMigration* instance, int new_var) {
    instance->setTapeGatewayRequestId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_vid
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_vid(castor::stager::TapeCopyForMigration* instance, const char** var) {
    *var = instance->vid().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setVid
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setVid(castor::stager::TapeCopyForMigration* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setVid(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_fileTransactionId
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_fileTransactionId(castor::stager::TapeCopyForMigration* instance, int* var) {
    *var = instance->fileTransactionId();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setFileTransactionId
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setFileTransactionId(castor::stager::TapeCopyForMigration* instance, int new_var) {
    instance->setFileTransactionId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_id
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_id(castor::stager::TapeCopyForMigration* instance, u_signed64* var) {
    *var = instance->id();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setId
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setId(castor::stager::TapeCopyForMigration* instance, u_signed64 new_var) {
    instance->setId(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_addStream
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_addStream(castor::stager::TapeCopyForMigration* instance, castor::stager::Stream* obj) {
    instance->addStream(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_removeStream
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_removeStream(castor::stager::TapeCopyForMigration* instance, castor::stager::Stream* obj) {
    instance->removeStream(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_stream
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_stream(castor::stager::TapeCopyForMigration* instance, castor::stager::Stream*** var, int* len) {
    std::vector<castor::stager::Stream*>& result = instance->stream();
    *len = result.size();
    *var = (castor::stager::Stream**) malloc((*len) * sizeof(castor::stager::Stream*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_diskCopy
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_diskCopy(castor::stager::TapeCopyForMigration* instance, castor::stager::DiskCopy** var) {
    *var = instance->diskCopy();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setDiskCopy
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setDiskCopy(castor::stager::TapeCopyForMigration* instance, castor::stager::DiskCopy* new_var) {
    instance->setDiskCopy(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_addSegments
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_addSegments(castor::stager::TapeCopyForMigration* instance, castor::stager::Segment* obj) {
    instance->addSegments(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_removeSegments
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_removeSegments(castor::stager::TapeCopyForMigration* instance, castor::stager::Segment* obj) {
    instance->removeSegments(obj);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_segments
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_segments(castor::stager::TapeCopyForMigration* instance, castor::stager::Segment*** var, int* len) {
    std::vector<castor::stager::Segment*>& result = instance->segments();
    *len = result.size();
    *var = (castor::stager::Segment**) malloc((*len) * sizeof(castor::stager::Segment*));
    for (int i = 0; i < *len; i++) {
      (*var)[i] = result[i];
    }
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_castorFile
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_castorFile(castor::stager::TapeCopyForMigration* instance, castor::stager::CastorFile** var) {
    *var = instance->castorFile();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setCastorFile
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setCastorFile(castor::stager::TapeCopyForMigration* instance, castor::stager::CastorFile* new_var) {
    instance->setCastorFile(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_status
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_status(castor::stager::TapeCopyForMigration* instance, castor::stager::TapeCopyStatusCodes* var) {
    *var = instance->status();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setStatus
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setStatus(castor::stager::TapeCopyForMigration* instance, castor::stager::TapeCopyStatusCodes new_var) {
    instance->setStatus(new_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_type
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_type(castor::stager::TapeCopyForMigration* instance,
                                        int* ret) {
    *ret = instance->type();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_clone
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_clone(castor::stager::TapeCopyForMigration* instance,
                                         castor::IObject** ret) {
    *ret = instance->clone();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_diskServer
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_diskServer(castor::stager::TapeCopyForMigration* instance, const char** var) {
    *var = instance->diskServer().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setDiskServer
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setDiskServer(castor::stager::TapeCopyForMigration* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setDiskServer(snew_var);
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_mountPoint
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_mountPoint(castor::stager::TapeCopyForMigration* instance, const char** var) {
    *var = instance->mountPoint().c_str();
    return 0;
  }

  //----------------------------------------------------------------------------
  // Cstager_TapeCopyForMigration_setMountPoint
  //----------------------------------------------------------------------------
  int Cstager_TapeCopyForMigration_setMountPoint(castor::stager::TapeCopyForMigration* instance, const char* new_var) {
    std::string snew_var(new_var, strlen(new_var));
    instance->setMountPoint(snew_var);
    return 0;
  }

} // End of extern "C"
