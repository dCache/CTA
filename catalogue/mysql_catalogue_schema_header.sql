CREATE TABLE ARCHIVE_FILE_ID(
  ID NUMERIC(20, 0),
  CONSTRAINT ARCHIVE_FILE_ID_PK PRIMARY KEY(ID)
);
INSERT INTO ARCHIVE_FILE_ID(ID) VALUES(1);
CREATE TABLE STORAGE_CLASS_ID(
  ID NUMERIC(20, 0),
  CONSTRAINT STORAGE_CLASS_ID_PK PRIMARY KEY(ID)
);
INSERT INTO STORAGE_CLASS_ID(ID) VALUES(1);
