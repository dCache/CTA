/* This file contains SQL code that is not generated automatically */
/* and is inserted at the beginning of the generated code           */

/* SQL statements for object types */
DROP INDEX I_ID2TYPE_FULL;
DROP TABLE ID2TYPE;
CREATE TABLE ID2TYPE (id INTEGER PRIMARY KEY, type NUMBER);
CREATE INDEX I_ID2TYPE_FULL on ID2TYPE (id, type);

/* Sequence for indices */
DROP SEQUENCE ids_seq;
CREATE SEQUENCE ids_seq;

/* SQL statements for requests status */
DROP INDEX I_REQUESTSSTATUS_FULL;
DROP TABLE REQUESTSSTATUS;
CREATE TABLE REQUESTSSTATUS (id INTEGER PRIMARY KEY, status CHAR(20), creation DATE, lastChange DATE);
CREATE INDEX I_REQUESTSSTATUS_FULL on REQUESTSSTATUS (id, status);

/* PL/SQL procedure for getting the next request to handle */
CREATE OR REPLACE PROCEDURE getNRStatement(reqid OUT INTEGER) AS
BEGIN
  SELECT ID INTO reqid FROM requestsStatus WHERE status = 'NEW' AND rownum <=1;
  UPDATE requestsStatus SET status = 'RUNNING', lastChange = SYSDATE WHERE ID = reqid;
END;

ALTER TABLE SvcClass2TapePool
  DROP CONSTRAINT fk_SvcClass2TapePool_Parent
  DROP CONSTRAINT fk_SvcClass2TapePool_Child;
ALTER TABLE DiskPool2SvcClass
  DROP CONSTRAINT fk_DiskPool2SvcClass_Parent
  DROP CONSTRAINT fk_DiskPool2SvcClass_Child;
ALTER TABLE Stream2TapeCopy
  DROP CONSTRAINT fk_Stream2TapeCopy_Parent
  DROP CONSTRAINT fk_Stream2TapeCopy_Child;
/* SQL statements for type BaseAddress */
DROP TABLE BaseAddress;
CREATE TABLE BaseAddress (objType NUMBER, cnvSvcName VARCHAR(2048), cnvSvcType NUMBER, target INTEGER, id INTEGER PRIMARY KEY);

/* SQL statements for type Client */
DROP TABLE Client;
CREATE TABLE Client (ipAddress NUMBER, port NUMBER, id INTEGER PRIMARY KEY);

/* SQL statements for type Disk2DiskCopyDoneRequest */
DROP TABLE Disk2DiskCopyDoneRequest;
CREATE TABLE Disk2DiskCopyDoneRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), diskCopyId INTEGER, id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type MoverCloseRequest */
DROP TABLE MoverCloseRequest;
CREATE TABLE MoverCloseRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), subReqId INTEGER, fileSize INTEGER, id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type PutStartRequest */
DROP TABLE PutStartRequest;
CREATE TABLE PutStartRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), subreqId INTEGER, diskServer VARCHAR(2048), fileSystem VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type GetUpdateStartRequest */
DROP TABLE GetUpdateStartRequest;
CREATE TABLE GetUpdateStartRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), subreqId INTEGER, diskServer VARCHAR(2048), fileSystem VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type QueryParameter */
DROP TABLE QueryParameter;
CREATE TABLE QueryParameter (value VARCHAR(2048), id INTEGER PRIMARY KEY, queryType INTEGER);

