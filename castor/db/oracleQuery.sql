/* This file contains SQL code that is not generated automatically */
/* and is inserted at the end of the generated code           */

/* A small table used to cross check code and DB versions */
DROP TABLE CastorVersion;
CREATE TABLE CastorVersion (version VARCHAR2(2048));
INSERT INTO CastorVersion VALUES ('2_0_0_17');

/* Indexes related to CastorFiles */
CREATE UNIQUE INDEX I_DiskServer_name on DiskServer (name);
CREATE UNIQUE INDEX I_CastorFile_fileIdNsHost on CastorFile (fileId, nsHost);
CREATE INDEX I_DiskCopy_Castorfile on DiskCopy (castorFile);
CREATE INDEX I_DiskCopy_FileSystem on DiskCopy (fileSystem);
CREATE INDEX I_TapeCopy_Castorfile on TapeCopy (castorFile);
CREATE INDEX I_SubRequest_Castorfile on SubRequest (castorFile);
CREATE INDEX I_FileSystem_DiskPool on FileSystem (diskPool);
CREATE INDEX I_SubRequest_DiskCopy on SubRequest (diskCopy);
CREATE INDEX I_SubRequest_Request on SubRequest (request);

/* A little function base index to speed up subrequestToDo */
CREATE INDEX I_SubRequest_Status on SubRequest
  (CASE status WHEN 0 THEN status WHEN 1 THEN status WHEN 2 THEN status ELSE NULL end);

/* Constraint on FileClass name */
ALTER TABLE FileClass ADD UNIQUE (name); 

/* Add unique constraint on castorFiles */
ALTER TABLE CastorFile ADD UNIQUE (fileId, nsHost); 

/* get current time as a time_t. Not that easy in ORACLE */
CREATE OR REPLACE FUNCTION getTime RETURN NUMBER IS
  ret NUMBER;
BEGIN
  SELECT (SYSDATE - to_date('01-jan-1970 01:00:00','dd-mon-yyyy HH:MI:SS')) * (24*60*60) INTO ret FROM DUAL;
  RETURN ret;
END;

/****************************************************************/
/* NbTapeCopiesInFS to work around ORACLE missing optimizations */
/****************************************************************/

/* This table keeps track of the number of TapeCopy to migrate
 * which have a diskCopy on this fileSystem. This table only exist
 * because Oracle is not able to optimize the following query :
 * SELECT max(A) from A, B where A.pk = B.fk;
 * Such a query is needed in bestTapeCopyForStream in order to
 * select filesystems effectively having tapecopies.
 * As a work around, we keep track of the tapecopies for each
 * filesystem. The cost is an increase of complexity and especially
 * of the number of triggers ensuring consistency of the whole database */
DROP TABLE NbTapeCopiesInFS;
CREATE TABLE NbTapeCopiesInFS (FS NUMBER, Stream NUMBER, NbTapeCopies NUMBER);
CREATE UNIQUE INDEX I_NbTapeCopiesInFS_FSStream on NbTapeCopiesInFS(FS, Stream);

/* Used to create a row INTO NbTapeCopiesInFS whenever a new
   FileSystem is created */
CREATE OR REPLACE TRIGGER tr_FileSystem_Insert
BEFORE INSERT ON FileSystem
FOR EACH ROW
BEGIN
  FOR item in (SELECT id FROM Stream) LOOP
    INSERT INTO NbTapeCopiesInFS (FS, Stream, NbTapeCopies) VALUES (:new.id, item.id, 0);
  END LOOP;
END;

/* Used to delete rows IN NbTapeCopiesInFS whenever a
   FileSystem is deleted */
CREATE OR REPLACE TRIGGER tr_FileSystem_Delete
BEFORE DELETE ON FileSystem
FOR EACH ROW
BEGIN
  DELETE FROM NbTapeCopiesInFS WHERE FS = :old.id;
END;

/* Used to create a row INTO NbTapeCopiesInFS whenever a new
   Stream is created */
CREATE OR REPLACE TRIGGER tr_Stream_Insert
BEFORE INSERT ON Stream
FOR EACH ROW
BEGIN
  FOR item in (SELECT id FROM FileSystem) LOOP
    INSERT INTO NbTapeCopiesInFS (FS, Stream, NbTapeCopies) VALUES (item.id, :new.id, 0);
  END LOOP;
END;

/* Used to delte rows IN NbTapeCopiesInFS whenever a
   Stream is deleted */
CREATE OR REPLACE TRIGGER tr_Stream_Delete
BEFORE DELETE ON Stream
FOR EACH ROW
BEGIN
  DELETE FROM NbTapeCopiesInFS WHERE Stream = :old.id;
END;

/* Updates the count of tapecopies in NbTapeCopiesInFS
   whenever a TapeCopy is linked to a Stream */
CREATE OR REPLACE TRIGGER tr_Stream2TapeCopy_Insert
AFTER INSERT ON Stream2TapeCopy
FOR EACH ROW
BEGIN
  UPDATE NbTapeCopiesInFS SET NbTapeCopies = NbTapeCopies + 1
   WHERE FS IN (SELECT DiskCopy.FileSystem
                  FROM DiskCopy, TapeCopy
                 WHERE DiskCopy.CastorFile = TapeCopy.castorFile
                   AND TapeCopy.id = :new.child)
     AND Stream = :new.parent;
END;

/* XXX update count into NbTapeCopiesInFS when a Disk2Disk copy occurs
   FOR a file in CANBEMIGR */

/***************************************/
/* Some triggers to prevent dead locks */
/***************************************/

/* Used to avoid LOCK TABLE TapeCopy whenever someone wants
   to deal with the tapeCopies on a CastorFile.
   Due to this trigger, locking the CastorFile is enough
   to be safe */
CREATE OR REPLACE TRIGGER tr_TapeCopy_CastorFile
BEFORE INSERT OR UPDATE OF castorFile ON TapeCopy
FOR EACH ROW WHEN (new.castorFile > 0)
DECLARE
  unused CastorFile%ROWTYPE;
BEGIN
  SELECT * INTO unused FROM CastorFile
   WHERE id = :new.castorFile FOR UPDATE;
END;

/* Used to avoid LOCK TABLE TapeCopy whenever someone wants
   to deal with the tapeCopies on a CastorFile.
   Due to this trigger, locking the CastorFile is enough
   to be safe */
CREATE OR REPLACE TRIGGER tr_DiskCopy_CastorFile
BEFORE INSERT OR UPDATE OF castorFile ON DiskCopy
FOR EACH ROW WHEN (new.castorFile > 0)
DECLARE
  unused CastorFile%ROWTYPE;
BEGIN
  SELECT * INTO unused FROM CastorFile
   WHERE id = :new.castorFile FOR UPDATE;
END;

