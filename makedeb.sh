#!/bin/sh

# $Id: makedeb.sh,v 1.5 2005/01/23 15:23:36 jdurand Exp $

if [ "x${MAJOR_CASTOR_VERSION}" = "x" ]; then
  echo "No MAJOR_CASTOR_VERSION environment variable"
  exit 1
fi
if [ "x${MINOR_CASTOR_VERSION}" = "x" ]; then
  echo "No MINOR_CASTOR_VERSION environment variable"
  exit 1
fi

#
## We expect ${MAJOR_CASTOR_VERSION} in the form a.b
#
echo ${MAJOR_CASTOR_VERSION} | egrep -q '^[0-9]+\.[0-9]+$'
if [ $? -ne 0 ]; then
  echo "MAJOR_CASTOR_VERSION (${MAJOR_CASTOR_VERSION}) should be in the form a.b, example: 2.0"
  exit 1
fi

#
## We expect ${MINOR_CASTOR_VERSION} in the form c.d
#
echo ${MINOR_CASTOR_VERSION} | egrep -q '^[0-9]+\.[0-9]+$'
if [ $? -ne 0 ]; then
  echo "MINOR_CASTOR_VERSION (${MINOR_CASTOR_VERSION}) should be in the form c.d, example: 99.1"
  exit 1
fi

a=`echo ${MAJOR_CASTOR_VERSION} | sed 's/\..*//g'`
b=`echo ${MAJOR_CASTOR_VERSION} | sed 's/.*\.//g'`
c=`echo ${MINOR_CASTOR_VERSION} | sed 's/\..*//g'`
d=`echo ${MINOR_CASTOR_VERSION} | sed 's/.*\.//g'`

#
## Make sure makedepend (/usr/X11R6/bin) is in the path
#
PATH=${PATH}:/usr/X11R6/bin
export PATH

#
## Force build rules to YES for a lot of things
#
for this in BuildCommands BuildCommon BuildCupvClient BuildCupvDaemon BuildCupvLibrary BuildGcDaemon BuildHsmTools BuildMonitorClient BuildMonitorServer BuildMonitorLibrary BuildMsgClient BuildMsgServer BuildMsgLibrary BuildNameServerClient BuildNameServerDaemon BuildNameServerLibrary BuildRfioClient BuildRfioServer BuildRfioLibrary BuildRmcServer BuildRmcLibrary BuildJob BuildRtcopyClient BuildRtcopyServer BuildRtcopyLibrary BuildRtstat BuildStageClient BuildStageDaemon BuildStageLibrary BuildRmMaster BuildRmNode BuildRmClient BuildRmLibrary BuildTapeClient BuildTapeDaemon BuildTapeLibrary BuildTpusage BuildVdqmClient BuildVdqmServer BuildVdqmLibrary BuildVolumeMgrClient BuildVolumeMgrDaemon BuildVolumeMgrLibrary BuildDlfClient BuildDlfDaemon BuildDlfLibrary BuildExpertClient BuildExpertDaemon BuildExpertLibrary BuildRHCpp BuildOraCpp BuildCastorClientCPPLibrary Accounting HasNroff HasCDK UseCupv UseOracle UseVmgr UseScheduler UseExpert UseMaui; do
    perl -pi -e "s/$this.*(YES|NO)/$this\tYES/g" config/site.def
done
#
## Force build rules to NO for a lot of things
#
for this in BuildExamples BuildSchedPlugin BuildCppTest ClientLogging HasNis RFIODaemonRealTime UseAdns UseDMSCapi UseHpss UseMtx UseMySQL UseLsf FollowRtLinks BuildSecurity; do
    perl -pi -e "s/$this.*(YES|NO)/$this\tNO/g" config/site.def
done
#
## Change __MAJOR_CASTOR_VERSION and __MINOR_CASTOR_VERSION everywhere it has to be changed
#
perl -pi -e s/__MAJOR_CASTOR_VERSION__/${MAJOR_CASTOR_VERSION}/g */Imakefile debian/*.install
perl -pi -e s/__MINOR_CASTOR_VERSION__/${MINOR_CASTOR_VERSION}/g */Imakefile debian/*.install
fakeroot dch --newversion ${a}.${b}.${c}-${d}
fakeroot dpkg-buildpackage
status=$?

exit $status
