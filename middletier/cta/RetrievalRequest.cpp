#include "cta/RetrievalRequest.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::RetrievalRequest::RetrievalRequest():
  m_priority(0) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::RetrievalRequest::~RetrievalRequest() throw() {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::RetrievalRequest::RetrievalRequest(const uint64_t priority,
  const std::string &id, const SecurityIdentity &user,
  const time_t creationTime):
  UserRequest(id, user, creationTime),
  m_priority(priority) {
}

//------------------------------------------------------------------------------
// getPriority
//------------------------------------------------------------------------------
uint64_t cta::RetrievalRequest::getPriority() const throw() {
  return m_priority;
}
