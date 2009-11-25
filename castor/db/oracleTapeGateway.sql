/*******************************************************************
 *
 * @(#)$RCSfile: oracleTapeGateway.sql,v $ $Revision: 1.12 $ $Date: 2009/08/13 15:14:25 $ $Author: gtaur $
 *
 * PL/SQL code for the tape gateway daemon
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* trigger to be on just if the tapegateway is running */

/* trigger recall */

CREATE OR REPLACE TRIGGER tr_Tape_Pending
AFTER UPDATE of status ON Tape
FOR EACH ROW
WHEN (new.status = 1 and new.tpmode=0)
DECLARE
 
  reqId NUMBER;
  unused NUMBER;

BEGIN
  BEGIN

    SELECT id INTO unused 
        FROM TapeGatewayRequest 
      	WHERE taperecall=:new.id;
 
  EXCEPTION WHEN NO_DATA_FOUND THEN

    INSERT INTO TapeGatewayRequest
	(accessmode, starttime, lastvdqmpingtime, vdqmvolreqid, id, streammigration, taperecall, status) 
	VALUES (0,null,null,null,ids_seq.nextval,null,:new.id,1)  RETURNING id into reqId;
    INSERT INTO id2type (id,type) VALUES (reqId,172); 

  END; 
END;
/

/* trigger migration */

CREATE OR REPLACE TRIGGER tr_Stream_Pending
AFTER UPDATE of status ON Stream
FOR EACH ROW
WHEN (new.status = 0 )
DECLARE 

  reqId NUMBER;
  unused NUMBER;

BEGIN 

  BEGIN

    SELECT id INTO unused 
	FROM TapeGatewayRequest 
	WHERE streammigration=:new.id;
 
  EXCEPTION WHEN NO_DATA_FOUND THEN

    INSERT INTO TapeGatewayRequest 
	(accessMode, startTime, lastVdqmPingTime, vdqmVolReqId, id, streamMigration, TapeRecall, Status) 
	VALUES (1,null,null,null,ids_seq.nextval,:new.id,null,0) RETURNING id INTO reqId;
    INSERT INTO id2type (id,type) VALUES (reqId,172);

  END;

END;
/

/* PROCEDURE */

/* attach drive request to tape */


create or replace
PROCEDURE tg_attachDriveReqToTape( tapeRequestId IN NUMBER,
                                                       inputVdqmId IN NUMBER, 
                                                       inputDgn IN VARCHAR2, 
                                                       inputLabel IN VARCHAR2,
                                                       inputDensity IN VARCHAR2) AS
BEGIN

-- update tapegatewayrequest which have been submitted to vdqm =>  WAITING_TAPESERVER
    UPDATE TapeGatewayRequest 
      SET status=2, lastvdqmpingtime=gettime(), starttime= gettime(), vdqmvolreqid=inputVdqmId
      WHERE id=tapeRequestId; -- these are the ones waiting for vdqm to be sent again

-- update stream for migration    
    UPDATE Stream 
      SET status=1 
      WHERE id = 
        (select streammigration FROM TapeGatewayRequest WHERE accessmode=1 AND  id=tapeRequestId);  

-- update tape for migration and recall    

    UPDATE Tape 
      SET status=2, dgn=inputDgn, label=inputLabel, density= inputDensity 
      WHERE id = 
        (SELECT tape 
          FROM Stream, TapeGatewayRequest 
          WHERE stream.id = tapegatewayrequest.streammigration 
          AND tapegatewayrequest.accessmode=1 
          AND tapegatewayrequest.id=tapeRequestId) 
      OR id =
        (SELECT taperecall 
          FROM TapeGatewayRequest  
          WHERE accessmode=0 
          AND  id=tapeRequestId);  
  COMMIT;
END;
/
        
/* attach the tapes to the streams  */

CREATE OR REPLACE PROCEDURE tg_attachTapesToStreams (startFseqs IN castor."cnumList",
                                                     strIds IN castor."cnumList", 
                                                     tapeVids IN castor."strList") AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  tapeId NUMBER;

BEGIN
 FOR i IN strIds.FIRST .. strIds.LAST LOOP
      tapeId:=NULL;
      -- update tapegatewayrequest
      UPDATE TapeGatewayRequest 
        SET status=1, lastfseq= startfseqs(i) 
        WHERE streammigration= strids(i); -- TO_BE_SENT_TO_VDQM
      
      UPDATE Tape SET stream=strids(i), status=2 
        WHERE tpmode=1 
        AND vid=tapeVids(i) 
        RETURNING id INTO tapeId;
 
      IF tapeId IS NULL THEN 
        BEGIN

        -- insert a tape if it is not already in the database
          INSERT INTO Tape 
               (vid,side,tpmode,errmsgtxt,errorcode,severity,vwaddress,id,stream,status) 
               VALUES (tapeVids(i),0,1,null,0,0,null,ids_seq.nextval,0,2) 
               RETURNING id INTO tapeId;
          INSERT INTO id2type (id,type) values (tapeId,29);
      
        EXCEPTION WHEN CONSTRAINT_VIOLATED THEN

       -- retry the select since a creation was done in between
          UPDATE Tape SET stream=strids(i), status=2
            WHERE tpmode=1 
            AND vid=tapeVids(i) 
            RETURNING id INTO tapeId;
        
        END;

      END IF;
  
    -- update the stream
      UPDATE Stream SET tape=tapeId 
        WHERE Stream.id=strIds(i); 

 END LOOP;
 COMMIT;
END;
/

/* update the db when a tape session is ended */

create or replace
PROCEDURE tg_endTapeSession ( transactionId IN NUMBER,
                                                inputErrorCode IN INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -02292);

  tpId NUMBER;
  trId NUMBER;
  strId NUMBER;
  reqMode INTEGER;
  segNum INTEGER;
  cfId NUMBER;
  tcIds "numList";
  srIds "numList";

BEGIN
  BEGIN
  -- take lock on taperequest
   SELECT id,taperecall, streammigration,accessmode
        INTO trId,tpId, strid, reqMode 
        FROM TapeGatewayRequest 
        WHERE vdqmvolreqid = transactionId FOR UPDATE;

   -- delete tapesubrequest
    DELETE FROM TapeGatewaySubRequest
        WHERE request=trId
        RETURNING tapecopy,id bulk collect
        INTO tcIds, srIds;

    FORALL i in srIds.FIRST .. srIds.LAST
     DELETE FROM id2type WHERE id=srIds(i);
     
  -- delete taperequest after the subrequest to avouid constraint violation  
     
    DELETE FROM TapeGatewayRequest
        WHERE id=trId;
    DELETE FROM id2type WHERE id=trId;

    IF reqMode = 0 THEN
     -- read
      SELECT id INTO tpId
        FROM Tape
        WHERE id =tpId
        FOR UPDATE;

       IF inputErrorCode != 0 THEN

          -- if a failure is reported
          -- fail all the segments
          FORALL i in tcIds.FIRST .. tcIds.LAST
            UPDATE Segment SET status=6
              WHERE copy = tcIds(i);

          -- mark tapecopies as  REC_RETRY
          FORALL i in tcIds.FIRST .. tcIds.LAST
            UPDATE TapeCopy  SET status=8, errorcode=inputErrorCode
              WHERE id=tcIds(i);

       END IF;

       -- resurrect lost tapesubrequest
       UPDATE Segment SET status=0
         WHERE status = 7
         AND tape=tpId;

       -- check if there is work for this tape
       SELECT count(*) INTO segNum
         FROM segment
         WHERE tape=tpId
         AND status=0;
       -- still segments in zero ... to be restarted

       IF segNum > 0 THEN
         UPDATE Tape SET status = 8
           WHERE id=tpId; --WAIT POLICY for rechandler
       ELSE
         UPDATE Tape SET status = 0
           WHERE id=tpId; --UNUSED
       END IF;

    ELSE

      -- write
      -- reset stream
       BEGIN
         DELETE FROM Stream WHERE id = strId;
         DELETE FROM id2type WHERE id=strId;

       EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
       -- constraint violation and we cannot delete the stream because there is an entry in Stream2TapeCopy
         UPDATE Stream SET status = 6, tape = null, lastFileSystemChange = null
           WHERE id = strId;
       END;
       -- reset tape
       UPDATE Tape SET status=0, stream = null WHERE stream = strId;

       IF inputErrorCode != 0 THEN
          -- if a failure is reported
          --retry MIG_RETRY
          FORALL i in tcIds.FIRST .. tcIds.LAST
            UPDATE TapeCopy  SET status=9, errorcode=inputErrorCode, nbretry=0
              WHERE id=tcIds(i);
       ELSE
         -- just resurrect them if they were lost
          FORALL i in tcIds.FIRST .. tcIds.LAST
           UPDATE TapeCopy  SET status=1
              WHERE id=tcIds(i) AND status=3;
       END IF;


    END IF;

    EXCEPTION WHEN  NO_DATA_FOUND THEN
      null;
    END;

    COMMIT;
