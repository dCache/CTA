#include "XrdProFilesystem.hpp"

#include "XrdOuc/XrdOucString.hh"
#include "XrdSec/XrdSecEntity.hh"
#include "XrdVersion.hh"

#include <iostream>
#include <pwd.h>
#include <sstream>
#include <sys/types.h>

XrdVERSIONINFO(XrdSfsGetFileSystem,XrdPro);

extern "C"
{
  XrdSfsFileSystem *XrdSfsGetFileSystem (XrdSfsFileSystem* native_fs, XrdSysLogger* lp, const char* configfn)
  {
    return new XrdProFilesystem;
  }
}

//------------------------------------------------------------------------------
// checkClient
//------------------------------------------------------------------------------
int XrdProFilesystem::checkClient(const XrdSecEntity *client, XrdOucErrInfo &eInfo) {
  /*
  std::cout << "Calling FSctl with:\ncmd=" << cmd << "\narg=" << args.Arg1 << std::endl;
  std::cout << "Client info:\n"
          << "\nclient->host: " << client->host
          << "\nclient->name: " << client->name
          << "\nclient->prot: " << client->prot
          << "\nclient->sessvar: " << client->sessvar
          << "\nclient->tident: " << client->tident << std::endl;
   */
  if(!client || !client->host || strncmp(client->host, "localhost", 9))
  {
    eInfo.setErrInfo(EACCES, "[ERROR] operation possible only from localhost");
    return SFS_ERROR;
  }
  struct passwd pwd;
  struct passwd *result;
  char *buf;
  size_t bufsize;
  bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
  {
    bufsize = 16384;
  }
  buf = (char *)malloc(bufsize);
  if(buf == NULL)
  {
    eInfo.setErrInfo(ENOMEM, "[ERROR] malloc of the buffer failed");
    return SFS_ERROR;
  }
  int rc = getpwnam_r(client->name, &pwd, buf, bufsize, &result);
  if(result == NULL)
  {
    if (rc == 0)
    {
      XrdOucString errString = "[ERROR] User ";
      errString += client->name;
      errString += " not found";
      eInfo.setErrInfo(EACCES, errString.c_str());
      return SFS_ERROR;
    }
    else
    {
      eInfo.setErrInfo(EACCES, "[ERROR] getpwnam_r failed");
      return SFS_ERROR;
    }
  }
  //std::cout << "Logging request received from client. Username: " << client->name << " uid: " << pwd.pw_uid << " gid: " << pwd.pw_gid << std::endl;
  free(buf);
  return SFS_OK;
}

//------------------------------------------------------------------------------
// parseArchiveRequest
//------------------------------------------------------------------------------
int XrdProFilesystem::parseRequest(const XrdSfsFSctl &args, ParsedRequest &req, XrdOucErrInfo &eInfo) {
  std::stringstream ss(args.Arg1);
  std::string s;
  getline(ss, s, '?');
  req.cmd = s;
  while (getline(ss, s, '+')) {
    req.args.push_back(s);
  }
  if(req.cmd.empty()) {
    eInfo.setErrInfo(EINVAL, "[ERROR] No command supplied");
    return SFS_ERROR;
  }
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeArchiveCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeArchiveCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() < 2) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Too few arguments provided");
    return SFS_ERROR;
  }
  std::cout << "archive request received:\n";
  for(int i=0; i<req.args.size()-1; i++) {
    std::cout << "SRC: " << req.args.at(i) << std::endl;
  }  
  std::cout << "DST: " << req.args.at(req.args.size()-1) << std::endl;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeCreateStorageClassCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeCreateStorageClassCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() != 2) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Wrong number of arguments provided");
    return SFS_ERROR;
  }
  std::cout << "create-storage-class request received:\n";
  std::cout << "NAME: " << req.args.at(0) << std::endl;
  std::cout << "Number of copies on tape: " << req.args.at(1) << std::endl;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeCreateStorageClassCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeChangeStorageClassCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() != 2) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Wrong number of arguments provided");
    return SFS_ERROR;
  }
  std::cout << "change-storage-class request received:\n";
  std::cout << "DIR: " << req.args.at(0) << std::endl;
  std::cout << "NAME: " << req.args.at(1) << std::endl;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeDeleteStorageClassCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeDeleteStorageClassCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() != 1) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Wrong number of arguments provided");
    return SFS_ERROR;
  }
  std::cout << "delete-storage-class request received:\n";
  std::cout << "NAME: " << req.args.at(0) << std::endl;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeListStorageClassCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeListStorageClassCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() != 0) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Wrong number of arguments provided");
    return SFS_ERROR;
  }
  std::cout << "list-storage-class request received:\n";
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeMkdirCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeMkdirCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() != 1) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Wrong number of arguments provided");
    return SFS_ERROR;
  }
  std::cout << "mkdir request received:\n";
  std::cout << "DIR: " << req.args.at(0) << std::endl;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// executeRmdirCommand
