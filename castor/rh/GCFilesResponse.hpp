/******************************************************************************
 *                      castor/rh/GCFilesResponse.hpp
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

#ifndef CASTOR_RH_GCFILESRESPONSE_HPP
#define CASTOR_RH_GCFILESRESPONSE_HPP

// Include Files
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class GCLocalFile;

  }; // end of namespace stager

  namespace rh {

    /**
     * class GCFilesResponse
     * A list of files to be deleted on a diskServer. Answer to a Files2Delete Request
     */
    class GCFilesResponse : public virtual Response {

    public:

      /**
       * Empty Constructor
       */
      GCFilesResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~GCFilesResponse() throw();

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
       * Add a castor::stager::GCLocalFile* object to the m_filesVector list
       */
      void addFiles(castor::stager::GCLocalFile* add_object) {
        m_filesVector.push_back(add_object);
      }

      /**
       * Remove a castor::stager::GCLocalFile* object from m_filesVector
       */
      void removeFiles(castor::stager::GCLocalFile* remove_object) {
        for (unsigned int i = 0; i < m_filesVector.size(); i++) {
          castor::stager::GCLocalFile* item = m_filesVector[i];
          if (item == remove_object) {
            std::vector<castor::stager::GCLocalFile*>::iterator it = m_filesVector.begin() + i;
            m_filesVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of castor::stager::GCLocalFile* objects held by m_filesVector
       * @return list of castor::stager::GCLocalFile* objects held by m_filesVector
       */
      std::vector<castor::stager::GCLocalFile*>& files() {
        return m_filesVector;
      }

    private:

      /// The id of this object
      u_signed64 m_id;

      std::vector<castor::stager::GCLocalFile*> m_filesVector;

    }; // end of class GCFilesResponse

  }; // end of namespace rh

}; // end of namespace castor

#endif // CASTOR_RH_GCFILESRESPONSE_HPP
