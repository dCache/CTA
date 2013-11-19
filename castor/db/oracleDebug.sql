/*******************************************************************
 *
 *
 * Some SQL code to ease support and debugging
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PL/SQL declaration for the castorDebug package */
CREATE OR REPLACE PACKAGE castorDebug AS
  TYPE DiskCopyDebug_typ IS RECORD (
    id INTEGER,
    status VARCHAR2(2048),
    creationtime VARCHAR2(2048),
    diskPool VARCHAR2(2048),
    location VARCHAR2(2048),
    available CHAR(1),
    diskCopySize NUMBER,
    castorFileSize NUMBER,
    gcWeight NUMBER);
  TYPE DiskCopyDebug IS TABLE OF DiskCopyDebug_typ;
  TYPE SubRequestDebug IS TABLE OF SubRequest%ROWTYPE;
  TYPE RequestDebug_typ IS RECORD (
    creationtime VARCHAR2(2048),
    SubReqId NUMBER,
    Status NUMBER,
    username VARCHAR2(2048),
    machine VARCHAR2(2048),
    svcClassName VARCHAR2(2048),
    ReqId NUMBER,
    ReqType VARCHAR2(20));
  TYPE RequestDebug IS TABLE OF RequestDebug_typ;
  TYPE RecallJobDebug_typ IS RECORD (
    id INTEGER,
    status VARCHAR2(2048),
    creationtime VARCHAR2(2048),
    fseq INTEGER,
    copyNb INTEGER,
    recallGroup VARCHAR(2048),
    svcClass VARCHAR(2048),
    euid INTEGER,
    egid INTEGER,
    vid VARCHAR(2048),
    nbRetriesWithinMount INTEGER,
    nbMounts INTEGER);
  TYPE RecallJobDebug IS TABLE OF RecallJobDebug_typ;
  TYPE MigrationJobDebug_typ IS RECORD (
    id INTEGER,
    status VARCHAR2(2048),
    creationTime VARCHAR2(2048),
    fileSize INTEGER,
    tapePoolName VARCHAR2(2048),
    destCopyNb INTEGER,
    fseq INTEGER,
    mountTransactionId INTEGER,
    originalVID VARCHAR2(2048),
    originalCopyNb INTEGER,
    nbRetries INTEGER,
    fileTransactionId INTEGER);
  TYPE MigrationJobDebug IS TABLE OF MigrationJobDebug_typ;
  TYPE Disk2DiskCopyJobDebug_typ IS RECORD (
    id INTEGER,
    status VARCHAR2(2048),
    creationTime VARCHAR2(2048),
    transferId VARCHAR2(2048),
    retryCounter INTEGER,
    nsOpenTime INTEGER,
    destSvcClassName VARCHAR2(2048),
    replicationType VARCHAR2(2048),
    replacedDCId INTEGER,
    destDCId INTEGER,
    drainingJob INTEGER);
  TYPE Disk2DiskCopyJobDebug IS TABLE OF Disk2DiskCopyJobDebug_typ;
END;
/

/* Return the castor file id associated with the reference number */
CREATE OR REPLACE FUNCTION getCF(ref NUMBER) RETURN NUMBER AS
  t NUMBER;
  cfId NUMBER;
BEGIN
  SELECT id INTO cfId FROM CastorFile WHERE id = ref OR fileId = ref;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- DiskCopy?
BEGIN
  SELECT castorFile INTO cfId FROM DiskCopy WHERE id = ref;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- SubRequest?
BEGIN
  SELECT castorFile INTO cfId FROM SubRequest WHERE id = ref;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- RecallJob?
BEGIN
  SELECT castorFile INTO cfId FROM RecallJob WHERE id = ref;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- MigrationJob?
BEGIN
  SELECT castorFile INTO cfId FROM MigrationJob WHERE id = ref;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- Disk2DiskCopyJob?
BEGIN
  SELECT castorFile INTO cfId FROM Disk2DiskCopyJob WHERE id = ref;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- nothing found
  RAISE_APPLICATION_ERROR (-20000, 'Could not find any CastorFile, SubRequest, DiskCopy, MigrationJob, RecallJob or Disk2DiskCopyJob with id = ' || ref);
END; END; END; END; END; END;
/

/* Function to convert seconds into a time string using the format:
 * DD-MON-YYYY HH24:MI:SS. If seconds is not defined then the current time
 * will be returned.
 */
CREATE OR REPLACE FUNCTION getTimeString
(seconds IN NUMBER DEFAULT NULL,
 format  IN VARCHAR2 DEFAULT 'DD-MON-YYYY HH24:MI:SS')
RETURN VARCHAR2 AS
BEGIN
  RETURN (to_char(to_date('01-JAN-1970', 'DD-MON-YYYY') +
          nvl(seconds, getTime()) / (60 * 60 * 24), format));
END;
/


/* Get the diskcopys associated with the reference number */
CREATE OR REPLACE FUNCTION getDCs(ref number) RETURN castorDebug.DiskCopyDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT DiskCopy.id, getObjStatusName('DiskCopy', 'status', DiskCopy.status) AS status,
                   getTimeString(DiskCopy.creationtime) AS creationtime,
                   DiskPool.name AS diskpool,
                   DiskServer.name || ':' || FileSystem.mountPoint || DiskCopy.path AS location,
                   decode(DiskServer.status, 2, 'N', decode(FileSystem.status, 2, 'N', 'Y')) AS available,
                   DiskCopy.diskCopySize AS diskcopysize,
                   CastorFile.fileSize AS castorfilesize,
                   trunc(DiskCopy.gcWeight, 2) AS gcweight
              FROM DiskCopy, FileSystem, DiskServer, DiskPool, CastorFile
             WHERE DiskCopy.fileSystem = FileSystem.id(+)
               AND FileSystem.diskServer = diskServer.id(+)
               AND DiskPool.id(+) = fileSystem.diskPool
               AND DiskCopy.castorFile = getCF(ref)
               AND DiskCopy.castorFile = CastorFile.id) LOOP
     PIPE ROW(d);
  END LOOP;
