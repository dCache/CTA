/******************************************************************************
 *                      castor/stager/ReqIdRequest.hpp
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

#ifndef CASTOR_STAGER_REQIDREQUEST_HPP
#define CASTOR_STAGER_REQIDREQUEST_HPP

// Include Files
#include "castor/stager/Request.hpp"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class FileRequest;

    /**
     * class ReqIdRequest
     * An abstract ancester for all requests refering to another one
     */
    class ReqIdRequest : public virtual Request {

    public:

      /**
       * Empty Constructor
       */
      ReqIdRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~ReqIdRequest() throw();

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
       * Get the value of m_parent
       * @return the value of m_parent
       */
      FileRequest* parent() const {
        return m_parent;
      }

      /**
       * Set the value of m_parent
       * @param new_var the new value of m_parent
       */
      void setParent(FileRequest* new_var) {
        m_parent = new_var;
      }

    private:

      FileRequest* m_parent;

    }; // end of class ReqIdRequest

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_REQIDREQUEST_HPP
