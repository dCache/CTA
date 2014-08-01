/******************************************************************************
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
#include <zlib.h>
#include "castor/tape/tapeserver/file/DiskFile.hpp"
#include "castor/tape/tapeserver/file/File.hpp"
#include "castor/exception/MemException.hpp"
#pragma once 

namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  
/**
 * Class managing a fixed size payload buffer. Some member functions also
 * allow read
 * @param capacity Size of the payload buffer in bytes
 */
class Payload
{
  Payload(const Payload&);
  Payload& operator=(const Payload&);
public:
  Payload(size_t capacity):
  m_data(new (std::nothrow) unsigned char[capacity]),m_totalCapacity(capacity),m_size(0) {
    if(NULL == m_data) {
      throw castor::exception::MemException("Failed to allocate memory for a new MemBlock!");
    }
  }
  
  ~Payload(){
    delete[] m_data;
  }
  
  /** Amount of data present in the payload buffer */
  size_t size() const {
    return m_size;
  }
  
  /** Reset the internal counters of the payload */
  void reset() {
    m_size = 0;
  }
  
  /** Remaining free space in the payload buffer */
  size_t remainingFreeSpace() const {
    return m_totalCapacity - m_size;
  }
  
  /** Total size of the payload block */
  size_t totalCapacity() const {
    return m_totalCapacity;
  }
    
  /** Returns a pointer to the beginning of the payload block */
  unsigned char* get(){
    return m_data;
  }
  
  /** Returns a pointer to the beginning of the payload block (readonly version) */
  unsigned char const*  get() const {
    return m_data;
  }
  
  /** 
   * Reads all the buffer in one go from a diskFile::ReadFile object 
   * @param from reference to the diskFile::ReadFile
   */
  size_t read(tape::diskFile::ReadFile& from){
    m_size = from.read(m_data,m_totalCapacity);
    return m_size;
  }

  /**
   * Reads one block from a tapeFile::readFile
   * @throws castor::tape::daemon::Payload::EOF
   * @param from reference to the tapeFile::ReadFile
   * @return whether another tape block will fit in the memory block.
   */
  bool append(tape::tapeFile::ReadFile & from){
    if (from.getBlockSize() > remainingFreeSpace()) {
      std::stringstream err;
      err << "Trying to read a tape file block with too little space left: BlockSize="
       << from.getBlockSize() << " remainingFreeSpace=" << remainingFreeSpace()
              << " (totalSize=" << m_totalCapacity << ")"; 
      throw castor::exception::MemException(err.str());
    }
    size_t readSize;
    try {
      readSize = from.read(m_data + m_size, from.getBlockSize());
    } catch (castor::tape::tapeFile::EndOfFile&) {
      throw castor::tape::exceptions::EndOfFile("In castor::tape::tapeserver::daemon::Payload::append: reached end of file");
    }
    m_size += readSize;
    return  from.getBlockSize() <= remainingFreeSpace();
  }
  
  /**
   * Write the complete buffer to a diskFile::WriteFile
   * @param to reference to the diskFile::WriteFile
   */
  void write(tape::diskFile::WriteFile& to){
    to.write(m_data,m_size);
  }
  
  /**
   * Write the complete buffer to a tapeFile::WriteFile, tape block by
   * tape block
   * @param to reference to the tapeFile::WriteFile
   */
  void write(tape::tapeFile::WriteFile& to) {
    size_t blockSize = to.getBlockSize();
    size_t writePosition = 0;
    // Write all possible full tape blocks
    while (m_size - writePosition > blockSize) {
      to.write(m_data + writePosition, blockSize);
      writePosition += blockSize;
    }
    // Write a remainder, if any
    if (m_size - writePosition) {
      to.write(m_data + writePosition, m_size - writePosition);
    }
  }
  
  /*
   Example for the Adler32
   * unsigned long chck = Pyaload::zeroAdler32();
   * while() {
   *   payload.read(from);
   *   chck = payload.adler32(chck);
   * }
   */
  
  
   /**
    * Compute adler32 checksum on the current data hold.
    * @param previous The previous adler32 checksum from all previous datablock
    * @return the updated checksum
    */
  unsigned long  adler32(unsigned long previous){
    return ::adler32(previous,m_data,m_size);
  }
  
  /**
   * Return the initial value for computing Adler32 checksum
   */
  static unsigned long zeroAdler32() {
     return  ::adler32(0L,Z_NULL,0);
   }
private:
  unsigned char* m_data;
  size_t m_totalCapacity;
  size_t m_size;
};

}}}}
