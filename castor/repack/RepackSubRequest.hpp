/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/RepackSubRequest.hpp
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

#ifndef CASTOR_REPACK_REPACKSUBREQUEST_HPP
#define CASTOR_REPACK_REPACKSUBREQUEST_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace repack {

    // Forward declarations
    class RepackSegment;
    class RepackRequest;

    /**
     * class RepackSubRequest
     */
    class RepackSubRequest : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      RepackSubRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~RepackSubRequest() throw();

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
       * Get the value of m_vid
       * The Tape Volumes for one Request.
       * @return the value of m_vid
       */
      std::string vid() const {
        return m_vid;
      }

      /**
       * Set the value of m_vid
       * The Tape Volumes for one Request.
       * @param new_var the new value of m_vid
       */
      void setVid(std::string new_var) {
        m_vid = new_var;
      }

      /**
       * Get the value of m_xsize
       * The full size of the request in Bytes.
       * @return the value of m_xsize
       */
      u_signed64 xsize() const {
        return m_xsize;
      }

      /**
       * Set the value of m_xsize
       * The full size of the request in Bytes.
       * @param new_var the new value of m_xsize
       */
      void setXsize(u_signed64 new_var) {
        m_xsize = new_var;
      }

      /**
       * Get the value of m_status
       * @return the value of m_status
       */
      int status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(int new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_cuuid
       * @return the value of m_cuuid
       */
      std::string cuuid() const {
        return m_cuuid;
      }

      /**
       * Set the value of m_cuuid
       * @param new_var the new value of m_cuuid
       */
      void setCuuid(std::string new_var) {
        m_cuuid = new_var;
      }

      /**
       * Get the value of m_filesMigrating
       * The amount files which are still being migrated to tape.
       * @return the value of m_filesMigrating
       */
      unsigned int filesMigrating() const {
        return m_filesMigrating;
      }

      /**
       * Set the value of m_filesMigrating
       * The amount files which are still being migrated to tape.
       * @param new_var the new value of m_filesMigrating
       */
      void setFilesMigrating(unsigned int new_var) {
        m_filesMigrating = new_var;
      }

      /**
       * Get the value of m_filesStaging
       * The amount of file to be staged.
       * @return the value of m_filesStaging
       */
      unsigned int filesStaging() const {
        return m_filesStaging;
      }

      /**
       * Set the value of m_filesStaging
       * The amount of file to be staged.
       * @param new_var the new value of m_filesStaging
       */
      void setFilesStaging(unsigned int new_var) {
        m_filesStaging = new_var;
      }

      /**
       * Get the value of m_files
       * @return the value of m_files
       */
      unsigned int files() const {
        return m_files;
      }

      /**
       * Set the value of m_files
       * @param new_var the new value of m_files
       */
      void setFiles(unsigned int new_var) {
        m_files = new_var;
      }

      /**
       * Get the value of m_filesFailed
       * @return the value of m_filesFailed
       */
      unsigned int filesFailed() const {
        return m_filesFailed;
      }

      /**
       * Set the value of m_filesFailed
       * @param new_var the new value of m_filesFailed
       */
      void setFilesFailed(unsigned int new_var) {
        m_filesFailed = new_var;
      }

      /**
       * Get the value of m_submitTime
       * Time of submission of files to the stager
       * @return the value of m_submitTime
       */
      u_signed64 submitTime() const {
        return m_submitTime;
      }

      /**
       * Set the value of m_submitTime
       * Time of submission of files to the stager
       * @param new_var the new value of m_submitTime
       */
      void setSubmitTime(u_signed64 new_var) {
        m_submitTime = new_var;
      }

      /**
       * Get the value of m_filesStaged
       * @return the value of m_filesStaged
       */
      u_signed64 filesStaged() const {
        return m_filesStaged;
      }

      /**
       * Set the value of m_filesStaged
       * @param new_var the new value of m_filesStaged
       */
      void setFilesStaged(u_signed64 new_var) {
        m_filesStaged = new_var;
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
       * Add a RepackSegment* object to the m_segmentVector list
       */
      void addSegment(RepackSegment* add_object) {
        m_segmentVector.push_back(add_object);
      }

      /**
       * Remove a RepackSegment* object from m_segmentVector
       */
      void removeSegment(RepackSegment* remove_object) {
        for (unsigned int i = 0; i < m_segmentVector.size(); i++) {
          RepackSegment* item = m_segmentVector[i];
          if (item == remove_object) {
            std::vector<RepackSegment*>::iterator it = m_segmentVector.begin() + i;
            m_segmentVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of RepackSegment* objects held by m_segmentVector
       * @return list of RepackSegment* objects held by m_segmentVector
       */
      std::vector<RepackSegment*>& segment() {
        return m_segmentVector;
      }

      /**
       * Get the value of m_requestID
       * @return the value of m_requestID
       */
      RepackRequest* requestID() const {
        return m_requestID;
      }

      /**
       * Set the value of m_requestID
       * @param new_var the new value of m_requestID
       */
      void setRequestID(RepackRequest* new_var) {
        m_requestID = new_var;
      }

    private:

      /// The Tape Volumes for one Request.
      std::string m_vid;

      /// The full size of the request in Bytes.
      u_signed64 m_xsize;

      int m_status;

      std::string m_cuuid;

      /// The amount files which are still being migrated to tape.
      unsigned int m_filesMigrating;

      /// The amount of file to be staged.
      unsigned int m_filesStaging;

      unsigned int m_files;

      unsigned int m_filesFailed;

      /// Time of submission of files to the stager
      u_signed64 m_submitTime;

      u_signed64 m_filesStaged;

      /// The id of this object
      u_signed64 m_id;

      std::vector<RepackSegment*> m_segmentVector;

      RepackRequest* m_requestID;

    }; /* end of class RepackSubRequest */

  } /* end of namespace repack */

} /* end of namespace castor */

#endif // CASTOR_REPACK_REPACKSUBREQUEST_HPP
