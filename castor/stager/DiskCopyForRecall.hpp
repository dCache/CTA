/******************************************************************************
 *                      castor/stager/DiskCopyForRecall.hpp
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

#ifndef CASTOR_STAGER_DISKCOPYFORRECALL_HPP
#define CASTOR_STAGER_DISKCOPYFORRECALL_HPP

// Include Files
#include "castor/stager/DiskCopy.hpp"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace stager {

    /**
     * class DiskCopyForRecall
     * 
     */
    class DiskCopyForRecall : public DiskCopy {

    public:

      /**
       * Empty Constructor
       */
      DiskCopyForRecall() throw();

      /**
       * Empty Destructor
       */
      virtual ~DiskCopyForRecall() throw();

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
       * Get the value of m_mountPoint
       * The mountpoint of the filesystem where the file to be recalled should be put
       * @return the value of m_mountPoint
       */
      std::string mountPoint() const {
        return m_mountPoint;
      }

      /**
       * Set the value of m_mountPoint
       * The mountpoint of the filesystem where the file to be recalled should be put
       * @param new_var the new value of m_mountPoint
       */
      void setMountPoint(std::string new_var) {
        m_mountPoint = new_var;
      }

      /**
       * Get the value of m_diskServer
       * The disk server on which the file to be recalled should be put
       * @return the value of m_diskServer
       */
      std::string diskServer() const {
        return m_diskServer;
      }

      /**
       * Set the value of m_diskServer
       * The disk server on which the file to be recalled should be put
       * @param new_var the new value of m_diskServer
       */
      void setDiskServer(std::string new_var) {
        m_diskServer = new_var;
      }

    private:

      /// The mountpoint of the filesystem where the file to be recalled should be put
      std::string m_mountPoint;

      /// The disk server on which the file to be recalled should be put
      std::string m_diskServer;

    }; // end of class DiskCopyForRecall

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_DISKCOPYFORRECALL_HPP