/* Used to avoid LOCK TABLE Stream2TapeCopy whenever someone wants
   to deal with the TapeCopies of a Stream.
   Due to this trigger, locking the Stream is enough
   to be safe */
CREATE OR REPLACE TRIGGER tr_Stream2TapeCopy_Stream
BEFORE INSERT OR UPDATE ON Stream2TapeCopy
FOR EACH ROW
DECLARE
  unused Stream%ROWTYPE;
BEGIN
  SELECT * INTO unused FROM Stream
   WHERE id = :new.Parent FOR UPDATE;
END;

/*********************/
/* FileSystem rating */
/*********************/

/* Computes a 'rate' for the filesystem which is an agglomeration
   of weight and fsDeviation. The goal is to be able to classify
   the fileSystems using a single value and to put an index on it */
CREATE OR REPLACE FUNCTION FileSystemRate
(weight IN NUMBER,
 deltaWeight IN NUMBER,
 fsDeviation IN NUMBER)
RETURN NUMBER DETERMINISTIC IS
BEGIN
  RETURN 1000*(weight + deltaWeight) - fsDeviation;
END;

/* FileSystem index based on the rate. */
CREATE INDEX I_FileSystem_Rate ON FileSystem(FileSystemRate(weight, deltaWeight, fsDeviation));

/*************************/
/* Procedure definitions */
/*************************/

/* PL/SQL method to make a SubRequest wait on another one, linked to the given DiskCopy */
CREATE OR REPLACE PROCEDURE makeSubRequestWait(subreqId IN INTEGER, dci IN INTEGER) AS
BEGIN
 UPDATE SubRequest
  SET parent = (SELECT id FROM SubRequest WHERE diskCopy = dci),
      status = 5, lastModificationTime = getTime() -- WAITSUBREQ
  WHERE SubRequest.id = subreqId;
END;

/* PL/SQL method to archive a SubRequest and its request if needed */
CREATE OR REPLACE PROCEDURE archiveSubReq(srId IN INTEGER) AS
  rid INTEGER;
  rtype INTEGER;
  rclient INTEGER;
  nb INTEGER;
BEGIN
  -- update status of SubRequest
  UPDATE SubRequest SET status = 8 -- FINISHED
   WHERE id = srId RETURNING request INTO rid;
  -- Try to see whether another subrequest in the same
  -- request is still procesing
  SELECT count(*) INTO nb FROM SubRequest
   WHERE request = rid AND status != 8; -- FINISHED
  -- Archive request, client and SubRequests if needed
  IF nb = 0 THEN
    -- DELETE request from Id2Type
    DELETE FROM Id2Type WHERE id = rid RETURNING type INTO rtype;
    -- delete request and get client id
    CASE rtype
      WHEN 35 THEN -- StageGetRequest
        DELETE FROM StageGetRequest WHERE id = rid RETURNING client into rclient;
      WHEN 40 THEN -- StagePutRequest
        DELETE FROM StagePutRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 44 THEN -- StageUpdateRequest
        DELETE FROM StageUpdateRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 39 THEN -- StagePutDoneRequest
        DELETE FROM StagePutDoneRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 42 THEN -- StageRmRequest
        DELETE FROM StageRmRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 51 THEN -- StageReleaseFilesRequest
        DELETE FROM StageReleaseFilesRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 43 THEN -- StageUpdateFileStatusRequest
        DELETE FROM StageUpdateFileStatusRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 36 THEN -- StagePrepareToGetRequest
        DELETE FROM StagePrepareToGetRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 37 THEN -- StagePrepareToPutRequest
        DELETE FROM StagePrepareToPutRequest WHERE id = rid RETURNING client INTO rclient;
      WHEN 38 THEN -- StagePrepareToUpdateRequest
        DELETE FROM StagePrepareToUpdateRequest WHERE id = rid RETURNING client INTO rclient;
    END CASE;
    -- DELETE Client
    DELETE FROM Id2Type WHERE id = rclient;
    DELETE FROM Client WHERE id = rclient;
    -- Delete SubRequests
    DELETE FROM Id2Type WHERE id IN
      (SELECT id FROM SubRequest WHERE request = rid);
    DELETE FROM SubRequest WHERE request = rid;
  END IF;
END;

/* PL/SQL method implementing anyTapeCopyForStream.
 * This implementation is not the original one. It uses NbTapeCopiesInFS
 * because a join on the tables between DiskServer and Stream2TapeCopy
 * costs too much. It should actually not be the case but ORACLE is unable
 * to optimize correctly queries having a ROWNUM clause. It procesed the
 * the query without it (yes !!!) and apply the clause afterwards.
 * Here is the previous select in case ORACLE improves some day :
 * SELECT \/*+ FIRST_ROWS *\/ TapeCopy.id INTO unused
 * FROM DiskServer, FileSystem, DiskCopy, CastorFile, TapeCopy, Stream2TapeCopy
 *  WHERE DiskServer.id = FileSystem.diskserver
 *   AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
 *   AND FileSystem.id = DiskCopy.filesystem
 *   AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
 *   AND DiskCopy.castorfile = CastorFile.id
 *   AND TapeCopy.castorfile = Castorfile.id
 *   AND Stream2TapeCopy.child = TapeCopy.id
 *   AND Stream2TapeCopy.parent = streamId
 *   AND TapeCopy.status = 2 -- WAITINSTREAMS
 *   AND ROWNUM < 2;  */
CREATE OR REPLACE PROCEDURE anyTapeCopyForStream(streamId IN INTEGER, res OUT INTEGER) AS
  unused INTEGER;
BEGIN
  SELECT NbTapeCopiesInFS.NbTapeCopies INTO unused
    FROM NbTapeCopiesInFS
   WHERE NbTapeCopiesInFS.stream = streamId
     AND NbTapeCopiesInFS.NbTapeCopies > 0
     AND ROWNUM < 2;
  res := 1;
EXCEPTION
 WHEN NO_DATA_FOUND THEN
  res := 0;
END;

/* PL/SQL method to update FileSystem weight for new streams */
CREATE OR REPLACE PROCEDURE updateFsFileOpened
(ds IN INTEGER, fs IN INTEGER,
 deviation IN INTEGER, fileSize IN INTEGER) AS
BEGIN
 UPDATE FileSystem SET deltaWeight = deltaWeight - deviation
  WHERE diskServer = ds;
 UPDATE FileSystem SET fsDeviation = 2 * deviation,
                       reservedSpace = reservedSpace + fileSize
  WHERE id = fs;
END;

/* PL/SQL method to update FileSystem free space when file are closed */
CREATE OR REPLACE PROCEDURE updateFsFileClosed
(fs IN INTEGER, reservation IN INTEGER, fileSize IN INTEGER) AS
BEGIN
 UPDATE FileSystem SET deltaFree = deltaFree - fileSize,
                       reservedSpace = reservedSpace - reservation
  WHERE id = fs;
