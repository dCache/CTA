/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
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

#ifdef __cplusplus
namespace castor {

  namespace stager {

#endif
    /**
     * enum DiskServerStatusCode
     * Possible status codes for a DiskServer
     */
    enum DiskServerStatusCode {
      DISKSERVER_PRODUCTION = 0,
      DISKSERVER_DRAINING = 1,
      DISKSERVER_DISABLED = 2,
      DISKSERVER_READONLY = 3
    }; /* end of enum DiskServerStatusCode */

    /**
     * Names of the differents representations, used to display
     * correctly the DiskServerStatusCode enum
     */
    extern const char* DiskServerStatusCodeStrings[4];

#ifdef __cplusplus
  } /* end of namespace stager */

} /* end of namespace castor */

#endif
