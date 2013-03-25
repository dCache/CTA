/*******************************************************************
 *
 * PL/SQL code for the stager and resource monitoring
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PL/SQL declaration for the castor package */
CREATE OR REPLACE PACKAGE castor AS
  TYPE DiskCopyCore IS RECORD (
    id INTEGER,
    path VARCHAR2(2048),
    status NUMBER,
    fsWeight NUMBER,
    mountPoint VARCHAR2(2048),
    diskServer VARCHAR2(2048));
  TYPE DiskCopy_Cur IS REF CURSOR RETURN DiskCopyCore;
  TYPE "strList" IS TABLE OF VARCHAR2(2048) index BY binary_integer;
  TYPE "cnumList" IS TABLE OF NUMBER index BY binary_integer;
  TYPE QueryLine IS RECORD (
    fileid INTEGER,
    nshost VARCHAR2(2048),
    diskCopyId INTEGER,
    diskCopyPath VARCHAR2(2048),
    filesize INTEGER,
    diskCopyStatus INTEGER,
    diskServerName VARCHAR2(2048),
    fileSystemMountPoint VARCHAR2(2048),
    nbaccesses INTEGER,
    lastKnownFileName VARCHAR2(2048),
    creationTime INTEGER,
    svcClass VARCHAR2(2048),
    lastAccessTime INTEGER,
    hwStatus INTEGER);
  TYPE QueryLine_Cur IS REF CURSOR RETURN QueryLine;
  TYPE FileList IS RECORD (
    fileId NUMBER,
    nsHost VARCHAR2(2048));
  TYPE FileList_Cur IS REF CURSOR RETURN FileList;
  TYPE DiskPoolQueryLine IS RECORD (
    isDP INTEGER,
    isDS INTEGER,
    diskServerName VARCHAR(2048),
    diskServerStatus INTEGER,
    fileSystemmountPoint VARCHAR(2048),
    fileSystemfreeSpace INTEGER,
    fileSystemtotalSpace INTEGER,
    fileSystemminfreeSpace INTEGER,
    fileSystemmaxFreeSpace INTEGER,
    fileSystemStatus INTEGER);
  TYPE DiskPoolQueryLine_Cur IS REF CURSOR RETURN DiskPoolQueryLine;
  TYPE DiskPoolsQueryLine IS RECORD (
    isDP INTEGER,
    isDS INTEGER,
    diskPoolName VARCHAR(2048),
    diskServerName VARCHAR(2048),
    diskServerStatus INTEGER,
    fileSystemmountPoint VARCHAR(2048),
    fileSystemfreeSpace INTEGER,
    fileSystemtotalSpace INTEGER,
    fileSystemminfreeSpace INTEGER,
    fileSystemmaxFreeSpace INTEGER,
    fileSystemStatus INTEGER);
  TYPE DiskPoolsQueryLine_Cur IS REF CURSOR RETURN DiskPoolsQueryLine;
  TYPE IDRecord IS RECORD (id INTEGER);
  TYPE IDRecord_Cur IS REF CURSOR RETURN IDRecord;
  TYPE UUIDRecord IS RECORD (uuid VARCHAR(2048));
  TYPE UUIDRecord_Cur IS REF CURSOR RETURN UUIDRecord;
  TYPE UUIDPairRecord IS RECORD (uuid1 VARCHAR(2048), uuid2 VARCHAR(2048));
  TYPE UUIDPairRecord_Cur IS REF CURSOR RETURN UUIDPairRecord;
  TYPE TransferRecord IS RECORD (subreId VARCHAR(2048), resId VARCHAR(2048), fileId NUMBER, nsHost VARCHAR2(2048));
  TYPE TransferRecord_Cur IS REF CURSOR RETURN TransferRecord;
  TYPE StringValue IS RECORD (value VARCHAR(2048));
  TYPE StringList_Cur IS REF CURSOR RETURN StringValue;
  TYPE FileEntry IS RECORD (
    fileid INTEGER,
    nshost VARCHAR2(2048));
  TYPE FileEntry_Cur IS REF CURSOR RETURN FileEntry;
  TYPE TapeAccessPriority IS RECORD (
    euid INTEGER,
    egid INTEGER,
    priority INTEGER);
  TYPE TapeAccessPriority_Cur IS REF CURSOR RETURN TapeAccessPriority;
  TYPE StreamReport IS RECORD (
    diskserver VARCHAR2(2048),
    mountPoint VARCHAR2(2048));
  TYPE StreamReport_Cur IS REF CURSOR RETURN StreamReport;
  TYPE FileResult IS RECORD (
    fileid INTEGER,
    nshost VARCHAR2(2048),
    errorcode INTEGER,
    errormessage VARCHAR2(2048));
  TYPE FileResult_Cur IS REF CURSOR RETURN FileResult;
  TYPE DiskCopyResult IS RECORD (
    dcId INTEGER,
    retCode INTEGER);
  TYPE DiskCopyResult_Cur IS REF CURSOR RETURN DiskCopyResult;
  TYPE LogEntry IS RECORD (
    timeinfo NUMBER,
    uuid VARCHAR2(2048),
    priority INTEGER,
    msg VARCHAR2(2048),
    fileId NUMBER,
    nsHost VARCHAR2(2048),
    source VARCHAR2(2048),
    params VARCHAR2(2048));
  TYPE LogEntry_Cur IS REF CURSOR RETURN LogEntry;
END castor;
/

/* Used to create a row in FileSystemsToCheck
   whenever a new FileSystem is created */
CREATE OR REPLACE TRIGGER tr_FileSystem_Insert
BEFORE INSERT ON FileSystem
FOR EACH ROW
BEGIN
  INSERT INTO FileSystemsToCheck (FileSystem, ToBeChecked) VALUES (:new.id, 0);
END;
/

/* Used to delete rows in FileSystemsToCheck
   whenever a FileSystem is deleted */
CREATE OR REPLACE TRIGGER tr_FileSystem_Delete
BEFORE DELETE ON FileSystem
FOR EACH ROW
BEGIN
  DELETE FROM FileSystemsToCheck WHERE FileSystem = :old.id;
END;
/

/* Checks consistency of DiskCopies when a FileSystem comes
 * back in production after a period spent in a DRAINING or a
 * DISABLED status.
 * Current checks/fixes include :
 *   - Canceling recalls for files that are STAGED or CANBEMIGR
 *     on the fileSystem that comes back. (Scheduled for bulk
 *     operation)
 *   - Dealing with files that are STAGEOUT on the fileSystem
 *     coming back but already exist on another one
 */
CREATE OR REPLACE PROCEDURE checkFSBackInProd(fsId NUMBER) AS
BEGIN
  -- Flag the filesystem for processing in a bulk operation later.
  -- We need to do this because some operations are database intensive
  -- and therefore it is often better to process several filesystems
  -- simultaneous with one query as opposed to one by one. Especially
  -- where full table scans are involved.
  UPDATE FileSystemsToCheck SET toBeChecked = 1
   WHERE fileSystem = fsId;
  -- Look for files that are STAGEOUT on the filesystem coming back to life
  -- but already STAGED/CANBEMIGR/WAITFS/STAGEOUT/
  -- WAITFS_SCHEDULING somewhere else
  FOR cf IN (SELECT /*+ USE_NL(D E) INDEX(D I_DiskCopy_Status6) */
                    UNIQUE D.castorfile, D.id dcId
               FROM DiskCopy D, DiskCopy E
              WHERE D.castorfile = E.castorfile
                AND D.fileSystem = fsId
                AND E.fileSystem != fsId
                AND decode(D.status,6,D.status,NULL) = dconst.DISKCOPY_STAGEOUT
                AND E.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR,
                                 dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_STAGEOUT,
                                 dconst.DISKCOPY_WAITFS_SCHEDULING)) LOOP
    -- Invalidate the DiskCopy
    UPDATE DiskCopy
       SET status = dconst.DISKCOPY_INVALID
     WHERE id = cf.dcId;
  END LOOP;
END;
/

/* PL/SQL method implementing bulkCheckFSBackInProd for processing
 * filesystems in one bulk operation to optimise database performance
 */
CREATE OR REPLACE PROCEDURE bulkCheckFSBackInProd AS
  fsIds "numList";
BEGIN
  -- Extract a list of filesystems which have been scheduled to be
  -- checked in a bulk operation on the database.
  UPDATE FileSystemsToCheck SET toBeChecked = 0
   WHERE toBeChecked = 1
  RETURNING fileSystem BULK COLLECT INTO fsIds;
  -- Nothing found, return
  IF fsIds.COUNT = 0 THEN
    RETURN;
  END IF;
  -- Look for recalls concerning files that are STAGED or CANBEMIGR
  -- on all filesystems scheduled to be checked, and restart their
  -- subrequests (reconsidering the recall source).
  FOR file IN (SELECT UNIQUE DiskCopy.castorFile
               FROM DiskCopy, RecallJob
              WHERE DiskCopy.castorfile = RecallJob.castorfile
                AND DiskCopy.fileSystem IN
                  (SELECT /*+ CARDINALITY(fsidTable 5) */ *
                     FROM TABLE(fsIds) fsidTable)
                AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR)) LOOP
    -- cancel the recall for that file
    deleteRecallJobs(file.castorFile);
    -- restart subrequests that were waiting on the recall
    UPDATE SubRequest
       SET status = dconst.SUBREQUEST_RESTART
     WHERE castorFile = file.castorFile
       AND status = dconst.SUBREQUEST_WAITTAPERECALL;
    -- commit that file
    COMMIT;
  END LOOP;
END;
/


/* SQL statement for the update trigger on the FileSystem table */
CREATE OR REPLACE TRIGGER tr_FileSystem_Update
BEFORE UPDATE OF status ON FileSystem
FOR EACH ROW WHEN (old.status != new.status)
BEGIN
  -- If the filesystem is coming back into PRODUCTION, initiate a consistency
  -- check for the diskcopies which reside on the filesystem.
  IF :old.status != dconst.FILESYSTEM_PRODUCTION AND
     :new.status = dconst.FILESYSTEM_PRODUCTION THEN
    checkFsBackInProd(:old.id);
  END IF;
  -- Cancel any ongoing draining operations if the filesystem is not in a
  -- DRAINING state
  IF :new.status != dconst.FILESYSTEM_DRAINING THEN
    UPDATE DrainingFileSystem
       SET status = 3  -- INTERRUPTED
     WHERE fileSystem = :new.id
       AND status IN (0, 1, 2, 7);  -- CREATED, INITIALIZING, RUNNING, RESTART
  END IF;
END;
/


/* SQL statement for the update trigger on the DiskServer table */
CREATE OR REPLACE TRIGGER tr_DiskServer_Update
BEFORE UPDATE OF status ON DiskServer
FOR EACH ROW WHEN (old.status != new.status)
BEGIN
  -- If the diskserver is coming back into PRODUCTION, initiate a consistency
  -- check for all the diskcopies on its associated filesystems which are in
  -- PRODUCTION.
  IF :old.status != dconst.DISKSERVER_PRODUCTION AND
     :new.status = dconst.DISKSERVER_PRODUCTION AND :new.hwOnline = 1 THEN
    FOR fs IN (SELECT id FROM FileSystem
                WHERE diskServer = :old.id
                  AND status = dconst.FILESYSTEM_PRODUCTION)
    LOOP
      checkFsBackInProd(fs.id);
    END LOOP;
  END IF;
  -- Cancel all draining operations if the diskserver is disabled.
  IF :new.status = dconst.DISKSERVER_DISABLED THEN
    UPDATE DrainingFileSystem
       SET status = 3  -- INTERRUPTED
     WHERE fileSystem IN
       (SELECT FileSystem.id FROM FileSystem
         WHERE FileSystem.diskServer = :new.id)
       AND status IN (0, 1, 2, 7);  -- CREATED, INITIALIZING, RUNNING, RESTART
  END IF;
  -- If the diskserver is in PRODUCTION cancel the draining operation of
  -- filesystems not in DRAINING.
  IF :new.status = dconst.DISKSERVER_PRODUCTION THEN
    UPDATE DrainingFileSystem
       SET status = 3  -- INTERRUPTED
     WHERE fileSystem IN
       (SELECT FileSystem.id FROM FileSystem
         WHERE FileSystem.diskServer = :new.ID
           AND FileSystem.status != 1)  -- DRAINING
       AND status IN (0, 1, 2, 7);  -- CREATED, INITIALIZING, RUNNING, RESTART
  END IF;
END;
/


/* Trigger used to check if the maxReplicaNb has been exceeded
 * after a diskcopy has changed its status to STAGED or CANBEMIGR
 */
CREATE OR REPLACE TRIGGER tr_DiskCopy_Stmt_Online
AFTER UPDATE OF STATUS ON DISKCOPY
DECLARE
  maxReplicaNb NUMBER;
  unused NUMBER;
  nbFiles NUMBER;
BEGIN
  -- Loop over the diskcopies to be processed
  FOR a IN (SELECT * FROM TooManyReplicasHelper)
  LOOP
    -- Lock the castorfile. This shouldn't be necessary as the procedure that
    -- caused the trigger to be executed should already have the lock.
    -- Nevertheless, we make sure!
    SELECT id INTO unused FROM CastorFile
     WHERE id = a.castorfile FOR UPDATE;
    -- Get the max replica number of the service class
    SELECT maxReplicaNb INTO maxReplicaNb
      FROM SvcClass WHERE id = a.svcclass;
    -- Produce a list of diskcopies to invalidate should too many replicas be
    -- online.
    FOR b IN (SELECT id FROM (
                SELECT rownum ind, id FROM (
                  SELECT /*+ INDEX (DiskCopy I_DiskCopy_Castorfile) */ DiskCopy.id
                    FROM DiskCopy, FileSystem, DiskPool2SvcClass, SvcClass,
                         DiskServer
                   WHERE DiskCopy.filesystem = FileSystem.id
                     AND FileSystem.diskpool = DiskPool2SvcClass.parent
                     AND FileSystem.diskserver = DiskServer.id
                     AND DiskPool2SvcClass.child = SvcClass.id
                     AND DiskCopy.castorfile = a.castorfile
                     AND DiskCopy.status IN (0, 10)  -- STAGED, CANBEMIGR
                     AND SvcClass.id = a.svcclass
                   -- Select non-PRODUCTION hardware first
                   ORDER BY decode(FileSystem.status, 0,
                            decode(DiskServer.status, 0, 0, 1), 1) ASC,
                            DiskCopy.gcWeight DESC))
               WHERE ind > maxReplicaNb)
    LOOP
      -- Sanity check, make sure that the last copy is never dropped!
      SELECT /*+ INDEX(DiskCopy I_DiskCopy_CastorFile) */ count(*) INTO nbFiles
        FROM DiskCopy, FileSystem, DiskPool2SvcClass, SvcClass, DiskServer
       WHERE DiskCopy.filesystem = FileSystem.id
         AND FileSystem.diskpool = DiskPool2SvcClass.parent
         AND FileSystem.diskserver = DiskServer.id
         AND DiskPool2SvcClass.child = SvcClass.id
         AND DiskCopy.castorfile = a.castorfile
         AND DiskCopy.status IN (0, 10)  -- STAGED, CANBEMIGR
         AND SvcClass.id = a.svcclass;
      IF nbFiles = 1 THEN
        EXIT;  -- Last file, so exit the loop
      END IF;
      -- Invalidate the diskcopy
      UPDATE DiskCopy
         SET status = dconst.DISKCOPY_INVALID,
             gcType = dconst.GCTYPE_TOOMANYREPLICAS
       WHERE id = b.id;
    END LOOP;
  END LOOP;
  -- cleanup the table so that we do not accumulate lines. This would trigger
  -- a n^2 behavior until the next commit.
  DELETE FROM TooManyReplicasHelper;
END;
/


/* Trigger used to provide input to the statement level trigger
 * defined above
 */
CREATE OR REPLACE TRIGGER tr_DiskCopy_Online
AFTER UPDATE OF status ON DiskCopy
FOR EACH ROW
WHEN ((old.status != 10) AND    -- !CANBEMIGR -> {STAGED, CANBEMIGR}
      (new.status = 0 OR new.status = 10))
DECLARE
  svcId  NUMBER;
  unused NUMBER;
  -- Trap `ORA-00001: unique constraint violated` errors
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -00001);
BEGIN
  -- Insert the information about the diskcopy being processed into
  -- the TooManyReplicasHelper. This information will be used later
  -- on the DiskCopy AFTER UPDATE statement level trigger. We cannot
  -- do the work of that trigger here as it would result in
  -- `ORA-04091: table is mutating, trigger/function` errors
  BEGIN
    SELECT SvcClass.id INTO svcId
      FROM FileSystem, DiskPool2SvcClass, SvcClass
     WHERE FileSystem.diskpool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = SvcClass.id
       AND FileSystem.id = :new.filesystem;
  EXCEPTION WHEN TOO_MANY_ROWS THEN
    -- The filesystem belongs to multiple service classes which is not
    -- supported by the replica management trigger.
    RETURN;
  END;
  -- Insert an entry into the TooManyReplicasHelper table.
  BEGIN
    INSERT INTO TooManyReplicasHelper (svcClass, castorFile)
    VALUES (svcId, :new.castorfile);
  EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
    RETURN;  -- Entry already exists!
  END;
END;
/


/* PL/SQL method to get the next SubRequest to do according to the given service */
CREATE OR REPLACE PROCEDURE subRequestToDo(service IN VARCHAR2,
                                           srId OUT INTEGER, srRetryCounter OUT INTEGER, srFileName OUT VARCHAR2,
                                           srProtocol OUT VARCHAR2, srXsize OUT INTEGER,
                                           srModeBits OUT INTEGER, srFlags OUT INTEGER,
                                           srSubReqId OUT VARCHAR2, srAnswered OUT INTEGER, srReqType OUT INTEGER,
                                           rId OUT INTEGER, rFlags OUT INTEGER, rUsername OUT VARCHAR2, rEuid OUT INTEGER,
                                           rEgid OUT INTEGER, rMask OUT INTEGER, rPid OUT INTEGER, rMachine OUT VARCHAR2,
                                           rSvcClassName OUT VARCHAR2, rUserTag OUT VARCHAR2, rReqId OUT VARCHAR2,
                                           rCreationTime OUT INTEGER, rLastModificationTime OUT INTEGER,
                                           rRepackVid OUT VARCHAR2, rGCWeight OUT INTEGER,
                                           clIpAddress OUT INTEGER, clPort OUT INTEGER, clVersion OUT INTEGER) AS
  CURSOR SRcur IS
    SELECT /*+ FIRST_ROWS_10 INDEX(SR I_SubRequest_RT_CT_ID) */ SR.id
      FROM SubRequest PARTITION (P_STATUS_0_1_2) SR  -- START, RESTART, RETRY
     WHERE SR.svcHandler = service
     ORDER BY SR.creationTime ASC;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  varSrId NUMBER;
  varRName VARCHAR2(100);
  varClientId NUMBER;
