/*******************************************************************
 *
 * @(#)$RCSfile: oracleCommon.schema.sql,v $ $Revision: 1.19 $ $Date: 2009/08/10 15:30:12 $ $Author: itglp $
 *
 * This file contains all schema definitions which are not generated automatically.
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* SQL statement to populate the intial schema version */
UPDATE UpgradeLog SET schemaVersion = '2_1_12_0';

/* Sequence for indices */
CREATE SEQUENCE ids_seq CACHE 300;

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
   svcHandler VARCHAR2(2048) CONSTRAINT NN_SubRequest_SvcHandler NOT NULL,
   reqType INTEGER CONSTRAINT NN_SubRequest_reqType NOT NULL
  )
  PCTFREE 50 PCTUSED 40 INITRANS 50
  ENABLE ROW MOVEMENT
  PARTITION BY LIST (STATUS)
   (
    PARTITION P_STATUS_0_1_2 VALUES (0, 1, 2),      -- *START
    PARTITION P_STATUS_3     VALUES (3),
    PARTITION P_STATUS_4     VALUES (4),
    PARTITION P_STATUS_5     VALUES (5),
    PARTITION P_STATUS_6     VALUES (6),
    PARTITION P_STATUS_7     VALUES (7),
    PARTITION P_STATUS_8     VALUES (8),
    PARTITION P_STATUS_9_10  VALUES (9, 10),        -- FAILED_*
    PARTITION P_STATUS_11    VALUES (11),
    PARTITION P_STATUS_12    VALUES (12),
    PARTITION P_STATUS_13_14 VALUES (13, 14),       -- *SCHED
    PARTITION P_STATUS_OTHER VALUES (DEFAULT)
   );

/* SQL statements for constraints on the SubRequest table */
ALTER TABLE SubRequest
  ADD CONSTRAINT PK_SubRequest_Id PRIMARY KEY (ID);
CREATE INDEX I_SubRequest_RT_CT_ID ON SubRequest(svcHandler, creationTime, id) LOCAL
 (PARTITION P_STATUS_0_1_2,
  PARTITION P_STATUS_3,
  PARTITION P_STATUS_4,
  PARTITION P_STATUS_5,
  PARTITION P_STATUS_6,
  PARTITION P_STATUS_7,
  PARTITION P_STATUS_8,
  PARTITION P_STATUS_9_10,
  PARTITION P_STATUS_11,
  PARTITION P_STATUS_12,
  PARTITION P_STATUS_13_14,
  PARTITION P_STATUS_OTHER);

CREATE INDEX I_SubRequest_CT_ID ON SubRequest(creationTime, id) LOCAL
 (PARTITION P_STATUS_0_1_2,
  PARTITION P_STATUS_3,
  PARTITION P_STATUS_4,
  PARTITION P_STATUS_5,
  PARTITION P_STATUS_6,
  PARTITION P_STATUS_7,
  PARTITION P_STATUS_8,
  PARTITION P_STATUS_9_10,
  PARTITION P_STATUS_11,
  PARTITION P_STATUS_12,
  PARTITION P_STATUS_13_14,
  PARTITION P_STATUS_OTHER);

/* Redefinition of table TapeCopy to make it partitioned by status */
ALTER TABLE Stream2TapeCopy DROP CONSTRAINT FK_Stream2TapeCopy_C;
DROP TABLE TapeCopy;
CREATE TABLE TapeCopy 
  (
    copyNb NUMBER, errorCode NUMBER, nbRetry NUMBER, missingCopies NUMBER, 
    fseq NUMBER, tapeGatewayRequestId NUMBER, vid VARCHAR2(2048), 
    fileTransactionId NUMBER, id INTEGER CONSTRAINT PK_TapeCopy_Id PRIMARY KEY 
    /* This one was not generated -> */ CONSTRAINT NN_TapeCopy_Id NOT NULL, 
    castorFile INTEGER, status INTEGER
  ) 
    INITRANS 50 /* This one ported by hand as well -> */ PCTUSED 40 PCTFREE 50 ENABLE ROW MOVEMENT
    PARTITION BY LIST (STATUS)
  (
    PARTITION P_STATUS_0_1   VALUES (0, 1),
    PARTITION P_STATUS_OTHER VALUES (DEFAULT)
   );

