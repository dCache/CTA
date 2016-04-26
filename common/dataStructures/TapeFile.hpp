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

#include <list>
#include <map>
#include <stdint.h>
#include <string>


namespace cta {
namespace common {
namespace dataStructures {

/**
 * This struct holds location information of a specific tape file 
 */
struct TapeFile {

  TapeFile();

  bool operator==(const TapeFile &rhs) const;

  bool operator!=(const TapeFile &rhs) const;

  /**
   * The volume indentifier of the tape on which the file has been written.
   */
  std::string vid;

  /**
   * The copy number of the file.  Copy numbers start from 1.
   */
  uint64_t copyNb;

  /**
   * The position of the file on tape in the form of its file sequence number.
   */
  uint64_t fSeq;

  /**
   * The position of the file on tape in the form of its logical block identifier.
   */
  uint64_t blockId;

  /**
   * The time the file recorded in the catalogue.
   */
  time_t creationTime;

}; // struct TapeFile

} // namespace dataStructures
} // namespace common
} // namespace cta

std::ostream &operator<<(std::ostream &os, const cta::common::dataStructures::TapeFile &obj);