END;
/


/* mark a migration or recall as failed saving in the db the error code associated with the failure */

CREATE OR REPLACE PROCEDURE tg_failFileTransfer(transactionId IN NUMBER,
                                                inputFileId IN NUMBER,
                                                inputNsHost IN VARCHAR2,
                                                inputFseq IN INTEGER, 
                                                inputErrorCode IN INTEGER)  AS
 cfId NUMBER;
 trId NUMBER;
 strId NUMBER;
 tpId NUMBER;
 tcId NUMBER;
 srId NUMBER;
 outMode INTEGER;

BEGIN
 
  SELECT accessmode, id, taperecall, streammigration INTO outMode, trId, tpId, strId 
    FROM  TapeGatewayRequest 
    WHERE transactionid = vdqmvolreqid 
    FOR UPDATE;
   
  SELECT id INTO cfId 
    FROM CastorFile 
    WHERE fileid=inputFileId
    AND nshost=inputNsHost 
    FOR UPDATE;

 -- delete tape subrequest
  DELETE FROM TapegatewaySubRequest 
    WHERE fseq=inputFseq 
    AND request=trId 
    RETURNING tapecopy,id  INTO tcId, srId;

  DELETE FROM id2type WHERE id=srId;
  
  IF outmode = 0 THEN
     -- read
     -- retry REC_RETRY 
     -- failed the segment on that tape
     UPDATE Segment SET status=6, severity=inputErrorCode, errorCode=-1 
       WHERE fseq= inputfseq 
       AND tape=tpId 
       RETURNING copy INTO tcId;
 
     -- mark tapecopy as REC_RETRY
     UPDATE TapeCopy  SET status=8, errorcode=inputErrorCode 
       WHERE id=tcId;   
  
  ELSE 
     -- write
     SELECT Tape.id INTO tpId 
       FROM Tape, Stream 
       WHERE Tape.id = Stream.tape
       AND Stream.id = strId;
    
     --retry MIG_RETRY
     -- mark tapecopy as MIG_RETRY
     UPDATE TapeCopy  SET status=9, errorcode=inputErrorCode 
       WHERE id=tcId; 
  
  END IF;  
EXCEPTION WHEN  NO_DATA_FOUND THEN
    null;
END;
/

/* retrieve from the db all the tapecopies that faced a failure for migration */

CREATE OR REPLACE PROCEDURE  tg_getFailedMigrations(tcs OUT castor.TapeCopy_Cur) AS
  ids "numList";
BEGIN
  -- get TAPECOPY_MIG_RETRY
  OPEN tcs FOR
    SELECT copynb, id, castorFile, status, errorCode, nbretry, missingCopies 
      FROM TapeCopy
      WHERE status=9 
      AND ROWNUM < 1000 
      FOR UPDATE SKIP LOCKED; 
END;
/


/* retrieve from the db all the tapecopies that faced a failure for recall */

CREATE OR REPLACE PROCEDURE  tg_getFailedRecalls(tcs OUT castor.TapeCopy_Cur) AS
  ids "numList";
BEGIN
  -- get TAPECOPY_REC_RETRY
  OPEN tcs FOR
    SELECT copynb, id, castorFile, status, errorCode, nbretry, missingCopies
     FROM TapeCopy
     WHERE status=8 
     AND ROWNUM < 1000 
     FOR UPDATE SKIP LOCKED; 
END;
/

/* default migration candidate selection policy */

create or replace
PROCEDURE tg_defaultMigrSelPolicy(streamId IN INTEGER,
                                  diskServerName OUT NOCOPY VARCHAR2,
                                  mountPoint OUT NOCOPY VARCHAR2,
                                  path OUT NOCOPY VARCHAR2,
                                  dci OUT INTEGER,
                                  castorFileId OUT INTEGER,
                                  fileId OUT INTEGER,
                                  nsHost OUT NOCOPY VARCHAR2, 
                                  fileSize OUT INTEGER,
                                  tapeCopyId OUT INTEGER, 
                                  lastUpdateTime OUT INTEGER) AS
  fileSystemId INTEGER := 0;
  diskServerId NUMBER;
  lastFSChange NUMBER;
  lastFSUsed NUMBER;
  lastButOneFSUsed NUMBER;
  findNewFS NUMBER := 1;
  nbMigrators NUMBER := 0;
  unused NUMBER;
  LockError EXCEPTION;
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  tapeCopyId := 0;
  -- First try to see whether we should resuse the same filesystem as last time
  SELECT lastFileSystemChange, lastFileSystemUsed, lastButOneFileSystemUsed
    INTO lastFSChange, lastFSUsed, lastButOneFSUsed
    FROM Stream WHERE id = streamId;
  IF getTime() < lastFSChange + 900 THEN
    SELECT (SELECT count(*) FROM stream WHERE lastFileSystemUsed = lastButOneFSUsed) +
           (SELECT count(*) FROM stream WHERE lastButOneFileSystemUsed = lastButOneFSUsed)
      INTO nbMigrators FROM DUAL;
    -- only go if we are the only migrator on the box
    IF nbMigrators = 1 THEN
      BEGIN
        -- check states of the diskserver and filesystem and get mountpoint and diskserver name
        SELECT name, mountPoint, FileSystem.id INTO diskServerName, mountPoint, fileSystemId
          FROM FileSystem, DiskServer
         WHERE FileSystem.diskServer = DiskServer.id
           AND FileSystem.id = lastButOneFSUsed
           AND FileSystem.status IN (0, 1)  -- PRODUCTION, DRAINING
           AND DiskServer.status IN (0, 1); -- PRODUCTION, DRAINING
        -- we are within the time range, so we try to reuse the filesystem
        SELECT /*+ FIRST_ROWS(1)  LEADING(D T ST) */
               D.path, D.id, D.castorfile, T.id
          INTO path, dci, castorFileId, tapeCopyId
          FROM DiskCopy D, TapeCopy T, Stream2TapeCopy ST
         WHERE decode(D.status, 10, D.status, NULL) = 10 -- CANBEMIGR
           AND D.filesystem = lastButOneFSUsed
           AND ST.parent = streamId
           AND T.status = 2 -- WAITINSTREAMS
           AND ST.child = T.id
           AND T.castorfile = D.castorfile
           AND ROWNUM < 2 FOR UPDATE OF t.id NOWAIT;
        SELECT CastorFile.FileId, CastorFile.NsHost, CastorFile.FileSize, CastorFile.lastUpdateTime
          INTO fileId, nsHost, fileSize, lastUpdateTime
          FROM CastorFile
         WHERE Id = castorFileId;
        -- we found one, no need to go for new filesystem
        findNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- found no tapecopy or diskserver, filesystem are down. We'll go through the normal selection
        NULL;
      WHEN LockError THEN
      -- We have observed ORA-00054 errors (resource busy and acquire with NOWAIT) even with
      -- the SKIP LOCKED clause. This is a workaround to ignore the error until we understand
      -- what to do, another thread will pick up the request so we don't do anything.
        NULL;
      END;
    END IF;
  END IF;
  IF findNewFS = 1 THEN
    FOR f IN (
    SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
       FROM Stream, SvcClass2TapePool, DiskPool2SvcClass, FileSystem, DiskServer
      WHERE Stream.id = streamId
        AND Stream.TapePool = SvcClass2TapePool.child
        AND SvcClass2TapePool.parent = DiskPool2SvcClass.child
        AND DiskPool2SvcClass.parent = FileSystem.diskPool
        AND FileSystem.diskServer = DiskServer.id
        AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
        AND DiskServer.status IN (0, 1) -- PRODUCTION, DRAINING
      ORDER BY -- first prefer diskservers where no migrator runs and filesystems with no recalls
               DiskServer.nbMigratorStreams ASC, FileSystem.nbRecallerStreams ASC,
               -- then order by rate as defined by the function
               fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams, FileSystem.nbWriteStreams,
                              FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
               -- finally use randomness to avoid preferring always the same FS
               DBMS_Random.value) LOOP
       BEGIN
         -- lock the complete diskServer as we will update all filesystems
         SELECT id INTO unused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
         SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
                f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
           INTO diskServerId, diskServerName, mountPoint, fileSystemId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = 10 -- CANBEMIGR
            AND D.filesystem = f.fileSystemId
            AND StT.parent = streamId
            AND T.status = 2 -- WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND ROWNUM < 2 FOR UPDATE OF t.id NOWAIT;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;

  IF tapeCopyId = 0 THEN
    -- Nothing found, return; locks will be released by the caller
    RETURN;
  END IF;
  
  IF findNewFS = 1 THEN
    UPDATE Stream
       SET lastFileSystemUsed = fileSystemId,
           lastButOneFileSystemUsed = lastFileSystemUsed,
           lastFileSystemChange = getTime()
     WHERE id = streamId;
  END IF;

  -- Update Filesystem state
  updateFSMigratorOpened(diskServerId, fileSystemId, 0);
