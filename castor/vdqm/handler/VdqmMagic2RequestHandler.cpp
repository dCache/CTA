/******************************************************************************
 *                      VdqmMagic2RequestHandler.hpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "castor/exception/NotSupported.hpp"
#include "castor/vdqm/handler/VdqmMagic2RequestHandler.hpp"
#include "h/vdqm_constants.h"

#include <iostream>


void castor::vdqm::handler::VdqmMagic2RequestHandler::handleVolPriority(
  vdqmVolPriority_t *const msg) throw (castor::exception::Exception) {
  ptr_IVdqmService->setVolPriority(msg->priority, msg->clientUID,
    msg->clientGID, msg->clientHost, msg->vid, msg->tpMode,
    msg->lifespanType);
}