END;

/* This table is needed to insure that bestTapeCopyForStream works Ok.
 * It basically serializes the queries ending to the same diskserver.
 * This is only needed because of lack of funstionnality in ORACLE.
 * The original goal was to lock the selected filesystem in the first
 * query of bestTapeCopyForStream. But a SELECT FOR UPDATE was not enough
 * because it does not revalidate the inner select and we were thus selecting
 * n times the same filesystem when n queries were processed in parallel.
 * (take care, they were processed sequentially due to the lock, but they
 * were still locking the same filesystem). Thus an UPDATE was needed but
 * UPDATE cannot use joins. Thus it was impossible to lock DiskServer
 * and FileSystem at the same time (we need to avoid the DiskServer to
 * be chosen again (with a different filesystem) before we update the
 * weight of all filesystems). Locking the diskserver only was fine but
 * was introducing a possible deadlock with a place where the FileSystem
 * is locked before the DiskServer. Thus this table..... */
DROP TABLE LockTable;
CREATE TABLE LockTable (DiskServerId NUMBER PRIMARY KEY, TheLock NUMBER);
INSERT INTO LockTable SELECT id, id FROM DiskServer;

/* Used to create a row INTO LockTable whenever a new
   DiskServer is created */
CREATE OR REPLACE TRIGGER tr_DiskServer_Insert
BEFORE INSERT ON DiskServer
FOR EACH ROW
BEGIN
  INSERT INTO LockTable (DiskServerId, TheLock) VALUES (:new.id, 0);
END;

/* Used to delete rows IN LockTable whenever a
   DiskServer is deleted */
CREATE OR REPLACE TRIGGER tr_DiskServer_Delete
BEFORE DELETE ON DiskServer
FOR EACH ROW
BEGIN
  DELETE FROM LockTable WHERE DiskServerId = :old.id;
END;

/* PL/SQL method implementing updateFileSystemForJob */
CREATE OR REPLACE PROCEDURE updateFileSystemForJob
(fs IN VARCHAR2, ds IN VARCHAR2,
 fileSize IN NUMBER) AS
  fsID NUMBER;
  dsId NUMBER;
  dev NUMBER;
  unused NUMBER;
BEGIN
  SELECT FileSystem.id, FileSystem.fsDeviation,
         DiskServer.id INTO fsId, dev, dsId
    FROM FileSystem, DiskServer
   WHERE FileSystem.diskServer = DiskServer.id
     AND FileSystem.mountPoint = fs
     AND DiskServer.name = ds;
  -- We have to lock the DiskServer in the LockTable TABLE if we want
  -- to avoid dead locks with bestTapeCopyForStream. See the definition
  -- of the table for a complete explanation on why it exists
  SELECT TheLock INTO unused FROM LockTable WHERE DiskServerId = dsId FOR UPDATE;
  updateFsFileOpened(dsId, fsId, dev, fileSize);
END;

/* PL/SQL method implementing bestTapeCopyForStream */
CREATE OR REPLACE PROCEDURE bestTapeCopyForStream(streamId IN INTEGER,
                                                  diskServerName OUT VARCHAR2, mountPoint OUT VARCHAR2,
                                                  path OUT VARCHAR2, dci OUT INTEGER,
                                                  castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                  nsHost OUT VARCHAR2, fileSize OUT INTEGER,
                                                  tapeCopyId OUT INTEGER) AS
 fileSystemId INTEGER;
 dsid NUMBER;
 deviation NUMBER;
 fsDiskServer NUMBER;
BEGIN
  -- We lock here a given DiskServer. See the comment for the creation of the LockTable
  -- table for a full explanation of why we need such a stupid UPDATE statement.
  UPDATE LockTable SET TheLock = 1
   WHERE DiskServerId =
   (SELECT DiskServer.id 
      FROM FileSystem, NbTapeCopiesInFS, DiskServer
     WHERE FileSystemRate(FileSystem.weight, FileSystem.deltaWeight, FileSystem.fsDeviation) =
     -- The double level of subselects is due to the fact that ORACLE is unable
     -- to use ROWNUM and ORDER BY at the same time. Thus, we have to first computes
     -- the maxRate and then select on it.
     (SELECT MAX(FileSystemRate(FileSystem.weight, FileSystem.deltaWeight, FileSystem.fsDeviation))
        FROM FileSystem, NbTapeCopiesInFS, DiskServer
       WHERE FileSystem.id = NbTapeCopiesInFS.FS
         AND NbTapeCopiesInFS.NbTapeCopies > 0
         AND NbTapeCopiesInFS.Stream = StreamId
         AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
         AND DiskServer.id = FileSystem.diskserver
         AND DiskServer.status IN (0, 1)) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
       -- here we need to put all the check again in case we have 2 filesystems
       -- with the same rate and one is not eligible !
       AND FileSystem.id = NbTapeCopiesInFS.FS
       AND NbTapeCopiesInFS.NbTapeCopies > 0
       AND NbTapeCopiesInFS.Stream = StreamId
       AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
       AND DiskServer.id = FileSystem.diskserver
       AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
       AND ROWNUM < 2)
   RETURNING DiskServerId INTO dsid;
  -- Now we got our Diskserver but we lost all other data (due to the fact we had
  -- to do an update and we could not do a join in the update)
  -- So let's get again the best filesystem on the diskServer (we could not get it straight
  -- due to the need of an update on the LockTable
  SELECT FileSystem.id INTO fileSystemId
    FROM FileSystem, NbTapeCopiesInFS
   WHERE FileSystemRate(FileSystem.weight, FileSystem.deltaWeight, FileSystem.fsDeviation) =
     (SELECT MAX(FileSystemRate(FileSystem.weight, FileSystem.deltaWeight, FileSystem.fsDeviation))
        FROM FileSystem, NbTapeCopiesInFS
       WHERE FileSystem.id = NbTapeCopiesInFS.FS
         AND NbTapeCopiesInFS.NbTapeCopies > 0
         AND NbTapeCopiesInFS.Stream = StreamId
         AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
         AND FileSystem.diskserver = dsId)
     -- Again, we need to put all the check again in case we have 2 filesystems
     -- with the same rate and one is not eligible !
     AND FileSystem.id = NbTapeCopiesInFS.FS
     AND NbTapeCopiesInFS.NbTapeCopies > 0
     AND NbTapeCopiesInFS.Stream = StreamId
     AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
     AND FileSystem.diskserver = dsId
     AND ROWNUM < 2;
  -- Now select what we need
  SELECT DiskServer.name, FileSystem.mountPoint, FileSystem.fsDeviation, FileSystem.diskserver, FileSystem.id
    INTO diskServerName, mountPoint, deviation, fsDiskServer, fileSystemId
    FROM FileSystem, DiskServer
   WHERE FileSystem.id = fileSystemId
     AND DiskServer.id = FileSystem.diskserver;
  SELECT /*+ FIRST_ROWS */
    DiskCopy.path, DiskCopy.id, CastorFile.id, CastorFile.fileId, CastorFile.nsHost, CastorFile.fileSize, TapeCopy.id
    INTO path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId
    FROM DiskCopy, CastorFile, TapeCopy, Stream2TapeCopy
   WHERE DiskCopy.filesystem = fileSystemId
     AND DiskCopy.castorfile = CastorFile.id
     AND DiskCopy.status = 10 -- CANBEMIGR
     AND TapeCopy.castorfile = Castorfile.id
     AND Stream2TapeCopy.child = TapeCopy.id
     AND Stream2TapeCopy.parent = streamId
     AND TapeCopy.status = 2 -- WAITINSTREAMS
     AND ROWNUM < 2;
  -- update status of selected tapecopy and stream
  UPDATE TapeCopy SET status = 3 -- SELECTED
   WHERE id = tapeCopyId;
  UPDATE Stream SET status = 3 -- RUNNING
   WHERE id = streamId;
  -- update NbTapeCopiesInFS accordingly. Take care to remove the
  -- TapeCopy from all streams and all filesystems
  UPDATE NbTapeCopiesInFS SET NbTapeCopies = NbTapeCopies - 1
   WHERE FS IN (SELECT DiskCopy.FileSystem
                  FROM DiskCopy, TapeCopy
                 WHERE DiskCopy.CastorFile = TapeCopy.castorFile
                   AND TapeCopy.id = tapeCopyId)
     AND Stream IN (SELECT parent FROM Stream2TapeCopy WHERE child = tapeCopyId);
  -- Update Filesystem state
  updateFsFileOpened(fsDiskServer, fileSystemId, deviation, 0);
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- No data found means the selected filesystem has no
    -- tapecopies to be migrated. Thus we go to next one
    NULL;
