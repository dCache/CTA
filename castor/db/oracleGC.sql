/*******************************************************************
 *
 *
 * PL/SQL code for stager cleanup and garbage collecting
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PL/SQL declaration for the castorGC package */
CREATE OR REPLACE PACKAGE castorGC AS
  TYPE SelectFiles2DeleteLine IS RECORD (
        path VARCHAR2(2048),
        id NUMBER,
        fileId NUMBER,
        nsHost VARCHAR2(2048),
        lastAccessTime INTEGER,
        nbAccesses NUMBER,
        gcWeight NUMBER,
        gcTriggeredBy VARCHAR2(2048),
        svcClassName VARCHAR2(2048));
  TYPE SelectFiles2DeleteLine_Cur IS REF CURSOR RETURN SelectFiles2DeleteLine;
  -- find out a gc function to be used from a given serviceClass
  FUNCTION getUserWeight(svcClassId NUMBER) RETURN VARCHAR2;
  FUNCTION getRecallWeight(svcClassId NUMBER) RETURN VARCHAR2;
  FUNCTION getCopyWeight(svcClassId NUMBER) RETURN VARCHAR2;
  FUNCTION getFirstAccessHook(svcClassId NUMBER) RETURN VARCHAR2;
  FUNCTION getAccessHook(svcClassId NUMBER) RETURN VARCHAR2;
  FUNCTION getUserSetGCWeight(svcClassId NUMBER) RETURN VARCHAR2;
  -- compute gcWeight from size
  FUNCTION size2GCWeight(s NUMBER) RETURN NUMBER;
  -- Default gc policy
  FUNCTION sizeRelatedUserWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION sizeRelatedRecallWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION sizeRelatedCopyWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION dayBonusFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER;
  FUNCTION halfHourBonusAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER;
  FUNCTION cappedUserSetGCWeight(oldGcWeight NUMBER, userDelta NUMBER) RETURN NUMBER;
  -- FIFO gc policy
  FUNCTION creationTimeUserWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION creationTimeRecallWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION creationTimeCopyWeight(fileSize NUMBER) RETURN NUMBER;
  -- LRU gc policy
  FUNCTION LRUFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER;
  FUNCTION LRUAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER;
  FUNCTION LRUpinUserSetGCWeight(oldGcWeight NUMBER, userDelta NUMBER) RETURN NUMBER;
END castorGC;
/