/* SQL statements for type StagePrepareToGetRequest */
DROP TABLE StagePrepareToGetRequest;
CREATE TABLE StagePrepareToGetRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StagePrepareToPutRequest */
DROP TABLE StagePrepareToPutRequest;
CREATE TABLE StagePrepareToPutRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StagePrepareToUpdateRequest */
DROP TABLE StagePrepareToUpdateRequest;
CREATE TABLE StagePrepareToUpdateRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageGetRequest */
DROP TABLE StageGetRequest;
CREATE TABLE StageGetRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StagePutRequest */
DROP TABLE StagePutRequest;
CREATE TABLE StagePutRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageUpdateRequest */
DROP TABLE StageUpdateRequest;
CREATE TABLE StageUpdateRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageRmRequest */
DROP TABLE StageRmRequest;
CREATE TABLE StageRmRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StagePutDoneRequest */
DROP TABLE StagePutDoneRequest;
CREATE TABLE StagePutDoneRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageUpdateFileStatusRequest */
DROP TABLE StageUpdateFileStatusRequest;
CREATE TABLE StageUpdateFileStatusRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageFileQueryRequest */
DROP TABLE StageFileQueryRequest;
CREATE TABLE StageFileQueryRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), fileName VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageRequestQueryRequest */
DROP TABLE StageRequestQueryRequest;
CREATE TABLE StageRequestQueryRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageFindRequestRequest */
DROP TABLE StageFindRequestRequest;
CREATE TABLE StageFindRequestRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type SubRequest */
DROP TABLE SubRequest;
CREATE TABLE SubRequest (retryCounter NUMBER, fileName VARCHAR(2048), protocol VARCHAR(2048), xsize INTEGER, priority NUMBER, subreqId VARCHAR(2048), flags NUMBER, modeBits NUMBER, id INTEGER PRIMARY KEY, diskcopy INTEGER, castorFile INTEGER, parent INTEGER, status INTEGER, request INTEGER);

/* SQL statements for type StageReleaseFilesRequest */
DROP TABLE StageReleaseFilesRequest;
CREATE TABLE StageReleaseFilesRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageAbortRequest */
DROP TABLE StageAbortRequest;
CREATE TABLE StageAbortRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageGetNextRequest */
DROP TABLE StageGetNextRequest;
CREATE TABLE StageGetNextRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER);

/* SQL statements for type StagePutNextRequest */
DROP TABLE StagePutNextRequest;
CREATE TABLE StagePutNextRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER);

