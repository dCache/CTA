CREATE SEQUENCE ARCHIVE_FILE_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE SEQUENCE LOGICAL_LIBRARY_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE SEQUENCE MEDIA_TYPE_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE SEQUENCE STORAGE_CLASS_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE SEQUENCE TAPE_POOL_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE SEQUENCE VIRTUAL_ORGANIZATION_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE SEQUENCE FILE_RECYCLE_LOG_ID_SEQ
  INCREMENT BY 1
  START WITH 1
  NO MAXVALUE
  MINVALUE 1
  NO CYCLE
  CACHE 20;
CREATE TABLE CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR    NUMERIC(20, 0)      CONSTRAINT CTA_CATALOGUE_SVM1_NN NOT NULL,
  SCHEMA_VERSION_MINOR    NUMERIC(20, 0)      CONSTRAINT CTA_CATALOGUE_SVM2_NN NOT NULL,
  NEXT_SCHEMA_VERSION_MAJOR NUMERIC(20, 0),
  NEXT_SCHEMA_VERSION_MINOR NUMERIC(20, 0),
  STATUS                  VARCHAR(100),
  IS_PRODUCTION           CHAR(1)         DEFAULT '0' CONSTRAINT CTA_CATALOGUE_IP_NN NOT NULL,
  CONSTRAINT CTA_CATALOGUE_IP_BOOL_CK     CHECK(IS_PRODUCTION IN ('0','1'))
);
CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME         VARCHAR(100)    CONSTRAINT ADMIN_USER_AUN_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ADMIN_USER_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT ADMIN_USER_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT ADMIN_USER_LUT_NN  NOT NULL,
  CONSTRAINT ADMIN_USER_PK PRIMARY KEY(ADMIN_USER_NAME)
);
CREATE TABLE DISK_SYSTEM(
  DISK_SYSTEM_NAME        VARCHAR(100)    CONSTRAINT DISK_SYSTEM_DSNM_NN NOT NULL,
  FILE_REGEXP             VARCHAR(100)    CONSTRAINT DISK_SYSTEM_FR_NN   NOT NULL,
  FREE_SPACE_QUERY_URL    VARCHAR(1000)   CONSTRAINT DISK_SYSTEM_FSQU_NN NOT NULL,
  REFRESH_INTERVAL        NUMERIC(20, 0)      CONSTRAINT DISK_SYSTEM_RI_NN   NOT NULL,
  TARGETED_FREE_SPACE     NUMERIC(20, 0)      CONSTRAINT DISK_SYSTEM_TFS_NN  NOT NULL,
  SLEEP_TIME              NUMERIC(20, 0)      CONSTRAINT DISK_SYSTEM_ST_NN   NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT DISK_SYSTEM_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT DISK_SYSTEM_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT DISK_SYSTEM_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT DISK_SYSTEM_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT DISK_SYSTEM_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT DISK_SYSTEM_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT DISK_SYSTEM_LUT_NN  NOT NULL,
  CONSTRAINT NAME_PK PRIMARY KEY(DISK_SYSTEM_NAME)
);
CREATE TABLE VIRTUAL_ORGANIZATION(
  VIRTUAL_ORGANIZATION_ID NUMERIC(20, 0)      CONSTRAINT VIRTUAL_ORGANIZATION_VOI_NN  NOT NULL,
  VIRTUAL_ORGANIZATION_NAME VARCHAR(100)  CONSTRAINT VIRTUAL_ORGANIZATION_VON_NN  NOT NULL,
  READ_MAX_DRIVES NUMERIC(20, 0)              CONSTRAINT VIRTUAL_ORGANIZATION_RMD_NN  NOT NULL,
  WRITE_MAX_DRIVES NUMERIC(20, 0)             CONSTRAINT VIRTUAL_ORGANIZATION_WMD_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT VIRTUAL_ORGANIZATION_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT VIRTUAL_ORGANIZATION_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT VIRTUAL_ORGANIZATION_LUT_NN  NOT NULL,
  CONSTRAINT VIRTUAL_ORGANIZATION_PK PRIMARY KEY(VIRTUAL_ORGANIZATION_ID),
  CONSTRAINT VIRTUAL_ORGANIZATION_VON_UN UNIQUE(VIRTUAL_ORGANIZATION_NAME)
);
CREATE TABLE STORAGE_CLASS(
  STORAGE_CLASS_ID        NUMERIC(20, 0)      CONSTRAINT STORAGE_CLASS_SCI_NN  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR(100)    CONSTRAINT STORAGE_CLASS_SCN_NN  NOT NULL,
  NB_COPIES               NUMERIC(3, 0)       CONSTRAINT STORAGE_CLASS_NC_NN   NOT NULL,
  VIRTUAL_ORGANIZATION_ID NUMERIC(20, 0)      CONSTRAINT STORAGE_CLASS_VOI_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT STORAGE_CLASS_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT STORAGE_CLASS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT STORAGE_CLASS_LUT_NN  NOT NULL,
  CONSTRAINT STORAGE_CLASS_PK PRIMARY KEY(STORAGE_CLASS_ID),
  CONSTRAINT STORAGE_CLASS_SCN_UN UNIQUE(STORAGE_CLASS_NAME),
  CONSTRAINT STORAGE_CLASS_VOI_FK FOREIGN KEY(VIRTUAL_ORGANIZATION_ID) REFERENCES VIRTUAL_ORGANIZATION(VIRTUAL_ORGANIZATION_ID)
);
CREATE TABLE TAPE_POOL(
  TAPE_POOL_ID            NUMERIC(20, 0)      CONSTRAINT TAPE_POOL_TPI_NN  NOT NULL,
  TAPE_POOL_NAME          VARCHAR(100)    CONSTRAINT TAPE_POOL_TPN_NN  NOT NULL,
  VIRTUAL_ORGANIZATION_ID NUMERIC(20, 0)      CONSTRAINT TAPE_POOL_VOI_NN  NOT NULL,
  NB_PARTIAL_TAPES        NUMERIC(20, 0)      CONSTRAINT TAPE_POOL_NPT_NN  NOT NULL,
  IS_ENCRYPTED            CHAR(1)         CONSTRAINT TAPE_POOL_IE_NN   NOT NULL,
  SUPPLY                  VARCHAR(100),
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT TAPE_POOL_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT TAPE_POOL_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT TAPE_POOL_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_POOL_PK PRIMARY KEY(TAPE_POOL_ID),
  CONSTRAINT TAPE_POOL_TPN_UN UNIQUE(TAPE_POOL_NAME),
  CONSTRAINT TAPE_POOL_IS_ENCRYPTED_BOOL_CK CHECK(IS_ENCRYPTED IN ('0', '1')),
  CONSTRAINT TAPE_POOL_VO_FK FOREIGN KEY(VIRTUAL_ORGANIZATION_ID) REFERENCES VIRTUAL_ORGANIZATION(VIRTUAL_ORGANIZATION_ID)
);
CREATE TABLE ARCHIVE_ROUTE(
  STORAGE_CLASS_ID        NUMERIC(20, 0)      CONSTRAINT ARCHIVE_ROUTE_SCI_NN  NOT NULL,
  COPY_NB                 NUMERIC(3, 0)       CONSTRAINT ARCHIVE_ROUTE_CN_NN   NOT NULL,
  TAPE_POOL_ID            NUMERIC(20, 0)      CONSTRAINT ARCHIVE_ROUTE_TPI_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ARCHIVE_ROUTE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT ARCHIVE_ROUTE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT ARCHIVE_ROUTE_LUT_NN  NOT NULL,
  CONSTRAINT ARCHIVE_ROUTE_PK PRIMARY KEY(STORAGE_CLASS_ID, COPY_NB),
  CONSTRAINT ARCHIVE_ROUTE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_ROUTE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_ID) REFERENCES TAPE_POOL(TAPE_POOL_ID),
  CONSTRAINT ARCHIVE_ROUTE_COPY_NB_GT_0_CK CHECK(COPY_NB > 0),
  CONSTRAINT ARCHIVE_ROUTE_SCI_TPI_UN UNIQUE(STORAGE_CLASS_ID, TAPE_POOL_ID)
);
CREATE TABLE MEDIA_TYPE(
  MEDIA_TYPE_ID          NUMERIC(20, 0)    CONSTRAINT MEDIA_TYPE_MTI_NN  NOT NULL,
  MEDIA_TYPE_NAME        VARCHAR(100)  CONSTRAINT MEDIA_TYPE_MTN_NN  NOT NULL,
  CARTRIDGE              VARCHAR(100)  CONSTRAINT MEDIA_TYPE_C_NN    NOT NULL,
  CAPACITY_IN_BYTES      NUMERIC(20, 0)    CONSTRAINT MEDIA_TYPE_CIB_NN  NOT NULL,
  PRIMARY_DENSITY_CODE   NUMERIC(3, 0),
  SECONDARY_DENSITY_CODE NUMERIC(3, 0),
  NB_WRAPS               NUMERIC(10, 0),
  MIN_LPOS               NUMERIC(20, 0),
  MAX_LPOS               NUMERIC(20, 0),
  USER_COMMENT           VARCHAR(1000) CONSTRAINT MEDIA_TYPE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)  CONSTRAINT MEDIA_TYPE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)  CONSTRAINT MEDIA_TYPE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      NUMERIC(20, 0)    CONSTRAINT MEDIA_TYPE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)  CONSTRAINT MEDIA_TYPE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)  CONSTRAINT MEDIA_TYPE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       NUMERIC(20, 0)    CONSTRAINT MEDIA_TYPE_LUT_NN  NOT NULL,
  CONSTRAINT MEDIA_TYPE_PK PRIMARY KEY(MEDIA_TYPE_ID),
  CONSTRAINT MEDIA_TYPE_MTN_UN UNIQUE(MEDIA_TYPE_NAME)
);
CREATE TABLE LOGICAL_LIBRARY(
  LOGICAL_LIBRARY_ID      NUMERIC(20, 0)      CONSTRAINT LOGICAL_LIBRARY_LLI_NN  NOT NULL,
  LOGICAL_LIBRARY_NAME    VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LLN_NN  NOT NULL,
  IS_DISABLED             CHAR(1)         DEFAULT '0' CONSTRAINT LOGICAL_LIBRARY_ID_NN NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT LOGICAL_LIBRARY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT LOGICAL_LIBRARY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT LOGICAL_LIBRARY_LUT_NN  NOT NULL,
  CONSTRAINT LOGICAL_LIBRARY_PK PRIMARY KEY(LOGICAL_LIBRARY_ID),
  CONSTRAINT LOGICAL_LIBRARY_LLN_UN UNIQUE(LOGICAL_LIBRARY_NAME),
  CONSTRAINT LOGICAL_LIBRARY_ID_BOOL_CK CHECK(IS_DISABLED IN ('0', '1'))
);
CREATE TABLE TAPE(
  VID                     VARCHAR(100)    CONSTRAINT TAPE_V_NN    NOT NULL,
  MEDIA_TYPE_ID           NUMERIC(20, 0)      CONSTRAINT TAPE_MTID_NN   NOT NULL,
  VENDOR                  VARCHAR(100)    CONSTRAINT TAPE_V2_NN   NOT NULL,
  LOGICAL_LIBRARY_ID      NUMERIC(20, 0)      CONSTRAINT TAPE_LLI_NN  NOT NULL,
  TAPE_POOL_ID            NUMERIC(20, 0)      CONSTRAINT TAPE_TPI_NN  NOT NULL,
  ENCRYPTION_KEY_NAME     VARCHAR(100),
  DATA_IN_BYTES           NUMERIC(20, 0)      CONSTRAINT TAPE_DIB_NN  NOT NULL,
  LAST_FSEQ               NUMERIC(20, 0)      CONSTRAINT TAPE_LF_NN   NOT NULL,
  NB_MASTER_FILES         NUMERIC(20, 0)      DEFAULT 0 CONSTRAINT TAPE_NB_MASTER_FILES_NN NOT NULL,
  MASTER_DATA_IN_BYTES    NUMERIC(20, 0)      DEFAULT 0 CONSTRAINT TAPE_MASTER_DATA_IN_BYTES_NN NOT NULL,
  IS_FULL                 CHAR(1)         CONSTRAINT TAPE_IF_NN   NOT NULL,
  IS_FROM_CASTOR          CHAR(1)         CONSTRAINT TAPE_IFC_NN  NOT NULL,
  DIRTY                   CHAR(1)         DEFAULT '1' CONSTRAINT TAPE_DIRTY_NN NOT NULL,
  NB_COPY_NB_1            NUMERIC(20, 0)     DEFAULT 0 CONSTRAINT TAPE_NB_COPY_NB_1_NN NOT NULL,
  COPY_NB_1_IN_BYTES      NUMERIC(20, 0)     DEFAULT 0 CONSTRAINT TAPE_COPY_NB_1_IN_BYTES_NN NOT NULL,
  NB_COPY_NB_GT_1         NUMERIC(20, 0)     DEFAULT 0 CONSTRAINT TAPE_NB_COPY_NB_GT_1_NN NOT NULL,
  COPY_NB_GT_1_IN_BYTES   NUMERIC(20, 0)     DEFAULT 0 CONSTRAINT TAPE_COPY_NB_GT_1_IN_BYTES_NN NOT NULL,
  LABEL_DRIVE             VARCHAR(100),
  LABEL_TIME              NUMERIC(20, 0)    ,
  LAST_READ_DRIVE         VARCHAR(100),
  LAST_READ_TIME          NUMERIC(20, 0)    ,
  LAST_WRITE_DRIVE        VARCHAR(100),
  LAST_WRITE_TIME         NUMERIC(20, 0)    ,
  READ_MOUNT_COUNT        NUMERIC(20, 0)      DEFAULT 0 CONSTRAINT TAPE_RMC_NN NOT NULL,
  WRITE_MOUNT_COUNT       NUMERIC(20, 0)      DEFAULT 0 CONSTRAINT TAPE_WMC_NN NOT NULL,
  USER_COMMENT            VARCHAR(1000),
  TAPE_STATE              VARCHAR(100)    CONSTRAINT TAPE_TS_NN NOT NULL,
  STATE_REASON            VARCHAR(1000),
  STATE_UPDATE_TIME       NUMERIC(20, 0)      CONSTRAINT TAPE_SUT_NN NOT NULL,
  STATE_MODIFIED_BY       VARCHAR(100)    CONSTRAINT TAPE_SMB_NN NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)      CONSTRAINT TAPE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)      CONSTRAINT TAPE_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_PK PRIMARY KEY(VID),
  CONSTRAINT TAPE_LOGICAL_LIBRARY_FK FOREIGN KEY(LOGICAL_LIBRARY_ID) REFERENCES LOGICAL_LIBRARY(LOGICAL_LIBRARY_ID),
  CONSTRAINT TAPE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_ID) REFERENCES TAPE_POOL(TAPE_POOL_ID),
  CONSTRAINT TAPE_IS_FULL_BOOL_CK CHECK(IS_FULL IN ('0', '1')),
  CONSTRAINT TAPE_IS_FROM_CASTOR_BOOL_CK CHECK(IS_FROM_CASTOR IN ('0', '1')),
  CONSTRAINT TAPE_DIRTY_BOOL_CK CHECK(DIRTY IN ('0','1')),
  CONSTRAINT TAPE_STATE_CK CHECK(TAPE_STATE IN ('ACTIVE', 'DISABLED', 'BROKEN')),
  CONSTRAINT TAPE_MEDIA_TYPE_FK FOREIGN KEY(MEDIA_TYPE_ID) REFERENCES MEDIA_TYPE(MEDIA_TYPE_ID)
);
CREATE INDEX TAPE_TAPE_POOL_ID_IDX ON TAPE(TAPE_POOL_ID);
CREATE INDEX TAPE_STATE_IDX ON TAPE(TAPE_STATE);
CREATE TABLE MOUNT_POLICY(
  MOUNT_POLICY_NAME          VARCHAR(100)    CONSTRAINT MOUNT_POLICY_MPN_NN  NOT NULL,
  ARCHIVE_PRIORITY           NUMERIC(20, 0)      CONSTRAINT MOUNT_POLICY_AP_NN   NOT NULL,
  ARCHIVE_MIN_REQUEST_AGE    NUMERIC(20, 0)      CONSTRAINT MOUNT_POLICY_AMRA_NN NOT NULL,
  RETRIEVE_PRIORITY          NUMERIC(20, 0)      CONSTRAINT MOUNT_POLICY_RP_NN   NOT NULL,
  RETRIEVE_MIN_REQUEST_AGE   NUMERIC(20, 0)      CONSTRAINT MOUNT_POLICY_RMRA_NN NOT NULL,
  USER_COMMENT               VARCHAR(1000)   CONSTRAINT MOUNT_POLICY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME     VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME     VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME          NUMERIC(20, 0)      CONSTRAINT MOUNT_POLICY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME      VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME      VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME           NUMERIC(20, 0)      CONSTRAINT MOUNT_POLICY_LUT_NN  NOT NULL,
  CONSTRAINT MOUNT_POLICY_PK PRIMARY KEY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR(100)    CONSTRAINT RQSTER_RULE_DIN_NN  NOT NULL,
  REQUESTER_NAME         VARCHAR(100)    CONSTRAINT RQSTER_RULE_RN_NN   NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR(100)    CONSTRAINT RQSTER_RULE_MPN_NN  NOT NULL,
  USER_COMMENT           VARCHAR(1000)   CONSTRAINT RQSTER_RULE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      NUMERIC(20, 0)      CONSTRAINT RQSTER_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       NUMERIC(20, 0)      CONSTRAINT RQSTER_RULE_LUT_NN  NOT NULL,
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
  CREATION_LOG_TIME      NUMERIC(20, 0)      CONSTRAINT RQSTER_GRP_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       NUMERIC(20, 0)      CONSTRAINT RQSTER_GRP_RULE_LUT_NN  NOT NULL,
  CONSTRAINT RQSTER_GRP_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_GROUP_NAME),
  CONSTRAINT RQSTER_GRP_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID         NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_AFI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DIN_NN  NOT NULL,
  DISK_FILE_ID            VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DFI_NN  NOT NULL,
  DISK_FILE_UID           NUMERIC(10, 0)      CONSTRAINT ARCHIVE_FILE_DFUID_NN  NOT NULL,
  DISK_FILE_GID           NUMERIC(10, 0)      CONSTRAINT ARCHIVE_FILE_DFGID_NN  NOT NULL,
  SIZE_IN_BYTES           NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_SIB_NN  NOT NULL,
  CHECKSUM_BLOB           BYTEA,
  CHECKSUM_ADLER32        NUMERIC(10, 0)      CONSTRAINT ARCHIVE_FILE_CB2_NN  NOT NULL,
  STORAGE_CLASS_ID        NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_SCI_NN  NOT NULL,
  CREATION_TIME           NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_CT2_NN  NOT NULL,
  RECONCILIATION_TIME     NUMERIC(20, 0)      CONSTRAINT ARCHIVE_FILE_RT_NN   NOT NULL,
  IS_DELETED              CHAR(1)         DEFAULT '0' CONSTRAINT ARCHIVE_FILE_ID_NN NOT NULL,
  COLLOCATION_HINT        VARCHAR(100),
  CONSTRAINT ARCHIVE_FILE_PK PRIMARY KEY(ARCHIVE_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_FILE_DIN_DFI_UN UNIQUE(DISK_INSTANCE_NAME, DISK_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_ID_BOOL_CK CHECK(IS_DELETED IN ('0', '1'))
);
CREATE INDEX ARCHIVE_FILE_DIN_IDX ON ARCHIVE_FILE(DISK_INSTANCE_NAME);
CREATE INDEX ARCHIVE_FILE_DFI_IDX ON ARCHIVE_FILE(DISK_FILE_ID);
CREATE TABLE TAPE_FILE(
  VID                      VARCHAR(100)   CONSTRAINT TAPE_FILE_V_NN    NOT NULL,
  FSEQ                     NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_F_NN    NOT NULL,
  BLOCK_ID                 NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_BI_NN   NOT NULL,
  LOGICAL_SIZE_IN_BYTES    NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_CSIB_NN NOT NULL,
  COPY_NB                  NUMERIC(3, 0)      CONSTRAINT TAPE_FILE_CN_NN   NOT NULL,
  CREATION_TIME            NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_CT_NN   NOT NULL,
  ARCHIVE_FILE_ID          NUMERIC(20, 0)     CONSTRAINT TAPE_FILE_AFI_NN  NOT NULL,
  CONSTRAINT TAPE_FILE_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_TAPE_FK FOREIGN KEY(VID)
    REFERENCES TAPE(VID),
  CONSTRAINT TAPE_FILE_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
  CONSTRAINT TAPE_FILE_COPY_NB_GT_0_CK CHECK(COPY_NB > 0)
);
CREATE INDEX TAPE_FILE_VID_IDX ON TAPE_FILE(VID);
CREATE INDEX TAPE_FILE_ARCHIVE_FILE_ID_IDX ON TAPE_FILE(ARCHIVE_FILE_ID);
CREATE TABLE ACTIVITIES_WEIGHTS (
  DISK_INSTANCE_NAME       VARCHAR(100),
  ACTIVITY                 VARCHAR(100),
  WEIGHT                   VARCHAR(100),
  USER_COMMENT             VARCHAR(1000)   CONSTRAINT ACTIV_WEIGHTS_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME   VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME   VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_CLHN_NN NOT NULL,
  CREATION_LOG_TIME        NUMERIC(20, 0)      CONSTRAINT ACTIV_WEIGHTS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME         NUMERIC(20, 0)      CONSTRAINT ACTIV_WEIGHTS_LUT_NN  NOT NULL
);
CREATE TABLE FILE_RECYCLE_LOG(
  FILE_RECYCLE_LOG_ID        NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_ID_NN NOT NULL,
  VID                        VARCHAR(100)        CONSTRAINT FILE_RECYCLE_LOG_VID_NN NOT NULL,
  FSEQ                       NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_FSEQ_NN NOT NULL,
  BLOCK_ID                   NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_BID_NN NOT NULL,
  COPY_NB                    NUMERIC(3, 0)           CONSTRAINT FILE_RECYCLE_LOG_COPY_NB_NN NOT NULL,
  TAPE_FILE_CREATION_TIME    NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_TFCT_NN NOT NULL,
  ARCHIVE_FILE_ID            NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_AFI_NN NOT NULL,
  DISK_INSTANCE_NAME         VARCHAR(100)        CONSTRAINT FILE_RECYCLE_LOG_DIN_NN NOT NULL,
  DISK_FILE_ID               VARCHAR(100)        CONSTRAINT FILE_RECYCLE_LOG_DFI_NN NOT NULL,
  DISK_FILE_ID_WHEN_DELETED  VARCHAR(100)        CONSTRAINT FILE_RECYCLE_LOG_DFIWD_NN NOT NULL,
  DISK_FILE_UID              NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_DFU_NN NOT NULL,
  DISK_FILE_GID              NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_DFG_NN NOT NULL,
  SIZE_IN_BYTES              NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_SIB_NN NOT NULL,
  CHECKSUM_BLOB              BYTEA,
  CHECKSUM_ADLER32           NUMERIC(10, 0)          CONSTRAINT FILE_RECYCLE_LOG_CA_NN NOT NULL,
  STORAGE_CLASS_ID           NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_SCI_NN NOT NULL,
  ARCHIVE_FILE_CREATION_TIME NUMERIC(20, 0)          CONSTRAINT FILE_RECYLE_LOG_CT_NN NOT NULL,
  RECONCILIATION_TIME        NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_RT_NN NOT NULL,
  COLLOCATION_HINT           VARCHAR(100),
  DISK_FILE_PATH             VARCHAR(2000),
  REASON_LOG                 VARCHAR(1000)       CONSTRAINT FILE_RECYCLE_LOG_RL_NN NOT NULL,
  RECYCLE_LOG_TIME           NUMERIC(20, 0)          CONSTRAINT FILE_RECYCLE_LOG_RLT_NN NOT NULL,
  CONSTRAINT FILE_RECYCLE_LOG_PK PRIMARY KEY(FILE_RECYCLE_LOG_ID),
  CONSTRAINT FILE_RECYCLE_LOG_VID_FK FOREIGN KEY(VID) REFERENCES TAPE(VID),
  CONSTRAINT FILE_RECYCLE_LOG_SC_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID)
);
CREATE INDEX FILE_RECYCLE_LOG_DFI_IDX ON FILE_RECYCLE_LOG(DISK_FILE_ID);

