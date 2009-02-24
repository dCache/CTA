/******************************************************************************
 *                      Security.hpp
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
 * @(#)$RCSfile: Security.hpp,v $ $Revision: 1.2 $ $Release$ $Date: 2009/02/24 17:34:11 $ $Author: riojac3 $
 *
 * 
 *
 * @author Rosa Garcia
 *****************************************************************************/

#ifndef EXCEPTION_SECURITY_HPP 
#define EXCEPTION_SECURITY_HPP 1

// Include Files
#include "castor/exception/Exception.hpp"

namespace castor {

  namespace exception {

    /**
     * Invalid argument exception
     */
    class Security : public castor::exception::Exception {
      
    public:
      
      /**
       * default constructor
       */
      Security(int se);

    };
      
  } // end of namespace exception

} // end of namespace castor

#endif // EXCEPTION_SECURITY_HPP