END; 

/* PL/SQL method implementing bestFileSystemForSegment */
CREATE OR REPLACE PROCEDURE bestFileSystemForSegment(segmentId IN INTEGER, diskServerName OUT VARCHAR2,
                                                     rmountPoint OUT VARCHAR2, rpath OUT VARCHAR2,
                                                     dci OUT INTEGER) AS
 fileSystemId NUMBER;
 castorFileId NUMBER;
 deviation NUMBER;
 fsDiskServer NUMBER;
 fileSize NUMBER;
BEGIN
 -- First get the DiskCopy and see whether it already has a fileSystem
 -- associated (case of a multi segment file)
 SELECT DiskCopy.fileSystem, DiskCopy.path, DiskCopy.id, DiskCopy.CastorFile
   INTO fileSystemId, rpath, dci, castorFileId
   FROM TapeCopy, Segment, DiskCopy
    WHERE Segment.id = segmentId
     AND Segment.copy = TapeCopy.id
     AND DiskCopy.castorfile = TapeCopy.castorfile;
 -- Check if the DiskCopy had a FileSystem associated
 IF fileSystemId > 0 THEN
   BEGIN
     -- it had one, force filesystem selection, unless it was disabled.
     SELECT DiskServer.name, DiskServer.id, FileSystem.mountPoint, FileSystem.fsDeviation
     INTO diskServerName, fsDiskServer, rmountPoint, deviation
     FROM DiskServer, FileSystem
      WHERE FileSystem.id = fileSystemId
       AND FileSystem.status = 0 -- FILESYSTEM_PRODUCTION
       AND DiskServer.id = FileSystem.diskServer
       AND DiskServer.status = 0; -- DISKSERVER_PRODUCTION
     updateFsFileOpened(fsDiskServer, fileSystemId, deviation, 0);
   EXCEPTION WHEN NO_DATA_FOUND THEN
     -- Error, the filesystem or the machine was probably disabled in between
     raise_application_error(-20101, 'In a multi-segment file, FileSystem or Machine was disabled before all segments were recalled');
   END;
 ELSE
   DECLARE
     CURSOR c1 IS SELECT DiskServer.name, FileSystem.mountPoint, FileSystem.id,
                       FileSystem.fsDeviation, FileSystem.diskserver, SubRequest.xsize
                    FROM DiskServer, FileSystem, DiskPool2SvcClass,
                         (SELECT id, svcClass from StageGetRequest UNION
                          SELECT id, svcClass from StagePrepareToGetRequest UNION
                          SELECT id, svcClass from StageGetNextRequest UNION
                          SELECT id, svcClass from StageUpdateRequest UNION
                          SELECT id, svcClass from StagePrepareToUpdateRequest UNION
                          SELECT id, svcClass from StageUpdateNextRequest) Request,
                         SubRequest, CastorFile
                   WHERE CastorFile.id = castorfileId
                     AND SubRequest.castorfile = castorfileId
                     AND Request.id = SubRequest.request
                     AND Request.svcclass = DiskPool2SvcClass.child
                     AND FileSystem.diskpool = DiskPool2SvcClass.parent
                     AND FileSystem.free + FileSystem.deltaFree - FileSystem.reservedSpace > CastorFile.fileSize
                     AND FileSystem.status = 0 -- FILESYSTEM_PRODUCTION
                     AND DiskServer.id = FileSystem.diskServer
                     AND DiskServer.status = 0 -- DISKSERVER_PRODUCTION
                   ORDER BY FileSystem.weight + FileSystem.deltaWeight DESC, FileSystem.fsDeviation ASC;
    BEGIN
      OPEN c1;
      FETCH c1 INTO diskServerName, rmountPoint, fileSystemId, deviation, fsDiskServer, fileSize;
      CLOSE c1;
      UPDATE DiskCopy SET fileSystem = fileSystemId WHERE id = dci;
      updateFsFileOpened(fsDiskServer, fileSystemId, deviation, fileSize);
    END;
  END IF;
END;

/* PL/SQL method implementing fileRecalled */
CREATE OR REPLACE PROCEDURE fileRecalled(tapecopyId IN INTEGER) AS
 SubRequestId NUMBER;
 dci NUMBER;
 fsId NUMBER;
 fileSize NUMBER;
BEGIN
SELECT SubRequest.id, DiskCopy.id, SubRequest.xsize
 INTO SubRequestId, dci, fileSize
 FROM TapeCopy, SubRequest, DiskCopy
 WHERE TapeCopy.id = tapecopyId
  AND DiskCopy.castorFile = TapeCopy.castorFile
  AND SubRequest.diskcopy = DiskCopy.id;
