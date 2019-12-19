/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          CTA Frontend Tape File Ls stream implementation
 * @copyright      Copyright 2019 CERN
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

#pragma once

#include <xroot_plugins/XrdCtaStream.hpp>
#include <xroot_plugins/XrdSsiCtaRequestMessage.hpp>
#include <xroot_plugins/Namespace.hpp>
#include <common/checksum/ChecksumBlobSerDeser.hpp>


namespace cta { namespace xrd {

/*!
 * Stream object which implements "tapefile ls" command.
 */
class TapeFileLsStream : public XrdCtaStream
{
public:
  TapeFileLsStream(const RequestMessage &requestMsg, cta::catalogue::Catalogue &catalogue, cta::Scheduler &scheduler);

private:
  /*!
   * Can we close the stream?
   */
  virtual bool isDone() const {
#if 0
    return m_isSummary ? m_isSummaryDone : !m_tapeFileItor.hasMore();
#endif
    return !m_tapeFileItor.hasMore();
  }

  /*!
   * Fill the buffer
   */
  virtual int fillBuffer(XrdSsiPb::OStreamBuffer<Data> *streambuf);

  catalogue::ArchiveFileItor              m_tapeFileItor;                 //!< Iterator across files which have been archived
  Namespace                               m_namespace;
#if 0
  bool                                    m_isSummary;                       //!< Full listing or short summary?
  bool                                    m_isSummaryDone;                   //!< Summary has been sent
#endif

  static constexpr const char* const LOG_SUFFIX  = "TapeFileLsStream";       //!< Identifier for log messages
};


TapeFileLsStream::TapeFileLsStream(const RequestMessage &requestMsg,
  cta::catalogue::Catalogue &catalogue, cta::Scheduler &scheduler) :
    XrdCtaStream(catalogue, scheduler)
#if 0
    m_isSummary(requestMsg.has_flag(admin::OptionBoolean::SUMMARY)),
    m_isSummaryDone(false)
#endif
{
  using namespace cta::admin;

  XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "TapeFileLsStream() constructor");

#if 0
  if(!requestMsg.has_flag(OptionBoolean::ALL))
  {
    bool has_any = false; // set to true if at least one optional option is set

    // Get the search criteria from the optional options

    m_searchCriteria.archiveFileId    = requestMsg.getOptional(OptionUInt64::ARCHIVE_FILE_ID, &has_any);
    m_searchCriteria.tapeFileCopyNb   = requestMsg.getOptional(OptionUInt64::COPY_NUMBER,     &has_any);
    m_searchCriteria.diskFileId       = requestMsg.getOptional(OptionString::DISKID,          &has_any);
    m_searchCriteria.vid              = requestMsg.getOptional(OptionString::VID,             &has_any);
    m_searchCriteria.tapePool         = requestMsg.getOptional(OptionString::TAPE_POOL,       &has_any);
    m_searchCriteria.diskFileOwnerUid = requestMsg.getOptional(OptionUInt64::OWNER_UID,       &has_any);
    m_searchCriteria.diskFileGid      = requestMsg.getOptional(OptionUInt64::GID,             &has_any);
    m_searchCriteria.storageClass     = requestMsg.getOptional(OptionString::STORAGE_CLASS,   &has_any);
    m_searchCriteria.diskFilePath     = requestMsg.getOptional(OptionString::PATH,            &has_any);
    m_searchCriteria.diskInstance     = requestMsg.getOptional(OptionString::INSTANCE,        &has_any);

    if(!has_any) {
      throw cta::exception::UserError("Must specify at least one search option, or --all");
    }
  }

  if(!m_isSummary) {
    m_tapeFileItor = m_catalogue.getArchiveFilesItor(m_searchCriteria);
  }
#endif
  cta::catalogue::TapeFileSearchCriteria  searchCriteria;
  searchCriteria.vid = requestMsg.getRequired(OptionString::VID);
  m_tapeFileItor = m_catalogue.getArchiveFilesItor(searchCriteria);
}


int TapeFileLsStream::fillBuffer(XrdSsiPb::OStreamBuffer<Data> *streambuf) {
#if 0
  // Special handling for -S (Summary) option
  if(m_isSummary) {
    common::dataStructures::ArchiveFileSummary summary = m_catalogue.getTapeFileSummary(m_searchCriteria);

    Data record;
    record.mutable_tfls_summary()->set_total_files(summary.totalFiles);
    record.mutable_tfls_summary()->set_total_size(summary.totalBytes);
    streambuf->Push(record);

    m_isSummaryDone = true;
    return streambuf->Size();
  }
#endif

  for(bool is_buffer_full = false; m_tapeFileItor.hasMore() && !is_buffer_full; )
  {
    const cta::common::dataStructures::ArchiveFile archiveFile = m_tapeFileItor.next();

    for(auto jt = archiveFile.tapeFiles.cbegin(); jt != archiveFile.tapeFiles.cend(); jt++) {
      Data record;

      // Archive file
      auto af = record.mutable_tfls_item()->mutable_af();
      af->set_archive_id(archiveFile.archiveFileID);
      af->set_storage_class(archiveFile.storageClass);
      af->set_creation_time(archiveFile.creationTime);
      af->set_size(archiveFile.fileSize);

      // Checksum
      common::ChecksumBlob csb;
      checksum::ChecksumBlobToProtobuf(archiveFile.checksumBlob, csb);
      for(auto csb_it = csb.cs().begin(); csb_it != csb.cs().end(); ++csb_it) {
        auto cs_ptr = af->add_checksum();
        cs_ptr->set_type(csb_it->type());
        cs_ptr->set_value(checksum::ChecksumBlob::ByteArrayToHex(csb_it->value()));
      }

      // Disk file
      auto df = record.mutable_tfls_item()->mutable_df();
      df->set_disk_id(archiveFile.diskFileId);
      df->set_path(m_namespace.getPath(archiveFile.diskFileId));
      df->set_disk_instance(archiveFile.diskInstance);
      df->mutable_owner_id()->set_uid(archiveFile.diskFileInfo.owner_uid);
      df->mutable_owner_id()->set_gid(archiveFile.diskFileInfo.gid);

      // Tape file
      auto tf = record.mutable_tfls_item()->mutable_tf();
      tf->set_vid(jt->vid);
      tf->set_copy_nb(jt->copyNb);
      tf->set_block_id(jt->blockId);
      tf->set_f_seq(jt->fSeq);
      tf->set_superseded_by_f_seq(jt->supersededByFSeq);
      tf->set_superseded_by_vid(jt->supersededByVid);

      // is_buffer_full is set to true when we have one full block of data in the buffer, i.e.
      // enough data to send to the client. The actual buffer size is double the block size,
      // so we can keep writing a few additional records after is_buffer_full is true. These
      // will be sent on the next iteration. If we exceed the hard limit of double the block
      // size, Push() will throw an exception.
      is_buffer_full = streambuf->Push(record);
    }
  }

  return streambuf->Size();
}

}} // namespace cta::xrd
