/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
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

#include "common/exception/Exception.hpp"
#include "mediachanger/RmcMountMsgBody.hpp"
#include "mediachanger/RmcUnmountMsgBody.hpp"

namespace cta    {
namespace mediachanger {

/**
 * Marshals the specified source message body structure and its implicit
 * header into the specified destination buffer.
 *
 * @param dst    The destination message buffer.
 * @param dstLen The length of the destination buffer.
 * @param src    The source structure.
 * @return       The total length of the message (header + body).
 */
size_t marshal(char *const dst, const size_t dstLen, const RmcMountMsgBody &src) ;

/**
 * Marshals the specified source message body structure and its implicit
 * header into the specified destination buffer.
 *
 * @param dst The destination message buffer.
 * @param src The source structure.
 * @return    The total length of the message (header + body).
 */
template<int n> size_t marshal(char (&dst)[n], const RmcMountMsgBody &src)  {
  return marshal(dst, n, src);
}

/**
 * Unmarshals a message body with the specified destination structure type
 * from the specified source buffer.
 *
 * @param src In/out parameter, before invocation points to the source
 * buffer where the message body should be unmarshalled from and on return
 * points to the byte in the source buffer immediately after the
 * unmarshalled message body.
 * @param srcLen In/out parameter, before invocation is the length of the
 * source buffer from where the message body should be unmarshalled and on
 * return is the number of bytes remaining in the source buffer.
 * @param dst The destination message body structure.
 */
void unmarshal(const char * &src, size_t &srcLen, RmcMountMsgBody &dst) ;

/**
 * Marshals the specified source message body structure and its implicit
 * header into the specified destination buffer.
 *
 * @param dst    The destination message buffer.
 * @param dstLen The length of the destination buffer.
 * @param src    The source structure.
 * @return       The total length of the message (header + body).
 */
size_t marshal(char *const dst, const size_t dstLen, const RmcUnmountMsgBody &src) ;

/**
 * Marshals the specified source message body structure and its implicit
 * header into the specified destination buffer.
 *
 * @param dst The destination message buffer.
 * @param src The source structure.
 * @return    The total length of the message (header + body).
 */
template<int n> size_t marshal(char (&dst)[n], const RmcUnmountMsgBody &src)  {
  return marshal(dst, n, src);
}

/**
 * Unmarshals a message body with the specified destination structure type
 * from the specified source buffer.
 *
 * @param src In/out parameter, before invocation points to the source
 * buffer where the message body should be unmarshalled from and on return
 * points to the byte in the source buffer immediately after the
 * unmarshalled message body.
 * @param srcLen In/out parameter, before invocation is the length of the
 * source buffer from where the message body should be unmarshalled and on
 * return is the number of bytes remaining in the source buffer.
 * @param dst The destination message body structure.
 */
void unmarshal(const char * &src, size_t &srcLen, RmcUnmountMsgBody &dst) ;

} // namespace mediachanger
} // namespace cta

