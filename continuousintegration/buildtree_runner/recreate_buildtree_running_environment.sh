#!/bin/bash -x

# This script recreates an environment similar to the one created by 
# it-puppet-hostgroup-cta profiles.

# (Re-)create the mhvtl environment
echo Wiping mhvtl and kubernetes library info...
systemctl stop mhvtl
sleep 2
rm -rf /etc/mhvtl
rm -rf /opt/mhvtl
rm -rf /opt/kubernetes/CTA/library
mkdir -p /opt/mhvtl

################################################################################
### Add castor repo for MHVTL latest
echo Adding castor repo...
yum-config-manager --add-repo=./castor.repo

################################################################################
### Make sure necessary packages are installed.
echo Installing mhvtl and tape utils...
yum install -y kmod-mhvtl mhvtl-utils lsscsi mt-st mtx sg3_utils

################################################################################
### puppet:///modules/hg_cta/00-cta-tape.rules
echo Installing udev rules...
cp -v 00-cta-tape.rules /etc/udev/rules.d/00-cta-tape.rules
chmod 0644 /etc/udev/rules.d/00-cta-tape.rules

################################################################################
### mhvtl config directory
echo Creating mhvtl config directory...
mkdir -p /etc/mhvtl

################################################################################
### mhvtl conf: template('hg_cta/mhvtl/mhvtl.conf.erb')
echo Installing mhvtl.conf....
cp -v mhvtl.conf /etc/mhvtl/mhvtl.conf
chmod 0644 /etc/mhvtl/mhvtl.conf

################################################################################
### template('hg_cta/mhvtl/generate_mhvtl_config.sh.erb')
echo mhvtl config pre-run
ls -lR /opt/mhvtl

echo Running mhvtl config generator...
bash -x ./generate_mhvtl_config.sh

echo mhvtl config post run
ls -lR /opt/mhvtl


################################################################################
### Create mhvtl directory
mkdir -p /opt/mhvtl
chown vtl.vtl /opt/mhvtl

################################################################################
#### Start mhvtl
echo Starting mhvtl...
systemctl start mhvtl
sleep 2

################################################################################
### puppet:///modules/hg_cta/generate_librarydevice_PV.sh
echo Generating kubernetes persistent volumes
bash -x ./generate_librarydevice_PV.sh


