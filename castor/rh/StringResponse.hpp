/******************************************************************************
 *                      castor/rh/StringResponse.hpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

#ifndef CASTOR_RH_STRINGRESPONSE_HPP
#define CASTOR_RH_STRINGRESPONSE_HPP

// Include Files
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace rh {

    /**
     * class StringResponse
     * The most simple response : just a string
     */
    class StringResponse : public virtual Response {

    public:

      /**
       * Empty Constructor
       */
      StringResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~StringResponse() throw();

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
       * Sets the id of the object
       * @param id The new id
       */
      virtual void setId(u_signed64 id);

      /**
       * gets the id of the object
       */
      virtual u_signed64 id() const;

      /**
       * Gets the type of the object
       */
      virtual int type() const;

      /*********************************/
      /* End of IObject abstract class */
      /*********************************/
      /**
       * Get the value of m_content
       * Content of the response
       * @return the value of m_content
       */
      std::string content() const {
        return m_content;
      }

      /**
       * Set the value of m_content
       * Content of the response
       * @param new_var the new value of m_content
       */
      void setContent(std::string new_var) {
        m_content = new_var;
      }

    private:

    private:

      /// Content of the response
      std::string m_content;

      /// The id of this object
      u_signed64 m_id;

    }; // end of class StringResponse

  }; // end of namespace rh

}; // end of namespace castor

#endif // CASTOR_RH_STRINGRESPONSE_HPP
