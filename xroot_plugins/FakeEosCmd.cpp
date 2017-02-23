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

#include "common/make_unique.hpp"
#include "xroot_plugins/FakeEosCmd.hpp"
#include "xroot_plugins/FakeEosCmdLineArgs.hpp"

#include <iostream>
#include <stdint.h>
#include <string>
#include <XrdCl/XrdClFileSystem.hh>

namespace cta {
namespace xroot_plugins {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
FakeEosCmd::FakeEosCmd(std::istream &inStream, std::ostream &outStream, std::ostream &errStream):
CmdLineTool(inStream, outStream, errStream) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
FakeEosCmd::~FakeEosCmd() noexcept {
}

//------------------------------------------------------------------------------
// exceptionThrowingMain
//------------------------------------------------------------------------------
int FakeEosCmd::exceptionThrowingMain(const int argc, char *const *const argv) {
  const FakeEosCmdLineArgs cmdLineArgs(argc, argv);

  if(cmdLineArgs.help) {
    printUsage(m_out);
    return 0;
  }

  m_out << "Hello World" << std::endl;

  const std::string protocol = "xroot";
  const std::string fsUrl = protocol + ":" + "//" + cmdLineArgs.ctaHost + ":" + std::to_string(cmdLineArgs.ctaPort);
  XrdCl::FileSystem fs(fsUrl, false);

  XrdCl::Buffer arg(cmdLineArgs.queryArg.size());
  arg.FromString(cmdLineArgs.queryArg);
  XrdCl::Buffer *response = nullptr;
  const XrdCl::XRootDStatus status = fs.Query(XrdCl::QueryCode::OpaqueFile, arg, response);
  std::unique_ptr<XrdCl::Buffer> smartResponse(response);

  std::cout << "status.ToStr()=" << status.ToStr() << std::endl;
  std::cout << "status.IsError()=" << (status.IsError() ? "true" : "false") << std::endl;
  std::cout << "status.IsFatal()=" << (status.IsFatal() ? "true" : "false") << std::endl;
  std::cout << "status.IsOK()=" << (status.IsOK() ? "true" : "false") << std::endl;
  return 0;
}

//------------------------------------------------------------------------------
// printUsage
//------------------------------------------------------------------------------
void FakeEosCmd::printUsage(std::ostream &os) {
  FakeEosCmdLineArgs::printUsage(os);
}

} // namespace xroot_plugins
} // namespace cta
