/*******************************************************************
 *
 * @(#)$RCSfile: castor_oracle_drop.sql,v $ $Revision: 1.45 $ $Date: 2008/11/24 17:39:55 $ $Author: waldron $
 *
 * This file drops all defined entities from a (stager) database.
 * For the time being, it does NOT drop the monitoring-related ones. 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

BEGIN

  -- Purge the recycle bin
  EXECUTE IMMEDIATE 'PURGE RECYCLEBIN';
 
  -- Drop all objects (ignore monitoring ones!)
  FOR rec IN (SELECT object_name, object_type
                FROM user_objects
               WHERE object_name NOT LIKE 'SYS_PLSQL_%'
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
      EXECUTE IMMEDIATE 'DROP TYPE "'||rec.object_name||'" FORCE';
    ELSIF rec.object_type = 'JOB' THEN
      DBMS_SCHEDULER.DROP_JOB(JOB_NAME => rec.object_name, FORCE => TRUE);
    END IF;
  END LOOP;

  -- Purge the recycle bin
  EXECUTE IMMEDIATE 'PURGE RECYCLEBIN';
END;
/