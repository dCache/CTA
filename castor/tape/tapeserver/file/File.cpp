/******************************************************************************
 *                      File.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "File.hpp"
#include "castor/exception/Errnum.hpp"
#include "castor/exception/Mismatch.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include <fstream>

const unsigned short max_unix_hostname_length = 256; //255 + 1 terminating character

using namespace castor::tape::AULFile;

ReadSession::ReadSession(drives::DriveGeneric & drive, const std::string &vid) throw (Exception) : m_drive(drive), m_vid(vid), m_corrupted(false), m_locked(false), m_fseq(1), m_currentFilePart(Header) { 
  m_drive.rewind();
  VOL1 vol1;
  m_drive.readExactBlock((void * )&vol1, sizeof(vol1), "[ReadSession::ReadSession()] - Reading VOL1");
  try {
    vol1.verify();
  } catch (std::exception & e) {
    throw TapeFormatError(e.what());
  }
  HeaderChecker::checkVOL1(vol1, vid); //after which we are at the end of VOL1 header (i.e. beginning of HDR1 of the first file) on success, or at BOT in case of exception
}

void HeaderChecker::checkVOL1(const VOL1 &vol1, const std::string &volId) throw (Exception) {
  if(vol1.getVSN().compare(volId)) {
    std::stringstream ex_str;
    ex_str << "[HeaderChecker::checkVOL1()] - VSN of tape (" << vol1.getVSN() << ") is not the one requested (" << volId << ")";
    throw TapeFormatError(ex_str.str());
  }
}

ReadFile::ReadFile(ReadSession *rs, const FileInfo &fileInfo, const PositioningMode positioningMode) throw (Exception) : m_currentBlockSize(0), m_session(rs), m_positioningMode(positioningMode) {
  if(m_session->isCorrupted()) {
    throw SessionCorrupted();
  }
  m_session->lock();
  position(fileInfo);
}

ReadFile::~ReadFile() throw () {
  m_session->release();
}

bool HeaderChecker::checkHeaderNumericalField(const std::string &headerField, const uint64_t value, const bool is_field_hex, const bool is_field_oct) throw (Exception) {
  uint64_t res = 0;
  std::stringstream field_converter;
  field_converter << headerField;
  if(is_field_hex && !is_field_oct) field_converter >> std::hex >> res;
  else if(!is_field_hex && is_field_oct) field_converter >> std::oct >> res;
  else if(!is_field_hex && !is_field_oct) field_converter >> res;
  else throw castor::exception::InvalidArgument();
  return value==res;
}

void HeaderChecker::checkHDR1(const HDR1 &hdr1, const FileInfo &fileInfo, const std::string &volId) throw (Exception) {
  if(!checkHeaderNumericalField(hdr1.getFileId(), (uint64_t)fileInfo.nsFileId, true, false)) { // the nsfileid stored in HDR1 is in hexadecimal while the one supplied in the Information structure is in decimal
    std::stringstream ex_str;
    ex_str << "[HeaderChecker::checkHDR1] - Invalid fileid detected: " << hdr1.getFileId() << ". Wanted: " << fileInfo.nsFileId << std::endl;
    throw TapeFormatError(ex_str.str());
  }
  
  //the following should never ever happen... but never say never...
  if(hdr1.getVSN().compare(volId)) {
    std::stringstream ex_str;
    ex_str << "[HeaderChecker::checkHDR1] - Wrong volume ID info found in hdr1: " << hdr1.getVSN() << ". Wanted: " << volId;
    throw TapeFormatError(ex_str.str());
  }
}

void HeaderChecker::checkUHL1(const UHL1 &uhl1, const FileInfo &fileInfo) throw (Exception) {
  if(!checkHeaderNumericalField(uhl1.getfSeq(), (uint64_t)fileInfo.fseq, true, false)) {
    std::stringstream ex_str;
    ex_str << "[HeaderChecker::checkUHL1] - Invalid fseq detected in uhl1: " << atol(uhl1.getfSeq().c_str()) << ". Wanted: " << fileInfo.fseq;
    throw TapeFormatError(ex_str.str());
  }
}

void HeaderChecker::checkUTL1(const UTL1 &utl1, const uint32_t fseq) throw (Exception) {
  if(!checkHeaderNumericalField(utl1.getfSeq(), (uint64_t)fseq, true, false)) {
    std::stringstream ex_str;
    ex_str << "[HeaderChecker::checkUTL1] - Invalid fseq detected in uhl1: " << atol(utl1.getfSeq().c_str()) << ". Wanted: " << fseq;
    throw TapeFormatError(ex_str.str());
  }
}

void ReadFile::setBlockSize(const UHL1 &uhl1) throw (Exception) {
  m_currentBlockSize = (size_t)atol(uhl1.getBlockSize().c_str());
  if(m_currentBlockSize<1) {
    std::stringstream ex_str;
    ex_str << "[ReadFile::setBlockSize] - Invalid block size in uhl1 detected";
    throw TapeFormatError(ex_str.str());
  }
}

void ReadFile::position(const FileInfo &fileInfo) throw (Exception) {  
  if(fileInfo.checksum==0 or fileInfo.nsFileId==0 or fileInfo.size==0 or fileInfo.fseq<1) {
    throw castor::exception::InvalidArgument();
  }
  
  if(m_session->getCurrentFilePart() != Header) {
    m_session->setCorrupted();
    throw SessionCorrupted();
  }
  
  if(m_positioningMode==ByBlockId) {
    // if we want the first file on tape (fileInfo.blockId==0) we need to skip the VOL1 header
    uint32_t destination_block = fileInfo.blockId ? fileInfo.blockId : 1;
    // we position using the sg locate because it is supposed to do the right thing possibly in a more optimized way (better than st's spaceBlocksForward/Backwards)
    m_session->m_drive.positionToLogicalObject(destination_block);// at this point we should be at the beginning of the headers of the desired file, so now let's check the headers...
  }
  else if(m_positioningMode==ByFSeq) {    
    int64_t fseq_delta = fileInfo.fseq - m_session->getCurrentFseq();
    if(fileInfo.fseq == 1) { // special case: we can rewind the tape to be faster (TODO: in the future we could also think of a threshold above which we rewind the tape anyway and then space forward)       
      m_session->m_drive.rewind();
      VOL1 vol1;
      m_session->m_drive.readExactBlock((void * )&vol1, sizeof(vol1), "[ReadFile::position] - Reading VOL1");
      try {
        vol1.verify();
      } catch (std::exception & e) {
        throw TapeFormatError(e.what());
      }
    }
    else if(fseq_delta == 0) {
      // do nothing we are in the correct place
    }
    else if(fseq_delta > 0) {
      m_session->m_drive.spaceFileMarksForward((uint32_t)fseq_delta*3); //we need to skip three file marks per file (header, payload, trailer)
    }
    else {
      m_session->m_drive.spaceFileMarksBackwards((uint32_t)abs(fseq_delta)*3+1); //we need to skip three file marks per file (trailer, payload, header) + 1 to go on the BOT (beginning of tape) side of the file mark before the header of the file we want to read
      m_session->m_drive.readFileMark("[ReadFile::position] Reading file mark right before the header of the file we want to read");
    }
  }
  else {
    throw UnsupportedPositioningMode();
  }
  
  //save the current fseq into the read session
  m_session->setCurrentFseq(fileInfo.fseq);
  m_session->setCurrentFilePart(Header);
  
  HDR1 hdr1;
  HDR2 hdr2;
  UHL1 uhl1;
  m_session->m_drive.readExactBlock((void *)&hdr1, sizeof(hdr1), "[ReadFile::position] - Reading HDR1");  
  m_session->m_drive.readExactBlock((void *)&hdr2, sizeof(hdr2), "[ReadFile::position] - Reading HDR2");
  m_session->m_drive.readExactBlock((void *)&uhl1, sizeof(uhl1), "[ReadFile::position] - Reading UHL1");
  m_session->m_drive.readFileMark("[ReadFile::position] - Reading file mark at the end of file header"); // after this we should be where we want, i.e. at the beginning of the file
  m_session->setCurrentFilePart(Payload);
  
  //the size of the headers is fine, now let's check each header  
  try {
    hdr1.verify();
    hdr2.verify();
    uhl1.verify();
  }
  catch (std::exception & e) {
    throw TapeFormatError(e.what());
  }
  
  //headers are valid here, let's see if they contain the right info, i.e. are we in the correct place?
  HeaderChecker::checkHDR1(hdr1, fileInfo, m_session->m_vid);
  //we disregard hdr2 on purpose as it contains no useful information, we now check the fseq in uhl1 (hdr1 also contains fseq info but it is modulo 10000, therefore useless)
  HeaderChecker::checkUHL1(uhl1, fileInfo);
  //now that we are all happy with the information contained within the headers, we finally get the block size for our file (provided it has a reasonable value)
  setBlockSize(uhl1);
}

size_t ReadFile::getBlockSize() throw (Exception) {
  if(m_currentBlockSize<1) {
    std::stringstream ex_str;
    ex_str << "[ReadFile::getBlockSize] - Invalid block size: " << m_currentBlockSize;
    throw TapeFormatError(ex_str.str());
  }
  return m_currentBlockSize;
}

size_t ReadFile::read(void *data, const size_t size) throw (Exception) {
  if(size!=m_currentBlockSize) {
    throw castor::exception::InvalidArgument();
  }
  size_t bytes_read = m_session->m_drive.readBlock(data, size);
  if(!bytes_read) {    // end of file reached! we will keep on reading until we have read the file mark at the end of the trailers
    m_session->setCurrentFilePart(Trailer);
    
    //let's read and check the trailers    
    EOF1 eof1;
    EOF2 eof2;
    UTL1 utl1;
    m_session->m_drive.readExactBlock((void *)&eof1, sizeof(eof1), "[ReadFile::read] - Reading HDR1");  
    m_session->m_drive.readExactBlock((void *)&eof2, sizeof(eof2), "[ReadFile::read] - Reading HDR2");
    m_session->m_drive.readExactBlock((void *)&utl1, sizeof(utl1), "[ReadFile::read] - Reading UTL1");
    m_session->m_drive.readFileMark("[ReadFile::read] - Reading file mark at the end of file trailer"); // after this we should be where we want, i.e. at the beginning of the file
    m_session->setCurrentFseq(m_session->getCurrentFseq() + 1); // moving on to the header of the next file 
    m_session->setCurrentFilePart(Header);

    //the size of the headers is fine, now let's check each header  
    try {
      eof1.verify();
      eof2.verify();
      utl1.verify();
    }
    catch (std::exception & e) {
      throw TapeFormatError(e.what());
    }
    // the following is a normal day exception: end of files exceptions are thrown at the end of each file being read    
    throw EndOfFile();
  }
  return bytes_read;
}

WriteSession::WriteSession(drives::DriveGeneric & drive, const std::string &volId, const uint32_t last_fseq, const bool compression) throw (Exception) : m_drive(drive), m_vid(volId), m_compressionEnabled(compression), m_corrupted(false), m_locked(false) {

  if(!volId.compare("")) {
    throw castor::exception::InvalidArgument();
  }
  
  m_drive.rewind();
  VOL1 vol1;
  m_drive.readExactBlock((void * )&vol1, sizeof(vol1), "[WriteSession::checkVOL1()] - Reading VOL1");
  try {
    vol1.verify();
  } catch (std::exception & e) {
    throw TapeFormatError(e.what());
  }  
  HeaderChecker::checkVOL1(vol1, volId); // now we know that we are going to write on the correct tape
  //if the tape is not empty let's move to the last trailer
  if(last_fseq>0) {
    uint32_t dst_filemark = last_fseq*3-1; // 3 file marks per file but we want to read the last trailer (hence the -1)
    m_drive.spaceFileMarksForward(dst_filemark);

    EOF1 eof1;
    EOF2 eof2;
    UTL1 utl1;
    m_drive.readExactBlock((void *)&eof1, sizeof(eof1), "[WriteSession::WriteSession] - Reading EOF1");  
    m_drive.readExactBlock((void *)&eof2, sizeof(eof2), "[WriteSession::WriteSession] - Reading EOF2");
    m_drive.readExactBlock((void *)&utl1, sizeof(utl1), "[WriteSession::WriteSession] - Reading UTL1");
    m_drive.readFileMark("[WriteSession::WriteSession] - Reading file mark at the end of file trailer"); // after this we should be where we want, i.e. at the end of the last trailer of the last file on tape

    //the size of the trailers is fine, now let's check each trailer  
    try {
      eof1.verify();
      eof2.verify();
      utl1.verify();
    } catch (std::exception & e) {
      throw TapeFormatError(e.what());
    }

    //trailers are valid here, let's see if they contain the right info, i.e. are we in the correct place?
    //we disregard eof1 and eof2 on purpose as they contain no useful information for us now, we now check the fseq in utl1 (hdr1 also contains fseq info but it is modulo 10000, therefore useless)
    HeaderChecker::checkUTL1(utl1, last_fseq);
  } 
  else {
    //else we are already where we want to be: at the end of the 80 bytes of the VOL1, all ready to write the headers of the first file
  }
  //now we need to get two pieces of information that will end up in the headers and trailers that we will write (siteName, hostName)
  setSiteName();
  setHostName();
}

void WriteSession::setHostName() throw (Exception) {
  char hostname_cstr[max_unix_hostname_length];
  castor::exception::Errnum::throwOnMinusOne(gethostname(hostname_cstr, max_unix_hostname_length), "Failed gethostname() in WriteFile::setHostName");
  m_hostName = hostname_cstr;
  std::transform(m_hostName.begin(), m_hostName.end(), m_hostName.begin(), ::toupper);
  m_hostName = m_hostName.substr(0, m_hostName.find("."));
}

void WriteSession::setSiteName() throw (Exception) {
  std::ifstream resolv;
  resolv.exceptions(std::ifstream::failbit|std::ifstream::badbit);
  try {
    resolv.open("/etc/resolv.conf", std::ifstream::in);
    std::string buf;
    while(std::getline(resolv, buf)) {
      if(std::string::npos != buf.find("search ")) {
        m_siteName = buf.substr(7);
        m_siteName = m_siteName.substr(0, m_siteName.find("."));
        std::transform(m_siteName.begin(), m_siteName.end(), m_siteName.begin(), ::toupper);
        break;
      }
    }
    resolv.close();
  }
  catch (std::ifstream::failure e) {
    throw castor::exception::Exception("Error opening, reading or closing /etc/resolv.conf");
  }
}

WriteFile::WriteFile(WriteSession *ws, const FileInfo info, const size_t blockSize) throw (Exception) : m_currentBlockSize(blockSize), m_session(ws), m_fileinfo(info), m_open(false), m_nonzeroFileWritten(false), m_numberOfBlocks(0) {
  if(m_session->isCorrupted()) {
    throw SessionCorrupted();
  }
  m_session->lock();
  HDR1 hdr1;
  HDR2 hdr2;
  UHL1 uhl1;
  std::stringstream s;
  s << std::hex << m_fileinfo.nsFileId;
  std::string fileId;
  s >> fileId;
  std::transform(fileId.begin(), fileId.end(), fileId.begin(), ::toupper);
  hdr1.fill(fileId, m_session->m_vid, m_fileinfo.fseq);
  hdr2.fill(m_currentBlockSize, m_session->m_compressionEnabled);
  uhl1.fill(m_fileinfo.fseq, m_currentBlockSize, m_session->getSiteName(), m_session->getHostName(), m_session->m_drive.getDeviceInfo());
  m_session->m_drive.writeBlock(&hdr1, sizeof(hdr1));
  m_session->m_drive.writeBlock(&hdr2, sizeof(hdr2));
  m_session->m_drive.writeBlock(&uhl1, sizeof(uhl1));
  m_session->m_drive.writeImmediateFileMarks(1);
  m_open=true;
}

uint32_t WriteFile::getPosition() throw (Exception) {  
  return m_session->m_drive.getPositionInfo().currentPosition;
}

void WriteFile::write(const void *data, const size_t size) throw (Exception) {
  m_session->m_drive.writeBlock(data, size);
  if(size>0) {
    m_nonzeroFileWritten = true;
    m_numberOfBlocks++;
  }
}

void WriteFile::close() throw (Exception) {
  if(!m_open) {
    m_session->setCorrupted();
    throw FileClosedTwice();
  }
  if(!m_nonzeroFileWritten) {
    m_session->setCorrupted();
    throw ZeroFileWritten();
  }
  m_session->m_drive.writeImmediateFileMarks(1); // filemark at the end the of data file
  EOF1 eof1;
  EOF2 eof2;
  UTL1 utl1;
  std::stringstream s;
  s << std::hex << m_fileinfo.nsFileId;
  std::string fileId;
  s >> fileId;
  std::transform(fileId.begin(), fileId.end(), fileId.begin(), ::toupper);
  eof1.fill(fileId, m_session->m_vid, m_fileinfo.fseq, m_numberOfBlocks);
  eof2.fill(m_currentBlockSize, m_session->m_compressionEnabled);
  utl1.fill(m_fileinfo.fseq, m_currentBlockSize, m_session->getSiteName(), m_session->getHostName(), m_session->m_drive.getDeviceInfo());
  m_session->m_drive.writeBlock(&eof1, sizeof(eof1));
  m_session->m_drive.writeBlock(&eof2, sizeof(eof2));
  m_session->m_drive.writeBlock(&utl1, sizeof(utl1));
  m_session->m_drive.writeImmediateFileMarks(1); // filemark at the end the of trailers
  m_open=false;
}

WriteFile::~WriteFile() throw () {
  if(m_open) {
    m_session->setCorrupted();
  }
  m_session->release();
}
