/******************************************************************************
 *                castor/tape/tapebridge/ConfigParams.hpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/exception/Exception.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/tape/tapebridge/ConfigParam.hpp"

#include <stdint.h>


namespace castor     {
namespace tape       {
namespace tapebridge {

/**
 * Abstract base-class of objects used to determine and store
 * tapebridged-daemon configuration-parameters.
 */
class ConfigParams {
public:

  /**
   * Virtual destructor.
   */
  virtual ~ConfigParams();

protected:

  /**
   * Determines the values of the configuration-parameters for which this
   * object is responsible.
   *
   * This method determines each required value by first reading the
   * environment variables, then if unsuccessful by reading castor.conf and
   * finally if still unsuccessfull by using the compile-time default.
   *
   * This method throws a castor::exception::InvalidArgument exception if the
   * source-value of one the configuration parameters for which this object
   * is responsible is invalid.
   */
  virtual void determineConfigParams()
    throw(castor::exception::InvalidArgument, castor::exception::Exception) = 0;

  /**
   * Determines the value of the specified configuration parameter.
   *
   * This method determines the required value by first reading the
   * environment variables, then if unsuccessful by reading castor.conf and
   * finally if still unsuccessfull by using the compile-time default.
   *
   * This method throws a castor::exception::InvalidArgument exception if the
   * source-value of the configuration parameter is invalid.
   *
   * @param param              In/Out Parameter: The configuration parameter to
   *                           be determined and set.
   * @param compileTimeDefault The compile-time default value for the
   *                           configuration parameter.
   */
  void determineUint64ConfigParam(
    ConfigParam<uint64_t> &param,
    const uint64_t        compileTimeDefault)
    throw(castor::exception::InvalidArgument, castor::exception::Exception);

}; // class ConfigParams

} // namespace tapebridge
} // namespace tape
} // namespace castor