//------------------------------------------------------------------------------
int XrdProFilesystem::executeRmdirCommand(ParsedRequest &req, XrdOucErrInfo &eInfo) {
  if(req.args.size() != 1) {
    eInfo.setErrInfo(EINVAL, "[ERROR] Wrong number of arguments provided");
    return SFS_ERROR;
  }
  std::cout << "rmdir request received:\n";
  std::cout << "DIR: " << req.args.at(0) << std::endl;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// dispatchRequest
//------------------------------------------------------------------------------
int XrdProFilesystem::dispatchRequest(XrdSfsFSctl &args, XrdOucErrInfo &eInfo) {
  ParsedRequest req;
  int checkParse = parseRequest(args, req, eInfo);
  if(SFS_OK!=checkParse) {
    return checkParse;
  }
  if(strcmp(req.cmd.c_str(), "/archive") == 0)
  {  
    return executeArchiveCommand(req, eInfo);
  }
  else if(strcmp(req.cmd.c_str(), "/create-storage-class") == 0)
  {  
    return executeCreateStorageClassCommand(req, eInfo);
  }  
  else if(strcmp(req.cmd.c_str(), "/change-storage-class") == 0)
  {  
    return executeChangeStorageClassCommand(req, eInfo);
  }
  else if(strcmp(req.cmd.c_str(), "/delete-storage-class") == 0)
  {  
    return executeDeleteStorageClassCommand(req, eInfo);
  }
  else if(strcmp(req.cmd.c_str(), "/list-storage-class") == 0)
  {  
    return executeListStorageClassCommand(req, eInfo);
  }
  else if(strcmp(req.cmd.c_str(), "/mkdir") == 0)
  {  
    return executeMkdirCommand(req, eInfo);
  }  
  else if(strcmp(req.cmd.c_str(), "/rmdir") == 0)
  {  
    return executeRmdirCommand(req, eInfo);
  }
  else
  {
    eInfo.setErrInfo(EINVAL, "[ERROR] Unknown command received");
    return SFS_ERROR;
  }
}

//------------------------------------------------------------------------------
// FSctl
//------------------------------------------------------------------------------
int XrdProFilesystem::FSctl(const int cmd, XrdSfsFSctl &args, XrdOucErrInfo &eInfo, const XrdSecEntity *client)
{
  int checkResult = checkClient(client, eInfo);
  if(SFS_OK!=checkResult) {
    return checkResult;
  }
  if(cmd == SFS_FSCTL_PLUGIO)
  {
    int checkDispatch = dispatchRequest(args, eInfo);
    if(SFS_OK!=checkDispatch) {
      return checkDispatch;
    }
    std::string response = "Request logged!";
    eInfo.setErrInfo(response.length(), response.c_str());
    return SFS_DATA;
  }
  else
  {
    eInfo.setErrInfo(EINVAL, "[ERROR] Unknown query request received");
    return SFS_ERROR;
  }
}

//------------------------------------------------------------------------------
// fsctl
//------------------------------------------------------------------------------
int XrdProFilesystem::fsctl(const int cmd, const char *args, XrdOucErrInfo &eInfo, const XrdSecEntity *client)
{
  (void)cmd; (void)args; (void)eInfo; (void)client;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// getStats
//------------------------------------------------------------------------------
int XrdProFilesystem::getStats(char *buff, int blen)
{
  (void)buff; (void)blen;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// getVersion
//------------------------------------------------------------------------------
const char * XrdProFilesystem::getVersion()
{
  return NULL;
}

//------------------------------------------------------------------------------
// exists
//------------------------------------------------------------------------------
int XrdProFilesystem::exists(const char *path, XrdSfsFileExistence &eFlag, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)eFlag; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// mkdir
//------------------------------------------------------------------------------
int XrdProFilesystem::mkdir(const char *path, XrdSfsMode mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)mode; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// prepare
//------------------------------------------------------------------------------
int XrdProFilesystem::prepare(XrdSfsPrep &pargs, XrdOucErrInfo &eInfo, const XrdSecEntity *client)
{
  (void)pargs; (void)eInfo; (void)client;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// rem
//------------------------------------------------------------------------------
int XrdProFilesystem::rem(const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// remdir
//------------------------------------------------------------------------------
int XrdProFilesystem::remdir(const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// rename
//------------------------------------------------------------------------------
int XrdProFilesystem::rename(const char *oPath, const char *nPath, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaqueO, const char *opaqueN)
{
  (void)oPath; (void)nPath; (void)eInfo; (void)client; (void)opaqueO; (void)opaqueN;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// stat
//------------------------------------------------------------------------------
int XrdProFilesystem::stat(const char *Name, struct stat *buf, XrdOucErrInfo &eInfo, const XrdSecEntity *client,const char *opaque)
{
  (void)Name; (void)buf; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// stat
//------------------------------------------------------------------------------
int XrdProFilesystem::stat(const char *path, mode_t &mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)mode; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// truncate
//------------------------------------------------------------------------------
int XrdProFilesystem::truncate(const char *path, XrdSfsFileOffset fsize, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)fsize; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// newDir
//------------------------------------------------------------------------------
XrdSfsDirectory * XrdProFilesystem::newDir(char *user, int MonID)
{
  (void)user; (void)MonID;
  return NULL;
}

//------------------------------------------------------------------------------
// newFile
//------------------------------------------------------------------------------
XrdSfsFile * XrdProFilesystem::newFile(char *user, int MonID)
{
  (void)user; (void)MonID;
  return NULL;
}

//------------------------------------------------------------------------------
// chksum
//------------------------------------------------------------------------------
int XrdProFilesystem::chksum(csFunc Func, const char *csName, const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client,const char *opaque)
{
  (void)Func; (void)csName; (void)path; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// chmod
//------------------------------------------------------------------------------
int XrdProFilesystem::chmod(const char *path, XrdSfsMode mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)mode; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// Disc
//------------------------------------------------------------------------------
void XrdProFilesystem::Disc(const XrdSecEntity *client)
{
  (void)client;
}

//------------------------------------------------------------------------------
// EnvInfo
//------------------------------------------------------------------------------
void XrdProFilesystem::EnvInfo(XrdOucEnv *envP)
{
  (void)envP;
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
XrdProFilesystem::XrdProFilesystem() {}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
XrdProFilesystem::~XrdProFilesystem() {}