BEGIN
  OPEN SRcur;
  -- Loop on candidates until we can lock one
  LOOP
    -- Fetch next candidate
    FETCH SRcur INTO varSrId;
    IF SRcur%NOTFOUND THEN
      -- No candidate, just return
      RETURN;
    END IF;
    BEGIN
      -- Try to take a lock on the current candidate, and revalidate its status
      SELECT /*+ INDEX(SR PK_SubRequest_ID) */ id INTO varSrId
        FROM SubRequest PARTITION (P_STATUS_0_1_2) SR
       WHERE id = varSrId FOR UPDATE NOWAIT;
      -- Since we are here, we got the lock. We have our winner, let's update it
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_WAITSCHED, subReqId = nvl(subReqId, uuidGen())
       WHERE id = varSrId
      RETURNING id, retryCounter, fileName, protocol, xsize, modeBits, flags, subReqId,
        answered, reqType, request, (SELECT object FROM Type2Obj WHERE type = reqType)
        INTO srId, srRetryCounter, srFileName, srProtocol, srXsize, srModeBits, srFlags, srSubReqId,
        srAnswered, srReqType, rId, varRName;
      EXIT;
    EXCEPTION
      WHEN NO_DATA_FOUND THEN
        -- Got to next candidate, this subrequest was processed already and its status changed
        NULL;
      WHEN SrLocked THEN
        -- Go to next candidate, this subrequest is being processed by another thread
        NULL;
    END;
  END LOOP;
  CLOSE SRcur;

  BEGIN
    -- XXX This could be done in a single EXECUTE IMMEDIATE statement, but to make it
    -- XXX efficient we implement a CASE construct. At a later time the FileRequests should
    -- XXX be merged in a single table (partitioned by reqType) to avoid the following block.
    CASE
      WHEN varRName = 'StagePrepareToPutRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StagePrepareToPutRequest WHERE id = rId;
      WHEN varRName = 'StagePrepareToGetRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StagePrepareToGetRequest WHERE id = rId;
      WHEN varRName = 'StagePrepareToUpdateRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StagePrepareToUpdateRequest WHERE id = rId;
      WHEN varRName = 'StageRepackRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, repackVid, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, rRepackVid, varClientId
          FROM StageRepackRequest WHERE id = rId;
      WHEN varRName = 'StagePutRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StagePutRequest WHERE id = rId;
      WHEN varRName = 'StageGetRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StageGetRequest WHERE id = rId;
      WHEN varRName = 'StageUpdateRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StageUpdateRequest WHERE id = rId;
      WHEN varRName = 'StagePutDoneRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StagePutDoneRequest WHERE id = rId;
      WHEN varRName = 'StageRmRequest' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, varClientId
          FROM StageRmRequest WHERE id = rId;
      WHEN varRName = 'SetFileGCWeight' THEN
        SELECT flags, username, euid, egid, mask, pid, machine, svcClassName, userTag, reqId, creationTime, lastModificationTime, weight, client
          INTO rFlags, rUsername, rEuid, rEgid, rMask, rPid, rMachine, rSvcClassName, rUserTag, rReqId, rCreationTime, rLastModificationTime, rGcWeight, varClientId
          FROM SetFileGCWeight WHERE id = rId;
      WHEN varRName = 'StageDiskCopyReplicaRequest' THEN
        -- This should never happen, a replication request cannot be restarted. Raise an error
        RAISE VALUE_ERROR;
    END CASE;
    SELECT ipAddress, port, version
      INTO clIpAddress, clPort, clVersion
      FROM Client WHERE id = varClientId;
  EXCEPTION WHEN OTHERS THEN
    -- Something went really wrong, our subrequest does not have the corresponding request or client,
    -- or there has been an attempt to restart a replication subrequest.
    -- Just drop it and re-raise exception. Some rare occurrences have happened in the past,
    -- this catch-all logic protects the stager-scheduling system from getting stuck with a single such case.
    archiveSubReq(varSrId, dconst.SUBREQUEST_FAILED_FINISHED);
    COMMIT;
    raise_application_error(-20100, 'Request got corrupted and could not be processed');
  END;
END;
/

/* PL/SQL method to fail a subrequest in WAITTAPERECALL
 * and eventually the recall itself if it's the only subrequest waiting for it
 */
CREATE OR REPLACE PROCEDURE failRecallSubReq(inSrId IN INTEGER, inCfId IN INTEGER) AS
  varNbSRs INTEGER;
BEGIN
  -- recall case. First fail the subrequest
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET status = dconst.SUBREQUEST_FAILED
   WHERE id = inSrId;
  -- check whether there are other subRequests waiting for a recall
  SELECT COUNT(*) INTO varNbSrs
    FROM SubRequest
   WHERE castorFile = inCfId
     AND status = dconst.SUBREQUEST_WAITTAPERECALL;
  IF varNbSrs = 0 THEN
    -- no other subrequests, so drop recalls
    deleteRecallJobs(inCfId);
  END IF;
END;
/

/* PL/SQL method to process bulk abort on a given get/prepareToGet request */
CREATE OR REPLACE PROCEDURE processAbortForGet(sr processBulkAbortFileReqsHelper%ROWTYPE) AS
  abortedSRstatus NUMBER;
BEGIN
  -- note the revalidation of the status and even of the existence of the subrequest
  -- as it may have changed before we got the lock on the Castorfile in processBulkAbortFileReqs
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ status
    INTO abortedSRstatus
    FROM SubRequest
   WHERE id = sr.srId;
  CASE
    WHEN abortedSRstatus = dconst.SUBREQUEST_START
      OR abortedSRstatus = dconst.SUBREQUEST_RESTART
      OR abortedSRstatus = dconst.SUBREQUEST_RETRY
      OR abortedSRstatus = dconst.SUBREQUEST_WAITSCHED
      OR abortedSRstatus = dconst.SUBREQUEST_WAITSUBREQ
      OR abortedSRstatus = dconst.SUBREQUEST_READY
      OR abortedSRstatus = dconst.SUBREQUEST_REPACK
      OR abortedSRstatus = dconst.SUBREQUEST_READYFORSCHED THEN
      -- standard case, we only have to fail the subrequest
      UPDATE SubRequest SET status = dconst.SUBREQUEST_FAILED WHERE id = sr.srId;
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, 0, '');
    WHEN abortedSRstatus = dconst.SUBREQUEST_WAITTAPERECALL THEN
        failRecallSubReq(sr.srId, sr.cfId);
        INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
        VALUES (sr.fileId, sr.nsHost, 0, '');
    WHEN abortedSRstatus = dconst.SUBREQUEST_FAILED
      OR abortedSRstatus = dconst.SUBREQUEST_FAILED_FINISHED THEN
      -- subrequest has failed, nothing to abort
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, serrno.EINVAL, 'Cannot abort failed subRequest');
    WHEN abortedSRstatus = dconst.SUBREQUEST_FINISHED
      OR abortedSRstatus = dconst.SUBREQUEST_ARCHIVED THEN
      -- subrequest is over, nothing to abort
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, serrno.EINVAL, 'Cannot abort completed subRequest');
    ELSE
      -- unknown status !
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, serrno.SEINTERNAL, 'Found unknown status for request : ' || TO_CHAR(abortedSRstatus));
  END CASE;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- subRequest was deleted in the mean time !
  INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
  VALUES (sr.fileId, sr.nsHost, serrno.ENOENT, 'Targeted SubRequest has just been deleted');
END;
/

/* PL/SQL method to process bulk abort on a given put/prepareToPut request */
CREATE OR REPLACE PROCEDURE processAbortForPut(sr processBulkAbortFileReqsHelper%ROWTYPE) AS
  abortedSRstatus NUMBER;
BEGIN
  -- note the revalidation of the status and even of the existence of the subrequest
  -- as it may have changed before we got the lock on the Castorfile in processBulkAbortFileReqs
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ status INTO abortedSRstatus FROM SubRequest WHERE id = sr.srId;
  CASE
    WHEN abortedSRstatus = dconst.SUBREQUEST_START
      OR abortedSRstatus = dconst.SUBREQUEST_RESTART
      OR abortedSRstatus = dconst.SUBREQUEST_RETRY
      OR abortedSRstatus = dconst.SUBREQUEST_WAITSCHED
      OR abortedSRstatus = dconst.SUBREQUEST_WAITSUBREQ
      OR abortedSRstatus = dconst.SUBREQUEST_READY
      OR abortedSRstatus = dconst.SUBREQUEST_REPACK
      OR abortedSRstatus = dconst.SUBREQUEST_READYFORSCHED THEN
      -- standard case, we only have to fail the subrequest
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_FAILED
       WHERE id = sr.srId;
      UPDATE DiskCopy
         SET status = decode(status, dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_FAILED,
                                     dconst.DISKCOPY_WAITFS_SCHEDULING, dconst.DISKCOPY_FAILED,
                                     dconst.DISKCOPY_INVALID)
       WHERE castorfile = sr.cfid AND status IN (dconst.DISKCOPY_STAGEOUT,
                                                 dconst.DISKCOPY_WAITFS,
                                                 dconst.DISKCOPY_WAITFS_SCHEDULING);
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, 0, '');
    WHEN abortedSRstatus = dconst.SUBREQUEST_FAILED
      OR abortedSRstatus = dconst.SUBREQUEST_FAILED_FINISHED THEN
      -- subrequest has failed, nothing to abort
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, serrno.EINVAL, 'Cannot abort failed subRequest');
    WHEN abortedSRstatus = dconst.SUBREQUEST_FINISHED
      OR abortedSRstatus = dconst.SUBREQUEST_ARCHIVED THEN
      -- subrequest is over, nothing to abort
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, serrno.EINVAL, 'Cannot abort completed subRequest');
    ELSE
      -- unknown status !
      INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
      VALUES (sr.fileId, sr.nsHost, serrno.SEINTERNAL, 'Found unknown status for request : ' || TO_CHAR(abortedSRstatus));
  END CASE;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- subRequest was deleted in the mean time !
  INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
  VALUES (sr.fileId, sr.nsHost, serrno.ENOENT, 'Targeted SubRequest has just been deleted');
END;
/

/* PL/SQL method to process bulk abort on a given Repack request */
CREATE OR REPLACE PROCEDURE processBulkAbortForRepack(origReqId IN INTEGER) AS
  abortedSRstatus INTEGER := -1;
  srsToUpdate "numList";
  dcmigrsToUpdate "numList";
  nbItems INTEGER;
  nbItemsDone INTEGER := 0;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  cfId INTEGER;
  srId INTEGER;
  firstOne BOOLEAN := TRUE;
  commitWork BOOLEAN := FALSE;
  varOriginalVID VARCHAR2(2048);
BEGIN
  -- get the VID of the aborted repack request
  SELECT repackVID INTO varOriginalVID FROM StageRepackRequest WHERE id = origReqId;
  -- Gather the list of subrequests to abort
  INSERT INTO ProcessBulkAbortFileReqsHelper (srId, cfId, fileId, nsHost, uuid) (
    SELECT /*+ INDEX(Subrequest I_Subrequest_CastorFile)*/
           SubRequest.id, CastorFile.id, CastorFile.fileId, CastorFile.nsHost, SubRequest.subreqId
      FROM SubRequest, CastorFile
     WHERE SubRequest.castorFile = CastorFile.id
       AND request = origReqId
       AND status IN (dconst.SUBREQUEST_START, dconst.SUBREQUEST_RESTART, dconst.SUBREQUEST_RETRY,
                      dconst.SUBREQUEST_WAITSUBREQ, dconst.SUBREQUEST_WAITTAPERECALL,
                      dconst.SUBREQUEST_REPACK));
  SELECT COUNT(*) INTO nbItems FROM processBulkAbortFileReqsHelper;
  -- handle aborts in bulk while avoiding deadlocks
  WHILE nbItems > 0 LOOP
    FOR sr IN (SELECT srId, cfId, fileId, nsHost, uuid FROM processBulkAbortFileReqsHelper) LOOP
      BEGIN
        IF firstOne THEN
          -- on the first item, we take a blocking lock as we are sure that we will not
          -- deadlock and we would like to process at least one item to not loop endlessly
          SELECT id INTO cfId FROM CastorFile WHERE id = sr.cfId FOR UPDATE;
          firstOne := FALSE;
        ELSE
          -- on the other items, we go for a non blocking lock. If we get it, that's
          -- good and we process this extra subrequest within the same session. If
          -- we do not get the lock, then we close the session here and go for a new
          -- one. This will prevent dead locks while ensuring that a minimal number of
          -- commits is performed.
          SELECT id INTO cfId FROM CastorFile WHERE id = sr.cfId FOR UPDATE NOWAIT;
        END IF;
        -- note the revalidation of the status and even of the existence of the subrequest
        -- as it may have changed before we got the lock on the Castorfile in processBulkAbortFileReqs
        SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ status
          INTO abortedSRstatus
          FROM SubRequest
         WHERE id = sr.srId;
        CASE
          WHEN abortedSRstatus = dconst.SUBREQUEST_START
            OR abortedSRstatus = dconst.SUBREQUEST_RESTART
            OR abortedSRstatus = dconst.SUBREQUEST_RETRY
            OR abortedSRstatus = dconst.SUBREQUEST_WAITSUBREQ THEN
            -- easy case, we only have to fail the subrequest
            INSERT INTO ProcessRepackAbortHelperSR (srId) VALUES (sr.srId);
          WHEN abortedSRstatus = dconst.SUBREQUEST_WAITTAPERECALL THEN
            -- recall case, fail the subRequest and cancel the recall if needed
            failRecallSubReq(sr.srId, sr.cfId);
          WHEN abortedSRstatus = dconst.SUBREQUEST_REPACK THEN
            -- trigger the update the subrequest status to FAILED
            INSERT INTO ProcessRepackAbortHelperSR (srId) VALUES (sr.srId);
            -- delete migration jobs of this repack, hence stopping selectively the migrations
            DELETE FROM MigrationJob WHERE castorfile = sr.cfId AND originalVID = varOriginalVID;
            -- delete migrated segments if no migration jobs remain
            BEGIN
              SELECT id INTO cfId FROM MigrationJob WHERE castorfile = sr.cfId AND ROWNUM < 2;
            EXCEPTION WHEN NO_DATA_FOUND THEN
              DELETE FROM MigratedSegment WHERE castorfile = sr.cfId;
              -- trigger the update of the DiskCopy to STAGED as no more migrations remain
              INSERT INTO ProcessRepackAbortHelperDCmigr (cfId) VALUES (sr.cfId);
            END;
           WHEN abortedSRstatus IN (dconst.SUBREQUEST_FAILED,
                                    dconst.SUBREQUEST_FINISHED,
                                    dconst.SUBREQUEST_FAILED_FINISHED,
                                    dconst.SUBREQUEST_ARCHIVED) THEN
             -- nothing to be done here
             NULL;
        END CASE;
        DELETE FROM processBulkAbortFileReqsHelper WHERE srId = sr.srId;
        nbItemsDone := nbItemsDone + 1;
      EXCEPTION WHEN SrLocked THEN
        commitWork := TRUE;
      END;
      -- commit anyway from time to time, to avoid too long redo logs
      IF commitWork OR nbItemsDone >= 1000 THEN
        -- exit the current loop and restart a new one, in order to commit without getting invalid ROWID errors
        EXIT;
      END IF;
    END LOOP;
    -- do the bulk updates
    SELECT srId BULK COLLECT INTO srsToUpdate FROM ProcessRepackAbortHelperSR;
    FORALL i IN 1 .. srsToUpdate.COUNT
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET parent = NULL, diskCopy = NULL, lastModificationTime = getTime(),
             status = dconst.SUBREQUEST_FAILED_FINISHED,
             errorCode = 1701, errorMessage = 'Aborted explicitely'  -- ESTCLEARED
       WHERE id = srsToUpdate(i);
    SELECT cfId BULK COLLECT INTO dcmigrsToUpdate FROM ProcessRepackAbortHelperDCmigr;
    FORALL i IN 1 .. dcmigrsToUpdate.COUNT
      UPDATE DiskCopy SET status = dconst.DISKCOPY_STAGED
       WHERE castorfile = dcmigrsToUpdate(i) AND status = dconst.DISKCOPY_CANBEMIGR;
    -- commit
    COMMIT;
    -- reset all counters
    nbItems := nbItems - nbItemsDone;
    nbItemsDone := 0;
    firstOne := TRUE;
    commitWork := FALSE;
  END LOOP;
  -- make sure to archive if actually nothing was done
  IF abortedSRstatus = -1 THEN
    BEGIN
      SELECT id, status INTO srId, abortedSRstatus
        FROM SubRequest
       WHERE request = origReqId
         AND status IN (dconst.SUBREQUEST_FINISHED, dconst.SUBREQUEST_FAILED_FINISHED)
         AND ROWNUM = 1;
      -- This procedure should really be called 'terminateSubReqAndArchiveRequest', and this is
      -- why we call it here: we need to trigger the logic to mark the whole request and all of its subrequests
      -- as ARCHIVED, so that they are cleaned up afterwards. Note that this is effectively
      -- a no-op for the status change of the single fetched SubRequest.
      archiveSubReq(srId, abortedSRstatus);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Should never happen, anyway ignore as there's nothing else to do
      NULL;
    END;
  END IF;
  COMMIT;
END;
/

/* PL/SQL method to process bulk abort on files related requests */
CREATE OR REPLACE PROCEDURE processBulkAbortFileReqs
(origReqId IN INTEGER, fileIds IN "numList", nsHosts IN strListTable, reqType IN NUMBER) AS
  nbItems NUMBER;
  nbItemsDone NUMBER := 0;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  unused NUMBER;
  firstOne BOOLEAN := TRUE;
  commitWork BOOLEAN := FALSE;
BEGIN
  -- Gather the list of subrequests to abort
  IF fileIds.count() = 0 THEN
    -- handle the case of an empty request, meaning that all files should be aborted
    INSERT INTO ProcessBulkAbortFileReqsHelper (srId, cfId, fileId, nsHost, uuid) (
      SELECT /*+ INDEX(Subrequest I_Subrequest_Request)*/
             SubRequest.id, CastorFile.id, CastorFile.fileId, CastorFile.nsHost, SubRequest.subreqId
        FROM SubRequest, CastorFile
       WHERE SubRequest.castorFile = CastorFile.id
         AND request = origReqId);
  ELSE
    -- handle the case of selective abort
    FOR i IN fileIds.FIRST .. fileIds.LAST LOOP
      DECLARE
        srId NUMBER;
        cfId NUMBER;
        srUuid VARCHAR(2048);
      BEGIN
        SELECT /*+ INDEX(Subrequest I_Subrequest_CastorFile)*/
               SubRequest.id, CastorFile.id, SubRequest.subreqId INTO srId, cfId, srUuid
          FROM SubRequest, CastorFile
         WHERE request = origReqId
           AND SubRequest.castorFile = CastorFile.id
           AND CastorFile.fileid = fileIds(i)
           AND CastorFile.nsHost = nsHosts(i);
        INSERT INTO processBulkAbortFileReqsHelper (srId, cfId, fileId, nsHost, uuid)
        VALUES (srId, cfId, fileIds(i), nsHosts(i), srUuid);
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- this fileid/nshost did not exist in the request, send an error back
        INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
        VALUES (fileIds(i), nsHosts(i), serrno.ENOENT, 'No subRequest found for this fileId/nsHost');
      END;
    END LOOP;
  END IF;
  SELECT COUNT(*) INTO nbItems FROM processBulkAbortFileReqsHelper;
  -- handle aborts in bulk while avoiding deadlocks
  WHILE nbItems > 0 LOOP
    FOR sr IN (SELECT srId, cfId, fileId, nsHost, uuid FROM processBulkAbortFileReqsHelper) LOOP
      BEGIN
        IF firstOne THEN
          -- on the first item, we take a blocking lock as we are sure that we will not
          -- deadlock and we would like to process at least one item to not loop endlessly
          SELECT id INTO unused FROM CastorFile WHERE id = sr.cfId FOR UPDATE;
          firstOne := FALSE;
        ELSE
          -- on the other items, we go for a non blocking lock. If we get it, that's
          -- good and we process this extra subrequest within the same session. If
          -- we do not get the lock, then we close the session here and go for a new
          -- one. This will prevent dead locks while ensuring that a minimal number of
          -- commits is performed.
          SELECT id INTO unused FROM CastorFile WHERE id = sr.cfId FOR UPDATE NOWAIT;
        END IF;
        -- we got the lock on the Castorfile, we can handle the abort for this subrequest
        CASE reqType
          WHEN 1 THEN processAbortForGet(sr);
          WHEN 2 THEN processAbortForPut(sr);
        END CASE;
        DELETE FROM processBulkAbortFileReqsHelper WHERE srId = sr.srId;
        -- make the scheduler aware so that it can remove the transfer from the queues if needed
        INSERT INTO TransfersToAbort (uuid) VALUES (sr.uuid);
        nbItemsDone := nbItemsDone + 1;
      EXCEPTION WHEN SrLocked THEN
        commitWork := TRUE;
      END;
      -- commit anyway from time to time, to avoid too long redo logs
      IF commitWork OR nbItemsDone >= 1000 THEN
        -- exit the current loop and restart a new one, in order to commit without getting invalid ROWID errors
        EXIT;
      END IF;
    END LOOP;
    -- commit
    COMMIT;
    -- wake up the scheduler so that it can remove the transfer from the queues
    DBMS_ALERT.SIGNAL('transfersToAbort', '');
    -- reset all counters
    nbItems := nbItems - nbItemsDone;
    nbItemsDone := 0;
    firstOne := TRUE;
    commitWork := FALSE;
  END LOOP;
