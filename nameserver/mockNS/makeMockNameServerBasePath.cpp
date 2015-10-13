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

/**
 * This program will create a new directory for the mock name server
 */

#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <attr/xattr.h>
#include <unistd.h>

#include "common/exception/Errnum.hpp"
#include "common/exception/Exception.hpp"
#include "common/Utils.hpp"

//------------------------------------------------------------------------------
// setXattr
//------------------------------------------------------------------------------
void setXattr(const std::string &path, const std::string &name, const std::string &value) {
  if(setxattr(path.c_str(), name.c_str(), value.c_str(), value.length(), 0)) {
    const int savedErrno = errno;
    std::ostringstream msg;
    msg << "Call to setxattr() failed: path=" << path << " name=" << name << " value=" << value << ": " << cta::Utils::errnoToString(savedErrno);
    throw cta::exception::Exception(msg.str());
  }
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(void) {
  try {
    umask(0);  
    char path[100];
    strncpy(path, "/tmp/CTAMockNSXXXXXX", 100);
    cta::exception::Errnum::throwOnNull(mkdtemp(path), "MockNameServer() - Failed to create temporary directory");
    std::string pathString(path);
    pathString+="/";
    std::stringstream uidss;
    uidss << getuid();
    std::stringstream gidss;
    gidss << getgid();
    setXattr(pathString.c_str(), "user.CTAStorageClass", "");
    setXattr(pathString.c_str(), "user.CTAuid", uidss.str());
    setXattr(pathString.c_str(), "user.CTAgid", gidss.str());
    std::cout << "New mock name server path: " << pathString << std::endl;
  } catch (cta::exception::Exception & e) {
    std::cerr << "Failed to create a new directory for the mock name server. CTA Exception: " << e.getMessageValue() << std::endl;
  } catch (std::exception & e) {
    std::cerr << "Failed to create a new directory for the mock name server. STD Exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Failed to create a new directory for the mock name server. Unknown Exception!" << std::endl;
  }
}

