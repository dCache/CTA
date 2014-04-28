/******************************************************************************
 *                castor/legacymsg/CupvProxyTcpIp.hpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/exception/Exception.hpp"
#include "castor/legacymsg/CupvProxy.hpp"
#include "castor/log/Logger.hpp"

#include <string>

namespace castor {
namespace legacymsg {

/**
 * Proxy class representing the CASTOR user-privilege validation-daemon.
 */
class CupvProxyTcpIp: public CupvProxy {
public:

  /**
   * Constructor.
   *
   * @param log The object representing the API of the CASTOR logging system.
   * @param cupvHostName The name of the host on which the cupvd daemon is
   * running.
   * @param vdqmPort The TCP/IP port on which the cupvd daemon is listening.
   * @param netTimeout The timeout in seconds to be applied when performing
   * network read and write operations.
   */
  CupvProxyTcpIp(log::Logger &log, const std::string &cupvHostName,
    const unsigned short cupvPort, const int netTimeout) throw();

  /**
   * Destructor.
   */
  ~CupvProxyTcpIp() throw();

  /**
   * Returns true if the specified authorization is granted else false.
   *
   * @param uid The unix user-ID of the user for whom the authorization is
   * requested.
   * @param gid The unix group-ID of the user for whom the authorization is
   * requested.
   * @param sourceHost The source host.
   * @param tagertHost The target host.
   * @param privilege The privilege which must be one of the following:
   * P_ADMIN, P_GRP_ADMIN, P_OPERATOR, P_TAPE_OPERATOR, P_TAPE_SYSTEM or
   * P_UPV_ADMIN.
   * @return True if the specified authorization is granted else false.
   */
  bool isGranted(
    const uid_t uid,
    const gid_t gid,
    const std::string &sourceHost,
    const std::string &targetHost,
    const int privilege) throw(castor::exception::Exception);

private:

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * The name of the host on which the cupvd daemon is running.
   */
  const std::string m_cupvHostName;

  /**
   * The TCP/IP port on which the cupvd daemon is listening.
   */
  const unsigned short m_cupvPort;

  /**
   * The timeout in seconds to be applied when performing network read and
   * write operations.
   */
  const int m_netTimeout;

}; // class CupvProxyTcpIp

} // namespace legacymsg
} // namespace castor