END;
/

/* PL/SQL method to process bulk abort requests */
CREATE OR REPLACE PROCEDURE processBulkAbort(abortReqId IN INTEGER, rIpAddress OUT INTEGER,
                                             rport OUT INTEGER, rReqUuid OUT VARCHAR2) AS
  clientId NUMBER;
  reqType NUMBER;
  requestId NUMBER;
  abortedReqUuid VARCHAR(2048);
  fileIds "numList";
  nsHosts strListTable;
  ids "numList";
  nsHostName VARCHAR2(2048);
BEGIN
  -- get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', '');
  -- get request and client informations and drop them from the DB
  DELETE FROM StageAbortRequest WHERE id = abortReqId
    RETURNING reqId, parentUuid, client INTO rReqUuid, abortedReqUuid, clientId;
  DELETE FROM Client WHERE id = clientId
    RETURNING ipAddress, port INTO rIpAddress, rport;
  -- list fileids to process and drop them from the DB; override the
  -- nsHost in case it is defined in the configuration
  SELECT fileid, decode(nsHostName, '', nsHost, nsHostName), id
    BULK COLLECT INTO fileIds, nsHosts, ids
    FROM NsFileId WHERE request = abortReqId;
  FORALL i IN 1 .. ids.COUNT DELETE FROM NsFileId WHERE id = ids(i);
  -- dispatch actual processing depending on request type
  BEGIN
    SELECT rType, id INTO reqType, requestId FROM
      (SELECT /*+ INDEX(StageGetRequest I_StageGetRequest_ReqId) */
              reqId, id, 1 as rtype from StageGetRequest UNION ALL
       SELECT /*+ INDEX(StagePrepareToGetRequest I_StagePTGRequest_ReqId) */
              reqId, id, 1 as rtype from StagePrepareToGetRequest UNION ALL
       SELECT /*+ INDEX(stagePutRequest I_stagePutRequest_ReqId) */
              reqId, id, 2 as rtype from StagePutRequest UNION ALL
       SELECT /*+ INDEX(StagePrepareToPutRequest I_StagePTPRequest_ReqId) */
              reqId, id, 2 as rtype from StagePrepareToPutRequest UNION ALL
       SELECT /*+ INDEX(StageRepackRequest I_RepackRequest_ReqId) */
              reqId, id, 3 as rtype from StageRepackRequest)
     WHERE reqId = abortedReqUuid;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- abort on non supported request type
    INSERT INTO ProcessBulkRequestHelper (fileId, nsHost, errorCode, errorMessage)
    VALUES (0, '', serrno.ENOENT, 'Request not found, or abort not supported for this request type');
    RETURN;
  END;
  IF reqType IN (1,2) THEN
    processBulkAbortFileReqs(requestId, fileIds, nsHosts, reqType);
  ELSE
    processBulkAbortForRepack(requestId);
  END IF;
END;
/

/* PL/SQL method to process bulk requests */
CREATE OR REPLACE PROCEDURE processBulkRequest(service IN VARCHAR2, requestId OUT INTEGER,
                                               rtype OUT INTEGER, rIpAddress OUT INTEGER,
                                               rport OUT INTEGER, rReqUuid OUT VARCHAR2,
                                               reuid OUT INTEGER, regid OUT INTEGER,
                                               freeParam OUT VARCHAR2,
                                               rSubResults OUT castor.FileResult_Cur) AS
  CURSOR Rcur IS SELECT /*+ FIRST_ROWS(10) */ id
                   FROM NewRequests
                  WHERE type IN (
                    SELECT type FROM Type2Obj
                     WHERE svcHandler = service
                       AND svcHandler IS NOT NULL);
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
BEGIN
  -- in case we do not find anything, rtype should be 0
  rType := 0;
  OPEN Rcur;
  -- Loop on candidates until we can lock one
  LOOP
    -- Fetch next candidate
    FETCH Rcur INTO requestId;
    EXIT WHEN Rcur%NOTFOUND;
    BEGIN
      -- Try to take a lock on the current candidate
      SELECT type INTO rType FROM NewRequests WHERE id = requestId FOR UPDATE NOWAIT;
      -- Since we are here, we got the lock. We have our winner,
      DELETE FROM NewRequests WHERE id = requestId;
      -- Clear the temporary table for subresults
      DELETE FROM ProcessBulkRequestHelper;
      -- dispatch actual processing depending on request type
      CASE rType
        WHEN 50 THEN -- Abort Request
          processBulkAbort(requestId, rIpAddress, rport, rReqUuid);
          reuid := -1;  -- not used
          regid := -1;  -- not used
      END CASE;
      -- open cursor on results
      OPEN rSubResults FOR
        SELECT fileId, nsHost, errorCode, errorMessage FROM ProcessBulkRequestHelper;
      -- and exit the loop
      EXIT;
    EXCEPTION
      WHEN NO_DATA_FOUND THEN
        -- Got to next candidate, this request was processed already and disappeared
        NULL;
      WHEN SrLocked THEN
        -- Go to next candidate, this request is being processed by another thread
        NULL;
    END;
  END LOOP;
  CLOSE Rcur;
END;
/

/* PL/SQL method to get the next failed SubRequest to do according to the given service */
/* the service parameter is not used now, it will with the new stager */
CREATE OR REPLACE PROCEDURE subRequestFailedToDo(srId OUT NUMBER, srFileName OUT VARCHAR2, srSubReqId OUT VARCHAR2,
                                                 srErrorCode OUT INTEGER, srErrorMessage OUT VARCHAR2, rReqId OUT VARCHAR2,
                                                 clIpAddress OUT INTEGER, clPort OUT INTEGER, clVersion OUT INTEGER,
                                                 srFileId OUT NUMBER) AS
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  CURSOR c IS
     SELECT /*+ FIRST_ROWS(10) INDEX(SR I_SubRequest_RT_CT_ID) */ SR.id
       FROM SubRequest PARTITION (P_STATUS_7) SR; -- FAILED
  varSRId NUMBER;
  varCFId NUMBER;
  varRId NUMBER;
  varSrAnswered INTEGER;
  varRName VARCHAR2(100);
  varClientId NUMBER;
BEGIN
  OPEN c;
  LOOP
    FETCH c INTO varSRId;
    EXIT WHEN c%NOTFOUND;
    BEGIN
      SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ answered INTO varSrAnswered
        FROM SubRequest PARTITION (P_STATUS_7)
       WHERE id = varSRId FOR UPDATE NOWAIT;
      IF varSrAnswered = 1 THEN
        -- already answered, archive and move on
        archiveSubReq(varSRId, dconst.SUBREQUEST_FAILED_FINISHED);
        -- release the lock on this request as it's completed
        COMMIT;
      ELSE
        -- we got our subrequest, select all relevant data and hold the lock
        SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ fileName, subReqId, errorCode, errorMessage,
          (SELECT object FROM Type2Obj WHERE type = reqType), request, castorFile
          INTO srFileName, srSubReqId, srErrorCode, srErrorMessage, varRName, varRId, varCFId
          FROM SubRequest
         WHERE id = varSRId;
        srId := varSRId;
        srFileId := 0;
        BEGIN
          CASE
            WHEN varRName = 'StagePrepareToPutRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StagePrepareToPutRequest WHERE id = varRId;
            WHEN varRName = 'StagePrepareToGetRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StagePrepareToGetRequest WHERE id = varRId;
            WHEN varRName = 'StagePrepareToUpdateRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StagePrepareToUpdateRequest WHERE id = varRId;
            WHEN varRName = 'StageRepackRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StageRepackRequest WHERE id = varRId;
            WHEN varRName = 'StagePutRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StagePutRequest WHERE id = varRId;
            WHEN varRName = 'StageGetRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StageGetRequest WHERE id = varRId;
            WHEN varRName = 'StageUpdateRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StageUpdateRequest WHERE id = varRId;
            WHEN varRName = 'StagePutDoneRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StagePutDoneRequest WHERE id = varRId;
            WHEN varRName = 'StageRmRequest' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM StageRmRequest WHERE id = varRId;
            WHEN varRName = 'SetFileGCWeight' THEN
              SELECT reqId, client
                INTO rReqId, varClientId
                FROM SetFileGCWeight WHERE id = varRId;
            ELSE
              -- Unsupported request type, should never happen
              RAISE NO_DATA_FOUND;
          END CASE;
          SELECT ipAddress, port, version
            INTO clIpAddress, clPort, clVersion
            FROM Client WHERE id = varClientId;
          IF varCFId > 0 THEN
            SELECT fileId INTO srFileId FROM CastorFile WHERE id = varCFId;
          END IF;
          EXIT;
        EXCEPTION WHEN NO_DATA_FOUND THEN
          -- This should never happen, we have either an orphaned subrequest
          -- or a request with an unsupported type.
          -- As we couldn't get the client, we just archive and move on.
          -- XXX For next version, call logToDLF() instead of silently archive.
          srId := 0;
          archiveSubReq(varSRId, dconst.SUBREQUEST_FAILED_FINISHED);
          COMMIT;
        END;
      END IF;
    EXCEPTION
      WHEN NO_DATA_FOUND THEN
        -- Go to next candidate, this subrequest was processed already and its status changed
        NULL;
      WHEN SrLocked THEN
        -- Go to next candidate, this subrequest is being processed by another thread
        NULL;
    END;
  END LOOP;
  CLOSE c;
END;
/


/* PL/SQL method to get the next request to do according to the given service */
CREATE OR REPLACE PROCEDURE requestToDo(service IN VARCHAR2, rId OUT INTEGER, rType OUT INTEGER) AS
BEGIN
  DELETE FROM NewRequests
   WHERE type IN (
     SELECT type FROM Type2Obj
      WHERE svcHandler = service
        AND svcHandler IS NOT NULL
     )
   AND ROWNUM < 2 RETURNING id, type INTO rId, rType;
EXCEPTION WHEN NO_DATA_FOUND THEN
  rId := 0;   -- nothing to do
  rType := 0;
END;
/


/* PL/SQL method to make a SubRequest wait on another one, linked to the given DiskCopy */
CREATE OR REPLACE PROCEDURE makeSubRequestWait(srId IN INTEGER, dci IN INTEGER) AS
BEGIN
  -- all wait on the original one; also prevent to wait on a PrepareToPut, for the
  -- case when Updates and Puts come after a PrepareToPut and they need to wait on
  -- the first Update|Put to complete.
  -- Cf. recreateCastorFile and the DiskCopy statuses 5 and 11
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET parent = (SELECT SubRequest.id
                     FROM SubRequest, DiskCopy
                    WHERE SubRequest.diskCopy = DiskCopy.id
                      AND DiskCopy.id = dci
                      AND SubRequest.reqType <> 37  -- OBJ_PrepareToPut
                      AND DiskCopy.status IN (dconst.DISKCOPY_WAITDISK2DISKCOPY, dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_STAGEOUT, dconst.DISKCOPY_WAITFS_SCHEDULING)
                      AND SubRequest.status IN (dconst.SUBREQUEST_WAITTAPERECALL, dconst.SUBREQUEST_READYFORSCHED, dconst.SUBREQUEST_READY)),
        status = 5, -- WAITSUBREQ
        lastModificationTime = getTime()
  WHERE SubRequest.id = srId;
END;
/


/* PL/SQL method to archive a SubRequest */
CREATE OR REPLACE PROCEDURE archiveSubReq(srId IN INTEGER, finalStatus IN INTEGER) AS
  unused INTEGER;
  rId INTEGER;
  rName VARCHAR2(100);
  rType NUMBER := 0;
  clientId INTEGER;
BEGIN
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id) */ SubRequest
     SET parent = NULL, diskCopy = NULL,  -- unlink this subrequest as it's dead now
         lastModificationTime = getTime(),
         status = finalStatus
   WHERE id = srId
   RETURNING request, reqType, (SELECT object FROM Type2Obj WHERE type = reqType) INTO rId, rType, rName;
  -- Try to see whether another subrequest in the same
  -- request is still being processed. For this, we
  -- need a master lock on the request.
  EXECUTE IMMEDIATE
    'BEGIN SELECT client INTO :clientId FROM '|| rName ||' WHERE id = :rId FOR UPDATE; END;'
    USING OUT clientId, IN rId;
  BEGIN
    -- note the decode trick to use the dedicated index I_SubRequest_Req_Stat_no89
    SELECT request INTO unused FROM SubRequest
     WHERE request = rId AND decode(status,8,NULL,9,NULL,status) IS NOT NULL
       AND ROWNUM < 2;  -- all but {FAILED_,}FINISHED
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- All subrequests have finished, we can archive:
    -- drop the associated Client entity
    DELETE FROM Client WHERE id = clientId;
    -- archive the successful subrequests
    UPDATE /*+ INDEX(SubRequest I_SubRequest_Request) */ SubRequest
       SET status = dconst.SUBREQUEST_ARCHIVED
     WHERE request = rId
       AND status = dconst.SUBREQUEST_FINISHED;
    -- in case of repack, change the status of the request
    IF rType = 119 THEN  -- OBJ_StageRepackRequest
      DECLARE
        nbfailures NUMBER;
      BEGIN
        SELECT count(*) INTO nbfailures FROM SubRequest
         WHERE request = rId
           AND status = dconst.SUBREQUEST_FAILED_FINISHED
           AND ROWNUM < 2;
        UPDATE StageRepackRequest
           SET status = CASE nbfailures WHEN 1 THEN tconst.REPACK_FAILED ELSE tconst.REPACK_FINISHED END,
               lastModificationTime = getTime()
         WHERE id = rId;
      END;
    END IF;
  END;
END;
/


/* PL/SQL method checking whether a given service class
 * is declared disk only and had only full diskpools.
 * Returns 1 in such a case, 0 else
 */
CREATE OR REPLACE FUNCTION checkFailJobsWhenNoSpace(svcClassId NUMBER)
RETURN NUMBER AS
  failJobsFlag NUMBER;
  defFileSize NUMBER;
  c NUMBER;
  availSpace NUMBER;
  reservedSpace NUMBER;
BEGIN
  -- Determine if the service class is D1 and the default
  -- file size. If the default file size is 0 we assume 2G
  SELECT failJobsWhenNoSpace,
         decode(defaultFileSize, 0, 2000000000, defaultFileSize)
    INTO failJobsFlag, defFileSize
    FROM SvcClass
   WHERE id = svcClassId;
  -- Check that the pool has space, taking into account current
  -- availability and space reserved by Put requests in the queue
  IF (failJobsFlag = 1) THEN
    SELECT count(*), sum(free - totalSize * minAllowedFreeSpace)
      INTO c, availSpace
      FROM DiskPool2SvcClass, FileSystem, DiskServer
     WHERE DiskPool2SvcClass.child = svcClassId
       AND DiskPool2SvcClass.parent = FileSystem.diskPool
       AND FileSystem.diskServer = DiskServer.id
       AND FileSystem.status = dconst.FILESYSTEM_PRODUCTION
       AND DiskServer.status = dconst.DISKSERVER_PRODUCTION
       AND DiskServer.hwOnline = 1
       AND totalSize * minAllowedFreeSpace < free - defFileSize;
    IF (c = 0) THEN
      RETURN 1;
    END IF;
  END IF;
  RETURN 0;
END;
/

/* PL/SQL method checking whether we have an existing routing for this service class and file class.
 * Returns 1 in case we do not have such a routing, 0 else
 */
CREATE OR REPLACE FUNCTION checkNoTapeRouting(fileClassId NUMBER)
RETURN NUMBER AS
  nbTCs INTEGER;
  varTpId INTEGER;
BEGIN
  -- get number of copies on tape requested by this file
  SELECT nbCopies INTO nbTCs
    FROM FileClass WHERE id = fileClassId;
  -- loop over the copies and check the routing of each of them
  FOR i IN 1..nbTCs LOOP
    SELECT tapePool INTO varTpId FROM MigrationRouting
     WHERE fileClass = fileClassId
       AND copyNb = i
       AND ROWNUM < 2;
  END LOOP;
  -- all routes could be found. Everything is ok
  RETURN 0;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- no route for at least one copy
  RETURN 1;
END;
/


/* PL/SQL method implementing getBestDiskCopyToReplicate. */
CREATE OR REPLACE PROCEDURE getBestDiskCopyToReplicate
  (cfId IN NUMBER, reuid IN NUMBER, regid IN NUMBER, internal IN NUMBER,
   destSvcClassId IN NUMBER, dcId OUT NUMBER, srcSvcClassId OUT NUMBER) AS
  destSvcClass VARCHAR2(2048);
BEGIN
  -- Select the best diskcopy available to replicate and for which the user has
  -- access too.
  SELECT id, srcSvcClassId INTO dcId, srcSvcClassId
    FROM (
      SELECT DiskCopy.id, SvcClass.id srcSvcClassId
        FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass, SvcClass
       WHERE DiskCopy.castorfile = cfId
         AND DiskCopy.status IN (0, 10) -- STAGED, CANBEMIGR
         AND FileSystem.id = DiskCopy.fileSystem
         AND FileSystem.diskpool = DiskPool2SvcClass.parent
         AND DiskPool2SvcClass.child = SvcClass.id
         AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING, dconst.FILESYSTEM_READONLY)
         AND DiskServer.id = FileSystem.diskserver
         AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING, dconst.DISKSERVER_READONLY)
         AND DiskServer.hwOnline = 1
         -- If this is an internal replication request make sure that the diskcopy
         -- is in the same service class as the source.
         AND (SvcClass.id = destSvcClassId OR internal = 0)
         -- Check that the user has the necessary access rights to replicate a
         -- file from the source service class. Note: instead of using a
         -- StageGetRequest type here we use a StagDiskCopyReplicaRequest type
         -- to be able to distinguish between and read and replication requst.
         AND checkPermission(SvcClass.name, reuid, regid, 133) = 0
         AND NOT EXISTS (
           -- Don't select source diskcopies which already failed more than 10 times
           SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */ 'x'
             FROM StageDiskCopyReplicaRequest R, SubRequest
            WHERE SubRequest.request = R.id
              AND R.sourceDiskCopy = DiskCopy.id
              AND SubRequest.status = 9 -- FAILED_FINISHED
           HAVING COUNT(*) >= 10)
       ORDER BY FileSystemRate(FileSystem.nbReadStreams, FileSystem.nbWriteStreams) DESC,
                DBMS_Random.value)
   WHERE ROWNUM < 2;
EXCEPTION WHEN NO_DATA_FOUND THEN
  RAISE; -- No diskcopy found that could be replicated
END;
/


/* PL/SQL method implementing getBestDiskCopyToRead used to return the
 * best location of a file based on monitoring information. This is
 * useful for xrootd so that it can avoid scheduling reads
 */
CREATE OR REPLACE PROCEDURE getBestDiskCopyToRead(cfId IN NUMBER,
                                                  svcClassId IN NUMBER,
                                                  diskServer OUT VARCHAR2,
                                                  filePath OUT VARCHAR2) AS
