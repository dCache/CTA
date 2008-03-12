/*******************************************************************
 *
 * @(#)$RCSfile: oracleTrailer.sql,v $ $Revision: 1.35 $ $Release$ $Date: 2008/03/12 21:39:36 $ $Author: murrayc3 $
 *
 * This file contains SQL code that is not generated automatically
 * and is inserted at the end of the generated code
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* SQL statements for object types */
CREATE TABLE Id2Type (
  id   INTEGER,
  type NUMBER,
  CONSTRAINT PK_Id2Type PRIMARY KEY (id));
CREATE INDEX I_Id2Type_typeId on Id2Type (type, id);

/* Enumerations */
CREATE TABLE TapeServerStatusCodes (
  id   NUMBER,
  name VARCHAR2(30),
  CONSTRAINT PK_TapeServerStatusCodes PRIMARY KEY (id));
INSERT INTO TapeServerStatusCodes VALUES (0, 'TAPESERVER_ACTIVE');
INSERT INTO TapeServerStatusCodes VALUES (1, 'TAPESERVER_INACTIVE');
COMMIT;

CREATE TABLE TapeDriveStatusCodes (
  id   NUMBER,
  name VARCHAR2(30),
  CONSTRAINT PK_TapeDriveStatusCodes PRIMARY KEY (id));
INSERT INTO TapeDriveStatusCodes VALUES (0, 'UNIT_UP');
INSERT INTO TapeDriveStatusCodes VALUES (1, 'UNIT_STARTING');
INSERT INTO TapeDriveStatusCodes VALUES (2, 'UNIT_ASSIGNED');
INSERT INTO TapeDriveStatusCodes VALUES (3, 'VOL_MOUNTED');
INSERT INTO TapeDriveStatusCodes VALUES (4, 'FORCED_UNMOUNT');
INSERT INTO TapeDriveStatusCodes VALUES (5, 'UNIT_DOWN');
INSERT INTO TapeDriveStatusCodes VALUES (6, 'WAIT_FOR_UNMOUNT');
INSERT INTO TapeDriveStatusCodes VALUES (7, 'STATUS_UNKNOWN');
COMMIT;

CREATE TABLE TapeStatusCodes (
  id   NUMBER,
  name VARCHAR2(30),
  CONSTRAINT PK_TapeStatusCodes PRIMARY KEY (id));
INSERT INTO TapeStatusCodes VALUES (0, 'TAPE_USED');
INSERT INTO TapeStatusCodes VALUES (1, 'TAPE_PENDING');
INSERT INTO TapeStatusCodes VALUES (2, 'TAPE_WAITDRIVE');
INSERT INTO TapeStatusCodes VALUES (3, 'TAPE_WAITMOUNT');
INSERT INTO TapeStatusCodes VALUES (4, 'TAPE_MOUNTED');
INSERT INTO TapeStatusCodes VALUES (5, 'TAPE_FINISHED');
INSERT INTO TapeStatusCodes VALUES (6, 'TAPE_FAILED');
INSERT INTO TapeStatusCodes VALUES (7, 'TAPE_UNKNOWN');
COMMIT;

CREATE TABLE TapeRequestStatusCodes (
  id NUMBER,
  name VARCHAR2(30),
  CONSTRAINT PK_TapeRequestStatusCodes PRIMARY KEY (id));
INSERT INTO TapeRequestStatusCodes VALUES (0, 'REQUEST_PENDING');
INSERT INTO TapeRequestStatusCodes VALUES (1, 'REQUEST_MATCHED');
INSERT INTO TapeRequestStatusCodes VALUES (2, 'REQUEST_BEINGSUBMITTED');
INSERT INTO TapeRequestStatusCodes VALUES (3, 'REQUEST_SUBMITTED');
INSERT INTO TapeRequestStatusCodes VALUES (4, 'REQUEST_FAILED');
COMMIT;

