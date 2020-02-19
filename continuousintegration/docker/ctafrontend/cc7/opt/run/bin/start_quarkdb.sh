#!/bin/bash

# I need 2 level of directories for quarkdb:
# /var/lib/<possible PV mount point>/<directory owned by xrootd>
# this way I can either use:
# - a PV that mounts a dedicated SSD for long tests
# - a directory in the container for shorter ones

QUARKDB_CONFIG=/etc/config/eos/xrootd-quarkdb.cfg

QUARKDB_DIRECTORY=$(cat ${QUARKDB_CONFIG} | grep redis.database | sed -e 's/.*redis.database\ \+//;s%/\ *$%%')

QUARKDB_SUBDIRECTORY=$(echo ${QUARKDB_DIRECTORY} | sed -e 's%/[^/]\+/*\ *$%%')

# make sure the first level of directory exists if there is no mounted PV
mkdir -p ${QUARKDB_SUBDIRECTORY}

yum-config-manager --enable eos-quarkdb

yum install -y quarkdb

quarkdb-create --path ${QUARKDB_DIRECTORY}

chown -R xrootd:xrootd ${QUARKDB_DIRECTORY}

cp -f ${QUARKDB_CONFIG} /etc/xrootd/xrootd-quarkdb.cfg

# quarkdb is starting as xrootd user and mgm as daemon
# the password file must be 400 for each service...
# for now copy and chown, later run quarkdb as daemon and use /etc/eos.keytab for both
cp /etc/eos.keytab /etc/eos.keytab.xrootd
chmod 400 /etc/eos.keytab.xrootd
chown xrootd:xrootd /etc/eos.keytab.xrootd

systemctl start xrootd@quarkdb

systemctl status xrootd@quarkdb
