/******************************************************************************
 *              vdqm_2.1.7-10-2_to_2.1.7-11.sql
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * This script upgrades a CASTOR VDQM v2.1.7-10 database into v2.1.7-10-1
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM CastorVersion WHERE release = '2_1_7_10_2';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts before this one.');
END;

--UPDATE CastorVersion SET release = '2_1_7_11';
COMMIT;

CREATE TABLE Tape2DriveDedication (vid VARCHAR2(2048), id INTEGER CONSTRAINT I_Tape2DriveDedication_Id PRIMARY KEY, tapeDrive INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* Not null column constraints */
ALTER TABLE Tape2DriveDedication MODIFY
  (vid CONSTRAINT NN_Tp2DrvDedic_vid NOT NULL);
ALTER TABLE Tape2DriveDedication MODIFY
  (tapeDrive CONSTRAINT NN_Tp2DrvDedic_tapeDrive NOT NULL);

/* Unique constraints */
-- A tape dedication is made unique by its VID and tape drive
ALTER TABLE Tape2DriveDedication
  ADD CONSTRAINT I_U_Tp2DrvDedic_vid_tapeDrive
    UNIQUE (vid, tapeDrive);

/* Foreign key constraints with an index for each */
ALTER TABLE Tape2DriveDedication
  ADD CONSTRAINT FK_Tp2DrvDedic_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_Tp2DrvDedic_tapeDrive
    FOREIGN KEY (tapeDrive)
    REFERENCES TapeDrive (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;
CREATE INDEX I_FK_Tp2DrvDedic_tapeDrive ON Tape2DriveDedication (tapeDrive);


/**
 * See the castorVdqmView package specification for documentation.
 */
CREATE OR REPLACE PACKAGE BODY castorVdqmView AS

  /**
   * This private function determines whether or not the specified drive and
   * GID pass the dedications of the drive.
   *
   * @param driveIdVar the ID of the drive.
   * @param gidVar     the GID of the client.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesGidDedications(
    driveIdVar IN NUMBER,
    gidVar     IN NUMBER)
    RETURN NUMBER AS
    nbGidDedicationsVar NUMBER;
  BEGIN
    -- Count the number of GID dedications for the drive
    -- (there should only be one)
    SELECT COUNT(*) INTO nbGidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND egid IS NOT NULL;

    -- Drive passes if there are no GID dedications for it
    IF nbGidDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more GID dedications

    -- Count the number of matching GID dedications
    SELECT COUNT(*) INTO nbGidDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND gidVar = TapeDriveDedication.egid;

    -- As there are GID dedications for the drive, it only passes if at least
    -- one matches
    IF nbGidDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesGidDedications;


  /**
   * This private function determines whether or not the specified drive and
   * access mode pass the dedications of the drive.
   *
   * @param driveIdVar    the ID of the drive.
   * @param accessModeVar the access mode of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesModeDedication(
    driveIdVar    IN NUMBER,
    accessModeVar IN NUMBER)
    RETURN NUMBER AS
    nbModeDedicationsVar NUMBER;
  BEGIN
    -- Count the number of mode dedications for the drive
    -- (there should only be one)
    SELECT COUNT(*) INTO nbModeDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND accessMode IS NOT NULL;

    -- Drive passes if there are no access mode dedications for it
    IF nbModeDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has a mode dedication

    -- Count the number of matching vid dedications
    -- (there should be a maximum of one)
    SELECT COUNT(*) INTO nbModeDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND TapeDriveDedication.accessMode = accessModeVar;

    -- As there is a mode dedication for the drive, the drive only passes if it
    -- matches
    IF nbModeDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesModeDedication;


  /**
   * This private function determines whether or not the specified drive and
   * host pass the dedications of the drive.
   *
   * @param driveIdVar    the ID of the drive.
   * @param clientHostVar the client host of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesHostDedications(
    driveIdVar    IN NUMBER,
    clientHostVar IN VARCHAR2)
    RETURN NUMBER AS
    nbHostDedicationsVar NUMBER;
  BEGIN
  /*
    -- Determine if the host is dedicated to another drive
    SELECT COUNT(*) INTO nbHostDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive != driveIdVar AND clientHost = clientHostVar;

    -- Drive does not pass if the host is dedicated to another drive
    IF nbHostDedicationsVar > 0 THEN
      RETURN 0;
    END IF;
  */
    -- Count the number of host dedications for the drive
    SELECT COUNT(*) INTO nbHostDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND clientHost IS NOT NULL;

    -- Drive passes if there are no host dedications for it
    IF nbHostDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more host dedications

    -- Count the number of matching host dedications
    SELECT COUNT(*) INTO nbHostDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND REGEXP_LIKE(clientHostVar, TapeDriveDedication.clientHost);

    -- As there are host dedications for the drive, it only passes if at least
    -- one matches
    IF nbHostDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesHostDedications;


  /**
   * This private function determines whether or not the specified drive and
   * UID pass the dedications of the drive.
   *
   * @param driveIdVar the ID of the drive.
   * @param uidVar     the UID of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesUidDedications(
    driveIdVar IN NUMBER,
    uidVar     IN NUMBER)
    RETURN NUMBER AS
    nbUidDedicationsVar NUMBER;
  BEGIN
    -- Count the number of UID dedications for the drive
    -- (there should only be one)
    SELECT COUNT(*) INTO nbUidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND euid IS NOT NULL;

    -- Drive passes if there are no UID dedications for it
    IF nbUidDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more UID dedications

    -- Count the number of matching UID dedications
    SELECT COUNT(*) INTO nbUidDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND uidVar = TapeDriveDedication.euid;

    -- As there are UID dedications for the drive, it only passes if at least
    -- one matches
    IF nbUidDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesUidDedications;


  /**
   * This private function determines whether or not the specified drive and
   * VID pass the dedications of the drive.
   *
   * @param driveIdVar the ID of the drive.
   * @param vidVar     the vid of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesVidDedications(
    driveIdVar IN NUMBER,
    vidVar     IN VARCHAR2)
    RETURN NUMBER AS
    nbVidDedicationsVar NUMBER;
  BEGIN
  /*
    -- Determine if the vid is dedicated to another drive
    SELECT COUNT(*) INTO nbVidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive != driveIdVar AND vid = vidVar;

    -- Drive does not pass if the vid is dedicated to another drive
    IF nbVidDedicationsVar > 0 THEN
      RETURN 0;
    END IF;
  */
    -- Count the number of vid dedications for the drive
    SELECT COUNT(*) INTO nbVidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND vid IS NOT NULL;

    -- Drive passes if there are no vid dedications for it
    IF nbVidDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more vid dedications

    -- Count the number of matching vid dedications
    SELECT COUNT(*) INTO nbVidDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND REGEXP_LIKE(vidVar, TapeDriveDedication.vid);

    -- As there are vid dedications for the drive, it only passes if at least
    -- one matches
    IF nbVidDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesVidDedications;


  /**
   * See the castorVdqmView package specification for documentation.
   */
  FUNCTION passesDedications(
    driveIdVar IN NUMBER,
    gidVar     IN NUMBER,
    hostVar    IN VARCHAR2,
    modeVar    IN NUMBER,
    uidVar     IN NUMBER,
    vidVar     IN VARCHAR2)
    RETURN NUMBER AS
    nbVidDedicationsVar NUMBER;
  BEGIN
    IF passesGidDedications(driveIdVar, gidVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesHostDedications(driveIdVar, hostVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesModeDedication(driveIdVar, modeVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesUidDedications(driveIdVar, uidVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesVidDedications(driveIdVar, vidVar) = 0 THEN
      RETURN 0;
    END IF;

    -- Drive has passed all of its dedications
    RETURN 1;
  END passesDedications;


  /**
   * See the castorVdqmView package specification for documentation.
   */
  FUNCTION getVdqmDedicate(driveIdVar IN NUMBER)
    RETURN VARCHAR2
  IS
    buf VARCHAR2(1024);
  BEGIN
    FOR accessModeDedication IN (
      SELECT accessMode
        FROM TapeDriveDedication
        WHERE tapeDrive = driveIdVar AND accessMode IS NOT NULL
        ORDER BY accessMode)
    LOOP
      -- Add a comma if there is already a dedication in the buffer
      IF LENGTH(buf) > 0 THEN
        buf := buf || ',';
      END IF;

      -- Add dedication to buffer
      buf := buf || 'mode=' || accessModeDedication.accessMode;
    END LOOP;

    FOR clientHostDedication IN (
      SELECT clientHost
        FROM TapeDriveDedication
        WHERE tapeDrive = driveIdVar AND clientHost IS NOT NULL
        ORDER BY clientHost)
    LOOP
      -- Add a comma if there is already a dedication in the buffer
      IF LENGTH(buf) > 0 THEN
        buf := buf || ',';
      END IF;

      -- Add dedication to buffer
      buf := buf || 'host=' || clientHostDedication.clientHost;
    END LOOP;

    FOR egidDedication IN (
      SELECT egid
        FROM TapeDriveDedication
        WHERE tapeDrive = driveIdVar AND egid IS NOT NULL
        ORDER BY egid)
    LOOP
      -- Add a comma if there is already a dedication in the buffer
      IF LENGTH(buf) > 0 THEN
        buf := buf || ',';
      END IF;

      -- Add dedication to buffer
      buf := buf || 'gid=' || egidDedication.egid;
    END LOOP;

    FOR euidDedication IN (
      SELECT euid
        FROM TapeDriveDedication
        WHERE tapeDrive = driveIdVar AND euid IS NOT NULL
        ORDER BY euid)
    LOOP
      -- Add a comma if there is already a dedication in the buffer
      IF LENGTH(buf) > 0 THEN
        buf := buf || ',';
      END IF;

      -- Add dedication to buffer
      buf := buf || 'uid=' || euidDedication.euid;
    END LOOP;

    FOR vidDedication IN (
      SELECT vid
        FROM TapeDriveDedication
        WHERE tapeDrive = driveIdVar AND vid IS NOT NULL
        ORDER BY vid)
    LOOP
      -- Add a comma if there is already a dedication in the buffer
      IF LENGTH(buf) > 0 THEN
        buf := buf || ',';
      END IF;

      -- Add dedication to buffer
      buf := buf || 'vid=' || vidDedication.vid;
    END LOOP;

    FOR vid2DrvDedication IN (
      SELECT vid
        FROM Tape2DriveDedication
        WHERE tapeDrive = driveIdVar
        ORDER BY vid)
    LOOP
      -- Add a comma if there is already a dedication in the buffer
      IF LENGTH(buf) > 0 THEN
        buf := buf || ',';
      END IF;

      -- Add dedication to buffer
      buf := buf || 'vid2drv=' || vid2DrvDedication.vid;
    END LOOP;

    RETURN buf;
  END getVdqmDedicate;

END castorVdqmView;


/**
 * This view shows candidate drive allocations that will reuse a current drive
 * allocation.
 */
CREATE OR REPLACE VIEW DriveAllocationsForReuse_VIEW AS SELECT UNIQUE
  TapeDrive.id as tapeDriveId,
  TapeDrive.tape as tapeId,
  TapeRequest.id as tapeRequestId,
  TapeAccessSpecification.accessMode,
  TapeRequest.modificationTime
FROM
  TapeRequest
INNER JOIN TapeAccessSpecification ON
  TapeRequest.tapeAccessSpecification = TapeAccessSpecification.id
INNER JOIN VdqmTape ON
  TapeRequest.tape = VdqmTape.id
INNER JOIN ClientIdentification ON
  TapeRequest.client = ClientIdentification.id
INNER JOIN TapeDrive ON
  TapeRequest.tape = TapeDrive.tape -- Request is for the tape in the drive
  AND (
    TapeRequest.requestedSrv IS NULL
    OR TapeRequest.requestedSrv = TapeDrive.tapeServer
  )
INNER JOIN TapeServer ON
  TapeDrive.tapeServer = TapeServer.id
WHERE
      TapeServer.actingMode=0 -- ACTIVE
  AND TapeRequest.tapeDrive IS NULL -- Request has not already been allocated
                                    -- a drive
  AND castorVdqmView.passesDedications(tapeDrive.id, ClientIdentification.egid,
    ClientIdentification.machine, TapeAccessSpecification.accessMode,
    ClientIdentification.euid, VdqmTape.vid)=1
ORDER BY
  TapeAccessSpecification.accessMode DESC,
  TapeRequest.modificationTime ASC;


/**
 * This view shows candidate tape drive allocations before any dedicatioins
 * have been taken into account.
 */
CREATE OR REPLACE VIEW PotentialDriveAllocations_VIEW AS SELECT UNIQUE
  TapeDrive.id as driveId,
  TapeDrive.driveName,
  TapeRequest.id as tapeRequestId,
  ClientIdentification.euid as clientEuid,
  ClientIdentification.egid as clientEgid,
  ClientIdentification.machine as clientMachine,
  TapeAccessSpecification.accessMode,
  VdqmTape.vid,
  TapeRequest.modificationTime,
  TapeRequest.creationTime,
  NVL(EffectiveVolumePriority_VIEW.priority,0) AS volumePriority
FROM
  TapeRequest
INNER JOIN TapeAccessSpecification ON
  TapeRequest.tapeAccessSpecification = TapeAccessSpecification.id
INNER JOIN VdqmTape ON
  TapeRequest.tape = VdqmTape.id
INNER JOIN ClientIdentification ON
  TapeRequest.client = ClientIdentification.id
INNER JOIN TapeDrive ON
  TapeRequest.deviceGroupName = TapeDrive.deviceGroupName
  AND (
    TapeRequest.requestedSrv IS NULL
    OR TapeRequest.requestedSrv = TapeDrive.tapeServer
  )
INNER JOIN TapeServer ON
  TapeDrive.tapeServer = TapeServer.id
LEFT OUTER JOIN EffectiveVolumePriority_VIEW ON
  VdqmTape.vid = EffectiveVolumePriority_VIEW.vid
  AND TapeAccessSpecification.accessMode = EffectiveVolumePriority_VIEW.tpMode
WHERE
  TapeDrive.status=0 -- UNIT_UP
  -- Exclude a request if its tape is associated with an on-going request
  AND NOT EXISTS (
    SELECT
      'x'
    FROM
      TapeRequest TapeRequest2
    WHERE
      TapeRequest2.tape = TapeRequest.tape
      AND TapeRequest2.tapeDrive IS NOT NULL
  )
  -- Exclude a request if its tape is already in a drive, such a request
  -- will be considered upon the release of the drive in question
  -- (cf. TapeDriveStatusHandler)
  AND NOT EXISTS (
    SELECT
      'x'
    FROM
      TapeDrive TapeDrive2
    WHERE
      TapeDrive2.tape = TapeRequest.tape
  )
  AND TapeServer.actingMode=0 -- TAPE_SERVER_ACTIVE
  AND TapeRequest.status=0 -- REQUEST_PENDING
ORDER BY
  TapeAccessSpecification.accessMode DESC,
  VolumePriority DESC,
  TapeRequest.creationTime ASC;


CREATE OR REPLACE PACKAGE BODY castorVdqm AS

  /**
   * Determines whether or not the specified drive and GID pass the dedications
   * of the drive.
   *
   * @param driveIdVar the ID of the drive.
   * @param gidVar     the GID of the client.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesGidDriveDedications(
    driveIdVar IN NUMBER,
    gidVar     IN NUMBER)
    RETURN NUMBER AS
    nbGidDedicationsVar NUMBER;
  BEGIN
    -- Count the number of GID dedications for the drive
    -- (there should only be one)
    SELECT COUNT(*) INTO nbGidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND egid IS NOT NULL;

    -- Drive passes if there are no GID dedications for it
    IF nbGidDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more GID dedications

    -- Count the number of matching GID dedications
    SELECT COUNT(*) INTO nbGidDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND gidVar = TapeDriveDedication.egid;

    -- As there are GID dedications for the drive, it only passes if at least
    -- one matches
    IF nbGidDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesGidDriveDedications;


  /**
   * Determines whether or not the specified drive and access mode pass the
   * dedications of the drive.
   *
   * @param driveIdVar    the ID of the drive.
   * @param accessModeVar the access mode of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesModeDriveDedication(
    driveIdVar    IN NUMBER,
    accessModeVar IN NUMBER)
    RETURN NUMBER AS
    nbModeDedicationsVar NUMBER;
  BEGIN
    -- Count the number of mode dedications for the drive
    -- (there should only be one)
    SELECT COUNT(*) INTO nbModeDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND accessMode IS NOT NULL;

    -- Drive passes if there are no access mode dedications for it
    IF nbModeDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has a mode dedication

    -- Count the number of matching vid dedications
    -- (there should be a maximum of one)
    SELECT COUNT(*) INTO nbModeDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND TapeDriveDedication.accessMode = accessModeVar;

    -- As there is a mode dedication for the drive, the drive only passes if it
    -- matches
    IF nbModeDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesModeDriveDedication;


  /**
   * Determines whether or not the specified drive and host pass the
   * dedications of the drive.
   *
   * @param driveIdVar    the ID of the drive.
   * @param clientHostVar the client host of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesHostDriveDedications(
    driveIdVar    IN NUMBER,
    clientHostVar IN VARCHAR2)
    RETURN NUMBER AS
    nbHostDedicationsVar NUMBER;
  BEGIN
    -- Count the number of host dedications for the drive
    SELECT COUNT(*) INTO nbHostDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND clientHost IS NOT NULL;

    -- Drive passes if there are no host dedications for it
    IF nbHostDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more host dedications

    -- Count the number of matching host dedications
    SELECT COUNT(*) INTO nbHostDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND REGEXP_LIKE(clientHostVar, TapeDriveDedication.clientHost);

    -- As there are host dedications for the drive, it only passes if at least
    -- one matches
    IF nbHostDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesHostDriveDedications;


  /**
   * Determines whether or not the specified drive and UID pass the dedications
   * of the drive.
   *
   * @param driveIdVar the ID of the drive.
   * @param uidVar     the UID of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesUidDriveDedications(
    driveIdVar IN NUMBER,
    uidVar     IN NUMBER)
    RETURN NUMBER AS
    nbUidDedicationsVar NUMBER;
  BEGIN
    -- Count the number of UID dedications for the drive
    -- (there should only be one)
    SELECT COUNT(*) INTO nbUidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND euid IS NOT NULL;

    -- Drive passes if there are no UID dedications for it
    IF nbUidDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more UID dedications

    -- Count the number of matching UID dedications
    SELECT COUNT(*) INTO nbUidDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND uidVar = TapeDriveDedication.euid;

    -- As there are UID dedications for the drive, it only passes if at least
    -- one matches
    IF nbUidDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesUidDriveDedications;


  /**
   * Determines whether or not the specified drive and VID pass the dedications
   * of the drive.
   *
   * @param driveIdVar the ID of the drive.
   * @param vidVar     the vid of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesVidDriveDedications(
    driveIdVar IN NUMBER,
    vidVar     IN VARCHAR2)
    RETURN NUMBER AS
    nbVidDedicationsVar NUMBER;
  BEGIN
    -- Count the number of vid dedications for the drive
    SELECT COUNT(*) INTO nbVidDedicationsVar
      FROM TapeDriveDedication
      WHERE tapeDrive = driveIdVar AND vid IS NOT NULL;

    -- Drive passes if there are no vid dedications for it
    IF nbVidDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Drive has one or more vid dedications

    -- Count the number of matching vid dedications
    SELECT COUNT(*) INTO nbVidDedicationsVar
      FROM TapeDriveDedication
      WHERE
            TapeDriveDedication.tapeDrive = driveIdVar
        AND REGEXP_LIKE(vidVar, TapeDriveDedication.vid);

    -- As there are vid dedications for the drive, it only passes if at least
    -- one matches
    IF nbVidDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesVidDriveDedications;


  /**
   * Determines whether or not the specified drive and VID pass the
   * tape-to-drive dedications.
   *
   * @param driveIdVar the ID of the drive.
   * @param vidVar     the vid of the volume request.
   * @return 1 if the dedications are passed, else 0.
   */
  FUNCTION passesTape2DriveDedications(
    driveIdVar IN NUMBER,
    vidVar     IN VARCHAR2)
    RETURN NUMBER AS
    nbDedicationsVar NUMBER;
  BEGIN
    -- Count the number of dedications for the tape
    SELECT COUNT(*) INTO nbDedicationsVar
      FROM Tape2DriveDedication
      WHERE Tape2DriveDedication.vid = vidVar;

    -- Dedications passed if there are no dedications for the tape
    IF nbDedicationsVar = 0 THEN
      RETURN 1;
    END IF;

    -- Tape has one or more dedications

    -- Count the number of matching dedications
    SELECT COUNT(*) INTO nbDedicationsVar
      FROM Tape2DriveDedication
      WHERE
        Tape2DriveDedication.vid = vidVar
        AND Tape2DriveDedication.tapeDrive = driveIdVar;

    -- As there are tape dedications, they are only passed if at least one
    -- matches
    IF nbDedicationsVar > 0 THEN
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
  END passesTape2DriveDedications;


  /**
   * This function determines if the specified drive and usage pass all drive
   * and tape dedications.
   *
   * @param driveIdVar the ID of the drive.
   * @param gidVar     the gid of the client.
   * @param hostVar    the host of the client.
   * @param modeVar    the tape access mode.
   * @param uidVar     the uid of the client.
   * @param vidVar     the vid of the volume request.
   * @return 1 if all dedications are passed, else 0.
   */
  FUNCTION passesDedications(
    driveIdVar IN NUMBER,
    gidVar     IN NUMBER,
    hostVar    IN VARCHAR2,
    modeVar    IN NUMBER,
    uidVar     IN NUMBER,
    vidVar     IN VARCHAR2)
    RETURN NUMBER AS
    nbVidDedicationsVar NUMBER;
  BEGIN
    IF passesGidDriveDedications(driveIdVar, gidVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesHostDriveDedications(driveIdVar, hostVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesModeDriveDedication(driveIdVar, modeVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesUidDriveDedications(driveIdVar, uidVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesVidDriveDedications(driveIdVar, vidVar) = 0 THEN
      RETURN 0;
    END IF;

    IF passesTape2DriveDedications(driveIdVar, vidVar) = 0 THEN
      RETURN 0;
    END IF;

    -- Drive has passed all of its dedications
    RETURN 1;
  END passesDedications;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE allocateDrive(
    returnVar         OUT NUMBER,
    tapeDriveIdVar    OUT NUMBER,
    tapeDriveNameVar  OUT NOCOPY VARCHAR2,
    tapeRequestIdVar  OUT NUMBER,
    tapeRequestVidVar OUT NOCOPY VARCHAR2
    ) AS

    tapeDriveStatusVar   NUMBER;
    tapeRequestStatusVar NUMBER;

  BEGIN
    returnVar         := 0; -- No possible allocation could be found
    tapeDriveIdVar    := 0;
    tapeDriveNameVar  := '';
    tapeRequestIdVar  := 0;
    tapeRequestVidVar := '';

    SELECT
      tapeDriveId,
      tapeRequestId
    INTO
      tapeDriveIdVar, tapeRequestIdVar
    FROM
      CandidateDriveAllocations_VIEW
    WHERE
      rownum < 2;

    -- If there is a possible drive allocation
    IF tapeDriveIdVar IS NOT NULL AND tapeRequestIdVar IS NOT NULL THEN

      -- The status of the drives and requests maybe modified by other scheduler
      -- threads.  The status of the drives may be modified by threads handling
      -- drive request messages.  The status of the requests may be modified by
      -- threads handling tape request messages.  Therefore get a lock on the
      -- corresponding drive and request rows and retrieve their statuses to see
      -- if the drive allocation is still valid
      SELECT TapeDrive.status INTO TapeDriveStatusVar
      FROM TapeDrive
      WHERE TapeDrive.id = tapeDriveIdVar
      FOR UPDATE;
      SELECT TapeRequest.status INTO TapeRequestStatusVar
      FROM TapeRequest
      WHERE TapeRequest.id = tapeRequestIdVar
      FOR UPDATE;

      -- Get the drive name (used for logging)
      SELECT TapeDrive.driveName INTO TapeDriveNameVar
      FROM TapeDrive
      WHERE TapeDrive.id = tapeDriveIdVar;

      -- Get the VID of the pending request (used for logging)
      SELECT VdqmTape.vid INTO tapeRequestVidVar
      FROM TapeRequest
      INNER JOIN VdqmTape ON TapeRequest.tape = VdqmTape.id
      WHERE TapeRequest.id = tapeRequestIdVar;

      -- If the drive allocation is still valid, i.e. drive status is UNIT_UP
      -- and request status is REQUEST_PENDING
      IF(TapeDriveStatusVar = 0) AND (TapeRequestStatusVar = 0) THEN

        -- Allocate the free drive to the pending request
        UPDATE TapeDrive SET
          status           = 1, -- UNIT_STARTING
          jobId            = 0,
          modificationTime = castorVdqmCommon.getTime(),
          runningTapeReq   = tapeRequestIdVar
        WHERE
          id = tapeDriveIdVar;
        UPDATE TapeRequest SET
          status           = 1, -- MATCHED
          tapeDrive        = tapeDriveIdVar,
          modificationTime = castorVdqmCommon.getTime()
        WHERE
          id = tapeRequestIdVar;

        -- A free drive has been allocated to a pending request
        returnVar := 1;

     -- Else the drive allocation is no longer valid
     ELSE

       -- A possible drive allocation was found but was invalidated by other
       -- threads before the appropriate locks could be taken
       returnVar := -1;

     END IF; -- If the drive allocation is still valid

    END IF; -- If there is a possible drive allocation

  EXCEPTION

    -- Do nothing if there was no free tape drive which could be allocated to a
    -- pending request
    WHEN NO_DATA_FOUND THEN NULL;

  END allocateDrive;


  /**
   * Future scheduler algorithm.  This procedure will replace allocateDrive
   * when it has been fully tested.
   */
  PROCEDURE allocateDrive2(
    returnVar         OUT NUMBER,
    tapeDriveIdVar    OUT NUMBER,
    tapeDriveNameVar  OUT NOCOPY VARCHAR2,
    tapeRequestIdVar  OUT NUMBER,
    tapeRequestVidVar OUT NOCOPY VARCHAR2
    ) AS

    tapeDriveStatusVar   NUMBER;
    tapeRequestStatusVar NUMBER;

  BEGIN
    returnVar         := 0; -- No possible allocation could be found
    tapeDriveIdVar    := 0;
    tapeDriveNameVar  := '';
    tapeRequestIdVar  := 0;
    tapeRequestVidVar := '';

    -- For each potential drive allocation, i.e. a drive allocation for which
    -- no dedications have been taken into account
    FOR potentialAllocation IN (
      SELECT driveId, driveName, tapeRequestId, clientEuid, clientEgid,
        clientMachine, accessMode, vid
      FROM PotentialDriveAllocations_VIEW
    ) LOOP
      -- The status of the drives and requests maybe modified by other scheduler
      -- threads.  The status of the drives may be modified by threads handling
      -- drive request messages.  The status of the requests may be modified by
      -- threads handling tape request messages.  Therefore get a lock on the
      -- corresponding drive and request rows and retrieve their statuses to see
      -- if the drive allocation is still valid
      SELECT TapeDrive.status INTO TapeDriveStatusVar
      FROM TapeDrive
      WHERE TapeDrive.id = potentialAllocation.driveId
      FOR UPDATE;
      SELECT TapeRequest.status INTO TapeRequestStatusVar
      FROM TapeRequest
      WHERE TapeRequest.id = potentialAllocation.tapeRequestId
      FOR UPDATE;

      -- If the drive allocation is still valid, i.e. drive status is UNIT_UP
      -- and request status is REQUEST_PENDING
      IF(TapeDriveStatusVar = 0) AND (TapeRequestStatusVar = 0) THEN

        IF passesDedications(
          potentialAllocation.driveId,
          potentialAllocation.clientEgid,
          potentialAllocation.clientMachine,
          potentialAllocation.accessMode,
          potentialAllocation.clientEuid,
          potentialAllocation.vid) = 1 THEN

          -- Set the drive id for logging
          tapeDriveIdVar := potentialAllocation.driveId;

          -- Set the drive name for logging
          TapeDriveNameVar := potentialAllocation.driveName;

          -- Set the request id for logging
          tapeRequestIdVar := potentialAllocation.tapeRequestId;

          -- Set the VID of the pending request for logging
          tapeRequestVidVar := potentialAllocation.vid;

          -- Allocate the free drive to the pending request
          UPDATE TapeDrive SET
            status           = 1, -- UNIT_STARTING
            jobId            = 0,
            modificationTime = castorVdqmCommon.getTime(),
            runningTapeReq   = potentialAllocation.tapeRequestId
          WHERE
            id = potentialAllocation.driveId;
          UPDATE TapeRequest SET
            status           = 1, -- MATCHED
            tapeDrive        = potentialAllocation.driveId,
            modificationTime = castorVdqmCommon.getTime()
          WHERE
            id = potentialAllocation.tapeRequestId;

          -- A free drive has been allocated to a pending request
          -- Set the return value of the procedure and exit this for loop
          returnVar := 1;
          EXIT;

        END IF; -- passesDedications

      END IF; -- The drive allocation is still valid

    END LOOP; -- For each potential drive allocation

  END allocateDrive2;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE getRequestToSubmit(tapeReqId OUT NUMBER) AS
  LockError EXCEPTION;
  PRAGMA EXCEPTION_INIT (LockError, -54);
  CURSOR c IS
     SELECT id
       FROM TapeRequest
      WHERE status = 1  -- MATCHED
      FOR UPDATE SKIP LOCKED;
  BEGIN
    tapeReqId := 0;
    OPEN c;
    FETCH c INTO tapeReqId;
    UPDATE TapeRequest SET status = 2 WHERE id = tapeReqId;  -- BEINGSUBMITTED
    CLOSE c;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- just return reqId = 0, nothing to do
    NULL;
  WHEN LockError THEN
    -- We have observed ORA-00054 errors (resource busy and acquire with
    -- NOWAIT) even with the SKIP LOCKED clause. This is a workaround to ignore
    -- the error until we understand what to do, another thread will pick up
    -- the request so we don't do anything.
    NULL;
  END getRequestToSubmit;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE reuseDriveAllocation(
    tapeIdVar          IN NUMBER,
    tapeDriveIdVar     IN NUMBER,
    returnVar         OUT NUMBER,
    tapeRequestIdVar  OUT NUMBER)
  AS
    tapeDriveStatusVar   NUMBER;
    mountedTapeIdVar     NUMBER;
    tapeRequestStatusVar NUMBER;
  BEGIN
    returnVar         := 0; -- No possible reuse was found
    tapeRequestIdVar  := 0;

    -- Try to find a candidate volume request that can reuse the current drive
    -- allocation
    SELECT tapeRequestId INTO tapeRequestIdVar
      FROM DriveAllocationsForReuse_VIEW
      WHERE
            DriveAllocationsForReuse_VIEW.TapeDriveId = tapeDriveIdVar
        AND DriveAllocationsForReuse_VIEW.TapeId      = tapeIdVar
        AND rownum < 2;

    -- A candidate was found, because a NO_DATA_FOUND exception was not raised

    -- The status of the drives including which tapes may be mounted in them,
    -- may be modified by other threads handling drive request messages.  The
    -- status of the requests may be modified by threads handling tape request
    -- messages.  Therefore get a lock on the corresponding drive and request
    -- rows and retrieve their statuses and mounted tape in the case of the
    -- drive, to see if the reuse of the drive allocation is still valid.
    SELECT TapeDrive.status, TapeDrive.tape
    INTO TapeDriveStatusVar, mountedTapeIdVar
    FROM TapeDrive
    WHERE TapeDrive.id = tapeDriveIdVar
    FOR UPDATE;
    SELECT TapeRequest.status
    INTO TapeRequestStatusVar
    FROM TapeRequest
    WHERE TapeRequest.id = tapeRequestIdVar
    FOR UPDATE;

    -- If the reuse of the drive allocation is still valid, i.e. the drive's
    -- status is VOL_MOUNTED and the correct tape is mounted and the tape
    -- request's status is REQUEST_PENDING
    IF(tapeDriveStatusVar = 3) AND(mountedTapeIdVar = tapeIdVar) AND
      (tapeRequestStatusVar = 0) THEN

      -- Reuse the drive allocation with the pending request
      UPDATE TapeRequest SET
        status           = 1,  -- MATCHED
        tapeDrive        = tapeDriveIdVar,
        modificationTime = castorVdqmCommon.getTime()
      WHERE id = tapeRequestIdVar;
      UPDATE TapeDrive SET
        status           = 1, -- UNIT_STARTING
        jobId            = 0,
        runningTapeReq   = tapeRequestIdVar,
        modificationTime = castorVdqmCommon.getTime()
      WHERE id = tapeDriveIdVar;

      -- The drive allocation was reused
      returnVar := 1;

     -- Else the reuse of the drive allocation is no longer valid
     ELSE

       -- A possible reuse of the drive allocation was found but was invalidated
       -- by other threads before the appropriate locks could be taken
       returnVar := -1;

    END IF; -- If the reuse of the drive allocation is still valid

  EXCEPTION
    -- Return a tape request ID of 0 if there was no candidate volume request
    -- which would have reused the current drive allocation
    WHEN NO_DATA_FOUND THEN NULL;
  END reuseDriveAllocation;


  /**
   * This procedure parses the specified dedicate string for gid, host, mode,
   * uid and VID dedications.  This procedure raises an application error with
   * an error number of -20001 if it detects an error in the dedicate string.
   *
   * @param dedicateStrVar the dedicate string to be parsed.
   * @param gidVar the extracted gid if there was one, else NULL.
   * @param hostVar the extracted host if there was one, else NULL.
   * @param modeVar the extracted mode if there was one, else NULL.
   * @param uidVar the extracted uid if there was one, else NULL.
   * @param vidVar the extracted vid if there was one, else NULL
   * @param vid2DrvVar the extracted vid2Drv if there was one, else NULL
   * @exception castorVdqmException.invalid_drive_dedicate if an error is
   * detected in the drive dedicate string.
   */
  PROCEDURE parseDedicateStr(
    dedicateStrVar  IN VARCHAR2,
    gidVar         OUT NUMBER,
    hostVar        OUT NOCOPY VARCHAR2,
    modeVar        OUT NUMBER,
    uidVar         OUT NUMBER,
    vidVar         OUT NOCOPY VARCHAR2
    ) AS
    dedicationStrsVar castorVdqmCommon.Varchar256List;
    nameValueListVar  castorVdqmCommon.Varchar256List;
    TYPE NameValue is RECORD (name VARCHAR2(256), val VARCHAR2(256));
    nameValueVar      NameValue;
    TYPE NameValueList IS TABLE OF NameValue INDEX BY BINARY_INTEGER;
    dedicationsVar    NameValueList;
    indexVar          NUMBER := 1;
    dummyNumVar       NUMBER := NULL;
    numAgeVar         NUMBER := 0; -- Number of     age dedications found so far
    numDateStrVar     NUMBER := 0; -- Number of dateStr dedications found so far
    numGidVar         NUMBER := 0; -- Number of     GID dedications found so far
    numHostVar        NUMBER := 0; -- Number of    host dedications found so far
    numModeVar        NUMBER := 0; -- Number of    mode dedications found so far
    numNameVar        NUMBER := 0; -- Number of    name dedications found so far
    numTimeStrVar     NUMBER := 0; -- Number of timeStr dedications found so far
    numUidVar         NUMBER := 0; -- Number of     UID dedications found so far
    numVidVar         NUMBER := 0; -- Number of     VID dedications found so far
  BEGIN
    gidVar     := NULL;
    hostVar    := NULL;
    modeVar    := NULL;
    uidVar     := NULL;
    vidVar     := NULL;

    -- Return if the dedicate string is NULL or empty
    IF dedicateStrVar IS NULL OR dedicateStrVar = '' THEN
      RETURN;
    END IF;

    -- Split the dedicate string into the individual dedications
    dedicationStrsVar := castorVdqmCommon.tokenize(dedicateStrVar, ',');

    -- Split each individual dedication into a name value pair
    FOR indexVar IN dedicationStrsVar.FIRST .. dedicationStrsVar.LAST LOOP
      nameValueListVar :=
        castorVdqmCommon.tokenize(dedicationStrsVar(indexVar), '=');

      -- Raise an application error if the dedication is not a name value pair
      IF nameValueListVar.count != 2 THEN
        castorVdqmException.throw(castorVdqmException.invalid_drive_dedicate_cd,
          'Invalid drive dedicate string.  ' ||
          'Dedication is not a valid name value pair ''' ||
          dedicationStrsVar(indexVar) || '''');
      END IF;

      nameValueVar.name := LOWER(LTRIM(RTRIM(nameValueListVar(1))));
      nameValueVar.val  := LTRIM(RTRIM(nameValueListVar(2)));

      dedicationsVar(dedicationsVar.count + 1) := nameValueVar;
    END LOOP;

    -- Extract the specific types of dedication (gid, host, etc.), raising an
    -- application error if there is an invalid dedication
    FOR indexVar IN dedicationsVar.FIRST .. dedicationsVar.LAST LOOP
      CASE
        WHEN dedicationsVar(indexVar).name = 'age' THEN
          IF numAgeVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one age dedication. ''');
          END IF;
          numAgeVar := numAgeVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || ' ''age'' dedications are not supported');
          END IF;
        WHEN dedicationsVar(indexVar).name = 'datestr' THEN
          IF numDateStrVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one datestr dedication. ''');
          END IF;
          numDateStrVar := numDateStrVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || ' ''datestr'' dedications are not supported');
          END IF;
        WHEN dedicationsVar(indexVar).name = 'gid' THEN
          IF numGidVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one gid dedication. ''');
          END IF;
          numGidVar := numGidVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            BEGIN
              gidVar := TO_NUMBER(dedicationsVar(indexVar).val);
            EXCEPTION
              WHEN VALUE_ERROR THEN
                castorVdqmException.throw(
                  castorVdqmException.invalid_drive_dedicate_cd,
                  'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
                  || '  Invalid gid dedication ''' ||
                  dedicationsVar(indexVar).val || ''' ' || SQLERRM);
            END;
          END IF;
        WHEN dedicationsVar(indexVar).name = 'host' THEN
          IF numhostVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one host dedication.');
          END IF;
          numHostVar := numHostVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            BEGIN
              SELECT COUNT(*) INTO dummyNumVar
                FROM DUAL WHERE REGEXP_LIKE(dummy,dedicationsVar(indexVar).val);
              hostVar := dedicationsVar(indexVar).val;
            EXCEPTION
              WHEN OTHERS THEN
                castorVdqmException.throw(
                  castorVdqmException.invalid_drive_dedicate_cd,
                  'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
                  || '  Invalid host dedication ''' ||
                  dedicationsVar(indexVar).val || ''' ' || SQLERRM);
            END;
          END IF;
        WHEN dedicationsVar(indexVar).name = 'mode' THEN
          IF numModeVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one mode dedication.');
          END IF;
          numModeVar := numModeVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            BEGIN
              modeVar := TO_NUMBER(dedicationsVar(indexVar).val);
            EXCEPTION
              WHEN VALUE_ERROR THEN
                castorVdqmException.throw(
                  castorVdqmException.invalid_drive_dedicate_cd,
                  'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
                  || '  Invalid mode dedication ''' ||
                  dedicationsVar(indexVar).val || ''' ' || SQLERRM);
            END;
          END IF;
        WHEN dedicationsVar(indexVar).name = 'name' THEN
          IF numNameVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one name dedication.');
          END IF;
          numNameVar := numNameVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || ' ''name'' dedications are not supported');
          END IF;
        WHEN dedicationsVar(indexVar).name = 'timestr' THEN
          IF numTimeStrVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one timestr dedication.');
          END IF;
          numTimeStrVar := numTimeStrVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || ' ''timestr'' dedications are not supported');
          END IF;
        WHEN dedicationsVar(indexVar).name = 'uid' THEN
          IF numUidVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one uid dedication.');
          END IF;
          numUidVar := numUidVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            BEGIN
              uidVar := TO_NUMBER(dedicationsVar(indexVar).val);
            EXCEPTION
              WHEN VALUE_ERROR THEN
                castorVdqmException.throw(
                  castorVdqmException.invalid_drive_dedicate_cd,
                  'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
                  || '  Invalid uid dedication ''' ||
                  dedicationsVar(indexVar).val || ''' ' || SQLERRM);
            END;
          END IF;
        WHEN dedicationsVar(indexVar).name = 'vid' THEN
          IF numVidVar > 0 THEN
            castorVdqmException.throw(
              castorVdqmException.invalid_drive_dedicate_cd,
              'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
              || '  More than one vid dedication.');
          END IF;
          numVidVar := numVidVar + 1;

          IF dedicationsVar(indexVar).val != '.*' THEN
            BEGIN
              SELECT COUNT(*) INTO dummyNumVar
                FROM DUAL WHERE REGEXP_LIKE(dummy,dedicationsVar(indexVar).val);
              vidVar := dedicationsVar(indexVar).val;
            EXCEPTION
              WHEN OTHERS THEN
                castorVdqmException.throw(
                castorVdqmException.invalid_drive_dedicate_cd,
                'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
                || '  Invalid vid dedication ''' || dedicationsVar(indexVar).val
                || ''' ' || SQLERRM);
            END;
          END IF;
        ELSE
          castorVdqmException.throw(
            castorVdqmException.invalid_drive_dedicate_cd,
            'Invalid drive dedicate string ''' || dedicateStrVar || '''.'
            || '  Unknown dedication type ''' || dedicationsVar(indexVar).name
            || '''');
      END CASE;
    END LOOP;

  END parseDedicateStr;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE dedicateDrive(driveNameVar IN VARCHAR2, serverNameVar IN  VARCHAR2,
    dgNameVar IN  VARCHAR2, dedicateVar IN  VARCHAR2) AS
    gidVar                   NUMBER;
    hostVar                  VARCHAR2(256);
    modeVar                  NUMBER;
    uidVar                   NUMBER;
    vidVar                   VARCHAR2(256);
    driveIdVar               NUMBER;
    TYPE dedicationList_t IS TABLE OF NUMBER INDEX BY BINARY_INTEGER;
    driveDedicationsToDelete dedicationList_t;
    dedicationIdVar          NUMBER;
  BEGIN

    -- Get the id of the tape drive
    BEGIN
      SELECT
        TapeDrive.id
        INTO driveIdVar
        FROM TapeDrive
        INNER JOIN TapeServer ON
          TapeDrive.tapeServer = TapeServer.id
        INNER JOIN DeviceGroupName ON
          TapeDrive.deviceGroupName = DeviceGroupName.id
        WHERE
              TapeDrive.driveName    = driveNameVar
          AND TapeServer.serverName  = serverNameVar
          AND DeviceGroupName.dgName = dgNameVar;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      castorVdqmException.throw(castorVdqmException.drive_not_found_cd,
        'Tape drive ''' || dgNameVar || ' ' || driveNameVar || '@' ||
        serverNameVar || ''' not found in database.');
    END;

    -- Lock the tape drive row
    BEGIN
      SELECT
        TapeDrive.id
        INTO driveIdVar
        FROM TapeDrive
        WHERE
          TapeDrive.id = driveIdVar
        FOR UPDATE;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      castorVdqmException.throw(castorVdqmException.drive_not_found_cd,
        'Tape drive ''' || dgNameVar || ' ' || driveNameVar || '@' ||
        serverNameVar || ''' not found in database.');
    END;

    -- Parse the dedication string raising an application error with an error
    -- number of -20001 if there is an error in the dedicate string
    parseDedicateStr(dedicateVar, gidVar, hostVar, modeVar, uidVar, vidVar);

    -- Delete all existing drive dedications associated with tape drive
    SELECT id BULK COLLECT INTO driveDedicationsToDelete
      FROM TapeDriveDedication
      WHERE TapeDriveDedication.tapeDrive = driveIdVar;
    IF driveDedicationsToDelete.COUNT > 0 THEN
      FOR i IN driveDedicationsToDelete.FIRST .. driveDedicationsToDelete.LAST
        LOOP
        DELETE FROM TapeDriveDedication
          WHERE TapeDriveDedication.id = driveDedicationsToDelete(i);
        DELETE FROM Id2Type
          WHERE Id2Type.id = driveDedicationsToDelete(i);
      END LOOP;
    END IF;

    -- Insert new drive dedications
    IF gidVar IS NOT NULL THEN
      INSERT INTO TapeDriveDedication(id, tapeDrive, egid)
        VALUES(ids_seq.nextval, driveIdVar, gidVar)
      RETURNING id INTO dedicationIdVar;
      INSERT INTO Id2Type (id, type)
        VALUES (dedicationIdVar, 90);
    END IF;
    IF hostVar IS NOT NULL THEN
      INSERT INTO TapeDriveDedication(id, tapeDrive, clientHost)
        VALUES(ids_seq.nextval, driveIdVar, hostVar)
      RETURNING id INTO dedicationIdVar;
      INSERT INTO Id2Type (id, type)
        VALUES (dedicationIdVar, 90);
    END IF;
    IF modeVar IS NOT NULL THEN
      INSERT INTO TapeDriveDedication(id, tapeDrive, accessMode)
        VALUES(ids_seq.nextval, driveIdVar, modeVar)
      RETURNING id INTO dedicationIdVar;
      INSERT INTO Id2Type (id, type)
        VALUES (dedicationIdVar, 90);
    END IF;
    IF uidVar IS NOT NULL THEN
      INSERT INTO TapeDriveDedication(id, tapeDrive, euid)
        VALUES(ids_seq.nextval, driveIdVar, uidVar)
      RETURNING id INTO dedicationIdVar;
      INSERT INTO Id2Type (id, type)
        VALUES (dedicationIdVar, 90);
    END IF;
    IF vidVar IS NOT NULL THEN
      INSERT INTO TapeDriveDedication(id, tapeDrive, vid)
        VALUES(ids_seq.nextval, driveIdVar, vidVar)
      RETURNING id INTO dedicationIdVar;
      INSERT INTO Id2Type (id, type)
        VALUES (dedicationIdVar, 90);
    END IF;

  END dedicateDrive;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE dedicateTape(vidVar IN VARCHAR2, driveNameVar IN VARCHAR2,
    serverNameVar IN  VARCHAR2, dgNameVar IN  VARCHAR2) AS
    driveIdVar           NUMBER;
    TYPE dedicationList_t IS TABLE OF NUMBER INDEX BY BINARY_INTEGER;
    dedicationsToDelete  dedicationList_t;
    dedicationIdVar      NUMBER;
  BEGIN
    -- If no tape drive is specified then remove all dedications for the
    -- specified tape and return
    IF driveNameVar IS NULL AND serverNameVar IS NULL AND dgNameVar IS NULL
      THEN
      -- Delete all existing dedications for the tape
      SELECT id BULK COLLECT INTO dedicationsToDelete
        FROM Tape2DriveDedication
        WHERE Tape2DriveDedication.vid = vidVar;
      IF dedicationsToDelete.COUNT > 0 THEN
        FOR i IN dedicationsToDelete.FIRST .. dedicationsToDelete.LAST LOOP
          DELETE FROM Tape2DriveDedication
            WHERE Tape2DriveDedication.id = dedicationsToDelete(i);
          DELETE FROM Id2Type
            WHERE Id2Type.id = dedicationsToDelete(i);
        END LOOP;
      END IF;

      RETURN;
    END IF;

    -- Get the id of the tape drive
    BEGIN
      SELECT
        TapeDrive.id
        INTO driveIdVar
        FROM TapeDrive
        INNER JOIN TapeServer ON
          TapeDrive.tapeServer = TapeServer.id
        INNER JOIN DeviceGroupName ON
          TapeDrive.deviceGroupName = DeviceGroupName.id
        WHERE
              TapeDrive.driveName    = driveNameVar
          AND TapeServer.serverName  = serverNameVar
          AND DeviceGroupName.dgName = dgNameVar;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      castorVdqmException.throw(castorVdqmException.drive_not_found_cd,
        'Tape drive ''' || dgNameVar || ' ' || driveNameVar || '@' ||
        serverNameVar || ''' not found in database.');
    END;

    -- Lock the tape drive row
    BEGIN
      SELECT
        TapeDrive.id
        INTO driveIdVar
        FROM TapeDrive
        WHERE
          TapeDrive.id = driveIdVar
        FOR UPDATE;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      castorVdqmException.throw(castorVdqmException.drive_not_found_cd,
        'Tape drive ''' || dgNameVar || ' ' || driveNameVar || '@' ||
        serverNameVar || ''' not found in database.');
    END;

    -- Delete all existing dedications for the tape
    SELECT id BULK COLLECT INTO dedicationsToDelete
      FROM Tape2DriveDedication
      WHERE Tape2DriveDedication.vid = vidVar;
    IF dedicationsToDelete.COUNT > 0 THEN
      FOR i IN dedicationsToDelete.FIRST .. dedicationsToDelete.LAST LOOP
        DELETE FROM Tape2DriveDedication
          WHERE Tape2DriveDedication.id = dedicationsToDelete(i);
        DELETE FROM Id2Type
          WHERE Id2Type.id = dedicationsToDelete(i);
      END LOOP;
    END IF;

    -- Insert new dedication
    INSERT INTO Tape2DriveDedication(id, vid, tapeDrive)
      VALUES(ids_seq.nextval, vidVar, driveIdVar)
      RETURNING id INTO dedicationIdVar;
    INSERT INTO Id2Type (id, type)
        VALUES (dedicationIdVar, 160);

  END dedicateTape;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE deleteDrive
  ( driveNameVar  IN  VARCHAR2
  , serverNameVar IN  VARCHAR2
  , dgNameVar     IN  VARCHAR2
  , resultVar     OUT INTEGER
  ) AS
    dgnIdVar        NUMBER;
    tapeServerIdVar NUMBER;
    driveIdVar      NUMBER;
    driveStatusVar  NUMBER;
  BEGIN
    resultVar := 0;

    BEGIN
      SELECT id INTO tapeServerIdVar FROM TapeServer
        WHERE
          serverName = serverNameVar;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      resultVar := -1; -- Tape server does not exist
      RETURN;
    END;

    BEGIN
      SELECT id INTO dgnIdVar FROM DeviceGroupName WHERE dgName = dgNameVar;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      resultVar := -2; -- DGN does not exits
      RETURN;
    END;

    BEGIN
      SELECT id, status INTO driveIdVar, driveStatusVar FROM TapeDrive
        WHERE
              deviceGroupName = dgnIdVar
          AND tapeServer = tapeServerIdVar
          AND driveName = driveNameVar
        FOR UPDATE;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      resultVar := -3; -- Tape drive does not exist
      RETURN;
    END;

    -- Not UNIT_UP and not UNIT_DOWN
    IF driveStatusVar != 0 AND driveStatusVar != 5 THEN
      resultVar := -4; -- Drive has a job assigned
      RETURN;
    END IF;

    DELETE FROM TapeDrive2TapeDriveComp WHERE parent    = driveIdVar;
    DELETE FROM TapeDriveDedication     WHERE tapeDrive = driveIdVar;
    DELETE FROM Tape2DriveDedication    WHERE tapeDrive = driveIdVar;
    DELETE FROM TapeDrive               WHERE id        = driveIdVar;
    DELETE FROM Id2Type                 WHERE id        = driveIdVar;

  END deleteDrive;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE writeRTPCDJobSubmission(
    tapeDriveIdVar    IN NUMBER,
    tapeRequestIdVar  IN NUMBER,
    returnVar        OUT NUMBER)
  AS
    tapeDriveStatusVar      NUMBER;
    runningTapeRequestIdVar NUMBER;
    tapeRequestStatusVar    NUMBER;
  BEGIN
    returnVar := 0; -- RTCPD job submission not written to database

    -- The status of the tape requests may be modified by threads handling tape
    -- request messages.  The status of the drives may be modified by threads
    -- handling drive request messages.  One such modification may be the
    -- bringing down of the drive in question, which would make it meaningless
    -- to record a successful RTPCD job submission.  Therefore get a lock on the
    -- corresponding request and drive rows and retrieve their statuses to see
    -- whether or not the success of the RTPCD job submission should be recorded
    -- in the database.
    SELECT TapeDrive.status, TapeDrive.runningTapeReq
    INTO tapeDriveStatusVar, runningTapeRequestIdVar
    FROM TapeDrive
    WHERE TapeDrive.id = tapeDriveIdVar
    FOR UPDATE;
    SELECT TapeRequest.status
    INTO tapeRequestStatusVar
    FROM TapeRequest
    WHERE TapeRequest.id = tapeRequestIdVar
    FOR UPDATE;

    -- If recording of successfull RTCPD job submission is permitted,
    -- i.e. the status of the drive is UNIT_STARTING and the drive is still
    -- paired with the tape request and the status of the tape request is
    -- REQUEST_BEINGSUBMITTED
    IF (tapeDriveStatusVar = 1) AND (runningTapeRequestIdVar = tapeRequestIdVar)
      AND (tapeRequestStatusVar = 2) THEN

      -- Set the state of the tape request to REQUEST_SUBMITTED (3)
      UPDATE TapeRequest
      SET TapeRequest.status = 3
      WHERE TapeRequest.id = tapeRequestIdVar;

      returnVar := 1; -- RTCPD job submission written to database
    END IF;

  EXCEPTION

    -- Do nothing if either the drive or request no longer exist in the database
    WHEN NO_DATA_FOUND THEN NULL;

  END writeRTPCDJobSubmission;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE writeFailedRTPCDJobSubmission(
    tapeDriveIdVar    IN NUMBER,
    tapeRequestIdVar  IN NUMBER,
    returnVar        OUT NUMBER)
  AS
    tapeDriveStatusVar      NUMBER;
    runningTapeRequestIdVar NUMBER;
    tapeRequestStatusVar    NUMBER;
  BEGIN
    returnVar := 0; -- Failed RTCPD job submission not written to database

    -- The status of the tape requests may be modified by threads handling tape
    -- request messages.  The status of the drives may be modified by threads
    -- handling drive request messages.  One such modification may be the
    -- bringing down of the drive in question, which would make it meaningless
    -- to record a successful RTPCD job submission.  Therefore get a lock on the
    -- corresponding request and drive rows and retrieve their statuses to see
    -- whether or not the success of the RTPCD job submission should be recorded
    -- in the database.
    SELECT TapeDrive.status, TapeDrive.runningTapeReq
    INTO tapeDriveStatusVar, runningTapeRequestIdVar
    FROM TapeDrive
    WHERE TapeDrive.id = tapeDriveIdVar
    FOR UPDATE;
    SELECT TapeRequest.status
    INTO tapeRequestStatusVar
    FROM TapeRequest
    WHERE TapeRequest.id = tapeRequestIdVar
    FOR UPDATE;

    -- If recording of failed RTCPD job submission is permitted,
    -- i.e. the status of the drive is UNIT_STARTING and the drive is still
    -- paired with the tape request and the status of the tape request is
    -- REQUEST_BEINGSUBMITTED
    IF (tapeDriveStatusVar = 1) AND (runningTapeRequestIdVar = tapeRequestIdVar)
      AND (tapeRequestStatusVar = 2) THEN

      -- Unlink the tape drive from the tape request
      UPDATE TapeDrive
      SET TapeDrive.runningTapeReq = NULL
      WHERE TapeDrive.id = tapeDriveIdVar;
      UPDATE TapeRequest
      SET TapeRequest.tapeDrive = NULL
      WHERE TapeRequest.id = tapeRequestIdVar;

      -- set the state of the tape drive to STATUS_UNKNOWN (7)
      UPDATE TapeDrive
      SET TapeDrive.status = 7
      WHERE TapeDrive.id = tapeDriveIdVar;

      -- Set the state of the tape request to REQUEST_PENDING (0)
      UPDATE TapeRequest
      SET TapeRequest.status = 0
      WHERE TapeRequest.id = tapeRequestIdVar;

      returnVar := 1; -- Failed RTCPD job submission written to database
    END IF;

  EXCEPTION

    -- Do nothing if either the drive or request no longer exist in the database
    WHEN NO_DATA_FOUND THEN NULL;

  END writeFailedRTPCDJobSubmission;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE setVolPriority(
    priorityVar     IN NUMBER,
    clientUIDVar    IN NUMBER,
    clientGIDVar    IN NUMBER,
    clientHostVar   IN VARCHAR2,
    vidVar          IN VARCHAR2,
    tpModeVar       IN NUMBER,
    lifespanTypeVar IN NUMBER)
  AS
    priorityIdVar NUMBER;
  BEGIN
    -- Try to select and get a lock on the row representing the priority if one
    -- already exists
    BEGIN
      SELECT id INTO priorityIdVar FROM VolumePriority WHERE
            VolumePriority.vid          = vidVar
        AND VolumePriority.tpMode       = tpModeVar
        AND VolumePriority.lifespanType = lifespanTypeVar
        FOR UPDATE;
    EXCEPTION
      WHEN NO_DATA_FOUND THEN
        priorityIdVar := NULL;
    END;

    -- If a row for the priority already exists
    IF priorityIdVar IS NOT NULL THEN

      -- Update it if it has an equal or lower priority
      UPDATE VolumePriority SET
        VolumePriority.priority   = priorityVar,
        VolumePriority.clientUID  = clientUIDVar,
        VolumePriority.clientGID  = clientGIDVar,
        VolumePriority.clientHost = clientHostVar
      WHERE
            VolumePriority.id = priorityIdVar
        AND VolumePriority.priority <= priorityVar;

    -- Else a row for the priority does not yet exist
    ELSE

      -- Try to create one
      BEGIN
        INSERT INTO VolumePriority(id, priority, clientUID, clientGID,
          clientHost, vid, tpMode, lifespanType)
        VALUES(ids_seq.nextval, priorityVar, clientUIDVar, clientGIDVar,
          clientHostVar, vidVar, tpModeVar, lifespanTypeVar)
        RETURNING id INTO priorityIdVar;
      EXCEPTION
        WHEN OTHERS THEN
          -- If another competing thread created the row first
          IF SQLCODE = -1 THEN -- ORA-00001: unique constraint violated

            -- Ensure priorityIdVar is NULL
            priorityIdVar := NULL;

          -- Else an unforeseen error has occured
          ELSE

            -- Reraise the current exception
            RAISE;

          END IF;
      END;

      -- If a row for the priority was successfully created then
      IF priorityIdVar IS NOT NULL THEN

        -- Update Id2Type
        INSERT INTO Id2Type (id, type)
        VALUES (priorityIdVar, 151); -- 151=OBJ_VolumePriority

      -- Else a competing thread created the row first
      ELSE

        -- Select and get a lock on the row
        SELECT id INTO priorityIdVar FROM VolumePriority WHERE
            VolumePriority.vid          = vidVar
        AND VolumePriority.tpMode       = tpModeVar
        AND VolumePriority.lifespanType = lifespanTypeVar
        FOR UPDATE;

        -- Update the row if it has an equal or lower priority
        UPDATE VolumePriority SET
          priority   = priorityVar,
          clientUID  = clientUIDVar,
          clientGID  = clientGIDVar,
          clientHost = clientHostVar
        WHERE
              VolumePriority.id = priorityIdVar
          AND VolumePriority.priority <= priorityVar;
      END IF;

    END IF; -- If a row for the priority already exists

  END setVolPriority;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE deleteVolPriority(
    vidVar           IN VARCHAR2,
    tpModeVar        IN NUMBER,
    lifespanTypeVar  IN NUMBER,
    returnVar       OUT NUMBER,
    priorityVar     OUT NUMBER,
    clientUIDVar    OUT NUMBER,
    clientGIDVar    OUT NUMBER,
    clientHostVar   OUT NOCOPY VARCHAR2)
  AS
    priorityIdVar NUMBER;
  BEGIN
    returnVar := 0;

    -- Try to get the ID of the volume priority row
    BEGIN
      SELECT id, priority, clientUID, clientGID, clientHost
        INTO priorityIdVar, priorityVar, clientUIDVar, clientGIDVar,
          clientHostVar
        FROM VolumePriority WHERE
            VolumePriority.vid          = vidVar
        AND VolumePriority.tpMode       = tpModeVar
        AND VolumePriority.lifespanType = lifespanTypeVar
        FOR UPDATE;
    EXCEPTION
      -- Do nothing if the specified volume priority row does not exist
      WHEN NO_DATA_FOUND THEN RETURN;
    END;

    -- Delete the volume priority row
    DELETE FROM VolumePriority WHERE VolumePriority.id = priorityIdVar;
    DELETE FROM Id2Type        WHERE Id2Type.id        = priorityIdVar;

    -- Give the ID of the volume priority row that was deleted
    returnVar := priorityIdVar;
  END deleteVolPriority;


  /**
   * See the castorVdqm package specification for documentation.
   */
  PROCEDURE deleteOldVolPriorities(
    maxAgeVar             IN NUMBER,
    prioritiesDeletedVar OUT NUMBER)
  AS
    nowVar NUMBER;
  BEGIN
    prioritiesDeletedVar := 0;

    nowVar := castorVdqmCommon.getTime();

    DELETE FROM VolumePriority
    WHERE
          VolumePriority.lifespanType = 0 -- Single-mount
      AND (nowVar - VolumePriority.modificationTime) > maxAgeVar;

    prioritiesDeletedVar := SQL%ROWCOUNT;
  END deleteOldVolPriorities;

END castorVdqm;


/**
 * This package provides the API to be used by the VDQM server application.
 */
CREATE OR REPLACE PACKAGE castorVdqm AS

  /**
   * This procedure tries to allocate a free tape drive to a pending tape
   * request.
   *
   * @param returnVar has a value of 1 if a free drive was successfully
   * allocated to a pending request, 0 if no possible allocation could be found
   * or -1 if an allocation was found but was invalidated by other threads
   * before the appropriate locks could be taken.
   * @param tapeDriveIdVar if a free drive was successfully allocated then the
   * value of this parameter will be the ID of the allocated tape drive, else
   * the value of this parameter will be undefined.
   * @param tapeDriveIdVar if a free drive was successfully allocated then the
   * value of this parameter will be the name of the allocated tape drive, else
   * the value of this parameter will be undefined.
   * @param tapeRequestIdVar if a free drive was successfully allocated then the
   * value of this parameter will be the ID of the pending request, else the
   * value of this parameter will be undefined.
   * @param tapeRequestVidVar if a free drive was successfully allocated then
   * the value of this parameter will be the VID of the pending request, else
   * the value of this parameter will be undefined.
   */
  PROCEDURE allocateDrive(returnVar OUT NUMBER, tapeDriveIdVar OUT NUMBER,
    tapeDriveNameVar  OUT NOCOPY VARCHAR2, tapeRequestIdVar  OUT NUMBER,
    tapeRequestVidVar OUT NOCOPY VARCHAR2);

  /**
   * This procedure gets a new matched request to be submitted to rtcpd.
   *
   * @tapeReqId the ID of the matched request.
   */
  PROCEDURE getRequestToSubmit(tapeReqId OUT NUMBER);

  /**
   * This procedure tries to reuse a drive allocation.
   *
   * @param tapeIdVar the ID of the tape of the current drive allocation.
   * @param tapeDriveIdVar the ID of the drive of the current drive allocation.
   * @param returnVar has a value of 1 if the specified drive allocation was
   * successfully reused, 0 if no possible reuse was found or -1 if a possible
   * reuse was found but was invalidated by other threads before the appropriate
   * locks could be taken.
   * @param tapeRequestIdVar if the drive allocation was successfully reused
   * then the value of this parameter will be the ID of the newly assigned
   * request, else the value of this parameter will be undefined.
   */
  PROCEDURE reuseDriveAllocation(tapeIdVar IN NUMBER, tapeDriveIdVar IN NUMBER,
    returnVar OUT NUMBER, tapeRequestIdVar  OUT NUMBER);

  /**
   * This procedure inserts the specified drive dedications into the database.
   *
   * @param driveNameVar the name of the tape drive to be dedicated
   * @param serverNameVar the name of the tape server of the tape drive
   * @param dgNameVar the name of the device group of the tape drive
   * @param dedicateVar the dedication string
   * @exception castorVdqmException.invalid_drive_dedicate if an error is
   * detected in the drive dedicate string.
   * @exception castorVdqmException.drive_not_found_application if the
   * specified tape drive cannot be found in the database.
   * @exception castorVdqmException.drive_server_not_found if the specified
   * tape drive and tape server combination cannot be found in the database.
   * @exception castorVdqmException.drive_dgn_not_found if the specified tape
   * drive and DGN combination cannot be found in the database.
   */
  PROCEDURE dedicateDrive(driveNameVar IN VARCHAR2, serverNameVar IN VARCHAR2,
    dgNameVar IN VARCHAR2, dedicateVar IN VARCHAR2);

  /**
   * This procedure inserts the specified tape dedication into the database.
   * If a tape drive is not specified, i.e. driveNAmeVar, serverNAmeVar and
   * dgNameVar are set to NULL, then all dedications for the specified tape
   * will be removed from the database.
   *
   * @param driveNameVar the name of the tape drive
   * @param serverNameVar the name of the tape server of the tape drive
   * @param dgNameVar the name of the device group of the tape drive
   * @param vidVar the VID of the tape to be dedicated
   * @exception castorVdqmException.invalid_drive_dedicate if an error is
   * detected in the drive dedicate string.
   * @exception castorVdqmException.drive_not_found_application if the
   * specified tape drive cannot be found in the database.
   * @exception castorVdqmException.drive_server_not_found if the specified
   * tape drive and tape server combination cannot be found in the database.
   * @exception castorVdqmException.drive_dgn_not_found if the specified tape
   * drive and DGN combination cannot be found in the database.
   */
  PROCEDURE dedicateTape(vidVar IN VARCHAR2, driveNameVar IN VARCHAR2,
    serverNameVar IN VARCHAR2, dgNameVar IN VARCHAR2);

  /**
   * This procedure deletes the specified drive from the database.
   *
   * @param driveNameVar the name of the tape drive to be delete
   * @param serverNameVar the name of the tape server of the tape drive
   * @param dgNameVar the name of the device group of the tape drive
   * @param resultVar is 0 if the deletion was successful. -1 if the specified
   * tape server does not exist, -2 if the specified device group name does not
   * exist, -3 if the specified tape drive does not exist and -4 if the
   * specified drive has a job assigned.
   */
  PROCEDURE deleteDrive(driveNameVar IN VARCHAR2, serverNameVar IN VARCHAR2,
    dgNameVar IN VARCHAR2, resultVar OUT INTEGER);

  /**
   * This procedure tries to write to the database the fact that a successful
   * RTCPD job submission has occured.  This update of the database may not be
   * possible if the corresponding drive and tape request states have been
   * modified by other threads.  For example a thread handling a tape drive
   * request message may have put the drive into the down state.  The RTCPD job
   * submission should be ignored in this case.
   *
   * @param tapeDriveIdVar the ID of the drive
   * @param tapeRequestIdVar the ID of the tape request
   * @param returnVar has a value of 1 if the occurance of the RTCPD job
   * submission was successfully written to the database, else 0.
   */
  PROCEDURE writeRTPCDJobSubmission(tapeDriveIdVar IN NUMBER,
    tapeRequestIdVar IN NUMBER, returnVar OUT NUMBER);

  /**
   * This procedure tries to write to the database the fact that a failed RTCPD
   * job submission has occured.  This update of the database may not be
   * possible if the corresponding drive and tape request states have been
   * modified by other threads.  For example a thread handling a tape drive
   * request message may have put the drive into the down state.  The failed
   * RTCPD job submission should be ignored in this case.
   *
   * @param tapeDriveIdVar the ID of the drive
   * @param tapeRequestIdVar the ID of the tape request
   * @param returnVar has a value of 1 if the occurance of the failed RTCPD job
   * submission was successfully written to the database, else 0.
   */
  PROCEDURE writeFailedRTPCDJobSubmission(tapeDriveIdVar IN NUMBER,
    tapeRequestIdVar IN NUMBER, returnVar OUT NUMBER);

  /**
   * This procedure sets the priority of a volume.
   *
   * @param priority the priority where 0 is the lowest priority and INT_MAX is
   * the highest.
   * @param clientUIDVar the user id of the client.
   * @param clientGIDVar the group id fo the client.
   * @param clientHostVar the host of the client.
   * @param vidVar the visual identifier of the volume.
   * @param tpModeVar the tape access mode.  Valid values are either 0 meaning
   * write-disabled or 1 meaning write-enabled.
   * @param lifespanTypeVar the type of lifespan to be assigned to the priority
   * setting.  Valid values are either 0 meaning single-shot or 1 meaning
   * unlimited.
   */
  PROCEDURE setVolPriority(priorityVar IN NUMBER, clientUIDVar IN NUMBER,
    clientGIDVar IN NUMBER, clientHostVar IN VARCHAR2, vidVar IN VARCHAR2,
    tpModeVar IN NUMBER, lifespanTypeVar IN NUMBER);

  /**
   * This procedure deletes the specified volume priority if it exist, else
   * does nothing.
   *
   * @param vidVar the visual identifier of the volume.
   * @param tpModeVar the tape access mode.  Valid values are either 0 meaning
   * write-disabled or 1 meaning write-enabled.
   * @param lifespanTypeVar the type of lifespan to be assigned to the priority
   * setting.  Valid values are either 0 meaning single-shot or 1 meaning
   * unlimited.
   * @param returnVar the ID id of the volume priority row if one was deleted,
   * else 0.
   * @param priorityVar the priority of the deleted volume priority if one was
   * deleted, else undefined.
   * @param clientUIDVar the client UID of the deleted volume priority if one
   * was deleted, else undefined.
   * @param clientGIDVar the client GID of the deleted volume priority if one
   * was deleted, else undefined.
   * @param clientHostVar the client host of the deleted volume priority if one
   * was deleted, else undefined.
   */
  PROCEDURE deleteVolPriority(vidVar IN VARCHAR2, tpModeVar IN NUMBER,
    lifespanTypeVar IN NUMBER, returnVar OUT NUMBER, priorityVar OUT NUMBER,
    clientUIDVar OUT NUMBER, clientGIDVar OUT NUMBER,
    clientHostVar OUT NOCOPY VARCHAR2);

  /**
   * This procedure deletes old single-mount volume priorities.
   *
   * @param maxAgeVar the maximum age of a volume priority in seconds.
   * @param prioritiesDeletedVar the number of volume priorities deleted.
   */
  PROCEDURE deleteOldVolPriorities(maxAgeVar IN NUMBER,
    prioritiesDeletedVar OUT NUMBER);

END castorVdqm;


/**
 * Converts an updated tape drive id of 0 to NULL.
 */
CREATE OR REPLACE TRIGGER TR_I_Tape2DriveDedication
  BEFORE INSERT ON TapeRequest
FOR EACH ROW
BEGIN
  -- Convert a tape drive id of 0 to NULL
  IF :NEW.tapeDrive = 0 THEN
    :NEW.tapeDrive := NULL;
  END IF;
END;


/**
 * Converts an updated tape drive id of 0 to NULL.
 */
CREATE OR REPLACE TRIGGER TR_U_Tape2DriveDedication
  BEFORE UPDATE ON Tape2DriveDedication
FOR EACH ROW
BEGIN
  -- Convert a tape drive id of 0 to NULL
  IF :NEW.tapeDrive = 0 THEN
    :NEW.tapeDrive := NULL;
  END IF;
END;
