--liquibase formatted sql

--changeset mvelosob:1 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
--precondition-sql-check expectedResult:"0.0" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
--!!!THIS FALSE PRECONDITION IS HERE TO BLOCK AN UPGRADE WHILE THE DEVELOPMENT OF THE NEW CATALOGUE VERSION IS BEING DEVELOPED!!!
UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=5;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=0;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';

--changeset mvelosob:2 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
DROP TABLE ACTIVITIES_WEIGHTS;
--rollback CREATE TABLE ACTIVITIES_WEIGHTS (
--rollback   DISK_INSTANCE_NAME       VARCHAR2(100),
--rollback   ACTIVITY                 VARCHAR2(100),
--rollback   WEIGHT                   VARCHAR2(100),
--rollback   USER_COMMENT             VARCHAR2(1000)   CONSTRAINT ACTIV_WEIGHTS_UC_NN   NOT NULL,
--rollback   CREATION_LOG_USER_NAME   VARCHAR2(100)    CONSTRAINT ACTIV_WEIGHTS_CLUN_NN NOT NULL,
--rollback   CREATION_LOG_HOST_NAME   VARCHAR2(100)    CONSTRAINT ACTIV_WEIGHTS_CLHN_NN NOT NULL,
--rollback   CREATION_LOG_TIME        NUMERIC(20, 0)      CONSTRAINT ACTIV_WEIGHTS_CLT_NN  NOT NULL,
--rollback   LAST_UPDATE_USER_NAME    VARCHAR2(100)    CONSTRAINT ACTIV_WEIGHTS_LUUN_NN NOT NULL,
--rollback   LAST_UPDATE_HOST_NAME    VARCHAR2(100)    CONSTRAINT ACTIV_WEIGHTS_LUHN_NN NOT NULL,
--rollback   LAST_UPDATE_TIME         NUMERIC(20, 0)      CONSTRAINT ACTIV_WEIGHTS_LUT_NN  NOT NULL
--rollback );

--changeset mvelosob:3 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE DRIVE_STATE ADD (RESERVATION_SESSION_ID NUMERIC(20, 0));
UPDATE DRIVE_STATE SET RESERVATION_SESSION_ID=0;
--rollback ALTER TABLE DRIVE_STATE DROP COLUMN RESERVATION_SESSION_ID

--changeset mvelosob:4 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
DROP TABLE TAPE_DRIVE;
--rollback CREATE TABLE TAPE_DRIVE (
--rollback   DRIVE_NAME                  VARCHAR2(100)       CONSTRAINT DRIVE_STATE_DN_NN NOT NULL,
--rollback   HOST                        VARCHAR2(100)       CONSTRAINT DRIVE_STATE_H_NN  NOT NULL,
--rollback   LOGICAL_LIBRARY             VARCHAR2(100)       CONSTRAINT DRIVE_STATE_LL_NN NOT NULL,
--rollback   SESSION_ID                  NUMERIC(20, 0),
--rollback   BYTES_TRANSFERED_IN_SESSION NUMERIC(20, 0),
--rollback   FILES_TRANSFERED_IN_SESSION NUMERIC(20, 0),
--rollback   LATEST_BANDWIDTH            VARCHAR2(100),
--rollback   SESSION_START_TIME          NUMERIC(20, 0),
--rollback   MOUNT_START_TIME            NUMERIC(20, 0),
--rollback   TRANSFER_START_TIME         NUMERIC(20, 0),
--rollback   UNLOAD_START_TIME           NUMERIC(20, 0),
--rollback   UNMOUNT_START_TIME          NUMERIC(20, 0),
--rollback   DRAINING_START_TIME         NUMERIC(20, 0),
--rollback   DOWN_OR_UP_START_TIME       NUMERIC(20, 0),
--rollback   PROBE_START_TIME            NUMERIC(20, 0),
--rollback   CLEANUP_START_TIME          NUMERIC(20, 0),
--rollback   START_START_TIME            NUMERIC(20, 0),
--rollback   SHUTDOWN_TIME               NUMERIC(20, 0),
--rollback   MOUNT_TYPE                  NUMERIC(10, 0)      CONSTRAINT DRIVE_STATE_MT_NN NOT NULL,
--rollback   DRIVE_STATUS                VARCHAR2(100)    DEFAULT 'UNKNOWN' CONSTRAINT DRIVE_STATE_DS_NN NOT NULL,
--rollback   DESIRED_UP                  CHAR(1)         DEFAULT '0' CONSTRAINT DRIVE_STATE_DU_NN  NOT NULL,
--rollback   DESIRED_FORCE_DOWN          CHAR(1)         DEFAULT '0' CONSTRAINT DRIVE_STATE_DFD_NN NOT NULL,
--rollback   REASON_UP_DOWN              VARCHAR2(1000),
--rollback   CURRENT_VID                 VARCHAR2(100),
--rollback   CTA_VERSION                 VARCHAR2(100),
--rollback   CURRENT_PRIORITY            NUMERIC(20, 0),
--rollback   CURRENT_ACTIVITY            VARCHAR2(100),
--rollback   CURRENT_ACTIVITY_WEIGHT     VARCHAR2(100),
--rollback   CURRENT_TAPE_POOL           VARCHAR2(100),
--rollback   NEXT_MOUNT_TYPE             NUMERIC(10, 0),
--rollback   NEXT_VID                    VARCHAR2(100),
--rollback   NEXT_TAPE_POOL              VARCHAR2(100),
--rollback   NEXT_PRIORITY               NUMERIC(20, 0),
--rollback   NEXT_ACTIVITY               VARCHAR2(100),
--rollback   NEXT_ACTIVITY_WEIGHT        VARCHAR2(100),
--rollback   DEV_FILE_NAME               VARCHAR2(100),
--rollback   RAW_LIBRARY_SLOT            VARCHAR2(100),
--rollback   CURRENT_VO                  VARCHAR2(100),
--rollback   NEXT_VO                     VARCHAR2(100),
--rollback   USER_COMMENT                VARCHAR2(1000),
--rollback   CREATION_LOG_USER_NAME      VARCHAR2(100),
--rollback   CREATION_LOG_HOST_NAME      VARCHAR2(100),
--rollback   CREATION_LOG_TIME           NUMERIC(20, 0),
--rollback   LAST_UPDATE_USER_NAME       VARCHAR2(100),
--rollback   LAST_UPDATE_HOST_NAME       VARCHAR2(100),
--rollback   LAST_UPDATE_TIME            NUMERIC(20, 0),
--rollback   DISK_SYSTEM_NAME            VARCHAR2(100)       CONSTRAINT DRIVE_STATE_DSN_NN NOT NULL,
--rollback   RESERVED_BYTES              NUMERIC(20, 0)         CONSTRAINT DRIVE_STATE_RB_NN  NOT NULL,
--rollback   CONSTRAINT DRIVE_STATE_DN_PK PRIMARY KEY(DRIVE_NAME),
--rollback   CONSTRAINT DRIVE_STATE_DU_BOOL_CK CHECK(DESIRED_UP IN ('0', '1')),
--rollback   CONSTRAINT DRIVE_STATE_DFD_BOOL_CK CHECK(DESIRED_FORCE_DOWN IN ('0', '1')),
--rollback   CONSTRAINT DRIVE_STATE_DS_STRING_CK CHECK(DRIVE_STATUS IN ('DOWN', 'UP', 'PROBING', 'STARTING',
--rollback   'MOUNTING', 'TRANSFERING', 'UNLOADING', 'UNMOUNTING', 'DRAININGTODISK', 'CLEANINGUP', 'SHUTDOWN',
--rollback   'UNKNOWN'))
--rollback );

