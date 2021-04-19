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

#include "common/log/Logger.hpp"
#include "mediachanger/MediaChangerProxy.hpp"

namespace cta {
namespace mediachanger {

/**
 * Manual media changer proxy that simply logs mount and dismount requests.
 */
class MmcProxy: public MediaChangerProxy {
public:

  /**
   * Constructor.
   *
   * @param log Object representing the API to the CTA logging system.
   */
  MmcProxy(log::Logger &log);

  /**
   * Requests the media changer to mount the specified tape for read-only
   * access into the drive in the specified library slot.
   *
   * Please note that this method provides a best-effort service because not all
   * media changers support read-only mounts.
   *
   * @param vid The volume identifier of the tape.
   * @param librarySlot The library slot containing the tape drive.
   */
  void mountTapeReadOnly(const std::string &vid, const LibrarySlot &librarySlot) override;

  /**
   * Requests the media changer to mount the specified tape for read/write
   * access into the drive in the specified library slot.
   *
   * @param vid The volume identifier of the tape.
   * @param librarySlot The library slot containing the tape drive.
   */
  void mountTapeReadWrite(const std::string &vid, const LibrarySlot &librarySlot) override;

  /** 
   * Requests the media changer to dismount the specified tape from the
   * drive in the specifed library slot.
   *
   * @param vid The volume identifier of the tape.
   * @param librarySlot The library slot containing the tape drive.
   */
  void dismountTape(const std::string &vid, const LibrarySlot &librarySlot) override;

private:

  /**
   * Object representing the API to the CTA logging system.
   */
  log::Logger &m_log;

}; // class MmcProxy

} // namespace mediachanger
} // namespace cta