CREATE OR REPLACE PACKAGE BODY castorGC AS

  FUNCTION getUserWeight(svcClassId NUMBER) RETURN VARCHAR2 AS
    ret VARCHAR2(2048);
  BEGIN
    SELECT userWeight INTO ret
      FROM SvcClass, GcPolicy
     WHERE SvcClass.id = svcClassId
       AND SvcClass.gcPolicy = GcPolicy.name;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- we did not get any policy, let's go for the default
    SELECT userWeight INTO ret
      FROM GcPolicy
     WHERE GcPolicy.name = 'default';
    RETURN ret;
  END;

  FUNCTION getRecallWeight(svcClassId NUMBER) RETURN VARCHAR2 AS
    ret VARCHAR2(2048);
  BEGIN
    SELECT recallWeight INTO ret
      FROM SvcClass, GcPolicy
     WHERE SvcClass.id = svcClassId
       AND SvcClass.gcPolicy = GcPolicy.name;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- we did not get any policy, let's go for the default
    SELECT recallWeight INTO ret
      FROM GcPolicy
     WHERE GcPolicy.name = 'default';
    RETURN ret;
  END;

  FUNCTION getCopyWeight(svcClassId NUMBER) RETURN VARCHAR2 AS
    ret VARCHAR2(2048);
  BEGIN
    SELECT copyWeight INTO ret
      FROM SvcClass, GcPolicy
     WHERE SvcClass.id = svcClassId
       AND SvcClass.gcPolicy = GcPolicy.name;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- we did not get any policy, let's go for the default
    SELECT copyWeight INTO ret
      FROM GcPolicy
     WHERE GcPolicy.name = 'default';
    RETURN ret;
  END;

  FUNCTION getFirstAccessHook(svcClassId NUMBER) RETURN VARCHAR2 AS
    ret VARCHAR2(2048);
  BEGIN
    SELECT firstAccessHook INTO ret
      FROM SvcClass, GcPolicy
     WHERE SvcClass.id = svcClassId
       AND SvcClass.gcPolicy = GcPolicy.name;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RETURN NULL;
  END;

  FUNCTION getAccessHook(svcClassId NUMBER) RETURN VARCHAR2 AS
    ret VARCHAR2(2048);
  BEGIN
    SELECT accessHook INTO ret
      FROM SvcClass, GcPolicy
     WHERE SvcClass.id = svcClassId
       AND SvcClass.gcPolicy = GcPolicy.name;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RETURN NULL;
  END;

  FUNCTION getUserSetGCWeight(svcClassId NUMBER) RETURN VARCHAR2 AS
    ret VARCHAR2(2048);
  BEGIN
    SELECT userSetGCWeight INTO ret
      FROM SvcClass, GcPolicy
     WHERE SvcClass.id = svcClassId
       AND SvcClass.gcPolicy = GcPolicy.name;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RETURN NULL;
  END;

  FUNCTION size2GCWeight(s NUMBER) RETURN NUMBER IS
  BEGIN
    IF s < 1073741824 THEN
      RETURN 1073741824/(s+1)*86400 + getTime();  -- 1GB/filesize (days) + current time as lastAccessTime
    ELSE
      RETURN 86400 + getTime();  -- the value for 1G file. We do not make any difference for big files and privilege FIFO
    END IF;
  END;

  FUNCTION sizeRelatedUserWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN size2GCWeight(fileSize);
  END;

  FUNCTION sizeRelatedRecallWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN size2GCWeight(fileSize);
  END;

  FUNCTION sizeRelatedCopyWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN size2GCWeight(fileSize);
  END;

  FUNCTION dayBonusFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN oldGcWeight - 86400;
  END;

  FUNCTION halfHourBonusAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN oldGcWeight + 1800;
  END;

  FUNCTION cappedUserSetGCWeight(oldGcWeight NUMBER, userDelta NUMBER) RETURN NUMBER AS
  BEGIN
    IF userDelta >= 18000 THEN -- 5h max
      RETURN oldGcWeight + 18000;
    ELSE
      RETURN oldGcWeight + userDelta;
    END IF;
  END;

  -- FIFO gc policy
  FUNCTION creationTimeUserWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION creationTimeRecallWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION creationTimeCopyWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  -- LRU and LRUpin gc policy
  FUNCTION LRUFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION LRUAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION LRUpinUserSetGCWeight(oldGcWeight NUMBER, userDelta NUMBER) RETURN NUMBER AS
  BEGIN
    IF userDelta >= 2592000 THEN -- 30 days max
      RETURN oldGcWeight + 2592000;
    ELSE
      RETURN oldGcWeight + userDelta;
    END IF;
  END;

END castorGC;
/

/* PL/SQL method implementing selectFiles2Delete
   This is the standard garbage collector: it sorts VALID diskcopies
   that do not need to go to tape by gcWeight and selects them for deletion up to
   the desired free space watermark */
CREATE OR REPLACE
PROCEDURE selectFiles2Delete(diskServerName IN VARCHAR2,
                             files OUT castorGC.SelectFiles2DeleteLine_Cur) AS
  dcIds "numList";
  freed INTEGER;
  deltaFree INTEGER;
  toBeFreed INTEGER;
  dontGC INTEGER;
  totalCount INTEGER;
  unused INTEGER;
  backoff INTEGER;
  CastorFileLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (CastorFileLocked, -54);
