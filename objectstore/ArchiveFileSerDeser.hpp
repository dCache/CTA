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

#include "common/UserIdentity.hpp"
#include "objectstore/cta.pb.h"
#include "common/dataStructures/TapeFile.hpp"
#include "EntryLogSerDeser.hpp"
#include "TapeFileSerDeser.hpp"
#include "DiskFileInfoSerDeser.hpp"

#include <string>
#include <stdint.h>
#include <limits>

namespace cta { namespace objectstore {
/**
 * A decorator class of scheduler's creation log adding serialization.
 */
class ArchiveFileSerDeser: public cta::common::dataStructures::ArchiveFile {
public:
  ArchiveFileSerDeser (): cta::common::dataStructures::ArchiveFile() {}
  ArchiveFileSerDeser (const cta::common::dataStructures::ArchiveFile & af): cta::common::dataStructures::ArchiveFile(af) {}
  operator cta::common::dataStructures::ArchiveFile() {
    return cta::common::dataStructures::ArchiveFile(*this);
  } 
  void serialize (cta::objectstore::serializers::ArchiveFile & osaf) const {
    osaf.set_archivefileid(archiveFileID);
    osaf.set_creationtime(creationTime);
    osaf.set_checksumtype(checksumType);
    osaf.set_checksumvalue(checksumValue);
    osaf.set_creationtime(creationTime);
    DiskFileInfoSerDeser dfisd(diskFileInfo);
    dfisd.serialize(*osaf.mutable_diskfileinfo());
    osaf.set_diskfileid(diskFileId);
    osaf.set_diskinstance(diskInstance);
    osaf.set_filesize(fileSize);
    osaf.set_reconciliationtime(reconciliationTime);
    osaf.set_storageclass(storageClass);
    for (auto & tf: tapeFiles)
      TapeFileSerDeser(tf.second).serialize(*osaf.add_tapefiles());
  }
  
  void deserialize (const cta::objectstore::serializers::ArchiveFile & osaf) {
    tapeFiles.clear();
    archiveFileID=osaf.archivefileid();
    creationTime=osaf.creationtime();
    checksumType=osaf.checksumtype();
    checksumValue=osaf.checksumvalue();
    diskFileId=osaf.diskfileid();
    DiskFileInfoSerDeser dfisd;
    dfisd.deserialize(osaf.diskfileinfo());
    diskFileInfo=dfisd;
    diskInstance=osaf.diskinstance();
    // TODO rename to filesize.
    fileSize=osaf.filesize();
    reconciliationTime=osaf.reconciliationtime();
    storageClass=osaf.storageclass();
    for (auto tf: osaf.tapefiles()) {
      TapeFileSerDeser tfsd;
      tfsd.deserialize(tf);
      tapeFiles[tfsd.copyNb]=tfsd;
    }
  }
};
  
}}