UPDATE DiskCopy SET status = 0 WHERE id = dci RETURNING fileSystem into fsid; -- DISKCOPY_STAGED
UPDATE SubRequest SET status = 1, lastModificationTime = getTime()
 WHERE id = SubRequestId; -- SUBREQUEST_RESTART
UPDATE SubRequest SET status = 1, lastModificationTime = getTime()
 WHERE parent = SubRequestId; -- SUBREQUEST_RESTART
updateFsFileClosed(fsId, fileSize, fileSize);
END;

/* PL/SQL method implementing castor package */
CREATE OR REPLACE PACKAGE castor AS
  TYPE DiskCopyCore IS RECORD (id INTEGER, path VARCHAR2(2048), status NUMBER, fsWeight NUMBER, mountPoint VARCHAR2(2048), diskServer VARCHAR2(2048));
  TYPE DiskCopy_Cur IS REF CURSOR RETURN DiskCopyCore;
  TYPE Segment_Cur IS REF CURSOR RETURN Segment%ROWTYPE;
  TYPE GCLocalFileCore IS RECORD (fileName VARCHAR2(2048), diskCopyId INTEGER);
  TYPE GCLocalFiles_Cur IS REF CURSOR RETURN GCLocalFileCore;
  TYPE "strList" IS TABLE OF VARCHAR2(2048) index by binary_integer;
  TYPE "cnumList" IS TABLE OF NUMBER index by binary_integer;
END castor;
CREATE OR REPLACE TYPE "numList" IS TABLE OF INTEGER;

/* PL/SQL method implementing isSubRequestToSchedule */
CREATE OR REPLACE PROCEDURE isSubRequestToSchedule
        (rsubreqId IN INTEGER, result OUT INTEGER,
         sources OUT castor.DiskCopy_Cur) AS
  stat INTEGER;
  dci INTEGER;
BEGIN
 SELECT DiskCopy.status, DiskCopy.id
  INTO stat, dci
  FROM DiskCopy, SubRequest, FileSystem, DiskServer
  WHERE SubRequest.id = rsubreqId
   AND SubRequest.castorfile = DiskCopy.castorfile
   AND DiskCopy.fileSystem = FileSystem.id
   AND FileSystem.status = 0 -- PRODUCTION
   AND FileSystem.diskserver = DiskServer.id
   AND DiskServer.status = 0 -- PRODUCTION
   AND DiskCopy.status IN (0, 1, 2, 5, 6, 10) -- STAGED, WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, STAGEOUT, CANBEMIGR
   AND ROWNUM < 2;
 IF stat IN (1, 2, 5) -- DISKCCOPY_WAIT*
 THEN
  -- Only DiskCopy, make SubRequest wait on the recalling one and do not schedule
  update SubRequest SET parent = (SELECT id FROM SubRequest where diskCopy = dci),
                        lastModificationTime = getTime() WHERE id = rsubreqId;
  result := 0;  -- no nschedule
 ELSE
  result := 1;  -- schedule and diskcopies available
  OPEN sources
    FOR SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status,
               FileSystem.weight, FileSystem.mountPoint,
               DiskServer.name
    FROM DiskCopy, SubRequest, FileSystem, DiskServer
    WHERE SubRequest.id = rsubreqId
      AND SubRequest.castorfile = DiskCopy.castorfile
      AND DiskCopy.status IN (0, 6, 10) -- STAGED, STAGEOUT, CANBEMIGR
      AND FileSystem.id = DiskCopy.fileSystem
      AND FileSystem.status = 0 -- PRODUCTION
      AND DiskServer.id = FileSystem.diskServer
      AND DiskServer.status = 0; -- PRODUCTION
 END IF;
EXCEPTION
 WHEN NO_DATA_FOUND -- In this case, schedule for recall
 THEN result := 2;  -- schedule and no diskcopies
END;

/* Build diskCopy path from fileId */
CREATE OR REPLACE PROCEDURE buildPathFromFileId(fid IN INTEGER,
                                                nsHost IN VARCHAR2,
                                                path OUT VARCHAR2) AS
BEGIN
  path := CONCAT(CONCAT(CONCAT(TO_CHAR(MOD(fid,100),'FM09'), '/'),
                        CONCAT(TO_CHAR(fid), '@')),
                 nsHost);
END;

/* PL/SQL method implementing getUpdateStart */
CREATE OR REPLACE PROCEDURE getUpdateStart
        (srId IN INTEGER, fileSystemId IN INTEGER,
         dci OUT INTEGER, rpath OUT VARCHAR2,
         rstatus OUT NUMBER, sources OUT castor.DiskCopy_Cur,
         reuid OUT INTEGER, regid OUT INTEGER) AS
  cfid INTEGER;
  fid INTEGER;
  nh VARCHAR2(2048);
  unused CastorFile%ROWTYPE;
