/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/query/DiskPoolQueryResponse.hpp"
#include "castor/query/DiskServerDescription.hpp"
#include "castor/query/FileSystemDescription.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::query::DiskServerDescription::DiskServerDescription() throw() :
  m_name(""),
  m_status(0),
  m_freeSpace(0),
  m_totalSpace(0),
  m_reservedSpace(0),
  m_id(0),
  m_query(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::query::DiskServerDescription::~DiskServerDescription() throw() {
  if (0 != m_query) {
    m_query->removeDiskServers(this);
  }
  for (unsigned int i = 0; i < m_fileSystemsVector.size(); i++) {
    m_fileSystemsVector[i]->setDiskServer(0);
    delete m_fileSystemsVector[i];
  }
  m_fileSystemsVector.clear();
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::query::DiskServerDescription::print(std::ostream& stream,
                                                 std::string indent,
                                                 castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# DiskServerDescription #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "name : " << m_name << std::endl;
  stream << indent << "status : " << m_status << std::endl;
  stream << indent << "freeSpace : " << m_freeSpace << std::endl;
  stream << indent << "totalSpace : " << m_totalSpace << std::endl;
  stream << indent << "reservedSpace : " << m_reservedSpace << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  stream << indent << "Query : " << std::endl;
  if (0 != m_query) {
    m_query->print(stream, indent + "  ", alreadyPrinted);
  } else {
    stream << indent << "  null" << std::endl;
  }
  {
    stream << indent << "FileSystems : " << std::endl;
    int i;
    std::vector<FileSystemDescription*>::const_iterator it;
    for (it = m_fileSystemsVector.begin(), i = 0;
         it != m_fileSystemsVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::query::DiskServerDescription::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::query::DiskServerDescription::TYPE() {
  return OBJ_DiskServerDescription;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::query::DiskServerDescription::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::query::DiskServerDescription::clone() {
  return new DiskServerDescription(*this);
}

