/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "Wrapper.hpp"
#include <errno.h>
#include <limits.h>
#include <stdexcept>
#include <scsi/sg.h>

using ::testing::_;
using ::testing::A;
using ::testing::An;
using ::testing::Invoke;

using namespace castor::tape;

DIR* System::fakeWrapper::opendir(const char* name) {
  /* Manage absence of directory */
  if (m_directories.end() == m_directories.find(std::string(name))) {
    errno = ENOENT;
    return NULL;
  }
  /* Dirty pointer gymnastics. Good enough for a test harness */
  ourDIR * dir = new ourDIR;
  dir->nextIdx = 0;
  dir->path = name;
  return (DIR*) dir;
}

int System::fakeWrapper::closedir(DIR* dirp) {
  delete ((ourDIR *) dirp);
  return 0;
}

struct dirent * System::fakeWrapper::readdir(DIR* dirp) {
  /* Dirty pointer gymnastics. Good enough for a test harness */
  ourDIR & dir = *((ourDIR *) dirp);
  /* Check we did not reach end of directory. This will create a new
   * entry in the map if it does not exist, but we should be protected by
   * opendir.
   */
  if (dir.nextIdx + 1 > m_directories[dir.path].size())
    return NULL;
  dir.dent_name = m_directories[dir.path][dir.nextIdx++];
  strncpy(dir.dent.d_name, dir.dent_name.c_str(), NAME_MAX);
  return & (dir.dent);
}

int System::fakeWrapper::readlink(const char* path, char* buf, size_t len) {
  /*
   * Mimic readlink. see man 3 readlink.
   */
  if (m_links.end() == m_links.find(std::string(path))) {
    errno = ENOENT;
    return -1;
  }
  const std::string & link = m_links[std::string(path)];
  /* Copy the link without the training \0 as it is the behavior 
   of the real readlink */
  size_t lenToCopy = link.size();
  if (lenToCopy > len) lenToCopy = len;
  link.copy(buf, lenToCopy);
  return len > link.size() ? link.size() : len;
}

char * System::fakeWrapper::realpath(const char* name, char* resolved) {
  /*
   * Mimic realpath. see man 3 realpath.
   */
  if (m_realpathes.end() == m_realpathes.find(std::string(name))) {
    errno = ENOENT;
    return NULL;
  }
  strncpy(resolved, m_realpathes[std::string(name)].c_str(), PATH_MAX);
  return resolved;
}

int System::fakeWrapper::open(const char* file, int oflag) {
  /* 
   * Mimic open. See man 2 open.
   * We only allow read for the moment.
   */
  if (oflag & (O_APPEND | O_CREAT)) {
    errno = EACCES;
    return -1;
  }
  if (m_files.end() == m_files.find(std::string(file))) {
    errno = ENOENT;
    return -1;
  }
  int ret = m_nextFD++;
  m_openFiles[ret] = m_files[std::string(file)];
  m_openFiles[ret]->reset();
  return ret;
}

ssize_t System::fakeWrapper::read(int fd, void* buf, size_t nbytes) {
  /*
   * Mimic read. See man 2 read
   */
  if (m_openFiles.end() == m_openFiles.find(fd)) {
    errno = EBADF;
    return -1;
  }
  return m_openFiles[fd]->read(buf, nbytes);
}

ssize_t System::fakeWrapper::write(int fd, const void *buf, size_t nbytes) {
  if (m_openFiles.end() == m_openFiles.find(fd)) {
    errno = EBADF;
    return -1;
  }
  return m_openFiles[fd]->write(buf, nbytes);
}

int System::fakeWrapper::ioctl(int fd, unsigned long int request, mtop * mt_cmd) {
  /*
   * Mimic ioctl. Actually delegate the job to a vfsFile
   */
  if (m_openFiles.end() == m_openFiles.find(fd)) {
    errno = EBADF;
    return -1;
  }
  return m_openFiles[fd]->ioctl(request, mt_cmd);
}

int System::fakeWrapper::ioctl(int fd, unsigned long int request, mtget* mt_status) {
  /*
   * Mimic ioctl. Actually delegate the job to a vfsFile
   */
  if (m_openFiles.end() == m_openFiles.find(fd)) {
    errno = EBADF;
    return -1;
  }
  return m_openFiles[fd]->ioctl(request, mt_status);
}

int System::fakeWrapper::ioctl(int fd, unsigned long int request, sg_io_hdr_t * sgh) {
  /*
   * Mimic ioctl. Actually delegate the job to a vfsFile
   */
  if (m_openFiles.end() == m_openFiles.find(fd)) {
    errno = EBADF;
    return -1;
  }
  return m_openFiles[fd]->ioctl(request, sgh);
}

