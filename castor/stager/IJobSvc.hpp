/******************************************************************************
 *                castor/stager/IJobSvc.hpp
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
 * @(#)$RCSfile: IJobSvc.hpp,v $ $Revision: 1.5 $ $Release$ $Date: 2007/03/26 16:59:46 $ $Author: itglp $
 *
 * This class provides stager methods related to job handling
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef STAGER_IJOBSVC_HPP
#define STAGER_IJOBSVC_HPP 1

// Include Files
#include "castor/Constants.hpp"
#include "castor/stager/ICommonSvc.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/stager/DiskCopyStatusCodes.hpp"
#include <vector>
#include <string>
#include <list>

namespace castor {

  // Forward declaration
  class IObject;
  class IClient;
  class IAddress;

  namespace stager {

    // Forward declaration
    class Tape;
    class Stream;
    class Request;
    class Segment;
    class TapeCopy;
    class DiskCopy;
    class DiskPool;
    class SvcClass;
    class FileClass;
    class TapePool;
    class FileSystem;
    class DiskServer;
    class SubRequest;
    class CastorFile;
    class GCLocalFile;
    class TapeCopyForMigration;
    class DiskCopyForRecall;

    /**
     * This class provides stager methods related to job handling
     */
    class IJobSvc : public virtual ICommonSvc {

    public:

      /**
       * Handles the start of a Get or Update job.
       * Schedules the corresponding SubRequest on a given
       * FileSystem and returns the DiskCopy to use for data
       * access.
       * Note that deallocation of the DiskCopy is the
       * responsability of the caller.
       * Depending on the available DiskCopies for the file
       * the SubRequest deals with, we have different cases :
       *  - no DiskCopy at all and file is not of size 0 :
       * a DiskCopy is created with status DISKCOPY_WAITTAPERECALL.
       * Null pointer is returned
       *  - no DiskCopy at all and file is of size 0 :
       * a DiskCopy is created with status DISKCOPY_WAIDISK2DISKCOPY.
       * This diskCopy is returned and the emptyFile content is
       * set to true.
       *  - one DiskCopy in DISKCOPY_WAITTAPERECALL, DISKCOPY_WAITFS
       * or DISKCOPY_WAITDISK2DISKCOPY status :
       * the SubRequest is linked to the one recalling and
       * put in SUBREQUEST_WAITSUBREQ status. Null pointer is
       * returned.
       *  - no valid (STAGE*, WAIT*) DiskCopy on the selected
       * FileSystem but some in status DISKCOPY_STAGEOUT or
       * DISKCOPY_STAGED on other FileSystems : a new DiskCopy
       * is created with status DISKCOPY_WAITDISK2DISKCOPY.
       * It is returned and the sources parameter is filed
       * with the DiskCopies found on the non selected FileSystems.
       *  - one DiskCopy on the selected FileSystem in
       * DISKCOPY_STAGEOUT or DISKCOPY_STAGED status :
       * the SubRequest is ready, the DiskCopy is returned and
       * sources remains empty.
       * @param subreq  the SubRequest to consider
       * @param fileSystem the selected FileSystem
       * @param sources this is a list of DiskCopies that
       * can be used as source of a Disk to Disk copy. This
       * list is never empty when diskCopy has status
       * DISKCOPY_DISK2DISKCOPY and always empty otherwise.
       * Note that the DiskCopies returned in sources must be
       * deallocated by the caller.
       * @param emptyFile whether the resulting diskCopy
       * deals with the recall of an empty file
       * @return the DiskCopy to use for the data access or
       * a null pointer if the data access will have to wait
       * and there is nothing more to be done. Even in case
       * of a non null pointer, the data access will have to
       * wait for a disk to disk copy if the returned DiskCopy
       * is in DISKCOPY_WAITDISKTODISKCOPY status. This
       * disk to disk copy is the responsability of the caller.
       * @exception Exception in case of error
       */
      virtual castor::stager::DiskCopy* getUpdateStart
      (castor::stager::SubRequest* subreq,
       castor::stager::FileSystem* fileSystem,
       std::list<castor::stager::DiskCopyForRecall*>& sources,
       bool* emptyFile)
        throw (castor::exception::Exception) = 0;

      /**
       * Handles the start of a Put job.
       * Links the DiskCopy associated to the SubRequest to
       * the given FileSystem and updates the DiskCopy status
       * to DISKCOPY_STAGEOUT.
       * Note that deallocation of the DiskCopy is the
       * responsability of the caller.
       * @param subreq  the SubRequest to consider
       * @param fileSystem the selected FileSystem
       * @return the DiskCopy to use for the data access
       * @exception Exception in case of error
       */
      virtual castor::stager::DiskCopy* putStart
      (castor::stager::SubRequest* subreq,
       castor::stager::FileSystem* fileSystem)
        throw (castor::exception::Exception) = 0;

      /**
       * Updates database after successful completion of a
       * disk to disk copy. This includes setting the DiskCopy
       * status to DISKCOPY_STAGED and setting the SubRequest
       * status to SUBREQUEST_READY.
       * Changes are commited
       * @param diskcopyId the id of the new DiskCopy
       * @param status the status of the new DiskCopy
       * @exception Exception throws an Exception in case of error
       */
      virtual void disk2DiskCopyDone
      (u_signed64 diskCopyId,
       castor::stager::DiskCopyStatusCodes status)
        throw (castor::exception::Exception) = 0;

      /**
       * Prepares a file for migration, when needed.
       * This is called both when a stagePut is over and when a
       * putDone request is processed.
       * In the case of a stagePut that in part of a PrepareToPut,
       * it actually does not prepare the file for migration
       * but only updates its size in DB and name server.
       * Otherwise (stagePut with no prepare and putDone),
       * it also updates the filesystem free space and creates
       * the needed TapeCopies according to the FileClass of the
       * castorFile.
       * @param subreq The SubRequest handling the file to prepare
       * @param fileSize The actual size of the castor file
       * @param timeStamp To know if the fileSize is still valid 
       * @exception Exception throws an Exception in case of error
       */
      virtual void prepareForMigration
      (castor::stager::SubRequest* subreq,
       u_signed64 fileSize, u_signed64 timeStamp)
        throw (castor::exception::Exception) = 0;

      /**
       * Informs the stager the a Get or Update SubRequest
       * (without write) was finished successfully.
       * The SubRequest and potentially the corresponding
       * Request will thus be removed from the DataBase
       * @param subReqId the id of the finished SubRequest
       */
      virtual void getUpdateDone(u_signed64 subReqId)
        throw (castor::exception::Exception) = 0;

      /**
       * Informs the stager the a Get or Update SubRequest
       * (without write) failed.
       * The SubRequest's status will thus be set to FAILED
       * @param subReqId the id of the failing SubRequest
       */
      virtual void getUpdateFailed(u_signed64 subReqId)
        throw (castor::exception::Exception) = 0;

      /**
       * Informs the stager the a Put or a PutDone SubRequest failed.
       * The SubRequest's status will thus be set to FAILED
       * @param subReqId the id of the failing SubRequest
       */
      virtual void putFailed(u_signed64 subReqId)
        throw (castor::exception::Exception) = 0;

      /**
       * Selects the next request the Job service should deal with.
       * Selects a Request in START status and move its status
       * PROCESSED to avoid double processing.
       * @return the Request to process
       * @exception Exception in case of error
       */
      virtual castor::stager::Request* requestToDo()
        throw (castor::exception::Exception) = 0;
    
    }; // end of class IJobSvc

  } // end of namespace stager

} // end of namespace castor

#endif // STAGER_IJOBSVC_HPP
