/* This file contains SQL code that is not generated automatically */
/* and is inserted at the beginning of the generated code          */

/* A small table used to cross check code and DB versions */
DROP TABLE CastorVersion;

/* Sequence for ids */
DROP SEQUENCE ids_seq;

/* indexes for optimizing queries */
DROP INDEX I_Id2Type_typeId;
DROP INDEX I_DiskServer_name;
DROP INDEX I_CastorFile_fileIdNsHost;
DROP INDEX I_DiskCopy_Castorfile;
DROP INDEX I_DiskCopy_FileSystem;
DROP INDEX I_TapeCopy_Castorfile;
DROP INDEX I_SubRequest_Castorfile;
DROP INDEX I_FileSystem_DiskPool;
DROP INDEX I_SubRequest_DiskCopy;
DROP INDEX I_SubRequest_Request;
DROP INDEX I_SubRequest_Status;
DROP INDEX I_SubRequest_Status7;
DROP INDEX I_QueryParameter_Query;
DROP INDEX I_NbTapeCopiesInFS_FSStream;
DROP INDEX I_FileSystem_Rate;

/* SQL statements for object types */
DROP TABLE Id2Type;

/* SQL statement for requests status */
DROP TABLE newRequests;

/* support tables - check oracleTrailer_create.sql */
DROP TABLE NbTapeCopiesInFS;
DROP TABLE LockTable;


/* SQL statements for type RepackSubRequest */
DROP TABLE RepackSubRequest;

/* SQL statements for type RepackSegment */
DROP TABLE RepackSegment;

/* SQL statements for type RepackRequest */
DROP TABLE RepackRequest;

DROP FUNCTION getTime;
DROP FUNCTION FileSystemRate;
DROP PROCEDURE makeSubRequestWait;
DROP PROCEDURE archiveSubReq;
DROP PROCEDURE anyTapeCopyForStream;
DROP PROCEDURE updateFsFileOpened;
DROP PROCEDURE updateFsFileClosed;
DROP PROCEDURE updateFileSystemForJob;
DROP PROCEDURE bestTapeCopyForStream;
DROP PROCEDURE bestFileSystemForSegment;
DROP PROCEDURE fileRecalled;
DROP PROCEDURE fileRecallFailed;
DROP PACKAGE castor;
DROP TYPE "numList";
DROP PROCEDURE isSubRequestToSchedule;
DROP PROCEDURE buildPathFromFileId;
DROP PROCEDURE getUpdateStart;
DROP PROCEDURE putStart;
DROP PROCEDURE putDoneStartProc;
DROP PROCEDURE updateAndCheckSubRequest;
DROP PROCEDURE disk2DiskCopyDone;
DROP PROCEDURE recreateCastorFile;
DROP PROCEDURE putDoneFunc;
DROP PROCEDURE prepareForMigration;
DROP PROCEDURE selectCastorFile;
DROP PROCEDURE resetStream;
DROP PROCEDURE bestFileSystemForJob;
DROP PROCEDURE anySegmentsForTape;
DROP PROCEDURE segmentsForTape;
DROP PROCEDURE selectFiles2Delete;
DROP PROCEDURE filesDeletedProc;
DROP PROCEDURE filesDeletionFailedProc;
DROP PROCEDURE getUpdateDoneProc;
DROP PROCEDURE getUpdateFailedProc;
DROP PROCEDURE putFailedProc;
DROP PROCEDURE failedSegments;
DROP PROCEDURE stageRelease;
DROP PROCEDURE stageRm;
DROP PACKAGE castorGC;
DROP FUNCTION defaultGCPolicy;
DROP FUNCTION nopinGCPolicy;
DROP PROCEDURE garbageCollectFS;
DROP PROCEDURE internalStageQuery;
DROP PROCEDURE fileIdStageQuery;
DROP PROCEDURE reqIdStageQuery;
DROP PROCEDURE userTagStageQuery;
