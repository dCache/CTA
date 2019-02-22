CREATE TABLE CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR    NUMERIC(20, 0)  CONSTRAINT CTA_CATALOGUE_SVM1_NN NOT NULL,
  SCHEMA_VERSION_MINOR    NUMERIC(20, 0)  CONSTRAINT CTA_CATALOGUE_SVM2_NN NOT NULL
);
CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME         VARCHAR(100)    CONSTRAINT ADMIN_USER_AUN_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ADMIN_USER_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)  CONSTRAINT ADMIN_USER_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)  CONSTRAINT ADMIN_USER_LUT_NN  NOT NULL,
  CONSTRAINT ADMIN_USER_PK PRIMARY KEY(ADMIN_USER_NAME)
);
CREATE TABLE STORAGE_CLASS(
  STORAGE_CLASS_ID        NUMERIC(20, 0)  CONSTRAINT STORAGE_CLASS_SCI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT STORAGE_CLASS_DIN_NN  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR(100)    CONSTRAINT STORAGE_CLASS_SCN_NN  NOT NULL,
  NB_COPIES               NUMERIC(20, 0)  CONSTRAINT STORAGE_CLASS_NC_NN   NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT STORAGE_CLASS_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)  CONSTRAINT STORAGE_CLASS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)  CONSTRAINT STORAGE_CLASS_LUT_NN  NOT NULL,
  CONSTRAINT STORAGE_CLASS_PK PRIMARY KEY(STORAGE_CLASS_ID),
  CONSTRAINT STORAGE_CLASS_DIN_SCN_UN UNIQUE(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME)
);
CREATE TABLE TAPE_POOL(
  TAPE_POOL_NAME          VARCHAR(100)    CONSTRAINT TAPE_POOL_TPN_NN  NOT NULL,
  VO                      VARCHAR(100)    CONSTRAINT TAPE_POOL_VO_NN   NOT NULL,
  NB_PARTIAL_TAPES        NUMERIC(20, 0)  CONSTRAINT TAPE_POOL_NPT_NN  NOT NULL,
  IS_ENCRYPTED            CHAR(1)         CONSTRAINT TAPE_POOL_IE_NN   NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT TAPE_POOL_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)  CONSTRAINT TAPE_POOL_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)  CONSTRAINT TAPE_POOL_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_POOL_PK PRIMARY KEY(TAPE_POOL_NAME),
  CONSTRAINT TAPE_POOL_IS_ENCRYPTED_BOOL_CK CHECK(IS_ENCRYPTED IN ('0', '1'))
);
CREATE TABLE ARCHIVE_ROUTE(
  STORAGE_CLASS_ID        NUMERIC(20, 0)  CONSTRAINT ARCHIVE_ROUTE_SCI_NN  NOT NULL,
  COPY_NB                 NUMERIC(20, 0)  CONSTRAINT ARCHIVE_ROUTE_CN_NN   NOT NULL,
  TAPE_POOL_NAME          VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_TPN_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ARCHIVE_ROUTE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)  CONSTRAINT ARCHIVE_ROUTE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)  CONSTRAINT ARCHIVE_ROUTE_LUT_NN  NOT NULL,
  CONSTRAINT ARCHIVE_ROUTE_PK PRIMARY KEY(STORAGE_CLASS_ID, COPY_NB),
  CONSTRAINT ARCHIVE_ROUTE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_ROUTE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_NAME)
    REFERENCES TAPE_POOL(TAPE_POOL_NAME),
  CONSTRAINT ARCHIVE_ROUTE_COPY_NB_GT_ZERO CHECK(COPY_NB > 0)
);
CREATE TABLE LOGICAL_LIBRARY(
  LOGICAL_LIBRARY_NAME    VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LLL_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT LOGICAL_LIBRARY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)  CONSTRAINT LOGICAL_LIBRARY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)  CONSTRAINT LOGICAL_LIBRARY_LUT_NN  NOT NULL,
  CONSTRAINT LOGICAL_LIBRARY_PK PRIMARY KEY(LOGICAL_LIBRARY_NAME)
);
CREATE TABLE TAPE(
  VID                     VARCHAR(100)    CONSTRAINT TAPE_V_UN    NOT NULL,
  MEDIA_TYPE              VARCHAR(100)    CONSTRAINT TAPE_MT_NN   NOT NULL,
  VENDOR                  VARCHAR(100)    CONSTRAINT TAPE_V2_NN   NOT NULL,
  LOGICAL_LIBRARY_NAME    VARCHAR(100)    CONSTRAINT TAPE_LLN_UN  NOT NULL,
  TAPE_POOL_NAME          VARCHAR(100)    CONSTRAINT TAPE_TPN_UN  NOT NULL,
  ENCRYPTION_KEY          VARCHAR(100),
  CAPACITY_IN_BYTES       NUMERIC(20, 0)  CONSTRAINT TAPE_CIB_UN  NOT NULL,
  DATA_IN_BYTES           NUMERIC(20, 0)  CONSTRAINT TAPE_DIB_UN  NOT NULL,
  LAST_FSEQ               NUMERIC(20, 0)  CONSTRAINT TAPE_LF_UN   NOT NULL,
  IS_DISABLED             CHAR(1)         CONSTRAINT TAPE_ID_UN   NOT NULL,
  IS_FULL                 CHAR(1)         CONSTRAINT TAPE_IF_UN   NOT NULL,
  LBP_IS_ON               CHAR(1),
  LABEL_DRIVE             VARCHAR(100),
  LABEL_TIME              NUMERIC(20, 0),
  LAST_READ_DRIVE         VARCHAR(100),
  LAST_READ_TIME          NUMERIC(20, 0),
  LAST_WRITE_DRIVE        VARCHAR(100),
  LAST_WRITE_TIME         NUMERIC(20, 0),
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT TAPE_UC_UN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLUN_UN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLHN_UN NOT NULL,
  CREATION_LOG_TIME       NUMERIC(20, 0)  CONSTRAINT TAPE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUUN_UN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUHN_UN NOT NULL,
  LAST_UPDATE_TIME        NUMERIC(20, 0)  CONSTRAINT TAPE_LUT_UN  NOT NULL,
  CONSTRAINT TAPE_PK PRIMARY KEY(VID),
  CONSTRAINT TAPE_LOGICAL_LIBRARY_FK FOREIGN KEY(LOGICAL_LIBRARY_NAME)
    REFERENCES LOGICAL_LIBRARY(LOGICAL_LIBRARY_NAME),
  CONSTRAINT TAPE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_NAME)
    REFERENCES TAPE_POOL(TAPE_POOL_NAME),
  CONSTRAINT TAPE_IS_DISABLED_BOOL_CK CHECK(IS_DISABLED IN ('0', '1')),
  CONSTRAINT TAPE_IS_FULL_BOOL_CK CHECK(IS_FULL IN ('0', '1')),
  CONSTRAINT TAPE_LBP_IS_ON_BOOL_CK CHECK(LBP_IS_ON IN ('0', '1'))
);
CREATE INDEX TAPE_TAPE_POOL_NAME_IDX ON TAPE(TAPE_POOL_NAME);
CREATE TABLE MOUNT_POLICY(
  MOUNT_POLICY_NAME        VARCHAR(100)    CONSTRAINT MOUNT_POLICY_MPN_NN  NOT NULL,
  ARCHIVE_PRIORITY         NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_AP_NN   NOT NULL,
  ARCHIVE_MIN_REQUEST_AGE  NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_AMRA_NN NOT NULL,
  RETRIEVE_PRIORITY        NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_RP_NN   NOT NULL,
  RETRIEVE_MIN_REQUEST_AGE NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_RMRA_NN NOT NULL,
  MAX_DRIVES_ALLOWED       NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_MDA_NN  NOT NULL,
  USER_COMMENT             VARCHAR(1000)   CONSTRAINT MOUNT_POLICY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME   VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME   VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME        NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME         NUMERIC(20, 0)  CONSTRAINT MOUNT_POLICY_LUT_NN  NOT NULL,
  CONSTRAINT MOUNT_POLICY_PK PRIMARY KEY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR(100)    CONSTRAINT RQSTER_RULE_DIN_NN  NOT NULL,
  REQUESTER_NAME         VARCHAR(100)    CONSTRAINT RQSTER_RULE_RN_NN   NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR(100)    CONSTRAINT RQSTER_RULE_MPN_NN  NOT NULL,
  USER_COMMENT           VARCHAR(1000)   CONSTRAINT RQSTER_RULE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      NUMERIC(20, 0)  CONSTRAINT RQSTER_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       NUMERIC(20, 0)  CONSTRAINT RQSTER_RULE_LUT_NN  NOT NULL,
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
  CREATION_LOG_TIME      NUMERIC(20, 0)  CONSTRAINT RQSTER_GRP_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       NUMERIC(20, 0)  CONSTRAINT RQSTER_GRP_RULE_LUT_NN  NOT NULL,
  CONSTRAINT RQSTER_GRP_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_GROUP_NAME),
  CONSTRAINT RQSTER_GRP_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID         NUMERIC(20, 0)  CONSTRAINT ARCHIVE_FILE_AFI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DIN_NN  NOT NULL,
  DISK_FILE_ID            VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DFI_NN  NOT NULL,
  DISK_FILE_PATH          VARCHAR(2000)   CONSTRAINT ARCHIVE_FILE_DFP_NN  NOT NULL,
  DISK_FILE_USER          VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DFU_NN  NOT NULL,
  DISK_FILE_GROUP         VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DFG_NN  NOT NULL,
  SIZE_IN_BYTES           NUMERIC(20, 0)  CONSTRAINT ARCHIVE_FILE_SIB_NN  NOT NULL,
  CHECKSUM_TYPE           VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_CT1_NN  NOT NULL,
  CHECKSUM_VALUE          VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_CV_NN   NOT NULL,
  STORAGE_CLASS_ID        NUMERIC(20, 0)  CONSTRAINT ARCHIVE_FILE_SCI_NN  NOT NULL,
  CREATION_TIME           NUMERIC(20, 0)  CONSTRAINT ARCHIVE_FILE_CT2_NN  NOT NULL,
  RECONCILIATION_TIME     NUMERIC(20, 0)  CONSTRAINT ARCHIVE_FILE_RT_NN   NOT NULL,
  CONSTRAINT ARCHIVE_FILE_PK PRIMARY KEY(ARCHIVE_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_FILE_DIN_DFI_UN UNIQUE(DISK_INSTANCE_NAME, DISK_FILE_ID)
);
CREATE INDEX ARCHIVE_FILE_DIN_DFP_IDX ON ARCHIVE_FILE(DISK_INSTANCE_NAME, DISK_FILE_PATH);
CREATE TABLE TAPE_FILE(
  VID                      VARCHAR(100)   CONSTRAINT TAPE_FILE_V_NN    NOT NULL,
  FSEQ                     NUMERIC(20, 0) CONSTRAINT TAPE_FILE_F_NN    NOT NULL,
  BLOCK_ID                 NUMERIC(20, 0) CONSTRAINT TAPE_FILE_BI_NN   NOT NULL,
  COMPRESSED_SIZE_IN_BYTES NUMERIC(20, 0) CONSTRAINT TAPE_FILE_CSIB_NN NOT NULL,
  COPY_NB                  NUMERIC(20, 0) CONSTRAINT TAPE_FILE_CN_NN   NOT NULL,
  CREATION_TIME            NUMERIC(20, 0) CONSTRAINT TAPE_FILE_CT_NN   NOT NULL,
  ARCHIVE_FILE_ID          NUMERIC(20, 0) CONSTRAINT TAPE_FILE_AFI_NN  NOT NULL,
  CONSTRAINT TAPE_FILE_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_TAPE_FK FOREIGN KEY(VID)
    REFERENCES TAPE(VID),
  CONSTRAINT TAPE_FILE_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
  CONSTRAINT TAPE_FILE_COPY_NB_GT_ZERO CHECK(COPY_NB > 0)
);
CREATE INDEX TAPE_FILE_VID_IDX ON TAPE_FILE(VID);
CREATE INDEX TAPE_FILE_ARCHIVE_FILE_ID_IDX ON TAPE_FILE(ARCHIVE_FILE_ID);
INSERT INTO CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR,
  SCHEMA_VERSION_MINOR)
VALUES(
  0,
  0);
