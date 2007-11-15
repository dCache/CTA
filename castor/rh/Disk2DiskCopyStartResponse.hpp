/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/rh/Disk2DiskCopyStartResponse.hpp
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

#ifndef CASTOR_RH_DISK2DISKCOPYSTARTRESPONSE_HPP
#define CASTOR_RH_DISK2DISKCOPYSTARTRESPONSE_HPP

// Include Files
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class DiskCopy;

  } /* end of namespace stager */

  namespace rh {

    /**
     * class Disk2DiskCopyStartResponse
     * Response to the Disk2DiskCopyRequest
     */
    class Disk2DiskCopyStartResponse : public virtual Response {

    public:

      /**
       * Empty Constructor
       */
      Disk2DiskCopyStartResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~Disk2DiskCopyStartResponse() throw();

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
       * Get the value of m_diskCopy
       * The information about the destination disk copy
       * @return the value of m_diskCopy
       */
      castor::stager::DiskCopy* diskCopy() const {
        return m_diskCopy;
      }

      /**
       * Set the value of m_diskCopy
       * The information about the destination disk copy
       * @param new_var the new value of m_diskCopy
       */
      void setDiskCopy(castor::stager::DiskCopy* new_var) {
        m_diskCopy = new_var;
      }

      /**
       * Get the value of m_sourceDiskCopy
       * The information about the source disk copy
       * @return the value of m_sourceDiskCopy
       */
      castor::stager::DiskCopy* sourceDiskCopy() const {
        return m_sourceDiskCopy;
      }

      /**
       * Set the value of m_sourceDiskCopy
       * The information about the source disk copy
       * @param new_var the new value of m_sourceDiskCopy
       */
      void setSourceDiskCopy(castor::stager::DiskCopy* new_var) {
        m_sourceDiskCopy = new_var;
      }

    private:

      /// The id of this object
      u_signed64 m_id;

      /// The information about the destination disk copy
      castor::stager::DiskCopy* m_diskCopy;

      /// The information about the source disk copy
      castor::stager::DiskCopy* m_sourceDiskCopy;

    }; /* end of class Disk2DiskCopyStartResponse */

  } /* end of namespace rh */

} /* end of namespace castor */

#endif // CASTOR_RH_DISK2DISKCOPYSTARTRESPONSE_HPP
