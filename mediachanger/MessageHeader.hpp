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

#include <stdint.h>

namespace cta {
namespace mediachanger {

/**
 * A message header
 */
struct MessageHeader {
  /**
   * The magic number of the message.
   */
  uint32_t magic;

  /**
   * The request type of the message.
   */
  uint32_t reqType;

  /**
   * The length of the message body in bytes if this is the header of any
   * message other than an acknowledge message.  If this is the header of
   * an acknowledge message then there is no message body and this field is
   * used to pass the status of the acknowledge.
   */
  uint32_t lenOrStatus;

  /**
   * Constructor.
   *
   * Sets all integer member-variables to 0.
   */
  MessageHeader() throw();
}; // struct MessageHeader

} // namespace mediachanger
} // namespace cta

