CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME VARCHAR2(100) NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT ADMIN_USER_PK PRIMARY KEY(ADMIN_USER_NAME)
);
CREATE TABLE ADMIN_HOST(
  ADMIN_HOST_NAME VARCHAR2(100),
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT ADMIN_HOST_PK PRIMARY KEY(ADMIN_HOST_NAME)
);
CREATE TABLE STORAGE_CLASS(
  STORAGE_CLASS_NAME VARCHAR2(100) NOT NULL,
  NB_COPIES          INTEGER       NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT STORAGE_CLASS_PK PRIMARY KEY(STORAGE_CLASS_NAME)
);
CREATE TABLE TAPE_POOL(
  TAPE_POOL_NAME   VARCHAR2(100) NOT NULL,
  NB_PARTIAL_TAPES INTEGER       NOT NULL,
  IS_ENCRYPTED     INTEGER       NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT TAPE_POOL_PK PRIMARY KEY(TAPE_POOL_NAME)
);
CREATE TABLE ARCHIVE_ROUTE(
  STORAGE_CLASS_NAME VARCHAR2(100) NOT NULL,
  COPY_NB            INTEGER       NOT NULL,
  TAPE_POOL_NAME     VARCHAR2(100) NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT ARCHIVE_ROUTE_PK PRIMARY KEY(STORAGE_CLASS_NAME, COPY_NB),
  CONSTRAINT ARCHIVE_ROUTE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_NAME)
    REFERENCES STORAGE_CLASS(STORAGE_CLASS_NAME),
  CONSTRAINT ARCHIVE_ROUTE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_NAME)
    REFERENCES TAPE_POOL(TAPE_POOL_NAME)
);
CREATE TABLE LOGICAL_LIBRARY(
  LOGICAL_LIBRARY_NAME VARCHAR2(100) NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT LOGICAL_LIBRARY_PK PRIMARY KEY(LOGICAL_LIBRARY_NAME)
);
CREATE TABLE TAPE(
  VID                  VARCHAR2(100) NOT NULL,
  LOGICAL_LIBRARY_NAME VARCHAR2(100) NOT NULL,
  TAPE_POOL_NAME       VARCHAR2(100) NOT NULL,
  ENCRYPTION_KEY       VARCHAR2(100) NOT NULL,
  CAPACITY_IN_BYTES    INTEGER       NOT NULL,
  DATA_IN_BYTES        INTEGER       NOT NULL,
  LAST_FSEQ            INTEGER       NOT NULL,
  IS_DISABLED          INTEGER       NOT NULL,
  IS_FULL              INTEGER       NOT NULL,
  LBP_IS_ON            INTEGER       NOT NULL,
  LABEL_DRIVE VARCHAR2(100) NOT NULL,
  LABEL_TIME  INTEGER NOT NULL,
  LAST_READ_DRIVE  VARCHAR2(100) NOT NULL,
  LAST_READ_TIME   INTEGER       NOT NULL,
  LAST_WRITE_DRIVE VARCHAR2(100) NOT NULL,
  LAST_WRITE_TIME  INTEGER       NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT TAPE_PK PRIMARY KEY(VID),
  CONSTRAINT TAPE_LOGICAL_LIBRARY_FK FOREIGN KEY(LOGICAL_LIBRARY_NAME)
    REFERENCES LOGICAL_LIBRARY(LOGICAL_LIBRARY_NAME),
  CONSTRAINT TAPE_TAPE_POOL_FK FOREIGN KEY(TAPE_POOL_NAME)
    REFERENCES TAPE_POOL(TAPE_POOL_NAME)
);
CREATE TABLE MOUNT_POLICY(
  MOUNT_POLICY_NAME VARCHAR2(100) NOT NULL,
  ARCHIVE_PRIORITY        INTEGER NOT NULL,
  ARCHIVE_MIN_REQUEST_AGE INTEGER NOT NULL,
  RETRIEVE_PRIORITY        INTEGER NOT NULL,
  RETRIEVE_MIN_REQUEST_AGE INTEGER NOT NULL,
  MAX_DRIVES_ALLOWED INTEGER NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT MOUNT_POLICY_PK PRIMARY KEY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER(
  REQUESTER_NAME VARCHAR2(100) NOT NULL,
  MOUNT_POLICY_NAME VARCHAR2(100) NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT REQUESTER_PK PRIMARY KEY(REQUESTER_NAME),
  CONSTRAINT REQUESTER_MOUNT_POLICY_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE REQUESTER_GROUP(
  REQUESTER_GROUP_NAME VARCHAR2(100) NOT NULL,
  MOUNT_POLICY_NAME VARCHAR2(100) NOT NULL,
  USER_COMMENT VARCHAR2(1000) NOT NULL,
  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,
  LAST_UPDATE_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_UPDATE_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_UPDATE_TIME       INTEGER       NOT NULL,
  CONSTRAINT REQUESTER_GROUP_PK PRIMARY KEY(REQUESTER_GROUP_NAME),
  CONSTRAINT REQUESTER_GRP_MOUNT_POLICY_FK FOREIGN KEY(MOUNT_POLICY_NAME)
    REFERENCES MOUNT_POLICY(MOUNT_POLICY_NAME)
);
CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID    INTEGER       NOT NULL,
  DISK_INSTANCE      VARCHAR2(100) NOT NULL,
  DISK_FILE_ID       VARCHAR2(100) NOT NULL,
  FILE_SIZE          INTEGER       NOT NULL,
  CHECKSUM_TYPE      VARCHAR2(100) NOT NULL,
  CHECKSUM_VALUE     VARCHAR2(100) NOT NULL,
  STORAGE_CLASS_NAME VARCHAR2(100) NOT NULL,
  CREATION_TIME      INTEGER       NOT NULL,
  LAST_UPDATE_TIME   INTEGER       NOT NULL,
  RECOVERY_PATH  VARCHAR2(2000) NOT NULL,
  RECOVERY_OWNER VARCHAR2(100)  NOT NULL,
  RECOVERY_GROUP VARCHAR2(100)  NOT NULL,
  RECOVERY_BLOB  VARCHAR2(100)  NOT NULL,
  CONSTRAINT ARCHIVE_FILE_PK PRIMARY KEY(ARCHIVE_FILE_ID),
  CONSTRAINT ARCHIVE_FILE_STORAGE_CLASS_FK FOREIGN KEY(STORAGE_CLASS_NAME) 
    REFERENCES STORAGE_CLASS(STORAGE_CLASS_NAME),
  CONSTRAINT ARCHIVE_FILE_DSK_INST_FILE_UN UNIQUE(DISK_INSTANCE, DISK_FILE_ID)
);
CREATE TABLE TAPE_FILE(
  VID             VARCHAR2(100) NOT NULL,
  FSEQ            INTEGER       NOT NULL,
  BLOCK_ID        INTEGER       NOT NULL,
  CREATION_TIME   INTEGER       NOT NULL,
  ARCHIVE_FILE_ID INTEGER       NOT NULL,
  CONSTRAINT TAPE_FILE_PK PRIMARY KEY(VID, FSEQ),
  CONSTRAINT TAPE_FILE_ARCHIVE_FILE_FK FOREIGN KEY(ARCHIVE_FILE_ID)
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  CONSTRAINT TAPE_FILE_FSEQ_BLOCK_ID_UN UNIQUE(FSEQ, BLOCK_ID)
);
