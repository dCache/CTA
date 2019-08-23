#!/bin/bash 

. /opt/run/bin/init_pod.sh

if [ ! -e /etc/buildtreeRunner ]; then
  yum-config-manager --enable cta-artifacts
  yum-config-manager --enable eos-citrine-commit
  yum-config-manager --enable eos-citrine-depend
  yum-config-manager --enable eos-citrine

  # Install missing RPMs
  yum -y install eos-client eos-server xrootd-client xrootd-debuginfo xrootd-server cta-cli cta-debuginfo sudo logrotate cta-fst-gcd

  ## Keep this temporary fix that may be needed if going to protobuf3-3.5.1 for CTA
  # Install eos-protobuf3 separately as eos is OK with protobuf3 but cannot use it..
  # yum -y install eos-protobuf3
fi

# Check that the /usr/bin/cta-fst-gcd executable has been installed
test -e /usr/bin/cta-fst-gcd && echo "/usr/bin/cta-fst-gcd EXISTS" || exit 1
test -f /usr/bin/cta-fst-gcd && echo "/usr/bin/cta-fst-gcd IS A REGULAR FILE" || exit 1
test -x /usr/bin/cta-fst-gcd && echo "/usr/bin/cta-fst-gcd IS EXECUTABLE" || exit 1

# create local users as the mgm is the only one doing the uid/user/group mapping in the full infrastructure
groupadd --gid 1100 eosusers
groupadd --gid 1200 powerusers
groupadd --gid 1300 ctaadmins
groupadd --gid 1400 eosadmins
useradd --uid 11001 --gid 1100 user1
useradd --uid 11002 --gid 1100 user2
useradd --uid 12001 --gid 1200 poweruser1
useradd --uid 12002 --gid 1200 poweruser2
useradd --uid 13001 --gid 1300 ctaadmin1
useradd --uid 13002 --gid 1300 ctaadmin2
useradd --uid 14001 --gid 1400 eosadmin1
useradd --uid 14002 --gid 1400 eosadmin2

# copy needed template configuration files (nice to get all lines for logs)
yes | cp -r /opt/ci/ctaeos/etc /

eoshost=`hostname -f`

EOS_INSTANCE=`hostname -s`
TAPE_FS_ID=65535
CTA_BIN=/usr/bin/eoscta_stub
CTA_XrdSecPROTOCOL=sss
CTA_PROC_DIR=/eos/${EOS_INSTANCE}/proc/cta
CTA_WF_DIR=${CTA_PROC_DIR}/workflow
# dir for cta tests only for eosusers and powerusers
CTA_TEST_DIR=/eos/${EOS_INSTANCE}/cta
# dir for gRPC tests, should be the same as eos.prefix in client.sh
GRPC_TEST_DIR=/eos/grpctest
# dir for eos instance basic tests writable and readable by anyone
EOS_TMP_DIR=/eos/${EOS_INSTANCE}/tmp

# setup eos host and instance name
  sed -i -e "s/DUMMY_HOST_TO_REPLACE/${eoshost}/" /etc/sysconfig/eos
  sed -i -e "s/DUMMY_INSTANCE_TO_REPLACE/${EOS_INSTANCE}/" /etc/sysconfig/eos
  sed -i -e "s/DUMMY_HOST_TO_REPLACE/${eoshost}/" /etc/xrd.cf.mgm
  sed -i -e "s/DUMMY_INSTANCE_TO_REPLACE/${EOS_INSTANCE}/" /etc/xrd.cf.mgm
  sed -i -e "s/DUMMY_HOST_TO_REPLACE/${eoshost}/" /etc/xrd.cf.mq
  sed -i -e "s/DUMMY_HOST_TO_REPLACE/${eoshost}/" /etc/xrd.cf.fst