int System::fakeWrapper::close(int fd) {
  /*
   * Mimic close. See man 2 close
   */
  if (m_openFiles.end() == m_openFiles.find(fd)) {
    errno = EBADF;
    return -1;
  }
  m_openFiles.erase(fd);
  return 0;
}

int System::fakeWrapper::stat(const char* path, struct stat* buf) {
  /*
   * Mimic stat. See man 2 stat
   */
  if (m_stats.end() == m_stats.find(std::string(path))) {
    errno = ENOENT;
    return -1;
  }
  *buf = m_stats[std::string(path)];
  return 0;
}

castor::tape::drives::DriveInterface * 
  System::fakeWrapper::getDriveByPath(const std::string & path) {
  std::map<std::string, castor::tape::drives::DriveInterface *>::iterator drive =
    m_pathToDrive.find(path);
  if (m_pathToDrive.end() == drive) {
    return NULL;
  } else {
    /* The drive will be deleted by the user, so we remove references to it */
    castor::tape::drives::DriveInterface * ret = drive->second;
    m_pathToDrive.erase(drive);
    return ret;
  }
}

/**
 * Function merging all types of files into a single pointer
 * based map. This allows usage of polymorphic 
 */
void System::fakeWrapper::referenceFiles() {
  for (std::map<std::string, regularFile>::iterator i = m_regularFiles.begin();
          i != m_regularFiles.end(); i++)
    m_files[i->first] = &m_regularFiles[i->first];
  for (std::map<std::string, stDeviceFile>::iterator i = m_stFiles.begin();
          i != m_stFiles.end(); i++)
    m_files[i->first] = &m_stFiles[i->first]; 
}

System::mockWrapper::mockWrapper() {
  m_DIR = reinterpret_cast<DIR*> (& m_DIRfake);
  ON_CALL(*this, opendir(::testing::_))
      .WillByDefault(::testing::Return(m_DIR));
}

void System::mockWrapper::delegateToFake() {
  ON_CALL(*this, opendir(_)).WillByDefault(Invoke(&fake, &fakeWrapper::opendir));
  ON_CALL(*this, readdir(_)).WillByDefault(Invoke(&fake, &fakeWrapper::readdir));
  ON_CALL(*this, closedir(_)).WillByDefault(Invoke(&fake, &fakeWrapper::closedir));
  ON_CALL(*this, readlink(_, _, _)).WillByDefault(Invoke(&fake, &fakeWrapper::readlink));
  ON_CALL(*this, realpath(_, _)).WillByDefault(Invoke(&fake, &fakeWrapper::realpath));
  ON_CALL(*this, open(_, _)).WillByDefault(Invoke(&fake, &fakeWrapper::open));
  ON_CALL(*this, read(_, _, _)).WillByDefault(Invoke(&fake, &fakeWrapper::read));
  ON_CALL(*this, write(_, _, _)).WillByDefault(Invoke(&fake, &fakeWrapper::write));
  /* We have an overloaded function. Have to use a static_cast trick to indicate
   the pointer to which function we want.*/
  ON_CALL(*this, ioctl(_, _, A<struct mtop *>())).WillByDefault(Invoke(&fake, 
        static_cast<int(fakeWrapper::*)(int , unsigned long int , mtop*)>(&fakeWrapper::ioctl)));
  ON_CALL(*this, ioctl(_, _, A<struct mtget *>())).WillByDefault(Invoke(&fake, 
        static_cast<int(fakeWrapper::*)(int , unsigned long int , mtget*)>(&fakeWrapper::ioctl)));
  ON_CALL(*this, ioctl(_, _, A<struct sg_io_hdr *>())).WillByDefault(Invoke(&fake, 
        static_cast<int(fakeWrapper::*)(int , unsigned long int , sg_io_hdr_t*)>(&fakeWrapper::ioctl)));
  ON_CALL(*this, close(_)).WillByDefault(Invoke(&fake, &fakeWrapper::close));
  ON_CALL(*this, stat(_, _)).WillByDefault(Invoke(&fake, &fakeWrapper::stat));
  ON_CALL(*this, getDriveByPath(_)).WillByDefault(Invoke(&fake, &fakeWrapper::getDriveByPath));
}

