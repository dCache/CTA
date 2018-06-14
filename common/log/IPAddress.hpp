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
 * A simple object around an IP address
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

// Include Files
#include <ostream>

namespace cta {

  namespace log {

    /**
     * A simple object around an IP address
     */
    class IPAddress {

    public:

      /**
       * Constructor
       */
      IPAddress(int ip) : m_ip(ip) {};

      /**
       * Accessor
       */
      int ip() const { return m_ip; }

    private:

      /// the IP address, as an int
      int m_ip;

    };

  } // end of namespace log

} // end of namespace cta

/**
 * non-member operator to stream an IpAdress
 */
std::ostream& operator<<(std::ostream& out, const cta::log::IPAddress& ip);