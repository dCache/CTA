/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          Text formatter for CTA Admin command tool
 * @description    CTA Admin command using Google Protocol Buffers and XRootD SSI transport
 * @copyright      Copyright 2017 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <iomanip>
#include <cmdline/CtaAdminTextFormatter.hpp>
#include <common/dataStructures/DriveStatus.hpp>
#include <common/dataStructures/MountType.hpp>

namespace cta {
namespace admin {

/**
 ** Generic utility methods
 **/

std::string TextFormatter::doubleToStr(double value, char unit) {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << value << unit;
  return ss.str();
}


std::string TextFormatter::timeToStr(const time_t &unixtime) {
  struct tm timeTm;
  localtime_r(&unixtime, &timeTm);

  char timeStr[17]; // YYYY-MM-DD HH:MM
  strftime(timeStr, 17, "%F %R", &timeTm);

  return timeStr;
}


std::string TextFormatter::dataSizeToStr(uint64_t value) {
  const std::vector<char> suffix = { 'K', 'M', 'G', 'T', 'P', 'E' };

  // Simple case, values less than 1000 bytes don't take a suffix
  if(value < 1000) return std::to_string(value);

  // Find the correct scaling, starting at 1 KB and working up. I'm assuming we won't have zettabytes
  // or yottabytes of data in a tapepool anytime soon.
  int unit;
  uint64_t divisor;
  for(unit = 0, divisor = 1000; unit < 6 && value >= divisor*1000; divisor *= 1000, ++unit) ;

  // Convert to format like "3.1G"
  double val_d = static_cast<double>(value) / static_cast<double>(divisor);
  return doubleToStr(val_d, suffix[unit]);
}


void TextFormatter::flush() {
  if(m_outputBuffer.empty()) return;

  // Check if first line is a header requiring special formatting
  bool is_header = false;
  if(m_outputBuffer.front().size() == 1 && m_outputBuffer.front().front() == "HEADER") {
    is_header = true;
    m_outputBuffer.erase(m_outputBuffer.begin());
    if(m_outputBuffer.empty()) return;
  }

  auto numCols = m_outputBuffer.front().size();
  std::vector<unsigned int> colSize(numCols);

  // Calculate column widths
  for(auto &l : m_outputBuffer) {
    if(l.size() != numCols) throw std::runtime_error("TextFormatter::flush(): incorrect number of columns");
    for(size_t c = 0; c < l.size(); ++c) {
      if(colSize.at(c) < l.at(c).size()) colSize[c] = l.at(c).size();
    }
  }

  // Output columns
  for(auto &l : m_outputBuffer) {
    if(is_header) { std::cout << TEXT_RED; }
    for(size_t c = 0; c < l.size(); ++c) {
      std::cout << std::setfill(' ')
                << std::setw(colSize.at(c)+1)
                << std::right
                << (l.at(c).empty() ? "-" : l.at(c))
                << ' ';
    }
    if(is_header) { std::cout << TEXT_NORMAL; is_header = false; }
    std::cout << std::endl;
  }

  // Empty buffer
  m_outputBuffer.clear();
}


/**
 ** Output for specific commands
 **/

void TextFormatter::printAdminLsHeader() {
  push_back("HEADER");
  push_back(
    "user",
    "c.user",
    "c.host", 
    "c.time",
    "m.user",
    "m.host",
    "m.time",
    "comment"
  );
}

void TextFormatter::print(const AdminLsItem &adls_item) {
  push_back(
    adls_item.user(),
    adls_item.creation_log().username(),
    adls_item.creation_log().host(),
    timeToStr(adls_item.creation_log().time()),
    adls_item.last_modification_log().username(),
    adls_item.last_modification_log().host(),
    timeToStr(adls_item.last_modification_log().time()),
    adls_item.comment()
  );
}

void TextFormatter::printArchiveFileLsHeader() {
  push_back("HEADER");
  push_back(
    "archive id",
    "copy no",
    "vid",
    "fseq",
    "block id",
    "instance",
    "disk id",
    "size",
    "checksum type",
    "checksum value",
    "storage class",
    "owner",
    "group",
    "creation time",
    "sc vid", // superceded
    "sc fseq",
    "path"
  );
}

void TextFormatter::print(const ArchiveFileLsItem &afls_item) {
  push_back(
    afls_item.af().archive_id(),
    afls_item.copy_nb(),
    afls_item.tf().vid(),
    afls_item.tf().f_seq(),
    afls_item.tf().block_id(),
    afls_item.af().disk_instance(),
    afls_item.af().disk_id(),
    dataSizeToStr(afls_item.af().size()),
    afls_item.af().cs().type(),
    afls_item.af().cs().value(),
    afls_item.af().storage_class(),
    afls_item.af().df().owner(),
    afls_item.af().df().group(),
    timeToStr(afls_item.af().creation_time()),
    afls_item.tf().superseded_by_vid(),
    afls_item.tf().superseded_by_f_seq(),
    afls_item.af().df().path()
  );
}

void TextFormatter::printArchiveFileLsSummaryHeader() {
  push_back("HEADER");
  push_back(
    "total files",
    "total size"
  );
}

void TextFormatter::print(const ArchiveFileLsSummary &afls_summary)
{
  push_back(
    afls_summary.total_files(),
    dataSizeToStr(afls_summary.total_size())
  );
}

void TextFormatter::printArchiveRouteLsHeader() {
  push_back("HEADER");
  push_back(
    "instance",
    "storage class",
    "copy number",
    "tapepool",
    "c.user",
    "c.host",
    "c.time",
    "m.user",
    "m.host",
    "m.time",
    "comment"
  );
}

void TextFormatter::print(const ArchiveRouteLsItem &arls_item) {
  push_back(
    arls_item.instance(),
    arls_item.storage_class(),
    arls_item.copy_number(),
    arls_item.tapepool(),
    arls_item.creation_log().username(),
    arls_item.creation_log().host(),
    timeToStr(arls_item.creation_log().time()),
    arls_item.last_modification_log().username(),
    arls_item.last_modification_log().host(),
    timeToStr(arls_item.last_modification_log().time()),
    arls_item.comment()
  );
}


void TextFormatter::printDriveLsHeader() {
  push_back("HEADER");
  push_back(
    "library",
    "drive",
    "host",
    "desired",
    "request",
    "status",
    "since",
    "vid",
    "tapepool",
    "files",
    "data",
    "MB/s",
    "session",
    "priority",
    "activity",
    "age"
  );
}

void TextFormatter::print(const DriveLsItem &drls_item)
{
  using namespace cta::common::dataStructures;

  const int DRIVE_TIMEOUT = 600; // Time after which a drive will be marked as STALE

  std::string mountType;
  std::string driveStatus;
  std::string driveStatusSince;
  std::string filesTransferredInSession;
  std::string bytesTransferredInSession;
  std::string latestBandwidth;
  std::string sessionId;
  std::string timeSinceLastUpdate;

  mountType   = toString(static_cast<MountType>(drls_item.mount_type()));
  driveStatus = toString(static_cast<DriveStatus>(drls_item.drive_status()));

  if(drls_item.drive_status() != DriveStatus::Unknown) {
    driveStatusSince = std::to_string(drls_item.drive_status_since());
  }

  if(drls_item.drive_status() == DriveStatus::Transferring) {
    filesTransferredInSession = std::to_string(drls_item.files_transferred_in_session());
    bytesTransferredInSession = dataSizeToStr(drls_item.bytes_transferred_in_session());
    latestBandwidth = std::to_string(drls_item.latest_bandwidth());
  }

  if(drls_item.drive_status() != DriveStatus::Up &&
     drls_item.drive_status() != DriveStatus::Down &&
     drls_item.drive_status() != DriveStatus::Unknown) {
    sessionId = std::to_string(drls_item.session_id());
  }

  timeSinceLastUpdate = std::to_string(drls_item.time_since_last_update()) +
    (drls_item.time_since_last_update() > DRIVE_TIMEOUT ? " [STALE]" : "");

  push_back(
    drls_item.logical_library(),
    drls_item.drive_name(),
    drls_item.host(),
    drls_item.desired_drive_state() == DriveLsItem::UP ? "Up" : "Down",
    mountType,
    driveStatus,
    driveStatusSince,
    drls_item.vid(),
    drls_item.tapepool(),
    filesTransferredInSession,
    bytesTransferredInSession,
    latestBandwidth,
    sessionId,
    drls_item.current_priority(),
    drls_item.current_activity(),
    timeSinceLastUpdate
  );
}

void TextFormatter::printFailedRequestLsHeader() {
  push_back("HEADER");
  push_back(
    "request type",
    "copy no",
    "tapepool/vid",
    "requester",
    "group",
    "path"
  );
}

void TextFormatter::print(const FailedRequestLsItem &frls_item) {
  std::string request_type;
  std::string tapepool_vid;

  switch(frls_item.request_type()) {
    case admin::RequestType::ARCHIVE_REQUEST:
      request_type = "archive";
      tapepool_vid = frls_item.tapepool();
      break;
    case admin::RequestType::RETRIEVE_REQUEST:
      request_type = "retrieve";
      tapepool_vid = frls_item.tf().vid();
      break;
    default:
      throw std::runtime_error("Unrecognised request type: " + std::to_string(frls_item.request_type()));
  }

  push_back(
    request_type,
    frls_item.copy_nb(),
    tapepool_vid,
    frls_item.requester().username(),
    frls_item.requester().groupname(),
    frls_item.af().df().path()
  );

  // Note: failure log messages are available in frls_item.failurelogs(). These are not currently
  //       displayed in the text output, only in JSON.
}

void TextFormatter::printFailedRequestLsSummaryHeader() {
  push_back("HEADER");
  push_back(
    "request type",
    "total files",
    "total size"
  );
}

void TextFormatter::print(const FailedRequestLsSummary &frls_summary) {
  std::string request_type =
    frls_summary.request_type() == RequestType::ARCHIVE_REQUEST  ? "archive" :
    frls_summary.request_type() == RequestType::RETRIEVE_REQUEST ? "retrieve" : "total";

  push_back(
    request_type,
    frls_summary.total_files(),
    dataSizeToStr(frls_summary.total_size())
  );
}

void TextFormatter::printListPendingArchivesHeader() {
  push_back("HEADER");
  push_back(
    "tapepool",
    "archive id",
    "storage class",
    "copy no",
    "disk id",
    "instance",
    "checksum type",
    "checksum value",
    "size",
    "user",
    "group",
    "path"
  );
}

void TextFormatter::print(const ListPendingArchivesItem &lpa_item) {
  push_back(
    lpa_item.tapepool(),
    lpa_item.af().archive_id(),
    lpa_item.af().storage_class(),
    lpa_item.copy_nb(),
    lpa_item.af().disk_id(),
    lpa_item.af().disk_instance(),
    lpa_item.af().cs().type(),
    lpa_item.af().cs().value(),
    dataSizeToStr(lpa_item.af().size()),
    lpa_item.af().df().owner(),
    lpa_item.af().df().group(),
    lpa_item.af().df().path()
  );
}

void TextFormatter::printListPendingArchivesSummaryHeader() {
  push_back("HEADER");
  push_back(
    "tapepool",
    "total files",
    "total size"
  );
}

void TextFormatter::print(const ListPendingArchivesSummary &lpa_summary) {
  push_back(
    lpa_summary.tapepool(),
    lpa_summary.total_files(),
    dataSizeToStr(lpa_summary.total_size())
  );
}

void TextFormatter::printListPendingRetrievesHeader() {
  push_back("HEADER");
  push_back(
    "vid",
    "archive id",
    "copy no",
    "fseq",
    "block id",
    "size",
    "user",
    "group",
    "path"
  );
}

void TextFormatter::print(const ListPendingRetrievesItem &lpr_item) {
  push_back(
    lpr_item.tf().vid(),
    lpr_item.af().archive_id(),
    lpr_item.copy_nb(),
    lpr_item.tf().f_seq(),
    lpr_item.tf().block_id(),
    dataSizeToStr(lpr_item.af().size()),
    lpr_item.af().df().owner(),
    lpr_item.af().df().group(),
    lpr_item.af().df().path()
  );
}

void TextFormatter::printListPendingRetrievesSummaryHeader() {
  push_back("HEADER");
  push_back(
    "vid",
    "total files",
    "total size"
  );
}

void TextFormatter::print(const ListPendingRetrievesSummary &lpr_summary) {
  push_back(
    lpr_summary.vid(),
    lpr_summary.total_files(),
    dataSizeToStr(lpr_summary.total_size())
  );
}

void TextFormatter::printRepackLsHeader() {
  push_back("HEADER");
  push_back(
    "vid",
    "repackBufferURL",
    "userProvidedFiles",
    "totalFilesToRetrieve",
    "totalBytesToRetrieve",
    "totalFilesToArchive",
    "totalBytesToArchive",
    "retrievedFiles",
    "archivedFiles",
    "failedToRetrieveFiles",
    "failedToRetrieveBytes",
    "failedToArchiveFiles",
    "failedToArchiveBytes",
    "lastExpandedFSeq",
    "status"
  );
}

void TextFormatter::print(const RepackLsItem &rels_item) {
  push_back(
   rels_item.vid(),
   rels_item.repack_buffer_url(),
   rels_item.user_provided_files(),
   rels_item.total_files_to_retrieve(),
   dataSizeToStr(rels_item.total_bytes_to_retrieve()),
   rels_item.total_files_to_archive(),
   dataSizeToStr(rels_item.total_bytes_to_archive()),
   rels_item.retrieved_files(),
   rels_item.archived_files(),
   rels_item.failed_to_retrieve_files(),
   dataSizeToStr(rels_item.failed_to_retrieve_bytes()),
   rels_item.failed_to_archive_files(),
   dataSizeToStr(rels_item.failed_to_retrieve_bytes()),
   rels_item.last_expanded_fseq(),
   rels_item.status()
  );
}

void TextFormatter::printTapeLsHeader() {
  push_back("HEADER");
  push_back(
    "vid",
    "media type",
    "vendor",
    "logical library",
    "tapepool",
    "vo",
    "encryption key",
    "capacity",
    "occupancy",
    "last fseq",
    "full",
    "disabled",
    "label drive",
    "label time",
    "last w drive",
    "last w time",
    "last r drive",
    "last r time",
    "c.user",
    "c.host",
    "c.time",
    "m.user",
    "m.host",
    "m.time"
  );
}

void TextFormatter::print(const TapeLsItem &tals_item) {
  push_back(
    tals_item.vid(),
    tals_item.media_type(),
    tals_item.vendor(),
    tals_item.logical_library(),
    tals_item.tapepool(),
    tals_item.vo(),
    tals_item.encryption_key(),
    dataSizeToStr(tals_item.capacity()),
    dataSizeToStr(tals_item.occupancy()),
    tals_item.last_fseq(),
    tals_item.full(),
    tals_item.disabled(),
    tals_item.has_label_log()        ? tals_item.label_log().drive()                  : "",
    tals_item.has_label_log()        ? timeToStr(tals_item.label_log().time())        : "",
    tals_item.has_last_written_log() ? tals_item.last_written_log().drive()           : "",
    tals_item.has_last_written_log() ? timeToStr(tals_item.last_written_log().time()) : "",
    tals_item.has_last_read_log()    ? tals_item.last_read_log().drive()              : "",
    tals_item.has_last_read_log()    ? timeToStr(tals_item.last_read_log().time())    : "",
    tals_item.creation_log().username(),
    tals_item.creation_log().host(),
    timeToStr(tals_item.creation_log().time()),
    tals_item.last_modification_log().username(),
    tals_item.last_modification_log().host(),
    timeToStr(tals_item.last_modification_log().time())
  );
}

void TextFormatter::printTapePoolLsHeader() {
  push_back("HEADER");
  push_back(
    "name",
    "vo",
    "#tapes",
    "#partial",
    "#phys files",
    "size",
    "used",
    "avail",
    "use%",
    "encrypt",
    "supply",
    "c.user",
    "c.host",
    "c.time",
    "m.user",
    "m.host",
    "m.time",
    "comment"
  );
}

void TextFormatter::print(const TapePoolLsItem &tpls_item)
{
  uint64_t avail = tpls_item.capacity_bytes() > tpls_item.data_bytes() ?
    tpls_item.capacity_bytes()-tpls_item.data_bytes() : 0; 

  double use_percent = tpls_item.capacity_bytes() > 0 ?
    (static_cast<double>(tpls_item.data_bytes())/static_cast<double>(tpls_item.capacity_bytes()))*100.0 : 0.0;

  push_back(
    tpls_item.name(),
    tpls_item.vo(),
    tpls_item.num_tapes(),
    tpls_item.num_partial_tapes(),
    tpls_item.num_physical_files(),
    dataSizeToStr(tpls_item.capacity_bytes()),
    dataSizeToStr(tpls_item.data_bytes()),
    dataSizeToStr(avail),
    doubleToStr(use_percent, '%'),
    tpls_item.encrypt(),
    tpls_item.supply(),
    tpls_item.created().username(),
    tpls_item.created().host(),
    timeToStr(tpls_item.created().time()),
    tpls_item.modified().username(),
    tpls_item.modified().host(),
    timeToStr(tpls_item.modified().time()),
    tpls_item.comment()
  );
}

}}