CREATE TABLE DRIVE_CONFIG (
  DRIVE_NAME                  VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_DN_NN  NOT NULL,
  CATEGORY                    VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_C_NN   NOT NULL,
  KEY_NAME                    VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_KN_NN  NOT NULL,
  VALUE                       VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_V_NN   NOT NULL,
  SOURCE                      VARCHAR(100)       CONSTRAINT DRIVE_CONFIG_S_NN   NOT NULL,
  CONSTRAINT DRIVE_CONFIG_DN_PK PRIMARY KEY(KEY_NAME)
);

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
INSERT INTO CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR,
  SCHEMA_VERSION_MINOR,
  STATUS)
VALUES(
  4,
  1,
  'PRODUCTION');
ALTER TABLE ARCHIVE_FILE DROP CONSTRAINT
  ARCHIVE_FILE_DIN_DFI_UN;
ALTER TABLE ARCHIVE_FILE ADD CONSTRAINT
  ARCHIVE_FILE_DIN_DFI_UN UNIQUE(DISK_INSTANCE_NAME, DISK_FILE_ID) DEFERRABLE INITIALLY IMMEDIATE;
ALTER TABLE CTA_CATALOGUE ADD CONSTRAINT
  CATALOGUE_STATUS_CONTENT_CK CHECK((NEXT_SCHEMA_VERSION_MAJOR IS NULL AND NEXT_SCHEMA_VERSION_MINOR IS NULL AND STATUS='PRODUCTION') OR (STATUS='UPGRADING'));