END;
/

/* drain disk migration candidate selection policy */

create or replace PROCEDURE tg_drainDiskMigrSelPolicy(streamId IN INTEGER,
                                    diskServerName OUT NOCOPY VARCHAR2, 
                                    mountPoint OUT NOCOPY VARCHAR2,
                                    path OUT NOCOPY VARCHAR2,
                                    dci OUT INTEGER,
                                    castorFileId OUT INTEGER,
                                    fileId OUT INTEGER,
                                    nsHost OUT NOCOPY VARCHAR2,
                                    fileSize OUT INTEGER,
                                    tapeCopyId OUT INTEGER,
                                    lastUpdateTime OUT INTEGER) AS
  fileSystemId INTEGER := 0;
  diskServerId NUMBER;
  fsDiskServer NUMBER;
  lastFSChange NUMBER;
  lastFSUsed NUMBER;
  lastButOneFSUsed NUMBER;
  findNewFS NUMBER := 1;
  nbMigrators NUMBER := 0;
  unused NUMBER;
  LockError EXCEPTION;
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  tapeCopyId := 0;
  -- First try to see whether we should resuse the same filesystem as last time
  SELECT lastFileSystemChange, lastFileSystemUsed, lastButOneFileSystemUsed
    INTO lastFSChange, lastFSUsed, lastButOneFSUsed
    FROM Stream WHERE id = streamId;
  IF getTime() < lastFSChange + 1800 THEN
    SELECT (SELECT count(*) FROM stream WHERE lastFileSystemUsed = lastFSUsed)
      INTO nbMigrators FROM DUAL;
    -- only go if we are the only migrator on the box
    IF nbMigrators = 1 THEN
      BEGIN
        -- check states of the diskserver and filesystem and get mountpoint and diskserver name
        SELECT diskserver.id, name, mountPoint, FileSystem.id INTO diskServerId, diskServerName, mountPoint, fileSystemId
          FROM FileSystem, DiskServer
         WHERE FileSystem.diskServer = DiskServer.id
           AND FileSystem.id = lastFSUsed
           AND FileSystem.status IN (0, 1)  -- PRODUCTION, DRAINING
           AND DiskServer.status IN (0, 1); -- PRODUCTION, DRAINING
        -- we are within the time range, so we try to reuse the filesystem
        
        SELECT /*+ ORDERED USE_NL(D T) INDEX(T I_TapeCopy_CF_Status_2) INDEX(ST I_Stream2TapeCopy_PC) */
              D.path, D.diskcopy_id, D.castorfile, T.id INTO path, dci, castorFileId, tapeCopyId
          FROM (SELECT /*+ INDEX(DK I_DiskCopy_FS_Status_10) */
                             DK.path path, DK.id diskcopy_id, DK.castorfile
                  FROM DiskCopy DK
                  WHERE decode(DK.status, 10, DK.status, NULL) = 10 -- CANBEMIGR
                  AND DK.filesystem = lastFSUsed)  D, TapeCopy T, Stream2TapeCopy ST
          WHERE T.castorfile = D.castorfile
          AND ST.child = T.id
          AND ST.parent = streamId
          AND decode(T.status, 2, T.status, NULL) = 2 -- WAITINSTREAMS
          AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;   
        
        SELECT  C.fileId, C.nsHost, C.fileSize,  C.lastUpdateTime
          INTO  fileId, nsHost, fileSize, lastUpdateTime
          FROM  castorfile C
          WHERE castorfileId = C.id;
         
        -- we found one, no need to go for new filesystem
        findNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
        -- found no tapecopy or diskserver, filesystem are down. We'll go through the normal selection
        NULL;
      END;
    END IF;
  END IF;
  IF findNewFS = 1 THEN
    -- We try first to reuse the diskserver of the lastFSUsed, even if we change filesystem
    FOR f IN (
      SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
        FROM FileSystem, DiskServer
       WHERE FileSystem.diskServer = DiskServer.id
         AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
         AND DiskServer.status IN (0, 1) -- PRODUCTION, DRAINING
         AND DiskServer.id = lastButOneFSUsed) LOOP
       BEGIN
         -- lock the complete diskServer as we will update all filesystems
         SELECT id INTO unused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
         SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
                f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
           INTO diskServerId, diskServerName, mountPoint, fileSystemId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = 10 -- CANBEMIGR
            AND D.filesystem = f.fileSystemId
            AND StT.parent = streamId
            AND T.status = 2 -- WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;
  IF tapeCopyId = 0 THEN
    -- Then we go for all potential filesystems. Note the duplication of code, due to the fact that ORACLE cannot order unions
    FOR f IN (
      SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
        FROM Stream, SvcClass2TapePool, DiskPool2SvcClass, FileSystem, DiskServer
       WHERE Stream.id = streamId
         AND Stream.TapePool = SvcClass2TapePool.child
         AND SvcClass2TapePool.parent = DiskPool2SvcClass.child
         AND DiskPool2SvcClass.parent = FileSystem.diskPool
         AND FileSystem.diskServer = DiskServer.id
         AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
         AND DiskServer.status IN (0, 1) -- PRODUCTION, DRAINING
       ORDER BY -- first prefer diskservers where no migrator runs and filesystems with no recalls
                DiskServer.nbMigratorStreams ASC, FileSystem.nbRecallerStreams ASC,
                -- then order by rate as defined by the function
                fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams, FileSystem.nbWriteStreams,
                               FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
                -- finally use randomness to avoid preferring always the same FS
                DBMS_Random.value) LOOP
       BEGIN
         -- lock the complete diskServer as we will update all filesystems
         SELECT id INTO unused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
         SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
                f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
           INTO diskServerId, diskServerName, mountPoint, fileSystemId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = 10 -- CANBEMIGR
            AND D.filesystem = f.fileSystemId
            AND StT.parent = streamId
            AND T.status = 2 -- WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;

  IF tapeCopyId = 0 THEN
    -- Nothing found, return; locks will be released by the caller
    RETURN;
  END IF;
  
  IF findNewFS = 1 THEN
    UPDATE Stream
       SET lastFileSystemUsed = fileSystemId,
           lastButOneFileSystemUsed = lastFileSystemUsed,
           lastFileSystemChange = getTime()
     WHERE id = streamId;
  END IF;

  -- Update Filesystem state
  updateFSMigratorOpened(diskServerId, fileSystemId, 0);
END;
/


/* repack migration candidate selection policy */