BEGIN
 -- Get and uid, gid
 SELECT euid, egid INTO reuid, regid FROM SubRequest,
      (SELECT id, euid, egid from StageGetRequest UNION
       SELECT id, euid, egid from StagePrepareToGetRequest UNION
       SELECT id, euid, egid from StageUpdateRequest UNION
       SELECT id, euid, egid from StagePrepareToUpdateRequest) Request
  WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
 -- Take a lock on the CastorFile. Associated with triggers,
 -- this guarantee we are the only ones dealing with its copies
 SELECT CastorFile.* INTO unused FROM CastorFile, SubRequest
  WHERE CastorFile.id = SubRequest.castorFile
    AND SubRequest.id = srId FOR UPDATE;
 -- Try to find local DiskCopy
 dci := 0;
 SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status
  INTO dci, rpath, rstatus
  FROM DiskCopy, SubRequest
  WHERE SubRequest.id = srId
    AND SubRequest.castorfile = DiskCopy.castorfile
    AND DiskCopy.filesystem = fileSystemId
    AND DiskCopy.status IN (0, 1, 2, 5, 6, 10); -- STAGED, WAITDISKTODISKCOPY, WAITTAPERECALL, WAIFS, STAGEOUT, CANBEMIGR
 -- If found local one, check whether to wait on it
 IF rstatus IN (1, 2, 5) THEN -- WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, Make SubRequest Wait
   makeSubRequestWait(srId, dci);
   dci := 0;
   rpath := '';
 END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN -- No disk copy found on selected FileSystem, look in others
 BEGIN
  -- Try to find remote DiskCopies
  SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status
  INTO dci, rpath, rstatus
  FROM DiskCopy, SubRequest, FileSystem, DiskServer
  WHERE SubRequest.id = srId
    AND SubRequest.castorfile = DiskCopy.castorfile
    AND DiskCopy.status IN (0, 1, 2, 5, 6, 10) -- STAGED, WAITDISKTODISKCOPY, WAITTAPERECALL, WAIFS, STAGEOUT, CANBEMIGR
    AND FileSystem.id = DiskCopy.fileSystem
    AND FileSystem.status = 0 -- PRODUCTION
    AND DiskServer.id = FileSystem.diskserver
    AND DiskServer.status = 0 -- PRODUCTION
    AND ROWNUM < 2;
  -- Found a DiskCopy, Check whether to wait on it
  IF rstatus IN (2,5) THEN -- WAITTAPERECALL, WAITFS, Make SubRequest Wait
    makeSubRequestWait(srId, dci);
    dci := 0;
    rpath := '';
    close sources;
  ELSE
    OPEN sources
    FOR SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status,
               FileSystem.weight, FileSystem.mountPoint,
               DiskServer.name
    FROM DiskCopy, SubRequest, FileSystem, DiskServer
    WHERE SubRequest.id = srId
      AND SubRequest.castorfile = DiskCopy.castorfile
      AND DiskCopy.status IN (0, 1, 2, 5, 6, 10) -- STAGED, WAITDISKTODISKCOPY, WAITTAPERECALL, WAIFS, STAGEOUT, CANBEMIGR
      AND FileSystem.id = DiskCopy.fileSystem
      AND FileSystem.status = 0 -- PRODUCTION
      AND DiskServer.id = FileSystem.diskServer
      AND DiskServer.status = 0; -- PRODUCTION
    -- create DiskCopy for Disk to Disk copy
    UPDATE SubRequest SET diskCopy = ids_seq.nextval,
                          lastModificationTime = getTime() WHERE id = srId
     RETURNING castorFile, diskCopy INTO cfid, dci;
    INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status, creationTime)
     VALUES (rpath, dci, fileSystemId, cfid, 1, getTime()); -- status WAITDISK2DISKCOPY
    INSERT INTO Id2Type (id, type) VALUES (dci, 5); -- OBJ_DiskCopy
    rstatus := 1; -- status WAITDISK2DISKCOPY
  END IF;
 EXCEPTION WHEN NO_DATA_FOUND THEN -- No disk copy found on any FileSystem
  -- create one for recall
  UPDATE SubRequest SET diskCopy = ids_seq.nextval, status = 4,
                        lastModificationTime = getTime() -- WAITTAPERECALL
   WHERE id = srId RETURNING castorFile, diskCopy INTO cfid, dci;
  SELECT fileId, nsHost INTO fid, nh FROM CastorFile WHERE id = cfid;
  buildPathFromFileId(fid, nh, rpath);
  INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status, creationTime)
   VALUES (rpath, dci, fileSystemId, cfid, 2, getTime()); -- status WAITTAPERECALL
  INSERT INTO Id2Type (id, type) VALUES (dci, 5); -- OBJ_DiskCopy
  rstatus := 99; -- WAITTAPERECALL, NEWLY CREATED
 END;
END;

/* PL/SQL method implementing putStart */
CREATE OR REPLACE PROCEDURE putStart
        (srId IN INTEGER, fileSystemId IN INTEGER,
         rdcId OUT INTEGER, rdcStatus OUT INTEGER,
         rdcPath OUT VARCHAR2) AS
BEGIN
 -- Get diskCopy Id
 SELECT diskCopy INTO rdcId FROM SubRequest WHERE SubRequest.id = srId;
 -- link DiskCopy and FileSystem and update DiskCopyStatus
 UPDATE DiskCopy SET status = 6, -- DISKCOPY_STAGEOUT
                     fileSystem = fileSystemId
  WHERE id = rdcId
  RETURNING status, path
  INTO rdcStatus, rdcPath;
EXCEPTION WHEN NO_DATA_FOUND THEN -- No data found means we were last
  NULL;
END;

/* PL/SQL method implementing updateAndCheckSubRequest */
CREATE OR REPLACE PROCEDURE updateAndCheckSubRequest(srId IN INTEGER, newStatus IN INTEGER, result OUT INTEGER) AS
  reqId INTEGER;
BEGIN
 -- Lock the access to the Request
 SELECT Id2Type.id INTO reqId
  FROM SubRequest, Id2Type
  WHERE SubRequest.id = srId
  AND Id2Type.id = SubRequest.request
  FOR UPDATE;
 -- Update Status
 UPDATE SubRequest SET status = newStatus,
                       lastModificationTime = getTime() WHERE id = srId;
 -- Check whether it was the last subrequest in the request
 SELECT id INTO result FROM SubRequest
  WHERE request = reqId
    AND status NOT IN (6, 7) -- SUBREQUEST_READY, SUBREQUEST_FAILED
    AND ROWNUM < 2;
EXCEPTION WHEN NO_DATA_FOUND THEN -- No data found means we were last
  result := 0;
END;

/* PL/SQL method implementing disk2DiskCopyDone */
CREATE OR REPLACE PROCEDURE disk2DiskCopyDone
(dcId IN INTEGER, dcStatus IN INTEGER) AS
BEGIN
  -- update DiskCopy
  UPDATE DiskCopy set status = dcStatus WHERE id = dcId; -- status DISKCOPY_STAGED
  -- update SubRequest
  UPDATE SubRequest set status = 6,
                        lastModificationTime = getTime()
   WHERE diskCopy = dcId; -- status SUBREQUEST_READY
END;

/* PL/SQL method implementing recreateCastorFile */
CREATE OR REPLACE PROCEDURE recreateCastorFile(cfId IN INTEGER,
                                               srId IN INTEGER,
                                               dcId OUT INTEGER) AS
  rpath VARCHAR2(2048);
  nbRes INTEGER;
  fid INTEGER;
  nh VARCHAR2(2048);
  unused CastorFile%ROWTYPE;
BEGIN
 -- Lock the access to the CastorFile
 -- This, together with triggers will avoid new TapeCopies
 -- or DiskCopies to be added
 SELECT * INTO unused FROM CastorFile WHERE id = cfId FOR UPDATE;
 -- check if recreation is possible for TapeCopies
 SELECT count(*) INTO nbRes FROM TapeCopy
   WHERE status = 3 -- TAPECOPY_SELECTED
   AND castorFile = cfId;
 IF nbRes > 0 THEN
   -- We found something, thus we cannot recreate
   dcId := 0;
   COMMIT;
   RETURN;
 END IF;
 -- check if recreation is possible for DiskCopies
 SELECT count(*) INTO nbRes FROM DiskCopy
   WHERE status IN (1, 2, 5, 6) -- WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, STAGEOUT
   AND castorFile = cfId;
 IF nbRes > 0 THEN
   -- We found something, thus we cannot recreate
   dcId := 0;
   COMMIT;
   RETURN;
 END IF;
 -- delete all tapeCopies
 DELETE from TapeCopy WHERE castorFile = cfId;
 -- set DiskCopies to INVALID
 UPDATE DiskCopy SET status = 7 -- INVALID
  WHERE castorFile = cfId AND status = 1; -- STAGED
 -- create new DiskCopy
 SELECT fileId, nsHost INTO fid, nh FROM CastorFile WHERE id = cfId;
 buildPathFromFileId(fid, nh, rpath);
 INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status, creationTime)
  VALUES (rpath, ids_seq.nextval, 0, cfId, 5, getTime()) -- status WAITFS
  RETURNING id INTO dcId;
 INSERT INTO Id2Type (id, type) VALUES (dcId, 5); -- OBJ_DiskCopy
 COMMIT;
 -- link SubRequest and DiskCopy
 UPDATE SubRequest SET diskCopy = dcId,
                       lastModificationTime = getTime() WHERE id = srId;
 COMMIT;
