/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/rh/BasicResponse.hpp"
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::rh::BasicResponse::BasicResponse() throw() :
  Response(),
  m_id(0) {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::rh::BasicResponse::~BasicResponse() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::rh::BasicResponse::print(std::ostream& stream,
                                      std::string indent,
                                      castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# BasicResponse #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->Response::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::rh::BasicResponse::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::rh::BasicResponse::TYPE() {
  return OBJ_BasicResponse;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::rh::BasicResponse::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::rh::BasicResponse::clone() {
  return new BasicResponse(*this);
}

