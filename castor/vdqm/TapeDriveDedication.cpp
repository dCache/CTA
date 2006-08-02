/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/vdqm/TapeDrive.hpp"
#include "castor/vdqm/TapeDriveDedication.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::vdqm::TapeDriveDedication::TapeDriveDedication() throw() :
  m_clientHost(""),
  m_euid(0),
  m_egid(0),
  m_vid(""),
  m_accessMode(0),
  m_startTime(0),
  m_endTime(0),
  m_reason(""),
  m_id(0),
  m_tapeDrive(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::vdqm::TapeDriveDedication::~TapeDriveDedication() throw() {
  if (0 != m_tapeDrive) {
    m_tapeDrive->removeTapeDriveDedication(this);
  }
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeDriveDedication::print(std::ostream& stream,
                                              std::string indent,
                                              castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeDriveDedication #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "clientHost : " << m_clientHost << std::endl;
  stream << indent << "euid : " << m_euid << std::endl;
  stream << indent << "egid : " << m_egid << std::endl;
  stream << indent << "vid : " << m_vid << std::endl;
  stream << indent << "accessMode : " << m_accessMode << std::endl;
  stream << indent << "startTime : " << m_startTime << std::endl;
  stream << indent << "endTime : " << m_endTime << std::endl;
  stream << indent << "reason : " << m_reason << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "TapeDrive : " << std::endl;
  if (0 != m_tapeDrive) {
    m_tapeDrive->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::vdqm::TapeDriveDedication::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::vdqm::TapeDriveDedication::TYPE() {
  return OBJ_TapeDriveDedication;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::vdqm::TapeDriveDedication::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::vdqm::TapeDriveDedication::clone() {
  return new TapeDriveDedication(*this);
}

