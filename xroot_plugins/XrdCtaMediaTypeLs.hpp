/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          CTA Media Type Ls stream implementation
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


namespace cta { namespace xrd {

/*!
 * Stream object which implements the "mediatype ls" command
 */
class MediaTypeLsStream: public XrdCtaStream{
public:
  /*!
   * Constructor
   *
   * @param[in]    requestMsg    RequestMessage containing command-line arguments
   * @param[in]    catalogue     CTA Catalogue
   * @param[in]    scheduler     CTA Scheduler
   */
  MediaTypeLsStream(const RequestMessage &requestMsg, cta::catalogue::Catalogue &catalogue, cta::Scheduler &scheduler);

private:
  /*!
   * Can we close the stream?
   */
  virtual bool isDone() const {
    return m_mediaTypeList.empty();
  }

  /*!
   * Fill the buffer
   */
  virtual int fillBuffer(XrdSsiPb::OStreamBuffer<Data> *streambuf);

  std::list<cta::catalogue::MediaTypeWithLogs> m_mediaTypeList;    //!< List of tape media types from the catalogue

  static constexpr const char* const LOG_SUFFIX  = "MediaTypeLsStream";    //!< Identifier for log messages
};


MediaTypeLsStream::MediaTypeLsStream(const RequestMessage &requestMsg, cta::catalogue::Catalogue &catalogue, cta::Scheduler &scheduler) :
  XrdCtaStream(catalogue, scheduler),
  m_mediaTypeList(catalogue.getMediaTypes())
{
  using namespace cta::admin;

  XrdSsiPb::Log::Msg(XrdSsiPb::Log::DEBUG, LOG_SUFFIX, "MediaTypeLsStream() constructor");
}

int MediaTypeLsStream::fillBuffer(XrdSsiPb::OStreamBuffer<Data> *streambuf) {
  for(bool is_buffer_full = false; !m_mediaTypeList.empty() && !is_buffer_full; m_mediaTypeList.pop_front()) {
    Data record;

    auto &mt      = m_mediaTypeList.front();
    auto  mt_item = record.mutable_mtls_item();

    mt_item->set_name(mt.name);
    mt_item->set_cartridge(mt.cartridge);
    mt_item->set_capacity(mt.capacityInBytes);
    mt_item->set_primary_density_code(mt.primaryDensityCode);
    mt_item->set_secondary_density_code(mt.secondaryDensityCode);
    if (mt.nbWraps) mt_item->set_number_of_wraps(mt.nbWraps.value());
    if (mt.minLPos) mt_item->set_min_lpos(mt.minLPos.value());
    if (mt.maxLPos) mt_item->set_max_lpos(mt.maxLPos.value());
    mt_item->set_comment(mt.comment);
    mt_item->mutable_creation_log()->set_username(mt.creationLog.username);
    mt_item->mutable_creation_log()->set_host(mt.creationLog.host);
    mt_item->mutable_creation_log()->set_time(mt.creationLog.time);
    mt_item->mutable_last_modification_log()->set_username(mt.lastModificationLog.username);
    mt_item->mutable_last_modification_log()->set_host(mt.lastModificationLog.host);
    mt_item->mutable_last_modification_log()->set_time(mt.lastModificationLog.time);

    is_buffer_full = streambuf->Push(record);
  }
  return streambuf->Size();
}

}} // namespace cta::xrd
