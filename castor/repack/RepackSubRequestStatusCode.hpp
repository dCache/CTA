/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/RepackSubRequestStatusCode.hpp
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

#ifndef CASTOR_REPACK_REPACKSUBREQUESTSTATUSCODE_HPP
#define CASTOR_REPACK_REPACKSUBREQUESTSTATUSCODE_HPP

#ifdef __cplusplus
namespace castor {

  namespace repack {

#endif
    /**
     * enum RepackSubRequestStatusCode
     */
    enum RepackSubRequestStatusCode {
      RSUBREQUEST_TOBECHECKED = 0,
      RSUBREQUEST_TOBESTAGED = 1,
      RSUBREQUEST_ONGOING = 2,
      RSUBREQUEST_TOBECLEANED = 3,
      RSUBREQUEST_DONE = 4,
      RSUBREQUEST_FAILED = 5,
      RSUBREQUEST_TOBEREMOVED = 6,
      RSUBREQUEST_TOBERESTARTED = 7,
      RSUBREQUEST_ARCHIVED = 8,
      RSUBREQUEST_ONHOLD = 9,
      RSUBREQUEST_ONHOLD_MAXTAPES = 10,
      RSUBREQUEST_ONHOLD_MAXFILES = 11,
      RSUBREQUEST_ONHOLD_MULTICOPY = 12
    }; /* end of enum RepackSubRequestStatusCode */

    /**
     * Names of the differents representations, used to display
     * correctly the RepackSubRequestStatusCode enum
     */
    extern const char* RepackSubRequestStatusCodeStrings[13];

#ifdef __cplusplus
  } /* end of namespace repack */

} /* end of namespace castor */

#endif
#endif /* CASTOR_REPACK_REPACKSUBREQUESTSTATUSCODE_HPP */
