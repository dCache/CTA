set sqlblanklines ON

DROP TABLE ARCHIVE_ROUTE;
DROP TABLE TAPE_FILE;
DROP TABLE ARCHIVE_FILE;
DROP TABLE TAPE;
DROP TABLE END_USER;
DROP TABLE ADMIN_USER;
DROP TABLE ADMIN_HOST;
DROP TABLE STORAGE_CLASS;
DROP TABLE TAPE_POOL;
DROP TABLE LOGICAL_LIBRARY;
DROP TABLE MOUNT_GROUP;

CREATE TABLE ADMIN_USER(
  ADMIN_USER_NAME  VARCHAR2(100) NOT NULL,
  ADMIN_GROUP_NAME VARCHAR2(100) NOT NULL,

  USER_COMMENT VARCHAR2(1000) NOT NULL,

  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(ADMIN_USER_NAME)
);

CREATE TABLE ADMIN_HOST(
  ADMIN_HOST_NAME VARCHAR2(100),

  USER_COMMENT VARCHAR2(1000) NOT NULL,

  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(ADMIN_HOST_NAME)
);

CREATE TABLE STORAGE_CLASS(
  STORAGE_CLASS_NAME VARCHAR2(100) NOT NULL,
  NB_COPIES          INTEGER       NOT NULL,

  USER_COMMENT VARCHAR2(1000) NOT NULL,

  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(STORAGE_CLASS_NAME)
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

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(TAPE_POOL_NAME)
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

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(STORAGE_CLASS_NAME, COPY_NB),
  FOREIGN KEY(STORAGE_CLASS_NAME) REFERENCES 
    STORAGE_CLASS(STORAGE_CLASS_NAME),
  FOREIGN KEY(TAPE_POOL_NAME) REFERENCES 
    TAPE_POOL(TAPE_POOL_NAME)
);

CREATE TABLE LOGICAL_LIBRARY(

  LOGICAL_LIBRARY_NAME VARCHAR2(100) NOT NULL,

  USER_COMMENT VARCHAR2(1000) NOT NULL,

  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(LOGICAL_LIBRARY_NAME)
);

CREATE TABLE TAPE(
  VID                  VARCHAR2(100) NOT NULL,
  LOGICAL_LIBRARY_NAME VARCHAR2(100) NOT NULL,
  TAPE_POOL_NAME       VARCHAR2(100) NOT NULL,
  ENCRYPTION_KEY       VARCHAR2(100) NOT NULL,

  CAPACITY_IN_BYTES    INTEGER NOT NULL,
  DATA_IN_BYTES        INTEGER NOT NULL,
  LAST_FSEQ            INTEGER NOT NULL,
  IS_BUSY              INTEGER NOT NULL,
  IS_DISABLED          INTEGER NOT NULL,
  IS_FULL              INTEGER NOT NULL,
  LBP_IS_ON            INTEGER NOT NULL,

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

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(VID),
  FOREIGN KEY(LOGICAL_LIBRARY_NAME) REFERENCES 
    LOGICAL_LIBRARY(LOGICAL_LIBRARY_NAME),
  FOREIGN KEY(TAPE_POOL_NAME) REFERENCES 
    TAPE_POOL(TAPE_POOL_NAME)
);

CREATE TABLE MOUNT_GROUP(
  MOUNT_GROUP_NAME VARCHAR2(100) NOT NULL,

  ARCHIVE_PRIORITY         INTEGER NOT NULL,
  MIN_ARCHIVE_FILES_QUEUED INTEGER NOT NULL,
  MIN_ARCHIVE_BYTES_QUEUED INTEGER NOT NULL,
  MIN_ARCHIVE_REQUEST_AGE  INTEGER NOT NULL,

  RETRIEVE_PRIORITY         INTEGER NOT NULL,
  MIN_RETRIEVE_FILES_QUEUED INTEGER NOT NULL,
  MIN_RETRIEVE_BYTES_QUEUED INTEGER NOT NULL,
  MIN_RETRIEVE_REQUEST_AGE  INTEGER NOT NULL,

  MAX_DRIVES_ALLOWED INTEGER NOT NULL,

  USER_COMMENT VARCHAR2(1000) NOT NULL,

  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(MOUNT_GROUP_NAME)
);

CREATE TABLE END_USER(
  USER_NAME  VARCHAR2(100) NOT NULL,
  GROUP_NAME VARCHAR2(100) NOT NULL,

  MOUNT_GROUP_NAME VARCHAR2(100) NOT NULL,

  USER_COMMENT VARCHAR2(1000) NOT NULL,

  CREATION_LOG_USER_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_GROUP_NAME VARCHAR2(100) NOT NULL,
  CREATION_LOG_HOST_NAME  VARCHAR2(100) NOT NULL,
  CREATION_LOG_TIME       INTEGER       NOT NULL,

  LAST_MOD_USER_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_GROUP_NAME VARCHAR2(100) NOT NULL,
  LAST_MOD_HOST_NAME  VARCHAR2(100) NOT NULL,
  LAST_MOD_TIME       INTEGER       NOT NULL,

  PRIMARY KEY(USER_NAME),
  FOREIGN KEY(MOUNT_GROUP_NAME) REFERENCES 
    MOUNT_GROUP(MOUNT_GROUP_NAME)
);

CREATE TABLE ARCHIVE_FILE(
  ARCHIVE_FILE_ID    INTEGER NOT NULL,

  DISK_INSTANCE      VARCHAR2(100) NOT NULL,
  DISK_FILE_ID       VARCHAR2(100) NOT NULL,
  FILE_SIZE          INTEGER       NOT NULL,
  CHECKSUM_TYPE      VARCHAR2(100) NOT NULL,
  CHECKSUM_VALUE     VARCHAR2(100) NOT NULL,
  STORAGE_CLASS_NAME VARCHAR2(100) NOT NULL,
  CREATION_TIME      INTEGER       NOT NULL,

  RECOVERY_PATH  VARCHAR2(2000) NOT NULL,
  RECOVERY_OWNER VARCHAR2(100)  NOT NULL,
  RECOVERY_GROUP VARCHAR2(100)  NOT NULL,
  RECOVERY_BLOB  VARCHAR2(100)  NOT NULL,

  PRIMARY KEY(ARCHIVE_FILE_ID),
  FOREIGN KEY(STORAGE_CLASS_NAME) 
    REFERENCES STORAGE_CLASS(STORAGE_CLASS_NAME),
  UNIQUE(DISK_INSTANCE, DISK_FILE_ID)
);

CREATE TABLE TAPE_FILE(
  VID             VARCHAR2(100) NOT NULL,
  FSEQ            INTEGER       NOT NULL,
  BLOCK_ID        INTEGER       NOT NULL,
  CREATION_TIME   INTEGER       NOT NULL,
  ARCHIVE_FILE_ID INTEGER       NOT NULL,

  PRIMARY KEY(VID, FSEQ),
  FOREIGN KEY(ARCHIVE_FILE_ID) 
    REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
  UNIQUE(FSEQ, BLOCK_ID)
);
