/*
 * @project        The CERN Tape Archive (CTA)
 * @copyright      Copyright(C) 2003-2021 CERN
 * @license        This program is free software: you can redistribute it and/or modify
 *                 it under the terms of the GNU General Public License as published by
 *                 the Free Software Foundation, either version 3 of the License, or
 *                 (at your option) any later version.
 *
 *                 This program is distributed in the hope that it will be useful,
 *                 but WITHOUT ANY WARRANTY; without even the implied warranty of
 *                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *                 GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public License
 *                 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "FilePositionEstimator.hpp"
#include "castor/tape/tapeserver/drive/DriveInterface.hpp"
#include "catalogue/MediaType.hpp"
#include <vector>

namespace castor { namespace tape { namespace tapeserver { namespace rao {

/**
 * This class implements an interpolation file position estimator.
 * The file position is determined by interpolating the position according to the
 * tape end of wrap positions and its mediatype.
 * @param endOfWrapPositions the end of wrap positions of the mounted tape
 * @param mediaType the media type of the mounted tape
 */
class InterpolationFilePositionEstimator : public FilePositionEstimator{
public:
  InterpolationFilePositionEstimator(const std::vector<drive::endOfWrapPosition> & endOfWrapPositions, const cta::catalogue::MediaType & mediaType);
  FilePositionInfos getFilePosition(const cta::RetrieveJob& job) const override;
  virtual ~InterpolationFilePositionEstimator();
  
  static const uint64_t c_blockSize = 256 * 1024;
private:
  std::vector<drive::endOfWrapPosition> m_endOfWrapPositions;
  cta::catalogue::MediaType m_mediaType;
  
  /**
   * In order for this position estimator to work, we need to check that the mediatype given in the constructor contains
   * the information we need.
   */
  void checkMediaTypeConsistency();
  
  /**
   * Determine the physical position of the blockId passed in parameter
   * @param blockId the blockId to determine its physical position
   */
  Position getPhysicalPosition(const uint64_t blockId) const;
  /**
   * Determine the wrap number in which the blockId passed in parameter is located
   * @param blockId the blockId to determine its wrap number
   */
  uint64_t determineWrapNb(const uint64_t blockId) const;
  /**
   * This method determine the longitudinal position (LPos) of the blockId 
   * passed in parameter
   * @param blockId the blockId to determine its longitudinal position
   * @param wrapNumber the wrapNumber where the blockId is located
   * @return the longitudinal position that has a value comprised between mediaType.minLPos and mediaType.maxLPos
   */
  uint64_t determineLPos(const uint64_t blockId, const uint64_t wrapNumber) const;
  /**
   * Determine the blockId of the end of the file passed in parameter
   * @param file the file to determine its end blockId
   * @return the blockId of the end of the file passed in parameter
   */
  uint64_t determineEndBlockId(const cta::common::dataStructures::TapeFile & file) const;
};

}}}}