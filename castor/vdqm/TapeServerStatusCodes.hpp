/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeServerStatusCodes.hpp
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

#ifndef CASTOR_VDQM_TAPESERVERSTATUSCODES_HPP
#define CASTOR_VDQM_TAPESERVERSTATUSCODES_HPP

#ifdef __cplusplus
namespace castor {

  namespace vdqm {

#endif
    /**
     * enum TapeServerStatusCodes
     * These flags are used to control, wether TapeRequests should be dedicated to tape
     * drives on this tape server or not.
     */
    enum TapeServerStatusCodes {
      TAPESERVER_ACTIVE, //If the tape server is in ACTIVE status, then its tape drives are used for the actual CASTOR production and the tapeRequest can be dedicated to this tape server
      TAPESERVER_INACTIVE //If the tape server is in INCATIVE status, then its tape drives are not involved into the actual CASTOR production.
    }; // end of enum TapeServerStatusCodes

    /**
     * Names of the differents representations, used to display
     * correctly the TapeServerStatusCodes enum
     */
    extern const char* TapeServerStatusCodesStrings[2];

#ifdef __cplusplus
  }; // end of namespace vdqm

}; // end of namespace castor

#endif
#endif // CASTOR_VDQM_TAPESERVERSTATUSCODES_HPP
