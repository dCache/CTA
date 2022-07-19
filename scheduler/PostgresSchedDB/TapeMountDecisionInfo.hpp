/*
 * @project      The CERN Tape Archive (CTA)
 * @copyright    Copyright © 2022 CERN
 * @license      This program is free software, distributed under the terms of the GNU General Public
 *               Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING". You can
 *               redistribute it and/or modify it under the terms of the GPL Version 3, or (at your
 *               option) any later version.
 *
 *               This program is distributed in the hope that it will be useful, but WITHOUT ANY
 *               WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *               PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 *               In applying this licence, CERN does not waive the privileges and immunities
 *               granted to it by virtue of its status as an Intergovernmental Organization or
 *               submit itself to any jurisdiction.
 */

#pragma once

#include "PostgresSchedDB.hpp"
#include "common/dataStructures/LabelFormat.hpp"
#include "common/dataStructures/MountType.hpp"

#include <memory>
#include <optional>
#include <string>
#include <cstdint>
#include <time.h>

namespace cta {

class PostgresSchedDB::TapeMountDecisionInfo : public SchedulerDatabase::TapeMountDecisionInfo {
 public:

   TapeMountDecisionInfo();

   std::unique_ptr<SchedulerDatabase::ArchiveMount> createArchiveMount(
      common::dataStructures::MountType mountType,
      const catalogue::TapeForWriting & tape, const std::string& driveName,
      const std::string & logicalLibrary, const std::string & hostName,
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,
      const uint64_t capacityInBytes,
      const std::optional<std::string> &activity,
      cta::common::dataStructures::Label::Format labelFormat) override;

   std::unique_ptr<SchedulerDatabase::RetrieveMount> createRetrieveMount(const std::string & vid,
      const std::string & tapePool, const std::string& driveName,
      const std::string& logicalLibrary, const std::string& hostName,
      const std::string& vo, const std::string& mediaType,
      const std::string& vendor,
      const uint64_t capacityInBytes,
      const std::optional<std::string> &activity,
      cta::common::dataStructures::Label::Format labelFormat) override;
};

} //namespace cta
