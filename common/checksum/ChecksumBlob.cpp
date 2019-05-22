/*!
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2019 CERN
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

#include <iomanip>

#include "ChecksumBlob.hpp"
#include "ChecksumBlobSerDeser.hpp"

namespace cta {
namespace checksum {

void ChecksumBlob::insert(ChecksumType type, const std::string &value) {
  // Validate the length of the checksum
  size_t expectedLength;
  switch(type) {
    case NONE:       expectedLength = 0;  break;
    case ADLER32:
    case CRC32:
    case CRC32C:     expectedLength = 4;  break;
    case MD5:        expectedLength = 16; break;
    case SHA1:       expectedLength = 20; break;
  }
  if(value.length() > expectedLength) throw exception::ChecksumValueMismatch(
    "Checksum length type=" + ChecksumTypeName.at(type) +
    " expected=" + std::to_string(expectedLength) +
      " actual=" + std::to_string(value.length()));
  // Pad bytearray to expected length with trailing zeros
  m_cs[type] = value + std::string(expectedLength-value.length(), 0);
}

void ChecksumBlob::insert(ChecksumType type, uint32_t value) {
  // This method is only valid for 32-bit checksums
  std::string cs;
  switch(type) {
    case ADLER32:
    case CRC32:
    case CRC32C:
      for(int i = 0; i < 4; ++i) {
        cs.push_back(static_cast<unsigned char>(value & 0xFF));
        value >>= 8;
      }
      m_cs[type] = cs;
      break;
    default:
      throw exception::ChecksumTypeMismatch(ChecksumTypeName.at(type) + " is not a 32-bit checksum");
  }
}

void ChecksumBlob::validate(ChecksumType type, const std::string &value) const {
  auto cs = m_cs.find(type);
  if(cs == m_cs.end()) throw exception::ChecksumTypeMismatch(
      "Checksum type " + ChecksumTypeName.at(type) + " not found");
  if(cs->second != value) throw exception::ChecksumValueMismatch(
      "Checksum value expected=0x" + ByteArrayToHex(value) +
                       "actual=0x" + ByteArrayToHex(cs->second));
}

void ChecksumBlob::validate(const ChecksumBlob &blob) const {
  if(m_cs.size() != blob.m_cs.size()) {
    throw exception::ChecksumBlobSizeMismatch("Checksum blob size does not match. expected=" +
      std::to_string(m_cs.size()) + " actual=" + std::to_string(blob.m_cs.size()));
  }

  auto it1 = m_cs.begin();
  auto it2 = blob.m_cs.begin();
  for( ; it1 != m_cs.end(); ++it1, ++it2) {
    if(it1->first != it2->first) throw exception::ChecksumTypeMismatch(
      "Checksum type expected=" + ChecksumTypeName.at(it1->first) +
                      "actual=" + ChecksumTypeName.at(it2->first));
    if(it1->second != it2->second) throw exception::ChecksumValueMismatch(
      "Checksum value expected=0x" + ByteArrayToHex(it1->second) +
                       "actual=0x" + ByteArrayToHex(it2->second));
  }
}

std::string ChecksumBlob::serialize() const {
  common::ChecksumBlob p_csb;
  ChecksumBlobToProtobuf(*this, p_csb);

  std::string bytearray;
  p_csb.SerializeToString(&bytearray);
  return bytearray;
}

size_t ChecksumBlob::length() const {
  common::ChecksumBlob p_csb;
  ChecksumBlobToProtobuf(*this, p_csb);
  return p_csb.ByteSizeLong();
}

void ChecksumBlob::deserialize(const std::string &bytearray) {
  common::ChecksumBlob p_csb;
  p_csb.ParseFromString(bytearray);
  ProtobufToChecksumBlob(p_csb, *this);
}

std::string ChecksumBlob::HexToByteArray(std::string hexString) {
  std::string bytearray;

  if(hexString.substr(0,2) == "0x" || hexString.substr(0,2) == "0X") {
    hexString.erase(0,2);
  }
  // ensure we have an even number of hex digits
  if(hexString.length() % 2 == 1) hexString.insert(0, "0");

  for(unsigned int i = 0; i < hexString.length(); i += 2) {
    uint8_t byte = strtol(hexString.substr(i,2).c_str(), nullptr, 16);
    bytearray.insert(0,1,byte);
  }

  return bytearray;
}

std::string ChecksumBlob::ByteArrayToHex(const std::string &bytearray) {
  if(bytearray.empty()) return "0";

  std::stringstream value;
  value << std::hex << std::setfill('0');
  for(auto c = bytearray.rbegin(); c != bytearray.rend(); ++c) {
    value << std::setw(2) << (static_cast<uint8_t>(*c) & 0xFF);
  }
  return value.str();
}

std::ostream &operator<<(std::ostream &os, const ChecksumBlob &csb) {
  os << "[ ";
  auto num_els = csb.m_cs.size();
  for(auto &cs : csb.m_cs) {
    bool is_last_el = --num_els > 0;
    os << "{ \"" << ChecksumTypeName.at(cs.first) << "\",0x"  << ChecksumBlob::ByteArrayToHex(cs.second)
       << (is_last_el ? " }," : " }");
  }
  os << " ]";

  return os;
}

}} // namespace cta::checksum