/* Not null column constraints */
ALTER TABLE CLIENTIDENTIFICATION MODIFY (EGID NOT NULL);
ALTER TABLE CLIENTIDENTIFICATION MODIFY (EUID NOT NULL);
ALTER TABLE CLIENTIDENTIFICATION MODIFY (MAGIC NOT NULL);
ALTER TABLE CLIENTIDENTIFICATION MODIFY (PORT NOT NULL);
ALTER TABLE ERRORHISTORY MODIFY (TAPE NOT NULL);
ALTER TABLE ERRORHISTORY MODIFY (TAPEDRIVE NOT NULL);
ALTER TABLE ERRORHISTORY MODIFY (TIMESTAMP NOT NULL);
ALTER TABLE ID2TYPE MODIFY (TYPE NOT NULL);
ALTER TABLE TAPEACCESSSPECIFICATION MODIFY (ACCESSMODE NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (DEVICEGROUPNAME NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (ERRCOUNT NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (JOBID NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (MODIFICATIONTIME NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (RESETTIME NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (RUNNINGTAPEREQ NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (STATUS NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (TAPE NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (TAPEACCESSMODE NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (TAPESERVER NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (TOTALMB NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (TRANSFERREDMB NOT NULL);
ALTER TABLE TAPEDRIVE MODIFY (USECOUNT NOT NULL);
ALTER TABLE TAPEDRIVE2TAPEDRIVECOMP MODIFY (CHILD NOT NULL);
ALTER TABLE TAPEDRIVE2TAPEDRIVECOMP MODIFY (PARENT NOT NULL);
ALTER TABLE TAPEDRIVECOMPATIBILITY MODIFY (PRIORITYLEVEL NOT NULL);
ALTER TABLE TAPEDRIVECOMPATIBILITY MODIFY (TAPEACCESSSPECIFICATION NOT NULL);
ALTER TABLE TAPEDRIVEDEDICATION MODIFY (ACCESSMODE NOT NULL);
ALTER TABLE TAPEDRIVEDEDICATION MODIFY (EGID NOT NULL);
ALTER TABLE TAPEDRIVEDEDICATION MODIFY (ENDTIME NOT NULL);
ALTER TABLE TAPEDRIVEDEDICATION MODIFY (EUID NOT NULL);
ALTER TABLE TAPEDRIVEDEDICATION MODIFY (STARTTIME NOT NULL);
ALTER TABLE TAPEDRIVEDEDICATION MODIFY (TAPEDRIVE NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (CLIENT NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (CREATIONTIME NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (DEVICEGROUPNAME NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (ERRORCODE NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (MODIFICATIONTIME NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (PRIORITY NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (REQUESTEDSRV NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (STATUS NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (TAPE NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (TAPEACCESSSPECIFICATION NOT NULL);
ALTER TABLE TAPEREQUEST MODIFY (TAPEDRIVE NOT NULL);
ALTER TABLE TAPESERVER MODIFY (ACTINGMODE NOT NULL);
ALTER TABLE VDQMTAPE MODIFY (ERRORCODE NOT NULL);
ALTER TABLE VDQMTAPE MODIFY (SEVERITY NOT NULL);
ALTER TABLE VDQMTAPE MODIFY (SIDE NOT NULL);
ALTER TABLE VDQMTAPE MODIFY (STATUS NOT NULL);
ALTER TABLE VDQMTAPE MODIFY (TPMODE NOT NULL);

/* Foreign key constraints with an index for each*/
ALTER TABLE ClientIdentification
  ADD CONSTRAINT FK_ClientIdentification_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;

ALTER TABLE DeviceGroupName
  ADD CONSTRAINT FK_DeviceGroupName_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;

ALTER TABLE ErrorHistory
  ADD CONSTRAINT FK_ErrorHistory_tapeDrive
    FOREIGN KEY (tapeDrive)
    REFERENCES TapeDrive (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_ErrorHistory_tape
    FOREIGN KEY (tape)
    REFERENCES VdqmTape (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE;
CREATE INDEX I_FK_ErrorHistory_tape ON ErrorHistory (tape);

ALTER TABLE TapeAccessSpecification
  ADD CONSTRAINT FK_TapeAccessSpecification_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;

ALTER TABLE TapeDrive
  ADD CONSTRAINT FK_TapeDrive_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeDrive_tape
    FOREIGN KEY (tape)
    REFERENCES VdqmTape (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_TapeDrive_runningTapeReq
    FOREIGN KEY (runningTapeReq)
    REFERENCES TapeRequest (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_TapeDrive_deviceGroupName
    FOREIGN KEY (deviceGroupName)
    REFERENCES DeviceGroupName (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_TapeDrive_status
    FOREIGN KEY (status)
    REFERENCES TapeDriveStatusCodes (id)
    DEFERRABLE
    INITIALLY IMMEDIATE
    ENABLE
  ADD CONSTRAINT FK_TapeDrive_tapeServer
    FOREIGN KEY (tapeServer)
    REFERENCES TapeServer (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE;
CREATE INDEX I_FK_TapeDrive_tape            ON TapeDrive (tape);
CREATE INDEX I_FK_TapeDrive_runningTapeReq  ON TapeDrive (runningTapeReq);
CREATE INDEX I_FK_TapeDrive_deviceGroupName ON TapeDrive (deviceGroupName);
CREATE INDEX I_FK_TapeDrive_status          ON TapeDrive (status);
CREATE INDEX I_FK_TapeDrive_tapeServer      ON TapeDrive (tapeServer);

ALTER TABLE TapeDriveCompatibility
  ADD CONSTRAINT FK_TapeDriveCompatibility_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeDriveComp_accessSpec
    FOREIGN KEY (tapeAccessSpecification)
    REFERENCES TapeAccessSpecification (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;
CREATE INDEX I_FK_TapeDriveComp_accessSpec ON TapeDriveCompatibility (tapeAccessSpecification);

ALTER TABLE TapeDriveDedication
  ADD CONSTRAINT FK_TapeDriveDedication_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeDriveDedic_tapeDrive
    FOREIGN KEY (tapeDrive)
    REFERENCES TapeDrive (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;
CREATE INDEX I_FK_TapeDriveDedic_tapeDrive ON TapeDriveDedication (tapeDrive);

ALTER TABLE TapeRequest
  ADD CONSTRAINT FK_TapeRequest_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeRequest_tape
    FOREIGN KEY (tape)
    REFERENCES VdqmTape (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeRequest_accessSpec
    FOREIGN KEY (tapeAccessSpecification)
    REFERENCES TapeAccessSpecification (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeRequest_requestedSrv
    FOREIGN KEY (requestedSrv)
    REFERENCES TapeServer (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_TapeRequest_tapeDrive
    FOREIGN KEY (tapeDrive)
    REFERENCES TapeDrive (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_TapeRequest_dgn
    FOREIGN KEY (deviceGroupName)
    REFERENCES DeviceGroupName (id)
    DEFERRABLE
    INITIALLY DEFERRED
    DISABLE
  ADD CONSTRAINT FK_TapeRequest_status
    FOREIGN KEY (status)
    REFERENCES TapeRequestStatusCodes (id)
    DEFERRABLE
    INITIALLY IMMEDIATE
    ENABLE
  ADD CONSTRAINT FK_TapeRequest_client
    FOREIGN KEY (client)
    REFERENCES ClientIdentification (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE;
CREATE INDEX I_FK_TapeRequest_tape         ON TapeRequest (tape);
CREATE INDEX I_FK_TapeRequest_accessSpec   ON TapeRequest (tapeAccessSpecification);
CREATE INDEX I_FK_TapeRequest_requestedSrv ON TapeRequest (requestedSrv);
CREATE INDEX I_FK_TapeRequest_tapeDrive    ON TapeRequest (tapeDrive);
CREATE INDEX I_FK_TapeRequest_dgn          ON TapeRequest (deviceGroupName);
CREATE INDEX I_FK_TapeRequest_status       ON TapeRequest (status);
CREATE INDEX I_FK_TapeRequest_client       ON TapeRequest (client);

ALTER TABLE TapeServer
  ADD CONSTRAINT FK_TapeServer_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_TapeServer_actingMode
    FOREIGN KEY (actingMode)
    REFERENCES TapeServerStatusCodes (id)
    DEFERRABLE
    INITIALLY IMMEDIATE
    ENABLE;
CREATE INDEX I_FK_TapeServer_actingMode ON TapeServer (actingMode);

ALTER TABLE VdqmTape
  ADD CONSTRAINT FK_VdqmTape_id
    FOREIGN KEY (id)
    REFERENCES Id2Type (id)
    DEFERRABLE
    INITIALLY DEFERRED
    ENABLE
  ADD CONSTRAINT FK_VdqmTape_status
    FOREIGN KEY (status)
    REFERENCES TapeStatusCodes (id)
    DEFERRABLE
    INITIALLY IMMEDIATE
    ENABLE;
CREATE INDEX I_FK_VdqmTape_status ON VdqmTape (status);

/* A small table used to cross check code and DB versions */
DECLARE
  nbTables NUMBER;
BEGIN
  SELECT COUNT(*) INTO nbTables FROM USER_TABLES
    WHERE TABLE_NAME='CASTORVERSION';

  IF nbTables = 0 THEN
    EXECUTE IMMEDIATE
      'CREATE TABLE CastorVersion' ||
      '  (schemaVersion VARCHAR2(20), release VARCHAR2(20))';
    EXECUTE IMMEDIATE
      'INSERT INTO CastorVersion VALUES (''-'', ''-'')';
    COMMIT;
  END IF;
END;
UPDATE CastorVersion SET schemaVersion = '2_1_6_0';

/* Sequence for indices */
CREATE SEQUENCE ids_seq CACHE 200;

/* get current time as a time_t. Not that easy in ORACLE */
CREATE OR REPLACE FUNCTION getTime RETURN NUMBER IS
  ret NUMBER;
BEGIN
  SELECT (SYSDATE - to_date('01-jan-1970 01:00:00','dd-mon-yyyy HH:MI:SS')) * (24*60*60) INTO ret FROM DUAL;
  RETURN ret;
END;

/* PL/SQL code for castorVdqm package */
CREATE OR REPLACE PACKAGE castorVdqm AS
  TYPE Drive2Req IS RECORD (
        tapeDrive NUMBER,
        tapeRequest NUMBER);
  TYPE Drive2Req_Cur IS REF CURSOR RETURN Drive2Req;
	TYPE TapeDrive_Cur IS REF CURSOR RETURN TapeDrive%ROWTYPE;
	TYPE TapeRequest_Cur IS REF CURSOR RETURN TapeRequest%ROWTYPE;
END castorVdqm;


/**
 * This view shows candidate "free tape drive to pending tape request"
 * allocations.
 */
CREATE OR REPLACE VIEW CANDIDATEDRIVEALLOCATIONS_VIEW
AS SELECT
  TapeDrive.id as tapeDriveID, TapeRequest.id as tapeRequestID
FROM
  TapeRequest
INNER JOIN
  TapeDrive
ON
  TapeRequest.deviceGroupName = TapeDrive.deviceGroupName
INNER JOIN
  TapeServer
ON
     TapeRequest.requestedSrv = TapeServer.id
  OR TapeRequest.requestedSrv = 0
WHERE
      TapeDrive.status=0 -- UNIT_UP
  AND TapeDrive.runningTapeReq=0
  -- Exclude a request if its tape is associated with an on-going request
  AND NOT EXISTS (
    SELECT
      'x'
    FROM
      TapeRequest TapeRequest2
    WHERE
      TapeRequest2.tapeDrive != 0
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
  AND TapeServer.actingMode=0 -- ACTIVE
  AND TapeRequest.tapeDrive=0
ORDER BY
  TapeRequest.modificationTime ASC;


/**
 * View used for generating the list of requests when replying to the
 * showqueues command
 */
create or replace view
  TAPEREQUESTSSHOWQUEUES_VIEW
as select
  TAPEREQUEST.ID,
  TAPEDRIVE.DRIVENAME,
  TAPEDRIVE.ID as TAPEDRIVEID,
  TAPEREQUEST.PRIORITY,
  CLIENTIDENTIFICATION.PORT as CLIENTPORT,
  CLIENTIDENTIFICATION.EUID as CLIENTEUID,
  CLIENTIDENTIFICATION.EGID as CLIENTEGID,
  TAPEACCESSSPECIFICATION.ACCESSMODE,
  TAPEREQUEST.MODIFICATIONTIME,
  CLIENTIDENTIFICATION.MACHINE AS CLIENTMACHINE,
  VDQMTAPE.VID,
  TAPESERVER.SERVERNAME as TAPESERVER,
  DEVICEGROUPNAME.DGNAME,
  CLIENTIDENTIFICATION.USERNAME as CLIENTUSERNAME
from
  TAPEREQUEST
left outer join
  TAPEDRIVE
on
  TAPEREQUEST.TAPEDRIVE = TAPEDRIVE.ID
left outer join
  CLIENTIDENTIFICATION
on
  TAPEREQUEST.CLIENT = CLIENTIDENTIFICATION.ID
inner join
  TAPEACCESSSPECIFICATION
on
  TAPEREQUEST.TAPEACCESSSPECIFICATION = TAPEACCESSSPECIFICATION.ID
left outer join
  VDQMTAPE
on
  TAPEREQUEST.TAPE = VDQMTAPE.ID
left outer join
  TAPESERVER
on
  TAPEREQUEST.REQUESTEDSRV = TAPESERVER.ID
left outer join
  DEVICEGROUPNAME
on
  TAPEREQUEST.DEVICEGROUPNAME = DEVICEGROUPNAME.ID;


/**
 * View used for generating the list of drives when replying to the showqueues
 * command
 */
create or replace view
  TAPEDRIVESHOWQUEUES_VIEW
as with
  TAPEDRIVE2MODEL
as
(
  select
    TAPEDRIVE2TAPEDRIVECOMP.PARENT as TAPEDRIVE,
    max(TAPEDRIVECOMPATIBILITY.TAPEDRIVEMODEL) as DRIVEMODEL
  from
    TAPEDRIVE2TAPEDRIVECOMP
  inner join
    TAPEDRIVECOMPATIBILITY
  on
    TAPEDRIVE2TAPEDRIVECOMP.CHILD = TAPEDRIVECOMPATIBILITY.ID
  group by
    parent
)
select
  TAPEDRIVE.STATUS, TAPEDRIVE.ID, TAPEDRIVE.RUNNINGTAPEREQ, TAPEDRIVE.JOBID,
  TAPEDRIVE.MODIFICATIONTIME, TAPEDRIVE.RESETTIME, TAPEDRIVE.USECOUNT,
  TAPEDRIVE.ERRCOUNT, TAPEDRIVE.TRANSFERREDMB, TAPEDRIVE.TAPEACCESSMODE,
  TAPEDRIVE.TOTALMB, TAPESERVER.SERVERNAME, VDQMTAPE.VID, TAPEDRIVE.DRIVENAME,
  DEVICEGROUPNAME.DGNAME, TAPEDRIVE2MODEL.DRIVEMODEL
from
  TAPEDRIVE
left outer join
  TAPESERVER
on
  TAPEDRIVE.TAPESERVER = TAPESERVER.ID
left outer join
  VDQMTAPE
on
  TAPEDRIVE.TAPE = VDQMTAPE.ID
left outer join
  DEVICEGROUPNAME
on
  TAPEDRIVE.DEVICEGROUPNAME = DEVICEGROUPNAME.ID
inner join
  TAPEDRIVE2MODEL
on
  TAPEDRIVE.ID = TAPEDRIVE2MODEL.TAPEDRIVE;


/**
 * COMMENTED OUT
 * PL/SQL method to dedicate a tape to a tape drive.
 * First it checks the preconditions that a tapeDrive must meet in order to be
 * assigned. The couples (drive,requests) are then orderd by the priorityLevel 
 * and by the modification time and processed one by one to verify
 * if any dedication exists and has to be applied.
 * Returns the relevant IDs if a couple was found, (0,0) otherwise.
 */  
/*
CREATE OR REPLACE PROCEDURE allocateDrive
 (ret OUT NUMBER) AS
  d2rCur castorVdqm.Drive2Req_Cur;
  d2r castorVdqm.Drive2Req;
  countDed INTEGER;
BEGIN
  ret := 0;
  
  -- Check all preconditions a tape drive must meet in order to be used by pending tape requests
  OPEN d2rCur FOR
  SELECT FreeTD.id, TapeRequest.id
    FROM TapeDrive FreeTD, TapeRequest, TapeDrive2TapeDriveComp, TapeDriveCompatibility, TapeServer
   WHERE FreeTD.status = 0  -- UNIT_UP
     AND FreeTD.runningTapeReq = 0  -- not associated with a tape request
     AND FreeTD.tape = 0 -- not associated with a tape
     AND FreeTD.tapeServer = TapeServer.id 
     AND TapeServer.actingMode = 0  -- ACTIVE
     AND TapeRequest.tapeDrive = 0
     AND (TapeRequest.requestedSrv = TapeServer.id OR TapeRequest.requestedSrv = 0)
     AND TapeDrive2TapeDriveComp.parent = FreeTD.id 
     AND TapeDrive2TapeDriveComp.child = TapeDriveCompatibility.id 
     AND TapeDriveCompatibility.tapeAccessSpecification = TapeRequest.tapeAccessSpecification
     AND FreeTD.deviceGroupName = TapeRequest.deviceGroupName
     AND NOT EXISTS (
       -- we explicitly exclude requests whose tape has already been assigned to a drive;
       -- those requests will be considered upon drive release (cf. TapeDriveStatusHandler)
       SELECT 'x'
         FROM TapeDrive UsedTD
        WHERE UsedTD.tape = TapeRequest.tape
       )
     -- AND TapeDrive.deviceGroupName = tapeDriveDgn.id 
     -- AND TapeRequest.deviceGroupName = tapeRequestDgn.id 
     -- AND tapeDriveDgn.libraryName = tapeRequestDgn.libraryName 
         -- in case we want to match by libraryName only
     ORDER BY TapeDriveCompatibility.priorityLevel ASC, 
              TapeRequest.modificationTime ASC;

  LOOP
    -- For each candidate couple, verify that the dedications (if any) are met
    FETCH d2rCur INTO d2r;
    EXIT WHEN d2rCur%NOTFOUND;

    SELECT count(*) INTO countDed
      FROM TapeDriveDedication
     WHERE tapeDrive = d2r.tapeDrive
       AND getTime() BETWEEN startTime AND endTime;
    IF countDed = 0 THEN    -- no dedications valid for this TapeDrive
      UPDATE TapeDrive SET
        status = 1, -- UNIT_STARTING = 1
        jobID = 0,
        modificationTime = getTime(),
        runningTapeReq = d2r.tapeRequest
        WHERE id = d2r.tapeDrive;
      UPDATE TapeRequest SET
        status = 1, -- MATCHED
        tapeDrive = d2r.tapeDrive,
        modificationTime = getTime()
        WHERE id = d2r.tapeRequest;
      ret := 1;
      CLOSE d2rCur;
      RETURN;
    END IF;

    -- We must check if the request matches the dedications for this tape drive
    SELECT count(*) INTO countDed
      FROM TapeDriveDedication tdd, VdqmTape, TapeRequest,
           ClientIdentification, TapeAccessSpecification, TapeDrive
     WHERE tdd.tapeDrive = d2r.tapeDrive
       AND getTime() BETWEEN startTime AND endTime
       AND tdd.clientHost(+) = ClientIdentification.machine
       AND tdd.euid(+) = ClientIdentification.euid
       AND tdd.egid(+) = ClientIdentification.egid
       AND tdd.vid(+) = VdqmTape.vid
       AND tdd.accessMode(+) = TapeAccessSpecification.accessMode
       AND TapeRequest.id = d2r.tapeRequest
       AND TapeRequest.tape = VdqmTape.id
       AND TapeRequest.tapeAccessSpecification = TapeAccessSpecification.id
       AND TapeRequest.client = ClientIdentification.id;
    IF countDed > 0 THEN  -- there's a matching dedication for at least a criterium
      UPDATE TapeDrive SET
        status = 1, -- UNIT_STARTING = 1
        jobID = 0,
        modificationTime = getTime(),
        runningTapeReq = d2r.tapeRequest
        WHERE id = d2r.tapeDrive;
      UPDATE TapeRequest SET
        status = 1, -- MATCHED
        tapeDrive = d2r.tapeDrive,
        modificationTime = getTime()
        WHERE id = d2r.tapeRequest;
      ret := 1;
      CLOSE d2rCur;
      RETURN;
    END IF;
    -- else the tape drive is dedicated to other request(s) and we can't use it, go on
  END LOOP;
  -- if the loop has been fully completed without assignment,
  -- no free tape drive has been found. 
  CLOSE d2rCur;
END;
*/


/**
 * PL/SQL procedure which tries to allocate a free tape drive to a pending tape
 * request.
 *
 * This method returns the ID of the corresponding tape request if the method
 * successfully allocates a drive, else 0.
 */
CREATE OR REPLACE
PROCEDURE allocateDrive
 (ret OUT NUMBER) AS
 tapeDriveID_var   NUMBER := 0;
 tapeRequestID_var NUMBER := 0;
BEGIN
  ret := 0;

  SELECT
    tapeDriveID,
    tapeRequestID
  INTO
    tapeDriveID_var, tapeRequestID_var
  FROM
    CANDIDATEDRIVEALLOCATIONS_VIEW
  WHERE
    rownum < 2;

  -- If there is a free drive which can be allocated to a pending request
  IF tapeDriveID_var != 0 AND tapeRequestID_var != 0 THEN

    UPDATE TapeDrive SET
      status           = 1, -- UNIT_STARTING
      jobID            = 0,
      modificationTime = getTime(),
      runningTapeReq   = tapeRequestID_var
    WHERE
      id = tapeDriveID_var;

    UPDATE TapeRequest SET
      status           = 1, -- MATCHED
      tapeDrive        = tapeDriveID_var,
      modificationTime = getTime()
    WHERE
      id = tapeRequestID_var;

    RET := 1;
  END IF;

EXCEPTION
  -- Do nothing if there was no free tape drive which could be allocated to a
  -- pending request
  WHEN NO_DATA_FOUND THEN NULL;
END;


/**
 * PL/SQL method to get a new matched request to be submitted to rtcpd
 */
CREATE OR REPLACE PROCEDURE requestToSubmit(tapeReqId OUT NUMBER) AS
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
  -- We have observed ORA-00054 errors (resource busy and acquire with NOWAIT) even with
  -- the SKIP LOCKED clause. This is a workaround to ignore the error until we understand
  -- what to do, another thread will pick up the request so we don't do anything.
  NULL;
END;


/**
 * PL/SQL method to check and reuse a tape allocation, that is a tape-tape
 * drive match
 */
CREATE OR REPLACE PROCEDURE reuseTapeAllocation(tapeId IN NUMBER,
  tapeDriveId IN NUMBER, tapeReqId OUT NUMBER) AS
BEGIN
  tapeReqId := 0;
  UPDATE TapeRequest
     SET status = 1,  -- MATCHED
         tapeDrive = tapeDriveId,
         modificationTime = getTime()
   WHERE tapeDrive = 0
     AND status = 0  -- PENDING
     AND tape = tapeId
     AND ROWNUM < 2
  RETURNING id INTO tapeReqId;
  IF tapeReqId > 0 THEN -- If a tape request was found
    UPDATE TapeDrive
       SET status = 1, -- UNIT_STARTING
           jobID = 0,
           runningTapeReq = tapeReqId,
           modificationTime = getTime()
     WHERE id = tapeDriveId;
  END IF;
END;
