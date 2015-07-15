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
 * A multi-file Response Handler for command line clients
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

// Include Files
#include "errno.h"
#include "VectorResponseHandler.hpp"
#include "castor/rh/Response.hpp"
#include "castor/exception/Exception.hpp"
#include <castor/IObject.hpp>
#include <vector>


//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::client::VectorResponseHandler::VectorResponseHandler
(std::vector<castor::rh::Response *> *vector)
  {
  if (0 == vector) {
    castor::exception::Exception e(EINVAL);
    e.getMessage() << "Null pointer passed to VectorResponseHandler constructor";
    throw e;
  }
  m_responses = vector;
}

//------------------------------------------------------------------------------
// handleResponse
//------------------------------------------------------------------------------
void castor::client::VectorResponseHandler::handleResponse
(castor::rh::Response& r)
   {
  castor::IObject *obj = r.clone();
  castor::rh::Response *resp = dynamic_cast<castor::rh::Response *>(obj);
  if (0 == resp) {
    delete obj;
    castor::exception::Exception e(EINVAL);
    e.getMessage() << "Could not cast down to Response*";
    throw e;
  }
  m_responses->push_back(resp);
}