BEGIN
  -- Select best diskcopy
  SELECT name, path INTO diskServer, filePath FROM (
    SELECT DiskServer.name, FileSystem.mountpoint || DiskCopy.path AS path
      FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
     WHERE DiskCopy.castorfile = cfId
       AND DiskCopy.fileSystem = FileSystem.id
       AND FileSystem.diskpool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = svcClassId
       AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
       AND FileSystem.diskserver = DiskServer.id
       AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
       AND DiskServer.hwOnline = 1
       AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_STAGEOUT, dconst.DISKCOPY_CANBEMIGR)
     ORDER BY FileSystemRate(FileSystem.nbReadStreams, FileSystem.nbWriteStreams) DESC,
              DBMS_Random.value)
   WHERE rownum < 2;
EXCEPTION WHEN NO_DATA_FOUND THEN
  RAISE; -- No file found to be read
END;
/


/* PL/SQL method implementing checkForD2DCopyOrRecall
 * dcId is the DiskCopy id of the best candidate for replica, 0 if none is found (tape recall), -1 in case of user error
 * Internally used by getDiskCopiesForJob and processPrepareRequest
 */
CREATE OR REPLACE PROCEDURE checkForD2DCopyOrRecall(cfId IN NUMBER, srId IN NUMBER, reuid IN NUMBER, regid IN NUMBER,
                                                    svcClassId IN NUMBER, dcId OUT NUMBER, srcSvcClassId OUT NUMBER) AS
  destSvcClass VARCHAR2(2048);
  userid NUMBER := reuid;
  groupid NUMBER := regid;