/* SQL statements for type StageUpdateNextRequest */
DROP TABLE StageUpdateNextRequest;
CREATE TABLE StageUpdateNextRequest (flags INTEGER, userName VARCHAR(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR(2048), svcClassName VARCHAR(2048), userTag VARCHAR(2048), reqId VARCHAR(2048), id INTEGER PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER);

/* SQL statements for type Tape */
DROP TABLE Tape;
CREATE TABLE Tape (vid VARCHAR(2048), side NUMBER, tpmode NUMBER, errMsgTxt VARCHAR(2048), errorCode NUMBER, severity NUMBER, vwAddress VARCHAR(2048), id INTEGER PRIMARY KEY, stream INTEGER, status INTEGER);

/* SQL statements for type Segment */
DROP TABLE Segment;
CREATE TABLE Segment (fseq NUMBER, offset INTEGER, bytes_in INTEGER, bytes_out INTEGER, host_bytes INTEGER, segmCksumAlgorithm VARCHAR(2048), segmCksum NUMBER, errMsgTxt VARCHAR(2048), errorCode NUMBER, severity NUMBER, blockId0 INTEGER, blockId1 INTEGER, blockId2 INTEGER, blockId3 INTEGER, id INTEGER PRIMARY KEY, tape INTEGER, copy INTEGER, status INTEGER);

/* SQL statements for type TapePool */
DROP TABLE TapePool;
CREATE TABLE TapePool (name VARCHAR(2048), id INTEGER PRIMARY KEY);

/* SQL statements for type TapeCopy */
DROP TABLE TapeCopy;
CREATE TABLE TapeCopy (copyNb NUMBER, id INTEGER PRIMARY KEY, castorFile INTEGER, status INTEGER);

/* SQL statements for type CastorFile */
DROP TABLE CastorFile;
CREATE TABLE CastorFile (fileId INTEGER, nsHost VARCHAR(2048), fileSize INTEGER, id INTEGER PRIMARY KEY, svcClass INTEGER, fileClass INTEGER);

/* SQL statements for type DiskCopy */
DROP TABLE DiskCopy;
CREATE TABLE DiskCopy (path VARCHAR(2048), gcWeight float, id INTEGER PRIMARY KEY, fileSystem INTEGER, castorFile INTEGER, status INTEGER);

/* SQL statements for type FileSystem */
DROP TABLE FileSystem;
CREATE TABLE FileSystem (free INTEGER, weight float, fsDeviation float, mountPoint VARCHAR(2048), deltaWeight float, id INTEGER PRIMARY KEY, diskPool INTEGER, diskserver INTEGER, status INTEGER);

/* SQL statements for type SvcClass */
DROP TABLE SvcClass;
CREATE TABLE SvcClass (nbDrives NUMBER, name VARCHAR(2048), defaultFileSize INTEGER, maxReplicaNb NUMBER, replicationPolicy VARCHAR(2048), gcPolicy VARCHAR(2048), migratorPolicy VARCHAR(2048), recallerPolicy VARCHAR(2048), id INTEGER PRIMARY KEY);
DROP INDEX I_SvcClass2TapePool_Child;
DROP INDEX I_SvcClass2TapePool_Parent;
DROP TABLE SvcClass2TapePool;
CREATE TABLE SvcClass2TapePool (Parent INTEGER, Child INTEGER);
CREATE INDEX I_SvcClass2TapePool_Child on SvcClass2TapePool (child);
CREATE INDEX I_SvcClass2TapePool_Parent on SvcClass2TapePool (parent);

/* SQL statements for type DiskPool */
DROP TABLE DiskPool;
CREATE TABLE DiskPool (name VARCHAR(2048), id INTEGER PRIMARY KEY);
DROP INDEX I_DiskPool2SvcClass_Child;
DROP INDEX I_DiskPool2SvcClass_Parent;
DROP TABLE DiskPool2SvcClass;
CREATE TABLE DiskPool2SvcClass (Parent INTEGER, Child INTEGER);
CREATE INDEX I_DiskPool2SvcClass_Child on DiskPool2SvcClass (child);
CREATE INDEX I_DiskPool2SvcClass_Parent on DiskPool2SvcClass (parent);

/* SQL statements for type Stream */
DROP TABLE Stream;
CREATE TABLE Stream (initialSizeToTransfer INTEGER, id INTEGER PRIMARY KEY, tape INTEGER, tapePool INTEGER, status INTEGER);
DROP INDEX I_Stream2TapeCopy_Child;
DROP INDEX I_Stream2TapeCopy_Parent;
DROP TABLE Stream2TapeCopy;
CREATE TABLE Stream2TapeCopy (Parent INTEGER, Child INTEGER);
CREATE INDEX I_Stream2TapeCopy_Child on Stream2TapeCopy (child);
CREATE INDEX I_Stream2TapeCopy_Parent on Stream2TapeCopy (parent);

/* SQL statements for type FileClass */
DROP TABLE FileClass;
CREATE TABLE FileClass (name VARCHAR(2048), minFileSize INTEGER, maxFileSize INTEGER, nbCopies NUMBER, id INTEGER PRIMARY KEY);

/* SQL statements for type DiskServer */
DROP TABLE DiskServer;
CREATE TABLE DiskServer (name VARCHAR(2048), id INTEGER PRIMARY KEY, status INTEGER);

ALTER TABLE SvcClass2TapePool
  ADD CONSTRAINT fk_SvcClass2TapePool_Parent FOREIGN KEY (Parent) REFERENCES SvcClass (id)
  ADD CONSTRAINT fk_SvcClass2TapePool_Child FOREIGN KEY (Child) REFERENCES TapePool (id);
ALTER TABLE DiskPool2SvcClass
  ADD CONSTRAINT fk_DiskPool2SvcClass_Parent FOREIGN KEY (Parent) REFERENCES DiskPool (id)
  ADD CONSTRAINT fk_DiskPool2SvcClass_Child FOREIGN KEY (Child) REFERENCES SvcClass (id);
ALTER TABLE Stream2TapeCopy
  ADD CONSTRAINT fk_Stream2TapeCopy_Parent FOREIGN KEY (Parent) REFERENCES Stream (id)
  ADD CONSTRAINT fk_Stream2TapeCopy_Child FOREIGN KEY (Child) REFERENCES TapeCopy (id);
/* This file contains SQL code that is not generated automatically */
/* and is inserted at the end of the generated code           */

/* A small table used to cross check code and DB versions */
DROP TABLE CastorVersion;
CREATE TABLE CastorVersion (version VARCHAR(2048));
INSERT INTO CastorVersion VALUES ('2.0.0.0');

/* Indexes related to CastorFiles */
CREATE UNIQUE INDEX I_DiskServer_name on DiskServer (name);
CREATE UNIQUE INDEX I_CastorFile_fileIdNsHost on CastorFile (fileId, nsHost);
CREATE INDEX I_DiskCopy_Castorfile on DiskCopy (castorFile);
CREATE INDEX I_TapeCopy_Castorfile on TapeCopy (castorFile);
CREATE INDEX I_SubRequest_Castorfile on SubRequest (castorFile);
CREATE INDEX I_FileSystem_DiskPool on FileSystem (diskPool);
CREATE INDEX I_SubRequest_DiskCopy on SubRequest (diskCopy);

/* Constraint on FileClass name */
ALTER TABLE FileClass ADD UNIQUE (name); 

/* Add unique constraint on castorFiles */
ALTER TABLE CastorFile ADD UNIQUE (fileId, nsHost); 

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
BEFORE INSERT ON Stream2TapeCopy
FOR EACH ROW
DECLARE
  unused Stream%ROWTYPE;
BEGIN
  SELECT * INTO unused FROM Stream
   WHERE id = :new.Parent FOR UPDATE;
END;

/* PL/SQL method to make a SubRequest wait on another one, linked to the given DiskCopy */
CREATE OR REPLACE PROCEDURE makeSubRequestWait(subreqId IN INTEGER, dci IN INTEGER) AS
BEGIN
 UPDATE SubRequest
  SET parent = (SELECT id FROM SubRequest WHERE diskCopy = dci), status = 5 -- WAITSUBREQ
  WHERE SubRequest.id = subreqId;
END;

/* PL/SQL method implementing anyTapeCopyForStream */
CREATE OR REPLACE PROCEDURE anyTapeCopyForStream(streamId IN INTEGER, res OUT INTEGER) AS
  unused INTEGER;
BEGIN
  SELECT TapeCopy.id INTO unused
  FROM DiskServer, FileSystem, DiskCopy, CastorFile, TapeCopy, Stream2TapeCopy
   WHERE DiskServer.id = FileSystem.diskserver
    AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
    AND FileSystem.id = DiskCopy.filesystem
    AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
    AND DiskCopy.castorfile = CastorFile.id
    AND TapeCopy.castorfile = Castorfile.id
    AND Stream2TapeCopy.child = TapeCopy.id
    AND Stream2TapeCopy.parent = streamId
    AND TapeCopy.status = 2 -- WAITInSTREAMS
    AND ROWNUM < 2;
  res := 1;
EXCEPTION
 WHEN NO_DATA_FOUND THEN
  res := 0;
END;

/* PL/SQL method to update FileSystem weight for new streams */
CREATE OR REPLACE PROCEDURE updateFsWeight
(ds IN INTEGER, fs IN INTEGER, deviation IN INTEGER) AS
BEGIN
 UPDATE FileSystem SET deltaWeight = deltaWeight - deviation
  WHERE diskServer = ds;
 UPDATE FileSystem SET fsDeviation = 2 * deviation
  WHERE id = fs;
END;

/* PL/SQL method implementing bestTapeCopyForStream */
CREATE OR REPLACE PROCEDURE bestTapeCopyForStream(streamId IN INTEGER,
                                                  diskServerName OUT VARCHAR, mountPoint OUT VARCHAR,
                                                  path OUT VARCHAR, dci OUT INTEGER,
                                                  castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                  nsHost OUT VARCHAR, fileSize OUT INTEGER,
                                                  tapeCopyId OUT INTEGER) AS
 fileSystemId INTEGER;
 deviation NUMBER;
 fsDiskServer NUMBER;
 CURSOR c1 IS SELECT DiskServer.name, FileSystem.mountPoint, FileSystem.fsDeviation, FileSystem.diskserver, DiskCopy.path, DiskCopy.id, FileSystem.id,
   CastorFile.id, CastorFile.fileId, CastorFile.nsHost, CastorFile.fileSize, TapeCopy.id
   FROM DiskServer, FileSystem, DiskCopy, CastorFile, TapeCopy, Stream2TapeCopy
   WHERE DiskServer.id = FileSystem.diskserver
    AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
    AND FileSystem.id = DiskCopy.filesystem
    AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
    AND DiskCopy.castorfile = CastorFile.id
    AND TapeCopy.castorfile = Castorfile.id
    AND Stream2TapeCopy.child = TapeCopy.id
    AND Stream2TapeCopy.parent = streamId
    AND TapeCopy.status = 2 -- WAITINSTREAMS
   ORDER by FileSystem.weight + FileSystem.deltaWeight DESC,
            FileSystem.fsDeviation ASC;
BEGIN
 OPEN c1;
 FETCH c1 INTO diskServerName, mountPoint, deviation, fsDiskServer, path, dci, fileSystemId, castorFileId, fileId, nsHost, fileSize, tapeCopyId;
 CLOSE c1;
 UPDATE TapeCopy SET status = 3 -- SELECTED
  WHERE id = tapeCopyId;
 UPDATE Stream SET status = 3 -- RUNNING
  WHERE id = streamId;
 updateFsWeight(fsDiskServer, fileSystemId, deviation);
END;

/* PL/SQL method implementing bestFileSystemForSegment */
CREATE OR REPLACE PROCEDURE bestFileSystemForSegment(segmentId IN INTEGER, diskServerName OUT VARCHAR,
                                                     rmountPoint OUT VARCHAR, rpath OUT VARCHAR,
                                                     dci OUT INTEGER) AS
 fileSystemId NUMBER;
 deviation NUMBER;
 fsDiskServer NUMBER;
 CURSOR c1 IS SELECT DiskServer.name, FileSystem.mountPoint, FileSystem.id, FileSystem.fsDeviation, FileSystem.diskserver, DiskCopy.path, DiskCopy.id
   FROM DiskServer, FileSystem, DiskPool2SvcClass,
      (SELECT id, svcClass from StageGetRequest UNION
       SELECT id, svcClass from StagePrepareToGetRequest UNION
       SELECT id, svcClass from StageGetNextRequest UNION
       SELECT id, svcClass from StageUpdateRequest UNION
       SELECT id, svcClass from StagePrepareToUpdateRequest UNION
       SELECT id, svcClass from StageUpdateNextRequest) Request,
      SubRequest, TapeCopy, Segment, DiskCopy, CastorFile
    WHERE Segment.id = segmentId
     AND Segment.copy = TapeCopy.id
     AND SubRequest.castorfile = TapeCopy.castorfile
     AND DiskCopy.castorfile = TapeCopy.castorfile
     AND Castorfile.id = TapeCopy.castorfile
     AND Request.id = SubRequest.request
     AND Request.svcclass = DiskPool2SvcClass.child
     AND FileSystem.diskpool = DiskPool2SvcClass.parent
     AND FileSystem.free > CastorFile.fileSize
     AND FileSystem.status = 0 -- FILESYSTEM_PRODUCTION
     AND DiskServer.id = FileSystem.diskServer
     AND DiskServer.status = 0 -- DISKSERVER_PRODUCTION
   ORDER by FileSystem.weight + FileSystem.deltaWeight DESC,
            FileSystem.fsDeviation ASC;
BEGIN
 OPEN c1;
 FETCH c1 INTO diskServerName, rmountPoint, fileSystemId, deviation, fsDiskServer, rpath, dci;
 CLOSE c1;
 UPDATE DiskCopy SET fileSystem = fileSystemId WHERE id = dci;
 updateFsWeight(fsDiskServer, fileSystemId, deviation);
END;

/* PL/SQL method implementing fileRecalled */
CREATE OR REPLACE PROCEDURE fileRecalled(tapecopyId IN INTEGER) AS
 SubRequestId NUMBER;
 dci NUMBER;
BEGIN
SELECT SubRequest.id, DiskCopy.id
 INTO SubRequestId, dci
 FROM TapeCopy, SubRequest, DiskCopy
 WHERE TapeCopy.id = tapecopyId
  AND DiskCopy.castorFile = TapeCopy.castorFile
  AND SubRequest.diskcopy = DiskCopy.id;
UPDATE DiskCopy SET status = 0 WHERE id = dci; -- DISKCOPY_STAGED
UPDATE SubRequest SET status = 1 WHERE id = SubRequestId; -- SUBREQUEST_RESTART
UPDATE SubRequest SET status = 1 WHERE parent = SubRequestId; -- SUBREQUEST_RESTART
END;

/* PL/SQL method implementing castor package */
CREATE OR REPLACE PACKAGE castor AS
  TYPE DiskCopyCore IS RECORD (id INTEGER, path VARCHAR(2048), status NUMBER, fsWeight NUMBER, mountPoint VARCHAR(2048), diskServer VARCHAR(2048));
  TYPE DiskCopy_Cur IS REF CURSOR RETURN DiskCopyCore;
END castor;
CREATE OR REPLACE TYPE strList IS TABLE OF VARCHAR(2048);
CREATE OR REPLACE TYPE numList IS TABLE OF INTEGER;

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
   AND DiskCopy.status IN (0, 1, 2, 5, 6) -- STAGED, WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, STAGEOUT
   AND ROWNUM < 2;
 IF stat IN (1, 2, 5) -- DISKCCOPY_WAIT*
 THEN
  -- Only DiskCopy, make SubRequest wait on the recalling one and do not schedule
  update SubRequest SET parent = (SELECT id FROM SubRequest where diskCopy = dci) WHERE id = rsubreqId;
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
      AND DiskCopy.status IN (0, 6) -- STAGED, STAGEOUT
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
                                                nsHost IN VARCHAR,
                                                path OUT VARCHAR) AS
