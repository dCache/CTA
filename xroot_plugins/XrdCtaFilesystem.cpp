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

#include "castor/common/CastorConfiguration.hpp"
#include "catalogue/CatalogueFactory.hpp"
#include "common/admin/AdminHost.hpp"
#include "common/admin/AdminUser.hpp"
#include "common/log/StdoutLogger.hpp"
#include "common/log/StringLogger.hpp"
#include "common/log/FileLogger.hpp"
#include "common/archiveRoutes/ArchiveRoute.hpp"
#include "common/Configuration.hpp"
#include "common/exception/Exception.hpp"
#include "common/TapePool.hpp"
#include "eos/messages/eos_messages.pb.h"
#include "objectstore/RootEntry.hpp"
#include "objectstore/BackendFactory.hpp"
#include "scheduler/LogicalLibrary.hpp"
#include "scheduler/OStoreDB/OStoreDB.hpp"
#include "scheduler/RetrieveRequestDump.hpp"
#include "scheduler/SchedulerDatabase.hpp"
#include "XrdOuc/XrdOucString.hh"
#include "XrdSec/XrdSecEntity.hh"
#include "XrdVersion.hh"
#include "xroot_plugins/XrdCtaFilesystem.hpp"
#include "xroot_plugins/XrdCtaFile.hpp"
#include "XrdCtaDir.hpp"
#include "version.h"

#include <memory>
#include <iostream>
#include <pwd.h>
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <sys/types.h>

XrdVERSIONINFO(XrdSfsGetFileSystem,XrdCta)

extern "C"
{
  XrdSfsFileSystem *XrdSfsGetFileSystem (XrdSfsFileSystem* native_fs, XrdSysLogger* lp, const char* configfn)
  {
    try {
      return new cta::xroot_plugins::XrdCtaFilesystem();
    } catch (cta::exception::Exception &ex) {
      std::cerr << "[ERROR] Could not load the CTA xroot plugin. CTA exception caught: " << ex.what() << "\n";
      return nullptr;
    } catch (std::exception &ex) {
      std::cerr << "[ERROR] Could not load the CTA xroot plugin. Exception caught: " << ex.what() << "\n";
      return nullptr;
    } catch (...) {
      std::cerr << "[ERROR] Could not load the CTA xroot plugin. Unknown exception caught!" << "\n";
      return nullptr;
    }
  }
}

