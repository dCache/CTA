/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/infoPolicy/DbInfoRecallPolicy.hpp
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

#ifndef CASTOR_INFOPOLICY_DBINFORECALLPOLICY_HPP
#define CASTOR_INFOPOLICY_DBINFORECALLPOLICY_HPP

// Include Files
#include "castor/infoPolicy/DbInfoPolicy.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace infoPolicy {

    /**
     * class DbInfoRecallPolicy
     */
    class DbInfoRecallPolicy : public virtual DbInfoPolicy {

    public:

      /**
       * Empty Constructor
       */
      DbInfoRecallPolicy() throw();

      /**
       * Empty Destructor
       */
      virtual ~DbInfoRecallPolicy() throw();

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
       * Get the value of m_numBytes
       * @return the value of m_numBytes
       */
      u_signed64 numBytes() const {
        return m_numBytes;
      }

      /**
       * Set the value of m_numBytes
       * @param new_var the new value of m_numBytes
       */
      void setNumBytes(u_signed64 new_var) {
        m_numBytes = new_var;
      }

      /**
       * Get the value of m_numFiles
       * @return the value of m_numFiles
       */
      u_signed64 numFiles() const {
        return m_numFiles;
      }

      /**
       * Set the value of m_numFiles
       * @param new_var the new value of m_numFiles
       */
      void setNumFiles(u_signed64 new_var) {
        m_numFiles = new_var;
      }

      /**
       * Get the value of m_oldest
       * @return the value of m_oldest
       */
      u_signed64 oldest() const {
        return m_oldest;
      }

      /**
       * Set the value of m_oldest
       * @param new_var the new value of m_oldest
       */
      void setOldest(u_signed64 new_var) {
        m_oldest = new_var;
      }

      /**
       * Get the value of m_vid
       * @return the value of m_vid
       */
      std::string vid() const {
        return m_vid;
      }

      /**
       * Set the value of m_vid
       * @param new_var the new value of m_vid
       */
      void setVid(std::string new_var) {
        m_vid = new_var;
      }

      /**
       * Get the value of m_tapeId
       * @return the value of m_tapeId
       */
      u_signed64 tapeId() const {
        return m_tapeId;
      }

      /**
       * Set the value of m_tapeId
       * @param new_var the new value of m_tapeId
       */
      void setTapeId(u_signed64 new_var) {
        m_tapeId = new_var;
      }

      /**
       * Get the value of m_priority
       * @return the value of m_priority
       */
      u_signed64 priority() const {
        return m_priority;
      }

      /**
       * Set the value of m_priority
       * @param new_var the new value of m_priority
       */
      void setPriority(u_signed64 new_var) {
        m_priority = new_var;
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

    private:

      u_signed64 m_numBytes;

      u_signed64 m_numFiles;

      u_signed64 m_oldest;

      std::string m_vid;

      u_signed64 m_tapeId;

      u_signed64 m_priority;

      /// The id of this object
      u_signed64 m_id;

    }; /* end of class DbInfoRecallPolicy */

  } /* end of namespace infoPolicy */

} /* end of namespace castor */

#endif // CASTOR_INFOPOLICY_DBINFORECALLPOLICY_HPP
