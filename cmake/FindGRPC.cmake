# @project      The CERN Tape Archive (CTA)
# @copyright    Copyright © 2015-2022 CERN
# @license      This program is free software, distributed under the terms of the GNU General Public
#               Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING". You can
#               redistribute it and/or modify it under the terms of the GPL Version 3, or (at your
#               option) any later version.
#
#               This program is distributed in the hope that it will be useful, but WITHOUT ANY
#               WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
#               PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
#               In applying this licence, CERN does not waive the privileges and immunities
#               granted to it by virtue of its status as an Intergovernmental Organization or
#               submit itself to any jurisdiction.
#
# Locate and configure the gRPC library
#
# Adds the following targets:
#
#  gRPC::grpc - gRPC library
#  gRPC::grpc++ - gRPC C++ library
#  gRPC::grpc++_reflection - gRPC C++ reflection library
#  gRPC::grpc_cpp_plugin - C++ generator plugin for Protocol Buffers
#

#
# Generates C++ sources from the .proto files
#
# grpc_generate_cpp (<SRCS> <HDRS> <DEST> [<ARGN>...])
#
#  SRCS - variable to define with autogenerated source files
#  HDRS - variable to define with autogenerated header files
#  DEST - directory where the source files will be created
#  ARGN - .proto files
#
function(GRPC_GENERATE_CPP SRCS HDRS DEST)
  if(NOT ARGN)
    message(SEND_ERROR "Error: GRPC_GENERATE_CPP() called without any proto files")
    return()
  endif()

  if(GRPC_GENERATE_CPP_APPEND_PATH)
    # Create an include path for each file specified
    foreach(FIL ${ARGN})
      get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
      get_filename_component(ABS_PATH ${ABS_FIL} PATH)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  else()
    set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(DEFINED PROTOBUF3_IMPORT_DIRS)
    foreach(DIR ${PROTOBUF3_IMPORT_DIRS})
      get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
      list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
      if(${_contains_already} EQUAL -1)
          list(APPEND _protobuf_include_path -I ${ABS_PATH})
      endif()
    endforeach()
  endif()

  set(${SRCS})
  set(${HDRS})
  foreach(FIL ${ARGN})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    list(APPEND ${SRCS} "${DEST}/${FIL_WE}.grpc.pb.cc")
    list(APPEND ${HDRS} "${DEST}/${FIL_WE}.grpc.pb.h")

    add_custom_command(
      OUTPUT "${DEST}/${FIL_WE}.grpc.pb.cc"
             "${DEST}/${FIL_WE}.grpc.pb.h"
      COMMAND ${PROTOBUF3_PROTOC3_EXECUTABLE}
      ARGS --grpc_out ${DEST} ${_protobuf_include_path} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${ABS_FIL}
      DEPENDS ${ABS_FIL} ${PROTOBUF3_PROTOC3_EXECUTABLE} gRPC::grpc_cpp_plugin
      COMMENT "Running C++ gRPC compiler on ${FIL}"
      VERBATIM )
  endforeach()

  set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
  set(${SRCS} ${${SRCS}} PARENT_SCOPE)
  set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()

# By default have GRPC_GENERATE_CPP macro pass -I to protoc
# for each directory where a proto file is referenced.
if(NOT DEFINED GRPC_GENERATE_CPP_APPEND_PATH)
  set(GRPC_GENERATE_CPP_APPEND_PATH TRUE)
endif()

# Find gRPC include directory
find_path(GRPC_INCLUDE_DIR grpc/grpc.h)
mark_as_advanced(GRPC_INCLUDE_DIR)

# Find gRPC library
find_library(GRPC_LIBRARY NAMES grpc)
mark_as_advanced(GRPC_LIBRARY)
add_library(gRPC::grpc UNKNOWN IMPORTED)
set_target_properties(gRPC::grpc PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES "-lpthread;-ldl"
    IMPORTED_LOCATION ${GRPC_LIBRARY}
)

# Find gRPC C++ library
find_library(GRPC_GRPC++_LIBRARY NAMES grpc++)
mark_as_advanced(GRPC_GRPC++_LIBRARY)
add_library(gRPC::grpc++ UNKNOWN IMPORTED)
set_target_properties(gRPC::grpc++ PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES gRPC::grpc
    IMPORTED_LOCATION ${GRPC_GRPC++_LIBRARY}
)

# Find gRPC C++ reflection library
find_library(GRPC_GRPC++_REFLECTION_LIBRARY NAMES grpc++_reflection)
mark_as_advanced(GRPC_GRPC++_REFLECTION_LIBRARY)
add_library(gRPC::grpc++_reflection UNKNOWN IMPORTED)
set_target_properties(gRPC::grpc++_reflection PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GRPC_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES gRPC::grpc++
    IMPORTED_LOCATION ${GRPC_GRPC++_REFLECTION_LIBRARY}
)

# Find gRPC CPP generator
find_program(GRPC_CPP_PLUGIN NAMES grpc_cpp_plugin)
mark_as_advanced(GRPC_CPP_PLUGIN)
add_executable(gRPC::grpc_cpp_plugin IMPORTED)
set_target_properties(gRPC::grpc_cpp_plugin PROPERTIES
    IMPORTED_LOCATION ${GRPC_CPP_PLUGIN}
)

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GRPC DEFAULT_MSG
    GRPC_LIBRARY GRPC_INCLUDE_DIR GRPC_GRPC++_REFLECTION_LIBRARY GRPC_CPP_PLUGIN)
