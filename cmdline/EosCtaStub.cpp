/*!
 * @project        The CERN Tape Archive (CTA)
 * @brief          Command-line tool to test EOS-CTA interface
 * @description    Proof-of-concept stub to combine Google Protocol Buffers and XRootD SSI transport
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

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cryptopp/base64.h>

#include "common/dataStructures/FrontendReturnCode.hpp"

#include "XrdSsiPbDebug.hpp"
#include "EosCtaApi.h"



// Define XRootD SSI Alert message callback

namespace XrdSsiPb {

/*!
 * Alert callback.
 *
 * Defines how Alert messages should be logged by EOS (or directed to the User)
 */

template<>
void RequestCallback<eos::wfe::Alert>::operator()(const eos::wfe::Alert &alert)
{
   std::cout << "AlertCallback():" << std::endl;
   OutputJsonString(std::cout, &alert);
}

} // namespace XrdSsiPb



//! Usage exception

const std::runtime_error Usage("Usage: eoscta_stub archive|retrieve|delete [options] [--stderr]");



/*!
 * Fill a Notification message from the command-line parameters
 *
 * @param[out]    notification    The protobuf to fill
 * @param[in]     argval          A base64-encoded blob
 */

void base64Decode(eos::wfe::Notification &notification, const std::string &argval)
{
   using namespace std;

   string base64str(argval.substr(7)); // delete "base64:" from start of string

   // Decode Base64 blob

   string decoded;
   CryptoPP::StringSource ss1(base64str, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

   // Split into file metadata and directory metadata parts

   string fmd = decoded.substr(decoded.find("fmd={ ")+6, decoded.find(" }")-7);
   string dmd = decoded.substr(decoded.find("dmd={ ")+6);
   dmd.resize(dmd.size()-3); // remove closing space/brace/quote

   // Process file metadata

   stringstream fss(fmd);
   while(!fss.eof())
   {
      string key;
      fss >> key;
      size_t eq_pos = key.find("=");
      string val(key.substr(eq_pos+1));
      key.resize(eq_pos);

           if(key == "fid") notification.mutable_file()->set_fid(stoi(val));
      else if(key == "pid") notification.mutable_file()->set_pid(stoi(val));
      else if(key == "ctime")
      {
         size_t pt_pos = val.find(".");
         notification.mutable_file()->mutable_ctime()->set_sec(stoi(val.substr(0, pt_pos)));
         notification.mutable_file()->mutable_ctime()->set_nsec(stoi(val.substr(pt_pos+1)));
      }
      else if(key == "mtime")
      {
         size_t pt_pos = val.find(".");
         notification.mutable_file()->mutable_mtime()->set_sec(stoi(val.substr(0, pt_pos)));
         notification.mutable_file()->mutable_mtime()->set_nsec(stoi(val.substr(pt_pos+1)));
      }
      else if(key == "uid") notification.mutable_file()->mutable_owner()->set_uid(stoi(val));
      else if(key == "gid") notification.mutable_file()->mutable_owner()->set_gid(stoi(val));
      else if(key == "size") notification.mutable_file()->set_size(stoi(val));
      else if(key == "xstype") notification.mutable_file()->mutable_cks()->set_name(val);
      else if(key == "xs") notification.mutable_file()->mutable_cks()->set_value(val);
      else if(key == "mode") notification.mutable_file()->set_mode(stoi(val));
      else if(key == "file") notification.mutable_file()->set_lpath(val);
      else { cout << "No match in protobuf for fmd:" << key << "=" << val << endl; }
   }

   // Process directory metadata

   string xattrn;

   stringstream dss(dmd);
   while(!dss.eof())
   {
      string key;
      dss >> key;
      size_t eq_pos = key.find("=");
      string val(key.substr(eq_pos+1));
      key.resize(eq_pos);

           if(key == "fid") notification.mutable_directory()->set_fid(stoi(val));
      else if(key == "pid") notification.mutable_directory()->set_pid(stoi(val));
      else if(key == "ctime")
      {
         size_t pt_pos = val.find(".");
         notification.mutable_directory()->mutable_ctime()->set_sec(stoi(val.substr(0, pt_pos)));
         notification.mutable_directory()->mutable_ctime()->set_nsec(stoi(val.substr(pt_pos+1)));
      }
      else if(key == "mtime")
      {
         size_t pt_pos = val.find(".");
         notification.mutable_directory()->mutable_mtime()->set_sec(stoi(val.substr(0, pt_pos)));
         notification.mutable_directory()->mutable_mtime()->set_nsec(stoi(val.substr(pt_pos+1)));
      }
      else if(key == "uid") notification.mutable_directory()->mutable_owner()->set_uid(stoi(val));
      else if(key == "gid") notification.mutable_directory()->mutable_owner()->set_gid(stoi(val));
      else if(key == "size") notification.mutable_directory()->set_size(stoi(val));
      else if(key == "mode") notification.mutable_directory()->set_mode(stoi(val));
      else if(key == "file") notification.mutable_directory()->set_lpath(val);
      else if(key == "xattrn") xattrn = val;
      else if(key == "xattrv")
      {
         // Insert extended attributes

         notification.mutable_directory()->mutable_xattr()->insert(google::protobuf::MapPair<string,string>(xattrn, val));
      }
      else { cout << "No match in protobuf for dmd:" << key << "=" << val << endl; }
   }
}



/*!
 * Fill a Notification message from the command-line parameters
 *
 * @param[out]    notification    The protobuf to fill
 * @param[out]    isStderr        --stderr appears on the command line
 * @param[out]    isJson          --json appears on the command line
 * @param[in]     argc            The number of command-line arguments
 * @param[in]     argv            The command-line arguments
 */

void fillNotification(eos::wfe::Notification &notification, bool &isStderr, bool &isJson, int argc, const char *const *const argv)
{
   isStderr = false;
   isJson = false;

   // First argument must be a valid command specifying which workflow action to execute

   if(argc < 2) throw Usage;

   const std::string wf_command(argv[1]);

   if(wf_command == "archive")
   {
      notification.mutable_wf()->set_event(eos::wfe::Workflow::CLOSEW);
   }
   else if(wf_command == "retrieve")
   {
      notification.mutable_wf()->set_event(eos::wfe::Workflow::PREPARE);
   }
   else if(wf_command == "delete")
   {
      notification.mutable_wf()->set_event(eos::wfe::Workflow::DELETE);
   }
   else throw Usage;

   // Parse the remaining command-line options

   for(int arg = 2; arg < argc; ++arg)
   {
      const std::string argstr(argv[arg]);

      if(argstr == "--stderr") { isStderr = true; continue; }
      if(argstr == "--json") { isJson = true; continue; }

      if(argstr.substr(0,2) != "--" || argc == ++arg) throw std::runtime_error("Arguments must be provided as --key value pairs");

      const std::string argval(argv[arg]);

           if(argstr == "--instance")            notification.mutable_wf()->mutable_instance()->set_name(argval);
      else if(argstr == "--srcurl")              notification.mutable_wf()->mutable_instance()->set_url(argval);

      else if(argstr == "--user")                notification.mutable_cli()->mutable_user()->set_username(argval);
      else if(argstr == "--group")               notification.mutable_cli()->mutable_user()->set_groupname(argval);

      else if(argstr == "--dsturl")              notification.mutable_transport()->set_url(argval); // for retrieve WF

      else if(argstr == "--diskid")              notification.mutable_file()->set_fid(std::stoi(argval));
      else if(argstr == "--diskfileowner")       notification.mutable_file()->mutable_owner()->set_username(argval);
      else if(argstr == "--diskfilegroup")       notification.mutable_file()->mutable_owner()->set_groupname(argval);
      else if(argstr == "--size")                notification.mutable_file()->set_size(std::stoi(argval));
      else if(argstr == "--checksumtype")        notification.mutable_file()->mutable_cks()->set_name(argval);
      else if(argstr == "--checksumvalue")       notification.mutable_file()->mutable_cks()->set_value(argval);
      else if(argstr == "--diskfilepath")        notification.mutable_file()->set_lpath(argval);
      else if(argstr == "--storageclass")        {
         google::protobuf::MapPair<std::string,std::string> sc("CTA_StorageClass", argval);
         notification.mutable_file()->mutable_xattr()->insert(sc);
      }

      else if(argstr == "--id")                  {} // eos::wfe::fxattr:sys.archiveFileId, not used for archive WF
      else if(argstr == "--diskpool")            {} // = default?
      else if(argstr == "--throughput")          {} // = 10000?
      else if(argstr == "--recoveryblob:base64") base64Decode(notification, argval);
      else throw std::runtime_error("Unrecognised key " + argstr);
   }
}



/*!
 * Sends a Notification to the CTA XRootD SSI server
 *
 * @param    argc[in]    The number of command-line arguments
 * @param    argv[in]    The command-line arguments
 */

int exceptionThrowingMain(int argc, const char *const *const argv)
{
   // Verify that the Google Protocol Buffer header and linked library versions are compatible

   GOOGLE_PROTOBUF_VERIFY_VERSION;

   eos::wfe::Notification notification;

   // Parse the command line arguments: fill the Notification fields

   bool isStderr, isJson;

   fillNotification(notification, isStderr, isJson, argc, argv);

   std::ostream &myout = isStderr ? std::cerr : std::cout;

   if(isJson)
   {
      XrdSsiPb::OutputJsonString(myout, &notification);
   }

   // Obtain a Service Provider

   std::string host("localhost");
   int port = 10955;
   std::string resource("/ctafrontend");

   XrdSsiPbServiceType cta_service(host, port, resource);

   // Send the Request to the Service and get a Response

   eos::wfe::Response response = cta_service.Send(notification);

   if(isJson)
   {
      XrdSsiPb::OutputJsonString(myout, &response);
   }

   // Handle responses

   switch(response.type())
   {
      using namespace eos::wfe;

      case Response::RSP_SUCCESS:         myout << response.message_txt() << std::endl; break;
      case Response::RSP_ERR_PROTOBUF:    throw XrdSsiPb::PbException(response.message_txt());
      case Response::RSP_ERR_CTA:         throw std::runtime_error(response.message_txt());
      // ... define other response types in the protocol buffer (e.g. user error)
      default:                            throw XrdSsiPb::PbException("Invalid response type.");
   }

   // Delete all global objects allocated by libprotobuf

   google::protobuf::ShutdownProtobufLibrary();

   return 0;
}



/*!
 * Start here
 *
 * @param    argc[in]    The number of command-line arguments
 * @param    argv[in]    The command-line arguments
 */

int main(int argc, const char **argv)
{
   try {    
      return exceptionThrowingMain(argc, argv);
   } catch (XrdSsiPb::PbException &ex) {
      std::cerr << "Error in Google Protocol Buffers: " << ex.what() << std::endl;
   } catch (XrdSsiPb::XrdSsiException &ex) {
      std::cerr << "Error from XRootD SSI Framework: " << ex.what() << std::endl;
   } catch (std::exception &ex) {
      std::cerr << "Caught exception: " << ex.what() << std::endl;
   } catch (...) {
      std::cerr << "Caught an unknown exception" << std::endl;
   }

   return cta::common::dataStructures::FrontendReturnCode::ctaErrorNoRetry;
}

