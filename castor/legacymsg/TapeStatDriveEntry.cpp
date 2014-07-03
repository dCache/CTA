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
 * @author dkruse@cern.ch
 *****************************************************************************/

#include "castor/legacymsg/TapeStatDriveEntry.hpp"

#include <string.h>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::legacymsg::TapeStatDriveEntry::TapeStatDriveEntry() throw():
  uid(0),
  jid(0),
  up(0),
  asn(0),
  asn_time(0),
  mode(0),
  tobemounted(0),
  cfseq(0) {
  memset(dgn, '\0', sizeof(dgn));
  memset(drive, '\0', sizeof(drive));
  memset(lblcode, '\0', sizeof(lblcode));
  memset(vid, '\0', sizeof(vid));
  memset(vsn, '\0', sizeof(vsn));
}  