/* Add index to allow fast lookup by VID (use for preventing 2 tape copies on the same tape.) */
CREATE INDEX I_TapeCopy_VID ON TapeCopy(VID);

/* This transaction id is the mean to track a migration, so it obviously needs to be unique */
ALTER TABLE TapeCopy ADD CONSTRAINT UN_TAPECOPY_FILETRID 
  UNIQUE (FileTransactionId) USING INDEX;

/* Create sequence for the File request IDs. */
CREATE SEQUENCE TG_FILETRID_SEQ START WITH 1 INCREMENT BY 1;
  
/* Recreate foreign key constraint between Stream2TapeCopy and TapeCopy */
ALTER TABLE Stream2TapeCopy
  ADD CONSTRAINT FK_Stream2TapeCopy_C FOREIGN KEY (Child) REFERENCES TapeCopy (id);

/* Indexes related to most used entities */
CREATE UNIQUE INDEX I_DiskServer_name ON DiskServer (name);

CREATE UNIQUE INDEX I_CastorFile_FileIdNsHost ON CastorFile (fileId, nsHost);
CREATE UNIQUE INDEX I_CastorFile_LastKnownFileName ON CastorFile (lastKnownFileName);
CREATE INDEX I_CastorFile_SvcClass ON CastorFile (svcClass);

CREATE INDEX I_DiskCopy_Castorfile ON DiskCopy (castorFile);
CREATE INDEX I_DiskCopy_FileSystem ON DiskCopy (fileSystem);
CREATE INDEX I_DiskCopy_Status ON DiskCopy (status);
CREATE INDEX I_DiskCopy_FS_Status_10 ON DiskCopy (fileSystem,decode(status,10,status,NULL));
CREATE INDEX I_DiskCopy_Status_9 ON DiskCopy (decode(status,9,status,NULL));
CREATE INDEX I_DiskCopy_FS_GCW ON DiskCopy (filesystem, status, gcweight, ID, castorFile);

CREATE INDEX I_TapeCopy_Castorfile ON TapeCopy (castorFile) LOCAL;
CREATE INDEX I_TapeCopy_Status ON TapeCopy (status) LOCAL;
CREATE INDEX I_TapeCopy_CF_Status_2 ON TapeCopy (castorFile,decode(status,2,status,NULL)) LOCAL;

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

/* Improve query execution in the checkFailJobsWhenNoSpace function */
CREATE INDEX I_StagePutRequest_SvcClass ON StagePutRequest (svcClass);

/* A primary key index for better scan of Stream2TapeCopy */
ALTER TABLE Stream2TapeCopy MODIFY
  (parent CONSTRAINT NN_Stream2TapeCopy_Parent NOT NULL,
   child  CONSTRAINT NN_Stream2TapeCopy_Child NOT NULL);

CREATE UNIQUE INDEX I_Stream2TapeCopy_PC ON Stream2TapeCopy (parent, child);

ALTER TABLE Stream2TapeCopy
  ADD CONSTRAINTS PK_Stream2TapeCopy_PC PRIMARY KEY (parent, child) USING INDEX;

/* Indexing GCFile by Request */
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

ALTER TABLE FileSystem MODIFY
  (status     CONSTRAINT NN_FileSystem_Status NOT NULL,
   diskServer CONSTRAINT NN_FileSystem_DiskServer NOT NULL,
   mountPoint CONSTRAINT NN_FileSystem_MountPoint NOT NULL);

ALTER TABLE FileSystem ADD CONSTRAINT UN_FileSystem_DSMountPoint
   UNIQUE (diskServer, mountPoint);

/* DiskServer constraints */
ALTER TABLE DiskServer MODIFY
  (status CONSTRAINT NN_DiskServer_Status NOT NULL,
   name CONSTRAINT NN_DiskServer_Name NOT NULL);

ALTER TABLE DiskServer ADD CONSTRAINT UN_DiskServer_Name UNIQUE (name);

/* An index to speed up queries in FileQueryRequest, FindRequestRequest, RequestQueryRequest */
CREATE INDEX I_QueryParameter_Query ON QueryParameter (query);

