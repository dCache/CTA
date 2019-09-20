CREATE SEQUENCE ARCHIVE_FILE_ID_SEQ
  INCREMENT BY 1
  START WITH 4294967296
  NOMAXVALUE
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;
CREATE SEQUENCE STORAGE_CLASS_ID_SEQ
  INCREMENT BY 1
  START WITH 4294967296
  NOMAXVALUE
  MINVALUE 1
  NOCYCLE
  CACHE 20
  NOORDER;
CREATE GLOBAL TEMPORARY TABLE TEMP_TAPE_FILE_INSERTION_BATCH(
  VID                   VARCHAR(100),
  FSEQ                  UINT64TYPE  ,
  BLOCK_ID              UINT64TYPE  ,
  LOGICAL_SIZE_IN_BYTES UINT64TYPE  ,
  COPY_NB               UINT64TYPE  ,
  CREATION_TIME         UINT64TYPE  ,
  ARCHIVE_FILE_ID       UINT64TYPE    
)
ON COMMIT DELETE ROWS;
CREATE INDEX TEMP_T_F_I_B_ARCHIVE_FILE_ID_I ON TEMP_TAPE_FILE_INSERTION_BATCH(ARCHIVE_FILE_ID);
