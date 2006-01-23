/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/DatabaseHelper.hpp
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
 * @(#)$RCSfile: DatabaseHelper.hpp,v $ $Revision: 1.2 $ $Release$ $Date: 2006/01/23 14:56:44 $ $Author: felixehm $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_REPACK_DATABASEHELPER_HPP
#define CASTOR_REPACK_DATABASEHELPER_HPP

// Include Files
#include "castor/repack/RepackCommonHeader.hpp"
#include <vector>
#include "castor/exception/Internal.hpp"
#include "castor/repack/RepackRequest.hpp"
#include "castor/repack/RepackSubRequest.hpp"
#include "castor/BaseObject.hpp"
#include "castor/BaseAddress.hpp"
#include "castor/Services.hpp"

namespace castor {
	
  namespace repack {

    // Forward declarations
    class RepackRequest;
    class IObject;
	class Services;
    

    /**
     * class DatabaseHelper
     * 
     */
    class DatabaseHelper  : public castor::BaseObject{

    public:

      /**
       * Empty Constructor
       */
      DatabaseHelper() throw();

      /**
       * Empty Destructor
       */
      virtual ~DatabaseHelper() throw();

      /**
       * Stores a RepackRequest in the Database
       * @param rreq The RepackRequest
       */
      int store_Request(castor::repack::RepackRequest* rreq);
            

    }; // end of class DatabaseHelper

  }; // end of namespace repack

}; // end of namespace castor

#endif // CASTOR_REPACK_DATABASEHELPER_HPP
