--liquibase formatted sql

--changeset jocamare:1 failOnError:true dbms:postgresql
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.0" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=4;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=1;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';

--changeset jocamare:2 failOnError:true dbms:postgresql
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.0" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
CREATE TABLE DRIVE_CONFIG (
	  DRIVE_NAME                  VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_DN_NN  NOT NULL,
	  CATEGORY                    VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_C_NN   NOT NULL,
	  KEY_NAME                    VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_KN_NN  NOT NULL,
	  VALUE                       VARCHAR(1000)      CONSTRAINT DRIVE_CONFIG_V_NN   NOT NULL,
	  SOURCE                      VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_S_NN   NOT NULL,
	  CONSTRAINT DRIVE_CONFIG_DN_PK PRIMARY KEY(KEY_NAME, DRIVE_NAME)
);
--rollback DROP DRIVE_CONFIG;

--changeset jocamare:3 failOnError:true dbms:postgresql
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.0" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
CREATE TABLE TAPE_DRIVE (
	  DRIVE_NAME                  VARCHAR(100)       CONSTRAINT DRIVE_STATE_DN_NN NOT NULL,
	  HOST                        VARCHAR(100)       CONSTRAINT DRIVE_STATE_H_NN  NOT NULL,
	  LOGICAL_LIBRARY             VARCHAR(100)       CONSTRAINT DRIVE_STATE_LL_NN NOT NULL,
	  SESSION_ID                  NUMERIC(20, 0),
	  BYTES_TRANSFERED_IN_SESSION NUMERIC(20, 0),
	  FILES_TRANSFERED_IN_SESSION NUMERIC(20, 0),
	  LATEST_BANDWIDTH            VARCHAR(100),
	  SESSION_START_TIME          NUMERIC(20, 0),
	  MOUNT_START_TIME            NUMERIC(20, 0),
	  TRANSFER_START_TIME         NUMERIC(20, 0),
	  UNLOAD_START_TIME           NUMERIC(20, 0),
	  UNMOUNT_START_TIME          NUMERIC(20, 0),
	  DRAINING_START_TIME         NUMERIC(20, 0),
	  DOWN_OR_UP_START_TIME       NUMERIC(20, 0),
	  PROBE_START_TIME            NUMERIC(20, 0),
	  CLEANUP_START_TIME          NUMERIC(20, 0),
	  START_START_TIME            NUMERIC(20, 0),
	  SHUTDOWN_TIME               NUMERIC(20, 0),
	  MOUNT_TYPE                  NUMERIC(10, 0)      CONSTRAINT DRIVE_STATE_MT_NN NOT NULL,
	  DRIVE_STATUS                VARCHAR(100)    DEFAULT 'UNKNOWN' CONSTRAINT DRIVE_STATE_DS_NN NOT NULL,
	  DESIRED_UP                  CHAR(1)         DEFAULT '0' CONSTRAINT DRIVE_STATE_DU_NN  NOT NULL,
	  DESIRED_FORCE_DOWN          CHAR(1)         DEFAULT '0' CONSTRAINT DRIVE_STATE_DFD_NN NOT NULL,
	  REASON_UP_DOWN              VARCHAR(1000),
	  CURRENT_VID                 VARCHAR(100),
	  CTA_VERSION                 VARCHAR(100),
	  CURRENT_PRIORITY            NUMERIC(20, 0),
	  CURRENT_ACTIVITY            VARCHAR(100),
	  CURRENT_ACTIVITY_WEIGHT     VARCHAR(100),
	  CURRENT_TAPE_POOL           VARCHAR(100),
	  NEXT_MOUNT_TYPE             NUMERIC(10, 0),
	  NEXT_VID                    VARCHAR(100),
	  NEXT_TAPE_POOL              VARCHAR(100),
	  NEXT_PRIORITY               NUMERIC(20, 0),
	  NEXT_ACTIVITY               VARCHAR(100),
	  NEXT_ACTIVITY_WEIGHT        VARCHAR(100),
	  DEV_FILE_NAME               VARCHAR(100),
	  RAW_LIBRARY_SLOT            VARCHAR(100),
	  CURRENT_VO                  VARCHAR(100),
	  NEXT_VO                     VARCHAR(100),
	  USER_COMMENT                VARCHAR(1000),
	  CREATION_LOG_USER_NAME      VARCHAR(100),
	  CREATION_LOG_HOST_NAME      VARCHAR(100),
	  CREATION_LOG_TIME           NUMERIC(20, 0),
	  LAST_UPDATE_USER_NAME       VARCHAR(100),
	  LAST_UPDATE_HOST_NAME       VARCHAR(100),
	  LAST_UPDATE_TIME            NUMERIC(20, 0),
	  DISK_SYSTEM_NAME            VARCHAR(100)       CONSTRAINT DRIVE_STATE_DSN_NN NOT NULL,
	  RESERVED_BYTES              NUMERIC(20, 0)         CONSTRAINT DRIVE_STATE_RB_NN  NOT NULL,
	  CONSTRAINT DRIVE_STATE_DN_PK PRIMARY KEY(DRIVE_NAME),
	  CONSTRAINT DRIVE_STATE_DU_BOOL_CK CHECK(DESIRED_UP IN ('0', '1')),
	  CONSTRAINT DRIVE_STATE_DFD_BOOL_CK CHECK(DESIRED_FORCE_DOWN IN ('0', '1')),
	  CONSTRAINT DRIVE_STATE_DS_STRING_CK CHECK(DRIVE_STATUS IN ('DOWN', 'UP', 'PROBING', 'STARTING',
			  'MOUNTING', 'TRANSFERING', 'UNLOADING', 'UNMOUNTING', 'DRAININGTODISK', 'CLEANINGUP', 'SHUTDOWN',
			  'UNKNOWN'))
);
--rollback DROP TAPE_DRIVE;

--changeset jocamare:4 failOnError:true dbms:postgresql
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"4.0" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=4;
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=1;
UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';
--rollback UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=4;
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=0;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=4;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=1;