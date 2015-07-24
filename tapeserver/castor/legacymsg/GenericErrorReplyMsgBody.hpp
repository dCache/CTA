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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include <shift/Castor_limits.h>
#include <stdint.h>

namespace castor {
namespace legacymsg {

/**
 * The body of an error reply message.
 */
struct GenericErrorReplyMsgBody {

  /**
   * In the case of success this field should be set to the empty string and in
   * the case of an error it should contain an appropriate error message.
   */
  char errorMessage[CA_MAXLINELEN+1];

  /**
   * Constructor.
   *
   * Sets all integer member-variables to 0, and all string member-varibles to
   * the empty string.
   */
  GenericErrorReplyMsgBody() throw();

}; // struct GenericErrorReplyMsgBody

} // namespace legacymsg
} // namespace castor

