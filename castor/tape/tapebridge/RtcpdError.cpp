/******************************************************************************
 *                      castor/tape/tapebridge/RtcpdError.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * 
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#include "castor/tape/tapebridge/RtcpdError.hpp"
#include "h/serrno.h"

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------
castor::tape::tapebridge::RtcpdError::RtcpdError() :
  m_errorScope(UNKNOWN_SCOPE),
  m_errorCode(SEINTERNAL),
  m_errorMessage("UNKNOWN"),
  m_fileTransactionId(0),
  m_nsHost("UNKNOWN"),
  m_nsFileId(0),
  m_tapeFSeq(0) {
  // Do nothing
}

//-----------------------------------------------------------------------------
// setErrorScope
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setErrorScope(
  const ErrorScope value) throw(castor::exception::InvalidArgument) {

  // Check the method arguments
  if(0 > value || RTCPDERRORSCOPE_MAX < value) {
    castor::exception::InvalidArgument ex;
    ex.getMessage() <<
      "Invalid RtcpdErrorScope"
      ": value=" << value;
    throw ex;
  }
    
  m_errorScope = value;
}

//-----------------------------------------------------------------------------
// getErrorScope
//-----------------------------------------------------------------------------
castor::tape::tapebridge::RtcpdError::ErrorScope
  castor::tape::tapebridge::RtcpdError::getErrorScope() const {
  return m_errorScope;
}

//-----------------------------------------------------------------------------
// setErrorCode
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setErrorCode(const int value) {
  m_errorCode = value;
}

//-----------------------------------------------------------------------------
// getErrorCode
//-----------------------------------------------------------------------------
int castor::tape::tapebridge::RtcpdError::getErrorCode() const {
  return m_errorCode;
}

//-----------------------------------------------------------------------------
// setErrorMessage
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setErrorMessage(
  const std::string &value) {
  m_errorMessage = value;
}

//-----------------------------------------------------------------------------
// getErrorMessage
//-----------------------------------------------------------------------------
const std::string &castor::tape::tapebridge::RtcpdError::getErrorMessage()
  const {
  return m_errorMessage;
}

//-----------------------------------------------------------------------------
// setFileTransactionId
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setFileTransactionId(
  const uint64_t value) {
  m_fileTransactionId = value;
}

//-----------------------------------------------------------------------------
// getFileTransactionId
//-----------------------------------------------------------------------------
uint64_t castor::tape::tapebridge::RtcpdError::getFileTransactionId() const {
  return m_fileTransactionId;
}

//-----------------------------------------------------------------------------
// setNsHost
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setNsHost(const std::string &value) {
  m_nsHost = value;
}

//-----------------------------------------------------------------------------
// getNsHost
//-----------------------------------------------------------------------------
const std::string &castor::tape::tapebridge::RtcpdError::getNsHost() const {
  return m_nsHost;
}

//-----------------------------------------------------------------------------
// setNsFileId
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setNsFileId(const uint64_t value) {
  m_nsFileId = value;
}

//-----------------------------------------------------------------------------
// getNsFileId
//-----------------------------------------------------------------------------
uint64_t castor::tape::tapebridge::RtcpdError::getNsFileId() const {
  return m_nsFileId;
}

//-----------------------------------------------------------------------------
// setTapeFSeq
//-----------------------------------------------------------------------------
void castor::tape::tapebridge::RtcpdError::setTapeFSeq(const int32_t value) {
  m_tapeFSeq = value;
}

//-----------------------------------------------------------------------------
// getTapeFSeq
//-----------------------------------------------------------------------------
int32_t castor::tape::tapebridge::RtcpdError::getTapeFSeq() const {
  return m_tapeFSeq;
}