BEGIN
  -- First of all, check if we are in a Disk1 pool
  dontGC := 0;
  FOR sc IN (SELECT disk1Behavior
               FROM SvcClass, DiskPool2SvcClass D2S, DiskServer, FileSystem
              WHERE SvcClass.id = D2S.child
                AND D2S.parent = FileSystem.diskPool
                AND FileSystem.diskServer = DiskServer.id
                AND DiskServer.name = diskServerName
              UNION ALL
             SELECT disk1Behavior
               FROM SvcClass, DataPool2SvcClass, DiskServer
              WHERE SvcClass.id = DataPool2SvcClass.child
                AND DataPool2SvcClass.parent = DiskServer.dataPool
                AND DiskServer.name = diskServerName) LOOP
    -- If any of the service classes to which we belong (normally a single one)
    -- say this is Disk1, we don't GC files.
    IF sc.disk1Behavior = 1 THEN
      dontGC := 1;
      EXIT;
    END IF;
  END LOOP;

  -- Loop on all concerned fileSystems/DataPools in a random order.
  totalCount := 0;
  FOR fs IN (SELECT * FROM (SELECT FileSystem.id AS fsId
                              FROM FileSystem, DiskServer
                             WHERE FileSystem.diskServer = DiskServer.id
                               AND DiskServer.name = diskServerName
                             UNION ALL
                            SELECT DiskServer.dataPool AS fsId
                              FROM DiskServer
                             WHERE DiskServer.name = diskServerName)
             ORDER BY dbms_random.value) LOOP
    -- Count the number of diskcopies on this filesystem that are in a
    -- BEINGDELETED state. These need to be reselected in any case.
    freed := 0;
    SELECT totalCount + count(*), nvl(sum(DiskCopy.diskCopySize), 0)
      INTO totalCount, freed
      FROM DiskCopy
     WHERE (fileSystem = fs.fsId OR dataPool = fs.fsId)
       AND decode(status, 9, status, NULL) = 9;  -- BEINGDELETED (decode used to use function-based index)

    -- estimate the number of GC running the "long" query, that is the one dealing with the GCing of
    -- VALID files.
    SELECT COUNT(*) INTO backoff
      FROM v$session s, v$sqltext t
     WHERE s.sql_id = t.sql_id AND t.sql_text LIKE '%I_DiskCopy_FS_GCW%';

    -- Process diskcopies that are in an INVALID state.
    UPDATE /*+ INDEX_RS_ASC(DiskCopy I_DiskCopy_Status_7_FS_DP)) */ DiskCopy
       SET status = 9, -- BEINGDELETED
           gcType = decode(gcType, NULL, dconst.GCTYPE_USER, gcType)
     WHERE (nvl(fileSystem,0)+nvl(dataPool,0) = fs.fsId)
       AND decode(status, 7, status, NULL) = 7  -- INVALID (decode used to use function-based index)
       AND rownum <= 10000 - totalCount
    RETURNING id BULK COLLECT INTO dcIds;
    COMMIT;

    -- If we have more than 10,000 files to GC, exit the loop. There is no point
    -- processing more as the maximum sent back to the client in one call is
    -- 10,000. This protects the garbage collector from being overwhelmed with
    -- requests and reduces the stager DB load. Furthermore, if too much data is
    -- sent back to the client, the transfer time between the stager and client
    -- becomes very long and the message may timeout or may not even fit in the
    -- clients receive buffer!
    totalCount := totalCount + dcIds.COUNT();
    EXIT WHEN totalCount >= 10000;

    -- Continue processing but with VALID files, only in case we are not already loaded
    IF dontGC = 0 AND backoff < 4 THEN
      -- Do not delete VALID files from non production hardware
      BEGIN
        IF fs.fsId > 0 THEN
          SELECT FileSystem.id INTO unused
            FROM DiskServer, FileSystem
           WHERE FileSystem.id = fs.fsId
             AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_READONLY)
             AND FileSystem.diskserver = DiskServer.id
             AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
             AND DiskServer.hwOnline = 1;
        ELSE
          SELECT DiskServer.id INTO unused
            FROM DiskServer
           WHERE dataPool = fs.fsId
             AND status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_READONLY)
             AND hwOnline = 1
             AND ROWNUM < 2;
        END IF;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        EXIT;
      END;
      -- Calculate the amount of space that would be freed on the filesystem
      -- if the files selected above were to be deleted.
      IF dcIds.COUNT > 0 THEN
        SELECT /*+ INDEX(DiskCopy PK_DiskCopy_Id) */ freed + sum(diskCopySize) INTO freed
          FROM DiskCopy
         WHERE DiskCopy.id IN
             (SELECT /*+ CARDINALITY(fsidTable 5) */ *
                FROM TABLE(dcIds) dcidTable);
      END IF;
      -- Get the amount of space to be liberated
      IF fs.fsId > 0 THEN
        SELECT decode(sign(maxFreeSpace * totalSize - free), -1, 0, maxFreeSpace * totalSize - free)
          INTO toBeFreed
          FROM FileSystem
         WHERE id = fs.fsId;
      ELSE
        SELECT decode(sign(maxFreeSpace * totalSize - free), -1, 0, maxFreeSpace * totalSize - free)
          INTO toBeFreed
          FROM DataPool
         WHERE id = fs.fsId;
      END IF;
      -- If space is still required even after removal of INVALID files, consider
      -- removing VALID files until we are below the free space watermark
      IF freed < toBeFreed THEN
        -- Loop on file deletions
        FOR dc IN (SELECT /*+ INDEX_RS_ASC(DiskCopy I_DiskCopy_FS_DP_GCW) */ DiskCopy.id, castorFile
                     FROM DiskCopy, CastorFile
                    WHERE (nvl(fileSystem,0)+nvl(dataPool,0) = fs.fsId)
                      AND status = dconst.DISKCOPY_VALID
                      AND CastorFile.id = DiskCopy.castorFile
                      AND CastorFile.tapeStatus IN (dconst.CASTORFILE_DISKONLY, dconst.CASTORFILE_ONTAPE)
                      ORDER BY gcWeight ASC) LOOP
          BEGIN
            -- Lock the CastorFile
            SELECT id INTO unused FROM CastorFile
             WHERE id = dc.castorFile FOR UPDATE NOWAIT;
            -- Mark the DiskCopy as being deleted
            UPDATE DiskCopy
               SET status = dconst.DISKCOPY_BEINGDELETED,
                   gcType = dconst.GCTYPE_AUTO
             WHERE id = dc.id RETURNING diskCopySize INTO deltaFree;
            totalCount := totalCount + 1;
            -- Update freed space
            freed := freed + deltaFree;
            -- update importance of remianing copies of the file if any
            UPDATE DiskCopy
               SET importance = importance + 1
             WHERE castorFile = dc.castorFile
               AND status = dconst.DISKCOPY_VALID;
            -- Shall we continue ?
            IF toBeFreed <= freed THEN
              EXIT;
            END IF;
            IF totalCount >= 10000 THEN
              EXIT;
            END IF;           
          EXCEPTION
            WHEN NO_DATA_FOUND THEN
              -- The file no longer exists or has the wrong state
              NULL;
            WHEN CastorFileLocked THEN
              -- Go to the next candidate, processing is taking place on the
              -- file
              NULL;
          END;
          COMMIT;
        END LOOP;
      END IF;
    END IF;
    -- We have enough files to exit the loop ?
    EXIT WHEN totalCount >= 10000;
  END LOOP;

  -- Now select all the BEINGDELETED diskcopies in this diskserver for the GC daemon
  OPEN files FOR
    SELECT /*+ INDEX(CastorFile PK_CastorFile_ID) */
           DC.path, DC.id,
           Castorfile.fileid, Castorfile.nshost,
           DC.lastAccessTime, DC.nbCopyAccesses, DC.gcWeight,
           DC.gcType, DC.svcClassList
      FROM CastorFile,
           (SELECT DiskCopy.castorFile,
                   FileSystem.mountPoint || DiskCopy.path AS path, DiskCopy.id,
                   DiskCopy.lastAccessTime, DiskCopy.nbCopyAccesses, DiskCopy.gcWeight,
                   getObjStatusName('DiskCopy', 'gcType', DiskCopy.gcType) AS gcType,
                   getSvcClassList(FileSystem.id) AS svcClassList
              FROM FileSystem, DiskServer, DiskCopy
             WHERE decode(DiskCopy.status, 9, DiskCopy.status, NULL) = 9 -- BEINGDELETED
               AND DiskCopy.fileSystem = FileSystem.id
               AND FileSystem.diskServer = DiskServer.id
               AND DiskServer.name = diskServerName
             UNION ALL
            SELECT DiskCopy.castorFile,
                   DataPool.name || '/' || DiskCopy.path AS path, DiskCopy.id,
                   DiskCopy.lastAccessTime, DiskCopy.nbCopyAccesses, DiskCopy.gcWeight,
                   getObjStatusName('DiskCopy', 'gcType', DiskCopy.gcType) AS gcType,
                   getSvcClassListDP(DiskServer.dataPool) AS svcClassList
              FROM DiskServer, DiskCopy, DataPool
             WHERE decode(DiskCopy.status, 9, DiskCopy.status, NULL) = 9 -- BEINGDELETED
               AND DiskCopy.dataPool = DataPool.id
               AND DiskCopy.dataPool = DiskServer.dataPool
               AND DiskServer.name = diskServerName
               AND ROWNUM < 2) DC
     WHERE DC.castorfile = CastorFile.id
       AND rownum <= 10000;
