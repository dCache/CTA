/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2015-2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mediachanger/io.hpp"
#include "mediachanger/CommonMarshal.hpp"

#include <errno.h>
#include <iostream>
#include <string.h>

namespace cta {
namespace mediachanger {

//-----------------------------------------------------------------------------
// marshal
//-----------------------------------------------------------------------------
size_t marshal(char *const dst, const size_t dstLen, const MessageHeader &src) {

  if(dst == NULL) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to marshal MessageHeader"
      << ": Pointer to destination buffer is NULL";
    throw ex;
  }

  // Calculate the length of the message header
  const uint32_t totalLen = 3 * sizeof(uint32_t);  // magic + reqType + len

  // Check that the message header buffer is big enough
  if(totalLen > dstLen) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to marshal MessageHeader"
      ": Buffer too small : required=" << totalLen << " actual=" << dstLen;
    throw ex;
  }

  // Marshal the message header
  char *p = dst;
  marshalUint32(src.magic      , p);
  marshalUint32(src.reqType    , p);
  marshalUint32(src.lenOrStatus, p);

  // Calculate the number of bytes actually marshalled
  const size_t nbBytesMarshalled = p - dst;

  // Check that the number of bytes marshalled was what was expected
  if(totalLen != nbBytesMarshalled) {
    cta::exception::Exception ex;
    ex.getMessage() << "Failed to marshal MessageHeader"
      ": Mismatch between expected total length and actual"
      ": expected=" << totalLen << " actual=" << nbBytesMarshalled;
    throw ex;
  }

  return totalLen;
}


//-----------------------------------------------------------------------------
// unmarshal
//-----------------------------------------------------------------------------
void unmarshal(const char * &src, size_t &srcLen, MessageHeader &dst)  {

  unmarshalUint32(src, srcLen, dst.magic);
  unmarshalUint32(src, srcLen, dst.reqType);
  unmarshalUint32(src, srcLen, dst.lenOrStatus);
}

} // namespace mediachanger
} // namespace cta
