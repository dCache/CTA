/******************************************************************************
 *                castor/tape/tapeserver/daemon/DriveCatalogue.cpp
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

#include "castor/exception/Internal.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogue.hpp"
#include "castor/utils/utils.hpp"

#include <string.h>
#include <time.h>

//-----------------------------------------------------------------------------
// drvState2Str
//-----------------------------------------------------------------------------
const char *castor::tape::tapeserver::daemon::DriveCatalogue::drvState2Str(
  const DriveState state) throw() {
  switch(state) {
  case DRIVE_STATE_INIT     : return "INIT";
  case DRIVE_STATE_DOWN     : return "DOWN";
  case DRIVE_STATE_UP       : return "UP";
  case DRIVE_STATE_WAITFORK : return "WAITFORK";
  case DRIVE_STATE_WAITLABEL: return "WAITLABEL";
  case DRIVE_STATE_RUNNING  : return "RUNNING";
  case DRIVE_STATE_WAITDOWN : return "WAITDOWN";
  default                   : return "UNKNOWN";
  }
}

//-----------------------------------------------------------------------------
// mountSessionType2Str
//-----------------------------------------------------------------------------
const char *castor::tape::tapeserver::daemon::DriveCatalogue::sessionType2Str(
  const SessionType sessionType) throw() {
  switch(sessionType) {
  case SESSION_TYPE_NONE        : return "NONE";
  case SESSION_TYPE_DATATRANSFER: return "DATATRANSFER";
  case SESSION_TYPE_LABEL       : return "LABEL";
  default                       : return "UNKNOWN";
  }
}

//-----------------------------------------------------------------------------
// populateCatalogue
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::populateCatalogue(
  const utils::TpconfigLines &lines) throw(castor::exception::Exception) {

  // Enter each TPCONFIG line into the catalogue
  for(utils::TpconfigLines::const_iterator itor = lines.begin();
    itor != lines.end(); itor++) {
    enterTpconfigLine(*itor);
  }
}

//-----------------------------------------------------------------------------
// enterTpconfigLine
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::enterTpconfigLine(
  const utils::TpconfigLine &line) throw(castor::exception::Exception) {

  DriveMap::iterator itor = m_drives.find(line.unitName);

  // If the drive is not in the catalogue
  if(m_drives.end() == itor) {
    // Insert it
    DriveEntry entry;
    entry.dgn = line.dgn;
    entry.devFilename = line.devFilename;
    entry.densities.push_back(line.density);
    entry.state = str2InitialState(line.initialState);
    entry.librarySlot = line.librarySlot;
    entry.devType = line.devType;
    m_drives[line.unitName] = entry;
  // Else the drive is already in the catalogue
  } else {
    checkTpconfigLine(itor->second, line);

    // Each TPCONFIG line for a given drive specifies a new tape density
    //
    // Add the new density to the list of supported densities already stored
    // within the tape-drive catalogue
    itor->second.densities.push_back(line.density);
  }
}

//-----------------------------------------------------------------------------
// str2InitialState
//-----------------------------------------------------------------------------
castor::tape::tapeserver::daemon::DriveCatalogue::DriveState
  castor::tape::tapeserver::daemon::DriveCatalogue::str2InitialState(
  const std::string &initialState) const throw(castor::exception::Exception) {
  std::string upperCaseInitialState = initialState;
  castor::utils::toUpper(upperCaseInitialState);

  if(upperCaseInitialState == "UP") {
    return DRIVE_STATE_UP;
  } else if(upperCaseInitialState == "DOWN") {
    return DRIVE_STATE_DOWN;
  } else {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to convert initial tape-drive state:"
      " Invalid string value: value=" << initialState;
    throw ex;
  }

  return DRIVE_STATE_INIT;
}

//-----------------------------------------------------------------------------
// checkTpconfigLine
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::checkTpconfigLine(
  const DriveEntry &catalogueEntry, const utils::TpconfigLine &line)
  throw(castor::exception::Exception) {
  checkTpconfigLineDgn(catalogueEntry.dgn, line);
  checkTpconfigLineDevFilename(catalogueEntry.devFilename, line);
  checkTpconfigLineDensity(catalogueEntry.densities, line);
  checkTpconfigLineInitialState(catalogueEntry.state, line);
  checkTpconfigLineLibrarySlot(catalogueEntry.librarySlot, line);
  checkTpconfigLineDevType(catalogueEntry.devType, line);
}

//-----------------------------------------------------------------------------
// checkTpconfigLineDgn
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::checkTpconfigLineDgn(
  const std::string &catalogueDgn, const utils::TpconfigLine &line)
  throw(castor::exception::Exception) {
  if(catalogueDgn != line.dgn) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid TPCONFIG line"
      ": A tape drive can only be asscoiated with one DGN"
      ": catalogueDgn=" << catalogueDgn << " lineDgn=" << line.dgn;
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// checkTpconfigLineDevFilename
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::
  checkTpconfigLineDevFilename(const std::string &catalogueDevFilename,
  const utils::TpconfigLine &line) throw(castor::exception::Exception) {
  if(catalogueDevFilename != line.devFilename) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid TPCONFIG line"
      ": A tape drive can only have one device filename"
      ": catalogueDevFilename=" << catalogueDevFilename <<
      " lineDevFilename=" << line.devFilename;
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// checkTpconfigLineDensity
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::checkTpconfigLineDensity(
  const std::list<std::string> &catalogueDensities,
  const utils::TpconfigLine &line) throw(castor::exception::Exception) {
  for(std::list<std::string>::const_iterator itor = catalogueDensities.begin();
    itor != catalogueDensities.end(); itor++) {
    if((*itor) == line.density) {
      castor::exception::Internal ex;
      ex.getMessage() << "Invalid TPCONFIG line"
        ": A tape drive can only be associated with a tape density once"
        ": repeatedDensity=" << line.density;
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// checkTpconfigLineInitialState
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::
  checkTpconfigLineInitialState(const DriveState catalogueInitialState,
  const utils::TpconfigLine &line) throw(castor::exception::Exception) {
  if(catalogueInitialState != str2InitialState(line.initialState)) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid TPCONFIG line"
      ": A tape drive can only have one initial state"
      ": catalogueInitialState=" << catalogueInitialState <<
      " lineInitialState=" << line.initialState;
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// checkTpconfigLineLibrarySlot
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::
  checkTpconfigLineLibrarySlot(
  const std::string &catalogueLibrarySlot,
  const utils::TpconfigLine &line) throw(castor::exception::Exception) {
  if(catalogueLibrarySlot != line.librarySlot) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid TPCONFIG line"
      ": A tape drive can only have one slot within its library"
      ": catalogueLibrarySlot=" << catalogueLibrarySlot <<
      " lineLibrarySlot=" << line.librarySlot;
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// checkTpconfigLineDevType
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::
  checkTpconfigLineDevType(const std::string &catalogueDevType,
  const utils::TpconfigLine &line) throw(castor::exception::Exception) {
  if(catalogueDevType != line.devType) {
    castor::exception::Internal ex;
    ex.getMessage() << "Invalid TPCONFIG line"
      ": A tape drive can only have one device type"
      ": catalogueDevType=" << catalogueDevType <<
      " lineDevType=" << line.devType;
    throw ex;
  }
}

//-----------------------------------------------------------------------------
// getUnitName
//-----------------------------------------------------------------------------
std::string castor::tape::tapeserver::daemon::DriveCatalogue::getUnitName(
  const pid_t sessionPid) const throw(castor::exception::Exception) {

  for(DriveMap::const_iterator i = m_drives.begin(); i!=m_drives.end(); i++) {
    if(sessionPid == i->second.sessionPid) return i->first;
  }
  castor::exception::Internal ex;
  ex.getMessage() << "Failed to find the unit name of the tape drive on which "
    << "the session with process ID " << sessionPid << " is running.";
  throw ex;
}

//-----------------------------------------------------------------------------
// getUnitNames
//-----------------------------------------------------------------------------
std::list<std::string> castor::tape::tapeserver::daemon::DriveCatalogue::getUnitNames() const throw(castor::exception::Exception) {
  std::list<std::string> unitNames;

  for(DriveMap::const_iterator itor = m_drives.begin();
    itor != m_drives.end(); itor++) {
    unitNames.push_back(itor->first);
  }

  return unitNames;
}

//-----------------------------------------------------------------------------
// getUnitNames
//-----------------------------------------------------------------------------
std::list<std::string> castor::tape::tapeserver::daemon::DriveCatalogue::getUnitNames(const DriveState state) const throw(castor::exception::Exception) {
  std::list<std::string> unitNames;

  for(DriveMap::const_iterator itor = m_drives.begin();
    itor != m_drives.end(); itor++) {
    if(state == itor->second.state) {
      unitNames.push_back(itor->first);
    }
  }

  return unitNames;
}

//-----------------------------------------------------------------------------
// getDgn
//-----------------------------------------------------------------------------
const std::string &castor::tape::tapeserver::daemon::DriveCatalogue::getDgn(
  const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get DGN of tape drive " <<
      unitName << ": Unknown drive";
    throw ex;
  }

  return itor->second.dgn;
}

//-----------------------------------------------------------------------------
// getVid
//-----------------------------------------------------------------------------
const std::string &castor::tape::tapeserver::daemon::DriveCatalogue::getVid(
  const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get VID of tape drive " <<
      unitName << ": Unknown drive";
    throw ex;
  }

  return itor->second.vid;
}

//-----------------------------------------------------------------------------
// getAssignmentTime
//-----------------------------------------------------------------------------
time_t castor::tape::tapeserver::daemon::DriveCatalogue::getAssignmentTime(
  const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get the assignment time of tape drive " <<
      unitName << ": Unknown drive";
    throw ex;
  }

  return itor->second.assignment_time;
}

//-----------------------------------------------------------------------------
// getDevFilename
//-----------------------------------------------------------------------------
const std::string
  &castor::tape::tapeserver::daemon::DriveCatalogue::getDevFilename(
    const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get the device filename of tape drive " <<
      unitName << ": Unknown drive";
    throw ex;
  }

  return itor->second.devFilename;
}

//-----------------------------------------------------------------------------
// getDensities
//-----------------------------------------------------------------------------
const std::list<std::string>
  &castor::tape::tapeserver::daemon::DriveCatalogue::getDensities(
    const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get the supported taep densities of tape"
      " drive " << unitName << ": Unknown drive";
    throw ex;
  }

  return itor->second.densities;
}

//-----------------------------------------------------------------------------
// getSessionType
//-----------------------------------------------------------------------------
castor::tape::tapeserver::daemon::DriveCatalogue::SessionType
  castor::tape::tapeserver::daemon::DriveCatalogue::getSessionType(
  const pid_t sessionPid) const throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "get the type of the session with pid " << sessionPid;

  std::string unitName;
  try {
    unitName = getUnitName(sessionPid);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": " << ne.getMessage();
    throw ex;
  }

  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": unit name " <<
      unitName << " is not known";
    throw ex;
  }

  return itor->second.sessionType;
}

//-----------------------------------------------------------------------------
// getState
//-----------------------------------------------------------------------------
castor::tape::tapeserver::daemon::DriveCatalogue::DriveState
  castor::tape::tapeserver::daemon::DriveCatalogue::getState(
  const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get current state of tape drive " <<
      unitName << ": Unknown drive";
    throw ex;
  }

  return itor->second.state;
}

//-----------------------------------------------------------------------------
// getLibrarySlot
//-----------------------------------------------------------------------------
const std::string &
  castor::tape::tapeserver::daemon::DriveCatalogue::getLibrarySlot(
    const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get library slot of tape drive " << unitName
      << ": Unknown drive";
    throw ex;
  }

  return itor->second.librarySlot;
}

//-----------------------------------------------------------------------------
// getDevType
//-----------------------------------------------------------------------------
const std::string &
  castor::tape::tapeserver::daemon::DriveCatalogue::getDevType(
    const std::string &unitName) const throw(castor::exception::Exception) {
  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to get device type of tape drive " << unitName <<
      ": Unknown drive";
    throw ex;
  }

  return itor->second.devType;
}

//-----------------------------------------------------------------------------
// updateVidAssignment
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::updateVidAssignment(
  const std::string &vid, const std::string &unitName)
  throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "update the VID of tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }
  
  itor->second.vid = vid;
  itor->second.assignment_time = time(0); // set to "now"
}

//-----------------------------------------------------------------------------
// getLabelCmdConnection
//-----------------------------------------------------------------------------
int castor::tape::tapeserver::daemon::DriveCatalogue::getLabelCmdConnection(
  const pid_t sessionPid) throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "get the file-descriptor of the connection with the label command"
    " associated with the session with pid " << sessionPid;

  std::string unitName;
  try {
    unitName = getUnitName(sessionPid);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": " << ne.getMessage();
    throw ex;
  }

  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": unit name " <<
      unitName << " is not known";
    throw ex;
  }

  const DriveState driveState = itor->second.state;
  const SessionType sessionType = itor->second.sessionType;
  if(DRIVE_STATE_WAITLABEL != driveState &&
    DRIVE_STATE_RUNNING != driveState &&
    DRIVE_STATE_WAITDOWN != driveState &&
    SESSION_TYPE_LABEL != sessionType) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() <<
      ": Invalid drive state and session type: driveState=" <<
      drvState2Str(driveState) << " sessionType=" <<
      sessionType2Str(sessionType);
    throw ex;
  }

  return itor->second.labelCmdConnection;
}

//-----------------------------------------------------------------------------
// configureUp
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::configureUp(const std::string &unitName) throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "configure tape drive " << unitName << " up";

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_UP:
  case DRIVE_STATE_RUNNING:
    break;
  case DRIVE_STATE_DOWN:
    itor->second.state = DRIVE_STATE_UP;
    break;
  case DRIVE_STATE_WAITDOWN:
    itor->second.state = DRIVE_STATE_RUNNING;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// configureDown
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::configureDown(
  const std::string &unitName) throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "configure tape drive " << unitName << " down";


  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_DOWN:
    break;
  case DRIVE_STATE_UP:
    itor->second.state = DRIVE_STATE_DOWN;
    break;
  case DRIVE_STATE_RUNNING:
    itor->second.state = DRIVE_STATE_WAITDOWN;
    break;
  default:
    {
      castor::exception::Internal ex;  
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  } 
}

//-----------------------------------------------------------------------------
// receivedVdqmJob
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::receivedVdqmJob(const legacymsg::RtcpJobRqstMsgBody &job) throw(castor::exception::Exception) {
  const std::string unitName(job.driveUnit);

  std::ostringstream task;
  task << "handle vdqm job for tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_UP:
    if(std::string(job.dgn) != itor->second.dgn) {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": DGN mistatch: catalogueDgn=" << itor->second.dgn << " vdqmJobDgn=" << job.dgn;
      throw ex;
    }
    itor->second.vdqmJob = job;
    itor->second.state = DRIVE_STATE_WAITFORK;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// receivedLabelJob
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::receivedLabelJob(
  const legacymsg::TapeLabelRqstMsgBody &job, const int labelCmdConnection)
  throw(castor::exception::Exception) {
  const std::string unitName(job.drive);

  std::ostringstream task;
  task << "handle label job for tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_UP:
    if(std::string(job.dgn) != itor->second.dgn) {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": DGN mistatch: catalogueDgn=" << itor->second.dgn << " vdqmJobDgn=" << job.dgn;
      throw ex;
    }
    itor->second.labelJob = job;
    itor->second.labelCmdConnection = labelCmdConnection;
    itor->second.state = DRIVE_STATE_WAITLABEL;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// getVdqmJob
//-----------------------------------------------------------------------------
const castor::legacymsg::RtcpJobRqstMsgBody &castor::tape::tapeserver::daemon::DriveCatalogue::getVdqmJob(const std::string &unitName) const throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "get vdqm job for tape drive " << unitName;

  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_WAITFORK:
  case DRIVE_STATE_RUNNING:
  case DRIVE_STATE_WAITDOWN:
    return itor->second.vdqmJob;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// getLabelJob
//-----------------------------------------------------------------------------
const castor::legacymsg::TapeLabelRqstMsgBody &castor::tape::tapeserver::daemon::DriveCatalogue::getLabelJob(const std::string &unitName) const throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "get label job for tape drive " << unitName;

  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_WAITFORK:
  case DRIVE_STATE_RUNNING:
  case DRIVE_STATE_WAITDOWN:
    return itor->second.labelJob;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// forkedMountSession
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::forkedMountSession(const std::string &unitName, const pid_t sessionPid) throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "handle fork of mount session for tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_WAITFORK:
    itor->second.sessionPid = sessionPid;
    itor->second.sessionType = SESSION_TYPE_DATATRANSFER;
    itor->second.state = DRIVE_STATE_RUNNING;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// forkedLabelSession
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::forkedLabelSession(
  const std::string &unitName, const pid_t sessionPid)
  throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "handle fork of label session for tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_WAITLABEL:
    itor->second.sessionPid = sessionPid;
    itor->second.sessionType = SESSION_TYPE_LABEL;
    itor->second.state = DRIVE_STATE_RUNNING;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// getSessionPid
//-----------------------------------------------------------------------------
pid_t castor::tape::tapeserver::daemon::DriveCatalogue::getSessionPid(
  const std::string &unitName) const throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "get process ID of mount session for tape drive " << unitName;

  DriveMap::const_iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_RUNNING:
  case DRIVE_STATE_WAITDOWN:
    return itor->second.sessionPid;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// sessionSucceeded
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::sessionSucceeded(const pid_t sessionPid) throw(castor::exception::Exception) {
  std::string unitName;
  try {
    unitName = getUnitName(sessionPid);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() <<
      "Failed to record tape session succeeded for session with pid " <<
      sessionPid << ": " << ne.getMessage();
    throw ex;
  }

  sessionSucceeded(unitName);  
}

//-----------------------------------------------------------------------------
// sessionSucceeded
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::sessionSucceeded(const std::string &unitName) throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "record tape session succeeded for tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_RUNNING:
    itor->second.state = DRIVE_STATE_UP;
    break;
  case DRIVE_STATE_WAITDOWN:
    itor->second.state = DRIVE_STATE_DOWN;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}

//-----------------------------------------------------------------------------
// sessionFailed
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::sessionFailed(const pid_t sessionPid) throw(castor::exception::Exception) {
  std::string unitName;
  try {
    unitName = getUnitName(sessionPid);
  } catch(castor::exception::Exception &ne) {
    castor::exception::Internal ex;
    ex.getMessage() <<
      "Failed to record tape session failed for session with pid " <<
      sessionPid << ": " << ne.getMessage();
    throw ex;
  }

  sessionFailed(unitName);
}

//-----------------------------------------------------------------------------
// sessionFailed
//-----------------------------------------------------------------------------
void castor::tape::tapeserver::daemon::DriveCatalogue::sessionFailed(const std::string &unitName) throw(castor::exception::Exception) {
  std::ostringstream task;
  task << "record tape session failed for tape drive " << unitName;

  DriveMap::iterator itor = m_drives.find(unitName);
  if(m_drives.end() == itor) {
    castor::exception::Internal ex;
    ex.getMessage() << "Failed to " << task.str() << ": Unknown drive";
    throw ex;
  }

  switch(itor->second.state) {
  case DRIVE_STATE_RUNNING:
  case DRIVE_STATE_WAITDOWN:
    itor->second.state = DRIVE_STATE_DOWN;
    break;
  default:
    {
      castor::exception::Internal ex;
      ex.getMessage() << "Failed to " << task.str() <<
        ": Incompatible drive state: state=" << drvState2Str(itor->second.state);
      throw ex;
    }
  }
}