BEGIN
  -- First check whether we are a disk only pool that is already full.
  -- In such a case, we should fail the request with an ENOSPACE error
  IF (checkFailJobsWhenNoSpace(svcClassId) = 1) THEN
    dcId := -1;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = serrno.ENOSPC, -- No space left on device
           errorMessage = 'File creation canceled since diskPool is full'
     WHERE id = srId;
    RETURN;
  END IF;
  -- Resolve the destination service class id to a name
  SELECT name INTO destSvcClass FROM SvcClass WHERE id = svcClassId;
  -- Determine if there are any copies of the file in the same service class
  -- on non PRODUCTION hardware. If we found something then set the user
  -- and group id to -1 this effectively disables the later privilege checks
  -- to see if the user can trigger a d2d or recall. (#55745)
  BEGIN
    SELECT -1, -1 INTO userid, groupid
      FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
     WHERE DiskCopy.fileSystem = FileSystem.id
       AND DiskCopy.castorFile = cfId
       AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR)
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = svcClassId
       AND FileSystem.diskServer = DiskServer.id
       AND (DiskServer.status != dconst.DISKSERVER_PRODUCTION
        OR  FileSystem.status != dconst.FILESYSTEM_PRODUCTION)
       AND ROWNUM < 2;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    NULL;  -- Nothing
  END;
  -- If we are in this procedure then we did not find a copy of the
  -- file in the target service class that could be used. So, we check
  -- to see if the user has the rights to create a file in the destination
  -- service class. I.e. check for StagePutRequest access rights
  IF checkPermission(destSvcClass, userid, groupid, 40) != 0 THEN
    -- Fail the subrequest and notify the client
    dcId := -1;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = serrno.EACCES,
           errorMessage = 'Insufficient user privileges to trigger a tape recall or file replication to the '''||destSvcClass||''' service class'
     WHERE id = srId;
    RETURN;
  END IF;
  -- Try to find a diskcopy to replicate
  getBestDiskCopyToReplicate(cfId, userid, groupid, 0, svcClassId, dcId, srcSvcClassId);
  -- We found at least one, therefore we schedule a disk2disk
  -- copy from the existing diskcopy not available to this svcclass
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- We found no diskcopies at all. We should not schedule
  -- and make a tape recall... except ... in 4 cases :
  --   - if there is some temporarily unavailable diskcopy
  --     that is in CANBEMIGR or STAGEOUT
  -- in such a case, what we have is an existing file, that
  -- was migrated, then overwritten but never migrated again.
  -- So the unavailable diskCopy is the only copy that is valid.
  -- We will tell the client that the file is unavailable
  -- and he/she will retry later
  --   - if we have an available STAGEOUT copy. This can happen
  -- when the copy is in a given svcclass and we were looking
  -- in another one. Since disk to disk copy is impossible in this
  -- case, the file is declared BUSY.
  --   - if we have an available WAITFS, WAITFSSCHEDULING copy in such
  -- a case, we tell the client that the file is BUSY
  --   - if we have some temporarily unavailable diskcopy(ies)
  --     that is in status STAGED and the file is disk only.
  -- In this case nothing can be recalled and the file is inaccessible
  -- until we have one of the unvailable copies back
  DECLARE
    dcStatus NUMBER;
    fsStatus NUMBER;
    dsStatus NUMBER;
    varNbCopies NUMBER;
  BEGIN
    SELECT DiskCopy.status, nvl(FileSystem.status, dconst.FILESYSTEM_PRODUCTION),
           nvl(DiskServer.status, dconst.DISKSERVER_PRODUCTION)
      INTO dcStatus, fsStatus, dsStatus
      FROM DiskCopy, FileSystem, DiskServer
     WHERE DiskCopy.castorfile = cfId
       AND DiskCopy.status IN (dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_STAGEOUT,
                               dconst.DISKCOPY_CANBEMIGR, dconst.DISKCOPY_WAITFS_SCHEDULING)
       AND FileSystem.id(+) = DiskCopy.fileSystem
       AND DiskServer.id(+) = FileSystem.diskserver
       AND ROWNUM < 2;
    -- We are in one of the 3 first special cases. Don't schedule, don't recall
    dcId := -1;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = CASE
             WHEN dcStatus IN (dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_WAITFS_SCHEDULING) THEN serrno.EBUSY
             WHEN dcStatus = dconst.DISKCOPY_STAGEOUT
               AND fsStatus IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
               AND dsStatus IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY) THEN serrno.EBUSY
             ELSE serrno.ESTNOTAVAIL -- File is currently not available
           END,
           errorMessage = CASE
             WHEN dcStatus IN (dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_WAITFS_SCHEDULING) THEN
               'File is being (re)created right now by another user'
             WHEN dcStatus = dconst.DISKCOPY_STAGEOUT
               AND fsStatus IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
               AND dsStatus IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY) THEN
               'File is being written to in another service class'
             ELSE
               'All copies of this file are unavailable for now. Please retry later'
           END
     WHERE id = srId;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- we are not in one of the 3 first special cases. Let's check the 4th one
    -- by checking whether the file is diskonly
    SELECT nbCopies INTO varNbCopies
      FROM FileClass, CastorFile
     WHERE FileClass.id = CastorFile.fileClass
       AND CastorFile.id = cfId;
    IF varNbCopies = 0 THEN
      -- we have indeed a disk only file, so fail the request
      dcId := -1;
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_FAILED,
             errorCode = serrno.ESTNOTAVAIL, -- File is currently not available
             errorMessage = 'All disk copies of this disk-only file are unavailable for now. Please retry later'
       WHERE id = srId;
    ELSE
      -- We did not find the very special case so we should recall from tape.
      -- Check whether the user has the rights to issue a tape recall to
      -- the destination service class.
      IF checkPermission(destSvcClass, userid, groupid, 161) != 0 THEN
        -- Fail the subrequest and notify the client
        dcId := -1;
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = dconst.SUBREQUEST_FAILED,
               errorCode = serrno.EACCES, -- Permission denied
               errorMessage = 'Insufficient user privileges to trigger a tape recall to the '''||destSvcClass||''' service class'
         WHERE id = srId;
      ELSE
        -- user has enough rights, green light for the recall
        dcId := 0;
      END IF;
    END IF;
  END;
END;
/


/* Build diskCopy path from fileId */
CREATE OR REPLACE PROCEDURE buildPathFromFileId(fid IN INTEGER,
                                                nsHost IN VARCHAR2,
                                                dcid IN INTEGER,
                                                path OUT VARCHAR2) AS
BEGIN
  path := TO_CHAR(MOD(fid,100),'FM09') || '/' || TO_CHAR(fid) || '@' || nsHost || '.' || TO_CHAR(dcid);
END;
/

/* parse a path to give back the FileSystem and path */
CREATE OR REPLACE PROCEDURE parsePath(inFullPath IN VARCHAR2,
                                      outFileSystem OUT INTEGER,
                                      outPath OUT VARCHAR2,
                                      outDcId OUT INTEGER) AS
  varPathPos INTEGER;
  varLastDotPos INTEGER;
  varColonPos INTEGER;
  varDiskServerName VARCHAR2(2048);
  varMountPoint VARCHAR2(2048);
BEGIN
  -- path starts after the second '/' from the end
  varPathPos := INSTR(inFullPath, '/', -1, 2);
  outPath := SUBSTR(inFullPath, varPathPos+1);
  -- DcId is the part after the last '.'
  varLastDotPos := INSTR(inFullPath, '.', -1, 1);
  outDcId := TO_NUMBER(SUBSTR(inFullPath, varLastDotPos+1));
  -- the mountPoint is between the ':' and the start of the path
  varColonPos := INSTR(inFullPath, ':', 1, 1);
  varMountPoint := SUBSTR(inFullPath, varColonPos+1, varPathPos-varColonPos);
  -- the diskserver is before the ':
  varDiskServerName := SUBSTR(inFullPath, 1, varColonPos-1);
  -- find out the filesystem Id
  SELECT FileSystem.id INTO outFileSystem
    FROM DiskServer, FileSystem
   WHERE DiskServer.name = varDiskServerName
     AND FileSystem.diskServer = DiskServer.id
     AND FileSystem.mountPoint = varMountPoint;
END;
/

/* PL/SQL method implementing createDiskCopyReplicaRequest */
CREATE OR REPLACE PROCEDURE createDiskCopyReplicaRequest
(sourceSrId IN INTEGER, sourceDcId IN INTEGER, sourceSvcId IN INTEGER,
 destSvcId IN INTEGER, ouid IN INTEGER, ogid IN INTEGER) AS
  srId NUMBER;
  cfId NUMBER;
  destDcId NUMBER;
  reqId NUMBER;
  clientId NUMBER;
  fileName VARCHAR2(2048);
  fileSize NUMBER;
  fileId NUMBER;
  nsHost VARCHAR2(2048);
  rpath VARCHAR2(2048);
  rfs VARCHAR(2048);
BEGIN
  -- Extract the castorfile associated with the request, this is needed to
  -- create the StageDiskCopyReplicaRequest's diskcopy and subrequest entries.
  -- Also for disk2disk copying across service classes make the originating
  -- subrequest wait on the completion of the transfer.
  IF sourceSrId > 0 THEN
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = 5, parent = ids_seq.nextval -- WAITSUBREQ
     WHERE id = sourceSrId
    RETURNING castorFile, parent INTO cfId, srId;
  ELSE
    SELECT castorfile INTO cfId FROM DiskCopy WHERE id = sourceDcId;
    SELECT ids_seq.nextval INTO srId FROM Dual;
  END IF;

  -- Extract CastorFile information
  SELECT fileid, nshost, filesize, lastknownfilename
    INTO fileId, nsHost, fileSize, fileName
    FROM CastorFile WHERE id = cfId;

  -- Create the Client entry.
  INSERT INTO Client (ipaddress, port, id, version, secure)
    VALUES (0, 0, ids_seq.nextval, 0, 0)
  RETURNING id INTO clientId;

  -- Create the StageDiskCopyReplicaRequest. The euid and egid values default to
  -- 0 here, this indicates the request came from the user root.
  SELECT ids_seq.nextval INTO destDcId FROM Dual;
  INSERT INTO StageDiskCopyReplicaRequest
    (svcclassname, reqid, creationtime, lastmodificationtime, id, svcclass,
     client, sourcediskcopy, destdiskcopy, sourceSvcClass)
  VALUES ((SELECT name FROM SvcClass WHERE id = destSvcId), uuidgen(), gettime(),
     gettime(), ids_seq.nextval, destSvcId, clientId, sourceDcId, destDcId, sourceSvcId)
  RETURNING id INTO reqId;

  -- Determine the requested filesystem value
  SELECT DiskServer.name || ':' || FileSystem.mountpoint INTO rfs
    FROM DiskCopy, FileSystem, DiskServer
   WHERE DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.diskServer = DiskServer.id
     AND DiskCopy.id = sourceDcId;

  -- Create the SubRequest setting the initial status to READYFORSCHED for
  -- immediate dispatching i.e no stager processing
  INSERT INTO SubRequest
    (retrycounter, filename, protocol, xsize, priority, subreqid, flags, modebits,
     creationtime, lastmodificationtime, answered, id, diskcopy, castorfile, parent,
     status, request, getnextstatus, errorcode, requestedfilesystems, svcHandler, reqType)
  VALUES (0, fileName, 'rfio', fileSize, 0, uuidgen(), 0, 0, gettime(), gettime(),
     0, srId, destDcId, cfId, 0, 13, reqId, 0, 0, rfs, 'JobReqSvc', 133); -- OBJ_StageDiskCopyReplicaRequest

  -- Create the DiskCopy without filesystem
  buildPathFromFileId(fileId, nsHost, destDcId, rpath);
  INSERT INTO DiskCopy
    (path, id, filesystem, castorfile, status, creationTime, lastAccessTime,
     gcWeight, diskCopySize, nbCopyAccesses, owneruid, ownergid)
  VALUES (rpath, destDcId, 0, cfId, 1, getTime(), getTime(), 0, fileSize, 0, ouid, ogid);  -- WAITDISK2DISKCOPY
END;
/

/* PL/SQL method implementing createEmptyFile */
CREATE OR REPLACE PROCEDURE createEmptyFile
(cfId IN NUMBER, fileId IN NUMBER, nsHost IN VARCHAR2,
 srId IN INTEGER, schedule IN INTEGER) AS
  dcPath VARCHAR2(2048);
  gcw NUMBER;
  gcwProc VARCHAR(2048);
  fsId NUMBER;
  dcId NUMBER;
  svcClassId NUMBER;
  ouid INTEGER;
  ogid INTEGER;
  fsPath VARCHAR2(2048);
BEGIN
  -- update filesize overriding any previous value
  UPDATE CastorFile SET fileSize = 0 WHERE id = cfId;
  -- get an id for our new DiskCopy
  SELECT ids_seq.nextval INTO dcId FROM DUAL;
  -- compute the DiskCopy Path
  buildPathFromFileId(fileId, nsHost, dcId, dcPath);
  -- find a fileSystem for this empty file
  SELECT id, svcClass, euid, egid, name || ':' || mountpoint
    INTO fsId, svcClassId, ouid, ogid, fsPath
    FROM (SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
                 FileSystem.id, Request.svcClass, Request.euid, Request.egid, DiskServer.name, FileSystem.mountpoint
            FROM DiskServer, FileSystem, DiskPool2SvcClass,
                 (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */
                         id, svcClass, euid, egid from StageGetRequest UNION ALL
                  SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */
                         id, svcClass, euid, egid from StagePrepareToGetRequest UNION ALL
                  SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                         id, svcClass, euid, egid from StageUpdateRequest UNION ALL
                  SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */
                         id, svcClass, euid, egid from StagePrepareToUpdateRequest) Request,
                  SubRequest
           WHERE SubRequest.id = srId
             AND Request.id = SubRequest.request
             AND Request.svcclass = DiskPool2SvcClass.child
             AND FileSystem.diskpool = DiskPool2SvcClass.parent
             AND FileSystem.status = dconst.FILESYSTEM_PRODUCTION
             AND DiskServer.id = FileSystem.diskServer
             AND DiskServer.status = dconst.DISKSERVER_PRODUCTION
             AND DiskServer.hwOnline = 1
        ORDER BY -- order by rate as defined by the function
                 fileSystemRate(FileSystem.nbReadStreams, FileSystem.nbWriteStreams) DESC,
                 -- finally use randomness to avoid preferring always the same FS
                 DBMS_Random.value)
   WHERE ROWNUM < 2;
  -- compute it's gcWeight
  gcwProc := castorGC.getRecallWeight(svcClassId);
  EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(0); END;'
    USING OUT gcw;
  -- then create the DiskCopy
  INSERT INTO DiskCopy
    (path, id, filesystem, castorfile, status,
     creationTime, lastAccessTime, gcWeight, diskCopySize, nbCopyAccesses, owneruid, ownergid)
  VALUES (dcPath, dcId, fsId, cfId, 0,   -- STAGED
          getTime(), getTime(), GCw, 0, 0, ouid, ogid);
  -- link to the SubRequest and schedule an access if requested
  IF schedule = 0 THEN
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET diskCopy = dcId WHERE id = srId;
  ELSE
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET diskCopy = dcId,
           requestedFileSystems = fsPath,
           xsize = 0, status = 13, -- READYFORSCHED
           getNextStatus = 1 -- FILESTAGED
     WHERE id = srId;
  END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
  raise_application_error(-20115, 'No suitable filesystem found for this empty file');
END;
/

/* PL/SQL method implementing replicateOnClose */
CREATE OR REPLACE PROCEDURE replicateOnClose(cfId IN NUMBER, ouid IN INTEGER, ogid IN INTEGER) AS
  unused NUMBER;
  srcDcId NUMBER;
  srcSvcClassId NUMBER;
  ignoreSvcClass NUMBER;
BEGIN
  -- Lock the castorfile
  SELECT id INTO unused FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Loop over all service classes where replication is required
  FOR a IN (SELECT SvcClass.id FROM (
              -- Determine the number of copies of the file in all service classes
              SELECT * FROM (
                SELECT SvcClass.id, count(*) available
                  FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass, SvcClass
                 WHERE DiskCopy.filesystem = FileSystem.id
                   AND DiskCopy.castorfile = cfId
                   AND FileSystem.diskpool = DiskPool2SvcClass.parent
                   AND DiskPool2SvcClass.child = SvcClass.id
                   AND DiskCopy.status IN (0, 1, 10)  -- STAGED, WAITDISK2DISKCOPY, CANBEMIGR
                   AND FileSystem.status IN
                       (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING, dconst.FILESYSTEM_READONLY)
                   AND DiskServer.id = FileSystem.diskserver
                   AND DiskServer.status IN
                       (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING, dconst.DISKSERVER_READONLY)
                 GROUP BY SvcClass.id)
             ) results, SvcClass
             -- Join the results with the service class table and determine if
             -- additional copies need to be created
             WHERE results.id = SvcClass.id
               AND SvcClass.replicateOnClose = 1
               AND results.available < SvcClass.maxReplicaNb)
  LOOP
    -- Ignore this replication if there is already a pending replication request
    -- for the given castorfile in the target/destination service class. Once
    -- the replication has gone through, this procedure will be called again.
    -- This insures that only one replication request is active at any given time
    -- and that we don't create too many replication requests that may exceed
    -- the maxReplicaNb defined on the service class
    BEGIN
      SELECT /*+ INDEX(StageDiskCopyReplicaRequest I_StageDiskCopyReplic_DestDC) */ DiskCopy.id INTO unused
        FROM DiskCopy, StageDiskCopyReplicaRequest
       WHERE StageDiskCopyReplicaRequest.destdiskcopy = DiskCopy.id
         AND StageDiskCopyReplicaRequest.svcclass = a.id
         AND DiskCopy.castorfile = cfId
         AND DiskCopy.status = 1  -- WAITDISK2DISKCOPY
         AND rownum < 2;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      BEGIN
        -- Select the best diskcopy to be replicated. Note: we force that the
        -- replication must happen internally within the service class. This
        -- prevents D2 style activities from impacting other more controlled
        -- service classes. E.g. D2 replication should not impact CDR
        getBestDiskCopyToReplicate(cfId, -1, -1, 1, a.id, srcDcId, srcSvcClassId);
        -- Trigger a replication request.
        createDiskCopyReplicaRequest(0, srcDcId, srcSvcClassId, a.id, ouid, ogid);
      EXCEPTION WHEN NO_DATA_FOUND THEN
        NULL;  -- No copies to replicate from
      END;
    END;
  END LOOP;
END;
/


/* PL/SQL method implementing getDiskCopiesForJob */
/* the result output is a DiskCopy status for STAGED, DISK2DISKCOPY, RECALL or WAITFS
   -1 for user failure, -2 or -3 for subrequest put in WAITSUBREQ */
CREATE OR REPLACE PROCEDURE getDiskCopiesForJob
        (srId IN INTEGER, result OUT INTEGER,
         sources OUT castor.DiskCopy_Cur) AS
  nbDCs INTEGER;
  nbDSs INTEGER;
  maxDCs INTEGER;
  upd INTEGER;
  dcIds "numList";
  dcStatus INTEGER;
  svcClassId NUMBER;
  srcSvcClassId NUMBER;
  cfId NUMBER;
  srcDcId NUMBER;
  d2dsrId NUMBER;
  reuid NUMBER;
  regid NUMBER;
BEGIN
  -- retrieve the castorFile and the svcClass for this subrequest
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
         SubRequest.castorFile, Request.euid, Request.egid, Request.svcClass, Request.upd
    INTO cfId, reuid, regid, svcClassId, upd
    FROM (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */
                 id, euid, egid, svcClass, 0 upd from StageGetRequest UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                 id, euid, egid, svcClass, 1 upd from StageUpdateRequest) Request,
         SubRequest
   WHERE Subrequest.request = Request.id
     AND Subrequest.id = srId;
  -- lock the castorFile to be safe in case of concurrent subrequests
  SELECT id INTO cfId FROM CastorFile WHERE id = cfId FOR UPDATE;

  -- First see whether we should wait on an ongoing request
  SELECT DiskCopy.id BULK COLLECT INTO dcIds
    FROM DiskCopy, FileSystem, DiskServer
   WHERE cfId = DiskCopy.castorFile
     AND FileSystem.id(+) = DiskCopy.fileSystem
     AND nvl(FileSystem.status, 0) IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
     AND DiskServer.id(+) = FileSystem.diskServer
     AND nvl(DiskServer.status, 0) IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
     AND nvl(DiskServer.hwOnline, 1) = 1
     AND DiskCopy.status = dconst.DISKCOPY_WAITFS_SCHEDULING;
  IF dcIds.COUNT > 0 THEN
    -- DiskCopy is in WAIT*, make SubRequest wait on previous subrequest and do not schedule
    makeSubRequestWait(srId, dcIds(1));
    result := -2;
    RETURN;
  END IF;

  -- Look for available diskcopies. The status is needed for the
  -- internal replication processing, and only if count = 1, hence
  -- the min() function does not represent anything here.
  -- Note that we accept copies in READONLY hardware here as we're processing Get
  -- and Update requests, and we would deny Updates switching to write mode in that case.
  SELECT COUNT(DiskCopy.id), min(DiskCopy.status) INTO nbDCs, dcStatus
    FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
   WHERE DiskCopy.castorfile = cfId
     AND DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.diskpool = DiskPool2SvcClass.parent
     AND DiskPool2SvcClass.child = svcClassId
     AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
     AND FileSystem.diskserver = DiskServer.id
     AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
     AND DiskServer.hwOnline = 1
     AND DiskCopy.status IN (0, 6, 10);  -- STAGED, STAGEOUT, CANBEMIGR
  IF nbDCs = 0 AND upd = 1 THEN
    -- we may be the first update inside a prepareToPut, and this is allowed
    SELECT COUNT(DiskCopy.id) INTO nbDCs
      FROM DiskCopy
     WHERE DiskCopy.castorfile = cfId
       AND DiskCopy.status = 5;  -- WAITFS
    IF nbDCs = 1 THEN
      result := 5;  -- DISKCOPY_WAITFS, try recreation
      RETURN;
      -- note that we don't do here all the needed consistency checks,
      -- but recreateCastorFile takes care of all cases and will fail
      -- the subrequest or make it wait if needed.
    END IF;
  END IF;

  IF nbDCs > 0 THEN
    -- Yes, we have some
    -- List available diskcopies for job scheduling
    OPEN sources
      FOR SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ DiskCopy.id, DiskCopy.path, DiskCopy.status,
                 FileSystemRate(FileSystem.nbReadStreams, FileSystem.nbWriteStreams) fsRate,
                 FileSystem.mountPoint, DiskServer.name
            FROM DiskCopy, SubRequest, FileSystem, DiskServer, DiskPool2SvcClass
           WHERE SubRequest.id = srId
             AND SubRequest.castorfile = DiskCopy.castorfile
             AND FileSystem.diskpool = DiskPool2SvcClass.parent
             AND DiskPool2SvcClass.child = svcClassId
             AND DiskCopy.status IN (0, 6, 10) -- STAGED, STAGEOUT, CANBEMIGR
             AND FileSystem.id = DiskCopy.fileSystem
             AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
             AND DiskServer.id = FileSystem.diskServer
             AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
             AND DiskServer.hwOnline = 1
           ORDER BY fsRate DESC;
    -- Internal replication processing
    IF upd = 1 OR (nbDCs = 1 AND dcStatus = 6) THEN -- DISKCOPY_STAGEOUT
      -- replication forbidden
      result := 0;  -- DISKCOPY_STAGED
    ELSE
      -- check whether there's already an ongoing replication
      BEGIN
        SELECT /*+ INDEX(StageDiskCopyReplicaRequest I_StageDiskCopyReplic_DestDC) */ DiskCopy.id INTO srcDcId
          FROM DiskCopy, StageDiskCopyReplicaRequest
         WHERE DiskCopy.id = StageDiskCopyReplicaRequest.destDiskCopy
           AND StageDiskCopyReplicaRequest.svcclass = svcClassId
           AND DiskCopy.castorfile = cfId
           AND DiskCopy.status = dconst.DISKCOPY_WAITDISK2DISKCOPY;
        -- found an ongoing replication, we don't trigger another one
        result := dconst.DISKCOPY_STAGED;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- ok, we can replicate; do we need to? compare total current
        -- # of diskcopies, regardless hardware availability, against maxReplicaNb.
        SELECT COUNT(*), max(maxReplicaNb) INTO nbDCs, maxDCs FROM (
          SELECT DiskCopy.id, maxReplicaNb
            FROM DiskCopy, FileSystem, DiskPool2SvcClass, SvcClass
           WHERE DiskCopy.castorfile = cfId
             AND DiskCopy.fileSystem = FileSystem.id
             AND FileSystem.diskpool = DiskPool2SvcClass.parent
             AND DiskPool2SvcClass.child = SvcClass.id
             AND SvcClass.id = svcClassId
             AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR));
        IF nbDCs < maxDCs OR maxDCs = 0 THEN
          -- we have to replicate. Do it only if we have enough
          -- available diskservers.
          SELECT COUNT(DISTINCT(DiskServer.name)) INTO nbDSs
            FROM DiskServer, FileSystem, DiskPool2SvcClass
           WHERE FileSystem.diskServer = DiskServer.id
             AND FileSystem.diskPool = DiskPool2SvcClass.parent
             AND DiskPool2SvcClass.child = svcClassId
             AND FileSystem.status = dconst.FILESYSTEM_PRODUCTION
             AND DiskServer.status = dconst.DISKSERVER_PRODUCTION
             AND DiskServer.hwOnline = 1
             AND DiskServer.id NOT IN (
               SELECT DISTINCT(DiskServer.id)
                 FROM DiskCopy, FileSystem, DiskServer
                WHERE DiskCopy.castorfile = cfId
                  AND DiskCopy.fileSystem = FileSystem.id
                  AND FileSystem.diskserver = DiskServer.id
                  AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR));
          IF nbDSs > 0 THEN
            BEGIN
              -- yes, we can replicate. Select the best candidate for replication
              srcDcId := 0;
              getBestDiskCopyToReplicate(cfId, -1, -1, 1, svcClassId, srcDcId, svcClassId);
              -- and create a replication request without waiting on it.
              createDiskCopyReplicaRequest(0, srcDcId, svcClassId, svcClassId, reuid, regid);
              -- result is different for logging purposes
              result := dconst.DISKCOPY_WAITDISK2DISKCOPY;
            EXCEPTION WHEN NO_DATA_FOUND THEN
              -- replication failed. We still go ahead with the access
              result := dconst.DISKCOPY_STAGED;
            END;
          ELSE
            -- no replication to be done
            result := dconst.DISKCOPY_STAGED;
          END IF;
        ELSE
          -- no replication to be done
          result := dconst.DISKCOPY_STAGED;
        END IF;
      END;
    END IF;   -- end internal replication processing
  ELSE
    -- No diskcopies available for this service class:
    -- first check whether there's already a disk to disk copy going on
    BEGIN
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile) INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */ SubRequest.id INTO d2dsrId
        FROM StageDiskCopyReplicaRequest Req, SubRequest
       WHERE SubRequest.request = Req.id
         AND Req.svcClass = svcClassId    -- this is the destination service class
         AND status IN (dconst.SUBREQUEST_READYFORSCHED, dconst.SUBREQUEST_READY)
         AND castorFile = cfId;
      -- found it, wait on it
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET parent = d2dsrId, status = 5  -- WAITSUBREQ
       WHERE id = srId;
      result := -2;
      RETURN;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- not found, we may have to schedule a disk to disk copy or trigger a recall
      checkForD2DCopyOrRecall(cfId, srId, reuid, regid, svcClassId, srcDcId, srcSvcClassId);
      IF srcDcId > 0 THEN
        -- create DiskCopyReplica request and make this subRequest wait on it
        createDiskCopyReplicaRequest(srId, srcDcId, srcSvcClassId, svcClassId, reuid, regid);
        result := -3; -- return code is different here for logging purposes
      ELSIF srcDcId = 0 THEN
        -- no diskcopy found at all, go for recall
        result := 2;  -- Tape Recall
      ELSE
        -- user error
        result := -1;
      END IF;
    END;
  END IF;
END;
/

/*** initMigration ***/
CREATE OR REPLACE PROCEDURE initMigration
(cfId IN INTEGER, datasize IN INTEGER, originalVID IN VARCHAR2,
 originalCopyNb IN INTEGER, destCopyNb IN INTEGER, inMJStatus IN INTEGER) AS
  varTpId INTEGER;
  varSizeThreshold INTEGER;
BEGIN
  varSizeThreshold := TO_NUMBER(getConfigOption('Migration', 'SizeThreshold', '300000000'));
  -- Find routing
  BEGIN
    SELECT tapePool INTO varTpId FROM MigrationRouting MR, CastorFile
     WHERE MR.fileClass = CastorFile.fileClass
       AND CastorFile.id = cfId
       AND MR.copyNb = destCopyNb
       AND (MR.isSmallFile = (CASE WHEN datasize < varSizeThreshold THEN 1 ELSE 0 END) OR MR.isSmallFile IS NULL);
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- No routing rule found means a user-visible error on the putDone or on the file close operation
    raise_application_error(-20100, 'Cannot find an appropriate tape routing for this file, aborting');
  END;
  -- Create tape copy and attach to the appropriate tape pool
  INSERT INTO MigrationJob (fileSize, creationTime, castorFile, originalVID, originalCopyNb, destCopyNb,
                            tapePool, nbRetries, status, mountTransactionId, id)
    VALUES (datasize, getTime(), cfId, originalVID, originalCopyNb, destCopyNb, varTpId, 0,
            inMJStatus, NULL, ids_seq.nextval);
END;
/

/* PL/SQL method internalPutDoneFunc, used by putDoneFunc.
   checks for diskcopies in STAGEOUT and creates the migration jobs
 */
CREATE OR REPLACE PROCEDURE internalPutDoneFunc (cfId IN INTEGER,
                                                 fs IN INTEGER,
                                                 context IN INTEGER,
                                                 nbTC IN INTEGER,
                                                 svcClassId IN INTEGER) AS
  tcId INTEGER;
  dcId INTEGER;
  gcwProc VARCHAR2(2048);
  gcw NUMBER;
  ouid INTEGER;
  ogid INTEGER;
BEGIN
  -- get function to use for computing the gc weight of the brand new diskCopy
  gcwProc := castorGC.getUserWeight(svcClassId);
  -- if no tape copy or 0 length file, no migration
  -- so we go directly to status STAGED
  IF nbTC = 0 OR fs = 0 THEN
    EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:fs, 0); END;'
      USING OUT gcw, IN fs;
    UPDATE DiskCopy
       SET status = 0, -- STAGED
           lastAccessTime = getTime(),  -- for the GC, effective lifetime of this diskcopy starts now
           gcWeight = gcw,
	   diskCopySize = fs
     WHERE castorFile = cfId AND status = 6 -- STAGEOUT
     RETURNING owneruid, ownergid INTO ouid, ogid;
  ELSE
    EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:fs, 10); END;'
      USING OUT gcw, IN fs;
    -- update the DiskCopy status to CANBEMIGR
    dcId := 0;
    UPDATE DiskCopy
       SET status = 10, -- CANBEMIGR
           lastAccessTime = getTime(),  -- for the GC, effective lifetime of this diskcopy starts now
           gcWeight = gcw,
	   diskCopySize = fs
     WHERE castorFile = cfId AND status = 6 -- STAGEOUT
     RETURNING id, owneruid, ownergid INTO dcId, ouid, ogid;
    IF dcId > 0 THEN
      -- Only if we really found the relevant diskcopy, create migration jobs
      -- This is an extra sanity check, see also the deleteOutOfDateStageOutDCs procedure
      FOR i IN 1..nbTC LOOP
        initMigration(cfId, fs, NULL, NULL, i, tconst.MIGRATIONJOB_PENDING);
      END LOOP;
    END IF;
  END IF;
  -- If we are a real PutDone (and not a put outside of a prepareToPut/Update)
  -- then we have to archive the original prepareToPut/Update subRequest
  IF context = 2 THEN
    -- There can be only a single PrepareTo request: any subsequent PPut would be
    -- rejected and any subsequent PUpdate would be directly archived (cf. processPrepareRequest).
    DECLARE
      srId NUMBER;
    BEGIN
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest.id INTO srId
        FROM SubRequest,
         (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id
            FROM StagePrepareToPutRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id
            FROM StagePrepareToUpdateRequest) Request
       WHERE SubRequest.castorFile = cfId
         AND SubRequest.request = Request.id
         AND SubRequest.status = 6;  -- READY
      archiveSubReq(srId, 8);  -- FINISHED
    EXCEPTION WHEN NO_DATA_FOUND THEN
      NULL;   -- ignore the missing subrequest
    END;
  END IF;
  -- Trigger the creation of additional copies of the file, if necessary.
  replicateOnClose(cfId, ouid, ogid);
END;
/


/* PL/SQL method implementing putDoneFunc */
CREATE OR REPLACE PROCEDURE putDoneFunc (cfId IN INTEGER,
                                         fs IN INTEGER,
                                         context IN INTEGER,
                                         svcClassId IN INTEGER) AS
  nc INTEGER;
BEGIN
  -- get number of migration jobs to create
  SELECT nbCopies INTO nc FROM FileClass, CastorFile
   WHERE CastorFile.id = cfId AND CastorFile.fileClass = FileClass.id;
  -- and execute the internal putDoneFunc with the number of migration jobs to be created
  internalPutDoneFunc(cfId, fs, context, nc, svcClassId);
END;
/

/* PL/SQL method implementing processPrepareRequest */
/* the result output is a DiskCopy status for STAGED, DISK2DISKCOPY or RECALL,
   -1 for user failure, -2 for subrequest put in WAITSUBREQ */
CREATE OR REPLACE PROCEDURE processPrepareRequest
        (srId IN INTEGER, result OUT INTEGER) AS
  nbDCs INTEGER;
  svcClassId NUMBER;
  srvSvcClassId NUMBER;
  cfId NUMBER;
  srcDcId NUMBER;
  recSvcClass NUMBER;
  recDcId NUMBER;
  reuid NUMBER;
  regid NUMBER;
BEGIN
  -- retrieve the castorfile, the svcclass and the reqId for this subrequest
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
         SubRequest.castorFile, Request.euid, Request.egid, Request.svcClass
    INTO cfId, reuid, regid, svcClassId
    FROM (SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */
                 id, euid, egid, svcClass FROM StagePrepareToGetRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */
                 id, euid, egid, svcClass FROM StagePrepareToUpdateRequest) Request,
         SubRequest
   WHERE Subrequest.request = Request.id
     AND Subrequest.id = srId;
  -- lock the castor file to be safe in case of two concurrent subrequest
  SELECT id INTO cfId FROM CastorFile WHERE id = cfId FOR UPDATE;

  -- Look for available diskcopies. Note that we never wait on other requests
  -- and we include WAITDISK2DISKCOPY as they are going to be available.
  -- For those ones, the filesystem link in the diskcopy table is set to 0,
  -- hence it is not possible to determine the service class via the
  -- filesystem -> diskpool -> svcclass relationship and the replication
  -- request is used.
  SELECT COUNT(*) INTO nbDCs FROM (
    SELECT DiskCopy.id
      FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
     WHERE DiskCopy.castorfile = cfId
       AND DiskCopy.fileSystem = FileSystem.id
       AND FileSystem.diskpool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = svcClassId
       AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
       AND FileSystem.diskserver = DiskServer.id
       AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
       AND DiskServer.hwOnline = 1
       AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_STAGEOUT, dconst.DISKCOPY_CANBEMIGR)
     UNION ALL
    SELECT /*+ INDEX(StageDiskCopyReplicaRequest I_StageDiskCopyReplic_DestDC) */ DiskCopy.id
      FROM DiskCopy, StageDiskCopyReplicaRequest
     WHERE DiskCopy.id = StageDiskCopyReplicaRequest.destDiskCopy
       AND StageDiskCopyReplicaRequest.svcclass = svcClassId
       AND DiskCopy.castorfile = cfId
       AND DiskCopy.status = 1); -- WAITDISK2DISKCOPY

  IF nbDCs > 0 THEN
    -- Yes, we have some
    result := 0;  -- DISKCOPY_STAGED
    RETURN;
  END IF;

  -- No diskcopies available for this service class:
  -- we may have to schedule a disk to disk copy or trigger a recall
  checkForD2DCopyOrRecall(cfId, srId, reuid, regid, svcClassId, srcDcId, srvSvcClassId);
  IF srcDcId > 0 THEN  -- disk to disk copy
    createDiskCopyReplicaRequest(srId, srcDcId, srvSvcClassId, svcClassId, reuid, regid);
    result := 1;  -- DISKCOPY_WAITDISK2DISKCOPY, for logging purposes
  ELSIF srcDcId = 0 THEN  -- recall
    -- let the stager trigger the recall
    result := 2;  -- Tape Recall
  ELSE
    result := -1;  -- user error
  END IF;
END;
/


/* PL/SQL method implementing processPutDoneRequest */
CREATE OR REPLACE PROCEDURE processPutDoneRequest
        (rsubreqId IN INTEGER, result OUT INTEGER) AS
  svcClassId NUMBER;
  cfId NUMBER;
  fs NUMBER;
  nbDCs INTEGER;
  putSubReq NUMBER;
BEGIN
  -- Get the svcClass and the castorFile for this subrequest
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id) INDEX(Req PK_StagePutDoneRequest_Id) */
         Req.svcclass, SubRequest.castorfile
    INTO svcClassId, cfId
    FROM SubRequest, StagePutDoneRequest Req
   WHERE Subrequest.request = Req.id
     AND Subrequest.id = rsubreqId;
  -- lock the castor file to be safe in case of two concurrent subrequest
  SELECT id, fileSize INTO cfId, fs
    FROM CastorFile
   WHERE CastorFile.id = cfId FOR UPDATE;

  -- Check whether there is a Put|Update going on
  -- If any, we'll wait on one of them
  BEGIN
    SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ id INTO putSubReq
      FROM SubRequest
     WHERE castorfile = cfId
       AND reqType IN (40, 44)  -- Put, Update
       AND status IN (0, 1, 2, 3, 6, 13) -- START, RESTART, RETRY, WAITSCHED, READY, READYFORSCHED
       AND ROWNUM < 2;
    -- we've found one, we wait
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET parent = putSubReq,
           status = 5, -- WAITSUBREQ
           lastModificationTime = getTime()
     WHERE id = rsubreqId;
    result := -1;  -- no go, request in wait
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- No put waiting, look now for available DiskCopies.
    -- Here we look on all FileSystems in our svcClass
    -- regardless their status, accepting Disabled ones
    -- as there's no real IO activity involved. However the
    -- risk is that the file might not come back and it's lost!
    SELECT COUNT(DiskCopy.id) INTO nbDCs
      FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
     WHERE DiskCopy.castorfile = cfId
       AND DiskCopy.fileSystem = FileSystem.id
       AND FileSystem.diskpool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = svcClassId
       AND FileSystem.diskserver = DiskServer.id
       AND DiskCopy.status = 6;  -- STAGEOUT
    IF nbDCs = 0 THEN
      -- This is a PutDone without a put (otherwise we would have found
      -- a DiskCopy on a FileSystem), so we fail the subrequest.
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET
        status = dconst.SUBREQUEST_FAILED,
        errorCode = 1,  -- EPERM
        errorMessage = 'putDone without a put, or wrong service class'
      WHERE id = rsubReqId;
      result := 0;  -- no go
      RETURN;
    END IF;
    -- All checks have been completed, let's do it
    putDoneFunc(cfId, fs, 2, svcClassId);   -- context = PutDone
    result := 1;
  END;
END;
/


/* PL/SQL method implementing recreateCastorFile */
CREATE OR REPLACE PROCEDURE recreateCastorFile(cfId IN INTEGER,
                                               srId IN INTEGER,
                                               dcId OUT INTEGER,
                                               rstatus OUT INTEGER,
                                               rmountPoint OUT VARCHAR2,
                                               rdiskServer OUT VARCHAR2) AS
  rpath VARCHAR2(2048);
  nbRes INTEGER;
  fid INTEGER;
  nh VARCHAR2(2048);
  fclassId INTEGER;
  sclassId INTEGER;
  putSC INTEGER;
  pputSC INTEGER;
  contextPIPP INTEGER;
  ouid INTEGER;
  ogid INTEGER;
BEGIN
  -- Get data and lock access to the CastorFile
  -- This, together with triggers will avoid new migration/recall jobs
  -- or DiskCopies to be added
  SELECT fileclass INTO fclassId FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Determine the context (Put inside PrepareToPut ?)
  BEGIN
    -- check that we are a Put/Update
    SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ Request.svcClass INTO putSC
      FROM (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */
                   id, svcClass FROM StagePutRequest UNION ALL
            SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                   id, svcClass FROM StageUpdateRequest) Request, SubRequest
     WHERE SubRequest.id = srId
       AND Request.id = SubRequest.request;
    BEGIN
      -- check that there is a PrepareToPut/Update going on. There can be only a single one
      -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
      -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest.diskCopy,
             PrepareRequest.svcClass INTO dcId, pputSC
        FROM (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */
                     id, svcClass FROM StagePrepareToPutRequest UNION ALL
              SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */
                     id, svcClass FROM StagePrepareToUpdateRequest) PrepareRequest, SubRequest
       WHERE SubRequest.CastorFile = cfId
         AND PrepareRequest.id = SubRequest.request
         AND SubRequest.status = 6;  -- READY
      -- if we got here, we are a Put/Update inside a PrepareToPut
      -- however, are we in the same service class ?
      IF putSC != pputSC THEN
        -- No, this means we are a Put/Update and another PrepareToPut
        -- is already running in a different service class. This is forbidden
        dcId := 0;
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = dconst.SUBREQUEST_FAILED,
               errorCode = serrno.EBUSY,
               errorMessage = 'A prepareToPut is running in another service class for this file'
         WHERE id = srId;
        RETURN;
      END IF;
      -- if we got here, we are a Put/Update inside a PrepareToPut
      -- both running in the same service class
      contextPIPP := 1;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- if we got here, we are a standalone Put/Update
      contextPIPP := 0;
    END;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    DECLARE
      nbPReqs NUMBER;
    BEGIN
      -- we are either a prepareToPut, or a prepareToUpdate and it's the only one (file is being created).
      -- In case of prepareToPut we need to check that we are the only one
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ count(SubRequest.diskCopy) INTO nbPReqs
        FROM (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id
                FROM StagePrepareToPutRequest UNION ALL
              SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id
                FROM StagePrepareToUpdateRequest) PrepareRequest, SubRequest
       WHERE SubRequest.castorFile = cfId
         AND PrepareRequest.id = SubRequest.request
         AND SubRequest.status = 6;  -- READY
      -- Note that we did not select ourselves (we are in status 3)
      IF nbPReqs > 0 THEN
        -- this means we are a PrepareToPut and another PrepareToPut/Update
        -- is already running. This is forbidden
        dcId := 0;
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = dconst.SUBREQUEST_FAILED,
               errorCode = serrno.EBUSY,
               errorMessage = 'Another prepareToPut/Update is ongoing for this file'
         WHERE id = srId;
        RETURN;
      END IF;
      -- Everything is ok then
      contextPIPP := 0;
    END;
  END;
  -- check if there is space in the diskpool in case of
  -- disk only pool
  -- First get the svcclass and the user
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ svcClass, euid, egid INTO sclassId, ouid, ogid
    FROM Subrequest,
         (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */
                 id, svcClass, euid, egid FROM StagePutRequest UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                 id, svcClass, euid, egid FROM StageUpdateRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */
                 id, svcClass, euid, egid FROM StagePrepareToPutRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */
                 id, svcClass, euid, egid FROM StagePrepareToUpdateRequest) Request
   WHERE SubRequest.id = srId
     AND Request.id = SubRequest.request;
  IF checkFailJobsWhenNoSpace(sclassId) = 1 THEN
    -- The svcClass is declared disk only and has no space
    -- thus we cannot recreate the file
    dcId := 0;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = 28, -- ENOSPC
           errorMessage = 'File creation canceled since disk pool is full'
     WHERE id = srId;
    RETURN;
  END IF;
  IF contextPIPP = 0 THEN
    -- Puts inside PrepareToPuts don't need the following checks
    -- check if the file can be routed to tape
    IF checkNoTapeRouting(fclassId) = 1 THEN
      -- We could not route the file to tape, so let's fail the opening
      dcId := 0;
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_FAILED,
             errorCode = 1727, -- ESTNOTAPEROUTE
             errorMessage = 'File recreation canceled since the file cannot be routed to tape'
       WHERE id = srId;
      RETURN;
    END IF;
    -- check if recreation is possible for disk2DiskCopies
    SELECT count(*) INTO nbRes FROM DiskCopy
     WHERE status = dconst.DISKCOPY_WAITDISK2DISKCOPY
       AND castorFile = cfId;
    IF nbRes > 0 THEN
      -- We found something, thus we cannot recreate
      dcId := 0;
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_FAILED,
             errorCode = serrno.EBUSY,
             errorMessage = 'File recreation canceled since file is being replicated'
       WHERE id = srId;
      RETURN;
    END IF;
    -- delete ongoing recalls
    deleteRecallJobs(cfId);
    -- fail recall requests pending on the previous file
    UPDATE SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = serrno.EINTR,
           errorMessage = 'Canceled by another user request'
     WHERE castorFile = cfId AND status IN (dconst.SUBREQUEST_WAITTAPERECALL, dconst.SUBREQUEST_REPACK);
    -- delete ongoing migrations
    deleteMigrationJobs(cfId);
    -- set DiskCopies to INVALID
    UPDATE DiskCopy
       SET status = dconst.DISKCOPY_INVALID,
           gcType = dconst.GCTYPE_OVERWRITTEN
     WHERE castorFile = cfId AND status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR);
    -- create new DiskCopy
    SELECT fileId, nsHost INTO fid, nh FROM CastorFile WHERE id = cfId;
    SELECT ids_seq.nextval INTO dcId FROM DUAL;
    buildPathFromFileId(fid, nh, dcId, rpath);
    INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status, creationTime, lastAccessTime, gcWeight, diskCopySize, nbCopyAccesses, owneruid, ownergid)
      VALUES (rpath, dcId, 0, cfId, 5, getTime(), getTime(), 0, 0, 0, ouid, ogid); -- status WAITFS
    rstatus := 5; -- WAITFS
    rmountPoint := '';
    rdiskServer := '';
  ELSE
    DECLARE
      fsId INTEGER;
      dsId INTEGER;
    BEGIN
      -- Retrieve the infos about the DiskCopy to be used
      SELECT fileSystem, status INTO fsId, rstatus FROM DiskCopy WHERE id = dcId;
      -- retrieve mountpoint and filesystem if any
      IF fsId = 0 THEN
        rmountPoint := '';
        rdiskServer := '';
      ELSE
        SELECT mountPoint, diskServer INTO rmountPoint, dsId
          FROM FileSystem WHERE FileSystem.id = fsId;
        SELECT name INTO rdiskServer FROM DiskServer WHERE id = dsId;
      END IF;
      -- See whether we should wait on another concurrent Put|Update request
      IF rstatus = 11 THEN  -- WAITFS_SCHEDULING
        -- another Put|Update request was faster than us, so we have to wait on it
        -- to be scheduled; we will be restarted at PutStart of that one
        DECLARE
          srParent NUMBER;
        BEGIN
          -- look for it
          SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ id INTO srParent
            FROM SubRequest
           WHERE reqType IN (40, 44)  -- Put, Update
             AND diskCopy = dcId
             AND status IN (dconst.SUBREQUEST_READYFORSCHED, dconst.SUBREQUEST_READY)
             AND ROWNUM < 2;   -- if we have more than one just take one of them
          UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
             SET status = 5, parent = srParent  -- WAITSUBREQ
           WHERE id = srId;
        EXCEPTION WHEN NO_DATA_FOUND THEN
          -- we didn't find that request: let's assume it failed, we override the status 11
          rstatus := 5;  -- WAITFS
        END;
      ELSE
        -- we are the first, we change status as we are about to go to the scheduler
        UPDATE DiskCopy SET status = 11  -- WAITFS_SCHEDULING
         WHERE castorFile = cfId
           AND status = 5;  -- WAITFS
        -- and we still return 5 = WAITFS to the stager so to go on
      END IF;
    END;
  END IF;
  -- link SubRequest and DiskCopy
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET diskCopy = dcId,
         lastModificationTime = getTime()
   WHERE id = srId;
  -- reset file size to 0 as the file has been truncated
  UPDATE CastorFile set fileSize = 0 WHERE id = cfId;
  -- we don't commit here, the stager will do that when
  -- the subRequest status will be updated to 6
END;
/

/* This procedure resets the lastKnownFileName the CastorFile that has a given name
   inside an autonomous transaction. This should be called before creating/renaming any
   CastorFile so that lastKnownFileName stays unique */
CREATE OR REPLACE PROCEDURE dropReusedLastKnownFileName(fileName IN VARCHAR2) AS
  PRAGMA AUTONOMOUS_TRANSACTION;
BEGIN
  UPDATE /*+ INDEX (I_CastorFile_lastKnownFileName) */ CastorFile
     SET lastKnownFileName = TO_CHAR(id)
   WHERE lastKnownFileName = normalizePath(fileName);
  COMMIT;
END;
/

/* PL/SQL method implementing selectCastorFile
 * This is only a wrapper on selectCastorFileInternal
 */
CREATE OR REPLACE PROCEDURE selectCastorFile (fId IN INTEGER,
                                              nh IN VARCHAR2,
                                              fc IN INTEGER,
                                              fs IN INTEGER,
                                              fn IN VARCHAR2,
                                              srId IN NUMBER,
                                              lut IN NUMBER,
                                              rid OUT INTEGER,
                                              rfs OUT INTEGER) AS
  nsHostName VARCHAR2(2048);
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  -- call internal method
  selectCastorFileInternal(fId, nsHostName, fc, fs, fn, srId, lut, TRUE, rid, rfs);
END;
/

/* PL/SQL method implementing selectCastorFile */
CREATE OR REPLACE PROCEDURE selectCastorFileInternal (fId IN INTEGER,
                                                      nh IN VARCHAR2,
                                                      fc IN INTEGER,
                                                      fs IN INTEGER,
                                                      fn IN VARCHAR2,
                                                      srId IN NUMBER,
                                                      lut IN NUMBER,
                                                      waitForLock IN BOOLEAN,
                                                      rid OUT INTEGER,
                                                      rfs OUT INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  previousLastKnownFileName VARCHAR2(2048);
  fcId NUMBER;
  varReqType INTEGER;
BEGIN
  -- Resolve the fileclass
  BEGIN
    SELECT id INTO fcId FROM FileClass WHERE classId = fc;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RAISE_APPLICATION_ERROR (-20010, 'File class '|| fc ||' not found in database');
  END;
  BEGIN
    -- try to find an existing file
    SELECT id, fileSize, lastKnownFileName
      INTO rid, rfs, previousLastKnownFileName
      FROM CastorFile
     WHERE fileId = fid AND nsHost = nh;
    -- In case its filename has changed, take care that the new name is
    -- not already the lastKnownFileName of another file, that was also
    -- renamed but for which the lastKnownFileName has not been updated
    -- We actually reset the lastKnownFileName of such a file if needed
    -- Note that this procedure will run in an autonomous transaction so that
    -- no dead lock can result from taking a second lock within this transaction
    IF fn != previousLastKnownFileName THEN
      dropReusedLastKnownFileName(fn);
    END IF;
    -- take a lock on the file. Note that the file may have disappeared in the
    -- meantime, this is why we first select (potentially having a NO_DATA_FOUND
    -- exception) before we update.
    IF waitForLock THEN
      SELECT id INTO rid FROM CastorFile WHERE id = rid FOR UPDATE;
    ELSE
      SELECT id INTO rid FROM CastorFile WHERE id = rid FOR UPDATE NOWAIT;
    END IF;
    -- The file is still there, so update lastAccessTime and lastKnownFileName.
    UPDATE CastorFile SET lastAccessTime = getTime(),
                          lastKnownFileName = normalizePath(fn)
     WHERE id = rid;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET castorFile = rid
     WHERE id = srId
     RETURNING reqType INTO varReqType;
    IF varReqType IN (37, 38, 40, 44) THEN  -- all write operations
      UPDATE CastorFile SET lastUpdateTime = lut
       WHERE id = rid;
    ELSE
      -- On pure read operations, we should actually check whether our disk cache is stale,
      -- that is IF CF.lastUpdateTime < lut THEN invalidate our diskcopies.
      -- This is pending the full implementation of bug #95189: Time discrepencies between
      -- disk servers and name servers can lead to silent data loss on input.
      -- The problem being that lut is the namespace's mtime, which can be modified by nstouch,
      -- hence nstouch followed by a Get would destroy the data on disk!
      NULL;
    END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- we did not find the file, let's create a new one
    -- take care that the name of the new file is not already the lastKnownFileName
    -- of another file, that was renamed but for which the lastKnownFileName has
    -- not been updated.
    -- We actually reset the lastKnownFileName of such a file if needed
    -- Note that this procedure will run in an autonomous transaction so that
    -- no dead lock can result from taking a second lock within this transaction
    dropReusedLastKnownFileName(fn);
    -- insert new row
    INSERT INTO CastorFile (id, fileId, nsHost, fileClass, fileSize,
                            creationTime, lastAccessTime, lastUpdateTime, lastKnownFileName)
      VALUES (ids_seq.nextval, fId, nh, fcId, fs, getTime(), getTime(), lut, normalizePath(fn))
      RETURNING id, fileSize INTO rid, rfs;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET castorFile = rid
     WHERE id = srId;
  END;
EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
  -- the violated constraint indicates that the file was created by another client
  -- while we were trying to create it ourselves. We can thus use the newly created file
  IF waitForLock THEN
    SELECT id, fileSize INTO rid, rfs FROM CastorFile
      WHERE fileId = fid AND nsHost = nh FOR UPDATE;
  ELSE
    SELECT id, fileSize INTO rid, rfs FROM CastorFile
      WHERE fileId = fid AND nsHost = nh FOR UPDATE NOWAIT;
  END IF;
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET castorFile = rid
   WHERE id = srId;
END;
/

/* PL/SQL method implementing stageForcedRm */
CREATE OR REPLACE PROCEDURE stageForcedRm (fid IN INTEGER,
                                           nh IN VARCHAR2,
                                           inGcType IN INTEGER DEFAULT NULL) AS
  cfId INTEGER;
  nbRes INTEGER;
  dcsToRm "numList";
  nsHostName VARCHAR2(2048);
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  -- Lock the access to the CastorFile
  -- This, together with triggers will avoid new migration/recall jobs
  -- or DiskCopies to be added
  SELECT id INTO cfId FROM CastorFile
   WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  -- list diskcopies
  SELECT id BULK COLLECT INTO dcsToRm
    FROM DiskCopy
   WHERE castorFile = cfId
     AND status IN (0, 5, 6, 10, 11);  -- STAGED, WAITFS, STAGEOUT, CANBEMIGR, WAITFS_SCHEDULING
  -- Stop ongoing recalls and migrations
  deleteRecallJobs(cfId);
  deleteMigrationJobs(cfId);
  -- mark all get/put requests for those diskcopies
  -- and the ones waiting on them as failed
  -- so that clients eventually get an answer
  FOR sr IN (SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ id, status FROM SubRequest
              WHERE diskcopy IN
                (SELECT /*+ CARDINALITY(dcidTable 5) */ *
                   FROM TABLE(dcsToRm) dcidTable)
                AND status IN (0, 1, 2, 5, 6, 12, 13)) LOOP   -- START, RESTART, RETRY, WAITSUBREQ, READY, READYFORSCHED
    UPDATE SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = serrno.EINTR,
           errorMessage = 'Canceled by another user request',
           parent = 0
     WHERE (id = sr.id) OR (parent = sr.id);
  END LOOP;
  -- Set selected DiskCopies to INVALID
  FORALL i IN 1 .. dcsToRm.COUNT
    UPDATE DiskCopy
       SET status = dconst.DISKCOPY_INVALID,
           gcType = inGcType
     WHERE id = dcsToRm(i);
END;
/


/* PL/SQL method implementing renamedFileCleanup */
CREATE OR REPLACE PROCEDURE renamedFileCleanup(inFileName IN VARCHAR2,
                                               inSrId IN INTEGER) AS
  varCfId INTEGER;
  varFileId INTEGER;
  varNsHost VARCHAR2(2048);
  varNsPath VARCHAR2(2048);
BEGIN
  -- try to find a file with the right name
  SELECT /*+ INDEX(CastorFile I_CastorFile_LastKnownFileName) */ fileId, nshost, id
    INTO varFileId, varNsHost, varCfId
    FROM CastorFile
   WHERE lastKnownFileName = inFileName;
  -- validate this file against the NameServer
  BEGIN
    SELECT getPathForFileid@remotens(fileId) INTO varNsPath
      FROM Cns_file_metadata@remotens
     WHERE fileid = varFileId;
    -- the nameserver contains a file with this fileid, but
    -- with a different name than the stager. Obviously the
    -- file got renamed and the requested deletion cannot succeed;
    -- anyway we update the stager catalogue with the new name
    DECLARE
      CONSTRAINT_VIOLATED EXCEPTION;
      PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
    BEGIN
      UPDATE CastorFile SET lastKnownFileName = varNsPath
       WHERE id = varCfId;
    EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
      -- we have another file that already uses the new name of this one...
      -- let's fix this, but we won't put the right name there
      UPDATE CastorFile SET lastKnownFileName = TO_CHAR(id)
       WHERE lastKnownFileName = varNsPath;
      UPDATE CastorFile SET lastKnownFileName = varNsPath
       WHERE id = varCfId;
    END;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- the file exists only in the stager db,
    -- execute stageForcedRm (cf. ns synch performed in GC daemon)
    stageForcedRm(varFileId, varNsHost, dconst.GCTYPE_NSSYNCH);
  END;
  -- in all cases we fail the subrequest
  UPDATE SubRequest
     SET status = dconst.SUBREQUEST_FAILED, errorCode=serrno.ENOENT,
         errorMessage = 'The file got renamed by another user request'
   WHERE id = inSrId;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- No file found with the given name, fail the subrequest with a generic ENOENT
  UPDATE SubRequest
     SET status = dconst.SUBREQUEST_FAILED, errorCode=serrno.ENOENT
   WHERE id = inSrId;
END;
/


/* PL/SQL method implementing stageRm */
CREATE OR REPLACE PROCEDURE stageRm (srId IN INTEGER,
                                     fid IN INTEGER,
                                     nh IN VARCHAR2,
                                     svcClassId IN INTEGER,
                                     ret OUT INTEGER) AS
  nsHostName VARCHAR2(2048);
  cfId INTEGER;
  dcsToRm "numList";
  dcsToRmStatus "numList";
  nbRJsDeleted INTEGER;
BEGIN
  ret := 0;
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  BEGIN
    -- Lock the access to the CastorFile
    -- This, together with triggers will avoid new migration/recall jobs
    -- or DiskCopies to be added
    SELECT id INTO cfId FROM CastorFile
     WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- This file does not exist in the stager catalog
    -- so we just fail the request
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = serrno.ENOENT,
           errorMessage = 'File not found on disk cache'
     WHERE id = srId;
    RETURN;
  END;

  -- select the list of DiskCopies to be deleted
  SELECT id, status BULK COLLECT INTO dcsToRm, dcsToRmStatus FROM (
    -- first physical diskcopies
    SELECT /*+ INDEX(DC I_DiskCopy_CastorFile) */ DC.id, DC.status
      FROM DiskCopy DC, FileSystem, DiskPool2SvcClass DP2SC
     WHERE DC.castorFile = cfId
       AND DC.status IN (0, 6, 10)  -- STAGED, STAGEOUT, CANBEMIGR
       AND DC.fileSystem = FileSystem.id
       AND FileSystem.diskPool = DP2SC.parent
       AND (DP2SC.child = svcClassId OR svcClassId = 0))
  UNION ALL
    -- and then diskcopies resulting from ongoing requests, for which the previous
    -- query wouldn't return any entry because of e.g. missing filesystem
    SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ DC.id, DC.status
      FROM (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id
              FROM StagePrepareToPutRequest WHERE svcClass = svcClassId UNION ALL
            SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id
              FROM StagePutRequest WHERE svcClass = svcClassId UNION ALL
            SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */ id
              FROM StageDiskCopyReplicaRequest WHERE svcClass = svcClassId) Request,
           SubRequest, DiskCopy DC
     WHERE SubRequest.diskCopy = DC.id
       AND Request.id = SubRequest.request
       AND DC.castorFile = cfId
       AND DC.status IN (dconst.DISKCOPY_WAITDISK2DISKCOPY, dconst.DISKCOPY_WAITFS,
                         dconst.DISKCOPY_WAITFS_SCHEDULING);

  -- in case we are dropping CANBEMIGR diskcopies, ensure that we have at least one copy left on disk
  IF dcsToRmStatus.COUNT > 0 THEN
    IF dcsToRmStatus(1) = dconst.DISKCOPY_CANBEMIGR THEN
      BEGIN
        SELECT castorFile INTO cfId
          FROM DiskCopy
         WHERE castorFile = cfId
           AND status = dconst.DISKCOPY_CANBEMIGR
           AND id NOT IN (SELECT /*+ CARDINALITY(dcidTable 5) */ * FROM TABLE(dcsToRm) dcidTable)
           AND ROWNUM < 2;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- nothing left, so we would lose the file. Better to forbid stagerm
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = dconst.SUBREQUEST_FAILED,
               errorCode = serrno.EBUSY,
               errorMessage = 'The file is not yet migrated'
         WHERE id = srId;
        RETURN;
      END;
    END IF;

    -- fail diskcopies : WAITFS[_SCHED] -> FAILED, others -> INVALID
    UPDATE DiskCopy
       SET status = decode(status, dconst.DISKCOPY_WAITFS, dconst.DISKCOPY_FAILED,
                                   dconst.DISKCOPY_WAITFS_SCHEDULING, dconst.DISKCOPY_FAILED,
                                   dconst.DISKCOPY_INVALID)
     WHERE id IN (SELECT /*+ CARDINALITY(dcidTable 5) */ * FROM TABLE(dcsToRm) dcidTable);

    -- fail the subrequests linked to the deleted diskcopies
    FOR sr IN (SELECT /*+ INDEX(SR I_SubRequest_DiskCopy) */ id, subreqId
                 FROM SubRequest SR
                WHERE diskcopy IN (SELECT /*+ CARDINALITY(dcidTable 5) */ * FROM TABLE(dcsToRm) dcidTable)
                  AND status IN (dconst.SUBREQUEST_START, dconst.SUBREQUEST_RESTART,
                                 dconst.SUBREQUEST_RETRY, dconst.SUBREQUEST_WAITTAPERECALL,
                                 dconst.SUBREQUEST_WAITSUBREQ, dconst.SUBREQUEST_READY,
                                 dconst.SUBREQUEST_READYFORSCHED)) LOOP
      UPDATE SubRequest
         SET status = CASE WHEN status IN (dconst.SUBREQUEST_READY, dconst.SUBREQUEST_READYFORSCHED)
                            AND reqType = 133  -- DiskCopyReplicaRequests
                           THEN dconst.SUBREQUEST_FAILED_FINISHED
                           ELSE dconst.SUBREQUEST_FAILED END,
             -- user requests in status WAITSUBREQ are always marked FAILED
             -- even if they wait on a replication
             errorCode = serrno.EINTR,
             errorMessage = 'Canceled by another user request'
       WHERE id = sr.id OR parent = sr.id;
      -- make the scheduler aware so that it can remove the transfer from the queues if needed
      INSERT INTO TransfersToAbort (uuid) VALUES (sr.subreqId);
    END LOOP;
    -- wake up the scheduler so that it can remove the transfer from the queues now
    DBMS_ALERT.SIGNAL('transfersToAbort', '');
  END IF;

  -- delete RecallJobs that should be canceled
  DELETE FROM RecallJob
   WHERE castorfile = cfId AND (svcClass = svcClassId OR svcClassId = 0);
  nbRJsDeleted := SQL%ROWCOUNT;
  -- in case we've dropped something, check whether we still have recalls ongoing
  IF nbRJsDeleted > 0 THEN
    BEGIN
      SELECT castorFile INTO cfId
        FROM RecallJob
       WHERE castorFile = cfId;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- all recalls are canceled for this file
      -- deal with potential waiting migrationJobs
      deleteMigrationJobsForRecall(cfId);
      -- fail corresponding requests
      UPDATE SubRequest
         SET status = dconst.SUBREQUEST_FAILED,
             errorCode = serrno.EINTR,
             errorMessage = 'Canceled by another user request'
       WHERE castorFile = cfId
         AND status = dconst.SUBREQUEST_WAITTAPERECALL;
    END;
  END IF;

  -- In case nothing was dropped at all, complain
  IF dcsToRm.COUNT = 0 AND nbRJsDeleted = 0 THEN
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = serrno.ENOENT,
           errorMessage = CASE WHEN svcClassId = 0 THEN 'File not found on disk cache'
                               ELSE 'File not found on this service class' END
     WHERE id = srId;
    RETURN;
  END IF;

  ret := 1;  -- ok
END;
/


/* PL/SQL method implementing a setFileGCWeight request */
CREATE OR REPLACE PROCEDURE setFileGCWeightProc
(fid IN NUMBER, nh IN VARCHAR2, svcClassId IN NUMBER, weight IN FLOAT, ret OUT INTEGER) AS
  CURSOR dcs IS
  SELECT DiskCopy.id, gcWeight
    FROM DiskCopy, CastorFile
   WHERE castorFile.id = diskcopy.castorFile
     AND fileid = fid
     AND nshost = getConfigOption('stager', 'nsHost', nh)
     AND fileSystem IN (
       SELECT FileSystem.id
         FROM FileSystem, DiskPool2SvcClass D2S
        WHERE FileSystem.diskPool = D2S.parent
          AND D2S.child = svcClassId);
  gcwProc VARCHAR(2048);
  gcw NUMBER;
BEGIN
  ret := 0;
  -- get gc userSetGCWeight function to be used, if any
  gcwProc := castorGC.getUserSetGCWeight(svcClassId);
  -- loop over diskcopies and update them
  FOR dc in dcs LOOP
    gcw := dc.gcWeight;
    -- compute actual gc weight to be used
    IF gcwProc IS NOT NULL THEN
      EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:oldGcw, :delta); END;'
        USING OUT gcw, IN gcw, weight;
    END IF;
    -- update DiskCopy
    UPDATE DiskCopy SET gcWeight = gcw WHERE id = dc.id;
    ret := 1;   -- some diskcopies found, ok
  END LOOP;
END;
/


/* PL/SQL method implementing updateAndCheckSubRequest */
CREATE OR REPLACE PROCEDURE updateAndCheckSubRequest(srId IN INTEGER, newStatus IN INTEGER, result OUT INTEGER) AS
  reqId INTEGER;
  rName VARCHAR2(100);
BEGIN
  -- Update Status
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET status = newStatus,
         answered = 1,
         lastModificationTime = getTime(),
         getNextStatus = decode(newStatus, 6, 1, 8, 1, 9, 1, 0)  -- READY, FINISHED or FAILED_FINISHED -> GETNEXTSTATUS_FILESTAGED
   WHERE id = srId
   RETURNING request, (SELECT object FROM Type2Obj WHERE type = reqType) INTO reqId, rName;
  -- Lock the access to the Request
  EXECUTE IMMEDIATE
    'BEGIN SELECT id INTO :reqId FROM '|| rName ||' WHERE id = :reqId FOR UPDATE; END;'
    USING IN OUT reqId;
  -- Check whether it was the last subrequest in the request
  SELECT /*+ INDEX(Subrequest I_Subrequest_Request)*/ id INTO result FROM SubRequest
   WHERE request = reqId
     AND status IN (0, 1, 2, 3, 4, 5, 7, 10, 12, 13)   -- all but FINISHED, FAILED_FINISHED, ARCHIVED
     AND answered = 0
     AND ROWNUM < 2;
EXCEPTION WHEN NO_DATA_FOUND THEN
  result := 0;
  -- No data found means we were last; check whether we have to archive
  IF newStatus IN (8, 9) THEN
    archiveSubReq(srId, newStatus);
  END IF;
END;
/

/* PL/SQL method implementing storeReports. This procedure stores new reports
 * and disables nodes for which last report (aka heartbeat) is too old. 
 * For efficiency reasons the input parameters to this method
 * are 2 vectors. The first ones is a list of strings with format :
 *  (diskServerName1, mountPoint1, diskServerName2, mountPoint2, ...)
 * representing a set of mountpoints with the diskservername repeated
 * for each mountPoint.
 * The second vector is a list of numbers with format :
 *  (maxFreeSpace1, minAllowedFreeSpace1, totalSpace1, freeSpace1,
 *   nbReadStreams1, nbWriteStreams1, nbRecalls1, nbMigrations1,
 *   maxFreeSpace2, ...)
 * where 8 values are given for each of the mountPoints in the first vector
 */
CREATE OR REPLACE PROCEDURE storeReports
(inStrParams IN castor."strList",
 inNumParams IN castor."cnumList") AS
 varDsId NUMBER;
 varFsId NUMBER;
 varHeartbeatTimeout NUMBER;
 emptyReport BOOLEAN := False;
BEGIN
  -- quick check of the vector lengths
  IF MOD(inStrParams.COUNT,2) != 0 THEN
    IF inStrParams.COUNT = 1 AND inStrParams(1) = 'Empty' THEN
      -- work around the "PLS-00418: array bind type must match PL/SQL table row type"
      -- error launched by Oracle when empty arrays are passed as parameters
      emptyReport := True;
    ELSE
      RAISE_APPLICATION_ERROR (-20125, 'Invalid call to storeReports : ' ||
                                       '1st vector has odd number of elements (' ||
                                       TO_CHAR(inStrParams.COUNT) || ')');
    END IF;
  END IF;
  IF MOD(inNumParams.COUNT,8) != 0 AND NOT emptyReport THEN
    RAISE_APPLICATION_ERROR (-20125, 'Invalid call to storeReports : ' ||
                             '2nd vector has wrong number of elements (' ||
                             TO_CHAR(inNumParams.COUNT) || ' instead of ' ||
                             TO_CHAR(inStrParams.COUNT*4) || ')');
  END IF;
  IF NOT emptyReport THEN
    -- Go through the concerned filesystems
    FOR i IN 0 .. inStrParams.COUNT/2-1 LOOP
      -- update DiskServer
      varDsId := NULL;
      UPDATE DiskServer
         SET hwOnline = 1,
             lastHeartbeatTime = getTime()
       WHERE name = inStrParams(2*i+1)
      RETURNING id INTO varDsId;
      -- if it did not exist, create it
      IF varDsId IS NULL THEN
        INSERT INTO DiskServer (name, id, status, hwOnline, lastHeartbeatTime)
         VALUES (inStrParams(2*i+1), ids_seq.nextval, dconst.DISKSERVER_DISABLED, 1, getTime())
        RETURNING id INTO varDsId;
      END IF;
      -- update FileSystem
      varFsId := NULL;
      UPDATE FileSystem
         SET maxFreeSpace = inNumParams(8*i+1),
             minAllowedFreeSpace = inNumParams(8*i+2),
             totalSize = inNumParams(8*i+3),
             free = inNumParams(8*i+4),
             nbReadStreams = inNumParams(8*i+5),
             nbWriteStreams = inNumParams(8*i+6),
             nbRecallerStreams = inNumParams(8*i+7),
             nbMigratorStreams = inNumParams(8*i+8)
       WHERE diskServer=varDsId AND mountPoint=inStrParams(2*i+2)
      RETURNING id INTO varFsId;
      -- if it did not exist, create it
      IF varFsId IS NULL THEN
        INSERT INTO FileSystem (mountPoint, maxFreeSpace, minAllowedFreeSpace, totalSize, free,
                                nbReadStreams, nbWriteStreams, nbRecallerStreams, nbMigratorStreams,
                                id, diskPool, diskserver, status)
        VALUES (inStrParams(2*i+2), inNumParams(8*i+1), inNumParams(8*i+2), inNumParams(8*i+3),
                inNumParams(8*i+4), inNumParams(8*i+5), inNumParams(8*i+6), inNumParams(8*i+7),
                inNumParams(8*i+8), ids_seq.nextval, 0, varDsId, dconst.FILESYSTEM_DISABLED);
      END IF;
    END LOOP;
  END IF;

  -- now disable nodes that have too old reports
  varHeartbeatTimeout := TO_NUMBER(getConfigOption('DiskServer', 'HeartbeatTimeout', '180'));
  UPDATE DiskServer
     SET hwOnline = 0
   WHERE lastHeartbeatTime < getTime() - varHeartbeatTimeout
     AND hwOnline = 1;
END;
/

/* PL/SQL method used by the stager to collect the logging made in the DB */
CREATE OR REPLACE PROCEDURE dumpDBLogs(logEntries OUT castor.LogEntry_Cur) AS
  rowIds strListTable;
BEGIN
  -- lock whatever we can from the table. This is to prevent deadlocks.
  SELECT ROWID BULK COLLECT INTO rowIds
    FROM DLFLogs FOR UPDATE NOWAIT;
  -- insert data on tmp table and drop selected entries
  INSERT INTO DLFLogsHelper (timeinfo, uuid, priority, msg, fileId, nsHost, SOURCE, params)
   (SELECT timeinfo, uuid, priority, msg, fileId, nsHost, SOURCE, params
    FROM DLFLogs WHERE ROWID IN (SELECT * FROM TABLE(rowIds)));
  DELETE FROM DLFLogs WHERE ROWID IN (SELECT * FROM TABLE(rowIds));
  -- return list of entries by opening a cursor on temp table
  OPEN logEntries FOR
    SELECT timeinfo, uuid, priority, msg, fileId, nsHost, source, params FROM DLFLogsHelper;
END;
/

/* PL/SQL method creating MigrationJobs for missing segments of a file if needed */
/* Can throw a -20100 exception when no route to tape is found for the missing segments */
CREATE OR REPLACE PROCEDURE createMJForMissingSegments(inCfId IN INTEGER,
                                                       inFileSize IN INTEGER,
                                                       inFileClassId IN INTEGER,
                                                       inAllCopyNbs IN "numList",
                                                       inAllVIDs IN strListTable,
                                                       inFileId IN INTEGER,
                                                       inNsHost IN VARCHAR2,
                                                       inLogParams IN VARCHAR2) AS
  varExpectedNbCopies INTEGER;
  varCreatedMJs INTEGER := 0;
  varNextCopyNb INTEGER := 1;
  varNb INTEGER;
BEGIN
  -- check whether there are missing segments and whether we should create new ones
  SELECT nbCopies INTO varExpectedNbCopies FROM FileClass WHERE id = inFileClassId;
  IF varExpectedNbCopies > inAllCopyNbs.COUNT THEN
    -- some copies are missing
    DECLARE
      unused INTEGER;
    BEGIN
      -- check presence of migration jobs for this file
      SELECT id INTO unused FROM MigrationJob WHERE castorFile=inCfId AND ROWNUM < 2;
      -- there are MigrationJobs already, so remigrations were already handled. Nothing to be done
      -- we typically are in a situation where the file was already waiting for recall for
      -- another recall group.
      -- log "detected missing copies on tape, but migrations ongoing"
      logToDLF(NULL, dlf.LVL_DEBUG, dlf.RECALL_MISSING_COPIES_NOOP, inFileId, inNsHost, 'stagerd',
               inLogParams || ' nbMissingCopies=' || TO_CHAR(varExpectedNbCopies-inAllCopyNbs.COUNT));
      RETURN;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- we need to remigrate this file
      NULL;
    END;
    -- log "detected missing copies on tape"
    logToDLF(NULL, dlf.LVL_SYSTEM, dlf.RECALL_MISSING_COPIES, inFileId, inNsHost, 'stagerd',
             inLogParams || ' nbMissingCopies=' || TO_CHAR(varExpectedNbCopies-inAllCopyNbs.COUNT));
    -- copies are missing, try to recreate them
    WHILE varExpectedNbCopies > inAllCopyNbs.COUNT + varCreatedMJs AND varNextCopyNb <= varExpectedNbCopies LOOP
      BEGIN
        -- check whether varNextCopyNb is already in use by a valid copy
        SELECT * INTO varNb FROM TABLE(inAllCopyNbs) WHERE COLUMN_VALUE=varNextCopyNb;
        -- this copy number is in use, go to next one
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- copy number is not in use, create a migrationJob using it (may throw exceptions)
        initMigration(inCfId, inFileSize, NULL, NULL, varNextCopyNb, tconst.MIGRATIONJOB_WAITINGONRECALL);
        varCreatedMJs := varCreatedMJs + 1;
        -- log "create new MigrationJob to migrate missing copy"
        logToDLF(NULL, dlf.LVL_SYSTEM, dlf.RECALL_MJ_FOR_MISSING_COPY, inFileId, inNsHost, 'stagerd',
                 inLogParams || ' copyNb=' || TO_CHAR(varNextCopyNb));
      END;
      varNextCopyNb := varNextCopyNb + 1;
    END LOOP;
    -- Did we create new copies ?
    IF varExpectedNbCopies > inAllCopyNbs.COUNT + varCreatedMJs THEN
      -- We did not create enough new copies, this means that we did not find enough
      -- valid copy numbers. Odd... Log something !
      logToDLF(NULL, dlf.LVL_ERROR, dlf.RECALL_COPY_STILL_MISSING, inFileId, inNsHost, 'stagerd',
               inLogParams || ' nbCopiesStillMissing=' ||
               TO_CHAR(varExpectedNbCopies - inAllCopyNbs.COUNT - varCreatedMJs));
    ELSE
      -- Yes, then create migrated segments for the existing segments if there are none
      SELECT count(*) INTO varNb FROM MigratedSegment WHERE castorFile = inCfId;
      IF varNb = 0 THEN
        FOR i IN inAllCopyNbs.FIRST .. inAllCopyNbs.LAST LOOP
          INSERT INTO MigratedSegment (castorFile, copyNb, VID)
          VALUES (inCfId, inAllCopyNbs(i), inAllVIDs(i));
        END LOOP;
      END IF;
    END IF;
  END IF;
END;
/

/* PL/SQL method creating RecallJobs
 * It also creates MigrationJobs for eventually missing segments
 * It returns 0 if successful, else an error code
 */
CREATE OR REPLACE FUNCTION createRecallJobs(inCfId IN INTEGER,
                                            inFileId IN INTEGER,
                                            inNsHost IN VARCHAR2,
                                            inFileSize IN INTEGER,
                                            inFileClassId IN INTEGER,
                                            inRecallGroupId IN INTEGER,
                                            inSvcClassId IN INTEGER,
                                            inEuid IN INTEGER,
                                            inEgid IN INTEGER,
                                            inRequestTime IN NUMBER,
                                            inLogParams IN VARCHAR2) RETURN INTEGER AS
  varAllCopyNbs "numList" := "numList"();
  varAllVIDs strListTable := strListTable();
  varFoundSeg boolean := FALSE;
  varI INTEGER := 1;
  NO_TAPE_ROUTE EXCEPTION;
  PRAGMA EXCEPTION_INIT(NO_TAPE_ROUTE, -20100);
  varErrorMsg VARCHAR2(2048);
BEGIN
  BEGIN
    -- loop over the existing segments
    FOR varSeg IN (SELECT s_fileId as fileId, 0 as lastModTime, copyNo, segSize, 0 as comprSize,
                          Cns_seg_metadata.vid, fseq, blockId, checksum_name, nvl(checksum, 0) as checksum,
                          Cns_seg_metadata.s_status as segStatus, Vmgr_tape_side.status as tapeStatus
                     FROM Cns_seg_metadata@remotens, Vmgr_tape_side@remotens
                    WHERE Cns_seg_metadata.s_fileid = inFileId
                      AND Vmgr_tape_side.VID = Cns_seg_metadata.VID
                    ORDER BY copyno, fsec) LOOP
      varFoundSeg := TRUE;
      -- Is the segment valid, on a valid tape ?
      IF varSeg.segStatus = '-' AND varSeg.tapeStatus NOT IN (1, 2, 32) THEN  -- DISABLED, EXPORTED, ARCHIVED
        -- create recallJob
        INSERT INTO RecallJob (id, castorFile, copyNb, recallGroup, svcClass, euid, egid,
                               vid, fseq, status, fileSize, creationTime, blockId, fileTransactionId)
        VALUES (ids_seq.nextval, inCfId, varSeg.copyno, inRecallGroupId, inSvcClassId,
                inEuid, inEgid, varSeg.vid, varSeg.fseq, tconst.RECALLJOB_PENDING, inFileSize, inRequestTime,
                varSeg.blockId, NULL);
        -- log "created new RecallJob"
        logToDLF(NULL, dlf.LVL_SYSTEM, dlf.RECALL_CREATING_RECALLJOB, inFileId, inNsHost, 'stagerd',
                 inLogParams || ' copyNb=' || TO_CHAR(varSeg.copyno) || ' TPVID=' || varSeg.vid ||
                 ' fseq=' || TO_CHAR(varSeg.fseq || ' FileSize=' || TO_CHAR(inFileSize)));
        -- remember the copy number and tape
        varAllCopyNbs.EXTEND;
        varAllCopyNbs(varI) := varSeg.copyno;
        varAllVIDs.EXTEND;
        varAllVIDs(varI) := varSeg.vid;
        varI := varI + 1;
      ELSE
        -- invalid segment or invalid tape found. Log it.
        -- "createRecallCandidate: found unusable segment"
        logToDLF(NULL, dlf.LVL_NOTICE, dlf.RECALL_INVALID_SEGMENT, inFileId, inNsHost, 'stagerd',
                 inLogParams || ' segStatus=' ||
                 CASE varSeg.segStatus WHEN '-' THEN 'OK'
                                       WHEN 'd' THEN 'DISABLED'
                                       ELSE 'UNKNOWN:' || varSeg.segStatus END || ' tapeStatus=' ||
                 CASE varSeg.tapeStatus WHEN 0 THEN 'OK' 
                                        WHEN 1 THEN 'DISABLED'
                                        WHEN 2 THEN 'EXPORTED'
                                        WHEN 4 THEN 'BUSY'
                                        WHEN 8 THEN 'FULL'
                                        WHEN 16 THEN 'RDONLY'
                                        WHEN 32 THEN 'ARCHIVED'
                                        ELSE 'UNKNOWN:' || TO_CHAR(varSeg.tapeStatus) END);
      END IF;
    END LOOP;
  EXCEPTION WHEN OTHERS THEN
    -- log "error when retrieving segments from namespace"
    logToDLF(NULL, dlf.LVL_ERROR, dlf.RECALL_UNKNOWN_NS_ERROR, inFileId, inNsHost, 'stagerd',
             inLogParams || ' ErrorMessage=' || SQLERRM);
    RETURN serrno.SEINTERNAL;
  END;
  -- If we did not find any valid segment to recall, log a critical error as the file is probably lost
  IF NOT varFoundSeg THEN
    -- log "createRecallCandidate: no segment found for this file. File is probably lost"
    logToDLF(NULL, dlf.LVL_CRIT, dlf.RECALL_NO_SEG_FOUND_AT_ALL, inFileId, inNsHost, 'stagerd', inLogParams);
    RETURN serrno.ESTNOSEGFOUND;
  END IF;
  -- If we found no valid segment (but some disabled ones), log a warning
  IF varAllCopyNbs.COUNT = 0 THEN
    -- log "createRecallCandidate: no valid segment to recall found"
    logToDLF(NULL, dlf.LVL_WARNING, dlf.RECALL_NO_SEG_FOUND, inFileId, inNsHost, 'stagerd', inLogParams);
    RETURN serrno.ESTNOSEGFOUND;
  END IF;
  BEGIN
    -- create missing segments if needed
    createMJForMissingSegments(inCfId, inFileSize, inFileClassId, varAllCopyNbs,
                               varAllVIDs, inFileId, inNsHost, inLogParams);
  EXCEPTION WHEN NO_TAPE_ROUTE THEN
    -- there's at least a missing segment and we cannot recreate it!
    -- log a "no route to tape defined for missing copy" error, but don't fail the recall
    logToDLF(NULL, dlf.LVL_ALERT, dlf.RECALL_MISSING_COPY_NO_ROUTE, inFileId, inNsHost, 'stagerd', inLogParams);
  WHEN OTHERS THEN
    -- some other error happened, log "unexpected error when creating missing copy", but don't fail the recall
    varErrorMsg := 'Oracle error caught : ' || SQLERRM;
    logToDLF(NULL, dlf.LVL_ERROR, dlf.RECALL_MISSING_COPY_ERROR, inFileId, inNsHost, 'stagerd',
      'errorCode=' || to_char(SQLCODE) ||' errorMessage="' || varErrorMsg
      ||'" stackTrace="' || dbms_utility.format_error_backtrace ||'"');
  END;
  RETURN 0;
END;
/

/* PL/SQL method that selects the recallGroup to be used */
CREATE OR REPLACE PROCEDURE getRecallGroup(inEuid IN INTEGER,
                                           inEgid IN INTEGER,
                                           outRecallGroup OUT INTEGER,
                                           outRecallGroupName OUT VARCHAR2) AS
BEGIN
  -- try to find a specific group
  SELECT RecallGroup.id, RecallGroup.name
    INTO outRecallGroup, outRecallGroupName
    FROM RecallGroup, RecallUser
   WHERE (RecallUser.euid = inEuid OR RecallUser.euid IS NULL)
     AND RecallUser.egid = inEgid
     AND RecallGroup.id = RecallUser.recallGroup;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- go back to default
  BEGIN
    SELECT id, name INTO outRecallGroup, outRecallGroupName
      FROM RecallGroup
     WHERE name='default';
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- default is not properly defined. Complain !
    RAISE_APPLICATION_ERROR (-20124, 'Configuration error : no default recallGroup defined');
  END;
END;
/

/* PL/SQL method used by the stager to handle recalls
 * note that this method should only be called with a lock on the concerned CastorFile
 */
CREATE OR REPLACE FUNCTION createRecallCandidate(inSrId IN INTEGER) RETURN INTEGER AS
  varFileId INTEGER;
  varNsHost VARCHAR2(2048);
  varFileName VARCHAR2(2048);
  varSvcClassId VARCHAR2(2048);
  varFileClassId INTEGER;
  varFileSize INTEGER;
  varCfId INTEGER;
  varRecallGroup INTEGER;
  varRecallGroupName VARCHAR2(2048);
  varSubReqUUID VARCHAR2(2048);
  varEuid INTEGER;
  varEgid INTEGER;
  varReqTime NUMBER;
  varReqUUID VARCHAR2(2048);
  varReqId INTEGER;
  varReqType VARCHAR2(2048);
  varIsBeingRecalled INTEGER;
  varRc INTEGER := 0;
BEGIN
  -- get some useful data from CastorFile, subrequest and request
  SELECT castorFile, request, fileName, SubReqId
    INTO varCfId, varReqId, varFileName, varSubReqUUID
    FROM SubRequest WHERE id = inSrId;
  SELECT fileid, nsHost, fileClass, fileSize INTO varFileId, varNsHost, varFileClassId, varFileSize
    FROM CastorFile WHERE id = varCfId;
  SELECT Request.reqId, Request.svcClass, Request.euid, Request.egid, Request.creationTime, Request.reqtype
    INTO varReqUUID, varSvcClassId, varEuid, varEgid, varReqTime, varReqType
    FROM (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */
                 id, svcClass, euid, egid, reqId, creationTime, 'StageGetRequest' as reqtype FROM StageGetRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */
                 id, svcClass, euid, egid, reqId, creationTime, 'StagePrepareToGetRequest' as reqtype FROM StagePrepareToGetRequest UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                 id, svcClass, euid, egid, reqId, creationTime, 'StageUpdateRequest' as reqtype FROM StageUpdateRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequest_Id) */
                 id, svcClass, euid, egid, reqId, creationTime, 'StagePrepareToUpdateRequest' as reqtype FROM StagePrepareToUpdateRequest) Request
   WHERE Request.id = varReqId;
  -- get the RecallGroup
  getRecallGroup(varEuid, varEgid, varRecallGroup, varRecallGroupName);
  -- check whether this file is already being recalled for this RecallGroup
  -- or being actively recalled by any group
  SELECT count(*) INTO varIsBeingRecalled
    FROM RecallJob
   WHERE castorFile = varCfId
     AND (recallGroup = varRecallGroup OR status = tconst.RECALLJOB_SELECTED);
  -- trigger recall only if recall is not already ongoing
  IF varIsBeingRecalled != 0 THEN
    -- createRecallCandidate: found already running recall
    logToDLF(NULL, dlf.LVL_SYSTEM, dlf.RECALL_FOUND_ONGOING_RECALL, varFileId, varNsHost, 'stagerd',
             'FileName=' || varFileName || ' REQID=' || varReqUUID ||
             ' SUBREQID=' || varSubReqUUID || ' RecallGroup=' || varRecallGroupName ||
             ' RequestType=' || varReqType);
  ELSE
    varRc := createRecallJobs(varCfId, varFileId, varNsHost, varFileSize, varFileClassId,
                              varRecallGroup, varSvcClassId, varEuid, varEgid, varReqTime,
                              'FileName=' || varFileName || ' REQID=' || varReqUUID ||
                              ' SUBREQID=' || varSubReqUUID || ' RecallGroup=' || varRecallGroupName ||
                              ' RequestType=' || varReqType);
  END IF;
  -- update the state of the SubRequest
  IF varRc = 0 THEN
    UPDATE Subrequest SET status = dconst.SUBREQUEST_WAITTAPERECALL WHERE id = inSrId;
    RETURN dconst.SUBREQUEST_WAITTAPERECALL;
  ELSE
    UPDATE Subrequest
       SET status = dconst.SUBREQUEST_FAILED,
           errorCode = varRc
     WHERE id = inSrId;
    RETURN dconst.SUBREQUEST_FAILED;
  END IF;
END;
/

CREATE OR REPLACE PROCEDURE deleteDiskCopies(inDcIds IN castor."cnumList", inFileIds IN castor."cnumList", inForce IN BOOLEAN, inDryRun IN BOOLEAN, outRes OUT castor.DiskCopyResult_Cur, outDiskPool OUT VARCHAR2) AS
  varNsHost VARCHAR2(100);
  varFileName VARCHAR2(2048);
  varCfId INTEGER;
  varNbRemaining INTEGER;
  varStatus INTEGER;
  varFStatus VARCHAR2(1);
  varLogParams VARCHAR2(2048);
BEGIN
  DELETE FROM DeleteDiskCopyHelper;
  FOR i IN 1..inDcIds.COUNT LOOP
    BEGIN
      -- get data and lock
      SELECT castorFile, DiskCopy.status, DiskPool.name
        INTO varCfId, varStatus, outDiskPool
        FROM DiskPool, FileSystem, DiskCopy
       WHERE DiskCopy.id = inDcIds(i)
         AND DiskCopy.fileSystem = FileSystem.id
         AND FileSystem.diskPool = DiskPool.id;
      SELECT nsHost, lastKnownFileName
        INTO varNsHost, varFileName
        FROM CastorFile
       WHERE id = varCfId
         AND fileId = inFileIds(i)
         FOR UPDATE;
      varLogParams := 'FileName="' || varFileName ||'" DiskPool="'|| outDiskPool
        ||'" dcId='|| inDcIds(i) ||' status='
        || getObjStatusName('DiskCopy', 'status', varStatus);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- diskcopy not found in stager
      INSERT INTO DeleteDiskCopyHelper (dcId, rc)
        VALUES (inDcIds(i), dconst.DELDC_ENOENT);
      COMMIT;
      CONTINUE;
    END;
    BEGIN
      -- get the Nameserver status in case we have to also drop the namespace entry
      SELECT status INTO varFStatus FROM Cns_file_metadata@RemoteNS
       WHERE fileid = inFileIds(i);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- not found in the Nameserver means that we can scrap everything and there's no data loss
      -- as we're anticipating the NS synchronization
      varFStatus := 'd';
    END;
    -- count remaining ones
    SELECT count(*) INTO varNbRemaining FROM DiskCopy
     WHERE castorFile = varCfId
       AND status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_CANBEMIGR)
       AND id != inDcIds(i);
    IF inForce = TRUE THEN
      -- the physical diskcopy is deemed lost: delete the diskcopy entry
      -- and potentially drop dangling entities
      IF NOT inDryRun THEN
        DELETE FROM DiskCopy WHERE id = inDcIds(i);
      END IF;
      IF varStatus = dconst.DISKCOPY_STAGEOUT THEN
        -- fail outstanding requests
        UPDATE SubRequest
           SET status = dconst.SUBREQUEST_FAILED,
               errorCode = serrno.SEINTERNAL,
               errorMessage = 'File got lost while being written to'
         WHERE diskCopy = inDcIds(i)
           AND status = dconst.SUBREQUEST_READY;
      END IF;
      -- was it the last active one?
      IF varNbRemaining = 0 THEN
        IF varStatus = dconst.DISKCOPY_CANBEMIGR AND NOT inDryRun THEN
          -- yes, drop the (now bound to fail) migration job(s)
          deleteMigrationJobs(varCfId);
        END IF;
        -- check if the entire castorFile chain can be dropped
        IF NOT inDryRun THEN
          deleteCastorFile(varCfId);
        END IF;
        IF varFStatus = 'm' THEN
          -- file is on tape: let's recall it. This may potentially trigger a new migration
          INSERT INTO DeleteDiskCopyHelper (dcId, rc)
            VALUES (inDcIds(i), dconst.DELDC_RECALL);
          IF NOT inDryRun THEN
            logToDLF(NULL, dlf.LVL_SYSTEM, dlf.DELETEDISKCOPY_RECALL, inFileIds(i), varNsHost, 'stagerd', varLogParams);
          END IF;
        ELSIF varFStatus = 'd' THEN
          -- file was dropped, report as if we have run a standard GC
          INSERT INTO DeleteDiskCopyHelper (dcId, rc)
            VALUES (inDcIds(i), dconst.DELDC_GC);
          IF NOT inDryRun THEN
            logToDLF(NULL, dlf.LVL_SYSTEM, dlf.DELETEDISKCOPY_GC, inFileIds(i), varNsHost, 'stagerd', varLogParams);
          END IF;
        ELSE
          -- file is really lost, we'll remove the namespace entry afterwards
          INSERT INTO DeleteDiskCopyHelper (dcId, rc)
            VALUES (inDcIds(i), dconst.DELDC_LOST);
          IF NOT inDryRun THEN
            logToDLF(NULL, dlf.LVL_WARNING, dlf.DELETEDISKCOPY_LOST, inFileIds(i), varNsHost, 'stagerd', varLogParams);
          END IF;
        END IF;
      ELSE
        -- it was not the last valid copy, replicate from another one
        INSERT INTO DeleteDiskCopyHelper (dcId, rc)
          VALUES (inDcIds(i), dconst.DELDC_REPLICATION);
        IF NOT inDryRun THEN
          logToDLF(NULL, dlf.LVL_SYSTEM, dlf.DELETEDISKCOPY_REPLICATION, inFileIds(i), varNsHost, 'stagerd', varLogParams);
        END IF;
      END IF;
    ELSE
      -- similarly to stageRm, check that the deletion is allowed:
      -- basically only STAGED files may be dropped in case no data loss is provoked,
      -- or files already dropped from the namespace. The rest is forbidden.
      IF (varStatus = dconst.DISKCOPY_STAGED AND (varNbRemaining > 0 OR varFStatus = 'm'))
         OR varFStatus = 'd' THEN
        INSERT INTO DeleteDiskCopyHelper (dcId, rc)
          VALUES (inDcIds(i), dconst.DELDC_GC);
        IF NOT inDryRun THEN
          UPDATE DiskCopy
             SET status = dconst.DISKCOPY_INVALID, gcType = dconst.GCTYPE_ADMIN
           WHERE id = inDcIds(i);
           logToDLF(NULL, dlf.LVL_SYSTEM, dlf.DELETEDISKCOPY_GC, inFileIds(i), varNsHost, 'stagerd', varLogParams);
        END IF;
      ELSE
        -- nothing is done, just record no-action
        INSERT INTO DeleteDiskCopyHelper (dcId, rc)
          VALUES (inDcIds(i), dconst.DELDC_NOOP);
        IF NOT inDryRun THEN
          logToDLF(NULL, dlf.LVL_SYSTEM, dlf.DELETEDISKCOPY_NOOP, inFileIds(i), varNsHost, 'stagerd',   varLogParams);
        END IF;
        COMMIT;
        CONTINUE;
      END IF;
    END IF;
    COMMIT;   -- release locks file by file
  END LOOP;
  -- return back all results for the python script to post-process them,
  -- including performing all required acations
  OPEN outRes FOR
    SELECT dcId, rc FROM DeleteDiskCopyHelper;
END;
/
