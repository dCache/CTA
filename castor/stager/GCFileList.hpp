/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

// Include Files
#include "castor/stager/Request.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class GCFile;

    /**
     * class GCFileList
     */
    class GCFileList : public virtual Request {

    public:

      /**
       * Empty Constructor
       */
      GCFileList() throw();

      /**
       * Empty Destructor
       */
      virtual ~GCFileList() throw();

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
       * Add a GCFile* object to the m_filesVector list
       */
      void addFiles(GCFile* add_object) {
        m_filesVector.push_back(add_object);
      }

      /**
       * Remove a GCFile* object from m_filesVector
       */
      void removeFiles(GCFile* remove_object) {
        for (unsigned int i = 0; i < m_filesVector.size(); i++) {
          GCFile* item = m_filesVector[i];
          if (item == remove_object) {
            std::vector<GCFile*>::iterator it = m_filesVector.begin() + i;
            m_filesVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of GCFile* objects held by m_filesVector
       * @return list of GCFile* objects held by m_filesVector
       */
      std::vector<GCFile*>& files() {
        return m_filesVector;
      }

    private:

      std::vector<GCFile*> m_filesVector;

    }; /* end of class GCFileList */

  } /* end of namespace stager */

} /* end of namespace castor */