create or replace
PROCEDURE tg_repackMigrSelPolicy(streamId IN INTEGER,
                                 diskServerName OUT NOCOPY VARCHAR2,
                                 mountPoint OUT NOCOPY VARCHAR2,
                                 path OUT NOCOPY VARCHAR2, dci OUT INTEGER,
                                 castorFileId OUT INTEGER, fileId OUT INTEGER,
                                 nsHost OUT NOCOPY VARCHAR2,
                                 fileSize OUT INTEGER,
                                 tapeCopyId OUT INTEGER,
                                 lastUpdateTime OUT INTEGER) AS
  fileSystemId INTEGER := 0;
  diskServerId NUMBER;
  lastFSChange NUMBER;
  lastFSUsed NUMBER;
  lastButOneFSUsed NUMBER;
  nbMigrators NUMBER := 0;
  unused NUMBER;
  LockError EXCEPTION;
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  tapeCopyId := 0;
  FOR f IN (
    SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
       FROM Stream, SvcClass2TapePool, DiskPool2SvcClass, FileSystem, DiskServer
      WHERE Stream.id = streamId
        AND Stream.TapePool = SvcClass2TapePool.child
        AND SvcClass2TapePool.parent = DiskPool2SvcClass.child
        AND DiskPool2SvcClass.parent = FileSystem.diskPool
        AND FileSystem.diskServer = DiskServer.id
        AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
        AND DiskServer.status IN (0, 1) -- PRODUCTION, DRAINING
      ORDER BY -- first prefer diskservers where no migrator runs and filesystems with no recalls
               DiskServer.nbMigratorStreams ASC, FileSystem.nbRecallerStreams ASC,
               -- then order by rate as defined by the function
               fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams, FileSystem.nbWriteStreams,
                              FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
               -- finally use randomness to avoid preferring always the same FS
               DBMS_Random.value) LOOP
    BEGIN
      -- lock the complete diskServer as we will update all filesystems
      SELECT id INTO unused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
      SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
             f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
        INTO diskServerId, diskServerName, mountPoint, fileSystemId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
        FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
       WHERE decode(D.status, 10, D.status, NULL) = 10 -- CANBEMIGR
         AND D.filesystem = f.fileSystemId
         AND StT.parent = streamId
         AND T.status = 2 -- WAITINSTREAMS
         AND StT.child = T.id
         AND T.castorfile = D.castorfile
         AND C.id = D.castorfile
         AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
      -- found something on this filesystem, no need to go on
      diskServerId := f.DiskServerId;
      fileSystemId := f.fileSystemId;
      EXIT;
    EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
      -- either the filesystem is already locked or we found nothing,
      -- let's go to the next one
      NULL;
    END;
  END LOOP;

  IF tapeCopyId = 0 THEN
    -- Nothing found, return; locks will be released by the caller
    RETURN;
  END IF;
  
  UPDATE Stream
     SET lastFileSystemUsed = fileSystemId,
         lastButOneFileSystemUsed = lastFileSystemUsed,
         lastFileSystemChange = getTime()
   WHERE id = streamId;

  -- Update Filesystem state
  updateFSMigratorOpened(diskServerId, fileSystemId, 0);
END;
/

/* get a candidate for migration */

create or replace
PROCEDURE tg_getFileToMigrate(transactionId IN NUMBER,
                                                ret OUT INTEGER,
                                                outVid OUT NOCOPY VARCHAR2,
                                                outputFile OUT castorTape.FileToMigrateCore_cur) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  strId NUMBER;
  policy VARCHAR2(100);
  ds VARCHAR2(2048);
  mp VARCHAR2(2048);
  path  VARCHAR2(2048);
  dcid NUMBER:=0;
  cfId NUMBER;
  fileId NUMBER;
  nsHost VARCHAR2(2048);
  fileSize  INTEGER;
  tcId  INTEGER:=0;
  lastUpdateTime NUMBER;
  knownName VARCHAR2(2048);
  newFseq NUMBER;
  trId NUMBER;
  srId NUMBER;
  unused INTEGER;
BEGIN
  ret:=0;
  BEGIN
    -- last fseq is the first fseq value available for this migration
    SELECT streammigration, lastfseq, id INTO strId, newFseq, trId
      FROM TapeGatewayRequest
     WHERE vdqmvolreqid = transactionId
     FOR UPDATE;

    SELECT vid INTO outVid
      FROM Tape
     WHERE id IN
         (SELECT tape
            FROM Stream
           WHERE id = strId);
  EXCEPTION WHEN NO_DATA_FOUND THEN
    ret:=-2;   -- stream is over
    RETURN;
  END;

  BEGIN  
    SELECT 1 INTO unused FROM dual 
      WHERE EXISTS (SELECT 'x' FROM Stream2TapeCopy 
                      WHERE parent=strId);
  EXCEPTION WHEN NO_DATA_FOUND THEN
    ret:=-1;   -- no more files
    RETURN;
  END;
 -- lock to avoid deadlock with mighunter 
 SELECT id INTO strId FROM stream WHERE id=strId FOR UPDATE;
  -- get the policy name and execute the policy
  BEGIN
    SELECT migrSelectPolicy INTO policy
      FROM Stream, TapePool
     WHERE Stream.id = strId
       AND Stream.tapePool = TapePool.id;
    -- check for NULL value
    IF policy IS NULL THEN
      policy := 'defaultMigrSelPolicy';
    END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    policy := 'defaultMigrSelPolicy';
  END;

  IF policy like 'defaultMigrSelPolicy' THEN
    -- default policy
    tg_defaultMigrSelPolicy(strId,ds,mp,path,dcid ,cfId, fileId,nsHost, fileSize,tcId, lastUpdateTime);
  ELSIF  policy LIKE 'repackMigrSelPolicy' THEN
    -- repack policy
    tg_repackMigrSelPolicy(strId,ds,mp,path,dcid ,cfId, fileId,nsHost, fileSize,tcId, lastUpdateTime);
  ELSIF  policy LIKE 'drainDiskMigrSelPolicy' THEN
    -- drain disk policy
    tg_drainDiskMigrSelPolicy(strId,ds,mp,path,dcid ,cfId, fileId,nsHost, fileSize,tcId, lastUpdateTime);
  ELSE
    -- default if we don't know at this point
    tg_defaultMigrSelPolicy(strId,ds,mp,path,dcid ,cfId, fileId,nsHost, fileSize,tcId, lastUpdateTime);
  END IF;

  IF tcId = 0 OR dcid=0 THEN
    ret := -1; -- the migration selection policy didn't find any candidate
    COMMIT;
    RETURN;
  END IF;

  -- Here we found a tapeCopy and we process it
  -- update status of selected tapecopy and stream
  UPDATE TapeCopy SET status = 3 -- SELECTED
   WHERE id = tcId;
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy
   WHERE child = tcId;


  SELECT lastknownfilename INTO knownName
    FROM CastorFile
   WHERE id = cfId; -- we rely on the check done before
   
  DECLARE 
  
  BEGIN
    INSERT INTO TapeGatewaySubRequest
      (fseq, id, tapecopy, request,diskcopy)
      VALUES (newFseq, ids_seq.nextval, tcId,trId, dcid)
      RETURNING id INTO srId;
      
    INSERT INTO id2type
      (id,type)
      VALUES (srId,180);
  

   UPDATE TapeGatewayRequest
     SET lastfseq=lastfseq+1
     WHERE id=trId; --increased when we have the proper value

   OPEN outputFile FOR
     SELECT fileId,nshost,lastUpdateTime,ds,mp,path,knownName,fseq,filesize,id
       FROM tapegatewaysubrequest
      WHERE id = srId;
      
  EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
   -- there is going to be another copy of the file on this tape.
   -- UNIQUE constraint violation on the diskcopy column in tapegatewaysubrequest
   -- detach from this stream
     DELETE FROM stream2tapecopy WHERE child= tcid AND parent= strid;
   -- resurrect the candidate if there is no other stream containing the tapecopy
     UPDATE tapecopy SET status=1 WHERE id = tcId AND NOT EXISTS (SELECT 'x' FROM stream2tapecopy WHERE child=tcId);
     COMMIT;
   -- get another candidate
      tg_getFileToMigrate(transactionId, ret, outVid, outputFile);
 END;
