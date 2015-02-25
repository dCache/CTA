#include "ObjectOps.hpp"
#include "objectstore/cta.pb.h"

namespace cta { namespace objectstore {
  
#define MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(A) \
  template <> const serializers::ObjectType ObjectOps<serializers::A>::typeId = serializers::A##_t
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(RootEntry);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(AgentRegister);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(Agent);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(JobPool);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(RecallFIFO);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(MigrationFIFO);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(RecallJob);
  MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID(Counter);
  
#undef MAKE_CTA_OBJECTSTORE_OBJECTOPS_TYPEID
}} 