/* An index to speed the queries on Segments by copy */
CREATE INDEX I_Segment_Copy ON Segment (copy);

/* Constraint on FileClass name */
ALTER TABLE FileClass ADD CONSTRAINT UN_FileClass_Name UNIQUE (name);

/* Add unique constraint on tapes */
ALTER TABLE Tape ADD CONSTRAINT UN_Tape_VIDSideTpMode UNIQUE (VID, side, tpMode);

/* Add unique constraint on svcClass name */
ALTER TABLE SvcClass ADD CONSTRAINT UN_SvcClass_Name UNIQUE (name);

/* Custom type to handle int arrays */
CREATE OR REPLACE TYPE "numList" IS TABLE OF INTEGER;
/

/* Custom type to handle strings returned by pipelined functions */
CREATE OR REPLACE TYPE strListTable AS TABLE OF VARCHAR2(2048);
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

ALTER TABLE DiskCopy
  MODIFY (status CONSTRAINT NN_DiskCopy_Status NOT NULL);

/* CastorFile constraints */
ALTER TABLE CastorFile ADD CONSTRAINT FK_CastorFile_SvcClass
  FOREIGN KEY (svcClass) REFERENCES SvcClass (id)
  INITIALLY DEFERRED DEFERRABLE;

ALTER TABLE CastorFile ADD CONSTRAINT FK_CastorFile_FileClass
  FOREIGN KEY (fileClass) REFERENCES FileClass (id)
  INITIALLY DEFERRED DEFERRABLE;

ALTER TABLE CastorFile ADD CONSTRAINT UN_CastorFile_LKFileName UNIQUE (LastKnownFileName);

ALTER TABLE CastorFile MODIFY (LastKnownFileName CONSTRAINT NN_CastorFile_LKFileName NOT NULL);

/* Stream constraints */
ALTER TABLE Stream ADD CONSTRAINT FK_Stream_TapePool
  FOREIGN KEY (tapePool) REFERENCES TapePool (id);
ALTER TABLE Tape MODIFY(lastVdqmPingTime CONSTRAINT NN_Tape_lastVdqmPingTime NOT NULL);

/* DiskPool2SvcClass constraints */
ALTER TABLE DiskPool2SvcClass ADD CONSTRAINT PK_DiskPool2SvcClass_PC
  PRIMARY KEY (parent, child);

