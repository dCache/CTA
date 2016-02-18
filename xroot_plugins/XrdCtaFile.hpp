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

#include "scheduler/Scheduler.hpp"

#include "XrdSfs/XrdSfsInterface.hh"

#include <string>
#include <vector>

namespace cta { namespace xrootPlugins {

/**
 * This class represents the heart of the xroot server plugin: it inherits from 
 * XrdSfsFile and it is used by XrdProFilesystem whenever a command is executed 
 * (that is when a new file path is requested by the user). All function 
 * documentation can be found in XrdSfs/XrdSfsInterface.hh.
 */
class XrdProFile : public XrdSfsFile {
public:
  XrdOucErrInfo  error;
  virtual int open(const char *fileName, XrdSfsFileOpenMode openMode, mode_t createMode, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual int close();
  virtual int fctl(const int cmd, const char *args, XrdOucErrInfo &eInfo);
  virtual const char *FName();
  virtual int getMmap(void **Addr, off_t &Size);
  virtual XrdSfsXferSize read(XrdSfsFileOffset offset, XrdSfsXferSize size);
  virtual XrdSfsXferSize read(XrdSfsFileOffset offset, char *buffer, XrdSfsXferSize size);
  virtual XrdSfsXferSize read(XrdSfsAio *aioparm);
  virtual XrdSfsXferSize write(XrdSfsFileOffset offset, const char *buffer, XrdSfsXferSize size);
  virtual int write(XrdSfsAio *aioparm);
  virtual int stat(struct ::stat *buf);
  virtual int sync();
  virtual int sync(XrdSfsAio *aiop);
  virtual int truncate(XrdSfsFileOffset fsize);
  virtual int getCXinfo(char cxtype[4], int &cxrsz);
  XrdProFile(cta::Scheduler *scheduler, const char *user=0, int MonID=0);
  ~XrdProFile();
protected:
  
  /**
   * The scheduler object pointer
   */
  cta::Scheduler *m_scheduler;
  
  /**
   * This is the string holding the result of the command
   */
  std::string m_data;
  
  /**
   * Decodes a string in base 64
   * 
   * @param msg string to decode
   * @return decoded string
   */
  std::string decode(const std::string msg) const;
  
  /**
   * Checks whether client has correct permissions and returns the corresponding SecurityIdentity structure
   * 
   * @param req     parsed request
   * @return The requester structure
   */
  cta::common::dataStructures::SecurityIdentity checkClient(const XrdSecEntity *client);
  
  /**
   * Replaces all occurrences in a string "str" of a substring "from" with the string "to"
   * 
   * @param str  The original string
   * @param from The substring to replace
   * @param to   The replacement string
   */
  void replaceAll(std::string& str, const std::string& from, const std::string& to) const;
  
  /**
   * Parses the command line and dispatches it to the relevant function
   * 
   * @param tokens The command line tokens
   */
  void dispatchCommand(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  
  /**
   * Given the command line string vector it returns the value of the specified option or an empty string if absent
   * 
   * @param tokens          The command line tokens 
   * @param optionShortName The short name of the required option
   * @param optionLongName  The long name of the required option
   * @return the value of the option or an empty string if absent
   */
  std::string getOptionValue(const std::vector<std::string> &tokens, const std::string& optionShortName, const std::string& optionLongName);
  
  /**
   * Given the command line string vector it returns true if the specified option is present, false otherwise
   * 
   * @param tokens          The command line tokens 
   * @param optionShortName The short name of the required option
   * @param optionLongName  The long name of the required option
   * @return true if the specified option is present, false otherwise
   */
  bool hasOption(const std::vector<std::string> &tokens, const std::string& optionShortName, const std::string& optionLongName);
  
  void xCom_bootstrap(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_admin(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_adminhost(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_tapepool(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_archiveroute(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_logicallibrary(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_tape(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_storageclass(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_user(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_usergroup(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_dedication(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_repack(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_shrink(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_verify(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_archivefile(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_test(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_drive(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_reconcile(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_listpendingarchives(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_listpendingretrieves(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_listdrivestates(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_archive(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_retrieve(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_deletearchive(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_cancelretrieve(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_updatefileinfo(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  void xCom_liststorageclass(const std::vector<std::string> &tokens, const cta::common::dataStructures::SecurityIdentity &requester);
  
  /**
   * Returns the response string properly formatted in a table
   * 
   * @param  responseTable The response 2-D matrix
   * @return the response string properly formatted in a table
   */
  std::string formatResponse(const std::vector<std::vector<std::string>> &responseTable);
  
  /**
   * Returns a string representation of the time
   * 
   * @param  time The input time
   * @return a string representation of the time
   */
  std::string timeToString(const time_t &time);
  
  /**
   * Adds the creation log and the last modification log to the current response row
   * 
   * @param  responseRow The current response row to modify
   * @param  creationLog the creation log
   * @param  lastModificationLog the last modification log
   */
  void addLogInfoToResponseRow(std::vector<std::string> &responseRow, const cta::common::dataStructures::EntryLog &creationLog, const cta::common::dataStructures::EntryLog &lastModificationLog);
  
  /**
   * Returns the help string
   * 
   * @param  programName The name of the client program
   * @return the help string
   */
  std::string getGenericHelp(const std::string &programName) const;
};

}}