END;

/* PL/SQL method implementing prepareForMigration */
CREATE OR REPLACE PROCEDURE prepareForMigration (srId IN INTEGER,
                                                 fs IN INTEGER,
                                                 fId OUT NUMBER,
                                                 nh OUT VARCHAR2,
                                                 userId OUT INTEGER,
                                                 groupId OUT INTEGER) AS
  nc INTEGER;
  cfId INTEGER;
  tcId INTEGER;
  fsId INTEGER;
  reservedSpace NUMBER;
BEGIN
 -- get CastorFile
 SELECT castorFile INTO cfId FROM SubRequest where id = srId;
 -- update CastorFile. This also takes a lock on it, insuring
 -- with triggers that we are the only ones to deal with its copies
 UPDATE CastorFile set fileSize = fs WHERE id = cfId
  RETURNING fileId, nsHost INTO fId, nh;
 -- get uid, gid and reserved space from Request
 SELECT euid, egid, xsize INTO userId, groupId, reservedSpace FROM SubRequest,
      (SELECT euid, egid, id from StagePutRequest UNION
       SELECT euid, egid, id from StagePrepareToPutRequest) Request
  WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
 -- update the DiskCopy status
 UPDATE DiskCopy SET status = 10 -- CANBEMIGR
  WHERE castorFile = cfid AND status = 6 -- STAGEOUT
  RETURNING fileSystem INTO fsId;
 -- update the FileSystem free space
 updateFsFileClosed(fsId, reservedSpace, fs);
 -- if 0 length file, stop here
 IF fs = 0 THEN
   COMMIT;
   RETURN;
 END IF;
 -- get number of copies to create
 SELECT nbCopies INTO nc FROM FileClass, CastorFile
  WHERE CastorFile.id = cfId AND CastorFile.fileClass = FileClass.id;
 -- Create TapeCopies
 <<TapeCopyCreation>>
 FOR i IN 1..nc LOOP
  INSERT INTO TapeCopy (id, copyNb, castorFile, status)
    VALUES (ids_seq.nextval, i, cfId, 0) -- TAPECOPY_CREATED
    RETURNING id INTO tcId;
  INSERT INTO Id2Type (id, type) VALUES (tcId, 30); -- OBJ_TapeCopy
 END LOOP TapeCopyCreation;
 -- archive Subrequest
 archiveSubReq(srId);
 COMMIT;
END;