void System::mockWrapper::disableGMockCallsCounting() {
  using testing::AnyNumber;
  EXPECT_CALL(*this, opendir(_)).Times(AnyNumber());
  EXPECT_CALL(*this, readdir(_)).Times(AnyNumber());
  EXPECT_CALL(*this, closedir(_)).Times(AnyNumber());
  EXPECT_CALL(*this, realpath(_, _)).Times(AnyNumber());
  EXPECT_CALL(*this, open(_, _)).Times(AnyNumber());
  EXPECT_CALL(*this, read(_, _, _)).Times(AnyNumber());
  EXPECT_CALL(*this, write(_, _, _)).Times(AnyNumber());
  EXPECT_CALL(*this, close(_)).Times(AnyNumber());
  EXPECT_CALL(*this, readlink(_, _, _)).Times(AnyNumber());
  EXPECT_CALL(*this, stat(_,_)).Times(AnyNumber());
  EXPECT_CALL(*this, ioctl(_, _, A<struct mtop *>())).Times(AnyNumber());
  EXPECT_CALL(*this, ioctl(_, _, A<struct mtget *>())).Times(AnyNumber());
  EXPECT_CALL(*this, ioctl(_, _, A<struct sg_io_hdr *>())).Times(AnyNumber());
  EXPECT_CALL(*this, getDriveByPath(_)).Times(AnyNumber());
}

void System::fakeWrapper::setupSLC5() {
  /*
   * Setup an tree similar to what we'll find in
   * and SLC5 system with mvhtl library (one media exchanger, 2 drives)
   */
  /*
   * First of, the description of all devices in sysfs.
   * In SLC5, sysfs is mounted on /sys/. If other mount point appear in the
   * future, we'll have to provide /proc/mounts (and use it).
   * SLC6 is similar, so this is not necessary at the time of writing.
   */
  m_directories["/sys/bus/scsi/devices"].push_back(".");
  m_directories["/sys/bus/scsi/devices"].push_back("..");
  m_directories["/sys/bus/scsi/devices"].push_back("3:0:0:0");
  m_directories["/sys/bus/scsi/devices"].push_back("3:0:1:0");
  m_directories["/sys/bus/scsi/devices"].push_back("3:0:2:0");
  m_realpathes["/sys/bus/scsi/devices/3:0:0:0"]
          = "/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0";
  m_realpathes["/sys/bus/scsi/devices/3:0:1:0"]
          = "/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0";
  m_realpathes["/sys/bus/scsi/devices/3:0:2:0"]
          = "/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0/type"] = "8\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/type"] = "1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/type"] = "1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0/vendor"] = "STK     \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/vendor"] = "STK     \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/vendor"] = "STK     \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0/model"] = "VL32STK1        \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/model"] = "T10000B         \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/model"] = "T10000B         \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0/rev"] = "0104\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/rev"] = "0104\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/rev"] = "0104\n";
  m_links["/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0/generic"]
          = "../../../../../../class/scsi_generic/sg2";
  m_links["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/generic"]
          = "../../../../../../class/scsi_generic/sg0";
  m_links["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/generic"]
          = "../../../../../../class/scsi_generic/sg1";
  m_stats["/dev/sg0"].st_rdev = makedev(21, 0);
  m_stats["/dev/sg0"].st_mode = S_IFCHR;
  m_stats["/dev/sg1"].st_rdev = makedev(21, 1);
  m_stats["/dev/sg1"].st_mode = S_IFCHR;
  m_stats["/dev/sg2"].st_rdev = makedev(21, 2);
  m_stats["/dev/sg2"].st_mode = S_IFCHR;
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:0/3:0:0:0/generic/dev"] = "21:2\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/generic/dev"] = "21:0\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/generic/dev"] = "21:1\n";
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back(".");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("..");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("bus");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("delete");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("device_blocked");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("driver");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("generic");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("iocounterbits");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("iodone_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("ioerr_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("iorequest_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("model");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("power");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("queue_depth");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("queue_type");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("rescan");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("rev");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_device:3:0:1:0");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_generic:sg0");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_level");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:nst0");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:nst0a");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:nst0l");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:nst0m");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:st0");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:st0a");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:st0l");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("scsi_tape:st0m");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("state");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("subsystem");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("tape");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("timeout");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("type");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("uevent");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0"].push_back("vendor");
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/scsi_tape:st0/dev"] =
          "9:0\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:1/3:0:1:0/scsi_tape:nst0/dev"] =
          "9:128\n";
  m_stats["/dev/st0"].st_rdev = makedev(9, 0);
  m_stats["/dev/st0"].st_mode = S_IFCHR;
  m_stats["/dev/nst0"].st_rdev = makedev(9, 128);
  m_stats["/dev/nst0"].st_mode = S_IFCHR;
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back(".");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("..");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("bus");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("delete");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("device_blocked");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("driver");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("generic");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("iocounterbits");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("iodone_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("ioerr_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("iorequest_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("model");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("power");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("queue_depth");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("queue_type");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("rescan");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("rev");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_device:3:0:2:0");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_generic:sg1s");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_level");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:nst1");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:nst1a");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:nst1l");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:nst1m");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:st1");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:st1a");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:st1l");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("scsi_tape:st1m");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("state");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("subsystem");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("tape");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("timeout");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("type");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("uevent");
  m_directories["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0"].push_back("vendor");
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/scsi_tape:st1/dev"] =
          "9:1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host3/target3:0:2/3:0:2:0/scsi_tape:nst1/dev"] =
          "9:129\n";
  m_stats["/dev/st1"].st_rdev = makedev(9, 1);
  m_stats["/dev/st1"].st_mode = S_IFCHR;
  m_stats["/dev/nst1"].st_rdev = makedev(9, 129);
  m_stats["/dev/nst1"].st_mode = S_IFCHR;
  m_stFiles["/dev/nst0"];
  m_stFiles["/dev/nst1"];
  referenceFiles();
}

