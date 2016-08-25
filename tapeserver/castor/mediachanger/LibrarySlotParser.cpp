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

#include "castor/mediachanger/AcsLibrarySlot.hpp"
#include "castor/mediachanger/LibrarySlotParser.hpp"
#include "castor/mediachanger/ManualLibrarySlot.hpp"
#include "castor/mediachanger/ScsiLibrarySlot.hpp"
#include "castor/utils/utils.hpp"

#include <sstream>
#include <vector>

//------------------------------------------------------------------------------
// parse
//------------------------------------------------------------------------------
castor::mediachanger::LibrarySlot *castor::mediachanger::LibrarySlotParser::
  parse(const std::string &str) {
  try {
    // Parse the string representation in two steps, first parsing the beginning
    // to get the library type and then the rst to get the details.  This two
    // step strategy gives the end user more detailed syntax errors wherei
    // necessary
    const TapeLibraryType libraryType = getLibrarySlotType(str);
    return parse(libraryType, str);
  } catch(cta::exception::Exception &ne) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to parse library slot from string"
      " representation: " << ne.getMessage().str();
    throw ex;
  }
}

//------------------------------------------------------------------------------
// getLibrarySlotType
//------------------------------------------------------------------------------
castor::mediachanger::TapeLibraryType castor::mediachanger::LibrarySlotParser::
  getLibrarySlotType(const std::string &str) {
  if(isAcs(str))    return TAPE_LIBRARY_TYPE_ACS;
  if(isManual(str)) return TAPE_LIBRARY_TYPE_MANUAL;
  if(isScsi(str))   return TAPE_LIBRARY_TYPE_SCSI;

  cta::exception::Exception ex;
  ex.getMessage() << "Cannot determine library slot type: str=" << str;
  throw ex;
}

//------------------------------------------------------------------------------
// isAcs
//------------------------------------------------------------------------------
bool castor::mediachanger::LibrarySlotParser::isAcs(const std::string &str)
  throw() {
  return 0 == str.find("acs");
}

//------------------------------------------------------------------------------
// isManual
//------------------------------------------------------------------------------
bool castor::mediachanger::LibrarySlotParser::isManual(const std::string &str)
  throw() {
  return 0 == str.find("manual");
}

//------------------------------------------------------------------------------
// isScsi
//------------------------------------------------------------------------------
bool castor::mediachanger::LibrarySlotParser::isScsi(const std::string &str)
  throw() {
  return 0 == str.find("smc");
}

//------------------------------------------------------------------------------
// parse
//------------------------------------------------------------------------------
castor::mediachanger::LibrarySlot *castor::mediachanger::LibrarySlotParser::
  parse(const TapeLibraryType libraryType, const std::string &str) {

  switch(libraryType) {
  case TAPE_LIBRARY_TYPE_ACS:    return parseAcsLibrarySlot(str);
  case TAPE_LIBRARY_TYPE_MANUAL: return parseManualLibrarySlot(str);
  case TAPE_LIBRARY_TYPE_SCSI:   return parseScsiLibrarySlot(str);
  default:
    {
      // Should never get here
      cta::exception::Exception ex;
      ex.getMessage() << "Unknown tape library type: libraryType=" <<
        libraryType;
      throw ex;
    }
  }
}

//------------------------------------------------------------------------------
// parseAcsLibrarySlot
//------------------------------------------------------------------------------
castor::mediachanger::AcsLibrarySlot *castor::mediachanger::LibrarySlotParser::
  parseAcsLibrarySlot(const std::string &str) {
  const std::string errMsg("Failed to construct AcsLibrarySlot");
  std::vector<std::string> components;
  castor::utils::splitString(str, ',', components);
  if(4 != components.size()) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": Invalid number of components"
      ": expected=4, actual=" << components.size();
    throw ex;
  }
  
  // check for acs in the beginning 
  const std::string &acsWithACS_NUMBERStr = components[0];
  if(0 != acsWithACS_NUMBERStr.find("acs")) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": Invalid tape library-slot format"
      ": expected=acsACS_NUMBER, actual=" << acsWithACS_NUMBERStr;
    throw ex;   
  }
  
  const std::string::size_type indexOfACS_NUMBER = 3;  // skip acs
  const std::string &acsStr = acsWithACS_NUMBERStr.substr(indexOfACS_NUMBER);
  const std::string &lsmStr = components[1];
  const std::string &panStr = components[2];
  const std::string &drvStr = components[3];

  // Each of the 4 components must be between 1 and than 3 characters long
  if(1 > acsStr.length() ||  3 < acsStr.length()) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": Invalid ACS_NUMBER string length"
      ": expected=1..3, actual=" << acsStr.length();
    throw ex;
  }
  if(1 > lsmStr.length() || 3 < lsmStr.length()) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": Invalid LSM_NUMBER string length"
      ": expected=1..3, actual=" << lsmStr.length();
    throw ex;
  }
  if(1 > panStr.length() || 3 < panStr.length()) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": Invalid PANEL_NUMBER string length"
      ": expected=1..3, actual=" << panStr.length();
    throw ex;
  }
  if(1 > drvStr.length() || 3 < drvStr.length()) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": Invalid TRANSPORT_NUMBER string length"
      ": expected=1..3, actual=" << drvStr.length();
    throw ex;
  }

  if(!utils::isValidUInt(acsStr)) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": ACS_NUMBER must be an unsigned integer:"
      " value=" << acsStr;
    throw ex;
  }
  if(!utils::isValidUInt(lsmStr)) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": LSM_NUMBER must be an unsigned integer:"
      " value=" << lsmStr;
    throw ex;
  }
  if(!utils::isValidUInt(panStr)) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": PANEL_NUMBER must be an unsigned integer:"
      " value=" << panStr;
    throw ex;
  }
  if(!utils::isValidUInt(drvStr)) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() << errMsg << ": TRANSPORT_NUMBER must be an unsigned"
      " integer: value=" << drvStr;
    throw ex;
  }

  return new AcsLibrarySlot(
    atoi(acsStr.c_str()),
    atoi(lsmStr.c_str()),
    atoi(panStr.c_str()),
    atoi(drvStr.c_str()));
}

//------------------------------------------------------------------------------
// parseManualLibrarySlot
//------------------------------------------------------------------------------
castor::mediachanger::ManualLibrarySlot *castor::mediachanger::
  LibrarySlotParser::parseManualLibrarySlot(const std::string &str) {
  return new ManualLibrarySlot(str);
}

//------------------------------------------------------------------------------
// parseScsiLibrarySlot
//------------------------------------------------------------------------------
castor::mediachanger::ScsiLibrarySlot *castor::mediachanger::
  LibrarySlotParser::parseScsiLibrarySlot(const std::string &str) {
  if(str.find("smc")) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to construct ScsiLibrarySlot"
      ": Library slot must start with smc: slot=" << str;
    throw ex;
  }

  const size_t drvOrdStrLen = str.length() - 3; // length of "smc" is 3
  const std::string drvOrdStr = str.substr(3, drvOrdStrLen);
  if(drvOrdStr.empty()) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to construct ScsiLibrarySlot"
      ": Missing drive ordinal: slot=" << str;
    throw ex;
  }

  if(!utils::isValidUInt(drvOrdStr)) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to construct ScsiLibrarySlot: Drive ordinal " <<
      drvOrdStr << " is not a valid unsigned integer: slot=" << str;
    throw ex;
  }

  const uint16_t drvOrd = atoi(drvOrdStr.c_str());
  return new ScsiLibrarySlot(drvOrd);
}
