/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/stager/MoverCloseRequest.hpp"
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::MoverCloseRequest::MoverCloseRequest() throw() :
  Request(),
  m_subReqId(0),
  m_fileSize(0),
  m_id(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::MoverCloseRequest::~MoverCloseRequest() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::MoverCloseRequest::print(std::ostream& stream,
                                              std::string indent,
                                              castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# MoverCloseRequest #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->Request::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "subReqId : " << m_subReqId << std::endl;
  stream << indent << "fileSize : " << m_fileSize << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::MoverCloseRequest::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::MoverCloseRequest::TYPE() {
  return OBJ_MoverCloseRequest;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::MoverCloseRequest::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::stager::MoverCloseRequest::clone() {
  return new MoverCloseRequest(*this);
}