END;
/


/* get a candidate for recall */


create or replace
PROCEDURE tg_getFileToRecall (transactionId IN NUMBER, 
                                                ret OUT INTEGER, 
                                                vidOut OUT NOCOPY VARCHAR2,
                                                outputFile OUT castorTape.FileToRecallCore_Cur) AS
 trId INTEGER;
 ds VARCHAR2(2048);
 mp VARCHAR2(2048);
 path VARCHAR2(2048); 
 segId NUMBER;
 dcId NUMBER;
 tcId NUMBER;
 tapeId NUMBER;
 srId NUMBER;
 newFseq INTEGER;
 cfId NUMBER;

BEGIN 
  ret:=0;
  BEGIN
  -- master lock on the taperequeststate
    SELECT id, taperecall INTO trId, tapeid 
      FROM TapeGatewayRequest 
      WHERE vdqmVolReqId=transactionid 
      FOR UPDATE;
  
    SELECT vid INTO vidOut 
      FROM Tape 
      WHERE id=tapeId;

  EXCEPTION WHEN  NO_DATA_FOUND THEN
     ret:=-2; -- ERROR
     RETURN;
  END; 
  
  BEGIN
   
    SELECT id INTO segId FROM 
      ( SELECT id FROM Segment 
         WHERE  tape = tapeId  
         AND status=0 
         ORDER BY fseq ASC) 
      WHERE ROWNUM<2;

    SELECT fseq, copy INTO  newFseq,tcId 
      FROM Segment 
      WHERE id = segId;

    SELECT Castorfile.id INTO cfId 
      FROM Castorfile,TapeCopy 
      WHERE  Castorfile.id=TapeCopy.castorfile 
      AND TapeCopy.id=tcId 
      FOR UPDATE OF CastorFile.id;

  EXCEPTION WHEN NO_DATA_FOUND THEN
     ret:=-1; -- NO MORE FILES
     COMMIT;
     RETURN;
  END;

  DECLARE
    application_error EXCEPTION;
    PRAGMA EXCEPTION_INIT(application_error,-20115);
  BEGIN
    bestFileSystemForSegment(segId,ds,mp,path,dcId);
  EXCEPTION WHEN  application_error  OR NO_DATA_FOUND THEN 
    ret:=-3;
    COMMIT;
    RETURN;
  END;
 
  INSERT INTO TapeGatewaySubRequest
    (fseq, id, tapecopy, request,diskcopy) 
    VALUES (newFseq, ids_seq.nextval, tcId,trId, dcid) 
    RETURNING id INTO srId;

  INSERT INTO id2type 
    (id,type) 
    VALUES (srId,180);    
  
  UPDATE Segment SET status=7 
    WHERE id=segId 
    AND status = 0;

  OPEN outputFile FOR 
    SELECT castorfile.fileid, castorfile.nshost, ds, mp, path, newFseq, srId 
      FROM TapeCopy,Castorfile  
      WHERE tcId=TapeCopy.id 
      AND CastorFile.id=TapeCopy.castorfile;
END;
/


/* get the information from the db for a successful migration */



create or replace PROCEDURE tg_getRepackVidAndFileInfo( inputFileId IN NUMBER,
                                      inputNsHost IN VARCHAR2,
                                      inFseq IN INTEGER,
                                      transactionId IN NUMBER, 
                                      byteTransfered IN NUMBER,
                                      outRepackVid OUT NOCOPY VARCHAR2,
                                      strVid OUT NOCOPY VARCHAR,
                                      outCopyNb OUT INTEGER,
                                      outLastTime OUT NUMBER,
                                      ret OUT INTEGER) AS 
  cfId NUMBER;
  fs NUMBER;
