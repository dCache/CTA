CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME         VARCHAR2(100)  NOT NULL,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT ADMIN_USER_PK PRIMARY KEY(ADMIN_USER_NAME)
);
CREATE TABLE ADMIN_HOST(
  ADMIN_HOST_NAME         VARCHAR2(100)  NOT NULL,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT ADMIN_HOST_PK PRIMARY KEY(ADMIN_HOST_NAME)
);
CREATE TABLE STORAGE_CLASS(
  DISK_INSTANCE_NAME      VARCHAR2(100)  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR2(100)  NOT NULL,
  NB_COPIES               INTEGER        NOT NULL,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT STORAGE_CLASS_PK PRIMARY KEY(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME)
);
CREATE TABLE TAPE_POOL(
  TAPE_POOL_NAME          VARCHAR2(100)  NOT NULL,
  NB_PARTIAL_TAPES        INTEGER        NOT NULL,
  IS_ENCRYPTED            INTEGER        NOT NULL,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT TAPE_POOL_PK PRIMARY KEY(TAPE_POOL_NAME),
  CONSTRAINT TAPE_POOL_IS_ENCRYPTED_BOOL_CK CHECK(IS_ENCRYPTED = 0 OR IS_ENCRYPTED = 1)
);
CREATE TABLE ARCHIVE_ROUTE(
  DISK_INSTANCE_NAME      VARCHAR2(100)  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR2(100)  NOT NULL,
  COPY_NB                 INTEGER        NOT NULL,
  TAPE_POOL_NAME          VARCHAR2(100)  NOT NULL,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT ARCHIVE_ROUTE_PK PRIMARY KEY(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME, COPY_NB),
  CONSTRAINT ARCHIVE_ROUTE_STORAGE_CLASS_FK FOREIGN KEY(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME)
    REFERENCES STORAGE_CLASS(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME),
  CONSTRAINT ARCHIVE_ROUTE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_NAME)
    REFERENCES TAPE_POOL(TAPE_POOL_NAME)
);
CREATE TABLE LOGICAL_LIBRARY(
  LOGICAL_LIBRARY_NAME    VARCHAR2(100)  NOT NULL,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT LOGICAL_LIBRARY_PK PRIMARY KEY(LOGICAL_LIBRARY_NAME)
);
CREATE TABLE TAPE(
  VID                     VARCHAR2(100)  NOT NULL,
  LOGICAL_LIBRARY_NAME    VARCHAR2(100)  NOT NULL,
  TAPE_POOL_NAME          VARCHAR2(100)  NOT NULL,
  ENCRYPTION_KEY          VARCHAR2(100),
  CAPACITY_IN_BYTES       INTEGER        NOT NULL,
  DATA_IN_BYTES           INTEGER        NOT NULL,
  LAST_FSEQ               INTEGER        NOT NULL,
  IS_DISABLED             INTEGER        NOT NULL,
  IS_FULL                 INTEGER        NOT NULL,
  LBP_IS_ON               INTEGER        NOT NULL,
  LABEL_DRIVE             VARCHAR2(100),
  LABEL_TIME              INTEGER,
  LAST_READ_DRIVE         VARCHAR2(100),
  LAST_READ_TIME          INTEGER,
  LAST_WRITE_DRIVE        VARCHAR2(100),
  LAST_WRITE_TIME         INTEGER,
  USER_COMMENT            VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME       INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME   VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME        INTEGER        NOT NULL,
  CONSTRAINT TAPE_PK PRIMARY KEY(VID),
  CONSTRAINT TAPE_LOGICAL_LIBRARY_FK FOREIGN KEY(LOGICAL_LIBRARY_NAME)
    REFERENCES LOGICAL_LIBRARY(LOGICAL_LIBRARY_NAME),
  CONSTRAINT TAPE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_NAME)
    REFERENCES TAPE_POOL(TAPE_POOL_NAME),
  CONSTRAINT TAPE_IS_DISABLED_BOOL_CK CHECK(IS_DISABLED = 0 OR IS_DISABLED = 1),
  CONSTRAINT TAPE_IS_FULL_BOOL_CK CHECK(IS_FULL = 0 OR IS_FULL = 1),
  CONSTRAINT TAPE_LBP_IS_ON_BOOL_CK CHECK(LBP_IS_ON = 0 OR LBP_IS_ON = 1)
);
CREATE TABLE MOUNT_POLICY(
  MOUNT_POLICY_NAME        VARCHAR2(100)  NOT NULL,
  ARCHIVE_PRIORITY         INTEGER        NOT NULL,
  ARCHIVE_MIN_REQUEST_AGE  INTEGER        NOT NULL,
  RETRIEVE_PRIORITY        INTEGER        NOT NULL,
  RETRIEVE_MIN_REQUEST_AGE INTEGER        NOT NULL,
  MAX_DRIVES_ALLOWED       INTEGER        NOT NULL,
  USER_COMMENT             VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME   VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME   VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME        INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME    VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME    VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME         INTEGER        NOT NULL,
  CONSTRAINT MOUNT_POLICY_PK PRIMARY KEY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR2(100)  NOT NULL,
  REQUESTER_NAME         VARCHAR2(100)  NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR2(100)  NOT NULL,
  USER_COMMENT           VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME      INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME       INTEGER        NOT NULL,
  CONSTRAINT RQSTER_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_NAME),
  CONSTRAINT RQSTER_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_GROUP_MOUNT_RULE(
  DISK_INSTANCE_NAME     VARCHAR2(100)  NOT NULL,
  REQUESTER_GROUP_NAME   VARCHAR2(100)  NOT NULL,
  MOUNT_POLICY_NAME      VARCHAR2(100)  NOT NULL,
  USER_COMMENT           VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME VARCHAR2(100)  NOT NULL,
  CREATION_LOG_HOST_NAME VARCHAR2(100)  NOT NULL,
  CREATION_LOG_TIME      INTEGER        NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100)  NOT NULL,
  LAST_UPDATE_TIME       INTEGER        NOT NULL,
  CONSTRAINT RQSTER_GRP_RULE_PK PRIMARY KEY(DISK_INSTANCE_NAME, REQUESTER_GROUP_NAME),
  CONSTRAINT RQSTER_GRP_RULE_MNT_PLC_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID         INTEGER        NOT NULL,
  DISK_INSTANCE_NAME      VARCHAR2(100)  NOT NULL,
  DISK_FILE_ID            VARCHAR2(100)  NOT NULL,
  DISK_FILE_PATH          VARCHAR2(2000) NOT NULL,
  DISK_FILE_USER          VARCHAR2(100)  NOT NULL,
  DISK_FILE_GROUP         VARCHAR2(100)  NOT NULL,
  DISK_FILE_RECOVERY_BLOB VARCHAR2(2000) NOT NULL,
  FILE_SIZE               INTEGER        NOT NULL,
  CHECKSUM_TYPE           VARCHAR2(100)  NOT NULL,
  CHECKSUM_VALUE          VARCHAR2(100)  NOT NULL,
  STORAGE_CLASS_NAME      VARCHAR2(100)  NOT NULL,
  CREATION_TIME           INTEGER        NOT NULL,
  RECONCILIATION_TIME     INTEGER        NOT NULL,
  CONSTRAINT ARCHIVE_FILE_PK PRIMARY KEY(ARCHIVE_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_STORAGE_CLASS_FK FOREIGN KEY(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME)
    REFERENCES STORAGE_CLASS(DISK_INSTANCE_NAME, STORAGE_CLASS_NAME)
);
CREATE TABLE TAPE_FILE(
  VID             VARCHAR2(100) NOT NULL,
  FSEQ            INTEGER       NOT NULL,
  BLOCK_ID        INTEGER       NOT NULL,
  COMPRESSED_SIZE INTEGER       NOT NULL,
  COPY_NB         INTEGER       NOT NULL,
  CREATION_TIME   INTEGER       NOT NULL,
  ARCHIVE_FILE_ID INTEGER       NOT NULL,
  CONSTRAINT TAPE_FILE_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_TAPE_FK FOREIGN KEY(VID)
    REFERENCES TAPE(VID),
  CONSTRAINT TAPE_FILE_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_VID_BLOCK_ID_UN UNIQUE(VID, BLOCK_ID),
  CONSTRAINT TAPE_FILE_VID_ARCH_FILE_ID_UN UNIQUE(VID, ARCHIVE_FILE_ID)
);
