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

#include "common/checksum/Checksum.hpp"

#include <sstream>

//------------------------------------------------------------------------------
// checksumTypeToStr
//------------------------------------------------------------------------------
const char *cta::Checksum::checksumTypeToStr(const ChecksumType enumValue)
  throw() {
  switch(enumValue) {
  case CHECKSUMTYPE_NONE   : return "NONE";
  case CHECKSUMTYPE_ADLER32: return "ADLER32";
  default                  : return "UNKNOWN";
  }
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::Checksum::Checksum(): m_type(CHECKSUMTYPE_NONE) {
}
  
//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::Checksum::Checksum(const ChecksumType &type, const ByteArray &byteArray):
  m_type(type),
  m_byteArray(byteArray) {
}

//------------------------------------------------------------------------------
// operator==
//------------------------------------------------------------------------------
bool cta::Checksum::operator==(const Checksum &rhs) const {
  return m_type == rhs.m_type && m_byteArray == rhs.m_byteArray;
}

//------------------------------------------------------------------------------
// getType
//------------------------------------------------------------------------------
cta::Checksum::ChecksumType cta::Checksum::getType() const throw() {
  return m_type;
}

//------------------------------------------------------------------------------
// getByteArray
//------------------------------------------------------------------------------
const cta::ByteArray &cta::Checksum::getByteArray() const throw() {
  return m_byteArray;
}

//------------------------------------------------------------------------------
// str
//------------------------------------------------------------------------------
std::string cta::Checksum::str() const {
  const auto arraySize = m_byteArray.getSize();
  std::ostringstream oss;

  if(0 < arraySize) {
    uint32_t checkSumInt32 = *((uint32_t *)(m_byteArray.getBytes()));
    oss << "0x" << std::hex << checkSumInt32;
  }

  return oss.str();
}