END;
/


/*
 * PL/SQL method implementing filesDeleted
 * Note that we don't increase the freespace of the fileSystem.
 * This is done by the monitoring daemon, that knows the
 * exact amount of free space.
 * dcIds gives the list of diskcopies to delete.
 * fileIds returns the list of castor files to be removed
 * from the name server
 */
CREATE OR REPLACE PROCEDURE filesDeletedProc
(dcIds IN castor."cnumList",
 fileIds OUT castor.FileList_Cur) AS
  fid NUMBER;
  fc NUMBER;
  nsh VARCHAR2(2048);
  nb INTEGER;
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
BEGIN
  IF dcIds.COUNT > 0 THEN
    -- List the castorfiles to be cleaned up afterwards
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      INSERT INTO FilesDeletedProcHelper (cfId, dcId) (
        SELECT castorFile, id FROM DiskCopy
         WHERE id = dcIds(i));
    -- Use a normal loop to clean castorFiles. Note: We order the list to
    -- prevent a deadlock
    FOR cf IN (SELECT cfId, dcId
                 FROM filesDeletedProcHelper
                ORDER BY cfId ASC) LOOP
      BEGIN
        -- Get data and lock the castorFile
        SELECT fileId, nsHost, fileClass
          INTO fid, nsh, fc
          FROM CastorFile
         WHERE id = cf.cfId FOR UPDATE;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- This means that the castorFile did not exist.
        -- There is thus no way to find out whether to remove the
        -- file from the nameserver. For safety, we thus keep it
        CONTINUE;
      END;
      -- delete the original diskcopy to be dropped
      DELETE FROM DiskCopy WHERE id = cf.dcId;
      -- Cleanup: attempt to delete the CastorFile. Thanks to FKs,
      -- this will fail if in the meantime some other activity took
      -- ownership of the CastorFile entry.
      BEGIN
        DELETE FROM CastorFile WHERE id = cf.cfId;
        -- And check whether this file potentially had copies on tape
        SELECT nbCopies INTO nb FROM FileClass WHERE id = fc;
        IF nb = 0 THEN
          -- This castorfile was created with no copy on tape
          -- So removing it from the stager means erasing
          -- it completely. We should thus also remove it
          -- from the name server
          INSERT INTO FilesDeletedProcOutput (fileId, nsHost) VALUES (fid, nsh);
        END IF;
      EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
        -- Ignore the deletion, some draining/rebalancing/recall activity
        -- started to reuse the CastorFile
        NULL;
      END;
    END LOOP;
  END IF;
  OPEN fileIds FOR
    SELECT fileId, nsHost FROM FilesDeletedProcOutput;