BEGIN
  outrepackvid:=NULL;
   -- ignore the repack state
  SELECT  castorfile.lastupdatetime, castorfile.id, fileSize INTO outlasttime, cfId, fs
    FROM CastorFile 
    WHERE castorfile.fileid = inputFileId 
    AND castorfile.nshost = inputNsHost;
  IF fs <> byteTransfered THEN
  -- fail the file
    tg_failfiletransfer(transactionid,inputfileid, inputNsHost, inFseq,  1613); -- wrongfilesize
    COMMIT;
    ret:=-1;
    RETURN;
  ELSE
    ret:=0;
  END IF;
      
  SELECT copynb INTO outcopynb 
    FROM TapeCopy,TapeGatewaySubRequest, TapeGatewayRequest  
    WHERE TapeCopy.id = TapeGatewaySubRequest.tapecopy 
    AND TapeGatewayRequest.id = TapeGatewaySubRequest.request
    AND Tapecopy.castorfile = cfId
    AND TapeGatewaySubRequest.fseq= inFseq
    AND TapeGatewayRequest.vdqmVolReqId = transactionId;
 
  SELECT vid INTO strVid 
    FROM Tape,Stream,TapeGatewayRequest 
    WHERE  Tape.id = Stream.tape
    AND  TapeGatewayRequest.streammigration = stream.id
    AND  TapeGatewayRequest.vdqmVolReqId = transactionId;

  BEGIN 
   --REPACK case
    -- Get the repackvid field from the existing request (if none, then we are not in a repack process
     SELECT StageRepackRequest.repackvid INTO outRepackVid
       FROM SubRequest, DiskCopy,StageRepackRequest
       WHERE StageRepackRequest.id = SubRequest.request
       AND DiskCopy.id = SubRequest.diskcopy
       AND DiskCopy.status = 10 -- CANBEMIGR
       AND SubRequest.status = 12 -- SUBREQUEST_REPACK
       AND DiskCopy.castorfile = cfId
       AND ROWNUM < 2;
  EXCEPTION WHEN NO_DATA_FOUND THEN
   -- standard migration
    null;
  END;
END;
/



/* get the information from the db for a successful recall */

CREATE OR REPLACE PROCEDURE tg_getSegmentInfo ( transactionId IN NUMBER,
                                                inFileId IN NUMBER, 
                                                inHost IN VARCHAR2,
                                                inFseq IN INTEGER, 
                                                outVid OUT NOCOPY VARCHAR2, 
                                                outCopy OUT INTEGER ) AS
 trId NUMBER;
 cfId NUMBER;

BEGIN
  SELECT Tape.vid, TapeGatewayRequest.id  INTO outVid, trId 
    FROM Tape,TapeGatewayRequest
    WHERE Tape.id=TapeGatewayRequest.taperecall 
    AND TapeGatewayRequest.vdqmvolreqid= transactionId
    AND Tape.tpmode = 0;

  SELECT copynb INTO outCopy FROM TapeCopy, TapeGatewaySubRequest
    WHERE TapeCopy.id = TapeGatewaySubRequest.tapecopy
    AND TapeGatewaySubRequest.fseq = inFseq
    AND TapeGatewaySubRequest.request = trId;
END;
/

/* get the stream without any tape associated */

CREATE OR REPLACE PROCEDURE tg_getStreamsWithoutTapes (strList OUT castorTape.Stream_Cur) AS

BEGIN
 -- get request in status TO_BE_RESOLVED
 OPEN strList FOR
    SELECT Stream.id, Stream.initialsizetotransfer, Stream.status, Stream.tapepool, TapePool.name
      FROM Stream,Tapepool 
      WHERE Stream.id IN 
        ( SELECT streamMigration 
            FROM TapeGatewayRequest 
            WHERE status=0 ) 
      AND Stream.tapepool=TapePool.id 
      FOR UPDATE OF Stream.id SKIP LOCKED;   
END;
/

/* get tape with a pending request in VDQM */


CREATE OR REPLACE PROCEDURE tg_getTapesWithDriveReqs (timeLimit IN NUMBER,
                                                      taperequest OUT castorTape.tapegatewayrequest_Cur) AS

 trIds "numList";

BEGIN 
-- get requests	 in  WAITING_TAPESERVER and ONGOING

    SELECT id BULK COLLECT into trIds 
      FROM TapeGatewayRequest 
      WHERE status IN (2,3) 
      AND  gettime() - TapeGatewayRequest.lastVdqmPingTime > timeLimit 
      FOR UPDATE SKIP LOCKED; 
	
  FORALL i IN trids.FIRST .. trids.LAST
      UPDATE TapeGatewayRequest 
        SET lastVdqmPingTime = gettime() 
        WHERE id = trIds(i);
 
  OPEN taperequest FOR
      SELECT TapeGatewayRequest.accessMode, TapeGatewayRequest.id,TapeGatewayRequest.starttime,
             TapeGatewayRequest.lastvdqmpingtime, TapeGatewayRequest.vdqmvolreqid,
             TapeGatewayRequest.status, Tape.vid  
         FROM Stream,TapeGatewayRequest,Tape 
         WHERE TapeGatewayRequest.id  IN 
            (SELECT /*+ CARDINALITY(trTable 5) */ * 
               FROM TABLE(trIds) trTable)  
         AND  Stream.id = TapeGatewayRequest.streamMigration 
         AND Stream.tape=Tape.id 
         AND TapeGatewayRequest.accessMode=1

       UNION ALL

      SELECT TapeGatewayRequest.accessMode, TapeGatewayRequest.id, TapeGatewayRequest.starttime, 
             TapeGatewayRequest.lastvdqmpingtime, TapeGatewayRequest.vdqmvolreqid,
             TapeGatewayRequest.status, Tape.vid  
        FROM tapegatewayrequest,Tape 
        WHERE Tape.id=TapeGatewayRequest.tapeRecall 
        AND TapeGatewayRequest.accessMode=0 
        AND TapeGatewayRequest.id IN 
            (SELECT /*+ CARDINALITY(trTable 5) */ * 
             FROM TABLE(trIds) trTable);
END;
/

/* get a tape without any drive requests sent to VDQM */



create or replace
PROCEDURE tg_getTapeWithoutDriveReq ( reqId OUT NUMBER,tapeMode OUT NUMBER, tapeSide OUT INTEGER, tapeVid OUT NOCOPY VARCHAR2) AS
BEGIN
-- get a request in TO_BE_SENT_TO_VDQM
   SELECT id, accessmode INTO reqId, tapeMode 
     FROM TapeGatewayRequest 
     WHERE status=1 AND ROWNUM < 2 FOR UPDATE SKIP LOCKED;
  IF tapemode = 0 THEN
    -- read
    SELECT  tpmode, side, vid INTO tapeMode, tapeSide, tapeVid
       FROM TapeGatewayRequest,Tape 
       WHERE TapeGatewayRequest.id = reqId
       AND Tape.id=TapeGatewayRequest.tapeRecall;
  ELSE
     -- write  
    SELECT  tpmode, side, vid INTO tapeMode, tapeSide, tapeVid
       FROM TapeGatewayRequest,Tape,Stream 
       WHERE TapeGatewayRequest.id = reqId
       AND Stream.id = TapeGatewayRequest.streamMigration 
       AND Stream.tape=Tape.id;
  END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
       reqId:=0; 
END;
/


/* get tape to release in VMGR */

create or replace
PROCEDURE  tg_getTapeToRelease ( inputVdqmReqId IN INTEGER, 
                                                   outputTape OUT NOCOPY VARCHAR2, 
                                                   outputMode OUT INTEGER ) AS
  strId NUMBER;
  tpId NUMBER;
BEGIN
  SELECT  accessmode, streammigration, tapeRecall 
     INTO  outputmode, strId, tpId 
    FROM TapeGatewayRequest
    WHERE vdqmvolreqid = inputvdqmreqid;
    
   IF outputmode = 0 THEN
     -- read case
     SELECT vid INTO outputTape 
       FROM Tape
       WHERE id=tpId; 
   ELSE
     -- write case
     SELECT Tape.vid INTO outputtape 
       FROM Tape,Stream 
       WHERE Stream.id=strId AND Stream.tape=Tape.id;
   END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
 -- already cleaned by the checker
 null;
END;
/


/* invalidate a file that it is not possible to tape as candidate to migrate or recall */

CREATE OR REPLACE PROCEDURE tg_invalidateFile (transactionId IN NUMBER,
                                               inputFileId IN NUMBER, 
                                               inputNsHost IN VARCHAR2,
                                               inputFseq IN INTEGER,
                                               inputErrorCode IN INTEGER) AS
BEGIN

  UPDATE TapeGatewayRequest SET lastfseq=lastfseq-1 
    WHERE vdqmvolreqid=transactionid; 
  
  tg_failfiletransfer(transactionid,inputfileid, inputNsHost, inputFseq, inputErrorCode);

END;
/

/* restart taperequest which had problems */




create or replace
PROCEDURE tg_restartLostReqs(trIds IN castor."cnumList") AS

 vdqmId INTEGER;

BEGIN

 FOR  i IN trIds.FIRST .. trIds.LAST LOOP   
    BEGIN
 
     --  STATUS ONGOING: these are the ones which were ongoing ... crash clean up needed 
      SELECT vdqmvolreqid INTO vdqmId 
        FROM  TapeGatewayRequest 
        WHERE id=trIds(i) ;
      tg_endTapeSession(vdqmId,0);

    EXCEPTION WHEN NO_DATA_FOUND THEN
     NULL;
    END;
 END LOOP;
 COMMIT;
END;
/


/* update the db after a successful migration */

create or replace PROCEDURE tg_setFileMigrated  (transactionId IN NUMBER, 
                                                inputFileId  IN NUMBER,
                                                inputNsHost IN VARCHAR2, 
                                                inputFseq IN INTEGER, 
                                                inputFileTransaction NUMBER,
                                                streamReport OUT castor.StreamReport_Cur) AS
 trId NUMBER;
 tcNumb INTEGER;
 cfId NUMBER;
 tcId NUMBER;
 srId NUMBER;
 tcIds "numList";
 srIds "numList";
 dcId NUMBER;

BEGIN
 
  SELECT id  INTO trId FROM TapeGatewayRequest 
    WHERE TapeGatewayRequest.vdqmvolreqid=transactionid 
    FOR UPDATE;
  
  SELECT id  INTO cfId FROM CastorFile 
    WHERE  fileid=inputFileId 
    AND nshost=inputNsHost 
    FOR UPDATE;
   
  srId:=NULL; 
  DELETE FROM TapeGatewaySubRequest 
    WHERE id = inputFileTransaction 
    AND fseq = inputfseq
    RETURNING tapecopy, id, diskcopy INTO tcId, srId, dcId;
    
  IF srid IS NULL THEN
    raise NO_DATA_FOUND;
  END IF;

  DELETE FROM id2type WHERE id= srId;

  UPDATE tapecopy SET status=5 WHERE id=tcId;
   
  SELECT count(*) INTO tcNumb 
    FROM tapecopy 
    WHERE castorfile = cfId  
    AND STATUS != 5;

  -- let's check if another copy should be done
  IF tcNumb = 0 THEN
     UPDATE DiskCopy SET status=0 
       WHERE castorfile = cfId
       AND status=10;

     DELETE FROM tapecopy 
       WHERE castorfile=cfId 
       RETURNING id BULK COLLECT INTO tcIds;
   
     FORALL i IN tcIds.FIRST .. tcIds.LAST
       DELETE FROM id2type 
         WHERE id=tcIds(i);
  END IF;

  -- archive Repack requests should any be in the db
  FOR i IN (SELECT id FROM SubRequest WHERE castorfile=cfId AND status=12) LOOP
    archivesubreq(i.id, 8);
  END LOOP;
  
  -- return data for informing the rmMaster
  OPEN streamReport FOR
   SELECT DiskServer.name,FileSystem.mountpoint 
     FROM DiskServer,FileSystem,DiskCopy 
     WHERE DiskCopy.id=dcId 
     AND DiskCopy.filesystem=FileSystem.id 
     AND FileSystem.diskserver = DiskServer.id;
  COMMIT;
END;
/


/* update the db after a successful recall */

create or replace PROCEDURE tg_setFileRecalled (transactionId IN NUMBER, 
                                                inputFileId  IN NUMBER,
                                                inputNsHost IN VARCHAR2, 
                                                inputFseq IN NUMBER, 
                                                inputFileTransaction IN NUMBER,
                                                streamReport OUT castor.StreamReport_Cur) AS
  tcId NUMBER;
  dcId NUMBER;
  cfId NUMBER;
  unused NUMBER;
  segId NUMBER;
  tpId NUMBER;
  srId NUMBER;
  subRequestId NUMBER;
  requestId NUMBER;
  reqType NUMBER;
  fs NUMBER;
  gcw NUMBER;
  gcwProc VARCHAR(2048);
  ouid INTEGER;
  ogid INTEGER;
  svcClassId NUMBER;
  trId NUMBER;
  missingTCs INTEGER;

BEGIN
  -- take locks
  SELECT id, taperecall INTO trId, tpId 
    FROM TapeGatewayRequest 
    WHERE TapeGatewayRequest.vdqmvolreqid=transactionid 
    FOR UPDATE;

  SELECT id, fileSize into cfId, fs FROM CastorFile 
    WHERE  fileid=inputFileId 
    AND nshost=inputNsHost 
    FOR UPDATE;
   
  srid:=NULL; 
  DELETE FROM TapeGatewaySubRequest 
    WHERE id= inputFileTransaction
    AND fseq = inputFseq 
    RETURNING tapecopy,id, diskcopy INTO tcId, srId, dcId;
    
  IF srid IS NULL THEN
    raise NO_DATA_FOUND;
  END IF; 
    
  DELETE FROM id2type WHERE id = srId;
 
  -- missing tapecopies handling
  SELECT missingCopies INTO missingTCs
    FROM TapeCopy WHERE id = tcId;
    
  -- delete tapecopies
  deleteTapeCopies(cfId);
  
  -- update diskcopy status, size and gweight
  SELECT SubRequest.id, SubRequest.request, Id2Type.type 
    INTO subRequestId, requestId, reqType
    FROM SubRequest, Id2Type
   WHERE SubRequest.request = Id2Type.id
     AND SubRequest.diskcopy = dcId;
  SELECT Request.svcClass, euid, egid INTO svcClassId, ouid, ogid
    FROM (SELECT id, svcClass, euid, egid FROM StageGetRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StagePrepareToGetRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StageUpdateRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StagePrepareToUpdateRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StageRepackRequest) Request
    WHERE Request.id = requestId;
    gcwProc := castorGC.getRecallWeight(svcClassId);
  EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:size); END;'
    USING OUT gcw, IN fs;
  UPDATE DiskCopy 
    SET status = decode(missingTCs, 0, 0, 10), -- DISKCOPY_STAGED if missingTCs = 0, otherwise CANBEMIGR
        lastAccessTime = getTime(),  -- for the GC, effective lifetime of this diskcopy starts now
        gcWeight = gcw,
        diskCopySize = fs
    WHERE id = dcid;
  
  IF reqType = 119 THEN  -- OBJ_StageRepackRequest
    startRepackMigration(subRequestId, cfId, dcid, ouid, ogid);
  ELSE
    -- restart this subrequest if it's not a repack one
    UPDATE SubRequest
       SET status = 1, getNextStatus = 1,  -- SUBREQUEST_RESTART, GETNEXTSTATUS_FILESTAGED
           lastModificationTime = getTime(), parent = 0
       WHERE id = subRequestId;
    -- and trigger new migrations if missing tape copies were detected
    IF missingTCs > 0 THEN
      DECLARE
        newTcId INTEGER;
      BEGIN
        FOR i IN 1..missingTCs LOOP
          INSERT INTO TapeCopy (id, copyNb, castorFile, status, nbRetry, missingCopies)
          VALUES (ids_seq.nextval, 0, cfId, 0, 0, 0)  -- TAPECOPY_CREATED
          RETURNING id INTO newTcId;
          INSERT INTO Id2Type (id, type) VALUES (newTcId, 30); -- OBJ_TapeCopy
        END LOOP;
      END;
    END IF;
  END IF;
  
  -- restart other requests waiting on this recall
  UPDATE SubRequest
     SET status = 1, getNextStatus = 1,  -- SUBREQUEST_RESTART, GETNEXTSTATUS_FILESTAGED
         lastModificationTime = getTime(), parent = 0
     WHERE parent = subRequestId;

  -- trigger the creation of additional copies of the file, if necessary.
  replicateOnClose(cfId, ouid, ogid);

  -- return data for informing the rmMaster
  OPEN streamReport FOR
    SELECT DiskServer.name, FileSystem.mountpoint 
      FROM DiskServer, FileSystem, DiskCopy
      WHERE DiskCopy.id = dcId
      AND DiskCopy.filesystem = FileSystem.id 
      AND FileSystem.diskserver = DiskServer.id;
  COMMIT;
