#include "ObjectStoreMiddleTier.hpp"
#include "objectstore/Backend.hpp"
#include "objectstore/RootEntry.hpp"
#include "objectstore/AdminUsersList.hpp"
#include "../libs/middletier/Exception.hpp"

namespace cta {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
OStoreMiddleTierAdmin::OStoreMiddleTierAdmin(objectstore::Backend& backend):
  m_backend(backend) {
  // check that we can at least access the root entry
  objectstore::RootEntry re(m_backend);
  objectstore::ScopedSharedLock reLock(re);
  re.fetch();
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OStoreMiddleTierAdmin::~OStoreMiddleTierAdmin() throw() {
}

//------------------------------------------------------------------------------
// createAdminUser
//------------------------------------------------------------------------------
void OStoreMiddleTierAdmin::createAdminUser(
  const SecurityIdentity &requester,
  const UserIdentity &user,
  const std::string &comment) {
  // TODO: authz is needed here!
  // Find the admin users list from the root entry.
  objectstore::RootEntry re(m_backend);
  objectstore::ScopedSharedLock reLock(re);
  re.fetch();
  objectstore::AdminUsersList aul(re.getAdminUsersList(), m_backend);
  reLock.release();
  objectstore::ScopedExclusiveLock auLock(aul);
  aul.fetch();
  AdminUser au(user, requester.user, time(NULL), comment);
  aul.add(au);
  aul.commit();
}

}
