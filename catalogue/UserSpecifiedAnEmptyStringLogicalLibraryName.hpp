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

#pragma once

#include "common/exception/UserError.hpp"

namespace cta {
namespace catalogue {

/**
 * User specified an empty string for a logical library name when this is not
 * permitted.
 */
class UserSpecifiedAnEmptyStringLogicalLibraryName: public exception::UserError {
public:
  /**
   * Constructor.
   *
   * @param context optional context string added to the message
   * at initialisation time.
   * @param embedBacktrace whether to embed a backtrace of where the
   * exception was throw in the message
   */
  UserSpecifiedAnEmptyStringLogicalLibraryName(const std::string &context = "", const bool embedBacktrace = true);

  /**
   * Destructor.
   */
  ~UserSpecifiedAnEmptyStringLogicalLibraryName() override;
}; // class UserSpecifiedAnEmptyStringLogicalLibraryName

} // namespace catalogue
} // namespace cta