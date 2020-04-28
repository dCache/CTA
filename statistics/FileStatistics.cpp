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


#include "FileStatistics.hpp"

namespace cta { 
namespace statistics {

FileStatistics::FileStatistics() {
}

FileStatistics::FileStatistics(const FileStatistics &other){
  nbMasterFiles = other.nbMasterFiles;
  masterDataInBytes = other.masterDataInBytes;
  nbCopyNb1 = other.nbCopyNb1;
  copyNb1InBytes = other.copyNb1InBytes;
  nbCopyNbGt1 = other.nbCopyNbGt1;
  copyNbGt1InBytes = other.copyNbGt1InBytes;
}

FileStatistics FileStatistics::operator +=(const FileStatistics& other) {
  nbMasterFiles += other.nbMasterFiles;
  masterDataInBytes += other.masterDataInBytes;
  nbCopyNb1 += other.nbCopyNb1;
  copyNb1InBytes += other.copyNb1InBytes;
  nbCopyNbGt1 += other.nbCopyNbGt1;
  copyNbGt1InBytes += other.copyNbGt1InBytes;
  return *this;
}

FileStatistics& FileStatistics::operator =(const FileStatistics& other){
  nbMasterFiles = other.nbMasterFiles;
  masterDataInBytes = other.masterDataInBytes;
  nbCopyNb1 = other.nbCopyNb1;
  copyNb1InBytes = other.copyNb1InBytes;
  nbCopyNbGt1 = other.nbCopyNbGt1;
  copyNbGt1InBytes = other.copyNbGt1InBytes;
  return *this;
}

}}
