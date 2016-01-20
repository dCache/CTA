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

#include <stdint.h>
#include <string>

#include "common/DRInfo.hpp"

namespace cta {

/**
 * Structure to store an archive request.
 */
struct ArchiveRequest {
  
  /**
   * The EOS src URL.
   */
  std::string src_URL;

  /**
   * The size of the file to be archived in bytes.
   */
  uint64_t fileSize;

  /**
   * The checksum type.
   */
  std::string checksumType;

  /**
   * The checksum value.
   */
  std::string checksumValue;
  
  /**
   * The storage class name.
   */
  std::string storageClass;
  
  /**
   * The disaster recovery info
   */
  cta::DRInfo drInfo;

}; // struct ArchiveRequest

} // namespace cta
