#include "cta/UserRequest.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::UserRequest::UserRequest():
  m_id(0),
  m_creationTime(time(NULL)) {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::UserRequest::UserRequest(const std::string &id,
  const SecurityIdentity &user):
  m_id(id),
  m_user(user),
  m_creationTime(time(NULL)) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::UserRequest::~UserRequest() throw() {
}

//------------------------------------------------------------------------------
// getId
//------------------------------------------------------------------------------
const std::string &cta::UserRequest::getId() const throw() {
  return m_id;
}

//------------------------------------------------------------------------------
// getUser
//------------------------------------------------------------------------------
const cta::SecurityIdentity &cta::UserRequest::getUser() const throw() {
  return m_user;
}

//------------------------------------------------------------------------------
// getCreationTime
//------------------------------------------------------------------------------
time_t cta::UserRequest::getCreationTime() const throw() {
  return m_creationTime;
}