END;
/


/* Get the recalljobs associated with the reference number */
CREATE OR REPLACE FUNCTION getRJs(ref number) RETURN castorDebug.RecallJobDebug PIPELINED AS
BEGIN
  FOR t IN (SELECT RecallJob.id, getObjStatusName('RecallJob', 'status', RecallJob.status) as status,
                   getTimeString(RecallJob.creationTime) as creationTime,
                   RecallJob.fseq, RecallJob.copyNb, RecallGroup.name as recallGroupName,
                   SvcClass.name as svcClassName, RecallJob.euid, RecallJob.egid, RecallJob.vid,
                   RecallJob.nbRetriesWithinMount, RecallJob.nbMounts
              FROM RecallJob, RecallGroup, SvcClass
             WHERE RecallJob.castorfile = getCF(ref)
               AND RecallJob.recallGroup = RecallGroup.id
               AND RecallJob.svcClass = SvcClass.id) LOOP
     PIPE ROW(t);
  END LOOP;
END;
/


/* Get the migration jobs associated with the reference number */
CREATE OR REPLACE FUNCTION getMJs(ref number) RETURN castorDebug.MigrationJobDebug PIPELINED AS
BEGIN
  FOR t IN (SELECT MigrationJob.id, getObjStatusName('MigrationJob', 'status', MigrationJob.status) as status,
                   getTimeString(MigrationJob.creationTime) as creationTime,
                   MigrationJob.fileSize, TapePool.name as tapePoolName,
                   MigrationJob.destCopyNb, MigrationJob.fseq,
                   MigrationJob.mountTransactionId,
                   MigrationJob.originalVID, MigrationJob.originalCopyNb,
                   MigrationJob.nbRetries, MigrationJob.fileTransactionId
              FROM MigrationJob, TapePool
             WHERE castorfile = getCF(ref)
               AND MigrationJob.tapePool = TapePool.id) LOOP
     PIPE ROW(t);
  END LOOP;
END;
/


/* Get the (disk2disk) copy jobs associated with the reference number */
CREATE OR REPLACE FUNCTION getCJs(ref number) RETURN castorDebug.Disk2DiskCopyJobDebug PIPELINED AS
BEGIN
  FOR t IN (SELECT Disk2DiskCopyJob.id, getObjStatusName('Disk2DiskCopyJob', 'status', Disk2DiskCopyJob.status) as status,
                   getTimeString(Disk2DiskCopyJob.creationTime) as creationTime,
                   Disk2DiskCopyJob.transferId, Disk2DiskCopyJob.retryCounter,
                   Disk2DiskCopyJob.nsOpenTime, SvcClass.name as destSvcClassName,
                   getObjStatusName('Disk2DiskCopyJob', 'replicationType', Disk2DiskCopyJob.replicationType) as replicationType,
                   Disk2DiskCopyJob.replacedDCId, Disk2DiskCopyJob.destDCId,
                   Disk2DiskCopyJob.drainingJob
              FROM Disk2DiskCopyJob, SvcClass
             WHERE castorfile = getCF(ref)
               AND Disk2DiskCopyJob.destSvcClass = SvcClass.id) LOOP
     PIPE ROW(t);
  END LOOP;
END;
/


/* Get the subrequests associated with the reference number. */
CREATE OR REPLACE FUNCTION getSRs(ref number) RETURN castorDebug.SubRequestDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT * FROM SubRequest WHERE castorfile = getCF(ref)) LOOP
     PIPE ROW(d);
  END LOOP;
END;
/


/* Get the requests associated with the reference number. */
CREATE OR REPLACE FUNCTION getRs(ref number) RETURN castorDebug.RequestDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT getTimeString(creationtime) AS creationtime,
                   SubRequest.id AS SubReqId, SubRequest.Status,
                   username, machine, svcClassName, Request.id AS ReqId, Request.type AS ReqType
              FROM SubRequest,
                    (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, username, machine, svcClassName, 'Get' AS type FROM StageGetRequest UNION ALL
                     SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id, username, machine, svcClassName, 'PGet' AS type FROM StagePrepareToGetRequest UNION ALL
                     SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id, username, machine, svcClassName, 'Put' AS type FROM StagePutRequest UNION ALL
                     SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id, username, machine, svcClassName, 'PPut' AS type FROM StagePrepareToPutRequest UNION ALL
                     SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, username, machine, svcClassName, 'Upd' AS type FROM StageUpdateRequest UNION ALL
                     SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id, username, machine, svcClassName, 'PUpd' AS type FROM StagePrepareToUpdateRequest UNION ALL
                     SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id, username, machine, svcClassName, 'Repack' AS type FROM StageRepackRequest UNION ALL
                     SELECT /*+ INDEX(StagePutDoneRequest PK_StagePutDoneRequest_Id) */ id, username, machine, svcClassName, 'PutDone' AS type FROM StagePutDoneRequest UNION ALL
                     SELECT /*+ INDEX(SetFileGCWeight PK_SetFileGCWeight_Id) */ id, username, machine, svcClassName, 'SetGCW' AS type FROM SetFileGCWeight) Request
             WHERE castorfile = getCF(ref)
               AND Request.id = SubRequest.request) LOOP
     PIPE ROW(d);
  END LOOP;
END;
/
