/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/FilesToMigrateListRequest.hpp
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
#include "castor/tape/tapegateway/FilesListRequest.hpp"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace tape {

    namespace tapegateway {

      /**
       * class FilesToMigrateListRequest
       */
      class FilesToMigrateListRequest : public FilesListRequest {

      public:

        /**
         * Empty Constructor
         */
        FilesToMigrateListRequest() throw();

        /**
         * Empty Destructor
         */
        virtual ~FilesToMigrateListRequest() throw();

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
      }; /* end of class FilesToMigrateListRequest */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

