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

#include "common/archiveNS/TapeFileLocation.hpp"
#include "common/checksum/Checksum.hpp"

#include <stdint.h>

namespace cta {

/**
 * A tape file entry within the archive namespace.
 */
struct NameServerTapeFile {

  /**
   * Default constructor.
   */
  NameServerTapeFile();

  /**
   * Equality operator.
   *
   * @param ths The right hand side of the operator.
   */
  bool operator==(const NameServerTapeFile &rhs) const;

  /**
   * The location of the file on tape.
   */
  TapeFileLocation tapeFileLocation;

  /**
   * The uncompressed size of the tape file in bytes.
   */
  uint64_t size;

  /**
   * The compressed size of the tape file in bytes.  In other words the actual
   * number of bytes it occupies on tape.
   */
  uint64_t compressedSize;

  /**
   * The checksum of the tape file.
   */
  Checksum checksum;

}; // class NameServer

} // namespace cta
