/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ObjectOps.hpp"
#include "objectstore/cta.pb.h"
#include "common/dataStructures/RepackInfo.hpp"
#include "common/log/TimingList.hpp"
#include "common/Timer.hpp"

namespace cta { namespace objectstore {

class Agent;
class GenericObject;

class RepackRequest: public ObjectOps<serializers::RepackRequest, serializers::RepackRequest_t> {
public:
  RepackRequest(const std::string & address, Backend & os);
  RepackRequest(Backend & os);
  RepackRequest(GenericObject & go);
  void initialize();
  
  // Parameters interface
  void setVid(const std::string & vid);
  void setType(common::dataStructures::RepackInfo::Type repackType);
  void setStatus(common::dataStructures::RepackInfo::Status repackStatus);
  common::dataStructures::RepackInfo getInfo();
  
  // Sub request management
  struct SubrequestInfo {
    std::string address;
    uint64_t fSeq;
    bool subrequestDeleted;  ///< A boolean set to true before deleting a request. Covers the race between request creation recording and request
    typedef std::set<SubrequestInfo> set;
    bool operator< (const SubrequestInfo & o) const { return fSeq < o.fSeq; }
  };
  /** 
   * Provide a list of addresses for a set or fSeqs. For expansion of repack requests.
   * The addresses could be provided from the repack request if previously recorded, or
   * generated if not. The repack request should then be committed (not done here) before the
   * sub requests are actually created. Sub requests could also be already present, and this
   * would not be an error case (the previous process doing the expansion managed to create them),
   * yet not update the object to reflect the last fSeq created.
   * This function implicitly records the information it generates (commit up t the caller);
   */
  SubrequestInfo::set getOrPrepareSubrequestInfo (std::set<uint32_t> fSeqs, AgentReference & agentRef);
private:
  struct RepackSubRequestPointer {
    std::string address;
    uint64_t fSeq;
    bool retrieveAccounted;
    bool archiveAccounted;
    bool failureAccounted;
    bool subrequestDeleted;
    typedef std::map<uint64_t, RepackSubRequestPointer> Map;
    void serialize (serializers::RepackSubRequestPointer & rsrp);
    void deserialize (const serializers::RepackSubRequestPointer & rsrp);
  };
  
public:
  /// Set the last fully created sub-requests address
  void setLastExpandedFSeq(uint64_t lastExpandedFSeq);
  uint64_t getLastExpandedFSeq();
  
  struct SubrequestStatistics {
    uint64_t fSeq;
    uint64_t files = 1;
    uint64_t bytes;
    typedef std::list<SubrequestStatistics> List;
    bool operator< (const SubrequestStatistics & o) const { return fSeq < o.fSeq; }
  };
  void reportRetriveSuccesses (SubrequestStatistics::List & retrieveSuccesses);
  void reportRetriveFailures (SubrequestStatistics::List & retrieveFailures);
  void reportArchiveSuccesses (SubrequestStatistics::List & archiveSuccesses);
  void reportArchiveFailures (SubrequestStatistics::List & archiveFailures);
  void reportSubRequestsForDeletion (std::list<uint64_t>& fSeqs);
  
  void garbageCollect(const std::string &presumedOwner, AgentReference & agentReference, log::LogContext & lc,
    cta::catalogue::Catalogue & catalogue) override;
    
  std::string dump();
  
  // An asynchronous request ownership updating class.
  class AsyncOwnerAndStatusUpdater {
    friend class RepackRequest;
  public:
    void wait();
    common::dataStructures::RepackInfo getInfo();
  private:
    std::function<std::string(const std::string &)> m_updaterCallback;
    std::unique_ptr<Backend::AsyncUpdater> m_backendUpdater;
    log::TimingList m_timingReport;
    utils::Timer m_timer;
    common::dataStructures::RepackInfo m_repackInfo;
  };
  // An owner updater factory. The owner MUST be previousOwner for the update to be executed.
  AsyncOwnerAndStatusUpdater *asyncUpdateOwnerAndStatus(const std::string &owner, const std::string &previousOwner,
      cta::optional<serializers::RepackRequestStatus> newStatus);
};

}} // namespace cta::objectstore