/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/DeviceGroupName.hpp
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

#ifndef CASTOR_VDQM_DEVICEGROUPNAME_HPP
#define CASTOR_VDQM_DEVICEGROUPNAME_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace vdqm {

    /**
     * class DeviceGroupName
     * Please note that this is a table with static entries! It contains all existing
     * device group names.
     */
    class DeviceGroupName : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      DeviceGroupName() throw();

      /**
       * Empty Destructor
       */
      virtual ~DeviceGroupName() throw();

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
       * Get the value of m_dgName
       * The dgn, which comes with the old protocol and is now used to specify the
       * physical library.
       * @return the value of m_dgName
       */
      std::string dgName() const {
        return m_dgName;
      }

      /**
       * Set the value of m_dgName
       * The dgn, which comes with the old protocol and is now used to specify the
       * physical library.
       * @param new_var the new value of m_dgName
       */
      void setDgName(std::string new_var) {
        m_dgName = new_var;
      }

      /**
       * Get the value of m_libraryName
       * The name of the library, to which the tape or tape drive belongs to.
       * @return the value of m_libraryName
       */
      std::string libraryName() const {
        return m_libraryName;
      }

      /**
       * Set the value of m_libraryName
       * The name of the library, to which the tape or tape drive belongs to.
       * @param new_var the new value of m_libraryName
       */
      void setLibraryName(std::string new_var) {
        m_libraryName = new_var;
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

      /// The dgn, which comes with the old protocol and is now used to specify the physical library.
      std::string m_dgName;

      /// The name of the library, to which the tape or tape drive belongs to.
      std::string m_libraryName;

      /// The id of this object
      u_signed64 m_id;

    }; /* end of class DeviceGroupName */

  } /* end of namespace vdqm */

} /* end of namespace castor */

#endif // CASTOR_VDQM_DEVICEGROUPNAME_HPP
