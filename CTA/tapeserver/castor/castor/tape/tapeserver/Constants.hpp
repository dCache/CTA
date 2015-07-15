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
 *
 *
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "Castor_limits.h"

#include <stdint.h>
#include <stdlib.h>


namespace castor     {
namespace tape       {
namespace tapeserver {
  /**
   * The compile-time default value of the tapebridged configuration parameter
   * named TAPEBRIDGE/MAXBYTESBEFOREFLUSH.
   *
   * The value of this parameter defines the maximum number of bytes to be
   * written to tape before a flush to tape (synchronised tape-mark).  Please
   * note that a flush occurs on a file boundary therefore more bytes will
   * normally be written to tape before the actual flush occurs.
   *
   * The value of this parameter is used when buffered tape-marks are being
   * used over multiple files as defined by the parameter named
   * TAPEBRIDGE/USEBUFFEREDTAPEMARKSOVERMULTIPLEFILES.
   */
  const uint64_t TAPEBRIDGE_MAXBYTESBEFOREFLUSH = 32000000000ULL; // 32 GB

  /**
   * The compile-time default value of the tapebridged configuration parameter
   * named TAPEBRIDGE/MAXFILESBEFOREFLUSH.
   *
   * The value of this parameter defines the maximum number of files to be
   * written to tape before a flush to tape (synchronised tape-mark).
   *
   * The value of this parameter is used when buffered tape-marks are being
   * used over multiple files as defined by the parameter named
   * TAPEBRIDGE/USEBUFFEREDTAPEMARKSOVERMULTIPLEFILES.
   */
  const uint64_t TAPEBRIDGE_MAXFILESBEFOREFLUSH = 200;

  /**
   * When the tapegatewayd daemon is asked for a set of files to migrate to
   * tape, this is the compile-time default for the maximum number of bytes
   * the resulting set can represent.  This number may be exceeded when the set
   * contains a single file.
   */
  const uint64_t TAPEBRIDGE_BULKREQUESTMIGRATIONMAXBYTES = 80000000000ULL;

  /**
   * When the tapegatewayd daemon is asked for a set of files to migrate to
   * tape, this is the compile-time default for the maximum number of files
   * that can be in that set.
   */
  const uint64_t TAPEBRIDGE_BULKREQUESTMIGRATIONMAXFILES = 500;

  /**
   * When the tapegatewayd daemon is asked for a set of files to recall from
   * tape, this is the compile-time default for the maximum number of bytes
   * the resulting set can represent.  This number may be exceeded when the set
   * contains a single file.
   */
  const uint64_t TAPEBRIDGE_BULKREQUESTRECALLMAXBYTES = 80000000000ULL;

  /**
   * When the tapegatewayd daemon is asked for a set of files to recall from
   * tape, this is the compile-time default for the maximum number of files
   * that can be in that set.
   */
  const uint64_t TAPEBRIDGE_BULKREQUESTRECALLMAXFILES = 500;

} // namespace tapeserver
} // namespace tape
} // namespace castor
