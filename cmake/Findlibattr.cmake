# @project        The CERN Tape Archive (CTA)
# @copyright      Copyright(C) 2015-2021 CERN
# @license        This program is free software: you can redistribute it and/or modify
#                 it under the terms of the GNU General Public License as published by
#                 the Free Software Foundation, either version 3 of the License, or
#                 (at your option) any later version.
#
#                 This program is distributed in the hope that it will be useful,
#                 but WITHOUT ANY WARRANTY; without even the implied warranty of
#                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#                 GNU General Public License for more details.
#
#                 You should have received a copy of the GNU General Public License
#                 along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This module will set the following variables:
#     LIBATTR_FOUND
#     LIBATTR_INCLUDE_DIRS
#     LIBATTR_LIBRARIES

find_path(LIBATTR_INCLUDE_DIRS
  xattr.h
  PATHS /usr/include/attr
  NO_DEFAULT_PATH)

find_library(LIBATTR_LIBRARIES
  NAME attr
  PATHS /usr/lib64
  NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libattr DEFAULT_MSG
  LIBATTR_INCLUDE_DIRS LIBATTR_LIBRARIES)
