/******************************************************************************
 *              stager_2.1.10-0_to_2.1.11-0.sql
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * This script upgrades a CASTOR v2.1.10-0 STAGER database to v2.1.11-0
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors */
WHENEVER SQLERROR EXIT FAILURE
BEGIN
  -- If we've encountered an error rollback any previously non committed
  -- operations. This prevents the UPDATE of the UpgradeLog from committing
  -- inconsistent data to the database.
  ROLLBACK;
  UPDATE UpgradeLog
     SET failureCount = failureCount + 1
   WHERE schemaVersion = '2_1_10_0'
     AND release = '2_1_11_0'
     AND state != 'COMPLETE';
  COMMIT;
END;
/

/* Verify that the script is running against the correct schema and version */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM CastorVersion
   WHERE schemaName = 'STAGER'
     AND release LIKE '2_1_10_0%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts for the STAGER before this one.');
END;
/

INSERT INTO UpgradeLog (schemaVersion, release, type)
VALUES ('2_1_11_0', '2_1_11_0', 'NON TRANSPARENT');
COMMIT;

/* Job management */
BEGIN
  FOR a IN (SELECT * FROM user_scheduler_jobs)
  LOOP
    -- Stop any running jobs
    IF a.state = 'RUNNING' THEN
      dbms_scheduler.stop_job(a.job_name, force=>TRUE);
    END IF;
    -- Schedule the start date of the job to 15 minutes from now. This
    -- basically pauses the job for 15 minutes so that the upgrade can
    -- go through as quickly as possible.
    dbms_scheduler.set_attribute(a.job_name, 'START_DATE', SYSDATE + 15/1440);
  END LOOP;
END;
/

/* Schema changes go here */
/**************************/

/* Bug: #78826 - Invalidate DiskCopies with a NULL status */
DECLARE
  cfId NUMBER;
BEGIN
  FOR a IN (SELECT id, castorFile FROM DiskCopy WHERE status IS NULL)
  LOOP
    BEGIN
      -- Lock the CastorFile
      SELECT id INTO cfId FROM CastorFile
       WHERE id = a.castorFile FOR UPDATE;
      -- Invalidate the DiskCopy
      UPDATE DiskCopy SET status = 7
       WHERE id = a.id;
      COMMIT;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      NULL;  -- Do nothing
    END;
  END LOOP;
END;
/

/* Bug: #78826 - Race condition between stageRm and disk2DiskCopyDone can result
 * in NULL DiskCopy states.
 */
DECLARE
  constraintName VARCHAR2(30);
BEGIN
  -- Check to see if the constraint already exists
  SELECT constraint_name INTO constraintName
    FROM user_constraints
   WHERE constraint_name = 'NN_DISKCOPY_STATUS';
EXCEPTION WHEN NO_DATA_FOUND THEN
  EXECUTE IMMEDIATE 'ALTER TABLE DiskCopy
    MODIFY (status CONSTRAINT NN_DiskCopy_Status NOT NULL)';
END;
/

/* Bug: #78440 - lastknownfilename column should be unique and not null */
UPDATE CastorFile SET lastKnownFileName = TO_CHAR(id)
 WHERE lastKnownFileName IN
  (SELECT lastKnownFileName FROM
     (SELECT lastKnownFileName, count(*)
        FROM CastorFile
       GROUP BY lastKnownFileName
      HAVING count(*) > 1));
UPDATE CastorFile SET lastKnownFileName = TO_CHAR(id)
 WHERE lastKnownFileName IS NULL;

DROP INDEX I_CastorFile_LastKnownFileName;
CREATE UNIQUE INDEX I_CastorFile_LastKnownFileName ON CastorFile (lastKnownFileName);

ALTER TABLE CastorFile 
  ADD CONSTRAINT UN_CastorFile_LKFileName UNIQUE (LastKnownFileName);
ALTER TABLE CastorFile 
  MODIFY (LastKnownFileName CONSTRAINT NN_CastorFile_LKFileName NOT NULL);

/* Bug: #80011 CM: Make use of the new Cns_openx API in the stager */
ALTER TABLE FileClass ADD (classId NUMBER);

/* New scheduler changes */

/* Global temporary table to handle output of the processBulkRequest
 * procedure
 */
CREATE GLOBAL TEMPORARY TABLE getFileIdsForSrsHelper
  (rowno NUMBER, fileId NUMBER, nsHost VARCHAR(2048))
  ON COMMIT DELETE ROWS;

/* Global temporary table to handle output of the processBulkAbortForGet
 * procedure
 */
DROP TABLE ProcessBulkAbortFileReqsHelper;
CREATE GLOBAL TEMPORARY TABLE ProcessBulkAbortFileReqsHelper
  (srId NUMBER, cfId NUMBER, fileId NUMBER, nsHost VARCHAR2(2048), uuid VARCHAR(2048))
  ON COMMIT DELETE ROWS;

/* DBMS_ALERT trigger */
CREATE OR REPLACE TRIGGER tr_SubRequest_informSchedReady
AFTER UPDATE OF status ON SubRequest
FOR EACH ROW WHEN (new.status = 13) -- SUBREQUEST_READYFORSCHED
BEGIN 
  DBMS_ALERT.SIGNAL('transferReadyToSchedule', ''); 
END;
/

CREATE TABLE RmMasterLock (unused NUMBER);

/* Global temporary table to handle running job synchronization */
CREATE GLOBAL TEMPORARY TABLE SyncRunningTransfersHelper
  (subreqId VARCHAR2(2048))
ON COMMIT DELETE ROWS;

CREATE GLOBAL TEMPORARY TABLE SyncRunningTransfersHelper2
  (subreqId VARCHAR2(2048), reqId VARCHAR2(2048),
   fileid NUMBER, nsHost VARCHAR2(2048),
   errorCode NUMBER, errorMsg VARCHAR2(2048))
ON COMMIT PRESERVE ROWS;

/* Add new entry to CastorConfig table */
INSERT INTO CastorConfig
  VALUES ('RmMaster', 'NoLSFMode', 'no', 'Whether we are running in NoLSF mode');


/* TapeGateway changes */

ALTER TABLE Stream ADD (vdqmVolReqId NUMBER);
ALTER TABLE Stream ADD (tapeGatewayRequestId NUMBER);

ALTER TABLE Tape ADD (startTime NUMBER);
ALTER TABLE Tape ADD (lastVdqmPingTime NUMBER CONSTRAINT NN_Tape_lastVdqmPingTime NOT NULL);
ALTER TABLE Tape ADD (vdqmVolReqId NUMBER);
ALTER TABLE Tape ADD (lastFseq NUMBER);
ALTER TABLE Tape ADD (tapeGatewayRequestId NUMBER);

ALTER TABLE TapeCopy ADD (fseq NUMBER);
ALTER TABLE TapeCopy ADD (tapeGatewayRequestId NUMBER);
ALTER TABLE TapeCopy ADD (vid VARCHAR2(2048));
ALTER TABLE TapeCopy ADD (fileTransactionId NUMBER);

/* Add index to allow fast lookup by VID (use for preventing 2 tape copies on
 * the same tape.)
 */
CREATE INDEX I_TapeCopy_VID ON TapeCopy(VID);

/* This transaction id is the mean to track a migration, so it obviously needs
 * to be unique
 */
ALTER TABLE TapeCopy ADD CONSTRAINT UN_TAPECOPY_FILETRID 
  UNIQUE (FileTransactionId) USING INDEX;

/* Create sequence for the File request IDs. */
CREATE SEQUENCE TG_FILETRID_SEQ START WITH 1 INCREMENT BY 1;

/* Trigger ensuring validity of VID in state transitions */
CREATE OR REPLACE TRIGGER TR_TapeCopy_VID
BEFORE INSERT OR UPDATE OF Status ON TapeCopy
FOR EACH ROW
BEGIN
  /* Enforce the state integrity of VID in state transitions */
  
  /* rtcpclientd is given full exception, no check */
  IF rtcpclientdIsRunning THEN RETURN; END IF;
  
  CASE :new.status
    WHEN  tconst.TAPECOPY_SELECTED THEN
      /* The VID MUST be defined when the tapecopy gets selected */
      IF :new.VID IS NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to TAPECOPY_SELECTED State without a VID (TC.ID: '||
          :new.ID||' VID:'|| :old.VID||'=>'||:new.VID||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
    WHEN tconst.TAPECOPY_STAGED THEN
       /* The VID MUST be defined when the tapecopy goes to staged */
       IF :new.VID IS NULL THEN
         RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to TAPECOPY_STAGED State without a VID (TC.ID: '||
          :new.ID||' VID:'|| :old.VID||'=>'||:new.VID||' Status:'||:old.status||'=>'||:new.status||')');
       END IF;
       /* The VID MUST remain the same when going to staged */
       IF :new.VID != :old.VID THEN
         RAISE_APPLICATION_ERROR(-20119,
           'Moving to STAGED State without carrying the VID over');
       END IF;
    ELSE
      /* In all other cases, VID should be NULL */
      IF :new.VID IS NOT NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to TapeCopy state where VID makes no sense, yet VID!=NULL (TC.ID: '||
          :new.ID||' VID:'|| :old.VID||'=>'||:new.VID||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
  END CASE;
END;
/

/* Remove unnecessary procedure */
DROP PROCEDURE TG_StartRepackMigration;


/* Abort support */
CREATE TABLE TransfersToAbort (uuid VARCHAR2(2048)
  CONSTRAINT NN_TransfersToAbort_Uuid NOT NULL);


/* Reset the contents of the Type2Obj table */
TRUNCATE TABLE Type2Obj;
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
INSERT INTO Type2Obj (type, object) VALUES (17, 'Request');
INSERT INTO Type2Obj (type, object) VALUES (18, 'Segment');
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
INSERT INTO Type2Obj (type, object) VALUES (50, 'StageAbortRequest');
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
INSERT INTO Type2Obj (type, object) VALUES (90, 'TapeDriveDedication');
INSERT INTO Type2Obj (type, object) VALUES (91, 'TapeAccessSpecification');
INSERT INTO Type2Obj (type, object) VALUES (92, 'TapeDriveCompatibility');
INSERT INTO Type2Obj (type, object) VALUES (95, 'SetFileGCWeight');
INSERT INTO Type2Obj (type, object) VALUES (96, 'RepackRequest');
INSERT INTO Type2Obj (type, object) VALUES (97, 'RepackSubRequest');
INSERT INTO Type2Obj (type, object) VALUES (98, 'RepackSegment');
INSERT INTO Type2Obj (type, object) VALUES (99, 'RepackAck');
INSERT INTO Type2Obj (type, object) VALUES (101, 'DiskServerDescription');
INSERT INTO Type2Obj (type, object) VALUES (102, 'FileSystemDescription');
INSERT INTO Type2Obj (type, object) VALUES (103, 'DiskPoolQueryOld');
INSERT INTO Type2Obj (type, object) VALUES (104, 'EndResponse');
INSERT INTO Type2Obj (type, object) VALUES (105, 'FileResponse');
INSERT INTO Type2Obj (type, object) VALUES (106, 'StringResponse');
INSERT INTO Type2Obj (type, object) VALUES (107, 'Response');
INSERT INTO Type2Obj (type, object) VALUES (108, 'IOResponse');
INSERT INTO Type2Obj (type, object) VALUES (109, 'AbortResponse');
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
INSERT INTO Type2Obj (type, object) VALUES (186, 'RecallPolicyElement');
INSERT INTO Type2Obj (type, object) VALUES (187, 'MigrationPolicyElement');
INSERT INTO Type2Obj (type, object) VALUES (188, 'StreamPolicyElement');
INSERT INTO Type2Obj (type, object) VALUES (189, 'RetryPolicyElement');
INSERT INTO Type2Obj (type, object) VALUES (190, 'VdqmTapeGatewayRequest');
INSERT INTO Type2Obj (type, object) VALUES (191, 'StageQueryResult');
INSERT INTO Type2Obj (type, object) VALUES (192, 'NsFileId');
INSERT INTO Type2Obj (type, object) VALUES (193, 'BulkRequestResult');
INSERT INTO Type2Obj (type, object) VALUES (194, 'FileResult');
INSERT INTO Type2Obj (type, object) VALUES (195, 'DiskPoolQuery');

UPDATE Type2Obj SET svcHandler = 'JobReqSvc' WHERE type IN (35, 40, 44);
UPDATE Type2Obj SET svcHandler = 'PrepReqSvc' WHERE type IN (36, 37, 38, 119);
UPDATE Type2Obj SET svcHandler = 'StageReqSvc' WHERE type IN (39, 42, 95);
UPDATE Type2Obj SET svcHandler = 'QueryReqSvc' WHERE type IN (33, 34, 41, 103, 131, 152, 155, 195);
UPDATE Type2Obj SET svcHandler = 'JobSvc' WHERE type IN (60, 64, 65, 67, 78, 79, 80, 93, 144, 147);
UPDATE Type2Obj SET svcHandler = 'GCSvc' WHERE type IN (73, 74, 83, 142, 149);
UPDATE Type2Obj SET svcHandler = 'BulkStageReqSvc' WHERE type IN (50);

/* Reset the contents of the ObjStatus table */
TRUNCATE TABLE ObjStatus;
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 0, 'REQUESTQUERYTYPE_FILENAME');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 1, 'REQUESTQUERYTYPE_REQID');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 2, 'REQUESTQUERYTYPE_USERTAG');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 3, 'REQUESTQUERYTYPE_FILEID');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 4, 'REQUESTQUERYTYPE_REQID_GETNEXT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 5, 'REQUESTQUERYTYPE_USERTAG_GETNEXT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('QueryParameter', 'queryType', 6, 'REQUESTQUERYTYPE_FILENAME_ALLSC');
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
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 0, 'TAPE_UNUSED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 1, 'TAPE_PENDING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 2, 'TAPE_WAITDRIVE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 3, 'TAPE_WAITMOUNT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 4, 'TAPE_MOUNTED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 5, 'TAPE_FINISHED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 6, 'TAPE_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 7, 'TAPE_UNKNOWN');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 8, 'TAPE_WAITPOLICY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Tape', 'status', 9, 'TAPE_ATTACHEDTOSTREAM');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 0, 'SEGMENT_UNPROCESSED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 5, 'SEGMENT_FILECOPIED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 6, 'SEGMENT_FAILED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 7, 'SEGMENT_SELECTED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Segment', 'status', 8, 'SEGMENT_RETRIED');
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
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 0, 'GCTYPE_AUTO');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 1, 'GCTYPE_USER');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 2, 'GCTYPE_TOO_MANY_REPLICAS');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 3, 'GCTYPE_DRAINING_FS');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskCopy', 'gcType', 4, 'GCTYPE_NS_SYNCH');
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
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'status', 0, 'FILESYSTEM_PRODUCTION');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'status', 1, 'FILESYSTEM_DRAINING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'status', 2, 'FILESYSTEM_DISABLED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 0, 'ADMIN_NONE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 1, 'ADMIN_FORCE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 2, 'ADMIN_RELEASE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('FileSystem', 'adminStatus', 3, 'ADMIN_DELETED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 0, 'STREAM_PENDING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 1, 'STREAM_WAITDRIVE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 2, 'STREAM_WAITMOUNT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 3, 'STREAM_RUNNING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 4, 'STREAM_WAITSPACE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 5, 'STREAM_CREATED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 6, 'STREAM_STOPPED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 7, 'STREAM_WAITPOLICY');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('Stream', 'status', 8, 'STREAM_TO_BE_SENT_TO_VDQM');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'status', 0, 'DISKSERVER_PRODUCTION');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'status', 1, 'DISKSERVER_DRAINING');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'status', 2, 'DISKSERVER_DISABLED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 0, 'ADMIN_NONE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 1, 'ADMIN_FORCE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 2, 'ADMIN_RELEASE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskServer', 'adminStatus', 3, 'ADMIN_DELETED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskPoolQuery', 'queryType', 0, 'DISKPOOLQUERYTYPE_DEFAULT');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskPoolQuery', 'queryType', 1, 'DISKPOOLQUERYTYPE_AVAILABLE');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('DiskPoolQuery', 'queryType', 2, 'DISKPOOLQUERYTYPE_TOTAL');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 0, 'TO_BE_RESOLVED');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 1, 'TO_BE_SENT_TO_VDQM');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 2, 'WAITING_TAPESERVER');
INSERT INTO ObjStatus (object, field, statusCode, statusName) VALUES ('TapeGatewayRequest', 'status', 3, 'ONGOING');

/* Addtitional SubRequest indexes */
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

/* Drop unused indexes */
DROP INDEX I_DiskCopy_GCWeight;

/* Update and revalidation of PL-SQL code */
/******************************************/




/**
 * Package containing the definition of all tape-archieve related PL/SQL 
 * constants.
 */
CREATE OR REPLACE PACKAGE tconst
AS

  -- TPMODE
  WRITE_DISABLE CONSTANT PLS_INTEGER :=  0;
  WRITE_ENABLE  CONSTANT PLS_INTEGER :=  1;

  SEGMENT_UNPROCESSED CONSTANT PLS_INTEGER := 0;
  SEGMENT_FILECOPIED  CONSTANT PLS_INTEGER := 5;
  SEGMENT_FAILED      CONSTANT PLS_INTEGER := 6;
  SEGMENT_SELECTED    CONSTANT PLS_INTEGER := 7;
  SEGMENT_RETRIED     CONSTANT PLS_INTEGER := 8;

  STREAM_PENDING    CONSTANT PLS_INTEGER := 0;
  STREAM_WAITDRIVE  CONSTANT PLS_INTEGER := 1;
  STREAM_WAITMOUNT  CONSTANT PLS_INTEGER := 2;
  STREAM_RUNNING    CONSTANT PLS_INTEGER := 3;
  STREAM_WAITSPACE  CONSTANT PLS_INTEGER := 4;
  STREAM_CREATED    CONSTANT PLS_INTEGER := 5;
  STREAM_STOPPED    CONSTANT PLS_INTEGER := 6;
  STREAM_WAITPOLICY CONSTANT PLS_INTEGER := 7;
  STREAM_TO_BE_SENT_TO_VDQM CONSTANT PLS_INTEGER := 8;

  TAPE_UNUSED     CONSTANT PLS_INTEGER := 0;
  TAPE_PENDING    CONSTANT PLS_INTEGER := 1;
  TAPE_WAITDRIVE  CONSTANT PLS_INTEGER := 2;
  TAPE_WAITMOUNT  CONSTANT PLS_INTEGER := 3;
  TAPE_MOUNTED    CONSTANT PLS_INTEGER := 4;
  TAPE_FINISHED   CONSTANT PLS_INTEGER := 5;
  TAPE_FAILED     CONSTANT PLS_INTEGER := 6;
  TAPE_UNKNOWN    CONSTANT PLS_INTEGER := 7;
  TAPE_WAITPOLICY CONSTANT PLS_INTEGER := 8;
  TAPE_ATTACHEDTOSTREAM CONSTANT PLS_INTEGER := 9;
  
  TPMODE_READ     CONSTANT PLS_INTEGER := 0;
  TPMODE_WRITE    CONSTANT PLS_INTEGER := 1;

  TAPECOPY_CREATED       CONSTANT PLS_INTEGER := 0;
  TAPECOPY_TOBEMIGRATED  CONSTANT PLS_INTEGER := 1;
  TAPECOPY_WAITINSTREAMS CONSTANT PLS_INTEGER := 2;
  TAPECOPY_SELECTED      CONSTANT PLS_INTEGER := 3;
  TAPECOPY_TOBERECALLED  CONSTANT PLS_INTEGER := 4;
  TAPECOPY_STAGED        CONSTANT PLS_INTEGER := 5;
  TAPECOPY_FAILED        CONSTANT PLS_INTEGER := 6;
  TAPECOPY_WAITPOLICY    CONSTANT PLS_INTEGER := 7;
  TAPECOPY_REC_RETRY     CONSTANT PLS_INTEGER := 8;
  TAPECOPY_MIG_RETRY     CONSTANT PLS_INTEGER := 9;

END tconst;
/


/**
 * Package containing the definition of all disk related PL/SQL constants.
 */
CREATE OR REPLACE PACKAGE dconst
AS

  DISKCOPY_STAGED            CONSTANT PLS_INTEGER :=  0;
  DISKCOPY_WAITDISK2DISKCOPY CONSTANT PLS_INTEGER :=  1;
  DISKCOPY_WAITTAPERECALL    CONSTANT PLS_INTEGER :=  2;
  DISKCOPY_DELETED           CONSTANT PLS_INTEGER :=  3;
  DISKCOPY_FAILED            CONSTANT PLS_INTEGER :=  4;
  DISKCOPY_WAITFS            CONSTANT PLS_INTEGER :=  5;
  DISKCOPY_STAGEOUT          CONSTANT PLS_INTEGER :=  6;
  DISKCOPY_INVALID           CONSTANT PLS_INTEGER :=  7;
  DISKCOPY_BEINGDELETED      CONSTANT PLS_INTEGER :=  9;
  DISKCOPY_CANBEMIGR         CONSTANT PLS_INTEGER := 10;
  DISKCOPY_WAITFS_SCHEDULING CONSTANT PLS_INTEGER := 11;

  DISKSERVER_PRODUCTION CONSTANT PLS_INTEGER := 0;
  DISKSERVER_DRAINING   CONSTANT PLS_INTEGER := 1;
  DISKSERVER_DISABLED   CONSTANT PLS_INTEGER := 2;

  FILESYSTEM_PRODUCTION CONSTANT PLS_INTEGER := 0;
  FILESYSTEM_DRAINING   CONSTANT PLS_INTEGER := 1;
  FILESYSTEM_DISABLED   CONSTANT PLS_INTEGER := 2;

  SUBREQUEST_START            CONSTANT PLS_INTEGER :=  0;
  SUBREQUEST_RESTART          CONSTANT PLS_INTEGER :=  1;
  SUBREQUEST_RETRY            CONSTANT PLS_INTEGER :=  2;
  SUBREQUEST_WAITSCHED        CONSTANT PLS_INTEGER :=  3;
  SUBREQUEST_WAITTAPERECALL   CONSTANT PLS_INTEGER :=  4;
  SUBREQUEST_WAITSUBREQ       CONSTANT PLS_INTEGER :=  5;
  SUBREQUEST_READY            CONSTANT PLS_INTEGER :=  6;
  SUBREQUEST_FAILED           CONSTANT PLS_INTEGER :=  7;
  SUBREQUEST_FINISHED         CONSTANT PLS_INTEGER :=  8;
  SUBREQUEST_FAILED_FINISHED  CONSTANT PLS_INTEGER :=  9;
  SUBREQUEST_FAILED_ANSWERING CONSTANT PLS_INTEGER := 10;
  SUBREQUEST_ARCHIVED         CONSTANT PLS_INTEGER := 11;
  SUBREQUEST_REPACK           CONSTANT PLS_INTEGER := 12;
  SUBREQUEST_READYFORSCHED    CONSTANT PLS_INTEGER := 13;
  SUBREQUEST_BEINGSCHED       CONSTANT PLS_INTEGER := 14;

  GETNEXTSTATUS_NOTAPPLICABLE CONSTANT PLS_INTEGER :=  0;
  GETNEXTSTATUS_FILESTAGED    CONSTANT PLS_INTEGER :=  1;
  GETNEXTSTATUS_NOTIFIED      CONSTANT PLS_INTEGER :=  2;

  DISKPOOLQUERYTYPE_DEFAULT   CONSTANT PLS_INTEGER :=  0;
  DISKPOOLQUERYTYPE_AVAILABLE CONSTANT PLS_INTEGER :=  1;
  DISKPOOLQUERYTYPE_TOTAL     CONSTANT PLS_INTEGER :=  2;

  DISKPOOLSPACETYPE_FREE     CONSTANT PLS_INTEGER :=  0;
  DISKPOOLSPACETYPE_CAPACITY CONSTANT PLS_INTEGER :=  1;

END dconst;
/

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
  IF svcClassName = '*' THEN
    RETURN 0;
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

/* Function to tokenize a string using a specified delimiter. If no delimiter
 * is specified the default is ','. The results are returned as a table e.g.
 * SELECT * FROM TABLE (strTokenizer(inputValue, delimiter))
 */
CREATE OR REPLACE FUNCTION strTokenizer(p_list VARCHAR2, p_del VARCHAR2 := ',')
  RETURN strListTable pipelined IS
  l_idx   INTEGER;
  l_list  VARCHAR2(32767) := p_list;
  l_value VARCHAR2(32767);
BEGIN
  LOOP
    l_idx := instr(l_list, p_del);
    IF l_idx > 0 THEN
      PIPE ROW(ltrim(rtrim(substr(l_list, 1, l_idx - 1))));
      l_list := substr(l_list, l_idx + length(p_del));
    ELSE
      PIPE ROW(ltrim(rtrim(l_list)));
      EXIT;
    END IF;
  END LOOP;
  RETURN;
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

/* Function to check if a diskserver and its given mountpoints have any files
 * attached to them.
 */
CREATE OR REPLACE
FUNCTION checkIfFilesExist(diskServerName IN VARCHAR2, mountPointName IN VARCHAR2)
RETURN NUMBER AS
  rtn NUMBER;
BEGIN
  SELECT count(*) INTO rtn
    FROM DiskCopy, FileSystem, DiskServer
   WHERE DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.diskserver = DiskServer.id
     AND DiskServer.name = diskServerName
     AND (FileSystem.mountpoint = mountPointName 
      OR  length(mountPointName) IS NULL)
     AND rownum = 1;
  RETURN rtn;
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
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Diskcopy)*/ SubRequest
     SET status = newSubReqStatus
   WHERE diskCopy = dcId RETURNING id BULK COLLECT INTO srIds;
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Parent)*/ SubRequest
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
CREATE OR REPLACE PROCEDURE cancelRecallForTape (inVid IN VARCHAR2) AS
BEGIN
  FOR a IN (SELECT DISTINCT(DiskCopy.id), DiskCopy.castorfile
              FROM Segment, Tape, TapeCopy, DiskCopy
             WHERE Segment.tape = Tape.id
               AND Segment.copy = TapeCopy.id
               AND DiskCopy.castorfile = TapeCopy.castorfile
               AND DiskCopy.status = 2  -- WAITTAPERECALL
               AND Tape.vid = inVid
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
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ count(*) INTO nb
        FROM SubRequest
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

/* Process the adminList provided by the user in oracleCommon.schema */
DECLARE
  adminUserId NUMBER;
  adminGroupId NUMBER;
  ind NUMBER;
  errmsg VARCHAR(2048);
BEGIN
  -- If the adminList is empty do nothing
  IF '&adminList' IS NULL THEN
    RETURN;
  END IF;
  -- Loop over the adminList
  FOR admin IN (SELECT column_value AS s
                  FROM TABLE(strTokenizer('&adminList',' '))) LOOP
    BEGIN
      ind := INSTR(admin.s, ':');
      IF ind = 0 THEN
        errMsg := 'Invalid <userid>:<groupid> ' || admin.s || ', ignoring';
        RAISE INVALID_NUMBER;
      END IF;
      errMsg := 'Invalid userid ' || SUBSTR(admin.s, 1, ind - 1) || ', ignoring';
      adminUserId := TO_NUMBER(SUBSTR(admin.s, 1, ind - 1));
      errMsg := 'Invalid groupid ' || SUBSTR(admin.s, ind) || ', ignoring';
      adminGroupId := TO_NUMBER(SUBSTR(admin.s, ind+1));
      INSERT INTO AdminUsers VALUES (adminUserId, adminGroupId);
    EXCEPTION WHEN INVALID_NUMBER THEN
      dbms_output.put_line(errMsg);
    END;
  END LOOP;
END;
/


/* PL/SQL method implementing checkPermission
 * The return value can be
 *   0 if access is granted
 *   1 if access denied
 */
CREATE OR REPLACE FUNCTION checkPermission(reqSvcClass IN VARCHAR2,
                                           reqEuid IN NUMBER,
                                           reqEgid IN NUMBER,
                                           reqTypeI IN NUMBER)
RETURN NUMBER AS
  res NUMBER;
  c NUMBER;
BEGIN
  -- Skip access control checks for admin/internal users
  SELECT count(*) INTO c FROM AdminUsers 
   WHERE egid = reqEgid
     AND (euid = reqEuid OR euid IS NULL);
  IF c > 0 THEN
    -- Admin access, just proceed
    RETURN 0;
  END IF;
  -- Perform the check
  SELECT count(*) INTO c
    FROM WhiteList
   WHERE (svcClass = reqSvcClass OR svcClass IS NULL
          OR (length(reqSvcClass) IS NULL AND svcClass = 'default'))
     AND (egid = reqEgid OR egid IS NULL)
     AND (euid = reqEuid OR euid IS NULL)
     AND (reqType = reqTypeI OR reqType IS NULL);
  IF c = 0 THEN
    -- Not found in White list -> no access
    RETURN 1;
  ELSE
    SELECT count(*) INTO c
      FROM BlackList
     WHERE (svcClass = reqSvcClass OR svcClass IS NULL
            OR (length(reqSvcClass) IS NULL AND svcClass = 'default'))
       AND (egid = reqEgid OR egid IS NULL)
       AND (euid = reqEuid OR euid IS NULL)
       AND (reqType = reqTypeI OR reqType IS NULL);
    IF c = 0 THEN
      -- Not Found in Black list -> access
      RETURN 0;
    ELSE
      -- Found in Black list -> no access
      RETURN 1;
    END IF;
  END IF;
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
    /* This line is a deprecated work around the issue of having changed the magic number of
     * DiskPoolQuery status from 103 to 195. It should be dropped as soon as all clients
     * are 2.1.10-1 or newer */
    IF p.reqType = 103 THEN p.reqType := 195; END IF; -- DiskPoolQuery fix
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
    /* This line is a deprecated work around the issue of having changed the magic number of
     * DiskPoolQuery status from 103 to 195. It should be dropped as soon as all clients
     * are 2.1.10-1 or newer */
    IF p.reqType = 103 THEN p.reqType := 195; END IF; -- DiskPoolQuery fix
    removePrivilege(p);
  END;

  -- List privileges
  PROCEDURE listPrivileges(svcClassName IN VARCHAR2, ieuid IN NUMBER,
                           iegid IN NUMBER, ireqType IN NUMBER,
                           plist OUT PrivilegeExt_Cur) AS
    ireqTypeFixed NUMBER;
  BEGIN
    /* ireqTypeFixed is a deprecated work around the issue of having changed the magic number of
     * DiskPoolQuery status from 103 to 195. It should be dropped as soon as all clients
     * are 2.1.10-1 or newer */
    ireqTypeFixed := ireqType;
    IF ireqTypeFixed = 103 THEN ireqTypeFixed := 195; END IF; -- DiskPoolQuery fix
    OPEN plist FOR
      SELECT decode(svcClass, NULL, '*', '*', '''*''', svcClass),
             euid, egid, reqType, 1
        FROM WhiteList
       WHERE (WhiteList.svcClass = svcClassName OR WhiteList.svcClass IS  NULL OR svcClassName IS NULL)
         AND (WhiteList.euid = ieuid OR WhiteList.euid IS NULL OR ieuid = -1)
         AND (WhiteList.egid = iegid OR WhiteList.egid IS NULL OR iegid = -1)
         AND (WhiteList.reqType = ireqTypeFixed OR WhiteList.reqType IS NULL OR ireqTypeFixed = 0)
    UNION
      SELECT decode(svcClass, NULL, '*', '*', '''*''', svcClass),
             euid, egid, reqType, 0
        FROM BlackList
       WHERE (BlackList.svcClass = svcClassName OR BlackList.svcClass IS  NULL OR svcClassName IS NULL)
         AND (BlackList.euid = ieuid OR BlackList.euid IS NULL OR ieuid = -1)
         AND (BlackList.egid = iegid OR BlackList.egid IS NULL OR iegid = -1)
         AND (BlackList.reqType = ireqTypeFixed OR BlackList.reqType IS NULL OR ireqTypeFixed = 0);
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
CREATE OR REPLACE PACKAGE castor AS
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
    lastAccessTime INTEGER,
    hwStatus INTEGER);
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
  TYPE UUIDRecord IS RECORD (uuid VARCHAR(2048));
  TYPE UUIDRecord_Cur IS REF CURSOR RETURN UUIDRecord;
  TYPE UUIDPairRecord IS RECORD (uuid1 VARCHAR(2048), uuid2 VARCHAR(2048));
  TYPE UUIDPairRecord_Cur IS REF CURSOR RETURN UUIDPairRecord;
  TYPE TransferRecord IS RECORD (subreId VARCHAR(2048), resId VARCHAR(2048), fileId NUMBER, nsHost VARCHAR2(2048));
  TYPE TransferRecord_Cur IS REF CURSOR RETURN TransferRecord;
  TYPE DiskServerName IS RECORD (diskServer VARCHAR(2048));
  TYPE DiskServerList_Cur IS REF CURSOR RETURN DiskServerName;
  /* These types are deprecated and should go when the jobmanager and LSF are dropped*/
  TYPE SchedulerJobLine IS RECORD (
    subReqId VARCHAR(2048),
    reqId VARCHAR(2048),
    noSpace INTEGER,
    noFSAvail INTEGER);
  TYPE SchedulerJobs_Cur IS REF CURSOR RETURN SchedulerJobLine;
  TYPE JobFailedSubReqList_Cur IS REF CURSOR RETURN JobFailedProcHelper%ROWTYPE;
  /* end of deprecated code */
  TYPE FileEntry IS RECORD (
    fileid INTEGER,
    nshost VARCHAR2(2048));
  TYPE FileEntry_Cur IS REF CURSOR RETURN FileEntry;
  TYPE PriorityMap_Cur IS REF CURSOR RETURN PriorityMap%ROWTYPE;
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
  FOR cf IN (SELECT /*+ USE_NL(E D)
                     INDEX_RS_ASC(E I_DiskCopy_Status)
                     INDEX_RS_ASC(D I_DiskCopy_CastorFile) */
                    UNIQUE D.castorfile, E.id
               FROM DiskCopy D, DiskCopy E
              WHERE D.castorfile = E.castorfile
                AND D.fileSystem IN
                  (SELECT /*+ CARDINALITY(fsidTable 5) */ *
                     FROM TABLE(fsIds) fsidTable)
                AND D.status IN (0, 10)
                AND E.status = 2) LOOP
    -- Cancel recall and restart subrequests
    cancelRecall(cf.castorfile, cf.id, 1); -- RESTART
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
  IF :old.status IN (1, 2) AND  -- DRAINING, DISABLED
     :new.status = 0 THEN       -- PRODUCTION
    checkFsBackInProd(:old.id);
  END IF;
  -- Cancel any ongoing draining operations if the filesystem is not in a
  -- DRAINING state
  IF :new.status != 1 THEN  -- DRAINING
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
  -- Cancel all draining operations if the diskserver is disabled.
  IF :new.status = 2 THEN  -- DISABLED
    UPDATE DrainingFileSystem
       SET status = 3  -- INTERRUPTED
     WHERE fileSystem IN
       (SELECT FileSystem.id FROM FileSystem
         WHERE FileSystem.diskServer = :new.id)
       AND status IN (0, 1, 2, 7);  -- CREATED, INITIALIZING, RUNNING, RESTART
  END IF;
  -- If the diskserver is in PRODUCTION cancel the draining operation of 
  -- filesystems not in DRAINING.
  IF :new.status = 0 THEN  -- PRODUCTION
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

CREATE OR REPLACE TRIGGER tr_Tape_Insert
  BEFORE INSERT ON Tape
FOR EACH ROW
/**
 * Converts an inserted lastVdqmPingTime of NULL to the current time.
 */
BEGIN
  IF :NEW.lastVdqmPingTime IS NULL THEN
    :NEW.lastVdqmPingTime := getTime();
  END IF;
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
      SELECT /*+ INDEX(SR PK_SubRequest_ID) */ id INTO srIntId
        FROM SubRequest PARTITION (P_STATUS_0_1_2) SR
       WHERE id = srIntId FOR UPDATE NOWAIT;
      -- Since we are here, we got the lock. We have our winner, let's update it
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
      OR abortedSRstatus = dconst.SUBREQUEST_READYFORSCHED
      OR abortedSRstatus = dconst.SUBREQUEST_BEINGSCHED THEN
      -- standard case, we only have to fail the subrequest
      UPDATE SubRequest SET status = 7 WHERE id = sr.srId;
      INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 0, '');
    WHEN abortedSRstatus = dconst.SUBREQUEST_WAITTAPERECALL THEN
      -- recall case, let's see whether we can cancel the recall
      DECLARE
        segId INTEGER;
        unusedIds "numList";
      BEGIN
        -- XXX First lock all segments for the file. Note that
        -- XXX this step should be dropped once the tapeGateway
        -- XXX is deployed. The current recaller does not take
        -- XXX the proper lock on the castorFiles, hence we
        -- XXX need this here
        SELECT Segment.id BULK COLLECT INTO unusedIds
          FROM Segment, TapeCopy
         WHERE TapeCopy.castorfile = sr.cfId
           AND TapeCopy.id = Segment.copy
         ORDER BY Segment.id
           FOR UPDATE OF Segment.id;
        -- Check whether we have any segment in SELECTED
        SELECT segment.id INTO segId
          FROM Segment, TapeCopy
         WHERE TapeCopy.castorfile = sr.cfId
           AND TapeCopy.id = Segment.copy
           AND Segment.status = 7 -- SELECTED
           AND ROWNUM < 2;
        -- Something is running, so give up
        INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 16, 'Cannot abort ongoing recall'); -- EBUSY
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- Nothing running, we can cancel the recall  
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET status = 7 WHERE id = sr.srId;
        deleteTapeCopies(sr.cfId);
        UPDATE DiskCopy SET status = dconst.DISKCOPY_FAILED
         WHERE castorfile = sr.cfid AND status = dconst.DISKCOPY_WAITTAPERECALL;
        INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 0, '');
      END;
    WHEN abortedSRstatus = dconst.SUBREQUEST_FAILED
      OR abortedSRstatus = dconst.SUBREQUEST_FAILED_FINISHED
      OR abortedSRstatus = dconst.SUBREQUEST_FAILED_ANSWERING THEN
      -- subrequest has failed, nothing to abort
      INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 22, 'Cannot abort failed subRequest'); -- EINVAL
    WHEN abortedSRstatus = dconst.SUBREQUEST_FINISHED
      OR abortedSRstatus = dconst.SUBREQUEST_ARCHIVED THEN
      -- subrequest is over, nothing to abort
      INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 22, 'Cannot abort completed subRequest'); -- EINVAL
    ELSE
      -- unknown status !
      INSERT INTO ProcessBulkRequestHelper
      VALUES (sr.fileId, sr.nsHost, 1015, 'Found unknown status for request : ' || TO_CHAR(abortedSRstatus)); -- SEINTERNAL
  END CASE;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- subRequest was deleted in the mean time !
  INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 2, 'Targeted SubRequest has just been deleted'); -- ENOENT
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
      OR abortedSRstatus = dconst.SUBREQUEST_READYFORSCHED
      OR abortedSRstatus = dconst.SUBREQUEST_BEINGSCHED THEN
      -- standard case, we only have to fail the subrequest
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_FAILED
       WHERE id = sr.srId;
      UPDATE DiskCopy SET status = dconst.DISKCOPY_FAILED
       WHERE castorfile = sr.cfid AND status IN (dconst.DISKCOPY_STAGEOUT,
                                                 dconst.DISKCOPY_WAITFS,
                                                 dconst.DISKCOPY_WAITFS_SCHEDULING);
      INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 0, '');
    WHEN abortedSRstatus = dconst.SUBREQUEST_FAILED
      OR abortedSRstatus = dconst.SUBREQUEST_FAILED_FINISHED
      OR abortedSRstatus = dconst.SUBREQUEST_FAILED_ANSWERING THEN
      -- subrequest has failed, nothing to abort
      INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 22, 'Cannot abort failed subRequest'); -- EINVAL
    WHEN abortedSRstatus = dconst.SUBREQUEST_FINISHED
      OR abortedSRstatus = dconst.SUBREQUEST_ARCHIVED THEN
      -- subrequest is over, nothing to abort
      INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 22, 'Cannot abort completed subRequest'); -- EINVAL
    ELSE
      -- unknown status !
      INSERT INTO ProcessBulkRequestHelper
      VALUES (sr.fileId, sr.nsHost, 1015, 'Found unknown status for request : ' || TO_CHAR(abortedSRstatus)); -- SEINTERNAL
  END CASE;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- subRequest was deleted in the mean time !
  INSERT INTO ProcessBulkRequestHelper VALUES (sr.fileId, sr.nsHost, 2, 'Targeted SubRequest has just been deleted'); -- ENOENT
