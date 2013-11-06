#
# Put a nice header here
#
# this is the configuration file for the CASTOR test suite,
# This is a template for an option file that can be used to
# configure the test suite. Please read it carefully and
# fill in the proper values.
#
# All lines are commented by default, with the dafault value
# indicated. To change the default, just edit and uncomment
#
# Note that the syntax of this file is the one of the python
# ConfigParser class. It implements a basic configuration
# file parser language which provides a structure similar
# to what you would find on Microsoft Windows INI files.
#


# generic info about the context in which the test suite will run
[Generic]

# castor directories to use for temporary files created by the test suite
CastorNoTapeDir = /castor/cern.ch/dev/c/murrayc3/notape
CastorTapeDir = /castor/cern.ch/dev/c/murrayc3/tape

# Put this to true if you want the test suite to skip tests when resources are missing
# Default behavior is to fail
# SkipTestsWhenResourceMissing = false

# Put this to true if you do not want to cleanup temporary files created
# by the test suite in the CASTOR namespace
# NoCleanup = false

# the service classes to be used by the test suite
TapeServiceClasses = default, dev
DiskOnlyServiceClasses = diskonly

# Remote machine and directory to be used for testing transfers outside CASTOR
remoteSpace = lxc2disk07.cern.ch:/var/tmp/rfiod


# Environment to be used for the tests
# Each line will be translated into an environment variable setting
[Environment]

# Stager related
STAGE_HOST = lxcastordev04
STAGE_PORT = 9002
STAGER_TRACE = 3
STAGE_SVCCLASS =

# ROOT related
ROOTSYS = /afs/cern.ch/sw/lcg/app/releases/ROOT/5.25.04/x86_64-slc5-gcc34-opt/root

# XROOT Base directory
XROOTSYS = /opt/xrootd

# GLOBUS Base directory
GLOBUSSYS = /opt/globus

# Tape pool name which will be use to test tape functionalities
# N.B. Tapepoool must exist and must have one and only one tape in it, 
# which must be writable. 
TAPEPOOLNAME = stager_dev04


# values to give to parameters in the .input files of the test suite
# any <TAG> string (where tag can contain anything but '>') will be
# replaced by the value of the corresponding entry
[Tags]

# Local file name to be used as sample data
localFileName = /etc/group

# Client uid/gid to be used for testing as unprivileged user; the chosen
# user shall be able via ACLs to write on the namespace.
# Provided default is dteam001:cg
unprivUid = 18118
unprivGid = 2688

# Control of which test to run when no flags is given for a given set
# If any flag is given on the command line, this part is ignored for the corresponding set
# Entries should be in the form
#  <testName> = <boolean value> 
# Test names are of the form SetName[_SubDir]*_LocalTestName.
# They can be truncated to match all tests starting with the given string
# recognized boolean values are 1, yes, true, on, 0, no, false, off
# and all alternates cases for these values
# If several options apply to a given file, the most precise one wins
# Here is an example of how to run all rfio tests, but for the remote ones :
# rfio_remote = false
# rfio = true
[TestsToRun]
