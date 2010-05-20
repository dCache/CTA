/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/GCTypeCodes.hpp
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

#ifndef CASTOR_STAGER_GCTYPECODES_HPP
#define CASTOR_STAGER_GCTYPECODES_HPP

#ifdef __cplusplus
namespace castor {

  namespace stager {

#endif
    /**
     * enum GCTypeCodes
     */
    enum GCTypeCodes {
      GCTYPE_AUTO = 0,
      GCTYPE_USER = 1,
      GCTYPE_TOO_MANY_REPLICAS = 2,
      GCTYPE_DRAINING_FS = 3,
      GCTYPE_NS_SYNCH = 4
    }; /* end of enum GCTypeCodes */

    /**
     * Names of the differents representations, used to display
     * correctly the GCTypeCodes enum
     */
    extern const char* GCTypeCodesStrings[5];

#ifdef __cplusplus
  } /* end of namespace stager */

} /* end of namespace castor */

#endif
#endif /* CASTOR_STAGER_GCTYPECODES_HPP */