END;
/

/* PL/SQL method to process bulk abort on files related requests */
CREATE OR REPLACE PROCEDURE processBulkAbortFileReqs
(abortReqId IN INTEGER, origReqId IN INTEGER,
 fileIds IN "numList", nsHosts IN strListTable, reqType IN NUMBER) AS
  nbItems NUMBER;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  unused NUMBER;
  firstOne BOOLEAN;
BEGIN
  -- Gather the list of subrequests to abort
  IF fileIds.count() = 0 THEN
    -- handle the case of an empty request, meaning that all files should be aborted
    INSERT INTO processBulkAbortFileReqsHelper (
      SELECT /*+ INDEX(Subrequest I_Subrequest_CastorFile)*/
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
        INSERT INTO processBulkAbortFileReqsHelper VALUES (srId, cfId, fileIds(i), nsHosts(i), srUuid);
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- this fileid/nshost did not exist in the request, send an error back
        INSERT INTO ProcessBulkRequestHelper
        VALUES (fileIds(i), nsHosts(i), 2, 'No subRequest found for this fileId/nsHost'); -- ENOENT
      END;
    END LOOP;
  END IF;
  SELECT COUNT(*) INTO nbItems FROM processBulkAbortFileReqsHelper;
  -- handle aborts in bulk while avoiding deadlocks
  WHILE nbItems > 0 LOOP
    firstOne := TRUE;
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
        INSERT INTO TransfersToAbort VALUES (sr.uuid);
        nbItems := nbItems - 1;
      EXCEPTION WHEN SrLocked THEN
        -- we close the session here and exit the inner loop
        COMMIT;
        EXIT;
      END;
    END LOOP;
    -- wake up the scheduler so that it can remove the transfer from the queues now
    DBMS_ALERT.SIGNAL('transfersToAbort', ''); 
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
  DELETE FROM Id2Type WHERE id = abortReqId;
  DELETE FROM Client WHERE id = clientId
    RETURNING ipAddress, port INTO rIpAddress, rport;
  DELETE FROM Id2Type WHERE id = clientId;
  -- list fileids to process and drop them from the DB; override the
  -- nsHost in case it is defined in the configuration
  SELECT fileid, decode(nsHostName, '', nsHost, nsHostName), id
    BULK COLLECT INTO fileIds, nsHosts, ids
    FROM NsFileId WHERE request = abortReqId;
  FORALL i IN ids.FIRST .. ids.LAST DELETE FROM NsFileId WHERE id = ids(i);
  FORALL i IN ids.FIRST .. ids.LAST DELETE FROM Id2Type WHERE id = ids(i);
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
              reqId, id, 2 as rtype from StagePrepareToPutRequest)
     WHERE reqId = abortedReqUuid;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- abort on non supported request type
    INSERT INTO ProcessBulkRequestHelper VALUES (0, '', 2, 'Request not found, or abort not supported for this request type'); -- ENOENT
    RETURN;
  END;
  processBulkAbortFileReqs(abortReqId, requestId, fileIds, nsHosts, reqType);
END;
/

/* PL/SQL method to process bulk requests */
CREATE OR REPLACE PROCEDURE processBulkRequest(service IN VARCHAR2,
                                               rtype OUT INTEGER, rIpAddress OUT INTEGER,
                                               rport OUT INTEGER, rReqUuid OUT VARCHAR2,
                                               rSubResults OUT castor.FileResult_Cur) AS
  CURSOR Rcur IS SELECT /*+ FIRST_ROWS(10) */ id
                   FROM NewRequests
                  WHERE type IN (
                    SELECT type FROM Type2Obj
                     WHERE svcHandler = service
                       AND svcHandler IS NOT NULL);
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  reqId NUMBER;
BEGIN
  -- in case we do not find anything, rtype should be 0
  rtype :=0;
  OPEN Rcur;
  -- Loop on candidates until we can lock one
  LOOP
    -- Fetch next candidate
    FETCH Rcur INTO reqId;
    EXIT WHEN Rcur%NOTFOUND;
    BEGIN
      -- Try to take a lock on the current candidate
      SELECT id INTO reqId FROM NewRequests WHERE id = reqId FOR UPDATE NOWAIT;
      -- Since we are here, we got the lock. We have our winner,
      DELETE FROM NewRequests WHERE id = reqId;
      -- Clear the temporary table for subresults
      DELETE FROM ProcessBulkRequestHelper;
      -- dispatch actual processing depending on request type
      SELECT type INTO rType FROM id2Type WHERE id = reqId;
      CASE rType
        WHEN 50 THEN -- Abort Request
          processBulkAbort(reqId, rIpAddress, rport, rReqUuid);
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
CREATE OR REPLACE PROCEDURE subRequestFailedToDo(srId OUT INTEGER, srRetryCounter OUT INTEGER, srFileName OUT VARCHAR2,
                                                 srProtocol OUT VARCHAR2, srXsize OUT INTEGER, srPriority OUT INTEGER,
                                                 srStatus OUT INTEGER, srModeBits OUT INTEGER, srFlags OUT INTEGER,
                                                 srSubReqId OUT VARCHAR2, srErrorCode OUT NUMBER,
                                                 srErrorMessage OUT VARCHAR2) AS
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  CURSOR c IS
     SELECT /*+ FIRST_ROWS(10) INDEX(SR I_SubRequest_RT_CT_ID) */ SR.id
       FROM SubRequest PARTITION (P_STATUS_7) SR; -- FAILED
  srAnswered INTEGER;
  srIntId NUMBER;
