/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/stager/TapeCopy.hpp"
#include "castor/stager/TapeCopyForMigration.hpp"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::TapeCopyForMigration::TapeCopyForMigration() throw() :
  TapeCopy(),
  m_diskServer(""),
  m_mountPoint("") {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::TapeCopyForMigration::~TapeCopyForMigration() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::TapeCopyForMigration::print(std::ostream& stream,
                                                 std::string indent,
                                                 castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# TapeCopyForMigration #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->TapeCopy::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "diskServer : " << m_diskServer << std::endl;
  stream << indent << "mountPoint : " << m_mountPoint << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::TapeCopyForMigration::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::TapeCopyForMigration::TYPE() {
  return OBJ_TapeCopyForMigration;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::TapeCopyForMigration::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::TapeCopyForMigration::clone() {
  return new TapeCopyForMigration(*this);
}

