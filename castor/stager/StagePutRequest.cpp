// Include Files
#include "castor/Constants.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/stager/Request.hpp"
#include "castor/stager/StagePutRequest.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::stager::StagePutRequest::StagePutRequest() throw() :
  Request(),
  m_id() {
};

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::stager::StagePutRequest::~StagePutRequest() throw() {
};

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::StagePutRequest::print(std::ostream& stream,
                                            std::string indent,
                                            castor::ObjectSet& alreadyPrinted) const {
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Call print on the parent class(es)
  this->Request::print(stream, indent, alreadyPrinted);
  // Output of all members
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::stager::StagePutRequest::print() const {
  ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::stager::StagePutRequest::TYPE() {
  return OBJ_StagePutRequest;
}

//------------------------------------------------------------------------------
// setId
//------------------------------------------------------------------------------
void castor::stager::StagePutRequest::setId(u_signed64 id) {
  m_id = id;
}

//------------------------------------------------------------------------------
// id
//------------------------------------------------------------------------------
u_signed64 castor::stager::StagePutRequest::id() const {
  return m_id;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::stager::StagePutRequest::type() const {
  return TYPE();
}

