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

#pragma once

#include "common/exception/Exception.hpp"

namespace cta {
namespace exception {

/**
 * Exception representing an unexpected mismatch between checksum types.
 */
class ChecksumBlobSizeMismatch: public exception::Exception {
public:
  /**
   * Constructor
   *
   * @param context optional context string added to the message at initialisation time
   * @param embedBacktrace whether to embed a backtrace of where the exception was thrown in the message
   */
  ChecksumBlobSizeMismatch(const std::string &context = "", const bool embedBacktrace = true) {}

  /**
   * Destructor
   */
  ~ChecksumBlobSizeMismatch() override {}
}; // class ChecksumTypeMismatch

}} // namespace cta::exception