BEGIN
  path := CONCAT(CONCAT(CONCAT(TO_CHAR(MOD(fid,100),'FM09'), '/'),
                        CONCAT(TO_CHAR(fid), '@')),
                 nsHost);
END;

/* PL/SQL method implementing getUpdateStart */
CREATE OR REPLACE PROCEDURE getUpdateStart
        (srId IN INTEGER, fileSystemId IN INTEGER,
         dci OUT INTEGER, rpath OUT VARCHAR,
         rstatus OUT NUMBER, sources OUT castor.DiskCopy_Cur,
         reuid OUT INTEGER, regid OUT INTEGER) AS
  cfid INTEGER;
  fid INTEGER;
  nh VARCHAR(2048);
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
    AND DiskCopy.status IN (0, 1, 2, 5, 6); -- STAGED, WAITDISKTODISKCOPY, WAITTAPERECALL, WAIFS, STAGEOUT
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
    AND DiskCopy.status IN (0, 1, 2, 5, 6) -- STAGED, WAITDISKTODISKCOPY, WAITTAPERECALL, WAIFS, STAGEOUT
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
      AND DiskCopy.status IN (0, 1, 2, 5, 6) -- STAGED, WAITDISKTODISKCOPY, WAITTAPERECALL, WAIFS, STAGEOUT
      AND FileSystem.id = DiskCopy.fileSystem
      AND FileSystem.status = 0 -- PRODUCTION
      AND DiskServer.id = FileSystem.diskServer
      AND DiskServer.status = 0; -- PRODUCTION
    -- create DiskCopy for Disk to Disk copy
    UPDATE SubRequest SET diskCopy = ids_seq.nextval WHERE id = srId
     RETURNING castorFile, diskCopy INTO cfid, dci;
    INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status)
     VALUES (rpath, dci, fileSystemId, cfid, 1); -- status WAITDISK2DISKCOPY
    INSERT INTO Id2Type (id, type) VALUES (dci, 5); -- OBJ_DiskCopy
    rstatus := 1; -- status WAITDISK2DISKCOPY
  END IF;
 EXCEPTION WHEN NO_DATA_FOUND THEN -- No disk copy found on any FileSystem
  -- create one for recall
  UPDATE SubRequest SET diskCopy = ids_seq.nextval, status = 4 -- WAITTAPERECALL
   WHERE id = srId RETURNING castorFile, diskCopy INTO cfid, dci;
  SELECT fileId, nsHost INTO fid, nh FROM CastorFile WHERE id = cfid;
  buildPathFromFileId(fid, nh, rpath);
  INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status)
   VALUES (rpath, dci, fileSystemId, cfid, 2); -- status WAITTAPERECALL
  INSERT INTO Id2Type (id, type) VALUES (dci, 5); -- OBJ_DiskCopy
  rstatus := 99; -- WAITTAPERECALL, NEWLY CREATED
 END;
