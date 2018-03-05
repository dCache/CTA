/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          XRootD SSI Responder class implementation
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

#include "common/dataStructures/ArchiveRequest.hpp"
#include "common/exception/Exception.hpp"

#include "XrdSsiPbLog.hpp"
#include "XrdSsiPbException.hpp"
#include "XrdSsiPbRequestProc.hpp"

#include "XrdSsiCtaServiceProvider.hpp"
#include "XrdSsiCtaRequestMessage.hpp"



namespace XrdSsiPb {

/*!
 * Convert a framework exception into a Response
 */
template<>
void ExceptionHandler<cta::xrd::Response, PbException>::operator()(cta::xrd::Response &response, const PbException &ex)
{
   response.set_type(cta::xrd::Response::RSP_ERR_PROTOBUF);
   response.set_message_txt(ex.what());
}



/*!
 * Process the Notification Request
 */
template <>
void RequestProc<cta::xrd::Request, cta::xrd::Response, cta::xrd::Alert>::ExecuteAction()
{
   try {
      // Perform a capability query on the XrdSsiProviderServer object: it must be a XrdSsiCtaServiceProvider

      XrdSsiCtaServiceProvider *cta_service_ptr;
     
      if(!(cta_service_ptr = dynamic_cast<XrdSsiCtaServiceProvider*>(XrdSsiProviderServer)))
      {
         throw cta::exception::Exception("XRootD Service is not a CTA Service");
      }

      // Output message in Json format
      Log::DumpProtobuf(Log::PROTOBUF, &m_request);

      cta::xrd::RequestMessage request_msg(*(m_resource.client), cta_service_ptr);
      request_msg.process(m_request, m_metadata, m_response_stream_ptr);
   } catch(cta::exception::UserError &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_USER);
      m_metadata.set_message_txt(ex.getMessageValue());
   } catch(PbException &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_PROTOBUF);
      m_metadata.set_message_txt(ex.what());
   } catch(std::exception &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_CTA);
      m_metadata.set_message_txt(ex.what());
   }
}

} // namespace XrdSsiPb
