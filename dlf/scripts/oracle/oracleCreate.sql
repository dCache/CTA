/******************************************************************************
 *              oracleCreate.sql
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * @(#)$RCSfile: oracleCreate.sql,v $ $Release: 1.2 $ $Release$ $Date: 2009/02/06 13:39:59 $ $Author: waldron $
 *
 * This script create a new DLF schema
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* SQL statements for table dlf_version */
CREATE TABLE dlf_version(schemaVersion VARCHAR2(20), release VARCHAR2(20));
INSERT INTO dlf_version VALUES ('2_1_2_0', 'releaseTag');

/* SQL statements for table dlf_sequences */
CREATE TABLE dlf_sequences(seq_name CHAR(15), seq_no NUMBER);

/* SQL statement for table dlf_monitoring */
CREATE TABLE dlf_monitoring(timestamp DATE NOT NULL, h_threads NUMBER, h_messages NUMBER, h_inits NUMBER, h_errors NUMBER, h_connections NUMBER, h_clientpeak NUMBER, h_timeouts NUMBER, db_threads NUMBER, db_commits NUMBER, db_errors NUMBER, db_inserts NUMBER, db_rollbacks NUMBER, db_selects NUMBER, db_updates NUMBER, db_cursors NUMBER, db_messages NUMBER, db_inits NUMBER, db_hashstats NUMBER, s_uptime NUMBER, s_mode NUMBER, s_queued NUMBER, s_response NUMBER(*,4), interval NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table dlf_config */
CREATE TABLE dlf_config(name VARCHAR2(255) NOT NULL, value VARCHAR2(255), description VARCHAR2(255));
ALTER TABLE dlf_config ADD CONSTRAINT i_config_name UNIQUE (name) ENABLE;

/* SQL statements for table dlf_messages */
CREATE TABLE dlf_messages(id NUMBER, timestamp DATE NOT NULL, timeusec NUMBER, reqid CHAR(36), subreqid CHAR(36), hostid NUMBER, facility NUMBER(3), severity NUMBER(3), msg_no NUMBER(5), pid NUMBER(10), tid NUMBER(10), nshostid NUMBER, nsfileid NUMBER, tapevid VARCHAR2(20), userid NUMBER(10), groupid NUMBER(10), sec_type VARCHAR2(20), sec_name VARCHAR2(255))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

CREATE INDEX i_msg_timestamp ON dlf_messages (timestamp) LOCAL;
CREATE INDEX i_msg_fac ON dlf_messages (facility) LOCAL;
CREATE INDEX i_msg_pid ON dlf_messages (pid) LOCAL;
CREATE INDEX i_msg_reqid ON dlf_messages (reqid) LOCAL;
CREATE INDEX i_msg_subreqid ON dlf_messages (subreqid) LOCAL;
CREATE INDEX i_msg_hostid ON dlf_messages (hostid) LOCAL;
CREATE INDEX i_msg_nshostid ON dlf_messages (nshostid) LOCAL;
CREATE INDEX i_msg_fileid ON dlf_messages (nsfileid) LOCAL;
CREATE INDEX i_msg_tapevid ON dlf_messages (tapevid) LOCAL;
CREATE INDEX i_msg_userid ON dlf_messages (userid) LOCAL;
CREATE INDEX i_msg_groupid ON dlf_messages (groupid) LOCAL;
CREATE INDEX i_msg_sec_type ON dlf_messages (sec_type) LOCAL;
CREATE INDEX i_msg_sec_name ON dlf_messages (sec_name) LOCAL;

/* SQL statements for table dlf_num_param_values */
CREATE TABLE dlf_num_param_values(id NUMBER, timestamp DATE NOT NULL, name VARCHAR2(20), value NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

CREATE INDEX i_num_id ON dlf_num_param_values (id) LOCAL;

/* SQL statements for table dlf_str_param_values */
CREATE TABLE dlf_str_param_values(id NUMBER, timestamp DATE NOT NULL, name VARCHAR2(20), value VARCHAR2(2048))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

CREATE INDEX i_str_id ON dlf_str_param_values (id) LOCAL;

/* SQL statements for table dlf_severities */
CREATE TABLE dlf_severities(sev_no NUMBER(3), sev_name VARCHAR2(20));

CREATE UNIQUE INDEX i_sev_no ON dlf_severities (sev_no);
CREATE UNIQUE INDEX i_sev_name ON dlf_severities (sev_name);

ALTER TABLE dlf_severities ADD CONSTRAINT i_sev_no UNIQUE (sev_no) ENABLE;
ALTER TABLE dlf_severities ADD CONSTRAINT i_sev_name UNIQUE (sev_name) ENABLE;

/* SQL statements for table dlf_facilities */
CREATE TABLE dlf_facilities(fac_no NUMBER(3), fac_name VARCHAR2(20));

CREATE UNIQUE INDEX i_fac_no ON dlf_facilities (fac_no);
CREATE UNIQUE INDEX i_fac_name ON dlf_facilities (fac_name);

ALTER TABLE dlf_facilities ADD CONSTRAINT i_fac_no UNIQUE (fac_no) ENABLE;
ALTER TABLE dlf_facilities ADD CONSTRAINT i_fac_name UNIQUE (fac_name) ENABLE;

/* SQL statements for table dlf_msg_texts */
CREATE TABLE dlf_msg_texts(fac_no NUMBER(3), msg_no NUMBER(5), msg_text VARCHAR2(512));

CREATE UNIQUE INDEX i_msg_texts ON dlf_msg_texts (fac_no, msg_no);

/* SQL statements for dlf_host_map */
CREATE TABLE dlf_host_map(hostid NUMBER, hostname VARCHAR2(64));

CREATE UNIQUE INDEX i_hostid ON dlf_host_map (hostid);
CREATE UNIQUE INDEX i_hostname ON dlf_host_map (hostname);

ALTER TABLE dlf_host_map ADD CONSTRAINT i_hostid UNIQUE (hostid) ENABLE;
ALTER TABLE dlf_host_map ADD CONSTRAINT i_hostname UNIQUE (hostname) ENABLE;

/* SQL statements for dlf_nshost_map */
CREATE TABLE dlf_nshost_map(nshostid NUMBER, nshostname VARCHAR2(64));

CREATE UNIQUE INDEX i_nshostid ON dlf_nshost_map (nshostid);
CREATE UNIQUE INDEX i_nshostname ON dlf_nshost_map (nshostname);

ALTER TABLE dlf_nshost_map ADD CONSTRAINT i_nshostid UNIQUE (nshostid) ENABLE;
ALTER TABLE dlf_nshost_map ADD CONSTRAINT i_nshostname UNIQUE (nshostname) ENABLE;

/* Fill the dlf_config table */
INSERT INTO dlf_config (name, value, description) VALUES ('instance', 'castordlf', 'The name of the castor2 instance');
INSERT INTO dlf_config (name, value, description) VALUES ('expiry', '90', 'The expiry time of the logging data in days');

/* Fill the dlf_severities table */
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('1', 'Emergency');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('2', 'Alert');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('3', 'Error');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('4', 'Warning');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('5', 'Auth');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('6', 'Security');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('7', 'Usage');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('8', 'System');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('9', 'Important');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('10', 'Monitoring');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('11', 'Debug');
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('12', 'UserError');

/* Fill the dlf_facilities table */
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (0, 'rtcpcld');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (1, 'migrator');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (2, 'recaller');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (4, 'RequestHandler');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (8, 'GC');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (9, 'Scheduler');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (10, 'TapeErrorHandler');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (11, 'Vdqm');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (12, 'rfio');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (13, 'SRMServer');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (14, 'SRMDaemon');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (15, 'Repack');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (17, 'tpdaemon');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (18, 'rtcpd');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (19, 'RmMaster');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (20, 'RmNode');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (21, 'JobManager');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (22, 'Stager');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (23, 'DiskCopy');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (24, 'Mighunter');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (25, 'Rechandler');
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (26, 'Job');

/* Fill the dlf_sequences table */
INSERT INTO dlf_sequences (seq_name, seq_no) VALUES ('id',       1);
INSERT INTO dlf_sequences (seq_name, seq_no) VALUES ('hostid',   1);
INSERT INTO dlf_sequences (seq_name, seq_no) VALUES ('nshostid', 1);


/* PL/SQL method implementing createPartition */
CREATE OR REPLACE PROCEDURE createPartitions
AS
  username VARCHAR2(2048);
  partitionMax NUMBER;
  tableSpaceName VARCHAR2(2048);
  highValue DATE;
  cnt NUMBER;
BEGIN
  -- Extract the name of the current user running the PL/SQL procedure. This name
  -- will be used within the tablespace names.
  SELECT SYS_CONTEXT('USERENV', 'CURRENT_USER')
    INTO username
    FROM dual;

  -- Loop over all partitioned tables
  FOR a IN (SELECT DISTINCT(table_name)
              FROM user_tab_partitions
             ORDER BY table_name)
  LOOP
    -- Determine the high value on which to split the MAX_VALUE partition of all
    -- tables
    SELECT max(substr(partition_name, 3, 10))
      INTO partitionMax
      FROM user_tab_partitions
     WHERE partition_name <> 'MAX_VALUE'
       AND table_name = a.table_name;

    partitionMax := TO_NUMBER(
      TO_CHAR(TRUNC(TO_DATE(partitionMax, 'YYYYMMDD') + 1), 'YYYYMMDD'));

    -- If this is the first execution there will be no high value, so set it to
    -- today
    IF partitionMax IS NULL THEN
      partitionMax := TO_NUMBER(TO_CHAR(SYSDATE, 'YYYYMMDD'));
    END IF;

    -- Create partition
    FOR b IN (SELECT TO_DATE(partitionMax, 'YYYYMMDD') + rownum - 1 value
                FROM all_objects
               WHERE rownum <=
                     TO_DATE(TO_CHAR(SYSDATE + 7, 'YYYYMMDD'), 'YYYYMMDD') -
                     TO_DATE(partitionMax, 'YYYYMMDD') + 1)
    LOOP
      -- To improve data management each daily partition has its own tablespace
      -- http://www.oracle.com/technology/oramag/oracle/06-sep/o56partition.html

      -- Check if a new tablespace is required before creating the partition
      tableSpaceName := 'DLF_'||TO_CHAR(b.value, 'YYYYMMDD')||'_'||username;
      SELECT count(*) INTO cnt
        FROM user_tablespaces
       WHERE tablespace_name = tableSpaceName;

      IF cnt = 0 THEN
        EXECUTE IMMEDIATE 'CREATE TABLESPACE '||tableSpaceName||'
                           DATAFILE SIZE 100M
                           AUTOEXTEND ON NEXT 200M
                           MAXSIZE 30G
                           EXTENT MANAGEMENT LOCAL
                           SEGMENT SPACE MANAGEMENT AUTO';
      END IF;

      -- If the tablespace is read only, alter its status to read write for this
      -- operation.
      FOR d IN (SELECT tablespace_name FROM user_tablespaces
                 WHERE tablespace_name = tableSpaceName
                   AND status = 'READ ONLY')
      LOOP
        EXECUTE IMMEDIATE 'ALTER TABLESPACE '||d.tablespace_name||' READ WRITE';
      END LOOP;

      highValue := TRUNC(b.value + 1);
      EXECUTE IMMEDIATE 'ALTER TABLE '||a.table_name||'
                         SPLIT PARTITION MAX_VALUE
                         AT    ('''||TO_CHAR(highValue, 'DD-MON-YYYY')||''')
                         INTO  (PARTITION P_'||TO_CHAR(b.value, 'YYYYMMDD')||'
                                TABLESPACE '||tableSpaceName||',
                                PARTITION MAX_VALUE)
                         UPDATE INDEXES';

      -- Move indexes to the correct tablespace
      FOR c IN (SELECT index_name
                  FROM user_indexes
                 WHERE table_name = a.table_name)
      LOOP
        EXECUTE IMMEDIATE 'ALTER INDEX '||c.index_name||'
                           REBUILD PARTITION P_'||TO_CHAR(b.value, 'YYYYMMDD')||'
                           TABLESPACE '||tableSpaceName;
      END LOOP;
    END LOOP;
  END LOOP;
END;
/


/* PL/SQL method implementing archiveData */
CREATE OR REPLACE PROCEDURE archiveData (expiry IN NUMBER)
AS
  username VARCHAR2(2048);
  expiryTime NUMBER;
BEGIN
  -- Extract the name of the current user running the PL/SQL procedure. This name
  -- will be used within the tablespace names.
  SELECT SYS_CONTEXT('USERENV', 'CURRENT_USER')
    INTO username
    FROM dual;

  -- Extract configurable expiry time
  expiryTime := expiry;
  IF expiryTime = -1 THEN
    SELECT TO_NUMBER(value) INTO expiryTime
      FROM dlf_config
     WHERE name = 'expiry';
  END IF;

  -- Drop partitions across all tables
  FOR a IN (SELECT *
              FROM user_tab_partitions
             WHERE partition_name < concat('P_', TO_CHAR(SYSDATE - expiryTime, 'YYYYMMDD'))
               AND partition_name <> 'MAX_VALUE'
             ORDER BY partition_name DESC)
  LOOP
    EXECUTE IMMEDIATE 'ALTER TABLE '||a.table_name||'
                       DROP PARTITION '||a.partition_name;
  END LOOP;

  -- Drop tablespaces
  FOR a IN (SELECT tablespace_name
              FROM user_tablespaces
             WHERE status <> 'OFFLINE'
               AND tablespace_name LIKE 'DLF_%_'||username
               AND tablespace_name < 'DLF_'||TO_CHAR(SYSDATE - expiryTime, 'YYYYMMDD')||'_'||username
             ORDER BY tablespace_name ASC)
  LOOP
    EXECUTE IMMEDIATE 'ALTER TABLESPACE '||a.tablespace_name||' OFFLINE';
    EXECUTE IMMEDIATE 'DROP TABLESPACE '||a.tablespace_name||'
                       INCLUDING CONTENTS AND DATAFILES';
  END LOOP;

  -- Set the status of tablespaces older then 2 days to read only.
  FOR a IN (SELECT tablespace_name
              FROM user_tablespaces
             WHERE status <> 'OFFLINE'
	       AND status <> 'READ ONLY'
               AND tablespace_name LIKE 'DLF_%_'||username
               AND tablespace_name < 'DLF_'||TO_CHAR(SYSDATE - 2, 'YYYYMMDD')||'_'||username
             ORDER BY tablespace_name ASC)
  LOOP
    EXECUTE IMMEDIATE 'ALTER TABLESPACE '||a.tablespace_name||' READ ONLY';
  END LOOP;
END;
/


/* Remove scheduler jobs before recreation */
BEGIN
  FOR a IN (SELECT job_name FROM user_scheduler_jobs)
  LOOP
    DBMS_SCHEDULER.DROP_JOB(a.job_name, TRUE);
  END LOOP;
END;
/


BEGIN
  -- Create a db job to be run every day and create new partitions
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'partitionCreationJob',
      JOB_TYPE        => 'STORED_PROCEDURE',
      JOB_ACTION      => 'createPartitions',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => TRUNC(SYSDATE) + 1/24,
      REPEAT_INTERVAL => 'FREQ=DAILY',
      ENABLED         => TRUE,
      COMMENTS        => 'Daily partitioning creation');

  -- Create a db job to be run every day and drop old data from the database
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'archiveDataJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN archiveData(-1); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => TRUNC(SYSDATE) + 2/24,
      REPEAT_INTERVAL => 'FREQ=DAILY',
      ENABLED         => TRUE,
      COMMENTS        => 'Daily data archiving');
END;
/


/* Trigger the initial creation of partitions */
BEGIN
  createPartitions();
END;
/


/* End-of-File */
