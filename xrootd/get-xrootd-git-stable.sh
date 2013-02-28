#!/bin/bash                                          

#-------------------------------------------------------------------------------
# Get the current version of XROOTD installed and checkout the header files 
# corresponding to that version used for compiling the castor-xrootd-plugin
#-------------------------------------------------------------------------------

XROOTD_VERSION=`xrdcp --version`

if [ -d xrootd/xrootd_hdr ]; then
    echo "Clone already present, just have to checkout the correct version."
else
    git clone http://xrootd.cern.ch/repos/xrootd.git xrootd/xrootd_hdr
fi

cd xrootd/xrootd_hdr
git pull
echo "Try to checkout the correct version of header files i.e. v$XROOTD_VERSION"
git checkout -b local-$XROOTD_VERSION $XROOTD_VERSION

