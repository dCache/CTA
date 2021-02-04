--liquibase formatted sql

--changeset ccaffy:1 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=4;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=0;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';

--changeset ccaffy:2 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE_FILE DROP CONSTRAINT TAPE_FILE_SS_VID_FSEQ_FK;
--rollback ALTER TABLE TAPE_FILE ADD CONSTRAINT TAPE_FILE_SS_VID_FSEQ_FK FOREIGN KEY(SUPERSEDED_BY_VID, SUPERSEDED_BY_FSEQ) REFERENCES TAPE_FILE(VID, FSEQ);

--changeset ccaffy:3 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
DROP INDEX  TAPE_FILE_SBV_SBF_IDX;
--rollback CREATE INDEX TAPE_FILE_SBV_SBF_IDX ON TAPE_FILE(SUPERSEDED_BY_VID, SUPERSEDED_BY_FSEQ);

--changeset ccaffy:4 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE_FILE DROP COLUMN SUPERSEDED_BY_VID;
ALTER TABLE TAPE_FILE DROP COLUMN SUPERSEDED_BY_FSEQ;
ALTER TABLE TAPE_FILE DROP COLUMN WRITE_START_WRAP;
ALTER TABLE TAPE_FILE DROP COLUMN WRITE_START_LPOS;
ALTER TABLE TAPE_FILE DROP COLUMN WRITE_END_WRAP;
ALTER TABLE TAPE_FILE DROP COLUMN WRITE_END_LPOS;
ALTER TABLE TAPE_FILE DROP COLUMN READ_START_WRAP;
ALTER TABLE TAPE_FILE DROP COLUMN READ_START_LPOS;
ALTER TABLE TAPE_FILE DROP COLUMN READ_END_WRAP;
ALTER TABLE TAPE_FILE DROP COLUMN READ_END_LPOS;
--rollback  ALTER TABLE TAPE_FILE ADD (
--rollback    SUPERSEDED_BY_VID VARCHAR2(100),
--rollback    SUPERSEDED_BY_FSEQ NUMERIC(20, 0),
--rollback    WRITE_START_WRAP NUMERIC(5, 0),
--rollback    WRITE_START_LPOS NUMERIC(10, 0),
--rollback    WRITE_END_WRAP NUMERIC(5, 0),
--rollback    WRITE_END_LPOS NUMERIC(10, 0),
--rollback    READ_START_WRAP NUMERIC(5, 0),
--rollback    READ_START_LPOS NUMERIC(10, 0),
--rollback    READ_END_WRAP NUMERIC(5, 0),
--rollback    READ_END_LPOS NUMERIC(10, 0)
--rollback  );

--changeset ccaffy:5 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
DROP TABLE TAPE_FILE_RECYCLE_BIN;
--rollback CREATE TABLE TAPE_FILE_RECYCLE_BIN(
--rollback   VID                      VARCHAR2(100)   CONSTRAINT TAPE_FILE_RB_V_NN    NOT NULL,
--rollback   FSEQ                     NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_RB_F_NN    NOT NULL,
--rollback   BLOCK_ID                 NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_RB_BI_NN   NOT NULL,
--rollback   LOGICAL_SIZE_IN_BYTES    NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_RB_CSIB_NN NOT NULL,
--rollback   COPY_NB                  NUMERIC(3, 0)      CONSTRAINT TAPE_FILE_RB_CN_NN   NOT NULL,
--rollback   CREATION_TIME            NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_RB_CT_NN   NOT NULL,
--rollback   ARCHIVE_FILE_ID          NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_RB_AFI_NN  NOT NULL,
--rollback   SUPERSEDED_BY_VID        VARCHAR2(100),
--rollback   SUPERSEDED_BY_FSEQ       NUMERIC(20, 0),
--rollback   WRITE_START_WRAP         NUMERIC(5, 0),
--rollback   WRITE_START_LPOS         NUMERIC(10, 0),
--rollback   WRITE_END_WRAP           NUMERIC(5, 0),
--rollback   WRITE_END_LPOS           NUMERIC(10, 0),
--rollback   READ_START_WRAP          NUMERIC(5, 0),
--rollback   READ_START_LPOS          NUMERIC(10, 0),
--rollback   READ_END_WRAP            NUMERIC(5, 0),
--rollback   READ_END_LPOS            NUMERIC(10, 0),
--rollback   CONSTRAINT TAPE_FILE_RB_PK PRIMARY KEY(VID, FSEQ),
--rollback   CONSTRAINT TAPE_FILE_RB_TAPE_FK FOREIGN KEY(VID)
--rollback     REFERENCES TAPE(VID),
--rollback   CONSTRAINT TAPE_FILE_RB_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
--rollback     REFERENCES ARCHIVE_FILE_RECYCLE_BIN(ARCHIVE_FILE_ID),
--rollback   CONSTRAINT TAPE_FILE_RB_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
--rollback   CONSTRAINT TAPE_FILE_RB_COPY_NB_GT_0_CK CHECK(COPY_NB > 0),
--rollback   CONSTRAINT TAPE_FILE_RB_SS_VID_FSEQ_FK FOREIGN KEY(SUPERSEDED_BY_VID, SUPERSEDED_BY_FSEQ)
--rollback     REFERENCES TAPE_FILE_RECYCLE_BIN(VID, FSEQ)
--rollback );

