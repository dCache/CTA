# This file must be sourced from another shell script
# . /opt/run/bin/init_pod.sh

LOGMOUNT=/mnt/logs

PV_PATH=""

if [ "-${MY_CONTAINER}-" != "--" ]; then
  PV_PATH="${LOGMOUNT}/${MY_NAME}/${MY_CONTAINER}"
else
  PV_PATH="${LOGMOUNT}/${MY_NAME}"
fi
mkdir -p ${PV_PATH}

echo "Copying initial /var/log content to ${PV_PATH}"
cd /var/log
tar -c . | tar -C ${PV_PATH} -xv

echo "Mounting logs volume ${PV_PATH} in /var/log"
mount --bind ${PV_PATH} /var/log

echo -n "Fixing reverse DNS for $(hostname): "
sed -i -c "s/^\($(hostname -i)\)\s\+.*$/\1 $(hostname -s).$(grep search /etc/resolv.conf | cut -d\  -f2) $(hostname -s)/" /etc/hosts
echo "DONE"

# Not needed anymore, keep it in case it comes back
#echo -n "Yum should resolve names using IPv4 DNS: "
#echo "ip_resolve=IPv4" >> /etc/yum.conf
#echo "DONE"

# redefining yum commands if in BUILDTREE environment
if [[ -n ${BUILDTREE_BASE} ]]; then
  echo "Configuring BUILDTREE environment"
  yum-config-manager() { echo "Skipping yum-config-manager $@"; }
  yum() { echo "Skipping yum $@"; }
  /opt/run/bin/mkSymlinks.sh

  if [ "-${MY_NAME}-" == "-ctafrontend-" ]; then
    echo "Adding cta user and group"
    /usr/bin/getent group cta || /usr/sbin/groupadd cta
    /usr/bin/getent passwd cta || /usr/sbin/useradd -s /bin/nologin -c "CTA system account" -g cta cta
  fi
fi
