/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2019  CERN
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

namespace cta { namespace utils { namespace json { namespace object { 

/**
 * Interface that allows the objects that inherits from the implementations of this interface
 * to be built from JSON or to generate the json string representation from its attributes 
 */
class JSONObject {
public:
  /**
   * Return the JSON representation of this object
   */
  virtual std::string getJSON() = 0;
  /**
   * Set the inherited object attributes from the json passed in parameter
   * @param json the json string used to set the inherited object attributes
   * @throws JSONObjectException if the json does not contain the correct key-value attributes
   */
  virtual void buildFromJSON(const std::string & json) = 0;
  /**
   * Return the inherited object expected JSON structure allowing to set its attributes
   * via the buildFromJSON() method
   * @return an example of JSON allowing to build the object e.g {"freeSpace",42}
   */
  virtual std::string getExpectedJSONToBuildObject() const = 0;
  virtual ~JSONObject();
private:

};

}}}}