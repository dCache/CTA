/*                        ORACLE ENTERPRISE EDITION                         */
/*                                                                          */
/* This file contains SQL code that will destroy the dlf database schema    */
/* DBA privileges must be present for removing the scheduled job            */

BEGIN

  -- Purge the recycle bin
  EXECUTE IMMEDIATE 'PURGE RECYCLEBIN';

  -- Drop all objects (ignore monitoring ones!)
  FOR rec IN (SELECT object_name, object_type FROM user_objects
              WHERE  object_name NOT LIKE 'PROC_%'
              AND    object_name NOT LIKE 'MONITORING_%'
              ORDER BY object_name, object_type)
  LOOP
    IF rec.object_type = 'TABLE' THEN
      EXECUTE IMMEDIATE 'DROP TABLE '||rec.object_name||' CASCADE CONSTRAINTS';
    ELSIF rec.object_type = 'PROCEDURE' THEN
      EXECUTE IMMEDIATE 'DROP PROCEDURE '||rec.object_name;
    ELSIF rec.object_type = 'FUNCTION' THEN
      EXECUTE IMMEDIATE 'DROP FUNCTION '||rec.object_name;
    ELSIF rec.object_type = 'PACKAGE' THEN
      EXECUTE IMMEDIATE 'DROP PACKAGE '||rec.object_name;
    ELSIF rec.object_type = 'SEQUENCE' THEN
      EXECUTE IMMEDIATE 'DROP SEQUENCE '||rec.object_name;
    ELSIF rec.object_type = 'TYPE' THEN
      EXECUTE IMMEDIATE 'DROP TYPE "'||rec.object_name||'"';
    ELSIF rec.object_type = 'JOB' THEN
      DBMS_SCHEDULER.DROP_JOB(JOB_NAME => rec.object_name, FORCE => TRUE);
    END IF;
  END LOOP;

  -- Drop tablespaces
  FOR rec IN (SELECT tablespace_name
                FROM user_tablespaces
               WHERE tablespace_name LIKE 'DLF_%'
                 AND tablespace_name NOT IN ('DLF_DATA', 'DLF_IDX', 'DLF_INDX')
                 AND LENGTH(tablespace_name) = 12)
  LOOP
    EXECUTE IMMEDIATE 'ALTER TABLESPACE '||rec.tablespace_name||' OFFLINE';
    EXECUTE IMMEDIATE 'DROP TABLESPACE '||rec.tablespace_name||'
                       INCLUDING CONTENTS AND DATAFILES';
  END LOOP;

  -- Purge the recycle bin
  EXECUTE IMMEDIATE 'PURGE RECYCLEBIN';
END;


/* End-of-File */