END;
/

/* PL/SQL method implementing filesDeletionFailedProc */
CREATE OR REPLACE PROCEDURE filesDeletionFailedProc
(dcIds IN castor."cnumList") AS
  cfId NUMBER;
BEGIN
  IF dcIds.COUNT > 0 THEN
    -- Loop over the files
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      UPDATE DiskCopy SET status = 4 -- FAILED
       WHERE id = dcIds(i);
  END IF;
END;
/



/* PL/SQL method implementing nsFilesDeletedProc */
CREATE OR REPLACE PROCEDURE nsFilesDeletedProc
(nh IN VARCHAR2,
 fileIds IN castor."cnumList",
 orphans OUT castor.IdRecord_Cur) AS
  unused NUMBER;
  nsHostName VARCHAR2(2048);
BEGIN
  IF fileIds.COUNT <= 0 THEN
    RETURN;
  END IF;
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  -- Loop over the deleted files and split the orphan ones
  -- from the normal ones
  FOR fid in fileIds.FIRST .. fileIds.LAST LOOP
    BEGIN
      SELECT id INTO unused FROM CastorFile
       WHERE fileid = fileIds(fid) AND nsHost = nsHostName;
      stageForcedRm(fileIds(fid), nsHostName, dconst.GCTYPE_NSSYNCH);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- this file was dropped from nameServer AND stager
      -- and still exists on disk. We put it into the list
      -- of orphan fileids to return
      INSERT INTO NsFilesDeletedOrphans (fileId) VALUES (fileIds(fid));
    END;
  END LOOP;
  -- return orphan ones
  OPEN orphans FOR SELECT * FROM NsFilesDeletedOrphans;
END;
/


/* PL/SQL method implementing stgFilesDeletedProc */
CREATE OR REPLACE PROCEDURE stgFilesDeletedProc
(dcIds IN castor."cnumList",
 stgOrphans OUT castor.IdRecord_Cur) AS
  unused NUMBER;
BEGIN
  -- Nothing to do
  IF dcIds.COUNT <= 0 THEN
    RETURN;
  END IF;
  -- Insert diskcopy ids into a temporary table
  FORALL i IN dcIds.FIRST..dcIds.LAST
   INSERT INTO StgFilesDeletedOrphans (diskCopyId) VALUES (dcIds(i));
  -- Return a list of diskcopy ids which no longer exist
  OPEN stgOrphans FOR
    SELECT diskCopyId FROM StgFilesDeletedOrphans
     WHERE NOT EXISTS (
        SELECT /*+ INDEX(DiskCopy PK_DiskCopy_Id) */ 'x' FROM DiskCopy
         WHERE id = diskCopyId);
END;
/


/** Cleanup job **/

/* A little generic method to delete efficiently */
CREATE OR REPLACE PROCEDURE bulkDelete(sel IN VARCHAR2, tab IN VARCHAR2) AS
BEGIN
  EXECUTE IMMEDIATE
  'DECLARE
    CURSOR s IS '||sel||'
    ids "numList";
  BEGIN
    LOOP
      OPEN s;
      FETCH s BULK COLLECT INTO ids LIMIT 100000;
      EXIT WHEN ids.count = 0;
      FORALL i IN 1 .. ids.COUNT
        DELETE FROM '||tab||' WHERE id = ids(i);
      CLOSE s;
      COMMIT;
    END LOOP;
  END;';
