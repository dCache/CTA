/******************************************************************************
 *                      castor/stager/StageOutRequest.hpp
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
 * @(#)$RCSfile: StageOutRequest.hpp,v $ $Revision: 1.3 $ $Release$ $Date: 2004/11/04 10:26:14 $ $Author: sponcec3 $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_STAGER_STAGEOUTREQUEST_HPP
#define CASTOR_STAGER_STAGEOUTREQUEST_HPP

// Include Files
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    /**
     * class StageOutRequest
     * A stageout request 
     */
    class StageOutRequest : public virtual Request {

    public:

      /**
       * Empty Constructor
       */
      StageOutRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~StageOutRequest() throw();

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
       * Get the value of m_openmode
       * Specifies the permissions to use when creating the internal disk file. Modified
       * by the process's umask (mode & ~umask).
       * @return the value of m_openmode
       */
      int openmode() const {
        return m_openmode;
      }

      /**
       * Set the value of m_openmode
       * Specifies the permissions to use when creating the internal disk file. Modified
       * by the process's umask (mode & ~umask).
       * @param new_var the new value of m_openmode
       */
      void setOpenmode(int new_var) {
        m_openmode = new_var;
      }

    private:

    private:

      /// Specifies the permissions to use when creating the internal disk file. Modified by the process's umask (mode & ~umask).
      int m_openmode;

      /// The id of this object
      u_signed64 m_id;

    }; // end of class StageOutRequest

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_STAGEOUTREQUEST_HPP