END;
/


/* save in the db the results returned by the retry policy for migration */

CREATE OR REPLACE PROCEDURE  tg_setMigRetryResult( tcToRetry IN castor."cnumList", 
                                                   tcToFail IN castor."cnumList"  ) AS
 srId NUMBER;
 cfId NUMBER;

BEGIN
   -- check because oracle cannot handle empty buffer
  IF tcToRetry( tcToRetry.FIRST) != -1 THEN
 
    -- restarted the one to be retried
    FORALL i IN tctoretry.FIRST .. tctoretry.LAST 
      UPDATE TapeCopy SET status=1,nbretry = nbretry+1  
        WHERE id = tcToRetry(i);    

  END IF;
  
  -- check because oracle cannot handle empty buffer
  IF tcToFail(tcToFail.FIRST) != -1 THEN 
    -- fail the tapecopies   
    FORALL i IN tctofail.FIRST .. tctofail.LAST 
      UPDATE TapeCopy SET status=6 
        WHERE id = tcToFail(i);

    -- fail repack subrequests
    FOR i IN tcToFail.FIRST .. tcToFail.LAST LOOP
        BEGIN 
        -- we don't need a lock on castorfile because we cannot have a parallel migration of the same file using repack
          SELECT SubRequest.id, SubRequest.castorfile into srId, cfId 
            FROM SubRequest,TapeCopy 
            WHERE TapeCopy.id = tcToFail(i) 
            AND SubRequest.castorfile = TapeCopy.castorfile 
            AND subrequest.status = 12;

          -- STAGED because the copy on disk most probably is valid and the failure of repack happened during the migration 

          UPDATE DiskCopy SET status=0 
            WHERE castorfile = cfId 
            AND status=10; -- otherwise repack will wait forever

          archivesubreq(srId,9);

        EXCEPTION WHEN NO_DATA_FOUND THEN
          NULL;
        END;
     END LOOP;
  END IF;
  
  COMMIT;
END;
/

/* save in the db the results returned by the retry policy for recall */

CREATE OR REPLACE PROCEDURE  tg_setRecRetryResult( tcToRetry IN castor."cnumList", 
                                                   tcToFail IN castor."cnumList"  ) AS
 tapeId NUMBER;
 cfId NUMBER;