END;
/

/* A generic method to delete requests of a given type */
CREATE OR REPLACE Procedure bulkDeleteRequests(reqType IN VARCHAR) AS
BEGIN
  bulkDelete('SELECT id FROM '|| reqType ||' R WHERE
    NOT EXISTS (SELECT 1 FROM SubRequest WHERE request = R.id);',
    reqType);
END;
/

/* Search and delete old archived/failed subrequests and their requests */
CREATE OR REPLACE PROCEDURE deleteTerminatedRequests AS
  failuresTimeOut INTEGER;
  successesTimeOut INTEGER;
  rate INTEGER;
  srIds "numList";
  ct NUMBER;
BEGIN
  -- select requested timeout for failed requests from configuration table
  failuresTimeOut := 3600*TO_NUMBER(getConfigOption('cleaning', 'failedRequestsTimeout', '168'));  -- 1 week
  -- compute a rate-dependent timeout for the successful requests by looking at the
  -- last half-hour of activity: keep max 1M of them.
  SELECT 1800 * 1000000 / (count(*)+1) INTO successesTimeOut
    FROM SubRequest
   WHERE status = dconst.SUBREQUEST_ARCHIVED
     AND lastModificationTime > getTime() - 1800;
  IF successesTimeOut > failuresTimeOut THEN
    -- in case of light load, don't keep successful request for longer than failed ones
    successesTimeOut := failuresTimeOut;
  END IF;
  
  -- Delete castorFiles if nothing is left for them. Here we use
  -- a temporary table as we need to commit every ~1000 operations
  -- and keeping a cursor opened on the original select may take
  -- too long, leading to ORA-01555 'snapshot too old' errors.
  EXECUTE IMMEDIATE 'TRUNCATE TABLE DeleteTermReqHelper';
  INSERT /*+ APPEND */ INTO DeleteTermReqHelper (srId, cfId)
    (SELECT SR.id, castorFile FROM SubRequest SR
      WHERE (SR.status = dconst.SUBREQUEST_ARCHIVED
             AND SR.lastModificationTime < getTime() - successesTimeOut)
         -- failed subrequests are kept according to the configured timeout
         OR (SR.status = dconst.SUBREQUEST_FAILED_FINISHED
             AND reqType != 119 AND SR.lastModificationTime < getTime() - failuresTimeOut));  -- StageRepackRequest
  COMMIT;  -- needed otherwise the next statement raises
           -- ORA-12838: cannot read/modify an object after modifying it in parallel
  -- 2nd part, separated from above for efficiency reasons
  INSERT /*+ APPEND */ INTO DeleteTermReqHelper (srId, cfId)
    (SELECT SR.id, castorFile FROM SubRequest SR, StageRepackRequest R
      WHERE SR.status = dconst.SUBREQUEST_FAILED_FINISHED
         -- only for the Repack case, we keep all failed subrequests around until
         -- the whole Repack request is over for more than <timeOut> seconds
        AND reqType = 119 AND R.lastModificationTime < getTime() - failuresTimeOut  -- StageRepackRequest
        AND R.id = SR.request);
  COMMIT;
  SELECT count(*) INTO ct FROM DeleteTermReqHelper;
  logToDLF(NULL, dlf.LVL_SYSTEM, dlf.DELETING_REQUESTS, 0, '', 'stagerd',
    'SubRequestsCount=' || ct);
  ct := 0;
  FOR cf IN (SELECT UNIQUE cfId FROM DeleteTermReqHelper) LOOP
    deleteCastorFile(cf.cfId);
    ct := ct + 1;
    IF ct = 1000 THEN
      COMMIT;
      ct := 0;
    END IF;
  END LOOP;

  -- Now delete all old subRequests. We reuse here the
  -- temporary table, which serves as a snapshot of the
  -- entries to be deleted, and we use the FORALL logic
  -- (cf. bulkDelete) instead of a simple DELETE ...
  -- WHERE id IN (SELECT srId FROM DeleteTermReqHelper)
  -- for efficiency reasons. Moreover, we don't risk
  -- here the ORA-01555 error keeping the cursor open
  -- between commits as we are selecting on our
  -- temporary table.
  DECLARE
    CURSOR s IS
      SELECT srId FROM DeleteTermReqHelper;
    ids "numList";
  BEGIN
    OPEN s;
    LOOP
      FETCH s BULK COLLECT INTO ids LIMIT 10000;
      EXIT WHEN ids.count = 0;
      FORALL i IN 1 .. ids.COUNT
        DELETE FROM SubRequest WHERE id = ids(i);
      COMMIT;
    END LOOP;
    CLOSE s;
  END;
  EXECUTE IMMEDIATE 'TRUNCATE TABLE DeleteTermReqHelper';

  -- And then related Requests, now orphaned.
    ---- Get ----
  bulkDeleteRequests('StageGetRequest');
    ---- Put ----
  bulkDeleteRequests('StagePutRequest');
    ---- PrepareToGet -----
  bulkDeleteRequests('StagePrepareToGetRequest');
    ---- PrepareToPut ----
  bulkDeleteRequests('StagePrepareToPutRequest');
    ---- PutDone ----
  bulkDeleteRequests('StagePutDoneRequest');
    ---- Rm ----
  bulkDeleteRequests('StageRmRequest');
    ---- SetGCWeight ----
  bulkDeleteRequests('SetFileGCWeight');

  -- Finally deal with Repack: this case is different because StageRepackRequests may be empty
  -- at the beginning. Therefore we only drop repacks that are in a completed state
  -- for more than the requested time.
  -- First failed ones (status FAILED, ABORTED)
  bulkDelete('SELECT id FROM StageRepackRequest R WHERE status IN (3, 5)
    AND NOT EXISTS (SELECT 1 FROM SubRequest WHERE request = R.id)
    AND lastModificationTime < getTime() - ' || failuresTimeOut || ';',
    'StageRepackRequest');
  -- Then successful ones (status FINISHED)
  bulkDelete('SELECT id FROM StageRepackRequest R WHERE status = 2
    AND NOT EXISTS (SELECT 1 FROM SubRequest WHERE request = R.id)
    AND lastModificationTime < getTime() - ' || successesTimeOut || ';',
    'StageRepackRequest');
