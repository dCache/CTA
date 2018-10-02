#!/bin/bash

# Which EOS repos should be enabled? (enable only one of these 2)
COMMIT_REPO_ENABLED=0  # enable commit repo? (0 -> no, 1-> yes)
TAG_REPO_ENABLED=1     # enable tag repo?    (0 -> no, 1-> yes)

usage() { cat <<EOF 1>&2
Usage: $0 -n <namespace>
EOF
exit 1
}

while getopts "n:" o; do
    case "${o}" in
        n)
            NAMESPACE=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${NAMESPACE}" ]; then
    usage
fi

if [ ! -z "${error}" ]; then
    echo -e "ERROR:\n${error}"
    exit 1
fi

kubectl -n ${NAMESPACE} exec ctaeos -- yum-config-manager --disable cta-artifacts

####
## CTA-CI REPOS HAVE A PRIORITY SET TO 4 (See: CTA/continuousintegration/docker/ctafrontend/cc7/etc/yum.repos.d/cta-ci.repo)
## EOS REPOS SHOULD HAVE A HIGHER PRIORITY (HENCE LOWER NUMBER) SO THAT WE ARE UPGRADING FROM EOS REPOS
## AND NOT CTA-CI IF ONE BRANCH IS AHEAD OF EOS
####
# This is the commit repo: any commit from any branch in EOS is pushed there => works if lucky...
echo -e "[eos-ci-eos-commit]\nname=EOS CI commit repo for eos packages\nbaseurl=http://storage-ci.web.cern.ch/storage-ci/eos/citrine/commit/el-7/x86_64/\npriority=3\ngpgcheck=0\nenabled=${COMMIT_REPO_ENABLED}\n\n" | kubectl -n ${NAMESPACE} exec -i ctaeos -- bash -c "cat > /etc/yum.repos.d/eos-ci-commit.repo"
# This is the koji testing repo: all tagged commits are going there.
# IF IT DOESN'T WORK WE CAN COMPLAIN HEAVILY
echo -e "[eos-ci-eos-tag]\nname=EOS koji testing repo for eos packages\nbaseurl=http://linuxsoft.cern.ch/internal/repos/eos7-testing/x86_64/os/Packages/\npriority=3\ngpgcheck=0\nenabled=${TAG_REPO_ENABLED}\n\n" | kubectl -n ${NAMESPACE} exec -i ctaeos -- bash -c "cat > /etc/yum.repos.d/eos-ci-tag.repo"
echo -e "[eos-ci-eos-depend]\nname=EOS CI repo for eos depend packages\nbaseurl=http://storage-ci.web.cern.ch/storage-ci/eos/citrine-depend/el-7/x86_64/\npriority=3\ngpgcheck=0\nenabled=1\n\n"  | kubectl -n ${NAMESPACE} exec -i ctaeos -- bash -c "cat > /etc/yum.repos.d/eos-ci-depend.repo"

kubectl -n ${NAMESPACE} exec ctaeos -- eos version

kubectl -n ${NAMESPACE} exec ctaeos -- sed -i '/^.:eos.*/d' /etc/yum/pluginconf.d/versionlock.list

kubectl -n ${NAMESPACE} exec ctaeos -- sed -i '/.*protected=1.*/d' /etc/yum.repos.d/cta-ci.repo

## Purge YUM cache before installing anything new
kubectl -n ${NAMESPACE} exec ctaeos -- yum clean all

kubectl -n ${NAMESPACE} exec ctaeos -- yum install -y eos-server eos-client

kubectl -n ${NAMESPACE} exec ctaeos -- systemctl restart eos@*

kubectl -n ${NAMESPACE} exec ctaeos -- systemctl status eos@*

kubectl -n ${NAMESPACE} exec ctaeos -- eos version

echo "Waiting for the EOS disk filesystem using /fst to boot and come on-line"
while test 1 != `kubectl -n ${NAMESPACE} exec ctaeos -- eos fs ls /fst | grep booted | grep online | wc -l`; do
  echo "Sleeping 1 second"
  sleep 1
done

echo "Available disk space inside EOS container:"
kubectl -n ${NAMESPACE} exec ctaeos -- df -h

echo "Launching archive_retrieve.sh:"
./archive_retrieve.sh -n ${NAMESPACE}