namespace cta { namespace xroot_plugins {

//------------------------------------------------------------------------------
// newFile
//------------------------------------------------------------------------------
XrdSfsFile * XrdCtaFilesystem::newFile(char *user, int MonID)
{  
  return new cta::xrootPlugins::XrdCtaFile(m_catalogue.get(), m_scheduler.get(), m_log.get(), user, MonID);
}

//------------------------------------------------------------------------------
// newDir
//------------------------------------------------------------------------------
XrdSfsDirectory * XrdCtaFilesystem::newDir(char *user, int MonID)
{
  return new cta::xrootPlugins::XrdCtaDir(m_catalogue.get(), m_log.get(), user, MonID);;
}

//------------------------------------------------------------------------------
// fsctl
//------------------------------------------------------------------------------
int XrdCtaFilesystem::fsctl(const int cmd, const char *args, XrdOucErrInfo &eInfo, const XrdSecEntity *client)
{
  (void)cmd; (void)args; (void)eInfo; (void)client;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// getStats
//------------------------------------------------------------------------------
int XrdCtaFilesystem::getStats(char *buff, int blen)
{
  (void)buff; (void)blen;
  return SFS_OK;
}

//------------------------------------------------------------------------------
// getVersion
//------------------------------------------------------------------------------
const char * XrdCtaFilesystem::getVersion()
{
  return nullptr;
}

//------------------------------------------------------------------------------
// exists
//------------------------------------------------------------------------------
int XrdCtaFilesystem::exists(const char *path, XrdSfsFileExistence &eFlag, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)eFlag; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// mkdir
//------------------------------------------------------------------------------
int XrdCtaFilesystem::mkdir(const char *path, XrdSfsMode mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)mode; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// prepare
//------------------------------------------------------------------------------
int XrdCtaFilesystem::prepare(XrdSfsPrep &pargs, XrdOucErrInfo &eInfo, const XrdSecEntity *client)
{
  (void)pargs; (void)eInfo; (void)client;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// rem
//------------------------------------------------------------------------------
int XrdCtaFilesystem::rem(const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// remdir
//------------------------------------------------------------------------------
int XrdCtaFilesystem::remdir(const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// rename
//------------------------------------------------------------------------------
int XrdCtaFilesystem::rename(const char *oPath, const char *nPath, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaqueO, const char *opaqueN)
{
  (void)oPath; (void)nPath; (void)eInfo; (void)client; (void)opaqueO; (void)opaqueN;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// stat
//------------------------------------------------------------------------------
int XrdCtaFilesystem::stat(const char *Name, struct ::stat *buf, XrdOucErrInfo &eInfo, const XrdSecEntity *client,const char *opaque)
{
  (void)Name; (void)buf; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// stat
//------------------------------------------------------------------------------
int XrdCtaFilesystem::stat(const char *path, mode_t &mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)mode; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// truncate
//------------------------------------------------------------------------------
int XrdCtaFilesystem::truncate(const char *path, XrdSfsFileOffset fsize, XrdOucErrInfo &eInfo, const XrdSecEntity *client, const char *opaque)
{
  (void)path; (void)fsize; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// chksum
//------------------------------------------------------------------------------
int XrdCtaFilesystem::chksum(csFunc Func, const char *csName, const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client,const char *opaque)
{
  (void)Func; (void)csName; (void)path; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// chmod
//------------------------------------------------------------------------------
int XrdCtaFilesystem::chmod(
  const char *path,
  XrdSfsMode mode,
  XrdOucErrInfo &eInfo,
  const XrdSecEntity *client,
  const char *opaque) {
  (void)path; (void)mode; (void)eInfo; (void)client; (void)opaque;
  eInfo.setErrInfo(ENOTSUP, "Not supported.");
  return SFS_ERROR;
}

//------------------------------------------------------------------------------
// Disc
//------------------------------------------------------------------------------
void XrdCtaFilesystem::Disc(const XrdSecEntity *client)
{
  (void)client;
}

//------------------------------------------------------------------------------
// EnvInfo
//------------------------------------------------------------------------------
void XrdCtaFilesystem::EnvInfo(XrdOucEnv *envP)
{
  (void)envP;
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
XrdCtaFilesystem::XrdCtaFilesystem():
  m_xrdOucBuffPool(1024, 65536), // XrdOucBuffPool(minsz, maxsz)
  m_ctaConf("/etc/cta/cta-frontend.conf"),
  m_backend(cta::objectstore::BackendFactory::createBackend(m_ctaConf.getConfEntString("ObjectStore", "BackendPath", nullptr)).release()),
  m_backendPopulator(*m_backend, "Frontend"),
  m_scheddb(*m_backend, m_backendPopulator.getAgentReference()) {
  using namespace cta;
  
  // Try to instantiate the logging system API
  try {
    std::string loggerURL=m_ctaConf.getConfEntString("Log", "URL", "syslog:");
    if (loggerURL == "syslog:") {
      m_log.reset(new log::SyslogLogger("cta-frontend", log::DEBUG));
    } else if (loggerURL == "stdout:") {
      m_log.reset(new log::StdoutLogger("cta-frontend"));
    } else if (loggerURL.substr(0, 5) == "file:") {
      m_log.reset(new log::FileLogger("cta-frontend", loggerURL.substr(5), log::DEBUG));
    } else {
      throw cta::exception::Exception(std::string("Unknown log URL: ")+loggerURL);
    }
  } catch(exception::Exception &ex) {
    throw cta::exception::Exception(std::string("Failed to instantiate object representing CTA logging system: ")+ex.getMessage().str());
  }
  
  const rdbms::Login catalogueLogin = rdbms::Login::parseFile("/etc/cta/cta_catalogue_db.conf");
  const uint64_t nbConns = m_ctaConf.getConfEntInt<uint64_t>("Catalogue", "NumberOfConnections", nullptr);
  const uint64_t nbArchiveFileListingConns = 2;
  m_catalogue = catalogue::CatalogueFactory::create(catalogueLogin, nbConns, nbArchiveFileListingConns);
  m_scheduler = cta::make_unique<cta::Scheduler>(*m_catalogue, m_scheddb, 5, 2*1000*1000);

  // If the backend is a VFS, make sure we don't delete it on exit.
  // If not, nevermind.
  try {
    dynamic_cast<objectstore::BackendVFS &>(*m_backend).noDeleteOnExit();
  } catch (std::bad_cast &){}

  const std::list<cta::log::Param> params = {cta::log::Param("version", CTA_VERSION)};
  cta::log::Logger &log= *m_log;
  log(log::INFO, std::string("cta-frontend started"), params);
  
  // Start the heartbeat thread for the agent object
  m_agentHeartbeat = cta::make_unique<objectstore::AgentHeartbeatThread> (m_backendPopulator.getAgentReference(), *m_backend, *m_log);
  m_agentHeartbeat->startThread();
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
XrdCtaFilesystem::~XrdCtaFilesystem() {
  m_agentHeartbeat->stopAndWaitThread();
}

}}