BEGIN
  OPEN c;
  LOOP
    FETCH c INTO srIntId;
    EXIT WHEN c%NOTFOUND;
    BEGIN
      SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ answered INTO srAnswered
        FROM SubRequest PARTITION (P_STATUS_7) 
       WHERE id = srIntId FOR UPDATE NOWAIT;
      IF srAnswered = 1 THEN
        -- already answered, ignore it
        archiveSubReq(srIntId, 9);  -- FAILED_FINISHED
      ELSE
        -- we got our subrequest
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ subrequest
           SET status = 10   -- FAILED_ANSWERING
         WHERE id = srIntId
        RETURNING retryCounter, fileName, protocol, xsize, priority, status,
                  modeBits, flags, subReqId, errorCode, errorMessage
        INTO srRetryCounter, srFileName, srProtocol, srXsize, srPriority, srStatus,
             srModeBits, srFlags, srSubReqId, srErrorCode, srErrorMessage;
        srId := srIntId;
        EXIT;
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
CREATE OR REPLACE PROCEDURE requestToDo(service IN VARCHAR2, rId OUT INTEGER) AS
BEGIN
  DELETE FROM NewRequests
   WHERE type IN (
     SELECT type FROM Type2Obj
      WHERE svcHandler = service
        AND svcHandler IS NOT NULL
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
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
  unused INTEGER;
  rId INTEGER;
  rname VARCHAR2(100);
  srIds "numList";
  clientId INTEGER;
BEGIN
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ request INTO rId
    FROM SubRequest
   WHERE id = srId;
  BEGIN
    -- Lock the access to the Request
    SELECT Id2Type.id INTO rId
      FROM Id2Type
     WHERE id = rId FOR UPDATE;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET parent = NULL, diskCopy = NULL,  -- unlink this subrequest as it's dead now
           lastModificationTime = getTime(),
           status = finalStatus
     WHERE id = srId;
    BEGIN
      -- Try to see whether another subrequest in the same
      -- request is still being processed
      SELECT /*+ INDEX(Subrequest I_Subrequest_Request)*/ id INTO unused FROM SubRequest
       WHERE request = rId AND status NOT IN (8, 9) AND ROWNUM < 2;  -- all but {FAILED_,}FINISHED
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- All subrequests have finished, we can archive
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
      SELECT /*+ INDEX(Subrequest I_Subrequest_Request)*/ id BULK COLLECT INTO srIds
        FROM SubRequest
       WHERE request = rId;
      FORALL i IN srIds.FIRST .. srIds.LAST
        DELETE FROM Id2Type WHERE id = srIds(i);
      -- archive the successful subrequests      
      UPDATE /*+ INDEX(SubRequest I_SubRequest_Request) */ SubRequest
         SET status = 11    -- ARCHIVED
       WHERE request = rId
         AND status = 8;  -- FINISHED
    END;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- No data found here means that the Id2Type entry is not there
    -- and the subrequest was already archived: just update this subrequest,
    -- don't bother with the whole request. Note that this could
    -- happen only if someone archives an already archived subrequest.
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET lastModificationTime = getTime(),
           status = finalStatus
     WHERE id = srId;
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
       AND FileSystem.status = 0 -- PRODUCTION
       AND DiskServer.status = 0 -- PRODUCTION
       AND totalSize * minAllowedFreeSpace < free - defFileSize;
    IF (c = 0) THEN
      RETURN 1;
    END IF;
    -- This is deprecated and should go when the jobmanager and LSF are dropped
    DECLARE
      noLSF VARCHAR2(2048);
    BEGIN
      noLSF := getConfigOption('RmMaster', 'NoLSFMode', 'no');
      IF LOWER(noLSF) != 'yes' THEN
        SELECT sum(xsize) INTO reservedSpace
          FROM SubRequest, StagePutRequest
         WHERE SubRequest.request = StagePutRequest.id
           AND SubRequest.status = 6  -- READY
           AND StagePutRequest.svcClass = svcClassId;
        IF availSpace < reservedSpace THEN
          RETURN 1;
        END IF;
      END IF;
    END;
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
  nbTPs INTEGER;
BEGIN
  -- get #tapeCopies requested by this file
  SELECT nbCopies INTO nbTCs
    FROM FileClass WHERE id = fileClassId;
  -- get #tapeCopies from the forcedFileClass: if no forcing
  -- we assume we have tape backend and we let the job
  SELECT nvl(nbCopies, nbTCs) INTO nbForcedTCs
    FROM FileClass, SvcClass
   WHERE SvcClass.forcedFileClass = FileClass.id(+)
     AND SvcClass.id = svcClassId;
  IF nbTCs > nbForcedTCs THEN
    -- typically, when nbTCs = 1 and nbForcedTCs = 0: fail the job
    RETURN 1;
  ELSE
    -- get #tapePools configured in this svcClass
    SELECT COUNT(*) INTO nbTPs FROM SvcClass2TapePool
     WHERE parent = svcClassId;
    IF nbTCs > 0 AND nbTPs = 0 THEN
      -- This is a configuration mistake, and we stop the user in this case.
      -- However, many other conditions should be met to make sure the file
      -- being written goes to tape (see e.g. bug #68020).
      -- To be reviewed once the migration policy logic is refactored.
      RETURN 1;
    ELSE
      RETURN 0;
    END IF;
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
         AND checkPermission(SvcClass.name, reuid, regid, 133) = 0
         AND NOT EXISTS (
           -- Don't select source diskcopies which already failed more than 10 times
           SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */ 'x'
             FROM StageDiskCopyReplicaRequest R, SubRequest
            WHERE SubRequest.request = R.id
              AND R.sourceDiskCopy = DiskCopy.id
              AND SubRequest.status = 9 -- FAILED_FINISHED
           HAVING COUNT(*) >= 10)
       ORDER BY FileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                               FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams,
                               FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
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
       AND FileSystem.status = 0 -- PRODUCTION
       AND FileSystem.diskserver = DiskServer.id
       AND DiskServer.status = 0 -- PRODUCTION
       AND DiskCopy.status IN (0, 6, 10)  -- STAGED, STAGEOUT, CANBEMIGR
     ORDER BY FileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams,
                             FileSystem.nbWriteStreams, FileSystem.nbReadWriteStreams,
                             FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
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
CREATE OR REPLACE
PROCEDURE checkForD2DCopyOrRecall(cfId IN NUMBER, srId IN NUMBER, reuid IN NUMBER, regid IN NUMBER,
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
       SET status = 7, -- FAILED
           errorCode = 28, -- ENOSPC
           errorMessage = 'File creation canceled since diskPool is full'
     WHERE id = srId;
    COMMIT;
    RETURN;
  END IF;
  -- Resolve the destination service class id to a name
  SELECT name INTO destSvcClass FROM SvcClass WHERE id = svcClassId;
  -- Determine if there are any copies of the file in the same service class
  -- on DISABLED or DRAINING hardware. If we found something then set the user
  -- and group id to -1 this effectively disables the later privilege checks
  -- to see if the user can trigger a d2d or recall. (#55745)
  BEGIN
    SELECT -1, -1 INTO userid, groupid
      FROM DiskCopy, FileSystem, DiskServer, DiskPool2SvcClass
     WHERE DiskCopy.fileSystem = FileSystem.id
       AND DiskCopy.castorFile = cfId
       AND DiskCopy.status IN (0, 10)  -- STAGED, CANBEMIGR
       AND FileSystem.diskPool = DiskPool2SvcClass.parent
       AND DiskPool2SvcClass.child = svcClassId
       AND FileSystem.diskServer = DiskServer.id
       AND (DiskServer.status != 0  -- !PRODUCTION
        OR  FileSystem.status != 0) -- !PRODUCTION
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
       SET status = 7, -- FAILED
           errorCode = 13, -- EACCES
           errorMessage = 'Insufficient user privileges to trigger a tape recall or file replication to the '''||destSvcClass||''' service class'
     WHERE id = srId;
    COMMIT;
    RETURN;
  END IF;
  -- Try to find a diskcopy to replicate
  getBestDiskCopyToReplicate(cfId, userid, groupid, 0, svcClassId, dcId, srcSvcClassId);
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
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = 7, -- FAILED
           errorCode = CASE
             WHEN dcStatus IN (5, 11) THEN 16 -- WAITFS, WAITFSSCHEDULING, EBUSY
             WHEN dcStatus = 6 AND fsStatus = 0 AND dsStatus = 0 THEN 16 -- STAGEOUT, PRODUCTION, PRODUCTION, EBUSY
             ELSE 1718 -- ESTNOTAVAIL
           END,
           errorMessage = CASE
             WHEN dcStatus IN (5, 11) THEN -- WAITFS, WAITFSSCHEDULING
               'File is being (re)created right now by another user'
             WHEN dcStatus = 6 AND fsStatus = 0 and dsStatus = 0 THEN -- STAGEOUT, PRODUCTION, PRODUCTION
               'File is being written to in another service class'
             ELSE
               'All copies of this file are unavailable for now. Please retry later'
           END
     WHERE id = srId;
    COMMIT;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- Check whether the user has the rights to issue a tape recall to
    -- the destination service class.
    IF checkPermission(destSvcClass, userid, groupid, 161) != 0 THEN
      -- Fail the subrequest and notify the client
      dcId := -1;
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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

  -- Determine the requested filesystem value
  SELECT DiskServer.name || ':' || FileSystem.mountpoint INTO rfs
    FROM DiskCopy, FileSystem, DiskServer
   WHERE DiskCopy.fileSystem = FileSystem.id
     AND FileSystem.diskServer = DiskServer.id
     AND DiskCopy.id = sourceDcId;

  -- Create the SubRequest setting the initial status to READYFORSCHED for
  -- immediate dispatching i.e no stager processing by the jobmanager daemon.
  INSERT INTO SubRequest
    (retrycounter, filename, protocol, xsize, priority, subreqid, flags, modebits,
     creationtime, lastmodificationtime, answered, id, diskcopy, castorfile, parent,
     status, request, getnextstatus, errorcode, requestedfilesystems, svcHandler)
  VALUES (0, fileName, 'rfio', fileSize, 0, uuidgen(), 0, 0, gettime(), gettime(),
     0, srId, destDcId, cfId, 0, 13, reqId, 0, 0, rfs, 'NotNullNeeded');
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

  -- Look for available diskcopies. The status is needed for the
  -- internal replication processing, and only if count = 1, hence
  -- the min() function does not represent anything here.
  SELECT COUNT(DiskCopy.id), min(DiskCopy.status) INTO nbDCs, dcStatus
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
            SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ DiskCopy.id, DiskCopy.path, DiskCopy.status,
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
                   -- XXX to be reviewed once LSF is dropped.
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
             AND FileSystem.status = 0 -- PRODUCTION
             AND DiskServer.status = 0 -- PRODUCTION
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
         AND status IN (13, 14, 6)  -- WAITINGFORSCHED, BEINGSCHED, READY
         AND castorFile = cfId;
      -- found it, wait on it
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
  SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */
         count(StageRepackRequest.repackVid) INTO nbTC
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
  -- update the Repack subRequest for this file. The status REPACK
  -- stays until the migration to the new tape is over.
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET diskCopy = dcId, status = 12  -- REPACK
   WHERE id = srId;   
  -- get the service class, uid and gid
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id) INDEX(R PK_StageRepackRequest_Id) */ R.svcClass, euid, egid
    INTO svcClassId, reuid, regid
    FROM StageRepackRequest R, SubRequest
   WHERE SubRequest.request = R.id
     AND SubRequest.id = srId;
  -- create the required number of tapecopies for the files
  -- XXX For the time being, nbTC will be 1 for sure until we're able
  -- XXX to handle repacking of dual-copy files 
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
         SubRequest.castorFile, Request.euid, Request.egid, Request.svcClass, Request.repack
    INTO cfId, reuid, regid, svcClassId, repack
    FROM (SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */
                 id, euid, egid, svcClass, 0 repack FROM StagePrepareToGetRequest UNION ALL
          SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */
                 id, euid, egid, svcClass, 1 repack FROM StageRepackRequest UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */
                 id, euid, egid, svcClass, 0 repack FROM StagePrepareToUpdateRequest) Request,
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
       AND FileSystem.status = 0 -- PRODUCTION
       AND FileSystem.diskserver = DiskServer.id
       AND DiskServer.status = 0 -- PRODUCTION
       AND DiskCopy.status IN (0, 6, 10)  -- STAGED, STAGEOUT, CANBEMIGR
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
          -- However at the moment the tapegateway does not handle double repacks,
          -- so we simply fail this repack and rely on Repack to submit
          -- such double tape repacks one by one.
          UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
    createDiskCopyReplicaRequest(srId, srcDcId, srvSvcClassId, svcClassId, reuid, regid);
    result := 1;  -- DISKCOPY_WAITDISK2DISKCOPY, for logging purposes
  ELSIF srcDcId = 0 THEN  -- recall
    BEGIN
      -- check whether there's already a recall, and get its svcClass
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ Request.svcClass, DiskCopy.id, repack
        INTO recSvcClass, recDcId, recRepack
        FROM (SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ 
                     id, svcClass, 0 repack FROM StagePrepareToGetRequest UNION ALL
              SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */
                     id, svcClass, 0 repack FROM StageGetRequest UNION ALL
              SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */
                     id, svcClass, 1 repack FROM StageRepackRequest UNION ALL
              SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                     id, svcClass, 0 repack FROM StageUpdateRequest UNION ALL
              SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */
                     id, svcClass, 0 repack FROM StagePrepareToUpdateRequest) Request,
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
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
    SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ subrequest.id INTO putSubReq
      FROM SubRequest, Id2Type
     WHERE SubRequest.castorfile = cfId
       AND SubRequest.request = Id2Type.id
       AND Id2Type.type IN (40, 44)  -- Put, Update
       AND SubRequest.status IN (0, 1, 2, 3, 6, 13, 14) -- START, RESTART, RETRY, WAITSCHED, READY, READYFORSCHED, BEINGSCHED
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
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = 7, -- FAILED
             errorCode = 22, -- EINVAL
             errorMessage = 'File recreation canceled since this service class doesn''t provide tape backend'
       WHERE id = srId;
      COMMIT;
      RETURN;
    END IF;
    -- check if recreation is possible for TapeCopies
    SELECT count(*) INTO nbRes FROM TapeCopy
     WHERE status = tconst.TAPECOPY_SELECTED
      AND castorFile = cfId;
    IF nbRes > 0 THEN
      -- We found something, thus we cannot recreate
      dcId := 0;
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
          SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ SubRequest.id INTO srParent
            FROM SubRequest, Id2Type
           WHERE request = Id2Type.id
             AND type IN (40, 44)  -- Put, Update
             AND diskCopy = dcId
             AND status IN (13, 14, 6)  -- READYFORSCHED, BEINGSCHED, READY
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
  -- Reset svcClass to the request's one as we want to use the new one for migration.
  -- However, we don't reset the filesize, this will be done at prepareForMigration time
  -- so that in case the old file needs to be recovered from tape after e.g. a failed
  -- transfer, we correctly have the previous size.
  UPDATE CastorFile SET svcClass = sclassId
   WHERE id = cfId;
  -- link SubRequest and DiskCopy
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET diskCopy = dcId,
         lastModificationTime = getTime()
   WHERE id = srId;
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

/* PL/SQL method implementing selectCastorFile */
CREATE OR REPLACE PROCEDURE selectCastorFile (fId IN INTEGER,
                                              nh IN VARCHAR2,
                                              sc IN INTEGER,
                                              fc IN INTEGER,
                                              fs IN INTEGER,
                                              fn IN VARCHAR2,
                                              srId IN NUMBER,
                                              lut IN NUMBER,
                                              rid OUT INTEGER,
                                              rfs OUT INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  nsHostName VARCHAR2(2048);
  previousLastKnownFileName VARCHAR2(2048);
  fcId NUMBER;
BEGIN
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
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
     WHERE fileId = fid AND nsHost = nsHostName;
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
    SELECT id INTO rid FROM CastorFile WHERE id = rid FOR UPDATE;
    -- The file is still there, so update lastAccessTime and lastKnownFileName.
    UPDATE CastorFile SET lastAccessTime = getTime(),
                          lastKnownFileName = normalizePath(fn)
     WHERE id = rid;
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET castorFile = rid
     WHERE id = srId;
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
    INSERT INTO CastorFile (id, fileId, nsHost, svcClass, fileClass, fileSize,
                            creationTime, lastAccessTime, lastUpdateTime, lastKnownFileName)
      VALUES (ids_seq.nextval, fId, nsHostName, sc, fcId, fs, getTime(), getTime(), lut, normalizePath(fn))
      RETURNING id, fileSize INTO rid, rfs;
    INSERT INTO Id2Type (id, type) VALUES (rid, 2); -- OBJ_CastorFile
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET castorFile = rid
     WHERE id = srId;
  END;
EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
  -- the violated constraint indicates that the file was created by another client
  -- while we were trying to create it ourselves. We can thus use the newly created file
  SELECT id, fileSize INTO rid, rfs FROM CastorFile
    WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest SET castorFile = rid
   WHERE id = srId;
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
   WHERE status = tconst.TAPECOPY_SELECTED
     AND castorFile = cfId;
  IF nbRes > 0 THEN
    -- We found something, thus we cannot recreate
    ret := 1;
    RETURN;
  END IF;
  -- check if recreation is possible for SubRequests
  SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ count(*) INTO nbRes FROM SubRequest
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
  FOR sr IN (SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ id, status FROM SubRequest
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
    UPDATE DiskCopy
       SET status = 7, -- INVALID
           gcType = inGcType
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
  srIds "numList";
  dcStatus INTEGER;
  nsHostName VARCHAR2(2048);
  migSvcClass NUMBER;
BEGIN
  ret := 0;
  -- Get the stager/nsHost configuration option
  nsHostName := getConfigOption('stager', 'nsHost', nh);
  BEGIN
    -- Lock the access to the CastorFile
    -- This, together with triggers will avoid new TapeCopies
    -- or DiskCopies to be added
    SELECT id, svcClass INTO cfId, migSvcClass FROM CastorFile
     WHERE fileId = fid AND nsHost = nsHostName FOR UPDATE;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- This file does not exist in the stager catalog
    -- so we just fail the request
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
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
      -- and then diskcopies resulting from ongoing requests, for which the previous
      -- query wouldn't return any entry because of e.g. missing filesystem
      SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ DC.id
        FROM (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id
                FROM StagePrepareToPutRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id
                FROM StagePrepareToGetRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id
                FROM StagePrepareToUpdateRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id
                FROM StageRepackRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id
                FROM StagePutRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id
                FROM StageGetRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id
                FROM StageUpdateRequest WHERE svcClass = scId UNION ALL
              SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */ id
                FROM StageDiskCopyReplicaRequest WHERE svcClass = scId) Request,
             SubRequest, DiskCopy DC
       WHERE SubRequest.diskCopy = DC.id
         AND Request.id = SubRequest.request
         AND DC.castorFile = cfId
         AND DC.status IN (1, 2, 5, 11)  -- WAITDISK2DISKCOPY, WAITTAPERECALL, WAITFS, WAITFS_SCHEDULING
      );
    IF dcsToRm.COUNT = 0 THEN
      -- We didn't find anything on this svcClass, fail and return
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = 7,  -- FAILED
             errorCode = 2,  -- ENOENT
             errorMessage = 'File not found on this service class'
       WHERE id = srId;
      RETURN;
    END IF;
    -- Select current status of the diskCopies
    SELECT status INTO dcStatus
      FROM DiskCopy
     WHERE id = dcsToRm(1);
    -- make sure we don't drop the last diskcopy of the original service class
    -- as it is needed to migrate safely. Indeed, nothing can insure that other copies
    -- are in service classes that can migrate to the requested tapepool(s)
    -- In case it is not the case, give up with the deletion.
    IF dcStatus = 10 THEN  -- CANBEMIGR
      SELECT count(*) INTO nbRes
        FROM DiskCopy, FileSystem, DiskPool2SvcClass 
       WHERE DiskCopy.fileSystem = FileSystem.id
         AND FileSystem.diskPool = DiskPool2SvcClass.parent
         AND DiskCopy.castorFile = cfId
         AND DiskCopy.status = 10  -- CANBEMIGR
         AND diskpool2svcclass.child = migSvcClass
         AND DiskCopy.id NOT IN
          (SELECT /*+ CARDINALITY(dcidTable 5) */ * FROM TABLE(dcsToRm) dcidTable);
      IF nbRes = 0 THEN
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = 7,  -- FAILED
               errorCode = 16,  -- EBUSY
               errorMessage = 'As the file is not yet migrated, we cannot drop the last copy in this service class'
         WHERE id = srId;
         RETURN;
      END IF;
    ELSE
      -- Check whether something else is left: if not, do as
      -- if we are performing a stageRm everywhere.
      SELECT count(*) INTO nbRes FROM DiskCopy
         WHERE castorFile = cfId
           AND status IN (0, 2, 5, 6, 10, 11)  -- STAGED, WAITTAPERECALL, STAGEOUT, CANBEMIGR, WAITFS, WAITFS_SCHEDULING
           AND id NOT IN (SELECT /*+ CARDINALITY(dcidTable 5) */ *
                            FROM TABLE(dcsToRm) dcidTable);
      IF nbRes = 0 THEN
        -- nothing found, so we're dropping the last copy; then
        -- we need to perform all the checks to make sure we can
        -- allow the removal.
        scId := 0;
      END IF;
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
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = 7,  -- FAILED
               errorCode = 16,  -- EBUSY
               errorMessage = 'The file is not yet migrated'
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
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = 7,  -- FAILED
             errorCode = 16,  -- EBUSY
             errorMessage = 'The file is being recalled from tape'
       WHERE id = srId;
      RETURN;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Nothing running. We still may have found nothing at all...
      SELECT count(*) INTO nbRes FROM DiskCopy
       WHERE castorFile = cfId
         AND status NOT IN (4, 7, 9);  -- anything but FAILED, INVALID, BEINGDELETED
      IF nbRes = 0 THEN
        UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
           SET status = 7,  -- FAILED
               errorCode = 2,  -- ENOENT
               errorMessage = 'File not found on disk cache'
         WHERE id = srId;
        RETURN;
      END IF;
      
      deleteTapeCopies(cfId);
      -- Reselect what needs to be removed
      SELECT id BULK COLLECT INTO dcsToRm
        FROM DiskCopy
       WHERE castorFile = cfId
         AND status IN (0, 1, 2, 5, 6, 10, 11);  -- STAGED, WAIT*, STAGEOUT, CANBEMIGR
    END;
  END IF;

  -- Now perform the remove:
  -- mark all get/put requests for those diskcopies
  -- and the ones waiting on them as failed
  -- so that clients eventually get an answer
  SELECT /*+ INDEX(SR I_SubRequest_DiskCopy) */ id
    BULK COLLECT INTO srIds
    FROM SubRequest SR
   WHERE diskcopy IN
     (SELECT /*+ CARDINALITY(dcidTable 5) */ * 	 
        FROM TABLE(dcsToRm) dcidTable) 	 
     AND status IN (0, 1, 2, 4, 5, 6, 13); -- START, RESTART, RETRY, WAITTAPERECALL, WAITSUBREQ, READY, READYFORSCHED
  IF srIds.COUNT > 0 THEN
    DECLARE
      srType INTEGER;
      srUuid VARCHAR(2048);
    BEGIN
      FOR i IN srIds.FIRST .. srIds.LAST LOOP
        SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ type, subreqId INTO srType, srUuid
          FROM SubRequest, Id2Type
         WHERE SubRequest.request = Id2Type.id
           AND SubRequest.id = srIds(i);
        UPDATE SubRequest
           SET status = CASE
               WHEN status IN (6, 13) AND srType = 133 THEN 9 ELSE 7
               END,  -- FAILED_FINISHED for DiskCopyReplicaRequests in status READYFORSCHED or READY, otherwise FAILED
               -- this so that user requests in status WAITSUBREQ are always marked FAILED even if they wait on a replication
               errorCode = 4,  -- EINTR
               errorMessage = 'Canceled by another user request'
         WHERE id = srIds(i) OR parent = srIds(i);
        -- make the scheduler aware so that it can remove the transfer from the queues if needed
        INSERT INTO TransfersToAbort VALUES (srUuid);
      END LOOP;
    END;
    -- wake up the scheduler so that it can remove the transfer from the queues now
    DBMS_ALERT.SIGNAL('transfersToAbort', '');
  END IF;
  -- Set selected DiskCopies to either INVALID or FAILED. We deliberately
  -- ignore WAITDISK2DISKCOPY's (see bug #78826)
  FOR i IN dcsToRm.FIRST .. dcsToRm.LAST LOOP
    SELECT status INTO dcStatus
      FROM DiskCopy
     WHERE id = dcsToRm(i);
    IF dcStatus = 1 THEN
      NULL;  -- Do nothing
    ELSE
      UPDATE DiskCopy
         -- WAITTAPERECALL,WAITFS[_SCHED] -> FAILED, others -> INVALID
         SET status = decode(status, 2,4, 5,4, 11,4, 7)
       WHERE id = dcsToRm(i);
    END IF;
  END LOOP;
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ Id2Type.id INTO reqId
    FROM SubRequest, Id2Type
   WHERE SubRequest.id = srId
     AND Id2Type.id = SubRequest.request
     FOR UPDATE OF Id2Type.id;
  -- Update Status
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET status = newStatus,
         answered = 1,
         lastModificationTime = getTime(),
         getNextStatus = decode(newStatus, 6, 1, 8, 1, 9, 1, 0)  -- READY, FINISHED or FAILED_FINISHED -> GETNEXTSTATUS_FILESTAGED
   WHERE id = srId;
  -- Check whether it was the last subrequest in the request
  SELECT /*+ INDEX(Subrequest I_Subrequest_Request)*/ id INTO result FROM SubRequest
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

/* PL/SQL function to elect a rmmaster daemon in NoLSFMode */
CREATE OR REPLACE FUNCTION isMonitoringMaster RETURN NUMBER IS
  locked EXCEPTION;
  PRAGMA EXCEPTION_INIT (locked, -54);
BEGIN
  LOCK TABLE RmMasterLock IN EXCLUSIVE MODE NOWAIT;
  RETURN 1;
EXCEPTION WHEN locked THEN
  RETURN 0;
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ castorfile, diskCopy
    INTO cfId, dcId
    FROM SubRequest WHERE id = srId;
  SELECT fileclass INTO fclassId FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Check that the file is not busy, i.e. that we are not
  -- in the middle of migrating it. If we are, just stop and raise
  -- a user exception
  SELECT count(*) INTO nbRes FROM TapeCopy
    WHERE status = tconst.TAPECOPY_SELECTED
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id) INDEX(Request PK_StageUpdateRequest_Id) */ svcClass INTO sclassId
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
      SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ count(*) INTO nbRes
        FROM SubRequest
       WHERE diskCopy = dcId AND id != srId;
      IF (nbRes > 0) THEN
        -- do we have a prepareTo Request ? There can be only a single one
        -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
        -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
        SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest.id INTO nbRes
          FROM (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id FROM StagePrepareToPutRequest UNION ALL
                SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id FROM StagePrepareToUpdateRequest) PrepareRequest,
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
         SubRequest.diskCopy, SubRequest.status, DiskCopy.fileSystem,
         Request.svcClass
    INTO rdcId, srStatus, prevFsId, srSvcClass
    FROM SubRequest, DiskCopy,
         (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id, svcClass FROM StagePutRequest UNION ALL
          SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, svcClass FROM StageGetRequest UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, svcClass FROM StageUpdateRequest) Request
   WHERE SubRequest.diskcopy = Diskcopy.id
     AND SubRequest.id = srId
     AND SubRequest.request = Request.id;
  -- Check that we did not cancel the SubRequest in the mean time
  IF srStatus IN (7, 9, 10) THEN -- FAILED, FAILED_FINISHED, FAILED_ANSWERING
    raise_application_error(-20104, 'SubRequest canceled while queuing in scheduler. Giving up.');
  END IF;
  -- Check to see if the proposed diskserver and filesystem selected by the
  -- scheduler to run the job is in the correct service class.
  -- XXX deprecated to be removed when LSF is dropped
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
  -- In case the DiskCopy was in WAITFS_SCHEDULING,
  -- restart the waiting SubRequests
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Parent)*/ SubRequest
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ euid, egid, svcClass, upd, diskCopy
    INTO reuid, regid, srSvcClass, isUpd, dcIdInReq
    FROM SubRequest,
        (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, euid, egid, svcClass, 0 AS upd FROM StageGetRequest UNION ALL
         SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, euid, egid, svcClass, 1 AS upd FROM StageUpdateRequest) Request
   WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
  -- Take a lock on the CastorFile. Associated with triggers,
  -- this guarantees we are the only ones dealing with its copies
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ CastorFile.fileSize, CastorFile.id
    INTO fileSize, cfId
    FROM CastorFile, SubRequest
   WHERE CastorFile.id = SubRequest.castorFile
     AND SubRequest.id = srId FOR UPDATE OF CastorFile;
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
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET DiskCopy = dci
   WHERE id = srId RETURNING protocol INTO proto;
  -- In case of an update, if the protocol used does not support
  -- updates properly (via firstByteWritten call), we should
  -- call firstByteWritten now and consider that the file is being
  -- modified.
  IF isUpd = 1 THEN
    handleProtoNoUpd(srId, proto);
  END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- No disk copy found on selected FileSystem. This can happen in 2 cases :
  --  + either a diskcopy was available and got disabled before this job
  --    was scheduled. Bad luck, we fail the request, the user will have to retry
  --  + or we are an update creating a file and there is a diskcopy in WAITFS
  --    or WAITFS_SCHEDULING associated to us. Then we have to call putStart
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
  -- It was not an update creating a file, so we fail
  raise_application_error(-20114, 'File invalidated while queuing in the scheduler, please try again');
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
    SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy) INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */
           SubRequest.status, StageDiskCopyReplicaRequest.svcClassName
      INTO unused, destSvcClass
      FROM SubRequest, StageDiskCopyReplicaRequest
     WHERE SubRequest.diskcopy = dcId
       AND SubRequest.request = StageDiskCopyReplicaRequest.id
       AND SubRequest.status = dconst.SUBREQUEST_READY;
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
(dcId IN INTEGER, srcDcId IN INTEGER, replicaFileSize IN INTEGER) AS
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
  srcFsId := -1;
  BEGIN
    SELECT status, gcWeight, diskCopySize, fileSystem
      INTO srcStatus, gcw, fileSize, srcFsId
      FROM DiskCopy
     WHERE id = srcDcId
       AND status IN (0, 10);  -- STAGED, CANBEMIGR
  EXCEPTION WHEN NO_DATA_FOUND THEN
    NULL;
  END;
  -- If no diskcopy was returned it means that the source has either:
  --   A) Been garbage collected while the copying was taking place OR
  --   B) The diskcopy is no longer in a STAGED or CANBEMIGR state. As
  --      A result we do not know which status to put the new copy in
  --      and/or cannot trust that the file was not modified mid transfer
  --
  -- If a diskcopy was returned but the size of the original file in
  -- comparison to the replica is different then some corruption has
  -- occurred and the new copy should not be kept
  --
  -- In all cases we invalidate the new copy!
  IF (srcFsId IS NULL) OR
     (srcFsId IS NOT NULL AND fileSize != replicaFileSize) THEN
    -- Begin the process of invalidating the file replica
    UPDATE DiskCopy SET status = 7 WHERE id = dcId -- INVALID
    RETURNING CastorFile INTO cfId;
    -- Restart the SubRequests waiting for the copy
    UPDATE /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ SubRequest
       SET status = 1, -- SUBREQUEST_RESTART
           lastModificationTime = getTime()
     WHERE diskCopy = dcId RETURNING id INTO srId;
    UPDATE /*+ INDEX(Subrequest I_Subrequest_Parent)*/ SubRequest
       SET status = 1,
           getNextStatus = 1, -- GETNEXTSTATUS_FILESTAGED
           lastModificationTime = getTime(),
           parent = 0
     WHERE parent = srId; -- SUBREQUEST_RESTART
    -- Archive the diskCopy replica request, status FAILED_FINISHED
    -- for abnormal transfer termination
    IF (srcFsId IS NOT NULL AND fileSize != replicaFileSize) THEN
      archiveSubReq(srId, 9);  -- FAILED_FINISHED
    ELSE
      archiveSubReq(srId, 8);  -- FINISHED
    END IF;
    -- Restart all entries in the snapshot of files to drain that may be
    -- waiting on the replication of the source diskcopy.
    UPDATE DrainingDiskCopy
       SET status = 1,  -- RESTARTED
           parent = 0
     WHERE status = 3  -- RUNNING
       AND (diskCopy = srcDcId
        OR  parent = srcDcId);
    drainFileSystem(srcFsId);
    -- If a file size mismatch has occurred raise an exception which
    -- will be logged by the d2dtransfer mover.
    IF (srcFsId IS NOT NULL AND fileSize != replicaFileSize) THEN
      -- Commit the invalidation of the replica. If we dont the raising of
      -- an application_error will trigger a rollback and the diskcopy will
      -- be stuck in WAITDISK2DISKCOPY
      COMMIT;
      raise_application_error(-20119, 'File replication size mismatch: (original size: '||fileSize||' - replica size: '||replicaFileSize||')');
    END IF;
    RETURN;
  END IF;
  -- The new replica looks OK, so lets keept it!
  -- update SubRequest and get data
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Diskcopy)*/ SubRequest
     SET status = 6, -- SUBREQUEST_READY
         lastModificationTime = getTime()
   WHERE diskCopy = dcId RETURNING id, protocol, request
    INTO srId, proto, reqId;
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id) INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */
         SvcClass.id INTO svcClassId
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
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Parent)*/ SubRequest
     SET status = 1,  -- SUBREQUEST_RESTART
         getNextStatus = 1, -- GETNEXTSTATUS_FILESTAGED
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
(dcId IN INTEGER, enoent IN INTEGER) AS
  fsId    NUMBER;
  cfId    NUMBER;
  ouid    INTEGER;
  ogid    INTEGER;
  srcDcId NUMBER;
  srcFsId NUMBER;
  srId    NUMBER;
BEGIN
  fsId := 0;
  srcFsId := -1;
  -- Lock the CastorFile
  SELECT id INTO cfId FROM CastorFile
   WHERE id =
    (SELECT castorFile
       FROM DiskCopy
      WHERE id = dcId) FOR UPDATE;
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
  -- Handle SubRequests
  BEGIN
    -- Get the corresponding subRequest, if it exists
    SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ id INTO srId
      FROM SubRequest
     WHERE diskCopy = dcId
       AND status IN (6, 14); -- READY, BEINGSHCED
    -- Wake up other subrequests waiting on it
    UPDATE /*+ INDEX(Subrequest I_Subrequest_Parent)*/ SubRequest
       SET status = 1, -- RESTART
           parent = 0
     WHERE parent = srId;
    -- Fail it
    archiveSubReq(srId, 9); -- FAILED_FINISHED
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- SubRequest not found, don't trigger replicateOnClose here
    -- as we may have been restarted
    NULL;
  END;
  -- Handle draining logic
  BEGIN
    -- Determine the source diskcopy and filesystem involved in the replication
    SELECT /*+ INDEX(StageDiskCopyReplicaRequest I_StageDiskCopyReplic_DestDC) */ sourceDiskCopy, fileSystem
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
  EXCEPTION WHEN NO_DATA_FOUND THEN
    NULL;
  END;
  -- Handle replication on close
  BEGIN
    -- Trigger the creation of additional copies of the file, if necessary.
    -- Note: We do this also on failure to be able to recover from transient
    -- errors, e.g. timeouts while waiting to be scheduled, but we don't on ENOENT.
    IF enoent = 0 THEN
      replicateOnClose(cfId, ouid, ogid);
    END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    NULL;
  END;
  -- If no filesystem was set on the diskcopy then we can safely delete it
  -- without waiting for garbage collection as the transfer was never started
  IF fsId = 0 THEN
    DELETE FROM DiskCopy WHERE id = dcId;
    DELETE FROM Id2Type WHERE id = dcId;
  END IF;
  -- Continue draining process
  drainFileSystem(srcFsId);
  -- WARNING: previous call to drainFileSystem has a COMMIT inside. So all
  -- locks have been released!!
END;
/


/* PL/SQL method implementing prepareForMigration
   returnCode can take 2 values :
    - 0 : Nothing special
    - 1 : The file got deleted while it was being written to
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
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ castorFile, diskCopy INTO cfId, dcId
    FROM SubRequest WHERE id = srId;
  -- Lock the CastorFile and get the fileid and name server host
  SELECT id, fileid, nsHost INTO cfId, fId, nh
    FROM CastorFile WHERE id = cfId FOR UPDATE;
  -- Determine the context (Put inside PrepareToPut or not)
  -- check that we are a Put or an Update
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ Request.id INTO unused
    FROM SubRequest,
       (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id FROM StagePutRequest UNION ALL
        SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id FROM StageUpdateRequest) Request
   WHERE SubRequest.id = srId
     AND Request.id = SubRequest.request;
  BEGIN
    -- Check that there is a PrepareToPut/Update going on. There can be only a
    -- single one or none. If there was a PrepareTo, any subsequent PPut would
    -- be rejected and any subsequent PUpdate would be directly archived (cf.
    -- processPrepareRequest).
    SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest.diskCopy INTO unused
      FROM SubRequest,
       (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id FROM StagePrepareToPutRequest UNION ALL
        SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id FROM StagePrepareToUpdateRequest) Request
     WHERE SubRequest.CastorFile = cfId
       AND Request.id = SubRequest.request
       AND SubRequest.status = 6; -- READY
    -- If we got here, we are a Put inside a PrepareToPut
    contextPIPP := 0;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- Here we are a standalone Put
    contextPIPP := 1;
  END;
  -- Check whether the diskCopy is still in STAGEOUT. If not, the file
  -- was deleted via stageRm while being written to. Thus, we should just give
  -- up
  BEGIN
    SELECT status INTO unused
      FROM DiskCopy WHERE id = dcId AND status = 6; -- STAGEOUT
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- So we are in the case, we give up
    returnCode := 1;
    RETURN;
  END;
  -- Now we can safely update CastorFile's file size
  UPDATE CastorFile SET fileSize = fs, lastUpdateTime = ts
   WHERE id = cfId AND (lastUpdateTime IS NULL OR ts >= lastUpdateTime);
  -- If ts < lastUpdateTime, we were late and another job already updated the
  -- CastorFile. So we nevertheless retrieve the real file size.
  SELECT fileSize INTO realFileSize FROM CastorFile WHERE id = cfId;
  -- Get svcclass from Request
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ svcClass INTO svcId
    FROM SubRequest,
      (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id, svcClass FROM StagePutRequest          UNION ALL
       SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, svcClass FROM StageUpdateRequest UNION ALL
       SELECT /*+ INDEX(StagePutDoneRequest PK_StagePutDoneRequest_Id) */ id, svcClass FROM StagePutDoneRequest) Request
   WHERE SubRequest.request = Request.id AND SubRequest.id = srId;
  IF contextPIPP != 0 THEN
    -- If not a put inside a PrepareToPut/Update, create TapeCopies
    -- and update DiskCopy status
    putDoneFunc(cfId, realFileSize, contextPIPP, svcId);
  ELSE
    -- If put inside PrepareToPut/Update, restart any PutDone currently
    -- waiting on this put/update
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = 1, parent = 0 -- RESTART
     WHERE id IN
      (SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest.id
         FROM SubRequest, Id2Type
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
CREATE OR REPLACE PROCEDURE getUpdateFailedProcExt
(srId IN NUMBER, errno IN NUMBER, errmsg IN VARCHAR2) AS
BEGIN
  -- Fail the subrequest. The stager will try and answer the client
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET status = 7, -- FAILED
         errorCode = errno,
         errorMessage = errmsg
   WHERE id = srId;
  -- Wake up other subrequests waiting on it
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Parent)*/ SubRequest
     SET parent = 0, status = 1, -- RESTART
         lastModificationTime = getTime()
   WHERE parent = srId;
END;
/

CREATE OR REPLACE PROCEDURE getUpdateFailedProc
(srId IN NUMBER) AS
BEGIN
  getUpdateFailedProcExt(srId, 1015, 'Job terminated with failure');  -- SEINTERNAL
END;
/

/* PL/SQL method implementing putFailedProc */
CREATE OR REPLACE PROCEDURE putFailedProcExt(srId IN NUMBER, errno IN NUMBER, errmsg IN VARCHAR2) AS
  dcId INTEGER;
  cfId INTEGER;
  unused INTEGER;
BEGIN
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ diskCopy, castorFile
    INTO dcId, cfId
    FROM SubRequest
   WHERE id = srId;
  -- Fail the subRequest
  UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
     SET status = 7, -- FAILED
         errorCode = errno,
         errorMessage = errmsg
   WHERE id = srId;
  -- Determine the context (Put inside PrepareToPut/Update ?)
  BEGIN
    -- Check that there is a PrepareToPut/Update going on. There can be only a single one
    -- or none. If there was a PrepareTo, any subsequent PPut would be rejected and any
    -- subsequent PUpdate would be directly archived (cf. processPrepareRequest).
    SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest.id INTO unused
      FROM (SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id FROM StagePrepareToPutRequest UNION ALL
            SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id FROM StagePrepareToUpdateRequest) PrepareRequest, SubRequest
     WHERE SubRequest.castorFile = cfId
       AND PrepareRequest.id = SubRequest.request
       AND SubRequest.status = 6; -- READY
    -- The select worked out, so we have a prepareToPut/Update
    -- In such a case, we do not cleanup DiskCopy and CastorFile
    -- but we have to wake up a potential waiting putDone
    UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
       SET status = 1, parent = 0 -- RESTART
     WHERE id IN
      (SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile) INDEX(StagePutDoneRequest PK_StagePutDoneRequest_Id) */ SubRequest.id
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

/* PL/SQL method implementing putFailedProc */
CREATE OR REPLACE PROCEDURE putFailedProc(srId IN NUMBER) AS
BEGIN
  putFailedProcExt(srId, 1015, 'Job terminated with failure');  -- SEINTERNAL
END;
/

/* PL/SQL function implementing checkAvailOfSchedulerRFS, this function can be
 * used to determine if any of the requested filesystems specified by a
 * transfer are available. Returns 0 if at least one requested filesystem is
 * available otherwise 1.
 * This is deprecated and should go when the jobmanager and LSF are dropped
 */
CREATE OR REPLACE FUNCTION checkAvailOfSchedulerRFS
  (rfs IN VARCHAR2, reqType IN NUMBER)
RETURN NUMBER IS
  rtn NUMBER;
BEGIN
  -- Count the number of requested filesystems which are available
  SELECT count(*) INTO rtn
    FROM DiskServer, FileSystem
   WHERE DiskServer.id = FileSystem.diskServer
     AND DiskServer.name || ':' || FileSystem.mountPoint IN
       (SELECT /*+ CARDINALITY(rfsTable 10) */ *
          FROM TABLE (strTokenizer(rfs, '|')) rfsTable)
     -- For a requested filesystem to be available the following criteria
     -- must be meet:
     --  - The diskserver and filesystem must not be in a DISABLED state
     --  - For StageDiskCopyReplicaRequests all other states are accepted
     --  - For all other requests the diskserver and filesystem must be in
     --    PRODUCTION
     AND decode(DiskServer.status, 2, 1,    -- Exclude DISABLED Diskservers
           decode(FileSystem.status, 2, 1,  -- Exclude DISABLED Filesystems
             decode(reqType, 133, 0,
                 decode(FileSystem.status + DiskServer.status, 0, 0, 1)))) = 0;
  IF rtn > 0 THEN
    RETURN 0;  -- We found some available requested filesystems
  END IF;
  RETURN 1;
END;
/


/* PL/SQL method implementing getSchedulerJobs. This method lists all known
 * transfers and whether they should be terminated because no space exists
 * in the target service class or none of the requested filesystems are
 * available.
 * This is deprecated and should go when the jobmanager and LSF are dropped
 * It has been replaced by getSchedulerJobs2
 */
CREATE OR REPLACE PROCEDURE getSchedulerJobs
  (transfers OUT castor.SchedulerJobs_Cur) AS
BEGIN
  OPEN transfers FOR
    -- Use the NO_MERGE hint to prevent Oracle from executing the
    -- checkFailJobsWhenNoSpace function for every row in the output. In
    -- situations where there are many PENDING transfers in the scheduler
    -- this can be extremely inefficient and expensive.
    SELECT /*+ NO_MERGE(NoSpSvc) */  -- NoSpSvc is a SvcClass table alias
           SR.subReqId, Request.reqId, NoSpSvc.NoSpace,
           -- If there are no requested filesystems, refer to the NFSSvc
           -- output otherwise call the checkAvailOfSchedulerRFS function
           decode(SR.requestedFileSystems, NULL, NoFSSvc.NoFSAvail,
             checkAvailOfSchedulerRFS(SR.requestedFileSystems,
                                      Request.reqType)) NoFSAvail
      FROM SubRequest SR,
        -- Union of all requests that could result in scheduler transfers
        (SELECT id, svcClass, reqid, 40  AS reqType
           FROM StagePutRequest                     UNION ALL
         SELECT id, svcClass, reqid, 133 AS reqType
           FROM StageDiskCopyReplicaRequest         UNION ALL
         SELECT id, svcClass, reqid, 35  AS reqType
           FROM StageGetRequest                     UNION ALL
         SELECT id, svcClass, reqid, 44  AS reqType
           FROM StageUpdateRequest) Request,
        -- Table of all service classes with a boolean flag to indicate
        -- if space is available
        (SELECT id, checkFailJobsWhenNoSpace(id) NoSpace
           FROM SvcClass) NoSpSvc,
        -- Table of all service classes with a boolean flag to indicate
        -- if there are any filesystems in PRODUCTION
        (SELECT id, nvl(NoFSAvail, 1) NoFSAvail FROM SvcClass
           LEFT JOIN
             (SELECT DP2Svc.child, decode(count(*), 0, 1, 0) NoFSAvail
                FROM DiskServer DS, FileSystem FS, DiskPool2SvcClass DP2Svc
               WHERE DS.ID = FS.diskServer
                 AND DS.status = 0  -- DISKSERVER_PRODUCTION
                 AND FS.diskPool = DP2Svc.parent
                 AND FS.status = 0  -- FILESYSTEM_PRODUCTION
               GROUP BY DP2Svc.child) results
             ON SvcClass.id = results.child) NoFSSvc
     WHERE SR.status = 6  -- READY
       AND SR.request = Request.id
       AND SR.lastModificationTime < getTime() - 60
       AND NoSpSvc.id = Request.svcClass
       AND NoFSSvc.id = Request.svcClass;
END;
/

/* PL/SQL method implementing getSchedulerTransfers.
   This method lists all known transfers
   that are started/pending for more than an hour */
CREATE OR REPLACE PROCEDURE getSchedulerTransfers
  (transfers OUT castor.UUIDPairRecord_Cur) AS
BEGIN
  OPEN transfers FOR
    SELECT SR.subReqId, Request.reqid
      FROM SubRequest SR,
        -- Union of all requests that could result in scheduler transfers
        (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */
                id, svcClass, reqid, 40  AS reqType FROM StagePutRequest             UNION ALL
         SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */
                id, svcClass, reqid, 133 AS reqType FROM StageDiskCopyReplicaRequest UNION ALL
         SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */
                id, svcClass, reqid, 35  AS reqType FROM StageGetRequest             UNION ALL
         SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                id, svcClass, reqid, 44  AS reqType FROM StageUpdateRequest) Request
     WHERE SR.status = 6  -- READY
       AND SR.request = Request.id
       AND SR.lastModificationTime < getTime() - 3600;
END;
/

/* PL/SQL method implementing getSchedulerD2dTransfers.
   This method lists all running D2d transfers */
CREATE OR REPLACE PROCEDURE getSchedulerD2dTransfers
  (transfers OUT castor.UUIDPairRecord_Cur) AS
BEGIN
  OPEN transfers FOR
    SELECT SR.subReqId, Request.reqid
      FROM SubRequest SR, StageDiskCopyReplicaRequest Request
     WHERE SR.status = 6  -- READY
       AND SR.request = Request.id;
END;
/

/* PL/SQL method implementing getFileIdsForSrs.
   This method returns the list of fileids associated to the given list of
   subrequests */
CREATE OR REPLACE PROCEDURE getFileIdsForSrs
  (subReqIds IN castor."strList", fileids OUT castor.FileEntry_Cur) AS
  fid NUMBER;
  nh VARCHAR(2048);
BEGIN
  FOR i IN subReqIds.FIRST .. subReqIds.LAST LOOP
    SELECT /*+ INDEX(Subrequest I_Subrequest_SubreqId)*/ fileid, nsHost INTO fid, nh
      FROM Castorfile, SubRequest
     WHERE SubRequest.subreqId = subReqIds(i)
       AND SubRequest.castorFile = CastorFile.id;
    INSERT INTO getFileIdsForSrsHelper VALUES (i, fid, nh);
  END LOOP;
  OPEN fileids FOR SELECT nh, fileid FROM getFileIdsForSrsHelper ORDER BY rowno;
END;
/

/* PL/SQL method implementing jobFailed, providing bulk termination of file
 * transfers.
 * This is deprecated and should go when the jobmanager and LSF are dropped.
 * It has been replaced by jobFailedLockedFile and jobFailedSafe
 */
CREATE OR REPLACE
PROCEDURE jobFailed(subReqIds IN castor."strList", errnos IN castor."cnumList",
                    failedSubReqs OUT castor.JobFailedSubReqList_Cur)
AS
  srId  NUMBER;
  dcId  NUMBER;
  cfId  NUMBER;
  rType NUMBER;
BEGIN
  -- Clear the temporary table
  DELETE FROM JobFailedProcHelper;
  -- Loop over all jobs to fail
  FOR i IN subReqIds.FIRST .. subReqIds.LAST LOOP
    BEGIN
      -- Get the necessary information needed about the request.
      SELECT /*+ INDEX(Subrequest I_Subrequest_SubreqId)*/ SubRequest.id, SubRequest.diskCopy,
             Id2Type.type, SubRequest.castorFile
        INTO srId, dcId, rType, cfId
        FROM SubRequest, Id2Type
       WHERE SubRequest.subReqId = subReqIds(i)
         AND SubRequest.status IN (6, 14)  -- READY, BEINGSCHED
         AND SubRequest.request = Id2Type.id;
       -- Lock the CastorFile.
       SELECT id INTO cfId FROM CastorFile
        WHERE id = cfId FOR UPDATE;
       -- Confirm SubRequest status hasn't changed after acquisition of lock
       SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ id INTO srId FROM SubRequest
        WHERE id = srId AND status IN (6, 14);  -- READY, BEINGSCHED
       -- Call the relevant cleanup procedure for the job, procedures that
       -- would have been called if the job failed on the remote execution host.
       IF rType = 40 THEN      -- StagePutRequest
         putFailedProc(srId);
       ELSIF rType = 133 THEN  -- StageDiskCopyReplicaRequest
         disk2DiskCopyFailed(dcId, 0);
       ELSE                    -- StageGetRequest or StageUpdateRequest
         getUpdateFailedProc(srId);
       END IF;
       -- Update the reason for termination, overriding the error code set above
       UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
          SET errorCode = decode(errnos(i), 0, errorCode, errnos(i)),
              errorMessage = ''
        WHERE id = srId;
       -- Record in the JobFailedProcHelper temporary table that an action was
       -- taken
       INSERT INTO JobFailedProcHelper VALUES (subReqIds(i));
       -- Release locks
       COMMIT;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      NULL;  -- The SubRequest may have be removed, nothing to be done.
    END;
  END LOOP;
  -- Return the list of terminated jobs
  OPEN failedSubReqs FOR
    SELECT subReqId FROM JobFailedProcHelper;
END;
/

/* PL/SQL method implementing transferFailedSafe, providing bulk termination of file
 * transfers.
 */
CREATE OR REPLACE
PROCEDURE transferFailedSafe(subReqIds IN castor."strList",
                             errnos IN castor."cnumList",
                             errmsg IN castor."strList") AS
  srId  NUMBER;
  dcId  NUMBER;
  cfId  NUMBER;
  rType NUMBER;
BEGIN
  -- give up if nothing to be done
  IF subReqIds.COUNT = 0 THEN RETURN; END IF;
  -- Loop over all transfers to fail
  FOR i IN subReqIds.FIRST .. subReqIds.LAST LOOP
    BEGIN
      -- Get the necessary information needed about the request.
      SELECT /*+ INDEX(Subrequest I_Subrequest_SubreqId)*/ SubRequest.id, SubRequest.diskCopy,
             Id2Type.type, SubRequest.castorFile
        INTO srId, dcId, rType, cfId
        FROM SubRequest, Id2Type
       WHERE SubRequest.subReqId = subReqIds(i)
         AND SubRequest.status IN (6, 14)  -- READY, BEINGSCHED
         AND SubRequest.request = Id2Type.id;
       -- Lock the CastorFile.
       SELECT id INTO cfId FROM CastorFile
        WHERE id = cfId FOR UPDATE;
       -- Confirm SubRequest status hasn't changed after acquisition of lock
       SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/ id INTO srId FROM SubRequest
        WHERE id = srId AND status IN (6, 14);  -- READY, BEINGSCHED
       -- Call the relevant cleanup procedure for the transfer, procedures that
       -- would have been called if the transfer failed on the remote execution host.
       IF rType = 40 THEN      -- StagePutRequest
         putFailedProc(srId);
       ELSIF rType = 133 THEN  -- StageDiskCopyReplicaRequest
         disk2DiskCopyFailed(dcId, 0);
       ELSE                    -- StageGetRequest or StageUpdateRequest
         getUpdateFailedProc(srId);
       END IF;
       -- Update the reason for termination, overriding the error code set above
       UPDATE SubRequest
          SET errorCode = decode(errnos(i), 0, errorCode, errnos(i)),
              errorMessage = decode(errmsg(i), NULL, errorMessage, errmsg(i))
        WHERE id = srId;
       -- Release locks
       COMMIT;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      NULL;  -- The SubRequest may have be removed, nothing to be done.
    END;
  END LOOP;
END;
/

/* PL/SQL method implementing transferFailedLockedFile, providing bulk termination of file
 * transfers. in case the castorfile is already locked
 */
CREATE OR REPLACE
PROCEDURE transferFailedLockedFile(subReqId IN castor."strList",
                                   errno IN castor."cnumList",
                                   errmsg IN castor."strList")
AS
  srId  NUMBER;
  dcId  NUMBER;
  rType NUMBER;
BEGIN
  FOR i IN subReqId.FIRST .. subReqId.LAST LOOP
    BEGIN
      -- Get the necessary information needed about the request.
      SELECT SubRequest.id, SubRequest.diskCopy, Id2Type.type
        INTO srId, dcId, rType
        FROM SubRequest, Id2Type
       WHERE SubRequest.subReqId = subReqId(i)
         AND SubRequest.status IN (6, 14)  -- READY, BEINGSCHED
         AND SubRequest.request = Id2Type.id;
       -- Update the reason for termination.
       UPDATE SubRequest
          SET errorCode = decode(errno(i), 0, errorCode, errno(i)),
              errorMessage = decode(errmsg(i), NULL, errorMessage, errmsg(i))
        WHERE id = srId;
       -- Call the relevant cleanup procedure for the transfer, procedures that
       -- would have been called if the transfer failed on the remote execution host.
       IF rType = 40 THEN      -- StagePutRequest
         putFailedProc(srId);
       ELSIF rType = 133 THEN  -- StageDiskCopyReplicaRequest
         disk2DiskCopyFailed(dcId, 0);
       ELSE                    -- StageGetRequest or StageUpdateRequest
         getUpdateFailedProc(srId);
       END IF;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      NULL;  -- The SubRequest may have be removed, nothing to be done.
    END;
  END LOOP;
END;
/

/* PL/SQL method implementing jobToSchedule
 * This is deprecated and should go when the jobmanager and LSF are dropped.
 * It has been replaced by transferToSchedule
 */
CREATE OR REPLACE
PROCEDURE jobToSchedule(srId OUT INTEGER,              srSubReqId OUT VARCHAR2,
                        srProtocol OUT VARCHAR2,       srXsize OUT INTEGER,
                        srRfs OUT VARCHAR2,            reqId OUT VARCHAR2,
                        cfFileId OUT INTEGER,          cfNsHost OUT VARCHAR2,
                        reqSvcClass OUT VARCHAR2,      reqType OUT INTEGER,
                        reqEuid OUT INTEGER,           reqEgid OUT INTEGER,
                        reqUsername OUT VARCHAR2,      srOpenFlags OUT VARCHAR2,
                        clientIp OUT INTEGER,          clientPort OUT INTEGER,
                        clientVersion OUT INTEGER,     clientType OUT INTEGER,
                        reqSourceDiskCopy OUT INTEGER, reqDestDiskCopy OUT INTEGER,
                        clientSecure OUT INTEGER,      reqSourceSvcClass OUT VARCHAR2,
                        reqCreationTime OUT INTEGER,   reqDefaultFileSize OUT INTEGER,
                        excludedHosts OUT castor.DiskServerList_Cur) AS
  cfId NUMBER;
  -- Cursor to select the next candidate for submission to the scheduler orderd
  -- by creation time.
  CURSOR c IS
    SELECT /*+ FIRST_ROWS(10) INDEX(SR I_SubRequest_RT_CT_ID) */ SR.id
      FROM SubRequest
 PARTITION (P_STATUS_13_14) SR  -- RESTART, READYFORSCHED, BEINGSCHED
     ORDER BY SR.creationTime ASC;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  srIntId NUMBER;
BEGIN
  -- Loop on all candidates for submission into LSF
  OPEN c;
  LOOP
    -- Retrieve the next candidate
    FETCH c INTO srIntId;
    IF c%NOTFOUND THEN
      -- There are no candidates available, return a srId of 0, this indicates
      -- to the job manager that there is nothing to do. The jobt manager will
      -- try again shortly.
      RETURN;
    END IF;
    BEGIN
      -- Try to lock the current candidate, verify that the status is valid. A
      -- valid subrequest is either in READYFORSCHED or has been stuck in
      -- BEINGSCHED for more than 1800 seconds (30 mins)
      SELECT /*+ INDEX(SR PK_SubRequest_ID) */ id INTO srIntId
        FROM SubRequest PARTITION (P_STATUS_13_14) SR
       WHERE id = srIntId
         AND ((status = 13)  -- READYFORSCHED
          OR  (status = 14   -- BEINGSCHED
         AND lastModificationTime < getTime() - 1800))
         FOR UPDATE;
      -- We have successfully acquired the lock, so we update the subrequest
      -- status and modification time
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = 14,  -- BEINGSHCED
             lastModificationTime = getTime()
       WHERE id = srIntId
      RETURNING id, subReqId, protocol, xsize, requestedFileSystems
        INTO srId, srSubReqId, srProtocol, srXsize, srRfs;
      EXIT;
    EXCEPTION
      -- Try again, either we failed to accquire the lock on the subrequest or
      -- the subrequest being processed is not the correct state
      WHEN NO_DATA_FOUND THEN
        NULL;
      WHEN SrLocked THEN
        NULL;
    END;
  END LOOP;
  CLOSE c;

  -- Extract the rest of the information required to submit a job into the
  -- scheduler through the job manager.
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
         CastorFile.id, CastorFile.fileId, CastorFile.nsHost, SvcClass.name,
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
         (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */
                 id, username, euid, egid, reqid, client, creationTime,
                 'w' direction, svcClass, NULL sourceDiskCopy,
                 NULL destDiskCopy, NULL sourceSvcClass
            FROM StagePutRequest
           UNION ALL
          SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ 
                 id, username, euid, egid, reqid, client, creationTime,
                 'r' direction, svcClass, NULL sourceDiskCopy,
                 NULL destDiskCopy, NULL sourceSvcClass
            FROM StageGetRequest
           UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ 
                 id, username, euid, egid, reqid, client, creationTime,
                 'o' direction, svcClass, NULL sourceDiskCopy,
                 NULL destDiskCopy, NULL sourceSvcClass
            FROM StageUpdateRequest
           UNION ALL
          SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */
                 id, username, euid, egid, reqid, client, creationTime,
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

CREATE OR REPLACE TRIGGER tr_SubRequest_informSchedReady AFTER UPDATE OF status ON SubRequest
FOR EACH ROW WHEN (new.status = 13) -- SUBREQUEST_READYFORSCHED
BEGIN
  DBMS_ALERT.SIGNAL('transferReadyToSchedule', '');
END;
/

/* PL/SQL method implementing transferToSchedule */
CREATE OR REPLACE
PROCEDURE transferToSchedule(srId OUT INTEGER,              srSubReqId OUT VARCHAR2,
                             srProtocol OUT VARCHAR2,       srXsize OUT INTEGER,
                             srRfs OUT VARCHAR2,            reqId OUT VARCHAR2,
                             cfFileId OUT INTEGER,          cfNsHost OUT VARCHAR2,
                             reqSvcClass OUT VARCHAR2,      reqType OUT INTEGER,
                             reqEuid OUT INTEGER,           reqEgid OUT INTEGER,
                             reqUsername OUT VARCHAR2,      srOpenFlags OUT VARCHAR2,
                             clientIp OUT INTEGER,          clientPort OUT INTEGER,
                             clientVersion OUT INTEGER,     clientType OUT INTEGER,
                             reqSourceDiskCopy OUT INTEGER, reqDestDiskCopy OUT INTEGER,
                             clientSecure OUT INTEGER,      reqSourceSvcClass OUT VARCHAR2,
                             reqCreationTime OUT INTEGER,   reqDefaultFileSize OUT INTEGER,
                             sourceRfs OUT VARCHAR2) AS
  cfId NUMBER;
  -- Cursor to select the next candidate for submission to the scheduler orderd
  -- by creation time.
  CURSOR c IS
    SELECT /*+ FIRST_ROWS(10) INDEX(SR I_SubRequest_CT_ID) */ SR.id
      FROM SubRequest
 PARTITION (P_STATUS_13_14) SR  -- RESTART, READYFORSCHED, BEINGSCHED
     ORDER BY status ASC, SR.creationTime ASC;
  SrLocked EXCEPTION;
  PRAGMA EXCEPTION_INIT (SrLocked, -54);
  srIntId NUMBER;
  svcClassId NUMBER;
  unusedMessage VARCHAR2(2048);
  unusedStatus INTEGER;
BEGIN
  -- Open a cursor on potential candidates
  OPEN c;
  -- Retrieve the first candidate
  FETCH c INTO srIntId;
  IF c%NOTFOUND THEN
    -- There is no candidate available. Wait for next alert concerning something
    -- to schedule for a maximum of 3 seconds.
    -- We do not wait forever in order to ensure that we will retry from time to
    -- time to dig out candidates that timed out in status BEINGSCHED. Plus we
    -- need to give the control back to the caller daemon in case it should exit
    CLOSE c;
    DBMS_ALERT.WAITONE('transferReadyToSchedule', unusedMessage, unusedStatus, 3);
    -- try again to find something now that we waited
    OPEN c;
    FETCH c INTO srIntId;
    IF c%NOTFOUND THEN
      -- still nothing. We will give back the control to the application
      -- so that it can handle cases like signals and exit. We will probably
      -- be back soon :-)
      RETURN;
    END IF;
  END IF;
  LOOP
    -- we reached this point because we have found at least one candidate
    -- let's loop on the candidates until we find one we can process
    BEGIN
      -- Try to lock the current candidate, verify that the status is valid. A
      -- valid subrequest is either in READYFORSCHED or has been stuck in
      -- BEINGSCHED for more than 1800 seconds (30 mins)
      SELECT /*+ INDEX(SR PK_SubRequest_ID) */ id INTO srIntId
        FROM SubRequest PARTITION (P_STATUS_13_14) SR
       WHERE id = srIntId
         AND status = dconst.SUBREQUEST_READYFORSCHED
         FOR UPDATE NOWAIT;
      -- We have successfully acquired the lock, so we update the subrequest
      -- status and modification time
      UPDATE /*+ INDEX(Subrequest PK_Subrequest_Id)*/ SubRequest
         SET status = dconst.SUBREQUEST_READY,
             lastModificationTime = getTime()
       WHERE id = srIntId
      RETURNING id, subReqId, protocol, xsize, requestedFileSystems
        INTO srId, srSubReqId, srProtocol, srXsize, srRfs;
      -- and we exit the loop on candidates
      EXIT;
    EXCEPTION
      -- Try again, either we failed to accquire the lock on the subrequest or
      -- the subrequest being processed is not the correct state
      WHEN NO_DATA_FOUND THEN
        NULL;
      WHEN SrLocked THEN
        NULL;
    END;
    -- we are here because the current candidate could not be handled
    -- let's go to the next one
    FETCH c INTO srIntId;
    IF c%NOTFOUND THEN
      -- no next one ? then we can return
      RETURN;
    END IF;
  END LOOP;
  CLOSE c;

  -- We finally got a valid candidate, let's process it
  -- Extract the rest of the information required by transfer manager
  SELECT /*+ INDEX(Subrequest PK_Subrequest_Id)*/
         CastorFile.id, CastorFile.fileId, CastorFile.nsHost, SvcClass.name, SvcClass.id,
         Request.type, Request.reqId, Request.euid, Request.egid, Request.username,
         Request.direction, Request.sourceDiskCopy, Request.destDiskCopy,
         Request.sourceSvcClass, Client.ipAddress, Client.port, Client.version,
         129 clientType, Client.secure, Request.creationTime,
         decode(SvcClass.defaultFileSize, 0, 2000000000, SvcClass.defaultFileSize)
    INTO cfId, cfFileId, cfNsHost, reqSvcClass, svcClassId, reqType, reqId, reqEuid, reqEgid,
         reqUsername, srOpenFlags, reqSourceDiskCopy, reqDestDiskCopy, reqSourceSvcClass,
         clientIp, clientPort, clientVersion, clientType, clientSecure, reqCreationTime,
         reqDefaultFileSize
    FROM SubRequest, CastorFile, SvcClass, Client,
         (SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */
                 id, username, euid, egid, reqid, client, creationTime,
                 'w' direction, svcClass, NULL sourceDiskCopy,
                 NULL destDiskCopy, NULL sourceSvcClass, 40 type
            FROM StagePutRequest
           UNION ALL
          SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */
                 id, username, euid, egid, reqid, client, creationTime,
                 'r' direction, svcClass, NULL sourceDiskCopy,
                 NULL destDiskCopy, NULL sourceSvcClass, 35 type
            FROM StageGetRequest
           UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */
                 id, username, euid, egid, reqid, client, creationTime,
                 'o' direction, svcClass, NULL sourceDiskCopy,
                 NULL destDiskCopy, NULL sourceSvcClass, 44 type
            FROM StageUpdateRequest
           UNION ALL
          SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */
                 id, username, euid, egid, reqid, client, creationTime,
                 'w' direction, svcClass, sourceDiskCopy, destDiskCopy,
                 (SELECT name FROM SvcClass WHERE id = sourceSvcClass), 133 type
            FROM StageDiskCopyReplicaRequest) Request
   WHERE SubRequest.id = srId
     AND SubRequest.castorFile = CastorFile.id
     AND Request.svcClass = SvcClass.id
     AND Request.id = SubRequest.request
     AND Request.client = Client.id;

  -- In case of disk2disk copies, requested filesystems are concerning the sources
  -- destinations are free
  IF reqType = 133 THEN  -- StageDiskCopyReplicaRequest
    sourceRfs := srRfs;
    srRfs := NULL;
  END IF;

  -- Select random filesystems to use if none is already requested
  IF LENGTH(srRfs) IS NULL THEN
    FOR line IN
      (SELECT candidate FROM
         (SELECT UNIQUE FIRST_VALUE (DiskServer.name || ':' || FileSystem.mountPoint)
                   OVER (PARTITION BY DiskServer.id ORDER BY DBMS_Random.value) AS candidate
            FROM DiskServer, FileSystem, DiskPool2SvcClass
           WHERE FileSystem.diskServer = DiskServer.id
             AND FileSystem.diskPool = DiskPool2SvcClass.parent
             AND DiskPool2SvcClass.child = SvcClassId
             AND DiskServer.status = dconst.DISKSERVER_PRODUCTION
             AND FileSystem.status = dconst.FILESYSTEM_PRODUCTION
             AND FileSystem.free - FileSystem.minAllowedFreeSpace * FileSystem.totalSize > srXsize
             -- this is to avoid disk2diskcopies to create new copies on diskservers already having one
             AND DiskServer.id NOT IN
               (SELECT diskserver FROM DiskCopy, FileSystem
                 WHERE DiskCopy.castorFile = cfId
                   AND DiskCopy.status IN (dconst.DISKCOPY_STAGED, dconst.DISKCOPY_WAITDISK2DISKCOPY,
                                           dconst.DISKCOPY_CANBEMIGR)
                   AND FileSystem.id = DiskCopy.fileSystem)
           ORDER BY DBMS_Random.value)
        WHERE ROWNUM <= 5) LOOP
      IF LENGTH(srRfs) IS NOT NULL THEN srRfs := srRfs || '|'; END IF;
      srRfs := srRfs || line.candidate;
    END LOOP;
  END IF;

END;
/

/* PL/SQL method implementing transfersToAbort */
CREATE OR REPLACE
PROCEDURE transfersToAbortProc(srUuidCur OUT castor.UUIDRecord_Cur) AS
  srUuid VARCHAR2(2048);
  srUuids strListTable;
  unusedMessage VARCHAR2(2048);
  unusedStatus INTEGER;
BEGIN
  BEGIN
    -- find out whether there is something
    SELECT uuid INTO srUuid FROM TransfersToAbort WHERE ROWNUM < 2;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- There is nothing to abort. Wait for next alert concerning something
    -- to abort or at least 3 seconds.
    DBMS_ALERT.WAITONE('transfersToAbort', unusedMessage, unusedStatus, 3);
  END;
  -- Either we found something or we timedout, in both cases
  -- we go back to python so that it can handle cases like signals and exit
  -- We will probably be back soon :-)
  DELETE FROM transfersToAbort RETURNING uuid BULK COLLECT INTO srUuids;
  OPEN srUuidCur FOR SELECT * FROM TABLE(srUuids);
END;
/

/* PL/SQL method implementing syncRunningTransfers
 * This is called by the transfer manager daemon on the restart of a disk server manager
 * in order to sync running transfers in the database with the reality of the machine.
 * This is particularly useful to terminate cleanly transfers interupted by a power cut
 */
CREATE OR REPLACE
PROCEDURE syncRunningTransfers(machine IN VARCHAR2,
                               transfers IN castor."strList",
                               killedTransfersCur OUT castor.TransferRecord_Cur) AS
  unused VARCHAR2(2048);
  fileid NUMBER;
  nsHost VARCHAR2(2048);
  reqId VARCHAR2(2048);
  killedTransfers castor."strList";
  errnos castor."cnumList";
  errmsg castor."strList";
BEGIN
  -- cleanup from previous round
  DELETE FROM SyncRunningTransfersHelper2;
  -- insert the list of running transfers into a temporary table for easy access
  FORALL i IN transfers.FIRST .. transfers.LAST
    INSERT INTO SyncRunningTransfersHelper VALUES (transfers(i));
  -- Go through all running transfers from the DB point of view for the given diskserver
  FOR SR IN (SELECT SubRequest.id, SubRequest.subreqId, SubRequest.castorfile, SubRequest.request
               FROM SubRequest, DiskCopy, FileSystem, DiskServer
              WHERE SubRequest.status = dconst.SUBREQUEST_READY
                AND Subrequest.diskCopy = DiskCopy.id
                AND DiskCopy.fileSystem = FileSystem.id
                AND DiskCopy.status = dconst.DISKCOPY_STAGEOUT
                AND FileSystem.diskServer = DiskServer.id
                AND DiskServer.name = machine) LOOP
    BEGIN
      -- check if they are running on the diskserver
      SELECT * INTO unused FROM SyncRunningTransfersHelper
       WHERE subreqId = SR.subreqId;
      -- this one was still running, nothing to do then
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- this transfer is not running anymore although the stager DB believes it is
      -- we first get its reqid and fileid
      SELECT Request.reqId INTO reqId FROM
        (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ reqId, id from StageGetRequest UNION ALL
         SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ reqId, id from StagePutRequest UNION ALL
         SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ reqId, id from StageUpdateRequest UNION ALL
         SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ reqId, id from StageRepackRequest) Request
       WHERE Request.id = SR.request;
      SELECT fileid, nsHost INTO fileid, nsHost FROM CastorFile WHERE id = SR.castorFile;
      -- and we put it in the list of transfers to be failed with code 1015 (SEINTERNAL)
      INSERT INTO SyncRunningTransfersHelper2 VALUES (SR.subreqId, reqId, fileid, nsHost, 1015, 'Transfer has been killed while running');
    END;
  END LOOP;
  -- fail the transfers that are no more running
  SELECT subreqId, errorCode, errorMsg BULK COLLECT
    INTO killedTransfers, errnos, errmsg
    FROM SyncRunningTransfersHelper2;
  -- Note that the next call will commit (even once per transfer to kill)
  -- This is ok as SyncRunningTransfersHelper2 was declared "ON COMMIT PRESERVE ROWS" and
  -- is a temporary table so it's content is only visible to our connection.
  transferFailedSafe(killedTransfers, errnos, errmsg);
  -- and return list of transfers that have been failed, for logging purposes
  OPEN killedTransfersCur FOR SELECT subreqId, reqId, fileid, nsHost FROM SyncRunningTransfersHelper2;
END;
/
/*******************************************************************
 *
 * @(#)RCSfile: oracleQuery.sql,v  Revision: 1.661  Date: 2009/06/17 10:55:44  Author: itglp 
 *
 * PL/SQL code for the stager query service
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
  euid IN INTEGER, egid IN INTEGER,
  result OUT castor.QueryLine_Cur) AS
BEGIN
  -- Here we get the status for each castorFile as follows: if a valid diskCopy is found,
  -- or if a request is found and its related diskCopy exists, the diskCopy status
  -- is returned, else -1 (INVALID) is returned.
  -- The case of svcClassId = 0 (i.e. '*') is handled separately for performance reasons
  -- and because it may include a check for read permissions.
  IF svcClassId = 0 THEN
    OPEN result FOR
      SELECT fileId, nsHost, dcId, path, fileSize, status, machine, mountPoint, nbCopyAccesses,
             lastKnownFileName, creationTime, svcClass, lastAccessTime, hwStatus
        FROM (
          SELECT UNIQUE CastorFile.id, CastorFile.fileId, CastorFile.nsHost, DC.id AS dcId,
                 DC.path, CastorFile.fileSize, DC.status,
                 CASE WHEN DC.svcClass IS NULL THEN
                   (SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ UNIQUE Req.svcClassName
                      FROM SubRequest,
                        (SELECT /*+ INDEX(StageGetRequest PK_StagePrepareToGetRequest_Id) */ id, svcClassName FROM StagePrepareToGetRequest    UNION ALL
                         SELECT /*+ INDEX(StageGetRequest PK_StagePrepareToPutRequest_Id) */ id, svcClassName FROM StagePrepareToPutRequest    UNION ALL
                         SELECT /*+ INDEX(StageGetRequest PK_StagePrepareToUpdateRequ_Id) */ id, svcClassName FROM StagePrepareToUpdateRequest UNION ALL
                         SELECT /*+ INDEX(StageGetRequest PK_StageRepackRequest_Id) */ id, svcClassName FROM StageRepackRequest                UNION ALL
                         SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, svcClassName FROM StageGetRequest) Req
                          WHERE SubRequest.diskCopy = DC.id
                            AND request = Req.id)              
                   ELSE DC.svcClass END AS svcClass,
                 DC.machine, DC.mountPoint, DC.nbCopyAccesses, CastorFile.lastKnownFileName,
                 DC.creationTime, DC.lastAccessTime, nvl(decode(DC.hwStatus, 2, 1, DC.hwStatus), -1) hwStatus
            FROM CastorFile,
              (SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status, DiskServer.name AS machine, FileSystem.mountPoint,
                      SvcClass.name AS svcClass, DiskCopy.filesystem, DiskCopy.CastorFile, 
                      DiskCopy.nbCopyAccesses, DiskCopy.creationTime, DiskCopy.lastAccessTime,
                      FileSystem.status + DiskServer.status AS hwStatus
                 FROM FileSystem, DiskServer, DiskPool2SvcClass, SvcClass,
                   (SELECT id, status, filesystem, castorFile, path, nbCopyAccesses, creationTime, lastAccessTime
                      FROM DiskCopy
                     WHERE CastorFile IN (SELECT /*+ CARDINALITY(cfidTable 5) */ * FROM TABLE(cfs) cfidTable)
                       AND status IN (0, 1, 2, 4, 5, 6, 7, 10, 11) -- search for diskCopies not BEINGDELETED
                     GROUP BY (id, status, filesystem, castorfile, path, nbCopyAccesses, creationTime, lastAccessTime)) DiskCopy
                WHERE FileSystem.id(+) = DiskCopy.fileSystem
                  AND nvl(FileSystem.status, 0) IN (0, 1) -- PRODUCTION, DRAINING
                  AND DiskServer.id(+) = FileSystem.diskServer
                  AND nvl(DiskServer.status, 0) IN (0, 1) -- PRODUCTION, DRAINING
                  AND DiskPool2SvcClass.parent(+) = FileSystem.diskPool
                  AND SvcClass.id(+) = DiskPool2SvcClass.child
                  AND (euid = 0 OR SvcClass.id IS NULL OR   -- if euid > 0 check read permissions for srmLs (see bug #69678)
                       checkPermission(SvcClass.name, euid, egid, 35) = 0)   -- OBJ_StageGetRequest
                 ) DC
           WHERE CastorFile.id = DC.castorFile)
       WHERE status IS NOT NULL    -- search for valid diskcopies
       ORDER BY fileid, nshost;
  ELSE
    OPEN result FOR
      SELECT fileId, nsHost, dcId, path, fileSize, status, machine, mountPoint, nbCopyAccesses,
             lastKnownFileName, creationTime, (SELECT name FROM svcClass WHERE id = svcClassId),
             lastAccessTime, hwStatus
        FROM (
          SELECT UNIQUE CastorFile.id, CastorFile.fileId, CastorFile.nsHost, DC.id AS dcId,
                 DC.path, CastorFile.fileSize,
                 CASE WHEN DC.dcSvcClass = svcClassId THEN DC.status
                      WHEN DC.fileSystem = 0 THEN
                       (SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/
                        UNIQUE decode(nvl(SubRequest.status, -1), -1, -1, DC.status)
                          FROM SubRequest,
                            (SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id, svcClassName FROM StagePrepareToGetRequest       UNION ALL
                             SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id, svcClassName FROM StagePrepareToPutRequest       UNION ALL
                             SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id, svcClassName FROM StagePrepareToUpdateRequest UNION ALL
                             SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id, svcClassName FROM StageRepackRequest                         UNION ALL
                             SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, svcClassName FROM StageGetRequest) Req
                              WHERE SubRequest.CastorFile = CastorFile.id
                                AND request = Req.id
                                AND svcClass = svcClassId)
                      END AS status,
                 DC.machine, DC.mountPoint, DC.nbCopyAccesses, CastorFile.lastKnownFileName,
                 DC.creationTime, DC.lastAccessTime, nvl(decode(DC.hwStatus, 2, 1, DC.hwStatus), -1) hwStatus
            FROM CastorFile,
              (SELECT DiskCopy.id, DiskCopy.path, DiskCopy.status, DiskServer.name AS machine, FileSystem.mountPoint,
                      DiskPool2SvcClass.child AS dcSvcClass, DiskCopy.filesystem, DiskCopy.CastorFile, 
                      DiskCopy.nbCopyAccesses, DiskCopy.creationTime, DiskCopy.lastAccessTime,
                      FileSystem.status + DiskServer.status AS hwStatus
                 FROM FileSystem, DiskServer, DiskPool2SvcClass,
                   (SELECT id, status, filesystem, castorFile, path, nbCopyAccesses, creationTime, lastAccessTime
                      FROM DiskCopy
                     WHERE CastorFile IN (SELECT /*+ CARDINALITY(cfidTable 5) */ * FROM TABLE(cfs) cfidTable)
                       AND status IN (0, 1, 2, 4, 5, 6, 7, 10, 11)  -- search for diskCopies not GCCANDIDATE or BEINGDELETED
                     GROUP BY (id, status, filesystem, castorfile, path, nbCopyAccesses, creationTime, lastAccessTime)) DiskCopy
                WHERE FileSystem.id(+) = DiskCopy.fileSystem
                  AND nvl(FileSystem.status, 0) IN (0, 1) -- PRODUCTION, DRAINING
                  AND DiskServer.id(+) = FileSystem.diskServer
                  AND nvl(DiskServer.status, 0) IN (0, 1) -- PRODUCTION, DRAINING
                  AND DiskPool2SvcClass.parent(+) = FileSystem.diskPool) DC
                  -- No extra check on read permissions here, it is not relevant
           WHERE CastorFile.id = DC.castorFile)
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
  euid IN INTEGER,
  egid IN INTEGER,
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
  internalStageQuery(cfIds, svcClassId, euid, egid, result);
END;
/


/*
 * PL/SQL method implementing the stager_qry based on file id or single filename
 */
CREATE OR REPLACE PROCEDURE fileIdStageQuery
 (fid IN NUMBER,
  nh IN VARCHAR2,
  svcClassId IN INTEGER,
  euid IN INTEGER,
  egid IN INTEGER,
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
  internalStageQuery(cfs, svcClassId, euid, egid, result);
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
         (SELECT /*+ INDEX(StagePrepareToGetRequest I_StagePTGRequest_ReqId) */ id
            FROM StagePreparetogetRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(StagePrepareToPutRequest I_StagePTPRequest_ReqId) */ id
            FROM StagePreparetoputRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest I_StagePTURequest_ReqId) */ id
            FROM StagePreparetoupdateRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(StageGetRequest I_StageGetRequest_ReqId) */ id
            FROM stageGetRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(stagePutRequest I_stagePutRequest_ReqId) */ id
            FROM stagePutRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(StageRepackRequest I_StageRepackRequest_ReqId) */ id
            FROM StageRepackRequest
           WHERE reqid LIKE rid) reqlist
   WHERE sr.request = reqlist.id;
  IF cfs.COUNT > 0 THEN
    internalStageQuery(cfs, svcClassId, 0, 0, result);
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
  SELECT /*+ NO_USE_HASH(REQLIST SR) USE_NL(REQLIST SR) 
             INDEX(SR I_SUBREQUEST_REQUEST) */
         sr.castorfile BULK COLLECT INTO cfs
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
    internalStageQuery(cfs, svcClassId, 0, 0, result);
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
    FROM (SELECT /*+ INDEX(StagePrepareToGetRequest I_StagePTGRequest_ReqId) */ id
            FROM StagePreparetogetRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest I_StagePTURequest_ReqId) */ id
            FROM StagePreparetoupdateRequest
           WHERE reqid = rid
          UNION ALL
          SELECT /*+ INDEX(StageRepackRequest I_StageRepackRequest_ReqId) */ id
            FROM StageRepackRequest
           WHERE reqid = rid
          );
  IF reqs.COUNT > 0 THEN
    UPDATE /*+ INDEX(Subrequest I_Subrequest_Request)*/ SubRequest 
       SET getNextStatus = 2  -- GETNEXTSTATUS_NOTIFIED
     WHERE getNextStatus = 1  -- GETNEXTSTATUS_FILESTAGED
       AND request IN (SELECT * FROM TABLE(reqs))
    RETURNING castorfile BULK COLLECT INTO cfs;
    internalStageQuery(cfs, svcClassId, 0, 0, result);
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
    UPDATE /*+ INDEX(Subrequest I_Subrequest_Request)*/ SubRequest 
       SET getNextStatus = 2  -- GETNEXTSTATUS_NOTIFIED
     WHERE getNextStatus = 1  -- GETNEXTSTATUS_FILESTAGED
       AND request IN (SELECT * FROM TABLE(reqs))
    RETURNING castorfile BULK COLLECT INTO cfs;
    internalStageQuery(cfs, svcClassId, 0, 0, result);
  ELSE
    notfound := 1;
  END IF;
END;
/

/* Internal function used by describeDiskPool[s] to return either available
 * (i.e. the space on PRODUCTION status resources) or total space depending on
 * the type of query */
CREATE OR REPLACE FUNCTION getSpace(status IN NUMBER, space IN NUMBER,
                                    queryType IN NUMBER, spaceType IN NUMBER)
RETURN NUMBER IS
BEGIN
  IF space < 0 THEN
    -- over used filesystems may report negative numbers, just cut to 0
    RETURN 0;
  END IF;
  IF (status > 0) AND  -- not in production
     (queryType = dconst.DISKPOOLQUERYTYPE_AVAILABLE OR
      (queryType = dconst.DISKPOOLQUERYTYPE_DEFAULT AND spaceType = dconst.DISKPOOLSPACETYPE_FREE)) THEN
    return 0;
  ELSE
    RETURN space;
  END IF;
END;
/

/*
 * PL/SQL method implementing the diskPoolQuery when listing pools
 */
CREATE OR REPLACE PROCEDURE describeDiskPools
(svcClassName IN VARCHAR2, reqEuid IN INTEGER, reqEgid IN INTEGER, queryType IN INTEGER,
 res OUT NUMBER, result OUT castor.DiskPoolsQueryLine_Cur) AS
BEGIN
  -- We use here analytic functions and the grouping sets functionality to
  -- get both the list of filesystems and a summary per diskserver and per
  -- diskpool. The grouping analytic function also allows to mark the summary
  -- lines for easy detection in the C++ code
  IF svcClassName = '*' THEN
    OPEN result FOR
      SELECT grouping(ds.name) AS IsDSGrouped,
             grouping(fs.mountPoint) AS IsFSGrouped,
             dp.name, ds.name, ds.status, fs.mountPoint,
             sum(getSpace(fs.status + ds.status,
                          fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_FREE)) AS freeSpace,
             sum(getSpace(fs.status + ds.status,
                          fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_CAPACITY)) AS totalSize,
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp
       WHERE dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         GROUP BY grouping sets(
             (dp.name, ds.name, ds.status, fs.mountPoint,
              getSpace(fs.status + ds.status,
                       fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_FREE),
              getSpace(fs.status + ds.status,
                       fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_CAPACITY),
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
             sum(getSpace(fs.status + ds.status,
                          fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_FREE)) AS freeSpace,
             sum(getSpace(fs.status + ds.status,
                          fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_CAPACITY)) AS totalSize,
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp,
             DiskPool2SvcClass d2s, SvcClass sc
       WHERE sc.name = svcClassName
         AND sc.id = d2s.child
         AND checkPermission(sc.name, reqEuid, reqEgid, 195) = 0
         AND d2s.parent = dp.id
         AND dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         GROUP BY grouping sets(
             (dp.name, ds.name, ds.status, fs.mountPoint,
              getSpace(fs.status + ds.status,
                       fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_FREE),
              getSpace(fs.status + ds.status,
                       fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_CAPACITY),
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
           WHEN sum(checkPermission(sc.name, reqEuid, reqEgid, 195)) THEN -1
           ELSE count(*) END
      INTO res
      FROM DiskPool2SvcClass d2s, SvcClass sc
     WHERE d2s.child = sc.id
       AND (sc.name = svcClassName OR svcClassName = '*');
  END IF;
END;
/



/*
 * PL/SQL method implementing the diskPoolQuery for a given pool
 */
CREATE OR REPLACE PROCEDURE describeDiskPool
(diskPoolName IN VARCHAR2, svcClassName IN VARCHAR2, queryType IN INTEGER,
 res OUT NUMBER, result OUT castor.DiskPoolQueryLine_Cur) AS
BEGIN
  -- We use here analytic functions and the grouping sets functionnality to
  -- get both the list of filesystems and a summary per diskserver and per
  -- diskpool. The grouping analytic function also allows to mark the summary
  -- lines for easy detection in the C++ code
  IF svcClassName = '*' THEN
    OPEN result FOR
      SELECT grouping(ds.name) AS IsDSGrouped,
             grouping(fs.mountPoint) AS IsGrouped,
             ds.name, ds.status, fs.mountPoint,
             sum(getSpace(fs.status + ds.status,
                          fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_FREE)) AS freeSpace,
             sum(getSpace(fs.status + ds.status,
                          fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_CAPACITY)) AS totalSize,
             fs.minFreeSpace, fs.maxFreeSpace, fs.status
        FROM FileSystem fs, DiskServer ds, DiskPool dp
       WHERE dp.id = fs.diskPool
         AND ds.id = fs.diskServer
         AND dp.name = diskPoolName
         GROUP BY grouping sets(
             (ds.name, ds.status, fs.mountPoint,
              getSpace(fs.status + ds.status,
                       fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_FREE),
              getSpace(fs.status + ds.status,
                       fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_CAPACITY),
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
             sum(getSpace(fs.status + ds.status,
                          fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_FREE)) AS freeSpace,
             sum(getSpace(fs.status + ds.status,
                          fs.totalSize,
                          queryType, dconst.DISKPOOLSPACETYPE_CAPACITY)) AS totalSize,
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
              getSpace(fs.status + ds.status,
                       fs.free - fs.minAllowedFreeSpace * fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_FREE),
              getSpace(fs.status + ds.status,
                       fs.totalSize,
                       queryType, dconst.DISKPOOLSPACETYPE_CAPACITY),
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
       AND (sc.name = svcClassName OR svcClassName = '*');
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
  /**
   * The StreamForPolicy record is used to pass information about a specific
   * stream to the stream-policy Python-function of a service-class.  The
   * Python-function is responsible for deciding whether or not the stream
   * should be started.
   */
  TYPE StreamForPolicy IS RECORD (
    id                  NUMBER,
    numTapeCopies       NUMBER,
    totalBytes          NUMBER,
    ageOfOldestTapeCopy NUMBER,
    tapePool            NUMBER);
  TYPE StreamForPolicy_Cur IS REF CURSOR RETURN StreamForPolicy;
  TYPE DbRecallInfo IS RECORD (
    vid VARCHAR2(2048),
    tapeId NUMBER,
    dataVolume NUMBER,
    numbFiles NUMBER,
    expireTime NUMBER,
    priority NUMBER);
  TYPE DbRecallInfo_Cur IS REF CURSOR RETURN DbRecallInfo;
  TYPE RecallMountsForPolicy IS RECORD (
    tapeId             NUMBER,
    vid                VARCHAR2(2048),
    numSegments        NUMBER,
    totalBytes         NUMBER,
    ageOfOldestSegment NUMBER,
    priority           NUMBER,
    status             NUMBER);
  TYPE RecallMountsForPolicy_Cur IS REF CURSOR RETURN RecallMountsForPolicy;
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

/* Trigger ensuring validity of VID in state transitions */
CREATE OR REPLACE TRIGGER TR_TapeCopy_VID
BEFORE INSERT OR UPDATE OF Status ON TapeCopy
FOR EACH ROW
BEGIN
  /* Enforce the state integrity of VID in state transitions */
  
  /* rtcpclientd is given full exception, no check */
  IF rtcpclientdIsRunning THEN RETURN; END IF;
  
  CASE :new.status
    WHEN  tconst.TAPECOPY_SELECTED THEN
      /* The VID MUST be defined when the tapecopy gets selected */
      IF :new.VID IS NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to TAPECOPY_SELECTED State without a VID (TC.ID: '||
          :new.ID||' VID:'|| :old.VID||'=>'||:new.VID||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
    WHEN tconst.TAPECOPY_STAGED THEN
       /* The VID MUST be defined when the tapecopy goes to staged */
       IF :new.VID IS NULL THEN
         RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to TAPECOPY_STAGED State without a VID (TC.ID: '||
          :new.ID||' VID:'|| :old.VID||'=>'||:new.VID||' Status:'||:old.status||'=>'||:new.status||')');
       END IF;
       /* The VID MUST remain the same when going to staged */
       IF :new.VID != :old.VID THEN
         RAISE_APPLICATION_ERROR(-20119,
           'Moving to STAGED State without carrying the VID over');
       END IF;
    ELSE
      /* In all other cases, VID should be NULL */
      IF :new.VID IS NOT NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to TapeCopy state where VID makes no sense, yet VID!=NULL (TC.ID: '||
          :new.ID||' VID:'|| :old.VID||'=>'||:new.VID||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
  END CASE;
END;
/

/* Trigger ensuring validity (or nullity) of tape in Stream state transitions */
/* The tape column is no protected for validity by a foreign key constrain, so this could be added for extra safety TODO */
CREATE OR REPLACE TRIGGER TR_Stream_Tape
BEFORE INSERT OR UPDATE OF Status ON Stream
FOR EACH ROW
BEGIN
  /* Enforce the state integrity of VID in state transitions */
  
  /* rtcpclientd is given full exception, no check */
  IF rtcpclientdIsRunning THEN RETURN; END IF;
  CASE :new.status
    WHEN  tconst.STREAM_TO_BE_SENT_TO_VDQM THEN
      /* The tape MUST be defined when the stream is ready to grab a drive */
      IF :new.tape IS NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to STREAM_TO_BE_SENT_TO_VDQM State without a tape (S.ID: '||
          :new.ID||' tape:'|| :old.tape||'=>'||:new.tape||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
    WHEN  tconst.STREAM_WAITDRIVE THEN
      /* The tape MUST be defined when the stream is ready to grab a drive */
      IF :new.tape IS NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to STREAM_WAITDRIVE State without a tape (S.ID: '||
          :new.ID||' tape:'|| :old.tape||'=>'||:new.tape||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
       /* The tape MUST remain the same when going to STREAM_WAITDRIVE */
       IF :new.tape != :old.tape THEN
         RAISE_APPLICATION_ERROR(-20119,
           'Moving to STREAM_WAITDRIVE State without carrying the tape over');
       END IF;    
    WHEN  tconst.STREAM_WAITMOUNT THEN
      /* The tape MUST be defined when the stream is ready to grab a drive */
      IF :new.tape IS NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to STREAM_WAITMOUNT State without a tape (S.ID: '||
          :new.ID||' tape:'|| :old.tape||'=>'||:new.tape||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
       /* The tape MUST remain the same when going to STREAM_WAITMOUNT */
       IF :new.tape != :old.tape THEN
         RAISE_APPLICATION_ERROR(-20119,
           'Moving to STREAM_WAITMOUNT State without carrying the tape over');
       END IF;    
    WHEN  tconst.STREAM_RUNNING THEN
      /* The tape MUST be defined when the stream is ready to grab a drive */
      IF :new.tape IS NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to STREAM_RUNNING State without a tape (S.ID: '||
          :new.ID||' tape:'|| :old.tape||'=>'||:new.tape||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
       /* The tape MUST remain the same when going to STREAM_RUNNING */
       IF :new.tape != :old.tape THEN
         RAISE_APPLICATION_ERROR(-20119,
           'Moving to STREAM_RUNNING State without carrying the tape over');
       END IF;    
    ELSE
      /* In all other cases, tape should be NULL */
      IF :new.tape IS NOT NULL THEN
        RAISE_APPLICATION_ERROR(-20119,
          'Moving/creating (in)to Stream state where VID makes no sense, yet tape!=NULL (stream.ID: '||
          :new.ID||' tape:'|| :old.tape||'=>'||:new.tape||' Status:'||:old.status||'=>'||:new.status||')');
      END IF;
  END CASE;
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
     AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
     AND FileSystem.id = DiskCopy.filesystem
     AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
     AND DiskCopy.castorfile = TapeCopy.castorfile
     AND Stream2TapeCopy.child = TapeCopy.id
     AND Stream2TapeCopy.parent = streamId
     AND TapeCopy.status = tconst.TAPECOPY_WAITINSTREAMS
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
                                                 diskServerName OUT NOCOPY VARCHAR2, mountPoint OUT NOCOPY VARCHAR2,
                                                 path OUT NOCOPY VARCHAR2, dci OUT INTEGER,
                                                 castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                 nsHost OUT NOCOPY VARCHAR2, fileSize OUT INTEGER,
                                                 tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
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
  -- First try to see whether we should reuse the same filesystem as last time
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
           AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
           AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING);
        -- we are within the time range, so we try to reuse the filesystem
        SELECT /*+ FIRST_ROWS(1)  LEADING(D T ST) */
               D.path, D.id, D.castorfile, T.id
          INTO path, dci, castorFileId, tapeCopyId
          FROM DiskCopy D, TapeCopy T, Stream2TapeCopy ST
         WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
         -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
           AND D.filesystem = lastButOneFSUsed
           AND ST.parent = streamId
           AND T.status = tconst.TAPECOPY_WAITINSTREAMS
           AND ST.child = T.id
           AND T.castorfile = D.castorfile
           AND ROWNUM < 2 FOR UPDATE OF t.id NOWAIT;
        SELECT CastorFile.FileId, CastorFile.NsHost, CastorFile.FileSize,
               CastorFile.lastUpdateTime
          INTO fileId, nsHost, fileSize, lastUpdateTime
          FROM CastorFile
         WHERE Id = castorFileId;
        -- we found one, no need to go for new filesystem
        findNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
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
        AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
        AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
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
          WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
          -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
            AND D.filesystem = f.fileSystemId
            AND StT.parent = streamId
            AND T.status = tconst.TAPECOPY_WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND ROWNUM < 2;
         -- found something on this filesystem, no need to go on
         diskServerId := f.DiskServerId;
         fileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR lockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;

  IF tapeCopyId = 0 THEN
    -- Nothing found, reset last filesystems used and exit
    UPDATE Stream
       SET lastFileSystemUsed = 0, lastButOneFileSystemUsed = 0 -- XXX Should this not be NULL?
     WHERE id = streamId;
    RETURN;
  END IF;

  -- Here we found a tapeCopy and we process it
  -- update status of selected tapecopy and stream
  UPDATE TapeCopy SET status = tconst.TAPECOPY_SELECTED
   WHERE id = tapeCopyId;
  IF findNewFS = 1 THEN
    UPDATE Stream
       SET status = tconst.STREAM_RUNNING,
           lastFileSystemUsed = fileSystemId,
           lastButOneFileSystemUsed = lastFileSystemUsed,
           lastFileSystemChange = getTime()
     WHERE id = streamId AND status IN (tconst.STREAM_WAITMOUNT,tconst.STREAM_RUNNING);
  ELSE
    -- only update status
    UPDATE Stream
       SET status = tconst.STREAM_RUNNING
     WHERE id = streamId AND status IN (tconst.STREAM_WAITMOUNT,tconst.STREAM_RUNNING);
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
                                                   diskServerName OUT NOCOPY VARCHAR2, mountPoint OUT NOCOPY VARCHAR2,
                                                   path OUT NOCOPY VARCHAR2, dci OUT INTEGER,
                                                   castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                   nsHost OUT NOCOPY VARCHAR2, fileSize OUT INTEGER,
                                                   tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
  varFSId INTEGER := 0;
  varDSId NUMBER;
  varFsDiskServer NUMBER; /* XXX TODO FIXME This variable is used uninitialized at the end of the function. Was already the case in revision 21389 */
  varLastFSChange NUMBER;
  varLastFSUsed NUMBER;
  varLastButOneFSUsed NUMBER;
  varFindNewFS NUMBER := 1;
  varNbMigrators NUMBER := 0;
  unused NUMBER;
  LockError EXCEPTION;
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  tapeCopyId := 0;
  -- First try to see whether we should reuse the same filesystem as last time
  SELECT lastFileSystemChange, lastFileSystemUsed, lastButOneFileSystemUsed
    INTO varLastFSChange, varLastFSUsed, varLastButOneFSUsed
    FROM Stream WHERE id = streamId;
  IF getTime() < varLastFSChange + 1800 THEN
    SELECT (SELECT count(*) FROM stream WHERE lastFileSystemUsed = varLastFSUsed)
      INTO varNbMigrators FROM DUAL;
    -- only go if we are the only migrator on the box
    IF varNbMigrators = 1 THEN
      BEGIN
        -- check states of the diskserver and filesystem and get mountpoint and diskserver name
        SELECT diskserver.id, name, mountPoint, FileSystem.id
          INTO varDSId, diskServerName, mountPoint, varFSId
          FROM FileSystem, DiskServer
         WHERE FileSystem.diskServer = DiskServer.id
           AND FileSystem.id = varLastFSUsed
           AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
           AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING);
        -- we are within the time range, so we try to reuse the filesystem
        SELECT /*+ ORDERED USE_NL(D T) INDEX(T I_TapeCopy_CF_Status_2)
                   INDEX(ST I_Stream2TapeCopy_PC) */
               D.path, D.diskcopy_id, D.castorfile, T.id
          INTO path, dci, castorFileId, tapeCopyId
          FROM (SELECT /*+ INDEX(DK I_DiskCopy_FS_Status_10) */
                       DK.path path, DK.id diskcopy_id, DK.castorfile
                  FROM DiskCopy DK
                 WHERE decode(DK.status, 10, DK.status, NULL) = dconst.DISKCOPY_CANBEMIGR
                 -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
                   AND DK.filesystem = varLastFSUsed) D, TapeCopy T, Stream2TapeCopy ST
         WHERE T.castorfile = D.castorfile
           AND ST.child = T.id
           AND ST.parent = streamId
           AND decode(T.status, 2, T.status, NULL) = tconst.TAPECOPY_WAITINSTREAMS
           -- 2 = tconst.TAPECOPY_WAITINSTREAMS. Has to be kept as a hardcoded number in order to use a function-based index.
           AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;   
        SELECT C.fileId, C.nsHost, C.fileSize, C.lastUpdateTime
          INTO fileId, nsHost, fileSize, lastUpdateTime
          FROM castorfile C
         WHERE castorfileId = C.id;
        -- we found one, no need to go for new filesystem
        varFindNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
        -- found no tapecopy or diskserver, filesystem are down. We'll go through the normal selection
        NULL;
      END;
    END IF;
  END IF;
  IF varFindNewFS = 1 THEN
    -- We try first to reuse the diskserver of the lastFSUsed, even if we change filesystem
    FOR f IN (
      SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
        FROM FileSystem, DiskServer
       WHERE FileSystem.diskServer = DiskServer.id
         AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
         AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
         AND DiskServer.id = varLastButOneFSUsed) LOOP
       BEGIN
         -- lock the complete diskServer as we will update all filesystems
         SELECT id INTO unused FROM DiskServer
          WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
         SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
                f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
           INTO varDSId, diskServerName, mountPoint, varFSId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
          -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
            AND D.filesystem = f.fileSystemId
            AND StT.parent = streamId
            AND T.status = tconst.TAPECOPY_WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND ROWNUM < 2;
         -- found something on this filesystem, no need to go on
         varDSId := f.DiskServerId;
         varFSId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR lockError THEN
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
         AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
         AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
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
           INTO varDSId, diskServerName, mountPoint, varFSId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
          -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
            AND D.filesystem = f.fileSystemId
            AND StT.parent = streamId
            AND T.status = tconst.TAPECOPY_WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND ROWNUM < 2;
         -- found something on this filesystem, no need to go on
         varDSId := f.DiskServerId;
         varFSId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR lockError THEN
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
  UPDATE TapeCopy SET status = tconst.TAPECOPY_SELECTED
   WHERE id = tapeCopyId;
  IF varFindNewFS = 1 THEN
    UPDATE Stream
       SET status = tconst.STREAM_RUNNING,
           lastFileSystemUsed = varFSId,
           lastButOneFileSystemUsed = varLastFSUsed,
           lastFileSystemChange = getTime()
     WHERE id = streamId AND status IN (tconst.STREAM_WAITMOUNT, tconst.STREAM_RUNNING);
  ELSE
    -- only update status
    UPDATE Stream
       SET status = tconst.STREAM_RUNNING
     WHERE id = streamId AND status IN (tconst.STREAM_WAITMOUNT, tconst.STREAM_RUNNING);
  END IF;
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy
   WHERE child = tapeCopyId;

  -- Update Filesystem state
  updateFSMigratorOpened(varFsDiskServer, varFSId, 0); /* XXX TODO FIXME This variable is used uninitialized at the end of the function. Was already the case in revision 21389 */
END;
/

/* repack migration candidate selection policy */
CREATE OR REPLACE PROCEDURE repackMigrSelPolicy(streamId IN INTEGER,
                                                diskServerName OUT VARCHAR2, mountPoint OUT VARCHAR2,
                                                path OUT VARCHAR2, dci OUT INTEGER,
                                                castorFileId OUT INTEGER, fileId OUT INTEGER,
                                                nsHost OUT VARCHAR2, fileSize OUT INTEGER,
                                                tapeCopyId OUT INTEGER, lastUpdateTime OUT INTEGER) AS
  varFSId INTEGER := 0;
  varDSId NUMBER;
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
        AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
        AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
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
        INTO varDSId, diskServerName, mountPoint, varFSId, path, dci, castorFileId, fileId, nsHost, fileSize, tapeCopyId, lastUpdateTime
        FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
       WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
       -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
         AND D.filesystem = f.fileSystemId
         AND StT.parent = streamId
         AND T.status = tconst.TAPECOPY_WAITINSTREAMS
         AND StT.child = T.id
         AND T.castorfile = D.castorfile
         AND C.id = D.castorfile
         AND ROWNUM < 2;
      -- found something on this filesystem, no need to go on
      varDSId := f.DiskServerId;
      varFSId := f.fileSystemId;
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
  UPDATE TapeCopy SET status = tconst.TAPECOPY_SELECTED
   WHERE id = tapeCopyId;
  UPDATE Stream
     SET status = tconst.STREAM_RUNNING,
         lastFileSystemUsed = varFSId,
         lastButOneFileSystemUsed = lastFileSystemUsed,
         lastFileSystemChange = getTime()
   WHERE id = streamId AND status IN (tconst.STREAM_WAITMOUNT, tconst.STREAM_RUNNING);
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy
   WHERE child = tapeCopyId;

  -- Update Filesystem state
  updateFSMigratorOpened(varDSId, varFSId, 0);
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
     AND DiskCopy.status = dconst.DISKCOPY_WAITTAPERECALL;
  -- Check if the DiskCopy had a FileSystem associated
  IF fileSystemId > 0 THEN
    BEGIN
      -- It had one, force filesystem selection, unless it was disabled.
      SELECT DiskServer.name, DiskServer.id, FileSystem.mountPoint
        INTO diskServerName, fsDiskServer, rmountPoint
        FROM DiskServer, FileSystem
       WHERE FileSystem.id = fileSystemId
         AND FileSystem.status = dconst.FILESYSTEM_PRODUCTION
         AND DiskServer.id = FileSystem.diskServer
         AND DiskServer.status = dconst.DISKSERVER_PRODUCTION;
      updateFsRecallerOpened(fsDiskServer, fileSystemId, 0); -- XXX Is this a status?
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- Error, the filesystem or the machine was probably disabled in between
      raise_application_error(-20101, 'In a multi-segment file, FileSystem or Machine was disabled before all segments were recalled');
    END;
  ELSE
    fileSystemId := 0;
    -- The DiskCopy had no FileSystem associated with it which indicates that
    -- This is a new recall. We try and select a good FileSystem for it!
    FOR a IN (SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/
                     DiskServer.name, FileSystem.mountPoint, FileSystem.id,
                     FileSystem.diskserver, CastorFile.fileSize
                FROM DiskServer, FileSystem, DiskPool2SvcClass,
                     (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, svcClass from StageGetRequest                            UNION ALL
                      SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id, svcClass from StagePrepareToGetRequest UNION ALL
                      SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id, svcClass from StageRepackRequest                   UNION ALL
                      SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, svcClass from StageUpdateRequest                   UNION ALL
                      SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id, svcClass from StagePrepareToUpdateRequest) Request,
                      SubRequest, CastorFile
               WHERE CastorFile.id = cfid
                 AND SubRequest.castorfile = cfid
                 AND SubRequest.status = dconst.SUBREQUEST_WAITTAPERECALL
                 AND Request.id = SubRequest.request
                 AND Request.svcclass = DiskPool2SvcClass.child
                 AND FileSystem.diskpool = DiskPool2SvcClass.parent
                 AND FileSystem.free - FileSystem.minAllowedFreeSpace * FileSystem.totalSize > CastorFile.fileSize
                 AND FileSystem.status = dconst.FILESYSTEM_PRODUCTION
                 AND DiskServer.id = FileSystem.diskServer
                 AND DiskServer.status = dconst.DISKSERVER_PRODUCTION
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
         AND status = dconst.DISKCOPY_STAGED;
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
  UPDATE DiskCopy SET status = dconst.DISKCOPY_FAILED
   WHERE castorFile = cfId
     AND status = dconst.DISKCOPY_WAITTAPERECALL;
  -- Drop tape copies. Ideally, we should keep some track that
  -- the recall failed in order to prevent future recalls until some
  -- sort of manual intervention. For the time being, as we can't
  -- say whether the failure is fatal or not, we drop everything
  -- and we won't deny a future request for recall.
  deleteTapeCopies(cfId);
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest 
     SET status = dconst.SUBREQUEST_FAILED,
         getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED, -- (not strictly correct but the request is over anyway)
         lastModificationTime = getTime(),
         errorCode = 1015,  -- SEINTERNAL
         errorMessage = 'File recall from tape has failed, please try again later',
         parent = 0
   WHERE castorFile = cfId
     AND status IN (dconst.SUBREQUEST_WAITTAPERECALL, dconst.SUBREQUEST_WAITSUBREQ);
END;
/

/* PL/SQL method implementing streamsToDo */
CREATE OR REPLACE PROCEDURE streamsToDo(res OUT castorTape.Stream_Cur) AS
  sId NUMBER;
  streams "numList";
BEGIN
   -- JUST rtcpclientd
  FOR s IN (SELECT id FROM Stream WHERE status = tconst.STREAM_PENDING) LOOP
    BEGIN
      SELECT /*+ LEADING(Stream2TapeCopy TapeCopy DiskCopy FileSystem DiskServer) */
             s.id INTO sId
        FROM Stream2TapeCopy, TapeCopy, DiskCopy, FileSystem, DiskServer
       WHERE Stream2TapeCopy.parent = s.id
         AND Stream2TapeCopy.child = TapeCopy.id
         AND TapeCopy.castorFile = DiskCopy.CastorFile
         AND DiskCopy.fileSystem = FileSystem.id
         AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
         AND DiskServer.id = FileSystem.DiskServer
         AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
         AND ROWNUM < 2;
      INSERT INTO StreamsToDoHelper VALUES (sId);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- just ignore as this stream has no available candidate
      NULL;
    END;
  END LOOP;
  SELECT id BULK COLLECT INTO Streams FROM StreamsToDoHelper;
  FORALL i in streams.FIRST..streams.LAST
    UPDATE Stream SET status = tconst.STREAM_WAITDRIVE
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
     AND DiskCopy.status = dconst.DISKCOPY_WAITTAPERECALL;
  -- delete any previous failed diskcopy for this castorfile (due to failed recall attempts for instance)
  DELETE FROM Id2Type WHERE id IN (SELECT id FROM DiskCopy WHERE castorFile = cfId AND status = dconst.DISKCOPY_FAILED);
  DELETE FROM DiskCopy WHERE castorFile = cfId AND status = dconst.DISKCOPY_FAILED;
  -- update diskcopy size and gweight
  SELECT Request.svcClass, euid, egid INTO svcClassId, ouid, ogid
    FROM (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, svcClass, euid, egid FROM StageGetRequest                                  UNION ALL
          SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id, svcClass, euid, egid FROM StagePrepareToGetRequest       UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, svcClass, euid, egid FROM StageUpdateRequest                         UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id, svcClass, euid, egid FROM StagePrepareToUpdateRequest UNION ALL
          SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id, svcClass, euid, egid FROM StageRepackRequest) Request
   WHERE Request.id = requestId;
  gcwProc := castorGC.getRecallWeight(svcClassId);
  EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || gcwProc || '(:size); END;'
    USING OUT gcw, IN fs;
  UPDATE DiskCopy
     SET status = dconst.DISKCOPY_STAGED,
         lastAccessTime = getTime(),  -- for the GC, effective lifetime of this diskcopy starts now
         gcWeight = gcw,
         diskCopySize = fs
   WHERE id = dci;
  -- determine the type of the request
  SELECT type INTO reqType FROM Id2Type WHERE id =
    (SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ request FROM SubRequest WHERE id = subRequestId);
  IF reqType = 119 THEN  -- OBJ_StageRepackRequest
    startRepackMigration(subRequestId, cfId, dci, ouid, ogid);
  ELSE
    -- restart this subrequest if it's not a repack one
    UPDATE /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest
       SET status = dconst.SUBREQUEST_RESTART,
           getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED,
           lastModificationTime = getTime(), parent = 0
     WHERE id = subRequestId;
    -- And trigger new migrations if missing tape copies were detected
    IF missingTCs > 0 THEN
      DECLARE
        tcId INTEGER;
      BEGIN
        UPDATE DiskCopy
           SET status = dconst.DISKCOPY_CANBEMIGR
         WHERE id = dci;
        FOR i IN 1..missingTCs LOOP
          INSERT INTO TapeCopy (id, copyNb, castorFile, status)
          VALUES (ids_seq.nextval, 0, cfId, tconst.TAPECOPY_CREATED)
          RETURNING id INTO tcId;
          INSERT INTO Id2Type (id, type) VALUES (tcId, 30); -- OBJ_TapeCopy
        END LOOP;
      END;
    END IF;
  END IF;
  -- restart other requests waiting on this recall
  UPDATE /*+ INDEX(ST I_SUBREQUEST_PARENT) */ SubRequest ST
       SET status = dconst.SUBREQUEST_RESTART,
           getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED,
           lastModificationTime = getTime(), parent = 0
   WHERE parent = subRequestId;
  -- Trigger the creation of additional copies of the file, if necessary.
  replicateOnClose(cfId, ouid, ogid);
END;
/

CREATE OR REPLACE PROCEDURE deleteOrStopStream(streamId IN INTEGER) AS
-- If the specified stream has no tape copies then this procedure deletes it,
-- else if the stream has tape copies then this procedure sets its status to
-- STREAM_STOPPED.  In both cases the associated tape is detached from the
-- stream and its status is set to TAPE_UNUSED.

  CHILD_RECORD_FOUND EXCEPTION;
  PRAGMA EXCEPTION_INIT(CHILD_RECORD_FOUND, -02292);
  DEADLOCK_DETECTED EXCEPTION;
  PRAGMA EXCEPTION_INIT(DEADLOCK_DETECTED, -00060);
  unused NUMBER;

BEGIN
  -- Try to take a lock on the stream, taking note that the stream may already
  -- have been delete because the migrator, rtcpclientd and mighunterd race to
  -- delete streams
  BEGIN
    SELECT id INTO unused FROM Stream WHERE id = streamId FOR UPDATE;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- Return because the stream has already been deleted
    RETURN;
  END;

  -- Try to delete the stream.  If the mighunter daemon is running in
  -- rtcpclientd mode, then this delete may fail for two expected reasons.  The
  -- mighunterd daemon may have added more tape copies in the meantime and
  -- will therefore cause a CHILD_RECORD_FOUND exception due to the
  -- corresponding entries in the Stream2TapeCopy table.  The mighunter daemon
  -- may be adding new tape copies right this moment and will therefore cause a
  -- DEADLOCK_DETECTED exception.
  BEGIN
    DELETE FROM Stream  WHERE id = streamId;
    DELETE FROM Id2Type WHERE id = streamId;
  EXCEPTION
    -- When the stream cannot be deleted
    WHEN CHILD_RECORD_FOUND OR DEADLOCK_DETECTED THEN
      -- Stop the stream and reset its tape link and last file system change
      UPDATE Stream
        SET
          status = TCONST.STREAM_STOPPED,
          tape = NULL,
          lastFileSystemChange = NULL
        WHERE
          id = streamId;
  END;

  -- Complete the detachment of the tape
  UPDATE Tape
    SET status = TCONST.TAPE_UNUSED, stream = NULL
    WHERE stream = streamId;
END deleteOrStopStream;
/

/* PL/SQL method implementing resetStream */
CREATE OR REPLACE PROCEDURE resetStream (streamId IN INTEGER) AS
BEGIN
  deleteOrStopStream(streamId);
END;
/

/* PL/SQL method implementing segmentsForTape */
CREATE OR REPLACE PROCEDURE segmentsForTape (tapeId IN INTEGER, segments
OUT castor.Segment_Cur) AS
  segs "numList";
  rows PLS_INTEGER := 500;
  CURSOR c1 IS
    SELECT Segment.id FROM Segment
     WHERE Segment.tape = tapeId AND Segment.status = tconst.SEGMENT_UNPROCESSED ORDER BY Segment.fseq
    FOR UPDATE;
BEGIN
  -- JUST rtcpclientd
  OPEN c1;
  FETCH c1 BULK COLLECT INTO segs LIMIT rows;
  CLOSE c1;

  IF segs.COUNT > 0 THEN
    UPDATE Tape SET status = tconst.TAPE_MOUNTED
     WHERE id = tapeId;
    FORALL j IN segs.FIRST..segs.LAST -- bulk update with the forall..
      UPDATE Segment SET status = tconst.SEGMENT_SELECTED
       WHERE id = segs(j);
  END IF;

  OPEN segments FOR
    SELECT fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm,
           segmCksum, errMsgTxt, errorCode, severity, blockId0, blockId1,
           blockId2, blockId3, creationTime, id, tape, copy, status, priority
      FROM Segment
     WHERE id IN (SELECT /*+ CARDINALITY(segsTable 5) */ *
                    FROM TABLE(segs) segsTable);
END;
/

/* PL/SQL method implementing anySegmentsForTape */
CREATE OR REPLACE PROCEDURE anySegmentsForTape
(tapeId IN INTEGER, nb OUT INTEGER) AS
BEGIN
  -- JUST rtcpclientd
  SELECT count(*) INTO nb FROM Segment
   WHERE Segment.tape = tapeId
     AND Segment.status = tconst.SEGMENT_UNPROCESSED;
  IF nb > 0 THEN
    UPDATE Tape SET status = tconst.TAPE_WAITMOUNT
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
    SELECT fseq, offset, bytes_in, bytes_out, host_bytes, segmCksumAlgorithm,
           segmCksum, errMsgTxt, errorCode, severity, blockId0, blockId1,
           blockId2, blockId3, creationTime, id, tape, copy, status, priority
      FROM Segment
     WHERE Segment.status = tconst.SEGMENT_FAILED;
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
  SELECT /*+ INDEX(Subrequest I_Subrequest_DiskCopy)*/ SubRequest.id, StageRepackRequest.repackvid
    INTO sreqid, ret
    FROM SubRequest, DiskCopy, CastorFile, StageRepackRequest
   WHERE stagerepackrequest.id = subrequest.request
     AND diskcopy.id = subrequest.diskcopy
     AND diskcopy.status = dconst.DISKCOPY_CANBEMIGR
     AND subrequest.status = dconst.SUBREQUEST_REPACK
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
  unused VARCHAR2(2048);
BEGIN
  SELECT value INTO unused
    FROM CastorConfig
   WHERE class = 'tape'
     AND KEY = 'interfaceDaemon'
     AND value = 'rtcpclientd';
  -- JUST rtcpclientd
  -- Deal with Migrations
  -- 1) Ressurect tapecopies for migration
  UPDATE TapeCopy SET status = tconst.TAPECOPY_TOBEMIGRATED WHERE status = tconst.TAPECOPY_SELECTED;
  -- 2) Clean up the streams
  UPDATE Stream SET status = tconst.STREAM_PENDING 
   WHERE status NOT IN (tconst.STREAM_PENDING, tconst.STREAM_CREATED, tconst.STREAM_STOPPED, tconst.STREAM_WAITPOLICY)
  RETURNING tape BULK COLLECT INTO tpIds;
  UPDATE Stream SET tape = NULL WHERE tape != 0;
  -- 3) Reset the tape for migration
  FORALL i IN tpIds.FIRST .. tpIds.LAST  
    UPDATE tape SET stream = 0, status = tconst.TAPE_UNUSED -- XXX Should not be NULL?
     WHERE status IN (tconst.TAPE_WAITDRIVE, tconst.TAPE_WAITMOUNT, tconst.TAPE_MOUNTED) AND id = tpIds(i);

  -- Deal with Recalls
  UPDATE Segment SET status = tconst.SEGMENT_UNPROCESSED
   WHERE status = tconst.SEGMENT_SELECTED; -- Resurrect SELECTED segment
  UPDATE Tape SET status = tconst.TAPE_PENDING
   WHERE tpmode = tconst.TPMODE_READ AND status IN (tconst.TAPE_WAITDRIVE, tconst.TAPE_WAITMOUNT, tconst.TAPE_MOUNTED); -- Resurrect the tapes running for recall
  UPDATE Tape A SET status = tconst.TAPE_WAITPOLICY
   WHERE status IN (tconst.TAPE_UNUSED, tconst.TAPE_FAILED, tconst.TAPE_UNKNOWN) AND EXISTS
    (SELECT id FROM Segment WHERE status = tconst.SEGMENT_UNPROCESSED AND tape = A.id);
  COMMIT;
END;
/

/** Functions for the MigHunterDaemon **/

CREATE OR REPLACE PROCEDURE migHunterCleanUp(svcName IN VARCHAR2)
AS
-- Cleans up the migration-hunter data in the database.
--
-- This procedure is called during the start-up logic of a new migration-hunter
-- daemon.
--
-- This procedure raises application error -20001 if the service-class
-- specified by svcName is unknown.
  svcId NUMBER;
BEGIN
  -- Get the database-ID of the service-class with the specified name
  BEGIN
    SELECT id INTO svcId FROM SvcClass WHERE name = svcName;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RAISE_APPLICATION_ERROR(-20001,
      'Failed to clean-up the migration-hunter data in the database' ||
      ': No such service-class' ||
      ': svcName=' || svcName);
  END;

  -- clean up tapecopies, WAITPOLICY reset into TOBEMIGRATED
  UPDATE
     /*+ LEADING(TC CF)
         INDEX_RS_ASC(CF PK_CASTORFILE_ID)
         INDEX_RS_ASC(TC I_TAPECOPY_STATUS) */ 
         TapeCopy TC
     SET status = tconst.TAPECOPY_TOBEMIGRATED
   WHERE status = tconst.TAPECOPY_WAITPOLICY
     AND EXISTS (
       SELECT 'x' 
         FROM CastorFile CF
        WHERE TC.castorFile = CF.id
          AND CF.svcclass = svcId);
  -- clean up streams, WAITPOLICY reset into CREATED
  UPDATE Stream SET status = tconst.STREAM_CREATED WHERE status = tconst.STREAM_WAITPOLICY AND tapepool IN
   (SELECT svcclass2tapepool.child
      FROM svcclass2tapepool
     WHERE svcId = svcclass2tapepool.parent);
  COMMIT;
END;
/


/* Gets the tape copies to be attached to the streams of the specified service class. */
CREATE OR REPLACE PROCEDURE inputForMigrationPolicy(
  svcclassName IN  VARCHAR2,
  policyName   OUT NOCOPY VARCHAR2,
  svcId        OUT NUMBER,
  dbInfo       OUT castorTape.DbMigrationInfo_Cur) AS
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
     SET status = tconst.TAPECOPY_WAITPOLICY
   WHERE status IN (tconst.TAPECOPY_CREATED, tconst.TAPECOPY_TOBEMIGRATED)
     AND (EXISTS
       (SELECT 'x' FROM SubRequest, StageRepackRequest
         WHERE StageRepackRequest.svcclass = svcId
           AND SubRequest.request = StageRepackRequest.id
           AND SubRequest.status = dconst.SUBREQUEST_REPACK
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
     AND Stream.status = tconst.STREAM_RUNNING;
  UPDATE stream SET status = tconst.STREAM_WAITPOLICY
   WHERE Stream.status IN (tconst.STREAM_WAITSPACE, tconst.STREAM_CREATED, tconst.STREAM_STOPPED)
     AND Stream.id
      IN (SELECT Stream.id FROM Stream,SvcClass2TapePool
           WHERE Stream.Tapepool = SvcClass2TapePool.child
             AND SvcClass2TapePool.parent = svcId)
  RETURNING Stream.id BULK COLLECT INTO strIds;
  COMMIT;
  
  -- check for overloaded streams
  SELECT count(*) INTO tcNum FROM stream2tapecopy 
   WHERE parent IN 
    (SELECT /*+ CARDINALITY(stridTable 5) */ *
       FROM TABLE(strIds) stridTable);
  IF (tcnum > 10000 * maxstream) AND (maxstream > 0) THEN
    -- emergency mode
    OPEN dbInfo FOR
      SELECT Stream.id, 10000, 10000, gettime
        FROM Stream
       WHERE Stream.id IN
         (SELECT /*+ CARDINALITY(stridTable 5) */ *
            FROM TABLE(strIds) stridTable)
         AND Stream.status = tconst.STREAM_WAITPOLICY
       GROUP BY Stream.id;
  ELSE
  -- return for policy
  OPEN dbInfo FOR
    SELECT /*+ INDEX(CastorFile PK_CastorFile_Id) */ Stream.id,
           count(distinct Stream2TapeCopy.child),
           sum(CastorFile.filesize), gettime() - min(CastorFile.creationtime)
      FROM Stream2TapeCopy, TapeCopy, CastorFile, Stream
     WHERE Stream.id IN
        (SELECT /*+ CARDINALITY(stridTable 5) */ *
           FROM TABLE(strIds) stridTable)
       AND Stream2TapeCopy.child = TapeCopy.id
       AND TapeCopy.castorfile = CastorFile.id
       AND Stream.id = Stream2TapeCopy.parent
       AND Stream.status = tconst.STREAM_WAITPOLICY
     GROUP BY Stream.id
   UNION ALL
    SELECT Stream.id, 0, 0, 0
      FROM Stream WHERE Stream.id IN
        (SELECT /*+ CARDINALITY(stridTable 5) */ *
           FROM TABLE(strIds) stridTable)
       AND Stream.status = tconst.STREAM_WAITPOLICY
       AND NOT EXISTS 
        (SELECT 'x' FROM Stream2TapeCopy ST WHERE ST.parent = Stream.ID);
  END IF;         
END;
/


CREATE OR REPLACE PROCEDURE streamsForStreamPolicy (
  inSvcClassName                 IN  VARCHAR2,
  outSvcClassId                  OUT NUMBER,
  outStreamPolicyName            OUT NOCOPY VARCHAR2,
  outNbDrives                    OUT INTEGER,
  outStreamsForPolicy            OUT castorTape.StreamForPolicy_Cur)
/**
 * For the service-class specified by inSvcClassName, this procedure gets the
 * service-class database ID, the stream-policy name and the list of candidate
 * streams to be passed to the stream-policy.
 *
 * Please note the list of candidate streams includes streams with no
 * tape-copies attached.
 *
 * On success this function sets and commits the status of the streams for the
 * stream-policy to STREAM_WAITPOLICY (tragic number 7).
 *
 * This procedure raises application error -20001 if the service-class specified
 * by inSvcClassName is unknown.  In this case no modification is made to the
 * database and no commit is executed.
 *
 * @param inSvcClassName      The name of the service-class 
 * @param outSvcClassId       The database ID of the service-class.
 * @param outStreamPolicyName The name of the stream-policy of the
 *                            service-class.
 * @param outNbDrives         The maximum number of drives the service-class
 *                            can use at any single moment in time.
 * @param outStreamsForPolicy Cursor to the set of candidate streams to be
 *                            processed by the stream-policy.
 */
AS
  varStreamIds "numList"; -- Stream ids to be passed to the stream-policy
  varNumTapeCopies NUMBER := 0;  -- Number of tape-copies on the policy-streams
  varTooManyTapeCopiesToQuery BOOLEAN := FALSE; -- True if there are too many
                                                -- tape-copies to query
BEGIN
  -- Get the id, stream-policy name and number of drives of the service-class
  -- specified by inSvcClassName
  BEGIN
    SELECT id, streamPolicy, nbDrives
      INTO outSvcClassId, outStreamPolicyName, outNbDrives
      FROM SvcClass
     WHERE SvcClass.name = inSvcClassName;
  EXCEPTION
    WHEN NO_DATA_FOUND THEN
      RAISE_APPLICATION_ERROR(-20001,
        'Unknown service-class name' ||
        ': inSvcClassName=' || inSvcClassName);
  END;

  -- Mark the streams to be processed by the stream-policy
  --
  -- Note that there is a COMMIT statement which means the database cannot help
  -- if the mighunter daemon crashes and forgets which streams it has marked
  -- for itself
  --
  -- Note that there is a race-condition between the MigHunterThread attaching
  -- tape-copies to newly created and empty streams and the the StreamThread
  -- deleting newly created threads with no tape-copies attached to them
  UPDATE Stream
     SET Stream.status = tconst.STREAM_WAITPOLICY
   WHERE Stream.status IN (tconst.STREAM_WAITSPACE, tconst.STREAM_CREATED, tconst.STREAM_STOPPED)
     AND Stream.id IN (
           SELECT Stream.id
             FROM Stream
            INNER JOIN SvcClass2TapePool
               ON (Stream.Tapepool = SvcClass2TapePool.child)
            WHERE SvcClass2TapePool.parent = outSvcClassId)
  RETURNING Stream.id BULK COLLECT INTO varStreamIds;
  COMMIT;
  
  -- Get the total number of tape-copies on the policy-streams
  SELECT count(*)
    INTO varNumTapeCopies
    FROM Stream2tapecopy 
   WHERE parent IN (
           SELECT /*+ CARDINALITY(streamIdTable 5) */ *
             FROM TABLE(varStreamIds) streamIdTable);

  -- Determine whether or not there are too many tape-copies to query, taking
  -- into account that nbDrives may have been modified and may be invalid
  -- (nbDrives < 1)
  IF outNbDrives >= 1 THEN
    varTooManyTapeCopiesToQuery := varNumTapeCopies > 10000 * outNbDrives;
  ELSE
    varTooManyTapeCopiesToQuery := varNumTapeCopies > 10000;
  END IF;

  IF varTooManyTapeCopiesToQuery THEN
    -- Enter emergency mode
    OPEN outStreamsForPolicy FOR
      SELECT Stream.id,
             10000, -- numTapeCopies
             10000*1073741824, -- totalBytes (Force file size to be 1 GiB)
             48*3600, -- ageOfOldestTapeCopy (Force age to be 48 hours)
             Stream.tapepool
        FROM Stream
       WHERE Stream.id IN (
                SELECT /*+ CARDINALITY(streamIdTable 5) */ *
                  FROM TABLE(varStreamIds) streamIdTable)
         AND Stream.status = tconst.STREAM_WAITPOLICY;
  ELSE
    OPEN outStreamsForPolicy FOR
      SELECT /*+ INDEX(CastorFile PK_CastorFile_Id) */ Stream.id,
             count(Stream2TapeCopy.child), -- numTapeCopies
             sum(CastorFile.filesize), -- totalBytes
             gettime() - min(CastorFile.creationtime), -- ageOfOldestTapeCopy
             Stream.tapepool
        FROM Stream2TapeCopy
       INNER JOIN Stream     ON (Stream2TapeCopy.parent = Stream.id    )
       INNER JOIN TapeCopy   ON (Stream2TapeCopy.child  = TapeCopy.id  )
       INNER JOIN CastorFile ON (TapeCopy.castorFile    = CastorFile.id)
       WHERE Stream.id IN (
               SELECT /*+ CARDINALITY(stridTable 5) */ *
                 FROM TABLE(varStreamIds) streamIdTable)
                  AND Stream.status = tconst.STREAM_WAITPOLICY
       GROUP BY Stream.tapepool, Stream.id
      UNION ALL /* Append streams with no tape-copies attached */
      SELECT Stream.id,
             0, -- numTapeCopies
             0, -- totalBytes
             0, -- ageOfOldestTapeCopy
             0  -- tapepool
        FROM Stream
       WHERE Stream.id IN (
               SELECT /*+ CARDINALITY(streamIdTable 5) */ *
                 FROM TABLE(varStreamIds) streamIdTable)
         AND Stream.status = tconst.STREAM_WAITPOLICY
         AND NOT EXISTS (
               SELECT 'x'
                 FROM Stream2TapeCopy ST
                WHERE ST.parent = Stream.ID);
  END IF;         
END streamsForStreamPolicy;
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

        INSERT INTO Stream
          (id, initialsizetotransfer, lastFileSystemChange, tape, lastFileSystemUsed,
           lastButOneFileSystemUsed, tapepool, status)
        VALUES (ids_seq.nextval, initSize, NULL, NULL, NULL, NULL, tpId, tconst.STREAM_CREATED)
        RETURN id INTO strId;
        INSERT INTO Id2Type (id, type) VALUES (strId,26); -- Stream type
    	IF doClone = 1 THEN
	  BEGIN
	    -- clone the new stream with one from the same tapepool
	    SELECT id, initialsizetotransfer INTO streamToClone, oldSize
              FROM Stream WHERE tapepool = tpId AND id != strId AND ROWNUM < 2;
            FOR tcId IN (SELECT child FROM Stream2TapeCopy
                          WHERE Stream2TapeCopy.parent = streamToClone)
            LOOP
              -- a take the first one, they are supposed to be all the same
              INSERT INTO stream2tapecopy (parent, child)
              VALUES (strId, tcId.child);
            END LOOP;
            UPDATE Stream SET initialSizeToTransfer = oldSize
             WHERE id = strId;
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

/* attach tapecopies to streams for rtcpclientd */
CREATE OR REPLACE PROCEDURE attachTCRtcp
(tapeCopyIds IN castor."cnumList",
 tapePoolId IN NUMBER)
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
       WHERE Stream.tapepool = tapePoolId;
      IF nbStream <> 0 THEN
        -- we have at least a stream for that tapepool
        SELECT id INTO unused
          FROM TapeCopy
         WHERE status IN (tconst.TAPECOPY_WAITINSTREAMS, tconst.TAPECOPY_WAITPOLICY) AND id = tapeCopyIds(i) FOR UPDATE;
        -- let's attach it to the different streams
        FOR streamId IN (SELECT id FROM Stream
                          WHERE Stream.tapepool = tapePoolId ) LOOP
          UPDATE TapeCopy SET status = tconst.TAPECOPY_WAITINSTREAMS
           WHERE status = tconst.TAPECOPY_WAITPOLICY AND id = tapeCopyIds(i);
          DECLARE CONSTRAINT_VIOLATED EXCEPTION;
          PRAGMA EXCEPTION_INIT (CONSTRAINT_VIOLATED, -1);
          BEGIN
            INSERT INTO stream2tapecopy (parent ,child)
            VALUES (streamId.id, tapeCopyIds(i));
          EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
            -- if the stream does not exist anymore
            UPDATE tapecopy SET status = tconst.TAPECOPY_WAITPOLICY WHERE id = tapeCopyIds(i);
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
    UPDATE TapeCopy SET status = tconst.TAPECOPY_TOBEMIGRATED WHERE id = tapeCopyIds(i) AND status = tconst.TAPECOPY_WAITPOLICY;
  COMMIT;
END;
/

/* attach tapecopies to streams for tapegateway */
CREATE OR REPLACE PROCEDURE attachTCGateway
(tapeCopyIds IN castor."cnumList",
 tapePoolId IN NUMBER)
AS
  unused NUMBER;
  streamId NUMBER; -- stream attached to the tapepool
  nbTapeCopies NUMBER;
BEGIN
  -- WARNING: tapegateway ONLY version
  FOR str IN (SELECT id FROM Stream WHERE tapepool = tapePoolId) LOOP
    BEGIN
      -- add choosen tapecopies to all Streams associated to the tapepool used by the policy
      SELECT id INTO streamId FROM stream WHERE id = str.id FOR UPDATE;
      -- add choosen tapecopies to all Streams associated to the tapepool used by the policy
      FOR i IN tapeCopyIds.FIRST .. tapeCopyIds.LAST LOOP
         BEGIN     
           SELECT /*+ index(tapecopy, PK_TAPECOPY_ID)*/ id INTO unused
             FROM TapeCopy
            WHERE Status in (tconst.TAPECOPY_WAITINSTREAMS, tconst.TAPECOPY_WAITPOLICY) AND id = tapeCopyIds(i) FOR UPDATE;
           DECLARE CONSTRAINT_VIOLATED EXCEPTION;
           PRAGMA EXCEPTION_INIT (CONSTRAINT_VIOLATED, -1);
           BEGIN
             INSERT INTO stream2tapecopy (parent ,child)
             VALUES (streamId, tapeCopyIds(i));
             UPDATE /*+ index(tapecopy, PK_TAPECOPY_ID)*/ TapeCopy
                SET Status = tconst.TAPECOPY_WAITINSTREAMS WHERE status = tconst.TAPECOPY_WAITPOLICY AND id = tapeCopyIds(i); 
           EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
             -- if the stream does not exist anymore
             -- it might also be that the tapecopy does not exist anymore
             -- already exist the tuple parent-child
             NULL;
           END;
         EXCEPTION WHEN NO_DATA_FOUND THEN
           -- Go on the tapecopy has been resurrected or migrated
           NULL;
         END;
      END LOOP; -- loop tapecopies
      COMMIT;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- no stream anymore
      NULL;
    END;
  END LOOP; -- loop streams

  -- resurrect the ones never attached
  FORALL i IN tapeCopyIds.FIRST .. tapeCopyIds.LAST
    UPDATE TapeCopy SET status = tconst.TAPECOPY_TOBEMIGRATED WHERE id = tapeCopyIds(i) AND status = tconst.TAPECOPY_WAITPOLICY;
  COMMIT;
END;
/

/* generic attach tapecopies to stream */
CREATE OR REPLACE PROCEDURE attachTapeCopiesToStreams 
(tapeCopyIds IN castor."cnumList",
 tapePoolId IN NUMBER) AS
  unused VARCHAR2(2048);
BEGIN
  BEGIN
    SELECT value INTO unused
      FROM CastorConfig
     WHERE class = 'tape'
       AND key   = 'interfaceDaemon'
       AND value = 'tapegatewayd';
  EXCEPTION WHEN NO_DATA_FOUND THEN  -- rtcpclientd
    attachTCRtcp(tapeCopyIds, tapePoolId);
    RETURN;
  END;
  -- tapegateway
  attachTCGateway(tapeCopyIds, tapePoolId);
END;
/

/* start choosen stream */
CREATE OR REPLACE PROCEDURE startChosenStreams
  (streamIds IN castor."cnumList") AS
BEGIN
  IF (TapegatewaydIsRunning) THEN
    FORALL i IN streamIds.FIRST .. streamIds.LAST
      UPDATE Stream S
         SET S.status = tconst.STREAM_PENDING,
             S.TapeGatewayRequestId = ids_seq.nextval
       WHERE S.status = tconst.STREAM_WAITPOLICY
         AND S.id = streamIds(i);
  ELSE
    FORALL i IN streamIds.FIRST .. streamIds.LAST
      UPDATE Stream S
         SET S.status = tconst.STREAM_PENDING
       WHERE S.status = tconst.STREAM_WAITPOLICY
         AND S.id = streamIds(i);
  END IF;
  COMMIT;
END;
/

/* stop chosen stream */
CREATE OR REPLACE PROCEDURE stopChosenStreams
        (streamIds IN castor."cnumList") AS
  nbTc NUMBER;
BEGIN
  FOR i IN streamIds.FIRST .. streamIds.LAST LOOP
    deleteOrStopStream(streamIds(i));
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
    UPDATE TapeCopy SET status = tconst.TAPECOPY_TOBEMIGRATED WHERE status = tconst.TAPECOPY_WAITPOLICY
       AND id = migrationCandidates(i);
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
    UPDATE TapeCopy SET status = tconst.TAPECOPY_FAILED WHERE id = tapecopyIds(i) AND status = tconst.TAPECOPY_WAITPOLICY;

  -- repack subrequests to be archived
  FOR i IN tapecopyIds.FIRST .. tapecopyIds.LAST LOOP
    BEGIN
      SELECT subrequest.id INTO srId FROM subrequest, tapecopy 
       WHERE subrequest.castorfile = tapecopy.castorfile
         AND tapecopy.id = tapecopyIds(i)
         AND subrequest.status = dconst.SUBREQUEST_REPACK;
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
       Tape.id,
       Tape.vid,
       count(distinct segment.id),
       sum(CastorFile.fileSize),
       getTime() - min(Segment.creationTime) age,
       max(Segment.priority)
      FROM TapeCopy, CastorFile, Segment, Tape
     WHERE Tape.id = Segment.tape
       AND TapeCopy.id = Segment.copy
       AND CastorFile.id = TapeCopy.castorfile
       AND Tape.status IN (tconst.TAPE_PENDING, tconst.TAPE_WAITDRIVE, tconst.TAPE_WAITPOLICY)
       AND Segment.status = tconst.SEGMENT_UNPROCESSED
     GROUP BY Tape.id, Tape.vid
     ORDER BY age DESC;
END;
/

CREATE OR REPLACE PROCEDURE tapesAndMountsForRecallPolicy (
  outRecallMounts      OUT castorTape.RecallMountsForPolicy_Cur,
  outNbMountsForRecall OUT NUMBER)
AS
-- Retrieves the input for the rechandler daemon to pass to the
-- rechandler-policy Python-function
--
-- @param outRecallMounts      List of recall-mounts which are either pending,
--                             waiting for a drive or waiting for the
--                             rechandler-policy.
-- @param outNbMountsForRecall The number of tapes currently mounted for recall
--                             for this stager.
BEGIN
  SELECT count(distinct Tape.vid )
    INTO outNbMountsForRecall
    FROM Tape
   WHERE Tape.status = tconst.TAPE_MOUNTED
     AND TPMODE = tconst.TPMODE_READ;

    OPEN outRecallMounts
     FOR SELECT /*+ NO_USE_MERGE(TAPE SEGMENT TAPECOPY CASTORFILE) NO_USE_HASH(TAPE SEGMENT TAPECOPY CASTORFILE) INDEX_RS_ASC(SEGMENT I_SEGMENT_TAPE) INDEX_RS_ASC(TAPE I_TAPE_STATUS) INDEX_RS_ASC(TAPE
COPY PK_TAPECOPY_ID) INDEX_RS_ASC(CASTORFILE PK_CASTORFILE_ID) */ Tape.id,
                Tape.vid,
                count ( distinct segment.id ),
                sum ( CastorFile.fileSize ),
                getTime ( ) - min ( Segment.creationTime ) age,
                max ( Segment.priority ),
                Tape.status
           FROM TapeCopy,
                CastorFile,
                Segment,
                Tape
          WHERE Tape.id = Segment.tape
            AND TapeCopy.id = Segment.copy
            AND CastorFile.id = TapeCopy.castorfile
            AND Tape.status IN (tconst.TAPE_PENDING, tconst.TAPE_WAITDRIVE, tconst.TAPE_WAITPOLICY)
            AND Segment.status = tconst.SEGMENT_UNPROCESSED
          GROUP BY Tape.id, Tape.vid, Tape.status
          ORDER BY age DESC;
    
END tapesAndMountsForRecallPolicy;
/

/* resurrect tapes */
CREATE OR REPLACE PROCEDURE resurrectTapes
(tapeIds IN castor."cnumList")
AS
BEGIN
  IF (TapegatewaydIsRunning) THEN
    FOR i IN tapeIds.FIRST .. tapeIds.LAST LOOP
      UPDATE Tape T
         SET T.TapegatewayRequestId = ids_seq.nextval,
             T.status = tconst.TAPE_PENDING
       WHERE T.status = tconst.TAPE_WAITPOLICY AND T.id = tapeIds(i);
      -- XXX FIXME TODO this is a hack needed to add the TapegatewayRequestId which was missing.
      UPDATE Tape T
         SET T.TapegatewayRequestId = ids_seq.nextval
       WHERE T.status = tconst.TAPE_PENDING AND T.TapegatewayRequestId IS NULL 
         AND T.id = tapeIds(i);
    END LOOP; 
  ELSE
    FORALL i IN tapeIds.FIRST .. tapeIds.LAST
      UPDATE Tape SET status = tconst.TAPE_PENDING WHERE status = tconst.TAPE_WAITPOLICY AND id = tapeIds(i);
  END IF;
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
  -- note that if the request is over (all in 9,11) or not started (0), nothing is done
  SELECT id INTO reqId 
    FROM StageRepackRequest R 
   WHERE repackVid = inputVid
     AND EXISTS 
       (SELECT 1 FROM SubRequest 
         WHERE request = R.id AND status IN (dconst.SUBREQUEST_WAITTAPERECALL, dconst.SUBREQUEST_REPACK));
  -- fail subrequests
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Request)*/ Subrequest SET status = dconst.SUBREQUEST_FAILED_FINISHED
   WHERE request = reqId AND status NOT IN (dconst.SUBREQUEST_FAILED_FINISHED, dconst.SUBREQUEST_ARCHIVED)
  RETURNING castorFile, diskcopy BULK COLLECT INTO cfIds, dcIds;
  SELECT /*+ INDEX(Subrequest I_Subrequest_Request)*/ id INTO srId 
    FROM SubRequest 
   WHERE request = reqId AND ROWNUM = 1;
  archiveSubReq(srId, 9);

  -- fail related diskcopies
  FORALL i IN dcIds.FIRST .. dcids.LAST
    UPDATE DiskCopy
       SET status = decode(status, dconst.DISKCOPY_WAITTAPERECALL, dconst.DISKCOPY_FAILED, dconst.DISKCOPY_INVALID) -- WAITTAPERECALL->FAILED, otherwise INVALID
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
  UPDATE tape SET status = tconst.TAPE_UNUSED WHERE vid = inputVid AND tpmode = tconst.TPMODE_READ;
  -- update other tapes which could have been involved
  FORALL i IN tapeIds.FIRST .. tapeIds.LAST
    UPDATE tape SET status = tconst.TAPE_UNUSED WHERE id = tapeIds(i);
  -- commit the transation
  COMMIT;
EXCEPTION WHEN NO_DATA_FOUND THEN 
  COMMIT;
END;
/

/*
restartStuckRecalls is a wrokaround procedure required by the rtcpclientd
daemon.

Restart the (recall) segments that are recognized as stuck.
This workaround (sr #112306: locking issue in CMS stager)
will be dropped as soon as the TapeGateway will be used in production.

Notes for query readability:
TAPE status:    (0)TAPE_UNUSED, (1)TAPE_PENDING, (2)TAPE_WAITDRIVE, 
                (3)TAPE_WAITMOUNT, (6)TAPE_FAILED
SEGMENT status: (0)SEGMENT_UNPROCESSED, (7)SEGMENT_SELECTED
*/
CREATE OR REPLACE PROCEDURE restartStuckRecalls AS
  unused VARCHAR2(2048);
BEGIN
  -- Do nothing and return if the tape gateway is running
  BEGIN
    SELECT value INTO unused
      FROM CastorConfig
     WHERE class = 'tape'
       AND key   = 'interfaceDaemon'
       AND value = 'tapegatewayd';
     RETURN;
  EXCEPTION WHEN NO_DATA_FOUND THEN  -- rtcpclientd
    -- Do nothing and continue
    NULL;
  END;

  -- Notes for query readability:
  -- TAPE status:    (0)TAPE_UNUSED, (1)TAPE_PENDING, (2)TAPE_WAITDRIVE, 
  --                 (3)TAPE_WAITMOUNT, (6)TAPE_FAILED
  -- SEGMENT status: (0)SEGMENT_UNPROCESSED, (7)SEGMENT_SELECTED

  -- Mark as unused all of the recall tapes whose state maybe stuck due to an
  -- rtcpclientd crash. Such tapes will be pending, waiting for a drive, or
  -- waiting for a mount, and will be associated with segments that are neither
  -- un-processed nor selected.
  UPDATE tape SET status=tconst.TAPE_UNUSED
   WHERE tpmode = tconst.TPMODE_READ
     AND status IN (tconst.TAPE_PENDING, tconst.TAPE_WAITDRIVE, tconst.TAPE_WAITMOUNT)
     AND id NOT IN (SELECT tape FROM segment 
                     WHERE status IN (tconst.SEGMENT_UNPROCESSED, tconst.SEGMENT_SELECTED));

  -- Mark as unprocessed all recall segments that are marked as being selected
  -- and are associated with unused or failed recall tapes that have 1 or more
  -- unprocessed or selected segments.
  UPDATE segment SET status = tconst.SEGMENT_UNPROCESSED 
   WHERE status = tconst.SEGMENT_SELECTED 
     AND tape IN (SELECT id FROM tape WHERE tpmode = tconst.TPMODE_READ 
                     AND status IN (tconst.TAPE_UNUSED, tconst.TAPE_FAILED) 
                     AND id IN (SELECT tape FROM segment 
                                 WHERE status IN (tconst.SEGMENT_UNPROCESSED, tconst.SEGMENT_SELECTED))
                  );

  -- Mark as pending all recall tapes that are unused or failed, and have
  -- unprocessed and selected segments.
  UPDATE tape SET status = tconst.TAPE_PENDING
   WHERE tpmode = tconst.TPMODE_READ 
     AND status IN (tconst.TAPE_UNUSED, tconst.TAPE_FAILED) 
     AND id IN (SELECT tape FROM segment 
                 WHERE status IN (tconst.SEGMENT_UNPROCESSED, tconst.SEGMENT_SELECTED));

  COMMIT;
END restartStuckRecalls;
/


/*
The default state of the stager database is to be compatible with the
rtcpclientd daemon as opposed to the tape gateway daemon.  Therefore create the
restartStuckRecallsJob which will call the restartStuckRecalls workaround
procedure every hour.
*/
BEGIN
  -- Remove database jobs before recreating them
  FOR j IN (SELECT job_name FROM user_scheduler_jobs
             WHERE job_name IN ('RESTARTSTUCKRECALLSJOB'))
  LOOP
    DBMS_SCHEDULER.DROP_JOB(j.job_name, TRUE);
  END LOOP;

  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'RESTARTSTUCKRECALLSJOB',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN restartStuckRecalls(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=60',
      ENABLED         => TRUE,
      COMMENTS        => 'Workaround to restart stuck recalls');
END;
/


/**
 * Returns true if the sppecified tape inteface daemon is running.
 *
 * @param daemonName The name of the interface daemon.
 */
CREATE OR REPLACE FUNCTION tapeInterfaceDaemonIsRunning(
  daemonName IN VARCHAR2)
RETURN BOOLEAN IS
  nbRows NUMBER := 0;
BEGIN
  SELECT COUNT(*) INTO nbRows
    FROM CastorConfig
    WHERE class = 'tape'
      AND key   = 'interfaceDaemon'
      AND VALUE = daemonName;

  RETURN nbRows > 0;
END tapeInterfaceDaemonIsRunning;
/


/**
 * Returns true if the rtcpclientd daemon is running.
 */ 
CREATE OR REPLACE FUNCTION rtcpclientdIsRunning
RETURN BOOLEAN IS 
BEGIN
  RETURN tapeInterfaceDaemonIsRunning('rtcpclientd');
END rtcpclientdIsRunning;
/


/**
 * Returns true if the tape gateway daemon is running.
 */
CREATE OR REPLACE FUNCTION tapegatewaydIsRunning
RETURN BOOLEAN IS
BEGIN
  RETURN tapeInterfaceDaemonIsRunning('tapegatewayd');
END tapegatewaydIsRunning;
/


CREATE OR REPLACE PROCEDURE lockCastorFileById(
/**
 * Locks the row in the castor-file table with the specified database ID.
 *
 * This procedure raises application error -20001 when no row exists in the
 * castor-file table with the specified database ID.
 *
 * @param inCastorFileId The database ID of the row to be locked.
 */
  inCastorFileId INTEGER
) AS
  varDummyCastorFileId INTEGER := 0;
BEGIN
  SELECT CastorFile.id
    INTO varDummyCastorFileId
    FROM CastorFile
   WHERE CastorFile.id = inCastorFileId
     FOR UPDATE;
EXCEPTION WHEN NO_DATA_FOUND THEN
  RAISE_APPLICATION_ERROR(-20001,
    'Castor-file does not exist' ||
    ': inCastorFileId=' || inCastorFileId);
END lockCastorFileById;
/
/*******************************************************************
 *
 * @(#)RCSfile: oracleTapeGateway.sql,v  Revision: 1.12  Date: 2009/08/13 15:14:25  Author: gtaur 
 *
 * PL/SQL code for the tape gateway daemon
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* PROCEDURE */

/* tg_findFromTGRequestId */
CREATE OR REPLACE
PROCEDURE tg_findFromTGRequestId (
  inTapeGatewayRequestId IN  INTEGER,
  outTapeId              OUT INTEGER,
  outStreamId            OUT INTEGER) AS
BEGIN
  -- Will return a valid ID in either outTapeId or outStreamId,
  -- and NULL in the other when finding the object corresponding to
  -- this TGR request ID.
  --
  -- Will throw an exception in case of non-unicity.

  -- Look for read tapes:
  BEGIN
    SELECT T.id INTO outTapeId
      FROM TAPE T
     WHERE T.TapeGatewayRequestId = inTapeGatewayRequestId
       AND T.tpMode = tconst.TPMODE_READ;
   EXCEPTION
     WHEN NO_DATA_FOUND THEN
       outTapeId := NULL;
     WHEN TOO_MANY_ROWS THEN
       RAISE_APPLICATION_ERROR (-20119, 
         'Found multiple read tapes for same TapeGatewayRequestId: '|| 
         inTapeGatewayRequestId || ' in tg_findFromTGRequestId');
     -- Let other exceptions fly through.
   END;
   
   -- Look for streams
   BEGIN
     SELECT S.id INTO outStreamId
       FROM Stream S
      WHERE S.TapeGatewayRequestId = inTapeGatewayRequestId;
   EXCEPTION
     WHEN NO_DATA_FOUND THEN
       outStreamId := NULL;
     WHEN TOO_MANY_ROWS THEN
       RAISE_APPLICATION_ERROR (-20119, 
         'Found multiple streams for same TapeGatewayRequestId: '|| 
         inTapeGatewayRequestId || ' in tg_findFromTGRequestId');
     -- Let other exceptions fly through.     
   END;
   
   -- Check for stream/tape collision
   IF (outStreamId IS NOT NULL AND outTapeId IS NOT NULL) THEN
     RAISE_APPLICATION_ERROR (-20119, 
       'Found both read tape (id='||outTapeId||') and Stream (id='||
       outStreamId||') for TapeGatewayRequestId: '||
       inTapeGatewayRequestId || ' in tg_findFromTGRequestId');
   END IF;
END;
/

/* tg_findFromVDQMReqId */
CREATE OR REPLACE
PROCEDURE tg_findFromVDQMReqId (
  inVDQMReqId IN  INTEGER,
  outTapeId              OUT INTEGER,
  outStreamId            OUT INTEGER) AS
BEGIN
  -- Will return a valid ID in either outTapeId or outStreamId,
  -- and NULL in the other when finding the object corresponding to
  -- this TGR request ID.
  --
  -- Will throw an exception in case of non-unicity.

  -- Look for read tapes:
  BEGIN
    SELECT T.id INTO outTapeId
      FROM TAPE T
     WHERE T.VDQMVolReqId = inVDQMReqId
       AND T.tpMode = tconst.TPMODE_READ;
   EXCEPTION
     WHEN NO_DATA_FOUND THEN
       outTapeId := NULL;
     WHEN TOO_MANY_ROWS THEN
       RAISE_APPLICATION_ERROR (-20119, 
         'Found multiple read tapes for same VDQMVolReqId: '|| 
         inVDQMReqId || ' in tg_findFromVDQMReqId');
     -- Let other exceptions fly through.
   END;
   
   -- Look for streams
   BEGIN
     SELECT S.id INTO outStreamId
       FROM Stream S
      WHERE S.VDQMVolReqId = inVDQMReqId;
   EXCEPTION
     WHEN NO_DATA_FOUND THEN
       outStreamId := NULL;
     WHEN TOO_MANY_ROWS THEN
       RAISE_APPLICATION_ERROR (-20119, 
         'Found multiple streams for same VDQMVolReqId: '|| 
         inVDQMReqId || ' in tg_findFromVDQMReqId');
     -- Let other exceptions fly through.     
   END;
   
   -- Check for stream/tape collision
   IF (outStreamId IS NOT NULL AND outTapeId IS NOT NULL) THEN
     RAISE_APPLICATION_ERROR (-20119, 
       'Found both read tape (id='||outTapeId||') and Stream (id='||
       outStreamId||') for VDQMVolReqId: '||
       inVDQMReqId || ' in tg_findFromVDQMReqId');
   END IF;
END;
/

/* tg_RequestIdFromVDQMReqId */
CREATE OR REPLACE
PROCEDURE tg_RequestIdFromVDQMReqId (
  inVDQMReqId IN  INTEGER,
  outTgrId    OUT INTEGER) AS
  varTapeId       INTEGER;
  varStreamId     INTEGER;
BEGIN
  -- Will return a valid tape gateway request Id if one and only one read
  -- tape or stream is found with this VDQM request ID.
  --
  -- Will throw an exception in case of non-unicity.
  --
  -- Will return NULL in case of not found.
  outTgrId := NULL;
  -- Look for read tapes:
  BEGIN
    SELECT T.id, T.TapeGatewayRequestId INTO varTapeId, outTgrId
      FROM TAPE T
     WHERE T.VDQMVolReqId = inVDQMReqId
       AND T.tpMode = tconst.TPMODE_READ;
   EXCEPTION
     WHEN NO_DATA_FOUND THEN
       NULL; -- It's OK, could be a stream.
     WHEN TOO_MANY_ROWS THEN
       RAISE_APPLICATION_ERROR (-20119, 
         'Found multiple read tapes for same VDQMVolReqId: '|| 
         inVDQMReqId || ' in tg_findFromVDQMReqId');
     -- Let other exceptions fly through.
   END;
   
   -- Look for streams
   BEGIN
     SELECT S.id, S.TapeGatewayRequestId INTO varStreamId, outTgrId
       FROM Stream S
      WHERE S.VDQMVolReqId = inVDQMReqId;
   EXCEPTION
     WHEN NO_DATA_FOUND THEN
       NULL; -- It's OK, might have been a tape.
     WHEN TOO_MANY_ROWS THEN
       RAISE_APPLICATION_ERROR (-20119, 
         'Found multiple streams for same VDQMVolReqId: '|| 
         inVDQMReqId || ' in tg_findFromVDQMReqId');
     -- Let other exceptions fly through.     
   END;
   
   -- Check for stream/tape collision
   IF (varStreamId IS NOT NULL AND varTapeId IS NOT NULL) THEN
     outTgrId := NULL;
     RAISE_APPLICATION_ERROR (-20119, 
       'Found both read tape (id='||varTapeId||') and Stream (id='||
       varStreamId||') for VDQMVolReqId: '||
       inVDQMReqId || ' in tg_findFromVDQMReqId');
   END IF;
END;
/

/* tg_findVDQMReqFromTGReqId */
CREATE OR REPLACE
PROCEDURE tg_findVDQMReqFromTGReqId (
  inTGReqId     IN  INTEGER,
  outVDQMReqId  OUT INTEGER) AS
  varTapeId         NUMBER;
  varStreamId       NUMBER;
BEGIN
  -- Helper function. Wrapper to another helper.
  tg_findFromTGRequestId (inTGReqId, varTapeId, varStreamId);
  IF (varTapeId IS NOT NULL) THEN
    SELECT T.vdqmVolReqId INTO outVDQMReqId
      FROM Tape T WHERE T.id = varTapeId;
  ELSIF (varStreamId IS NOT NULL) THEN
    SELECT S.vdqmVolReqId INTO outVDQMReqId
      FROM Stream S WHERE S.id = varStreamId;  
  ELSE
    RAISE_APPLICATION_ERROR (-20119, 
         'Could not find stream or tape read for TG request Id='|| 
         inTGReqId || ' in tg_findVDQMReqFromTGReqId');
  END IF;
END;
/

/* attach drive request to tape */
CREATE OR REPLACE
PROCEDURE tg_attachDriveReqToTape(
  inTapeRequestId IN NUMBER,
  inVdqmId    IN NUMBER,
  inDgn       IN VARCHAR2,
  inLabel     IN VARCHAR2,
  inDensity   IN VARCHAR2) AS
  varTapeId   INTEGER;
  varStreamId INTEGER;
/* Update the status and propoerties of the Tape structure related to
 * a tape request, and the Stream state in case of a write.
 * All other properties are attached to the tape itself.
 */
BEGIN
  -- Update tape of stream, whichever is relevant. First find:
  tg_findFromTGRequestId (inTapeRequestId, varTapeId, varStreamId);
  
  -- Process one or the other (we trust the called function to not provide both)
  IF (varTapeId IS NOT NULL) THEN
    -- In the case of a read, only the tape itself is involved
    -- update reading tape which have been submitted to vdqm => WAIT_DRIVE.
    UPDATE Tape T
       SET T.lastvdqmpingtime = gettime(),
           T.starttime        = gettime(),
           T.vdqmvolreqid     = inVdqmId,
           T.Status           = tconst.TAPE_WAITDRIVE,
           T.dgn              = inDgn,
           T.label            = inLabel,
           T.density          = inDensity
     WHERE T.Id = varTapeId;
    COMMIT;
    RETURN;
  ELSIF (varStreamId IS NOT NULL) THEN
    -- We have to update the tape as well (potentially, we keep the can-fail
    -- query based update of the previous system.
    SAVEPOINT Tape_Mismatch;
    DECLARE
      varTapeFromStream NUMBER;
      varTp             Tape%ROWTYPE;
    BEGIN
      UPDATE STREAM S
         SET S.Status = tconst.STREAM_WAITDRIVE
       WHERE S.Id = varStreamId
      RETURNING S.Tape
        INTO varTapeFromStream;
      BEGIN
        SELECT T.* INTO varTp
          FROM Tape T
         WHERE T.Id = varTapeFromStream
           FOR UPDATE;
      EXCEPTION
        WHEN NO_DATA_FOUND OR TOO_MANY_ROWS THEN
          ROLLBACK TO SAVEPOINT Tape_Mismatch;
          RAISE_APPLICATION_ERROR (-20119,
            'Wrong number of tapes found for stream '||varStreamId);
      END;
      IF (varTp.TpMode != tconst.TPMODE_WRITE) THEN
        ROLLBACK TO SAVEPOINT Tape_Mismatch;
        RAISE_APPLICATION_ERROR (-20119,
          'Wrong type of tape found for stream:'||varStreamId||' tape:'||
          varTp.Id||' TpMode:'||varTp.TpMode);
      END IF;
      varTp.Status          := tconst.TAPE_ATTACHEDTOSTREAM;
      varTp.dgn             := inDgn;
      varTp.label           := inLabel;
      varTp.density         := inDensity;
      varTp.vdqmvolreqid    := NULL; -- The VDQM request ID "belong" to the stream in write mode"
      UPDATE Tape T
         SET ROW = varTp
       WHERE T.Id = varTp.Id;
      UPDATE Stream S
         SET S.vdqmvolreqid = inVdqmId
       WHERE S.id = varStreamId;
      COMMIT;
      RETURN;
    END; -- END of local block for varTapeFromStream and varTp
  ELSE RAISE_APPLICATION_ERROR (-20119,
       'Found no stream or read tape for TapeRequestId: '||inTapeRequestId);
  END IF;
END;
/
        
/* attach the tapes to the streams  */
CREATE OR REPLACE
PROCEDURE tg_attachTapesToStreams (
  inStartFseqs IN castor."cnumList",
  inStrIds     IN castor."cnumList",
  inTapeVids   IN castor."strList") AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  varTapeId NUMBER;
  varUnused NUMBER;
BEGIN
  -- Sanity check
  IF (inStartFseqs.COUNT != inStrIds.COUNT
    OR inStrIds.COUNT != inTapeVids.COUNT) THEN
    RAISE_APPLICATION_ERROR (-20119,
       'Size mismatch for arrays: inStartFseqs.COUNT='||inStartFseqs.COUNT
       ||' inStrIds.COUNT='||inStrIds.COUNT
       ||' inTapeVids.COUNT='||inTapeVids.COUNT);
  END IF;
  FOR i IN inStrIds.FIRST .. inStrIds.LAST LOOP
    varTapeId:=NULL;
    -- Lock the stream (will be updated later).
    SELECT S.Id INTO varUnused
      FROM Stream S
     WHERE S.Id = inStrIds(i)
       FOR UPDATE;
    -- Try and update the tape. In case of failure (not found) we'll create it.
    UPDATE Tape T
       SET T.Stream = inStrIds(i),
           T.Status = tconst.TAPE_WAITDRIVE,
           T.lastFseq = inStartfseqs(i)
     WHERE T.tpmode= tconst.TPMODE_WRITE
       AND T.vid=inTapeVids(i)
    RETURNING T.Id INTO varTapeId;
    -- If there was indeed no tape, just create it.
    IF varTapeId IS NULL THEN
      DECLARE
        varTape Tape%ROWTYPE;
      BEGIN
        -- Try to insert the tape
        SELECT ids_seq.nextval INTO varTape.id FROM DUAL;
        varTape.vid       := inTapeVids(i);
        varTape.side      := 0;
        varTape.tpMode    := tconst.TPMODE_WRITE;
        varTape.errMsgTxt := NULL;
        varTape.errorCode := 0;
        varTape.severity  := 0;
        varTape.vwaddress := NULL;
        varTape.stream    := inStrIds(i);
        varTape.status    := tconst.TAPE_WAITDRIVE;
        varTape.lastFseq  := inStartfseqs(i);
        varTape.lastVdqmPingTime := getTime();
        INSERT INTO Tape T
        VALUES varTape RETURNING T.id into varTapeId;
        INSERT INTO id2type (id,type) values (varTape.Id,29);
      EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
      -- TODO: proper locking could prevent this.
      -- It could happen that the tape go created in the mean time. So now we
      -- can update it
        UPDATE Tape T
           SET T.Stream = inStrIds(i),
               T.Status = tconst.TAPE_WAITDRIVE
         WHERE T.tpmode = tconst.TPMODE_WRITE
           AND T.vid = inTapeVids(i)
        RETURNING T.id INTO varTapeId;
      END;
      -- If this failed, then blow up!
      IF (varTapeId IS NULL) THEN
        ROLLBACK;
        RAISE_APPLICATION_ERROR (-20119,
          'in tg_attachTapesToStreams, failed to recreate or update tape in '||
          'write mode for tape VID ='||inTapeVids(i)||' for tape '||i||
          ' out of '||inTapeVids.COUNT||'. Rolled back the whole operation.');
      END IF;
    END IF;
    -- Finally update the stream we locked earlier
    UPDATE Stream S
       SET S.tape = varTapeId,
           S.status = tconst.STREAM_TO_BE_SENT_TO_VDQM
     WHERE S.id = inStrIds(i);
    -- And save this loop's result
    COMMIT;
  END LOOP;
END;
/

/* update the db when a tape session is ended */
CREATE OR REPLACE
PROCEDURE tg_endTapeSession(inTransId IN NUMBER, inErrorCode IN INTEGER) AS
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -02292);
  varUnused NUMBER;
  varTpId NUMBER;        -- TapeGateway Taperecall
  varTgrId NUMBER;       -- TapeGatewayRequest ID
  varStrId NUMBER;       -- Stream ID
  varSegNum INTEGER;     -- Segment count
  varTcIds "numList";    -- TapeCopy Ids

BEGIN
  -- Prepare to revert changes
  SAVEPOINT MainEndTapeSession;
  -- Find the Tape read or Stream for this VDQM request
  tg_findFromVDQMReqId (inTransId, varTpId, varStrId);
  -- Pre-process the read and write: find corresponding TapeGatewayRequest Id.
  -- Lock corresponding Tape or Stream. This will bomb if we
  -- don't find exactly ones (which is good).
  varTgrId := NULL;
  IF (varTpId IS NOT NULL) THEN
    -- Find and lock tape
    SELECT T.TapeGatewayRequestId INTO varTgrId
      FROM Tape T
     WHERE T.Id = varTpId
       FOR UPDATE;
  ELSIF (varStrId IS NOT NULL) THEN
    -- Find and lock stream
    SELECT S.TapeGatewayRequestId INTO varTgrId
      FROM Stream S
     WHERE S.Id = varStrId
       FOR UPDATE;
  ELSE
    -- Nothing found for the VDQMRequestId: whine and leave.
    ROLLBACK TO SAVEPOINT MainEndTapeSession;
    RAISE_APPLICATION_ERROR (-20119,
     'No tape or stream found for VDQM ID='|| inTransId);
  END IF;
  -- If we failed to get the TG req Id, no point in going further.
  IF (varTgrId IS NULL) THEN
    ROLLBACK TO SAVEPOINT MainEndTapeSession;
    RAISE_APPLICATION_ERROR (-20119,
     'Got NULL TapeGatewayRequestId for tape ID='|| varTpId||
     ' or Stream Id='|| varStrId||' processing VDQM Id='||inTransId||
     ' in tg_endTapeSession.');
  END IF;

  -- Common processing for reads and write: find and lock the tape copies.
  SELECT TC.id BULK COLLECT INTO varTcIds
    FROM TapeCopy TC
   WHERE TC.TapeGatewayRequestId = varTgrId
     FOR UPDATE OF TC.id;

  -- Process the read case
  IF (varTpId IS NOT NULL) THEN
    IF (inErrorCode != 0) THEN
        -- if a failure is reported
        -- fail all the segments
        UPDATE Segment SEG
           SET SEG.status=tconst.SEGMENT_FAILED
         WHERE SEG.copy IN (SELECT * FROM TABLE(varTcIds));
        -- mark tapecopies as  REC_RETRY
        UPDATE TapeCopy TC
           SET TC.status    = tconst.TAPECOPY_REC_RETRY,
               TC.errorcode = inErrorCode
         WHERE TC.id IN (SELECT * FROM TABLE(varTcIds));
    END IF;
    -- resurrect lost segments
    UPDATE Segment SEG
       SET SEG.status = tconst.SEGMENT_UNPROCESSED
     WHERE SEG.status = tconst.SEGMENT_SELECTED
       AND SEG.tape = varTpId;
    -- check if there is work for this tape
    SELECT count(*) INTO varSegNum
      FROM segment SEG
     WHERE SEG.Tape = varTpId
       AND status = tconst.SEGMENT_UNPROCESSED;
    -- Restart the unprocessed segments' tape if there are any.
    IF varSegNum > 0 THEN
      UPDATE Tape T
         SET T.status = tconst.TAPE_WAITPOLICY -- for rechandler
       WHERE T.id=varTpId;
    ELSE
      UPDATE Tape
         SET status = tconst.TAPE_UNUSED
       WHERE id=varTpId;
     END IF;
  ELSIF (varStrId IS NOT NULL) THEN

    -- Process the write case
    deleteOrStopStream(varStrId);
    IF inErrorCode != 0 THEN
      -- if a failure is reported
      -- retry MIG_RETRY
      UPDATE TapeCopy TC
         SET TC.status=tconst.TAPECOPY_MIG_RETRY,
             TC.VID=NULL,
             TC.errorcode=inErrorCode,
             TC.nbretry=0
       WHERE TC.id IN (SELECT * FROM TABLE(varTcIds));
    ELSE
      -- just resurrect them if they were lost
      UPDATE TapeCopy TC
         SET TC.status = tconst.TAPECOPY_TOBEMIGRATED,
             TC.VID = NULL
       WHERE TC.id IN (SELECT * FROM TABLE(varTcIds))
         AND TC.status = tconst.TAPECOPY_SELECTED;
    END IF;
  ELSE

    -- Small infusion of paranoia ;-) We should never reach that point...
    ROLLBACK TO SAVEPOINT MainEndTapeSession;
    RAISE_APPLICATION_ERROR (-20119,
     'No tape or stream found on second pass for VDQM ID='|| inTransId ||
     ' in tg_endTapeSession');
  END IF;
  COMMIT;
END;
/

/* mark a migration or recall as failed saving in the db the error code associated with the failure */
CREATE OR REPLACE
PROCEDURE tg_failFileTransfer(
  inTransId      IN NUMBER,    -- The VDQM transaction ID
  inFileId    IN NUMBER,       -- File ID
  inNsHost    IN VARCHAR2,     -- NS Host
  inFseq      IN INTEGER,      -- Tapecopy's fSeq
  inErrorCode IN INTEGER)  AS  -- Error Code
  varUnused NUMBER;            -- dummy
  varTgrId NUMBER;             -- Tape Gateway Request Id
  varStrId NUMBER;             -- Stream Id
  varTpId NUMBER;              -- Tape Id
  varTcId NUMBER;              -- TapeCopy Id
BEGIN
  -- Prepare to return everything to its original state in case of problem.
  SAVEPOINT MainFailFileSession;
  
  -- Find related Read tape or stream from VDQM Id
  tg_findFromVDQMReqId(inTransId, varTpId, varStrId);
  
  -- Lock related castorfile -- TODO: This should be a procedure-based access to
  -- the disk system.
  SELECT CF.id INTO varUnused 
    FROM CastorFile CF
   WHERE CF.fileid = inFileId
     AND CF.nsHost = inNsHost 
    FOR UPDATE;
  
  -- Case dependent part
  IF (varTpId IS NOT NULL) THEN
    -- We handle a read case
    -- fail the segment on that tape
    UPDATE Segment SEG
       SET SEG.status    = tconst.SEGMENT_FAILED,
           SEG.severity  = inErrorCode,
           SEG.errorCode = -1 
     WHERE SEG.fseq = inFseq 
       AND SEG.tape = varTpId 
    RETURNING SEG.copy INTO varTcId;
    -- mark tapecopy as REC_RETRY
    UPDATE TapeCopy TC
       SET TC.status    = tconst.TAPECOPY_REC_RETRY,
           TC.errorcode = inErrorCode 
     WHERE TC.id = varTcId;  
  ELSIF (varStrId IS NOT NULL) THEN
    -- Write case
    SELECT T.id, S.TapeGatewayRequestId INTO varTpId, varTgrId
      FROM Tape T, Stream  S
     WHERE T.id = S.tape
       AND S.id = varStrId;
    -- mark tapecopy as MIG_RETRY. It should be the tapecopy with the proper 
    -- TapegatewayRequest + having a matching Fseq.
    UPDATE TapeCopy TC
       SET TC.status    = tconst.TAPECOPY_MIG_RETRY,
           TC.errorcode = inErrorCode,
           TC.vid       = NULL
     WHERE TC.TapegatewayRequestId = varTgrId
       AND TC.fSeq = inFseq; 
  ELSE
  
    -- Complain in case of failure
    ROLLBACK TO SAVEPOINT MainFailFileSession;
    RAISE_APPLICATION_ERROR (-20119, 
     'No tape or stream found on second pass for VDQM ID='|| inTransId||
     ' in tg_failFileTransfer');
  END IF;
EXCEPTION WHEN  NO_DATA_FOUND THEN
  NULL;
END;
/

/* retrieve from the db all the tapecopies that faced a failure for migration */
CREATE OR REPLACE
PROCEDURE  tg_getFailedMigrations(outTapeCopies_c OUT castor.TapeCopy_Cur) AS
BEGIN
  -- get TAPECOPY_MIG_RETRY
  OPEN outTapeCopies_c FOR
    SELECT *
      FROM TapeCopy TC
     WHERE TC.status = tconst.TAPECOPY_MIG_RETRY
       AND ROWNUM < 1000 
       FOR UPDATE SKIP LOCKED; 
END;
/


/* retrieve from the db all the tapecopies that faced a failure for recall */
CREATE OR REPLACE
PROCEDURE  tg_getFailedRecalls(outTapeCopies_c OUT castor.TapeCopy_Cur) AS
BEGIN
  -- get TAPECOPY_REC_RETRY
  OPEN outTapeCopies_c FOR
    SELECT *
      FROM TapeCopy TC
     WHERE TC.status = tconst.TAPECOPY_REC_RETRY
      AND ROWNUM < 1000 
      FOR UPDATE SKIP LOCKED;
END;
/

/* default migration candidate selection policy */
CREATE OR REPLACE
PROCEDURE tg_defaultMigrSelPolicy(inStreamId IN INTEGER,
                                  outDiskServerName OUT NOCOPY VARCHAR2,
                                  outMountPoint OUT NOCOPY VARCHAR2,
                                  outPath OUT NOCOPY VARCHAR2,
                                  outDiskCopyId OUT INTEGER,
                                  outCastorFileId OUT INTEGER,
                                  outFileId OUT INTEGER,
                                  outNsHost OUT NOCOPY VARCHAR2, 
                                  outFileSize OUT INTEGER,
                                  outTapeCopyId OUT INTEGER, 
                                  outLastUpdateTime OUT INTEGER) AS
  /* Find the next tape copy to migrate from a given stream ID.
   * 
   * Procedure's input: Stream Id for a stream that is locked by caller.
   *
   * Procedure's output: Returns a non-zero TapeCopy ID on full success
   * Can return a non-zero DiskServer Id when a DiskServer got selected without 
   * selecting any tape copy.
   * Data modification: The function updates the stream's filesystem information
   * in case a new one got seleted.
   *
   * Lock taken on the diskserver in some cases.
   * Lock taken on the tapecopy if it selects one.
   * Lock taken on  the Stream when a new disk server is selected.
   * 
   * Commits: The function does not commit data.
   *
   * Per policy we should only propose a tape copy for a file that does not 
   * already have a tapecopy attached for or mirgated to the same
   * tape.
   * The tape's VID can be found from the streamId by:
   * Stream->Tape->VID.
   * The tapecopies carry VID themselves, when in stated STAGED, SELECTED and 
   * in error states. In other states the VID must be null, per constraint.
   * The already migrated tape copies are kept until the whole set of siblings 
   * have been migrated. Nothing else is guaranteed to be.
   * 
   * From this we can find a list of our potential siblings (by castorfile) from
   * this TapeGatewayRequest, and prevent the selection of tapecopies whose 
   * siblings already live on the same tape.
   */
  varFileSystemId INTEGER := 0;
  varDiskServerId NUMBER;
  varLastFSChange NUMBER;
  varLastFSUsed NUMBER;
  varLastButOneFSUsed NUMBER;
  varFindNewFS NUMBER := 1;
  varNbMigrators NUMBER := 0;
  varUnused NUMBER;
  LockError EXCEPTION;
  varVID VARCHAR2(2048 BYTE);
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  outTapeCopyId := 0;
  -- Find out which tape we're talking about
  SELECT T.VID INTO varVID 
    FROM Tape T, Stream S 
   WHERE S.Id = inStreamId 
     AND T.Id = S.Tape;
  -- First try to see whether we should resuse the same filesystem as last time
  SELECT S.lastFileSystemChange, S.lastFileSystemUsed, 
         S.lastButOneFileSystemUsed
    INTO varLastFSChange, varLastFSUsed, varLastButOneFSUsed
    FROM Stream S 
   WHERE S.id = inStreamId;
  -- If the filesystem has changed in the last 5 minutes, consider its reuse
  IF getTime() < varLastFSChange + 900 THEN
    -- Count the number of streams referencing the filesystem
    SELECT (SELECT count(*) FROM stream S
             WHERE S.lastFileSystemUsed = varLastButOneFSUsed) +
           (SELECT count(*) FROM stream S 
             WHERE S.lastButOneFileSystemUsed = varLastButOneFSUsed)
      INTO varNbMigrators FROM DUAL;
    -- only go if we are the only migrator on the file system.
    IF varNbMigrators = 1 THEN
      BEGIN
        -- check states of the diskserver and filesystem and get mountpoint 
        -- and diskserver name
        SELECT DS.name, FS.mountPoint, FS.id 
          INTO outDiskServerName, outMountPoint, varFileSystemId
          FROM FileSystem FS, DiskServer DS
         WHERE FS.diskServer = DS.id
           AND FS.id = varLastButOneFSUsed
           AND FS.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
           AND DS.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING);
        -- we are within the time range, so we try to reuse the filesystem
        SELECT /*+ FIRST_ROWS(1)  LEADING(D T ST) */
               D.path, D.id, D.castorfile, T.id
          INTO outPath, outDiskCopyId, outCastorFileId, outTapeCopyId
          FROM DiskCopy D, TapeCopy T, Stream2TapeCopy STTC
         WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
         -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
           AND D.filesystem = varLastButOneFSUsed
           AND STTC.parent = inStreamId
           AND T.status = tconst.TAPECOPY_WAITINSTREAMS
           AND STTC.child = T.id
           AND T.castorfile = D.castorfile
           -- Do not select a tapecopy for which a sibling TC is or will be on 
           -- on this tape.
           AND varVID NOT IN (
                 SELECT DISTINCT T2.VID FROM TapeCopy T2
                  WHERE T2.CastorFile=T.Castorfile
                    AND T2.Status IN (tconst.TAPECOPY_SELECTED, tconst.TAPECOPY_STAGED))
           AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
        -- Get addition info
        SELECT CF.FileId, CF.NsHost, CF.FileSize, CF.lastUpdateTime
          INTO outFileId, outNsHost, outFileSize, outLastUpdateTime
          FROM CastorFile CF
         WHERE CF.Id = outCastorFileId;
        -- we found one, no need to go for new filesystem
        varFindNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
        -- found no tapecopy or diskserver, filesystem are down. We'll go 
        -- through the normal selection
        NULL;
      END;
    END IF;
  END IF;
  IF varFindNewFS = 1 THEN
    FOR f IN (
    SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, 
           DiskServer.name, FileSystem.mountPoint
      FROM Stream, SvcClass2TapePool, DiskPool2SvcClass, FileSystem, DiskServer
     WHERE Stream.id = inStreamId
       AND Stream.TapePool = SvcClass2TapePool.child
       AND SvcClass2TapePool.parent = DiskPool2SvcClass.child
       AND DiskPool2SvcClass.parent = FileSystem.diskPool
       AND FileSystem.diskServer = DiskServer.id
       AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
       AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
     ORDER BY -- first prefer diskservers where no migrator runs and filesystems
              -- with no recalls
              DiskServer.nbMigratorStreams ASC, 
              FileSystem.nbRecallerStreams ASC,
              -- then order by rate as defined by the function
              fileSystemRate(FileSystem.readRate,
                             FileSystem.writeRate,
                             FileSystem.nbReadStreams,
                             FileSystem.nbWriteStreams,
                             FileSystem.nbReadWriteStreams,
                             FileSystem.nbMigratorStreams,
                             FileSystem.nbRecallerStreams) DESC,
              -- finally use randomness to avoid preferring always the same FS
              DBMS_Random.value) LOOP
    BEGIN
      -- Get ready to release lock if the diskserver or tapecopy is not 
      -- to our liking
      SAVEPOINT DServ_TCopy_Lock;
      -- lock the complete diskServer as we will update all filesystems
      SELECT D.id INTO varUnused FROM DiskServer D WHERE D.id = f.DiskServerId 
         FOR UPDATE NOWAIT;
      SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
             F.diskServerId, f.name, f.mountPoint, 
             f.fileSystemId, D.path, D.id,
             D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, 
             C.lastUpdateTime
          INTO varDiskServerId, outDiskServerName, outMountPoint, 
             varFileSystemId, outPath, outDiskCopyId,
             outCastorFileId, outFileId, outNsHost, outFileSize, outTapeCopyId, 
             outLastUpdateTime
          FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
         WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
         -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
           AND D.filesystem = f.fileSystemId
           AND StT.parent = inStreamId
           AND T.status = tconst.TAPECOPY_WAITINSTREAMS
           AND StT.child = T.id
           AND T.castorfile = D.castorfile
           AND C.id = D.castorfile
           AND varVID NOT IN (
                 SELECT DISTINCT T2.VID FROM TapeCopy T2
                  WHERE T2.CastorFile=T.Castorfile
                    AND T2.Status IN (tconst.TAPECOPY_SELECTED, tconst.TAPECOPY_STAGED))
           AND ROWNUM < 2 FOR UPDATE OF t.id NOWAIT;
        -- found something on this filesystem, no need to go on
        varDiskServerId := f.DiskServerId;
        varFileSystemId := f.fileSystemId;
        EXIT;
      EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's rollback in case there was NO_DATA_FOUND to release the lock
         ROLLBACK TO SAVEPOINT DServ_TCopy_Lock;
       END;
    END LOOP;
  END IF;

  IF outTapeCopyId = 0 THEN
    -- Nothing found, return; locks will be released by the caller
    RETURN;
  END IF;
  
  IF varFindNewFS = 1 THEN
    UPDATE Stream S
       SET S.lastFileSystemUsed = varFileSystemId,
           -- We store the old value (implicitely available
           -- when reading (reading = :old) to the new row value
           -- (write = :new). So it works.
           S.lastButOneFileSystemUsed = S.lastFileSystemUsed,
           S.lastFileSystemChange = getTime()
     WHERE S.id = inStreamId;
  END IF;

  -- Update Filesystem state
  updateFSMigratorOpened(varDiskServerId, varFileSystemId, 0);
END;
/

/* drain disk migration candidate selection policy */

CREATE OR REPLACE 
PROCEDURE tg_drainDiskMigrSelPolicy(inStreamId        IN INTEGER,
                                    outDiskServerName OUT NOCOPY VARCHAR2,
                                    outMountPoint     OUT NOCOPY VARCHAR2,
                                    outPath           OUT NOCOPY VARCHAR2,
                                    outDCI            OUT INTEGER,
                                    outCastorFileId   OUT INTEGER,
                                    outNsFileId       OUT INTEGER,
                                    outNsHost         OUT NOCOPY VARCHAR2,
                                    outFileSize       OUT INTEGER,
                                    outTapeCopyId     OUT INTEGER,
                                    outLastUpdateTime OUT INTEGER) AS
  varFileSystemId INTEGER := 0;
  varDiskServerId NUMBER;
  varLastFSChange NUMBER;
  varLastFSUsed NUMBER;
  varLastButOneFSUsed NUMBER;
  varPenultimateDiskServer NUMBER;
  varFindNewFS NUMBER := 1;
  varNbMigrators NUMBER := 0;
  varUnused NUMBER;
  LockError EXCEPTION;
  varVID VARCHAR2(2048 BYTE);
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  outTapeCopyId := 0;
  -- Find out which tape we're talking about
  SELECT T.VID INTO varVID 
    FROM Tape T, Stream S 
   WHERE S.Id = inStreamId 
     AND T.Id = S.Tape;
  -- First try to see whether we should resuse the same filesystem as last time
  SELECT lastFileSystemChange, lastFileSystemUsed, lastButOneFileSystemUsed
    INTO varLastFSChange, varLastFSUsed, varLastButOneFSUsed
    FROM Stream WHERE id = inStreamId;
  IF getTime() < varLastFSChange + 1800 THEN
    SELECT (SELECT count(*) FROM stream WHERE lastFileSystemUsed = varLastFSUsed)
      INTO varNbMigrators FROM DUAL;
    -- only go if we are the only migrator on the box
    IF varNbMigrators = 1 THEN
      BEGIN
        -- check states of the diskserver and filesystem and get mountpoint and diskserver name
        SELECT diskserver.id, name, mountPoint, FileSystem.id INTO varDiskServerId, outDiskServerName, outMountPoint, varFileSystemId
          FROM FileSystem, DiskServer
         WHERE FileSystem.diskServer = DiskServer.id
           AND FileSystem.id = varLastFSUsed
           AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
           AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING);
        -- we are within the time range, so we try to reuse the filesystem
        SELECT /*+ ORDERED USE_NL(D T) INDEX(T I_TapeCopy_CF_Status_2) INDEX(ST I_Stream2TapeCopy_PC) */
              D.path, D.diskcopy_id, D.castorfile, T.id INTO outPath, outDCI, outCastorFileId, outTapeCopyId
          FROM (SELECT /*+ INDEX(DK I_DiskCopy_FS_Status_10) */
                             DK.path path, DK.id diskcopy_id, DK.castorfile
                  FROM DiskCopy DK
                  WHERE decode(DK.status, 10, DK.status, NULL) = dconst.DISKCOPY_CANBEMIGR
                  -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
                  AND DK.filesystem = varLastFSUsed)  D, TapeCopy T, Stream2TapeCopy ST
          WHERE T.castorfile = D.castorfile
          AND ST.child = T.id
          AND ST.parent = inStreamId
          AND decode(T.status, 2, T.status, NULL) = tconst.TAPECOPY_WAITINSTREAMS
          -- 2 = tconst.TAPECOPY_WAITINSTREAMS. Has to be kept as a hardcoded number in order to use a function-based index.
          -- Do not select a tapecopy for which a sibling TC is or will be on 
          -- on this tape.
          AND varVID NOT IN (
                SELECT DISTINCT T2.VID FROM TapeCopy T2
                 WHERE T2.CastorFile=T.Castorfile
                   AND T2.Status IN (tconst.TAPECOPY_SELECTED, tconst.TAPECOPY_STAGED))
          AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;   
        -- Get addition info
        SELECT  C.fileId, C.nsHost, C.fileSize,  C.lastUpdateTime
          INTO  outNsFileId, outNsHost, outFileSize, outLastUpdateTime
          FROM  castorfile C
          WHERE outCastorFileId = C.id;
        -- we found one, no need to go for new filesystem
        varFindNewFS := 0;
      EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
        -- found no tapecopy or diskserver, filesystem are down. We'll go through the normal selection
        NULL;
      END;
    END IF;
  END IF;
  IF varFindNewFS = 1 THEN
    -- We try first to reuse the diskserver of the varLastFSUsed, even if we change filesystem
    BEGIN
      SELECT FS.DiskServer INTO varPenultimateDiskServer
        FROM FileSystem FS WHERE FS.id = varLastButOneFSUsed;
    EXCEPTION WHEN NO_DATA_FOUND THEN
       varPenultimateDiskServer := NULL;
    END;
    FOR f IN (
      SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
        FROM FileSystem, DiskServer
       WHERE FileSystem.diskServer = DiskServer.id
         AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
         AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
         AND DiskServer.id = varPenultimateDiskServer) LOOP
       BEGIN
         -- lock the complete diskServer as we will update all filesystems
         SELECT id INTO varUnused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
         SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
                f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, 
                D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
           INTO varDiskServerId, outDiskServerName, outMountPoint, varFileSystemId, outPath, outDCI, 
                outCastorFileId, outNsFileId, outNsHost, outFileSize, outTapeCopyId, outLastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
          -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
            AND D.filesystem = f.fileSystemId
            AND StT.parent = inStreamId
            AND T.status = tconst.TAPECOPY_WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            -- Do not select a tapecopy for which a sibling TC is or will be on 
            -- on this tape.
            AND varVID NOT IN (
                  SELECT DISTINCT T2.VID FROM TapeCopy T2
                   WHERE T2.CastorFile=T.Castorfile
                     AND T2.Status IN (tconst.TAPECOPY_SELECTED, tconst.TAPECOPY_STAGED))
            AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
         -- found something on this filesystem, no need to go on
         varDiskServerId := f.DiskServerId;
         varFileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;
  IF outTapeCopyId = 0 THEN
    -- Then we go for all potential filesystems. Note the duplication of code, due to the fact that ORACLE cannot order unions
    FOR f IN (
      SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
        FROM Stream, SvcClass2TapePool, DiskPool2SvcClass, FileSystem, DiskServer
       WHERE Stream.id = inStreamId
         AND Stream.TapePool = SvcClass2TapePool.child
         AND SvcClass2TapePool.parent = DiskPool2SvcClass.child
         AND DiskPool2SvcClass.parent = FileSystem.diskPool
         AND FileSystem.diskServer = DiskServer.id
         AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
         AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
       ORDER BY -- first prefer diskservers where no migrator runs and filesystems with no recalls
                DiskServer.nbMigratorStreams ASC, FileSystem.nbRecallerStreams ASC,
                -- then order by rate as defined by the function
                fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams, FileSystem.nbWriteStreams,
                               FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
                -- finally use randomness to avoid preferring always the same FS
                DBMS_Random.value) LOOP
       BEGIN
         -- lock the complete diskServer as we will update all filesystems
         SELECT id INTO varUnused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
         SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
                f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id,
                D.castorfile, C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
           INTO varDiskServerId, outDiskServerName, outMountPoint, varFileSystemId, outPath, outDCI, 
                outCastorFileId, outNsFileId, outNsHost, outFileSize, outTapeCopyId, outLastUpdateTime
           FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
          WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
          -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
            AND D.filesystem = f.fileSystemId
            AND StT.parent = inStreamId
            AND T.status = tconst.TAPECOPY_WAITINSTREAMS
            AND StT.child = T.id
            AND T.castorfile = D.castorfile
            AND C.id = D.castorfile
            AND varVID NOT IN (
                  SELECT DISTINCT T2.VID FROM TapeCopy T2
                   WHERE T2.CastorFile=T.Castorfile
                     AND T2.Status IN (tconst.TAPECOPY_SELECTED, tconst.TAPECOPY_STAGED))
            AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
         -- found something on this filesystem, no need to go on
         varDiskServerId := f.DiskServerId;
         varFileSystemId := f.fileSystemId;
         EXIT;
       EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
         -- either the filesystem is already locked or we found nothing,
         -- let's go to the next one
         NULL;
       END;
    END LOOP;
  END IF;

  IF outTapeCopyId = 0 THEN
    -- Nothing found, return; locks will be released by the caller
    RETURN;
  END IF;
  
  IF varFindNewFS = 1 THEN
    UPDATE Stream
       SET lastFileSystemUsed = varFileSystemId,
           lastButOneFileSystemUsed = lastFileSystemUsed,
           lastFileSystemChange = getTime()
     WHERE id = inStreamId;
  END IF;

  -- Update Filesystem state
  updateFSMigratorOpened(varDiskServerId, varFileSystemId, 0);
END;
/


/* repack migration candidate selection policy */

CREATE OR REPLACE 
PROCEDURE tg_repackMigrSelPolicy(inStreamId        IN  INTEGER,
                                 outDiskServerName OUT NOCOPY VARCHAR2,
                                 outMountPoint     OUT NOCOPY VARCHAR2,
                                 outPath           OUT NOCOPY VARCHAR2,
                                 outDCI            OUT INTEGER,
                                 outCastorFileId   OUT INTEGER,
                                 outNsFileId       OUT INTEGER,
                                 outNsHost         OUT NOCOPY VARCHAR2,
                                 outFileSize       OUT INTEGER,
                                 outTapeCopyId     OUT INTEGER,
                                 outLastUpdateTime OUT INTEGER) AS
  varFileSystemId INTEGER := 0;
  varDiskServerId NUMBER;
  varUnused NUMBER;
  LockError EXCEPTION;
  varVID VARCHAR2(2048 BYTE);
  PRAGMA EXCEPTION_INIT (LockError, -54);
BEGIN
  outTapeCopyId := 0;
  -- Find out which tape we're talking about
  SELECT T.VID INTO varVID 
    FROM Tape T, Stream S 
   WHERE S.Id = inStreamId 
     AND T.Id = S.Tape;
  FOR f IN (
    SELECT FileSystem.id AS FileSystemId, DiskServer.id AS DiskServerId, DiskServer.name, FileSystem.mountPoint
       FROM Stream, SvcClass2TapePool, DiskPool2SvcClass, FileSystem, DiskServer
      WHERE Stream.id = inStreamId
        AND Stream.TapePool = SvcClass2TapePool.child
        AND SvcClass2TapePool.parent = DiskPool2SvcClass.child
        AND DiskPool2SvcClass.parent = FileSystem.diskPool
        AND FileSystem.diskServer = DiskServer.id
        AND FileSystem.status IN (dconst.FILESYSTEM_PRODUCTION, dconst.FILESYSTEM_DRAINING)
        AND DiskServer.status IN (dconst.DISKSERVER_PRODUCTION, dconst.DISKSERVER_DRAINING)
      ORDER BY -- first prefer diskservers where no migrator runs and filesystems with no recalls
               DiskServer.nbMigratorStreams ASC, FileSystem.nbRecallerStreams ASC,
               -- then order by rate as defined by the function
               fileSystemRate(FileSystem.readRate, FileSystem.writeRate, FileSystem.nbReadStreams, FileSystem.nbWriteStreams,
                              FileSystem.nbReadWriteStreams, FileSystem.nbMigratorStreams, FileSystem.nbRecallerStreams) DESC,
               -- finally use randomness to avoid preferring always the same FS
               DBMS_Random.value) LOOP
    BEGIN
      -- lock the complete diskServer as we will update all filesystems
      SELECT id INTO varUnused FROM DiskServer WHERE id = f.DiskServerId FOR UPDATE NOWAIT;
      SELECT /*+ FIRST_ROWS(1) LEADING(D T StT C) */
             f.diskServerId, f.name, f.mountPoint, f.fileSystemId, D.path, D.id, D.castorfile,
             C.fileId, C.nsHost, C.fileSize, T.id, C.lastUpdateTime
        INTO varDiskServerId, outDiskServerName, outMountPoint, varFileSystemId, outPath, outDCI, outCastorFileId,
             outNsFileId, outNsHost, outFileSize, outTapeCopyId, outLastUpdateTime
        FROM DiskCopy D, TapeCopy T, Stream2TapeCopy StT, Castorfile C
       WHERE decode(D.status, 10, D.status, NULL) = dconst.DISKCOPY_CANBEMIGR
       -- 10 = dconst.DISKCOPY_CANBEMIGR. Has to be kept as a hardcoded number in order to use a function-based index.
         AND D.filesystem = f.fileSystemId
         AND StT.parent = inStreamId
         AND T.status = tconst.TAPECOPY_WAITINSTREAMS
         AND StT.child = T.id
         AND T.castorfile = D.castorfile
         AND C.id = D.castorfile
         AND varVID NOT IN (
               SELECT DISTINCT T2.VID FROM TapeCopy T2
                WHERE T2.CastorFile=T.Castorfile
                  AND T2.Status IN (tconst.TAPECOPY_SELECTED, tconst.TAPECOPY_STAGED))
         AND ROWNUM < 2 FOR UPDATE OF T.id NOWAIT;
      -- found something on this filesystem, no need to go on
      varDiskServerId := f.DiskServerId;
      varFileSystemId := f.fileSystemId;
      EXIT;
    EXCEPTION WHEN NO_DATA_FOUND OR LockError THEN
      -- either the filesystem is already locked or we found nothing,
      -- let's go to the next one
      NULL;
    END;
  END LOOP;

  IF outTapeCopyId = 0 THEN
    -- Nothing found, return; locks will be released by the caller
    RETURN;
  END IF;
  
  UPDATE Stream
     SET lastFileSystemUsed = varFileSystemId,
         lastButOneFileSystemUsed = lastFileSystemUsed,
         lastFileSystemChange = getTime()
   WHERE id = inStreamId;

  -- Update Filesystem state
  updateFSMigratorOpened(varDiskServerId, varFileSystemId, 0);
END;
/

/* get a candidate for migration */
CREATE OR REPLACE
PROCEDURE tg_getFileToMigrate(
  inVDQMtransacId IN  NUMBER,
  outRet           OUT INTEGER,
  outVid        OUT NOCOPY VARCHAR2,
  outputFile    OUT castorTape.FileToMigrateCore_cur) AS
  /*
   * This procedure finds the next file to migrate according to a policy, chosen
   * from the context.
   */
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -1);
  varStrId NUMBER;
  varPolicy VARCHAR2(100);
  varDiskServer VARCHAR2(2048);
  varMountPoint VARCHAR2(2048);
  varPath  VARCHAR2(2048);
  varDiskCopyId NUMBER:=0;
  varCastorFileId NUMBER;
  varFileId NUMBER;
  varNsHost VARCHAR2(2048);
  varFileSize  INTEGER;
  varTapeCopyId  INTEGER:=0;
  varLastUpdateTime NUMBER;
  varLastKnownName VARCHAR2(2048);
  varTgRequestId NUMBER;
  varUnused INTEGER;
BEGIN
  outRet:=0;
  -- Get ready to rollback
  SAVEPOINT MainGetFileMigr;
  BEGIN
    -- Find stream
    tg_FindFromVDQMReqId (inVDQMtransacId, varUnused, varStrId);
    IF (varStrId IS NULL) THEN
      ROLLBACK TO SAVEPOINT MainFailFileSession;
      RAISE_APPLICATION_ERROR (-20119,
        'No stream found on second pass for VDQM ID='|| inVDQMtransacId||
        ' in tg_getFileToMigrate');
    END IF;
    -- Extracte tape gateway request Id.
    SELECT S.TapeGatewayRequestId INTO varTgRequestId
      FROM Stream S
     WHERE S.Id = varStrId;
    -- Get Tape's VID.
    SELECT T.VID INTO outVid
      FROM Tape T
     WHERE T.Id IN
         (SELECT S.Tape
            FROM Stream S
           WHERE S.Id = varStrId);
  EXCEPTION WHEN NO_DATA_FOUND THEN
    ROLLBACK TO SAVEPOINT MainFailFileSession;
    outRet:=-2;   -- stream is over
    RETURN;
  END;
  -- Check for existence of tape copies for this stream
  BEGIN
    SELECT 1 INTO varUnused FROM dual
      WHERE EXISTS (SELECT 'x' FROM Stream2TapeCopy STTC
                      WHERE STTC.parent=varStrId);
  EXCEPTION WHEN NO_DATA_FOUND THEN
    outRet:=-1;   -- no more files
    RETURN;
  END;
  -- lock to avoid deadlock with mighunter
  SELECT S.Id INTO varUnused FROM Stream S WHERE S.Id=varStrId
     FOR UPDATE OF S.Id;
  -- get the policy name and execute the policy
  /* BEGIN */
  SELECT TP.migrSelectPolicy INTO varPolicy
    FROM Stream S, TapePool TP
   WHERE S.Id = varStrId
     AND S.tapePool = TP.Id;
  -- check for NULL value
  IF varPolicy IS NULL THEN
    varPolicy := 'defaultMigrSelPolicy';
  END IF;
  /* Commenting out this catch as stream with no tape pool is an error condition
  TODO: Check and drop entirely.
  EXCEPTION WHEN NO_DATA_FOUND THEN
    varPolicy := 'defaultMigrSelPolicy';
  END;*/

  IF  varPolicy = 'repackMigrSelPolicy' THEN
    -- repack policy
    tg_repackMigrSelPolicy(varStrId,varDiskServer,varMountPoint,varPath,
      varDiskCopyId ,varCastorFileId,varFileId,varNsHost,varFileSize,
      varTapeCopyId,varLastUpdateTime);
  ELSIF  varPolicy = 'drainDiskMigrSelPolicy' THEN
    -- drain disk policy
    tg_drainDiskMigrSelPolicy(varStrId,varDiskServer,varMountPoint,varPath,
      varDiskCopyId ,varCastorFileId,varFileId,varNsHost,varFileSize,
      varTapeCopyId,varLastUpdateTime);
  ELSE
    -- default
    tg_defaultMigrSelPolicy(varStrId,varDiskServer,varMountPoint,varPath,
      varDiskCopyId ,varCastorFileId,varFileId,varNsHost,varFileSize,
      varTapeCopyId,varLastUpdateTime);
  END IF;

  IF varTapeCopyId = 0 OR varDiskCopyId=0 THEN
    outRet := -1; -- the migration selection policy didn't find any candidate
    COMMIT; -- TODO: Check if ROLLBACK is not better...
    RETURN;
  END IF;

  -- Here we found a tapeCopy and we process it
  -- update status of selected tapecopy and stream
  -- Sanity check: There should be no tapecopies for the same castor file where
  -- the volume ID is the same.
  DECLARE
    varConflicts NUMBER;
  BEGIN
    SELECT COUNT(*) INTO varConflicts
      FROM TapeCopy TC
     WHERE TC.CastorFile = varCastorFileId
       AND TC.VID = outVID
       AND TC.Id != varTapeCopyId
       AND TC.Status NOT IN (tconst.TAPECOPY_MIG_RETRY,
                             tconst.TAPECOPY_REC_RETRY,
                             tconst.TAPECOPY_FAILED);
    IF (varConflicts != 0) THEN
      RAISE_APPLICATION_ERROR (-20119, 'About to move a second copy to the same tape!');
    END IF;
  END;
  UPDATE TapeCopy TC
     SET TC.Status = tconst.TAPECOPY_SELECTED,
         TC.VID = outVID
   WHERE TC.Id = varTapeCopyId;
  -- detach the tapecopy from the stream now that it is SELECTED;
  DELETE FROM Stream2TapeCopy STTC
   WHERE STTC.child = varTapeCopyId;

  SELECT CF.lastKnownFileName INTO varLastKnownName
    FROM CastorFile CF
   WHERE CF.Id = varCastorFileId; -- we rely on the check done before TODO: which check?

  DECLARE
    varNewFseq NUMBER;
  BEGIN
   -- Atomically increment and read the next FSEQ to be written to. fSeq is held
   -- in the tape structure.
   UPDATE Tape T
      SET T.lastfseq=T.lastfseq+1
     WHERE T.Id IN
         (SELECT S.Tape
            FROM Stream S
           WHERE S.Id = varStrId)
     RETURNING T.lastfseq-1 into varNewFseq; -- The previous value is where we'll write

   -- Update the tapecopy and attach it to a newly created file transaction ID
   UPDATE TapeCopy TC
      SET TC.fSeq = varNewFseq,
          TC.tapeGatewayRequestId = varTgRequestId,
          TC.fileTransactionId = TG_FileTrId_Seq.NEXTVAL
    WHERE TC.Id = varTapeCopyId;

   OPEN outputFile FOR
     SELECT varFileId,varNshost,varLastUpdateTime,varDiskServer,varMountPoint,
            varPath,varLastKnownName,TC.fseq,varFileSize,TC.fileTransactionId
       FROM TapeCopy TC
      WHERE TC.Id = varTapeCopyId;

  END;
  COMMIT;
END;
/

/* get a candidate for recall */
CREATE OR REPLACE
PROCEDURE tg_getFileToRecall (inTransId IN  NUMBER, outRet OUT INTEGER,
  outVid OUT NOCOPY VARCHAR2, outFile OUT castorTape.FileToRecallCore_Cur) AS
  varTgrId         INTEGER; -- TapeGateway Request Id
  varDSName VARCHAR2(2048); -- Disk Server name
  varMPoint VARCHAR2(2048); -- Mount Point
  varPath   VARCHAR2(2048); -- Diskcopy path
  varSegId          NUMBER; -- Segment Id
  varDcId           NUMBER; -- Disk Copy Id
  varTcId           NUMBER; -- Tape Copy Id
  varTapeId         NUMBER; -- Tape Id
  varNewFSeq       INTEGER; -- new FSeq
  varUnused         NUMBER;
BEGIN 
  outRet:=0;
  BEGIN
    -- master lock on the tape read
    -- Find tape
    tg_FindFromVDQMReqId (inTransId, varTapeId, varUnused);
    IF (varTapeId IS NULL) THEN 
      outRet := -2;
      RETURN;
    END IF;
    -- Take lock on tape
    SELECT T.TapeGatewayRequestId, T.vid INTO varTgrId, outVid
      FROM Tape T
     WHERE T.id = varTapeId
       FOR UPDATE;
  EXCEPTION WHEN  NO_DATA_FOUND THEN
     outRet:=-2; -- ERROR
     RETURN;
  END; 
  BEGIN
    -- Find the unprocessed segment of this tape with lowest fSeq
    SELECT   SEG.id,   SEG.fSeq, SEG.Copy 
      INTO varSegId, varNewFSeq, varTcId 
      FROM Segment SEG
     WHERE SEG.tape = varTapeId  
       AND SEG.status = tconst.SEGMENT_UNPROCESSED
       AND ROWNUM < 2
     ORDER BY SEG.fseq ASC;
    -- Lock the corresponding castorfile
    SELECT CF.id INTO varUnused 
      FROM Castorfile CF, TapeCopy TC
     WHERE CF.id = TC.castorfile 
       AND TC.id = varTcId 
       FOR UPDATE OF CF.id;
  EXCEPTION WHEN NO_DATA_FOUND THEN
     outRet := -1; -- NO MORE FILES
     COMMIT;
     RETURN;
  END;
  DECLARE
    application_error EXCEPTION;
    PRAGMA EXCEPTION_INIT(application_error,-20115);
  BEGIN
    bestFileSystemForSegment(varSegId,varDSName,varMPoint,varPath,varDcId);
  EXCEPTION WHEN application_error  OR NO_DATA_FOUND THEN 
    outRet := -3;
    COMMIT;
    RETURN;
  END;
  -- Update the TapeCopy's parameters
  UPDATE TapeCopy TC
     SET TC.fseq = varNewFSeq,
         TC.TapeGatewayRequestId = varTgrId,
         TC.FileTransactionID = TG_FileTrId_Seq.NEXTVAL
   WHERE TC.id = varTcId;
   -- Update the segment's status
  UPDATE Segment SEG 
     SET SEG.status = tconst.SEGMENT_SELECTED
   WHERE SEG.id=varSegId 
     AND SEG.status = tconst.SEGMENT_UNPROCESSED;
  OPEN outFile FOR 
    SELECT CF.fileid, CF.nshost, varDSName, varMPoint, varPath, varNewFSeq , 
           TC.FileTransactionID
      FROM TapeCopy TC, Castorfile CF
     WHERE TC.id = varTcId
       AND CF.id=TC.castorfile;
END;
/

/* get the information from the db for a successful migration */
CREATE OR REPLACE
PROCEDURE tg_getRepackVidAndFileInfo(
  inFileId          IN  NUMBER, 
  inNsHost          IN VARCHAR2,
  inFseq            IN  INTEGER, 
  inTransId         IN  NUMBER, 
  inBytesTransfered IN  NUMBER,
  outRepackVid     OUT NOCOPY VARCHAR2, 
  outVID           OUT NOCOPY VARCHAR2,
  outCopyNb        OUT INTEGER, 
  outLastTime      OUT NUMBER,
  outSvcClass      OUT NOCOPY VARCHAR2,
  outFileClass     OUT NOCOPY VARCHAR2,
  outTapePool      OUT NOCOPY VARCHAR2,
  outRet           OUT INTEGER) AS 
  varCfId              NUMBER;  -- CastorFile Id
  varFileSize          NUMBER;  -- Expected file size
  varTgrId             NUMBER;  -- Tape gateway request Id
BEGIN
  outRepackVid:=NULL;
   -- ignore the repack state
  BEGIN
    SELECT CF.lastupdatetime, CF.id, CF.fileSize,     SC.name,      FC.name 
      INTO outLastTime,     varCfId, varFileSize, outSvcClass, outFileClass
      FROM CastorFile CF 
      LEFT OUTER JOIN SvcClass SC ON SC.Id = CF.SvcClass
      LEFT OUTER JOIN FileClass FC ON FC.Id = CF.FileClass
     WHERE CF.fileid = inFileId 
       AND CF.nshost = inNsHost;
     IF (outSvcClass IS NULL) THEN
       outSvcClass := 'UNKNOWN';
     END IF;
     IF (outFileClass IS NULL) THEN
       outFileClass := 'UNKNOWN';
     END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RAISE_APPLICATION_ERROR (-20119,
         'Castorfile not found for File ID='||inFileId||' and nshost = '||
           inNsHost||' in tg_getRepackVidAndFileInfo.');
  END;
  IF varFileSize <> inBytesTransfered THEN
  -- fail the file
    tg_failFileTransfer(inTransId,inFileId, inNsHost, inFseq,  1613); -- wrongfilesize
    COMMIT;
    outRet := -1;
    RETURN;
  ELSE
    outRet:=0;
  END IF;
  
  tg_RequestIdFromVDQMReqId(inTransId, varTgrId);
  IF (varTgrId IS NOT NULL) THEN
    BEGIN
      SELECT TC.copyNb INTO outcopynb 
        FROM TapeCopy TC
       WHERE TC.TapeGatewayRequestId = varTgrId
         AND TC.castorfile = varCfId
         AND TC.fseq= inFseq;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      RAISE_APPLICATION_ERROR (-20119,
         'Tapecopy not found for castorfile='||varCFId||'(File ID='||inFileId||' and nshost = '||
           inNsHost||') and fSeq='||inFseq||' in tg_getRepackVidAndFileInfo.');
    END;
    BEGIN
      SELECT  T.vid,    TP.name
        INTO outVID, outTapePool 
        FROM Tape T, Stream S
        LEFT OUTER JOIN TapePool TP ON TP.Id = S.TapePool
       WHERE T.id = S.tape
         AND S.TapeGatewayRequestId = varTgrId;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        RAISE_APPLICATION_ERROR (-20119,
           'Tape not found for tapeGatewayRequestId='||varTgrId||'(File ID='||inFileId||' and nshost = '||
             inNsHost||' and castorfile='||varCFId||'and fSeq='||inFseq||') in tg_getRepackVidAndFileInfo.');
    END;
  END IF;
  
  BEGIN 
   --REPACK case
    -- Get the repackvid field from the existing request (if none, then we are not in a repack process
     SELECT /*+ INDEX(sR I_Subrequest_Castorfile) INDEX(SRR PK_StageRepackRequest_Id) */
            SRR.repackvid INTO outRepackVid
       FROM SubRequest sR, StageRepackRequest SRR
      WHERE SRR.id = SR.request
        AND sR.status = dconst.SUBREQUEST_REPACK
        AND sR.castorFile = varCfId
        AND ROWNUM < 2;
     IF (outRepackVid IS NOT NULL) THEN
       SELECT TP.name INTO  outTapePool
         FROM Tape T
         LEFT OUTER JOIN Stream S on S.Tape = T.id
         LEFT OUTER JOIN TapePool TP on TP.id = S.TapePool
        WHERE T.VID =  outRepackVid
          AND T.tpMode = TCONST.TPMODE_WRITE;
     END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
   -- standard migration
    NULL;
  END;
  -- Format nicely in case of failure to find in both cases (repack and previous)
  IF (outTapePool IS NULL) THEN
    outTapePool := 'UNKNOWN';
  END IF;
END;
/

/* get the information from the db for a successful recall */
CREATE OR REPLACE
PROCEDURE tg_getSegmentInfo(
  inTransId     IN NUMBER,
  inFseq        IN INTEGER,
  outVid       OUT NOCOPY VARCHAR2,
  outCopyNb    OUT INTEGER ) AS
  varTrId          NUMBER;
  varTapeId        NUMBER;
  varUnused        NUMBER;
BEGIN
  -- We are looking for a recalled tape with this TGR request ID.
  tg_FindFromVDQMReqId (inTransId, varTapeId, varUnused);

  -- Throw an exception in case of not found.
  IF (varTapeId IS NULL) THEN
    outVid := NULL;
    outCopyNb := NULL;
    RAISE_APPLICATION_ERROR (-20119,
         'No tape read found for VDQMReqId'||
         inTransId || ' in tg_getSegmentInfo');
  END IF;

  -- Get the data
  BEGIN
  SELECT T.vid, T.TapeGatewayRequestId  INTO outVid, varTrId
    FROM Tape T
   WHERE T.id=varTapeId;

  SELECT TC.copynb INTO outCopyNb
    FROM TapeCopy TC
   WHERE TC.fseq = inFseq
     AND TC.TapeGateWayRequestId = varTrId;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    outVid := NULL;
    outCopyNb := NULL;
    RAISE_APPLICATION_ERROR (-20119,
         'Tailed to find tape or tape copy for VDQM Id='||
         inTransId || ',TapeId='||varTapeId||' TapeGatewayReqId='||varTrId||
         ' in tg_getSegmentInfo');
  END;
END;
/

/* get the stream without any tape associated */
CREATE OR REPLACE
PROCEDURE tg_getStreamsWithoutTapes(outStrList OUT castorTape.Stream_Cur) AS
BEGIN
  -- get streams in state PENDING with a non-NULL TapeGatewayRequestId
  OPEN outStrList FOR
    SELECT S.id, S.initialsizetotransfer, S.status, S.tapepool, TP.name
      FROM Stream S,Tapepool TP
     WHERE S.status = tconst.STREAM_PENDING
       AND S.TapeGatewayRequestId IS NOT NULL
       AND S.tapepool=TP.id 
       FOR UPDATE OF S.id SKIP LOCKED;   
END;
/

/* get tape with a pending request in VDQM */
CREATE OR REPLACE
PROCEDURE tg_getTapesWithDriveReqs(
  inTimeLimit     IN  NUMBER,
  outTapeRequest OUT castorTape.tapegatewayrequest_Cur) AS
  varTgrId        "numList";
  varTapeReadIds  "numList";
  varTapeWriteIds "numList";
  varStreamIds    "numList";
  varNow          NUMBER;
BEGIN 
  -- get requests in WAITING_TAPESERVER and ONGOING
  -- With the tape gateway request table dropping, this boils down to
  -- streams in state STREAM_WAITDRIVE, STREAM_WAITMOUNT, STREAM_RUNNING
  -- or Tape reads in state TAPE_WAITTAPEDRIVE or MOUNTED.
  
  -- In addition, we only look for the tape reads/streams which have a VDQM ping
  -- time older than inTimeLimit.  
  
  -- TODO: The function name should reflect the fact that it's actually dealing
  -- with a timeout mechanism.
  
  -- TODO: I do not have the TAPE_WAITMOUNT state in the lifecycle diagram, but
  -- include it nevertheless. This is a safe option as the select will be limite
  -- to tapes in tape read mode. If the diagram is right, this will have no
  -- effect and if the diagram is wrong, this will lead to cover the
  -- non-decribed case.

  -- No need to query the clock all the time
  varNow := gettime();
  
  -- Find all the tapes and lock
  SELECT T.id BULK COLLECT INTO varTapeReadIds
    FROM Tape T
   WHERE T.status IN ( tconst.TAPE_WAITDRIVE, tconst.TAPE_WAITMOUNT, tconst.TAPE_MOUNTED )
     AND T.tpMode = tconst.TPMODE_READ
     AND T.TapeGatewayRequestId IS NOT NULL
     AND varNow - T.lastVdqmPingTime > inTimeLimit
     FOR UPDATE SKIP LOCKED;
     
  -- Find all the streams and lock
  SELECT S.id, T.id BULK COLLECT INTO varStreamIds, varTapeWriteIds
    FROM Stream S, Tape T
   WHERE S.Status IN ( tconst.STREAM_WAITDRIVE, tconst.STREAM_WAITMOUNT, tconst.STREAM_RUNNING )
     AND S.TapeGatewayRequestId IS NOT NULL
     AND S.Tape = T.Id
     AND varNow - T.lastVdqmPingTime > inTimeLimit
     FOR UPDATE SKIP LOCKED;
     
  -- Update the last VDQM ping time for all of them.
  varNow := gettime();
  UPDATE Tape T
     SET T.lastVdqmPingTime = varNow
   WHERE T.id IN ( SELECT /*+ CARDINALITY(trTable 5) */ * 
                     FROM TABLE (varTapeReadIds)
                    UNION ALL SELECT /*+ CARDINALITY(trTable 5) */ *
                     FROM TABLE (varTapeWriteIds));
                   
  -- Return them. For VDQM request IT, we have to split the select in 2 and
  -- union in the end, unlike in the previous statement.
  OPEN outTapeRequest FOR
    -- Read case
    SELECT T.TpMode, T.TapeGatewayRequestId, T.startTime,
           T.lastvdqmpingtime, T.vdqmVolReqid, 
           T.vid
      FROM Tape T
     WHERE T.Id IN (SELECT /*+ CARDINALITY(trTable 5) */ *
                    FROM TABLE(varTapeReadIds))
     UNION ALL
    -- Write case
    SELECT T.tpMode, S.TapeGatewayRequestId, T.startTime,
           T.lastvdqmpingtime, S.vdqmVolReqid,
           T.vid
      FROM Tape T, Stream S
     WHERE S.Id IN (SELECT /*+ CARDINALITY(trTable 5) */ *
                    FROM TABLE(varStreamIds))
       AND S.Tape = T.id;
END;
/

/* get a tape without any drive requests sent to VDQM */
CREATE OR REPLACE
PROCEDURE tg_getTapeWithoutDriveReq(
  outReqId    OUT NUMBER,
  outTapeMode OUT NUMBER,
  outTapeSide OUT INTEGER,
  outTapeVid  OUT NOCOPY VARCHAR2) AS
  varStreamId     NUMBER;
  varTapeId       NUMBER;
BEGIN
  -- Initially looked for tapegateway request in state TO_BE_SENT_TO_VDQM
  -- Find a tapegateway request id for which there is a tape read in
  -- state TAPE_PENDING or a Stream in state STREAM_WAIT_TAPE.
  -- This method is called until there are no more pending tapes
  -- We serve writes first and then reads
  BEGIN
    BEGIN
      SELECT S.id INTO varStreamId
        FROM Stream S
       WHERE S.status = tconst.STREAM_TO_BE_SENT_TO_VDQM
         AND ROWNUM < 2
       ORDER BY dbms_random.value()
         FOR UPDATE SKIP LOCKED;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      varStreamId := NULL; -- Nothing to be found, we'll just carry on to the reads.
    END;
    IF (varStreamId IS NOT NULL) THEN
      SELECT S.TapeGatewayRequestId,     1,      T.side,      T.vid
        INTO outReqId, outTapeMode, outTapeSide, outTapeVid
        FROM Stream S, Tape T
       WHERE T.id = S.tape
         AND S.id = varStreamId;
      RETURN;
    END IF;
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RAISE_APPLICATION_ERROR (-20119, 'Stream in stage STREAM_TOBESENTTOVDQM has no tape attached!' ||
      'Stream='||varStreamId);
  END;
  BEGIN -- The read casse
    SELECT T.TapeGatewayRequestId,     0,      T.side,      T.vid,      T.id
      INTO outReqId,         outTapeMode, outTapeSide, outTapeVid, varTapeId
      FROM Tape T
     WHERE T.tpMode = tconst.TPMODE_READ
       AND T.status = tconst.TAPE_PENDING
       AND ROWNUM < 2
       FOR UPDATE SKIP LOCKED;
     -- Potential lazy/late definition of the request id
     -- We might be confronted to a not defined request id if the tape was created
     -- by the stager straight into pending state in the absence of a recall policy
     -- otherwise, the tape will go through resurrect tape (rec handler) and all
     -- will be fine.
     -- If we get here, we found a tape so the request id must be defined when leaving.
     IF (outReqId IS NULL OR outReqId = 0) THEN
       SELECT ids_seq.nextval INTO outReqId FROM DUAL;
       UPDATE Tape T SET T.TapeGatewayRequestId = outReqId WHERE T.id = varTapeId;
     END IF; 
  EXCEPTION WHEN NO_DATA_FOUND THEN
    outReqId := 0;
  END;
END;
/


/* get tape to release in VMGR */
CREATE OR REPLACE
PROCEDURE tg_getTapeToRelease(
  inVdqmReqId IN  INTEGER, 
  outVID      OUT NOCOPY VARCHAR2, 
  outMode     OUT INTEGER ) AS
  varStrId        NUMBER;
  varTpId         NUMBER;
BEGIN
  -- Find Tape read or stream for this vdqm request
  tg_findFromVDQMReqId(inVdqmReqId, varTpId, varStrId);
  
   IF (varTpId IS NOT NULL) THEN -- read case
     outMode := 0;
     SELECT T.vid INTO outVID 
       FROM Tape T
       WHERE T.id = varTpId; 
   ELSIF (varStrId IS NOT NULL) THEN -- write case
     outMode := 1;
     SELECT T.vid INTO outVID 
       FROM Tape T, Stream S
      WHERE S.id=varStrId
        AND S.tape=T.id;
   END IF;
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- already cleaned by the checker
  NULL;
END;
/

/* invalidate a file that it is not possible to tape as candidate to migrate or recall */
CREATE OR REPLACE
PROCEDURE tg_invalidateFile(
  inTransId   IN NUMBER,
  inFileId    IN NUMBER, 
  inNsHost    IN VARCHAR2,
  inFseq      IN INTEGER,
  inErrorCode IN INTEGER) AS
  varTapeId      NUMBER;
  varStreamId    NUMBER;
BEGIN
  tg_findFromVDQMReqId (inTransId, varTapeId, varStreamId);
  IF (varStreamId IS NOT NULL) THEN -- We want the tape so in case we are 
  --migrating, "convert" the stream Id into a tape id.
    SELECT S.tape INTO varTapeId 
      FROM Stream S
     WHERE S.Id=varStreamId;
  END IF;
  -- Now we should have a tape id in all cases
  IF (varTapeId IS NOT NULL) THEN
    UPDATE Tape T
       SET T.lastfseq = T.lastfseq-1,
           T.vdqmvolreqid = inTransId
     WHERE T.id = varTapeId;
     tg_failfiletransfer(inTransId, inFileId, inNsHost, inFseq, inErrorCode);
  ELSE
    RAISE_APPLICATION_ERROR (-20119, 
         'Tailed to find tape for VDQM Id='|| 
         inTransId || ' in tg_invalidateFile');
  END IF;
END;
/


/* restart taperequest which had problems */
CREATE OR REPLACE
PROCEDURE tg_restartLostReqs(
  trIds IN castor."cnumList") AS
  vdqmId INTEGER;
BEGIN
 FOR  i IN trIds.FIRST .. trIds.LAST LOOP   
   BEGIN
     tg_findVDQMReqFromTGReqId(trIds(i), vdqmId);
     tg_endTapeSession(vdqmId,0);
   EXCEPTION WHEN NO_DATA_FOUND THEN
     NULL;
   END;
 END LOOP;
 COMMIT;
END;
/


/* update the db after a successful migration */
CREATE OR REPLACE
PROCEDURE TG_SetFileMigrated(
  inTransId         IN  NUMBER, 
  inFileId          IN  NUMBER,
  inNsHost          IN  VARCHAR2, 
  inFseq            IN  INTEGER, 
  inFileTransaction IN  NUMBER) AS
  varUnused             NUMBER;
  varTapeCopyCount      INTEGER;
  varCfId               NUMBER;
  varTcId               NUMBER;
  varTcIds              "numList";
  varTapeId             NUMBER;
  varStreamId           NUMBER;
BEGIN
  -- Find Stream or tape from vdqm vol req ID Lock
  tg_findFromVDQMReqId (inTransId, varTapeId, varStreamId);
  IF (varTapeId IS NOT NULL) THEN
    SELECT T.Id INTO varUnused
      FROM Tape T WHERE T.Id = varTapeId
       FOR UPDATE;
  ELSIF (varStreamId IS NOT NULL) THEN
    SELECT S.Id INTO varUnused
      FROM Stream S WHERE S.Id = varStreamId
       FOR UPDATE;  
  ELSE
    RAISE_APPLICATION_ERROR (-20119, 
         'Could not find stream or tape read for VDQM request Id='|| 
         inTransId || ' in TG_SetFileMigrated');
  END IF;
  -- Lock the CastorFile
  SELECT CF.id INTO varCfId FROM CastorFile CF
   WHERE CF.fileid = inFileId 
     AND CF.nsHost = inNsHost 
     FOR UPDATE;
  -- Locate the corresponding tape copy and Disk Copy, Lock
  SELECT   TC.id
    INTO varTcId
    FROM TapeCopy TC
   WHERE TC.FileTransactionId = inFileTransaction
     AND TC.fSeq = inFseq
     FOR UPDATE;
  UPDATE tapecopy TC
     SET TC.status = tconst.TAPECOPY_STAGED
   WHERE TC.id = varTcId;
  SELECT count(*) INTO varTapeCopyCount
    FROM tapecopy TC
    WHERE TC.castorfile = varCfId  
     AND STATUS != tconst.TAPECOPY_STAGED;
  -- let's check if another copy should be done, if not, we're done for this file.
  IF varTapeCopyCount = 0 THEN
     -- Mark all disk copies as staged and delete all tape copies together.
     UPDATE DiskCopy DC
        SET DC.status= dconst.DISKCOPY_STAGED
      WHERE DC.castorFile = varCfId
        AND DC.status= dconst.DISKCOPY_CANBEMIGR;
     DELETE FROM tapecopy TC
      WHERE castorfile = varCfId 
  RETURNING id BULK COLLECT INTO varTcIds;
     FORALL i IN varTcIds.FIRST .. varTcIds.LAST
       DELETE FROM id2type 
         WHERE id=varTcIds(i);
  END IF;
  -- archive Repack requests should any be in the db
  FOR i IN (
    SELECT /*+ INDEX(SR I_Subrequest_Castorfile)*/ SR.id FROM SubRequest SR
    WHERE SR.castorfile = varCfId AND
          SR.status= dconst.SUBREQUEST_REPACK
    ) LOOP
      archivesubreq(i.id, 8); -- SUBREQUEST_FINISHED
  END LOOP;
  COMMIT;
END;
/


/* update the db after a successful recall */
CREATE OR REPLACE
PROCEDURE tg_setFileRecalled(
  inTransId          IN  NUMBER, 
  inFileId           IN  NUMBER,
  inNsHost           IN  VARCHAR2, 
  inFseq             IN  NUMBER, 
  inFileTransaction  IN  NUMBER) AS
  varTcId               NUMBER;         -- TapeCopy Id
  varDcId               NUMBER;         -- DiskCopy Id
  varCfId               NUMBER;         -- CastorFile Id
  srId NUMBER;
  varSubrequestId       NUMBER; 
  varRequestId          NUMBER;
  varFileSize           NUMBER;
  varGcWeight           NUMBER;         -- Garbage collection weight
  varGcWeightProc       VARCHAR(2048);  -- Garbage collection weighting procedure name
  varEuid               INTEGER;        -- Effective user Id
  varEgid               INTEGER;        -- Effective Group Id
  varSvcClassId         NUMBER;         -- Service Class Id
  varMissingCopies      INTEGER;
  varUnused             NUMBER;
  varTapeId             NUMBER;
  varStreamId           NUMBER;
BEGIN
  SAVEPOINT TGReq_CFile_TCopy_Locking;
  -- Find Stream or tape from vdqm vol req ID Lock
  tg_findFromVDQMReqId (inTransId, varTapeId, varStreamId);
  IF (varTapeId IS NOT NULL) THEN
    SELECT T.Id INTO varUnused
      FROM Tape T WHERE T.Id = varTapeId
       FOR UPDATE;
  ELSIF (varStreamId IS NOT NULL) THEN
    SELECT S.Id INTO varUnused
      FROM Stream S WHERE S.Id = varStreamId
       FOR UPDATE;  
  ELSE
    ROLLBACK TO SAVEPOINT TGReq_CFile_TCopy_Locking;
    RAISE_APPLICATION_ERROR (-20119, 
         'Could not find stream or tape read for VDQM request Id='|| 
         inTransId || ' in TG_SetFileMigrated');
  END IF;
  -- find and lock castor file for the nsHost/fileID
  SELECT CF.id, CF.fileSize INTO varCfId, varFileSize
    FROM CastorFile CF 
   WHERE CF.fileid = inFileId 
     AND CF.nsHost = inNsHost 
     FOR UPDATE;
  -- Find and lock the tape copy
  varTcId := NULL;
  SELECT TC.id INTO varTcId
    FROM TapeCopy TC
   WHERE TC.FileTransactionId = inFileTransaction
     AND TC.fSeq = inFseq
     FOR UPDATE;
  -- find and lock the disk copy. There should be only one.
  SELECT DC.id INTO varDcId
    FROM DiskCopy DC
   WHERE DC.castorFile = varCfId
     AND DC.status = dconst.DISKCOPY_WAITTAPERECALL
     FOR UPDATE;
  -- If nothing found, die releasing the locks
  IF varTCId = NULL THEN
    ROLLBACK TO SAVEPOINT TGReq_CFile_TCopy_Locking;
    RAISE NO_DATA_FOUND;
  END IF;
  -- missing tapecopies handling
  SELECT TC.missingCopies INTO varMissingCopies
    FROM TapeCopy TC WHERE TC.id = varTcId;
  -- delete tapecopies
  deleteTapeCopies(varCfId);
  -- update diskcopy status, size and gweight
  SELECT /*+ INDEX(SR I_Subrequest_DiskCopy)*/ SR.id, SR.request
    INTO varSubrequestId, varRequestId
    FROM SubRequest SR
   WHERE SR.diskcopy = varDcId;
  SELECT REQ.svcClass, REQ.euid, REQ.egid INTO varSvcClassId, varEuid, varEgid
    FROM (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, svcClass, euid, egid FROM StageGetRequest                                  UNION ALL
          SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id, svcClass, euid, egid FROM StagePrepareToGetRequest       UNION ALL
          SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, svcClass, euid, egid FROM StageUpdateRequest                         UNION ALL
          SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id, svcClass, euid, egid FROM StagePrepareToUpdateRequest UNION ALL
          SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id, svcClass, euid, egid FROM StageRepackRequest) REQ
    WHERE REQ.id = varRequestId;
  varGcWeightProc := castorGC.getRecallWeight(varSvcClassId);
  EXECUTE IMMEDIATE 'BEGIN :newGcw := ' || varGcWeightProc || '(:size); END;'
    USING OUT varGcWeight, IN varFileSize;
  UPDATE DiskCopy DC
    SET DC.status = decode(varMissingCopies,
                           0, dconst.DISKCOPY_STAGED,
                              dconst.DISKCOPY_CANBEMIGR), -- DISKCOPY_STAGED if varMissingCopies = 0, otherwise CANBEMIGR
        DC.lastAccessTime = getTime(),  -- for the GC, effective lifetime of this diskcopy starts now
        DC.gcWeight = varGcWeight,
        DC.diskCopySize = varFileSize
    WHERE Dc.id = varDcId;
  -- restart this subrequest so that the stager can follow it up
  UPDATE /*+ INDEX(SR PK_Subrequest_Id)*/ SubRequest SR
     SET SR.status = dconst.SUBREQUEST_RESTART, 
         SR.getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED,
         SR.lastModificationTime = getTime(), SR.parent = 0
   WHERE SR.id = varSubrequestId;
  -- and trigger new migrations if missing tape copies were detected
  IF varMissingCopies > 0 THEN
    DECLARE
      newTcId INTEGER;
    BEGIN
      FOR i IN 1..varMissingCopies LOOP
        INSERT INTO TapeCopy (id, copyNb, castorFile, status, nbRetry, missingCopies)
        VALUES (ids_seq.nextval, 0, varCfId, TCONST.TAPECOPY_CREATED, 0, 0)
        RETURNING id INTO newTcId;
        INSERT INTO Id2Type (id, type) VALUES (newTcId, 30); -- OBJ_TapeCopy
      END LOOP;
    END;
  END IF;
  -- restart other requests waiting on this recall
  UPDATE /*+ INDEX(SR I_Subrequest_Parent)*/ SubRequest SR
     SET SR.status = dconst.SUBREQUEST_RESTART,
         SR.getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED,
         SR.lastModificationTime = getTime(), SR.parent = 0
   WHERE SR.parent = varSubrequestId;
  -- trigger the creation of additional copies of the file, if necessary.
  replicateOnClose(varCfId, varEuid, varEgid);
  COMMIT;
END;
/




/* save in the db the results returned by the retry policy for migration */
CREATE OR REPLACE
PROCEDURE tg_setMigRetryResult(
  tcToRetry IN castor."cnumList",
  tcToFail  IN castor."cnumList" ) AS
  srId NUMBER;
  cfId NUMBER;

BEGIN
   -- check because oracle cannot handle empty buffer
  IF tcToRetry( tcToRetry.FIRST) != -1 THEN
    
    -- restarted the one to be retried
    FOR i IN tctoretry.FIRST .. tctoretry.LAST LOOP
      UPDATE TapeCopy SET
        status = tconst.TAPECOPY_TOBEMIGRATED,
        nbretry = nbretry+1,
        vid = NULL  -- this tapecopy will not go to this volume after all, at least not now...
        WHERE id = tcToRetry(i);
    END LOOP;
  END IF;

  -- check because oracle cannot handle empty buffer
  IF tcToFail(tcToFail.FIRST) != -1 THEN
    -- fail the tapecopies
    FORALL i IN tctofail.FIRST .. tctofail.LAST
      UPDATE TapeCopy SET
        status = tconst.TAPECOPY_FAILED
      WHERE id = tcToFail(i);

    -- fail repack subrequests
    FOR i IN tcToFail.FIRST .. tcToFail.LAST LOOP
        BEGIN
        -- we don't need a lock on castorfile because we cannot have a parallel migration of the same file using repack
          SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/
                 SubRequest.id, SubRequest.castorfile into srId, cfId
            FROM SubRequest,TapeCopy
            WHERE TapeCopy.id = tcToFail(i)
            AND SubRequest.castorfile = TapeCopy.castorfile
            AND subrequest.status = dconst.SUBREQUEST_REPACK;

          -- STAGED because the copy on disk most probably is valid and the failure of repack happened during the migration

          UPDATE DiskCopy
            SET status = dconst.DISKCOPY_STAGED
            WHERE castorfile = cfId
            AND status=dconst.DISKCOPY_CANBEMIGR; -- otherwise repack will wait forever

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
CREATE OR REPLACE PROCEDURE tg_setRecRetryResult(
  tcToRetry IN castor."cnumList", 
  tcToFail  IN castor."cnumList"  ) AS
  tapeId NUMBER;
  cfId NUMBER;

BEGIN
  -- I restart the recall that I want to retry
  -- check because oracle cannot handle empty buffer
  IF tcToRetry(tcToRetry.FIRST) != -1 THEN 

    -- tapecopy => TOBERECALLED
    FORALL i IN tcToRetry.FIRST .. tcToRetry.LAST
      UPDATE TapeCopy
        SET status    = tconst.TAPECOPY_TOBERECALLED,
            errorcode = 0,
            nbretry   = nbretry+1 
        WHERE id=tcToRetry(i);
    
    -- segment => UNPROCESSED
    -- tape => PENDING if UNUSED OR FAILED with still segments unprocessed
    FOR i IN tcToRetry.FIRST .. tcToRetry.LAST LOOP
      UPDATE Segment
        SET status = tconst.SEGMENT_UNPROCESSED
        WHERE copy = tcToRetry(i)
        RETURNING tape INTO tapeId;
      UPDATE Tape
        SET status = tconst.TAPE_WAITPOLICY
        WHERE id = tapeId AND
          status IN (tconst.TAPE_UNUSED, tconst.TAPE_FAILED);
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
      UPDATE DiskCopy SET status = dconst.DISKCOPY_FAILED
        WHERE castorFile = cfId 
        AND status = dconst.DISKCOPY_WAITTAPERECALL;
      
      -- Drop tape copies. Ideally, we should keep some track that
      -- the recall failed in order to prevent future recalls until some
      -- sort of manual intervention. For the time being, as we can't
      -- say whether the failure is fatal or not, we drop everything
      -- and we won't deny a future request for recall.
      deleteTapeCopies(cfId);
      
      -- fail subrequests
      UPDATE /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest 
        SET status = dconst.SUBREQUEST_FAILED,
            getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED, --  (not strictly correct but the request is over anyway)
            lastModificationTime = getTime(),
            errorCode = 1015,  -- SEINTERNAL
            errorMessage = 'File recall from tape has failed, please try again later',
            parent = 0
        WHERE castorFile = cfId 
        AND status IN (dconst.SUBREQUEST_WAITTAPERECALL, dconst.SUBREQUEST_WAITSUBREQ);
    
    END LOOP;
  END IF;
  COMMIT;
END;
/


/* update the db when a tape session is started */
CREATE OR REPLACE
PROCEDURE  tg_startTapeSession(
  inVdqmReqId    IN  NUMBER,
  outVid         OUT NOCOPY VARCHAR2,
  outAccessMode  OUT INTEGER,
  outRet         OUT INTEGER,
  outDensity     OUT NOCOPY VARCHAR2,
  outLabel       OUT NOCOPY VARCHAR2 ) AS
  varTGReqId         NUMBER;
  varTpId            NUMBER;
  varStreamId        NUMBER;
  varUnused          NUMBER;
BEGIN
  outRet:=-2;
  -- set the request to ONGOING
  -- Transition from REQUEST WAITING TAPE SERVER to ONGOING
  -- is equivalent to WAITTAPERIVE to MOUNTED for the tape read
  -- and WAITDRIVE ot WAITMOUNT to RUNNING for a stream.

  -- Step 1, pick the stream or tape.
  tg_findFromVDQMReqId(inVdqmReqId, varTpId, varStreamId);
  IF (varTpId IS NOT NULL) THEN
    -- Read case
    outAccessMode := 0;
    BEGIN
      SELECT 1 INTO varUnused FROM dual
       WHERE EXISTS (SELECT 'x' FROM Segment S
                      WHERE S.tape = varTpId);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      UPDATE Tape T
         SET T.lastvdqmpingtime=0
       WHERE T.id = varTpId; -- to force the cleanup
      outRet:=-1; --NO MORE FILES
      COMMIT;
      RETURN;
    END;
    UPDATE Tape T
       SET T.status = tconst.TAPE_MOUNTED
     WHERE T.id = varTpId
       AND T.tpmode = tconst.TPMODE_READ
    RETURNING T.vid,  T.label,  T.density
      INTO   outVid, outLabel, outDensity; -- tape is MOUNTED
    outRet:=0;
    COMMIT;
    RETURN;
  ELSIF (varStreamId IS NOT NULL) THEN
    -- Write case
    outAccessMode := 1;
    BEGIN
      SELECT 1 INTO varUnused FROM dual
       WHERE EXISTS (SELECT 'x' FROM Stream2TapeCopy STTC
                      WHERE STTC.parent = varStreamId);
    EXCEPTION WHEN NO_DATA_FOUND THEN
      -- no more files
      SELECT S.tape INTO varTpId
        FROM Stream S
       WHERE S.id = varStreamId;
      UPDATE Tape T
         SET T.lastvdqmpingtime=0
       WHERE T.id=varTpId; -- to force the cleanup
      outRet:=-1; --NO MORE FILES
      outVid:=NULL;
      COMMIT;
      RETURN;
    END;
    UPDATE Stream S
       SET S.status = tconst.STREAM_RUNNING
     WHERE S.id = varStreamId
     RETURNING S.tape INTO varTpId; -- RUNNING
    UPDATE Tape T
       SET T.status = tconst.TAPE_MOUNTED
     WHERE T.id = varTpId
    RETURNING T.vid,  T.label,  T.density
        INTO outVid, outLabel, outDensity;
    outRet:=0;
    COMMIT;
  ELSE
    -- Not found
    outRet:=-2; -- UNKNOWN request
  END IF;
END;
/

/* Check configuration */
CREATE OR REPLACE PROCEDURE tg_checkConfiguration AS
  unused VARCHAR2(2048);
BEGIN
  -- This fires an exception if the db is configured not to run the tapegateway
  SELECT value INTO unused FROM castorconfig WHERE class='tape' AND key='interfaceDaemon' AND value='tapegatewayd';
END;
/


/* delete streams for not existing tapepools */
CREATE OR REPLACE
PROCEDURE tg_deleteStream(inStrId IN NUMBER) AS
  varUnused NUMBER;
  varTcIds  "numList"; -- TapeCopy Ids
  varTgrId   NUMBER;   -- TapeGatewayRequest Id
BEGIN
  -- First lock the stream
  SELECT S.id INTO varUnused FROM Stream S 
   WHERE S.id = inStrId FOR UPDATE;
  -- Disconnect the tapecopies
  DELETE FROM stream2tapecopy STTC
   WHERE STTC.parent = inStrId 
  RETURNING STTC.child BULK COLLECT INTO varTcIds;
  -- Hand back the orphaned tape copies to the MigHunter (by setting back their
  -- statues, mighunter will pick them up on it).
  FORALL i IN varTcIds.FIRST .. VarTcIds.LAST
    UPDATE tapecopy TC
       SET TC.status = tconst.TAPECOPY_TOBEMIGRATED
     WHERE TC.Id = varTcIds(i)
       AND NOT EXISTS (SELECT 'x' FROM stream2tapecopy STTC 
                        WHERE STTC.child = varTcIds(i));
  -- Finally drop the stream itself
  DELETE FROM id2type ITT where ITT.id = inStrId;
  DELETE FROM Stream S where S.id= inStrId;
END;
/


/* delete taperequest  for not existing tape */
CREATE OR REPLACE
PROCEDURE tg_deleteTapeRequest( inTGReqId IN NUMBER ) AS
  /* The tape gateway request does not exist per se, but 
   * references to its ID should be removed (with needed consequences
   * from the structures pointing to it) */
  CONSTRAINT_VIOLATED EXCEPTION;
  PRAGMA EXCEPTION_INIT(CONSTRAINT_VIOLATED, -02292);
  varTpReqId NUMBER;     -- Tape Recall (TapeGatewayReequest.TapeRecall)
  varStrId NUMBER;       -- Stream Id.
  varSegNum INTEGER;
  varCfId NUMBER;        -- CastorFile Id
  varTcIds "numList";    -- Tapecopies IDs
  varSrIds "numList";
BEGIN
  -- Find the relevant stream or reading tape id.
  tg_findFromTGRequestId (inTGReqId, varTpReqId, varStrId);
  -- Find out whether this is a read or a write
  IF (varTpReqId IS NOT NULL) THEN
    -- Lock and reset the tape in case of a read
    UPDATE Tape T
      SET T.status = tconst.TAPE_UNUSED
      WHERE T.id = varTpReqId;
    SELECT SEG.copy BULK COLLECT INTO varTcIds 
      FROM Segment SEG 
     WHERE SEG.tape = varTpReqId;
    FOR i IN varTcIds.FIRST .. varTcIds.LAST  LOOP
      -- lock castorFile	
      SELECT TC.castorFile INTO varCfId 
        FROM TapeCopy TC, CastorFile CF
        WHERE TC.id = varTcIds(i) 
        AND CF.id = TC.castorfile 
        FOR UPDATE OF CF.id;
      -- fail diskcopy, drop tapecopies
      UPDATE DiskCopy DC SET DC.status = dconst.DISKCOPY_FAILED
       WHERE DC.castorFile = varCfId 
         AND DC.status = dconst.DISKCOPY_WAITTAPERECALL;
      deleteTapeCopies(varCfId);
      -- Fail the subrequest
      UPDATE /*+ INDEX(SR I_Subrequest_Castorfile)*/ SubRequest SR
         SET SR.status = dconst.SUBREQUEST_FAILED,
             SR.getNextStatus = dconst.GETNEXTSTATUS_FILESTAGED, --  (not strictly correct but the request is over anyway)
             SR.lastModificationTime = getTime(),
             SR.errorCode = 1015,  -- SEINTERNAL
             SR.errorMessage = 'File recall from tape has failed, please try again later',
             SR.parent = 0
       WHERE SR.castorFile = varCfId 
         AND SR.status IN (dconst.SUBREQUEST_WAITTAPERECALL, dconst.SUBREQUEST_WAITSUBREQ);
    END LOOP;
  ELSIF (varStrId IS NOT NULL) THEN
    -- In case of a write, reset the stream
    DeleteOrStopStream (varStrId);
  ELSE
    -- Wrong Access Mode encountered. Notify.
    RAISE_APPLICATION_ERROR(-20292, 'tg_deleteTapeRequest: no read tape or '||
      'stream found for TapeGatewayRequestId: '|| inTGReqId);
  END IF;
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
  unused INTEGER;
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
           gcType = decode(gcType, NULL, 1, gcType)
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
         (SELECT 'x' FROM StageDiskCopyReplicaRequest, DiskCopy D
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
      -- Do not delete STAGED files from non production hardware
      BEGIN
        SELECT FileSystem.id INTO unused
          FROM DiskServer, FileSystem
         WHERE FileSystem.id = fs.id
           AND FileSystem.status = 0  -- PRODUCTION
           AND FileSystem.diskserver = DiskServer.id
           AND DiskServer.status = 0; -- PRODUCTION
      EXCEPTION WHEN NO_DATA_FOUND THEN
        EXIT;
      END;
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
                     SELECT /*+ INDEX(DiskCopy I_DiskCopy_FileSystem) */ id, castorFile FROM DiskCopy
                      WHERE fileSystem = fs.id
                        AND status = 0 -- STAGED
                        AND NOT EXISTS (
                          SELECT /*+ INDEX(SubRequest I_SubRequest_DiskCopy) */ 'x'
                            FROM SubRequest
                           WHERE SubRequest.diskcopy = DiskCopy.id
                             AND SubRequest.status IN (4, 5, 6, 12, 13, 14)) -- being processed (WAIT*, READY, *SCHED)
                        AND NOT EXISTS
                          -- Ignore diskcopies with active replications
                          (SELECT /*+ INDEX(DCRR I_StageDiskCopyReplic_DestDC) */ 'x'
                             FROM StageDiskCopyReplicaRequest DCRR, DiskCopy DD
                            WHERE DCRR.destDiskCopy = DD.id
                              AND DCRR.sourceDiskCopy = DiskCopy.id
                              AND DD.status = 1)  -- WAITD2D
                        ORDER BY gcWeight ASC)
                    WHERE rownum <= 10000 - totalCount) LOOP
          -- Mark the DiskCopy
          UPDATE DiskCopy
             SET status = 9, -- BEINGDELETED
                 gcType = 0  -- GCTYPE_AUTO
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
                WHEN DiskCopy.gcType = 4 THEN 'NS synchronization'
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
    -- List the castorfiles to be cleaned up afterwards
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      INSERT INTO filesDeletedProcHelper VALUES
           ((SELECT castorFile FROM DiskCopy
              WHERE id = dcIds(i)));
    -- Loop over the deleted files; first use FORALL for bulk operation
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      DELETE FROM Id2Type WHERE id = dcIds(i);
    FORALL i IN dcIds.FIRST .. dcIds.LAST
      DELETE FROM DiskCopy WHERE id = dcIds(i);
    -- Then use a normal loop to clean castorFiles. Note: We order the list to
    -- prevent a deadlock
    FOR cf IN (SELECT DISTINCT(cfId)
                 FROM filesDeletedProcHelper
                ORDER BY cfId ASC) LOOP
      DECLARE
        nb NUMBER;
      BEGIN
        -- First try to lock the castorFile
        SELECT id INTO nb FROM CastorFile
         WHERE id = cf.cfId FOR UPDATE;
        -- See whether it has any DiskCopy
        SELECT count(*) INTO nb FROM DiskCopy
         WHERE castorFile = cf.cfId;
        -- If any DiskCopy, give up
        IF nb = 0 THEN
          -- Delete the TapeCopies
          deleteTapeCopies(cf.cfId);
          -- See whether pending SubRequests exist
          SELECT /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ count(*) INTO nb
            FROM SubRequest
           WHERE castorFile = cf.cfId
             AND status IN (0, 1, 2, 3, 4, 5, 6, 7, 10, 12, 13, 14);  -- All but FINISHED, FAILED_FINISHED, ARCHIVED
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
              -- Check whether this file potentially had TapeCopies
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
            UPDATE /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest
               SET status = 7 -- FAILED
             WHERE castorFile = cf.cfId
               AND status IN (0, 1, 2, 3, 4, 5, 6, 12, 13, 14);
          END IF;
        END IF;
      EXCEPTION WHEN NO_DATA_FOUND THEN
        -- Ignore, this means that the castorFile did not exist.
        -- There is thus no way to find out whether to remove the
        -- file from the nameserver. For safety, we thus keep it
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
  UPDATE /*+ INDEX(Subrequest I_Subrequest_Castorfile)*/ SubRequest
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
      stageForcedRm(fileIds(fid), nsHostName, 4);  -- GCTYPE_NS_SYNCH
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
  FOR f IN (SELECT /*+ USE_NL(D C) INDEX(D I_DISKCOPY_STATUS) */ c.filesize, c.id,
                   c.fileId, c.nsHost, d.fileSystem, d.id AS dcId, d.status AS dcStatus
              FROM DiskCopy d, Castorfile c
             WHERE c.id = d.castorFile
               AND d.creationTime < getTime() - timeOut
               AND d.status IN (5, 6, 11) -- WAITFS, STAGEOUT, WAITFS_SCHEDULING
               AND NOT EXISTS (
                 SELECT /*+ INDEX(ID2TYPE PK_ID2TYPE_ID) */ 'x'
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
        SELECT /*+ INDEX(Subrequest I_Subrequest_Diskcopy)*/ id
          INTO srId   -- there can only be one outstanding PrepareToPut/Update, if any
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
         DFS.comments Comments,
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
                   6, 'DELETING',
                   7, 'RESTART', 'UNKNOWN') Status,
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
         -- already transferred. If the process is in a CREATED, INITIALIZING,
         -- DELETING or RESTART status N/A will be returned.
         decode(DFS.status, 0, 'N/A', 1, 'N/A', 6, 'N/A', 7, 'N/A',
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
                           (getTime() - DFS.startTime))))), 'N/A')), 'N/A') ETC
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



/* SQL statement for the removeFailedDrainingTransfers procedure */
CREATE OR REPLACE PROCEDURE removeFailedDrainingTransfers(fsId IN NUMBER)
AS
  CURSOR c IS
    SELECT STDCRR.id, SR.id subrequest, STDCRR.client
      FROM StageDiskCopyReplicaRequest STDCRR, DrainingDiskCopy DDC,
           SubRequest SR
     WHERE STDCRR.sourceDiskCopy = DDC.diskCopy
       AND SR.request = STDCRR.id
       AND decode(DDC.status, 4, DDC.status, NULL) = 4  -- FAILED
       AND SR.status = 9  -- FAILED_FINISHED
       AND DDC.fileSystem = fsId;
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
  END LOOP;
  -- Delete all data related to the filesystem from the draining diskcopy table
  DELETE FROM DrainingDiskCopy
   WHERE fileSystem = fsId;
END;
/


/* Procedure responsible for stopping the draining process for a diskserver
 * or filesystem. In no filesystem is specified then all filesystems
 * associated to the diskserver will be stopped.
 */
CREATE OR REPLACE PROCEDURE stopDraining(inNodeName   IN VARCHAR,
                                         inMountPoint IN VARCHAR2 DEFAULT NULL,
                                         inRestart    IN NUMBER DEFAULT 0)
AS
  fsIds  "numList";
  unused NUMBER;
  mntPnt VARCHAR2(2048);
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
  -- Update the filesystem entries to DELETING or RESTART depending in the
  -- inRestart option. The drainManager job will take care of actually doing
  -- the work.
  FOR i IN fsIds.FIRST .. fsIds.LAST
  LOOP
    -- Check to see if the mountpoint and diskserver combination allow for a
    -- draining operation to begin.
    BEGIN
      SELECT mountPoint INTO mntPnt
        FROM FileSystem WHERE id = fsIds(i);
      -- If restarting verify that the diskserver and filesystem are in a 
      -- valid state, code copied from startDraining.
      IF inRestart = 1 THEN
        SELECT FS.diskPool INTO unused
          FROM FileSystem FS, DiskServer DS
         WHERE FS.diskServer = DS.id
           AND FS.id = fsIds(i)
           AND (FS.status = 1 OR DS.status = 1)
           AND FS.status != 2
           AND DS.status != 2;
      END IF;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      raise_application_error
        (-20116, 'Mountpoint: Restart not possible '||mntPnt||' is not in a DRAINING state or diskserver is DISABLED');
    END;
    UPDATE DrainingFileSystem
       SET status = decode(inRestart, 1, 7, 6)
     WHERE fileSystem = fsIds(i);
  END LOOP;
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
                        inMaxTransfers IN NUMBER DEFAULT 50,
                        inComments     IN VARCHAR DEFAULT 'N/A')
AS
  ret    NUMBER;
  fsIds  "numList";
  svcId  NUMBER;
  unused NUMBER;
  mntPnt VARCHAR2(2048);
  maxCommentLen NUMBER;
BEGIN
  -- Check that the comment is not too long.
  SELECT char_length INTO maxCommentLen
    FROM user_tab_columns
   WHERE table_name = 'DRAININGFILESYSTEM'
     AND column_name = 'COMMENTS';
  IF length(inComments) > maxCommentLen THEN
    raise_application_error
      (-20020, 'Comment exceeds maximum length of '|| maxCommentLen ||' bytes');
  END IF;
  -- Check that the nodename and mountpoint input options are valid.
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
            (-20101, 'Mountpoint: '||mntPnt||' belongs to multiple service classes, please specify which service class to use, using the --svcclass option');
        WHEN NO_DATA_FOUND THEN
          raise_application_error
            (-20120, 'Mountpoint: '||mntPnt||' does not belong to any service class');
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
          (-20117, 'Mountpoint: '||mntPnt||' does not belong to the '''||inSvcClass||''' service class');
      END;
    END IF;
    -- Check to see if the mountpoint and diskserver combination allow for a
    -- draining operation to begin.
    BEGIN
      SELECT FS.diskPool INTO unused
        FROM FileSystem FS, DiskServer DS
       WHERE FS.diskServer = DS.id
         AND FS.id = fsIds(i)
         AND (FS.status = 1 OR DS.status = 1)
         AND FS.status != 2
         AND DS.status != 2;
    EXCEPTION WHEN NO_DATA_FOUND THEN
      raise_application_error
        (-20116, 'Mountpoint: '||mntPnt||' is not in a DRAINING state or diskserver is DISABLED');
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
      removeFailedDrainingTransfers(fsIds(i));
      DELETE FROM DrainingFileSystem
       WHERE fileSystem = fsIds(i);
      -- Insert the new mountpoint into the list of those to be drained. The
      -- drainManager job will later pick it up and start the draining process.
      INSERT INTO DrainingFileSystem
        (userName, machine, creationTime, fileSystem, svcClass, autoDelete,
         fileMask, maxTransfers, comments)
      VALUES
        (-- For the time being the draindiskserver command is distributed with
         -- the castor-dbtools package and uses the /etc/castor/ORASTAGERCONFIG
         -- password to connect to the database. As the file is only readable
         -- by the root user and the st group, the OS_USER will either be root
         -- or stage. This is not very interesting!!
         sys_context('USERENV', 'OS_USER'),
         sys_context('USERENV', 'HOST'),
         getTime(), fsIds(i), svcId, inAutoDelete, inFileMask, inMaxTransfers,
         inComments);
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
  fileMask   NUMBER;
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
  RETURNING svcclass, autoDelete, fileMask INTO svcId, autoDelete, fileMask;
  IF svcId = 0 THEN
    RETURN;  -- Do nothing
  END IF;
  -- Extract the next diskcopy to be processed.
  dcId := 0;
  UPDATE DrainingDiskCopy
     SET status = 2  -- PROCESSING
   WHERE diskCopy = (
     SELECT diskCopy FROM (
       SELECT DDC.diskCopy
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
    SELECT id INTO cfId FROM CastorFile
     WHERE id = cfId FOR UPDATE;
    -- Check that the status of the diskcopy matches what was specified in the
    -- filemask for the filesystem. If the diskcopy is not in the expected
    -- status then it is no longer a candidate to be replicated.
    SELECT ownerUid, ownerGid INTO ouid, ogid
      FROM DiskCopy
     WHERE id = dcId
       AND ((status = 0        AND fileMask = 0)    -- STAGED
        OR  (status = 10       AND fileMask = 1)    -- CANBEMIGR
        OR  (status IN (0, 10) AND fileMask = 2));  -- ALL
  EXCEPTION WHEN NO_DATA_FOUND THEN
    -- The diskcopy no longer exists or is not of interest so we delete it from
    -- the snapshot of files to be processed.
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
    SELECT /*+ INDEX(StageDiskCopyReplicaRequest I_StageDiskCopyReplic_DestDC) */ DiskCopy.id INTO res
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
  SELECT /*+ INDEX(R I_StageDiskCopyReplic_SourceDC) */ count(*) INTO res
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
  dcIds "numList";
BEGIN
  -- Delete (and restart if necessary) the filesystems which are:
  --  A) in a DELETING state and have no transfers pending in the scheduler.
  --  B) are COMPLETED and older than 7 days.
  FOR A IN (SELECT fileSystem, status FROM DrainingFileSystem
             WHERE (NOT EXISTS
               (SELECT 'x' FROM DrainingDiskCopy
                 WHERE fileSystem = DrainingFileSystem.fileSystem
                   AND status = 3)  -- WAITD2D
               AND status IN (6, 7))  -- DELETING, RESTART
                OR (status = 5 AND lastUpdateTime < getTime() - (7 * 86400)))
  LOOP
    -- If the status is RESTART, reset the draining filesystem entry to
    -- its default values and set its status to CREATED, otherwise delete it!
    removeFailedDrainingTransfers(a.fileSystem);
    IF a.status = 7 THEN
      UPDATE DrainingFileSystem
         SET creationTime = getTime(), startTime = 0, lastUpdateTime = 0,
             status = 0, totalFiles = 0, totalBytes = 0
       WHERE fileSystem = a.fileSystem;        
    ELSE
      DELETE FROM DrainingFileSystem
       WHERE fileSystem = a.fileSystem;
    END IF;
  END LOOP;
  -- If the draining filesystem is in a DELETING state and there are still
  -- running transfers for which the diskcopy they were waiting on has
  -- disappeared delete them. SR #112993
  SELECT DDC.diskCopy BULK COLLECT INTO dcIds
    FROM DrainingDiskCopy DDC, DrainingFileSystem DFS
   WHERE DDC.fileSystem = DFS.fileSystem
     AND DDC.status = 3  -- WAITD2D
     AND DFS.status = 6  -- DELETING
     AND NOT EXISTS
       (SELECT 'x' FROM DiskCopy WHERE id = DDC.diskCopy);
  DELETE FROM DrainingDiskCopy
   WHERE status = 3
     AND diskCopy IN
       (SELECT /*+ CARDINALITY(dcIdTable 5) */ *
          FROM TABLE (dcIds) dcIdTable);
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
  -- Regenerate the statistics for the DrainingDiskCopy table
  DBMS_STATS.GATHER_TABLE_STATS
    (OWNNAME          => sys_context('USERENV', 'CURRENT_SCHEMA'),
     TABNAME          =>'DRAININGDISKCOPY',
     ESTIMATE_PERCENT => 100,
     METHOD_OPT       => 'FOR ALL COLUMNS SIZE 100',
     NO_INVALIDATE    => FALSE,
     FORCE            => TRUE);
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
    diskCopySize NUMBER,
    castorFileSize NUMBER,
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
                   getTimeString(DiskCopy.creationtime) AS creationtime,
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
                    (SELECT /*+ INDEX(StageGetRequest PK_StageGetRequest_Id) */ id, username, machine, svcClassName, 'Get' AS type FROM StageGetRequest UNION ALL
                     SELECT /*+ INDEX(StagePrepareToGetRequest PK_StagePrepareToGetRequest_Id) */ id, username, machine, svcClassName, 'PGet' AS type FROM StagePrepareToGetRequest UNION ALL
                     SELECT /*+ INDEX(StagePutRequest PK_StagePutRequest_Id) */ id, username, machine, svcClassName, 'Put' AS type FROM StagePutRequest UNION ALL
                     SELECT /*+ INDEX(StagePrepareToPutRequest PK_StagePrepareToPutRequest_Id) */ id, username, machine, svcClassName, 'PPut' AS type FROM StagePrepareToPutRequest UNION ALL
                     SELECT /*+ INDEX(StageUpdateRequest PK_StageUpdateRequest_Id) */ id, username, machine, svcClassName, 'Upd' AS type FROM StageUpdateRequest UNION ALL
                     SELECT /*+ INDEX(StagePrepareToUpdateRequest PK_StagePrepareToUpdateRequ_Id) */ id, username, machine, svcClassName, 'PUpd' AS type FROM StagePrepareToUpdateRequest UNION ALL
                     SELECT /*+ INDEX(StageRepackRequest PK_StageRepackRequest_Id) */ id, username, machine, svcClassName, 'Repack' AS type FROM StageRepackRequest UNION ALL
                     SELECT /*+ INDEX(StagePutDoneRequest PK_StagePutDoneRequest_Id) */ id, username, machine, svcClassName, 'PutDone' AS type FROM StagePutDoneRequest UNION ALL
                     SELECT /*+ INDEX(StageDiskCopyReplicaRequest PK_StageDiskCopyReplicaRequ_Id) */ id, username, machine, svcClassName, 'DCRepl' AS type FROM StageDiskCopyReplicaRequest UNION ALL
                     SELECT /*+ INDEX(SetFileGCWeight PK_SetFileGCWeight_Id) */ id, username, machine, svcClassName, 'SetFileGCWeight' AS type FROM SetFileGCWeight) Request
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
          SELECT svcClass, status, waitTime, diskCopySize, found FROM (
            SELECT /*+ USE_NL(TapeCopy DiskCopy CastorFile) */
                   SvcClass.name svcClass,
                   decode(sign(TapeCopy.status - 2), -1, 'PENDING',
                     decode(TapeCopy.status, 6, 'FAILED', 'SELECTED')) status,
                   (getTime() - DiskCopy.creationTime) waitTime,
                   DiskCopy.diskCopySize, 1 found, RANK() OVER (PARTITION BY
                   DiskCopy.castorFile ORDER BY DiskCopy.id ASC) rank
              FROM DiskCopy, CastorFile, TapeCopy, SvcClass
             WHERE DiskCopy.castorFile = CastorFile.id
               AND CastorFile.id = TapeCopy.castorFile
               AND CastorFile.svcClass = SvcClass.id
               AND decode(DiskCopy.status, 10, DiskCopy.status, NULL) = 10  -- CANBEMIGR
               AND TapeCopy.status IN (0, 1, 2, 3, 6)) -- CREATED, TOBEMIGRATED, WAITINSTREAMS,
                                                       -- SELECTED, FAILED
           WHERE rank = 1
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

/* Recompile all invalid procedures, triggers and functions */
/************************************************************/
BEGIN
  FOR a IN (SELECT object_name, object_type
              FROM user_objects
             WHERE object_type IN ('PROCEDURE', 'TRIGGER', 'FUNCTION')
               AND status = 'INVALID')
  LOOP
    IF a.object_type = 'PROCEDURE' THEN
      EXECUTE IMMEDIATE 'ALTER PROCEDURE '||a.object_name||' COMPILE';
    ELSIF a.object_type = 'TRIGGER' THEN
      EXECUTE IMMEDIATE 'ALTER TRIGGER '||a.object_name||' COMPILE';
    ELSE
      EXECUTE IMMEDIATE 'ALTER FUNCTION '||a.object_name||' COMPILE';
    END IF;
  END LOOP;
END;
/

/* Flag the schema upgrade as COMPLETE */
/***************************************/
UPDATE UpgradeLog SET endDate = sysdate, state = 'COMPLETE'
 WHERE release = '2_1_11_0';
COMMIT;
