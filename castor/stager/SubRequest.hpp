/******************************************************************************
 *                      castor/stager/SubRequest.hpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_STAGER_SUBREQUEST_HPP
#define CASTOR_STAGER_SUBREQUEST_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/stager/SubRequestGetNextStatusCodes.hpp"
#include "castor/stager/SubRequestStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class FileRequest;
    class CastorFile;

    /**
     * class SubRequest
     * A subpart of a request dealing with a single castor file
     */
    class SubRequest : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      SubRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~SubRequest() throw();

      /**
       * Outputs this object in a human readable format
       * @param stream The stream where to print this object
       * @param indent The indentation to use
       * @param alreadyPrinted The set of objects already printed.
       * This is to avoid looping when printing circular dependencies
       */
      virtual void print(std::ostream& stream,
                         std::string indent,
                         castor::ObjectSet& alreadyPrinted) const;

      /**
       * Outputs this object in a human readable format
       */
      virtual void print() const;

      /**
       * Gets the type of this kind of objects
       */
      static int TYPE();

      /********************************************/
      /* Implementation of IObject abstract class */
      /********************************************/
      /**
       * Gets the type of the object
       */
      virtual int type() const;

      /**
       * virtual method to clone any object
       */
      virtual castor::IObject* clone();

      /*********************************/
      /* End of IObject abstract class */
      /*********************************/
      /**
       * Get the value of m_retryCounter
       * @return the value of m_retryCounter
       */
      unsigned int retryCounter() const {
        return m_retryCounter;
      }

      /**
       * Set the value of m_retryCounter
       * @param new_var the new value of m_retryCounter
       */
      void setRetryCounter(unsigned int new_var) {
        m_retryCounter = new_var;
      }

      /**
       * Get the value of m_fileName
       * Name of the file this SubRequest deals with.
       * When stored in the catalog, this is redundant with the link to the CastorFile
       * table. However, this is needed in the client
       * @return the value of m_fileName
       */
      std::string fileName() const {
        return m_fileName;
      }

      /**
       * Set the value of m_fileName
       * Name of the file this SubRequest deals with.
       * When stored in the catalog, this is redundant with the link to the CastorFile
       * table. However, this is needed in the client
       * @param new_var the new value of m_fileName
       */
      void setFileName(std::string new_var) {
        m_fileName = new_var;
      }

      /**
       * Get the value of m_protocol
       * The protocol that will be used to access the file this SubRequest deals with
       * @return the value of m_protocol
       */
      std::string protocol() const {
        return m_protocol;
      }

      /**
       * Set the value of m_protocol
       * The protocol that will be used to access the file this SubRequest deals with
       * @param new_var the new value of m_protocol
       */
      void setProtocol(std::string new_var) {
        m_protocol = new_var;
      }

      /**
       * Get the value of m_xsize
       * @return the value of m_xsize
       */
      u_signed64 xsize() const {
        return m_xsize;
      }

      /**
       * Set the value of m_xsize
       * @param new_var the new value of m_xsize
       */
      void setXsize(u_signed64 new_var) {
        m_xsize = new_var;
      }

      /**
       * Get the value of m_priority
       * The priority defines in which order the files will be processed by the user when
       * calling stage_get_next. The files of the SubRequest of same priority are given
       * in a random order and lower priority files come first.
       * @return the value of m_priority
       */
      unsigned int priority() const {
        return m_priority;
      }

      /**
       * Set the value of m_priority
       * The priority defines in which order the files will be processed by the user when
       * calling stage_get_next. The files of the SubRequest of same priority are given
       * in a random order and lower priority files come first.
       * @param new_var the new value of m_priority
       */
      void setPriority(unsigned int new_var) {
        m_priority = new_var;
      }

      /**
       * Get the value of m_subreqId
       * The Cuuid identifying the SubRequest, stored as a human readable string
       * @return the value of m_subreqId
       */
      std::string subreqId() const {
        return m_subreqId;
      }

      /**
       * Set the value of m_subreqId
       * The Cuuid identifying the SubRequest, stored as a human readable string
       * @param new_var the new value of m_subreqId
       */
      void setSubreqId(std::string new_var) {
        m_subreqId = new_var;
      }

      /**
       * Get the value of m_flags
       * open flags for the file handled by this SubRequest
       * @return the value of m_flags
       */
      int flags() const {
        return m_flags;
      }

      /**
       * Set the value of m_flags
       * open flags for the file handled by this SubRequest
       * @param new_var the new value of m_flags
       */
      void setFlags(int new_var) {
        m_flags = new_var;
      }

      /**
       * Get the value of m_modeBits
       * Permissions for the file handled by this SubRequest (essentially used at
       * creation time)
       * @return the value of m_modeBits
       */
      int modeBits() const {
        return m_modeBits;
      }

      /**
       * Set the value of m_modeBits
       * Permissions for the file handled by this SubRequest (essentially used at
       * creation time)
       * @param new_var the new value of m_modeBits
       */
      void setModeBits(int new_var) {
        m_modeBits = new_var;
      }

      /**
       * Get the value of m_creationTime
       * The time when this SubRequest was created
       * @return the value of m_creationTime
       */
      u_signed64 creationTime() const {
        return m_creationTime;
      }

      /**
       * Set the value of m_creationTime
       * The time when this SubRequest was created
       * @param new_var the new value of m_creationTime
       */
      void setCreationTime(u_signed64 new_var) {
        m_creationTime = new_var;
      }

      /**
       * Get the value of m_lastModificationTime
       * Time of the last modification on this SubRequest
       * @return the value of m_lastModificationTime
       */
      u_signed64 lastModificationTime() const {
        return m_lastModificationTime;
      }

      /**
       * Set the value of m_lastModificationTime
       * Time of the last modification on this SubRequest
       * @param new_var the new value of m_lastModificationTime
       */
      void setLastModificationTime(u_signed64 new_var) {
        m_lastModificationTime = new_var;
      }

      /**
       * Get the value of m_answered
       * Whether somebody answered to this subRequest already. 1 means that it is the
       * case, 0 does not ensure it is not the case.
       * This was introduced to deal with prepareToGet where the answer is sent before
       * the status of the subrequest changes
       * @return the value of m_answered
       */
      int answered() const {
        return m_answered;
      }

      /**
       * Set the value of m_answered
       * Whether somebody answered to this subRequest already. 1 means that it is the
       * case, 0 does not ensure it is not the case.
       * This was introduced to deal with prepareToGet where the answer is sent before
       * the status of the subrequest changes
       * @param new_var the new value of m_answered
       */
      void setAnswered(int new_var) {
        m_answered = new_var;
      }

      /**
       * Get the value of m_errorCode
       * error code associated with this subrequest. Has a meaning only for subrequests
       * in status SUBREQUEST_FAILED
       * @return the value of m_errorCode
       */
      int errorCode() const {
        return m_errorCode;
      }

      /**
       * Set the value of m_errorCode
       * error code associated with this subrequest. Has a meaning only for subrequests
       * in status SUBREQUEST_FAILED
       * @param new_var the new value of m_errorCode
       */
      void setErrorCode(int new_var) {
        m_errorCode = new_var;
      }

      /**
       * Get the value of m_errorMessage
       * error message associated with this subrequest. Has a meaning only for
       * subrequests in status SUBREQUEST_FAILED
       * @return the value of m_errorMessage
       */
      std::string errorMessage() const {
        return m_errorMessage;
      }

      /**
       * Set the value of m_errorMessage
       * error message associated with this subrequest. Has a meaning only for
       * subrequests in status SUBREQUEST_FAILED
       * @param new_var the new value of m_errorMessage
       */
      void setErrorMessage(std::string new_var) {
        m_errorMessage = new_var;
      }

      /**
       * Get the value of m_requestedFileSystems
       * The filesystems requested to fulfil the jobs resource requirements in the
       * scheduler
       * @return the value of m_requestedFileSystems
       */
      std::string requestedFileSystems() const {
        return m_requestedFileSystems;
      }

      /**
       * Set the value of m_requestedFileSystems
       * The filesystems requested to fulfil the jobs resource requirements in the
       * scheduler
       * @param new_var the new value of m_requestedFileSystems
       */
      void setRequestedFileSystems(std::string new_var) {
        m_requestedFileSystems = new_var;
      }

      /**
       * Get the value of m_svcHandler
       * Stager service that should handle this subrequest. Clients should leave this
       * field empty.
       * @return the value of m_svcHandler
       */
      std::string svcHandler() const {
        return m_svcHandler;
      }

      /**
       * Set the value of m_svcHandler
       * Stager service that should handle this subrequest. Clients should leave this
       * field empty.
       * @param new_var the new value of m_svcHandler
       */
      void setSvcHandler(std::string new_var) {
        m_svcHandler = new_var;
      }

      /**
       * Get the value of m_reqType
       * The numeric type of the request. Clients should leave this field empty.
       * @return the value of m_reqType
       */
      int reqType() const {
        return m_reqType;
      }

      /**
       * Set the value of m_reqType
       * The numeric type of the request. Clients should leave this field empty.
       * @param new_var the new value of m_reqType
       */
      void setReqType(int new_var) {
        m_reqType = new_var;
      }

      /**
       * Get the value of m_id
       * The id of this object
       * @return the value of m_id
       */
      u_signed64 id() const {
        return m_id;
      }

      /**
       * Set the value of m_id
       * The id of this object
       * @param new_var the new value of m_id
       */
      void setId(u_signed64 new_var) {
        m_id = new_var;
      }

      /**
       * Get the value of m_diskcopy. DEPRECATED, will always return 0.
       * @return the value of m_diskcopy
       */
      void* diskcopy() const {
        return 0;
      }

      /**
       * Set the value of m_diskcopy. DEPRECATED, will not do anything
       * @param new_var the new value of m_diskcopy
       */
      void setDiskcopy(void* /*new_var*/) {
      }

      /**
       * Get the value of m_castorFile
       * @return the value of m_castorFile
       */
      CastorFile* castorFile() const {
        return m_castorFile;
      }

      /**
       * Set the value of m_castorFile
       * @param new_var the new value of m_castorFile
       */
      void setCastorFile(CastorFile* new_var) {
        m_castorFile = new_var;
      }

      /**
       * Get the value of m_parent
       * @return the value of m_parent
       */
      SubRequest* parent() const {
        return m_parent;
      }

      /**
       * Set the value of m_parent
       * @param new_var the new value of m_parent
       */
      void setParent(SubRequest* new_var) {
        m_parent = new_var;
      }

      /**
       * Add a SubRequest* object to the m_childVector list
       */
      void addChild(SubRequest* add_object) {
        m_childVector.push_back(add_object);
      }

      /**
       * Remove a SubRequest* object from m_childVector
       */
      void removeChild(SubRequest* remove_object) {
        for (unsigned int i = 0; i < m_childVector.size(); i++) {
          SubRequest* item = m_childVector[i];
          if (item == remove_object) {
            std::vector<SubRequest*>::iterator it = m_childVector.begin() + i;
            m_childVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of SubRequest* objects held by m_childVector
       * @return list of SubRequest* objects held by m_childVector
       */
      std::vector<SubRequest*>& child() {
        return m_childVector;
      }

      /**
       * Get the value of m_status
       * @return the value of m_status
       */
      SubRequestStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(SubRequestStatusCodes new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_request
       * @return the value of m_request
       */
      FileRequest* request() const {
        return m_request;
      }

      /**
       * Set the value of m_request
       * @param new_var the new value of m_request
       */
      void setRequest(FileRequest* new_var) {
        m_request = new_var;
      }

      /**
       * Get the value of m_getNextStatus
       * DoNotStream
       * @return the value of m_getNextStatus
       */
      SubRequestGetNextStatusCodes getNextStatus() const {
        return m_getNextStatus;
      }

      /**
       * Set the value of m_getNextStatus
       * DoNotStream
       * @param new_var the new value of m_getNextStatus
       */
      void setGetNextStatus(SubRequestGetNextStatusCodes new_var) {
        m_getNextStatus = new_var;
      }

    private:

      unsigned int m_retryCounter;

      /*
       * Name of the file this SubRequest deals with.
       * When stored in the catalog, this is redundant with the link to the CastorFile table. However, this is needed in the client
      */
      std::string m_fileName;

      /// The protocol that will be used to access the file this SubRequest deals with
      std::string m_protocol;

      u_signed64 m_xsize;

      /// The priority defines in which order the files will be processed by the user when calling stage_get_next. The files of the SubRequest of same priority are given in a random order and lower priority files come first.
      unsigned int m_priority;

      /// The Cuuid identifying the SubRequest, stored as a human readable string
      std::string m_subreqId;

      /// open flags for the file handled by this SubRequest
      int m_flags;

      /// Permissions for the file handled by this SubRequest (essentially used at creation time)
      int m_modeBits;

      /// The time when this SubRequest was created
      u_signed64 m_creationTime;

      /// Time of the last modification on this SubRequest
      u_signed64 m_lastModificationTime;

      /*
       * Whether somebody answered to this subRequest already. 1 means that it is the case, 0 does not ensure it is not the case.
       * This was introduced to deal with prepareToGet where the answer is sent before the status of the subrequest changes
      */
      int m_answered;

      /// error code associated with this subrequest. Has a meaning only for subrequests in status SUBREQUEST_FAILED
      int m_errorCode;

      /// error message associated with this subrequest. Has a meaning only for subrequests in status SUBREQUEST_FAILED
      std::string m_errorMessage;

      /// The filesystems requested to fulfil the jobs resource requirements in the scheduler
      std::string m_requestedFileSystems;

      /// Stager service that should handle this subrequest. Clients should leave this field empty.
      std::string m_svcHandler;

      /// The numeric type of the request. Clients should leave this field empty.
      int m_reqType;

      /// The id of this object
      u_signed64 m_id;

      /// unused placeholder that allows the CASTOR client library to be backward compatible at the bibary level
      // although the diskcopy member has been dropped
      void* m_diskcopy;

      CastorFile* m_castorFile;

      SubRequest* m_parent;

      std::vector<SubRequest*> m_childVector;

      SubRequestStatusCodes m_status;

      FileRequest* m_request;

      /// DoNotStream
      SubRequestGetNextStatusCodes m_getNextStatus;

    }; /* end of class SubRequest */

  } /* end of namespace stager */

} /* end of namespace castor */

#endif // CASTOR_STAGER_SUBREQUEST_HPP
