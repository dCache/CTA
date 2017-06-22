#!/bin/bash

EOSINSTANCE=ctaeos
EOS_DIR=/eos/ctaeos/cta/$(uuidgen)
TEST_FILE_NAME_BASE=test

NB_PROCS=1
NB_FILES=1
FILE_KB_SIZE=1


usage() { cat <<EOF 1>&2
Usage: $0 [-n <nb_files>] [-s <file_kB_size>] [-p <# parallel procs>]
EOF
exit 1
}

while getopts "n:s:p:" o; do
    case "${o}" in
        n)
            NB_FILES=${OPTARG}
            ;;
        s)
            FILE_KB_SIZE=${OPTARG}
            ;;
        p)
            NB_PROCS=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ ! -z "${error}" ]; then
    echo -e "ERROR:\n${error}"
    exit 1
fi




STATUS_FILE=$(mktemp)

dd if=/dev/urandom of=/tmp/testfile bs=1k count=${FILE_KB_SIZE} || exit 1

echo "Creating test dir in eos: ${EOS_DIR}"
# uuid should be unique no need to remove dir before...
# XrdSecPROTOCOL=sss eos -r 0 0 root://${EOSINSTANCE} rm -Fr ${EOS_DIR}
eos root://${EOSINSTANCE} mkdir -p ${EOS_DIR}

for ((i=0;i<${NB_FILES};i++)); do
  TEST_FILE_NAME=${TEST_FILE_NAME_BASE}$(printf %.4d $i)
  xrdcp --silent /tmp/testfile root://${EOSINSTANCE}/${EOS_DIR}/${TEST_FILE_NAME}
done

eos root://${EOSINSTANCE} ls ${EOS_DIR} | egrep "${TEST_FILE_NAME_BASE}[0-9]+" | sed -e 's/$/ copied/' > ${STATUS_FILE=}


SECONDS_PASSED=0
WAIT_FOR_ARCHIVED_FILE_TIMEOUT=60
while test 0 != $(grep -c copied$ ${STATUS_FILE}); do
  echo "Waiting for files to be archived to tape: Seconds passed = ${SECONDS_PASSED}"
  sleep 1
  let SECONDS_PASSED=SECONDS_PASSED+1

  if test ${SECONDS_PASSED} == ${WAIT_FOR_ARCHIVED_FILE_TIMEOUT}; then
    echo "Timed out after ${WAIT_FOR_ARCHIVED_FILE_TIMEOUT} seconds waiting for file to be archived to tape"
    break
  fi

  echo "$(grep -c archived$ ${STATUS_FILE})/${NB_FILES} archived"

  for TEST_FILE_NAME in $(grep copied$ ${STATUS_FILE} | sed -e 's/ .*$//'); do
    eos root://${EOSINSTANCE} info ${EOS_DIR}/${TEST_FILE_NAME} | awk '{print $4;}' | grep -q tape && sed -i ${STATUS_FILE} -e "s/${TEST_FILE_NAME} copied/${TEST_FILE_NAME} archived/"
  done

done

ARCHIVED=$(grep -c archived$ ${STATUS_FILE})


echo "###"
echo "${ARCHIVED}/${NB_FILES} archived"
echo "###"

echo "Removing disk replica of all archived files"
for TEST_FILE_NAME in $(grep archived$ ${STATUS_FILE} | sed -e 's/ .*$//'); do
    XrdSecPROTOCOL=sss eos -r 0 0 root://${EOSINSTANCE} file drop ${EOS_DIR}/${TEST_FILE_NAME} 1 &> /dev/null || echo "Could not remove disk replica for ${EOS_DIR}/${TEST_FILE_NAME}"
    test 1 = $(eos root://${EOSINSTANCE} info ${EOS_DIR}/${TEST_FILE_NAME} | grep -c nodrain) && sed -i ${STATUS_FILE} -e "s/${TEST_FILE_NAME} archived/${TEST_FILE_NAME} tapeonly/"
done

TAPEONLY=$(grep -c tapeonly$ ${STATUS_FILE})

echo "###"
echo "${TAPEONLY}/${ARCHIVED} on tape only"
echo "###"


echo "Trigerring EOS retrieve workflow as poweruser1:powerusers (12001:1200)"
#for TEST_FILE_NAME in $(grep tapeonly$ ${STATUS_FILE} | sed -e 's/ .*$//'); do
#  XrdSecPROTOCOL=sss xrdfs ${EOSINSTANCE} prepare -s "${EOS_DIR}/${TEST_FILE_NAME}?eos.ruid=12001&eos.rgid=1200" || echo "Could not trigger retrieve for ${EOS_DIR}/${TEST_FILE_NAME}"
#done

grep tapeonly$ ${STATUS_FILE} | sed -e 's/ .*$//' | XrdSecPROTOCOL=sss xargs --max-procs=${NB_PROCS} -iTEST_FILE_NAME xrdfs ${EOSINSTANCE} prepare -s "${EOS_DIR}/TEST_FILE_NAME?eos.ruid=12001&eos.rgid=1200"


# Wait for the copy to appear on disk
SECONDS_PASSED=0
WAIT_FOR_RETRIEVED_FILE_TIMEOUT=60
while test 0 != $(grep -c tapeonly$ ${STATUS_FILE}); do
  echo "Waiting for files to be retrieved from tape: Seconds passed = ${SECONDS_PASSED}"
  sleep 1
  let SECONDS_PASSED=SECONDS_PASSED+1

  if test ${SECONDS_PASSED} == ${WAIT_FOR_RETRIEVED_FILE_TIMEOUT}; then
    echo "Timed out after ${WAIT_FOR_ARCHIVED_FILE_TIMEOUT} seconds waiting for file to be retrieved tape"
    break
  fi

  echo "$(grep -c retrieved$ ${STATUS_FILE})/${NB_FILES} retrieved"

  for TEST_FILE_NAME in $(grep tapeonly$ ${STATUS_FILE} | sed -e 's/ .*$//'); do
    test 2 = $(eos root://${EOSINSTANCE} info ${EOS_DIR}/${TEST_FILE_NAME} | grep -c nodrain) && sed -i ${STATUS_FILE} -e "s/${TEST_FILE_NAME} tapeonly/${TEST_FILE_NAME} retrieved/"
  done

done

RETRIEVED=$(grep -c retrieved$ ${STATUS_FILE})

echo "###"
echo Results:
echo "RETRIEVED/TAPEONLY/ARCHIVED/NB_FILES"
echo "${RETRIEVED}/${TAPEONLY}/${ARCHIVED}/${NB_FILES}"
echo "###"


test ${RETRIEVED} = ${NB_FILES} && exit 0

echo "ERROR there were some lost files during the archive/retrieve test with ${NB_FILES} files."
exit 1