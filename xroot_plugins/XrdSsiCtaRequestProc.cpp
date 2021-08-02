/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2015-2021 CERN
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

   // Also log the error in the XRootD log
   XrdSsiPb::Log::Msg(XrdSsiPb::Log::ERROR, "ExceptionHandler", ex.what());
}



/*!
 * Process the Notification Request
 */
template <>
void RequestProc<cta::xrd::Request, cta::xrd::Response, cta::xrd::Alert>::ExecuteAction()
{
   const std::string ErrorFunction("In RequestProc::ExecuteAction(): ");
   XrdSsiCtaServiceProvider *cta_service_ptr;

   try {
      // Perform a capability query (sanity check):
      // the object pointed to by XrdSsiProviderServer must be a XrdSsiCtaServiceProvider
      if(!(cta_service_ptr = dynamic_cast<XrdSsiCtaServiceProvider*>(XrdSsiProviderServer)))
      {
         // If not, this is fatal unrecoverable error
         throw std::logic_error("XRootD Service is not a CTA Service");
      }

      // Process the message
      cta::xrd::RequestMessage request_msg(*(m_resource.client), cta_service_ptr);
      request_msg.process(m_request, m_metadata, m_response_stream_ptr);
   } catch(PbException &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_PROTOBUF);
      m_metadata.set_message_txt(ex.what());
      cta_service_ptr->getLogContext().log(cta::log::ERR, ErrorFunction + "RSP_ERR_PROTOBUF: " + ex.what());
   } catch(cta::exception::UserError &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_USER);
      m_metadata.set_message_txt(ex.getMessageValue());
      cta_service_ptr->getLogContext().log(cta::log::ERR, ErrorFunction + "RSP_ERR_USER: " + ex.getMessageValue());
   } catch(cta::exception::Exception &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_CTA);
      m_metadata.set_message_txt(ex.getMessageValue());
      cta_service_ptr->getLogContext().log(cta::log::ERR, ErrorFunction + "RSP_ERR_CTA: " + ex.what());
   } catch(std::runtime_error &ex) {
      m_metadata.set_type(cta::xrd::Response::RSP_ERR_CTA);
      m_metadata.set_message_txt(ex.what());
      cta_service_ptr->getLogContext().log(cta::log::ERR, ErrorFunction + "RSP_ERR_CTA: " + ex.what());
   }
}

} // namespace XrdSsiPb
