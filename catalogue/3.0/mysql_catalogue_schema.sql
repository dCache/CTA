CREATE TABLE ARCHIVE_FILE_ID(
  ID BIGINT UNSIGNED,
  CONSTRAINT ARCHIVE_FILE_ID_PK PRIMARY KEY(ID)
);
INSERT INTO ARCHIVE_FILE_ID(ID) VALUES(1);
CREATE TABLE LOGICAL_LIBRARY_ID(
  ID BIGINT UNSIGNED,
  CONSTRAINT LOGICAL_LIBRARY_ID_PK PRIMARY KEY(ID)
);
INSERT INTO LOGICAL_LIBRARY_ID(ID) VALUES(1);
CREATE TABLE MEDIA_TYPE_ID(
  ID BIGINT UNSIGNED,
  CONSTRAINT MEDIA_TYPE_ID_PK PRIMARY KEY(ID)
);
INSERT INTO MEDIA_TYPE_ID(ID) VALUES(1);
CREATE TABLE STORAGE_CLASS_ID(
  ID BIGINT UNSIGNED,
  CONSTRAINT STORAGE_CLASS_ID_PK PRIMARY KEY(ID)
);
INSERT INTO STORAGE_CLASS_ID(ID) VALUES(1);
CREATE TABLE TAPE_POOL_ID(
  ID BIGINT UNSIGNED,
  CONSTRAINT TAPE_POOL_ID_PK PRIMARY KEY(ID)
);
INSERT INTO TAPE_POOL_ID(ID) VALUES(1);
CREATE TABLE VIRTUAL_ORGANIZATION_ID(
  ID BIGINT UNSIGNED,
  CONSTRAINT VIRTUAL_ORGANIZATION_ID_PK PRIMARY KEY(ID)
);
INSERT INTO VIRTUAL_ORGANIZATION_ID(ID) VALUES(1);
CREATE TABLE CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR    BIGINT UNSIGNED      CONSTRAINT CTA_CATALOGUE_SVM1_NN NOT NULL,
  SCHEMA_VERSION_MINOR    BIGINT UNSIGNED      CONSTRAINT CTA_CATALOGUE_SVM2_NN NOT NULL,
  NEXT_SCHEMA_VERSION_MAJOR BIGINT UNSIGNED,
  NEXT_SCHEMA_VERSION_MINOR BIGINT UNSIGNED,
  STATUS                  VARCHAR(100),
  IS_PRODUCTION           CHAR(1)         DEFAULT '0' CONSTRAINT CTA_CATALOGUE_IP_NN NOT NULL,
  CONSTRAINT CTA_CATALOGUE_IP_BOOL_CK     CHECK(IS_PRODUCTION IN ('0','1'))
);
CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME         VARCHAR(100)    CONSTRAINT ADMIN_USER_AUN_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ADMIN_USER_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ADMIN_USER_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT ADMIN_USER_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ADMIN_USER_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT ADMIN_USER_LUT_NN  NOT NULL,
  CONSTRAINT ADMIN_USER_PK PRIMARY KEY(ADMIN_USER_NAME)
);
CREATE TABLE DISK_SYSTEM(
  DISK_SYSTEM_NAME        VARCHAR(100)    CONSTRAINT DISK_SYSTEM_DSNM_NN NOT NULL,
  FILE_REGEXP             VARCHAR(100)    CONSTRAINT DISK_SYSTEM_FR_NN   NOT NULL,
  FREE_SPACE_QUERY_URL    VARCHAR(1000)   CONSTRAINT DISK_SYSTEM_FSQU_NN NOT NULL,
  REFRESH_INTERVAL        BIGINT UNSIGNED      CONSTRAINT DISK_SYSTEM_RI_NN   NOT NULL,
  TARGETED_FREE_SPACE     BIGINT UNSIGNED      CONSTRAINT DISK_SYSTEM_TFS_NN  NOT NULL,
  SLEEP_TIME              BIGINT UNSIGNED      CONSTRAINT DISK_SYSTEM_ST_NN   NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT DISK_SYSTEM_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT DISK_SYSTEM_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT DISK_SYSTEM_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT DISK_SYSTEM_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT DISK_SYSTEM_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT DISK_SYSTEM_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT DISK_SYSTEM_LUT_NN  NOT NULL,
  CONSTRAINT NAME_PK PRIMARY KEY(DISK_SYSTEM_NAME)
);
CREATE TABLE VIRTUAL_ORGANIZATION(
  VIRTUAL_ORGANIZATION_ID BIGINT UNSIGNED      CONSTRAINT VIRTUAL_ORGANIZATION_VOI_NN  NOT NULL,
  VIRTUAL_ORGANIZATION_NAME VARCHAR(100)  CONSTRAINT VIRTUAL_ORGANIZATION_VON_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT VIRTUAL_ORGANIZATION_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT VIRTUAL_ORGANIZATION_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT VIRTUAL_ORGANIZATION_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT VIRTUAL_ORGANIZATION_LUT_NN  NOT NULL,
  CONSTRAINT VIRTUAL_ORGANIZATION_PK PRIMARY KEY(VIRTUAL_ORGANIZATION_ID),
  CONSTRAINT VIRTUAL_ORGANIZATION_VON_UN UNIQUE(VIRTUAL_ORGANIZATION_NAME)
);
CREATE TABLE STORAGE_CLASS(
  STORAGE_CLASS_ID        BIGINT UNSIGNED      CONSTRAINT STORAGE_CLASS_SCI_NN  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR(100)    CONSTRAINT STORAGE_CLASS_SCN_NN  NOT NULL,
  NB_COPIES               TINYINT UNSIGNED       CONSTRAINT STORAGE_CLASS_NC_NN   NOT NULL,
  VIRTUAL_ORGANIZATION_ID BIGINT UNSIGNED      CONSTRAINT STORAGE_CLASS_VOI_NN  NOT NULL,  
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT STORAGE_CLASS_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT STORAGE_CLASS_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT STORAGE_CLASS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT STORAGE_CLASS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT STORAGE_CLASS_LUT_NN  NOT NULL,
  CONSTRAINT STORAGE_CLASS_PK PRIMARY KEY(STORAGE_CLASS_ID),
  CONSTRAINT STORAGE_CLASS_SCN_UN UNIQUE(STORAGE_CLASS_NAME),
  CONSTRAINT STORAGE_CLASS_VOI_FK FOREIGN KEY(VIRTUAL_ORGANIZATION_ID) REFERENCES VIRTUAL_ORGANIZATION(VIRTUAL_ORGANIZATION_ID)
);
CREATE TABLE TAPE_POOL(
  TAPE_POOL_ID            BIGINT UNSIGNED      CONSTRAINT TAPE_POOL_TPI_NN  NOT NULL,
  TAPE_POOL_NAME          VARCHAR(100)    CONSTRAINT TAPE_POOL_TPN_NN  NOT NULL,
  VIRTUAL_ORGANIZATION_ID BIGINT UNSIGNED      CONSTRAINT TAPE_POOL_VOI_NN  NOT NULL,
  NB_PARTIAL_TAPES        BIGINT UNSIGNED      CONSTRAINT TAPE_POOL_NPT_NN  NOT NULL,
  IS_ENCRYPTED            CHAR(1)         CONSTRAINT TAPE_POOL_IE_NN   NOT NULL,
  SUPPLY                  VARCHAR(100),
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT TAPE_POOL_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_POOL_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT TAPE_POOL_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_POOL_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT TAPE_POOL_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_POOL_PK PRIMARY KEY(TAPE_POOL_ID),
  CONSTRAINT TAPE_POOL_TPN_UN UNIQUE(TAPE_POOL_NAME),
  CONSTRAINT TAPE_POOL_IS_ENCRYPTED_BOOL_CK CHECK(IS_ENCRYPTED IN ('0', '1')),
  CONSTRAINT TAPE_POOL_VO_FK FOREIGN KEY(VIRTUAL_ORGANIZATION_ID) REFERENCES VIRTUAL_ORGANIZATION(VIRTUAL_ORGANIZATION_ID)
);
CREATE TABLE ARCHIVE_ROUTE(
  STORAGE_CLASS_ID        BIGINT UNSIGNED      CONSTRAINT ARCHIVE_ROUTE_SCI_NN  NOT NULL,
  COPY_NB                 TINYINT UNSIGNED       CONSTRAINT ARCHIVE_ROUTE_CN_NN   NOT NULL,
  TAPE_POOL_ID            BIGINT UNSIGNED      CONSTRAINT ARCHIVE_ROUTE_TPI_NN  NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT ARCHIVE_ROUTE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT ARCHIVE_ROUTE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT ARCHIVE_ROUTE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT ARCHIVE_ROUTE_LUT_NN  NOT NULL,
  CONSTRAINT ARCHIVE_ROUTE_PK PRIMARY KEY(STORAGE_CLASS_ID, COPY_NB),
  CONSTRAINT ARCHIVE_ROUTE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_ROUTE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_ID) REFERENCES TAPE_POOL(TAPE_POOL_ID),
  CONSTRAINT ARCHIVE_ROUTE_COPY_NB_GT_0_CK CHECK(COPY_NB > 0),
  CONSTRAINT ARCHIVE_ROUTE_SCI_TPI_UN UNIQUE(STORAGE_CLASS_ID, TAPE_POOL_ID)
);
CREATE TABLE MEDIA_TYPE(
  MEDIA_TYPE_ID          BIGINT UNSIGNED    CONSTRAINT MEDIA_TYPE_MTI_NN  NOT NULL,
  MEDIA_TYPE_NAME        VARCHAR(100)  CONSTRAINT MEDIA_TYPE_MTN_NN  NOT NULL,
  CARTRIDGE              VARCHAR(100)  CONSTRAINT MEDIA_TYPE_C_NN    NOT NULL,
  CAPACITY_IN_BYTES      BIGINT UNSIGNED    CONSTRAINT MEDIA_TYPE_CIB_NN  NOT NULL,
  PRIMARY_DENSITY_CODE   TINYINT UNSIGNED,
  SECONDARY_DENSITY_CODE TINYINT UNSIGNED,
  NB_WRAPS               INT UNSIGNED,
  MIN_LPOS               BIGINT UNSIGNED,
  MAX_LPOS               BIGINT UNSIGNED,
  USER_COMMENT           VARCHAR(1000) CONSTRAINT MEDIA_TYPE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)  CONSTRAINT MEDIA_TYPE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)  CONSTRAINT MEDIA_TYPE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      BIGINT UNSIGNED    CONSTRAINT MEDIA_TYPE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)  CONSTRAINT MEDIA_TYPE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)  CONSTRAINT MEDIA_TYPE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       BIGINT UNSIGNED    CONSTRAINT MEDIA_TYPE_LUT_NN  NOT NULL,
  CONSTRAINT MEDIA_TYPE_PK PRIMARY KEY(MEDIA_TYPE_ID),
  CONSTRAINT MEDIA_TYPE_MTN_UN UNIQUE(MEDIA_TYPE_NAME)
);
CREATE TABLE LOGICAL_LIBRARY(
  LOGICAL_LIBRARY_ID      BIGINT UNSIGNED      CONSTRAINT LOGICAL_LIBRARY_LLI_NN  NOT NULL,
  LOGICAL_LIBRARY_NAME    VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LLN_NN  NOT NULL,
  IS_DISABLED             CHAR(1)         DEFAULT '0' CONSTRAINT LOGICAL_LIBRARY_ID_NN NOT NULL,
  USER_COMMENT            VARCHAR(1000)   CONSTRAINT LOGICAL_LIBRARY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT LOGICAL_LIBRARY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT LOGICAL_LIBRARY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT LOGICAL_LIBRARY_LUT_NN  NOT NULL,
  CONSTRAINT LOGICAL_LIBRARY_PK PRIMARY KEY(LOGICAL_LIBRARY_ID),
  CONSTRAINT LOGICAL_LIBRARY_LLN_UN UNIQUE(LOGICAL_LIBRARY_NAME),
  CONSTRAINT LOGICAL_LIBRARY_ID_BOOL_CK CHECK(IS_DISABLED IN ('0', '1'))
);
CREATE TABLE TAPE(
  VID                     VARCHAR(100)    CONSTRAINT TAPE_V_NN    NOT NULL,
  MEDIA_TYPE_ID           BIGINT UNSIGNED      CONSTRAINT TAPE_MTID_NN   NOT NULL,
  VENDOR                  VARCHAR(100)    CONSTRAINT TAPE_V2_NN   NOT NULL,
  LOGICAL_LIBRARY_ID      BIGINT UNSIGNED      CONSTRAINT TAPE_LLI_NN  NOT NULL,
  TAPE_POOL_ID            BIGINT UNSIGNED      CONSTRAINT TAPE_TPI_NN  NOT NULL,
  ENCRYPTION_KEY_NAME     VARCHAR(100),
  DATA_IN_BYTES           BIGINT UNSIGNED      CONSTRAINT TAPE_DIB_NN  NOT NULL,
  LAST_FSEQ               BIGINT UNSIGNED      CONSTRAINT TAPE_LF_NN   NOT NULL,
  NB_MASTER_FILES         BIGINT UNSIGNED      DEFAULT 0 CONSTRAINT TAPE_NB_MASTER_FILES_NN NOT NULL,
  MASTER_DATA_IN_BYTES    BIGINT UNSIGNED      DEFAULT 0 CONSTRAINT TAPE_MASTER_DATA_IN_BYTES_NN NOT NULL,
  IS_DISABLED             CHAR(1)         CONSTRAINT TAPE_ID_NN   NOT NULL,
  IS_FULL                 CHAR(1)         CONSTRAINT TAPE_IF_NN   NOT NULL,
  IS_READ_ONLY            CHAR(1)         CONSTRAINT TAPE_IRO_NN  NOT NULL,
  IS_FROM_CASTOR          CHAR(1)         CONSTRAINT TAPE_IFC_NN  NOT NULL,
  IS_ARCHIVED             CHAR(1)         DEFAULT '0' CONSTRAINT TAPE_IA_NN   NOT NULL,
  IS_EXPORTED             CHAR(1)         DEFAULT '0' CONSTRAINT TAPE_IE_NN   NOT NULL,
  DIRTY                   CHAR(1)         DEFAULT '1' CONSTRAINT TAPE_DIRTY_NN NOT NULL,
  NB_COPY_NB_1            BIGINT UNSIGNED     DEFAULT 0 CONSTRAINT TAPE_NB_COPY_NB_1_NN NOT NULL,
  COPY_NB_1_IN_BYTES      BIGINT UNSIGNED     DEFAULT 0 CONSTRAINT TAPE_COPY_NB_1_IN_BYTES_NN NOT NULL,
  NB_COPY_NB_GT_1         BIGINT UNSIGNED     DEFAULT 0 CONSTRAINT TAPE_NB_COPY_NB_GT_1_NN NOT NULL,
  COPY_NB_GT_1_IN_BYTES   BIGINT UNSIGNED     DEFAULT 0 CONSTRAINT TAPE_COPY_NB_GT_1_IN_BYTES_NN NOT NULL,
  LABEL_DRIVE             VARCHAR(100),
  LABEL_TIME              BIGINT UNSIGNED    ,
  LAST_READ_DRIVE         VARCHAR(100),
  LAST_READ_TIME          BIGINT UNSIGNED    ,
  LAST_WRITE_DRIVE        VARCHAR(100),
  LAST_WRITE_TIME         BIGINT UNSIGNED    ,
  READ_MOUNT_COUNT        BIGINT UNSIGNED      DEFAULT 0 CONSTRAINT TAPE_RMC_NN NOT NULL,
  WRITE_MOUNT_COUNT       BIGINT UNSIGNED      DEFAULT 0 CONSTRAINT TAPE_WMC_NN NOT NULL,
  USER_COMMENT            VARCHAR(1000),
  CREATION_LOG_USER_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR(100)    CONSTRAINT TAPE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME       BIGINT UNSIGNED      CONSTRAINT TAPE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR(100)    CONSTRAINT TAPE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME        BIGINT UNSIGNED      CONSTRAINT TAPE_LUT_NN  NOT NULL,
  CONSTRAINT TAPE_PK PRIMARY KEY(VID),
  CONSTRAINT TAPE_LOGICAL_LIBRARY_FK FOREIGN KEY(LOGICAL_LIBRARY_ID) REFERENCES LOGICAL_LIBRARY(LOGICAL_LIBRARY_ID),
  CONSTRAINT TAPE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_ID) REFERENCES TAPE_POOL(TAPE_POOL_ID),
  CONSTRAINT TAPE_IS_DISABLED_BOOL_CK CHECK(IS_DISABLED IN ('0', '1')),
  CONSTRAINT TAPE_IS_FULL_BOOL_CK CHECK(IS_FULL IN ('0', '1')),
  CONSTRAINT TAPE_IS_READ_ONLY_BOOL_CK CHECK(IS_READ_ONLY IN ('0', '1')),
  CONSTRAINT TAPE_IS_FROM_CASTOR_BOOL_CK CHECK(IS_FROM_CASTOR IN ('0', '1')),
  CONSTRAINT TAPE_IS_ARCHVIED_BOOL_CK CHECK(IS_ARCHIVED IN ('0', '1')),
  CONSTRAINT TAPE_IS_EXPORTED_BOOL_CK CHECK(IS_EXPORTED IN ('0', '1')),
  CONSTRAINT TAPE_DIRTY_BOOL_CK CHECK(DIRTY IN ('0','1')),
  CONSTRAINT TAPE_MEDIA_TYPE_FK FOREIGN KEY(MEDIA_TYPE_ID) REFERENCES MEDIA_TYPE(MEDIA_TYPE_ID)
);
CREATE INDEX TAPE_TAPE_POOL_ID_IDX ON TAPE(TAPE_POOL_ID);
CREATE TABLE MOUNT_POLICY(
  MOUNT_POLICY_NAME        VARCHAR(100)    CONSTRAINT MOUNT_POLICY_MPN_NN  NOT NULL,
  ARCHIVE_PRIORITY         BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_AP_NN   NOT NULL,
  ARCHIVE_MIN_REQUEST_AGE  BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_AMRA_NN NOT NULL,
  RETRIEVE_PRIORITY        BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_RP_NN   NOT NULL,
  RETRIEVE_MIN_REQUEST_AGE BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_RMRA_NN NOT NULL,
  MAX_DRIVES_ALLOWED       BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_MDA_NN  NOT NULL,
  USER_COMMENT             VARCHAR(1000)   CONSTRAINT MOUNT_POLICY_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME   VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME   VARCHAR(100)    CONSTRAINT MOUNT_POLICY_CLHN_NN NOT NULL,
  CREATION_LOG_TIME        BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR(100)    CONSTRAINT MOUNT_POLICY_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME         BIGINT UNSIGNED      CONSTRAINT MOUNT_POLICY_LUT_NN  NOT NULL,
  CONSTRAINT MOUNT_POLICY_PK PRIMARY KEY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR(100)    CONSTRAINT RQSTER_RULE_DIN_NN  NOT NULL,
  REQUESTER_NAME         VARCHAR(100)    CONSTRAINT RQSTER_RULE_RN_NN   NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR(100)    CONSTRAINT RQSTER_RULE_MPN_NN  NOT NULL,
  USER_COMMENT           VARCHAR(1000)   CONSTRAINT RQSTER_RULE_UC_NN   NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLUN_NN NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR(100)    CONSTRAINT RQSTER_RULE_CLHN_NN NOT NULL,
  CREATION_LOG_TIME      BIGINT UNSIGNED      CONSTRAINT RQSTER_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       BIGINT UNSIGNED      CONSTRAINT RQSTER_RULE_LUT_NN  NOT NULL,
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
  CREATION_LOG_TIME      BIGINT UNSIGNED      CONSTRAINT RQSTER_GRP_RULE_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR(100)    CONSTRAINT RQSTER_GRP_RULE_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME       BIGINT UNSIGNED      CONSTRAINT RQSTER_GRP_RULE_LUT_NN  NOT NULL,
  CONSTRAINT RQSTER_GRP_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_GROUP_NAME),
  CONSTRAINT RQSTER_GRP_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID         BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_AFI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DIN_NN  NOT NULL,
  DISK_FILE_ID            VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_DFI_NN  NOT NULL,
  DISK_FILE_UID           INT UNSIGNED      CONSTRAINT ARCHIVE_FILE_DFUID_NN  NOT NULL,
  DISK_FILE_GID           INT UNSIGNED      CONSTRAINT ARCHIVE_FILE_DFGID_NN  NOT NULL,
  SIZE_IN_BYTES           BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_SIB_NN  NOT NULL,
  CHECKSUM_BLOB           VARBINARY(200),
  CHECKSUM_ADLER32        INT UNSIGNED      CONSTRAINT ARCHIVE_FILE_CB2_NN  NOT NULL,
  STORAGE_CLASS_ID        BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_SCI_NN  NOT NULL,
  CREATION_TIME           BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_CT2_NN  NOT NULL,
  RECONCILIATION_TIME     BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RT_NN   NOT NULL,
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
  FSEQ                     BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_F_NN    NOT NULL,
  BLOCK_ID                 BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_BI_NN   NOT NULL,
  LOGICAL_SIZE_IN_BYTES    BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_CSIB_NN NOT NULL,
  COPY_NB                  TINYINT UNSIGNED      CONSTRAINT TAPE_FILE_CN_NN   NOT NULL,
  CREATION_TIME            BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_CT_NN   NOT NULL,
  ARCHIVE_FILE_ID          BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_AFI_NN  NOT NULL,
  SUPERSEDED_BY_VID        VARCHAR(100),
  SUPERSEDED_BY_FSEQ       BIGINT UNSIGNED    ,
  WRITE_START_WRAP         SMALLINT UNSIGNED,
  WRITE_START_LPOS         INT UNSIGNED,
  WRITE_END_WRAP           SMALLINT UNSIGNED,
  WRITE_END_LPOS           INT UNSIGNED,
  READ_START_WRAP          SMALLINT UNSIGNED,
  READ_START_LPOS          INT UNSIGNED,
  READ_END_WRAP            SMALLINT UNSIGNED,
  READ_END_LPOS            INT UNSIGNED,
  CONSTRAINT TAPE_FILE_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_TAPE_FK FOREIGN KEY(VID)
    REFERENCES TAPE(VID),
  CONSTRAINT TAPE_FILE_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
  CONSTRAINT TAPE_FILE_COPY_NB_GT_0_CK CHECK(COPY_NB > 0),
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
  CREATION_LOG_TIME        BIGINT UNSIGNED      CONSTRAINT ACTIV_WEIGHTS_CLT_NN  NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_LUUN_NN NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR(100)    CONSTRAINT ACTIV_WEIGHTS_LUHN_NN NOT NULL,
  LAST_UPDATE_TIME         BIGINT UNSIGNED      CONSTRAINT ACTIV_WEIGHTS_LUT_NN  NOT NULL
);
CREATE TABLE ARCHIVE_FILE_RECYCLE_BIN(
  ARCHIVE_FILE_ID         BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_AFI_NN  NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_RB_DIN_NN  NOT NULL,
  DISK_FILE_ID            VARCHAR(100)    CONSTRAINT ARCHIVE_FILE_RB_DFI_NN  NOT NULL,
  DISK_FILE_ID_WHEN_DELETED VARCHAR(100)  CONSTRAINT ARCHIVE_FILE_RB_DFIWD_NN NOT NULL,
  DISK_FILE_UID           INT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_DFUID_NN  NOT NULL,
  DISK_FILE_GID           INT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_DFGID_NN  NOT NULL,
  SIZE_IN_BYTES           BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_SIB_NN  NOT NULL,
  CHECKSUM_BLOB           VARBINARY(200),
  CHECKSUM_ADLER32        INT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_CB2_NN  NOT NULL,
  STORAGE_CLASS_ID        BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_SCI_NN  NOT NULL,
  CREATION_TIME           BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_CT2_NN  NOT NULL,
  RECONCILIATION_TIME     BIGINT UNSIGNED      CONSTRAINT ARCHIVE_FILE_RB_RT_NN   NOT NULL,
  COLLOCATION_HINT        VARCHAR(100),
  DISK_FILE_PATH          VARCHAR(2000) CONSTRAINT ARCHIVE_FILE_RB_DFP_NN NOT NULL,
  DELETION_TIME           BIGINT UNSIGNED    CONSTRAINT ARCHIVE_FILE_RB_DT_NN NOT NULL,
  CONSTRAINT ARCHIVE_FILE_RB_PK PRIMARY KEY(ARCHIVE_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_RB_SC_FK FOREIGN KEY(STORAGE_CLASS_ID) REFERENCES STORAGE_CLASS(STORAGE_CLASS_ID),
  CONSTRAINT ARCHIVE_FILE_RB_DIN_DFI_UN UNIQUE(DISK_INSTANCE_NAME, DISK_FILE_ID)
);
CREATE TABLE TAPE_FILE_RECYCLE_BIN(
  VID                      VARCHAR(100)   CONSTRAINT TAPE_FILE_RB_V_NN    NOT NULL,
  FSEQ                     BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_RB_F_NN    NOT NULL,
  BLOCK_ID                 BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_RB_BI_NN   NOT NULL,
  LOGICAL_SIZE_IN_BYTES    BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_RB_CSIB_NN NOT NULL,
  COPY_NB                  TINYINT UNSIGNED      CONSTRAINT TAPE_FILE_RB_CN_NN   NOT NULL,
  CREATION_TIME            BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_RB_CT_NN   NOT NULL,
  ARCHIVE_FILE_ID          BIGINT UNSIGNED     CONSTRAINT TAPE_FILE_RB_AFI_NN  NOT NULL,
  SUPERSEDED_BY_VID        VARCHAR(100),
  SUPERSEDED_BY_FSEQ       BIGINT UNSIGNED,
  WRITE_START_WRAP         SMALLINT UNSIGNED,
  WRITE_START_LPOS         INT UNSIGNED,
  WRITE_END_WRAP           SMALLINT UNSIGNED,
  WRITE_END_LPOS           INT UNSIGNED,
  READ_START_WRAP          SMALLINT UNSIGNED,
  READ_START_LPOS          INT UNSIGNED,
  READ_END_WRAP            SMALLINT UNSIGNED,
  READ_END_LPOS            INT UNSIGNED,
  CONSTRAINT TAPE_FILE_RB_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_RB_TAPE_FK FOREIGN KEY(VID)
    REFERENCES TAPE(VID),
  CONSTRAINT TAPE_FILE_RB_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE_RECYCLE_BIN(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_RB_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
  CONSTRAINT TAPE_FILE_RB_COPY_NB_GT_0_CK CHECK(COPY_NB > 0),
  CONSTRAINT TAPE_FILE_RB_SS_VID_FSEQ_FK FOREIGN KEY(SUPERSEDED_BY_VID, SUPERSEDED_BY_FSEQ)
    REFERENCES TAPE_FILE_RECYCLE_BIN(VID, FSEQ)
);
INSERT INTO CTA_CATALOGUE(
  SCHEMA_VERSION_MAJOR,
  SCHEMA_VERSION_MINOR,
  STATUS)
VALUES(
  3,
  0,
  'PRODUCTION');
ALTER TABLE CTA_CATALOGUE ADD CONSTRAINT 
  CATALOGUE_STATUS_CONTENT_CK CHECK((NEXT_SCHEMA_VERSION_MAJOR IS NULL AND NEXT_SCHEMA_VERSION_MINOR IS NULL AND STATUS='PRODUCTION') OR (STATUS='UPGRADING'));