END;
/

/* Search and delete old diskCopies in bad states */
CREATE OR REPLACE PROCEDURE deleteFailedDiskCopies(timeOut IN NUMBER) AS
  dcIds "numList";
  cfIds "numList";
BEGIN
  LOOP
    -- select INVALID diskcopies without filesystem (they can exist after a
    -- stageRm that came before the diskcopy had been created on disk) and ALL FAILED
    -- ones (coming from failed recalls or failed removals from the GC daemon).
    -- Note that we don't select INVALID diskcopies from recreation of files
    -- because they are taken by the standard GC as they physically exist on disk.
    -- go only for 1000 at a time and retry if the limit was reached
    SELECT id
      BULK COLLECT INTO dcIds
      FROM DiskCopy
     WHERE (status = 4 OR (status = 7 AND fileSystem = 0))
       AND creationTime < getTime() - timeOut
       AND ROWNUM <= 1000;
    SELECT /*+ INDEX(DC PK_DiskCopy_ID) */ UNIQUE castorFile
      BULK COLLECT INTO cfIds
      FROM DiskCopy DC
     WHERE id IN (SELECT /*+ CARDINALITY(ids 5) */ * FROM TABLE(dcIds) ids);
    -- drop the DiskCopies - not in bulk because of the constraint violation check
    FOR i IN 1 .. dcIds.COUNT LOOP
      DECLARE
        CONSTRAINT_VIOLATED EXCEPTION;
        PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
      BEGIN
        DELETE FROM DiskCopy WHERE id = dcIds(i);
      EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
        IF sqlerrm LIKE '%constraint (CASTOR_STAGER.FK_DRAININGERRORS_CASTORFILE) violated%' OR
           sqlerrm LIKE '%constraint (CASTOR_STAGER.FK_DISK2DISKCOPYJOB_SRCDCID) violated%' THEN
          -- Ignore the deletion, this diskcopy was either implied in a draining action and
          -- the draining error is still around or it is the source of another d2d copy that
          -- is not over
          NULL;
        ELSE
          -- Any other constraint violation is an error
          RAISE;
        END IF;
      END;
    END LOOP;
    COMMIT;
    -- maybe delete the CastorFiles if nothing is left for them
    FOR i IN 1 .. cfIds.COUNT LOOP
      deleteCastorFile(cfIds(i));
    END LOOP;
    COMMIT;
    -- exit if we did less than 1000
    IF dcIds.COUNT < 1000 THEN EXIT; END IF;
  END LOOP;
END;
/

/* Deal with old diskCopies in STAGEOUT */
CREATE OR REPLACE PROCEDURE deleteOutOfDateStageOutDCs(timeOut IN NUMBER) AS
  srId NUMBER;