# Add this for SSI prococol buffer workflow (xrootd >=4.8.2)
echo "mgmofs.protowfendpoint ctafrontend:10955" >> /etc/xrd.cf.mgm
echo "mgmofs.protowfresource /ctafrontend"  >> /etc/xrd.cf.mgm

# Add configmap based configuration (initially Namespace)
test -f /etc/config/eos/xrd.cf.mgm && cat /etc/config/eos/xrd.cf.mgm >> /etc/xrd.cf.mgm

# quarkDB only for systemd initially...
cat /etc/config/eos/xrd.cf.mgm | grep mgmofs.nslib | grep -qi eosnsquarkdb && /opt/run/bin/start_quarkdb.sh

# prepare eos startup
  # skip systemd for eos initscripts
    export SYSTEMCTL_SKIP_REDIRECT=1
#  echo y | xrdsssadmin -k ${EOS_INSTANCE}+ -u daemon -g daemon add /etc/eos.keytab
# need a deterministic key for taped and it must be forwardable in case of kubernetes
# see [here](http://xrootd.org/doc/dev47/sec_config.htm#_Toc489606587)
# can only have one key????
echo -n '0 u:daemon g:daemon n:ctaeos+ N:6361884315374059521 c:1481241620 e:0 f:0 k:1a08f769e9c8e0c4c5a7e673247c8561cd23a0e7d8eee75e4a543f2d2dd3fd22' > /etc/eos.keytab 
    chmod 400 /etc/eos.keytab
    chown daemon:daemon /etc/eos.keytab
  mkdir -p /run/lock/subsys
  mkdir -p /var/eos/config/${eoshost}
    chown daemon:root /var/eos/config
    chown daemon:root /var/eos/config/${eoshost}
  touch   /var/eos/config/${eoshost}/default.eoscf
    chown daemon:daemon /var/eos/config/${eoshost}/default.eoscf

# add taped SSS must be in a kubernetes secret
#echo >> /etc/eos.keytab
#echo '0 u:stage g:tape n:taped+ N:6361736405290319874 c:1481207182 e:0 f:0 k:8e2335f24cf8c7d043b65b3b47758860cbad6691f5775ebd211b5807e1a6ec84' >> /etc/eos.keytab

  #/etc/init.d/eos master mgm
  #/etc/init.d/eos master mq
    touch /var/eos/eos.mq.master
    touch /var/eos/eos.mgm.rw
    echo "Configured mq mgm on localhost as master"

  source /etc/sysconfig/eos

  mkdir -p /fst
  chown daemon:daemon /fst/


# Waiting for /CANSTART file before starting eos
echo -n "Waiting for /CANSTART before going further"
for ((i=0;i<600;i++)); do
  test -f /CANSTART && break
  sleep 1
  echo -n .
done
test -f /CANSTART && echo OK || exit 1

# setting higher OS limits for EOS processes
maxproc=$(ulimit -u)
echo "Setting nproc for user daemon to ${maxproc}"
cat >> /etc/security/limits.conf <<EOF
daemon soft nproc ${maxproc}
daemon hard nproc ${maxproc}
EOF
echo "Checking limits..."
echo -n "nproc..."
if [ "${maxproc}" -eq "$(sudo -u daemon bash -c 'ulimit -u')" ]; then
  echo OK
else
  echo FAILED
fi
echo
echo "Limits summary for user daemon:"
sudo -u daemon bash -c 'ulimit -a'

NB_STARTED_CTA_FST_GCD=0
if test -f /var/log/eos/fst/cta-fst-gcd.log; then
  NB_STARTED_CTA_FST_GCD=`grep "cta-fst-gcd started" /var/log/eos/fst/cta-fst-gcd.log | wc -l`
fi