END;

/* PL/SQL method implementing putStart */
CREATE OR REPLACE PROCEDURE putStart
        (srId IN INTEGER, fileSystemId IN INTEGER,
         rdcId OUT INTEGER, rdcStatus OUT INTEGER,
         rdcPath OUT VARCHAR) AS
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
 UPDATE SubRequest SET status = newStatus WHERE id = srId;
 -- Check whether it was the last subrequest in the request
 SELECT id INTO result FROM SubRequest
  WHERE request = reqId
    AND status NOT IN (6, 7) -- SUBREQUEST_READY, SUBREQUEST_FAILED
    AND ROWNUM < 2;
EXCEPTION WHEN NO_DATA_FOUND THEN -- No data found means we were last
  result := 0;
END;

/* PL/SQL method implementing disk2DiskCopyDone */
CREATE OR REPLACE PROCEDURE disk2DiskCopyDone(dcId IN INTEGER) AS
BEGIN
  -- update DiskCopy
  UPDATE DiskCopy set status = 0 WHERE id = dcId; -- status DISKCOPY_STAGED
  -- update SubRequest
  UPDATE SubRequest set status = 6 WHERE diskCopy = dcId; -- status SUBREQUEST_READY
END;

/* PL/SQL method implementing recreateCastorFile */
CREATE OR REPLACE PROCEDURE recreateCastorFile(cfId IN INTEGER,
                                               srId IN INTEGER,
                                               dcId OUT INTEGER) AS
  rpath VARCHAR(2048);
  nbRes INTEGER;
  fid INTEGER;
  nh VARCHAR(2048);
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
 INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status)
  VALUES (rpath, ids_seq.nextval, 0, cfId, 5) -- status WAITFS
  RETURNING id INTO dcId;
 INSERT INTO Id2Type (id, type) VALUES (dcId, 5); -- OBJ_DiskCopy
 COMMIT;
 -- link SubRequest and DiskCopy
 UPDATE SubRequest SET diskCopy = dcId WHERE id = srId;
 COMMIT;