BEGIN
  -- I restart the recall that I want to retry
  -- check because oracle cannot handle empty buffer
  IF tcToRetry(tcToRetry.FIRST) != -1 THEN 

    -- tapecopy => TOBERECALLED
    FORALL i IN tcToRetry.FIRST .. tcToRetry.LAST
      UPDATE TapeCopy SET status=4, errorcode=0, nbretry= nbretry+1 
        WHERE id=tcToRetry(i);
    
    -- segment => UNPROCESSED
    -- tape => PENDING if UNUSED OR FAILED with still segments unprocessed
    FOR i IN tcToRetry.FIRST .. tcToRetry.LAST LOOP
      UPDATE Segment SET status=0
       WHERE copy = tcToRetry(i)
      RETURNING tape INTO tapeId;
      UPDATE Tape set status = 8
       WHERE id = tapeId AND status IN (0, 6);
    END LOOP;
  END IF;
  
  -- I mark as failed the hopeless recall
  -- check because oracle cannot handle empty buffer
  IF tcToFail(tcToFail.FIRST) != -1 THEN
    FOR i IN tcToFail.FIRST .. tcToFail.LAST  LOOP

      -- lock castorFile	
      SELECT castorFile INTO cfId 
        FROM TapeCopy,CastorFile
        WHERE TapeCopy.id = tcToFail(i) 
        AND CastorFile.id = TapeCopy.castorfile 
        FOR UPDATE OF castorfile.id;

      -- fail diskcopy
      UPDATE DiskCopy SET status = 4 -- DISKCOPY_FAILED
        WHERE castorFile = cfId 
        AND status = 2; -- DISKCOPY_WAITTAPERECALL   
      
      -- Drop tape copies. Ideally, we should keep some track that
      -- the recall failed in order to prevent future recalls until some
      -- sort of manual intervention. For the time being, as we can't
      -- say whether the failure is fatal or not, we drop everything
      -- and we won't deny a future request for recall.
      deleteTapeCopies(cfId);
      
      -- fail subrequests
      UPDATE SubRequest 
        SET status = 7, -- SUBREQUEST_FAILED
            getNextStatus = 1, -- GETNEXTSTATUS_FILESTAGED (not strictly correct but the request is over anyway)
            lastModificationTime = getTime(),
            errorCode = 1015,  -- SEINTERNAL
            errorMessage = 'File recall from tape has failed, please try again later',
            parent = 0
        WHERE castorFile = cfId 
        AND status IN (4, 5); -- WAITTAPERECALL, WAITSUBREQ
    
    END LOOP;
  END IF;
  COMMIT;
END;
/


/* update the db when a tape session is started */

CREATE OR REPLACE PROCEDURE  tg_startTapeSession ( inputVdqmReqId IN NUMBER, 
                                                   outVid OUT NOCOPY VARCHAR2, 
                                                   outMode OUT INTEGER, 
                                                   ret OUT INTEGER, 
                                                   outDensity OUT NOCOPY VARCHAR2, 
                                                   outLabel OUT NOCOPY VARCHAR2 ) AS
 reqId NUMBER;
 tpId NUMBER;
 segToDo INTEGER;
 tcToDo INTEGER;

BEGIN

  ret:=-2;
  -- set the request to ONGOING
  UPDATE TapeGatewayRequest SET status=3 
    WHERE vdqmVolreqid = inputVdqmReqId  
    AND status =2 
    RETURNING id, accessmode INTO reqId, outMode;
  
  IF reqId IS NULL THEN
  
   ret:=-2; -- UNKNOWN request
 
  ELSE  
 
    IF outMode = 0 THEN
      -- read tape mounted
      SELECT count(*) INTO segToDo 
        FROM Segment 
        WHERE status=0 
        AND tape = 
           (SELECT taperecall 
              FROM TapeGatewayRequest 
              WHERE id=reqId);
 
      IF segToDo = 0 THEN

        UPDATE TapeGatewayRequest SET lastvdqmpingtime=0 
          WHERE id=reqId; -- to force the cleanup
        ret:=-1; --NO MORE FILES

      ELSE

        UPDATE Tape SET status=4 
          WHERE id = 
             (SELECT taperecall 
                FROM TapeGatewayRequest 
                WHERE id=reqId) 
          AND tpmode=0  
          RETURNING vid, label, density INTO outVid, outLabel, outDensity; -- tape is MOUNTED 
        
        ret:=0;
      
      END IF;
    ELSE 
      -- write
      SELECT COUNT(*) INTO tcToDo 
        FROM Stream2TapeCopy 
        WHERE parent IN 
          ( SELECT streammigration 
              FROM TapeGatewayRequest 
              WHERE id=reqId );

      IF tcToDo = 0 THEN

        UPDATE TapeGatewayRequest SET lastvdqmpingtime=0 
          WHERE id=reqId; -- to force the cleanup

        ret:=-1; --NO MORE FILES
        outVid:=NULL;
      
      ELSE
      
        UPDATE Stream SET status=3 
          WHERE id = 
             ( SELECT streammigration 
                 FROM TapeGatewayRequest 
                 WHERE id=reqId ) 
          RETURNING tape INTO tpId; -- RUNNING

        UPDATE Tape SET status=4  
          WHERE id = tpId 
          RETURNING vid, label, density INTO outVid, outLabel, outDensity; -- MOUNTED

        ret:=0;

      END IF;    

    END IF; 

  END IF;
  COMMIT;
END;
/


/* Check configuration */

create or replace
PROCEDURE tg_checkConfiguration AS
unused VARCHAR2(2048);
BEGIN
 SELECT value INTO unused FROM castorconfig WHERE class='tape' AND key='daemonName' AND value='tapegatewayd';
END;
/


/* delete streams for not existing tapepools */

create or replace
PROCEDURE tg_deleteStream( strId IN NUMBER ) AS
unused NUMBER;
tcIds "numList";
trId NUMBER;
BEGIN
 DELETE FROM tapegatewayrequest WHERE streammigration=strId RETURNING id INTO trId;
 DELETE FROM id2type WHERE id=trId;
 SELECT id INTO unused FROM Stream WHERE id=strId FOR UPDATE;
 DELETE FROM stream2tapecopy WHERE parent=strId RETURNING child BULK COLLECT INTO tcIds;
 FORALL i IN tcIds.FIRST .. tcIds.LAST
    UPDATE tapecopy SET status = 1 WHERE tcIds(i)=id AND NOT EXISTS (SELECT 'x' FROM stream2tapecopy WHERE stream2tapecopy.child=tcIds(i));
 DELETE FROM id2type where id=strId;
 DELETE FROM stream where id=strId;
END;
/
	

/* delete taperequest  for not existing tape */

create or replace
PROCEDURE tg_deleteTapeRequest( reqId IN NUMBER ) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -02292);
  tpId NUMBER;
  strId NUMBER;
  reqMode INTEGER;
  segNum INTEGER;
  cfId NUMBER;
  tcIds "numList";
  srIds "numList";
 
BEGIN

   --delete the tape request. subrequests don't exist
    DELETE FROM TapeGatewayRequest 
      WHERE id=reqId AND status=1  
      RETURNING taperecall, streammigration,accessmode
      INTO tpId, strid, reqMode;

    DELETE FROM id2type WHERE id= reqid;
    IF reqmode = 0 THEN
      SELECT id INTO tpId
        FROM Tape
        WHERE id =tpId
        FOR UPDATE;
        
      SELECT copy BULK COLLECT INTO tcIds FROM Segment WHERE tape=tpId;
    
      FOR i IN tcIds.FIRST .. tcIds.LAST  LOOP
        -- lock castorFile	
        SELECT castorFile INTO cfId 
          FROM TapeCopy,CastorFile
          WHERE TapeCopy.id = tcIds(i) 
          AND CastorFile.id = TapeCopy.castorfile 
          FOR UPDATE OF castorfile.id;

        -- fail diskcopy
        UPDATE DiskCopy SET status = 4 -- DISKCOPY_FAILED
          WHERE castorFile = cfId 
          AND status = 2; -- DISKCOPY_WAITTAPERECALL   

        deleteTapeCopies(cfId);
        UPDATE SubRequest 
          SET status = 7, -- SUBREQUEST_FAILED
              getNextStatus = 1, -- GETNEXTSTATUS_FILESTAGED (not strictly correct but the request is over anyway)
              lastModificationTime = getTime(),
              errorCode = 1015,  -- SEINTERNAL
              errorMessage = 'File recall from tape has failed, please try again later',
              parent = 0
          WHERE castorFile = cfId 
          AND status IN (4, 5); -- WAITTAPERECALL, WAITSUBREQ
    
      END LOOP;
    END IF;
    IF reqmode = 1 THEN
    -- write
    -- reset stream
       BEGIN
         DELETE FROM Stream WHERE id = strId;
         DELETE FROM id2type WHERE id=strId;

       EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
       -- constraint violation and we cannot delete the stream because there is an entry in Stream2TapeCopy
         UPDATE Stream SET status = 6, tape = null, lastFileSystemChange = null
           WHERE id = strId;
       END;
       -- reset tape
       UPDATE Tape SET status=0, stream = null WHERE stream = strId;
  END IF;
END;
/


