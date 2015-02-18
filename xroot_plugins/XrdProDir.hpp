#pragma once

#include <iostream>

#include "XrdSec/XrdSecEntity.hh"
#include "XrdSfs/XrdSfsInterface.hh"

#include "libs/client/MockMiddleTierAdmin.hpp"

class XrdProDir : public XrdSfsDirectory
{
public:
  XrdOucErrInfo error;
  virtual int open(const char *path, const XrdSecEntity *client = 0, const char *opaque = 0);
  virtual const char *nextEntry();
  virtual int close();
  virtual const char *FName();
  XrdProDir(cta::MiddleTierAdmin *clientAPI, const char *user=0, int MonID=0);
  virtual ~XrdProDir();
protected:
  
  /**
   * Iterator holding contents of the directory
   */
  cta::DirectoryIterator m_itor;
  
  /**
   * Pointer to the client API object
   */
  cta::MiddleTierAdmin *m_clientAPI;
  
  /**
   * Checks whether client has correct permissions and fills the UserIdentity structure
   * 
   * @param req     parsed request
   * @param requester The structure to be filled
   * @return SFS_OK in case check is passed, SFS_ERROR otherwise
   */
  int checkClient(const XrdSecEntity *client, cta::SecurityIdentity &requester);
}; // class XrdProDir