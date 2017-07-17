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
#include <google/protobuf/util/json_util.h>

#include "../frontend/test_util.h"
#include "common/dataStructures/FrontendReturnCode.hpp"
#include "EosCtaApi.h"



//! Usage exception

const std::runtime_error Usage("Usage: eoscta_stub archive|retrieve|delete [options] [--stderr]");



// Define the XRootD SSI callbacks

namespace XrdSsiPb {

/*!
 * Response callback.
 *
 * This is the CTA Front End response to the Notification.
 */

template<>
void RequestCallback<eos::wfe::Response>::operator()(const eos::wfe::Response &response)
{
   using namespace std;

   std::cout << "MetadataCallback():" << std::endl;
   OutputJsonString(&response);
}



/*!
 * Alert callback.
 *
 * This is for messages which should be logged by EOS or directed to the User.
 */

template<>
void RequestCallback<eos::wfe::Alert>::operator()(const eos::wfe::Alert &alert)
{
   std::cout << "AlertCallback():" << std::endl;
   OutputJsonString(&alert);
}



/*!
 * Convert exceptions into Alerts.
 *
 * This tells the framework how to log exceptions received on the client side
 */

template<>
void ExceptionToAlert<eos::wfe::Alert>::operator()(const std::exception &e, eos::wfe::Alert &alert)
{
   alert.set_audience(eos::wfe::Alert::EOSLOG);
   alert.set_message_txt(e.what());
}

} // namespace XrdSsiPb



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
   // Verify that the version of the Google Protocol Buffer library that we linked against is
   // compatible with the version of the headers we compiled against

   GOOGLE_PROTOBUF_VERIFY_VERSION;

   // Fill the Notification protobuf object from the command line arguments

   eos::wfe::Notification notification;
   bool isStderr, isJson;

   fillNotification(notification, isStderr, isJson, argc, argv);

   if(isJson)
   {
      // Output the protocol buffer as a JSON object (for debugging)

      OutputJsonString(&notification);
   }

   // Obtain a Service Provider

   std::string host("localhost");
   int port = 10955;
   std::string resource("/ctafrontend");

   XrdSsiPbServiceType cta_service(host, port, resource);

   // Send the Notification (Request) to the Service

   cta_service.Send(notification);

   // Wait for the response callback.

   std::cout << "Request sent, going to sleep..." << std::endl;

   int wait_secs = 20;

   while(wait_secs--)
   {
      std::cerr << ".";
      sleep(1);
   }

   std::cout << "done." << std::endl;

   // Send output to stdout or stderr?

   std::ostream &myout = isStderr ? std::cerr : std::cout;

   myout << "Hello, world" << std::endl;

   // Optional: Delete all global objects allocated by libprotobuf

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
   } catch (std::exception &ex) {
      std::cerr << "Failed to execute the command. Reason: " << ex.what() << std::endl;
      return cta::common::dataStructures::FrontendReturnCode::ctaErrorNoRetry;
   } catch (...) {
      std::cerr << "Failed to execute the command for an unknown reason" << std::endl;
      return cta::common::dataStructures::FrontendReturnCode::ctaErrorNoRetry;
   }
}

