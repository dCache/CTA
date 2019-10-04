#
# - Find tools needed for building RPM Packages
#   on Linux systems and defines macro that helps to
#   build source or binary RPM, the MACRO assumes
#   CMake 2.4.x which includes CPack support.
#   CPack is used to build tar.gz source tarball
#   which may be used by a custom user-made spec file.
#
# - Define RPMTools_ADD_RPM_TARGETS which defines
#   two (top-level) CUSTOM targets for building
#   source and binary RPMs
#
# Those CMake macros are provided by the TSP Developer Team
# https://savannah.nongnu.org/projects/tsp
#

IF (WIN32)  
  MESSAGE(STATUS "RPM tools not available on Win32 systems")
ENDIF(WIN32)

IF (UNIX)
  # Look for RPM builder executable
  FIND_PROGRAM(RPMTools_RPMBUILD_EXECUTABLE 
    NAMES rpmbuild
    PATHS "/usr/bin;/usr/lib/rpm"
    PATH_SUFFIXES bin
    DOC "The RPM builder tool")
  
  IF (RPMTools_RPMBUILD_EXECUTABLE)
    MESSAGE(STATUS "Looking for RPMTools... - found rpmbuild in ${RPMTools_RPMBUILD_EXECUTABLE}")
    SET(RPMTools_RPMBUILD_FOUND "YES")
    GET_FILENAME_COMPONENT(RPMTools_BINARY_DIRS ${RPMTools_RPMBUILD_EXECUTABLE} PATH)
  ELSE (RPMTools_RPMBUILD_EXECUTABLE) 
    SET(RPMTools_RPMBUILD_FOUND "NO")
    MESSAGE(STATUS "Looking for RPMTools... - rpmbuild NOT FOUND")
  ENDIF (RPMTools_RPMBUILD_EXECUTABLE)

  # Detect if CPack was included or not
  IF (NOT DEFINED "CPACK_PACKAGE_NAME") 
    MESSAGE(FATAL_ERROR "CPack was not included, you should include CPack before Using RPMTools")
  ENDIF (NOT DEFINED "CPACK_PACKAGE_NAME")
  
  IF (RPMTools_RPMBUILD_FOUND)
    SET(RPMTools_FOUND TRUE)    
    #
    # - first arg  (ARGV0) is RPM name
    # - second arg (ARGV1) is the RPM spec file path [optional]
    # - third arg  (ARGV2) is the RPM ROOT DIRECTORY used to build RPMs [optional]
    #
    MACRO(RPMTools_ADD_RPM_TARGETS RPMNAME)

      #
      # If no spec file is provided create a minimal one
      #
      IF ("${ARGV1}" STREQUAL "")
	SET(SPECFILE_PATH "${CMAKE_BINARY_DIR}/${RPMNAME}.spec")
      ELSE ("${ARGV1}" STREQUAL "")
	SET(SPECFILE_PATH "${ARGV1}")
      ENDIF("${ARGV1}" STREQUAL "")
      
      # Verify whether if RPM_ROOTDIR was provided or not
      IF("${ARGV2}" STREQUAL "") 
	SET(RPM_ROOTDIR ${CMAKE_BINARY_DIR}/RPM)
      ELSE ("${ARGV2}" STREQUAL "")
	SET(RPM_ROOTDIR "${ARGV2}")	
      ENDIF("${ARGV2}" STREQUAL "")
      MESSAGE(STATUS "RPMTools:: Using RPM_ROOTDIR=${RPM_ROOTDIR}")

      # Prepare RPM build tree
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR})
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/tmp)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/BUILD)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/RPMS)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/SOURCES)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/SPECS)
      FILE(MAKE_DIRECTORY ${RPM_ROOTDIR}/SRPMS)

      # Append the distribution name to the CPACK package tarball name
      #set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_SOURCE_PACKAGE_FILE_NAME}${RPMTools_RPMBUILD_DIST}")

      #
      # We check whether if the provided spec file is
      # to be configure or not.
      # 
      IF ("${ARGV1}" STREQUAL "")
	SET(SPECFILE_PATH "${RPM_ROOTDIR}/SPECS/${RPMNAME}.spec")
	SET(SPECFILE_NAME "${RPMNAME}.spec")
	MESSAGE(STATUS "No Spec file given generate a minimal one --> ${RPM_ROOTDIR}/SPECS/${RPMNAME}.spec")
      	FILE(WRITE ${RPM_ROOTDIR}/SPECS/${RPMNAME}.spec
	  "# -*- rpm-spec -*-
Summary:        ${RPMNAME}
Name:           ${RPMNAME}
Version:        ${PACKAGE_VERSION}
Release:        1
License:        Unknown
Group:          Unknown
Source:         ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:	cmake

%define prefix /opt/${RPMNAME}-%{version}
%define rpmprefix $RPM_BUILD_ROOT%{prefix}
%define srcdirname %{name}-%{version}-Source

%description
${RPMNAME} : No description for now

%prep
%setup -q -n %{srcdirname}

%build
cd ..
rm -rf build_tree
mkdir build_tree
cd build_tree
cmake -DCMAKE_INSTALL_PREFIX=%{rpmprefix} ../%{srcdirname}
make
  
%install 
cd ../build_tree
make install

%clean
rm -rf %{srcdirname}
rm -rf build_tree

%files
%defattr(-,root,root,-)
%dir %{prefix}
%{prefix}/*

%changelog
* Wed Feb 28 2007 Erk <eric.noulard@gmail.com>
  Generated by CMake UseRPMTools macros"
	)

      ELSE ("${ARGV1}" STREQUAL "")
	SET(SPECFILE_PATH "${ARGV1}")
	
	GET_FILENAME_COMPONENT(SPECFILE_EXT ${SPECFILE_PATH} EXT)      
	IF ("${SPECFILE_EXT}" STREQUAL ".spec")
	  # This is a 'ready-to-use' spec file which does not need to be CONFIGURED
	  GET_FILENAME_COMPONENT(SPECFILE_NAME ${SPECFILE_PATH} NAME)
	  MESSAGE(STATUS "Simple copy spec file <${SPECFILE_PATH}> --> <${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME}>")
	  CONFIGURE_FILE(
	    ${SPECFILE_PATH} 
	    ${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME}
	    COPYONLY)
	ELSE ("${SPECFILE_EXT}" STREQUAL ".spec")
	  # This is a to-be-configured spec file
	  GET_FILENAME_COMPONENT(SPECFILE_NAME ${SPECFILE_PATH} NAME_WE)
	  SET(SPECFILE_NAME "${SPECFILE_NAME}.spec")
	  MESSAGE(STATUS "Configuring spec file <${SPECFILE_PATH}> --> <${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME}>")
	  CONFIGURE_FILE(
	    ${SPECFILE_PATH} 
	    ${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME}
	    @ONLY)
	ENDIF ("${SPECFILE_EXT}" STREQUAL ".spec")
      ENDIF("${ARGV1}" STREQUAL "")
            
      ADD_CUSTOM_TARGET(${RPMNAME}_srpm
	COMMAND cpack -G TGZ --config CPackSourceConfig.cmake
	COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz ${RPM_ROOTDIR}/SOURCES
	COMMAND ${RPMTools_RPMBUILD_EXECUTABLE} -bs --define=\"_topdir ${RPM_ROOTDIR}\"  --define '_source_filedigest_algorithm md5' --define '_binary_filedigest_algorithm md5' --define 'neutralpackage 1' --nodeps --buildroot=${RPM_ROOTDIR}/tmp ${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME} 
	)
      
      ADD_CUSTOM_TARGET(${RPMNAME}_rpm
	COMMAND cpack -G TGZ --config CPackSourceConfig.cmake
	COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz ${RPM_ROOTDIR}/SOURCES    
	COMMAND ${RPMTools_RPMBUILD_EXECUTABLE} -bb --define=\"_topdir ${RPM_ROOTDIR}\" $ENV{RPMDEFS} --buildroot=${RPM_ROOTDIR}/tmp ${RPM_ROOTDIR}/SPECS/${SPECFILE_NAME} 
	)  
    ENDMACRO(RPMTools_ADD_RPM_TARGETS)

  ELSE (RPMTools_RPMBUILD_FOUND)
    SET(RPMTools FALSE)
  ENDIF (RPMTools_RPMBUILD_FOUND)  
  
ENDIF (UNIX)
  
