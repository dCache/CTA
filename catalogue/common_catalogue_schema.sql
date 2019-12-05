CREATE TABLE CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR    UINT64TYPE      CONSTRAINT CTA_CATALOGUE_SVM1_NN NOT NULL,
  SCHEMA_VERSION_MINOR    UINT64TYPE      CONSTRAINT CTA_CATALOGUE_SVM2_NN NOT NULL
);
CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME         VARCHAR(100)    CONSTRAINT ADMIN_USER_AUN_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ADMIN_USER_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT ADMIN_USER_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT ADMIN_USER_LUT_NN  NOT NULL,
  CONSTRAINT ADMIN_USER_PK PRIMARY KEY(ADMIN_USER_NAME)
);
CREATE TABLE DISK_SYSTEM(
  DISK_SYSTEM_NAME        VARCHAR(100)    CONSTRAINT DISK_SYSTEM_DSNM_NN NOT NULL,
  FILE_REGEXP             VARCHAR(100)    CONSTRAINT DISK_SYSTEM_FR_NN   NOT NULL,
  FREE_SPACE_QUERY_URL    VARCHAR(1000)   CONSTRAINT DISK_SYSTEM_FSQU_NN NOT NULL,
  REFRESH_INTERVAL        UINT64TYPE      CONSTRAINT DISK_SYSTEM_RI_NN   NOT NULL,
  TARGETED_FREE_SPACE     UINT64TYPE      CONSTRAINT DISK_SYSTEM_TFS_NN  NOT NULL,
  SLEEP_TIME              UINT64TYPE      CONSTRAINT DISK_SYSTEM_ST_NN   NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT DISK_SYSTEM_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT DISK_SYSTEM_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT DISK_SYSTEM_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT DISK_SYSTEM_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT DISK_SYSTEM_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT DISK_SYSTEM_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT DISK_SYSTEM_LUT_NN  NOT NULL,
  CONSTRAINT NAME_PK PRIMARY KEY(DISK_SYSTEM_NAME)
);
CREATE TABLE STORAGE_CLASS(
  STORAGE_CLASS_ID        UINT64TYPE      CONSTRAINT STORAGE_CLASS_SCI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT STORAGE_CLASS_DIN_NN  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR(100)    CONSTRAINT STORAGE_CLASS_SCN_NN  NOT NULL,
  NB_COPIES               UINT8TYPE       CONSTRAINT STORAGE_CLASS_NC_NN   NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT STORAGE_CLASS_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT STORAGE_CLASS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT STORAGE_CLASS_LUT_NN  NOT NULL,
  CONSTRAINT STORAGE_CLASS_PK PRIMARY KEY(STORAGE_CLASS_ID),
  CONSTRAINT STORAGE_CLASS_DIN_SCN_UN UNIQUE(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME)
);
CREATE TABLE TAPE_POOL(
  TAPE_POOL_ID            UINT64TYPE      CONSTRAINT TAPE_POOL_TPI_NN  NOT NULL,
  TAPE_POOL_NAME          VARCHAR(100)    CONSTRAINT TAPE_POOL_TPN_NN  NOT NULL,
  VO                      VARCHAR(100)    CONSTRAINT TAPE_POOL_VO_NN   NOT NULL,
  NB_PARTIAL_TAPES        UINT64TYPE      CONSTRAINT TAPE_POOL_NPT_NN  NOT NULL,
  IS_ENCRYPTED            CHAR(1)         CONSTRAINT TAPE_POOL_IE_NN   NOT NULL,
  SUPPLY                  VARCHAR(100),
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT TAPE_POOL_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT TAPE_POOL_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT TAPE_POOL_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_POOL_PK PRIMARY KEY(TAPE_POOL_ID),
  CONSTRAINT TAPE_POOL_TPN_UN UNIQUE(TAPE_POOL_NAME),
  CONSTRAINT TAPE_POOL_IS_ENCRYPTED_BOOL_CK CHECK(IS_ENCRYPTED IN ('0', '1'))
);
CREATE TABLE ARCHIVE_ROUTE(
  STORAGE_CLASS_ID        UINT64TYPE      CONSTRAINT ARCHIVE_ROUTE_SCI_NN  NOT NULL,
  COPY_NB                 UINT8TYPE       CONSTRAINT ARCHIVE_ROUTE_CN_NN   NOT NULL,
  TAPE_POOL_ID            UINT64TYPE      CONSTRAINT ARCHIVE_ROUTE_TPI_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ARCHIVE_ROUTE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT ARCHIVE_ROUTE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT ARCHIVE_ROUTE_LUT_NN  NOT NULL,
  CONSTRAINT ARCHIVE_ROUTE_PK PRIMARY KEY(STORAGE_CLASS_ID, COPY_NB),
  CONSTRAINT ARCHIVE_ROUTE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_ROUTE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_ID) REFERENCES TAPE_POOL(TAPE_POOL_ID),
  CONSTRAINT ARCHIVE_ROUTE_COPY_NB_GT_ZERO CHECK(COPY_NB > 0)
);
CREATE TABLE LOGICAL_LIBRARY(
  LOGICAL_LIBRARY_ID      UINT64TYPE      CONSTRAINT LOGICAL_LIBRARY_LLI_NN  NOT NULL,
  LOGICAL_LIBRARY_NAME    VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LLN_NN  NOT NULL,
  IS_DISABLED             CHAR(1)         DEFAULT '0' CONSTRAINT LOGICAL_LIBRARY_ID_NN NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT LOGICAL_LIBRARY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT LOGICAL_LIBRARY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT LOGICAL_LIBRARY_LUT_NN  NOT NULL,
  CONSTRAINT LOGICAL_LIBRARY_PK PRIMARY KEY(LOGICAL_LIBRARY_ID),
  CONSTRAINT LOGICAL_LIBRARY_LLN UNIQUE(LOGICAL_LIBRARY_NAME),
  CONSTRAINT LOGICAL_LIBRARY_ID_BOOL_CK CHECK(IS_DISABLED IN ('0', '1'))
);
CREATE TABLE TAPE(
  VID                     VARCHAR(100)    CONSTRAINT TAPE_V_NN    NOT NULL,
  MEDIA_TYPE              VARCHAR(100)    CONSTRAINT TAPE_MT_NN   NOT NULL,
  VENDOR                  VARCHAR(100)    CONSTRAINT TAPE_V2_NN   NOT NULL,
  LOGICAL_LIBRARY_ID      UINT64TYPE      CONSTRAINT TAPE_LLI_NN  NOT NULL,
  TAPE_POOL_ID            UINT64TYPE      CONSTRAINT TAPE_TPI_NN  NOT NULL,
  ENCRYPTION_KEY_NAME     VARCHAR(100),
  CAPACITY_IN_BYTES       UINT64TYPE      CONSTRAINT TAPE_CIB_NN  NOT NULL,
  DATA_IN_BYTES           UINT64TYPE      CONSTRAINT TAPE_DIB_NN  NOT NULL,
  LAST_FSEQ               UINT64TYPE      CONSTRAINT TAPE_LF_NN   NOT NULL,
  IS_DISABLED             CHAR(1)         CONSTRAINT TAPE_ID_NN   NOT NULL,
  IS_FULL                 CHAR(1)         CONSTRAINT TAPE_IF_NN   NOT NULL,
  IS_READ_ONLY            CHAR(1)         CONSTRAINT TAPE_IRO_NN  NOT NULL,
  IS_FROM_CASTOR          CHAR(1)         CONSTRAINT TAPE_IFC_NN  NOT NULL,
  IS_ARCHIVED             CHAR(1)         DEFAULT '0' CONSTRAINT TAPE_IA_NN   NOT NULL,
  IS_EXPORTED             CHAR(1)         DEFAULT '0' CONSTRAINT TAPE_IE_NN   NOT NULL,
  LABEL_DRIVE             VARCHAR(100),
  LABEL_TIME              UINT64TYPE    ,
  LAST_READ_DRIVE         VARCHAR(100),
  LAST_READ_TIME          UINT64TYPE    ,
  LAST_WRITE_DRIVE        VARCHAR(100),
  LAST_WRITE_TIME         UINT64TYPE    ,
  READ_MOUNT_COUNT        UINT64TYPE      DEFAULT 0 CONSTRAINT TAPE_RMC_NN NOT NULL,
  WRITE_MOUNT_COUNT       UINT64TYPE      DEFAULT 0 CONSTRAINT TAPE_WMC_NN NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT TAPE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       UINT64TYPE      CONSTRAINT TAPE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        UINT64TYPE      CONSTRAINT TAPE_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_PK PRIMARY KEY(VID),
  CONSTRAINT TAPE_LOGICAL_LIBRARY_FK FOREIGN KEY(LOGICAL_LIBRARY_ID) REFERENCES LOGICAL_LIBRARY(LOGICAL_LIBRARY_ID),
  CONSTRAINT TAPE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_ID) REFERENCES TAPE_POOL(TAPE_POOL_ID),
  CONSTRAINT TAPE_IS_DISABLED_BOOL_CK CHECK(IS_DISABLED IN ('0', '1')),
  CONSTRAINT TAPE_IS_FULL_BOOL_CK CHECK(IS_FULL IN ('0', '1')),
  CONSTRAINT TAPE_IS_READ_ONLY_BOOL_CK CHECK(IS_READ_ONLY IN ('0', '1')),
  CONSTRAINT TAPE_IS_FROM_CASTOR_BOOL_CK CHECK(IS_FROM_CASTOR IN ('0', '1')),
  CONSTRAINT TAPE_IS_ARCHVIED_BOOL_CK CHECK(IS_ARCHIVED IN ('0', '1')),
  CONSTRAINT TAPE_IS_EXPORTED_BOOL_CK CHECK(IS_EXPORTED IN ('0', '1'))
);
CREATE INDEX TAPE_TAPE_POOL_ID_IDX ON TAPE(TAPE_POOL_ID);
CREATE TABLE MOUNT_POLICY(
  MOUNT_POLICY_NAME        VARCHAR(100)    CONSTRAINT MOUNT_POLICY_MPN_NN  NOT NULL,
  ARCHIVE_PRIORITY         UINT64TYPE      CONSTRAINT MOUNT_POLICY_AP_NN   NOT NULL,
  ARCHIVE_MIN_REQUEST_AGE  UINT64TYPE      CONSTRAINT MOUNT_POLICY_AMRA_NN NOT NULL,
  RETRIEVE_PRIORITY        UINT64TYPE      CONSTRAINT MOUNT_POLICY_RP_NN   NOT NULL,
  RETRIEVE_MIN_REQUEST_AGE UINT64TYPE      CONSTRAINT MOUNT_POLICY_RMRA_NN NOT NULL,
  MAX_DRIVES_ALLOWED       UINT64TYPE      CONSTRAINT MOUNT_POLICY_MDA_NN  NOT NULL,
  USER_COMMENT             VARCHAR(1000)   CONSTRAINT MOUNT_POLICY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME   VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME   VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME        UINT64TYPE      CONSTRAINT MOUNT_POLICY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME         UINT64TYPE      CONSTRAINT MOUNT_POLICY_LUT_NN  NOT NULL,
  CONSTRAINT MOUNT_POLICY_PK PRIMARY KEY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR(100)    CONSTRAINT RQSTER_RULE_DIN_NN  NOT NULL,
  REQUESTER_NAME         VARCHAR(100)    CONSTRAINT RQSTER_RULE_RN_NN   NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR(100)    CONSTRAINT RQSTER_RULE_MPN_NN  NOT NULL,
  USER_COMMENT           VARCHAR(1000)   CONSTRAINT RQSTER_RULE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      UINT64TYPE      CONSTRAINT RQSTER_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       UINT64TYPE      CONSTRAINT RQSTER_RULE_LUT_NN  NOT NULL,
  CONSTRAINT RQSTER_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_NAME),
  CONSTRAINT RQSTER_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_GROUP_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_DIN_NN  NOT NULL,
  REQUESTER_GROUP_NAME   VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_RGN_NN  NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_MPN_NN  NOT NULL,
  USER_COMMENT           VARCHAR(1000)   CONSTRAINT RQSTER_GRP_RULE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      UINT64TYPE      CONSTRAINT RQSTER_GRP_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       UINT64TYPE      CONSTRAINT RQSTER_GRP_RULE_LUT_NN  NOT NULL,
  CONSTRAINT RQSTER_GRP_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_GROUP_NAME),
  CONSTRAINT RQSTER_GRP_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID         UINT64TYPE      CONSTRAINT ARCHIVE_FILE_AFI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DIN_NN  NOT NULL,
  DISK_FILE_ID            VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DFI_NN  NOT NULL,
  DISK_FILE_PATH          VARCHAR(2000)   CONSTRAINT ARCHIVE_FILE_DFP_NN  NOT NULL,
  DISK_FILE_UID           UINT64TYPE      CONSTRAINT ARCHIVE_FILE_DFUID_NN  NOT NULL,
  DISK_FILE_GID           UINT64TYPE      CONSTRAINT ARCHIVE_FILE_DFGID_NN  NOT NULL,
  SIZE_IN_BYTES           UINT64TYPE      CONSTRAINT ARCHIVE_FILE_SIB_NN  NOT NULL,
  CHECKSUM_BLOB           CHECKSUM_BLOB_TYPE,
  CHECKSUM_ADLER32        UINT32TYPE      CONSTRAINT ARCHIVE_FILE_CB2_NN  NOT NULL,
  STORAGE_CLASS_ID        UINT64TYPE      CONSTRAINT ARCHIVE_FILE_SCI_NN  NOT NULL,
  CREATION_TIME           UINT64TYPE      CONSTRAINT ARCHIVE_FILE_CT2_NN  NOT NULL,
  RECONCILIATION_TIME     UINT64TYPE      CONSTRAINT ARCHIVE_FILE_RT_NN   NOT NULL,
  IS_DELETED              CHAR(1)         DEFAULT '0' CONSTRAINT ARCHIVE_FILE_ID_NN NOT NULL,
  COLLOCATION_HINT        VARCHAR(100),
  CONSTRAINT ARCHIVE_FILE_PK PRIMARY KEY(ARCHIVE_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_FILE_DIN_DFI_UN UNIQUE(DISK_INSTANCE_NAME, DISK_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_ID_BOOL_CK CHECK(IS_DELETED IN ('0', '1'))
);
CREATE INDEX ARCHIVE_FILE_DIN_DFP_IDX ON ARCHIVE_FILE(DISK_INSTANCE_NAME, DISK_FILE_PATH);
CREATE TABLE TAPE_FILE(
  VID                      VARCHAR(100)   CONSTRAINT TAPE_FILE_V_NN    NOT NULL,
  FSEQ                     UINT64TYPE     CONSTRAINT TAPE_FILE_F_NN    NOT NULL,
  BLOCK_ID                 UINT64TYPE     CONSTRAINT TAPE_FILE_BI_NN   NOT NULL,
  LOGICAL_SIZE_IN_BYTES    UINT64TYPE     CONSTRAINT TAPE_FILE_CSIB_NN NOT NULL,
  COPY_NB                  UINT8TYPE      CONSTRAINT TAPE_FILE_CN_NN   NOT NULL,
  CREATION_TIME            UINT64TYPE     CONSTRAINT TAPE_FILE_CT_NN   NOT NULL,
  ARCHIVE_FILE_ID          UINT64TYPE     CONSTRAINT TAPE_FILE_AFI_NN  NOT NULL,
  SUPERSEDED_BY_VID        VARCHAR(100),
  SUPERSEDED_BY_FSEQ       UINT64TYPE    ,
  WRITE_START_WRAP         UINT16TYPE,
  WRITE_START_LPOS         UINT32TYPE,
  WRITE_END_WRAP           UINT16TYPE,
  WRITE_END_LPOS           UINT32TYPE,
  READ_START_WRAP          UINT16TYPE,
  READ_START_LPOS          UINT32TYPE,
  READ_END_WRAP            UINT16TYPE,
  READ_END_LPOS            UINT32TYPE,
  CONSTRAINT TAPE_FILE_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_TAPE_FK FOREIGN KEY(VID)
    REFERENCES TAPE(VID),
  CONSTRAINT TAPE_FILE_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
  CONSTRAINT TAPE_FILE_COPY_NB_GT_ZERO CHECK(COPY_NB > 0),
  CONSTRAINT TAPE_FILE_SS_VID_FSEQ_FK FOREIGN KEY(SUPERSEDED_BY_VID, SUPERSEDED_BY_FSEQ)
    REFERENCES TAPE_FILE(VID, FSEQ)
);
CREATE INDEX TAPE_FILE_VID_IDX ON TAPE_FILE(VID);
CREATE INDEX TAPE_FILE_ARCHIVE_FILE_ID_IDX ON TAPE_FILE(ARCHIVE_FILE_ID);
CREATE INDEX TAPE_FILE_SBV_SBF_IDX ON TAPE_FILE(SUPERSEDED_BY_VID, SUPERSEDED_BY_FSEQ);
CREATE TABLE ACTIVITIES_WEIGHTS (
  DISK_INSTANCE_NAME       VARCHAR(100),
  ACTIVITY                 VARCHAR(100),
  WEIGHT                   VARCHAR(100),
  USER_COMMENT             VARCHAR(1000)   CONSTRAINT ACTIV_WEIGHTS_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME   VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME   VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_CLHN_NN NOT NULL,
  CREATION_LOG_TIME        UINT64TYPE      CONSTRAINT ACTIV_WEIGHTS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME         UINT64TYPE      CONSTRAINT ACTIV_WEIGHTS_LUT_NN  NOT NULL
);
CREATE TABLE USAGESTATS (
  GID                     NUMERIC(6, 0)   DEFAULT 0 CONSTRAINT NN_USAGESTATS_GID NOT NULL,
  TIMESTAMP               NUMERIC(20, 0)  DEFAULT 0 CONSTRAINT NN_USAGESTATS_TS NOT NULL,
  MAXFILEID               NUMERIC(20, 0),
  FILECOUNT               NUMERIC(20, 0),
  FILESIZE                NUMERIC(20, 0),
  SEGCOUNT                NUMERIC(20, 0),
  SEGSIZE                 NUMERIC(20, 0),
  SEG2COUNT               NUMERIC(20, 0),
  SEG2SIZE                NUMERIC(20, 0),
  CONSTRAINT PK_USAGESTATS_GID_TS PRIMARY KEY (GID, TIMESTAMP)
);
CREATE TABLE EXPERIMENTS (
 NAME                     VARCHAR(20),
 GID                      NUMERIC(6, 0) CONSTRAINT EXPERIMENTS_GID_PK PRIMARY KEY
);
INSERT INTO CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR,
  SCHEMA_VERSION_MINOR)
VALUES(
  0,
  0);