/* PL/SQL method implementing selectCastorFile */
CREATE OR REPLACE PROCEDURE selectCastorFile (fId IN INTEGER,
                                              nh IN VARCHAR2,
                                              sc IN INTEGER,
                                              fc IN INTEGER,
                                              fs IN INTEGER,
                                              rid OUT INTEGER,
                                              rfs OUT INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
BEGIN
  BEGIN
    -- try to find an existing file
    SELECT id, fileSize INTO rid, rfs FROM CastorFile
      WHERE fileId = fid AND nsHost = nh;
    -- update lastAccess time
    UPDATE CastorFile SET LastAccessTime = getTime(), nbAccesses = nbAccesses + 1
      WHERE id = rid;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- insert new row
    INSERT INTO CastorFile (id, fileId, nsHost, svcClass, fileClass, fileSize,
                            creationTime, lastAccessTime, nbAccesses)
      VALUES (ids_seq.nextval, fId, nh, sc, fc, fs, getTime(), getTime(), 1)
      RETURNING id, fileSize INTO rid, rfs;
    INSERT INTO Id2Type (id, type) VALUES (rid, 2); -- OBJ_CastorFile
  END;
EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
  -- retry the select since a creation was done in between
  SELECT id, fileSize INTO rid, rfs FROM CastorFile
    WHERE fileId = fid AND nsHost = nh;
END;

/* PL/SQL method implementing resetStream */
CREATE OR REPLACE PROCEDURE resetStream (sid IN INTEGER) AS
  nbRes NUMBER;
  unused Stream%ROWTYPE;
BEGIN
  BEGIN
    -- First lock the stream
    SELECT * INTO unused from Stream where id = sid FOR UPDATE;
    -- Selecting any column with hint FIRST_ROW and relying
    -- on the exception mechanism in case nothing is found is
    -- far better than issuing a SELECT count(*) because ORACLE
    -- will then ignore the FIRST_ROWS and take ages...
    SELECT /*+ FIRST_ROWS */ Tapecopy.id INTO nbRes
      FROM Stream2TapeCopy, TapeCopy
      WHERE Stream2TapeCopy.Parent = sid
        AND Stream2TapeCopy.Child = TapeCopy.id
        AND status = 2 -- TAPECOPY_WAITINSTREAMS
        AND ROWNUM < 2;
    -- We'we found one, update stream status
    UPDATE Stream SET status = 0, tape = 0 WHERE id = sid; -- STREAM_PENDING
  EXCEPTION  WHEN NO_DATA_FOUND THEN
    -- We've found nothing, delete stream
    DELETE FROM Stream2TapeCopy WHERE Parent = sid;
    DELETE FROM Stream WHERE id = sid;
  END;
  -- in any case, unlink tape and stream
  UPDATE Tape SET Stream = 0 WHERE Stream = sid;
END;

/* PL/SQL method implementing bestFileSystemForJob */
CREATE OR REPLACE PROCEDURE bestFileSystemForJob
(fileSystems IN castor."strList", machines IN castor."strList",
 minFree IN castor."cnumList", rMountPoint OUT VARCHAR2,
 rDiskServer OUT VARCHAR2) AS
 ds NUMBER;
 fs NUMBER;
 dev NUMBER;
 TYPE cursorContent IS RECORD
   (mountPoint VARCHAR2(2048), dsName VARCHAR2(2048),
    dsId NUMBER, fsId NUMBER, deviation NUMBER);
 TYPE AnyCursor IS REF CURSOR RETURN cursorContent;
 c1 AnyCursor;
BEGIN
 IF fileSystems.COUNT > 0 THEN
  DECLARE
   fsIds "numList" := "numList"();
   nextIndex NUMBER := 1;
  BEGIN
   fsIds.EXTEND(fileSystems.COUNT);
   FOR i in fileSystems.FIRST .. fileSystems.LAST LOOP
    BEGIN
      SELECT FileSystem.id INTO fsIds(nextIndex)
        FROM FileSystem, DiskServer
       WHERE FileSystem.mountPoint = fileSystems(i)
         AND DiskServer.name = machines(i)
         AND FileSystem.diskServer = DiskServer.id
         AND minFree(i) <= FileSystem.free + FileSystem.deltaFree - FileSystem.reservedSpace
         AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
         AND FileSystem.status IN (0, 1); -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
      nextIndex := nextIndex + 1;
    EXCEPTION  WHEN NO_DATA_FOUND THEN
      NULL;
    END;  
   END LOOP;
   OPEN c1 FOR
    SELECT FileSystem.mountPoint, Diskserver.name,
           DiskServer.id, FileSystem.id, FileSystem.fsDeviation
    FROM FileSystem, DiskServer
    WHERE FileSystem.diskserver = DiskServer.id
      AND FileSystem.id MEMBER OF fsIds
    ORDER by FileSystem.weight + FileSystem.deltaWeight DESC,
             FileSystem.fsDeviation ASC;
  END;
 ELSE
  OPEN c1 FOR
   SELECT FileSystem.mountPoint, Diskserver.name,
          DiskServer.id, FileSystem.id, FileSystem.fsDeviation
   FROM FileSystem, DiskServer
   WHERE FileSystem.diskserver = DiskServer.id
    AND FileSystem.free + FileSystem.deltaFree - FileSystem.reservedSpace >= minFree(0)
    AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
    AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
   ORDER by FileSystem.weight + FileSystem.deltaWeight DESC,
            FileSystem.fsDeviation ASC;
 END IF;
 FETCH c1 INTO rMountPoint, rDiskServer, ds, fs, dev;
 CLOSE c1;
END;

/* PL/SQL method implementing anySegmentsForTape */
CREATE OR REPLACE PROCEDURE anySegmentsForTape
(tapeId IN INTEGER, nb OUT INTEGER) AS
BEGIN
  SELECT count(*) INTO nb FROM Segment
  WHERE Segment.tape = tapeId
    AND Segment.status = 0;
  IF nb > 0 THEN
    UPDATE Tape SET status = 3 -- WAITMOUNT
    WHERE id = tapeId;
  END IF;
END;

/* PL/SQL method implementing segmentsForTape */
CREATE OR REPLACE PROCEDURE segmentsForTape
(tapeId IN INTEGER, segments OUT castor.Segment_Cur) AS
  segs "numList";
BEGIN
  SELECT Segment.id BULK COLLECT INTO segs FROM Segment
   WHERE Segment.tape = tapeId
     AND Segment.status = 0 FOR UPDATE;
  IF segs.COUNT > 0 THEN
    UPDATE Tape SET status = 4 -- MOUNTED
     WHERE id = tapeId;
    UPDATE Segment set status = 7 -- SELECTED
     WHERE id MEMBER OF segs;
  END IF;
  OPEN segments FOR SELECT * FROM Segment 
                     where id MEMBER OF segs;
END;

/* PL/SQL method implementing selectFiles2Delete */
CREATE OR REPLACE PROCEDURE selectFiles2Delete
(DiskServerId IN NUMBER,
 GCLocalFiles OUT castor.GCLocalFiles_Cur) AS
  files "numList";
BEGIN
  
  SELECT DiskCopy.id BULK COLLECT INTO files
    FROM DiskCopy, FileSystem, DiskServer
   WHERE DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.DiskServer = DiskServer.id
     AND DiskServer.name = DiskServer
     AND DiskCopy.status = 8 -- GC_CANDIDATE
     FOR UPDATE;
  IF files.COUNT > 0 THEN
    UPDATE DiskCopy set status = 9 -- BEING_DELETED
     WHERE id MEMBER OF files;
  END IF;
  OPEN GCLocalFiles FOR
    SELECT FileSystem.mountPoint||DiskCopy.path, DiskCopy.id 
      FROM DiskCopy, FileSystem
     WHERE DiskCopy.fileSystem = FileSystem.id
       AND DiskCopy.id MEMBER OF files;
END;

/* PL/SQL method implementing filesDeleted */
CREATE OR REPLACE PROCEDURE filesDeleted
("numlist" IN fileIds) AS
  cfId NUMBER;
  nb NUMBER;
BEGIN
  -- Loop over the deleted files
  FOR dcid IN fileIds LOOP
    -- delete the DiskCopy
    DELETE FROM DiskCopy WHERE id = dcid
      RETURNING castorFile INTO cfId;
    -- Lock the Castor File
    SELECT * FROM CastorFile where id = cfID FOR UPDATE;
    -- See whether the castorfile has no other DiskCopy
    SELECT count(*) INTO nb FROM DiskCopy
      WHERE castorFile = cfId;
    -- If any DiskCopy, give up
    IF nb > 0 THEN CONTINUE; END IF;
    -- See whether the castorfile has any TapeCopy
    SELECT count(*) INTO nb FROM TapeCopy
      WHERE castorFile = cfId;
    -- If any TapeCopy, give up
    IF nb > 0 THEN CONTINUE; END IF;
    -- See whether the castorfile has any SubRequest
    SELECT count(*) INTO nb FROM SubRequest
      WHERE castorFile = cfId;
    -- If any SubRequest, give up
    IF nb > 0 THEN CONTINUE; END IF;
    -- Delete the CastorFile
    DELETE FROM CastorFile WHERE id = cfId;
  END LOOP;
END;

/* PL/SQL method implementing getUpdateDone */
CREATE OR REPLACE PROCEDURE getUpdateDone
(subReqId IN NUMBER) AS
BEGIN
  archiveSubReq(subReqId);
END;

/* PL/SQL method implementing getUpdateFailed */
CREATE OR REPLACE PROCEDURE getUpdateFailed
(subReqId IN NUMBER) AS
BEGIN
  UPDATE SubRequest SET status = 7 -- FAILED
   WHERE id = subReqId;
END;

/* PL/SQL method implementing segmentsForTape */
CREATE OR REPLACE PROCEDURE putFailed
(subReqId IN NUMBER) AS
BEGIN
  UPDATE SubRequest SET status = 7 -- FAILED
   WHERE id = subReqId;
END;

/* PL/SQL method implementing failedSegments */
CREATE OR REPLACE PROCEDURE failedSegments
(segments OUT castor.Segment_Cur) AS
BEGIN
  OPEN segments FOR SELECT * FROM Segment
                     WHERE Segment.status = 6; -- SEGMENT_FAILED
END;
