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

#include "castor/mediachanger/DismountCmd.hpp"

#include <getopt.h>
#include <iostream>
 
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
castor::mediachanger::DismountCmd::DismountCmd(
  std::istream &inStream, std::ostream &outStream, std::ostream &errStream,
  MediaChangerFacade &mc) throw():
  CmdLineTool(inStream, outStream, errStream, mc) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
castor::mediachanger::DismountCmd::~DismountCmd() throw() {
  // Do nothing
}

//------------------------------------------------------------------------------
// exceptionThrowingMain
//------------------------------------------------------------------------------
void castor::mediachanger::DismountCmd::exceptionThrowingMain(const int argc,
  char *const *const argv) {
  m_cmdLine = DismountCmdLine(argc, argv);

  // Display the usage message to standard out and exit with success if the
  // user requested help
  if(m_cmdLine.help) {
    m_out << DismountCmdLine::getUsage();
    return;
  }

  // Setup debug mode to be on or off depending on the command-line arguments
  m_debugBuf.setDebug(m_cmdLine.debug);

  m_dbg << "VID        = " << m_cmdLine.vid << std::endl;
  m_dbg << "DRIVE_SLOT = " << m_cmdLine.driveLibrarySlot.str() << std::endl;

  m_mc.dismountTape(m_cmdLine.vid, m_cmdLine.driveLibrarySlot);
}