END;

/* PL/SQL method implementing prepareForMigration */
CREATE OR REPLACE PROCEDURE prepareForMigration (srId IN INTEGER,
                                                 fs IN INTEGER,
                                                 fId OUT NUMBER,
                                                 nh OUT VARCHAR,
                                                 userId OUT INTEGER,
                                                 groupId OUT INTEGER) AS
  nc INTEGER;
  cfId INTEGER;
  tcId INTEGER;
BEGIN
 -- get CastorFile
 SELECT castorFile INTO cfId FROM SubRequest where id = srId;
 -- update CastorFile. This also takes a lock on it, insuring
 -- with triggers that we are the only ones to deal with its copies
 UPDATE CastorFile set fileSize = fs WHERE id = cfId
  RETURNING fileId, nsHost INTO fId, nh;
 -- get uid, gid from Request
 SELECT euid, egid INTO userId, groupId FROM SubRequest,
      (SELECT euid, egid, id from StagePutRequest UNION
       SELECT euid, egid, id from StagePrepareToPutRequest) Request
  WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
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
 COMMIT;
END;

/* PL/SQL method implementing selectCastorFile */
CREATE OR REPLACE PROCEDURE selectCastorFile (fId IN INTEGER,
                                              nh IN VARCHAR,
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
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- insert new row
    INSERT INTO CastorFile (id, fileId, nsHost, svcClass, fileClass, fileSize)
      VALUES (ids_seq.nextval, fId, nh, sc, fc, fs)
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
  -- Used (together with a trigger) to avoid 
  SELECT * INTO unused from Stream where id = sid FOR UPDATE;
  SELECT count(*) INTO nbRes
    FROM Stream2TapeCopy, TapeCopy
    WHERE Stream2TapeCopy.Parent = sid
      AND Stream2TapeCopy.Child = TapeCopy.id
      AND status = 2 -- TAPECOPY_WAITINSTREAMS
      AND ROWNUM < 2;
  IF nbRes > 0 THEN
    UPDATE Stream SET status = 0 WHERE id = sid; -- STREAM_PENDING
  ELSE
    DELETE FROM Stream2TapeCopy WHERE Parent = sid;
    DELETE FROM Stream WHERE id = sid;
    UPDATE Tape SET Stream = 0 WHERE Stream = sid;
  END IF;
END;

/* PL/SQL method implementing bestFileSystemForJob */
CREATE OR REPLACE PROCEDURE bestFileSystemForJob
(fileSystems IN strList, machines IN strList,
 minFree IN INTEGER, rMountPoint OUT VARCHAR,
 rDiskServer OUT VARCHAR) AS
 ds NUMBER;
 fs NUMBER;
 dev NUMBER;
 TYPE AnyCursor IS REF CURSOR;
 c1 castor.AnyCursor;
BEGIN
 IF fileSystems.COUNT > 0 THEN
  DECLARE
   fsIds numList := numList();
  BEGIN
   fsIds.EXTEND(fileSystems.COUNT);
   FOR i in fileSystems.FIRST .. fileSystems.LAST LOOP
    SELECT FileSystem.id INTO fsIds(i)
      FROM FileSystem, DiskServer
     WHERE FileSystem.mountPoint = fileSystems(i)
       AND DiskServer.name = machines(i);
   END LOOP;
   OPEN c1 FOR
    SELECT FileSystem.mountPoint, Diskserver.name,
           DiskServer.id, FileSystem.id, FileSystem.fsDeviation
    FROM FileSystem, DiskServer
    WHERE FileSystem.diskserver = DiskServer.id
     AND FileSystem.id MEMBER OF fsIds
     AND FileSystem.free >= minFree
     AND ROWNUM < 2
    ORDER by FileSystem.weight + FileSystem.deltaWeight DESC,
             FileSystem.fsDeviation ASC;
  END;
 ELSE
  OPEN c1 FOR
   SELECT FileSystem.mountPoint, Diskserver.name,
          DiskServer.id, FileSystem.id, FileSystem.fsDeviation
   FROM FileSystem, DiskServer
   WHERE FileSystem.diskserver = DiskServer.id
    AND FileSystem.free >= minFree
    AND ROWNUM < 2
   ORDER by FileSystem.weight + FileSystem.deltaWeight DESC,
            FileSystem.fsDeviation ASC;
 END IF;
 FETCH c1 INTO rMountPoint, rDiskServer, ds, fs, dev;
 CLOSE c1;
 updateFsWeight(ds, fs, dev);
END;
