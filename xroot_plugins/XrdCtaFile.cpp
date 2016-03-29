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

#include "cmdline/CTACmd.hpp"
#include "scheduler/SchedulerDatabase.hpp"
#include "xroot_plugins/XrdCtaFile.hpp"

#include "XrdSec/XrdSecEntity.hh"
#include "common/Configuration.hpp"

#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <pwd.h>
#include <sstream>
#include <string>
#include <time.h>

namespace cta { namespace xrootPlugins {

//------------------------------------------------------------------------------
// checkClient
//------------------------------------------------------------------------------
cta::common::dataStructures::SecurityIdentity XrdProFile::checkClient(const XrdSecEntity *client) {
// TEMPORARILY commented out host check for demo purposes:
//  if(!client || !client->host || strncmp(client->host, "localhost", 9))
//  {
//    throw cta::exception::Exception(std::string(__FUNCTION__)+": [ERROR] operation possible only from localhost");
//  }
  cta::common::dataStructures::SecurityIdentity cliIdentity;
  struct passwd pwd;
  struct passwd *result;
  long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
  {
    bufsize = 16384;
  }
  std::unique_ptr<char> buf((char *)malloc((size_t)bufsize));
  if(buf.get() == NULL)
  {
    throw cta::exception::Exception(std::string(__FUNCTION__)+": [ERROR] malloc of the buffer failed");
  }
  int rc = getpwnam_r(client->name, &pwd, buf.get(), bufsize, &result);
  if(result == NULL)
  {
    if (rc == 0)
    {
      throw cta::exception::Exception(std::string(__FUNCTION__)+": [ERROR] User "+client->name+" not found");
    }
    else
    {
      throw cta::exception::Exception(std::string(__FUNCTION__)+": [ERROR] getpwnam_r failed");
    }
  }
  std::cout << "Request received from client. Username: " << client->name << " uid: " << pwd.pw_uid << " gid: " << pwd.pw_gid << std::endl;
  cta::common::dataStructures::UserIdentity user;
  user.name=client->name;
  user.group=client->grps;
  cliIdentity.user=user;
  cliIdentity.host=client->host;
  return cliIdentity;
}

//------------------------------------------------------------------------------
// commandDispatcher
//------------------------------------------------------------------------------
void XrdProFile::dispatchCommand(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::string command(tokens[1]);
  
  std::vector<std::string> adminCommands = {"bs","bootstrap","ad","admin","ah","adminhost","tp","tapepool","ar","archiveroute","ll","logicallibrary",
          "ta","tape","sc","storageclass","us","user","mg","mountgroup","de","dedication","re","repack","sh","shrink","ve","verify",
          "af","archivefile","te","test","dr","drive","rc","reconcile","lpa","listpendingarchives","lpr","listpendingretrieves","lds","listdrivestates"};
  
  if (std::find(adminCommands.begin(), adminCommands.end(), command) != adminCommands.end()) {
    m_scheduler->authorizeCliIdentity(cliIdentity);
  }
  
  if     ("bs"  == command || "bootstrap"            == command) {xCom_bootstrap(tokens, cliIdentity);}
  else if("ad"  == command || "admin"                == command) {xCom_admin(tokens, cliIdentity);}
  else if("ah"  == command || "adminhost"            == command) {xCom_adminhost(tokens, cliIdentity);}
  else if("tp"  == command || "tapepool"             == command) {xCom_tapepool(tokens, cliIdentity);}
  else if("ar"  == command || "archiveroute"         == command) {xCom_archiveroute(tokens, cliIdentity);}
  else if("ll"  == command || "logicallibrary"       == command) {xCom_logicallibrary(tokens, cliIdentity);}
  else if("ta"  == command || "tape"                 == command) {xCom_tape(tokens, cliIdentity);}
  else if("sc"  == command || "storageclass"         == command) {xCom_storageclass(tokens, cliIdentity);}
  else if("us"  == command || "user"                 == command) {xCom_user(tokens, cliIdentity);}
  else if("mg"  == command || "mountgroup"           == command) {xCom_mountgroup(tokens, cliIdentity);}
  else if("de"  == command || "dedication"           == command) {xCom_dedication(tokens, cliIdentity);}
  else if("re"  == command || "repack"               == command) {xCom_repack(tokens, cliIdentity);}
  else if("sh"  == command || "shrink"               == command) {xCom_shrink(tokens, cliIdentity);}
  else if("ve"  == command || "verify"               == command) {xCom_verify(tokens, cliIdentity);}
  else if("af"  == command || "archivefile"          == command) {xCom_archivefile(tokens, cliIdentity);}
  else if("te"  == command || "test"                 == command) {xCom_test(tokens, cliIdentity);}
  else if("dr"  == command || "drive"                == command) {xCom_drive(tokens, cliIdentity);}
  else if("rc"  == command || "reconcile"            == command) {xCom_reconcile(tokens, cliIdentity);}
  else if("lpa" == command || "listpendingarchives"  == command) {xCom_listpendingarchives(tokens, cliIdentity);}
  else if("lpr" == command || "listpendingretrieves" == command) {xCom_listpendingretrieves(tokens, cliIdentity);}
  else if("lds" == command || "listdrivestates"      == command) {xCom_listdrivestates(tokens, cliIdentity);}
  else if("a"   == command || "archive"              == command) {xCom_archive(tokens, cliIdentity);}
  else if("r"   == command || "retrieve"             == command) {xCom_retrieve(tokens, cliIdentity);}
  else if("da"  == command || "deletearchive"        == command) {xCom_deletearchive(tokens, cliIdentity);}
  else if("cr"  == command || "cancelretrieve"       == command) {xCom_cancelretrieve(tokens, cliIdentity);}
  else if("ufi" == command || "updatefileinfo"       == command) {xCom_updatefileinfo(tokens, cliIdentity);}
  else if("lsc" == command || "liststorageclass"     == command) {xCom_liststorageclass(tokens, cliIdentity);}
  
  else {m_data = getGenericHelp(tokens[0]);}
}

//------------------------------------------------------------------------------
// decode
//------------------------------------------------------------------------------
std::string XrdProFile::decode(const std::string msg) const {
  std::string ret;
  CryptoPP::StringSource ss1(msg, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(ret)));
  return ret;
}

//------------------------------------------------------------------------------
// open
//------------------------------------------------------------------------------
int XrdProFile::open(const char *fileName, XrdSfsFileOpenMode openMode, mode_t createMode, const XrdSecEntity *client, const char *opaque) {
  try {
    const cta::common::dataStructures::SecurityIdentity cliIdentity = checkClient(client);

    if(!strlen(fileName)) { //this should never happen
      m_data = getGenericHelp("");
      return SFS_OK;
    }

    std::vector<std::string> tokens;
    std::stringstream ss(fileName+1); //let's skip the first slash which is always prepended since we are asking for an absolute path
    std::string item;
    while (std::getline(ss, item, '&')) {
      replaceAll(item, "_", "/"); 
      //need to add this because xroot removes consecutive slashes, and the 
      //cryptopp base64 algorithm may produce consecutive slashes. This is solved 
      //in cryptopp-5.6.3 (using Base64URLEncoder instead of Base64Encoder) but we 
      //currently have cryptopp-5.6.2. To be changed in the future...
      item = decode(item);
      tokens.push_back(item);
    }

    if(tokens.size() == 0) { //this should never happen
      m_data = getGenericHelp("");
      return SFS_OK;
    }
    if(tokens.size() < 2) {
      m_data = getGenericHelp(tokens[0]);
      return SFS_OK;
    }  
    dispatchCommand(tokens, cliIdentity);
    return SFS_OK;
  } catch (cta::exception::Exception &ex) {
    m_data = "[ERROR] CTA exception caught: ";
    m_data += ex.getMessageValue();
    m_data += "\n";
    return SFS_OK;
  } catch (std::exception &ex) {
    m_data = "[ERROR] Exception caught: ";
    m_data += ex.what();
    m_data += "\n";
    return SFS_OK;
  } catch (...) {
    m_data = "[ERROR] Unknown exception caught!";
    m_data += "\n";
    return SFS_OK;
  }
}

//------------------------------------------------------------------------------
// close
//------------------------------------------------------------------------------
int XrdProFile::close() {
  return SFS_OK;
}

//------------------------------------------------------------------------------
// fctl
//------------------------------------------------------------------------------
int XrdProFile::fctl(const int cmd, const char *args, XrdOucErrInfo &eInfo) {  
  error.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// FName
//------------------------------------------------------------------------------
const char* XrdProFile::FName() {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return NULL;
}

//------------------------------------------------------------------------------
// getMmap
//------------------------------------------------------------------------------
int XrdProFile::getMmap(void **Addr, off_t &Size) {
  *Addr = const_cast<char *>(m_data.c_str());
  Size = m_data.length();
  return SFS_OK; //change to "return SFS_ERROR;" in case the read function below is wanted, in that case uncomment the lines in that function.
}

//------------------------------------------------------------------------------
// read
//------------------------------------------------------------------------------
XrdSfsXferSize XrdProFile::read(XrdSfsFileOffset offset, char *buffer, XrdSfsXferSize size) {
//  if((unsigned long)offset<m_data.length()) {
//    strncpy(buffer, m_data.c_str()+offset, size);
//    return m_data.length()-offset;
//  }
//  else {
//    return 0;
//  }
  error.setErrInfo(ENOTSUP, "Not supported.");
  return 0;
}

//------------------------------------------------------------------------------
// read
//------------------------------------------------------------------------------
XrdSfsXferSize XrdProFile::read(XrdSfsFileOffset offset, XrdSfsXferSize size) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return 0;
}

//------------------------------------------------------------------------------
// read
//------------------------------------------------------------------------------
XrdSfsXferSize XrdProFile::read(XrdSfsAio *aioparm) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return 0;
}

//------------------------------------------------------------------------------
// write
//------------------------------------------------------------------------------
XrdSfsXferSize XrdProFile::write(XrdSfsFileOffset offset, const char *buffer, XrdSfsXferSize size) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return 0;
}

//------------------------------------------------------------------------------
// write
//------------------------------------------------------------------------------
int XrdProFile::write(XrdSfsAio *aioparm) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// stat
//------------------------------------------------------------------------------
int XrdProFile::stat(struct stat *buf) {
  buf->st_size=m_data.length();
  return SFS_OK;
}