/* TapeGateway tables */
CREATE TABLE TapeGatewayRequest (accessMode NUMBER, startTime INTEGER, lastVdqmPingTime INTEGER, vdqmVolReqId NUMBER, nbRetry NUMBER, lastFseq NUMBER, id INTEGER CONSTRAINT PK_TapeGatewayRequest_Id PRIMARY KEY, streamMigration INTEGER, tapeRecall INTEGER, status INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;
CREATE TABLE TapeGatewaySubRequest (fseq NUMBER, id INTEGER CONSTRAINT PK_TapeGatewaySubRequest_Id PRIMARY KEY, tapecopy INTEGER, request INTEGER, diskcopy INTEGER) INITRANS 50 PCTFREE 50 ENABLE ROW MOVEMENT;
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 0, 'TO_BE_RESOLVED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 1, 'TO_BE_SENT_TO_VDQM');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 2, 'WAITING_TAPESERVER');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 3, 'ONGOING');

/* Index and Constraints for the tapegateway tables */
CREATE INDEX I_TGSubRequest_Request ON TapeGatewaySubRequest(request);
CREATE UNIQUE INDEX I_TGSubRequest_TapeCopy ON TapeGatewaySubRequest(tapeCopy);
CREATE UNIQUE INDEX I_TGRequest_Tape ON TapeGatewayRequest(tapeRecall);
CREATE UNIQUE INDEX I_TGRequest_Stream ON TapeGatewayRequest(streamMigration);
CREATE UNIQUE INDEX I_TGRequest_VdqmVolReqId ON TapeGatewayRequest(vdqmVolReqId);

ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT FK_TGSubRequest_TC FOREIGN KEY (tapeCopy) REFERENCES TapeCopy (id);
ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT FK_TGSubRequest_DC FOREIGN KEY (diskCopy) REFERENCES DiskCopy (id);
ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT FK_TGSubRequest_TGR FOREIGN KEY (request) REFERENCES TapeGatewayRequest(id);
ALTER TABLE TapeGatewayRequest ADD CONSTRAINT FK_TapeGatewayRequest_SM FOREIGN KEY (streamMigration) REFERENCES Stream (id);
ALTER TABLE TapeGatewayRequest ADD CONSTRAINT FK_TapeGatewayRequest_TR FOREIGN KEY (tapeRecall) REFERENCES Tape (id);
ALTER TABLE TapeGatewaySubRequest ADD CONSTRAINT UN_TGSubRequest_TR_DC UNIQUE (request, diskcopy);

/* Global temporary table to handle output of the filesDeletedProc procedure */
CREATE GLOBAL TEMPORARY TABLE FilesDeletedProcOutput
  (fileId NUMBER, nsHost VARCHAR2(2048))
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

/* Global temporary table to handle output of the jobFailed procedure
 * This table is deprecated and should go when the jobmanager and LSF are dropped.
 */
CREATE GLOBAL TEMPORARY TABLE JobFailedProcHelper
  (subReqId VARCHAR2(2048))
  ON COMMIT PRESERVE ROWS;

/* Global temporary table to handle output of the processBulkAbortForGet procedure */
CREATE GLOBAL TEMPORARY TABLE ProcessBulkAbortFileReqsHelper
  (srId NUMBER, cfId NUMBER, fileId NUMBER, nsHost VARCHAR2(2048), uuid VARCHAR(2048))
  ON COMMIT DELETE ROWS;

/* Global temporary table to handle output of the processBulkRequest procedure */
CREATE GLOBAL TEMPORARY TABLE ProcessBulkRequestHelper
  (fileId NUMBER, nsHost VARCHAR2(2048), errorCode NUMBER, errorMessage VARCHAR2(2048))
  ON COMMIT PRESERVE ROWS;

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

/* Global temporary table to handle output of the processBulkRequest procedure */
CREATE GLOBAL TEMPORARY TABLE getFileIdsForSrsHelper (rowno NUMBER, fileId NUMBER, nsHost VARCHAR(2048))
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

/* Create the AdminUsers table */
CREATE TABLE AdminUsers (euid NUMBER, egid NUMBER);
ALTER TABLE AdminUsers ADD CONSTRAINT UN_AdminUsers_euid_egid UNIQUE (euid, egid);
INSERT INTO AdminUsers VALUES (0, 0);   -- root/root, to be removed
INSERT INTO AdminUsers VALUES (-1, -1); -- internal requests

/* Prompt for stage:st account */
PROMPT Configuration of the admin part of the B/W list
UNDEF stageUid
ACCEPT stageUid NUMBER PROMPT 'Enter the stage user id: ';
UNDEF stageGid
ACCEPT stageGid NUMBER PROMPT 'Enter the st group id: ';
INSERT INTO AdminUsers VALUES (&stageUid, &stageGid);

/* Prompt for additional administrators */
PROMPT In order to define admins that will be exempt of B/W list checks,
PROMPT (e.g. c3 group at CERN), please give a space separated list of
PROMPT <userid>:<groupid> pairs. userid can be empty, meaning any user
PROMPT in the specified group.
UNDEF adminList
ACCEPT adminList CHAR PROMPT 'List of admins: ';

/* Define the service handlers for the appropriate sets of stage request objects */
UPDATE Type2Obj SET svcHandler = 'JobReqSvc' WHERE type IN (35, 40, 44);
UPDATE Type2Obj SET svcHandler = 'PrepReqSvc' WHERE type IN (36, 37, 38, 119);
UPDATE Type2Obj SET svcHandler = 'StageReqSvc' WHERE type IN (39, 42, 95);
UPDATE Type2Obj SET svcHandler = 'QueryReqSvc' WHERE type IN (33, 34, 41, 103, 131, 152, 155, 195);
UPDATE Type2Obj SET svcHandler = 'JobSvc' WHERE type IN (60, 64, 65, 67, 78, 79, 80, 93, 144, 147);
UPDATE Type2Obj SET svcHandler = 'GCSvc' WHERE type IN (73, 74, 83, 142, 149);
UPDATE Type2Obj SET svcHandler = 'BulkStageReqSvc' WHERE type IN (50);

/* Set default values for the StageDiskCopyReplicaRequest table */
ALTER TABLE StageDiskCopyReplicaRequest MODIFY flags DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY euid DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY egid DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY mask DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY pid DEFAULT 0;
ALTER TABLE StageDiskCopyReplicaRequest MODIFY machine DEFAULT 'stager';

/* Indexing StageDiskCopyReplicaRequest by source and destination diskcopy id */
CREATE INDEX I_StageDiskCopyReplic_SourceDC 
  ON StageDiskCopyReplicaRequest (sourceDiskCopy);
CREATE INDEX I_StageDiskCopyReplic_DestDC 
  ON StageDiskCopyReplicaRequest (destDiskCopy);

/* Define a table for some configuration key-value pairs and populate it */
CREATE TABLE CastorConfig
  (class VARCHAR2(2048) CONSTRAINT NN_CastorConfig_Class NOT NULL, 
   key VARCHAR2(2048) CONSTRAINT NN_CastorConfig_Key NOT NULL, 
   value VARCHAR2(2048) CONSTRAINT NN_CastorConfig_Value NOT NULL, 
   description VARCHAR2(2048));

ALTER TABLE CastorConfig ADD CONSTRAINT UN_CastorConfig_class_key UNIQUE (class, key);

/* Prompt for the value of the general/instance options */
UNDEF instanceName
ACCEPT instanceName DEFAULT castorstager PROMPT 'Enter the name of the castor instance: (default: castor_stager, example: castoratlas) '
SET VER OFF

INSERT INTO CastorConfig
  VALUES ('general', 'instance', '&instanceName', 'Name of this Castor instance');

/* Prompt for the value of the stager/nsHost option */
UNDEF stagerNsHost
ACCEPT stagerNsHost DEFAULT undefined PROMPT 'Enter the name of the stager/nsHost: (default: undefined, example: castorns) '

INSERT INTO CastorConfig
  VALUES ('stager', 'nsHost', '&stagerNsHost', 'The name of the name server host to set in the CastorFile table overriding the CNS/HOST option defined in castor.conf');


INSERT INTO CastorConfig
  VALUES ('general', 'owner', sys_context('USERENV', 'CURRENT_USER'), 'The database owner of the schema');
INSERT INTO CastorConfig
  VALUES ('cleaning', 'terminatedRequestsTimeout', '120', 'Maximum timeout for successful and failed requests in hours');
INSERT INTO CastorConfig
  VALUES ('cleaning', 'outOfDateStageOutDCsTimeout', '72', 'Timeout for STAGEOUT diskCopies in hours');
INSERT INTO CastorConfig
  VALUES ('cleaning', 'failedDCsTimeout', '72', 'Timeout for failed diskCopies in hours');
INSERT INTO CastorConfig 
  VALUES ('tape', 'interfaceDaemon', 'rtcpclientd', 'The name of the daemon used to interface to the tape system');
INSERT INTO CastorConfig
  VALUES ('RmMaster', 'NoLSFMode', 'no', 'Whether we are running in NoLSF mode');
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


/************/
/* Aborting */
/************/

CREATE TABLE TransfersToAbort (uuid VARCHAR2(2048)
  CONSTRAINT NN_TransfersToAbort_Uuid NOT NULL);

/*******************************/
/* running job synchronization */
/*******************************/

CREATE GLOBAL TEMPORARY TABLE SyncRunningTransfersHelper(subreqId VARCHAR2(2048)) ON COMMIT DELETE ROWS;
CREATE GLOBAL TEMPORARY TABLE SyncRunningTransfersHelper2
(subreqId VARCHAR2(2048), reqId VARCHAR2(2048),
 fileid NUMBER, nsHost VARCHAR2(2048),
 errorCode NUMBER, errorMsg VARCHAR2(2048))
ON COMMIT PRESERVE ROWS;

/***************************************************/
/* rmMaster main lock, only used to elect a master */
/***************************************************/

CREATE TABLE RmMasterLock (unused NUMBER);
