/******************************************************************************
 *                      castor/stager/GCRemovedFile.hpp
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
 * @(#)$RCSfile: GCRemovedFile.hpp,v $ $Revision: 1.1 $ $Release$ $Date: 2005/02/09 17:05:45 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_STAGER_GCREMOVEDFILE_HPP
#define CASTOR_STAGER_GCREMOVEDFILE_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    /**
     * class GCRemovedFile
     * Represents a file that was just deleted on a DiskServer
     */
    class GCRemovedFile : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      GCRemovedFile() throw();

      /**
       * Empty Destructor
       */
      virtual ~GCRemovedFile() throw();

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
       * Get the value of m_diskCopyId
       * Id of the DiskCopy reprensenting the deleted file in the catalog
       * @return the value of m_diskCopyId
       */
      u_signed64 diskCopyId() const {
        return m_diskCopyId;
      }

      /**
       * Set the value of m_diskCopyId
       * Id of the DiskCopy reprensenting the deleted file in the catalog
       * @param new_var the new value of m_diskCopyId
       */
      void setDiskCopyId(u_signed64 new_var) {
        m_diskCopyId = new_var;
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

      /// Id of the DiskCopy reprensenting the deleted file in the catalog
      u_signed64 m_diskCopyId;

      /// The id of this object
      u_signed64 m_id;

    }; // end of class GCRemovedFile

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_GCREMOVEDFILE_HPP
