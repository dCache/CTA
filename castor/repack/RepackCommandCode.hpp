/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/RepackCommandCode.hpp
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

#ifndef CASTOR_REPACK_REPACKCOMMANDCODE_HPP
#define CASTOR_REPACK_REPACKCOMMANDCODE_HPP

#ifdef __cplusplus
namespace castor {

  namespace repack {

#endif
    /**
     * enum RepackCommandCode
     */
    enum RepackCommandCode {
      REPACK = 1,
      RESTART = 2,
      REMOVE = 3,
      GET_STATUS = 4,
      GET_STATUS_ALL = 5,
      ARCHIVE = 6,
      ARCHIVE_ALL = 7,
      GET_NS_STATUS = 8,
      GET_ERROR = 9,
      GET_MULTICOPYINFO = 10
    }; /* end of enum RepackCommandCode */

    /**
     * Names of the differents representations, used to display
     * correctly the RepackCommandCode enum
     */
    extern const char* RepackCommandCodeStrings[11];

#ifdef __cplusplus
  } /* end of namespace repack */

} /* end of namespace castor */

#endif
#endif /* CASTOR_REPACK_REPACKCOMMANDCODE_HPP */