if [ "-${CI_CONTEXT}-" == '-systemd-' ]; then
  # generate eos_env file for systemd
  cat /etc/sysconfig/eos | sed -e 's/^export\s*//' > /etc/sysconfig/eos_env
  test -e /usr/lib64/libjemalloc.so.1 && echo LD_PRELOAD=/usr/lib64/libjemalloc.so.1 >> /etc/sysconfig/eos_env

  # start eos
  systemctl start eos@mq
  systemctl start eos@mgm
  systemctl start eos@fst

  echo -n "Waiting for eos to start"
  for ((i=1;i<20;i++)); do systemctl status eos@{mq,mgm,fst} &>/dev/null && break; sleep 1; echo -n .; done
  systemctl status eos@{mq,mgm,fst} &>/dev/null && echo OK || echo FAILED

  systemctl status eos@{mq,mgm,fst}

  systemctl start cta-fst-gcd

else
  # Using jemalloc as specified in
  # it-puppet-module-eos:
  #  code/templates/etc_sysconfig_mgm.erb
  #  code/templates/etc_sysconfig_mgm_env.erb
  #  code/templates/etc_sysconfig_fst.erb
  #  code/templates/etc_sysconfig_fst_env.erb
  test -e /usr/lib64/libjemalloc.so.1 && echo "Using jemalloc for EOS processes"
  test -e /usr/lib64/libjemalloc.so.1 && export LD_PRELOAD=/usr/lib64/libjemalloc.so.1

  # start and setup eos for xrdcp to the ${CTA_TEST_DIR}
  #/etc/init.d/eos start
    /usr/bin/xrootd -n mq -c /etc/xrd.cf.mq -l /var/log/eos/xrdlog.mq -b -Rdaemon
    /usr/bin/xrootd -n mgm -c /etc/xrd.cf.mgm -m -l /var/log/eos/xrdlog.mgm -b -Rdaemon
    /usr/bin/xrootd -n fst -c /etc/xrd.cf.fst -l /var/log/eos/xrdlog.fst -b -Rdaemon


  runuser -u daemon setsid /usr/bin/cta-fst-gcd > /dev/null 2>&1 < /dev/null &
fi

echo "Giving cta-fst-gcd 1 second to start logging"
sleep 1

let EXPECTED_NB_STARTED_CTA_FST_GCD=NB_STARTED_CTA_FST_GCD+1
ACTUAL_NB_STARTED_CTA_FST_GCD=0
if test -f /var/log/eos/fst/cta-fst-gcd.log; then
  ACTUAL_NB_STARTED_CTA_FST_GCD=`grep "cta-fst-gcd started" /var/log/eos/fst/cta-fst-gcd.log | wc -l`
else
  echo "/usr/bin/cta-fst-gcd DOES NOT EXIST"
  exit 1
fi
if test ${EXPECTED_NB_STARTED_CTA_FST_GCD} = ${ACTUAL_NB_STARTED_CTA_FST_GCD}; then
  echo "/usr/bin/cta-fst-gcd LOGGED 'cta-fst-gcd started'"
else
  echo "/usr/bin/cta-fst-gcd DID NOT LOG 'cta-fst-gcd started'"
  exit 1
