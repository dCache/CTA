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

#include "JSONFreeSpace.hpp"

namespace cta { namespace disk {

JSONFreeSpace::JSONFreeSpace(): JSONCObject() {
}

void JSONFreeSpace::buildFromJSON(const std::string& json) {
  JSONCObject::buildFromJSON(json);
  m_freeSpace = jsonGetValue<uint64_t>("freeSpace");
}

std::string JSONFreeSpace::getJSON(){
  reinitializeJSONCObject();
  jsonSetValue("freeSpace",m_freeSpace);
  return JSONCObject::getJSON();
}

std::string JSONFreeSpace::getExpectedJSONToBuildObject() const {
  return "{\"freeSpace\":42}";
}

JSONFreeSpace::~JSONFreeSpace() {
}

}}