#include "StorageClass.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::StorageClass::StorageClass():
  m_nbCopies(0),
  m_creationTime(time(NULL)) {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::StorageClass::StorageClass(
  const std::string &name,
  const uint16_t nbCopies,
  const UserIdentity &creator,
  const time_t creationTime,
  const std::string &comment):
  m_name(name),
  m_nbCopies(nbCopies),
  m_creationTime(creationTime),
  m_creator(creator),
  m_comment(comment) {
}

//------------------------------------------------------------------------------
// getName
//------------------------------------------------------------------------------
const std::string &cta::StorageClass::getName() const throw() {
  return m_name;
}

//------------------------------------------------------------------------------
// getNbCopies
//------------------------------------------------------------------------------
uint16_t cta::StorageClass::getNbCopies() const throw() {
  return m_nbCopies;
}

//------------------------------------------------------------------------------
// getCreationTime
//------------------------------------------------------------------------------
time_t cta::StorageClass::getCreationTime() const throw() {
  return m_creationTime;
}

//------------------------------------------------------------------------------
// getCreator
//------------------------------------------------------------------------------
const cta::UserIdentity &cta::StorageClass::getCreator()
  const throw() {
  return m_creator;
}

//------------------------------------------------------------------------------
// getComment
//------------------------------------------------------------------------------
const std::string &cta::StorageClass::getComment() const throw() {
  return m_comment;
}
