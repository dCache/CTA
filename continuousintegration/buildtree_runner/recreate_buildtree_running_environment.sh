#!/bin/bash -e

# This script recreates an environment similar to the one created by 
# it-puppet-hostgroup-cta profiles.

# (Re-)create the mhvtl environment
echo Wiping mhvtl and kubernetes library info...
systemctl stop mhvtl || true
sleep 2
rm -rf /etc/mhvtl
umount /opt/mhvtl || true
rm -rf /opt/mhvtl
rm -rf /opt/kubernetes/CTA/library
mkdir -p /opt/mhvtl
mount -t tmpfs -o size=512m tmpfs /opt/mhvtl

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
./generate_mhvtl_config.sh

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
./generate_librarydevice_PV.sh

################################################################################
### puppet:///modules/hg_cta/generate_PV.sh
echo Generating the log persistent volume
mkdir -pv /shared/logs
sudo kubectl delete pv log || true
sudo kubectl create -f ./log_PV.yaml
sudo kubectl get persistentvolumes -l type=logs

echo Generating the stg persistent volume
rm -rf /shared/stg
mkdir -pv /shared/stg
sudo kubectl delete pv stg || true
sudo kubectl create -f ./stg_PV.yaml
sudo kubectl get persistentvolumes -l type=stg

echo Generating the share host path
rm -rf /shared/cta
mkdir -pv /shared/cta/catdb
touch /shared/cta/catdb/catdb
chmod 1777 /shared/cta