void System::fakeWrapper::setupSLC6() {
  /*
   * Setup an tree similar to what we'll find in
   * and SLC6 system with mvhtl library (one media exchanger, 2 drives).
   * There is also one SCSI device present /dev/sg0 as HDD.
   */
  /*
   * First of, the description of all devices in sysfs.
   * In SLC6, sysfs is mounted on /sys/. If other mount point appear in the
   * future, we'll have to provide /proc/mounts (and use it).
   */
  m_directories["/sys/bus/scsi/devices"].push_back(".");
  m_directories["/sys/bus/scsi/devices"].push_back("..");
  m_directories["/sys/bus/scsi/devices"].push_back("0:0:0:0"); /* disk */
  m_directories["/sys/bus/scsi/devices"].push_back("6:0:0:0"); /* mediumx */
  m_directories["/sys/bus/scsi/devices"].push_back("6:0:1:0"); /* tape */
  m_directories["/sys/bus/scsi/devices"].push_back("6:0:2:0"); /* tape */
  
  m_directories["/sys/bus/scsi/devices"].push_back("host0");
  m_directories["/sys/bus/scsi/devices"].push_back("host1");
  m_directories["/sys/bus/scsi/devices"].push_back("host2");
  m_directories["/sys/bus/scsi/devices"].push_back("host3");
  m_directories["/sys/bus/scsi/devices"].push_back("host4");
  m_directories["/sys/bus/scsi/devices"].push_back("host5");
  m_directories["/sys/bus/scsi/devices"].push_back("host6");
  m_directories["/sys/bus/scsi/devices"].push_back("target0:0:0");
  m_directories["/sys/bus/scsi/devices"].push_back("target6:0:0");
  m_directories["/sys/bus/scsi/devices"].push_back("target6:0:1");
  m_directories["/sys/bus/scsi/devices"].push_back("target6:0:2");
  
  m_realpathes["/sys/bus/scsi/devices/0:0:0:0"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0";
  m_realpathes["/sys/bus/scsi/devices/6:0:0:0"]
          = "/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0";
  m_realpathes["/sys/bus/scsi/devices/6:0:1:0"]
          = "/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0";
  m_realpathes["/sys/bus/scsi/devices/6:0:2:0"]
          = "/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0";
  m_realpathes["/sys/bus/scsi/devices/host0"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host0";
  m_realpathes["/sys/bus/scsi/devices/host1"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host1";
  m_realpathes["/sys/bus/scsi/devices/host2"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host2";
  m_realpathes["/sys/bus/scsi/devices/host3"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host3";
  m_realpathes["/sys/bus/scsi/devices/host4"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host4";
  m_realpathes["/sys/bus/scsi/devices/host5"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host5";
  m_realpathes["/sys/bus/scsi/devices/host6"]
          = "/sys/devices/pseudo_0/adapter0/host6";
  m_realpathes["/sys/bus/scsi/devices/target0:0:0"]
          = "/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0";
  m_realpathes["/sys/bus/scsi/devices/target6:0:0"]
          = "/sys/devices/pseudo_0/adapter0/host6/target6:0:0";
  m_realpathes["/sys/bus/scsi/devices/target6:0:1"]
          = "/sys/devices/pseudo_0/adapter0/host6/target6:0:1";
  m_realpathes["/sys/bus/scsi/devices/target6:0:2"]
          = "/sys/devices/pseudo_0/adapter0/host6/target6:0:2";
  m_regularFiles["/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/type"] = "0\n";
  m_regularFiles["/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/vendor"] = "ATA     \n";
  m_regularFiles["/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/model"] = "TOSHIBA THNSNF12\n";
  m_regularFiles["/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/rev"] = "FSLA\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0/type"] = "8\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/type"] = "1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/type"] = "1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0/vendor"] = "STK     \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/vendor"] = "STK     \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/vendor"] = "STK     \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0/model"] = "VL32STK1        \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/model"] = "T10000B         \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/model"] = "T10000B         \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0/rev"] = "0104\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/rev"] = "0104\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/rev"] = "0104\n";
  
  
  m_links["/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/generic"]
          = "scsi_generic/sg0";
  m_links["/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0/generic"]
          = "scsi_generic/sg3";
  m_links["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/generic"]
          = "scsi_generic/sg1";
    m_links["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/generic"]
          = "scsi_generic/sg2";
    
  m_stats["/dev/sg0"].st_rdev = makedev(21, 0);
  m_stats["/dev/sg0"].st_mode = S_IFCHR;
  m_stats["/dev/sg1"].st_rdev = makedev(21, 1);
  m_stats["/dev/sg1"].st_mode = S_IFCHR;
  m_stats["/dev/sg2"].st_rdev = makedev(21, 2);
  m_stats["/dev/sg2"].st_mode = S_IFCHR;
  m_stats["/dev/sg3"].st_rdev = makedev(21, 3);
  m_stats["/dev/sg3"].st_mode = S_IFCHR;

  m_regularFiles["/sys/devices/pci0000:00/0000:00:1f.2/host0/target0:0:0/0:0:0:0/generic/dev"]  = "21:0\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:0/6:0:0:0/generic/dev"]  = "21:3\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/generic/dev"] = "21:1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/generic/dev"] = "21:2\n";

  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back(".");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("..");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("bsg");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("delete");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("device_blocked");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("dh_state");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("driver");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("evt_media_change");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("generic");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("iocounterbits");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("iodone_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("ioerr_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("iorequest_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("modalias");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("model");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("power");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("queue_depth");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("queue_ramp_up_period");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("queue_type");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("rescan");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("rev");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("scsi_device");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("scsi_generic");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("scsi_level");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("scsi_tape");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("state");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("subsystem");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("tape");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("timeout");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("type");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("uevent");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0"].push_back("vendor");

  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back(".");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("..");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("nst0");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("nst0a");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("nst0l");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("nst0m");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("st0");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("st0a");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("st0l");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape"].push_back("st0m");

  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape/st0/dev"] =
          "9:0\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/scsi_tape/nst0/dev"] =
          "9:128\n";
  m_stats["/dev/st0"].st_rdev = makedev(9, 0);
  m_stats["/dev/st0"].st_mode = S_IFCHR;
  m_stats["/dev/nst0"].st_rdev = makedev(9, 128);
  m_stats["/dev/nst0"].st_mode = S_IFCHR;
  
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back(".");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("..");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("bsg");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("delete");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("device_blocked");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("dh_state");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("driver");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("evt_media_change");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("generic");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("iocounterbits");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("iodone_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("ioerr_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("iorequest_cnt");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("modalias");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("model");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("power");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("queue_depth");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("queue_ramp_up_period");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("queue_type");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("rescan");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("rev");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("scsi_device");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("scsi_generic");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("scsi_level");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("scsi_tape");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("state");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("subsystem");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("tape");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("timeout");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("type");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("uevent");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0"].push_back("vendor");

  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back(".");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("..");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("nst1");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("nst1a");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("nst1l");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("nst1m");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("st1");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("st1a");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("st1l");
  m_directories["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape"].push_back("st1m");

  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape/st1/dev"] =
          "9:1\n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/scsi_tape/nst1/dev"] =
          "9:129\n";
  m_stats["/dev/st1"].st_rdev = makedev(9, 1);
  m_stats["/dev/st1"].st_mode = S_IFCHR;
  m_stats["/dev/nst1"].st_rdev = makedev(9, 129);
  m_stats["/dev/nst1"].st_mode = S_IFCHR;

  m_stFiles["/dev/nst0"];
  m_stFiles["/dev/nst1"];
  referenceFiles();
}

void castor::tape::System::fakeWrapper::setupForVirtualDriveSLC6() {
  setupSLC6();
  // Change the type of the drive to VIRTUAL so we get the proper drive for the test.
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:1/6:0:1:0/model"] = "VIRTUAL         \n";
  m_regularFiles["/sys/devices/pseudo_0/adapter0/host6/target6:0:2/6:0:2:0/model"] = "VIRTUAL         \n";
  // This simulates the result of stat with a symlink to /dev/nst0
  m_stats["/dev/tape_T10D6116"].st_rdev = makedev(9,128);
  m_stats["/dev/noSuchTape"].st_rdev = makedev(9,5);
}