//------------------------------------------------------------------------------
// sync
//------------------------------------------------------------------------------
int XrdProFile::sync() {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// sync
//------------------------------------------------------------------------------
int XrdProFile::sync(XrdSfsAio *aiop) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// truncate
//------------------------------------------------------------------------------
int XrdProFile::truncate(XrdSfsFileOffset fsize) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// getCXinfo
//------------------------------------------------------------------------------
int XrdProFile::getCXinfo(char cxtype[4], int &cxrsz) {
  error.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
XrdProFile::XrdProFile(
  cta::catalogue::Catalogue *catalogue,
  cta::Scheduler *scheduler,
  const char *user,
  int MonID):
  error(user, MonID),
  m_catalogue(catalogue),
  m_scheduler(scheduler),
  m_data("") {  
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
XrdProFile::~XrdProFile() {  
}

//------------------------------------------------------------------------------
// replaceAll
//------------------------------------------------------------------------------
void XrdProFile::replaceAll(std::string& str, const std::string& from, const std::string& to) const {
  if(from.empty() || str.empty())
    return;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

//------------------------------------------------------------------------------
// getOptionValue
//------------------------------------------------------------------------------
std::string XrdProFile::getOptionValue(const std::vector<std::string> &tokens, const std::string& optionShortName, const std::string& optionLongName, const bool encoded) {
  for(auto it=tokens.cbegin(); it!=tokens.cend(); it++) {
    if(optionShortName == *it || optionLongName == *it) {
      auto it_next=it+1;
      if(it_next!=tokens.cend()) {
        if(!encoded) return *it_next;
        else return decode(*it_next);
      }
      else {
        return "";
      }
    }
  }
  return "";
}

//------------------------------------------------------------------------------
// hasOption
//------------------------------------------------------------------------------
bool XrdProFile::hasOption(const std::vector<std::string> &tokens, const std::string& optionShortName, const std::string& optionLongName) {
  for(auto it=tokens.cbegin(); it!=tokens.cend(); it++) {
    if(optionShortName == *it || optionLongName == *it) {
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
// timeToString
//------------------------------------------------------------------------------
std::string XrdProFile::timeToString(const time_t &time) {
  std::string timeString(ctime(&time));
  timeString=timeString.substr(0,timeString.size()-1); //remove newline
  return timeString;
}

//------------------------------------------------------------------------------
// formatResponse
//------------------------------------------------------------------------------
std::string XrdProFile::formatResponse(const std::vector<std::vector<std::string>> &responseTable) {
  if(responseTable.empty()||responseTable.at(0).empty()) {
    return "";
  }
  std::vector<int> columnSizes;
  for(uint i=0; i<responseTable.at(0).size(); i++) {
    uint columnSize=0;
    for(uint j=0; j<responseTable.size(); j++) {
      if(responseTable.at(i).at(j).size()>columnSize) {
        columnSize=responseTable.at(i).at(j).size();
      }
    }
    columnSize++; //add one space
    columnSizes.push_back(columnSize);
  }
  std::stringstream responseSS;
  for(auto row=responseTable.cbegin(); row!=responseTable.cend(); row++) {
    if(row==responseTable.cbegin()) responseSS << "\x1b[31;1m";
    for(uint i=0; i<row->size(); i++) {      
      responseSS << " " << std::setw(columnSizes.at(i)) << row->at(i);      
    }
    if(row==responseTable.cbegin()) responseSS << "\x1b[0m" << std::endl;
  }
  return responseSS.str();
}

//------------------------------------------------------------------------------
// addLogInfoToResponseRow
//------------------------------------------------------------------------------
void XrdProFile::addLogInfoToResponseRow(std::vector<std::string> &responseRow, const cta::common::dataStructures::EntryLog &creationLog, const cta::common::dataStructures::EntryLog &lastModificationLog) {
  responseRow.push_back(creationLog.user.name);
  responseRow.push_back(creationLog.user.group);
  responseRow.push_back(creationLog.host);
  responseRow.push_back(timeToString(creationLog.time));
  responseRow.push_back(lastModificationLog.user.name);
  responseRow.push_back(lastModificationLog.user.group);
  responseRow.push_back(lastModificationLog.host);
  responseRow.push_back(timeToString(lastModificationLog.time));
}

//------------------------------------------------------------------------------
// xCom_bootstrap
//------------------------------------------------------------------------------
void XrdProFile::xCom_bootstrap(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " bs/bootstrap --user/-u <user> --group/-g <group> --hostname/-h <host_name> --comment/-m <\"comment\">" << std::endl;
  std::string user = getOptionValue(tokens, "-u", "--user", false);
  std::string group = getOptionValue(tokens, "-g", "--group", false);
  std::string hostname = getOptionValue(tokens, "-h", "--hostname", false);
  std::string comment = getOptionValue(tokens, "-m", "--comment", false);
  if(user.empty()||group.empty()||hostname.empty()||comment.empty()) {
    m_data = help.str();
    return;
  }
  cta::common::dataStructures::UserIdentity adminUser;
  adminUser.name=user;
  adminUser.group=group;
  m_catalogue->createBootstrapAdminAndHostNoAuth(cliIdentity, adminUser, hostname, comment);
}

//------------------------------------------------------------------------------
// xCom_admin
//------------------------------------------------------------------------------
void XrdProFile::xCom_admin(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ad/admin add/ch/rm/ls:" << std::endl
       << "\tadd --user/-u <user> --group/-g <group> --comment/-m <\"comment\">" << std::endl
       << "\tch  --user/-u <user> --group/-g <group> --comment/-m <\"comment\">" << std::endl
       << "\trm  --user/-u <user> --group/-g <group>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string user = getOptionValue(tokens, "-u", "--user", false);
    std::string group = getOptionValue(tokens, "-g", "--group", false);
    if(user.empty()||group.empty()) {
      m_data = help.str();
      return;
    }
    cta::common::dataStructures::UserIdentity adminUser;
    adminUser.name=user;
    adminUser.group=group;
    if("add" == tokens[2] || "ch" == tokens[2]) {
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()) {
        m_data = help.str();
        return;
      }
      if("add" == tokens[2]) { //add
        m_catalogue->createAdminUser(cliIdentity, adminUser, comment);
      }
      else { //ch
        m_catalogue->modifyAdminUserComment(cliIdentity, adminUser, comment);
      }
    }
    else { //rm
      m_catalogue->deleteAdminUser(adminUser);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::AdminUser> list= m_catalogue->getAdminUsers();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"user","group","c.user","c.group","c.host","c.time","m.user","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->user.name);
        currentRow.push_back(it->user.group);
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_adminhost
//------------------------------------------------------------------------------
void XrdProFile::xCom_adminhost(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ah/adminhost add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <host_name> --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <host_name> --comment/-m <\"comment\">" << std::endl
       << "\trm  --name/-n <host_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string hostname = getOptionValue(tokens, "-n", "--name", false);
    if(hostname.empty()) {
      m_data = help.str();
      return;
    }
    if("add" == tokens[2] || "ch" == tokens[2]) {
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()) {
        m_data = help.str();
        return;
      }
      if("add" == tokens[2]) { //add
        m_catalogue->createAdminHost(cliIdentity, hostname, comment);
      }
      else { //ch
        m_catalogue->modifyAdminHostComment(cliIdentity, hostname, comment);
      }
    }
    else { //rm
      m_catalogue->deleteAdminHost(hostname);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::AdminHost> list= m_catalogue->getAdminHosts();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"hostname","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->name);
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_tapepool
//------------------------------------------------------------------------------
void XrdProFile::xCom_tapepool(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " tp/tapepool add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <tapepool_name> --partialtapesnumber/-p <number_of_partial_tapes> [--encryption/-e or --clear/-c] --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <tapepool_name> [--partialtapesnumber/-p <number_of_partial_tapes>] [--encryption/-e or --clear/-c] [--comment/-m <\"comment\">]" << std::endl
       << "\trm  --name/-n <tapepool_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string name = getOptionValue(tokens, "-n", "--name", false);
    if(name.empty()) {
      m_data = help.str();
      return;
    }
    if("add" == tokens[2]) { //add
      std::string ptn_s = getOptionValue(tokens, "-p", "--partialtapesnumber", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()||ptn_s.empty()) {
        m_data = help.str();
        return;
      }
      std::stringstream ptn_ss(ptn_s);
      uint64_t ptn = 0;
      ptn_ss >> ptn;
      bool encryption=false;
      if((hasOption(tokens, "-e", "--encryption") && hasOption(tokens, "-c", "--clear"))) {
        m_data = help.str();
        return;
      }
      encryption=hasOption(tokens, "-e", "--encryption");
      m_catalogue->createTapePool(cliIdentity, name, ptn, encryption, comment);
    }
    else if("ch" == tokens[2]) { //ch
      std::string ptn_s = getOptionValue(tokens, "-p", "--partialtapesnumber", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()&&ptn_s.empty()) {
        m_data = help.str();
        return;
      }
      if(!comment.empty()) {
        m_catalogue->modifyTapePoolComment(cliIdentity, name, comment);
      }
      if(!ptn_s.empty()) {
        std::stringstream ptn_ss(ptn_s);
        uint64_t ptn = 0;
        ptn_ss >> ptn;
        m_catalogue->modifyTapePoolNbPartialTapes(cliIdentity, name, ptn);
      }
      if(hasOption(tokens, "-e", "--encryption")) {
        m_catalogue->setTapePoolEncryption(cliIdentity, name, true);
      }
      if(hasOption(tokens, "-c", "--clear")) {
        m_catalogue->setTapePoolEncryption(cliIdentity, name, false);
      }
    }
    else { //rm
      m_catalogue->deleteTapePool(name);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::TapePool> list= m_catalogue->getTapePools();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"name","# partial tapes","encrypt","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->name);
        currentRow.push_back(std::to_string((unsigned long long)it->nbPartialTapes));
        if(it->encryption) currentRow.push_back("true"); else currentRow.push_back("false");
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_archiveroute
//------------------------------------------------------------------------------
void XrdProFile::xCom_archiveroute(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ar/archiveroute add/ch/rm/ls:" << std::endl
       << "\tadd --storageclass/-s <storage_class_name> --copynb/-c <copy_number> --tapepool/-t <tapepool_name> --comment/-m <\"comment\">" << std::endl
       << "\tch  --storageclass/-s <storage_class_name> --copynb/-c <copy_number> [--tapepool/-t <tapepool_name>] [--comment/-m <\"comment\">]" << std::endl
       << "\trm  --storageclass/-s <storage_class_name> --copynb/-c <copy_number>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string scn = getOptionValue(tokens, "-s", "--storageclass", false);
    std::string cn_s = getOptionValue(tokens, "-c", "--copynb", false);
    if(scn.empty()||cn_s.empty()) {
      m_data = help.str();
      return;
    }    
    std::stringstream cn_ss(cn_s);
    uint64_t cn = 0;
    cn_ss >> cn;
    if("add" == tokens[2]) { //add
      std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()||tapepool.empty()) {
        m_data = help.str();
        return;
      }
      m_catalogue->createArchiveRoute(cliIdentity, scn, cn, tapepool, comment);
    }
    else if("ch" == tokens[2]) { //ch
      std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()&&tapepool.empty()) {
        m_data = help.str();
        return;
      }
      if(!comment.empty()) {
        m_catalogue->modifyArchiveRouteComment(cliIdentity, scn, cn, comment);
      }
      if(!tapepool.empty()) {
        m_catalogue->modifyArchiveRouteTapePoolName(cliIdentity, scn, cn, tapepool);
      }
    }
    else { //rm
      m_catalogue->deleteArchiveRoute(scn, cn);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::ArchiveRoute> list= m_catalogue->getArchiveRoutes();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"storage class","copy number","tapepool","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->storageClassName);
        currentRow.push_back(std::to_string((unsigned long long)it->copyNb));
        currentRow.push_back(it->tapePoolName);
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_logicallibrary
//------------------------------------------------------------------------------
void XrdProFile::xCom_logicallibrary(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ll/logicallibrary add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <logical_library_name> --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <logical_library_name> --comment/-m <\"comment\">" << std::endl
       << "\trm  --name/-n <logical_library_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string hostname = getOptionValue(tokens, "-n", "--name", false);
    if(hostname.empty()) {
      m_data = help.str();
      return;
    }
    if("add" == tokens[2] || "ch" == tokens[2]) {
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()) {
        m_data = help.str();
        return;
      }
      if("add" == tokens[2]) { //add
        m_catalogue->createLogicalLibrary(cliIdentity, hostname, comment);
      }
      else { //ch
        m_catalogue->modifyLogicalLibraryComment(cliIdentity, hostname, comment);
      }
    }
    else { //rm
      m_catalogue->deleteLogicalLibrary(hostname);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::LogicalLibrary> list= m_catalogue->getLogicalLibraries();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"name","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->name);
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_tape
//------------------------------------------------------------------------------
void XrdProFile::xCom_tape(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ta/tape add/ch/rm/reclaim/ls/label:" << std::endl
       << "\tadd     --vid/-v <vid> --logicallibrary/-l <logical_library_name> --tapepool/-t <tapepool_name> --capacity/-c <capacity_in_bytes> [--encryptionkey/-k <encryption_key>]" << std::endl
       << "\t        [--enabled/-e or --disabled/-d] [--free/-f or --full/-F] [--comment/-m <\"comment\">] " << std::endl
       << "\tch      --vid/-v <vid> [--logicallibrary/-l <logical_library_name>] [--tapepool/-t <tapepool_name>] [--capacity/-c <capacity_in_bytes>] [--encryptionkey/-k <encryption_key>]" << std::endl
       << "\t        [--enabled/-e or --disabled/-d] [--free/-f or --full/-F] [--comment/-m <\"comment\">]" << std::endl
       << "\trm      --vid/-v <vid>" << std::endl
       << "\treclaim --vid/-v <vid>" << std::endl
       << "\tls      [--header/-h] [--vid/-v <vid>] [--logicallibrary/-l <logical_library_name>] [--tapepool/-t <tapepool_name>] [--capacity/-c <capacity_in_bytes>]" << std::endl
       << "\t        [--lbp/-p or --nolbp/-P] [--enabled/-e or --disabled/-d] [--free/-f or --full/-F] [--busy/-b or --notbusy/-n]" << std::endl
       << "\tlabel   --vid/-v <vid> [--force/-f] [--lbp/-l] [--tag/-t <tag_name>]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2] || "reclaim" == tokens[2] || "label" == tokens[2]) {
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    if(vid.empty()) {
      m_data = help.str();
      return;
    }
    if("add" == tokens[2]) { //add
      std::string logicallibrary = getOptionValue(tokens, "-l", "--logicallibrary", false);
      std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
      std::string capacity_s = getOptionValue(tokens, "-c", "--capacity", false);
      if(logicallibrary.empty()||tapepool.empty()||capacity_s.empty()) {
        m_data = help.str();
        return;
      }
      std::stringstream capacity_ss(capacity_s);
      int capacity = 0;
      capacity_ss >> capacity;
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      bool disabled=false;
      bool full=false;
      if((hasOption(tokens, "-e", "--enabled") && hasOption(tokens, "-d", "--disabled")) || (hasOption(tokens, "-f", "--free") && hasOption(tokens, "-F", "--full"))) {
        m_data = help.str();
        return;
      }
      disabled=hasOption(tokens, "-d", "--disabled");
      full=hasOption(tokens, "-F", "--full");
      std::string encryptionkey = getOptionValue(tokens, "-k", "--encryptionkey", false);
      m_catalogue->createTape(cliIdentity, vid, logicallibrary, tapepool, encryptionkey, capacity, disabled, full, comment);
    }
    else if("ch" == tokens[2]) { //ch
      std::string logicallibrary = getOptionValue(tokens, "-l", "--logicallibrary", false);
      std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
      std::string capacity_s = getOptionValue(tokens, "-c", "--capacity", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      std::string encryptionkey = getOptionValue(tokens, "-k", "--encryptionkey", false);
      if(comment.empty() && logicallibrary.empty() && tapepool.empty() && capacity_s.empty() && encryptionkey.empty() && !hasOption(tokens, "-e", "--enabled")
              && !hasOption(tokens, "-d", "--disabled") && !hasOption(tokens, "-f", "--free") && !hasOption(tokens, "-F", "--full")) {
        m_data = help.str();
        return;
      }
      if((hasOption(tokens, "-e", "--enabled") && hasOption(tokens, "-d", "--disabled")) || (hasOption(tokens, "-f", "--free") && hasOption(tokens, "-F", "--full"))) {
        m_data = help.str();
        return;
      }
      if(!logicallibrary.empty()) {
        m_catalogue->modifyTapeLogicalLibraryName(cliIdentity, vid, logicallibrary);
      }
      if(!tapepool.empty()) {
        m_catalogue->modifyTapeTapePoolName(cliIdentity, vid, tapepool);
      }
      if(!capacity_s.empty()) {
        std::stringstream capacity_ss(capacity_s);
        uint64_t capacity = 0;
        capacity_ss >> capacity;
        m_catalogue->modifyTapeCapacityInBytes(cliIdentity, vid, capacity);
      }
      if(!comment.empty()) {
        m_catalogue->modifyTapeComment(cliIdentity, vid, comment);
      }
      if(!encryptionkey.empty()) {
        m_catalogue->modifyTapeEncryptionKey(cliIdentity, vid, encryptionkey);
      }
      if(hasOption(tokens, "-e", "--enabled")) {
        m_scheduler->setTapeDisabled(cliIdentity, vid, false);
      }
      if(hasOption(tokens, "-d", "--disabled")) {
        m_scheduler->setTapeDisabled(cliIdentity, vid, true);
      }
      if(hasOption(tokens, "-f", "--free")) {
        m_scheduler->setTapeFull(cliIdentity, vid, false);
      }
      if(hasOption(tokens, "-F", "--full")) {
        m_scheduler->setTapeFull(cliIdentity, vid, true);
      }
    }
    else if("reclaim" == tokens[2]) { //reclaim
      m_catalogue->reclaimTape(cliIdentity, vid);
    }
    else if("label" == tokens[2]) { //label
      m_scheduler->labelTape(cliIdentity, vid, hasOption(tokens, "-f", "--force"), hasOption(tokens, "-l", "--lbp"), getOptionValue(tokens, "-t", "--tag", false));
    }
    else { //rm
      m_catalogue->deleteTape(vid);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    std::string logicallibrary = getOptionValue(tokens, "-l", "--logicallibrary", false);
    std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
    std::string capacity = getOptionValue(tokens, "-c", "--capacity", false);
    if((hasOption(tokens, "-e", "--enabled") && hasOption(tokens, "-d", "--disabled")) 
            || (hasOption(tokens, "-f", "--free") && hasOption(tokens, "-F", "--full")) 
            || (hasOption(tokens, "-b", "--busy") && hasOption(tokens, "-n", "--notbusy"))
            || (hasOption(tokens, "-p", "--lbp") && hasOption(tokens, "-P", "--nolbp"))) {
      m_data = help.str();
      return;
    }
    std::string disabled="";
    std::string full="";
    std::string busy="";
    std::string lbp="";
    if(hasOption(tokens, "-e", "--enabled")) {
      disabled = "false";
    }
    if(hasOption(tokens, "-d", "--disabled")) {
      disabled = "true";
    }
    if(hasOption(tokens, "-f", "--free")) {
      full = "false";
    }
    if(hasOption(tokens, "-F", "--full")) {
      full = "true";
    }
    if(hasOption(tokens, "-b", "--busy")) {
      busy = "false";
    }
    if(hasOption(tokens, "-n", "--notbusy")) {
      busy = "true";
    }
    if(hasOption(tokens, "-p", "--lbp")) {
      lbp = "false";
    }
    if(hasOption(tokens, "-P", "--nolbp")) {
      lbp = "true";
    }
    std::list<cta::common::dataStructures::Tape> list= m_catalogue->getTapes(vid, logicallibrary, tapepool, capacity, disabled, full, busy, lbp);
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"vid","logical library","tapepool","encription key","capacity","occupancy","last fseq","busy","full","disabled","lpb","label drive","label time",
                                         "last w drive","last w time","last r drive","last r time","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->vid);
        currentRow.push_back(it->logicalLibraryName);
        currentRow.push_back(it->tapePoolName);
        currentRow.push_back(it->encryptionKey);
        currentRow.push_back(std::to_string((unsigned long long)it->capacityInBytes));
        currentRow.push_back(std::to_string((unsigned long long)it->dataOnTapeInBytes));
        currentRow.push_back(std::to_string((unsigned long long)it->lastFSeq));
        if(it->busy) currentRow.push_back("true"); else currentRow.push_back("false");
        if(it->full) currentRow.push_back("true"); else currentRow.push_back("false");
        if(it->disabled) currentRow.push_back("true"); else currentRow.push_back("false");
        if(it->lbp) currentRow.push_back("true"); else currentRow.push_back("false");
        currentRow.push_back(it->labelLog.drive);
        currentRow.push_back(std::to_string((unsigned long long)it->labelLog.time));
        currentRow.push_back(it->lastWriteLog.drive);
        currentRow.push_back(std::to_string((unsigned long long)it->lastWriteLog.time));
        currentRow.push_back(it->lastReadLog.drive);
        currentRow.push_back(std::to_string((unsigned long long)it->lastReadLog.time));
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_storageclass
//------------------------------------------------------------------------------
void XrdProFile::xCom_storageclass(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " sc/storageclass add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <storage_class_name> --copynb/-c <number_of_tape_copies> --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <storage_class_name> [--copynb/-c <number_of_tape_copies>] [--comment/-m <\"comment\">]" << std::endl
       << "\trm  --name/-n <storage_class_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string scn = getOptionValue(tokens, "-n", "--name", false);
    if(scn.empty()) {
      m_data = help.str();
      return;
    }  
    if("add" == tokens[2]) { //add
      std::string cn_s = getOptionValue(tokens, "-c", "--copynb", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()||cn_s.empty()) {
        m_data = help.str();
        return;
      }  
      std::stringstream cn_ss(cn_s);
      uint64_t cn = 0;
      cn_ss >> cn;
      m_catalogue->createStorageClass(cliIdentity, scn, cn, comment);
    }
    else if("ch" == tokens[2]) { //ch
      std::string cn_s = getOptionValue(tokens, "-c", "--copynb", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()&&cn_s.empty()) {
        m_data = help.str();
        return;
      }
      if(!comment.empty()) {
        m_catalogue->modifyStorageClassComment(cliIdentity, scn, comment);
      }
      if(!cn_s.empty()) {  
        std::stringstream cn_ss(cn_s);
        uint64_t cn = 0;
        cn_ss >> cn;
        m_catalogue->modifyStorageClassNbCopies(cliIdentity, scn, cn);
      }
    }
    else { //rm
      m_catalogue->deleteStorageClass(scn);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::StorageClass> list= m_catalogue->getStorageClasses();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"storage class","number of copies","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->name);
        currentRow.push_back(std::to_string((unsigned long long)it->nbCopies));
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_user
//------------------------------------------------------------------------------
void XrdProFile::xCom_user(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " us/user add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <user_name> --group/-g <group_name> --mountgroup/-u <mount_group_name> --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <user_name> --group/-g <group_name> [--mountgroup/-u <mount_group_name>] [--comment/-m <\"comment\">]" << std::endl
       << "\trm  --name/-n <user_name> --group/-g <group_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string user = getOptionValue(tokens, "-n", "--name", false);
    std::string group = getOptionValue(tokens, "-g", "--group", false);
    if(user.empty()||group.empty()) {
      m_data = help.str();
      return;
    }  
    if("add" == tokens[2]) { //add
      std::string mountgroup = getOptionValue(tokens, "-u", "--mountgroup", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()||mountgroup.empty()) {
        m_data = help.str();
        return;
      }
      m_catalogue->createUser(cliIdentity, user, group, mountgroup, comment);
    }
    else if("ch" == tokens[2]) { //ch
      std::string mountgroup = getOptionValue(tokens, "-u", "--mountgroup", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if(comment.empty()&&mountgroup.empty()) {
        m_data = help.str();
        return;
      }
      if(!comment.empty()) {
        m_catalogue->modifyUserComment(cliIdentity, user, group, comment);
      }
      if(!mountgroup.empty()) {
        m_catalogue->modifyUserMountGroup(cliIdentity, user, group, mountgroup);
      }
    }
    else { //rm
      m_catalogue->deleteUser(user, group);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::User> list= m_catalogue->getUsers();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"user","group","cta group","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->name);
        currentRow.push_back(it->group);
        currentRow.push_back(it->mountGroupName);
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_mountgroup
//------------------------------------------------------------------------------
void XrdProFile::xCom_mountgroup(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " mg/mountgroup add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <mountgroup_name> --archivepriority/--ap <priority_value> --minarchivefilesqueued/--af <minFilesQueued> --minarchivebytesqueued/--ab <minBytesQueued> " << std::endl
       << "\t    --minarchiverequestage/--aa <minRequestAge> --retrievepriority/--rp <priority_value> --minretrievefilesqueued/--rf <minFilesQueued> " << std::endl
       << "\t    --minretrievebytesqueued/--rb <minBytesQueued> --minretrieverequestage/--ra <minRequestAge> --maxdrivesallowed/-d <maxDrivesAllowed> --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <mountgroup_name> [--archivepriority/--ap <priority_value>] [--minarchivefilesqueued/--af <minFilesQueued>] [--minarchivebytesqueued/--ab <minBytesQueued>] " << std::endl
       << "\t   [--minarchiverequestage/--aa <minRequestAge>] [--retrievepriority/--rp <priority_value>] [--minretrievefilesqueued/--rf <minFilesQueued>] " << std::endl
       << "\t   [--minretrievebytesqueued/--rb <minBytesQueued>] [--minretrieverequestage/--ra <minRequestAge>] [--maxdrivesallowed/-d <maxDrivesAllowed>] [--comment/-m <\"comment\">]" << std::endl
       << "\trm  --name/-n <mountgroup_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string group = getOptionValue(tokens, "-n", "--name", false);
    if(group.empty()) {
      m_data = help.str();
      return;
    }
    if("add" == tokens[2] || "ch" == tokens[2]) {      
      std::string archivepriority_s = getOptionValue(tokens, "--ap", "--archivepriority", false);
      std::string minarchivefilesqueued_s = getOptionValue(tokens, "--af", "--minarchivefilesqueued", false);
      std::string minarchivebytesqueued_s = getOptionValue(tokens, "--ab", "--minarchivebytesqueued", false);
      std::string minarchiverequestage_s = getOptionValue(tokens, "--aa", "--minarchiverequestage", false);
      std::string retrievepriority_s = getOptionValue(tokens, "--rp", "--retrievepriority", false);
      std::string minretrievefilesqueued_s = getOptionValue(tokens, "--rf", "--minretrievefilesqueued", false);
      std::string minretrievebytesqueued_s = getOptionValue(tokens, "--rb", "--minretrievebytesqueued", false);
      std::string minretrieverequestage_s = getOptionValue(tokens, "--ra", "--minretrieverequestage", false);
      std::string maxdrivesallowed_s = getOptionValue(tokens, "-d", "--maxdrivesallowed", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if("add" == tokens[2]) { //add
        if(archivepriority_s.empty()||minarchivefilesqueued_s.empty()||minarchivebytesqueued_s.empty()||minarchiverequestage_s.empty()||retrievepriority_s.empty()
                ||minretrievefilesqueued_s.empty()||minretrievebytesqueued_s.empty()||minretrieverequestage_s.empty()||maxdrivesallowed_s.empty()||comment.empty()) {
          m_data = help.str();
          return;
        }
        uint64_t archivepriority; std::stringstream archivepriority_ss; archivepriority_ss << archivepriority_s; archivepriority_ss >> archivepriority;
        uint64_t minarchivefilesqueued; std::stringstream minarchivefilesqueued_ss; minarchivefilesqueued_ss << minarchivefilesqueued_s; minarchivefilesqueued_ss >> minarchivefilesqueued;
        uint64_t minarchivebytesqueued; std::stringstream minarchivebytesqueued_ss; minarchivebytesqueued_ss << minarchivebytesqueued_s; minarchivebytesqueued_ss >> minarchivebytesqueued;
        uint64_t minarchiverequestage; std::stringstream minarchiverequestage_ss; minarchiverequestage_ss << minarchiverequestage_s; minarchiverequestage_ss >> minarchiverequestage;
        uint64_t retrievepriority; std::stringstream retrievepriority_ss; retrievepriority_ss << retrievepriority_s; retrievepriority_ss >> retrievepriority;
        uint64_t minretrievefilesqueued; std::stringstream minretrievefilesqueued_ss; minretrievefilesqueued_ss << minretrievefilesqueued_s; minretrievefilesqueued_ss >> minretrievefilesqueued;
        uint64_t minretrievebytesqueued; std::stringstream minretrievebytesqueued_ss; minretrievebytesqueued_ss << minretrievebytesqueued_s; minretrievebytesqueued_ss >> minretrievebytesqueued;
        uint64_t minretrieverequestage; std::stringstream minretrieverequestage_ss; minretrieverequestage_ss << minretrieverequestage_s; minretrieverequestage_ss >> minretrieverequestage;
        uint64_t maxdrivesallowed; std::stringstream maxdrivesallowed_ss; maxdrivesallowed_ss << maxdrivesallowed_s; maxdrivesallowed_ss >> maxdrivesallowed;
        m_catalogue->createMountGroup(cliIdentity, group, archivepriority, minarchivefilesqueued, minarchivebytesqueued, minarchiverequestage, retrievepriority, minretrievefilesqueued, minretrievebytesqueued, minretrieverequestage, maxdrivesallowed, comment);
      }
      else if("ch" == tokens[2]) { //ch
        if(archivepriority_s.empty()&&minarchivefilesqueued_s.empty()&&minarchivebytesqueued_s.empty()&&minarchiverequestage_s.empty()&&retrievepriority_s.empty()
                &&minretrievefilesqueued_s.empty()&&minretrievebytesqueued_s.empty()&&minretrieverequestage_s.empty()&&maxdrivesallowed_s.empty()&&comment.empty()) {
          m_data = help.str();
          return;
        }
        if(!archivepriority_s.empty()) {
          uint64_t archivepriority; std::stringstream archivepriority_ss; archivepriority_ss << archivepriority_s; archivepriority_ss >> archivepriority;
          m_catalogue->modifyMountGroupArchivePriority(cliIdentity, group, archivepriority);
        }
        if(!minarchivefilesqueued_s.empty()) {
          uint64_t minarchivefilesqueued; std::stringstream minarchivefilesqueued_ss; minarchivefilesqueued_ss << minarchivefilesqueued_s; minarchivefilesqueued_ss >> minarchivefilesqueued;
          m_catalogue->modifyMountGroupArchiveMinFilesQueued(cliIdentity, group, minarchivefilesqueued);
        }
        if(!minarchivebytesqueued_s.empty()) {
          uint64_t minarchivebytesqueued; std::stringstream minarchivebytesqueued_ss; minarchivebytesqueued_ss << minarchivebytesqueued_s; minarchivebytesqueued_ss >> minarchivebytesqueued;
          m_catalogue->modifyMountGroupArchiveMinBytesQueued(cliIdentity, group, minarchivebytesqueued);
        }
        if(!minarchiverequestage_s.empty()) {
          uint64_t minarchiverequestage; std::stringstream minarchiverequestage_ss; minarchiverequestage_ss << minarchiverequestage_s; minarchiverequestage_ss >> minarchiverequestage;
          m_catalogue->modifyMountGroupArchiveMinRequestAge(cliIdentity, group, minarchiverequestage);
        }
        if(!retrievepriority_s.empty()) {
          uint64_t retrievepriority; std::stringstream retrievepriority_ss; retrievepriority_ss << retrievepriority_s; retrievepriority_ss >> retrievepriority;
          m_catalogue->modifyMountGroupRetrievePriority(cliIdentity, group, retrievepriority);
        }
        if(!minretrievefilesqueued_s.empty()) {
          uint64_t minretrievefilesqueued; std::stringstream minretrievefilesqueued_ss; minretrievefilesqueued_ss << minretrievefilesqueued_s; minretrievefilesqueued_ss >> minretrievefilesqueued;
          m_catalogue->modifyMountGroupRetrieveMinFilesQueued(cliIdentity, group, minretrievefilesqueued);
        }
        if(!minretrievebytesqueued_s.empty()) {
          uint64_t minretrievebytesqueued; std::stringstream minretrievebytesqueued_ss; minretrievebytesqueued_ss << minretrievebytesqueued_s; minretrievebytesqueued_ss >> minretrievebytesqueued;
          m_catalogue->modifyMountGroupRetrieveMinBytesQueued(cliIdentity, group, minretrievebytesqueued);
        }
        if(!minretrieverequestage_s.empty()) {
          uint64_t minretrieverequestage; std::stringstream minretrieverequestage_ss; minretrieverequestage_ss << minretrieverequestage_s; minretrieverequestage_ss >> minretrieverequestage;
          m_catalogue->modifyMountGroupRetrieveMinRequestAge(cliIdentity, group, minretrieverequestage);
        }
        if(!maxdrivesallowed_s.empty()) {
          uint64_t maxdrivesallowed; std::stringstream maxdrivesallowed_ss; maxdrivesallowed_ss << maxdrivesallowed_s; maxdrivesallowed_ss >> maxdrivesallowed;
          m_catalogue->modifyMountGroupMaxDrivesAllowed(cliIdentity, group, maxdrivesallowed);
        }
        if(!comment.empty()) {
          m_catalogue->modifyMountGroupComment(cliIdentity, group, comment);
        }
      }
    }
    else { //rm
      m_catalogue->deleteMountGroup(group);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::MountGroup> list= m_catalogue->getMountGroups();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"cta group","a.priority","a.minFiles","a.minBytes","a.minAge","r.priority","r.minFiles","r.minBytes","r.minAge","MaxDrives","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->name);
        currentRow.push_back(std::to_string((unsigned long long)it->archive_priority));
        currentRow.push_back(std::to_string((unsigned long long)it->archive_minFilesQueued));
        currentRow.push_back(std::to_string((unsigned long long)it->archive_minBytesQueued));
        currentRow.push_back(std::to_string((unsigned long long)it->archive_minRequestAge));
        currentRow.push_back(std::to_string((unsigned long long)it->retrieve_priority));
        currentRow.push_back(std::to_string((unsigned long long)it->retrieve_minFilesQueued));
        currentRow.push_back(std::to_string((unsigned long long)it->retrieve_minBytesQueued));
        currentRow.push_back(std::to_string((unsigned long long)it->retrieve_minRequestAge));
        currentRow.push_back(std::to_string((unsigned long long)it->maxDrivesAllowed));
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_dedication
//------------------------------------------------------------------------------
void XrdProFile::xCom_dedication(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " de/dedication add/ch/rm/ls:" << std::endl
       << "\tadd --name/-n <drive_name> [--readonly/-r or --writeonly/-w] [--mountgroup/-u <mount_group_name>] [--vid/-v <tape_vid>] [--tag/-t <tag_name>] --from/-f <DD/MM/YYYY> --until/-u <DD/MM/YYYY> --comment/-m <\"comment\">" << std::endl
       << "\tch  --name/-n <drive_name> [--readonly/-r or --writeonly/-w] [--mountgroup/-u <mount_group_name>] [--vid/-v <tape_vid>] [--tag/-t <tag_name>] [--from/-f <DD/MM/YYYY>] [--until/-u <DD/MM/YYYY>] [--comment/-m <\"comment\">]" << std::endl
       << "\trm  --name/-n <drive_name>" << std::endl
       << "\tls  [--header/-h]" << std::endl;
  if("add" == tokens[2] || "ch" == tokens[2] || "rm" == tokens[2]) {
    std::string drive = getOptionValue(tokens, "-n", "--name", false);
    if(drive.empty()) {
      m_data = help.str();
      return;
    } 
    if("add" == tokens[2] || "ch" == tokens[2]) {
      bool readonly = hasOption(tokens, "-r", "--readonly");
      bool writeonly = hasOption(tokens, "-w", "--writeonly");
      std::string mountgroup = getOptionValue(tokens, "-u", "--mountgroup", false);
      std::string vid = getOptionValue(tokens, "-v", "--vid", false);
      std::string tag = getOptionValue(tokens, "-t", "--tag", false);
      std::string from_s = getOptionValue(tokens, "-f", "--from", false);
      std::string until_s = getOptionValue(tokens, "-u", "--until", false);
      std::string comment = getOptionValue(tokens, "-m", "--comment", false);
      if("add" == tokens[2]) { //add
        if(comment.empty()||from_s.empty()||until_s.empty()||(mountgroup.empty()&&vid.empty()&&tag.empty()&&!readonly&&!writeonly)||(readonly&&writeonly)) {
          m_data = help.str();
          return;
        }
        struct tm time;
        if(NULL==strptime(from_s.c_str(), "%d/%m/%Y", &time)) {
          m_data = help.str();
          return;
        }
        time_t from = mktime(&time);  // timestamp in current timezone
        if(NULL==strptime(until_s.c_str(), "%d/%m/%Y", &time)) {
          m_data = help.str();
          return;
        }
        time_t until = mktime(&time);  // timestamp in current timezone
        cta::common::dataStructures::DedicationType type=cta::common::dataStructures::DedicationType::readwrite;
        if(readonly) {
          type=cta::common::dataStructures::DedicationType::readonly;
        }
        else if(writeonly) {
          type=cta::common::dataStructures::DedicationType::writeonly;
        }
        m_catalogue->createDedication(cliIdentity, drive, type, mountgroup, tag, vid, from, until, comment);
      }
      else if("ch" == tokens[2]) { //ch
        if((comment.empty()&&from_s.empty()&&until_s.empty()&&mountgroup.empty()&&vid.empty()&&tag.empty()&&!readonly&&!writeonly)||(readonly&&writeonly)) {
          m_data = help.str();
          return;
        }
        if(!comment.empty()) {
          m_catalogue->modifyDedicationComment(cliIdentity, drive, comment);
        }
        if(!from_s.empty()) {
          struct tm time;
          if(NULL==strptime(from_s.c_str(), "%d/%m/%Y", &time)) {
            m_data = help.str();
            return;
          }
          time_t from = mktime(&time);  // timestamp in current timezone
          m_catalogue->modifyDedicationFrom(cliIdentity, drive, from);
        }
        if(!until_s.empty()) {
          struct tm time;
          if(NULL==strptime(until_s.c_str(), "%d/%m/%Y", &time)) {
            m_data = help.str();
            return;
          }
          time_t until = mktime(&time);  // timestamp in current timezone
          m_catalogue->modifyDedicationUntil(cliIdentity, drive, until);
        }
        if(!mountgroup.empty()) {
          m_catalogue->modifyDedicationMountGroup(cliIdentity, drive, mountgroup);
        }
        if(!vid.empty()) {
          m_catalogue->modifyDedicationVid(cliIdentity, drive, vid);
        }
        if(!tag.empty()) {
          m_catalogue->modifyDedicationTag(cliIdentity, drive, tag);
        }
        if(readonly) {
          m_catalogue->modifyDedicationType(cliIdentity, drive, cta::common::dataStructures::DedicationType::readonly);          
        }
        if(writeonly) {
          m_catalogue->modifyDedicationType(cliIdentity, drive, cta::common::dataStructures::DedicationType::writeonly);
        }
      }
    }
    else { //rm
      m_catalogue->deleteDedication(drive);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::Dedication> list= m_catalogue->getDedications();
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"drive","type","vid","user group","tag","from","until","c.name","c.group","c.host","c.time","m.name","m.group","m.host","m.time","comment"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        std::string type_s;
        switch(it->dedicationType) {
          case cta::common::dataStructures::DedicationType::readonly:
            type_s = "readonly";
            break;
          case cta::common::dataStructures::DedicationType::writeonly:
            type_s = "writeonly";
            break;
          default:
            type_s = "readwrite";
            break;
        }     
        currentRow.push_back(it->driveName);
        currentRow.push_back(type_s);
        currentRow.push_back(it->vid);
        currentRow.push_back(it->mountGroup);
        currentRow.push_back(it->tag);
        currentRow.push_back(timeToString(it->fromTimestamp));
        currentRow.push_back(timeToString(it->untilTimestamp));
        addLogInfoToResponseRow(currentRow, it->creationLog, it->lastModificationLog);
        currentRow.push_back(it->comment);
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_repack
//------------------------------------------------------------------------------
void XrdProFile::xCom_repack(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " re/repack add/rm/ls/err:" << std::endl
       << "\tadd --vid/-v <vid> [--justexpand/-e or --justrepack/-r] [--tag/-t <tag_name>]" << std::endl
       << "\trm  --vid/-v <vid>" << std::endl
       << "\tls  [--header/-h] [--vid/-v <vid>]" << std::endl
       << "\terr --vid/-v <vid>" << std::endl;
  if("add" == tokens[2] || "err" == tokens[2] || "rm" == tokens[2]) {
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    if(vid.empty()) {
      m_data = help.str();
      return;
    }  
    if("add" == tokens[2]) { //add
      std::string tag = getOptionValue(tokens, "-t", "--tag", false);
      bool justexpand = hasOption(tokens, "-e", "--justexpand");
      bool justrepack = hasOption(tokens, "-r", "--justrepack");
      if(justexpand&&justrepack) {
        m_data = help.str();
        return;
      }
      cta::common::dataStructures::RepackType type=cta::common::dataStructures::RepackType::expandandrepack;
      if(justexpand) {
        type=cta::common::dataStructures::RepackType::justexpand;
      }
      if(justrepack) {
        type=cta::common::dataStructures::RepackType::justrepack;
      }
      m_scheduler->repack(cliIdentity, vid, tag, type);
    }
    else if("err" == tokens[2]) { //err
      cta::common::dataStructures::RepackInfo info = m_scheduler->getRepack(cliIdentity, vid);
      if(info.errors.size()>0) {
        std::vector<std::vector<std::string>> responseTable;
        std::vector<std::string> header = {"fseq","error message"};
        if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
        for(auto it = info.errors.cbegin(); it != info.errors.cend(); it++) {
          std::vector<std::string> currentRow;
          currentRow.push_back(std::to_string((unsigned long long)it->first));
          currentRow.push_back(it->second);
          responseTable.push_back(currentRow);
        }
        m_data = formatResponse(responseTable);
      }
    }
    else { //rm
      m_scheduler->cancelRepack(cliIdentity, vid);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::RepackInfo> list;
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    if(vid.empty()) {      
      list = m_scheduler->getRepacks(cliIdentity);
    }
    else {
      list.push_back(m_scheduler->getRepack(cliIdentity, vid));
    }
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"vid","files","size","type","tag","to retrieve","to archive","failed","archived","status","name","group","host","time"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::string type_s;
        switch(it->repackType) {
          case cta::common::dataStructures::RepackType::expandandrepack:
            type_s = "expandandrepack";
            break;
          case cta::common::dataStructures::RepackType::justexpand:
            type_s = "justexpand";
            break;
          case cta::common::dataStructures::RepackType::justrepack:
            type_s = "justrepack";
            break;
        }
        std::vector<std::string> currentRow;
        currentRow.push_back(it->vid);
        currentRow.push_back(std::to_string((unsigned long long)it->totalFiles));
        currentRow.push_back(std::to_string((unsigned long long)it->totalSize));
        currentRow.push_back(type_s);
        currentRow.push_back(it->tag);
        currentRow.push_back(std::to_string((unsigned long long)it->filesToRetrieve));//change names
        currentRow.push_back(std::to_string((unsigned long long)it->filesToArchive));
        currentRow.push_back(std::to_string((unsigned long long)it->filesFailed));
        currentRow.push_back(std::to_string((unsigned long long)it->filesArchived));
        currentRow.push_back(it->repackStatus);
        currentRow.push_back(it->creationLog.user.name);
        currentRow.push_back(it->creationLog.user.group);
        currentRow.push_back(it->creationLog.host);        
        currentRow.push_back(timeToString(it->creationLog.time));
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_shrink
//------------------------------------------------------------------------------
void XrdProFile::xCom_shrink(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " sh/shrink --tapepool/-t <tapepool_name>" << std::endl;
  std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
  if(tapepool.empty()) {
    m_data = help.str();
    return;
  }
  m_scheduler->shrink(cliIdentity, tapepool);
}

//------------------------------------------------------------------------------
// xCom_verify
//------------------------------------------------------------------------------
void XrdProFile::xCom_verify(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ve/verify add/rm/ls/err:" << std::endl
       << "\tadd [--vid/-v <vid>] [--complete/-c or --partial/-p <number_of_files_per_tape>] [--tag/-t <tag_name>]" << std::endl
       << "\trm  [--vid/-v <vid>]" << std::endl
       << "\tls  [--header/-h] [--vid/-v <vid>]" << std::endl
       << "\terr --vid/-v <vid>" << std::endl;
  if("add" == tokens[2] || "err" == tokens[2] || "rm" == tokens[2]) {
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    if(vid.empty()) {
      m_data = help.str();
      return;
    }  
    if("add" == tokens[2]) { //add
      std::string tag = getOptionValue(tokens, "-t", "--tag", false);
      std::string numberOfFiles_s = getOptionValue(tokens, "-p", "--partial", false);
      bool complete = hasOption(tokens, "-c", "--complete");
      if(complete&&!numberOfFiles_s.empty()) {
        m_data = help.str();
        return;
      }
      uint64_t numberOfFiles=0; //0 means do a complete verification
      if(!numberOfFiles_s.empty()) {
        std::stringstream numberOfFiles_ss;
        numberOfFiles_ss << numberOfFiles_s;
        numberOfFiles_ss >> numberOfFiles;
      }
      m_scheduler->verify(cliIdentity, vid, tag, numberOfFiles);
    }
    else if("err" == tokens[2]) { //err
      cta::common::dataStructures::VerifyInfo info = m_scheduler->getVerify(cliIdentity, vid);
      if(info.errors.size()>0) {
        std::vector<std::vector<std::string>> responseTable;
        std::vector<std::string> header = {"fseq","error message"};
        if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
        for(auto it = info.errors.cbegin(); it != info.errors.cend(); it++) {
          std::vector<std::string> currentRow;
          currentRow.push_back(std::to_string((unsigned long long)it->first));
          currentRow.push_back(it->second);
          responseTable.push_back(currentRow);
        }
        m_data = formatResponse(responseTable);
      }
    }
    else { //rm
      m_scheduler->cancelVerify(cliIdentity, vid);
    }
  }
  else if("ls" == tokens[2]) { //ls
    std::list<cta::common::dataStructures::VerifyInfo> list;
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    if(vid.empty()) {      
      list = m_scheduler->getVerifys(cliIdentity);
    }
    else {
      list.push_back(m_scheduler->getVerify(cliIdentity, vid));
    }
    if(list.size()>0) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"vid","files","size","tag","to verify","failed","verified","status","name","group","host","time"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = list.cbegin(); it != list.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->vid);
        currentRow.push_back(std::to_string((unsigned long long)it->totalFiles));
        currentRow.push_back(std::to_string((unsigned long long)it->totalSize));
        currentRow.push_back(it->tag);
        currentRow.push_back(std::to_string((unsigned long long)it->filesToVerify));
        currentRow.push_back(std::to_string((unsigned long long)it->filesFailed));
        currentRow.push_back(std::to_string((unsigned long long)it->filesVerified));
        currentRow.push_back(it->verifyStatus);
        currentRow.push_back(it->creationLog.user.name);
        currentRow.push_back(it->creationLog.user.group);
        currentRow.push_back(it->creationLog.host);       
        currentRow.push_back(timeToString(it->creationLog.time));
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_archivefile
//------------------------------------------------------------------------------
void XrdProFile::xCom_archivefile(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " af/archivefile ls [--header/-h] [--id/-I <archive_file_id>] [--eosid/-e <eos_id>] [--copynb/-c <copy_no>] [--vid/-v <vid>] [--tapepool/-t <tapepool>] "
          "[--owner/-o <owner>] [--group/-g <group>] [--storageclass/-s <class>] [--path/-p <fullpath>] [--summary/-S] [--all/-a] (default gives error)" << std::endl;
  if("ls" == tokens[2]) { //ls
    std::string id_s = getOptionValue(tokens, "-I", "--id", false);
    std::string eosid = getOptionValue(tokens, "-e", "--eosid", false);
    std::string copynb = getOptionValue(tokens, "-c", "--copynb", false);
    std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
    std::string vid = getOptionValue(tokens, "-v", "--vid", false);
    std::string owner = getOptionValue(tokens, "-o", "--owner", false);
    std::string group = getOptionValue(tokens, "-g", "--group", false);
    std::string storageclass = getOptionValue(tokens, "-s", "--storageclass", false);
    std::string path = getOptionValue(tokens, "-p", "--path", false);
    bool summary = hasOption(tokens, "-S", "--summary");
    bool all = hasOption(tokens, "-a", "--all");
    if(!all && (id_s.empty() && eosid.empty() && copynb.empty() && tapepool.empty() && vid.empty() && owner.empty() && group.empty() && storageclass.empty() && path.empty())) {
      m_data = help.str();
      return;
    }
    uint64_t id; std::stringstream id_ss; id_ss << id_s; id_ss >> id;
    if(!summary) {
      std::list<cta::common::dataStructures::ArchiveFile> list=m_catalogue->getArchiveFiles(id, eosid, copynb, tapepool, vid, owner, group, storageclass, path);
      if(list.size()>0) {
        std::vector<std::vector<std::string>> responseTable;
        std::vector<std::string> header = {"id","copy no","vid","fseq","block id","EOS id","size","checksum type","checksum value","storage class","owner","group","instance","path"};
        if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
        for(auto it = list.cbegin(); it != list.cend(); it++) {
          for(auto jt = it->tapeCopies.cbegin(); jt != it->tapeCopies.cend(); jt++) {
            std::vector<std::string> currentRow;
            currentRow.push_back(std::to_string((unsigned long long)it->archiveFileID));
            currentRow.push_back(std::to_string((unsigned long long)jt->first));
            currentRow.push_back(jt->second.vid);
            currentRow.push_back(std::to_string((unsigned long long)jt->second.fSeq));
            currentRow.push_back(std::to_string((unsigned long long)jt->second.blockId));
            currentRow.push_back(it->eosFileID);
            currentRow.push_back(std::to_string((unsigned long long)it->fileSize));
            currentRow.push_back(it->checksumType);
            currentRow.push_back(it->checksumValue);
            currentRow.push_back(it->storageClass);
            currentRow.push_back(it->drData.drOwner);
            currentRow.push_back(it->drData.drGroup);
            currentRow.push_back(it->drData.drInstance);
            currentRow.push_back(it->drData.drPath);          
            responseTable.push_back(currentRow);
          }
        }
        m_data = formatResponse(responseTable);
      }
    }
    else { //summary
      cta::common::dataStructures::ArchiveFileSummary summary=m_catalogue->getArchiveFileSummary(id, eosid, copynb, tapepool, vid, owner, group, storageclass, path);
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"total number of files","total size"};
      std::vector<std::string> row = {std::to_string((unsigned long long)summary.totalFiles),std::to_string((unsigned long long)summary.totalBytes)};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);
      responseTable.push_back(row);
    }
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_test
//------------------------------------------------------------------------------
void XrdProFile::xCom_test(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " te/test read/write/write_auto (to be run on an empty self-dedicated drive; it is a synchronous command that returns performance stats and errors; all locations are local to the tapeserver):" << std::endl
       << "\tread  --drive/-d <drive_name> --vid/-v <vid> --firstfseq/-f <first_fseq> --lastfseq/-l <last_fseq> --checkchecksum/-c --output/-o <\"null\" or output_dir> [--tag/-t <tag_name>]" << std::endl
       << "\twrite --drive/-d <drive_name> --vid/-v <vid> --file/-f <filename> [--tag/-t <tag_name>]" << std::endl
       << "\twrite_auto --drive/-d <drive_name> --vid/-v <vid> --number/-n <number_of_files> --size/-s <file_size> --input/-i <\"zero\" or \"urandom\"> [--tag/-t <tag_name>]" << std::endl;
  std::string drive = getOptionValue(tokens, "-d", "--drive", false);
  std::string vid = getOptionValue(tokens, "-v", "--vid", false);
  if(vid.empty() || drive.empty()) {
    m_data = help.str();
    return;
  }
  std::string tag = getOptionValue(tokens, "-t", "--tag", false);
  if("read" == tokens[2]) {
    std::string firstfseq_s = getOptionValue(tokens, "-f", "--firstfseq", false);
    std::string lastfseq_s = getOptionValue(tokens, "-l", "--lastfseq", false);
    std::string output = getOptionValue(tokens, "-o", "--output", false);
    if(firstfseq_s.empty() || lastfseq_s.empty() || output.empty()) {
      m_data = help.str();
      return;
    }    
    bool checkchecksum = hasOption(tokens, "-c", "--checkchecksum");
    uint64_t firstfseq; std::stringstream firstfseq_ss; firstfseq_ss << firstfseq_s; firstfseq_ss >> firstfseq;
    uint64_t lastfseq; std::stringstream lastfseq_ss; lastfseq_ss << lastfseq_s; lastfseq_ss >> lastfseq;
    cta::common::dataStructures::ReadTestResult res = m_scheduler->readTest(cliIdentity, drive, vid, firstfseq, lastfseq, checkchecksum, output, tag);   
    std::vector<std::vector<std::string>> responseTable;
    std::vector<std::string> header = {"fseq","checksum type","checksum value","error"};
    responseTable.push_back(header);
    for(auto it = res.checksums.cbegin(); it != res.checksums.cend(); it++) {
      std::vector<std::string> currentRow;
      currentRow.push_back(std::to_string((unsigned long long)it->first));
      currentRow.push_back(it->second.first);
      currentRow.push_back(it->second.second);
      if(res.errors.find(it->first) != res.errors.cend()) {
        currentRow.push_back(res.errors.at(it->first));
      }
      else {
        currentRow.push_back("-");
      }
      responseTable.push_back(currentRow);
    }
    m_data = formatResponse(responseTable);
    std::stringstream ss;
    ss << std::endl << "Drive: " << res.driveName << " Vid: " << res.vid << " #Files: " << res.totalFilesRead << " #Bytes: " << res.totalBytesRead 
       << " Time: " << res.totalTimeInSeconds << " s Speed(avg): " << (long double)res.totalBytesRead/(long double)res.totalTimeInSeconds << " B/s" <<std::endl;
    m_data += ss.str();   
  }
  else if("write" == tokens[2] || "write_auto" == tokens[2]) {
    cta::common::dataStructures::WriteTestResult res;
    if("write" == tokens[2]) { //write
      std::string file = getOptionValue(tokens, "-f", "--file", false);
      if(file.empty()) {
        m_data = help.str();
        return;
      }  
      res = m_scheduler->writeTest(cliIdentity, drive, vid, file, tag);
    }
    else { //write_auto
      std::string number_s = getOptionValue(tokens, "-n", "--number", false);
      std::string size_s = getOptionValue(tokens, "-s", "--size", false);
      std::string input = getOptionValue(tokens, "-i", "--input", false);
      if(number_s.empty()||size_s.empty()||(input!="zero"&&input!="urandom")) {
        m_data = help.str();
        return;
      }
      uint64_t number; std::stringstream number_ss; number_ss << number_s; number_ss >> number;
      uint64_t size; std::stringstream size_ss; size_ss << size_s; size_ss >> size;
      cta::common::dataStructures::TestSourceType type;
      if(input=="zero") { //zero
        type = cta::common::dataStructures::TestSourceType::devzero;
      }
      else { //urandom
        type = cta::common::dataStructures::TestSourceType::devurandom;
      }
      res = m_scheduler->write_autoTest(cliIdentity, drive, vid, number, size, type, tag);
    }
    std::vector<std::vector<std::string>> responseTable;
    std::vector<std::string> header = {"fseq","checksum type","checksum value","error"};
    responseTable.push_back(header);
    for(auto it = res.checksums.cbegin(); it != res.checksums.cend(); it++) {
      std::vector<std::string> currentRow;
      currentRow.push_back(std::to_string((unsigned long long)it->first));
      currentRow.push_back(it->second.first);
      currentRow.push_back(it->second.second);
      if(res.errors.find(it->first) != res.errors.cend()) {
        currentRow.push_back(res.errors.at(it->first));
      }
      else {
        currentRow.push_back("-");
      }
      responseTable.push_back(currentRow);
    }
    m_data = formatResponse(responseTable);
    std::stringstream ss;
    ss << std::endl << "Drive: " << res.driveName << " Vid: " << res.vid << " #Files: " << res.totalFilesWritten << " #Bytes: " << res.totalBytesWritten 
       << " Time: " << res.totalTimeInSeconds << " s Speed(avg): " << (long double)res.totalBytesWritten/(long double)res.totalTimeInSeconds << " B/s" <<std::endl;
    m_data += ss.str();    
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_drive
//------------------------------------------------------------------------------
void XrdProFile::xCom_drive(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " dr/drive up/down (it is a synchronous command):" << std::endl
       << "\tup   --drive/-d <drive_name>" << std::endl
       << "\tdown --drive/-d <drive_name> [--force/-f]" << std::endl;
  std::string drive = getOptionValue(tokens, "-d", "--drive", false);
  if(drive.empty()) {
    m_data = help.str();
    return;
  }
  if("up" == tokens[2]) {
    m_scheduler->setDriveStatus(cliIdentity, drive, true, false);
  }
  else if("down" == tokens[2]) {
    m_scheduler->setDriveStatus(cliIdentity, drive, false, hasOption(tokens, "-f", "--force"));
  }
  else {
    m_data = help.str();
  }
}

//------------------------------------------------------------------------------
// xCom_reconcile
//------------------------------------------------------------------------------
void XrdProFile::xCom_reconcile(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " rc/reconcile (it is a synchronous command, with a possibly long execution time, returns the list of files unknown to EOS, to be deleted manually by the admin after proper checks)" << std::endl;
  std::list<cta::common::dataStructures::ArchiveFile> list = m_scheduler->reconcile(cliIdentity);  
  if(list.size()>0) {
    std::vector<std::vector<std::string>> responseTable;
    std::vector<std::string> header = {"id","copy no","vid","fseq","block id","EOS id","size","checksum type","checksum value","storage class","owner","group","instance","path"};
    responseTable.push_back(header);    
    for(auto it = list.cbegin(); it != list.cend(); it++) {
      for(auto jt = it->tapeCopies.cbegin(); jt != it->tapeCopies.cend(); jt++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(std::to_string((unsigned long long)it->archiveFileID));
        currentRow.push_back(std::to_string((unsigned long long)jt->first));
        currentRow.push_back(jt->second.vid);
        currentRow.push_back(std::to_string((unsigned long long)jt->second.fSeq));
        currentRow.push_back(std::to_string((unsigned long long)jt->second.blockId));
        currentRow.push_back(it->eosFileID);
        currentRow.push_back(std::to_string((unsigned long long)it->fileSize));
        currentRow.push_back(it->checksumType);
        currentRow.push_back(it->checksumValue);
        currentRow.push_back(it->storageClass);
        currentRow.push_back(it->drData.drOwner);
        currentRow.push_back(it->drData.drGroup);
        currentRow.push_back(it->drData.drInstance);
        currentRow.push_back(it->drData.drPath);          
        responseTable.push_back(currentRow);
      }
    }
    m_data = formatResponse(responseTable);
  }
}

//------------------------------------------------------------------------------
// xCom_listpendingarchives
//------------------------------------------------------------------------------
void XrdProFile::xCom_listpendingarchives(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " lpa/listpendingarchives [--header/-h] [--tapepool/-t <tapepool_name>] [--extended/-x]" << std::endl;
  std::string tapepool = getOptionValue(tokens, "-t", "--tapepool", false);
  bool extended = hasOption(tokens, "-x", "--extended");
  std::map<std::string, std::list<cta::common::dataStructures::ArchiveJob> > result;
  if(tapepool.empty()) {
    result = m_scheduler->getPendingArchiveJobs(cliIdentity);
  }
  else {
    std::list<cta::common::dataStructures::ArchiveJob> list = m_scheduler->getPendingArchiveJobs(cliIdentity, tapepool);
    if(list.size()>0) {
      result[tapepool] = list;
    }
  }
  if(result.size()>0) {
    if(extended) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"tapepool","id","storage class","copy no.","EOS id","checksum type","checksum value","size","user","group","instance","path","diskpool","diskpool throughput"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = result.cbegin(); it != result.cend(); it++) {
        for(auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
          std::vector<std::string> currentRow;
          currentRow.push_back(it->first);
          currentRow.push_back(std::to_string((unsigned long long)jt->archiveFileID));
          currentRow.push_back(jt->request.storageClass);
          currentRow.push_back(std::to_string((unsigned long long)jt->copyNumber));
          currentRow.push_back(jt->request.eosFileID);
          currentRow.push_back(jt->request.checksumType);
          currentRow.push_back(jt->request.checksumValue);         
          currentRow.push_back(std::to_string((unsigned long long)jt->request.fileSize));
          currentRow.push_back(jt->request.requester.name);
          currentRow.push_back(jt->request.requester.group);
          currentRow.push_back(jt->request.drData.drInstance);
          currentRow.push_back(jt->request.drData.drPath);
          currentRow.push_back(jt->request.diskpoolName);
          currentRow.push_back(std::to_string((unsigned long long)jt->request.diskpoolThroughput));
          responseTable.push_back(currentRow);
        }
      }
      m_data = formatResponse(responseTable);
    }
    else {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"tapepool","total files","total size"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = result.cbegin(); it != result.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->first);
        currentRow.push_back(std::to_string((unsigned long long)it->second.size()));
        uint64_t size=0;
        for(auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
          size += jt->request.fileSize;
        }
        currentRow.push_back(std::to_string((unsigned long long)size));
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
}

//------------------------------------------------------------------------------
// xCom_listpendingretrieves
//------------------------------------------------------------------------------
void XrdProFile::xCom_listpendingretrieves(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " lpr/listpendingretrieves [--header/-h] [--vid/-v <vid>] [--extended/-x]" << std::endl;
  std::string vid = getOptionValue(tokens, "-v", "--vid", false);
  bool extended = hasOption(tokens, "-x", "--extended");
  std::map<std::string, std::list<cta::common::dataStructures::RetrieveJob> > result;
  if(vid.empty()) {
    result = m_scheduler->getPendingRetrieveJobs(cliIdentity);
  }
  else {
    std::list<cta::common::dataStructures::RetrieveJob> list = m_scheduler->getPendingRetrieveJobs(cliIdentity, vid);
    if(list.size()>0) {
      result[vid] = list;
    }
  }
  if(result.size()>0) {
    if(extended) {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"vid","id","copy no.","fseq","block id","size","user","group","instance","path","diskpool","diskpool throughput"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = result.cbegin(); it != result.cend(); it++) {
        for(auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
          std::vector<std::string> currentRow;
          currentRow.push_back(it->first);
          currentRow.push_back(std::to_string((unsigned long long)jt->request.archiveFileID));
          cta::common::dataStructures::ArchiveFile file = m_catalogue->getArchiveFileById(jt->request.archiveFileID);
          currentRow.push_back(std::to_string((unsigned long long)(jt->tapeCopies.at(it->first).first)));
          currentRow.push_back(std::to_string((unsigned long long)(jt->tapeCopies.at(it->first).second.fSeq)));
          currentRow.push_back(std::to_string((unsigned long long)(jt->tapeCopies.at(it->first).second.blockId)));
          currentRow.push_back(std::to_string((unsigned long long)file.fileSize));
          currentRow.push_back(jt->request.requester.name);
          currentRow.push_back(jt->request.requester.group);
          currentRow.push_back(jt->request.drData.drInstance);
          currentRow.push_back(jt->request.drData.drPath);
          currentRow.push_back(jt->request.diskpoolName);
          currentRow.push_back(std::to_string((unsigned long long)jt->request.diskpoolThroughput));
          responseTable.push_back(currentRow);
        }
      }
      m_data = formatResponse(responseTable);
    }
    else {
      std::vector<std::vector<std::string>> responseTable;
      std::vector<std::string> header = {"vid","total files","total size"};
      if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
      for(auto it = result.cbegin(); it != result.cend(); it++) {
        std::vector<std::string> currentRow;
        currentRow.push_back(it->first);
        currentRow.push_back(std::to_string((unsigned long long)it->second.size()));
        uint64_t size=0;
        for(auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
          cta::common::dataStructures::ArchiveFile file = m_catalogue->getArchiveFileById(jt->request.archiveFileID);
          size += file.fileSize;
        }
        currentRow.push_back(std::to_string((unsigned long long)size));
        responseTable.push_back(currentRow);
      }
      m_data = formatResponse(responseTable);
    }
  }
}

//------------------------------------------------------------------------------
// xCom_listdrivestates
//------------------------------------------------------------------------------
void XrdProFile::xCom_listdrivestates(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " lds/listdrivestates [--header/-h]" << std::endl;
  std::list<cta::common::dataStructures::DriveState> result = m_scheduler->getDriveStates(cliIdentity);  
  if(result.size()>0) {
    std::vector<std::vector<std::string>> responseTable;
    std::vector<std::string> header = {"logical library","host","drive","status","since","mount","vid","tapepool","session id","since","files","bytes","latest speed"};
    if(hasOption(tokens, "-h", "--header")) responseTable.push_back(header);    
    for(auto it = result.cbegin(); it != result.cend(); it++) {
      std::vector<std::string> currentRow;
      currentRow.push_back(it->logicalLibrary);
      currentRow.push_back(it->host);
      currentRow.push_back(it->name);
      currentRow.push_back(cta::common::dataStructures::toString(it->status));
      currentRow.push_back(std::to_string((unsigned long long)(time(NULL)-it->currentStateStartTime)));
      currentRow.push_back(cta::common::dataStructures::toString(it->mountType));
      currentRow.push_back(it->currentVid);
      currentRow.push_back(it->currentTapePool);
      currentRow.push_back(std::to_string((unsigned long long)it->sessionId));
      currentRow.push_back(std::to_string((unsigned long long)(time(NULL)-it->sessionStartTime)));
      currentRow.push_back(std::to_string((unsigned long long)it->filesTransferedInSession));
      currentRow.push_back(std::to_string((unsigned long long)it->bytesTransferedInSession));
      currentRow.push_back(std::to_string((long double)it->latestBandwidth));
      responseTable.push_back(currentRow);
    }
    m_data = formatResponse(responseTable);
  }
}

//------------------------------------------------------------------------------
// xCom_archive
//------------------------------------------------------------------------------
void XrdProFile::xCom_archive(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " a/archive --encoded <\"true\" or \"false\"> --user <user> --group <group> --eosid <EOS_unique_id> --srcurl <src_URL> --size <size> --checksumtype <checksum_type>" << std::endl
                    << "\t--checksumvalue <checksum_value> --storageclass <storage_class> --dr_instance <DR_instance> --dr_path <DR_path> --dr_owner <DR_owner>" << std::endl
                    << "\t--dr_ownergroup <DR_group> --dr_blob <DR_blob> --diskpool <diskpool_name> --throughput <diskpool_throughput>" << std::endl;
  std::string encoded_s = getOptionValue(tokens, "", "--encoded", false);
  if(encoded_s!="true" && encoded_s!="false") {
    m_data = help.str();
    return;
  }
  bool encoded = encoded_s=="true"?true:false;
  std::string user = getOptionValue(tokens, "", "--user", encoded);
  std::string group = getOptionValue(tokens, "", "--group", encoded);
  std::string eosid = getOptionValue(tokens, "", "--eosid", encoded);
  std::string srcurl = getOptionValue(tokens, "", "--srcurl", encoded);
  std::string size_s = getOptionValue(tokens, "", "--size", encoded);
  std::string checksumtype = getOptionValue(tokens, "", "--checksumtype", encoded);
  std::string checksumvalue = getOptionValue(tokens, "", "--checksumvalue", encoded);
  std::string storageclass = getOptionValue(tokens, "", "--storageclass", encoded);
  std::string dr_instance = getOptionValue(tokens, "", "--dr_instance", encoded);
  std::string dr_path = getOptionValue(tokens, "", "--dr_path", encoded);
  std::string dr_owner = getOptionValue(tokens, "", "--dr_owner", encoded);
  std::string dr_ownergroup = getOptionValue(tokens, "", "--dr_ownergroup", encoded);
  std::string dr_blob = getOptionValue(tokens, "", "--dr_blob", encoded);
  std::string diskpool = getOptionValue(tokens, "", "--diskpool", encoded);
  std::string throughput_s = getOptionValue(tokens, "", "--throughput", encoded);
  if(user.empty() || group.empty() || eosid.empty() || srcurl.empty() || size_s.empty() || checksumtype.empty() || checksumvalue.empty()
          || storageclass.empty() || dr_instance.empty() || dr_path.empty() || dr_owner.empty() || dr_ownergroup.empty() || dr_blob.empty() || diskpool.empty() || throughput_s.empty()) {
    m_data = help.str();
    return;
  }
  uint64_t size; std::stringstream size_ss; size_ss << size_s; size_ss >> size;
  uint64_t throughput; std::stringstream throughput_ss; throughput_ss << throughput_s; throughput_ss >> throughput;
  cta::common::dataStructures::UserIdentity originator;
  originator.name=user;
  originator.group=group;
  cta::common::dataStructures::DRData drData;
  drData.drBlob=dr_blob;
  drData.drGroup=dr_ownergroup;
  drData.drInstance=dr_instance;
  drData.drOwner=dr_owner;
  drData.drPath=dr_path;
  cta::common::dataStructures::ArchiveRequest request;
  request.checksumType=checksumtype;
  request.checksumValue=checksumvalue;
  request.diskpoolName=diskpool;
  request.diskpoolThroughput=throughput;
  request.drData=drData;
  request.eosFileID=eosid;
  request.fileSize=size;
  request.requester=originator;
  request.srcURL=srcurl;
  request.storageClass=storageclass;  
  uint64_t archiveFileId = m_scheduler->queueArchiveRequest(cliIdentity, request);
  std::stringstream res_ss;
  res_ss << archiveFileId << std::endl;
  m_data = res_ss.str();
}

//------------------------------------------------------------------------------
// xCom_retrieve
//------------------------------------------------------------------------------
void XrdProFile::xCom_retrieve(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " r/retrieve --encoded <\"true\" or \"false\"> --user <user> --group <group> --id <CTA_ArchiveFileID> --dsturl <dst_URL> --dr_instance <DR_instance> --dr_path <DR_path>" << std::endl
                    << "\t--dr_owner <DR_owner> --dr_ownergroup <DR_group> --dr_blob <DR_blob> --diskpool <diskpool_name> --throughput <diskpool_throughput>" << std::endl;
  std::string encoded_s = getOptionValue(tokens, "", "--encoded", false);
  if(encoded_s!="true" && encoded_s!="false") {
    m_data = help.str();
    return;
  }
  bool encoded = encoded_s=="true"?true:false;
  std::string user = getOptionValue(tokens, "", "--user", encoded);
  std::string group = getOptionValue(tokens, "", "--group", encoded);
  std::string id_s = getOptionValue(tokens, "", "--id", encoded);
  std::string dsturl = getOptionValue(tokens, "", "--dsturl", encoded);
  std::string dr_instance = getOptionValue(tokens, "", "--dr_instance", encoded);
  std::string dr_path = getOptionValue(tokens, "", "--dr_path", encoded);
  std::string dr_owner = getOptionValue(tokens, "", "--dr_owner", encoded);
  std::string dr_ownergroup = getOptionValue(tokens, "", "--dr_ownergroup", encoded);
  std::string dr_blob = getOptionValue(tokens, "", "--dr_blob", encoded);
  std::string diskpool = getOptionValue(tokens, "", "--diskpool", encoded);
  std::string throughput_s = getOptionValue(tokens, "", "--throughput", encoded);
  if(user.empty() || group.empty() || id_s.empty() || dsturl.empty() || dr_instance.empty() || dr_path.empty() || dr_owner.empty() || dr_ownergroup.empty() || dr_blob.empty() || diskpool.empty() || throughput_s.empty()) {
    m_data = help.str();
    return;
  }
  uint64_t id; std::stringstream id_ss; id_ss << id_s; id_ss >> id;
  uint64_t throughput; std::stringstream throughput_ss; throughput_ss << throughput_s; throughput_ss >> throughput;
  cta::common::dataStructures::UserIdentity originator;
  originator.name=user;
  originator.group=group;
  cta::common::dataStructures::DRData drData;
  drData.drBlob=dr_blob;
  drData.drGroup=dr_ownergroup;
  drData.drInstance=dr_instance;
  drData.drOwner=dr_owner;
  drData.drPath=dr_path;
  cta::common::dataStructures::RetrieveRequest request;
  request.diskpoolName=diskpool;
  request.diskpoolThroughput=throughput;
  request.drData=drData;
  request.archiveFileID=id;
  request.requester=originator;
  request.dstURL=dsturl;
  m_scheduler->queueRetrieveRequest(cliIdentity, request);
}

//------------------------------------------------------------------------------
// xCom_deletearchive
//------------------------------------------------------------------------------
void XrdProFile::xCom_deletearchive(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " da/deletearchive --encoded <\"true\" or \"false\"> --user <user> --group <group> --id <CTA_ArchiveFileID>" << std::endl;
  std::string encoded_s = getOptionValue(tokens, "", "--encoded", false);
  if(encoded_s!="true" && encoded_s!="false") {
    m_data = help.str();
    return;
  }
  bool encoded = encoded_s=="true"?true:false;
  std::string user = getOptionValue(tokens, "", "--user", encoded);
  std::string group = getOptionValue(tokens, "", "--group", encoded);
  std::string id_s = getOptionValue(tokens, "", "--id", encoded);
  if(user.empty() || group.empty() || id_s.empty()) {
    m_data = help.str();
    return;
  }
  uint64_t id; std::stringstream id_ss; id_ss << id_s; id_ss >> id;
  cta::common::dataStructures::UserIdentity originator;
  originator.name=user;
  originator.group=group;
  cta::common::dataStructures::DeleteArchiveRequest request;
  request.archiveFileID=id;
  request.requester=originator;
  m_scheduler->deleteArchiveRequest(cliIdentity, request);
}

//------------------------------------------------------------------------------
// xCom_cancelretrieve
//------------------------------------------------------------------------------
void XrdProFile::xCom_cancelretrieve(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " cr/cancelretrieve --encoded <\"true\" or \"false\"> --user <user> --group <group> --id <CTA_ArchiveFileID> --dsturl <dst_URL> --dr_instance <DR_instance> --dr_path <DR_path>" << std::endl
                    << "\t--dr_owner <DR_owner> --dr_ownergroup <DR_group> --dr_blob <DR_blob>" << std::endl;
  std::string encoded_s = getOptionValue(tokens, "", "--encoded", false);
  if(encoded_s!="true" && encoded_s!="false") {
    m_data = help.str();
    return;
  }
  bool encoded = encoded_s=="true"?true:false;
  std::string user = getOptionValue(tokens, "", "--user", encoded);
  std::string group = getOptionValue(tokens, "", "--group", encoded);
  std::string id_s = getOptionValue(tokens, "", "--id", encoded);
  std::string dsturl = getOptionValue(tokens, "", "--dsturl", encoded);
  std::string dr_instance = getOptionValue(tokens, "", "--dr_instance", encoded);
  std::string dr_path = getOptionValue(tokens, "", "--dr_path", encoded);
  std::string dr_owner = getOptionValue(tokens, "", "--dr_owner", encoded);
  std::string dr_ownergroup = getOptionValue(tokens, "", "--dr_ownergroup", encoded);
  std::string dr_blob = getOptionValue(tokens, "", "--dr_blob", encoded);
  if(user.empty() || group.empty() || id_s.empty() || dsturl.empty() || dr_instance.empty() || dr_path.empty() || dr_owner.empty() || dr_ownergroup.empty() || dr_blob.empty()) {
    m_data = help.str();
    return;
  }
  uint64_t id; std::stringstream id_ss; id_ss << id_s; id_ss >> id;
  cta::common::dataStructures::UserIdentity originator;
  originator.name=user;
  originator.group=group;
  cta::common::dataStructures::DRData drData;
  drData.drBlob=dr_blob;
  drData.drGroup=dr_ownergroup;
  drData.drInstance=dr_instance;
  drData.drOwner=dr_owner;
  drData.drPath=dr_path;
  cta::common::dataStructures::CancelRetrieveRequest request;
  request.drData=drData;
  request.archiveFileID=id;
  request.requester=originator;
  request.dstURL=dsturl;
  m_scheduler->cancelRetrieveRequest(cliIdentity, request);
}

//------------------------------------------------------------------------------
// xCom_updatefileinfo
//------------------------------------------------------------------------------
void XrdProFile::xCom_updatefileinfo(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " ufi/updatefileinfo --encoded <\"true\" or \"false\"> --user <user> --group <group> --id <CTA_ArchiveFileID> --storageclass <storage_class> --dr_instance <DR_instance> --dr_path <DR_path>" << std::endl
                    << "\t--dr_owner <DR_owner> --dr_ownergroup <DR_group> --dr_blob <DR_blob>" << std::endl;
  std::string encoded_s = getOptionValue(tokens, "", "--encoded", false);
  if(encoded_s!="true" && encoded_s!="false") {
    m_data = help.str();
    return;
  }
  bool encoded = encoded_s=="true"?true:false;
  std::string user = getOptionValue(tokens, "", "--user", encoded);
  std::string group = getOptionValue(tokens, "", "--group", encoded);
  std::string id_s = getOptionValue(tokens, "", "--id", encoded);
  std::string storageclass = getOptionValue(tokens, "", "--storageclass", encoded);
  std::string dr_instance = getOptionValue(tokens, "", "--dr_instance", encoded);
  std::string dr_path = getOptionValue(tokens, "", "--dr_path", encoded);
  std::string dr_owner = getOptionValue(tokens, "", "--dr_owner", encoded);
  std::string dr_ownergroup = getOptionValue(tokens, "", "--dr_ownergroup", encoded);
  std::string dr_blob = getOptionValue(tokens, "", "--dr_blob", encoded);
  if(user.empty() || group.empty() || id_s.empty() || storageclass.empty() || dr_instance.empty() || dr_path.empty() || dr_owner.empty() || dr_ownergroup.empty() || dr_blob.empty()) {
    m_data = help.str();
    return;
  }
  uint64_t id; std::stringstream id_ss; id_ss << id_s; id_ss >> id;
  cta::common::dataStructures::UserIdentity originator;
  originator.name=user;
  originator.group=group;
  cta::common::dataStructures::DRData drData;
  drData.drBlob=dr_blob;
  drData.drGroup=dr_ownergroup;
  drData.drInstance=dr_instance;
  drData.drOwner=dr_owner;
  drData.drPath=dr_path;
  cta::common::dataStructures::UpdateFileInfoRequest request;
  request.drData=drData;
  request.archiveFileID=id;
  request.requester=originator;
  request.storageClass=storageclass;
  m_scheduler->updateFileInfoRequest(cliIdentity, request);
}

//------------------------------------------------------------------------------
// xCom_liststorageclass
//------------------------------------------------------------------------------
void XrdProFile::xCom_liststorageclass(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &cliIdentity) {
  std::stringstream help;
  help << tokens[0] << " lsc/liststorageclass --encoded <\"true\" or \"false\"> --user <user> --group <group>" << std::endl;
  std::string encoded_s = getOptionValue(tokens, "", "--encoded", false);
  if(encoded_s!="true" && encoded_s!="false") {
    m_data = help.str();
    return;
  }
  bool encoded = encoded_s=="true"?true:false;
  std::string user = getOptionValue(tokens, "", "--user", encoded);
  std::string group = getOptionValue(tokens, "", "--group", encoded);
  if(user.empty() || group.empty()) {
    m_data = help.str();
    return;
  }
  cta::common::dataStructures::UserIdentity originator;
  originator.name=user;
  originator.group=group;
  cta::common::dataStructures::ListStorageClassRequest request;
  request.requester=originator;
  m_scheduler->listStorageClassRequest(cliIdentity, request);
}
  
//------------------------------------------------------------------------------
// getGenericHelp
//------------------------------------------------------------------------------
std::string XrdProFile::getGenericHelp(const std::string &programName) const {
  std::stringstream help;
  help << "CTA ADMIN commands:" << std::endl;
  help << "" << std::endl;
  help << "For each command there is a short version and a long one. Subcommands (add/rm/ls/ch/reclaim) do not have short versions." << std::endl;
  help << "" << std::endl;
  help << programName << " admin/ad          add/ch/rm/ls"               << std::endl;
  help << programName << " adminhost/ah      add/ch/rm/ls"               << std::endl;
  help << programName << " archivefile/af    ls"                         << std::endl;
  help << programName << " archiveroute/ar   add/ch/rm/ls"               << std::endl;
  help << programName << " bootstrap/bs"                                 << std::endl;
  help << programName << " dedication/de     add/ch/rm/ls"               << std::endl;
  help << programName << " drive/dr          up/down"                    << std::endl;
  help << programName << " listdrivestates/lds"                          << std::endl;
  help << programName << " listpendingarchives/lpa"                      << std::endl;
  help << programName << " listpendingretrieves/lpr"                     << std::endl;
  help << programName << " logicallibrary/ll add/ch/rm/ls"               << std::endl;
  help << programName << " reconcile/rc"                                 << std::endl;
  help << programName << " repack/re         add/rm/ls/err"              << std::endl;
  help << programName << " shrink/sh"                                    << std::endl;
  help << programName << " storageclass/sc   add/ch/rm/ls"               << std::endl;
  help << programName << " tape/ta           add/ch/rm/reclaim/ls/label" << std::endl;
  help << programName << " tapepool/tp       add/ch/rm/ls"               << std::endl;
  help << programName << " test/te           read/write"                 << std::endl;
  help << programName << " user/us           add/ch/rm/ls"               << std::endl;
  help << programName << " mountgroup/mg     add/ch/rm/ls"               << std::endl;
  help << programName << " verify/ve         add/rm/ls/err"              << std::endl;
  help << "" << std::endl;
  help << "CTA EOS commands:" << std::endl;
  help << "" << std::endl;
  help << "For each command there is a short version and a long one." << std::endl;
  help << "" << std::endl;
  help << programName << " archive/a"                                    << std::endl;
  help << programName << " cancelretrieve/cr"                            << std::endl;
  help << programName << " deletearchive/da"                             << std::endl;
  help << programName << " liststorageclass/lsc"                         << std::endl;
  help << programName << " retrieve/r"                                   << std::endl;
  help << programName << " updatefileinfo/ufi"                           << std::endl;
  return help.str();
}

}}
