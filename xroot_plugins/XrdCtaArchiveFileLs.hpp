/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          CTA Frontend Archive File Ls stream implementation
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

#pragma once

#include "XrdSsiPbOStreamBuffer.hpp"
#include "catalogue/Catalogue.hpp"



namespace cta { namespace xrd {

/*!
 * Stream object which implements "af ls" command.
 */
class ArchiveFileLsStream : public XrdSsiStream
{
public:
   ArchiveFileLsStream(cta::catalogue::ArchiveFileItor archiveFileItor) :
      XrdSsiStream(XrdSsiStream::isActive),
      m_archiveFileItor(std::move(archiveFileItor))
   {
#ifdef XRDSSI_DEBUG
      std::cerr << "[DEBUG] ArchiveFileLsStream() constructor" << std::endl;
#endif
   }

   virtual ~ArchiveFileLsStream() {
#ifdef XRDSSI_DEBUG
      std::cerr << "[DEBUG] ArchiveFileLsStream() destructor" << std::endl;
#endif
   }

   /*!
    * Synchronously obtain data from an active stream
    *
    * Active streams can only exist on the server-side. This XRootD SSI Stream class is marked as an
    * active stream in the constructor.
    *
    * @param[out]       eInfo   The object to receive any error description.
    * @param[in,out]    dlen    input:  the optimal amount of data wanted (this is a hint)
    *                           output: the actual amount of data returned in the buffer.
    * @param[in,out]    last    input:  should be set to false.
    *                           output: if true it indicates that no more data remains to be returned
    *                                   either for this call or on the next call.
    *
    * @return    Pointer to the Buffer object that contains a pointer to the the data (see below). The
    *            buffer must be returned to the stream using Buffer::Recycle(). The next member is usable.
    * @retval    0    No more data remains or an error occurred:
    *                 last = true:  No more data remains.
    *                 last = false: A fatal error occurred, eRef has the reason.
    */
   virtual Buffer *GetBuff(XrdSsiErrInfo &eInfo, int &dlen, bool &last) {
#ifdef XRDSSI_DEBUG
      std::cerr << "[DEBUG] ArchiveFileLsStream::GetBuff(): XrdSsi buffer fill request (" << dlen << " bytes)" << std::endl;
#endif

      XrdSsiPb::OStreamBuffer<cta::xrd::Data> *streambuf;

      try {
         if(!m_archiveFileItor.hasMore()) {
            // Nothing more to send, close the stream
            last = true;
            return nullptr;
         }

         streambuf = new XrdSsiPb::OStreamBuffer<cta::xrd::Data>(dlen);

         for(bool is_buffer_full = false; m_archiveFileItor.hasMore() && !is_buffer_full; )
         {
            const cta::common::dataStructures::ArchiveFile archiveFile = m_archiveFileItor.next();

            for(auto jt = archiveFile.tapeFiles.cbegin(); jt != archiveFile.tapeFiles.cend(); jt++) {
               cta::xrd::Data record;

               // Copy number
               record.mutable_af_ls_item()->set_copy_nb(jt->first);

               // Archive file
               auto af = record.mutable_af_ls_item()->mutable_af();
               af->set_archive_id(archiveFile.archiveFileID);
               af->set_disk_instance(archiveFile.diskInstance);
               af->set_disk_id(archiveFile.diskFileId);
               af->set_size(archiveFile.fileSize);
               af->mutable_cs()->set_type(archiveFile.checksumType);
               af->mutable_cs()->set_value(archiveFile.checksumValue);
               af->set_storage_class(archiveFile.storageClass);
               af->mutable_df()->set_owner(archiveFile.diskFileInfo.owner);
               af->mutable_df()->set_group(archiveFile.diskFileInfo.group);
               af->mutable_df()->set_path(archiveFile.diskFileInfo.path);
               af->set_creation_time(archiveFile.creationTime);

               // Tape file
               auto tf = record.mutable_af_ls_item()->mutable_tf();
               tf->set_vid(jt->second.vid);
               tf->set_f_seq(jt->second.fSeq);
               tf->set_block_id(jt->second.blockId);

               // is_buffer_full is set to true when we have one full block of data in the buffer, i.e.
               // enough data to send to the client. The actual buffer size is double the block size,
               // so we can keep writing a few additional records after is_buffer_full is true. These
               // will be sent on the next iteration. If we exceed the hard limit of double the block
               // size, Push() will throw an exception.
               is_buffer_full = streambuf->Push(record);
            }
         }
         dlen = streambuf->Size();
#ifdef XRDSSI_DEBUG
         std::cerr << "[DEBUG] ArchiveFileLsStream::GetBuff(): Returning buffer with " << dlen << " bytes of data." << std::endl;
#endif
      } catch(cta::exception::Exception &ex) {
         throw std::runtime_error(ex.getMessage().str());
      } catch(std::exception &ex) {
         std::ostringstream errMsg;
         errMsg << __FUNCTION__ << " failed: " << ex.what();
         eInfo.Set(errMsg.str().c_str(), ECANCELED);
         delete streambuf;
      } catch(...) {
         std::ostringstream errMsg;
         errMsg << __FUNCTION__ << " failed: Caught an unknown exception";
         eInfo.Set(errMsg.str().c_str(), ECANCELED);
         delete streambuf;
      }
      return streambuf;
   }

private:
   catalogue::ArchiveFileItor m_archiveFileItor;
};

}} // namespace cta::xrd
