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

#pragma once

#include "catalogue/Catalogue.hpp"
#include "common/Configuration.hpp"
#include "common/log/Logger.hpp"
#include "common/make_unique.hpp"
#include "objectstore/BackendPopulator.hpp"
#include "objectstore/BackendVFS.hpp"
#include "scheduler/OStoreDB/OStoreDBWithAgent.hpp"
#include "scheduler/Scheduler.hpp"
#include "XrdSfs/XrdSfsInterface.hh"

#include <google/protobuf/util/json_util.h>
#include <memory>


namespace cta { namespace xrootPlugins { 
/**
 * This class is the entry point for the xroot plugin: it is returned by 
 * XrdSfsGetFileSystem when the latter is called by the xroot server to load the
 * plugin. All function documentation can be found in XrdSfs/XrdSfsInterface.hh.
 */
class XrdCtaFilesystem : public ::XrdSfsFileSystem {
public:
  virtual XrdSfsDirectory *newDir(char *user=0, int MonID=0);
  virtual XrdSfsFile *newFile(char *user=0, int MonID=0);
  virtual int chksum(csFunc Func, const char *csName, const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0,const char *opaque = 0);
  virtual int chmod(const char *path, XrdSfsMode mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual void Disc(const XrdSecEntity *client = 0);
  virtual void EnvInfo(XrdOucEnv *envP);
  virtual int FSctl(const int cmd, XrdSfsFSctl &args, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0);
  virtual int fsctl(const int cmd, const char *args, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0);
  virtual int getStats(char *buff, int blen);
  virtual const char *getVersion();
  virtual int exists(const char *path, XrdSfsFileExistence &eFlag, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual int mkdir(const char *path, XrdSfsMode mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual int prepare(XrdSfsPrep &pargs, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0);
  virtual int rem(const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual int remdir(const char *path, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual int rename(const char *oPath, const char *nPath, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaqueO = 0, const char *opaqueN = 0);
  virtual int stat(const char *Name, struct ::stat *buf, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0,const char *opaque = 0);
  virtual int stat(const char *path, mode_t &mode, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual int truncate(const char *path, XrdSfsFileOffset fsize, XrdOucErrInfo &eInfo, const XrdSecEntity *client = 0, const char *opaque = 0);
  XrdCtaFilesystem();
  ~XrdCtaFilesystem();
  
protected:
  
  /**
   * The CTA configuration
   */
  cta::common::Configuration m_ctaConf;
  
  /**
   * The VFS backend for the objectstore DB
   */
  std::unique_ptr<cta::objectstore::Backend> m_backend;
  
  /**
   * The object used to populate the backend
   */
  cta::objectstore::BackendPopulator m_backendPopulator;
  
  /**
   * The database or object store holding all CTA persistent objects
   */
  cta::OStoreDBWithAgent m_scheddb;

  /**
   * The CTA catalogue of tapes and tape files.
   */
  std::unique_ptr<cta::catalogue::Catalogue> m_catalogue;

  /**
   * The scheduler.
   */
  std::unique_ptr<cta::Scheduler> m_scheduler;
  
  /**
   * The logger.
   */
  std::unique_ptr<log::Logger> m_log;

  /**
   * A deleter for instances of the XrdOucBuffer class.
   *
   * The destructor of the XrdOucBuffer class is private.  The Recycle()
   * method can be called on an instance of the XrdOucBuffer class in order to
   * effective delete that instance.
   */
  struct XrdOucBufferDeleter {
    void operator()(XrdOucBuffer* buf) {
      buf->Recycle();
    }
  };

  /**
   * Convenience typedef for an std::unique_ptr type that can delete instances
   * of the XrdOucBuffer class.
   */
  typedef std::unique_ptr<XrdOucBuffer, XrdOucBufferDeleter> UniqueXrdOucBuffer;

  /**
   * Convenience method to create a UniqueXrdOucBuffer.
   */
  static UniqueXrdOucBuffer make_UniqueXrdOucBuffer(const size_t bufSize) {
    char *const cbuf = static_cast<char *>(malloc(bufSize));
    if(nullptr == cbuf) {
      cta::exception::Exception ex;
      ex.getMessage() << __FUNCTION__ << " failed: Failed to malloc " << bufSize << " bytes";
      throw ex;
    }
    XrdOucBuffer *xbuf = new XrdOucBuffer(cbuf, bufSize);
    if(nullptr == xbuf) {
      cta::exception::Exception ex;
      ex.getMessage() << __FUNCTION__ << " failed: Failed to allocate an XrdOucBuffer";
      throw ex;
    }
    return UniqueXrdOucBuffer(xbuf);
  }

  /**
   * Processes the specified wrapper message.
   *
   * @param msg The message.
   * @param client Same semantic as the XrdCtaFilesystem::FSctl() method.
   * @return The result in the form of an XrdOucBuffer to be sent back to the
   * client.
   */
  UniqueXrdOucBuffer processWrapperMsg(const eos::wfe::Wrapper &msg, const XrdSecEntity *const client);

  /**
   * Processes the specified notification message.
   *
   * @param msg The message.
   * @param client Same semantic as the XrdCtaFilesystem::FSctl() method.
   * @return The result in the form of an XrdOucBuffer to be sent back to the
   * client.
   */
  UniqueXrdOucBuffer processNotificationMsg(const eos::wfe::Notification &msg, const XrdSecEntity *const client);

  /**
   * Processes the specified CLOSEW event.
   *
   * @param msg The notification message.
   * @param client Same semantic as the XrdCtaFilesystem::FSctl() method.
   * @return The result in the form of an XrdOucBuffer to be sent back to the
   * client.
   */
  UniqueXrdOucBuffer processCLOSEW(const eos::wfe::Notification &msg, const XrdSecEntity *const client);

  /**
   * Processes the specified CLOSEW event triggered by an EOS user writing a
   * file to disk, as opposed to a tape server.
   *
   * A user uses the "default" workflow when they write a file to disk.  A tape
   * server uses the "cta" workflow when it writes a file to disk.
   *
   * @param msg The message.
   * @param client Same semantic as the XrdCtaFilesystem::FSctl() method.
   * @return The result in the form of an XrdOucBuffer to be sent back to the
   * client.
   */
  UniqueXrdOucBuffer processDefaultCLOSEW(const eos::wfe::Notification &msg, const XrdSecEntity *const client);

  /**
   * Return the JSON representation of teh specified Google protocol buffer
   * message.
   * @param protobufMsg The Google protocol buffer message.
   * @return The JSON string.
   */
  template <typename T> static std::string toJson(T protobufMsg) {
    google::protobuf::util::JsonPrintOptions jsonPrintOptions;
    jsonPrintOptions.add_whitespace = false;
    jsonPrintOptions.always_print_primitive_fields = false;
    std::string json;
    google::protobuf::util::MessageToJsonString(protobufMsg, &json, jsonPrintOptions);
    return json;
  }

  /**
   * Returns the CTA_StorageClass of the specified directory.
   *
   * This method throws an exception if the specified directory does not have a
   * CTA_StorageClass.
   *
   * @param dir The directory.
   * @return The storage class.
   */
  std::string getDirStorageClass(const eos::wfe::Md &dir) const;

}; // XrdCtaFilesystem

}}
