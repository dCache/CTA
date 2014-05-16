/******************************************************************************
 *         castor/legacymsg/NsProxyTcpIpFactory.hpp
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
 * @author dkruse@cern.ch
 *****************************************************************************/

#include "castor/legacymsg/NsProxyTcpIp.hpp"
#include "castor/legacymsg/NsProxyTcpIpFactory.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::legacymsg::NsProxyTcpIpFactory::NsProxyTcpIpFactory(log::Logger &log, const int netTimeout) throw():
    m_log(log),
    m_netTimeout(netTimeout) {
} 

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::legacymsg::NsProxyTcpIpFactory::~NsProxyTcpIpFactory() throw() {
}

//------------------------------------------------------------------------------
// create
//------------------------------------------------------------------------------
castor::legacymsg::NsProxy *castor::legacymsg::NsProxyTcpIpFactory::create() {
  return new NsProxyTcpIp(m_log, m_netTimeout);
}
