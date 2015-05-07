/**
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

#include <string>

namespace cta {

/**
 * Class representing a file atrribute.
 */
struct FileAttribute {

  /**
   * The name of the attribute.
   */
  std::string name;

  /**
   * The value of the attribute.
   */
  std::string value;

  /**
   * Constructor.
   */
  FileAttribute();

  /**
   * Constructor.
   *
   * @param name The name of the attribute.
   * @param value value of the attribute.
   */
  FileAttribute(const std::string &name, const std::string &value);

}; // struct FileAttribute

} // namespace cta
