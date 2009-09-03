/* Type2Obj metatable definition */
CREATE TABLE Type2Obj (type INTEGER CONSTRAINT PK_Type2Obj_Type PRIMARY KEY, object VARCHAR2(100) CONSTRAINT NN_Type2Obj_Object NOT NULL, svcHandler VARCHAR2(100));

/* ObjStatus metatable definition */
CREATE TABLE ObjStatus (object VARCHAR2(100) CONSTRAINT NN_ObjStatus_object NOT NULL, field VARCHAR2(100) CONSTRAINT NN_ObjStatus_field NOT NULL, statusCode INTEGER CONSTRAINT NN_ObjStatus_statusCode NOT NULL, statusName VARCHAR2(100) CONSTRAINT NN_ObjStatus_statusName NOT NULL, CONSTRAINT UN_ObjStatus_objectFieldCode UNIQUE (object, field, statusCode));

/* SQL statements for type BaseAddress */
CREATE TABLE BaseAddress (objType NUMBER, cnvSvcName VARCHAR2(2048), cnvSvcType NUMBER, target INTEGER, id INTEGER CONSTRAINT PK_BaseAddress_Id PRIMARY KEY) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type Client */
CREATE TABLE Client (ipAddress NUMBER, port NUMBER, version NUMBER, secure NUMBER, id INTEGER CONSTRAINT PK_Client_Id PRIMARY KEY) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type Disk2DiskCopyDoneRequest */
CREATE TABLE Disk2DiskCopyDoneRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, diskCopyId INTEGER, sourceDiskCopyId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_Disk2DiskCopyDoneRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type GetUpdateDone */
CREATE TABLE GetUpdateDone (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, subReqId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_GetUpdateDone_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type GetUpdateFailed */
CREATE TABLE GetUpdateFailed (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, subReqId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_GetUpdateFailed_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type PutFailed */
CREATE TABLE PutFailed (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, subReqId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_PutFailed_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type Files2Delete */
CREATE TABLE Files2Delete (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, diskServer VARCHAR2(2048), id INTEGER CONSTRAINT PK_Files2Delete_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type FilesDeleted */
CREATE TABLE FilesDeleted (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_FilesDeleted_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type FilesDeletionFailed */
CREATE TABLE FilesDeletionFailed (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_FilesDeletionFailed_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type GCFile */
CREATE TABLE GCFile (diskCopyId INTEGER, id INTEGER CONSTRAINT PK_GCFile_Id PRIMARY KEY, request INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type GCLocalFile */
CREATE TABLE GCLocalFile (fileName VARCHAR2(2048), diskCopyId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), lastAccessTime INTEGER, nbAccesses NUMBER, gcWeight NUMBER, gcTriggeredBy VARCHAR2(2048), svcClassName VARCHAR2(2048), id INTEGER CONSTRAINT PK_GCLocalFile_Id PRIMARY KEY) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type MoverCloseRequest */
CREATE TABLE MoverCloseRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, subReqId INTEGER, fileSize INTEGER, timeStamp INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), csumType VARCHAR2(2048), csumValue VARCHAR2(2048), id INTEGER CONSTRAINT PK_MoverCloseRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type PutStartRequest */
CREATE TABLE PutStartRequest (subreqId INTEGER, diskServer VARCHAR2(2048), fileSystem VARCHAR2(2048), fileId INTEGER, nsHost VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_PutStartRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type PutDoneStart */
CREATE TABLE PutDoneStart (subreqId INTEGER, diskServer VARCHAR2(2048), fileSystem VARCHAR2(2048), fileId INTEGER, nsHost VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_PutDoneStart_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type GetUpdateStartRequest */
CREATE TABLE GetUpdateStartRequest (subreqId INTEGER, diskServer VARCHAR2(2048), fileSystem VARCHAR2(2048), fileId INTEGER, nsHost VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_GetUpdateStartRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type QueryParameter */
CREATE TABLE QueryParameter (value VARCHAR2(2048), id INTEGER CONSTRAINT PK_QueryParameter_Id PRIMARY KEY, query INTEGER, queryType INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 0, 'REQUESTQUERYTYPE_FILENAME');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 1, 'REQUESTQUERYTYPE_REQID');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 2, 'REQUESTQUERYTYPE_USERTAG');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 3, 'REQUESTQUERYTYPE_FILEID');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 4, 'REQUESTQUERYTYPE_REQID_GETNEXT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 5, 'REQUESTQUERYTYPE_USERTAG_GETNEXT');

/* SQL statements for type StagePrepareToGetRequest */
CREATE TABLE StagePrepareToGetRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StagePrepareToGetRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StagePrepareToPutRequest */
CREATE TABLE StagePrepareToPutRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StagePrepareToPutRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StagePrepareToUpdateRequest */
CREATE TABLE StagePrepareToUpdateRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StagePrepareToUpdateRequ_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StagePutRequest */
CREATE TABLE StagePutRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StagePutRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageUpdateRequest */
CREATE TABLE StageUpdateRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageUpdateRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageRmRequest */
CREATE TABLE StageRmRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageRmRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StagePutDoneRequest */
CREATE TABLE StagePutDoneRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, parentUuid VARCHAR2(2048), id INTEGER CONSTRAINT PK_StagePutDoneRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER, parent INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageFileQueryRequest */
CREATE TABLE StageFileQueryRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, fileName VARCHAR2(2048), id INTEGER CONSTRAINT PK_StageFileQueryRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type SubRequest */
CREATE TABLE SubRequest (retryCounter NUMBER, fileName VARCHAR2(2048), protocol VARCHAR2(2048), xsize INTEGER, priority NUMBER, subreqId VARCHAR2(2048), flags NUMBER, modeBits NUMBER, creationTime INTEGER, lastModificationTime INTEGER, answered NUMBER, errorCode NUMBER, errorMessage VARCHAR2(2048), requestedFileSystems VARCHAR2(2048), svcHandler VARCHAR2(2048), id INTEGER CONSTRAINT PK_SubRequest_Id PRIMARY KEY, diskcopy INTEGER, castorFile INTEGER, parent INTEGER, status INTEGER, request INTEGER, getNextStatus INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 0, 'SUBREQUEST_START');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 1, 'SUBREQUEST_RESTART');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 2, 'SUBREQUEST_RETRY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 3, 'SUBREQUEST_WAITSCHED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 4, 'SUBREQUEST_WAITTAPERECALL');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 5, 'SUBREQUEST_WAITSUBREQ');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 6, 'SUBREQUEST_READY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 7, 'SUBREQUEST_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 8, 'SUBREQUEST_FINISHED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 9, 'SUBREQUEST_FAILED_FINISHED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 10, 'SUBREQUEST_FAILED_ANSWERING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 11, 'SUBREQUEST_ARCHIVED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 12, 'SUBREQUEST_REPACK');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 13, 'SUBREQUEST_READYFORSCHED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'status', 14, 'SUBREQUEST_BEINGSCHED');

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'getNextStatus', 0, 'GETNEXTSTATUS_NOTAPPLICABLE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'getNextStatus', 1, 'GETNEXTSTATUS_FILESTAGED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('SubRequest', 'getNextStatus', 2, 'GETNEXTSTATUS_NOTIFIED');

/* SQL statements for type StageReleaseFilesRequest */
CREATE TABLE StageReleaseFilesRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageReleaseFilesRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageAbortRequest */
CREATE TABLE StageAbortRequest (parentUuid VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageAbortRequest_Id PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageGetNextRequest */
CREATE TABLE StageGetNextRequest (parentUuid VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageGetNextRequest_Id PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StagePutNextRequest */
CREATE TABLE StagePutNextRequest (parentUuid VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StagePutNextRequest_Id PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageUpdateNextRequest */
CREATE TABLE StageUpdateNextRequest (parentUuid VARCHAR2(2048), flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageUpdateNextRequest_Id PRIMARY KEY, parent INTEGER, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type Tape */
CREATE TABLE Tape (vid VARCHAR2(2048), side NUMBER, tpmode NUMBER, errMsgTxt VARCHAR2(2048), errorCode NUMBER, severity NUMBER, vwAddress VARCHAR2(2048), dgn VARCHAR2(2048), label VARCHAR2(2048), density VARCHAR2(2048), devtype VARCHAR2(2048), id INTEGER CONSTRAINT PK_Tape_Id PRIMARY KEY, stream INTEGER, status INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 0, 'TAPE_UNUSED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 1, 'TAPE_PENDING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 2, 'TAPE_WAITDRIVE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 3, 'TAPE_WAITMOUNT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 4, 'TAPE_MOUNTED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 5, 'TAPE_FINISHED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 6, 'TAPE_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 7, 'TAPE_UNKNOWN');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 8, 'TAPE_WAITPOLICY');

/* SQL statements for type Segment */
CREATE TABLE Segment (fseq NUMBER, offset INTEGER, bytes_in INTEGER, bytes_out INTEGER, host_bytes INTEGER, segmCksumAlgorithm VARCHAR2(2048), segmCksum NUMBER, errMsgTxt VARCHAR2(2048), errorCode NUMBER, severity NUMBER, blockId0 INTEGER, blockId1 INTEGER, blockId2 INTEGER, blockId3 INTEGER, creationTime INTEGER, priority INTEGER, id INTEGER CONSTRAINT PK_Segment_Id PRIMARY KEY, copy INTEGER, status INTEGER, tape INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 0, 'SEGMENT_UNPROCESSED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 5, 'SEGMENT_FILECOPIED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 6, 'SEGMENT_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 7, 'SEGMENT_SELECTED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 8, 'SEGMENT_RETRIED');

/* SQL statements for type TapePool */
CREATE TABLE TapePool (name VARCHAR2(2048), migrSelectPolicy VARCHAR2(2048), id INTEGER CONSTRAINT PK_TapePool_Id PRIMARY KEY) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type TapeCopy */
CREATE TABLE TapeCopy (copyNb NUMBER, errorCode NUMBER, nbRetry NUMBER, missingCopies NUMBER, id INTEGER CONSTRAINT PK_TapeCopy_Id PRIMARY KEY, castorFile INTEGER, status INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 0, 'TAPECOPY_CREATED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 1, 'TAPECOPY_TOBEMIGRATED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 2, 'TAPECOPY_WAITINSTREAMS');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 3, 'TAPECOPY_SELECTED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 4, 'TAPECOPY_TOBERECALLED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 5, 'TAPECOPY_STAGED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 6, 'TAPECOPY_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 7, 'TAPECOPY_WAITPOLICY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 8, 'TAPECOPY_REC_RETRY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeCopy', 'status', 9, 'TAPECOPY_MIG_RETRY');

/* SQL statements for type CastorFile */
CREATE TABLE CastorFile (fileId INTEGER, nsHost VARCHAR2(2048), fileSize INTEGER, creationTime INTEGER, lastAccessTime INTEGER, lastKnownFileName VARCHAR2(2048), lastUpdateTime INTEGER, id INTEGER CONSTRAINT PK_CastorFile_Id PRIMARY KEY, svcClass INTEGER, fileClass INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type DiskCopy */
CREATE TABLE DiskCopy (path VARCHAR2(2048), gcWeight NUMBER, creationTime INTEGER, lastAccessTime INTEGER, diskCopySize INTEGER, nbCopyAccesses NUMBER, owneruid NUMBER, ownergid NUMBER, id INTEGER CONSTRAINT PK_DiskCopy_Id PRIMARY KEY, gcType INTEGER, fileSystem INTEGER, castorFile INTEGER, status INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 0, 'GCTYPE_AUTO');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 1, 'GCTYPE_USER');

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 0, 'DISKCOPY_STAGED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 1, 'DISKCOPY_WAITDISK2DISKCOPY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 2, 'DISKCOPY_WAITTAPERECALL');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 3, 'DISKCOPY_DELETED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 4, 'DISKCOPY_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 5, 'DISKCOPY_WAITFS');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 6, 'DISKCOPY_STAGEOUT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 7, 'DISKCOPY_INVALID');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 9, 'DISKCOPY_BEINGDELETED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 10, 'DISKCOPY_CANBEMIGR');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'status', 11, 'DISKCOPY_WAITFS_SCHEDULING');

/* SQL statements for type FileSystem */
CREATE TABLE FileSystem (free INTEGER, mountPoint VARCHAR2(2048), minFreeSpace NUMBER, minAllowedFreeSpace NUMBER, maxFreeSpace NUMBER, totalSize INTEGER, readRate INTEGER, writeRate INTEGER, nbReadStreams NUMBER, nbWriteStreams NUMBER, nbReadWriteStreams NUMBER, nbMigratorStreams NUMBER, nbRecallerStreams NUMBER, id INTEGER CONSTRAINT PK_FileSystem_Id PRIMARY KEY, diskPool INTEGER, diskserver INTEGER, status INTEGER, adminStatus INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'status', 0, 'FILESYSTEM_PRODUCTION');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'status', 1, 'FILESYSTEM_DRAINING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'status', 2, 'FILESYSTEM_DISABLED');

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 0, 'ADMIN_NONE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 1, 'ADMIN_FORCE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 2, 'ADMIN_RELEASE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 3, 'ADMIN_DELETED');

/* SQL statements for type SvcClass */
CREATE TABLE SvcClass (nbDrives NUMBER, name VARCHAR2(2048), defaultFileSize INTEGER, maxReplicaNb NUMBER, migratorPolicy VARCHAR2(2048), recallerPolicy VARCHAR2(2048), streamPolicy VARCHAR2(2048), gcPolicy VARCHAR2(2048), disk1Behavior NUMBER, replicateOnClose NUMBER, failJobsWhenNoSpace NUMBER, id INTEGER CONSTRAINT PK_SvcClass_Id PRIMARY KEY, forcedFileClass INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;
CREATE TABLE SvcClass2TapePool (Parent INTEGER, Child INTEGER) INITRANS 50 PCTFREE 50;
CREATE INDEX I_SvcClass2TapePool_C on SvcClass2TapePool (child);
CREATE INDEX I_SvcClass2TapePool_P on SvcClass2TapePool (parent);

/* SQL statements for type DiskPool */
CREATE TABLE DiskPool (name VARCHAR2(2048), id INTEGER CONSTRAINT PK_DiskPool_Id PRIMARY KEY) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;
CREATE TABLE DiskPool2SvcClass (Parent INTEGER, Child INTEGER) INITRANS 50 PCTFREE 50;
CREATE INDEX I_DiskPool2SvcClass_C on DiskPool2SvcClass (child);
CREATE INDEX I_DiskPool2SvcClass_P on DiskPool2SvcClass (parent);

/* SQL statements for type Stream */
CREATE TABLE Stream (initialSizeToTransfer INTEGER, lastFileSystemChange INTEGER, id INTEGER CONSTRAINT PK_Stream_Id PRIMARY KEY, tape INTEGER, lastFileSystemUsed INTEGER, lastButOneFileSystemUsed INTEGER, tapePool INTEGER, status INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;
CREATE TABLE Stream2TapeCopy (Parent INTEGER, Child INTEGER) INITRANS 50 PCTFREE 50;
CREATE INDEX I_Stream2TapeCopy_C on Stream2TapeCopy (child);
CREATE INDEX I_Stream2TapeCopy_P on Stream2TapeCopy (parent);

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 0, 'STREAM_PENDING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 1, 'STREAM_WAITDRIVE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 2, 'STREAM_WAITMOUNT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 3, 'STREAM_RUNNING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 4, 'STREAM_WAITSPACE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 5, 'STREAM_CREATED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 6, 'STREAM_STOPPED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 7, 'STREAM_WAITPOLICY');

/* SQL statements for type FileClass */
CREATE TABLE FileClass (name VARCHAR2(2048), nbCopies NUMBER, id INTEGER CONSTRAINT PK_FileClass_Id PRIMARY KEY) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type DiskServer */
CREATE TABLE DiskServer (name VARCHAR2(2048), readRate INTEGER, writeRate INTEGER, nbReadStreams NUMBER, nbWriteStreams NUMBER, nbReadWriteStreams NUMBER, nbMigratorStreams NUMBER, nbRecallerStreams NUMBER, id INTEGER CONSTRAINT PK_DiskServer_Id PRIMARY KEY, status INTEGER, adminStatus INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'status', 0, 'DISKSERVER_PRODUCTION');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'status', 1, 'DISKSERVER_DRAINING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'status', 2, 'DISKSERVER_DISABLED');

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 0, 'ADMIN_NONE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 1, 'ADMIN_FORCE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 2, 'ADMIN_RELEASE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 3, 'ADMIN_DELETED');

/* SQL statements for type SetFileGCWeight */
CREATE TABLE SetFileGCWeight (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, weight NUMBER, id INTEGER CONSTRAINT PK_SetFileGCWeight_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageRepackRequest */
CREATE TABLE StageRepackRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, repackVid VARCHAR2(2048), id INTEGER CONSTRAINT PK_StageRepackRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageDiskCopyReplicaRequest */
CREATE TABLE StageDiskCopyReplicaRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageDiskCopyReplicaRequ_Id PRIMARY KEY, svcClass INTEGER, client INTEGER, sourceSvcClass INTEGER, destDiskCopy INTEGER, sourceDiskCopy INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type NsFilesDeleted */
CREATE TABLE NsFilesDeleted (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_NsFilesDeleted_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type Disk2DiskCopyStartRequest */
CREATE TABLE Disk2DiskCopyStartRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, diskCopyId INTEGER, sourceDiskCopyId INTEGER, diskServer VARCHAR2(2048), mountPoint VARCHAR2(2048), fileId INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_Disk2DiskCopyStartReques_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type FirstByteWritten */
CREATE TABLE FirstByteWritten (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, subReqId INTEGER, fileId INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_FirstByteWritten_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StageGetRequest */
CREATE TABLE StageGetRequest (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_StageGetRequest_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type StgFilesDeleted */
CREATE TABLE StgFilesDeleted (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, nsHost VARCHAR2(2048), id INTEGER CONSTRAINT PK_StgFilesDeleted_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type DiskPoolQuery */
CREATE TABLE DiskPoolQuery (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, diskPoolName VARCHAR2(2048), id INTEGER CONSTRAINT PK_DiskPoolQuery_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type VersionQuery */
CREATE TABLE VersionQuery (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, id INTEGER CONSTRAINT PK_VersionQuery_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type ChangePrivilege */
CREATE TABLE ChangePrivilege (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, isGranted NUMBER, id INTEGER CONSTRAINT PK_ChangePrivilege_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type BWUser */
CREATE TABLE BWUser (euid NUMBER, egid NUMBER, id INTEGER CONSTRAINT PK_BWUser_Id PRIMARY KEY, request INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type RequestType */
CREATE TABLE RequestType (reqType NUMBER, id INTEGER CONSTRAINT PK_RequestType_Id PRIMARY KEY, request INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type ListPrivileges */
CREATE TABLE ListPrivileges (flags INTEGER, userName VARCHAR2(2048), euid NUMBER, egid NUMBER, mask NUMBER, pid NUMBER, machine VARCHAR2(2048), svcClassName VARCHAR2(2048), userTag VARCHAR2(2048), reqId VARCHAR2(2048), creationTime INTEGER, lastModificationTime INTEGER, userId NUMBER, groupId NUMBER, requestType NUMBER, id INTEGER CONSTRAINT PK_ListPrivileges_Id PRIMARY KEY, svcClass INTEGER, client INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for type TapeGatewayRequest */
CREATE TABLE TapeGatewayRequest (accessMode NUMBER, startTime INTEGER, lastVdqmPingTime INTEGER, vdqmVolReqId NUMBER, nbRetry NUMBER, lastFseq NUMBER, id INTEGER CONSTRAINT PK_TapeGatewayRequest_Id PRIMARY KEY, streamMigration INTEGER, tapeRecall INTEGER, status INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 0, 'TO_BE_RESOLVED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 1, 'TO_BE_SENT_TO_VDQM');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 2, 'WAITING_TAPESERVER');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 3, 'ONGOING');

/* SQL statements for type TapeGatewaySubRequest */
CREATE TABLE TapeGatewaySubRequest (fseq NUMBER, id INTEGER CONSTRAINT PK_TapeGatewaySubRequest_Id PRIMARY KEY, tapecopy INTEGER, request INTEGER, diskcopy INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;

/* SQL statements for constraints on SvcClass */
ALTER TABLE SvcClass2TapePool
  ADD CONSTRAINT FK_SvcClass2TapePool_P FOREIGN KEY (Parent) REFERENCES SvcClass (id)
  ADD CONSTRAINT FK_SvcClass2TapePool_C FOREIGN KEY (Child) REFERENCES TapePool (id);

/* SQL statements for constraints on DiskPool */
ALTER TABLE DiskPool2SvcClass
  ADD CONSTRAINT FK_DiskPool2SvcClass_P FOREIGN KEY (Parent) REFERENCES DiskPool (id)
  ADD CONSTRAINT FK_DiskPool2SvcClass_C FOREIGN KEY (Child) REFERENCES SvcClass (id);

/* SQL statements for constraints on Stream */
ALTER TABLE Stream2TapeCopy
  ADD CONSTRAINT FK_Stream2TapeCopy_P FOREIGN KEY (Parent) REFERENCES Stream (id)
  ADD CONSTRAINT FK_Stream2TapeCopy_C FOREIGN KEY (Child) REFERENCES TapeCopy (id);

/* Fill Type2Obj metatable */
INSERT INTO Type2Obj (type, object) VALUES (0, 'INVALID');
INSERT INTO Type2Obj (type, object) VALUES (1, 'Ptr');
INSERT INTO Type2Obj (type, object) VALUES (2, 'CastorFile');
INSERT INTO Type2Obj (type, object) VALUES (4, 'Cuuid');
INSERT INTO Type2Obj (type, object) VALUES (5, 'DiskCopy');
INSERT INTO Type2Obj (type, object) VALUES (6, 'DiskFile');
INSERT INTO Type2Obj (type, object) VALUES (7, 'DiskPool');
INSERT INTO Type2Obj (type, object) VALUES (8, 'DiskServer');
INSERT INTO Type2Obj (type, object) VALUES (10, 'FileClass');
INSERT INTO Type2Obj (type, object) VALUES (12, 'FileSystem');
INSERT INTO Type2Obj (type, object) VALUES (13, 'IClient');
INSERT INTO Type2Obj (type, object) VALUES (14, 'MessageAck');
INSERT INTO Type2Obj (type, object) VALUES (16, 'ReqIdRequest');
INSERT INTO Type2Obj (type, object) VALUES (17, 'Request');
INSERT INTO Type2Obj (type, object) VALUES (18, 'Segment');
INSERT INTO Type2Obj (type, object) VALUES (21, 'StageGetNextRequest');
INSERT INTO Type2Obj (type, object) VALUES (26, 'Stream');
INSERT INTO Type2Obj (type, object) VALUES (27, 'SubRequest');
INSERT INTO Type2Obj (type, object) VALUES (28, 'SvcClass');
INSERT INTO Type2Obj (type, object) VALUES (29, 'Tape');
INSERT INTO Type2Obj (type, object) VALUES (30, 'TapeCopy');
INSERT INTO Type2Obj (type, object) VALUES (31, 'TapePool');
INSERT INTO Type2Obj (type, object) VALUES (33, 'StageFileQueryRequest');
INSERT INTO Type2Obj (type, object) VALUES (35, 'StageGetRequest');
INSERT INTO Type2Obj (type, object) VALUES (36, 'StagePrepareToGetRequest');
INSERT INTO Type2Obj (type, object) VALUES (37, 'StagePrepareToPutRequest');
INSERT INTO Type2Obj (type, object) VALUES (38, 'StagePrepareToUpdateRequest');
INSERT INTO Type2Obj (type, object) VALUES (39, 'StagePutDoneRequest');
INSERT INTO Type2Obj (type, object) VALUES (40, 'StagePutRequest');
INSERT INTO Type2Obj (type, object) VALUES (42, 'StageRmRequest');
INSERT INTO Type2Obj (type, object) VALUES (44, 'StageUpdateRequest');
INSERT INTO Type2Obj (type, object) VALUES (45, 'FileRequest');
INSERT INTO Type2Obj (type, object) VALUES (46, 'QryRequest');
INSERT INTO Type2Obj (type, object) VALUES (48, 'StagePutNextRequest');
INSERT INTO Type2Obj (type, object) VALUES (49, 'StageUpdateNextRequest');
INSERT INTO Type2Obj (type, object) VALUES (50, 'StageAbortRequest');
INSERT INTO Type2Obj (type, object) VALUES (51, 'StageReleaseFilesRequest');
INSERT INTO Type2Obj (type, object) VALUES (58, 'DiskCopyForRecall');
INSERT INTO Type2Obj (type, object) VALUES (59, 'TapeCopyForMigration');
INSERT INTO Type2Obj (type, object) VALUES (60, 'GetUpdateStartRequest');
INSERT INTO Type2Obj (type, object) VALUES (62, 'BaseAddress');
INSERT INTO Type2Obj (type, object) VALUES (64, 'Disk2DiskCopyDoneRequest');
INSERT INTO Type2Obj (type, object) VALUES (65, 'MoverCloseRequest');
INSERT INTO Type2Obj (type, object) VALUES (66, 'StartRequest');
INSERT INTO Type2Obj (type, object) VALUES (67, 'PutStartRequest');
INSERT INTO Type2Obj (type, object) VALUES (69, 'IObject');
INSERT INTO Type2Obj (type, object) VALUES (70, 'IAddress');
INSERT INTO Type2Obj (type, object) VALUES (71, 'QueryParameter');
INSERT INTO Type2Obj (type, object) VALUES (72, 'DiskCopyInfo');
INSERT INTO Type2Obj (type, object) VALUES (73, 'Files2Delete');
INSERT INTO Type2Obj (type, object) VALUES (74, 'FilesDeleted');
INSERT INTO Type2Obj (type, object) VALUES (76, 'GCLocalFile');
INSERT INTO Type2Obj (type, object) VALUES (78, 'GetUpdateDone');
INSERT INTO Type2Obj (type, object) VALUES (79, 'GetUpdateFailed');
INSERT INTO Type2Obj (type, object) VALUES (80, 'PutFailed');
INSERT INTO Type2Obj (type, object) VALUES (81, 'GCFile');
INSERT INTO Type2Obj (type, object) VALUES (82, 'GCFileList');
INSERT INTO Type2Obj (type, object) VALUES (83, 'FilesDeletionFailed');
INSERT INTO Type2Obj (type, object) VALUES (84, 'TapeRequest');
INSERT INTO Type2Obj (type, object) VALUES (85, 'ClientIdentification');
INSERT INTO Type2Obj (type, object) VALUES (86, 'TapeServer');
INSERT INTO Type2Obj (type, object) VALUES (87, 'TapeDrive');
INSERT INTO Type2Obj (type, object) VALUES (88, 'DeviceGroupName');
INSERT INTO Type2Obj (type, object) VALUES (89, 'ErrorHistory');
INSERT INTO Type2Obj (type, object) VALUES (90, 'TapeDriveDedication');
INSERT INTO Type2Obj (type, object) VALUES (91, 'TapeAccessSpecification');
INSERT INTO Type2Obj (type, object) VALUES (92, 'TapeDriveCompatibility');
INSERT INTO Type2Obj (type, object) VALUES (93, 'PutDoneStart');
INSERT INTO Type2Obj (type, object) VALUES (95, 'SetFileGCWeight');
INSERT INTO Type2Obj (type, object) VALUES (96, 'RepackRequest');
INSERT INTO Type2Obj (type, object) VALUES (97, 'RepackSubRequest');
INSERT INTO Type2Obj (type, object) VALUES (98, 'RepackSegment');
INSERT INTO Type2Obj (type, object) VALUES (99, 'RepackAck');
INSERT INTO Type2Obj (type, object) VALUES (101, 'DiskServerDescription');
INSERT INTO Type2Obj (type, object) VALUES (102, 'FileSystemDescription');
INSERT INTO Type2Obj (type, object) VALUES (103, 'DiskPoolQuery');
INSERT INTO Type2Obj (type, object) VALUES (104, 'EndResponse');
INSERT INTO Type2Obj (type, object) VALUES (105, 'FileResponse');
INSERT INTO Type2Obj (type, object) VALUES (106, 'StringResponse');
INSERT INTO Type2Obj (type, object) VALUES (107, 'Response');
INSERT INTO Type2Obj (type, object) VALUES (108, 'IOResponse');
INSERT INTO Type2Obj (type, object) VALUES (109, 'AbortResponse');
INSERT INTO Type2Obj (type, object) VALUES (111, 'FileQueryResponse');
INSERT INTO Type2Obj (type, object) VALUES (113, 'GetUpdateStartResponse');
INSERT INTO Type2Obj (type, object) VALUES (114, 'BasicResponse');
INSERT INTO Type2Obj (type, object) VALUES (115, 'StartResponse');
INSERT INTO Type2Obj (type, object) VALUES (116, 'GCFilesResponse');
INSERT INTO Type2Obj (type, object) VALUES (117, 'FileQryResponse');
INSERT INTO Type2Obj (type, object) VALUES (118, 'DiskPoolQueryResponse');
INSERT INTO Type2Obj (type, object) VALUES (119, 'StageRepackRequest');
INSERT INTO Type2Obj (type, object) VALUES (120, 'DiskServerStateReport');
INSERT INTO Type2Obj (type, object) VALUES (121, 'DiskServerMetricsReport');
INSERT INTO Type2Obj (type, object) VALUES (122, 'FileSystemStateReport');
INSERT INTO Type2Obj (type, object) VALUES (123, 'FileSystemMetricsReport');
INSERT INTO Type2Obj (type, object) VALUES (124, 'DiskServerAdminReport');
INSERT INTO Type2Obj (type, object) VALUES (125, 'FileSystemAdminReport');
INSERT INTO Type2Obj (type, object) VALUES (126, 'StreamReport');
INSERT INTO Type2Obj (type, object) VALUES (127, 'FileSystemStateAck');
INSERT INTO Type2Obj (type, object) VALUES (128, 'MonitorMessageAck');
INSERT INTO Type2Obj (type, object) VALUES (129, 'Client');
INSERT INTO Type2Obj (type, object) VALUES (130, 'JobSubmissionRequest');
INSERT INTO Type2Obj (type, object) VALUES (131, 'VersionQuery');
INSERT INTO Type2Obj (type, object) VALUES (132, 'VersionResponse');
INSERT INTO Type2Obj (type, object) VALUES (133, 'StageDiskCopyReplicaRequest');
INSERT INTO Type2Obj (type, object) VALUES (134, 'RepackResponse');
INSERT INTO Type2Obj (type, object) VALUES (135, 'RepackFileQry');
INSERT INTO Type2Obj (type, object) VALUES (136, 'CnsInfoMigrationPolicy');
INSERT INTO Type2Obj (type, object) VALUES (137, 'DbInfoMigrationPolicy');
INSERT INTO Type2Obj (type, object) VALUES (138, 'CnsInfoRecallPolicy');
INSERT INTO Type2Obj (type, object) VALUES (139, 'DbInfoRecallPolicy');
INSERT INTO Type2Obj (type, object) VALUES (140, 'DbInfoStreamPolicy');
INSERT INTO Type2Obj (type, object) VALUES (141, 'PolicyObj');
INSERT INTO Type2Obj (type, object) VALUES (142, 'NsFilesDeleted');
INSERT INTO Type2Obj (type, object) VALUES (143, 'NsFilesDeletedResponse');
INSERT INTO Type2Obj (type, object) VALUES (144, 'Disk2DiskCopyStartRequest');
INSERT INTO Type2Obj (type, object) VALUES (145, 'Disk2DiskCopyStartResponse');
INSERT INTO Type2Obj (type, object) VALUES (146, 'ThreadNotification');
INSERT INTO Type2Obj (type, object) VALUES (147, 'FirstByteWritten');
INSERT INTO Type2Obj (type, object) VALUES (148, 'VdqmTape');
INSERT INTO Type2Obj (type, object) VALUES (149, 'StgFilesDeleted');
INSERT INTO Type2Obj (type, object) VALUES (150, 'StgFilesDeletedResponse');
INSERT INTO Type2Obj (type, object) VALUES (151, 'VolumePriority');
INSERT INTO Type2Obj (type, object) VALUES (152, 'ChangePrivilege');
INSERT INTO Type2Obj (type, object) VALUES (153, 'BWUser');
INSERT INTO Type2Obj (type, object) VALUES (154, 'RequestType');
INSERT INTO Type2Obj (type, object) VALUES (155, 'ListPrivileges');
INSERT INTO Type2Obj (type, object) VALUES (156, 'Privilege');
INSERT INTO Type2Obj (type, object) VALUES (157, 'ListPrivilegesResponse');
INSERT INTO Type2Obj (type, object) VALUES (158, 'PriorityMap');
INSERT INTO Type2Obj (type, object) VALUES (159, 'VectorAddress');
INSERT INTO Type2Obj (type, object) VALUES (160, 'Tape2DriveDedication');
INSERT INTO Type2Obj (type, object) VALUES (161, 'TapeRecall');
INSERT INTO Type2Obj (type, object) VALUES (162, 'FileMigratedNotification');
INSERT INTO Type2Obj (type, object) VALUES (163, 'FileRecalledNotification');
INSERT INTO Type2Obj (type, object) VALUES (164, 'FileToMigrateRequest');
INSERT INTO Type2Obj (type, object) VALUES (165, 'FileToMigrate');
INSERT INTO Type2Obj (type, object) VALUES (166, 'FileToRecallRequest');
INSERT INTO Type2Obj (type, object) VALUES (167, 'FileToRecall');
INSERT INTO Type2Obj (type, object) VALUES (168, 'VolumeRequest');
INSERT INTO Type2Obj (type, object) VALUES (169, 'Volume');
INSERT INTO Type2Obj (type, object) VALUES (170, 'TapeGatewayRequest');
INSERT INTO Type2Obj (type, object) VALUES (171, 'DbInfoRetryPolicy');
INSERT INTO Type2Obj (type, object) VALUES (172, 'EndNotification');
INSERT INTO Type2Obj (type, object) VALUES (173, 'NoMoreFiles');
INSERT INTO Type2Obj (type, object) VALUES (174, 'NotificationAcknowledge');
INSERT INTO Type2Obj (type, object) VALUES (175, 'FileErrorReport');
INSERT INTO Type2Obj (type, object) VALUES (176, 'BaseFileInfo');
INSERT INTO Type2Obj (type, object) VALUES (178, 'RmMasterReport');
INSERT INTO Type2Obj (type, object) VALUES (179, 'EndNotificationErrorReport');
INSERT INTO Type2Obj (type, object) VALUES (180, 'TapeGatewaySubRequest');
INSERT INTO Type2Obj (type, object) VALUES (181, 'GatewayMessage');
INSERT INTO Type2Obj (type, object) VALUES (182, 'DumpNotification');
INSERT INTO Type2Obj (type, object) VALUES (183, 'PingNotification');
INSERT INTO Type2Obj (type, object) VALUES (184, 'DumpParameters');
INSERT INTO Type2Obj (type, object) VALUES (185, 'DumpParametersRequest');
COMMIT;


CREATE TABLE CastorVersion (schemaVersion VARCHAR2(20), release VARCHAR2(20));
INSERT INTO CastorVersion VALUES ('-', '2_1_9_0');

/*******************************************************************
 *
 * @(#)RCSfile: oracleCommon.schema.sql,v  Revision: 1.19  Date: 2009/08/10 15:30:12  Author: itglp 
 *
 * This file contains all schema definitions which are not generated automatically.
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* A small table used to cross check code and DB versions */
UPDATE CastorVersion SET schemaVersion = '2_1_9_0';

/* Sequence for indices */
CREATE SEQUENCE ids_seq CACHE 300;

/* SQL statements for object types */
CREATE TABLE Id2Type (id INTEGER CONSTRAINT PK_Id2Type_Id PRIMARY KEY, type NUMBER) ENABLE ROW MOVEMENT;

/* SQL statements for requests status */
/* Partitioning enables faster response (more than indexing) for the most frequent queries - credits to Nilo Segura */
CREATE TABLE newRequests (type NUMBER(38) CONSTRAINT NN_NewRequests_Type NOT NULL, id NUMBER(38) CONSTRAINT NN_NewRequests_Id NOT NULL, creation DATE CONSTRAINT NN_NewRequests_Creation NOT NULL, CONSTRAINT PK_NewRequests_Type_Id PRIMARY KEY (type, id))
ORGANIZATION INDEX
COMPRESS
PARTITION BY LIST (type)
 (
  PARTITION type_16 VALUES (16)  TABLESPACE stager_data,
  PARTITION type_21 VALUES (21)  TABLESPACE stager_data,
  PARTITION type_33 VALUES (33)  TABLESPACE stager_data,
  PARTITION type_34 VALUES (34)  TABLESPACE stager_data,
  PARTITION type_35 VALUES (35)  TABLESPACE stager_data,
  PARTITION type_36 VALUES (36)  TABLESPACE stager_data,
  PARTITION type_37 VALUES (37)  TABLESPACE stager_data,
  PARTITION type_38 VALUES (38)  TABLESPACE stager_data,
  PARTITION type_39 VALUES (39)  TABLESPACE stager_data,
  PARTITION type_40 VALUES (40)  TABLESPACE stager_data,
  PARTITION type_41 VALUES (41)  TABLESPACE stager_data,
  PARTITION type_42 VALUES (42)  TABLESPACE stager_data,
  PARTITION type_43 VALUES (43)  TABLESPACE stager_data,
  PARTITION type_44 VALUES (44)  TABLESPACE stager_data,
  PARTITION type_45 VALUES (45)  TABLESPACE stager_data,
  PARTITION type_46 VALUES (46)  TABLESPACE stager_data,
  PARTITION type_48 VALUES (48)  TABLESPACE stager_data,
  PARTITION type_49 VALUES (49)  TABLESPACE stager_data,
  PARTITION type_50 VALUES (50)  TABLESPACE stager_data,
  PARTITION type_51 VALUES (51)  TABLESPACE stager_data,
  PARTITION type_60 VALUES (60)  TABLESPACE stager_data,
  PARTITION type_64 VALUES (64)  TABLESPACE stager_data,
  PARTITION type_65 VALUES (65)  TABLESPACE stager_data,
  PARTITION type_66 VALUES (66)  TABLESPACE stager_data,
  PARTITION type_67 VALUES (67)  TABLESPACE stager_data,
  PARTITION type_78 VALUES (78)  TABLESPACE stager_data,
  PARTITION type_79 VALUES (79)  TABLESPACE stager_data,
  PARTITION type_80 VALUES (80)  TABLESPACE stager_data,
  PARTITION type_84 VALUES (84)  TABLESPACE stager_data,
  PARTITION type_90 VALUES (90)  TABLESPACE stager_data,
  PARTITION type_142 VALUES (142)  TABLESPACE stager_data,
  PARTITION type_144 VALUES (144)  TABLESPACE stager_data,
  PARTITION type_147 VALUES (147)  TABLESPACE stager_data,
  PARTITION type_149 VALUES (149)  TABLESPACE stager_data,
  PARTITION notlisted VALUES (default) TABLESPACE stager_data
 );

/* Redefinition of table SubRequest to make it partitioned by status */
/* Unfortunately it has already been defined, so we drop and recreate it */
/* Note that if the schema changes, this part has to be updated manually! */
DROP TABLE SubRequest;
CREATE TABLE SubRequest
  (retryCounter NUMBER, fileName VARCHAR2(2048), protocol VARCHAR2(2048),
   xsize INTEGER, priority NUMBER, subreqId VARCHAR2(2048), flags NUMBER,
   modeBits NUMBER, creationTime INTEGER CONSTRAINT NN_SubRequest_CreationTime 
   NOT NULL, lastModificationTime INTEGER, answered NUMBER, errorCode NUMBER, 
   errorMessage VARCHAR2(2048), id NUMBER CONSTRAINT NN_SubRequest_Id NOT NULL,
   diskcopy INTEGER, castorFile INTEGER, parent INTEGER, status INTEGER,
   request INTEGER, getNextStatus INTEGER, requestedFileSystems VARCHAR2(2048),
   svcHandler VARCHAR2(2048) CONSTRAINT NN_SubRequest_SvcHandler NOT NULL
  )
  PCTFREE 50 PCTUSED 40 INITRANS 50
  ENABLE ROW MOVEMENT
  PARTITION BY LIST (STATUS)
   (
    PARTITION P_STATUS_0_1_2   VALUES (0, 1, 2),      -- *START
    PARTITION P_STATUS_3_13_14 VALUES (3, 13, 14),    -- *SCHED
    PARTITION P_STATUS_4       VALUES (4),
    PARTITION P_STATUS_5       VALUES (5),
    PARTITION P_STATUS_6       VALUES (6),
    PARTITION P_STATUS_7       VALUES (7),
    PARTITION P_STATUS_8       VALUES (8),
    PARTITION P_STATUS_9_10    VALUES (9, 10),        -- FAILED_*
    PARTITION P_STATUS_11      VALUES (11),
    PARTITION P_STATUS_12      VALUES (12),
    PARTITION P_STATUS_OTHER   VALUES (DEFAULT)
   );

/* SQL statements for constraints on the SubRequest table */
ALTER TABLE SubRequest
  ADD CONSTRAINT PK_SubRequest_Id PRIMARY KEY (ID);
CREATE INDEX I_SubRequest_RT_CT_ID ON SubRequest(svcHandler, creationTime, id) LOCAL
 (PARTITION P_STATUS_0_1_2,
  PARTITION P_STATUS_3_13_14,
  PARTITION P_STATUS_4,
  PARTITION P_STATUS_5,
  PARTITION P_STATUS_6,
  PARTITION P_STATUS_7,
  PARTITION P_STATUS_8,
  PARTITION P_STATUS_9_10,
  PARTITION P_STATUS_11,
  PARTITION P_STATUS_12,
  PARTITION P_STATUS_OTHER);  

/* Redefinition of table TapeCopy to make it partitioned by status */
ALTER TABLE Stream2TapeCopy DROP CONSTRAINT FK_Stream2TapeCopy_C;
DROP TABLE TapeCopy;
CREATE TABLE TapeCopy
  (copyNb NUMBER, errorCode NUMBER, nbRetry NUMBER, missingCopies NUMBER,
   id INTEGER CONSTRAINT PK_TapeCopy_Id PRIMARY KEY CONSTRAINT NN_TapeCopy_Id NOT NULL,
   castorFile INTEGER, status INTEGER)
  PCTFREE 50 PCTUSED 40 INITRANS 50
  ENABLE ROW MOVEMENT
  PARTITION BY LIST (STATUS)
   (PARTITION P_STATUS_0_1   VALUES (0, 1),
    PARTITION P_STATUS_OTHER VALUES (DEFAULT)
  );

/* Recreate foreign key constraint between Stream2TapeCopy and TapeCopy */
ALTER TABLE Stream2TapeCopy
  ADD CONSTRAINT FK_Stream2TapeCopy_C FOREIGN KEY (Child) REFERENCES TapeCopy (id);

/* Indexes related to most used entities */
CREATE UNIQUE INDEX I_DiskServer_name ON DiskServer (name);

CREATE UNIQUE INDEX I_CastorFile_FileIdNsHost ON CastorFile (fileId, nsHost);
CREATE INDEX I_CastorFile_LastKnownFileName ON CastorFile (lastKnownFileName);
CREATE INDEX I_CastorFile_SvcClass ON CastorFile (svcClass);

CREATE INDEX I_DiskCopy_Castorfile ON DiskCopy (castorFile);
CREATE INDEX I_DiskCopy_FileSystem ON DiskCopy (fileSystem);
CREATE INDEX I_DiskCopy_Status ON DiskCopy (status);
CREATE INDEX I_DiskCopy_GCWeight ON DiskCopy (gcWeight);
CREATE INDEX I_DiskCopy_FS_Status_10 ON DiskCopy (fileSystem,decode(status,10,status,NULL));
CREATE INDEX I_DiskCopy_Status_9 ON DiskCopy (decode(status,9,status,NULL));

CREATE INDEX I_TapeCopy_Castorfile ON TapeCopy (castorFile) LOCAL;
CREATE INDEX I_TapeCopy_Status ON TapeCopy (status) LOCAL;
CREATE INDEX I_TapeCopy_CF_Status_2 ON TapeCopy (castorFile,decode(status,2,status,null)) LOCAL;

CREATE INDEX I_FileSystem_DiskPool ON FileSystem (diskPool);
CREATE INDEX I_FileSystem_DiskServer ON FileSystem (diskServer);

CREATE INDEX I_SubRequest_Castorfile ON SubRequest (castorFile);
CREATE INDEX I_SubRequest_DiskCopy ON SubRequest (diskCopy);
CREATE INDEX I_SubRequest_Request ON SubRequest (request);
CREATE INDEX I_SubRequest_Parent ON SubRequest (parent);
CREATE INDEX I_SubRequest_SubReqId ON SubRequest (subReqId);
CREATE INDEX I_SubRequest_LastModTime ON SubRequest (lastModificationTime) LOCAL;

CREATE INDEX I_StagePTGRequest_ReqId ON StagePrepareToGetRequest (reqId);
CREATE INDEX I_StagePTPRequest_ReqId ON StagePrepareToPutRequest (reqId);
CREATE INDEX I_StagePTURequest_ReqId ON StagePrepareToUpdateRequest (reqId);
CREATE INDEX I_StageGetRequest_ReqId ON StageGetRequest (reqId);
CREATE INDEX I_StagePutRequest_ReqId ON StagePutRequest (reqId);
CREATE INDEX I_StageRepackRequest_ReqId ON StageRepackRequest (reqId);

/* A primary key index for better scan of Stream2TapeCopy */
CREATE UNIQUE INDEX I_Stream2TapeCopy_PC ON Stream2TapeCopy (parent, child);

CREATE INDEX I_GCFile_Request ON GCFile (request);

/* Indexing Tape by Status */
CREATE INDEX I_Tape_Status ON Tape (status);

/* Indexing Segments by Tape */
CREATE INDEX I_Segment_Tape ON Segment (tape);

/* Indexing Segments by Tape and Status */
CREATE INDEX I_Segment_TapeStatus ON Segment (tape, status);

/* Indexing Stream by TapePool */
CREATE INDEX I_Stream_TapePool ON Stream (tapePool); 

/* FileSystem constraints */
ALTER TABLE FileSystem ADD CONSTRAINT FK_FileSystem_DiskServer 
  FOREIGN KEY (diskServer) REFERENCES DiskServer(id);

ALTER TABLE FileSystem
  MODIFY (status CONSTRAINT NN_FileSystem_Status NOT NULL);

ALTER TABLE FileSystem
  MODIFY (diskServer CONSTRAINT NN_FileSystem_DiskServer NOT NULL);

/* DiskServer constraints */
ALTER TABLE DiskServer
  MODIFY (status CONSTRAINT NN_DiskServer_Status NOT NULL);

/* An index to speed up queries in FileQueryRequest, FindRequestRequest, RequestQueryRequest */
CREATE INDEX I_QueryParameter_Query ON QueryParameter (query);

/* An index to speed the queries on Segments by copy */
CREATE INDEX I_Segment_Copy ON Segment (copy);

/* Constraint on FileClass name */
ALTER TABLE FileClass ADD CONSTRAINT UN_FileClass_Name UNIQUE (name);

/* Add unique constraint on tapes */
ALTER TABLE Tape ADD CONSTRAINT UN_Tape_VIDSideTpMode UNIQUE (VID, side, tpMode);

/* Add unique constraint on svcClass name */
ALTER TABLE SvcClass ADD CONSTRAINT UN_SvcClass_Name UNIQUE (NAME);

/* Custom type to handle int arrays */
CREATE OR REPLACE TYPE "numList" IS TABLE OF INTEGER;
/

/* Default policy for migration */
ALTER TABLE TapePool MODIFY (migrSelectPolicy DEFAULT 'defaultMigrSelPolicy');

/* SvcClass constraints */
ALTER TABLE SvcClass
  MODIFY (name CONSTRAINT NN_SvcClass_Name NOT NULL);

ALTER TABLE SvcClass 
  MODIFY (forcedFileClass CONSTRAINT NN_SvcClass_ForcedFileClass NOT NULL);

ALTER TABLE SvcClass MODIFY (gcPolicy DEFAULT 'default');

/* DiskCopy constraints */
ALTER TABLE DiskCopy MODIFY (nbCopyAccesses DEFAULT 0);

ALTER TABLE DiskCopy MODIFY (gcType DEFAULT NULL);

ALTER TABLE DiskCopy ADD CONSTRAINT FK_DiskCopy_CastorFile
  FOREIGN KEY (castorFile) REFERENCES CastorFile (id)
  INITIALLY DEFERRED DEFERRABLE;

/* CastorFile constraints */
ALTER TABLE CastorFile ADD CONSTRAINT FK_CastorFile_SvcClass
  FOREIGN KEY (svcClass) REFERENCES SvcClass (id)
  INITIALLY DEFERRED DEFERRABLE;

ALTER TABLE CastorFile ADD CONSTRAINT FK_CastorFile_FileClass
  FOREIGN KEY (fileClass) REFERENCES FileClass (id)
  INITIALLY DEFERRED DEFERRABLE;

/* Stream constraints */
ALTER TABLE Stream ADD CONSTRAINT FK_Stream_TapePool
  FOREIGN KEY (tapePool) REFERENCES TapePool (id);

/* Index and Constraints for the tapegateway tables */
CREATE INDEX I_TGSubRequest_Request ON TapeGatewaySubRequest(request);
CREATE UNIQUE INDEX I_TGSubRequest_TapeCopy ON TapeGatewaySubRequest(tapeCopy);
CREATE UNIQUE INDEX I_TGSubRequest_DiskCopy ON TapeGatewaySubRequest(diskCopy);
CREATE UNIQUE INDEX I_TGRequest_Tape ON TapeGatewayRequest(tapeRecall);
CREATE UNIQUE INDEX I_TGRequest_Stream ON TapeGatewayRequest(streamMigration);
CREATE UNIQUE INDEX I_TGRequest_VdqmVolReqId ON TapeGatewayRequest(vdqmVolReqId);

ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT FK_TGSubRequest_TC FOREIGN KEY (tapeCopy) REFERENCES TapeCopy (id);
ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT FK_TGSubRequest_DC FOREIGN KEY (tapeCopy) REFERENCES DiskCopy (id);
ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT FK_TGSubRequest_TGR FOREIGN KEY (request) REFERENCES TapeGatewayRequest(id);
ALTER TABLE TapeGatewayRequest ADD CONSTRAINT FK_TGSubRequest_SM FOREIGN KEY (streamMigration) REFERENCES Stream (id);
ALTER TABLE TapeGatewayRequest ADD CONSTRAINT FK_TGSubRequest_TR FOREIGN KEY (tapeRecall) REFERENCES Tape (id);

/* Global temporary table to handle output of the filesDeletedProc procedure */
CREATE GLOBAL TEMPORARY TABLE FilesDeletedProcOutput
  (fileid NUMBER, nshost VARCHAR2(2048))
  ON COMMIT PRESERVE ROWS;

/* Global temporary table to store castor file ids temporarily in the filesDeletedProc procedure */
CREATE GLOBAL TEMPORARY TABLE FilesDeletedProcHelper
  (cfId NUMBER)
  ON COMMIT DELETE ROWS;

/* Global temporary table for the filesClearedProc procedure */
CREATE GLOBAL TEMPORARY TABLE FilesClearedProcHelper
  (cfId NUMBER)
  ON COMMIT DELETE ROWS;

/* Global temporary table to handle output of the nsFilesDeletedProc procedure */
CREATE GLOBAL TEMPORARY TABLE NsFilesDeletedOrphans
  (fileid NUMBER)
  ON COMMIT DELETE ROWS;

/* Global temporary table to handle output of the stgFilesDeletedProc procedure */
CREATE GLOBAL TEMPORARY TABLE StgFilesDeletedOrphans
  (diskCopyId NUMBER)
  ON COMMIT DELETE ROWS;

/* Tables to log the activity performed by the cleanup job */
CREATE TABLE CleanupJobLog
  (fileId NUMBER CONSTRAINT NN_CleanupJobLog_FileId NOT NULL, 
   nsHost VARCHAR2(2048) CONSTRAINT NN_CleanupJobLog_NsHost NOT NULL,
   operation INTEGER CONSTRAINT NN_CleanupJobLog_Operation NOT NULL);
 
/* Temporary table to handle removing of priviledges */
CREATE GLOBAL TEMPORARY TABLE RemovePrivilegeTmpTable
  (svcClass VARCHAR2(2048),
   euid NUMBER,
   egid NUMBER,
   reqType NUMBER)
  ON COMMIT DELETE ROWS;

/* Global temporary table to store ids temporarily in the bulkCreateObj procedures */
CREATE GLOBAL TEMPORARY TABLE BulkSelectHelper
  (objId NUMBER)
  ON COMMIT DELETE ROWS;

/* Global temporary table to store the information on diskcopyies which need to
 * processed to see if too many replicas are online. This temporary table is
 * required to solve the error: `ORA-04091: table is mutating, trigger/function`
 */
CREATE GLOBAL TEMPORARY TABLE TooManyReplicasHelper
  (svcClass NUMBER, castorFile NUMBER)
  ON COMMIT DELETE ROWS;

ALTER TABLE TooManyReplicasHelper 
  ADD CONSTRAINT UN_TooManyReplicasHelp_SVC_CF UNIQUE (svcClass, castorFile);

/* Global temporary table to store subRequest and castorFile ids for cleanup operations.
   See the deleteTerminatedRequest procedure for more details.
 */
CREATE GLOBAL TEMPORARY TABLE DeleteTermReqHelper
  (srId NUMBER, cfId NUMBER)
  ON COMMIT PRESERVE ROWS;

/* Global temporary table used in streamsToDo to temporarily
 * store interesting streams.
 */
CREATE GLOBAL TEMPORARY TABLE StreamsToDoHelper (id NUMBER)
  ON COMMIT DELETE ROWS;

/* SQL statements for table PriorityMap */
CREATE TABLE PriorityMap (euid INTEGER, egid INTEGER, priority INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;
ALTER TABLE PriorityMap ADD CONSTRAINT UN_Priority_euid_egid UNIQUE (euid, egid);

/*
 * Black and while list mechanism
 * In order to be able to enter a request for a given service class, you need :
 *   - to be in the white list for this service class
 *   - to not be in the black list for this services class
 * Being in a list means :
 *   - either that your uid,gid is explicitely in the list
 *   - or that your gid is in the list with null uid (that is group wildcard)
 *   - or there is an entry with null uid and null gid (full wild card)
 * The permissions can also have a request type. Default is null, that is everything.
 * By default anybody can do anything
 */
CREATE TABLE WhiteList (svcClass VARCHAR2(2048), euid NUMBER, egid NUMBER, reqType NUMBER);
CREATE TABLE BlackList (svcClass VARCHAR2(2048), euid NUMBER, egid NUMBER, reqType NUMBER);

INSERT INTO WhiteList VALUES (NULL, NULL, NULL, NULL);


/* Define the service handlers for the appropriate sets of stage request objects */
UPDATE Type2Obj SET svcHandler = 'JobReqSvc' WHERE type IN (35, 40, 44);
UPDATE Type2Obj SET svcHandler = 'PrepReqSvc' WHERE type IN (36, 37, 38, 119);
UPDATE Type2Obj SET svcHandler = 'StageReqSvc' WHERE type IN (39, 42, 95);
UPDATE Type2Obj SET svcHandler = 'QueryReqSvc' WHERE type IN (33, 34, 41, 103, 131, 152, 155);
UPDATE Type2Obj SET svcHandler = 'JobSvc' WHERE type IN (60, 64, 65, 67, 78, 79, 80, 93, 144, 147);
UPDATE Type2Obj SET svcHandler = 'GCSvc' WHERE type IN (73, 74, 83, 142, 149);

/* Set default values for the StageDiskCopyReplicaRequest table */
ALTER TABLE StageDiskCopyReplicaRequest MODIFY flags DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY euid DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY egid DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY mask DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY pid DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY machine DEFAULT 'stager';

/* Indexing StageDiskCopyReplicaRequest by source diskcopy id */
CREATE INDEX I_StageDiskCopyReplic_SourceDC 
  ON StageDiskCopyReplicaRequest (sourceDiskCopy);

/* Define a table for some configuration key-value pairs and populate it */
CREATE TABLE CastorConfig
  (class VARCHAR2(2048) CONSTRAINT NN_CastorConfig_Class NOT NULL, 
   key VARCHAR2(2048) CONSTRAINT NN_CastorConfig_Key NOT NULL, 
   value VARCHAR2(2048) CONSTRAINT NN_CastorConfig_Value NOT NULL, 
   description VARCHAR2(2048));

ALTER TABLE CastorConfig ADD CONSTRAINT UN_CastorConfig_class_key UNIQUE (class, key);

INSERT INTO CastorConfig
  VALUES ('general', 'instance', 'castorstager', 'Name of this Castor instance');
INSERT INTO CastorConfig
  VALUES ('general', 'owner', 'castor_stager', 'The database owner of the schema');

INSERT INTO CastorConfig
  VALUES ('cleaning', 'terminatedRequestsTimeout', '120', 'Maximum timeout for successful and failed requests in hours');
INSERT INTO CastorConfig
  VALUES ('cleaning', 'outOfDateStageOutDCsTimeout', '72', 'Timeout for STAGEOUT diskCopies in hours');
INSERT INTO CastorConfig
  VALUES ('cleaning', 'failedDCsTimeout', '72', 'Timeout for failed diskCopies in hours');

INSERT INTO CastorConfig
  VALUES ('stager', 'nsHost', 'undefined', 'The name of the name server host to set in the CastorFile table overriding the CNS/HOST option defined in castor.conf');

/* Populate the general/owner option of the CastorConfig table */
BEGIN
  UPDATE CastorConfig 
     SET value = sys_context('USERENV', 'CURRENT_USER')
   WHERE class = 'general'
     AND key = 'owner';
END;
/

/* Drop the tapegateway tables created in the oracleSchema until such a time
 * that the tables are needed by a release
 */
DROP TABLE TapeGatewaySubRequest;
DROP TABLE TapeGatewayRequest;

COMMIT;


/*********************************************************************/
/* FileSystemsToCheck used to optimise the processing of filesystems */
/* when they change status                                           */
/*********************************************************************/
CREATE TABLE FileSystemsToCheck (FileSystem NUMBER CONSTRAINT PK_FSToCheck_FS PRIMARY KEY, ToBeChecked NUMBER);
INSERT INTO FileSystemsToCheck SELECT id, 0 FROM FileSystem;


/**************/
/* Accounting */
/**************/

/* WARNING!!!! Changing this to a materialized view which is refresh at a set
 * frequency causes problems with the disk server draining tools.
 */
CREATE TABLE Accounting (euid INTEGER CONSTRAINT NN_Accounting_Euid NOT NULL, 
                         fileSystem INTEGER CONSTRAINT NN_Accounting_Filesystem NOT NULL,
                         nbBytes INTEGER);
ALTER TABLE Accounting 
ADD CONSTRAINT PK_Accounting_EuidFs PRIMARY KEY (euid, fileSystem);

/* SQL statement for the creation of the AccountingSummary view */
CREATE OR REPLACE VIEW AccountingSummary
AS
  SELECT (SELECT cast(last_start_date AS DATE) 
            FROM dba_scheduler_jobs
           WHERE job_name = 'ACCOUNTINGJOB'
             AND owner = 
              (SELECT value FROM CastorConfig
                WHERE class = 'general' AND key = 'owner')) timestamp,
         3600 interval, SvcClass.name SvcClass, Accounting.euid, 
         sum(Accounting.nbbytes) totalBytes
    FROM Accounting, FileSystem, DiskPool2SvcClass, svcclass
   WHERE Accounting.filesystem = FileSystem.id
     AND FileSystem.diskpool = DiskPool2SvcClass.parent
     AND DiskPool2SvcClass.child = SvcClass.id
   GROUP BY SvcClass.name, Accounting.euid
   ORDER BY SvcClass.name, Accounting.euid;


/************************************/
/* Garbage collection related table */
/************************************/

/* A table storing the Gc policies and detailing there configuration
 * For each policy, identified by a name, parameters are :
 *   - userWeight : the name of the PL/SQL function to be called to
 *     precompute the GC weight when a file is written by the user.
 *   - recallWeight : the name of the PL/SQL function to be called to
 *     precompute the GC weight when a file is recalled
 *   - copyWeight : the name of the PL/SQL function to be called to
 *     precompute the GC weight when a file is disk to disk copied
 *   - firstAccessHook : the name of the PL/SQL function to be called
 *     when the file is accessed for the first time. Can be NULL.
 *   - accessHook : the name of the PL/SQL function to be called
 *     when the file is accessed (except for the first time). Can be NULL.
 *   - userSetGCWeight : the name of the PL/SQL function to be called
 *     when a setFileGcWeight user request is processed can be NULL.
 * All functions return a number that is the new gcWeight.
 * In general, here are the signatures :
 *   userWeight(fileSize NUMBER, DiskCopyStatus NUMBER)
 *   recallWeight(fileSize NUMBER)
 *   copyWeight(fileSize NUMBER, DiskCopyStatus NUMBER, sourceWeight NUMBER))
 *   firstAccessHook(oldGcWeight NUMBER, creationTime NUMBER)
 *   accessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER)
 *   userSetGCWeight(oldGcWeight NUMBER, userDelta NUMBER)
 * Few notes :
 *   diskCopyStatus can be STAGED(0) or CANBEMIGR(10)
 */
CREATE TABLE GcPolicy (name VARCHAR2(2048) CONSTRAINT NN_GcPolicy_Name NOT NULL CONSTRAINT PK_GcPolicy_Name PRIMARY KEY,
                       userWeight VARCHAR2(2048) CONSTRAINT NN_GcPolicy_UserWeight NOT NULL,
                       recallWeight VARCHAR2(2048) CONSTRAINT NN_GcPolicy_RecallWeight NOT NULL,
                       copyWeight VARCHAR2(2048) CONSTRAINT NN_GcPolicy_CopyWeight NOT NULL,
                       firstAccessHook VARCHAR2(2048) DEFAULT NULL,
                       accessHook VARCHAR2(2048) DEFAULT NULL,
                       userSetGCWeight VARCHAR2(2048) DEFAULT NULL);

/* Default policy, mainly based on file sizes */
INSERT INTO GcPolicy VALUES ('default',
                             'castorGC.sizeRelatedUserWeight',
                             'castorGC.sizeRelatedRecallWeight',
                             'castorGC.sizeRelatedCopyWeight',
                             'castorGC.dayBonusFirstAccessHook',
                             'castorGC.halfHourBonusAccessHook',
                             'castorGC.cappedUserSetGCWeight');
INSERT INTO GcPolicy VALUES ('FIFO',
                             'castorGC.creationTimeUserWeight',
                             'castorGC.creationTimeRecallWeight',
                             'castorGC.creationTimeCopyWeight',
                             NULL,
                             NULL,
                             NULL);
INSERT INTO GcPolicy VALUES ('LRU',
                             'castorGC.creationTimeUserWeight',
                             'castorGC.creationTimeRecallWeight',
                             'castorGC.creationTimeCopyWeight',
                             'castorGC.LRUFirstAccessHook',
                             'castorGC.LRUAccessHook',
                             NULL);


/*********************/
/* FileSystem rating */
/*********************/

/* Computes a 'rate' for the filesystem which is an agglomeration
   of weight and fsDeviation. The goal is to be able to classify
   the fileSystems using a single value and to put an index on it */
CREATE OR REPLACE FUNCTION fileSystemRate
(readRate IN NUMBER,
 writeRate IN NUMBER,
 nbReadStreams IN NUMBER,
 nbWriteStreams IN NUMBER,
 nbReadWriteStreams IN NUMBER,
 nbMigratorStreams IN NUMBER,
 nbRecallerStreams IN NUMBER)
RETURN NUMBER DETERMINISTIC IS
BEGIN
  RETURN - nbReadStreams - nbWriteStreams - nbReadWriteStreams - nbMigratorStreams - nbRecallerStreams;
END;
/

/* FileSystem index based on the rate. */
CREATE INDEX I_FileSystem_Rate
    ON FileSystem(fileSystemRate(readRate, writeRate,
	          nbReadStreams,nbWriteStreams, nbReadWriteStreams, nbMigratorStreams, nbRecallerStreams));
/*******************************************************************
 * @(#)RCSfile: oracleDrain.schema.sql,v  Revision: 1.4  Date: 2009/07/05 13:49:08  Author: waldron 
 * Schema creation code for Draining FileSystems Logic
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* SQL statement for the creation of the DrainingFileSystem table */
CREATE TABLE DrainingFileSystem
  (userName       VARCHAR2(30) CONSTRAINT NN_DrainingFs_UserName NOT NULL,
   machine        VARCHAR2(500) CONSTRAINT NN_DrainingFs_Machine NOT NULL,
   creationTime   NUMBER DEFAULT 0,
   startTime      NUMBER DEFAULT 0,
   lastUpdateTime NUMBER DEFAULT 0,
   fileSystem     NUMBER CONSTRAINT NN_DrainingFs_FileSystem NOT NULL,
   /* Current state of the draining process, one of:
    *   0 -- CREATED
    *   1 -- INITIALIZING
    *   2 -- RUNNING
    *   3 -- INTERRUPTED
    *   4 -- FAILED
    *   5 -- COMPLETED
    *   6 -- DELETING
    */
   status         NUMBER DEFAULT 0,
   svcClass       NUMBER CONSTRAINT NN_DrainingFs_SvcClass NOT NULL,
   /* Flag to indicate whether files should be invalidated so that they can be
    * removed by the garbage collection process after a file is replicated to
    * another diskserver.
    */
   autoDelete     NUMBER DEFAULT 0,
   /* Column to indicate which files should be replicated. Valid values are:
    *   0 -- STAGED,
    *   1 -- CANBEMIGR
    *   2 -- ALL
    */
   fileMask       NUMBER DEFAULT 1,
   /* The maximum number of current transfers (job slots) available for draining
    * the filesystem.
    */
   maxTransfers   NUMBER DEFAULT 50,
   totalFiles     NUMBER DEFAULT 0,
   totalBytes     NUMBER DEFAULT 0)
  /* Allow shrink operations */
  ENABLE ROW MOVEMENT;

/* SQL statement for primary key constraint on DrainingFileSystem */
ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT PK_DrainingFs_FileSystem
  PRIMARY KEY (fileSystem);

/* SQL statements for check constraints on the DrainingFileSystem table */
ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT CK_DrainingFs_Status
  CHECK (status IN (0, 1, 2, 3, 4, 5, 6));

ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT CK_DrainingFs_FileMask
  CHECK (fileMask IN (0, 1, 2));

ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT CK_DrainingFs_AutoDelete
  CHECK (autoDelete IN (0, 1));

ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT CK_DrainingFs_MaxTransfers
  CHECK (maxTransfers > 0);

/* SQL statements for foreign key constraints on DrainingFileSystem */
ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT FK_DrainingFs_SvcClass
  FOREIGN KEY (svcClass)
  REFERENCES SvcClass (id);

ALTER TABLE DrainingFileSystem
  ADD CONSTRAINT FK_DrainingFs_FileSystem
  FOREIGN KEY (fileSystem)
  REFERENCES FileSystem (id);

/* SQL statements for indexes on DrainingFileSystem table */
CREATE INDEX I_DrainingFileSystem_SvcClass
  ON DrainingFileSystem (svcClass);


/* SQL statements for the creation of the DrainingDiskCopy table
 *
 * The way the logic for draining a filesystems works is to essentially create
 * a list of all the files that need to be replicated to other diskservers and
 * to process that list until all files have been replicated.
 *
 * This list/queue could have been done with Oracle Advanced Queuing (AQ).
 * However, due to the complexities of setting it up and the lack of prior
 * experience on behalf of the CASTOR developers and CERN DBA's we create a
 * simple queue using a standard table.
 */
CREATE TABLE DrainingDiskCopy
  (fileSystem     NUMBER CONSTRAINT NN_DrainingDCs_FileSystem NOT NULL,
   /* Status of the diskcopy to be replicated. Note: this is not the same as
    * the status of the diskcopy i.e. STAGED, CANBEMIGR. It is an internal
    * status assigned to each diskcopy (file) as a means of tracking how far the
    * file is in the lifecycle of draining a filesystem.
    * The status can be one of:
    *   0 -- CREATED
    *   1 -- RESTARTED
    *   2 -- PROCESSING    (Transient state)
    *   3 -- WAITD2D
    *   4 -- FAILED
    */
   status         NUMBER DEFAULT 0 CONSTRAINT NN_DrainingDCs_Status NOT NULL,
   /* A link to the diskcopy. Note: this is deliberately not enforced with a
    * foreign key constraint!!!
    */
   diskCopy       NUMBER CONSTRAINT NN_DrainingDCs_DiskCopy NOT NULL,
   parent         NUMBER DEFAULT 0 CONSTRAINT NN_DrainingDCs_Parent NOT NULL,
   creationTime   NUMBER DEFAULT 0,
   priority       NUMBER DEFAULT 0,
   fileSize       NUMBER DEFAULT 0 CONSTRAINT NN_DrainingDCs_FileSize NOT NULL,
   comments       VARCHAR2(2048) DEFAULT NULL)
  /* Allow shrink operations */
  ENABLE ROW MOVEMENT;

/* SQL statement for primary key constraint on DrainingDiskCopy */
ALTER TABLE DrainingDiskCopy
  ADD CONSTRAINT PK_DrainingDCs_DiskCopy
  PRIMARY KEY (diskCopy);

/* SQL statement for check constraints on the DrainingDiskCopy table */
ALTER TABLE DrainingDiskCopy
  ADD CONSTRAINT CK_DrainingDCs_Status
  CHECK (status IN (0, 1, 2, 3, 4));

/* SQL statement for foreign key constraints on DrainingDiskCopy */
ALTER TABLE DrainingDiskCopy
  ADD CONSTRAINT FK_DrainingDCs_FileSystem
  FOREIGN KEY (fileSystem)
  REFERENCES DrainingFileSystem (fileSystem);

/* SQL statements for indexes on DrainingDiskCopy table */
CREATE INDEX I_DrainingDCs_FileSystem
  ON DrainingDiskCopy (fileSystem);

CREATE INDEX I_DrainingDCs_Status
  ON DrainingDiskCopy (status);

/* This index is essentially the same as the one on the SubRequest table which
 * allows us to process entries in order. In this case by priority and
 * creationTime.
 */
CREATE INDEX I_DrainingDCs_PC
  ON DrainingDiskCopy (priority, creationTime);

CREATE INDEX I_DrainingDCs_Parent
  ON DrainingDiskCopy (parent);
/*******************************************************************
 * @(#)RCSfile: oracleMonitoring.schema.sql,v  Revision: 1.6  Date: 2009/07/05 13:46:14  Author: waldron 
 * Schema creation code for Monitoring tables
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* SQL statement for table MonDiskCopyStats */
CREATE TABLE MonDiskCopyStats
  (timestamp DATE, interval NUMBER, diskServer VARCHAR2(255), mountPoint VARCHAR2(255), dsStatus VARCHAR2(50), fsStatus VARCHAR2(50), available VARCHAR2(2), status VARCHAR2(100), totalFileSize NUMBER, nbFiles NUMBER);

/* SQL statement for table MonWaitTapeMigrationStats */
CREATE TABLE MonWaitTapeMigrationStats
  (timestamp DATE, interval NUMBER, svcClass VARCHAR2(255), status VARCHAR2(10), minWaitTime NUMBER, maxWaitTime NUMBER, avgWaitTime NUMBER, minFileSize NUMBER, maxFileSize NUMBER, avgFileSize NUMBER, bin_LT_1 NUMBER, bin_1_To_6 NUMBER, bin_6_To_12 NUMBER, bin_12_To_24 NUMBER, bin_24_To_48 NUMBER, bin_GT_48 NUMBER, totalFileSize NUMBER, nbFiles NUMBER);

/* SQL statement for table MonWaitTapeRecallStats  */
CREATE TABLE MonWaitTapeRecallStats
  (timestamp DATE, interval NUMBER, svcClass VARCHAR2(255), minWaitTime NUMBER, maxWaitTime NUMBER, avgWaitTime NUMBER, minFileSize NUMBER, maxFileSize NUMBER, avgFileSize NUMBER, bin_LT_1 NUMBER, bin_1_To_6 NUMBER, bin_6_To_12 NUMBER, bin_12_To_24 NUMBER, bin_24_To_48 NUMBER, bin_GT_48 NUMBER, totalFileSize NUMBER, nbFiles NUMBER);

/*******************************************************************
 *
 * @(#)RCSfile: oracleCommon.sql,v  Revision: 1.697  Date: 2009/08/13 14:11:33  Author: itglp 
 *
 * This file contains some common PL/SQL utilities for the stager database.
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/


/* Get current time as a time_t. Not that easy in ORACLE */
CREATE OR REPLACE FUNCTION getTime RETURN NUMBER IS
  epoch            TIMESTAMP WITH TIME ZONE;
  now              TIMESTAMP WITH TIME ZONE;
  interval         INTERVAL DAY(9) TO SECOND;
  interval_days    NUMBER;
  interval_hours   NUMBER;
  interval_minutes NUMBER;
  interval_seconds NUMBER;
BEGIN
  epoch := TO_TIMESTAMP_TZ('01-JAN-1970 00:00:00 00:00',
    'DD-MON-YYYY HH24:MI:SS TZH:TZM');
  now := SYSTIMESTAMP AT TIME ZONE '00:00';
  interval         := now - epoch;
  interval_days    := EXTRACT(DAY    FROM (interval));
  interval_hours   := EXTRACT(HOUR   FROM (interval));
  interval_minutes := EXTRACT(MINUTE FROM (interval));
  interval_seconds := EXTRACT(SECOND FROM (interval));

  RETURN interval_days * 24 * 60 * 60 + interval_hours * 60 * 60 +
    interval_minutes * 60 + interval_seconds;
END;
/


/* Generate a universally unique id (UUID) */
CREATE OR REPLACE FUNCTION uuidGen RETURN VARCHAR2 IS
  ret VARCHAR2(36);
BEGIN
  -- Note: the guid generator provided by ORACLE produces sequential uuid's, not
  -- random ones. The reason for this is because random uuid's are not good for
  -- indexing!
  RETURN lower(regexp_replace(sys_guid(), '(.{8})(.{4})(.{4})(.{4})(.{12})', '\1-\2-\3-\4-\5'));
END;
/


/* Function to check if a service class exists by name. This function can return
 * the id of the named service class or raise an application error if it does
 * not exist.
 * @param svcClasName The name of the service class (Note: can be NULL)
 * @param allowNull   Flag to indicate whether NULL or '' service class names are
 *                    permitted.
 * @param raiseError  Flag to indicate whether the function should raise an
 *                    application error when the service class doesn't exist or
 *                    return a boolean value of 0.
 */
CREATE OR REPLACE FUNCTION checkForValidSvcClass
(svcClassName VARCHAR2, allowNull NUMBER, raiseError NUMBER) RETURN NUMBER IS
  ret NUMBER;
BEGIN
  -- Check if the service class name is allowed to be NULL. This is quite often
  -- the case if the calling function supports '*' (null) to indicate that all
  -- service classes are being targeted. Nevertheless, in such a case we
  -- return the id of the default one.
  IF svcClassName IS NULL OR length(svcClassName) IS NULL THEN
    IF allowNull = 1 THEN
      SELECT id INTO ret FROM SvcClass WHERE name = 'default';
      RETURN ret;
    END IF;
  END IF;
  -- We do accept '*' as being valid, as it is the wildcard
  IF SvcClassName = '*' THEN
    RETURN 1;
  END IF;
  -- Check to see if service class exists by name and return its id
  BEGIN
    SELECT id INTO ret FROM SvcClass WHERE name = svcClassName;
    RETURN ret;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- If permitted to do so raise an application error if the service class does
    -- not exist
    IF raiseError = 1 THEN
      raise_application_error(-20113, 'Invalid service class ''' || svcClassName || '''');
    END IF;
    RETURN 0;
  END;
END;
/


/* Function to return a comma separate list of service classes that a
 * filesystem belongs to.
 */
CREATE OR REPLACE FUNCTION getSvcClassList(fsId NUMBER) RETURN VARCHAR2 IS
  svcClassList VARCHAR2(4000) := NULL;
  c INTEGER := 0;
BEGIN
  FOR a IN (SELECT Distinct(SvcClass.name)
              FROM FileSystem, DiskPool2SvcClass, SvcClass
             WHERE FileSystem.id = fsId
               AND FileSystem.diskpool = DiskPool2SvcClass.parent
               AND DiskPool2SvcClass.child = SvcClass.id
             ORDER BY SvcClass.name)
  LOOP
    svcClassList := svcClassList || ',' || a.name;
    c := c + 1;
    IF c = 5 THEN
      svcClassList := svcClassList || ',...';
      EXIT;
    END IF;
  END LOOP;
  RETURN ltrim(svcClassList, ',');
END;
/


/* Function to extract a configuration option from the castor config
 * table.
 */
CREATE OR REPLACE FUNCTION getConfigOption
(className VARCHAR2, optionName VARCHAR2, defaultValue VARCHAR2) 
RETURN VARCHAR2 IS
  returnValue VARCHAR2(2048) := defaultValue;
BEGIN
  SELECT value INTO returnValue
    FROM CastorConfig
   WHERE class = className
     AND key = optionName
     AND value != 'undefined';
  RETURN returnValue;
EXCEPTION WHEN NO_DATA_FOUND THEN
  RETURN returnValue;
END;
/

/* Function to normalize a filepath, i.e. to drop multiple '/'s and resolve any '..' */
CREATE OR REPLACE FUNCTION normalizePath(path IN VARCHAR2) RETURN VARCHAR2 IS
  buf VARCHAR2(2048);
  ret VARCHAR2(2048);
BEGIN
  -- drop '.'s and multiple '/'s
  ret := replace(regexp_replace(path, '[/]+', '/'), '/./', '/');
  LOOP
    buf := ret;
    -- a path component is by definition anything between two slashes, except
    -- the '..' string itself. This is not taken into account, resulting in incorrect
    -- parsing when relative paths are used (see bug #49002). We're not concerned by
    -- those cases; however this code could be fixed and improved by using string
    -- tokenization as opposed to expensive regexp parsing.
    ret := regexp_replace(buf, '/[^/]+/\.\./', '/');
    EXIT WHEN ret = buf;
  END LOOP;
  RETURN ret;
END;
/


/* PL/SQL method deleting tapecopies (and segments) of a castorfile */
CREATE OR REPLACE PROCEDURE deleteTapeCopies(cfId NUMBER) AS
BEGIN
  -- Loop over the tapecopies
  FOR t IN (SELECT id FROM TapeCopy WHERE castorfile = cfId) LOOP
    FOR s IN (SELECT id FROM Segment WHERE copy = t.id) LOOP
    -- Delete the segment(s)
      DELETE FROM Id2Type WHERE id = s.id;
      DELETE FROM Segment WHERE id = s.id;
    END LOOP;
    -- Delete from Stream2TapeCopy
    DELETE FROM Stream2TapeCopy WHERE child = t.id;
    -- Delete the TapeCopy
    DELETE FROM Id2Type WHERE id = t.id;
    DELETE FROM TapeCopy WHERE id = t.id;
  END LOOP;
END;
/

/* PL/SQL method canceling a given recall */
CREATE OR REPLACE PROCEDURE cancelRecall
(cfId NUMBER, dcId NUMBER, newSubReqStatus NUMBER) AS
  srIds "numList";
  unused NUMBER;
BEGIN
  -- Lock the CastorFile
  SELECT id INTO unused FROM CastorFile
   WHERE id = cfId FOR UPDATE;
  -- Cancel the recall
  deleteTapeCopies(cfId);
  -- Invalidate the DiskCopy
  UPDATE DiskCopy SET status = 7 WHERE id = dcId; -- INVALID
  -- Look for request associated to the recall and fail
  -- it and all the waiting ones
  UPDATE SubRequest SET status = newSubReqStatus
   WHERE diskCopy = dcId RETURNING id BULK COLLECT INTO srIds;
  UPDATE SubRequest
     SET status = newSubReqStatus, parent = 0 -- FAILED
   WHERE status = 5 -- WAITSUBREQ
     AND parent IN
       (SELECT /*+ CARDINALITY(sridTable 5) */ *
          FROM TABLE(srIds) sridTable)
     AND castorfile = cfId;
END;
/

/* PL/SQL method FOR canceling a recall by tape VID, The subrequests associated with
   the recall with be FAILED */
CREATE OR REPLACE PROCEDURE cancelRecallForTape (vid IN VARCHAR2) AS
BEGIN
  FOR a IN (SELECT DISTINCT(DiskCopy.id), DiskCopy.castorfile
              FROM Segment, Tape, TapeCopy, DiskCopy
             WHERE Segment.tape = Tape.id
               AND Segment.copy = TapeCopy.id
               AND DiskCopy.castorfile = TapeCopy.castorfile
               AND DiskCopy.status = 2  -- WAITTAPERECALL
               AND Tape.vid = vid
             ORDER BY DiskCopy.id ASC)
  LOOP
    cancelRecall(a.castorfile, a.id, 7);
  END LOOP;
END;
/


/* PL/SQL method to delete a CastorFile only when no Disk|TapeCopies are left for it */
/* Internally used in filesDeletedProc, putFailedProc and deleteOutOfDateDiskCopies */
CREATE OR REPLACE PROCEDURE deleteCastorFile(cfId IN NUMBER) AS
  nb NUMBER;
  LockError EXCEPTION;
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  -- First try to lock the castorFile
  SELECT id INTO nb FROM CastorFile
   WHERE id = cfId FOR UPDATE NOWAIT;
  -- See whether it has any DiskCopy
  SELECT count(*) INTO nb FROM DiskCopy
   WHERE castorFile = cfId;
  -- If any DiskCopy, give up
  IF nb = 0 THEN
    -- See whether it has any TapeCopy
    SELECT count(*) INTO nb FROM TapeCopy
     WHERE castorFile = cfId AND status != 6; -- FAILED
    -- If any TapeCopy, give up
    IF nb = 0 THEN
      -- See whether pending SubRequests exist
      SELECT count(*) INTO nb FROM SubRequest
       WHERE castorFile = cfId
         AND status IN (0, 1, 2, 3, 4, 5, 6, 7, 10, 12, 13, 14);   -- All but FINISHED, FAILED_FINISHED, ARCHIVED
      -- If any SubRequest, give up
      IF nb = 0 THEN
        DECLARE
          fid NUMBER;
          fc NUMBER;
          nsh VARCHAR2(2048);
        BEGIN
          -- Delete the failed TapeCopies
          deleteTapeCopies(cfId);
          -- Delete the CastorFile
          DELETE FROM id2Type WHERE id = cfId;
          DELETE FROM CastorFile WHERE id = cfId
            RETURNING fileId, nsHost, fileClass
            INTO fid, nsh, fc;
          -- check whether this file potentially had TapeCopies
          SELECT nbCopies INTO nb FROM FileClass WHERE id = fc;
          IF nb = 0 THEN
            -- This castorfile was created with no TapeCopy
            -- So removing it from the stager means erasing
            -- it completely. We should thus also remove it
            -- from the name server
            INSERT INTO FilesDeletedProcOutput VALUES (fid, nsh);
          END IF;
        END;
      END IF;
    END IF;
  END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- ignore, this means that the castorFile did not exist.
  -- There is thus no way to find out whether to remove the
  -- file from the nameserver. For safety, we thus keep it
  NULL;
WHEN LockError THEN
  -- ignore, somebody else is dealing with this castorFile, 
  NULL;
END;
/
/*******************************************************************
 *
 * @(#)RCSfile: oraclePerm.sql,v  Revision: 1.655  Date: 2009/03/26 14:11:58  Author: itglp 
 *
 * PL/SQL code for permission and B/W list handling
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/


/* PL/SQL method implementing checkPermission */
CREATE OR REPLACE PROCEDURE checkPermission(isvcClass IN VARCHAR2,
                                            ieuid IN NUMBER,
                                            iegid IN NUMBER,
                                            ireqType IN NUMBER,
                                            res OUT NUMBER) AS
  c NUMBER;
  svcId NUMBER;
  reqName VARCHAR2(100);
BEGIN
  -- First resolve the service class
  svcId := checkForValidSvcClass(isvcClass, 1, 0);
  -- Perform the check
  SELECT count(*) INTO c
    FROM WhiteList
   WHERE (svcClass = isvcClass OR svcClass IS NULL
          OR (length(isvcClass) IS NULL AND svcClass = 'default'))
     AND (egid = iegid OR egid IS NULL)
     AND (euid = ieuid OR euid IS NULL)
     AND (reqType = ireqType OR reqType IS NULL);
  IF c = 0 THEN
    -- Not found in White list -> no access
    IF svcId > 0 THEN
      -- Service class exists, we give permission denied
      res := -1;
    -- Special case where we accept '*' as a service class for Qry,
    -- DiskPoolQuery and RM requests.
    ELSIF isvcClass = '*' AND 
          (ireqType = 33 OR ireqType = 42 OR ireqType = 103) THEN
      res := -1;
    ELSE
      -- Service class does not exist
      res := -2;
    END IF;
  ELSE
    SELECT count(*) INTO c
      FROM BlackList
     WHERE (svcClass = isvcClass OR svcClass IS NULL
            OR (length(isvcClass) IS NULL AND svcClass = 'default'))
       AND (egid = iegid OR egid IS NULL)
       AND (euid = ieuid OR euid IS NULL)
       AND (reqType = ireqType OR reqType IS NULL);
    IF c = 0 THEN
      -- Not Found in Black list -> access
      -- in this case return the service class id
      res := svcId;
    ELSE
      -- Found in Black list -> no access
      res := -1;
    END IF;
  END IF;
END;
/


/* Function to wrap the checkPermission procedure so that is can be
 * used within SQL queries. The function returns 0 if the user has
 * access on the service class for the given request type otherwise
 * 1 if access is denied
 */
CREATE OR REPLACE
FUNCTION checkPermissionOnSvcClass(reqSvcClass IN VARCHAR2,
                                   reqEuid IN NUMBER,
                                   reqEgid IN NUMBER,
                                   reqType IN NUMBER)
RETURN NUMBER AS
  res NUMBER;
BEGIN
  -- Skip access control checks for special internal users
  IF reqEuid = -1 AND reqEgid = -1 THEN
    RETURN 0;
  END IF;
  -- Check the users access rights
  checkPermission(reqSvcClass, reqEuid, reqEgid, reqType, res);
  IF res > 0 THEN
    RETURN 0;
  END IF;
  RETURN 1;
END;
/


/**
  * Black and while list mechanism
  * In order to be able to enter a request for a given service class, you need :
  *   - to be in the white list for this service class
  *   - to not be in the black list for this services class
  * Being in a list means :
  *   - either that your uid,gid is explicitely in the list
  *   - or that your gid is in the list with null uid (that is group wildcard)
  *   - or there is an entry with null uid and null gid (full wild card)
  * The permissions can also have a request type. Default is null, that is everything
  */
CREATE OR REPLACE PACKAGE castorBW AS
  -- defines a privilege
  TYPE Privilege IS RECORD (
    svcClass VARCHAR2(2048),
    euid NUMBER,
    egid NUMBER,
    reqType NUMBER);
  -- defines a privilege, plus a "direction"
  TYPE PrivilegeExt IS RECORD (
    svcClass VARCHAR2(2048),
    euid NUMBER,
    egid NUMBER,
    reqType NUMBER,
    isGranted NUMBER);
  -- a cursor of privileges
  TYPE PrivilegeExt_Cur IS REF CURSOR RETURN PrivilegeExt;
  -- Intersection of 2 privileges
  -- raises -20109, "Empty privilege" in case the intersection is empty
  FUNCTION intersection(p1 IN Privilege, p2 IN Privilege) RETURN Privilege;
  -- Does one privilege P1 contain another one P2 ?
  FUNCTION contains(p1 Privilege, p2 Privilege) RETURN Boolean;
  -- Intersection of a privilege P with the WhiteList
  -- The result is stored in the temporary table removePrivilegeTmpTable
  -- that is cleaned up when the procedure starts
  PROCEDURE intersectionWithWhiteList(p Privilege);
  -- Difference between privilege P1 and privilege P2
  -- raises -20108, "Invalid privilege intersection" in case the difference
  -- can not be computed
  -- raises -20109, "Empty privilege" in case the difference is empty
  FUNCTION diff(P1 Privilege, P2 Privilege) RETURN Privilege;
  -- remove privilege P from list L
  PROCEDURE removePrivilegeFromBlackList(p Privilege);
  -- Add privilege P to WhiteList
  PROCEDURE addPrivilegeToWL(p Privilege);
  -- Add privilege P to BlackList
  PROCEDURE addPrivilegeToBL(p Privilege);
  -- cleanup BlackList after privileges were removed from the whitelist
  PROCEDURE cleanupBL;
  -- Add privilege P
  PROCEDURE addPrivilege(P Privilege);
  -- Remove privilege P
  PROCEDURE removePrivilege(P Privilege);
  -- Add privilege(s)
  PROCEDURE addPrivilege(svcClassName VARCHAR2, euid NUMBER, egid NUMBER, reqType NUMBER);
  -- Remove privilege(S)
  PROCEDURE removePrivilege(svcClassName VARCHAR2, euid NUMBER, egid NUMBER, reqType NUMBER);
  -- List privilege(s)
  PROCEDURE listPrivileges(svcClassName IN VARCHAR2, ieuid IN NUMBER,
                           iegid IN NUMBER, ireqType IN NUMBER,
                           plist OUT PrivilegeExt_Cur);
END castorBW;
/

CREATE OR REPLACE PACKAGE BODY castorBW AS

  -- Intersection of 2 privileges
  FUNCTION intersection(p1 IN Privilege, p2 IN Privilege)
  RETURN Privilege AS
    res Privilege;
  BEGIN
    IF p1.euid IS NULL OR p1.euid = p2.euid THEN
      res.euid := p2.euid;
    ELSIF p2.euid IS NULL THEN
      res.euid := p1.euid;
    ELSE
      raise_application_error(-20109, 'Empty privilege');
    END IF;
    IF p1.egid IS NULL OR p1.egid = p2.egid THEN
      res.egid := p2.egid;
    ELSIF p2.egid IS NULL THEN
      res.egid := p1.egid;
    ELSE
      raise_application_error(-20109, 'Empty privilege');
    END IF;
    IF p1.svcClass IS NULL OR p1.svcClass = p2.svcClass THEN
      res.svcClass := p2.svcClass;
    ELSIF p2.svcClass IS NULL THEN
      res.svcClass := p1.svcClass;
    ELSE
      raise_application_error(-20109, 'Empty privilege');
    END IF;
    IF p1.reqType IS NULL OR p1.reqType = p2.reqType THEN
      res.reqType := p2.reqType;
    ELSIF p2.reqType IS NULL THEN
      res.reqType := p1.reqType;
    ELSE
      raise_application_error(-20109, 'Empty privilege');
    END IF;
    RETURN res;
  END;

  -- Does one privilege P1 contain another one P2 ?
  FUNCTION contains(p1 Privilege, p2 Privilege) RETURN Boolean AS
  BEGIN
    IF p1.euid IS NOT NULL -- p1 NULL means it contains everything !
       AND (p2.euid IS NULL OR p1.euid != p2.euid) THEN
      RETURN FALSE;
    END IF;
    IF p1.egid IS NOT NULL -- p1 NULL means it contains everything !
       AND (p2.egid IS NULL OR p1.egid != p2.egid) THEN
      RETURN FALSE;
    END IF;
    IF p1.svcClass IS NOT NULL -- p1 NULL means it contains everything !
       AND (p2.svcClass IS NULL OR p1.svcClass != p2.svcClass) THEN
      RETURN FALSE;
    END IF;
    IF p1.reqType IS NOT NULL -- p1 NULL means it contains everything !
       AND (p2.reqType IS NULL OR p1.reqType != p2.reqType) THEN
      RETURN FALSE;
    END IF;
    RETURN TRUE;
  END;

  -- Intersection of a privilege P with the WhiteList
  -- The result is stored in the temporary table removePrivilegeTmpTable
  PROCEDURE intersectionWithWhiteList(p Privilege) AS
    wlr Privilege;
    tmp Privilege;
    empty_privilege EXCEPTION;
    PRAGMA EXCEPTION_INIT(empty_privilege, -20109);
  BEGIN
    DELETE FROM RemovePrivilegeTmpTable;
    FOR r IN (SELECT * FROM WhiteList) LOOP
      BEGIN
        wlr.svcClass := r.svcClass;
        wlr.euid := r.euid;
        wlr.egid := r.egid;
        wlr.reqType := r.reqType;
        tmp := intersection(wlr, p);
        INSERT INTO RemovePrivilegeTmpTable
        VALUES (tmp.svcClass, tmp.euid, tmp.egid, tmp.reqType);
      EXCEPTION WHEN empty_privilege THEN
        NULL;
      END;
    END LOOP;
  END;

  -- Difference between privilege P1 and privilege P2
  FUNCTION diff(P1 Privilege, P2 Privilege) RETURN Privilege AS
    empty_privilege EXCEPTION;
    PRAGMA EXCEPTION_INIT(empty_privilege, -20109);
    unused Privilege;
  BEGIN
    IF contains(P1, P2) THEN
      IF (P1.euid = P2.euid OR (P1.euid IS NULL AND P2.euid IS NULL)) AND
         (P1.egid = P2.egid OR (P1.egid IS NULL AND P2.egid IS NULL)) AND
         (P1.svcClass = P2.svcClass OR (P1.svcClass IS NULL AND P2.svcClass IS NULL)) AND
         (P1.reqType = P2.reqType OR (P1.reqType IS NULL AND P2.reqType IS NULL)) THEN
        raise_application_error(-20109, 'Empty privilege');
      ELSE
        raise_application_error(-20108, 'Invalid privilege intersection');
      END IF;
    ELSIF contains(P2, P1) THEN
      raise_application_error(-20109, 'Empty privilege');
    ELSE
      BEGIN
        unused := intersection(P1, P2);
        -- no exception, so the intersection is not empty.
        -- we don't know how to handle such a case
        raise_application_error(-20108, 'Invalid privilege intersection');
      EXCEPTION WHEN empty_privilege THEN
      -- P1 and P2 do not intersect, the diff is thus P1
        RETURN P1;
      END;
    END IF;
  END;

  -- remove privilege P from list L
  PROCEDURE removePrivilegeFromBlackList(p Privilege) AS
    blr Privilege;
    tmp Privilege;
    empty_privilege EXCEPTION;
    PRAGMA EXCEPTION_INIT(empty_privilege, -20109);
  BEGIN
    FOR r IN (SELECT * FROM BlackList) LOOP
      BEGIN
        blr.svcClass := r.svcClass;
        blr.euid := r.euid;
        blr.egid := r.egid;
        blr.reqType := r.reqType;
        tmp := diff(blr, p);
      EXCEPTION WHEN empty_privilege THEN
        -- diff raised an exception saying that the diff is empty
        -- thus we drop the line
        DELETE FROM BlackList
         WHERE nvl(svcClass, -1) = nvl(r.svcClass, -1) AND
               nvl(euid, -1) = nvl(r.euid, -1) AND
               nvl(egid, -1) = nvl(r.egid, -1) AND
               nvl(reqType, -1) = nvl(r.reqType, -1);
      END;
    END LOOP;
  END;

  -- Add privilege P to list L :
  PROCEDURE addPrivilegeToWL(p Privilege) AS
    wlr Privilege;
    extended boolean := FALSE;
    ret NUMBER;
  BEGIN
    -- check if the service class exists
    ret := checkForValidSvcClass(p.svcClass, 1, 1);

    FOR r IN (SELECT * FROM WhiteList) LOOP
      wlr.svcClass := r.svcClass;
      wlr.euid := r.euid;
      wlr.egid := r.egid;
      wlr.reqType := r.reqType;
      -- check if we extend a privilege
      IF contains(p, wlr) THEN
        IF extended THEN
          -- drop this row, it merged into the new one
          DELETE FROM WhiteList
           WHERE nvl(svcClass, -1) = nvl(wlr.svcClass, -1) AND
                 nvl(euid, -1) = nvl(wlr.euid, -1) AND
                 nvl(egid, -1) = nvl(wlr.egid, -1) AND
                 nvl(reqType, -1) = nvl(wlr.reqType, -1);
        ELSE
          -- replace old row with new one
          UPDATE WhiteList
             SET svcClass = p.svcClass,
                 euid = p.euid,
                 egid = p.egid,
                 reqType = p.reqType
           WHERE nvl(svcClass, -1) = nvl(wlr.svcClass, -1) AND
                 nvl(euid, -1) = nvl(wlr.euid, -1) AND
                 nvl(egid, -1) = nvl(wlr.egid, -1) AND
                 nvl(reqType, -1) = nvl(wlr.reqType, -1);
          extended := TRUE;
        END IF;
      END IF;
      -- check if privilege is there
      IF contains(wlr, p) THEN RETURN; END IF;
    END LOOP;
    IF NOT extended THEN
      INSERT INTO WhiteList VALUES p;
    END IF;
  END;

  -- Add privilege P to list L :
  PROCEDURE addPrivilegeToBL(p Privilege) AS
    blr Privilege;
    extended boolean := FALSE;
    ret NUMBER;
  BEGIN
    -- check if the service class exists
    ret := checkForValidSvcClass(p.svcClass, 1, 1);

    FOR r IN (SELECT * FROM BlackList) LOOP
      blr.svcClass := r.svcClass;
      blr.euid := r.euid;
      blr.egid := r.egid;
      blr.reqType := r.reqType;
      -- check if privilege is there
      IF contains(blr, p) THEN RETURN; END IF;
      -- check if we extend a privilege
      IF contains(p, blr) THEN
        IF extended THEN
          -- drop this row, it merged into the new one
          DELETE FROM BlackList
           WHERE nvl(svcClass, -1) = nvl(blr.svcClass, -1) AND
                 nvl(euid, -1) = nvl(blr.euid, -1) AND
                 nvl(egid, -1) = nvl(blr.egid, -1) AND
                 nvl(reqType, -1) = nvl(blr.reqType, -1);
        ELSE
          -- replace old row with new one
          UPDATE BlackList
             SET svcClass = p.svcClass,
                 euid = p.euid,
                 egid = p.egid,
                 reqType = p.reqType
           WHERE nvl(svcClass, -1) = nvl(blr.svcClass, -1) AND
                 nvl(euid, -1) = nvl(blr.euid, -1) AND
                 nvl(egid, -1) = nvl(blr.egid, -1) AND
                 nvl(reqType, -1) = nvl(blr.reqType, -1);
          extended := TRUE;
        END IF;
      END IF;
    END LOOP;
    IF NOT extended THEN
      INSERT INTO BlackList VALUES p;
    END IF;
  END;

  -- cleanup BlackList when a privilege was removed from the whitelist
  PROCEDURE cleanupBL AS
    blr Privilege;
    c NUMBER;
  BEGIN
    FOR r IN (SELECT * FROM BlackList) LOOP
      blr.svcClass := r.svcClass;
      blr.euid := r.euid;
      blr.egid := r.egid;
      blr.reqType := r.reqType;
      intersectionWithWhiteList(blr);
      SELECT COUNT(*) INTO c FROM RemovePrivilegeTmpTable;
      IF c = 0 THEN
        -- we can safely drop this line
        DELETE FROM BlackList
         WHERE nvl(svcClass, -1) = nvl(r.svcClass, -1) AND
               nvl(euid, -1) = nvl(r.euid, -1) AND
               nvl(egid, -1) = nvl(r.egid, -1) AND
               nvl(reqType, -1) = nvl(r.reqType, -1);
      END IF;
    END LOOP;
  END;

  -- Add privilege P
  PROCEDURE addPrivilege(P Privilege) AS
  BEGIN
    removePrivilegeFromBlackList(P);
    addPrivilegeToWL(P);
  END;

  -- Remove privilege P
  PROCEDURE removePrivilege(P Privilege) AS
    c NUMBER;
    wlr Privilege;
  BEGIN
    -- Check first whether there is something to remove
    intersectionWithWhiteList(P);
    SELECT COUNT(*) INTO c FROM RemovePrivilegeTmpTable;
    IF c = 0 THEN RETURN; END IF;
    -- Remove effectively what can be removed
    FOR r IN (SELECT * FROM WHITELIST) LOOP
      wlr.svcClass := r.svcClass;
      wlr.euid := r.euid;
      wlr.egid := r.egid;
      wlr.reqType := r.reqType;
      IF contains(P, wlr) THEN
        DELETE FROM WhiteList
         WHERE nvl(svcClass, -1) = nvl(wlr.svcClass, -1) AND
               nvl(euid, -1) = nvl(wlr.euid, -1) AND
               nvl(egid, -1) = nvl(wlr.egid, -1) AND
               nvl(reqType, -1) = nvl(wlr.reqType, -1);
      END IF;
    END LOOP;
    -- cleanup blackList
    cleanUpBL();
    -- check what remains
    intersectionWithWhiteList(P);
    SELECT COUNT(*) INTO c FROM removePrivilegeTmpTable;
    IF c = 0 THEN RETURN; END IF;
    -- If any, add them to blackList
    FOR q IN (SELECT * FROM RemovePrivilegeTmpTable) LOOP
      wlr.svcClass := q.svcClass;
      wlr.euid := q.euid;
      wlr.egid := q.egid;
      wlr.reqType := q.reqType;
      addPrivilegeToBL(wlr);
    END LOOP;
  END;

  -- Add privilege
  PROCEDURE addPrivilege(svcClassName VARCHAR2, euid NUMBER, egid NUMBER, reqType NUMBER) AS
    p castorBW.Privilege;
  BEGIN
    p.svcClass := svcClassName;
    p.euid := euid;
    p.egid := egid;
    p.reqType := reqType;
    addPrivilege(p);
  END;

  -- Remove privilege
  PROCEDURE removePrivilege(svcClassName VARCHAR2, euid NUMBER, egid NUMBER, reqType NUMBER) AS
    p castorBW.Privilege;
  BEGIN
    p.svcClass := svcClassName;
    p.euid := euid;
    p.egid := egid;
    p.reqType := reqType;
    removePrivilege(p);
  END;

  -- List privileges
  PROCEDURE listPrivileges(svcClassName IN VARCHAR2, ieuid IN NUMBER,
                           iegid IN NUMBER, ireqType IN NUMBER,
                           plist OUT PrivilegeExt_Cur) AS
  BEGIN
    OPEN plist FOR
      SELECT decode(svcClass, NULL, '*', '*', '''*''', svcClass),
             euid, egid, reqType, 1
        FROM WhiteList
       WHERE (WhiteList.svcClass = svcClassName OR WhiteList.svcClass IS  NULL OR svcClassName IS NULL)
         AND (WhiteList.euid = ieuid OR WhiteList.euid IS NULL OR ieuid = -1)
         AND (WhiteList.egid = iegid OR WhiteList.egid IS NULL OR iegid = -1)
         AND (WhiteList.reqType = ireqType OR WhiteList.reqType IS NULL OR ireqType = 0)
    UNION
      SELECT decode(svcClass, NULL, '*', '*', '''*''', svcClass),
             euid, egid, reqType, 0
        FROM BlackList
       WHERE (BlackList.svcClass = svcClassName OR BlackList.svcClass IS  NULL OR svcClassName IS NULL)
         AND (BlackList.euid = ieuid OR BlackList.euid IS NULL OR ieuid = -1)
         AND (BlackList.egid = iegid OR BlackList.egid IS NULL OR iegid = -1)
         AND (BlackList.reqType = ireqType OR BlackList.reqType IS NULL OR ireqType = 0);
  END;

END castorBW;
/

/*******************************************************************
 *
 * @(#)RCSfile: oracleStager.sql,v  Revision: 1.752  Date: 2009/08/17 15:08:33  Author: sponcec3 
 *
 * PL/SQL code for the stager and resource monitoring
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PL/SQL declaration for the castor package */
create or replace PACKAGE castor AS
  TYPE DiskCopyCore IS RECORD (
    id INTEGER,
    path VARCHAR2(2048),
    status NUMBER,
    fsWeight NUMBER,
    mountPoint VARCHAR2(2048),
    diskServer VARCHAR2(2048));
  TYPE DiskCopy_Cur IS REF CURSOR RETURN DiskCopyCore;
  TYPE TapeCopy_Cur IS REF CURSOR RETURN TapeCopy%ROWTYPE;
  TYPE Tape_Cur IS REF CURSOR RETURN Tape%ROWTYPE;
  TYPE Segment_Cur IS REF CURSOR RETURN Segment%ROWTYPE;
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
    lastAccessTime INTEGER);
  TYPE QueryLine_Cur IS REF CURSOR RETURN QueryLine;
  TYPE FileList_Cur IS REF CURSOR RETURN FilesDeletedProcOutput%ROWTYPE;
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
  TYPE DiskServerName IS RECORD (diskServer VARCHAR(2048));
  TYPE DiskServerList_Cur IS REF CURSOR RETURN DiskServerName;
  TYPE SchedulerResourceLine IS RECORD (
    diskServerName VARCHAR(2048),
    diskServerStatus INTEGER,
    diskServerAdminStatus INTEGER,
    fileSystemMountPoint VARCHAR(2048),
    fileSystemStatus INTEGER,
    fileSystemAdminStatus INTEGER,
    fileSystemSvcClassName VARCHAR(2048));
  TYPE SchedulerResources_Cur IS REF CURSOR RETURN SchedulerResourceLine;
  TYPE FileEntry IS RECORD (
    fileid INTEGER,
    nshost VARCHAR2(2048));
  TYPE FileEntry_Cur IS REF CURSOR RETURN FileEntry;
  TYPE PriorityMap_Cur IS REF CURSOR RETURN PriorityMap%ROWTYPE;
  TYPE StreamReport IS RECORD (
   diskserver VARCHAR2(2048),
   mountPoint VARCHAR2(2048));
  TYPE StreamReport_Cur IS REF CURSOR RETURN  StreamReport;  
END castor;
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
   srIds "numList"; 	 
 BEGIN 	 
   -- Flag the filesystem for processing in a bulk operation later. 	 
   -- We need to do this because some operations are database intensive 	 
   -- and therefore it is often better to process several filesystems 	 
   -- simultaneous with one query as opposed to one by one. Especially 	 
   -- where full table scans are involved. 	 
   UPDATE FileSystemsToCheck SET toBeChecked = 1 	 
    WHERE fileSystem = fsId; 	 
   -- Look for files that are STAGEOUT on the filesystem coming back to life 	 
   -- but already STAGED/CANBEMIGR/WAITTAPERECALL/WAITFS/STAGEOUT/ 	 
   -- WAITFS_SCHEDULING somewhere else 	 
   FOR cf IN (SELECT UNIQUE d.castorfile, d.id 	 
                FROM DiskCopy d, DiskCopy e 	 
               WHERE d.castorfile = e.castorfile 	 
                 AND d.fileSystem = fsId 	 
                 AND e.fileSystem != fsId 	 
                 AND d.status = 6 -- STAGEOUT 	 
                 AND e.status IN (0, 10, 2, 5, 6, 11)) LOOP -- STAGED/CANBEMIGR/WAITTAPERECALL/WAITFS/STAGEOUT/WAITFS_SCHEDULING 	 
     -- Invalidate the DiskCopy 	 
     UPDATE DiskCopy 	 
        SET status = 7  -- INVALID 	 
      WHERE id = cf.id;
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
  -- on all filesystems scheduled to be checked.
  FOR cf IN (SELECT UNIQUE d.castorfile, e.id
               FROM DiskCopy d, DiskCopy e
              WHERE d.castorfile = e.castorfile
                AND d.fileSystem IN
                  (SELECT /*+ CARDINALITY(fsidTable 5) */ *
                     FROM TABLE(fsIds) fsidTable)
                AND d.status IN (0, 10)
                AND e.status = 2) LOOP
    -- Cancel recall and restart subrequests
    cancelRecall(cf.castorfile, cf.id, 1); -- RESTART
  END LOOP;
END;
/


/* SQL statement for the update trigger on the FileSystem table */
CREATE OR REPLACE TRIGGER tr_FileSystem_Update
BEFORE UPDATE OF status ON FileSystem
FOR EACH ROW
BEGIN
  -- If the filesystem is coming back into PRODUCTION, initiate a consistency
  -- check for the diskcopies which reside on the filesystem.
  IF :old.status IN (1, 2) AND  -- DRAINING, DISABLED
     :new.status = 0 THEN       -- PRODUCTION
    checkFsBackInProd(:old.id);
  END IF;
  -- Cancel any ongoing draining operations if the filesystem is no longer in
  -- a DRAINING state.
  IF :old.status = 1 AND        -- DRAINING
     :new.status IN (0, 2) THEN -- PRODUCTION, DISABLED
    UPDATE DrainingFileSystem
       SET status = 3  -- INTERRUPTED
     WHERE fileSystem = :new.id
       AND status IN (0, 1, 2);  -- CREATED, INITIALIZING, RUNNING
  END IF;
END;
/


/* SQL statement for the update trigger on the DiskServer table */
CREATE OR REPLACE TRIGGER tr_DiskServer_Update
BEFORE UPDATE OF status ON DiskServer
FOR EACH ROW
BEGIN
  -- If the diskserver is coming back into PRODUCTION, initiate a consistency
  -- check for all the diskcopies on its associated filesystems which are in
  -- a PRODUCTION.
  IF :old.status IN (1, 2) AND  -- DRAINING, DISABLED
     :new.status = 0 THEN       -- PRODUCTION
    FOR fs IN (SELECT id FROM FileSystem
                WHERE diskServer = :old.id
                  AND status = 0)  -- PRODUCTION
    LOOP
      checkFsBackInProd(fs.id);
    END LOOP;
  END IF;
  -- Cancel any ongoing draining operations if:
  --  A) The diskserver is going into a DISABLED state      or
  --  B) The diskserver is going into PRODUCTION but its associated filesystems
  --     are not in DRAINING.
  IF :old.status = 1 AND        -- DRAINING
     :new.status IN (0, 2) THEN -- PRODUCTION, DISABLED
   UPDATE DrainingFileSystem
      SET status = 3  -- INTERRUPTED
    WHERE fileSystem IN
      (SELECT id FROM FileSystem
        WHERE diskServer = :old.id
          AND (status != 1  --  FILESYSTEM_DRAINING
           OR  :new.status = 2))  -- DISKSERVER_DISABLED
      AND status IN (0, 1, 2);  -- CREATED, INITIALIZING, RUNNING
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
                  SELECT DiskCopy.id
                    FROM DiskCopy, FileSystem, DiskPool2SvcClass, SvcClass,
                         DiskServer
                   WHERE DiskCopy.filesystem = FileSystem.id
                     AND FileSystem.diskpool = DiskPool2SvcClass.parent
                     AND FileSystem.diskserver = DiskServer.id
                     AND DiskPool2SvcClass.child = SvcClass.id
                     AND DiskCopy.castorfile = a.castorfile
                     AND DiskCopy.status IN (0, 10)  -- STAGED, CANBEMIGR
                     AND SvcClass.id = a.svcclass
                   -- Select DISABLED or DRAINING hardware first
                   ORDER BY decode(FileSystem.status, 0,
                            decode(DiskServer.status, 0, 0, 1), 1) ASC,
                            DiskCopy.gcWeight DESC))
               WHERE ind > maxReplicaNb)
    LOOP
      -- Sanity check, make sure that the last copy is never dropped!
      SELECT count(*) INTO nbFiles
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
         SET status = 7,  -- INVALID
             gcType = 2   -- Too many replicas
       WHERE id = b.id;
    END LOOP;
  END LOOP;
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
    INSERT INTO TooManyReplicasHelper
    VALUES (svcId, :new.castorfile);
  EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
    RETURN;  -- Entry already exists!
  END;
END;
/


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
  unused NUMBER;
BEGIN
  SELECT id INTO unused FROM CastorFile
   WHERE id = :new.castorFile FOR UPDATE;
END;
/


/* Used to avoid LOCK TABLE TapeCopy whenever someone wants
   to deal with the tapeCopies on a CastorFile.
   Due to this trigger, locking the CastorFile is enough
   to be safe */
CREATE OR REPLACE TRIGGER tr_DiskCopy_CastorFile
BEFORE INSERT OR UPDATE OF castorFile ON DiskCopy
FOR EACH ROW WHEN (new.castorFile > 0)
DECLARE
  unused NUMBER;
BEGIN
  SELECT id INTO unused FROM CastorFile
   WHERE id = :new.castorFile FOR UPDATE;
END;
/


/* PL/SQL method to get the next SubRequest to do according to the given service */
CREATE OR REPLACE PROCEDURE subRequestToDo(service IN VARCHAR2,
                                           srId OUT INTEGER, srRetryCounter OUT INTEGER, srFileName OUT VARCHAR2,
                                           srProtocol OUT VARCHAR2, srXsize OUT INTEGER, srPriority OUT INTEGER,
                                           srStatus OUT INTEGER, srModeBits OUT INTEGER, srFlags OUT INTEGER,
                                           srSubReqId OUT VARCHAR2, srAnswered OUT INTEGER, srSvcHandler OUT VARCHAR2) AS
  CURSOR SRcur IS SELECT /*+ FIRST_ROWS(10) INDEX(SR I_SubRequest_RT_CT_ID) */ SR.id
                    FROM SubRequest PARTITION (P_STATUS_0_1_2) SR
                   WHERE SR.svcHandler = service
                   ORDER BY SR.creationTime ASC;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  srIntId NUMBER;
BEGIN
  OPEN SRcur;
  -- Loop on candidates until we can lock one
  LOOP
    -- Fetch next candidate
    FETCH SRcur INTO srIntId;
    EXIT WHEN SRcur%NOTFOUND;
    BEGIN
      -- Try to take a lock on the current candidate, and revalidate its status
      SELECT id INTO srIntId FROM SubRequest PARTITION (P_STATUS_0_1_2) SR WHERE id = srIntId FOR UPDATE NOWAIT;
      -- Since we are here, we got the lock. We have our winner, let's update it
      UPDATE SubRequest
         SET status = 3, subReqId = nvl(subReqId, uuidGen()) -- WAITSCHED
       WHERE id = srIntId
      RETURNING id, retryCounter, fileName, protocol, xsize, priority, status, modeBits, flags, subReqId, answered, svcHandler
        INTO srId, srRetryCounter, srFileName, srProtocol, srXsize, srPriority, srStatus, srModeBits, srFlags, srSubReqId, srAnswered, srSvcHandler;
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
END;
/


/* PL/SQL method to get the next failed SubRequest to do according to the given service */
/* the service parameter is not used now, it will with the new stager */
CREATE OR REPLACE PROCEDURE subRequestFailedToDo(srId OUT INTEGER, srRetryCounter OUT INTEGER, srFileName OUT VARCHAR2,
                                                 srProtocol OUT VARCHAR2, srXsize OUT INTEGER, srPriority OUT INTEGER,
                                                 srStatus OUT INTEGER, srModeBits OUT INTEGER, srFlags OUT INTEGER,
                                                 srSubReqId OUT VARCHAR2, srErrorCode OUT NUMBER,
                                                 srErrorMessage OUT VARCHAR2) AS
LockError EXCEPTION;
PRAGMA EXCEPTION_INIT (LockError, -54);
InvalidRowid EXCEPTION;
PRAGMA EXCEPTION_INIT (InvalidRowid, -10632);
CURSOR c IS
   SELECT id, answered
     FROM SubRequest
    WHERE status = 7  -- FAILED
      AND ROWNUM < 2
    FOR UPDATE SKIP LOCKED;
srAnswered INTEGER;
BEGIN
  srId := 0;
  OPEN c;
  FETCH c INTO srId, srAnswered;
  IF srAnswered = 1 THEN
    -- already answered, ignore it
    archiveSubReq(srId, 9);  -- FAILED_FINISHED
    srId := 0;
  ELSE
    UPDATE subrequest SET status = 10 WHERE id = srId   -- FAILED_ANSWERING
      RETURNING retryCounter, fileName, protocol, xsize, priority, status,
                modeBits, flags, subReqId, errorCode, errorMessage
      INTO srRetryCounter, srFileName, srProtocol, srXsize, srPriority, srStatus,
           srModeBits, srFlags, srSubReqId, srErrorCode, srErrorMessage;
  END IF;
  CLOSE c;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- just return srId = 0, nothing to do
  NULL;
WHEN InvalidRowid THEN
  -- Ignore random ORA-10632 errors (invalid rowid) due to interferences with the online shrinking
  NULL;
WHEN LockError THEN
  -- We have observed ORA-00054 errors (resource busy and acquire with NOWAIT) even with
  -- the SKIP LOCKED clause. This is a workaround to ignore the error until we understand
  -- what to do, another thread will pick up the request so we don't do anything.
  NULL;
END;
/


/* PL/SQL method to get the next request to do according to the given service */
CREATE OR REPLACE PROCEDURE requestToDo(service IN VARCHAR2, rId OUT INTEGER) AS
BEGIN
  DELETE FROM NewRequests
   WHERE type IN (
     SELECT type FROM Type2Obj
      WHERE svcHandler = service
     )
   AND ROWNUM < 2 RETURNING id INTO rId;
EXCEPTION WHEN NO_DATA_FOUND THEN
  rId := 0;   -- nothing to do
END;
/


/* PL/SQL method to make a SubRequest wait on another one, linked to the given DiskCopy */
CREATE OR REPLACE PROCEDURE makeSubRequestWait(srId IN INTEGER, dci IN INTEGER) AS
BEGIN
  -- all wait on the original one; also prevent to wait on a PrepareToPut, for the
  -- case when Updates and Puts come after a PrepareToPut and they need to wait on
  -- the first Update|Put to complete.
  -- Cf. recreateCastorFile and the DiskCopy statuses 5 and 11
  UPDATE SubRequest
     SET parent = (SELECT SubRequest.id
                     FROM SubRequest, DiskCopy, Id2Type
                    WHERE SubRequest.diskCopy = DiskCopy.id
                      AND DiskCopy.id = dci
                      AND SubRequest.request = Id2Type.id
                      AND Id2Type.type <> 37  -- OBJ_PrepareToPut
                      AND SubRequest.parent = 0
                      AND DiskCopy.status IN (1, 2, 5, 6, 11) -- WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, STAGEOUT, WAITFS_SCHEDULING
                      AND SubRequest.status IN (0, 1, 2, 4, 13, 14, 6)), -- START, RESTART, RETRY, WAITTAPERECALL, READYFORSCHED, BEINGSCHED, READY
        status = 5, -- WAITSUBREQ
        lastModificationTime = getTime()
  WHERE SubRequest.id = srId;
END;
/


/* PL/SQL method to archive a SubRequest */
CREATE OR REPLACE PROCEDURE archiveSubReq(srId IN INTEGER, finalStatus IN INTEGER) AS
  nb INTEGER;
  rId INTEGER;
  rname VARCHAR2(100);
  srIds "numList";
  clientId INTEGER;
BEGIN
  UPDATE SubRequest
     SET parent = NULL, diskCopy = NULL,  -- unlink this subrequest as it's dead now
         lastModificationTime = getTime(),
         status = finalStatus
   WHERE id = srId
  RETURNING request INTO rId;
  -- Lock the access to the Request
  SELECT Id2Type.id INTO rId
    FROM Id2Type
   WHERE id = rId FOR UPDATE;
  -- Try to see whether another subrequest in the same
  -- request is still being processed
  SELECT count(*) INTO nb FROM SubRequest
   WHERE request = rId AND status NOT IN (8, 9);  -- all but {FAILED_,}FINISHED

  IF nb = 0 THEN
    -- all subrequests have finished, we can archive
    SELECT object INTO rname
      FROM Id2Type, Type2Obj
     WHERE id = rId
       AND Type2Obj.type = Id2Type.type;
    -- drop the associated Client entity and all Id2Type entries
    EXECUTE IMMEDIATE
      'BEGIN SELECT client INTO :cId FROM '|| rname ||' WHERE id = :rId; END;'
      USING OUT clientId, IN rId;
    DELETE FROM Client WHERE id = clientId;
    DELETE FROM Id2Type WHERE id IN (rId, clientId);
    SELECT id BULK COLLECT INTO srIds
      FROM SubRequest
     WHERE request = rId;
    FORALL i IN srIds.FIRST .. srIds.LAST
      DELETE FROM Id2Type WHERE id = srIds(i);
    -- archive the successful subrequests      
    UPDATE /*+ SubRequest I_SubRequest_Request */ SubRequest
       SET status = 11    -- ARCHIVED
     WHERE request = rId
       AND status = 8;  -- FINISHED
  END IF;
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
      FROM diskpool2svcclass, FileSystem, DiskServer
     WHERE diskpool2svcclass.child = svcClassId
       AND diskpool2svcclass.parent = FileSystem.diskPool
       AND FileSystem.diskServer = DiskServer.id
       AND FileSystem.status = 0 -- PRODUCTION
       AND DiskServer.status = 0 -- PRODUCTION
       AND totalSize * minAllowedFreeSpace < free - defFileSize;
    IF (c = 0) THEN
      RETURN 1;
    END IF;
    SELECT sum(xsize) INTO reservedSpace
      FROM SubRequest, StagePutRequest R, DiskCopy
     WHERE SubRequest.request = R.id
       AND SubRequest.diskCopy = DiskCopy.id
       AND SubRequest.status = 6  -- READY
       AND DiskCopy.status IN (5, 6, 11)  -- WAITFS[_SCHEDULING], STAGEOUT
       AND DiskCopy.fileSystem = 0  -- not yet scheduled
       AND R.svcClass = svcClassId;
    IF availSpace < reservedSpace THEN
      RETURN 1;
    END IF;
  END IF;
  RETURN 0;
END;
/

/* PL/SQL method checking whether the given service class
 * doesn't provide tape backend and the given file class asks for tape copies.
 * Returns 1 in such a case, 0 else
 */
CREATE OR REPLACE FUNCTION checkFailPutWhenTape0(svcClassId NUMBER, fileClassId NUMBER)
RETURN NUMBER AS
  nbTCs INTEGER;
  nbForcedTCs INTEGER;
BEGIN
  -- get #tapeCopies requested by this file
  SELECT nbCopies INTO nbTCs
    FROM FileClass WHERE id = fileClassId;
  -- get #tapeCpies from the forcedFileClass: if no forcing
  -- we assume we have tape backend and we let the job
  SELECT nvl(nbCopies, nbTCs) INTO nbForcedTCs
    FROM FileClass, SvcClass
   WHERE SvcClass.forcedFileClass = FileClass.id(+)
     AND SvcClass.id = svcClassId;
  IF nbTCs > nbForcedTCs THEN
    -- typically, when nbTCs = 1 and nbForcedTCs = 0: fail the job
    RETURN 1;
  ELSE
    RETURN 0;
  END IF;
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
         AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
         AND DiskServer.id = FileSystem.diskserver
         AND DiskServer.status IN (0, 1) -- PRODUCTION, DRAINING
         -- If this is an internal replication request make sure that the diskcopy
         -- is in the same service class as the source.
         AND (SvcClass.id = destSvcClassId OR internal = 0)
         -- Check that the user has the necessary access rights to replicate a
         -- file from the source service class. Note: instead of using a
         -- StageGetRequest type here we use a StagDiskCopyReplicaRequest type
         -- to be able to distinguish between and read and replication requst.
         AND checkPermissionOnSvcClass(SvcClass.name, reuid, regid, 133) = 0
         AND NOT EXISTS (
           -- Don't select source diskcopies which already failed more than 10 times
           SELECT 'x'
             FROM StageDiskCopyReplicaRequest R, SubRequest
            WHERE SubRequest.request = R.id
              AND R.sourceDiskCopy = DiskCopy.id
              AND SubRequest.status = 9 -- FAILED_FINISHED
           HAVING COUNT(*) >= 10)
       ORDER BY FileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                               FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams,
                               FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC)
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
       AND FileSystem.status = 0 -- PRODUCTION
       AND FileSystem.diskserver = DiskServer.id
       AND DiskServer.status = 0 -- PRODUCTION
       AND DiskCopy.status IN (0, 6, 10)  -- STAGED, STAGEOUT, CANBEMIGR
     ORDER BY FileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                             FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams,
                             FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC)
   WHERE rownum < 2;
EXCEPTION WHEN NO_DATA_FOUND THEN
  RAISE; -- No file found to be read
END;
/


/* PL/SQL method implementing checkForD2DCopyOrRecall
 * dcId is the DiskCopy id of the best candidate for replica, 0 if none is found (tape recall), -1 in case of user error
 * Internally used by getDiskCopiesForJob and processPrepareRequest
 */
CREATE OR REPLACE
PROCEDURE checkForD2DCopyOrRecall(cfId IN NUMBER, srId IN NUMBER, reuid IN NUMBER, regid IN NUMBER,
                                  svcClassId IN NUMBER, dcId OUT NUMBER, srcSvcClassId OUT NUMBER) AS
  destSvcClass VARCHAR2(2048);
  authDest NUMBER;
BEGIN
  -- First check whether we are a disk only pool that is already full.
  -- In such a case, we should fail the request with an ENOSPACE error
  IF (checkFailJobsWhenNoSpace(svcClassId) = 1) THEN
    dcId := -1;
    UPDATE SubRequest
       SET status = 7, -- FAILED
           errorCode = 28, -- ENOSPC
           errorMessage = 'File creation canceled since diskPool is full'
     WHERE id = srId;
    COMMIT;
    RETURN;
  END IF;
  -- Resolve the destination service class id to a name
  SELECT name INTO destSvcClass FROM SvcClass WHERE id = svcClassId;
  -- If we are in this procedure then we did not find a copy of the
  -- file in the target service class that could be used. So, we check
  -- to see if the user has the rights to create a file in the destination
  -- service class. I.e. check for StagePutRequest access rights
  checkPermission(destSvcClass, reuid, regid, 40, authDest);
  IF authDest < 0 THEN
    -- Fail the subrequest and notify the client
    dcId := -1;
    UPDATE SubRequest
       SET status = 7, -- FAILED
           errorCode = 13, -- EACCES
           errorMessage = 'Insufficient user privileges to trigger a tape recall or file replication to the '''||destSvcClass||''' service class'
     WHERE id = srId;
    COMMIT;
    RETURN;
  END IF;
  -- Try to find a diskcopy to replicate
  getBestDiskCopyToReplicate(cfId, reuid, regid, 0, svcClassId, dcId, srcSvcClassId);
  -- We found at least one, therefore we schedule a disk2disk
  -- copy from the existing diskcopy not available to this svcclass
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- We found no diskcopies at all. We should not schedule
  -- and make a tape recall... except ... in 3 cases :
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
  DECLARE
    dcStatus NUMBER;
    fsStatus NUMBER;
    dsStatus NUMBER;
  BEGIN
    SELECT DiskCopy.status, nvl(FileSystem.status, 0), nvl(DiskServer.status, 0)
      INTO dcStatus, fsStatus, dsStatus
      FROM DiskCopy, FileSystem, DiskServer
     WHERE DiskCopy.castorfile = cfId
       AND DiskCopy.status IN (5, 6, 10, 11) -- WAITFS, STAGEOUT, CANBEMIGR, WAITFSSCHEDULING
       AND FileSystem.id(+) = DiskCopy.fileSystem
       AND DiskServer.id(+) = FileSystem.diskserver
       AND ROWNUM < 2;
    -- We are in one of the special cases. Don't schedule, don't recall
    dcId := -1;
    UPDATE SubRequest
       SET status = 7, -- FAILED
           errorCode = CASE
             WHEN dcStatus IN (5, 11) THEN 16 -- WAITFS, WAITFSSCHEDULING, EBUSY
             WHEN dcStatus = 6 AND fsStatus = 0 and dsStatus = 0 THEN 16 -- STAGEOUT, PRODUCTION, PRODUCTION, EBUSY
             ELSE 1718 -- ESTNOTAVAIL
           END,
           errorMessage = CASE
             WHEN dcStatus IN (5, 11) THEN -- WAITFS, WAITFSSCHEDULING
               'File is being (re)created right now by another user'
             WHEN dcStatus = 6 AND fsStatus = 0 and dsStatus = 0 THEN -- STAGEOUT, PRODUCTION, PRODUCTION
               'File is being written to in another SvcClass'
             ELSE
               'All copies of this file are unavailable for now. Please retry later'
           END
     WHERE id = srId;
    COMMIT;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- Check whether the user has the rights to issue a tape recall to
    -- the destination service class.
    checkPermission(destSvcClass, reuid, regid, 161, authDest);
    IF authDest < 0 THEN
      -- Fail the subrequest and notify the client
      dcId := -1;
      UPDATE SubRequest
         SET status = 7, -- FAILED
             errorCode = 13, -- EACCES
             errorMessage = 'Insufficient user privileges to trigger a tape recall to the '''||destSvcClass||''' service class'
       WHERE id = srId;
      COMMIT;
    ELSE
      -- We did not find the very special case so trigger a tape recall.
      dcId := 0;
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
  path := CONCAT(CONCAT(CONCAT(CONCAT(TO_CHAR(MOD(fid,100),'FM09'), '/'),
                 CONCAT(TO_CHAR(fid), '@')),
                 nsHost), CONCAT('.', TO_CHAR(dcid)));
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
BEGIN
  -- Extract the castorfile associated with the request, this is needed to
  -- create the StageDiskCopyReplicaRequest's diskcopy and subrequest entries.
  -- Also for disk2disk copying across service classes make the originating
  -- subrequest wait on the completion of the transfer.
  IF sourceSrId > 0 THEN
    UPDATE SubRequest
       SET status = 5, parent = ids_seq.nextval -- WAITSUBREQ
     WHERE id = sourceSrId
    RETURNING castorFile, parent INTO cfId, srId;
  ELSE
    SELECT castorfile INTO cfId FROM DiskCopy WHERE id = sourceDcId;
    SELECT ids_seq.nextval INTO srId FROM Dual;
  END IF;

  SELECT fileid, nshost, filesize, lastknownfilename
    INTO fileId, nsHost, fileSize, fileName
    FROM CastorFile WHERE id = cfId;

  -- Create the Client entry.
  INSERT INTO Client (ipaddress, port, id, version, secure)
    VALUES (0, 0, ids_seq.nextval, 0, 0)
  RETURNING id INTO clientId;
  INSERT INTO Id2Type (id, type) VALUES (clientId, 129);  -- OBJ_Client

  -- Create the StageDiskCopyReplicaRequest. The euid and egid values default to
  -- 0 here, this indicates the request came from the user root. When the
  -- jobmanager daemon encounters the StageDiskCopyReplicaRequest it will
  -- automatically use the stage:st account for submission into LSF.
  SELECT ids_seq.nextval INTO destDcId FROM Dual;
  INSERT INTO StageDiskCopyReplicaRequest
    (svcclassname, reqid, creationtime, lastmodificationtime, id, svcclass,
     client, sourcediskcopy, destdiskcopy, sourceSvcClass)
  VALUES ((SELECT name FROM SvcClass WHERE id = destSvcId), uuidgen(), gettime(),
     gettime(), ids_seq.nextval, destSvcId, clientId, sourceDcId, destDcId, sourceSvcId)
  RETURNING id INTO reqId;
  INSERT INTO Id2Type (id, type) VALUES (reqId, 133);  -- OBJ_StageDiskCopyReplicaRequest;

  -- Create the SubRequest setting the initial status to READYFORSCHED for
  -- immediate dispatching i.e no stager processing by the jobmanager daemon.
  INSERT INTO SubRequest
    (retrycounter, filename, protocol, xsize, priority, subreqid, flags, modebits,
     creationtime, lastmodificationtime, answered, id, diskcopy, castorfile, parent,
     status, request, getnextstatus, errorcode, svcHandler)
  VALUES (0, fileName, 'rfio', fileSize, 0, uuidgen(), 0, 0, gettime(), gettime(),
     0, srId, destDcId, cfId, 0, 13, reqId, 0, 0, 'NotNullNeeded');
  INSERT INTO Id2Type (id, type) VALUES (srId, 27);  -- OBJ_SubRequest

  -- Create the DiskCopy without filesystem
  buildPathFromFileId(fileId, nsHost, destDcId, rpath);
  INSERT INTO DiskCopy
    (path, id, filesystem, castorfile, status, creationTime, lastAccessTime,
     gcWeight, diskCopySize, nbCopyAccesses, owneruid, ownergid)
  VALUES (rpath, destDcId, 0, cfId, 1, getTime(), getTime(), 0, fileSize, 0, ouid, ogid);  -- WAITDISK2DISKCOPY
  INSERT INTO Id2Type (id, type) VALUES (destDcId, 5);  -- OBJ_DiskCopy
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
  -- get an id for our new DiskCopy
  SELECT ids_seq.nextval INTO dcId FROM DUAL;
  -- compute the DiskCopy Path
  buildPathFromFileId(fileId, nsHost, dcId, dcPath);
  -- find a fileSystem for this empty file
  SELECT id, svcClass, euid, egid, name || ':' || mountpoint
    INTO fsId, svcClassId, ouid, ogid, fsPath
    FROM (SELECT FileSystem.id, Request.svcClass, Request.euid, Request.egid, DiskServer.name, FileSystem.mountpoint
            FROM DiskServer, FileSystem, DiskPool2SvcClass,
                 (SELECT id, svcClass, euid, egid from StageGetRequest UNION ALL
                  SELECT id, svcClass, euid, egid from StagePrepareToGetRequest UNION ALL
                  SELECT id, svcClass, euid, egid from StageUpdateRequest UNION ALL
                  SELECT id, svcClass, euid, egid from StagePrepareToUpdateRequest) Request,
                  SubRequest
           WHERE SubRequest.id = srId
             AND Request.id = SubRequest.request
             AND Request.svcclass = DiskPool2SvcClass.child
             AND FileSystem.diskpool = DiskPool2SvcClass.parent
             AND FileSystem.status = 0 -- FILESYSTEM_PRODUCTION
             AND DiskServer.id = FileSystem.diskServer
             AND DiskServer.status = 0 -- DISKSERVER_PRODUCTION
        ORDER BY -- first prefer DSs without concurrent migrators/recallers
                 DiskServer.nbRecallerStreams ASC, FileSystem.nbMigratorStreams ASC,
                 -- then order by rate as defined by the function
                 fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                                FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams,
                                FileSystem.nbRecallerStreams) DESC,
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
  INSERT INTO Id2Type (id, type) VALUES (dcId, 5);  -- OBJ_DiskCopy
  -- link to the SubRequest and schedule an access if requested
  IF schedule = 0 THEN
    UPDATE SubRequest SET diskCopy = dcId WHERE id = srId;
  ELSE
    UPDATE SubRequest
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
                   AND FileSystem.status IN (0, 1)  -- PRODUCTION, DRAINING
                   AND DiskServer.id = FileSystem.diskserver
                   AND DiskServer.status IN (0, 1)  -- PRODUCTION, DRAINING
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
      SELECT DiskCopy.id INTO unused
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
  upd INTEGER;
  dcIds "numList";
  svcClassId NUMBER;
  srcSvcClassId NUMBER;
  cfId NUMBER;
  srcDcId NUMBER;
  d2dsrId NUMBER;
  reuid NUMBER;
  regid NUMBER;
BEGIN
  -- retrieve the castorFile and the svcClass for this subrequest
  SELECT SubRequest.castorFile, Request.euid, Request.egid, Request.svcClass, Request.upd
    INTO cfId, reuid, regid, svcClassId, upd
    FROM (SELECT id, euid, egid, svcClass, 0 upd from StageGetRequest UNION ALL
          SELECT id, euid, egid, svcClass, 1 upd from StageUpdateRequest) Request,
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
     AND nvl(FileSystem.status, 0) = 0 -- PRODUCTION
     AND DiskServer.id(+) = FileSystem.diskServer
     AND nvl(DiskServer.status, 0) = 0 -- PRODUCTION
     AND DiskCopy.status IN (2, 11); -- WAITTAPERECALL, WAITFS_SCHEDULING
  IF dcIds.COUNT > 0 THEN
    -- DiskCopy is in WAIT*, make SubRequest wait on previous subrequest and do not schedule
    makeSubRequestWait(srId, dcIds(1));
    result := -2;
    COMMIT;
    RETURN;
  END IF;

  -- Look for available diskcopies
  SELECT COUNT(DiskCopy.id) INTO nbDCs
    FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
   WHERE DiskCopy.castorfile = cfId
     AND DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.diskpool = DiskPool2SvcClass.parent
     AND DiskPool2SvcClass.child = svcClassId
     AND FileSystem.status = 0 -- PRODUCTION
     AND FileSystem.diskserver = DiskServer.id
     AND DiskServer.status = 0 -- PRODUCTION
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
      FOR SELECT id, path, status, fsRate, mountPoint, name FROM (
            SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status,
                   FileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                                  FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams,
                                  FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) fsRate,
                   FileSystem.mountPoint, DiskServer.name,
                   -- Use the power of analytics to create a cumulative value of the length
                   -- of the diskserver name and filesystem mountpoint. We do this because
                   -- when there are many many copies of a file the requested filesystems
                   -- string created by the stager from these results e.g. ds1:fs1|ds2:fs2|
                   -- can exceed the maximum length allowed by LSF causing the submission
                   -- process to fail.
                   SUM(LENGTH(DiskServer.name) + LENGTH(FileSystem.mountPoint) + 2)
                   OVER (ORDER BY FileSystemRate(FileSystem.readRate, FileSystem.writeRate,
                                                 FileSystem.nbReadStreams,
                                                 FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams,
                                                 FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams)
                          DESC ROWS BETWEEN UNBOUNDED PRECEDING AND CURRENT ROW) bytes
              FROM DiskCopy, SubRequest, FileSystem, DiskServer, DiskPool2SvcClass
             WHERE SubRequest.id = srId
               AND SubRequest.castorfile = DiskCopy.castorfile
               AND FileSystem.diskpool = DiskPool2SvcClass.parent
               AND DiskPool2SvcClass.child = svcClassId
               AND DiskCopy.status IN (0, 6, 10) -- STAGED, STAGEOUT, CANBEMIGR
               AND FileSystem.id = DiskCopy.fileSystem
               AND FileSystem.status = 0  -- PRODUCTION
               AND DiskServer.id = FileSystem.diskServer
               AND DiskServer.status = 0  -- PRODUCTION
             ORDER BY fsRate DESC)
         WHERE bytes <= 200;
    -- Give an hint to the stager whether internal replication can happen or not:
    -- count the number of diskservers which DON'T contain a diskCopy for this castorFile
    -- and are hence eligible for replication should it need to be done
    SELECT COUNT(DISTINCT(DiskServer.name)) INTO nbDSs
      FROM DiskServer, FileSystem, DiskPool2SvcClass
     WHERE FileSystem.diskServer = DiskServer.id
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = svcClassId
       AND FileSystem.status = 0 -- PRODUCTION
       AND DiskServer.status = 0 -- PRODUCTION
       AND DiskServer.id NOT IN (
         SELECT DISTINCT(DiskServer.id)
           FROM DiskCopy, FileSystem, DiskServer
          WHERE DiskCopy.castorfile = cfId
            AND DiskCopy.fileSystem = FileSystem.id
            AND FileSystem.diskserver = DiskServer.id
            AND DiskCopy.status IN (0, 10));  -- STAGED, CANBEMIGR
    IF nbDSs = 0 THEN
      -- no room for replication
      result := 0;  -- DISKCOPY_STAGED
    ELSE
      -- we have some diskservers, the stager will ultimately decide whether to replicate
      result := 1;  -- DISKCOPY_WAITDISK2DISKCOPY
    END IF;
  ELSE
    -- No diskcopies available for this service class:
    -- first check whether there's already a disk to disk copy going on
    BEGIN
      SELECT SubRequest.id INTO d2dsrId
        FROM StageDiskCopyReplicaRequest Req, SubRequest
       WHERE SubRequest.request = Req.id
         AND Req.svcClass = svcClassId    -- this is the destination service class
         AND status IN (13, 14, 6)  -- WAITINGFORSCHED, BEINGSCHED, READY
         AND castorFile = cfId;
      -- found it, wait on it
      UPDATE SubRequest
         SET parent = d2dsrId, status = 5  -- WAITSUBREQ
       WHERE id = srId;
      result := -2;
      COMMIT;
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
        result := 2;  -- DISKCOPY_WAITTAPERECALL
      ELSE
        -- user error
        result := -1;
      END IF;
    END;
  END IF;
END;
/


/* PL/SQL method internalPutDoneFunc, used by fileRecalled and putDoneFunc.
   checks for diskcopies in STAGEOUT and creates the tapecopies for migration
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
      -- Only if we really found the relevant diskcopy, create TapeCopies
      -- This is an extra sanity check, see also the deleteOutOfDateStageOutDCs procedure
      FOR i IN 1..nbTC LOOP
        INSERT INTO TapeCopy (id, copyNb, castorFile, status)
             VALUES (ids_seq.nextval, i, cfId, 0) -- TAPECOPY_CREATED
        RETURNING id INTO tcId;
        INSERT INTO Id2Type (id, type) VALUES (tcId, 30); -- OBJ_TapeCopy
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
      SELECT SubRequest.id INTO srId
        FROM SubRequest,
         (SELECT id FROM StagePrepareToPutRequest UNION ALL
          SELECT id FROM StagePrepareToUpdateRequest) Request
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
  -- get number of TapeCopies to create
  SELECT nbCopies INTO nc FROM FileClass, CastorFile
   WHERE CastorFile.id = cfId AND CastorFile.fileClass = FileClass.id;
  -- and execute the internal putDoneFunc with the number of TapeCopies to be created
  internalPutDoneFunc(cfId, fs, context, nc, svcClassId);
END;
/


/* PL/SQL procedure implementing startRepackMigration */
CREATE OR REPLACE PROCEDURE startRepackMigration
(srId IN INTEGER, cfId IN INTEGER, dcId IN INTEGER,
 reuid OUT INTEGER, regid OUT INTEGER) AS
  nbTC NUMBER(2);
  nbTCInFC NUMBER(2);
  fs NUMBER;
  svcClassId NUMBER;
BEGIN
  UPDATE DiskCopy SET status = 6  -- DISKCOPY_STAGEOUT
   WHERE id = dcId RETURNING diskCopySize INTO fs;
  -- how many do we have to create ?
  SELECT count(StageRepackRequest.repackVid) INTO nbTC
    FROM SubRequest, StageRepackRequest
   WHERE SubRequest.request = StageRepackRequest.id
     AND (SubRequest.id = srId
       OR (SubRequest.castorFile = cfId
         AND SubRequest.status IN (4, 5)));  -- WAITTAPERECALL, WAITSUBREQ
  SELECT nbCopies INTO nbTCInFC
    FROM FileClass, CastorFile
   WHERE CastorFile.id = cfId
     AND FileClass.id = Castorfile.fileclass;
  -- we are not allowed to create more TapeCopies than in the FileClass specified
  IF nbTCInFC < nbTC THEN
    nbTC := nbTCInFC;
  END IF;
  -- update all the Repack subRequests for this file. The status REPACK
  -- stays until the migration to the new tape is over.
  UPDATE SubRequest
     SET diskCopy = dcId, status = 12  -- REPACK
   WHERE SubRequest.castorFile = cfId
     AND SubRequest.status IN (3, 4)  -- WAITSCHED, WAITTAPERECALL
     AND SubRequest.request IN
       (SELECT id FROM StageRepackRequest);   
  -- get the service class, uid and gid
  SELECT R.svcClass, euid, egid INTO svcClassId, reuid, regid
    FROM StageRepackRequest R, SubRequest
   WHERE SubRequest.request = R.id
     AND SubRequest.id = srId;
  -- create the required number of tapecopies for the files
  internalPutDoneFunc(cfId, fs, 0, nbTC, svcClassId);
  -- set svcClass in the CastorFile for the migration
  UPDATE CastorFile SET svcClass = svcClassId WHERE id = cfId;
  -- update remaining STAGED diskcopies to CANBEMIGR too
  -- we may have them as result of disk2disk copies, and so far
  -- we only dealt with dcId
  UPDATE DiskCopy SET status = 10  -- DISKCOPY_CANBEMIGR
   WHERE castorFile = cfId AND status = 0;  -- DISKCOPY_STAGED
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
  repack INTEGER;
  cfId NUMBER;
  srcDcId NUMBER;
  recSvcClass NUMBER;
  recDcId NUMBER;
  recRepack INTEGER;
  reuid NUMBER;
  regid NUMBER;
BEGIN
  -- retrieve the castorfile, the svcclass and the reqId for this subrequest
  SELECT SubRequest.castorFile, Request.euid, Request.egid, Request.svcClass, Request.repack
    INTO cfId, reuid, regid, svcClassId, repack
    FROM (SELECT id, euid, egid, svcClass, 0 repack FROM StagePrepareToGetRequest UNION ALL
          SELECT id, euid, egid, svcClass, 1 repack FROM StageRepackRequest UNION ALL
          SELECT id, euid, egid, svcClass, 0 repack FROM StagePrepareToUpdateRequest) Request,
         SubRequest
   WHERE Subrequest.request = Request.id
     AND Subrequest.id = srId;
  -- lock the castor file to be safe in case of two concurrent subrequest
  SELECT id INTO cfId FROM CastorFile WHERE id = cfId FOR UPDATE;

  -- Look for available diskcopies. Note that we never wait on other requests
  -- and we include WAITDISK2DISKCOPY as they are going to be available.
  SELECT COUNT(DiskCopy.id) INTO nbDCs
    FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
   WHERE DiskCopy.castorfile = cfId
     AND DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.diskpool = DiskPool2SvcClass.parent
     AND DiskPool2SvcClass.child = svcClassId
     AND FileSystem.status = 0 -- PRODUCTION
     AND FileSystem.diskserver = DiskServer.id
     AND DiskServer.status = 0 -- PRODUCTION
     AND DiskCopy.status IN (0, 1, 6, 10);  -- STAGED, WAITDISK2DISKCOPY, STAGEOUT, CANBEMIGR

  -- For DiskCopyReplicaRequests which are waiting to be scheduled, the filesystem
  -- link in the diskcopy table is set to 0. As a consequence of this it is not
  -- possible to determine the service class via the filesystem -> diskpool -> svcclass
  -- relationship, as assumed in the previous query. Instead the service class of
  -- the replication request must be used!!!
  IF nbDCs = 0 THEN
    SELECT COUNT(DiskCopy.id) INTO nbDCs
      FROM DiskCopy, StageDiskCopyReplicaRequest
     WHERE DiskCopy.id = StageDiskCopyReplicaRequest.destDiskCopy
       AND StageDiskCopyReplicaRequest.svcclass = svcClassId
       AND DiskCopy.castorfile = cfId
       AND DiskCopy.status = 1; -- WAITDISK2DISKCOPY
  END IF;

  IF nbDCs > 0 THEN
    -- Yes, we have some
    result := 0;  -- DISKCOPY_STAGED
    IF repack = 1 THEN
      BEGIN
        -- In case of Repack, start the repack migration on one diskCopy
        SELECT DiskCopy.id INTO srcDcId
          FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
         WHERE DiskCopy.castorfile = cfId
           AND DiskCopy.fileSystem = FileSystem.id
           AND FileSystem.diskpool = DiskPool2SvcClass.parent
           AND DiskPool2SvcClass.child = svcClassId
           AND FileSystem.status = 0 -- PRODUCTION
           AND FileSystem.diskserver = DiskServer.id
           AND DiskServer.status = 0 -- PRODUCTION
           AND DiskCopy.status = 0  -- STAGED
           AND ROWNUM < 2;
        startRepackMigration(srId, cfId, srcDcId, reuid, regid);
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- no data found here means that either the file
        -- is being written/migrated or there's a disk-to-disk
        -- copy going on: for this case we should actually wait
        BEGIN
          SELECT DiskCopy.id INTO srcDcId
            FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
           WHERE DiskCopy.castorfile = cfId
             AND DiskCopy.fileSystem = FileSystem.id
             AND FileSystem.diskpool = DiskPool2SvcClass.parent
             AND DiskPool2SvcClass.child = svcClassId
             AND FileSystem.status = 0 -- PRODUCTION
             AND FileSystem.diskserver = DiskServer.id
             AND DiskServer.status = 0 -- PRODUCTION
             AND DiskCopy.status = 1  -- WAITDISK2DISKCOPY
             AND ROWNUM < 2;
          -- found it, we wait on it
          makeSubRequestWait(srId, srcDcId);
          result := -2;
        EXCEPTION WHEN NO_DATA_FOUND THEN
          -- the file is being written/migrated. This may happen in two cases:
          -- either there's another repack going on for the same file, or another
          -- user is overwriting the file.
          -- In the first case, if this request comes for a tape other
          -- than the one being repacked, i.e. the file has a double tape copy,
          -- then we should make the request wait on the first repack (it may be
          -- for a different service class than the one being used right now).
          -- In the second case, we just have to fail this request.
          -- However at the moment it's not easy to restart a waiting repack after
          -- a migration (relevant db callback should be put in rtcpcld_updcFileMigrated(),
          -- rtcpcldCatalogueInterface.c:3300), so we simply fail this repack
          -- request and rely for the time being on Repack to submit
          -- such double tape repacks one by one.
          UPDATE SubRequest
             SET status = 7,  -- FAILED
                 errorCode = 16,  -- EBUSY
                 errorMessage = 'File is currently being written or migrated'
           WHERE id = srId;
          COMMIT;
          result := -1;  -- user error
        END;
      END;
    END IF;
    RETURN;
  END IF;

  -- No diskcopies available for this service class:
  -- we may have to schedule a disk to disk copy or trigger a recall
  checkForD2DCopyOrRecall(cfId, srId, reuid, regid, svcClassId, srcDcId, srvSvcClassId);
  IF srcDcId > 0 THEN  -- disk to disk copy
    IF repack = 1 THEN
      createDiskCopyReplicaRequest(srId, srcDcId, srvSvcClassId, svcClassId, reuid, regid);
      result := -2;  -- Repack waits on the disk to disk copy
    ELSE
      createDiskCopyReplicaRequest(0, srcDcId, srvSvcClassId, svcClassId, reuid, regid);
      result := 1;  -- DISKCOPY_WAITDISK2DISKCOPY, for logging purposes
    END IF;
  ELSIF srcDcId = 0 THEN  -- recall
    BEGIN
      -- check whether there's already a recall, and get its svcClass
      SELECT Request.svcClass, DiskCopy.id, repack
        INTO recSvcClass, recDcId, recRepack
        FROM (SELECT id, svcClass, 0 repack FROM StagePrepareToGetRequest UNION ALL
              SELECT id, svcClass, 0 repack FROM StageGetRequest UNION ALL
              SELECT id, svcClass, 1 repack FROM StageRepackRequest UNION ALL
              SELECT id, svcClass, 0 repack FROM StageUpdateRequest UNION ALL
              SELECT id, svcClass, 0 repack FROM StagePrepareToUpdateRequest) Request,
             SubRequest, DiskCopy
       WHERE SubRequest.request = Request.id
         AND SubRequest.castorFile = cfId
         AND DiskCopy.castorFile = cfId
         AND DiskCopy.status = 2  -- WAITTAPERECALL
         AND SubRequest.status = 4;  -- WAITTAPERECALL
      -- we found one: we need to wait if either we are in a different svcClass
      -- so that afterwards a disk-to-disk copy is triggered, or in case of
      -- Repack so to trigger the repack migration. We also protect ourselves
      -- from a double repack request on the same file.
      IF repack = 1 AND recRepack = 1 THEN
        -- we are not able to handle a double repack, see the detailed comment above
        UPDATE SubRequest
           SET status = 7,  -- FAILED
               errorCode = 16,  -- EBUSY
               errorMessage = 'File is currently being repacked'
         WHERE id = srId;
        COMMIT;
        result := -1;  -- user error
        RETURN;
      END IF;
      IF svcClassId <> recSvcClass OR repack = 1 THEN
        -- make this request wait on the existing WAITTAPERECALL diskcopy
        makeSubRequestWait(srId, recDcId);
        result := -2;
      ELSE
        -- this is a PrepareToGet, and another request is recalling the file
        -- on our svcClass, so we can archive this one
        result := 0;  -- DISKCOPY_STAGED
      END IF;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- let the stager trigger the recall
      result := 2;  -- DISKCOPY_WAITTAPERECALL
    END;
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
  SELECT Req.svcclass, SubRequest.castorfile
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
    SELECT subrequest.id INTO putSubReq
      FROM SubRequest, Id2Type
     WHERE SubRequest.castorfile = cfId
       AND SubRequest.request = Id2Type.id
       AND Id2Type.type IN (40, 44)  -- Put, Update
       AND SubRequest.status IN (0, 1, 2, 3, 6, 13, 14) -- START, RESTART, RETRY, WAITSCHED, READY, READYFORSCHED, BEINGSCHED
       AND ROWNUM < 2;
    -- we've found one, we wait
    UPDATE SubRequest
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
      UPDATE SubRequest SET
        status = 7,  -- FAILED
        errorCode = 1,  -- EPERM
        errorMessage = 'putDone without a put, or wrong service class'
      WHERE id = rsubReqId;
      result := 0;  -- no go
      COMMIT;
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
  -- This, together with triggers will avoid new TapeCopies
  -- or DiskCopies to be added
  SELECT fileclass INTO fclassId FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Determine the context (Put inside PrepareToPut ?)
  BEGIN
    -- check that we are a Put/Update
    SELECT Request.svcClass INTO putSC
      FROM (SELECT id, svcClass FROM StagePutRequest UNION ALL
            SELECT id, svcClass FROM StageUpdateRequest) Request, SubRequest
     WHERE SubRequest.id = srId
       AND Request.id = SubRequest.request;
    BEGIN
      -- check that there is a PrepareToPut/Update going on. There can be only a single one
      -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
      -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
      SELECT SubRequest.diskCopy, PrepareRequest.svcClass INTO dcId, pputSC
        FROM (SELECT id, svcClass FROM StagePrepareToPutRequest UNION ALL
              SELECT id, svcClass FROM StagePrepareToUpdateRequest) PrepareRequest, SubRequest
       WHERE SubRequest.CastorFile = cfId
         AND PrepareRequest.id = SubRequest.request
         AND SubRequest.status = 6;  -- READY
      -- if we got here, we are a Put/Update inside a PrepareToPut
      -- however, are we in the same service class ?
      IF putSC != pputSC THEN
        -- No, this means we are a Put/Update and another PrepareToPut
        -- is already running in a different service class. This is forbidden
        dcId := 0;
        UPDATE SubRequest
           SET status = 7, -- FAILED
               errorCode = 16, -- EBUSY
               errorMessage = 'A prepareToPut is running in another service class for this file'
         WHERE id = srId;
        COMMIT;
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
      -- In case of prepareToPut we need to check that we are we the only one
      SELECT count(SubRequest.diskCopy) INTO nbPReqs
        FROM (SELECT id FROM StagePrepareToPutRequest UNION ALL
              SELECT id FROM StagePrepareToUpdateRequest) PrepareRequest, SubRequest
       WHERE SubRequest.castorFile = cfId
         AND PrepareRequest.id = SubRequest.request
         AND SubRequest.status = 6;  -- READY
      -- Note that we did not select ourselves (we are in status 3)
      IF nbPReqs > 0 THEN
        -- this means we are a PrepareToPut and another PrepareToPut/Update
        -- is already running. This is forbidden
        dcId := 0;
    UPDATE SubRequest
       SET status = 7, -- FAILED
               errorCode = 16, -- EBUSY
               errorMessage = 'Another prepareToPut/Update is ongoing for this file'
         WHERE id = srId;
        COMMIT;
        RETURN;
      END IF;
      -- Everything is ok then
      contextPIPP := 0;
    END;
  END;
  -- check if there is space in the diskpool in case of
  -- disk only pool
  -- First get the svcclass and the user
  SELECT svcClass, euid, egid INTO sclassId, ouid, ogid
    FROM Subrequest,
         (SELECT id, svcClass, euid, egid FROM StagePutRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StageUpdateRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StagePrepareToPutRequest UNION ALL
          SELECT id, svcClass, euid, egid FROM StagePrepareToUpdateRequest) Request
   WHERE SubRequest.id = srId
     AND Request.id = SubRequest.request;
  IF checkFailJobsWhenNoSpace(sclassId) = 1 THEN
    -- The svcClass is declared disk only and has no space
    -- thus we cannot recreate the file
    dcId := 0;
    UPDATE SubRequest
       SET status = 7, -- FAILED
           errorCode = 28, -- ENOSPC
           errorMessage = 'File creation canceled since disk pool is full'
     WHERE id = srId;
    COMMIT;
    RETURN;
  END IF;
  IF contextPIPP = 0 THEN
    -- Puts inside PrepareToPuts don't need the following checks
    -- check if the file existed in advance with a fileclass incompatible with this svcClass
    IF checkFailPutWhenTape0(sclassId, fclassId) = 1 THEN
      -- The svcClass is disk only and the file being overwritten asks for tape copy.
      -- This is impossible, so we deny the operation
      dcId := 0;
      UPDATE SubRequest
         SET status = 7, -- FAILED
             errorCode = 22, -- EINVAL
             errorMessage = 'File recreation canceled since this service class doesn''t provide tape backend'
       WHERE id = srId;
      COMMIT;
      RETURN;
    END IF;
    -- check if recreation is possible for TapeCopies
    SELECT count(*) INTO nbRes FROM TapeCopy
     WHERE status = 3 -- TAPECOPY_SELECTED
      AND castorFile = cfId;
    IF nbRes > 0 THEN
      -- We found something, thus we cannot recreate
      dcId := 0;
      UPDATE SubRequest
         SET status = 7, -- FAILED
             errorCode = 16, -- EBUSY
             errorMessage = 'File recreation canceled since file is being migrated'
        WHERE id = srId;
      COMMIT;
      RETURN;
    END IF;
    -- check if recreation is possible for DiskCopies
    SELECT count(*) INTO nbRes FROM DiskCopy
     WHERE status IN (1, 2, 5, 6, 11) -- WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, STAGEOUT, WAITFS_SCHEDULING
       AND castorFile = cfId;
    IF nbRes > 0 THEN
      -- We found something, thus we cannot recreate
      dcId := 0;
      UPDATE SubRequest
         SET status = 7, -- FAILED
             errorCode = 16, -- EBUSY
             errorMessage = 'File recreation canceled since file is either being recalled, or replicated or created by another user'
       WHERE id = srId;
      COMMIT;
      RETURN;
    END IF;
    -- delete all tapeCopies
    deleteTapeCopies(cfId);
    -- set DiskCopies to INVALID
    UPDATE DiskCopy SET status = 7 -- INVALID
     WHERE castorFile = cfId AND status IN (0, 10); -- STAGED, CANBEMIGR
    -- create new DiskCopy
    SELECT fileId, nsHost INTO fid, nh FROM CastorFile WHERE id = cfId;
    SELECT ids_seq.nextval INTO dcId FROM DUAL;
    buildPathFromFileId(fid, nh, dcId, rpath);
    INSERT INTO DiskCopy (path, id, FileSystem, castorFile, status, creationTime, lastAccessTime, gcWeight, diskCopySize, nbCopyAccesses, owneruid, ownergid)
         VALUES (rpath, dcId, 0, cfId, 5, getTime(), getTime(), 0, 0, 0, ouid, ogid); -- status WAITFS
    INSERT INTO Id2Type (id, type) VALUES (dcId, 5); -- OBJ_DiskCopy
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
          SELECT SubRequest.id INTO srParent
            FROM SubRequest, Id2Type
           WHERE request = Id2Type.id
             AND type IN (40, 44)  -- Put, Update
             AND diskCopy = dcId
             AND status IN (13, 14, 6)  -- READYFORSCHED, BEINGSCHED, READY
             AND ROWNUM < 2;   -- if we have more than one just take one of them
          UPDATE SubRequest
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
  -- Reset svcClass to the request's one as we want to use the new one for migration.
  -- However, we don't reset the filesize, this will be done at prepareForMigration time
  -- so that in case the old file needs to be recovered from tape after e.g. a failed
  -- transfer, we correctly have the previous size.
  UPDATE CastorFile SET svcClass = sclassId
   WHERE id = cfId;
  -- link SubRequest and DiskCopy
  UPDATE SubRequest SET diskCopy = dcId,
                        lastModificationTime = getTime()
   WHERE id = srId;
  -- we don't commit here, the stager will do that when
  -- the subRequest status will be updated to 6
END;
/


/* PL/SQL method implementing selectCastorFile */
CREATE OR REPLACE PROCEDURE selectCastorFile (fId IN INTEGER,
                                              nh IN VARCHAR2,
                                              sc IN INTEGER,
                                              fc IN INTEGER,
                                              fs IN INTEGER,
                                              fn IN VARCHAR2,
                                              lut IN NUMBER,
                                              rid OUT INTEGER,
                                              rfs OUT INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  nsHostName VARCHAR2(2048);
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  BEGIN
    -- try to find an existing file and lock it
    SELECT id, fileSize INTO rid, rfs FROM CastorFile
     WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
    -- update lastAccess time
    UPDATE CastorFile SET LastAccessTime = getTime(),
                          lastKnownFileName = normalizePath(fn)
     WHERE id = rid;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- insert new row
    INSERT INTO CastorFile (id, fileId, nsHost, svcClass, fileClass, fileSize,
                            creationTime, lastAccessTime, lastUpdateTime, lastKnownFileName)
      VALUES (ids_seq.nextval, fId, nsHostName, sc, fc, fs, getTime(), getTime(), lut, normalizePath(fn))
      RETURNING id, fileSize INTO rid, rfs;
    INSERT INTO Id2Type (id, type) VALUES (rid, 2); -- OBJ_CastorFile
  END;
EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
  -- retry the select since a creation was done in between
  SELECT id, fileSize INTO rid, rfs FROM CastorFile
    WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
END;
/

/* PL/SQL method implementing stageRelease */
CREATE OR REPLACE PROCEDURE stageRelease (fid IN INTEGER,
                                          nh IN VARCHAR2,
                                          ret OUT INTEGER) AS
  cfId INTEGER;
  nbRes INTEGER;
  nsHostName VARCHAR2(2048);
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  -- Lock the access to the CastorFile
  -- This, together with triggers will avoid new TapeCopies
  -- or DiskCopies to be added
  SELECT id INTO cfId FROM CastorFile
   WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  -- check if removal is possible for TapeCopies
  SELECT count(*) INTO nbRes FROM TapeCopy
   WHERE status = 3 -- TAPECOPY_SELECTED
     AND castorFile = cfId;
  IF nbRes > 0 THEN
    -- We found something, thus we cannot recreate
    ret := 1;
    RETURN;
  END IF;
  -- check if recreation is possible for SubRequests
  SELECT count(*) INTO nbRes FROM SubRequest
   WHERE status != 11 AND castorFile = cfId;   -- ARCHIVED
  IF nbRes > 0 THEN
    -- We found something, thus we cannot recreate
    ret := 2;
    RETURN;
  END IF;
  -- set DiskCopies to INVALID
  UPDATE DiskCopy SET status = 7 -- INVALID
   WHERE castorFile = cfId AND status = 0; -- STAGED
  ret := 0;
END;
/

/* PL/SQL method implementing stageForcedRm */
CREATE OR REPLACE PROCEDURE stageForcedRm (fid IN INTEGER,
                                           nh IN VARCHAR2) AS
  cfId INTEGER;
  nbRes INTEGER;
  dcsToRm "numList";
  nsHostName VARCHAR2(2048);
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  -- Lock the access to the CastorFile
  -- This, together with triggers will avoid new TapeCopies
  -- or DiskCopies to be added
  SELECT id INTO cfId FROM CastorFile
   WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  -- list diskcopies
  SELECT id BULK COLLECT INTO dcsToRm
    FROM DiskCopy
   WHERE castorFile = cfId
     AND status IN (0, 5, 6, 10, 11);  -- STAGED, WAITFS, STAGEOUT, CANBEMIGR, WAITFS_SCHEDULING
  -- Stop ongoing recalls
  deleteTapeCopies(cfId);
  -- mark all get/put requests for those diskcopies
  -- and the ones waiting on them as failed
  -- so that clients eventually get an answer
  FOR sr IN (SELECT id, status FROM SubRequest
              WHERE diskcopy IN
                (SELECT /*+ CARDINALITY(dcidTable 5) */ *
                   FROM TABLE(dcsToRm) dcidTable)
                AND status IN (0, 1, 2, 5, 6, 12, 13)) LOOP   -- START, RESTART, RETRY, WAITSUBREQ, READY, READYFORSCHED
    UPDATE SubRequest
       SET status = 7,  -- FAILED
           errorCode = 4,  -- EINTR
           errorMessage = 'Canceled by another user request',
           parent = 0
     WHERE (id = sr.id) OR (parent = sr.id);
  END LOOP;
  -- Set selected DiskCopies to INVALID
  FORALL i IN dcsToRm.FIRST .. dcsToRm.LAST
    UPDATE DiskCopy SET status = 7 -- INVALID
     WHERE id = dcsToRm(i);
END;
/


/* PL/SQL method implementing stageRm */
CREATE OR REPLACE PROCEDURE stageRm (srId IN INTEGER,
                                     fid IN INTEGER,
                                     nh IN VARCHAR2,
                                     svcClassId IN INTEGER,
                                     ret OUT INTEGER) AS
  cfId INTEGER;
  scId INTEGER;
  nbRes INTEGER;
  dcsToRm "numList";
  dcStatus INTEGER;
  nsHostName VARCHAR2(2048);
BEGIN
  ret := 0;
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  BEGIN
    -- Lock the access to the CastorFile
    -- This, together with triggers will avoid new TapeCopies
    -- or DiskCopies to be added
    SELECT id INTO cfId FROM CastorFile
     WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- This file does not exist in the stager catalog
    -- so we just fail the request
    UPDATE SubRequest
       SET status = 7,  -- FAILED
           errorCode = 2,  -- ENOENT
           errorMessage = 'File not found on disk cache'
     WHERE id = srId;
    RETURN;
  END;
  -- First select involved diskCopies
  scId := svcClassId;
  dcStatus := 0;
  IF scId > 0 THEN
    SELECT id BULK COLLECT INTO dcsToRm FROM (
      -- first physical diskcopies
      SELECT DC.id
        FROM DiskCopy DC, FileSystem, DiskPool2SvcClass DP2SC
       WHERE DC.castorFile = cfId
         AND DC.status IN (0, 6, 10)  -- STAGED, STAGEOUT, CANBEMIGR
         AND DC.fileSystem = FileSystem.id
         AND FileSystem.diskPool = DP2SC.parent
         AND DP2SC.child = scId)
    UNION ALL (
      -- and then diskcopies resulting from previous PrepareToPut|recall|replica requests
      SELECT DC.id
        FROM (SELECT id, svcClass FROM StagePrepareToPutRequest UNION ALL
              SELECT id, svcClass FROM StagePrepareToUpdateRequest UNION ALL
              SELECT id, svcClass FROM StagePrepareToGetRequest UNION ALL
              SELECT id, svcClass FROM StageRepackRequest UNION ALL
              SELECT id, svcClass FROM StageDiskCopyReplicaRequest) PrepareRequest,
             SubRequest, DiskCopy DC
       WHERE SubRequest.diskCopy = DC.id
         AND PrepareRequest.id = SubRequest.request
         AND PrepareRequest.svcClass = scId
         AND DC.castorFile = cfId
         AND DC.status IN (1, 2, 5, 11)  -- WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, WAITFS_SCHEDULING
      );
    IF dcsToRm.COUNT = 0 THEN
      -- We didn't find anything on this svcClass, fail and return
      UPDATE SubRequest
         SET status = 7,  -- FAILED
             errorCode = 2,  -- ENOENT
             errorMessage = 'File not found on this service class'
       WHERE id = srId;
      RETURN;
    END IF;
    -- Check whether something else is left: if not, do as
    -- we are performing a stageRm everywhere.
    -- First select current status of the diskCopies: if CANBEMIGR,
    -- make sure we don't drop the last remaining valid migratable copy,
    -- i.e. exclude the disk only copies from the count.
    SELECT status INTO dcStatus
      FROM DiskCopy
     WHERE id = dcsToRm(1);
    IF dcStatus = 10 THEN  -- CANBEMIGR
      SELECT count(*) INTO nbRes FROM DiskCopy
       WHERE castorFile = cfId
         AND status = 10  -- CANBEMIGR
         AND id NOT IN (
           (SELECT /*+ CARDINALITY(dcidTable 5) */ *
              FROM TABLE(dcsToRm) dcidTable)
           UNION
           (SELECT DC.id     -- all diskcopies in Tape0 pools
              FROM DiskCopy DC, FileSystem, DiskPool2SvcClass D2S, SvcClass, FileClass
             WHERE DC.castorFile = cfId
               AND DC.fileSystem = FileSystem.id
               AND FileSystem.diskPool = D2S.parent
               AND D2S.child = SvcClass.id
               AND SvcClass.forcedFileClass = FileClass.id
               AND FileClass.nbCopies = 0));
    ELSE
      SELECT count(*) INTO nbRes FROM DiskCopy
         WHERE castorFile = cfId
           AND status IN (0, 2, 5, 6, 10, 11)  -- STAGED, WAITTAPERECALL, STAGEOUT, CANBEMIGR, WAITFS, WAITFS_SCHEDULING
           AND id NOT IN (SELECT /*+ CARDINALITY(dcidTable 5) */ *
                            FROM TABLE(dcsToRm) dcidTable);
    END IF;
    IF nbRes = 0 THEN
      -- nothing found, so we're dropping the last copy; then
      -- we need to perform all the checks to make sure we can
      -- allow the removal.
      scId := 0;
    END IF;
  END IF;

  IF scId = 0 THEN
    -- full cleanup is to be performed, do all necessary checks beforehand
    DECLARE
      segId INTEGER;
      unusedIds "numList";
    BEGIN
      -- check if removal is possible for migration
      SELECT count(*) INTO nbRes FROM DiskCopy
       WHERE status = 10 -- DISKCOPY_CANBEMIGR
         AND castorFile = cfId;
      IF nbRes > 0 THEN
        -- We found something, thus we cannot remove
        UPDATE SubRequest
           SET status = 7,  -- FAILED
               errorCode = 16,  -- EBUSY
               errorMessage = 'The file is not yet migrated'
         WHERE id = srId;
        RETURN;
      END IF;
      -- check if removal is possible for Disk2DiskCopy
      SELECT count(*) INTO nbRes FROM DiskCopy
       WHERE status = 1 -- DISKCOPY_WAITDISK2DISKCOPY
         AND castorFile = cfId;
      IF nbRes > 0 THEN
        -- We found something, thus we cannot remove
        UPDATE SubRequest
           SET status = 7,  -- FAILED
               errorCode = 16,  -- EBUSY
               errorMessage = 'The file is being replicated'
         WHERE id = srId;
        RETURN;
      END IF;
      -- Stop ongoing recalls if stageRm either everywhere or the only available diskcopy.
      -- This is not entirely clean: a proper operation here should be to
      -- drop the SubRequest waiting for recall but keep the recall if somebody
      -- else is doing it, and taking care of other WAITSUBREQ requests as well...
      -- but it's fair enough, provided that the last stageRm will cleanup everything.
      -- XXX First lock all segments for the file. Note that
      -- XXX this step should be dropped once the tapeGateway
      -- XXX is deployed. The current recaller does not take
      -- XXX the proper lock on the castorFiles, hence we
      -- XXX need this here
      SELECT Segment.id BULK COLLECT INTO unusedIds
        FROM Segment, TapeCopy
       WHERE TapeCopy.castorfile = cfId
         AND TapeCopy.id = Segment.copy
       ORDER BY Segment.id
      FOR UPDATE OF Segment.id;
      -- Check whether we have any segment in SELECTED
      SELECT segment.id INTO segId
        FROM Segment, TapeCopy
       WHERE TapeCopy.castorfile = cfId
         AND TapeCopy.id = Segment.copy
         AND Segment.status = 7 -- SELECTED
         AND ROWNUM < 2;
      -- Something is running, so give up
      UPDATE SubRequest
         SET status = 7,  -- FAILED
             errorCode = 16,  -- EBUSY
             errorMessage = 'The file is being recalled from tape'
       WHERE id = srId;
      RETURN;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Nothing running
      deleteTapeCopies(cfId);
      -- Invalidate the DiskCopies
      UPDATE DiskCopy
         SET status = 7  -- INVALID
       WHERE status = 2  -- WAITTAPERECALL
         AND castorFile = cfId;
      -- Mark the 'recall' SubRequests as failed
      -- so that clients eventually get an answer
      UPDATE SubRequest
         SET status = 7,  -- FAILED
             errorCode = 4,  -- EINTR
             errorMessage = 'Recall canceled by another user request'
       WHERE castorFile = cfId and status IN (4, 5);   -- WAITTAPERECALL, WAITSUBREQ
      -- Reselect what needs to be removed
      SELECT id BULK COLLECT INTO dcsToRm
        FROM DiskCopy
       WHERE castorFile = cfId
         AND status IN (0, 1, 2, 5, 6, 10, 11);  -- WAITDISK2DISKCOPY, WAITTAPERECALL, STAGED, WAITFS, STAGEOUT, CANBEMIGR, WAITFS_SCHEDULING
    END;
  END IF;

  -- Now perform the remove:
  -- mark all get/put requests for those diskcopies
  -- and the ones waiting on them as failed
  -- so that clients eventually get an answer
  FORALL i IN dcsToRm.FIRST .. dcsToRm.LAST
    UPDATE SubRequest
       SET status = 7, parent = 0,  -- FAILED
           errorCode = 4,  -- EINTR
           errorMessage = 'Canceled by another user request'
     WHERE diskCopy = dcsToRm(i)
       AND status IN (0, 1, 2, 5, 6, 13);
  -- Set selected DiskCopies to either INVALID or FAILED
  FORALL i IN dcsToRm.FIRST .. dcsToRm.LAST
    UPDATE DiskCopy SET status = 
           decode(status, 1,4, 2,4, 5,4, 11,4, 7) -- WAITDISK2DISKCOPY,WAITTAPERECALL,WAITFS[_SCHED] -> FAILED, others -> INVALID
     WHERE id = dcsToRm(i);
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
BEGIN
  -- Lock the access to the Request
  SELECT Id2Type.id INTO reqId
    FROM SubRequest, Id2Type
   WHERE SubRequest.id = srId
     AND Id2Type.id = SubRequest.request
     FOR UPDATE OF Id2Type.id;
  -- Update Status
  UPDATE SubRequest
     SET status = newStatus,
         answered = 1,
         lastModificationTime = getTime(),
         getNextStatus = decode(newStatus, 6, 1, 8, 1, 9, 1, 0)  -- READY, FINISHED or FAILED_FINISHED -> GETNEXTSTATUS_FILESTAGED
   WHERE id = srId;
  -- Check whether it was the last subrequest in the request
  result := 1;
  SELECT id INTO result FROM SubRequest
   WHERE request = reqId
     AND status IN (0, 1, 2, 3, 4, 5, 7, 10, 12, 13, 14)   -- all but FINISHED, FAILED_FINISHED, ARCHIVED
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


/* PL/SQL method implementing storeClusterStatus */
CREATE OR REPLACE PROCEDURE storeClusterStatus
(machines IN castor."strList",
 fileSystems IN castor."strList",
 machineValues IN castor."cnumList",
 fileSystemValues IN castor."cnumList") AS
 found   NUMBER;
 ind     NUMBER;
 dsId    NUMBER := 0;
 fs      NUMBER := 0;
 fsIds   "numList";
BEGIN
  -- Sanity check
  IF machines.COUNT = 0 OR fileSystems.COUNT = 0 THEN
    RETURN;
  END IF;
  -- First Update Machines
  FOR i IN machines.FIRST .. machines.LAST LOOP
    ind := machineValues.FIRST + 9 * (i - machines.FIRST);
    IF machineValues(ind + 1) = 3 THEN -- ADMIN DELETED
      BEGIN
        -- Resolve the machine name to its id
        SELECT id INTO dsId FROM DiskServer
         WHERE name = machines(i);
        -- If any of the filesystems belonging to the diskserver are currently
        -- in the process of being drained then do not delete the diskserver or
        -- its associated filesystems. Why? Because of unique constraint
        -- violations between the FileSystem and DrainingDiskCopy table.
        SELECT fileSystem BULK COLLECT INTO fsIds
          FROM DrainingFileSystem DFS, FileSystem FS
         WHERE DFS.fileSystem = FS.id
           AND FS.diskServer = dsId;
        IF fsIds.COUNT > 0 THEN
          -- Entries found so flag the draining process as DELETING
          UPDATE DrainingFileSystem
             SET status = 6  -- DELETING
           WHERE fileSystem IN
             (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
                FROM TABLE (fsIds) fsIdTable);
        ELSE
          -- There is no outstanding process to drain the diskservers
          -- filesystems so we can now delete it.
          DELETE FROM Id2Type WHERE id = dsId;
          DELETE FROM Id2Type WHERE id IN
            (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
               FROM TABLE (fsIds) fsIdTable);
          DELETE FROM FileSystem WHERE diskServer = dsId;
          DELETE FROM DiskServer WHERE name = machines(i);
        END IF;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        NULL;  -- Already deleted
      END;
    ELSE
      BEGIN
        SELECT id INTO dsId FROM DiskServer
         WHERE name = machines(i);
        UPDATE DiskServer
           SET status             = machineValues(ind),
               adminStatus        = machineValues(ind + 1),
               readRate           = machineValues(ind + 2),
               writeRate          = machineValues(ind + 3),
               nbReadStreams      = machineValues(ind + 4),
               nbWriteStreams     = machineValues(ind + 5),
               nbReadWriteStreams = machineValues(ind + 6),
               nbMigratorStreams  = machineValues(ind + 7),
               nbRecallerStreams  = machineValues(ind + 8)
         WHERE name = machines(i);
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- We should insert a new machine here
        INSERT INTO DiskServer (name, id, status, adminStatus, readRate,
                                writeRate, nbReadStreams, nbWriteStreams,
                                nbReadWriteStreams, nbMigratorStreams,
                                nbRecallerStreams)
         VALUES (machines(i), ids_seq.nextval, machineValues(ind),
                 machineValues(ind + 1), machineValues(ind + 2),
                 machineValues(ind + 3), machineValues(ind + 4),
                 machineValues(ind + 5), machineValues(ind + 6),
                 machineValues(ind + 7), machineValues(ind + 8));
        INSERT INTO Id2Type (id, type) VALUES (ids_seq.currval, 8); -- OBJ_DiskServer
      END;
    END IF;
    -- Release the lock on the DiskServer as soon as possible to prevent
    -- deadlocks with other activities e.g. recaller
    COMMIT;
  END LOOP;
  -- And then FileSystems
  ind := fileSystemValues.FIRST;
  FOR i in fileSystems.FIRST .. fileSystems.LAST LOOP
    IF fileSystems(i) NOT LIKE ('/%') THEN
      SELECT id INTO dsId FROM DiskServer
       WHERE name = fileSystems(i);
    ELSE
      IF fileSystemValues(ind + 1) = 3 THEN -- ADMIN DELETED
        BEGIN
          -- Resolve the mountpoint name to its id
          SELECT id INTO fs
            FROM FileSystem
           WHERE mountPoint = fileSystems(i)
             AND diskServer = dsId;
          -- Check to see if the filesystem is currently in the process of
          -- being drained. If so, we flag it for deletion.
          found := 0;
          UPDATE DrainingFileSystem
             SET status = 6  -- DELETING
           WHERE fileSystem = fs
          RETURNING fs INTO found;
          -- No entry found so delete the filesystem.
          IF found = 0 THEN
            DELETE FROM Id2Type WHERE id = fs;
            DELETE FROM FileSystem WHERE id = fs;
          END IF;
        EXCEPTION WHEN NO_DATA_FOUND THEN
          NULL;  -- Already deleted
        END;
      ELSE
        BEGIN
          SELECT diskServer INTO dsId FROM FileSystem
           WHERE mountPoint = fileSystems(i) AND diskServer = dsId;
          UPDATE FileSystem
             SET status              = fileSystemValues(ind),
                 adminStatus         = fileSystemValues(ind + 1),
                 readRate            = fileSystemValues(ind + 2),
                 writeRate           = fileSystemValues(ind + 3),
                 nbReadStreams       = fileSystemValues(ind + 4),
                 nbWriteStreams      = fileSystemValues(ind + 5),
                 nbReadWriteStreams  = fileSystemValues(ind + 6),
                 nbMigratorStreams   = fileSystemValues(ind + 7),
                 nbRecallerStreams   = fileSystemValues(ind + 8),
                 free                = fileSystemValues(ind + 9),
                 totalSize           = fileSystemValues(ind + 10),
                 minFreeSpace        = fileSystemValues(ind + 11),
                 maxFreeSpace        = fileSystemValues(ind + 12),
                 minAllowedFreeSpace = fileSystemValues(ind + 13)
           WHERE mountPoint          = fileSystems(i)
             AND diskServer          = dsId;
        EXCEPTION WHEN NO_DATA_FOUND THEN
          -- we should insert a new filesystem here
          INSERT INTO FileSystem (free, mountPoint, minFreeSpace,
                                  minAllowedFreeSpace, maxFreeSpace, totalSize,
                                  readRate, writeRate, nbReadStreams,
                                  nbWriteStreams, nbReadWriteStreams,
                                  nbMigratorStreams, nbRecallerStreams, id,
                                  diskPool, diskserver, status, adminStatus)
          VALUES (fileSystemValues(ind + 9), fileSystems(i), fileSystemValues(ind+11),
                  fileSystemValues(ind + 13), fileSystemValues(ind + 12),
                  fileSystemValues(ind + 10), fileSystemValues(ind + 2),
                  fileSystemValues(ind + 3), fileSystemValues(ind + 4),
                  fileSystemValues(ind + 5), fileSystemValues(ind + 6),
                  fileSystemValues(ind + 7), fileSystemValues(ind + 8),
                  ids_seq.nextval, 0, dsId, 2, 1); -- FILESYSTEM_DISABLED, ADMIN_FORCE
          INSERT INTO Id2Type (id, type) VALUES (ids_seq.currval, 12); -- OBJ_FileSystem
        END;
      END IF;
      ind := ind + 14;
    END IF;
    -- Release the lock on the FileSystem as soon as possible to prevent
    -- deadlocks with other activities e.g. recaller
    COMMIT;
  END LOOP;
END;
/


/* PL/SQL method implementing selectPriority */
CREATE OR REPLACE PROCEDURE selectPriority(
  inUid IN INTEGER,
  inGid IN INTEGER,
  inPriority IN INTEGER,
  dbInfo OUT castor.PriorityMap_Cur) AS
BEGIN
  OPEN dbInfo FOR
    SELECT euid, egid, priority FROM PriorityMap
     WHERE (euid = inUid OR inUid = -1)
       AND (egid = inGid OR inGid = -1)
       AND (priority = inPriority OR inPriority = -1);
END;
/

/* PL/SQL method implementing enterPriority
   it can raise constraint violation exception */
CREATE OR REPLACE PROCEDURE enterPriority(
  inUid IN NUMBER,
  inGid IN NUMBER,
  inPriority IN INTEGER) AS
BEGIN
  INSERT INTO PriorityMap (euid, egid, priority)
  VALUES (inUid, inGid, inPriority);
END;
/


/* PL/SQL method implementing deletePriority */
CREATE OR REPLACE PROCEDURE deletePriority(
  inUid IN INTEGER,
  inGid IN INTEGER) AS
BEGIN
  DELETE FROM PriorityMap
   WHERE (euid = inUid OR inUid = -1)
     AND (egid = inGid OR inGid = -1);
END;
/
/*******************************************************************
 *
 * @(#)RCSfile: oracleJob.sql,v  Revision: 1.687  Date: 2009/07/31 15:17:04  Author: waldron 
 *
 * PL/SQL code for scheduling and job handling
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/


/* This method should be called when the first byte is written to a file
 * opened with an update. This will kind of convert the update from a
 * get to a put behavior.
 */
CREATE OR REPLACE PROCEDURE firstByteWrittenProc(srId IN INTEGER) AS
  dcId NUMBER;
  cfId NUMBER;
  nbres NUMBER;
  stat NUMBER;
  fclassId NUMBER;
  sclassId NUMBER;
BEGIN
  -- Get data and lock the CastorFile
  SELECT castorfile, diskCopy INTO cfId, dcId
    FROM SubRequest WHERE id = srId;
  SELECT fileclass INTO fclassId FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Check that the file is not busy, i.e. that we are not
  -- in the middle of migrating it. If we are, just stop and raise
  -- a user exception
  SELECT count(*) INTO nbRes FROM TapeCopy
    WHERE status = 3 -- TAPECOPY_SELECTED
    AND castorFile = cfId;
  IF nbRes > 0 THEN
    raise_application_error(-20106, 'Trying to update a busy file (ongoing migration)');
  END IF;
  -- Check that we can indeed open the file in write mode
  -- 3 criteria have to be met :
  --   - we are not using a INVALID copy (we should not update an old version)
  --   - we are not in a disk only svcClass and the file class asks for tape copy
  --   - there is no other update/put going on or there is a prepareTo and we are
  --     dealing with the same copy.
  SELECT status INTO stat FROM DiskCopy WHERE id = dcId;
  -- First the invalid case
  IF stat = 7 THEN -- INVALID
    raise_application_error(-20106, 'Trying to update an invalid copy of a file (file has been modified by somebody else concurrently)');
  END IF;
  -- Then the disk only check
  SELECT svcClass INTO sclassId
    FROM Subrequest, StageUpdateRequest Request
   WHERE SubRequest.id = srId
     AND Request.id = SubRequest.request;
  IF checkFailPutWhenTape0(sclassId, fclassId) = 1 THEN
     raise_application_error(-20106, 'File update canceled since this service class doesn''t provide tape backend');
  END IF;
  -- Otherwise, either we are alone or we are on the right copy and we
  -- only have to check that there is a prepareTo statement. We do the check
  -- only when needed, that is STAGEOUT case
  IF stat = 6 THEN -- STAGEOUT
    BEGIN
      -- do we have other ongoing requests ?
      SELECT count(*) INTO nbRes FROM SubRequest WHERE diskCopy = dcId AND id != srId;
      IF (nbRes > 0) THEN
        -- do we have a prepareTo Request ? There can be only a single one
        -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
        -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
        SELECT SubRequest.id INTO nbRes
          FROM (SELECT id FROM StagePrepareToPutRequest UNION ALL
                SELECT id FROM StagePrepareToUpdateRequest) PrepareRequest,
               SubRequest
         WHERE SubRequest.CastorFile = cfId
           AND PrepareRequest.id = SubRequest.request
           AND SubRequest.status = 6;  -- READY
      END IF;
      -- we do have a prepareTo, so eveything is fine
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- No prepareTo, so prevent the writing
      raise_application_error(-20106, 'Trying to update a file already open for write (and no prepareToPut/Update context found)');
    END;
  ELSE
    -- If we are not having a STAGEOUT diskCopy, we are the only ones to write,
    -- so we have to setup everything
    -- invalidate all diskcopies
    UPDATE DiskCopy SET status = 7 -- INVALID
     WHERE castorFile = cfId
       AND status IN (0, 10);
    -- except the one we are dealing with that goes to STAGEOUT
    UPDATE DiskCopy
       SET status = 6 -- STAGEOUT
     WHERE id = dcid;
    -- Suppress all Tapecopies (avoid migration of previous version of the file)
    deleteTapeCopies(cfId);
  END IF;
  -- Invalidate any ongoing replications
  UPDATE DiskCopy SET status = 7 -- INVALID
   WHERE castorFile = cfId
     AND status = 1; -- WAITDISK2DISKCOPY
END;
/


/* Checks whether the protocol used is supporting updates and when not
 * calls firstByteWrittenProc as if the file was already modified */
CREATE OR REPLACE PROCEDURE handleProtoNoUpd
(srId IN INTEGER, protocol VARCHAR2) AS
BEGIN
  IF protocol != 'rfio'  AND
     protocol != 'rfio3' AND
     protocol != 'xroot' THEN
    firstByteWrittenProc(srId);
  END IF;
END;
/


/* PL/SQL method implementing putStart */
CREATE OR REPLACE PROCEDURE putStart
        (srId IN INTEGER, selectedDiskServer IN VARCHAR2, selectedMountPoint IN VARCHAR2,
         rdcId OUT INTEGER, rdcStatus OUT INTEGER, rdcPath OUT VARCHAR2) AS
  srStatus INTEGER;
  srSvcClass INTEGER;
  fsId INTEGER;
  prevFsId INTEGER;
  blah NUMBER;
BEGIN
  -- Get diskCopy and subrequest related information
  SELECT SubRequest.diskCopy, SubRequest.status, DiskCopy.fileSystem,
         Request.svcClass
    INTO rdcId, srStatus, prevFsId, srSvcClass
    FROM SubRequest, DiskCopy,
         (SELECT id, svcClass FROM StagePutRequest UNION ALL
          SELECT id, svcClass FROM StageGetRequest UNION ALL
          SELECT id, svcClass FROM StageUpdateRequest) Request
   WHERE SubRequest.diskcopy = Diskcopy.id
     AND SubRequest.id = srId
     AND SubRequest.request = Request.id;
  -- Check that we did not cancel the SubRequest in the mean time
  IF srStatus IN (7, 9, 10) THEN -- FAILED, FAILED_FINISHED, FAILED_ANSWERING
    raise_application_error(-20104, 'SubRequest canceled while queuing in scheduler. Giving up.');
  END IF;
  -- Check to see if the proposed diskserver and filesystem selected by the
  -- scheduler to run the job is in the correct service class.
  BEGIN
    SELECT FileSystem.id INTO fsId
      FROM DiskServer, FileSystem, DiskPool2SvcClass
     WHERE FileSystem.diskserver = DiskServer.id
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = srSvcClass
       AND DiskServer.name = selectedDiskServer
       AND FileSystem.mountPoint = selectedMountPoint;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20114, 'Job scheduled to the wrong service class. Giving up.');
  END;
  -- Check that a job has not already started for this diskcopy. Refer to
  -- bug #14358
  IF prevFsId > 0 AND prevFsId <> fsId THEN
    raise_application_error(-20107, 'This job has already started for this DiskCopy. Giving up.');
  END IF;
  -- Get older castorFiles with the same name and drop their lastKnownFileName
  UPDATE /*+ INDEX (CastorFile) */ CastorFile
     SET lastKnownFileName = TO_CHAR(id)
   WHERE id IN (
    SELECT /*+ INDEX (cfOld I_CastorFile_lastKnownFileName) */ cfOld.id 
      FROM CastorFile cfOld, CastorFile cfNew, SubRequest
     WHERE cfOld.lastKnownFileName = cfNew.lastKnownFileName
       AND cfOld.fileid <> cfNew.fileid
       AND cfNew.id = SubRequest.castorFile
       AND SubRequest.id = srId);
  -- In case the DiskCopy was in WAITFS_SCHEDULING,
  -- restart the waiting SubRequests
  UPDATE SubRequest 
     SET status = 1, lastModificationTime = getTime(), parent = 0 -- SUBREQUEST_RESTART
   WHERE parent = srId;
  -- link DiskCopy and FileSystem and update DiskCopyStatus
  UPDATE DiskCopy 
     SET status = 6, -- DISKCOPY_STAGEOUT
         fileSystem = fsId,
         nbCopyAccesses = nbCopyAccesses + 1
   WHERE id = rdcId
   RETURNING status, path
   INTO rdcStatus, rdcPath;
END;
/


/* PL/SQL method implementing getUpdateStart */
CREATE OR REPLACE PROCEDURE getUpdateStart
        (srId IN INTEGER, selectedDiskServer IN VARCHAR2, selectedMountPoint IN VARCHAR2,
         dci OUT INTEGER, rpath OUT VARCHAR2, rstatus OUT NUMBER, reuid OUT INTEGER,
         regid OUT INTEGER, diskCopySize OUT NUMBER) AS
  cfid INTEGER;
  fid INTEGER;
  dcId INTEGER;
  fsId INTEGER;
  dcIdInReq INTEGER;
  nh VARCHAR2(2048);
  fileSize INTEGER;
  srSvcClass INTEGER;
  proto VARCHAR2(2048);
  isUpd NUMBER;
  nbAc NUMBER;
  gcw NUMBER;
  gcwProc VARCHAR2(2048);
  cTime NUMBER;
BEGIN
  -- Get data
  SELECT euid, egid, svcClass, upd, diskCopy
    INTO reuid, regid, srSvcClass, isUpd, dcIdInReq
    FROM SubRequest,
        (SELECT id, euid, egid, svcClass, 0 AS upd FROM StageGetRequest UNION ALL
         SELECT id, euid, egid, svcClass, 1 AS upd FROM StageUpdateRequest) Request
   WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
  -- Take a lock on the CastorFile. Associated with triggers,
  -- this guarantees we are the only ones dealing with its copies
  SELECT CastorFile.fileSize, CastorFile.id
    INTO fileSize, cfId
    FROM CastorFile, SubRequest
   WHERE CastorFile.id = SubRequest.castorFile
     AND SubRequest.id = srId FOR UPDATE;
  -- Check to see if the proposed diskserver and filesystem selected by the
  -- scheduler to run the job is in the correct service class.
  BEGIN
    SELECT FileSystem.id INTO fsId
      FROM DiskServer, FileSystem, DiskPool2SvcClass
     WHERE FileSystem.diskserver = DiskServer.id
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = srSvcClass
       AND DiskServer.name = selectedDiskServer
       AND FileSystem.mountPoint = selectedMountPoint;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20114, 'Job scheduled to the wrong service class. Giving up.');
  END;
  -- Try to find local DiskCopy
  SELECT /*+ INDEX(DiskCopy I_DiskCopy_Castorfile) */ id, nbCopyAccesses, gcWeight, creationTime
    INTO dcId, nbac, gcw, cTime
    FROM DiskCopy
   WHERE DiskCopy.castorfile = cfId
     AND DiskCopy.filesystem = fsId
     AND DiskCopy.status IN (0, 6, 10) -- STAGED, STAGEOUT, CANBEMIGR
     AND ROWNUM < 2;
  -- We found it, so we are settled and we'll use the local copy.
  -- It might happen that we have more than one, because LSF may have
  -- scheduled a replication on a fileSystem which already had a previous diskcopy.
  -- We don't care and we randomly took the first one.
  -- First we will compute the new gcWeight of the diskcopy
  IF nbac = 0 THEN
    gcwProc := castorGC.getFirstAccessHook(srSvcClass);
    IF gcwProc IS NOT NULL THEN
      EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:oldGcw, :cTime); END;'
        USING OUT gcw, IN gcw, IN cTime;
    END IF;
  ELSE
    gcwProc := castorGC.getAccessHook(srSvcClass);
    IF gcwProc IS NOT NULL THEN
      EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:oldGcw, :cTime, :nbAc); END;'
        USING OUT gcw, IN gcw, IN cTime, IN nbac;
    END IF;
  END IF;
  -- Here we also update the gcWeight taking into account the new lastAccessTime
  -- and the weightForAccess from our svcClass: this is added as a bonus to
  -- the selected diskCopy.
  UPDATE DiskCopy
     SET gcWeight = gcw,
         lastAccessTime = getTime(),
         nbCopyAccesses = nbCopyAccesses + 1
   WHERE id = dcId
  RETURNING id, path, status, diskCopySize INTO dci, rpath, rstatus, diskCopySize;
  -- Let's update the SubRequest and set the link with the DiskCopy
  UPDATE SubRequest SET DiskCopy = dci WHERE id = srId RETURNING protocol INTO proto;
  -- In case of an update, if the protocol used does not support
  -- updates properly (via firstByteWritten call), we should
  -- call firstByteWritten now and consider that the file is being
  -- modified.
  IF isUpd = 1 THEN
    handleProtoNoUpd(srId, proto);
  END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- No disk copy found on selected FileSystem. This can happen in 3 cases :
  --  + either a diskcopy was available and got disabled before this job
  --    was scheduled. Bad luck, we restart from scratch
  --  + or we are an update creating a file and there is a diskcopy in WAITFS
  --    or WAITFS_SCHEDULING associated to us. Then we have to call putStart
  --  + or we are recalling a 0-size file
  -- So we first check the update hypothesis
  IF isUpd = 1 AND dcIdInReq IS NOT NULL THEN
    DECLARE
      stat NUMBER;
    BEGIN
      SELECT status INTO stat FROM DiskCopy WHERE id = dcIdInReq;
      IF stat IN (5, 11) THEN -- WAITFS, WAITFS_SCHEDULING
        -- it is an update creating a file, let's call putStart
        putStart(srId, selectedDiskServer, selectedMountPoint, dci, rstatus, rpath);
        RETURN;
      END IF;
    END;
  END IF;
  -- Now we check the 0-size file hypothesis
  -- XXX this is currently broken, to be fixed later
  -- It was not an update creating a file, so we restart
  UPDATE SubRequest SET status = 1 WHERE id = srId;
  dci := 0;
  rpath := '';
END;
/


/* PL/SQL method implementing disk2DiskCopyStart */
CREATE OR REPLACE PROCEDURE disk2DiskCopyStart
(dcId IN INTEGER, srcDcId IN INTEGER, destdiskServer IN VARCHAR2,
 destMountPoint IN VARCHAR2, destPath OUT VARCHAR2, destSvcClass OUT VARCHAR2,
 srcDiskServer OUT VARCHAR2, srcMountPoint OUT VARCHAR2, srcPath OUT VARCHAR2,
 srcSvcClass OUT VARCHAR2) AS
  fsId NUMBER;
  cfId NUMBER;
  dsId NUMBER;
  res NUMBER;
  unused NUMBER;
  nbCopies NUMBER;
  cfNsHost VARCHAR2(2048);
BEGIN
  -- Check that we did not cancel the replication request in the mean time
  BEGIN
    SELECT SubRequest.status,
           StageDiskCopyReplicaRequest.svcClassName
      INTO unused, destSvcClass
      FROM SubRequest, StageDiskCopyReplicaRequest
     WHERE SubRequest.diskcopy = dcId
       AND SubRequest.request = StageDiskCopyReplicaRequest.id
       AND SubRequest.status = 6; -- READY
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20111, 'Replication request canceled while queuing in scheduler. Giving up.');
  END;
  -- Check to see if the proposed diskserver and filesystem selected by the
  -- scheduler to run the destination end of disk2disk copy transfer is in the
  -- correct service class. I.e the service class of the original request. This
  -- is done to prevent files being written to an incorrect service class when
  -- diskservers/filesystems are moved.
  BEGIN
    SELECT FileSystem.id, DiskServer.id INTO fsId, dsId
      FROM DiskServer, FileSystem, DiskPool2SvcClass, SvcClass
     WHERE FileSystem.diskserver = DiskServer.id
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = SvcClass.id
       AND SvcClass.name = destSvcClass
       AND DiskServer.name = destDiskServer
       AND FileSystem.mountPoint = destMountPoint;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20108, 'The destination job has been scheduled to the wrong service class.');
  END;
  -- Check whether the source diskcopy is still available. It may no longer be
  -- the case if it got disabled before the job started.
  BEGIN
    SELECT DiskServer.name, FileSystem.mountPoint, DiskCopy.path,
           CastorFile.id, CastorFile.nsHost, SvcClass.name
      INTO srcDiskServer, srcMountPoint, srcPath, cfId, cfNsHost, srcSvcClass
      FROM DiskCopy, CastorFile, DiskServer, FileSystem, DiskPool2SvcClass,
           SvcClass, StageDiskCopyReplicaRequest
     WHERE DiskCopy.id = srcDcId
       AND DiskCopy.castorfile = CastorFile.id
       AND DiskCopy.status IN (0, 10) -- STAGED, CANBEMIGR
       AND FileSystem.id = DiskCopy.filesystem
       AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = SvcClass.id
       AND DiskServer.id = FileSystem.diskserver
       AND DiskServer.status IN (0, 1)
       -- For diskpools which belong to multiple service classes, make sure
       -- we are checking for the file in the correct service class!
       AND StageDiskCopyReplicaRequest.sourceDiskCopy = DiskCopy.id
       AND StageDiskCopyReplicaRequest.sourceSvcClass = SvcClass.id
       AND StageDiskCopyReplicaRequest.destDiskCopy = dcId;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20109, 'The source DiskCopy to be replicated is no longer available.');
  END;
  -- Prevent multiple copies of the file being created on the same diskserver
  SELECT count(*) INTO nbCopies
    FROM DiskCopy, FileSystem
   WHERE DiskCopy.filesystem = FileSystem.id
     AND FileSystem.diskserver = dsId
     AND DiskCopy.castorfile = cfId
     AND DiskCopy.id != dcId
     AND DiskCopy.status IN (0, 1, 2, 10); -- STAGED, DISK2DISKCOPY, WAITTAPERECALL, CANBEMIGR
  IF nbCopies > 0 THEN
    raise_application_error(-20112, 'Multiple copies of this file already found on this diskserver');
  END IF;
  -- Update the filesystem of the destination diskcopy. If the update fails
  -- either the diskcopy doesn't exist anymore indicating the cancellation of
  -- the subrequest or another transfer has already started for it.
  UPDATE DiskCopy SET filesystem = fsId
   WHERE id = dcId
     AND filesystem = 0
     AND status = 1 -- WAITDISK2DISKCOPY
  RETURNING path INTO destPath;
  IF destPath IS NULL THEN
    raise_application_error(-20110, 'A transfer has already started for this DiskCopy.');
  END IF;
END;
/


/* PL/SQL method implementing disk2DiskCopyDone */
CREATE OR REPLACE PROCEDURE disk2DiskCopyDone
(dcId IN INTEGER, srcDcId IN INTEGER) AS
  srId       INTEGER;
  cfId       INTEGER;
  srcStatus  INTEGER;
  srcFsId    NUMBER;
  proto      VARCHAR2(2048);
  reqId      NUMBER;
  svcClassId NUMBER;
  gcwProc    VARCHAR2(2048);
  gcw        NUMBER;
  fileSize   NUMBER;
  ouid       INTEGER;
  ogid       INTEGER;
BEGIN
  -- Lock the CastorFile
  SELECT castorFile INTO cfId
    FROM DiskCopy
   WHERE id = dcId;
  SELECT id INTO cfId FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Try to get the source diskcopy status
  BEGIN
    SELECT status, gcWeight, diskCopySize, fileSystem
      INTO srcStatus, gcw, fileSize, srcFsId
      FROM DiskCopy
     WHERE id = srcDcId
       AND status IN (0, 10);  -- STAGED, CANBEMIGR
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- If no diskcopy was returned it means that the source has either:
    --   A) Been GCed while the copying was taking place or
    --   B) The diskcopy is no longer in a STAGED or CANBEMIGR state.
    -- As we do not know which status to put the new copy in and/or cannot
    -- trust that the file was not modified mid transfer, we invalidate the
    -- new copy.
    UPDATE DiskCopy SET status = 7 WHERE id = dcId -- INVALID
    RETURNING CastorFile INTO cfId;
    -- Restart the SubRequests waiting for the copy
    UPDATE SubRequest SET status = 1, -- SUBREQUEST_RESTART
                          lastModificationTime = getTime()
     WHERE diskCopy = dcId RETURNING id INTO srId;
    UPDATE SubRequest SET status = 1,
                          lastModificationTime = getTime(),
                          parent = 0
     WHERE parent = srId; -- SUBREQUEST_RESTART
    -- Archive the diskCopy replica request
    archiveSubReq(srId, 8);  -- FINISHED
    -- Restart all entries in the snapshot of files to drain that may be
    -- waiting on the replication of the source diskcopy.
    UPDATE DrainingDiskCopy
       SET status = 1,  -- RESTARTED
           parent = 0
     WHERE status = 3  -- RUNNING
       AND (diskCopy = srcDcId
        OR  parent = srcDcId);
    drainFileSystem(srcFsId);
    RETURN;
  END;
  -- Otherwise, we can validate the new diskcopy
  -- update SubRequest and get data
  UPDATE SubRequest SET status = 6, -- SUBREQUEST_READY
                        getNextStatus = 1, -- GETNEXTSTATUS_FILESTAGED
                        lastModificationTime = getTime()
   WHERE diskCopy = dcId RETURNING id, protocol, request
    INTO srId, proto, reqId;
  SELECT SvcClass.id INTO svcClassId
    FROM SvcClass, StageDiskCopyReplicaRequest Req, SubRequest
   WHERE SubRequest.id = srId
     AND SubRequest.request = Req.id
     AND Req.svcClass = SvcClass.id;
  -- Compute gcWeight
  gcwProc := castorGC.getCopyWeight(svcClassId);
  EXECUTE IMMEDIATE
    'BEGIN :newGcw := ' || gcwProc || '(:size, :status, :gcw); END;'
    USING OUT gcw, IN fileSize, srcStatus, gcw;
  -- Update status
  UPDATE DiskCopy
     SET status = srcStatus,
         gcWeight = gcw
   WHERE id = dcId
  RETURNING castorFile, owneruid, ownergid
    INTO cfId, ouid, ogid;
  -- Wake up waiting subrequests
  UPDATE SubRequest SET status = 1,  -- SUBREQUEST_RESTART
                        lastModificationTime = getTime(),
                        parent = 0
   WHERE parent = srId;
  -- Archive the diskCopy replica request
  archiveSubReq(srId, 8);  -- FINISHED
  -- Trigger the creation of additional copies of the file, if necessary.
  replicateOnClose(cfId, ouid, ogid);
  -- Restart all entries in the snapshot of files to drain that may be
  -- waiting on the replication of the source diskcopy.
  UPDATE DrainingDiskCopy
     SET status = 1,  -- RESTARTED
         parent = 0
   WHERE status = 3  -- RUNNING
     AND (diskCopy = srcDcId
      OR  parent = srcDcId);
  drainFileSystem(srcFsId);
  -- WARNING: previous call to drainFileSystem has a COMMIT inside. So all
  -- locks have been released!!
END;
/


/* PL/SQL method implementing disk2DiskCopyFailed */
CREATE OR REPLACE PROCEDURE disk2DiskCopyFailed
(dcId IN INTEGER, enoent IN INTEGER, res OUT INTEGER) AS
  fsId    NUMBER;
  cfId    NUMBER;
  ouid    INTEGER;
  ogid    INTEGER;
  srcDcId NUMBER;
  srcFsId NUMBER;
BEGIN
  fsId := 0;
  res := 0;
  IF enoent = 1 THEN
    -- Set all diskcopies to FAILED. We're preemptying the NS synchronization here
    UPDATE DiskCopy SET status = 4 -- FAILED
     WHERE castorFile =
       (SELECT castorFile FROM DiskCopy WHERE id = dcId); 
  ELSE
    -- Set the diskcopy status to INVALID so that it will be garbage collected
    -- at a later date.
    UPDATE DiskCopy SET status = 7 -- INVALID
     WHERE status = 1 -- WAITDISK2DISKCOPY
       AND id = dcId
     RETURNING fileSystem, castorFile, owneruid, ownergid
      INTO fsId, cfId, ouid, ogid;
  END IF;
  BEGIN
    -- Get the corresponding subRequest, if it exists
    SELECT id INTO res
      FROM SubRequest
     WHERE diskCopy = dcId
       AND status IN (6, 14); -- READY, BEINGSHCED
    -- Wake up other subrequests waiting on it
    UPDATE SubRequest SET status = 1, -- RESTART
                          parent = 0
     WHERE parent = res;
    -- Fail it
    archiveSubReq(res, 9); -- FAILED_FINISHED
    -- If no filesystem was set on the diskcopy then we can safely delete it
    -- without waiting for garbage collection as the transfer was never started
    IF fsId = 0 THEN
      DELETE FROM DiskCopy WHERE id = dcId;
      DELETE FROM Id2Type WHERE id = dcId;
    END IF;
    -- Trigger the creation of additional copies of the file, if necessary.
    -- Note: We do this also on failure to be able to recover from transient
    -- errors, e.g. timeouts while waiting to be scheduled, but we don't on ENOENT.
    IF enoent = 0 THEN
      replicateOnClose(cfId, ouid, ogid);
    END IF;
    -- Diskserver draining logic
    BEGIN
      -- Determine the source diskcopy and filesystem involved in the replication
      SELECT sourceDiskCopy, fileSystem
        INTO srcDcId, srcFsId
        FROM DiskCopy, StageDiskCopyReplicaRequest
       WHERE StageDiskCopyReplicaRequest.sourceDiskCopy = DiskCopy.id
         AND StageDiskCopyReplicaRequest.destDiskCopy = dcId;
      -- Restart all entries in the snapshot of files to drain that may be
      -- waiting on the replication of the source diskcopy.
      UPDATE DrainingDiskCopy
         SET status = 1,  -- RESTARTED
             parent = 0
       WHERE status = 3  -- RUNNING
         AND (diskCopy = srcDcId
          OR  parent = srcDcId);
      drainFileSystem(srcFsId);
    END;
    -- WARNING: previous call to drainFileSystem has a COMMIT inside. So all
    -- locks have been released!!
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- SubRequest not found, don't trigger replicateOnClose here
    -- as we may have been restarted
    NULL;
  END;
END;
/


/* PL/SQL method implementing prepareForMigration
   returnCode can take 3 values :
    - 0 : nothing special
    - 1 : the file got deleted while it was written to
    - 2 : the file is 0 bytes and was thus "migrated" by only
          changing the diskCopy status.
*/
CREATE OR REPLACE PROCEDURE prepareForMigration (srId IN INTEGER,
                                                 fs IN INTEGER,
                                                 ts IN NUMBER,
                                                 fId OUT NUMBER,
                                                 nh OUT VARCHAR2,
                                                 returnCode OUT INTEGER) AS
  cfId INTEGER;
  dcId INTEGER;
  svcId INTEGER;
  realFileSize INTEGER;
  unused INTEGER;
  contextPIPP INTEGER;
BEGIN
  returnCode := 0;
  -- Get CastorFile
  SELECT castorFile, diskCopy INTO cfId, dcId FROM SubRequest WHERE id = srId;
  -- Lock the CastorFile
  SELECT id INTO cfId FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Determine the context (Put inside PrepareToPut or not)
  -- check that we are a Put or an Update
  SELECT Request.id INTO unused
    FROM SubRequest,
       (SELECT id FROM StagePutRequest UNION ALL
        SELECT id FROM StageUpdateRequest) Request
   WHERE SubRequest.id = srId
     AND Request.id = SubRequest.request;
  BEGIN
    -- Check that there is a PrepareToPut/Update going on. There can be only a single one
    -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
    -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
    SELECT SubRequest.diskCopy INTO unused
      FROM SubRequest,
       (SELECT id FROM StagePrepareToPutRequest UNION ALL
        SELECT id FROM StagePrepareToUpdateRequest) Request
     WHERE SubRequest.CastorFile = cfId
       AND Request.id = SubRequest.request
       AND SubRequest.status = 6; -- READY
    -- if we got here, we are a Put inside a PrepareToPut
    contextPIPP := 0;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- here we are a standalone Put
    contextPIPP := 1;
  END;
  -- Check whether the diskCopy is still in STAGEOUT. If not, the file
  -- was deleted via stageRm while being written to. Thus, we should just give up
  BEGIN
    SELECT status INTO unused
      FROM DiskCopy WHERE id = dcId AND status = 6; -- STAGEOUT
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- So we are in the case, we give up
    returnCode := 1;
    -- But we still would like to have the fileId and nameserver
    -- host for logging reasons
    SELECT fileId, nsHost INTO fId, nh
      FROM CastorFile WHERE id = cfId;
    RETURN;
  END;
  -- Now we can safely update CastorFile's file size
  UPDATE CastorFile SET fileSize = fs, lastUpdateTime = ts
   WHERE id = cfId AND (lastUpdateTime IS NULL OR ts >= lastUpdateTime);
  -- If ts < lastUpdateTime, we were late and another job already updated the
  -- CastorFile. So we nevertheless retrieve the real file size, and
  -- we take a lock on the CastorFile. Together with triggers, this insures that
  -- we are the only ones to deal with its copies.
  SELECT fileId, nsHost, fileSize INTO fId, nh, realFileSize
    FROM CastorFile
    WHERE id = cfId
    FOR UPDATE;
  -- Get uid, gid and svcclass from Request
  SELECT svcClass INTO svcId
    FROM SubRequest,
      (SELECT id, svcClass FROM StagePutRequest UNION ALL
       SELECT id, svcClass FROM StageUpdateRequest UNION ALL
       SELECT id, svcClass FROM StagePutDoneRequest) Request
   WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
  IF contextPIPP != 0 THEN
    -- If not a put inside a PrepareToPut/Update, create TapeCopies
    -- and update DiskCopy status
    putDoneFunc(cfId, realFileSize, contextPIPP, svcId);
    -- identify the cases where a file is 0 bytes and has thus been
    -- "migrated" by only updating the DiskCopy status
    -- this case will then be handled by the OraJobSvc in order
    -- to drop any potential existing segment in the nameserver
    IF realFileSize = 0 THEN
      returnCode := 2;
    END IF;
  ELSE
    -- If put inside PrepareToPut/Update, restart any PutDone currently
    -- waiting on this put/update
    UPDATE SubRequest SET status = 1, parent = 0 -- RESTART
     WHERE id IN
      (SELECT SubRequest.id FROM SubRequest, Id2Type
        WHERE SubRequest.request = Id2Type.id
          AND Id2Type.type = 39       -- PutDone
          AND SubRequest.castorFile = cfId
          AND SubRequest.status = 5); -- WAITSUBREQ
  END IF;
  -- Archive Subrequest
  archiveSubReq(srId, 8);  -- FINISHED
END;
/


/* PL/SQL method implementing getUpdateDone */
CREATE OR REPLACE PROCEDURE getUpdateDoneProc
(srId IN NUMBER) AS
BEGIN
  archiveSubReq(srId, 8);  -- FINISHED
END;
/


/* PL/SQL method implementing getUpdateFailed */
CREATE OR REPLACE PROCEDURE getUpdateFailedProc
(srId IN NUMBER) AS
BEGIN
  -- Fail the subrequest. The stager will try and answer the client
  UPDATE SubRequest
     SET status = 7 -- FAILED
   WHERE id = srId;
  -- Wake up other subrequests waiting on it
  UPDATE SubRequest
     SET parent = 0, status = 1, -- RESTART
         lastModificationTime = getTime()
   WHERE parent = srId;
END;
/


/* PL/SQL method implementing putFailedProc */
CREATE OR REPLACE PROCEDURE putFailedProc(srId IN NUMBER) AS
  dcId INTEGER;
  cfId INTEGER;
  unused INTEGER;
BEGIN
  SELECT diskCopy, castorFile INTO dcId, cfId
    FROM SubRequest
   WHERE id = srId;
  -- Fail the subRequest
  UPDATE SubRequest
     SET status = 7 -- FAILED
   WHERE id = srId;
  -- Determine the context (Put inside PrepareToPut/Update ?)
  BEGIN
    -- Check that there is a PrepareToPut/Update going on. There can be only a single one
    -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
    -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
    SELECT SubRequest.id INTO unused
      FROM (SELECT id FROM StagePrepareToPutRequest UNION ALL
            SELECT id FROM StagePrepareToUpdateRequest) PrepareRequest, SubRequest
     WHERE SubRequest.castorFile = cfId
       AND PrepareRequest.id = SubRequest.request
       AND SubRequest.status = 6; -- READY
    -- The select worked out, so we have a prepareToPut/Update
    -- In such a case, we do not cleanup DiskCopy and CastorFile
    -- but we have to wake up a potential waiting putDone
    UPDATE SubRequest SET status = 1, parent = 0 -- RESTART
     WHERE id IN
      (SELECT SubRequest.id
        FROM StagePutDoneRequest, SubRequest
       WHERE SubRequest.CastorFile = cfId
         AND StagePutDoneRequest.id = SubRequest.request
         AND SubRequest.status = 5); -- WAITSUBREQ
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- This means we are a standalone put
    -- thus cleanup DiskCopy and maybe the CastorFile
    -- (the physical file is dropped by the job)
    DELETE FROM DiskCopy WHERE id = dcId;
    DELETE FROM Id2Type WHERE id = dcId;
    deleteCastorFile(cfId);
  END;
END;
/


/* PL/SQL method implementing getSchedulerResources */
CREATE OR REPLACE
PROCEDURE getSchedulerResources(resources OUT castor.SchedulerResources_Cur) AS
BEGIN
  -- Provide a list of all scheduler resources. For the moment this is just
  -- the status of all diskservers, their associated filesystems and the
  -- service class they are in.
  OPEN resources
    FOR SELECT DiskServer.name, DiskServer.status, DiskServer.adminstatus,
               Filesystem.mountpoint, FileSystem.status, FileSystem.adminstatus,
	       SvcClass.name
          FROM DiskServer, FileSystem, DiskPool2SvcClass, SvcClass
         WHERE FileSystem.diskServer = DiskServer.id
           AND FileSystem.diskPool = DiskPool2SvcClass.parent
           AND DiskPool2SvcClass.child = SvcClass.id;
END;
/


/* PL/SQL method implementing failSchedulerJob */
CREATE OR REPLACE
PROCEDURE failSchedulerJob(srSubReqId IN VARCHAR2, srErrorCode IN NUMBER, res OUT INTEGER) AS
  reqType NUMBER;
  srId NUMBER;
  dcId NUMBER;
BEGIN
  -- Get the necessary information needed about the request and subrequest
  SELECT SubRequest.id, SubRequest.diskcopy, Id2Type.type
    INTO srId, dcId, reqType
    FROM SubRequest, Id2Type
   WHERE SubRequest.subreqid = srSubReqId
     AND SubRequest.request = Id2Type.id
     AND SubRequest.status IN (6, 14); -- READY, BEINGSCHED
  -- Set the error code
  UPDATE SubRequest
     SET errorCode = srErrorCode
   WHERE id = srId;
  -- Call the relevant cleanup procedures for the job, procedures that they
  -- would have called themselves if the job had failed!
  IF reqType = 40 THEN -- Put
    putFailedProc(srId);
  ELSIF reqType = 133 THEN -- DiskCopyReplica
    disk2DiskCopyFailed(dcId, 0, res);
  ELSE -- Get or Update
    getUpdateFailedProc(srId);
  END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- The subrequest may have been removed, nothing to be done
  res := 0;
  RETURN;
END;
/


/* PL/SQL method implementing postJobChecks */
CREATE OR REPLACE
PROCEDURE postJobChecks(srSubReqId IN VARCHAR2, srErrorCode IN NUMBER, res OUT INTEGER) AS
  unused NUMBER;
BEGIN
  -- Check to see if the subrequest status is SUBREQUEST_READY
  SELECT status INTO unused FROM SubRequest
   WHERE subreqid = srSubReqId
     AND status = 6; -- READY
  -- The job status is incorrect so terminate the scheduler job
  failSchedulerJob(srSubReqId, srErrorCode, res);
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- The subrequest has the correct status because A) the job was successfully
  -- scheduled in LSF or B) a postJobChecks call has already been performed by
  -- another jobmanager daemon
  res := 0;
  RETURN;
END;
/


/* PL/SQL method implementing jobToSchedule */
CREATE OR REPLACE
PROCEDURE jobToSchedule(srId OUT INTEGER, srSubReqId OUT VARCHAR2, srProtocol OUT VARCHAR2,
                        srXsize OUT INTEGER, srRfs OUT VARCHAR2, reqId OUT VARCHAR2,
                        cfFileId OUT INTEGER, cfNsHost OUT VARCHAR2, reqSvcClass OUT VARCHAR2,
                        reqType OUT INTEGER, reqEuid OUT INTEGER, reqEgid OUT INTEGER,
                        reqUsername OUT VARCHAR2, srOpenFlags OUT VARCHAR2, clientIp OUT INTEGER,
                        clientPort OUT INTEGER, clientVersion OUT INTEGER, clientType OUT INTEGER,
                        reqSourceDiskCopy OUT INTEGER, reqDestDiskCopy OUT INTEGER,
                        clientSecure OUT INTEGER, reqSourceSvcClass OUT VARCHAR2,
                        reqCreationTime OUT INTEGER, reqDefaultFileSize OUT INTEGER,
                        excludedHosts OUT castor.DiskServerList_Cur) AS
  dsId NUMBER;
  cfId NUMBER;
  unused NUMBER;
BEGIN
  -- Get the next subrequest to be scheduled.
  UPDATE SubRequest
     SET status = 14, lastModificationTime = getTime() -- BEINGSCHED
   WHERE status = 13 -- READYFORSCHED
     AND rownum < 2
 RETURNING id, subReqId, protocol, xsize, requestedFileSystems
    INTO srId, srSubReqId, srProtocol, srXsize, srRfs;

  -- Extract the rest of the information required to submit a job into the
  -- scheduler through the job manager.
  SELECT CastorFile.id, CastorFile.fileId, CastorFile.nsHost, SvcClass.name,
         Id2type.type, Request.reqId, Request.euid, Request.egid, Request.username,
	 Request.direction, Request.sourceDiskCopy, Request.destDiskCopy,
         Request.sourceSvcClass, Client.ipAddress, Client.port, Client.version,
	 (SELECT type
            FROM Id2type
           WHERE id = Client.id) clientType, Client.secure, Request.creationTime,
         decode(SvcClass.defaultFileSize, 0, 2000000000, SvcClass.defaultFileSize)
    INTO cfId, cfFileId, cfNsHost, reqSvcClass, reqType, reqId, reqEuid, reqEgid,
         reqUsername, srOpenFlags, reqSourceDiskCopy, reqDestDiskCopy, reqSourceSvcClass,
         clientIp, clientPort, clientVersion, clientType, clientSecure, reqCreationTime,
         reqDefaultFileSize
    FROM SubRequest, CastorFile, SvcClass, Id2type, Client,
         (SELECT id, username, euid, egid, reqid, client, creationTime,
                 'w' direction, svcClass, NULL sourceDiskCopy, NULL destDiskCopy,
                 NULL sourceSvcClass
            FROM StagePutRequest
           UNION ALL
          SELECT id, username, euid, egid, reqid, client, creationTime,
                 'r' direction, svcClass, NULL sourceDiskCopy, NULL destDiskCopy,
                 NULL sourceSvcClass
            FROM StageGetRequest
           UNION ALL
          SELECT id, username, euid, egid, reqid, client, creationTime,
                 'o' direction, svcClass, NULL sourceDiskCopy, NULL destDiskCopy,
                 NULL sourceSvcClass
            FROM StageUpdateRequest
           UNION ALL
          SELECT id, username, euid, egid, reqid, client, creationTime,
                 'w' direction, svcClass, sourceDiskCopy, destDiskCopy,
                 (SELECT name FROM SvcClass WHERE id = sourceSvcClass)
            FROM StageDiskCopyReplicaRequest) Request
   WHERE SubRequest.id = srId
     AND SubRequest.castorFile = CastorFile.id
     AND Request.svcClass = SvcClass.id
     AND Id2type.id = SubRequest.request
     AND Request.id = SubRequest.request
     AND Request.client = Client.id;

  -- Extract additional information required for StageDiskCopyReplicaRequest's
  IF reqType = 133 THEN
    -- Set the requested filesystem for the job to the mountpoint of the
    -- source disk copy. The scheduler plugin needs this information to correctly
    -- schedule access to the filesystem.
    BEGIN
      SELECT CONCAT(CONCAT(DiskServer.name, ':'), FileSystem.mountpoint), DiskServer.id
        INTO srRfs, dsId
        FROM DiskServer, FileSystem, DiskCopy, DiskPool2SvcClass, SvcClass
       WHERE DiskCopy.id = reqSourceDiskCopy
         AND DiskCopy.status IN (0, 10) -- STAGED, CANBEMIGR
         AND DiskCopy.filesystem = FileSystem.id
         AND FileSystem.status IN (0, 1)  -- PRODUCTION, DRAINING
         AND FileSystem.diskserver = DiskServer.id
         AND DiskServer.status IN (0, 1)  -- PRODUCTION, DRAINING
         AND FileSystem.diskPool = DiskPool2SvcClass.parent
         AND DiskPool2SvcClass.child = SvcClass.id
         AND SvcClass.name = reqSourceSvcClass;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- The source diskcopy has been removed before the job manager daemon 
      -- could enter the job into LSF. Under this circumstance fail the 
      -- diskcopy transfer. This will restart the subrequest and trigger a tape
      -- recall if possible
      disk2DiskCopyFailed(reqDestDiskCopy, 0, unused);
      COMMIT;
      RAISE;
    END;

    -- Provide the job manager with a list of hosts to exclude as destination
    -- diskservers.
    OPEN excludedHosts FOR
      SELECT distinct(DiskServer.name)
        FROM DiskCopy, DiskServer, FileSystem, DiskPool2SvcClass, SvcClass
       WHERE DiskCopy.filesystem = FileSystem.id
         AND FileSystem.diskserver = DiskServer.id
         AND FileSystem.diskpool = DiskPool2SvcClass.parent
         AND DiskPool2SvcClass.child = SvcClass.id
         AND SvcClass.name = reqSvcClass
         AND DiskCopy.castorfile = cfId
         AND DiskCopy.id != reqSourceDiskCopy
         AND DiskCopy.status IN (0, 1, 2, 10); -- STAGED, DISK2DISKCOPY, WAITTAPERECALL, CANBEMIGR
  END IF;
END;
/
/*******************************************************************
 *
 * @(#)RCSfile: oracleQuery.sql,v  Revision: 1.662  Date: 2009/08/14 15:49:42  Author: itglp 
 *
 * PL/SQL code for the stager and resource monitoring
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/


/*
 * PL/SQL method implementing the core part of stage queries
 * It takes a list of castorfile ids as input
 */
CREATE OR REPLACE PROCEDURE internalStageQuery
 (cfs IN "numList",
  svcClassId IN NUMBER,
  result OUT castor.QueryLine_Cur) AS
BEGIN
  -- Here we get the status for each cf as follows: if a valid diskCopy is found,
  -- or if a request is found and its related diskCopy exists,
  -- the diskCopy status is returned, else -1 (INVALID) is returned.
  -- The case of svcClassId = 0 (i.e. '*') is handled separately for performance reasons.
  IF svcClassId = 0 THEN
    OPEN result FOR
      SELECT fileId, nsHost, dcId, path, fileSize, status, machine, mountPoint, nbCopyAccesses,
             lastKnownFileName, creationTime, svcClass, lastAccessTime
        FROM (
          SELECT UNIQUE CastorFile.id, CastorFile.fileId, CastorFile.nsHost, DC.id AS dcId,
                 DC.path, CastorFile.fileSize, DC.status,
                 CASE WHEN DC.svcClass IS NULL THEN
                   -- this only happens on waiting for recall/replica or prepareToPut
                   (SELECT UNIQUE Req.svcClassName
                      FROM SubRequest,
                        (SELECT id, svcClassName FROM StagePrepareToGetRequest    UNION ALL
                         SELECT id, svcClassName FROM StagePrepareToPutRequest    UNION ALL
                         SELECT id, svcClassName FROM StagePrepareToUpdateRequest UNION ALL
                         SELECT id, svcClassName FROM StageRepackRequest          UNION ALL
                         SELECT id, svcClassName FROM StageGetRequest) Req
                          WHERE SubRequest.castorFile = CastorFile.id
                            AND request = Req.id
                            AND rownum < 2)   -- if many requests exist for the same file, pick one of them         
                   ELSE DC.svcClass END AS svcClass,
                 DC.machine, DC.mountPoint, DC.nbCopyAccesses, CastorFile.lastKnownFileName,
                 DC.creationTime, DC.lastAccessTime
            FROM CastorFile,
              (SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status, DiskServer.name AS machine, FileSystem.mountPoint,
                      SvcClass.name AS svcClass, DiskCopy.filesystem, DiskCopy.CastorFile, 
                      DiskCopy.nbCopyAccesses, DiskCopy.creationTime, DiskCopy.lastAccessTime
                 FROM FileSystem, DiskServer, DiskPool2SvcClass, SvcClass,
                   (SELECT id, status, filesystem, castorFile, path, nbCopyAccesses, creationTime, lastAccessTime
                      FROM DiskCopy
                     WHERE CastorFile IN (SELECT /*+ CARDINALITY(cfidTable 5) */ * FROM TABLE(cfs) cfidTable)
                       AND status IN (0, 1, 2, 4, 5, 6, 7, 10, 11) -- search for diskCopies not BEINGDELETED
                     GROUP BY (id, status, filesystem, castorfile, path, nbCopyAccesses, creationTime, lastAccessTime)) DiskCopy
                WHERE FileSystem.id(+) = DiskCopy.fileSystem
                  AND nvl(FileSystem.status, 0) = 0 -- PRODUCTION
                  AND DiskServer.id(+) = FileSystem.diskServer
                  AND nvl(DiskServer.status, 0) = 0 -- PRODUCTION
                  AND DiskPool2SvcClass.parent(+) = FileSystem.diskPool
                  AND SvcClass.id(+) = DiskPool2SvcClass.child) DC
           WHERE CastorFile.id IN (SELECT /*+ CARDINALITY(cfidTable 5) */ * FROM TABLE(cfs) cfidTable)
             AND CastorFile.id = DC.castorFile)
       WHERE status IS NOT NULL    -- search for valid diskcopies
       ORDER BY fileid, nshost;
  ELSE
    OPEN result FOR
      SELECT fileId, nsHost, dcId, path, fileSize, status, machine, mountPoint, nbCopyAccesses,
             lastKnownFileName, creationTime, (SELECT name FROM svcClass WHERE id = svcClassId),
             lastAccessTime
        FROM (
          SELECT UNIQUE CastorFile.id, CastorFile.fileId, CastorFile.nsHost, DC.id AS dcId,
                 DC.path, CastorFile.fileSize,
                 CASE WHEN DC.dcSvcClass = svcClassId THEN DC.status
                      WHEN DC.fileSystem = 0 THEN
                       (SELECT UNIQUE decode(nvl(SubRequest.status, -1), -1, -1, DC.status)
                          FROM SubRequest,
                            (SELECT id, svcClass FROM StagePrepareToGetRequest    UNION ALL
                             SELECT id, svcClass FROM StagePrepareToPutRequest    UNION ALL
                             SELECT id, svcClass FROM StagePrepareToUpdateRequest UNION ALL
                             SELECT id, svcClass FROM StageRepackRequest          UNION ALL
                             SELECT id, svcClass FROM StageGetRequest) Req
                              WHERE SubRequest.CastorFile = CastorFile.id
                                AND request = Req.id
                                AND svcClass = svcClassId)
                      END AS status,
                 DC.machine, DC.mountPoint, DC.nbCopyAccesses, CastorFile.lastKnownFileName,
                 DC.creationTime, DC.lastAccessTime
            FROM CastorFile,
              (SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status, DiskServer.name AS machine, FileSystem.mountPoint,
                      DiskPool2SvcClass.child AS dcSvcClass, DiskCopy.filesystem, DiskCopy.CastorFile, 
                      DiskCopy.nbCopyAccesses, DiskCopy.creationTime, DiskCopy.lastAccessTime
                 FROM FileSystem, DiskServer, DiskPool2SvcClass,
                   (SELECT id, status, filesystem, castorFile, path, nbCopyAccesses, creationTime, lastAccessTime
                      FROM DiskCopy
                     WHERE CastorFile IN (SELECT /*+ CARDINALITY(cfidTable 5) */ * FROM TABLE(cfs) cfidTable)
                       AND status IN (0, 1, 2, 4, 5, 6, 7, 10, 11)  -- search for diskCopies not GCCANDIDATE or BEINGDELETED
                     GROUP BY (id, status, filesystem, castorfile, path, nbCopyAccesses, creationTime, lastAccessTime)) DiskCopy
                WHERE FileSystem.id(+) = DiskCopy.fileSystem
                  AND nvl(FileSystem.status, 0) = 0 -- PRODUCTION
                  AND DiskServer.id(+) = FileSystem.diskServer
                  AND nvl(DiskServer.status, 0) = 0 -- PRODUCTION
                  AND DiskPool2SvcClass.parent(+) = FileSystem.diskPool) DC
           WHERE CastorFile.id IN (SELECT /*+ CARDINALITY(cfidTable 5) */ * FROM TABLE(cfs) cfidTable)
             AND CastorFile.id = DC.castorFile)
       WHERE status IS NOT NULL     -- search for valid diskcopies
       ORDER BY fileid, nshost;
   END IF;
END;
/


/*
 * PL/SQL method implementing the stager_qry based on file name for directories
 */
CREATE OR REPLACE PROCEDURE fileNameStageQuery
 (fn IN VARCHAR2,
  svcClassId IN INTEGER,
  maxNbResponses IN INTEGER,
  result OUT castor.QueryLine_Cur) AS
  cfIds "numList";
BEGIN
  IF substr(fn, -1, 1) = '/' THEN  -- files in a 'subdirectory'
    SELECT /*+ INDEX(CastorFile I_CastorFile_LastKnownFileName) INDEX(DiskCopy I_DiskCopy_CastorFile) */ 
           CastorFile.id
      BULK COLLECT INTO cfIds
      FROM DiskCopy, FileSystem, DiskPool2SvcClass, CastorFile
     WHERE CastorFile.lastKnownFileName LIKE normalizePath(fn)||'%'
       AND DiskCopy.castorFile = CastorFile.id
       AND DiskCopy.fileSystem = FileSystem.id
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND (DiskPool2SvcClass.child = svcClassId OR svcClassId = 0)
       AND ROWNUM <= maxNbResponses + 1;
  -- ELSE exact match, not implemented here any longer but in fileIdStageQuery 
  END IF;
  IF cfIds.COUNT > maxNbResponses THEN
    -- We have too many rows, we just give up
    raise_application_error(-20102, 'Too many matching files');
  END IF;
  internalStageQuery(cfIds, svcClassId, result);
END;
/


/*
 * PL/SQL method implementing the stager_qry based on file id or single filename
 */
CREATE OR REPLACE PROCEDURE fileIdStageQuery
 (fid IN NUMBER,
  nh IN VARCHAR2,
  svcClassId IN INTEGER,
  fileName IN VARCHAR2,
  result OUT castor.QueryLine_Cur) AS
  cfs "numList";
  currentFileName VARCHAR2(2048);
  nsHostName VARCHAR2(2048);
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  -- Extract CastorFile ids from the fileid
  SELECT id BULK COLLECT INTO cfs FROM CastorFile 
   WHERE fileId = fid AND nshost = nsHostName;
  -- Check and fix when needed the LastKnownFileNames
  IF (cfs.COUNT > 0) THEN
    SELECT lastKnownFileName INTO currentFileName
      FROM CastorFile
     WHERE id = cfs(cfs.FIRST);
    IF currentFileName != fileName THEN
      UPDATE CastorFile SET lastKnownFileName = fileName
       WHERE id = cfs(cfs.FIRST);
      COMMIT;
    END IF;
  END IF;
  -- Finally issue the actual query
  internalStageQuery(cfs, svcClassId, result);
END;
/


/*
 * PL/SQL method implementing the stager_qry based on request id
 */
CREATE OR REPLACE PROCEDURE reqIdStageQuery
 (rid IN VARCHAR2,
  svcClassId IN INTEGER,
  notfound OUT INTEGER,
  result OUT castor.QueryLine_Cur) AS
  cfs "numList";
BEGIN
  SELECT /*+ NO_USE_HASH(REQLIST SR) USE_NL(REQLIST SR) 
             INDEX(SR I_SUBREQUEST_REQUEST) */
         sr.castorfile BULK COLLECT INTO cfs
    FROM SubRequest sr,
         (SELECT id
            FROM StagePreparetogetRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM StagePreparetoputRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM StagePreparetoupdateRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM stageGetRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM stagePutRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM StageRepackRequest
           WHERE reqid LIKE rid) reqlist
   WHERE sr.request = reqlist.id;
  IF cfs.COUNT > 0 THEN
    internalStageQuery(cfs, svcClassId, result);
  ELSE
    notfound := 1;
  END IF;
END;
/


/*
 * PL/SQL method implementing the stager_qry based on user tag
 */
CREATE OR REPLACE PROCEDURE userTagStageQuery
 (tag IN VARCHAR2,
  svcClassId IN INTEGER,
  notfound OUT INTEGER,
  result OUT castor.QueryLine_Cur) AS
  cfs "numList";
BEGIN
  SELECT sr.castorfile BULK COLLECT INTO cfs
    FROM SubRequest sr,
         (SELECT id
            FROM StagePreparetogetRequest
           WHERE userTag LIKE tag
          UNION ALL
          SELECT id
            FROM StagePreparetoputRequest
           WHERE userTag LIKE tag
          UNION ALL
          SELECT id
            FROM StagePreparetoupdateRequest
           WHERE userTag LIKE tag
          UNION ALL
          SELECT id
            FROM stageGetRequest
           WHERE userTag LIKE tag
          UNION ALL
          SELECT id
            FROM stagePutRequest
           WHERE userTag LIKE tag) reqlist
   WHERE sr.request = reqlist.id;
  IF cfs.COUNT > 0 THEN
    internalStageQuery(cfs, svcClassId, result);
  ELSE
    notfound := 1;
  END IF;
END;
/


/*
 * PL/SQL method implementing the LastRecalls stager_qry based on request id
 */
CREATE OR REPLACE PROCEDURE reqIdLastRecallsStageQuery
 (rid IN VARCHAR2,
  svcClassId IN INTEGER,
  notfound OUT INTEGER,
  result OUT castor.QueryLine_Cur) AS
  cfs "numList";
  reqs "numList";
BEGIN
  SELECT id BULK COLLECT INTO reqs
    FROM (SELECT id
            FROM StagePreparetogetRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM StagePreparetoupdateRequest
           WHERE reqid LIKE rid
          UNION ALL
          SELECT id
            FROM StageRepackRequest
           WHERE reqid LIKE rid
          );
  IF reqs.COUNT > 0 THEN
    FORALL i IN reqs.FIRST..reqs.LAST
      UPDATE SubRequest SET getNextStatus = 2  -- GETNEXTSTATUS_NOTIFIED
       WHERE getNextStatus = 1  -- GETNEXTSTATUS_FILESTAGED
         AND request = reqs(i)
      RETURNING castorfile BULK COLLECT INTO cfs;
    internalStageQuery(cfs, svcClassId, result);
  ELSE
    notfound := 1;
  END IF;
END;
/


/*
 * PL/SQL method implementing the LastRecalls stager_qry based on user tag
 */
CREATE OR REPLACE PROCEDURE userTagLastRecallsStageQuery
 (tag IN VARCHAR2,
  svcClassId IN INTEGER,
  notfound OUT INTEGER,
  result OUT castor.QueryLine_Cur) AS
  cfs "numList";
  reqs "numList";
BEGIN
  SELECT id BULK COLLECT INTO reqs
    FROM (SELECT id
            FROM StagePreparetogetRequest
           WHERE userTag LIKE tag
          UNION ALL
          SELECT id
            FROM StagePreparetoupdateRequest
           WHERE userTag LIKE tag
          );
  IF reqs.COUNT > 0 THEN
    FORALL i IN reqs.FIRST..reqs.LAST
      UPDATE SubRequest SET getNextStatus = 2  -- GETNEXTSTATUS_NOTIFIED
       WHERE getNextStatus = 1  -- GETNEXTSTATUS_FILESTAGED
         AND request = reqs(i)
      RETURNING castorfile BULK COLLECT INTO cfs;
    internalStageQuery(cfs, svcClassId, result);
  ELSE
    notfound := 1;
  END IF;
END;
/

/* Internal function used by describeDiskPool[s] to return free available
 * space, i.e. the free space on PRODUCTION status resources */
CREATE OR REPLACE FUNCTION getAvailFreeSpace(status IN NUMBER, freeSpace IN NUMBER) RETURN NUMBER IS
BEGIN
  IF status > 0 THEN
    -- anything but PRODUCTION == 0 means no space
    RETURN 0;
  END IF;
  IF freeSpace < 0 THEN
    -- over used filesystems may report negative numbers, just cut to 0
    RETURN 0;
  ELSE
    RETURN freeSpace;
  END IF;
END;
/

/*
 * PL/SQL method implementing the diskPoolQuery when listing pools
 */
CREATE OR REPLACE PROCEDURE describeDiskPools
(svcClassName IN VARCHAR2, reqEuid IN INTEGER, reqEgid IN INTEGER,
 res OUT NUMBER, result OUT castor.DiskPoolsQueryLine_Cur) AS
BEGIN
  -- We use here analytic functions and the grouping sets functionnality to
  -- get both the list of filesystems and a summary per diskserver and per
  -- diskpool. The grouping analytic function also allows to mark the summary
  -- lines for easy detection in the C++ code
  IF svcClassName IS NULL THEN
    OPEN result FOR
      SELECT grouping(ds.name) AS IsDSGrouped,
             grouping(fs.mountPoint) AS IsFSGrouped,
             dp.name, ds.name, ds.status, fs.mountPoint,
             sum(getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize)) AS freeSpace,
             sum(fs.totalSize),
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp
       WHERE dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         GROUP BY grouping sets(
             (dp.name, ds.name, ds.status, fs.mountPoint,
               getAvailFreeSpace(fs.status + ds.status,
                                 fs.free - fs.minAllowedFreeSpace * fs.totalSize),
               fs.totalSize,
               fs.minFreeSpace, fs.maxFreeSpace, fs.status),
             (dp.name, ds.name, ds.status),
             (dp.name)
            )
         ORDER BY dp.name, IsDSGrouped DESC, ds.name, IsFSGrouped DESC, fs.mountpoint;
  ELSE 
    OPEN result FOR
      SELECT grouping(ds.name) AS IsDSGrouped,
             grouping(fs.mountPoint) AS IsFSGrouped,
             dp.name, ds.name, ds.status, fs.mountPoint,
             sum(getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize)) AS freeSpace,
             sum(fs.totalSize),
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp,
             DiskPool2SvcClass d2s, SvcClass sc
       WHERE sc.name = svcClassName
         AND sc.id = d2s.child
         AND checkPermissionOnSvcClass(sc.name, reqEuid, reqEgid, 103) = 0
         AND d2s.parent = dp.id
         AND dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         GROUP BY grouping sets(
             (dp.name, ds.name, ds.status, fs.mountPoint,
               getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize),
               fs.totalSize,
               fs.minFreeSpace, fs.maxFreeSpace, fs.status),
             (dp.name, ds.name, ds.status),
             (dp.name)
            )
         ORDER BY dp.name, IsDSGrouped DESC, ds.name, IsFSGrouped DESC, fs.mountpoint;
  END IF;
  -- If no results are available, check to see if any diskpool exists and if
  -- access to view all the diskpools has been revoked. The information extracted
  -- here will be used to send an appropriate error message to the client.
  IF result%ROWCOUNT = 0 THEN
    SELECT CASE count(*)
           WHEN sum(checkPermissionOnSvcClass(sc.name, reqEuid, reqEgid, 103)) THEN -1
           ELSE count(*) END
      INTO res
      FROM DiskPool2SvcClass d2s, SvcClass sc
     WHERE d2s.child = sc.id
       AND (sc.name = svcClassName OR svcClassName IS NULL);
  END IF;
END;
/



/*
 * PL/SQL method implementing the diskPoolQuery for a given pool
 */
CREATE OR REPLACE PROCEDURE describeDiskPool
(diskPoolName IN VARCHAR2, svcClassName IN VARCHAR2,
 res OUT NUMBER, result OUT castor.DiskPoolQueryLine_Cur) AS
BEGIN
  -- We use here analytic functions and the grouping sets functionnality to
  -- get both the list of filesystems and a summary per diskserver and per
  -- diskpool. The grouping analytic function also allows to mark the summary
  -- lines for easy detection in the C++ code
  IF svcClassName IS NULL THEN
    OPEN result FOR
      SELECT grouping(ds.name) AS IsDSGrouped,
             grouping(fs.mountPoint) AS IsGrouped,
             ds.name, ds.status, fs.mountPoint,
             sum(getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize)) AS freeSpace,
             sum(fs.totalSize),
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp
       WHERE dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         AND dp.name = diskPoolName
         GROUP BY grouping sets(
             (ds.name, ds.status, fs.mountPoint,
               getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize),
               fs.totalSize,
               fs.minFreeSpace, fs.maxFreeSpace, fs.status),
             (ds.name, ds.status),
             (dp.name)
            )
         ORDER BY IsDSGrouped DESC, ds.name, IsGrouped DESC, fs.mountpoint;
  ELSE
    OPEN result FOR
      SELECT grouping(ds.name) AS IsDSGrouped,
             grouping(fs.mountPoint) AS IsGrouped,
             ds.name, ds.status, fs.mountPoint,
             sum(getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize)) AS freeSpace,
             sum(fs.totalSize),
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp,
             DiskPool2SvcClass d2s, SvcClass sc
       WHERE sc.name = svcClassName
         AND sc.id = d2s.child
         AND d2s.parent = dp.id
         AND dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         AND dp.name = diskPoolName
         GROUP BY grouping sets(
             (ds.name, ds.status, fs.mountPoint,
               getAvailFreeSpace(fs.status + ds.status,
                 fs.free - fs.minAllowedFreeSpace * fs.totalSize),
               fs.totalSize,
               fs.minFreeSpace, fs.maxFreeSpace, fs.status),
             (ds.name, ds.status),
             (dp.name)
            )
         ORDER BY IsDSGrouped DESC, ds.name, IsGrouped DESC, fs.mountpoint;
  END IF;
  -- If no results are available, check to see if any diskpool exists and if
  -- access to view all the diskpools has been revoked. The information extracted
  -- here will be used to send an appropriate error message to the client.
  IF result%ROWCOUNT = 0 THEN
    SELECT count(*) INTO res
      FROM DiskPool dp, DiskPool2SvcClass d2s, SvcClass sc
     WHERE d2s.child = sc.id
       AND d2s.parent = dp.id
       AND dp.name = diskPoolName
       AND (sc.name = svcClassName OR svcClassName IS NULL);
  END IF;
END;
/
/*******************************************************************	
 * @(#)RCSfile: oracleTape.sql,v  Revision: 1.761  Date: 2009/08/10 15:30:13  Author: itglp 
 *
 * PL/SQL code for the interface to the tape system
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PL/SQL declaration for the castorTape package */
CREATE OR REPLACE PACKAGE castorTape AS 
   TYPE TapeGatewayRequestExtended IS RECORD (
    accessMode NUMBER,
    id NUMBER,
    starttime NUMBER,
    lastvdqmpingtime NUMBER, 
    vdqmvolreqid NUMBER, 
    status NUMBER,
    vid VARCHAR2(2048));
  TYPE TapeGatewayRequest_Cur IS REF CURSOR RETURN TapeGatewayRequestExtended;
  TYPE TapeGatewayRequestCore IS RECORD (
    tpmode INTEGER,
    side INTEGER,
    vid VARCHAR2(2048),
    tapeRequestId NUMBER);
  TYPE TapeGatewayRequestCore_Cur IS REF CURSOR RETURN TapeGatewayRequestCore;
  TYPE StreamCore IS RECORD (
    id INTEGER,
    initialSizeToTransfer INTEGER,
    status NUMBER,
    tapePoolId NUMBER,
    tapePoolName VARCHAR2(2048));
  TYPE Stream_Cur IS REF CURSOR RETURN StreamCore; 
  TYPE DbMigrationInfo IS RECORD (
    id NUMBER,
    copyNb NUMBER,
    fileName VARCHAR2(2048),
    nsHost VARCHAR2(2048),
    fileId NUMBER,
    fileSize NUMBER);
  TYPE DbMigrationInfo_Cur IS REF CURSOR RETURN DbMigrationInfo;
  TYPE DbStreamInfo IS RECORD (
    id NUMBER,
    numFile NUMBER,
    byteVolume NUMBER,
    age NUMBER);
  TYPE DbStreamInfo_Cur IS REF CURSOR RETURN DbStreamInfo;
  TYPE DbRecallInfo IS RECORD (
    vid VARCHAR2(2048),
    tapeId NUMBER,
    dataVolume NUMBER,
    numbFiles NUMBER,
    expireTime NUMBER,
    priority NUMBER);
  TYPE DbRecallInfo_Cur IS REF CURSOR RETURN DbRecallInfo;
  TYPE FileToRecallCore IS RECORD (
   fileId NUMBER,
   nsHost VARCHAR2(2048),
   diskserver VARCHAR2(2048),
   mountPoint VARCHAR(2048),
   path VARCHAR2(2048),
   fseq INTEGER,
   fileTransactionId NUMBER);
  TYPE FileToRecallCore_Cur IS REF CURSOR RETURN  FileToRecallCore;  
  TYPE FileToMigrateCore IS RECORD (
   fileId NUMBER,
   nsHost VARCHAR2(2048),
   lastModificationTime NUMBER,
   diskserver VARCHAR2(2048),
   mountPoint VARCHAR(2048),
   path VARCHAR2(2048),
   lastKnownFilename VARCHAR2(2048), 
   fseq INTEGER,
   fileSize NUMBER,
   fileTransactionId NUMBER);
  TYPE FileToMigrateCore_Cur IS REF CURSOR RETURN  FileToMigrateCore;  
END castorTape;
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

/* PL/SQL methods to update FileSystem weight for new migrator streams */
CREATE OR REPLACE PROCEDURE updateFsMigratorOpened
(ds IN INTEGER, fs IN INTEGER, fileSize IN INTEGER) AS
BEGIN
  /* We lock first the diskserver in order to lock all the
     filesystems of this DiskServer in an atomical way */
  UPDATE DiskServer SET nbMigratorStreams = nbMigratorStreams + 1 WHERE id = ds;
  UPDATE FileSystem SET nbMigratorStreams = nbMigratorStreams + 1 WHERE id = fs;
END;
/

/* PL/SQL methods to update FileSystem weight for new recaller streams */
CREATE OR REPLACE PROCEDURE updateFsRecallerOpened
(ds IN INTEGER, fs IN INTEGER, fileSize IN INTEGER) AS
BEGIN
  /* We lock first the diskserver in order to lock all the
     filesystems of this DiskServer in an atomical way */
  UPDATE DiskServer SET nbRecallerStreams = nbRecallerStreams + 1 WHERE id = ds;
  UPDATE FileSystem SET nbRecallerStreams = nbRecallerStreams + 1,
                        free = free - fileSize   -- just an evaluation, monitoring will update it
   WHERE id = fs;
END;
/


/* PL/SQL method implementing anyTapeCopyForStream.*/
CREATE OR REPLACE PROCEDURE anyTapeCopyForStream(streamId IN INTEGER, res OUT INTEGER) AS
  unused INTEGER;
BEGIN
  -- JUST rtcpclientd
  SELECT /*+ FIRST_ROWS */ TapeCopy.id INTO unused
    FROM DiskServer, FileSystem, DiskCopy, TapeCopy, Stream2TapeCopy
   WHERE DiskServer.id = FileSystem.diskserver
     AND DiskServer.status IN (0, 1) -- DISKSERVER_PRODUCTION, DISKSERVER_DRAINING
     AND FileSystem.id = DiskCopy.filesystem
     AND FileSystem.status IN (0, 1) -- FILESYSTEM_PRODUCTION, FILESYSTEM_DRAINING
     AND DiskCopy.castorfile = TapeCopy.castorfile
     AND Stream2TapeCopy.child = TapeCopy.id
     AND Stream2TapeCopy.parent = streamId
     AND TapeCopy.status = 2 -- WAITINSTREAMS
     AND ROWNUM < 2; 
  res := 1;
EXCEPTION
 WHEN NO_DATA_FOUND THEN
  res := 0;
END;
/

/* PL/SQL method implementing bestTapeCopyForStream */
CREATE OR REPLACE PROCEDURE bestTapeCopyForStream(streamId IN INTEGER,
                                                  diskServerName OUT VARCHAR2, mountPoint OUT VARCHAR2,
                                                  path OUT VARCHAR2, dci OUT INTEGER,
                                                  castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                  nsHost OUT VARCHAR2, fileSize OUT INTEGER,
                                                  tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
  policy VARCHAR(2048);
BEGIN
  -- get the policy name
  BEGIN
    SELECT migrSelectPolicy INTO policy
      FROM Stream, TapePool
     WHERE Stream.id = streamId
       AND Stream.tapePool = TapePool.id;
    -- check for NULL value
    IF policy IS NULL THEN
      policy := 'defaultMigrSelPolicy';
    END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    policy := 'defaultMigrSelPolicy';
  END;
  EXECUTE IMMEDIATE 'BEGIN ' || policy || '(:streamId, :diskServerName, :mountPoint, :path, :dci, :castorFileId, :fileId, :nsHost, :fileSize, :tapeCopyId, :lastUpdateTime); END;'
    USING IN streamId, OUT diskServerName, OUT mountPoint, OUT path, OUT dci, OUT castorFileId, OUT fileId, OUT nsHost, OUT fileSize, OUT tapeCopyId, OUT lastUpdateTime;
END;
/

/* default migration candidate selection policy */
CREATE OR REPLACE PROCEDURE defaultMigrSelPolicy(streamId IN INTEGER,
                                                 diskServerName OUT VARCHAR2, mountPoint OUT VARCHAR2,
                                                 path OUT VARCHAR2, dci OUT INTEGER,
                                                 castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                 nsHost OUT VARCHAR2, fileSize OUT INTEGER,
                                                 tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
  fileSystemId INTEGER := 0;
  diskServerId NUMBER;
  lastFSChange NUMBER;
  lastFSUsed NUMBER;
  lastButOneFSUsed NUMBER;
  findNewFS NUMBER := 1;
  nbMigrators NUMBER := 0;
  unused NUMBER;
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
           AND ROWNUM < 2;
        SELECT CastorFile.FileId, CastorFile.NsHost, CastorFile.FileSize, CastorFile.lastUpdateTime
          INTO fileId, nsHost, fileSize, lastUpdateTime
          FROM CastorFile
         WHERE Id = castorFileId;
        -- we found one, no need to go for new filesystem
        findNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- found no tapecopy or diskserver, filesystem are down. We'll go through the normal selection
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
       DECLARE
         lock_detected EXCEPTION;
         PRAGMA EXCEPTION_INIT(lock_detected, -54);
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
            AND ROWNUM < 2;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR lock_detected THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;

  IF tapeCopyId = 0 THEN
    -- Nothing found, reset last filesystems used and exit
    UPDATE Stream
       SET lastFileSystemUsed = 0, lastButOneFileSystemUsed = 0
     WHERE id = streamId;
    RETURN;
  END IF;

  -- Here we found a tapeCopy and we process it
  -- update status of selected tapecopy and stream
  UPDATE TapeCopy SET status = 3 -- SELECTED
   WHERE id = tapeCopyId;
  IF findNewFS = 1 THEN
    UPDATE Stream
       SET status = 3, -- RUNNING
           lastFileSystemUsed = fileSystemId,
           lastButOneFileSystemUsed = lastFileSystemUsed,
           lastFileSystemChange = getTime()
     WHERE id = streamId AND status IN (2,3);
  ELSE
    -- only update status
    UPDATE Stream
       SET status = 3 -- RUNNING
     WHERE id = streamId AND status IN (2,3);
  END IF;
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy
   WHERE child = tapeCopyId;

  -- Update Filesystem state
  updateFSMigratorOpened(diskServerId, fileSystemId, 0);
END;
/

/* drain disk migration candidate selection policy */
CREATE OR REPLACE PROCEDURE drainDiskMigrSelPolicy(streamId IN INTEGER,
                                                   diskServerName OUT VARCHAR2, mountPoint OUT VARCHAR2,
                                                   path OUT VARCHAR2, dci OUT INTEGER,
                                                   castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                   nsHost OUT VARCHAR2, fileSize OUT INTEGER,
                                                   tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
  fileSystemId INTEGER := 0;
  diskServerId NUMBER;
  fsDiskServer NUMBER;
  lastFSChange NUMBER;
  lastFSUsed NUMBER;
  lastButOneFSUsed NUMBER;
  findNewFS NUMBER := 1;
  nbMigrators NUMBER := 0;
  unused NUMBER;
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
        SELECT P.path, P.diskcopy_id, P.castorfile,
             C.fileId, C.nsHost, C.fileSize, P.id, C.lastUpdateTime
        INTO path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
        FROM (SELECT /*+ ORDERED USE_NL(D T) INDEX(T I_TapeCopy_CF_Status_2) INDEX(ST I_Stream2TapeCopy_PC) */
              D.path, D.diskcopy_id, D.castorfile, T.id
                FROM (SELECT /*+ INDEX(DK I_DiskCopy_FS_Status_10) */
                             DK.path path, DK.id diskcopy_id, DK.castorfile
                        FROM DiskCopy DK
                       WHERE decode(DK.status, 10, DK.status, NULL) = 10 -- CANBEMIGR
                         AND DK.filesystem = lastFSUsed) D,
                      TapeCopy T, Stream2TapeCopy ST
               WHERE T.castorfile = D.castorfile
                 AND ST.child = T.id
                 AND ST.parent = streamId
                 AND decode(T.status, 2, T.status, NULL) = 2 -- WAITINSTREAMS
                 AND ROWNUM < 2) P, castorfile C
         WHERE P.castorfile = C.id;
        -- we found one, no need to go for new filesystem
        findNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND THEN
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
       DECLARE
         lock_detected EXCEPTION;
         PRAGMA EXCEPTION_INIT(lock_detected, -54);
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
            AND ROWNUM < 2;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR lock_detected THEN
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
       DECLARE
         lock_detected EXCEPTION;
         PRAGMA EXCEPTION_INIT(lock_detected, -54);
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
            AND ROWNUM < 2;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR lock_detected THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;

  IF tapeCopyId = 0 THEN
    -- Nothing found, reset last filesystems used and exit
    UPDATE Stream
       SET lastFileSystemUsed = 0, lastButOneFileSystemUsed = 0
     WHERE id = streamId;
    RETURN;
  END IF;

  -- Here we found a tapeCopy and we process it
  -- update status of selected tapecopy and stream
  UPDATE TapeCopy SET status = 3 -- SELECTED
   WHERE id = tapeCopyId;
  IF findNewFS = 1 THEN
    UPDATE Stream
       SET status = 3, -- RUNNING
           lastFileSystemUsed = fileSystemId,
           lastButOneFileSystemUsed = lastFileSystemUsed,
           lastFileSystemChange = getTime()
     WHERE id = streamId AND status IN (2,3);
  ELSE
    -- only update status
    UPDATE Stream
       SET status = 3 -- RUNNING
     WHERE id = streamId AND status IN (2,3);
  END IF;
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy
   WHERE child = tapeCopyId;

  -- Update Filesystem state
  updateFSMigratorOpened(fsDiskServer, fileSystemId, 0);

END;
/

/* repack migration candidate selection policy */
CREATE OR REPLACE PROCEDURE repackMigrSelPolicy(streamId IN INTEGER,
                                                diskServerName OUT VARCHAR2, mountPoint OUT VARCHAR2,
                                                path OUT VARCHAR2, dci OUT INTEGER,
                                                castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                nsHost OUT VARCHAR2, fileSize OUT INTEGER,
                                                tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
  fileSystemId INTEGER := 0;
  diskServerId NUMBER;
  lastFSChange NUMBER;
  lastFSUsed NUMBER;
  lastButOneFSUsed NUMBER;
  nbMigrators NUMBER := 0;
  unused NUMBER;
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
    DECLARE
      lock_detected EXCEPTION;
      PRAGMA EXCEPTION_INIT(lock_detected, -54);
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
         AND ROWNUM < 2;
      -- found something on this filesystem, no need to go on
      diskServerId := f.DiskServerId;
      fileSystemId := f.fileSystemId;
      EXIT;
    EXCEPTION WHEN NO_DATA_FOUND OR lock_detected THEN
      -- either the filesystem is already locked or we found nothing,
      -- let's go to the next one
      NULL;
    END;
  END LOOP;

  IF tapeCopyId = 0 THEN
    -- Nothing found, reset last filesystems used and exit
    UPDATE Stream
       SET lastFileSystemUsed = 0, lastButOneFileSystemUsed = 0
     WHERE id = streamId;
    RETURN;
  END IF;

  -- Here we found a tapeCopy and we process it
  -- update status of selected tapecopy and stream
  UPDATE TapeCopy SET status = 3 -- SELECTED
   WHERE id = tapeCopyId;
  UPDATE Stream
     SET status = 3, -- RUNNING
         lastFileSystemUsed = fileSystemId,
         lastButOneFileSystemUsed = lastFileSystemUsed,
         lastFileSystemChange = getTime()
   WHERE id = streamId AND status IN (2,3);
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy
   WHERE child = tapeCopyId;

  -- Update Filesystem state
  updateFSMigratorOpened(diskServerId, fileSystemId, 0);
END;
/

/* PL/SQL method implementing bestFileSystemForSegment */
CREATE OR REPLACE PROCEDURE bestFileSystemForSegment(segmentId IN INTEGER, diskServerName OUT VARCHAR2,
                                                     rmountPoint OUT VARCHAR2, rpath OUT VARCHAR2,
                                                     dcid OUT INTEGER) AS
  fileSystemId NUMBER;
  cfid NUMBER;
  fsDiskServer NUMBER;
  fileSize NUMBER;
  nb NUMBER;
BEGIN
  -- First get the DiskCopy and see whether it already has a fileSystem
  -- associated (case of a multi segment file)
  -- We also select on the DiskCopy status since we know it is
  -- in WAITTAPERECALL status and there may be other ones
  -- INVALID, GCCANDIDATE, DELETED, etc...
  SELECT DiskCopy.fileSystem, DiskCopy.path, DiskCopy.id, DiskCopy.CastorFile
    INTO fileSystemId, rpath, dcid, cfid
    FROM TapeCopy, Segment, DiskCopy
   WHERE Segment.id = segmentId
     AND Segment.copy = TapeCopy.id
     AND DiskCopy.castorfile = TapeCopy.castorfile
     AND DiskCopy.status = 2; -- WAITTAPERECALL
  -- Check if the DiskCopy had a FileSystem associated
  IF fileSystemId > 0 THEN
    BEGIN
      -- It had one, force filesystem selection, unless it was disabled.
      SELECT DiskServer.name, DiskServer.id, FileSystem.mountPoint
        INTO diskServerName, fsDiskServer, rmountPoint
        FROM DiskServer, FileSystem
       WHERE FileSystem.id = fileSystemId
         AND FileSystem.status = 0 -- FILESYSTEM_PRODUCTION
         AND DiskServer.id = FileSystem.diskServer
         AND DiskServer.status = 0; -- DISKSERVER_PRODUCTION
      updateFsRecallerOpened(fsDiskServer, fileSystemId, 0);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Error, the filesystem or the machine was probably disabled in between
      raise_application_error(-20101, 'In a multi-segment file, FileSystem or Machine was disabled before all segments were recalled');
    END;
  ELSE
    fileSystemId := 0;
    -- The DiskCopy had no FileSystem associated with it which indicates that
    -- This is a new recall. We try and select a good FileSystem for it!
    FOR a IN (SELECT DiskServer.name, FileSystem.mountPoint, FileSystem.id,
                     FileSystem.diskserver, CastorFile.fileSize
                FROM DiskServer, FileSystem, DiskPool2SvcClass,
                     (SELECT id, svcClass from StageGetRequest UNION ALL
                      SELECT id, svcClass from StagePrepareToGetRequest UNION ALL
                      SELECT id, svcClass from StageRepackRequest UNION ALL
                      SELECT id, svcClass from StageUpdateRequest UNION ALL
                      SELECT id, svcClass from StagePrepareToUpdateRequest) Request,
                      SubRequest, CastorFile
               WHERE CastorFile.id = cfid
                 AND SubRequest.castorfile = cfid
                 AND SubRequest.status = 4 -- SUBREQUEST_WAITTAPERECALL
                 AND Request.id = SubRequest.request
                 AND Request.svcclass = DiskPool2SvcClass.child
                 AND FileSystem.diskpool = DiskPool2SvcClass.parent
                 AND FileSystem.free - FileSystem.minAllowedFreeSpace * FileSystem.totalSize > CastorFile.fileSize
                 AND FileSystem.status = 0 -- FILESYSTEM_PRODUCTION
                 AND DiskServer.id = FileSystem.diskServer
                 AND DiskServer.status = 0 -- DISKSERVER_PRODUCTION
            ORDER BY -- first prefer DSs without concurrent migrators/recallers
                     DiskServer.nbRecallerStreams ASC, FileSystem.nbMigratorStreams ASC,
                     -- then order by rate as defined by the function
                     fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                                    FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams,
                                    FileSystem.nbRecallerStreams) DESC,
                     -- finally use randomness to avoid preferring always the same FS
                     DBMS_Random.value)
    LOOP
      diskServerName := a.name;
      rmountPoint    := a.mountPoint;
      fileSystemId   := a.id;
      -- Check that we don't already have a copy of this file on this filesystem.
      -- This will never happen in normal operations but may be the case if a filesystem
      -- was disabled and did come back while the tape recall was waiting.
      -- Even if we optimize by cancelling remaining unneeded tape recalls when a
      -- fileSystem comes back, the ones running at the time of the come back will have
      -- the problem.
      SELECT count(*) INTO nb
        FROM DiskCopy
       WHERE fileSystem = a.id
         AND castorfile = cfid
         AND status = 0; -- STAGED
      IF nb != 0 THEN
        raise_application_error(-20103, 'Recaller could not find a FileSystem in production in the requested SvcClass and without copies of this file');
      END IF;
      -- Set the diskcopy's filesystem
      UPDATE DiskCopy
         SET fileSystem = a.id
       WHERE id = dcid;
      updateFsRecallerOpened(a.diskServer, a.id, a.fileSize);
      RETURN;
    END LOOP;

    IF fileSystemId = 0 THEN
      raise_application_error(-20115, 'No suitable filesystem found for this recalled segment');
    END IF;
  END IF;
END;
/

/* PL/SQL method implementing fileRecallFailed */
CREATE OR REPLACE PROCEDURE fileRecallFailed(tapecopyId IN INTEGER) AS
 cfId NUMBER;
BEGIN
  SELECT castorFile INTO cfId FROM TapeCopy
   WHERE id = tapecopyId;
  UPDATE DiskCopy SET status = 4 -- DISKCOPY_FAILED
   WHERE castorFile = cfId
     AND status = 2; -- DISKCOPY_WAITTAPERECALL
  -- Drop tape copies. Ideally, we should keep some track that
  -- the recall failed in order to prevent future recalls until some
  -- sort of manual intervention. For the time being, as we can't
  -- say whether the failure is fatal or not, we drop everything
  -- and we won't deny a future request for recall.
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
END;
/


/* PL/SQL method implementing streamsToDo */
CREATE OR REPLACE PROCEDURE streamsToDo(res OUT castorTape.Stream_Cur) AS
  sId NUMBER;
  streams "numList";
BEGIN
   -- JUST rtcpclientd
  FOR s IN (SELECT id FROM Stream WHERE status = 0) LOOP
    BEGIN
      SELECT /*+ LEADING(Stream2TapeCopy TapeCopy DiskCopy FileSystem DiskServer) */
             s.id INTO sId
        FROM Stream2TapeCopy, TapeCopy, DiskCopy, FileSystem, DiskServer
       WHERE Stream2TapeCopy.parent = s.id
         AND Stream2TapeCopy.child = TapeCopy.id
         AND TapeCopy.castorFile = DiskCopy.CastorFile
         AND DiskCopy.fileSystem = FileSystem.id
         AND FileSystem.status IN (0, 1) -- PRODUCTION, DRAINING
         AND DiskServer.id = FileSystem.DiskServer
         AND DiskServer.status IN (0, 1) -- PRODUCTION, DRAINING
         AND ROWNUM < 2;
      INSERT INTO StreamsToDoHelper VALUES (sId);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- just ignore as this stream has no available candidate
      NULL;
    END;
  END LOOP;
  SELECT id BULK COLLECT INTO Streams FROM StreamsToDoHelper;
  FORALL i in streams.FIRST..streams.LAST
    UPDATE Stream SET status = 1 -- WAITDRIVE
     WHERE id = streams(i);
  OPEN res FOR
    SELECT Stream.id, Stream.InitialSizeToTransfer, Stream.status,
           TapePool.id, TapePool.name
      FROM Stream, TapePool
     WHERE Stream.id MEMBER OF streams
       AND Stream.TapePool = TapePool.id;
END;
/


/* PL/SQL method implementing fileRecalled */
CREATE OR REPLACE PROCEDURE fileRecalled(tapecopyId IN INTEGER) AS
  subRequestId NUMBER;
  requestId NUMBER;
  dci NUMBER;
  reqType NUMBER;
  cfId NUMBER;
  fs NUMBER;
  gcw NUMBER;
  gcwProc VARCHAR(2048);
  ouid INTEGER;
  ogid INTEGER;
  svcClassId NUMBER;
  missingTCs INTEGER;
BEGIN
  SELECT SubRequest.id, SubRequest.request, DiskCopy.id,
         CastorFile.id, Castorfile.FileSize, TapeCopy.missingCopies
    INTO subRequestId, requestId, dci, cfId, fs, missingTCs
    FROM TapeCopy, SubRequest, DiskCopy, CastorFile
   WHERE TapeCopy.id = tapecopyId
     AND CastorFile.id = TapeCopy.castorFile
     AND DiskCopy.castorFile = TapeCopy.castorFile
     AND SubRequest.diskcopy(+) = DiskCopy.id
     AND DiskCopy.status = 2;  -- DISKCOPY_WAITTAPERECALL
  -- delete any previous failed diskcopy for this castorfile (due to failed recall attempts for instance)
  DELETE FROM Id2Type WHERE id IN (SELECT id FROM DiskCopy WHERE castorFile = cfId AND status = 4);
  DELETE FROM DiskCopy WHERE castorFile = cfId AND status = 4;  -- FAILED
  -- update diskcopy size and gweight
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
     SET status = 0,  -- DISKCOPY_STAGED
         lastAccessTime = getTime(),  -- for the GC, effective lifetime of this diskcopy starts now
         gcWeight = gcw,
         diskCopySize = fs
   WHERE id = dci;
  -- determine the type of the request
  SELECT type INTO reqType FROM Id2Type WHERE id =
    (SELECT request FROM SubRequest WHERE id = subRequestId);
  IF reqType = 119 THEN  -- OBJ_StageRepackRequest
    startRepackMigration(subRequestId, cfId, dci, ouid, ogid);
  ELSE
    -- restart this subrequest if it's not a repack one
    UPDATE SubRequest
       SET status = 1, getNextStatus = 1,  -- SUBREQUEST_RESTART, GETNEXTSTATUS_FILESTAGED
           lastModificationTime = getTime(), parent = 0
     WHERE id = subRequestId;
    -- And trigger new migrations if missing tape copies were detected
    IF missingTCs > 0 THEN
      DECLARE
        tcId INTEGER;
      BEGIN
        UPDATE DiskCopy
           SET status = 10  -- DISKCOPY_CANBEMIGR
         WHERE id = dci;
        FOR i IN 1..missingTCs LOOP
          INSERT INTO TapeCopy (id, copyNb, castorFile, status)
          VALUES (ids_seq.nextval, 0, cfId, 0) -- TAPECOPY_CREATED
          RETURNING id INTO tcId;
          INSERT INTO Id2Type (id, type) VALUES (tcId, 30); -- OBJ_TapeCopy
        END LOOP;
      END;
    END IF;
  END IF;
  -- restart other requests waiting on this recall
  UPDATE /*+ INDEX(ST I_SUBREQUEST_PARENT) */ SubRequest ST
       SET status = 1, getNextStatus = 1,  -- SUBREQUEST_RESTART, GETNEXTSTATUS_FILESTAGED
           lastModificationTime = getTime(), parent = 0
   WHERE parent = subRequestId;
  -- Trigger the creation of additional copies of the file, if necessary.
  replicateOnClose(cfId, ouid, ogid);
END;
/


/* PL/SQL method implementing resetStream */

CREATE OR REPLACE PROCEDURE resetStream (sid IN INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -02292);
BEGIN  
   DELETE FROM Stream WHERE id = sid;
   DELETE FROM Id2Type WHERE id = sid;
   UPDATE Tape SET status = 0, stream = null WHERE stream = sid;
EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
  -- constraint violation and we cannot delete the stream
  UPDATE Stream SET status = 6, tape = null, lastFileSystemChange = null WHERE id = sid; 
  UPDATE Tape SET status = 0, stream = null WHERE stream = sid;
END;
/


/* PL/SQL method implementing segmentsForTape */
CREATE OR REPLACE PROCEDURE segmentsForTape (tapeId IN INTEGER, segments
OUT castor.Segment_Cur) AS
  segs "numList";
  rows PLS_INTEGER := 500;
  CURSOR c1 IS
    SELECT Segment.id FROM Segment
    WHERE Segment.tape = tapeId AND Segment.status = 0 ORDER BY Segment.fseq
    FOR UPDATE;
BEGIN
   -- JUST rtcpclientd
  OPEN c1;
  FETCH c1 BULK COLLECT INTO segs LIMIT rows;
  CLOSE c1;

  IF segs.COUNT > 0 THEN
     UPDATE Tape SET status = 4 -- MOUNTED
       WHERE id = tapeId;
     FORALL j IN segs.FIRST..segs.LAST -- bulk update with the forall..
       UPDATE Segment SET status = 7 -- SELECTED
       WHERE id = segs(j);
  END IF;

  OPEN segments FOR
    SELECT fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm, segmCksum,
           errMsgTxt, errorCode, severity, blockId0, blockId1, blockId2, blockId3,
           creationTime, id, tape, copy, status, priority
      FROM Segment
     WHERE id IN (SELECT /*+ CARDINALITY(segsTable 5) */ * FROM TABLE(segs) segsTable);
END;
/


/* PL/SQL method implementing anySegmentsForTape */
CREATE OR REPLACE PROCEDURE anySegmentsForTape
(tapeId IN INTEGER, nb OUT INTEGER) AS
BEGIN
   -- JUST rtcpclientd
  SELECT count(*) INTO nb FROM Segment
  WHERE Segment.tape = tapeId
    AND Segment.status = 0;
  IF nb > 0 THEN
    UPDATE Tape SET status = 3 -- WAITMOUNT
    WHERE id = tapeId;
  END IF;
END;
/


/* PL/SQL method implementing failedSegments */
CREATE OR REPLACE PROCEDURE failedSegments
(segments OUT castor.Segment_Cur) AS
BEGIN
   -- JUST rtcpclientd
  OPEN segments FOR
    SELECT fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm, segmCksum,
           errMsgTxt, errorCode, severity, blockId0, blockId1, blockId2, blockId3,
           creationTime, id, tape, copy, status, priority
      FROM Segment
     WHERE Segment.status = 6; -- SEGMENT_FAILED
END;
/


/* PL/SQL procedure which is executed whenever a files has been written to tape by the migrator to
 * check, whether the file information has to be added to the NameServer or to replace an entry
 * (repack case)
 */
CREATE OR REPLACE PROCEDURE checkFileForRepack(fid IN INTEGER, ret OUT VARCHAR2) AS
  sreqid NUMBER;
BEGIN
   -- JUST rtcpclientd
  ret := NULL;
  -- Get the repackvid field from the existing request (if none, then we are not in a repack process)
  SELECT SubRequest.id, StageRepackRequest.repackvid
    INTO sreqid, ret
    FROM SubRequest, DiskCopy, CastorFile, StageRepackRequest
   WHERE stagerepackrequest.id = subrequest.request
     AND diskcopy.id = subrequest.diskcopy
     AND diskcopy.status = 10 -- CANBEMIGR
     AND subrequest.status = 12 -- SUBREQUEST_REPACK
     AND diskcopy.castorfile = castorfile.id
     AND castorfile.fileid = fid
     AND ROWNUM < 2;
  archiveSubReq(sreqid, 8); -- XXX this step is to be moved after and if the operation has been
                            -- XXX successful, once the migrator is properly rewritten
EXCEPTION WHEN NO_DATA_FOUND THEN
  NULL;
END;
/

/* PL/SQL method implementing rtcpclientdCleanUp */
CREATE OR REPLACE PROCEDURE rtcpclientdCleanUp AS
  tpIds "numList";
BEGIN
  -- JUST rtcpclientd
  -- Deal with Migrations
  -- 1) Ressurect tapecopies for migration
  UPDATE TapeCopy SET status = 1 WHERE status = 3;
  -- 2) Clean up the streams
  UPDATE Stream SET status = 0 
   WHERE status NOT IN (0, 5, 6, 7) --PENDING, CREATED, STOPPED, WAITPOLICY
  RETURNING tape BULK COLLECT INTO tpIds;
  UPDATE Stream SET tape = NULL WHERE tape != 0;
  -- 3) Reset the tape for migration
  FORALL i IN tpIds.FIRST .. tpIds.LAST  
    UPDATE tape SET stream = 0, status = 0 WHERE status IN (2, 3, 4) AND id = tpIds(i);

  -- Deal with Recalls
  UPDATE Segment SET status = 0 WHERE status = 7; -- Resurrect SELECTED segment
  UPDATE Tape SET status = 1 WHERE tpmode = 0 AND status IN (2, 3, 4); -- Resurrect the tapes running for recall
  UPDATE Tape A SET status = 8 
   WHERE status IN (0, 6, 7) AND EXISTS
    (SELECT id FROM Segment WHERE status = 0 AND tape = A.id);
  COMMIT;
END;
/


/** Functions for the MigHunterDaemon **/

/** Cleanup before starting a new MigHunterDaemon **/
CREATE OR REPLACE PROCEDURE migHunterCleanUp(svcName IN VARCHAR2)
AS
  svcId NUMBER;
BEGIN
  SELECT id INTO svcId FROM SvcClass WHERE name = svcName;
  -- clean up tapecopies, WAITPOLICY reset into TOBEMIGRATED
  UPDATE
     /*+ LEADING(TC CF)
         INDEX_RS_ASC(CF PK_CASTORFILE_ID)
         INDEX_RS_ASC(TC I_TAPECOPY_STATUS) */ 
         TapeCopy TC
     SET status = 1
   WHERE status = 7 
     AND EXISTS (
       SELECT 'x' 
         FROM CastorFile CF
        WHERE TC.castorFile = CF.id
          AND CF.svcclass = svcId);
  -- clean up streams, WAITPOLICY reset into CREATED
  UPDATE Stream SET status = 5 WHERE status = 7 AND tapepool IN
   (SELECT svcclass2tapepool.child
      FROM svcclass2tapepool
     WHERE svcId = svcclass2tapepool.parent);
  COMMIT;
END;
/

/* Get input for python migration policy */
CREATE OR REPLACE PROCEDURE inputForMigrationPolicy
(svcclassName IN VARCHAR2,
 policyName OUT NOCOPY VARCHAR2,
 svcId OUT NUMBER,
 dbInfo OUT castorTape.DbMigrationInfo_Cur) AS
 tcIds "numList";
BEGIN
  -- do the same operation of getMigrCandidate and return the dbInfoMigrationPolicy
  -- we look first for repack condidates for this svcclass
  -- we update atomically WAITPOLICY
  SELECT SvcClass.migratorpolicy, SvcClass.id INTO policyName, svcId
    FROM SvcClass
   WHERE SvcClass.name = svcClassName;

  UPDATE
     /*+ LEADING(TC CF)
         INDEX_RS_ASC(CF PK_CASTORFILE_ID)
         INDEX_RS_ASC(TC I_TAPECOPY_STATUS) */ 
         TapeCopy TC 
     SET status = 7
   WHERE status IN (0, 1)
     AND (EXISTS
       (SELECT 'x' FROM SubRequest, StageRepackRequest
         WHERE StageRepackRequest.svcclass = svcId
           AND SubRequest.request = StageRepackRequest.id
           AND SubRequest.status = 12  -- SUBREQUEST_REPACK
           AND TC.castorfile = SubRequest.castorfile
      ) OR EXISTS (
        SELECT 'x'
          FROM CastorFile CF
         WHERE TC.castorFile = CF.id
           AND CF.svcClass = svcId)) AND rownum < 10000
    RETURNING TC.id -- CREATED / TOBEMIGRATED
    BULK COLLECT INTO tcIds;
  COMMIT;
  -- return the full resultset
  OPEN dbInfo FOR
    SELECT TapeCopy.id, TapeCopy.copyNb, CastorFile.lastknownfilename,
           CastorFile.nsHost, CastorFile.fileid, CastorFile.filesize
      FROM Tapecopy,CastorFile
     WHERE CastorFile.id = TapeCopy.castorfile
       AND TapeCopy.id IN 
         (SELECT /*+ CARDINALITY(tcidTable 5) */ * 
            FROM table(tcIds) tcidTable);
END;
/

/* Get input for python Stream Policy */
CREATE OR REPLACE PROCEDURE inputForStreamPolicy
(svcClassName IN VARCHAR2,
 policyName OUT NOCOPY VARCHAR2,
 runningStreams OUT INTEGER,
 maxStream OUT INTEGER,
 dbInfo OUT castorTape.DbStreamInfo_Cur)
AS
  tpId NUMBER; -- used in the loop
  tcId NUMBER; -- used in the loop
  streamId NUMBER; -- stream attached to the tapepool
  svcId NUMBER; -- id for the svcclass
  strIds "numList";
  tcNum NUMBER;
BEGIN
  -- info for policy
  SELECT streamPolicy, nbDrives, id INTO policyName, maxStream, svcId
    FROM SvcClass WHERE SvcClass.name = svcClassName;
  SELECT count(*) INTO runningStreams
    FROM Stream, SvcClass2TapePool
   WHERE Stream.TapePool = SvcClass2TapePool.child
     AND SvcClass2TapePool.parent = svcId
     AND Stream.status = 3;
  UPDATE stream SET status = 7
   WHERE Stream.status IN (4, 5, 6)
     AND Stream.id
      IN (SELECT Stream.id FROM Stream,SvcClass2TapePool
           WHERE Stream.Tapepool = SvcClass2TapePool.child
             AND SvcClass2TapePool.parent = svcId)
  RETURNING Stream.id BULK COLLECT INTO strIds;
  COMMIT;
  
  -- check for overloaded streams
  SELECT count(*) INTO tcNum FROM stream2tapecopy 
   WHERE parent IN (SELECT /*+ CARDINALITY(stridTable 5) */ * FROM TABLE(strIds) stridTable);
  IF (tcnum > 10000 * maxstream) AND (maxstream > 0) THEN
    -- emergency mode
    OPEN dbInfo FOR
      SELECT Stream.id, 10000, 10000, gettime
        FROM Stream
       WHERE Stream.id IN (SELECT /*+ CARDINALITY(stridTable 5) */ * FROM TABLE(strIds) stridTable)
         AND Stream.status = 7
       GROUP BY Stream.id;
  ELSE
  -- return for policy
  OPEN dbInfo FOR
    SELECT /*+ INDEX(CastorFile PK_CastorFile_Id) */ Stream.id,
           count(distinct Stream2TapeCopy.child),
           sum(CastorFile.filesize), gettime() - min(CastorFile.creationtime)
      FROM Stream2TapeCopy, TapeCopy, CastorFile, Stream
     WHERE Stream.id IN
        (SELECT /*+ CARDINALITY(stridTable 5) */ * FROM TABLE(strIds) stridTable)
       AND Stream2TapeCopy.child = TapeCopy.id
       AND TapeCopy.castorfile = CastorFile.id
       AND Stream.id = Stream2TapeCopy.parent
       AND Stream.status = 7
     GROUP BY Stream.id
   UNION ALL
    SELECT Stream.id, 0, 0, 0
      FROM Stream WHERE Stream.id IN
        (SELECT /*+ CARDINALITY(stridTable 5) */ * FROM TABLE(strIds) stridTable)
       AND Stream.status = 7
       AND NOT EXISTS 
        (SELECT 'x' FROM Stream2TapeCopy ST WHERE ST.parent = Stream.ID);
 END IF;         
END;
/

/* createOrUpdateStream */
CREATE OR REPLACE PROCEDURE createOrUpdateStream
(svcClassName IN VARCHAR2,
 initialSizeToTransfer IN NUMBER, -- total initialSizeToTransfer for the svcClass
 volumeThreashold IN NUMBER, -- parameter given by -V
 initialSizeCeiling IN NUMBER, -- to calculate the initialSizeToTransfer per stream
 doClone IN INTEGER,
 nbMigrationCandidate IN INTEGER,
 retCode OUT INTEGER) -- all candidate before applying the policy
AS
  nbOldStream NUMBER; -- stream for the specific svcclass
  nbDrives NUMBER; -- drives associated to the svcclass
  initSize NUMBER; --  the initialSizeToTransfer per stream
  tpId NUMBER; -- tape pool id
  strId NUMBER; -- stream id
  streamToClone NUMBER; -- stream id to clone
  svcId NUMBER; --svcclass id
  tcId NUMBER; -- tape copy id
  oldSize NUMBER; -- value for a cloned stream
BEGIN
  retCode := 0;
  -- get streamFromSvcClass
  BEGIN
    SELECT id INTO svcId FROM SvcClass
     WHERE name = svcClassName AND ROWNUM < 2;
    SELECT count(Stream.id) INTO nbOldStream
      FROM Stream, SvcClass2TapePool
     WHERE SvcClass2TapePool.child = Stream.tapepool
       AND SvcClass2TapePool.parent = svcId;
  EXCEPTION
    WHEN NO_DATA_FOUND THEN
    -- RTCPCLD_MSG_NOTPPOOLS
    -- restore candidate
    retCode := -1;
    RETURN;
  END;

  IF nbOldStream <= 0 AND initialSizeToTransfer < volumeThreashold THEN
    -- restore WAITINSTREAM to TOBEMIGRATED, not enough data
    retCode := -2 ; -- RTCPCLD_MSG_DATALIMIT
    RETURN;
  END IF;

  IF nbOldStream >= 0 AND (doClone = 1 OR nbMigrationCandidate > 0) THEN
    -- stream creator
    SELECT SvcClass.nbDrives INTO nbDrives FROM SvcClass WHERE id = svcId;
    IF nbDrives = 0 THEN
    	retCode := -3; -- RESTORE NEEDED
    	RETURN;
    END IF;
    -- get the initialSizeToTransfer to associate to the stream
    IF initialSizeToTransfer/nbDrives > initialSizeCeiling THEN
      initSize := initialSizeCeiling;
    ELSE
      initSize := initialSizeToTransfer/nbDrives;
    END IF;

    -- loop until the max number of stream
    IF nbOldStream < nbDrives THEN
      LOOP
        -- get the tape pool with less stream
        BEGIN
         -- tapepool without stream randomly chosen
          SELECT a INTO tpId
            FROM (
              SELECT TapePool.id AS a FROM TapePool,SvcClass2TapePool
               WHERE TapePool.id NOT IN (SELECT TapePool FROM Stream)
                 AND TapePool.id = SvcClass2TapePool.child
	         AND SvcClass2TapePool.parent = svcId
            ORDER BY dbms_random.value
	    ) WHERE ROWNUM < 2;
        EXCEPTION WHEN NO_DATA_FOUND THEN
          -- at least one stream foreach tapepool
           SELECT tapepool INTO tpId
             FROM (
               SELECT tapepool, count(*) AS c
                 FROM Stream
                WHERE tapepool IN (
                  SELECT SvcClass2TapePool.child
                    FROM SvcClass2TapePool
                   WHERE SvcClass2TapePool.parent = svcId)
             GROUP BY tapepool
             ORDER BY c ASC, dbms_random.value)
           WHERE ROWNUM < 2;
	END;

        -- STREAM_CREATED
        INSERT INTO Stream
          (id, initialsizetotransfer, lastFileSystemChange, tape, lastFileSystemUsed,
           lastButOneFileSystemUsed, tapepool, status)
        VALUES (ids_seq.nextval, initSize, null, null, null, null, tpId, 5) RETURN id INTO strId;
        INSERT INTO Id2Type (id, type) values (strId,26); -- Stream type
    	IF doClone = 1 THEN
	  BEGIN
	    -- clone the new stream with one from the same tapepool
	    SELECT id, initialsizetotransfer INTO streamToClone, oldSize
              FROM Stream WHERE tapepool = tpId AND id != strId AND ROWNUM < 2;
            FOR tcId IN (SELECT child FROM Stream2TapeCopy
                          WHERE Stream2TapeCopy.parent = streamToClone)
            LOOP
              -- a take the first one, they are supposed to be all the same
              INSERT INTO stream2tapecopy (parent, child) VALUES (strId, tcId.child);
            END LOOP;
            UPDATE Stream SET initialSizeToTransfer = oldSize WHERE id = strId;
           EXCEPTION WHEN NO_DATA_FOUND THEN
  	    -- no stream to clone for this tapepool
  	    NULL;
	   END;
	END IF;
        nbOldStream := nbOldStream + 1;
        EXIT WHEN nbOldStream >= nbDrives;
      END LOOP;
    END IF;
  END IF;
END;
/

/* attach tapecopies to stream */
CREATE OR REPLACE PROCEDURE attachTapeCopiesToStreams
(tapeCopyIds IN castor."cnumList",
 tapePoolIds IN castor."cnumList")
AS
  streamId NUMBER; -- stream attached to the tapepool
  counter NUMBER := 0;
  unused NUMBER;
  nbStream NUMBER;
BEGIN
  -- add choosen tapecopies to all Streams associated to the tapepool used by the policy
  FOR i IN tapeCopyIds.FIRST .. tapeCopyIds.LAST LOOP
    BEGIN
      SELECT count(id) INTO nbStream FROM Stream
       WHERE Stream.tapepool = tapePoolIds(i);
      IF nbStream <> 0 THEN
        -- we have at least a stream for that tapepool
        SELECT id INTO unused
          FROM TapeCopy
         WHERE Status in (2,7) AND id = tapeCopyIds(i) FOR UPDATE;
        -- let's attach it to the different streams
        FOR streamId IN (SELECT id FROM Stream WHERE Stream.tapepool = tapePoolIds(i)) LOOP
          UPDATE TapeCopy SET Status = 2 WHERE Status = 7 AND id = tapeCopyIds(i);
          DECLARE CONSTRAINT_VIOLATED EXCEPTION;
          PRAGMA EXCEPTION_INIT (CONSTRAINT_VIOLATED, -1);
          BEGIN
            INSERT INTO stream2tapecopy (parent ,child) VALUES (streamId.id, tapeCopyIds(i));
          EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
            -- if the stream does not exist anymore
            UPDATE tapecopy SET status = 7 WHERE id = tapeCopyIds(i);
            -- it might also be that the tapecopy does not exist anymore
          END;
        END LOOP; -- stream loop
      END IF;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Go on the tapecopy has been resurrected or migrated
      NULL;
    END;
    counter := counter + 1;
    IF counter = 100 THEN
      counter := 0;
      COMMIT;
    END IF;
  END LOOP; -- loop tapecopies

  -- resurrect the one never attached
  FORALL i IN tapeCopyIds.FIRST .. tapeCopyIds.LAST
    UPDATE TapeCopy SET Status = 1 WHERE id = tapeCopyIds(i) AND Status = 7;
  COMMIT;
END;
/

/* start choosen stream */
CREATE OR REPLACE PROCEDURE startChosenStreams
        (streamIds IN castor."cnumList", initSize IN NUMBER) AS
BEGIN
  FORALL i IN streamIds.FIRST .. streamIds.LAST
    UPDATE Stream
       SET status = 0, -- PENDING
           -- initialSize overwritten to initSize only if it is 0
           initialSizeToTransfer = decode(initialSizeToTransfer, 0, initSize, initialSizeToTransfer)
     WHERE Stream.status = 7 -- WAITPOLICY
       AND id = streamIds(i);
  COMMIT;
END;
/

/* stop chosen stream */
CREATE OR REPLACE PROCEDURE stopChosenStreams
        (streamIds IN castor."cnumList") AS
  nbTc NUMBER;
BEGIN
  FOR i IN streamIds.FIRST .. streamIds.LAST LOOP
    SELECT count(*) INTO nbTc FROM stream2tapecopy WHERE parent = streamIds(i);
    IF nbTc = 0 THEN
      DELETE FROM Stream WHERE id = streamIds(i);
    ELSE
      UPDATE Stream
         SET status = 6 -- STOPPED
       WHERE Stream.status = 7 -- WAITPOLICY
         AND id = streamIds(i);
    END IF;
    COMMIT;
  END LOOP;
END;
/

/* resurrect Candidates */
CREATE OR REPLACE PROCEDURE resurrectCandidates
(migrationCandidates IN castor."cnumList") -- all candidate before applying the policy
AS
  unused "numList";
BEGIN
  FORALL i IN migrationCandidates.FIRST .. migrationCandidates.LAST
    UPDATE TapeCopy SET Status = 1 WHERE Status = 7 AND id = migrationCandidates(i);
  COMMIT;
END;
/

/* invalidate tape copies */
CREATE OR REPLACE PROCEDURE invalidateTapeCopies
(tapecopyIds IN castor."cnumList") -- tapecopies not in the nameserver
AS
  srId NUMBER;
BEGIN
  -- tapecopies
  FORALL i IN tapecopyIds.FIRST .. tapecopyIds.LAST
    UPDATE TapeCopy SET status = 6 WHERE id = tapecopyIds(i) AND status = 7;

  -- repack subrequests to be archived
  FOR i IN tapecopyIds.FIRST .. tapecopyIds.LAST LOOP
    BEGIN
      SELECT subrequest.id INTO srId FROM subrequest, tapecopy 
       WHERE subrequest.castorfile = tapecopy.castorfile
         AND tapecopy.id = tapecopyIds(i)
         AND subrequest.status = 12;
      archivesubreq(srId,9);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- no repack pending
      NULL;
    END;
  END LOOP;
  COMMIT;
END;
/


/** Functions for the RecHandlerDaemon **/

/* Get input for python recall policy */
CREATE OR REPLACE PROCEDURE inputForRecallPolicy(dbInfo OUT castorTape.DbRecallInfo_Cur) AS
  svcId NUMBER;
BEGIN
  OPEN dbInfo FOR
    SELECT
       /*+ NO_USE_MERGE(TAPE SEGMENT TAPECOPY CASTORFILE)
           NO_USE_HASH(TAPE SEGMENT TAPECOPY CASTORFILE)
           INDEX_RS_ASC(SEGMENT I_SEGMENT_TAPE)
           INDEX_RS_ASC(TAPE I_TAPE_STATUS)
           INDEX_RS_ASC(TAPECOPY PK_TAPECOPY_ID)
           INDEX_RS_ASC(CASTORFILE PK_CASTORFILE_ID) */
       Tape.id, Tape.vid, count(distinct segment.id), sum(CastorFile.fileSize),
       getTime() - min(Segment.creationTime), max(Segment.priority)
      FROM TapeCopy, CastorFile, Segment, Tape
     WHERE Tape.id = Segment.tape
       AND TapeCopy.id = Segment.copy
       AND CastorFile.id = TapeCopy.castorfile
       AND Tape.status IN (1, 2, 8)  -- PENDING, WAITDRIVE, WAITPOLICY
       AND Segment.status = 0  -- SEGMENT_UNPROCESSED
     GROUP BY Tape.id, Tape.vid;
END;
/

/* resurrect tapes */
CREATE OR REPLACE PROCEDURE resurrectTapes
(tapeIds IN castor."cnumList")
AS
BEGIN
  FORALL i IN tapeIds.FIRST .. tapeIds.LAST
    UPDATE Tape SET status = 1 WHERE status = 8 AND id = tapeIds(i);
  COMMIT;
END;
/


/* clean the db for repack, it is used as workaround because of repack abort limitation */
CREATE OR REPLACE PROCEDURE removeAllForRepack (inputVid IN VARCHAR2) AS
  reqId NUMBER;
  srId NUMBER;
  cfIds "numList";
  dcIds "numList";
  tcIds "numList";
  segIds "numList";
  tapeIds "numList";
BEGIN
  -- look for the request. If not found, raise NO_DATA_FOUND error;
  -- note that if the request is over (all in 9,11) or not started (0), nothing is done
  SELECT id INTO reqId 
    FROM StageRepackRequest R 
   WHERE repackVid = inputVid
     AND EXISTS 
       (SELECT 1 FROM SubRequest 
         WHERE request = R.id AND status IN (4, 12));  -- WAITTAPERECALL, REPACK
  -- fail subrequests
  UPDATE Subrequest SET status = 9
   WHERE request = reqId AND status NOT IN (9, 11)
  RETURNING castorFile, diskcopy BULK COLLECT INTO cfIds, dcIds;
  SELECT id INTO srId 
    FROM SubRequest 
   WHERE request = reqId AND ROWNUM = 1;
  archiveSubReq(srId, 9);

  -- fail related diskcopies
  FORALL i IN dcIds.FIRST .. dcids.LAST
    UPDATE DiskCopy
       SET status = decode(status, 2, 4, 7) -- WAITTAPERECALL->FAILED, otherwise INVALID
     WHERE id = dcIds(i);

  -- delete tapecopy from id2type and get the ids
  DELETE FROM id2type WHERE id IN 
   (SELECT id FROM TAPECOPY
     WHERE castorfile IN (SELECT /*+ CARDINALITY(cfIdsTable 5) */ *
                            FROM TABLE(cfIds) cfIdsTable))
  RETURNING id BULK COLLECT INTO tcIds;

  -- detach tapecopies from stream
  FORALL i IN tcids.FIRST .. tcids.LAST
    DELETE FROM stream2tapecopy WHERE child=tcIds(i);

  -- delete tapecopies
  FORALL i IN tcids.FIRST .. tcids.LAST
    DELETE FROM tapecopy WHERE id = tcIds(i);

  -- delete segments using the tapecopy link
  DELETE FROM segment WHERE copy IN
   (SELECT /*+ CARDINALITY(tcIdsTable 5) */ *
      FROM TABLE(tcids) tcIdsTable)
  RETURNING id, tape BULK COLLECT INTO segIds, tapeIds;

  FORALL i IN segIds.FIRST .. segIds.LAST
    DELETE FROM id2type WHERE id = segIds(i);

  -- delete the orphan segments (this should not be necessary)
  DELETE FROM segment WHERE tape IN 
    (SELECT id FROM tape WHERE vid = inputVid) 
  RETURNING id BULK COLLECT INTO segIds;
  FORALL i IN segIds.FIRST .. segIds.LAST
    DELETE FROM id2type WHERE id = segIds(i);

  -- update the tape as not used
  UPDATE tape SET status = 0 WHERE vid = inputVid AND tpmode = 0;
  -- update other tapes which could have been involved
  FORALL i IN tapeIds.FIRST .. tapeIds.LAST
    UPDATE tape SET status = 0 WHERE id = tapeIds(i);
  -- commit the transation
  COMMIT;
END;
/

/*******************************************************************
 *
 * @(#)RCSfile: oracleGC.sql,v  Revision: 1.698  Date: 2009/08/17 15:08:33  Author: sponcec3 
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
  TYPE JobLogEntry IS RECORD (
    fileid NUMBER,
    nshost VARCHAR2(2048),
    operation INTEGER);
  TYPE JobLogEntry_Cur IS REF CURSOR RETURN JobLogEntry;
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
  FUNCTION sizeRelatedUserWeight(fileSize NUMBER, DiskCopyStatus NUMBER) RETURN NUMBER;
  FUNCTION sizeRelatedRecallWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION sizeRelatedCopyWeight(fileSize NUMBER, DiskCopyStatus NUMBER, sourceWeight NUMBER) RETURN NUMBER;
  FUNCTION dayBonusFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER;
  FUNCTION halfHourBonusAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER;
  FUNCTION cappedUserSetGCWeight(oldGcWeight NUMBER, userDelta NUMBER) RETURN NUMBER;
  -- FIFO gc policy
  FUNCTION creationTimeUserWeight(fileSize NUMBER, DiskCopyStatus NUMBER) RETURN NUMBER;
  FUNCTION creationTimeRecallWeight(fileSize NUMBER) RETURN NUMBER;
  FUNCTION creationTimeCopyWeight(fileSize NUMBER, DiskCopyStatus NUMBER, sourceWeight NUMBER) RETURN NUMBER;
  -- LRU gc policy
  FUNCTION LRUFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER;
  FUNCTION LRUAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER;
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

  FUNCTION sizeRelatedUserWeight(fileSize NUMBER, DiskCopyStatus NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN size2GCWeight(fileSize);
  END;

  FUNCTION sizeRelatedRecallWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN size2GCWeight(fileSize);
  END;

  FUNCTION sizeRelatedCopyWeight(fileSize NUMBER, DiskCopyStatus NUMBER, sourceWeight NUMBER) RETURN NUMBER AS
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
  FUNCTION creationTimeUserWeight(fileSize NUMBER, DiskCopyStatus NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION creationTimeRecallWeight(fileSize NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION creationTimeCopyWeight(fileSize NUMBER, DiskCopyStatus NUMBER, sourceWeight NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  -- LRU gc policy
  FUNCTION LRUFirstAccessHook(oldGcWeight NUMBER, creationTime NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

  FUNCTION LRUAccessHook(oldGcWeight NUMBER, creationTime NUMBER, nbAccesses NUMBER) RETURN NUMBER AS
  BEGIN
    RETURN getTime();
  END;

END castorGC;
/

/* PL/SQL method implementing selectFiles2Delete
   This is the standard garbage collector: it sorts STAGED
   diskcopies by gcWeight and selects them for deletion up to
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
BEGIN
  -- First of all, check if we are in a Disk1 pool
  dontGC := 0;
  FOR sc IN (SELECT disk1Behavior
               FROM SvcClass, DiskPool2SvcClass D2S, DiskServer, FileSystem
              WHERE SvcClass.id = D2S.child
                AND D2S.parent = FileSystem.diskPool
                AND FileSystem.diskServer = DiskServer.id
                AND DiskServer.name = diskServerName) LOOP
    -- If any of the service classes to which we belong (normally a single one)
    -- say this is Disk1, we don't GC STAGED files.
    IF sc.disk1Behavior = 1 THEN
      dontGC := 1;
      EXIT;
    END IF;
  END LOOP;

  -- Loop on all concerned fileSystems in a random order.
  totalCount := 0;
  FOR fs IN (SELECT FileSystem.id
               FROM FileSystem, DiskServer
              WHERE FileSystem.diskServer = DiskServer.id
                AND DiskServer.name = diskServerName
             ORDER BY dbms_random.value) LOOP

    -- Count the number of diskcopies on this filesystem that are in a
    -- BEINGDELETED state. These need to be reselected in any case.
    freed := 0;
    SELECT totalCount + count(*), nvl(sum(DiskCopy.diskCopySize), 0)
      INTO totalCount, freed
      FROM DiskCopy
     WHERE DiskCopy.fileSystem = fs.id
       AND decode(DiskCopy.status, 9, DiskCopy.status, NULL) = 9; -- BEINGDELETED

    -- Process diskcopies that are in an INVALID state.
    UPDATE DiskCopy
       SET status = 9, -- BEINGDELETED
           gcType = decode(gcType, NULL, 1, gcType) -- USER
     WHERE fileSystem = fs.id
       AND status = 7  -- INVALID
       AND NOT EXISTS
         -- Ignore diskcopies with active subrequests
         (SELECT /*+ INDEX(SubRequest I_SubRequest_DiskCopy) */ 'x'
            FROM SubRequest
           WHERE SubRequest.diskcopy = DiskCopy.id
             AND SubRequest.status IN (4, 5, 6, 12, 13, 14)) -- being processed (WAIT*, READY, *SCHED)
       AND NOT EXISTS
         -- Ignore diskcopies with active replications
         (SELECT * FROM StageDiskCopyReplicaRequest, DiskCopy D
           WHERE StageDiskCopyReplicaRequest.destDiskCopy = D.id
             AND StageDiskCopyReplicaRequest.sourceDiskCopy = DiskCopy.id
             AND D.status = 1)  -- WAITD2D
       AND rownum <= 10000 - totalCount
    RETURNING id BULK COLLECT INTO dcIds;
    COMMIT;

    -- If we have more than 10,000 files to GC, exit the loop. There is no point
    -- processing more as the maximum sent back to the client in one call is
    -- 10,000. This protects the garbage collector from being overwhelmed with
    -- requests and reduces the stager DB load. Furthermore, if too much data is
    -- sent back to the client, the transfer time between the stager and client
    -- becomes very long and the message may timeout or may not even fit in the
    -- clients receive buffer!!!!
    totalCount := totalCount + dcIds.COUNT();
    EXIT WHEN totalCount >= 10000;

    -- Continue processing but with STAGED files
    IF dontGC = 0 THEN
      -- Calculate the amount of space that would be freed on the filesystem
      -- if the files selected above were to be deleted.
      IF dcIds.COUNT > 0 THEN
        SELECT freed + sum(diskCopySize) INTO freed
          FROM DiskCopy
         WHERE DiskCopy.id IN
             (SELECT /*+ CARDINALITY(fsidTable 5) */ *
                FROM TABLE(dcIds) dcidTable);
      END IF;
      -- Get the amount of space to be liberated
      SELECT decode(sign(maxFreeSpace * totalSize - free), -1, 0, maxFreeSpace * totalSize - free)
        INTO toBeFreed
        FROM FileSystem
       WHERE id = fs.id;
      -- If space is still required even after removal of INVALID files, consider
      -- removing STAGED files until we are below the free space watermark
      IF freed < toBeFreed THEN
        -- Loop on file deletions. Select only enough files until we reach the
        -- 10000 return limit.
        FOR dc IN (SELECT id, castorFile FROM (
                     SELECT DiskCopy.id, DiskCopy.castorFile 
                       FROM DiskCopy, FileSystem, DiskServer
                      WHERE DiskCopy.filesystem = FileSystem.id
                        AND FileSystem.id = fs.id
                        AND FileSystem.status = 0 -- PRODUCTION
                        AND FileSystem.diskserver = DiskServer.id
                        AND DiskServer.status = 0 -- PRODUCTION
                        AND DiskCopy.status = 0 -- STAGED
                        AND NOT EXISTS (
                          SELECT /*+ INDEX(SubRequest I_SubRequest_DiskCopy) */ 'x'
                            FROM SubRequest
                           WHERE SubRequest.diskcopy = DiskCopy.id
                             AND SubRequest.status IN (4, 5, 6, 12, 13, 14)) -- being processed (WAIT*, READY, *SCHED)
                        ORDER BY gcWeight ASC)
                   WHERE rownum <= 10000 - totalCount) LOOP
          -- Mark the DiskCopy
          UPDATE DiskCopy
             SET status = 9, -- BEINGDELETED
                 gcType = 0  -- AUTO
           WHERE id = dc.id RETURNING diskCopySize INTO deltaFree;
          totalCount := totalCount + 1;
          -- Update freed space
          freed := freed + deltaFree;
          -- Shall we continue ?
          IF toBeFreed <= freed THEN
            EXIT;
          END IF;
        END LOOP;
      END IF;
      COMMIT;
    END IF;
    -- We have enough files to exit the loop ?
    EXIT WHEN totalCount >= 10000;
  END LOOP;

  -- Now select all the BEINGDELETED diskcopies in this diskserver for the GC daemon
  OPEN files FOR
    SELECT /*+ INDEX(CastorFile PK_CastorFile_ID) */ FileSystem.mountPoint || DiskCopy.path,
           DiskCopy.id,
           Castorfile.fileid, Castorfile.nshost,
           DiskCopy.lastAccessTime, DiskCopy.nbCopyAccesses, DiskCopy.gcWeight,
           CASE WHEN DiskCopy.gcType = 0 THEN 'Automatic'
                WHEN DiskCopy.gcType = 1 THEN 'User Requested'
                WHEN DiskCopy.gcType = 2 THEN 'Too many replicas'
                WHEN DiskCopy.gcType = 3 THEN 'Draining filesystem'
                ELSE 'Unknown' END,
           getSvcClassList(FileSystem.id)
      FROM CastorFile, DiskCopy, FileSystem, DiskServer
     WHERE decode(DiskCopy.status, 9, DiskCopy.status, NULL) = 9 -- BEINGDELETED
       AND DiskCopy.castorfile = CastorFile.id
       AND DiskCopy.fileSystem = FileSystem.id
       AND FileSystem.diskServer = DiskServer.id
       AND DiskServer.name = diskServerName
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
BEGIN
  IF dcIds.COUNT > 0 THEN
    -- list the castorfiles to be cleaned up afterwards
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      INSERT INTO filesDeletedProcHelper VALUES
           ((SELECT castorFile FROM DiskCopy
              WHERE id = dcIds(i)));
    -- Loop over the deleted files; first use FORALL for bulk operation
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      DELETE FROM Id2Type WHERE id = dcIds(i);
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      DELETE FROM DiskCopy WHERE id = dcIds(i);
    -- then use a normal loop to clean castorFiles
    FOR cf IN (SELECT DISTINCT(cfId) FROM filesDeletedProcHelper) LOOP
      DECLARE
        nb NUMBER;
        LockError EXCEPTION;
        PRAGMA EXCEPTION_INIT (LockError, -54);
      BEGIN
        -- First try to lock the castorFile
        SELECT id INTO nb FROM CastorFile
         WHERE id = cf.cfId FOR UPDATE NOWAIT;
        -- See whether it has any DiskCopy
        SELECT count(*) INTO nb FROM DiskCopy
         WHERE castorFile = cf.cfId;
        -- If any DiskCopy, give up
        IF nb = 0 THEN
          -- Delete the TapeCopies
          deleteTapeCopies(cf.cfId);
          -- See whether pending SubRequests exist
          SELECT count(*) INTO nb FROM SubRequest
           WHERE castorFile = cf.cfId
             AND status IN (0, 1, 2, 3, 4, 5, 6, 7, 10, 12, 13, 14);   -- All but FINISHED, FAILED_FINISHED, ARCHIVED
          IF nb = 0 THEN
            -- No Subrequest, delete the CastorFile
            DECLARE
              fid NUMBER;
              fc NUMBER;
              nsh VARCHAR2(2048);
            BEGIN
              -- Delete the CastorFile
              DELETE FROM id2Type WHERE id = cf.cfId;
              DELETE FROM CastorFile WHERE id = cf.cfId
              RETURNING fileId, nsHost, fileClass
                INTO fid, nsh, fc;
              -- check whether this file potentially had TapeCopies
              SELECT nbCopies INTO nb FROM FileClass WHERE id = fc;
              IF nb = 0 THEN
                -- This castorfile was created with no TapeCopy
                -- So removing it from the stager means erasing
                -- it completely. We should thus also remove it
                -- from the name server
                INSERT INTO FilesDeletedProcOutput VALUES (fid, nsh);
              END IF;
            END;
          ELSE
            -- SubRequests exist, fail them
            UPDATE SubRequest SET status = 7 -- FAILED
             WHERE castorFile = cf.cfId
               AND status IN (0, 1, 2, 3, 4, 5, 6, 12, 13, 14);
          END IF;
        END IF;
      EXCEPTION
        WHEN NO_DATA_FOUND THEN
          -- ignore, this means that the castorFile did not exist.
          -- There is thus no way to find out whether to remove the
          -- file from the nameserver. For safety, we thus keep it
          NULL;
        WHEN LockError THEN
          -- ignore, somebody else is dealing with this castorFile, 
        NULL;
      END;
    END LOOP;
  END IF;
  OPEN fileIds FOR SELECT * FROM FilesDeletedProcOutput;
END;
/

/*
 * PL/SQL method removing completely a file from the stager
 * including all its related objects (diskcopy, tapecopy, segments...)
 * The given files are supposed to already have been removed from the
 * name server
 * Note that we don't increase the freespace of the fileSystem.
 * This is done by the monitoring daemon, that knows the
 * exact amount of free space.
 * cfIds gives the list of files to delete.
 */
CREATE OR REPLACE PROCEDURE filesClearedProc(cfIds IN castor."cnumList") AS
  dcIds "numList";
BEGIN
  IF cfIds.COUNT <= 0 THEN
    RETURN;
  END IF;
  -- first convert the input array into a temporary table
  FORALL i IN cfIds.FIRST..cfIds.LAST
    INSERT INTO FilesClearedProcHelper (cfId) VALUES (cfIds(i));
  -- delete the DiskCopies in bulk
  SELECT id BULK COLLECT INTO dcIds
    FROM Diskcopy WHERE castorfile IN (SELECT cfId FROM FilesClearedProcHelper);
  FORALL i IN dcIds.FIRST .. dcIds.LAST
    DELETE FROM Id2Type WHERE id = dcIds(i);
  FORALL i IN dcIds.FIRST .. dcIds.LAST
    DELETE FROM DiskCopy WHERE id = dcIds(i);
  -- put SubRequests into FAILED (for non FINISHED ones)
  UPDATE SubRequest
     SET status = 7,  -- FAILED
         errorCode = 16,  -- EBUSY
         errorMessage = 'Request canceled by another user request'
   WHERE castorfile IN (SELECT cfId FROM FilesClearedProcHelper)
     AND status IN (4, 5, 6, 12, 13, 14);  -- being processed (WAIT*, READY, *SCHED)
  -- Loop over the deleted files for cleaning the tape copies
  FOR i in cfIds.FIRST .. cfIds.LAST LOOP
    deleteTapeCopies(cfIds(i));
  END LOOP;
  -- Finally drop castorFiles in bulk
  FORALL i IN cfIds.FIRST .. cfIds.LAST
    DELETE FROM Id2Type WHERE id = cfIds(i);
  FORALL i IN cfIds.FIRST .. cfIds.LAST
    DELETE FROM CastorFile WHERE id = cfIds(i);
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
      stageForcedRm(fileIds(fid), nsHostName);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- this file was dropped from nameServer AND stager
      -- and still exists on disk. We put it into the list
      -- of orphan fileids to return
      INSERT INTO NsFilesDeletedOrphans VALUES(fileIds(fid));
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
   INSERT INTO StgFilesDeletedOrphans VALUES(dcIds(i));
  -- Return a list of diskcopy ids which no longer exist
  OPEN stgOrphans FOR
    SELECT diskCopyId FROM StgFilesDeletedOrphans
     WHERE NOT EXISTS (
        SELECT 'x' FROM DiskCopy
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
      FORALL i IN ids.FIRST..ids.LAST
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
  timeOut INTEGER;
  rate INTEGER;
  srIds "numList";
  ct NUMBER;
BEGIN
  -- select requested timeout from configuration table
  timeout := 3600*TO_NUMBER(getConfigOption('cleaning', 'terminatedRequestsTimeout', '120'));
  -- get a rough estimate of the current request processing rate
  SELECT count(*) INTO rate
    FROM SubRequest
   WHERE status IN (9, 11)  -- FAILED_FINISHED, ARCHIVED
     AND lastModificationTime > getTime() - 1800;
  IF rate > 0 AND (500000 / rate * 1800) < timeOut THEN
    timeOut := 500000 / rate * 1800;  -- keep 500k requests max
  END IF;
  
  -- delete castorFiles if nothing is left for them. Here we use
  -- a temporary table as we need to commit every ~1000 operations
  -- and keeping a cursor opened on the original select may take
  -- too long, leading to ORA-01555 'snapshot too old' errors.
  EXECUTE IMMEDIATE 'TRUNCATE TABLE DeleteTermReqHelper';
  INSERT /*+ APPEND */ INTO DeleteTermReqHelper
    (SELECT id, castorFile FROM SubRequest
      WHERE status IN (9, 11)
        AND lastModificationTime < getTime() - timeOut);
  COMMIT;
  ct := 0;
  FOR cf IN (SELECT UNIQUE cfId FROM DeleteTermReqHelper) LOOP
    deleteCastorFile(cf.cfId);
    ct := ct + 1;
    IF ct = 1000 THEN
      COMMIT;
      ct := 0;
    END IF;
  END LOOP;

  -- now delete all old subRequest. We reuse here the
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
      FORALL i IN ids.FIRST..ids.LAST
        DELETE FROM SubRequest WHERE id = ids(i);
      COMMIT;
    END LOOP;
    CLOSE s;
  END;
  EXECUTE IMMEDIATE 'TRUNCATE TABLE DeleteTermReqHelper';

  -- and then related Requests
    ---- Get ----
  bulkDeleteRequests('StageGetRequest');
    ---- Put ----
  bulkDeleteRequests('StagePutRequest');
    ---- Update ----
  bulkDeleteRequests('StageUpdateRequest');
    ---- PrepareToGet -----
  bulkDeleteRequests('StagePrepareToGetRequest');
    ---- PrepareToPut ----
  bulkDeleteRequests('StagePrepareToPutRequest');
    ---- PrepareToUpdate ----
  bulkDeleteRequests('StagePrepareToUpdateRequest');
    ---- PutDone ----
  bulkDeleteRequests('StagePutDoneRequest');
    ---- Rm ----
  bulkDeleteRequests('StageRmRequest');
    ---- Repack ----
  bulkDeleteRequests('StageRepackRequest');
    ---- DiskCopyReplica ----
  bulkDeleteRequests('StageDiskCopyReplicaRequest');
    ---- SetGCWeight ----
  bulkDeleteRequests('SetFileGCWeight');
END;
/

/* Search and delete old diskCopies in bad states */
CREATE OR REPLACE PROCEDURE deleteFailedDiskCopies(timeOut IN NUMBER) AS
  dcIds "numList";
  cfIds "numList";
  ct INTEGER;
BEGIN
  -- select INVALID diskcopies without filesystem (they can exist after a
  -- stageRm that came before the diskcopy had been created on disk) and ALL FAILED
  -- ones (coming from failed recalls or failed removals from the GC daemon).
  -- Note that we don't select INVALID diskcopies from recreation of files
  -- because they are taken by the standard GC as they physically exist on disk.
  SELECT id
    BULK COLLECT INTO dcIds
    FROM DiskCopy
   WHERE (status = 4 OR (status = 7 AND fileSystem = 0))
     AND creationTime < getTime() - timeOut;
  SELECT /*+ INDEX(DC PK_DiskCopy_ID) */ UNIQUE castorFile
    BULK COLLECT INTO cfIds
    FROM DiskCopy DC
   WHERE id IN (SELECT /*+ CARDINALITY(ids 5) */ * FROM TABLE(dcIds) ids);
  -- drop the DiskCopies
  FORALL i IN dcIds.FIRST..dcIds.LAST
    DELETE FROM Id2Type WHERE id = dcIds(i);
  FORALL i IN dcIds.FIRST..dcIds.LAST
    DELETE FROM DiskCopy WHERE id = dcIds(i);
  COMMIT;
  -- maybe delete the CastorFiles if nothing is left for them
  IF cfIds.COUNT > 0 THEN
    ct := 0;
    FOR c IN cfIds.FIRST..cfIds.LAST LOOP
      deleteCastorFile(cfIds(c));
      ct := ct + 1;
      IF ct = 1000 THEN
        -- commit every 1000, don't pause
        ct := 0;
        COMMIT;
      END IF;
    END LOOP;
    COMMIT;
  END IF;
END;
/

/* Deal with old diskCopies in STAGEOUT */
CREATE OR REPLACE PROCEDURE deleteOutOfDateStageOutDCs(timeOut IN NUMBER) AS
  srId NUMBER;
BEGIN
  -- Deal with old DiskCopies in STAGEOUT/WAITFS. The rule is to drop
  -- the ones with 0 fileSize and issue a putDone for the others
  FOR f IN (SELECT c.filesize, c.id, c.fileId, c.nsHost, d.fileSystem, d.id AS dcId, d.status AS dcStatus
              FROM DiskCopy d, Castorfile c
             WHERE c.id = d.castorFile
               AND d.creationTime < getTime() - timeOut
               AND d.status IN (5, 6, 11) -- WAITFS, STAGEOUT, WAITFS_SCHEDULING
               AND NOT EXISTS (
                 SELECT 'x'
                   FROM SubRequest, Id2Type
                  WHERE castorFile = c.id
                    AND SubRequest.request = Id2Type.id
                    AND status IN (0, 1, 2, 3, 5, 6, 13, 14) -- all active
                    AND type NOT IN (37, 38))) LOOP -- ignore PrepareToPut, PrepareToUpdate
    IF (0 = f.fileSize) OR (f.dcStatus <> 6) THEN
      -- here we invalidate the diskcopy and let the GC run
      UPDATE DiskCopy SET status = 7  -- INVALID
       WHERE id = f.dcid;
      -- and we also fail the correspondent prepareToPut/Update request if it exists
      BEGIN
        SELECT id INTO srId   -- there can only be one outstanding PrepareToPut/Update, if any
          FROM SubRequest
         WHERE status = 6 AND diskCopy = f.dcid;
        archiveSubReq(srId, 9);  -- FAILED_FINISHED
      EXCEPTION WHEN NO_DATA_FOUND THEN
        NULL;
      END;
      INSERT INTO CleanupJobLog VALUES (f.fileId, f.nsHost, 0);
    ELSE
      -- here we issue a putDone
      -- context 2 : real putDone. Missing PPut requests are ignored.
      -- svcClass 0 since we don't know it. This will trigger a
      -- default behavior in the putDoneFunc
      putDoneFunc(f.id, f.fileSize, 2, 0);
      INSERT INTO CleanupJobLog VALUES (f.fileId, f.nsHost, 1);
    END IF;
  END LOOP;
  COMMIT;
END;
/


/* Deal with stuck recalls - this workaround should be dropped
   after rtcpclientd is reviewed */
CREATE OR REPLACE PROCEDURE restartStuckRecalls AS
BEGIN
  UPDATE Segment SET status = 0 WHERE status = 7 and tape IN
    (SELECT id from Tape WHERE tpmode = 0 AND status IN (0, 6) AND id IN
      (SELECT tape FROM Segment WHERE status = 7));
  UPDATE Tape SET status = 1 WHERE tpmode = 0 AND status IN (0, 6) AND id IN
    (SELECT tape FROM Segment WHERE status in (0, 7));
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


/* PL/SQL method used by the stager to log what it has been done by the cleanup job */
CREATE OR REPLACE PROCEDURE dumpCleanupLogs(jobLog OUT castorGC.JobLogEntry_Cur) AS
  unused NUMBER;
BEGIN
  SELECT fileid INTO unused FROM CleanupJobLog WHERE ROWNUM < 2;
  -- if we got here, we have something in the log table, let's lock it and dump it
  LOCK TABLE CleanupJobLog IN EXCLUSIVE MODE;
  OPEN jobLog FOR
    SELECT * FROM CleanupJobLog;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- nothing to do
  NULL;
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
                                'BULKCHECKFSBACKINPRODJOB',
                                'RESTARTSTUCKRECALLSJOB',
                                'ACCOUNTINGJOB'))
  LOOP
    DBMS_SCHEDULER.DROP_JOB(j.job_name, TRUE);
  END LOOP;

  -- Create a db job to be run every 20 minutes executing the deleteTerminatedRequests procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'houseKeepingJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN deleteTerminatedRequests(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=20',
      ENABLED         => TRUE,
      COMMENTS        => 'Cleaning of terminated requests');

  -- Create a db job to be run twice a day executing the cleanup procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'cleanupJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN cleanup(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=HOURLY; INTERVAL=12',
      ENABLED         => TRUE,
      COMMENTS        => 'Database maintenance');

  -- Create a db job to be run every 5 minutes executing the bulkCheckFSBackInProd procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'bulkCheckFSBackInProdJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN bulkCheckFSBackInProd(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=5',
      ENABLED         => TRUE,
      COMMENTS        => 'Bulk operation to processing filesystem state changes');

  -- Create a db job to be run every hour executing the restartStuckRecalls workaround procedure
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'restartStuckRecallsJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN restartStuckRecalls(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=60',
      ENABLED         => TRUE,
      COMMENTS        => 'Workaround to restart stuck recalls');

  -- Create a db job to be run every hour that generates the accounting information
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'accountingJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN 
                            DELETE FROM Accounting;
                            INSERT INTO Accounting (euid, fileSystem, nbBytes)
                              (SELECT owneruid, fileSystem, sum(diskCopySize)
                                 FROM DiskCopy
                                WHERE DiskCopy.status IN (0, 10)
                                  AND DiskCopy.owneruid IS NOT NULL
                                  AND DiskCopy.ownergid IS NOT NULL
                                GROUP BY owneruid, fileSystem);
                          END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=60',
      ENABLED         => TRUE,
      COMMENTS        => 'Generation of accounting information');
END;
/

/*******************************************************************
 * @(#)RCSfile: oracleDrain.sql,v  Revision: 1.7  Date: 2009/08/13 14:11:04  Author: itglp 
 * PL/SQL code for Draining FileSystems Logic
 *
 * Additional procedures modified to support the DrainingFileSystems
 * logic include: disk2DiskCopyDone, disk2DiskCopyFailed,
 * selectFiles2Delete, storeClusterStatus tr_DiskServer_Update and
 * tr_FileSystem_Update
 *
 * Note: the terms filesystem and mountpoint are used interchangeably
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* SQL statement for the delete trigger on the DrainingFileSystem table */
CREATE OR REPLACE TRIGGER tr_DrainingFileSystem_Delete
BEFORE DELETE ON DrainingFileSystem
FOR EACH ROW
DECLARE
  CURSOR c IS
    SELECT STDCRR.id, SR.id subrequest, STDCRR.client
      FROM StageDiskCopyReplicaRequest STDCRR, DrainingDiskCopy DDC,
           SubRequest SR
     WHERE STDCRR.sourceDiskCopy = DDC.diskCopy
       AND SR.request = STDCRR.id
       AND decode(DDC.status, 4, DDC.status, NULL) = 4  -- FAILED
       AND SR.status = 9  -- FAILED_FINISHED
       AND DDC.fileSystem = :old.fileSystem;
  reqIds    "numList";
  clientIds "numList";
  srIds     "numList";
BEGIN
  -- Remove failed transfers requests from the StageDiskCopyReplicaRequest
  -- table. If we do not do this files which failed due to "Maximum number of
  -- attempts exceeded" cannot be resubmitted to the system.
  -- (see getBestDiskCopyToReplicate)
  LOOP
    OPEN c;
    FETCH c BULK COLLECT INTO reqIds, srIds, clientIds LIMIT 10000;
    -- Break out of the loop when the cursor returns no results
    EXIT WHEN reqIds.count = 0;
    -- Delete data
    FORALL i IN reqIds.FIRST .. reqIds.LAST
      DELETE FROM Id2Type WHERE id IN (reqIds(i), clientIds(i), srIds(i));
    FORALL i IN clientIds.FIRST .. clientIds.LAST
      DELETE FROM Client WHERE id = clientIds(i);
    FORALL i IN srIds.FIRST .. srIds.LAST
      DELETE FROM SubRequest WHERE id = srIds(i);
    CLOSE c;
    -- Release locks
    COMMIT;
  END LOOP;
  -- Delete all data related to the filesystem from the draining diskcopy table
  DELETE FROM DrainingDiskCopy
   WHERE fileSystem = :old.fileSystem;
END;
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


/* Function used to convert bytes into a human readable string using powers
 * of 1000 (SI units). E.g 2000 = 2k.
 */
CREATE OR REPLACE FUNCTION sizeOfFmtSI(bytes IN NUMBER) RETURN VARCHAR2 AS
  unit VARCHAR2(2);
  num  NUMBER := bytes;
BEGIN
  IF bytes > 1000000000000000 THEN  -- PETA
    num := trunc(bytes / 1000000000000000, 2);
    unit := 'P';
  ELSIF bytes > 1000000000000 THEN  -- TERA
    num := trunc(bytes / 1000000000000, 2);
    unit := 'T';
  ELSIF bytes > 1000000000 THEN     -- GIGA
    num := trunc(bytes / 1000000000, 2);
    unit := 'G';
  ELSIF bytes > 1000000 THEN        -- MEGA
    num := trunc(bytes / 1000000, 2);
    unit := 'M';
  ELSIF bytes > 1000 THEN           -- KILO
    num := trunc(bytes / 1000, 2);
    unit := 'k';
  END IF;

  RETURN (to_char(num, 'FM99900.009') || unit);
END;
/


/* Function used to covert elapsed time in seconds into a human readable
 * string. For example, 3600 seconds = '00 01:00:00'
 */
CREATE OR REPLACE FUNCTION getInterval(startTime IN NUMBER, endTime IN NUMBER)
RETURN VARCHAR2 AS
  elapsed NUMBER;
  ret     VARCHAR2(2048);
BEGIN
  -- If the elapsed time is negative or greater than 99 days return an error
  elapsed := ceil(endTime) - floor(startTime);
  IF elapsed > 86400 * 99 OR elapsed < 0 THEN
    RETURN '## ##:##:##';
  END IF;
  -- Convert the elapsed time in seconds to an interval string
  -- e.g. +000000011 13:46:40.
  ret := rtrim(numtodsinterval(elapsed, 'SECOND'), 0);
  -- Remove the trailing '.' and leading '+'
  ret := substr(ret, 2, length(ret) - 2);
  -- Remove the leading '0'
  RETURN substr(ret, + 8);
END;
/


/* SQL statement for the creation of the DrainingOverview view */
CREATE OR REPLACE VIEW DrainingOverview
AS
  SELECT DS.name DiskServer,
         FS.mountPoint MountPoint,
         -- Determine the status of the filesystem being drained. If it's in a
         -- PRODUCTION status then the status of the diskserver is taken.
         decode(
           decode(FS.status, 0, DS.status, FS.status),
                  0, 'PRODUCTION',
                  1, 'DRAINING',
                  2, 'DISABLED', 'UNKNOWN') FileSystemStatus,
         DFS.username Username,
         DFS.machine Machine,
         getTimeString(ceil(DFS.creationTime)) Created,
         DFS.maxTransfers MaxTransfers,
         DFS.totalFiles TotalFiles,
         sizeOfFmtSI(DFS.totalBytes) TotalSize,
         -- Translate the fileMask value to string
         decode(DFS.fileMask,
                0, 'STAGED',
                1, 'CANBEMIGR',
                2, 'ALL', 'UNKNOWN') FileMask,
         -- Translate the autoDelete value to a string
         decode(DFS.autoDelete, 0, 'NO', 1, 'YES', 'UNKNOWN') AutoDelete,
         -- The target service class
         SVC.name SvcClass,
         -- Determine the status of the draining process. If the last update
         -- time is more than 60 minutes ago the process is considered to be
         -- STALLED.
         decode(DFS.status, 0, 'CREATED', 1, 'INITIALIZING', 2,
           decode(sign((getTime() - 3600) - DFS.lastUpdateTime),
                  -1, 'RUNNING', 'STALLED'),
                   3, 'INTERRUPTED',
                   4, 'FAILED',
                   5, 'COMPLETED',
                   6, 'DELETING', 'UNKNOWN') Status,
         nvl(DDCS.filesRemaining, 0) FilesRemaining,
         sizeOfFmtSI(nvl(DDCS.bytesRemaining, 0)) SizeRemaining,
         nvl(DDCS.running, 0) Running,
         nvl(DDCS.failed, 0) Failed,
         -- Calculate how long the process for draining the filesystem has been
         -- running. If the process is in a CREATED or INITIALIZING status
         -- 00 00:00:00 will be returned.
         decode(DFS.status, 0, '00 00:00:00', 1, '00 00:00:00',
           decode(sign(DFS.status - 3), -1,
             getInterval(DFS.startTime, gettime()),
               getInterval(DFS.startTime, DFS.lastUpdateTime))) RunTime,
         -- Calculate how far the process has gotten as a percentage of the data
         -- already transferred. If the process is in a CREATED, INITIALIZING or
         -- DELETING status N/A will be returned.
         decode(DFS.status, 0, 'N/A', 1, 'N/A', 6, 'N/A',
           decode(DFS.totalBytes, 0, '100%',
             concat(to_char(
               floor(((DFS.totalBytes - nvl(DDCS.bytesRemaining, 0)) /
                       DFS.totalBytes) * 100)), '%'))) Progress,
         -- Calculate the estimated time to completion in seconds if the process
         -- is in a RUNNING status and more than 10% of the data has already by
         -- transferred.
         decode(DFS.status, 2, decode(DFS.totalBytes, 0, 'N/A',
           decode(sign((getTime() - 3600) - DFS.lastUpdateTime), -1,
             decode(sign((((DFS.totalBytes - nvl(DDCS.bytesRemaining, 0)) /
                            DFS.totalBytes) * 100) - 10), -1, 'N/A',
               getInterval(0, trunc(DDCS.bytesRemaining / ((DFS.totalBytes -
                           nvl(DDCS.bytesRemaining, 0)) /
                           (getTime() - DFS.startTime))))), 'N/A'), 'N/A')) ETC
    FROM (
      SELECT fileSystem,
             max(decode(status, 3, nbFiles, 0)) Running,
             max(decode(status, 4, nbFiles, 0)) Failed,
             sum(nbFiles) FilesRemaining,
             sum(totalFileSize) BytesRemaining
        FROM (
          SELECT fileSystem, status, count(*) nbFiles,
                 sum(fileSize) totalFileSize
            FROM DrainingDiskCopy
           GROUP BY fileSystem, status
        )
       GROUP BY fileSystem
    ) DDCS
   RIGHT JOIN DrainingFileSystem DFS
      ON DFS.fileSystem = DDCS.fileSystem
   INNER JOIN FileSystem FS
      ON DFS.fileSystem = FS.id
   INNER JOIN DiskServer DS
      ON FS.diskServer = DS.id
   INNER JOIN SvcClass SVC
      ON DFS.svcClass = SVC.id
   ORDER BY DS.name, FS.mountPoint;


/* SQL statement for the creation of the DrainingFailures view */
CREATE OR REPLACE VIEW DrainingFailures
AS
  SELECT DS.name DiskServer,
  	 FS.mountPoint MountPoint,
         regexp_replace
           ((FS.MountPoint || '/' || DC.path),'(/){2,}','/') Path,
         nvl(DDC.comments, 'Unknown') Comments
    FROM DrainingDiskCopy DDC, DiskCopy DC, FileSystem FS, DiskServer DS
   WHERE DDC.diskCopy = DC.id
     AND DDC.status = 4  -- FAILED
     AND DC.fileSystem = FS.id
     AND FS.diskServer = DS.id
   ORDER BY DS.name, FS.mountPoint, DC.path;


/* Procedure responsible for stopping the draining process for a diskserver
 * or filesystem. In no filesystem is specified then all filesystems
 * associated to the diskserver will be stopped.
 */
CREATE OR REPLACE PROCEDURE stopDraining(inNodeName   IN VARCHAR,
                                         inMountPoint IN VARCHAR2 DEFAULT NULL)
AS
  fsIds "numList";
BEGIN
  -- Check that the nodename and mountpoint input options are valid
  SELECT FileSystem.id BULK COLLECT INTO fsIds
    FROM FileSystem, DiskServer
   WHERE FileSystem.diskServer = DiskServer.id
     AND (FileSystem.mountPoint = inMountPoint
      OR inMountPoint IS NULL)
     AND DiskServer.name = inNodeName;
  IF fsIds.COUNT = 0 THEN
    IF inMountPoint IS NULL THEN
      raise_application_error
        (-20019, 'Diskserver does not exist or has no mountpoints');
    ELSE
      raise_application_error
        (-20015, 'Diskserver and mountpoint does not exist');
    END IF;
  END IF;
  -- Update the filesystem entries to DELETING. The drainManager job will
  -- finalize the deletion of the entry once all outstanding transfers have
  -- terminated.
  UPDATE DrainingFileSystem
     SET status = 6  -- DELETING
   WHERE fileSystem IN
     (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
        FROM TABLE (fsIds) fsIdTable);
END;
/


/* Procedure responsible for starting the draining process for a diskserver
 * or filesystem. If no filesystem is specified then all filesystems
 * associated to the diskserver will be add to the list of filesystems to be
 * drained.
 */
CREATE OR REPLACE
PROCEDURE startDraining(inNodeName     IN VARCHAR,
                        inMountPoint   IN VARCHAR DEFAULT NULL,
                        inSvcClass     IN VARCHAR DEFAULT NULL,
                        inFileMask     IN NUMBER DEFAULT 1,
                        inAutoDelete   IN NUMBER DEFAULT 0,
                        inMaxTransfers IN NUMBER DEFAULT 50)
AS
  ret    NUMBER;
  fsIds  "numList";
  svcId  NUMBER;
  unused NUMBER;
  mntPnt VARCHAR2(2048);
BEGIN
  -- Check that the nodename and mountpoint input options are valid
  SELECT FileSystem.id BULK COLLECT INTO fsIds
    FROM FileSystem, DiskServer
   WHERE FileSystem.diskServer = Diskserver.id
     AND (FileSystem.mountPoint = inMountPoint
      OR inMountPoint IS NULL)
     AND DiskServer.name = inNodeName;
  IF fsIds.COUNT = 0 THEN
    IF inMountPoint IS NULL THEN
      raise_application_error
        (-20019, 'Diskserver does not exist or has no mountpoints');
    ELSE
      raise_application_error
        (-20015, 'Diskserver and mountpoint does not exist');
    END IF;
  END IF;
  -- Loop over the mountpoints extracted above and validate that the service
  -- class and status of the mountpoint are correct.
  FOR i IN fsIds.FIRST .. fsIds.LAST
  LOOP
    SELECT mountPoint INTO mntPnt
      FROM FileSystem WHERE id = fsIds(i);
    -- If no service class option is defined attempt to automatically find one.
    -- Note: this only works if the filesystem belongs to one and only one
    -- service class. If this is not the case the user must explicit provide
    -- one on the command line using the --svcclass option to the
    -- draindiskserver tool.
    IF inSvcClass IS NULL THEN
      BEGIN
        SELECT SvcClass.id INTO svcId
          FROM FileSystem, DiskPool2SvcClass, SvcClass
         WHERE FileSystem.diskPool = DiskPool2SvcClass.parent
           AND DiskPool2SvcClass.child = SvcClass.id
           AND FileSystem.id = fsIds(i);
      EXCEPTION
        WHEN TOO_MANY_ROWS THEN
          raise_application_error
            (-20101, 'Mountpoint: '||mntPnt||' belongs to multiple service
                      classes, please specify which service class to use, using
                      the --svcclass option');
        WHEN NO_DATA_FOUND THEN
          raise_application_error
            (-20120, 'Mountpoint: '||mntPnt||' does not belong to any
                      service class');
      END;
    ELSE
      -- Check if the user supplied service class name exists
      ret := checkForValidSvcClass(inSvcClass, 0, 1);
      -- Check that the mountpoint belongs to the service class provided by the
      -- user. This check is necessary as we do not support the draining of a
      -- mountpoint to a service class which it is not already a member of.
      BEGIN
        SELECT SvcClass.id INTO SvcId
          FROM FileSystem, DiskPool2SvcClass, SvcClass
         WHERE FileSystem.diskPool = DiskPool2SvcClass.parent
           AND DiskPool2SvcClass.child = SvcClass.id
           AND FileSystem.id = fsIds(i)
           AND SvcClass.name = inSvcClass;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        raise_application_error
          (-20117, 'Mountpoint: '||mntPnt||' does not belong to the '''||
                    inSvcClass||''' service class');
      END;
    END IF;
    -- If the mountpoint is not in a DRAINING status then the draining process
    -- cannot proceed.
    BEGIN
      SELECT FS.diskPool INTO unused
        FROM FileSystem FS, DiskServer DS
       WHERE FS.diskServer = DS.id
         AND FS.id = fsIds(i)
         AND decode(FS.status, 0, DS.status, FS.status) = 1;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      raise_application_error
        (-20116, 'Mountpoint: '||mntPnt||' is not in a DRAINING state');
    END;
    -- Check to see if the mountpoint is already being drained. Note: we do not
    -- allow the resubmission of a mountpoint without a prior DELETION unless
    -- the previous process was not in a FAILED or COMPLETED state
    BEGIN
      SELECT fileSystem INTO unused
        FROM DrainingFileSystem
       WHERE fileSystem = fsIds(i)
         AND status NOT IN (4, 5);  -- FAILED, COMPLETED
      raise_application_error
        (-20118, 'Mountpoint: '||mntPnt||' is already being drained');
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Cleanup
      DELETE FROM DrainingFileSystem
       WHERE fileSystem = fsIds(i);
      -- Insert the new mountpoint into the list of those to be drained. The
      -- drainManager job will later pick it up and start the draining process.
      INSERT INTO DrainingFileSystem
        (userName, machine, creationTime, fileSystem, svcClass, autoDelete,
         fileMask, maxTransfers)
      VALUES
        (-- For the time being the draindiskserver command is distributed with
         -- the castor-dbtools package and uses the /etc/castor/ORASTAGERCONFIG
         -- password to connect to the database. As the file is only readable
         -- by the root user and the st group, the OS_USER will either be root
         -- or stage. This is not very interesting!!
         sys_context('USERENV', 'OS_USER'),
         sys_context('USERENV', 'HOST'),
         getTime(), fsIds(i), svcId, inAutoDelete, inFileMask, inMaxTransfers);
    END;
  END LOOP;
END;
/


/* Procedure responsible for processing the snapshot of files that need to be
 * replicated in order to drain a filesystem.
 */
CREATE OR REPLACE
PROCEDURE drainFileSystem(fsId IN NUMBER)
AS
  unused     NUMBER;
  res        NUMBER;
  dcId       NUMBER;
  cfId       NUMBER;
  svcId      NUMBER;
  ouid       NUMBER;
  ogid       NUMBER;
  autoDelete NUMBER;
BEGIN
  -- As this procedure is called recursively we release the previous calls
  -- locks. This prevents the procedure from locking too many castorfile
  -- entries which could result in service degradation.
  COMMIT;
  -- Update the filesystems entry in the DrainingFileSystem table to indicate
  -- that activity is ongoing. If no entry exist then the filesystem is not
  -- under the control of the draining logic.
  svcId := 0;
  UPDATE DrainingFileSystem
     SET lastUpdateTime = getTime()
   WHERE fileSystem = fsId
     AND status = 2  -- RUNNING
  RETURNING svcclass, autoDelete INTO svcId, autoDelete;
  IF svcId = 0 THEN
    RETURN;  -- Do nothing
  END IF;
  -- Extract the next diskcopy to be processed. Note: this is identical to the
  -- way that subrequests are picked up in the SubRequestToDo procedure. The N
  -- levels of SELECTS and hints allow us to process the entries in the
  -- DrainingDiskCopy snapshot in an ordered way.
  dcId := 0;
  UPDATE DrainingDiskCopy
     SET status = 2  -- PROCESSING
   WHERE diskCopy = (
     SELECT diskCopy FROM (
       SELECT /*+ INDEX(DC I_DrainingDCs_PC) */ DDC.diskCopy
         FROM DrainingDiskCopy DDC
        WHERE DDC.fileSystem = fsId
          AND DDC.status IN (0, 1)  -- CREATED, RESTARTED
        ORDER BY DDC.priority DESC, DDC.creationTime ASC)
     WHERE rownum < 2)
  RETURNING diskCopy INTO dcId;
  IF dcId = 0 THEN
    -- If there are no transfers outstanding related to the draining process we
    -- can update the filesystem entry in the DrainingFileSystem table for the
    -- last time.
    UPDATE DrainingFileSystem
       SET status = decode((SELECT count(*)
                              FROM DrainingDiskCopy
                             WHERE status = 4  -- FAILED
                               AND fileSystem = fsId), 0, 5, 4),
           lastUpdateTime = getTime()
     WHERE fileSystem = fsId
       -- Check to make sure there are no transfers outstanding.
       AND (SELECT count(*) FROM DrainingDiskCopy
             WHERE status = 3  -- RUNNING
               AND fileSystem = fsId) = 0;
    RETURN;  -- Out of work!
  END IF;
  -- The DrainingDiskCopy table essentially contains a snapshot of the work
  -- that needs to be done at the time the draining process was initiated for a
  -- filesystem. As a result, the diskcopy id previously extracted may no longer
  -- be valid as the snapshot information is outdated. Here we simply verify
  -- that the diskcopy is still what we expected.
  BEGIN
    -- Determine the castorfile id
    SELECT castorFile INTO cfId FROM DiskCopy WHERE id = dcId;
    -- Lock the castorfile
    SELECT id INTO cfId
      FROM CastorFile WHERE id = cfId FOR UPDATE;
    -- Check that the status of the diskcopy is STAGED or CANBEMIGR. If the
    -- diskcopy is not in one of these states then it is no longer a candidate
    -- to be replicated. For example, it may have been deleted, resulting in the
    -- diskcopy being invalidated and dropped by the garbage collector.
    SELECT ownerUid, ownerGid INTO ouid, ogid
      FROM DiskCopy
     WHERE id = dcId
       AND status IN (0, 10);  -- STAGED, CANBEMIGR
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- The diskcopy no longer exists so we delete it from the snapshot of files
    -- to be processed.
    DELETE FROM DrainingDiskCopy
      WHERE diskCopy = dcId AND fileSystem = fsId;
    drainFileSystem(fsId);
    RETURN;  -- No replication required
  END;
  -- Just because the file was listed in the snapshot doesn't mean that it must
  -- be replicated! Here we check to see if the file is available on another
  -- diskserver in the target service class and if enough copies of the file
  -- are available.
  --
  -- The decode logic used here essentially states:
  --   If replication on close is enabled and there are enough copies available
  --   to satisfy the maxReplicaNb on the service class, then do not replicate.
  -- Otherwise,
  --   If replication on close is disabled and a copy exists elsewhere, then do
  --   do not replicate. All other cases result in triggering replication.
  --
  -- Notes:
  -- - The check to see if we have enough copies online when replication on
  --   close is enabled will effectively rebalance the pool!
  SELECT decode(replicateOnClose, 1,
         decode((available - maxReplicaNb) -
                 abs(available - maxReplicaNb), 0, 0, 1),
         decode(sign(available), 1, 0, 1)) INTO res
    FROM (
      SELECT count(*) available
        FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
       WHERE DiskCopy.fileSystem = FileSystem.id
         AND DiskCopy.castorFile = cfId
         AND DiskCopy.id != dcId
         AND DiskCopy.status IN (0, 10)  -- STAGED, CANBEMIGR
         AND FileSystem.diskPool = DiskPool2SvcClass.parent
         AND DiskPool2SvcClass.child = svcId
         AND FileSystem.status IN (0, 1)  -- PRODUCTION, DRAINING
         AND FileSystem.diskServer = DiskServer.id
         AND DiskServer.status IN (0, 1)  -- PRODUCTION, DRAINING
     ) results, SvcClass
   WHERE SvcClass.id = svcId;
  IF res = 0 THEN
    -- No replication is required, there are enough copies of the file to
    -- satisfy the requirements of the target service class.
    IF autoDelete = 1 THEN
      -- Invalidate the diskcopy so that it can be garbage collected.
      UPDATE DiskCopy
         SET status = 7,  -- INVALID
             gctype = 3   -- Draining filesystem
       WHERE id = dcId AND fileSystem = fsId;
    END IF;
    -- Delete the diskcopy from the snapshot as no action is required.
    DELETE FROM DrainingDiskCopy
     WHERE diskCopy = dcId AND fileSystem = fsId;
    drainFileSystem(fsId);
    RETURN;
  END IF;
  -- Check to see if there is already an outstanding request to replicate the
  -- castorfile to the target service class. If so, we wait on that replication
  -- to complete. This avoids having multiple requests to replicate the same
  -- file to the same target service class multiple times.
  BEGIN
    SELECT DiskCopy.id INTO res
      FROM DiskCopy, StageDiskCopyReplicaRequest
     WHERE StageDiskCopyReplicaRequest.destDiskCopy = DiskCopy.id
       AND StageDiskCopyReplicaRequest.svcclass = svcId
       AND DiskCopy.castorFile = cfId
       AND DiskCopy.status = 1  -- WAITDISK2DISKCOPY
       AND rownum < 2;
    IF res > 0 THEN
      -- Wait on another replication to complete.
      UPDATE DrainingDiskCopy
         SET status = 3,  -- WAITD2D
             parent = res
       WHERE diskCopy = dcId AND fileSystem = fsId;
       RETURN;
    END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    NULL;  -- No pending replications running
  END;
  -- If we have attempted to replicate the file more than 10 times already then
  -- give up! The error will be exposed later to an administrator for manual
  -- corrective action.
  SELECT count(*) INTO res
    FROM StageDiskCopyReplicaRequest R, SubRequest
   WHERE SubRequest.request = R.id
     AND R.sourceDiskCopy = dcId
     AND SubRequest.status = 9; -- FAILED_FINISHED
  IF res >= 10 THEN
    UPDATE DrainingDiskCopy
       SET status = 4,
           comments = 'Maximum number of attempts exceeded'
     WHERE diskCopy = dcId AND fileSystem = fsId;
    drainFileSystem(fsId);
    RETURN;  -- Try again
  END IF;
  -- Trigger a replication request for the file
  createDiskCopyReplicaRequest(0, dcId, svcId, svcId, ouid, ogid);
  -- Update the status of the file
  UPDATE DrainingDiskCopy SET status = 3  -- WAITD2D
   WHERE diskCopy = dcId AND fileSystem = fsId;
END;
/


/* SQL statement for DBMS_SCHEDULER job creation */
BEGIN
  -- Remove jobs related to the draining logic before recreating them
  FOR j IN (SELECT job_name FROM user_scheduler_jobs
             WHERE job_name IN ('DRAINMANAGERJOB'))
  LOOP
    DBMS_SCHEDULER.DROP_JOB(j.job_name, TRUE);
  END LOOP;

  -- Create the drain manager job to be executed every minute
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'drainManagerJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN DrainManager(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 5/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=1',
      ENABLED         => TRUE,
      COMMENTS        => 'Database job to manage the draining process');
END;
/


/* Procedure responsible for managing the draining process */
CREATE OR REPLACE PROCEDURE drainManager AS
  fsIds "numList";
BEGIN
  -- Delete the filesystems which are:
  --  A) in a DELETING state and have no transfers pending in the scheduler.
  --  B) are COMPLETED and older the 7 days.
  DELETE FROM DrainingFileSystem
   WHERE (NOT EXISTS (SELECT 'x' FROM DrainingDiskCopy
                       WHERE fileSystem = DrainingFileSystem.fileSystem
                         AND status = 3)  -- WAITD2D
          AND status = 6)  -- DELETING
      OR (status = 5 AND lastUpdateTime < getTime() - (7 * 86400));
  -- Process filesystems which in a CREATED state
  UPDATE DrainingFileSystem
     SET status = 1  -- INITIALIZING
   WHERE status = 0  -- CREATED
  RETURNING fileSystem BULK COLLECT INTO fsIds;
  -- Commit, this isn't really necessary but its done so that the user gets
  -- feedback when listing the filesystems which are being drained.
  COMMIT;
  IF fsIds.COUNT = 0 THEN
    -- Shrink the DrainingDiskCopy to keep the number of blocks small
    EXECUTE IMMEDIATE 'ALTER TABLE DrainingDiskCopy SHRINK SPACE CASCADE';
    RETURN;  -- No results
  END IF;
  -- Create the DrainingDiskCopy snapshot
  INSERT /*+ APPEND */ INTO DrainingDiskCopy
    (fileSystem, diskCopy, creationTime, priority, fileSize)
      (SELECT /*+ index(DC I_DiskCopy_FileSystem) */
              DC.fileSystem, DC.id, DC.creationTime, DC.status,
              DC.diskCopySize
         FROM DiskCopy DC, DrainingFileSystem DFS
        WHERE DFS.fileSystem = DC.fileSystem
          AND DFS.fileSystem IN
            (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
               FROM TABLE (fsIds) fsIdTable)
          AND ((DC.status = 0         AND DFS.fileMask = 0)    -- STAGED
           OR  (DC.status = decode(DC.status, 10, DC.status, NULL)
                AND DFS.fileMask = 1)                          -- CANBEMIGR
           OR  (DC.status IN (0, 10)  AND DFS.fileMask = 2))); -- ALL
  -- Update the DrainingFileSystem counters
  FOR a IN (SELECT fileSystem, count(*) files, sum(DDC.fileSize) bytes
              FROM DrainingDiskCopy DDC
             WHERE DDC.fileSystem IN
               (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
                  FROM TABLE (fsIds) fsIdTable)
               AND DDC.status = 0  -- CREATED
             GROUP BY DDC.fileSystem)
  LOOP
    UPDATE DrainingFileSystem
       SET totalFiles = a.files,
           totalBytes = a.bytes
     WHERE fileSystem = a.fileSystem;
  END LOOP;
  -- Update the filesystem entries to RUNNING
  UPDATE DrainingFileSystem
     SET status = 2,  -- RUNNING
         startTime = getTime(),
         lastUpdateTime = getTime()
   WHERE fileSystem IN
     (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
        FROM TABLE (fsIds) fsIdTable);
  -- Start the process of draining the filesystems. For an explanation of the
  -- query refer to: "Creating N Copies of a Row":
  -- http://forums.oracle.com/forums/message.jspa?messageID=1953433#1953433
  FOR a IN ( SELECT fileSystem
               FROM DrainingFileSystem
              WHERE fileSystem IN
                (SELECT /*+ CARDINALITY(fsIdTable 5) */ *
                   FROM TABLE (fsIds) fsIdTable)
            CONNECT BY CONNECT_BY_ROOT fileSystem = fileSystem
                AND LEVEL <= maxTransfers
                AND PRIOR sys_guid() IS NOT NULL
              ORDER BY totalBytes ASC, fileSystem)
  LOOP
    drainFileSystem(a.fileSystem);
  END LOOP;
END;
/
/*******************************************************************
 *
 * @(#)RCSfile: oracleDebug.sql,v  Revision: 1.14  Date: 2009/08/13 13:34:16  Author: itglp 
 *
 * Some SQL code to ease support and debugging
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PL/SQL declaration for the castorDebug package */
CREATE OR REPLACE PACKAGE castorDebug AS
  TYPE DiskCopyDebug_typ IS RECORD (
    id INTEGER,
    diskPool VARCHAR2(2048),
    location VARCHAR2(2048),
    available CHAR(1),
    status NUMBER,
    creationtime VARCHAR2(2048),
    gcWeight NUMBER);
  TYPE DiskCopyDebug IS TABLE OF DiskCopyDebug_typ;
  TYPE SubRequestDebug IS TABLE OF SubRequest%ROWTYPE;
  TYPE RequestDebug_typ IS RECORD (
    creationtime VARCHAR2(2048),
    SubReqId NUMBER,
    SubReqParentId NUMBER,
    Status NUMBER,
    username VARCHAR2(2048),
    machine VARCHAR2(2048),
    svcClassName VARCHAR2(2048),
    ReqId NUMBER,
    ReqType VARCHAR2(20));
  TYPE RequestDebug IS TABLE OF RequestDebug_typ;
  TYPE TapeCopyDebug_typ IS RECORD (
    TCId NUMBER,
    TCStatus NUMBER,
    TCMissing NUMBER,
    TCNbRetry NUMBER,
    SegId NUMBER,
    SegStatus NUMBER,
    SegErrCode NUMBER,
    VID VARCHAR2(2048),
    tpMode NUMBER,
    TapeStatus NUMBER,
    nbStreams NUMBER,
    SegErr VARCHAR2(2048));
  TYPE TapeCopyDebug IS TABLE OF TapeCopyDebug_typ;
END;
/


/* Return the castor file id associated with the reference number */
CREATE OR REPLACE FUNCTION getCF(ref NUMBER) RETURN NUMBER AS
  t NUMBER;
  cfId NUMBER;
BEGIN
  SELECT type INTO t FROM id2Type WHERE id = ref;
  IF (t = 2) THEN -- CASTORFILE
    RETURN ref;
  ELSIF (t = 5) THEN -- DiskCopy
    SELECT castorFile INTO cfId FROM DiskCopy WHERE id = ref;
  ELSIF (t = 27) THEN -- SubRequest
    SELECT castorFile INTO cfId FROM SubRequest WHERE id = ref;
  ELSIF (t = 30) THEN -- TapeCopy
    SELECT castorFile INTO cfId FROM TapeCopy WHERE id = ref;
  ELSIF (t = 18) THEN -- Segment
    SELECT castorFile INTO cfId FROM TapeCopy, Segment WHERE Segment.id = ref AND TapeCopy.id = Segment.copy;
  END IF;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- fileid ?
  SELECT id INTO cfId FROM CastorFile WHERE fileId = ref;
  RETURN cfId;
END;
/


/* Get the diskcopys associated with the reference number */
CREATE OR REPLACE FUNCTION getDCs(ref number) RETURN castorDebug.DiskCopyDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT DiskCopy.id,
                   DiskPool.name AS diskpool,
                   DiskServer.name || ':' || FileSystem.mountPoint || DiskCopy.path AS location,
                   decode(DiskServer.status, 2, 'N', decode(FileSystem.status, 2, 'N', 'Y')) AS available,
                   DiskCopy.status AS status,
                   getTimeString(creationtime) AS creationtime,
                   trunc(DiskCopy.gcWeight, 2) AS gcweight
              FROM DiskCopy, FileSystem, DiskServer, DiskPool
             WHERE DiskCopy.fileSystem = FileSystem.id(+)
               AND FileSystem.diskServer = diskServer.id(+)
               AND DiskPool.id(+) = fileSystem.diskPool
               AND DiskCopy.castorfile = getCF(ref)) LOOP
     PIPE ROW(d);
  END LOOP;
END;
/


/* Get the tapecopys, segments and streams associated with the reference number */
CREATE OR REPLACE FUNCTION getTCs(ref number) RETURN castorDebug.TapeCopyDebug PIPELINED AS
BEGIN
  FOR t IN (SELECT TapeCopy.id AS TCId, TapeCopy.status AS TCStatus,
                   TapeCopy.missingCopies AS TCmissing, TapeCopy.nbRetry AS TCNbRetry,
                   Segment.Id, Segment.status AS SegStatus, Segment.errorCode AS SegErrCode,
                   Tape.vid AS VID, Tape.tpMode AS tpMode, Tape.Status AS TapeStatus,
                   CASE WHEN Stream2TapeCopy.child IS NULL THEN 0 ELSE count(*) END AS nbStreams,
                   Segment.errMsgTxt AS SegErr
              FROM TapeCopy, Segment, Tape, Stream2TapeCopy
             WHERE TapeCopy.id = Segment.copy(+)
               AND Segment.tape = Tape.id(+)
               AND TapeCopy.castorfile = getCF(ref)
               AND Stream2TapeCopy.child(+) = TapeCopy.id
              GROUP BY TapeCopy.id, TapeCopy.status, TapeCopy.missingCopies, TapeCopy.nbRetry,
                       Segment.id, Segment.status, Segment.errorCode, Tape.vid, Tape.tpMode,
                       Tape.Status, Segment.errMsgTxt, Stream2TapeCopy.child) LOOP
     PIPE ROW(t);
  END LOOP;
END;
/


/* Get the subrequests associated with the reference number. (By castorfile/diskcopy/
 * subrequest/tapecopy or fileid
 */
CREATE OR REPLACE FUNCTION getSRs(ref number) RETURN castorDebug.SubRequestDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT * FROM SubRequest WHERE castorfile = getCF(ref)) LOOP
     PIPE ROW(d);
  END LOOP;
END;
/


/* Get the requests associated with the reference number. (By castorfile/diskcopy/
 * subrequest/tapecopy or fileid
 */
CREATE OR REPLACE FUNCTION getRs(ref number) RETURN castorDebug.RequestDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT getTimeString(creationtime) AS creationtime,
                   SubRequest.id AS SubReqId, SubRequest.parent AS SubReqParentId, SubRequest.Status,
                   username, machine, svcClassName, Request.id AS ReqId, Request.type AS ReqType
              FROM SubRequest,
                    (SELECT id, username, machine, svcClassName, 'Get' AS type FROM StageGetRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PGet' AS type FROM StagePrepareToGetRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'Put' AS type FROM StagePutRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PPut' AS type FROM StagePrepareToPutRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'Upd' AS type FROM StageUpdateRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PUpd' AS type FROM StagePrepareToUpdateRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'Repack' AS type FROM StageRepackRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'GetNext' AS type FROM StageGetNextRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'UpdNext' AS type FROM StageUpdateNextRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PutDone' AS type FROM StagePutDoneRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'DCRepl' AS type FROM StageDiskCopyReplicaRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'SetFileGCWeight' AS type FROM SetFileGCWeight) Request
             WHERE castorfile = getCF(ref)
               AND Request.id = SubRequest.request) LOOP
     PIPE ROW(d);
  END LOOP;
END;
/
/*******************************************************************
 * @(#)RCSfile: oracleMonitoring.sql,v  Revision: 1.8  Date: 2009/07/05 13:46:14  Author: waldron 
 * PL/SQL code for stager monitoring
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* CastorMon Package Specification */
CREATE OR REPLACE PACKAGE CastorMon AS

  /**
   * This procedure generates statistics on the status of all diskcopies. The
   * results are grouped by DiskServer, MountPoint and DiskCopy status.
   *
   * @param interval The frequency at which the data is refreshed.
   */
  PROCEDURE diskCopyStats(interval IN NUMBER);

  /**
   * This procedure generates statistics on files which are waiting to be
   * migrated to tape.
   * @param interval The frequency at which the data is refreshed.
   */
  PROCEDURE waitTapeMigrationStats(interval IN NUMBER);

  /**
   * This procedure generates statistics on files which are waiting to be
   * recalled from tape.
   * @param interval The frequency at which the data is refreshed.
   */
  PROCEDURE waitTapeRecallStats(interval IN NUMBER);

END CastorMon;
/


/* CastorMon Package Body */
CREATE OR REPLACE PACKAGE BODY CastorMon AS

  /**
   * PL/SQL method implementing diskCopyStats
   * See the castorMon package specification for documentation.
   */
  PROCEDURE diskCopyStats(interval IN NUMBER) AS
  BEGIN
    -- Truncate the MonDiskCopyStats table
    EXECUTE IMMEDIATE 'DELETE FROM MonDiskCopyStats';
    -- Populate the MonDiskCopyStats table
    INSERT INTO MonDiskCopyStats
      (timestamp, interval, diskServer, mountPoint, dsStatus, fsStatus,
       available, status, totalFileSize, nbFiles)
      -- Gather data
      SELECT sysdate timestamp, interval, a.name diskServer, a.mountPoint,
             a.dsStatus, a.fsStatus, a.available, a.statusName status,
             nvl(b.totalFileSize, 0) totalFileSize, nvl(b.nbFiles, 0) nbFiles
        FROM (
          -- Produce a matrix of all possible diskservers, filesystems and
          -- diskcopy states.
          SELECT DiskServer.name, FileSystem.mountPoint, FileSystem.id,
                 (SELECT statusName FROM ObjStatus
                   WHERE object = 'DiskServer'
                     AND field = 'status'
                     AND statusCode = DiskServer.status) dsStatus,
                 (SELECT statusName FROM ObjStatus
                   WHERE object = 'FileSystem'
                     AND field = 'status'
                     AND statusCode = FileSystem.status) fsStatus,
                 decode(DiskServer.status, 2, 'N',
                   decode(FileSystem.status, 2, 'N', 'Y')) available,
                 ObjStatus.statusName
            FROM DiskServer, FileSystem, ObjStatus
           WHERE FileSystem.diskServer = DiskServer.id
             AND ObjStatus.object(+) = 'DiskCopy'
             AND ObjStatus.field = 'status'
             AND ObjStatus.statusCode IN (0, 4, 5, 6, 7, 9, 10, 11)) a
        -- Attach the aggregation information for all filesystems to the results
        -- extracted above.
        LEFT JOIN (
          SELECT DiskCopy.fileSystem, ObjStatus.statusName,
                 sum(DiskCopy.diskCopySize) totalFileSize, count(*) nbFiles
            FROM DiskCopy, ObjStatus
           WHERE DiskCopy.status = ObjStatus.statusCode
             AND ObjStatus.object = 'DiskCopy'
             AND ObjStatus.field = 'status'
             AND DiskCopy.status IN (0, 4, 5, 6, 7, 9, 10, 11)
           GROUP BY DiskCopy.fileSystem, ObjStatus.statusName) b
          ON (a.id = b.fileSystem AND a.statusName = b.statusName)
       ORDER BY a.name, a.mountPoint, a.statusName;
  END diskCopyStats;


  /**
   * PL/SQL method implementing waitTapeMigrationStats
   * See the castorMon package specification for documentation.
   */
  PROCEDURE waitTapeMigrationStats(interval IN NUMBER) AS
  BEGIN
    -- Truncate the MonWaitTapeMigrationStats table
    EXECUTE IMMEDIATE 'DELETE FROM MonWaitTapeMigrationStats';
    -- Populate the MonWaitTapeMigrationStats table
    INSERT INTO MonWaitTapeMigrationStats
      (timestamp, interval, svcClass, status, minWaitTime, maxWaitTime,
       avgWaitTime, minFileSize, maxFileSize, avgFileSize, bin_LT_1,
       bin_1_To_6, bin_6_To_12, bin_12_To_24, bin_24_To_48, bin_GT_48,
       totalFileSize, nbFiles)
      -- Gather data
      SELECT sysdate timestamp, interval, b.svcClass, nvl(b.status, '-') status,
             -- File age statistics
             round(nvl(min(a.waitTime), 0), 0) minWaitTime,
             round(nvl(max(a.waitTime), 0), 0) maxWaitTime,
             round(nvl(avg(a.waitTime), 0), 0) avgWaitTime,
             -- File size statistics
             round(nvl(min(a.diskCopySize), 0), 0) minFileSize,
             round(nvl(max(a.diskCopySize), 0), 0) maxFileSize,
             round(nvl(avg(a.diskCopySize), 0), 0) avgFileSize,
             -- Wait time stats
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 1     AND 3600
                      THEN 1 ELSE 0 END) BIN_LT_1,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 3600  AND 21600
                      THEN 1 ELSE 0 END) BIN_1_TO_6,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 21600 AND 43200
                      THEN 1 ELSE 0 END) BIN_6_TO_12,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 43200 AND 86400
                      THEN 1 ELSE 0 END) BIN_12_TO_24,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 86400 AND 172800
                      THEN 1 ELSE 0 END) BIN_24_TO_48,
             sum(CASE WHEN nvl(a.waitTime, 0) > 172800
                      THEN 1 ELSE 0 END) BIN_GT_48,
             -- Summary values
             nvl(sum(a.diskCopySize), 0) totalFileSize, nvl(sum(a.found), 0) nbFiles
        FROM (
          -- Determine the service class of all tapecopies and their associated
          -- status.
          SELECT /*+ USE_NL(TapeCopy DiskCopy CastorFile) */ 
                 SvcClass.name svcClass,
                 decode(sign(TapeCopy.status - 2), -1, 'PENDING',
                   decode(TapeCopy.status, 6, 'FAILED', 'SELECTED')) status,
                 (getTime() - DiskCopy.creationTime) waitTime,
                 DiskCopy.diskCopySize, 1 found
            FROM DiskCopy, CastorFile, TapeCopy, SvcClass 
           WHERE DiskCopy.castorFile = CastorFile.id
             AND CastorFile.id = TapeCopy.castorFile
             AND CastorFile.svcClass = SvcClass.id
             AND decode(DiskCopy.status, 10, DiskCopy.status, NULL) = 10  -- CANBEMIGR
             AND TapeCopy.status IN (0, 1, 2, 3, 6) -- CREATED, TOBEMIGRATED, WAITINSTREAMS,
                                                    -- SELECTED, FAILED
        ) a
        -- Attach a list of all service classes and possible states (PENDING,
        -- SELECTED) to the results above.
        RIGHT JOIN (
          SELECT SvcClass.name svcClass, a.status
            FROM SvcClass,
             (SELECT 'PENDING'  status FROM Dual UNION ALL
              SELECT 'SELECTED' status FROM Dual UNION ALL
              SELECT 'FAILED'   status FROM Dual) a) b
           ON (a.svcClass = b.svcClass AND a.status = b.status)
       GROUP BY GROUPING SETS (b.svcClass, b.status), (b.svcClass)
       ORDER BY b.svcClass, b.status;
  END waitTapeMigrationStats;


  /**
   * PL/SQL method implementing waitTapeRecallStats
   * See the castorMon package specification for documentation.
   */
  PROCEDURE waitTapeRecallStats(interval IN NUMBER) AS
  BEGIN
    -- Truncate the MonWaitTapeRecallStats table
    EXECUTE IMMEDIATE 'DELETE FROM MonWaitTapeRecallStats';
    -- Populate the MonWaitTapeRecallStats table
    INSERT INTO MonWaitTapeRecallStats
      (timestamp, interval, svcClass, minWaitTime, maxWaitTime, avgWaitTime,
       minFileSize, maxFileSize, avgFileSize, bin_LT_1, bin_1_To_6, bin_6_To_12,
       bin_12_To_24, bin_24_To_48, bin_GT_48, totalFileSize, nbFiles)
      -- Gather data
      SELECT sysdate timestamp, interval, SvcClass.name svcClass,
             -- File age statistics
             round(nvl(min(a.waitTime), 0), 0) minWaitTime,
             round(nvl(max(a.waitTime), 0), 0) maxWaitTime,
             round(nvl(avg(a.waitTime), 0), 0) avgWaitTime,
             -- File size statistics
             round(nvl(min(a.fileSize), 0), 0) minFileSize,
             round(nvl(max(a.fileSize), 0), 0) maxFileSize,
             round(nvl(avg(a.fileSize), 0), 0) avgFileSize,
             -- Wait time stats
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 1     AND 3600
                      THEN 1 ELSE 0 END) BIN_LT_1,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 3600  AND 21600
                      THEN 1 ELSE 0 END) BIN_1_TO_6,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 21600 AND 43200
                      THEN 1 ELSE 0 END) BIN_6_TO_12,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 43200 AND 86400
                      THEN 1 ELSE 0 END) BIN_12_TO_24,
             sum(CASE WHEN nvl(a.waitTime, 0) BETWEEN 86400 AND 172800
                      THEN 1 ELSE 0 END) BIN_24_TO_48,
             sum(CASE WHEN nvl(a.waitTime, 0) > 172800
                      THEN 1 ELSE 0 END) BIN_GT_48,
             -- Summary values
             nvl(sum(a.fileSize), 0) totalFileSize, nvl(sum(a.found), 0) nbFiles
        FROM (
          -- Determine the list of ongoing a. We need to join with the
          -- SubRequest and Request tables here to work out the service class
          -- as the DiskCopy filesystem pointer is 0 until the file is
          -- successfully recalled.
          SELECT Request.svcClassName svcClass, CastorFile.fileSize,
                 (getTime() - DiskCopy.creationTime) waitTime, 1 found
            FROM DiskCopy, SubRequest, CastorFile,
              (SELECT id, svcClassName FROM StageGetRequest UNION ALL
               SELECT id, svcClassName FROM StagePrepareToGetRequest UNION ALL
               SELECT id, svcClassName FROM StageUpdateRequest UNION ALL
               SELECT id, svcClassName FROM StageRepackRequest) Request
           WHERE DiskCopy.id = SubRequest.diskCopy
             AND DiskCopy.status = 2  -- DISKCOPY_WAITTAPERECALL
             AND DiskCopy.castorFile = CastorFile.id
             AND SubRequest.status = 4  -- SUBREQUEST_WAITTAPERECALL
             AND SubRequest.parent = 0
             AND SubRequest.request = Request.id
        ) a
        -- Attach a list of all service classes
        RIGHT JOIN SvcClass
           ON SvcClass.name = a.svcClass
        GROUP BY SvcClass.name
        ORDER BY SvcClass.name;
  END waitTapeRecallStats;

END CastorMon;
/


/* Database jobs */
BEGIN
  -- Drop all monitoring jobs
  FOR j IN (SELECT job_name FROM user_scheduler_jobs
             WHERE job_name LIKE ('MONITORINGJOB_%'))
  LOOP
    DBMS_SCHEDULER.DROP_JOB(j.job_name, TRUE);
  END LOOP;

  -- Recreate monitoring jobs
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'monitoringJob_60mins',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'DECLARE
                            interval NUMBER := 3600;
                          BEGIN
                            castorMon.diskCopyStats(interval);
                            castorMon.waitTapeMigrationStats(interval);
                            castorMon.waitTapeRecallStats(interval);
                          END;',
      JOB_CLASS       => 'CASTOR_MON_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=60',
      ENABLED         => TRUE,
      COMMENTS        => 'Generation of monitoring information');
END;
/