--changeset mvelosob:5 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE DRIVE_STATE DROP CONSTRAINT DRIVE_DSN_NN;
ALTER TABLE DRIVE_STATE DROP CONSTRAINT DRIVE_RB_NN;
--rollback ALTER TABLE DRIVE_STATE MODIFY DISK_SYSTEM_NAME VARCHAR2(100) CONSTRAINT DRIVE_STATE_DSN_NN NOT NULL;
--rollback ALTER TABLE DRIVE_STATE MODIFY RESERVED_BYTES NUMERIC(20, 0) CONSTRAINT DRIVE_STATE_RB_NN NOT NULL;

--changeset mvelosob:6 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE DISK_SYSTEM DROP CONSTRAINT DISK_SYSTEM_FSQU_NN;
ALTER TABLE DISK_SYSTEM DROP CONSTRAINT DISK_SYSTEM_RI_NN;
ALTER TABLE DISK_SYSTEM MODIFY DISK_INSTANCE_NAME CONSTRAINT DISK_SYSTEM_DINM_NN NOT NULL;
ALTER TABLE DISK_SYSTEM MODIFY DISK_INSTANCE_SPACE_NAME CONSTRAINT DISK_SYSTEM_DISNM_NN NOT NULL;
ALTER TABLE DISK_SYSTEM ADD CONSTRAINT DISK_SYSTEM_DIN_DISN_FK FOREIGN KEY (DISK_INSTANCE_NAME, DISK_INSTANCE_SPACE_NAME) REFERENCES DISK_INSTANCE_SPACE (DISK_INSTANCE_NAME, DISK_INSTANCE_SPACE_NAME);
--rollback ALTER TABLE DISK_SYSTEM MODIFY FREE_SPACE_QUERY_URL CONSTRAINT DISK_SYSTEM_FSQU_NN NOT NULL;
--rollback ALTER TABLE DISK_SYSTEM MODIFY REFRESH_INTERVAL CONSTRAINT DISK_SYSTEM_RI_NN NOT NULL;
--rollback ALTER TABLE DISK_SYSTEM DROP CONSTRAINT DISK_SYSTEM_DIN_DISN_FK;
--rollback ALTER TABLE DISK_SYSTEM DROP CONSTRAINT DISK_SYSTEM_DINM_NN;
--rollback ALTER TABLE DISK_SYSTEM DROP CONSTRAINT DISK_SYSTEM_DISNM_NN;

--changeset mvelosob:7 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE VIRTUAL_ORGANIZATION ADD (DISK_INSTANCE_NAME VARCHAR2(100));
UPDATE VIRTUAL_ORGANIZATION SET DISK_INSTANCE_NAME=NULL;
--rollback ALTER TABLE VIRTUAL_ORGANIZATION DROP COLUMN DISK_INSTANCE_NAME


--changeset mvelosob:8 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.6" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=5;
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=0;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=4;
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=6;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=5;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=0;
