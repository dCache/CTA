###############################################################################
#
# CMake module to search for Oracle client library (OCI)
#
# On success, the macro sets the following variables:
# ORACLE_FOUND = if the library found
# ORACLE_LIBRARY = full path to the library
# ORACLE_LIBRARIES = full path to the library
# ORACLE_INCLUDE_DIR = where to find the library headers also defined,
# but not for general use are
# ORACLE_VERSION = version of library which was found, e.g. "1.2.5"
#
# Copyright (c) 2009-2013 Mateusz Loskot <mateusz@loskot.net>
#
# Developed with inspiration from Petr Vanek <petr@scribus.info>
# who wrote similar macro for TOra - http://torasql.com/
#
# Module source: http://github.com/mloskot/workshop/tree/master/cmake/
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################

set(ORACLE_OCI_NAMES clntsh libclntsh oci)
set(ORACLE_NNZ_NAMES nnz10 libnnz10 nnz11 libnnz11 ociw32)
set(ORACLE_OCCI_NAMES libocci occi oraocci10 oraocci11)

# If ORACLE_HOME not defined, use it.
if(DEFINED ENV{ORACLE_HOME})
  set(ORACLE_HOME $ENV{ORACLE_HOME})
  message("ORACLE_HOME=${ORACLE_HOME}")
  find_path(ORACLE_INCLUDE_DIR
    NAMES oci.h
    PATHS
    ${ORACLE_HOME}/rdbms/public
    ${ORACLE_HOME}/include
    ${ORACLE_HOME}/sdk/include # Oracle SDK
    ${ORACLE_HOME}/OCI/include # Oracle XE on Windows
  )
  set(ORACLE_LIB_DIR
    ${ORACLE_HOME}/lib
    ${ORACLE_HOME}/sdk/lib # Oracle SDK
    ${ORACLE_HOME}/sdk/lib/msvc
    ${ORACLE_HOME}/OCI/lib/msvc # Oracle XE on Windows
  )
  find_library(ORACLE_OCI_LIBRARY NAMES ${ORACLE_OCI_NAMES} PATHS ${ORACLE_LIB_DIR})
  find_library(ORACLE_OCCI_LIBRARY NAMES ${ORACLE_OCCI_NAMES} PATHS ${ORACLE_LIB_DIR})
  find_library(ORACLE_NNZ_LIBRARY NAMES ${ORACLE_NNZ_NAMES} PATHS ${ORACLE_LIB_DIR})

  if(APPLE)
    set(ORACLE_OCIEI_NAMES libociei ociei)

    find_library(ORACLE_OCIEI_LIBRARY
      NAMES libociei ociei
      PATHS ${ORACLE_LIB_DIR})

    if(ORACLE_OCIEI_LIBRARY)
      set(ORACLE_LIBRARY ${ORACLE_LIBRARY} ${ORACLE_OCIEI_LIBRARY})
    else(ORACLE_OCIEI_LIBRARY)
      message(STATUS
        "libociei.dylib is not found. It may cause crash if you are building BUNDLE")
    endif()
  endif()

else(DEFINED ENV{ORACLE_HOME})
  # Attempt known locations in Linux
  file(GLOB ORACLE_INCLUDE_DIR "/usr/include/oracle/*/client/")
  message("ORACLE_INCLUDE_DIR=${ORACLE_INCLUDE_DIR}")
  find_path(ORACLE_INCLUDE_DIR
    NAMES oci.h
    PATHS
    ${ORACLE_INCLUDE_DIR}) # Oracle instant client 11 on Linux
  message("ORACLE_INCLUDE_DIR=${ORACLE_INCLUDE_DIR}")
  set(ORACLE_LIB_DIR
    /usr/lib64/oracle/*/client/lib64
    /usr/lib/oracle/*/client/lib)
  find_library(ORACLE_OCI_LIBRARY NAMES ${ORACLE_OCI_NAMES} PATHS ${ORACLE_LIB_DIR})
  find_library(ORACLE_OCCI_LIBRARY NAMES ${ORACLE_OCCI_NAMES} PATHS ${ORACLE_LIB_DIR})
  find_library(ORACLE_NNZ_LIBRARY NAMES ${ORACLE_NNZ_NAMES} PATHS ${ORACLE_LIB_DIR})
endif(DEFINED ENV{ORACLE_HOME})

set(ORACLE_LIBRARY ${ORACLE_OCI_LIBRARY} ${ORACLE_OCCI_LIBRARY} ${ORACLE_NNZ_LIBRARY})
set(ORACLE_LIBRARIES ${ORACLE_LIBRARY})

# Handle the QUIETLY and REQUIRED arguments and set ORACLE_FOUND to TRUE
# if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ORACLE DEFAULT_MSG ORACLE_LIBRARY ORACLE_INCLUDE_DIR)

mark_as_advanced(ORACLE_INCLUDE_DIR ORACLE_LIBRARY)