CREATE TABLE ARCHIVE_FILE_ID(
  ID UINT64TYPE,
  CONSTRAINT ARCHIVE_FILE_ID_PK PRIMARY KEY(ID)
);
INSERT INTO ARCHIVE_FILE_ID(ID) VALUES(1);
CREATE TABLE LOGICAL_LIBRARY_ID(
  ID UINT64TYPE,
  CONSTRAINT LOGICAL_LIBRARY_ID_PK PRIMARY KEY(ID)
);
INSERT INTO LOGICAL_LIBRARY_ID(ID) VALUES(1);
CREATE TABLE MEDIA_TYPE_ID(
  ID UINT64TYPE,
  CONSTRAINT MEDIA_TYPE_ID_PK PRIMARY KEY(ID)
);
INSERT INTO MEDIA_TYPE_ID(ID) VALUES(1);
CREATE TABLE STORAGE_CLASS_ID(
  ID UINT64TYPE,
  CONSTRAINT STORAGE_CLASS_ID_PK PRIMARY KEY(ID)
);
INSERT INTO STORAGE_CLASS_ID(ID) VALUES(1);
CREATE TABLE TAPE_POOL_ID(
  ID UINT64TYPE,
  CONSTRAINT TAPE_POOL_ID_PK PRIMARY KEY(ID)
);
INSERT INTO TAPE_POOL_ID(ID) VALUES(1);
CREATE TABLE VIRTUAL_ORGANIZATION_ID(
  ID UINT64TYPE,
  CONSTRAINT VIRTUAL_ORGANIZATION_ID_PK PRIMARY KEY(ID)
);
INSERT INTO VIRTUAL_ORGANIZATION_ID(ID) VALUES(1);
CREATE TABLE FILE_RECYCLE_LOG_ID(
  ID UINT64TYPE,
  CONSTRAINT FILE_RECYCLE_LOG_ID PRIMARY KEY(ID)
);
INSERT INTO FILE_RECYCLE_LOG_ID(ID) VALUES (1);
