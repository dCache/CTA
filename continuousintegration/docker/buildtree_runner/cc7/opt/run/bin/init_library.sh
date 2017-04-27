#!/bin/bash

LIBRARY_CONFIG_DIR=/etc/config/library

function get_conf {
  test -r ${LIBRARY_CONFIG_DIR}/$1 && cat ${LIBRARY_CONFIG_DIR}/$1 || echo -n UNDEF
}

LIBRARYTYPE=UNDEF

echo "export LIBRARYTYPE=UNDEF" > /tmp/library-rc.sh


case "$(get_conf library.type)" in
  "UNDEF")
    echo "library configmap is not defined"
    ls ${LIBRARY_CONFIG_DIR}
    exit 1
    ;;
  "mhvtl")
    echo "Configuring mhvtl library"

    if [ "-${driveslot}-" == "--" ]; then
      echo "DRIVESLOT is not defined, using driveslot 0"
      echo -n 0 > /tmp/driveslot
    else
      echo "DRIVESLOT is set using its value"
      echo -n ${driveslot} > /tmp/driveslot
    fi


cat <<EOF >/tmp/library-rc.sh
export LIBRARYTYPE=mhvtl
export LIBRARYNAME=$(get_conf library.name)
export LIBRARYDEVICE=$(get_conf library.device)
export DRIVENAMES=$(get_conf library.drivenames)
export DRIVEDEVICES=$(get_conf library.drivedevices)
export TAPES=$(get_conf library.tapes)
export driveslot=$(cat /tmp/driveslot)
EOF
    ;;
  "ibm")
    echo "Configuring ibm library"

    if [ "-${driveslot}-" == "--" ]; then
      echo "DRIVESLOT is not defined, taking first physical drive with a serial number in the list"
      eval "export DRIVEDEVICES=$(get_conf library.drivedevices)"
      eval "export DRIVESERIALS=$(get_conf library.driveserials)"
      for i in "${!DRIVESERIALS[@]}"; do
        MY_DRIVE_SERIAL=$(sg_inq /dev/${DRIVEDEVICES[$i]} | grep 'Unit serial number' | sed -e 's/ //g' | cut -d: -f2)
        if [ "-${DRIVESERIALS[$i]}-" == "-${MY_DRIVE_SERIAL}-" ]; then
          echo -n $i
          break
        fi
      done > /tmp/driveslot
    else
      echo "DRIVESLOT is set using its value"
      echo -n ${driveslot} > /tmp/driveslot
    fi

cat <<EOF >/tmp/library-rc.sh
export LIBRARYTYPE=ibm
export LIBRARYNAME=$(get_conf library.name) 
export LIBRARYDEVICE=$(get_conf library.device)
export DRIVENAMES=$(get_conf library.drivenames)
export DRIVEDEVICES=$(get_conf library.drivedevices)
export DRIVESERIALS=$(get_conf library.driveserials)
export TAPES=$(get_conf library.tapes)
export driveslot=$(cat /tmp/driveslot)
EOF
  ;;
  *)
    echo "Error unknown library type: $(get_conf library.type)"
    exit 1
  ;;
esac

exit 0