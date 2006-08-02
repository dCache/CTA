/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/RequestQueryType.hpp
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

#ifndef CASTOR_STAGER_REQUESTQUERYTYPE_HPP
#define CASTOR_STAGER_REQUESTQUERYTYPE_HPP

#ifdef __cplusplus
namespace castor {

  namespace stager {

#endif
    /**
     * enum RequestQueryType
     * An enumeration defining the different types of Request queries
     */
    enum RequestQueryType {
      REQUESTQUERYTYPE_FILENAME = 0,
      REQUESTQUERYTYPE_REQID = 1,
      REQUESTQUERYTYPE_USERTAG = 2,
      REQUESTQUERYTYPE_FILEID = 3,
      REQUESTQUERYTYPE_REQID_GETNEXT = 4,
      REQUESTQUERYTYPE_USERTAG_GETNEXT = 5
    }; // end of enum RequestQueryType

    /**
     * Names of the differents representations, used to display
     * correctly the RequestQueryType enum
     */
    extern const char* RequestQueryTypeStrings[6];

#ifdef __cplusplus
  }; // end of namespace stager

}; // end of namespace castor

#endif
#endif // CASTOR_STAGER_REQUESTQUERYTYPE_HPP
