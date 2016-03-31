    CREATE TABLE ADMIN_USER(
      ADMIN_USER_NAME  VARCHAR2(100),
      ADMIN_GROUP_NAME VARCHAR2(100),
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(ADMIN_USER_NAME)
    );
    CREATE TABLE ADMIN_HOST(
      ADMIN_HOST_NAME VARCHAR2(100),
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(ADMIN_HOST_NAME)
    );
    CREATE TABLE STORAGE_CLASS(
      STORAGE_CLASS_NAME VARCHAR2(100),
      NB_COPIES          INTEGER,
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(STORAGE_CLASS_NAME)
    );
    CREATE TABLE TAPE_POOL(
      TAPE_POOL_NAME   VARCHAR2(100),
      NB_PARTIAL_TAPES INTEGER,
      IS_ENCRYPTED     INTEGER,
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(TAPE_POOL_NAME)
    );
    CREATE TABLE ARCHIVE_ROUTE(
      STORAGE_CLASS_NAME VARCHAR2(100),
      COPY_NB            INTEGER,
      TAPE_POOL_NAME     VARCHAR2(100),
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(STORAGE_CLASS_NAME, COPY_NB),
      FOREIGN KEY(STORAGE_CLASS_NAME) REFERENCES 
        STORAGE_CLASS(STORAGE_CLASS_NAME),
      FOREIGN KEY(TAPE_POOL_NAME) REFERENCES 
        TAPE_POOL(TAPE_POOL_NAME)
    );
    CREATE TABLE LOGICAL_LIBRARY(
      LOGICAL_LIBRARY_NAME VARCHAR2(100),
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(LOGICAL_LIBRARY_NAME)
    );
    CREATE TABLE TAPE(
      VID                  VARCHAR2(100),
      LOGICAL_LIBRARY_NAME VARCHAR2(100),
      TAPE_POOL_NAME       VARCHAR2(100),
      ENCRYPTION_KEY       VARCHAR2(100),
      CAPACITY_IN_BYTES    INTEGER,
      DATA_IN_BYTES        INTEGER,
      LAST_FSEQ            INTEGER,
      IS_BUSY              INTEGER,
      IS_DISABLED          INTEGER,
      IS_FULL              INTEGER,
      LBP_IS_ON            INTEGER,
      LABEL_DRIVE VARCHAR2(100),
      LABEL_TIME  INTEGER,
      LAST_READ_DRIVE VARCHAR2(100),
      LAST_READ_TIME  INTEGER,
      LAST_WRITE_DRIVE VARCHAR2(100),
      LAST_WRITE_TIME  INTEGER,
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(VID),
      FOREIGN KEY(LOGICAL_LIBRARY_NAME) REFERENCES 
        LOGICAL_LIBRARY(LOGICAL_LIBRARY_NAME),
      FOREIGN KEY(TAPE_POOL_NAME) REFERENCES 
        TAPE_POOL(TAPE_POOL_NAME)
    );
    CREATE TABLE MOUNT_GROUP(
      MOUNT_GROUP_NAME VARCHAR2(100),
      ARCHIVE_PRIORITY         INTEGER,
      MIN_ARCHIVE_FILES_QUEUED INTEGER,
      MIN_ARCHIVE_BYTES_QUEUED INTEGER,
      MIN_ARCHIVE_REQUEST_AGE  INTEGER,
      RETRIEVE_PRIORITY         INTEGER,
      MIN_RETRIEVE_FILES_QUEUED INTEGER,
      MIN_RETRIEVE_BYTES_QUEUED INTEGER,
      MIN_RETRIEVE_REQUEST_AGE  INTEGER,
      MAX_DRIVES_ALLOWED INTEGER,
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(MOUNT_GROUP_NAME)
    );
    CREATE TABLE END_USER(
      USER_NAME  VARCHAR2(100),
      GROUP_NAME VARCHAR2(100),
      MOUNT_GROUP_NAME VARCHAR2(100),
      USER_COMMENT VARCHAR2(1000),
      CREATION_LOG_USER_NAME  VARCHAR2(100),
      CREATION_LOG_GROUP_NAME VARCHAR2(100),
      CREATION_LOG_HOST_NAME  VARCHAR2(100),
      CREATION_LOG_TIME       INTEGER,
      LAST_MOD_USER_NAME  VARCHAR2(100),
      LAST_MOD_GROUP_NAME VARCHAR2(100),
      LAST_MOD_HOST_NAME  VARCHAR2(100),
      LAST_MOD_TIME       INTEGER,
      PRIMARY KEY(USER_NAME),
      FOREIGN KEY(MOUNT_GROUP_NAME) REFERENCES 
        MOUNT_GROUP(MOUNT_GROUP_NAME)
    );
    CREATE TABLE ARCHIVE_FILE(
      ARCHIVE_FILE_ID    INTEGER,
      DISK_INSTANCE      VARCHAR2(100),
      DISK_FILE_ID       VARCHAR2(100),
      FILE_SIZE          INTEGER,
      CHECKSUM_TYPE      VARCHAR2(100),
      CHECKSUM_VALUE     VARCHAR2(100),
      STORAGE_CLASS_NAME VARCHAR2(100),
      CREATION_TIME      INTEGER,
      RECOVERY_PATH     VARCHAR2(2000),
      RECOVERY_OWNER    VARCHAR2(100),
      RECOVERY_GROUP    VARCHAR2(100),
      RECOVERY_BLOB     VARCHAR2(100),
      PRIMARY KEY(ARCHIVE_FILE_ID),
      FOREIGN KEY(STORAGE_CLASS_NAME) 
        REFERENCES STORAGE_CLASS(STORAGE_CLASS_NAME),
      UNIQUE(DISK_INSTANCE, DISK_FILE_ID)
    );
    CREATE TABLE TAPE_FILE(
      VID             VARCHAR2(100),
      FSEQ            INTEGER,
      BLOCK_ID        INTEGER,
      CREATION_TIME   INTEGER,
      ARCHIVE_FILE_ID INTEGER,
      PRIMARY KEY(VID, FSEQ),
      FOREIGN KEY(ARCHIVE_FILE_ID) 
        REFERENCES ARCHIVE_FILE(ARCHIVE_FILE_ID),
      UNIQUE(FSEQ, BLOCK_ID)
    );