BEGIN
  -- Deal with old DiskCopies in STAGEOUT/WAITFS. The rule is to drop
  -- the ones with 0 fileSize and issue a putDone for the others
  FOR f IN (SELECT /*+ USE_NL(D C S) LEADING(D C S) INDEX(D I_DiskCopy_Status_Open) INDEX(S I_SubRequest_CastorFile) */
                   C.filesize, C.id, C.fileId, C.nsHost, D.fileSystem, D.id AS dcId, D.status AS dcStatus
              FROM DiskCopy D, Castorfile C
             WHERE C.id = D.castorFile
               AND D.creationTime < getTime() - timeOut
               AND decode(D.status,6,D.status,decode(D.status,5,D.status,decode(D.status,11,D.status,NULL))) IS NOT NULL
               AND NOT EXISTS (
                 SELECT 'x'
                   FROM SubRequest
                  WHERE castorFile = C.id
                    AND status IN (0, 1, 2, 3, 5, 6, 13) -- all active
                    AND reqType != 37)) LOOP -- ignore PrepareToPut
    IF (0 = f.fileSize) OR (f.dcStatus <> 6) THEN  -- DISKCOPY_STAGEOUT
      -- here we invalidate the diskcopy and let the GC run
      UPDATE DiskCopy SET status = 7  -- INVALID
       WHERE id = f.dcid;
      -- and we also fail the correspondent prepareToPut request if it exists
      BEGIN
        SELECT /*+ INDEX_RS_ASC(Subrequest I_Subrequest_Diskcopy)*/ id
          INTO srId   -- there can only be one outstanding PrepareToPut, if any
          FROM SubRequest
         WHERE status = 6 AND diskCopy = f.dcid;
        archiveSubReq(srId, 9);  -- FAILED_FINISHED
      EXCEPTION WHEN NO_DATA_FOUND THEN
        NULL;
      END;
      logToDLF(NULL, dlf.LVL_WARNING, dlf.FILE_DROPPED_BY_CLEANING, f.fileId, f.nsHost, 'stagerd', '');
    ELSE
      -- here we issue a putDone
      -- context 2 : real putDone. Missing PPut requests are ignored.
      -- svcClass 0 since we don't know it. This will trigger a
      -- default behavior in the putDoneFunc
      putDoneFunc(f.id, f.fileSize, 2, 0);
      logToDLF(NULL, dlf.LVL_WARNING, dlf.PUTDONE_ENFORCED_BY_CLEANING, f.fileId, f.nsHost, 'stagerd', '');
    END IF;
  END LOOP;
  COMMIT;
END;
/

/* Runs cleanup operations */
CREATE OR REPLACE PROCEDURE cleanup AS
  t INTEGER;
BEGIN
  -- First perform some cleanup of old stuff:
  -- for each, read relevant timeout from configuration table
  t := TO_NUMBER(getConfigOption('cleaning', 'outOfDateStageOutDCsTimeout', '72'));
  deleteOutOfDateStageOutDCs(t*3600);
  t := TO_NUMBER(getConfigOption('cleaning', 'failedDCsTimeout', '72'));
  deleteFailedDiskCopies(t*3600);
END;
/

/*
 * Database jobs
 */
BEGIN
  -- Remove database jobs before recreating them
  FOR j IN (SELECT job_name FROM user_scheduler_jobs
             WHERE job_name IN ('HOUSEKEEPINGJOB',
                                'CLEANUPJOB',
                                'BULKCHECKFSBACKINPRODJOB'))
  LOOP
    DBMS_SCHEDULER.DROP_JOB(j.job_name, TRUE);
  END LOOP;

  -- Create a db job to be run every 20 minutes executing the deleteTerminatedRequests procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'houseKeepingJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN startDbJob(''BEGIN deleteTerminatedRequests(); END;'', ''stagerd''); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=20',
      ENABLED         => TRUE,
      COMMENTS        => 'Cleaning of terminated requests');

  -- Create a db job to be run twice a day executing the cleanup procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'cleanupJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN startDbJob(''BEGIN cleanup(); END;'', ''stagerd''); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=HOURLY; INTERVAL=12',
      ENABLED         => TRUE,
      COMMENTS        => 'Database maintenance');

  -- Create a db job to be run every 5 minutes executing the bulkCheckFSBackInProd procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'bulkCheckFSBackInProdJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN startDbJob(''BEGIN bulkCheckFSBackInProd(); END;'', ''stagerd''); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=5',
      ENABLED         => TRUE,
      COMMENTS        => 'Bulk operation to processing filesystem state changes');
END;
/

