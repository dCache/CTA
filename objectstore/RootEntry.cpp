#include "RootEntry.hpp"

#include <cxxabi.h>

// construtor, when the backend store exists.
// Checks the existence and correctness of the root entry
cta::objectstore::RootEntry::RootEntry(Backend & os):
  ObjectOps<serializers::RootEntry>(os, s_rootEntryName) {}

// Get the name of the agent register (or exception if not available)
std::string cta::objectstore::RootEntry::getAgentRegister() {
  // Check that the fetch was done
  if (!m_payloadInterpreted)
    throw ObjectOpsBase::NotFetched("In RootEntry::getAgentRegister: object not yet fetched")
  // If the registry is defined, return it, job done.
  if (m_payload.agentregister().size())
    return m_payload.agentregister();
  throw NotAllocatedEx("In RootEntry::getAgentRegister: agentRegister not yet allocated");
}

// Get the name of a (possibly freshly created) agent register
std::string cta::objectstore::RootEntry::allocateOrGetAgentRegister(Agent & agent) {
  // Check if the agent register exists
  try {
    return getAgentRegister(agent);
  } catch (NotAllocatedEx &) {
    // If we get here, the agent register is not created yet, so we have to do it:
    // lock the entry again, for writing
    serializers::RootEntry res;
    lockExclusiveAndRead(res);
    // If the registry is already defined, somebody was faster. We're done.
    if (res.agentregister().size()) {
      unlock();
      return res.agentregister();
    }
    // We will really create the register
    // decide on the object's name
    std::string arName (agent.nextId("agentRegister"));
    // Record the agent in the intent log
    res.add_agentregisterintentlog(arName);
    // Commit the intents
    write(res);
    // The potential object can now be garbage collected if we die from here.
    // Create the object, then lock. The name should be unique, so no race.
    serializers::Register ars;
    writeChild(arName, ars);
    // If we lived that far, we can update the root entry to point to our
    // new agent register, and remove the name from the intent log.
    res.set_agentregister(arName);
    res.mutable_agentregisterintentlog()->RemoveLast();
    write(res);
    // release the lock, and return the register name
    unlock();
    return arName;
  }
}

// Get the name of the JobPool (or exception if not available)
std::string cta::objectstore::RootEntry::getJobPool(Agent & agent) {
  // Check if the job pool exists
  serializers::RootEntry res;
  getPayloadFromObjectStoreAutoLock(res);
  // If the registry is defined, return it, job done.
  if (res.jobpool().size())
    return res.jobpool();
  throw NotAllocatedEx("In RootEntry::getJobPool: jobPool not yet allocated");
}

// Get the name of a (possibly freshly created) job pool
std::string cta::objectstore::RootEntry::allocateOrGetJobPool(Agent & agent) {
  // Check if the job pool exists
  try {
    return getJobPool(agent);
  } catch (NotAllocatedEx &) {
    // If we get here, the job pool is not created yet, so we have to do it:
    // lock the entry again, for writing
    serializers::RootEntry res;
    lockExclusiveAndRead(res);
    // If the registry is already defined, somebody was faster. We're done.
    if (res.jobpool().size()) {
      unlock();
      return res.jobpool();
    }
    // We will really create the register
    // decide on the object's name
    std::string jpName (agent.nextId("jobPool"));
    // Record the agent in the intent log
    agent.addToOwnership(jpName);
    // The potential object can now be garbage collected if we die from here.
    // Create the object, then lock. The name should be unique, so no race.
    serializers::JobPool jps;
    jps.set_migration("");
    jps.set_recall("");
    jps.set_recallcounter("");
    writeChild(jpName, jps);
    // If we lived that far, we can update the root entry to point to our
    // new agent register, and remove the name from the intent log.
    res.set_jobpool(jpName);
    write(res);
    // release the lock, and return the register name
    unlock();
    // Clear intent log
    agent.removeFromOwnership(jpName);
    return jpName;
  }
}

// Dump the root entry
std::string cta::objectstore::RootEntry::dump (Agent & agent) {
  std::stringstream ret;
  serializers::RootEntry res;
  getPayloadFromObjectStoreAutoLock(res);
  ret << "<<<< Root entry dump start" << std::endl;
  if (res.has_agentregister()) ret << "agentRegister=" << res.agentregister() << std::endl;
  ret << "agentRegister Intent Log size=" << res.agentregisterintentlog_size() << std::endl;
  for (int i=0; i<res.agentregisterintentlog_size(); i++) {
    ret << "agentRegisterIntentLog=" << res.agentregisterintentlog(i) << std::endl;
  }
  if (res.has_jobpool()) ret << "jobPool=" << res.jobpool() << std::endl;
  if (res.has_driveregister()) ret << "driveRegister=" << res.driveregister() << std::endl;
  if (res.has_taperegister()) ret << "tapeRegister=" << res.taperegister() << std::endl;
  ret << ">>>> Root entry dump start" << std::endl;
  return ret.str();
}

const std::string cta::objectstore::RootEntry::s_rootEntryName("root");