fi

  eos vid enable krb5
  eos vid enable sss
  eos vid enable unix
  EOS_MGM_URL="root://${eoshost}" eosfstregister -r /fst default:1

  # Add user daemon to sudoers this is to allow recalls for the moment using this command
  #  XrdSecPROTOCOL=sss xrdfs ctaeos prepare -s "/eos/ctaeos/cta/${TEST_FILE_NAME}?eos.ruid=12001&eos.rgid=1200"
  eos vid set membership 2 +sudo

  eos node set ${eoshost} on
  eos space set default on
  eos attr -r set default=replica /eos
  eos attr -r set sys.forced.nstripes=1 /eos

  eos fs add -m ${TAPE_FS_ID} tape localhost:1234 /does_not_exist tape
  eos mkdir ${CTA_PROC_DIR}
  eos mkdir ${CTA_WF_DIR}

  # Configure gRPC interface:
  #
  # 1. Map requests to EOS virtual identities
  #eos -r 0 0 vid add gateway eoscta grpc
  #eos -r 0 0 vid add gateway [:1] grpc
  for i in {1..10}
  do
    ip_addr=10.254.75.$i
    eos -r 0 0 vid add gateway $ip_addr grpc
  done
  # 2. Add authorisation key
  #
  # Note: EOS_AUTH_KEY must be the same as the one specified in client.sh
  EOS_AUTH_KEY=migration-test-token
  eos -r 0 0 vid set map -grpc key:${EOS_AUTH_KEY} vuid:2 vgid:2
  echo "eos vid ls:"
  eos -r 0 0 vid ls
  # 3. Create top-level directory and set permissions to writeable by all
  eos mkdir ${GRPC_TEST_DIR}
  eos chmod 777 ${GRPC_TEST_DIR}

  # ${CTA_TEST_DIR} must be writable by eosusers and powerusers
  # but as there is no sticky bit in eos, we need to remove deletion for non owner to eosusers members
  # this is achieved through the ACLs.
  # ACLs in EOS are evaluated when unix permissions are failing, hence the 555 unix permission.
  eos mkdir ${CTA_TEST_DIR}
  eos chmod 555 ${CTA_TEST_DIR}
  eos attr set sys.acl=g:eosusers:rwx!d,u:poweruser1:rwx+dp,u:poweruser2:rwx+dp /eos/ctaeos/cta
  eos attr set CTA_StorageClass=ctaStorageClass ${CTA_TEST_DIR}
    
  # Link the attributes of CTA worklow directory to the test directory
  eos attr link ${CTA_WF_DIR} ${CTA_TEST_DIR}

  # Prepare the tmp dir so that we can test that the EOS instance is OK
  eos mkdir ${EOS_TMP_DIR}
  eos chmod 777 ${EOS_TMP_DIR}

  echo "Waiting for the EOS disk filesystem using /fst to boot and come on-line"
  while test 1 != `eos fs ls /fst | egrep 'booted.*online' | wc -l`; do
    echo "Sleeping 1 second"
    sleep 1
  done

# test EOS
  eos -b node ls

  echo "Waiting for the basic file transfer test to succeed (workaround for the booted/online issue )"
  for ((i=0; i<300; i++)); do
    # xrdcp --force to overwrite testFile if it was already created in the previous loop
    # but xrdcp failed for another reason
    xrdcp --force /etc/group root://${eoshost}:/${EOS_TMP_DIR}/testFile && break
    # If the file exists the loop exited on a successfull xrdcp, otherwise it exited upon timeout and all xrdcp failed
    eos rm ${EOS_TMP_DIR}/testFile
    echo -n "."
    sleep 1
  done
  echo OK
  failed_xrdcp_test=$i
  if test $failed_xrdcp_test -eq 0; then
    echo "[SUCCESS]: basic file transfer test on online/booted FS." | tee -a /var/log/CI_tests
  else
    echo "[ERROR]: basic file transfer test on online/booted FS (${failed_xrdcp_test} attempts)." | tee -a /var/log/CI_tests
  fi

# prepare EOS workflow
  # enable eos workflow engine
  eos space config default space.wfe=on
  # set the thread-pool size of concurrently running workflows
  # it should not be ridiculous (was 10 and we have 500 in production)
  eos space config default space.wfe.ntx=200
  ## Is the following really needed?
  # set interval in which the WFE engine is running
  # eos space config default space.wfe.interval=1

# prepare EOS garbage collectors
  # enable the 'file archived' garbage collector
  eos space config default space.filearchivedgc=on

# configure preprod directory separately
/opt/run/bin/eos_configure_preprod.sh

touch /EOSOK

if [ "-${CI_CONTEXT}-" == '-nosystemd-' ]; then
  /bin/bash
else
  # Add a DNS cache on the client as kubernetes DNS complains about `Nameserver limits were exceeded`
  yum install -y systemd-resolved
  systemctl start systemd-resolved
fi