--changeset ccaffy:6 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
DROP TABLE ARCHIVE_FILE_RECYCLE_BIN;
--rollback CREATE TABLE ARCHIVE_FILE_RECYCLE_BIN(
--rollback   ARCHIVE_FILE_ID         NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_RB_AFI_NN  NOT NULL,
--rollback   DISK_INSTANCE_NAME      VARCHAR2(100)    CONSTRAINT ARCHIVE_FILE_RB_DIN_NN  NOT NULL,
--rollback   DISK_FILE_ID            VARCHAR2(100)    CONSTRAINT ARCHIVE_FILE_RB_DFI_NN  NOT NULL,
--rollback   DISK_FILE_ID_WHEN_DELETED VARCHAR2(100)  CONSTRAINT ARCHIVE_FILE_RB_DFIWD_NN NOT NULL,
--rollback   DISK_FILE_UID           NUMERIC(10, 0)      CONSTRAINT ARCHIVE_FILE_RB_DFUID_NN  NOT NULL,
--rollback   DISK_FILE_GID           NUMERIC(10, 0)      CONSTRAINT ARCHIVE_FILE_RB_DFGID_NN  NOT NULL,
--rollback   SIZE_IN_BYTES           NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_RB_SIB_NN  NOT NULL,
--rollback   CHECKSUM_BLOB           RAW(200),
--rollback   CHECKSUM_ADLER32        NUMERIC(10, 0)      CONSTRAINT ARCHIVE_FILE_RB_CB2_NN  NOT NULL,
--rollback   STORAGE_CLASS_ID        NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_RB_SCI_NN  NOT NULL,
--rollback   CREATION_TIME           NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_RB_CT2_NN  NOT NULL,
--rollback   RECONCILIATION_TIME     NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_RB_RT_NN   NOT NULL,
--rollback   COLLOCATION_HINT        VARCHAR2(100),
--rollback   DISK_FILE_PATH          VARCHAR2(2000) CONSTRAINT ARCHIVE_FILE_RB_DFP_NN NOT NULL,
--rollback   DELETION_TIME           NUMERIC(20, 0)    CONSTRAINT ARCHIVE_FILE_RB_DT_NN NOT NULL,
--rollback   CONSTRAINT ARCHIVE_FILE_RB_PK PRIMARY KEY(ARCHIVE_FILE_ID),
--rollback   CONSTRAINT ARCHIVE_FILE_RB_SC_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
--rollback   CONSTRAINT ARCHIVE_FILE_RB_DIN_DFI_UN UNIQUE(DISK_INSTANCE_NAME, DISK_FILE_ID)
--rollback );

--changeset ccaffy:7 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE MODIFY TAPE_STATE VARCHAR2(100) CONSTRAINT TAPE_TS_NN NOT NULL;
--rollback ALTER TABLE TAPE DROP CONSTRAINT TAPE_TS_NN;

--changeset ccaffy:8 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE MODIFY STATE_UPDATE_TIME NUMERIC(20,0) CONSTRAINT TAPE_SUT_NN NOT NULL;
--rollback ALTER TABLE TAPE DROP CONSTRAINT TAPE_SUT_NN;

--changeset ccaffy:9 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE MODIFY STATE_MODIFIED_BY VARCHAR2(100) CONSTRAINT TAPE_SMB_NN NOT NULL;
--rollback ALTER TABLE TAPE DROP CONSTRAINT TAPE_SMB_NN;

--changeset ccaffy:10 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE DROP CONSTRAINT TAPE_IS_DISABLED_BOOL_CK;
--rollback ALTER TABLE TAPE ADD CONSTRAINT TAPE_IS_DISABLED_BOOL_CK CHECK(IS_DISABLED IN ('0', '1'));

--changeset ccaffy:11 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
DROP INDEX TAPE_IS_DISABLED_IDX;
--rollback CREATE INDEX TAPE_IS_DISABLED_IDX ON TAPE(IS_DISABLED);

--changeset ccaffy:12 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE DROP COLUMN IS_DISABLED;

--changeset ccaffy:13 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE DROP CONSTRAINT TAPE_IS_READ_ONLY_BOOL_CK;

--changeset ccaffy:14 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE TAPE DROP COLUMN IS_READ_ONLY;

--changeset ccaffy:15 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE VIRTUAL_ORGANIZATION MODIFY READ_MAX_DRIVES NUMERIC(20,0) CONSTRAINT VIRTUAL_ORGANIZATION_RMD_NN NOT NULL;

--changeset ccaffy:16 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE VIRTUAL_ORGANIZATION MODIFY WRITE_MAX_DRIVES NUMERIC(20,0) CONSTRAINT VIRTUAL_ORGANIZATION_WMD_NN NOT NULL;

--changeset ccaffy:17 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
ALTER TABLE MOUNT_POLICY DROP COLUMN MAX_DRIVES_ALLOWED;

--changeset ccaffy:18 failOnError:true dbms:oracle
--preconditions onFail:HALT onError:HALT
--precondition-sql-check expectedResult:"3.2" SELECT CONCAT(CONCAT(CAST(SCHEMA_VERSION_MAJOR as VARCHAR(10)),'.'), CAST(SCHEMA_VERSION_MINOR AS VARCHAR(10))) AS CATALOGUE_VERSION FROM CTA_CATALOGUE;
UPDATE CTA_CATALOGUE SET STATUS='PRODUCTION';
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=4;
UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=0;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=NULL;
UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=NULL;
--rollback UPDATE CTA_CATALOGUE SET STATUS='UPGRADING';
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MAJOR=3;
--rollback UPDATE CTA_CATALOGUE SET SCHEMA_VERSION_MINOR=2;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MAJOR=4;
--rollback UPDATE CTA_CATALOGUE SET NEXT_SCHEMA_VERSION_MINOR=0;