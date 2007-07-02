/******************************************************************************
 *              dlf_2.1.3-17_to_2.1.3-18.sql
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
 * @(#)$RCSfile: dlf_2.1.3-17_to_2.1.3-23.sql,v $ $Revision: 1.1 $ $Release$ $Date: 2007/07/02 06:43:39 $ $Author $
 *
 * This script upgrades a CASTOR v2.1.3-17 DLF database to 2.1.3-18
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works on sqlplus and sql developer */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  found INTEGER;
BEGIN
  SELECT count(*) INTO found FROM dlf_version WHERE release = '2_1_3_17' OR release LIKE '%2.2%';
  IF found = 0 THEN
    -- Error, we can't apply this script
   raise_application_error(-20000, 'PL/SQL revision mismatch. Please run previous upgrade scripts before this one.');
  END IF;
END;


/* upgrade dlf_version to new model */
UPDATE dlf_version SET release = '2_1_3_18';

/* remove deprecated option */
DELETE FROM dlf_settings WHERE name = 'ARCHIVE_MODE';

/* remove scheduler jobs, these will be recreated later */
BEGIN
  DBMS_SCHEDULER.DROP_JOB(JOB_NAME => 'dlf_partition_job', FORCE => TRUE);
  DBMS_SCHEDULER.DROP_JOB(JOB_NAME => 'dlf_archive_job', FORCE => TRUE);
  DBMS_SCHEDULER.DROP_JOB(JOB_NAME => 'dlf_stats_5mins', FORCE => TRUE);
  DBMS_SCHEDULER.DROP_JOB(JOB_NAME => 'dlf_stats_15mins', FORCE => TRUE);
END;

 
/*
 * dlf_partition procedure
 */
CREATE OR REPLACE PROCEDURE DLF_PARTITION 
AS

  -- Variables
  v_partition_max INTEGER;
  v_high_value	  DATE;
  v_table_name	  VARCHAR2(20);
  v_tablespace    VARCHAR2(20);
  v_exists        INTEGER;
  v_split_name    VARCHAR2(20);

BEGIN

  -- Set the nls_date_format
  EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "DD-MON-YYYY"';

  -- Loop over all partitioned tables
  FOR a IN (SELECT DISTINCT(table_name)
              FROM user_tab_partitions
             WHERE table_name LIKE 'DLF_%'
             ORDER BY table_name)
  LOOP

    -- Determine the high value on which to split the MAX_VALUE partition
    SELECT MAX(SUBSTR(PARTITION_NAME, 3, 10))
      INTO v_partition_max
      FROM user_tab_partitions
     WHERE partition_name <> 'MAX_VALUE'
       AND table_name = a.table_name;

      v_partition_max := TO_NUMBER(
        TO_CHAR(TRUNC(TO_DATE(v_partition_max, 'YYYYMMDD') + 1), 'YYYYMMDD'));

    -- If this is the first execution there will be no high value, so
    -- set it to today
    IF v_partition_max IS NULL THEN
      v_partition_max := TO_NUMBER(TO_CHAR(SYSDATE, 'YYYYMMDD'));
    END IF;
 
    -- Create partitions
    FOR b IN (SELECT TO_DATE(v_partition_max, 'YYYYMMDD') + rownum - 1 value
                FROM all_objects
               WHERE rownum <= 
                     TO_DATE(TO_CHAR(SYSDATE + 7, 'YYYYMMDD'), 'YYYYMMDD') -
                     TO_DATE(v_partition_max, 'YYYYMMDD') + 1)
    LOOP

      -- To improve data management each daily partition has its own tablespace
      -- http://www.oracle.com/technology/oramag/oracle/06-sep/o56partition.html

      -- Check if a new tablespace is required before creating the partition
      SELECT COUNT(*) INTO v_exists 
        FROM  user_tablespaces
       WHERE tablespace_name = 'DLF_'||TO_CHAR(b.value, 'YYYYMMDD');
      
      IF v_exists = 0 THEN
        EXECUTE IMMEDIATE 'CREATE TABLESPACE DLF_'||TO_CHAR(b.value, 'YYYYMMDD')||'
                           DATAFILE SIZE 50M
                           AUTOEXTEND ON NEXT 100M 
                           MAXSIZE 10G
                           EXTENT MANAGEMENT LOCAL 
                           SEGMENT SPACE MANAGEMENT AUTO';
      END IF;      
    
      v_high_value := TRUNC(b.value + 1);
      EXECUTE IMMEDIATE 'ALTER TABLE '||a.table_name||' 
                         SPLIT PARTITION MAX_VALUE 
                         AT    ('''||TO_CHAR(v_high_value, 'DD-MON-YYYY')||''') 
                         INTO  (PARTITION P_'||TO_CHAR(b.value, 'YYYYMMDD')||'
                                TABLESPACE DLF_'||TO_CHAR(b.value, 'YYYYMMDD')||', 
                                PARTITION MAX_VALUE)
                         UPDATE INDEXES';
    END LOOP;
  END LOOP;
  
  -- Set the status of tablespaces older then 2 days to read only
  v_tablespace := 'DLF_'||TO_CHAR(SYSDATE - 2, 'YYYYMMDD');
  FOR a IN (SELECT DISTINCT(tablespace_name)
              FROM user_tablespaces
             WHERE tablespace_name LIKE 'DLF_%'
               AND tablespace_name NOT IN ('DLF_DATA', 'DLF_IDX', 'DLF_INDX')
               AND tablespace_name < v_tablespace
               AND status <> 'READ ONLY')
  LOOP
    EXECUTE IMMEDIATE 'ALTER TABLESPACE '||a.tablespace_name||' READ ONLY';
  END LOOP;
END;


/*
 * dlf_partition scheduler
 */
BEGIN
DBMS_SCHEDULER.CREATE_JOB (
	JOB_NAME 	=> 'dlf_partition_job',
	JOB_TYPE 	=> 'STORED_PROCEDURE',
	JOB_ACTION	=> 'DLF_PARTITION',
	START_DATE 	=> TRUNC(SYSDATE) + 1/24,
	REPEAT_INTERVAL => 'FREQ=DAILY',
	ENABLED 	=> TRUE,
	COMMENTS	=> 'Daily partitioning procedure');
END;


/* trigger a partition run immediately */
BEGIN
	dlf_partition();
END;


/*
 * archive/backup procedure 
 */
CREATE OR REPLACE PROCEDURE dlf_archive
AS

  -- Variables
  v_expire      NUMBER := 0;
  v_value       VARCHAR2(255);

BEGIN

  -- Set the nls_date_format
  EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "DD-MON-YYYY"';

  SELECT value INTO v_value FROM dlf_settings WHERE name = 'ARCHIVE_EXPIRY';
  v_expire := TO_NUMBER(v_value);

  -- Drop partition
  IF v_expire > 0 THEN
    FOR a IN (SELECT table_name, partition_name
                FROM user_tab_partitions
               WHERE partition_name = CONCAT('P_', TO_CHAR(SYSDATE - v_expire, 'YYYYMMDD'))
                 AND table_name IN ('DLF_MESSAGES', 'DLF_NUM_PARAM_VALUES', 'DLF_STR_PARAM_VALUES')
               ORDER BY partition_name)
    LOOP
      EXECUTE IMMEDIATE 'ALTER TABLE '||a.table_name||'
                         DROP PARTITION '||a.partition_name;
    END LOOP;
  END IF;
END;


/*
 * dlf_archive scheduler
 */
BEGIN
DBMS_SCHEDULER.CREATE_JOB (
	JOB_NAME 	=> 'dlf_archive_job',
	JOB_TYPE 	=> 'STORED_PROCEDURE',
	JOB_ACTION 	=> 'DLF_ARCHIVE',
	START_DATE 	=> TRUNC(SYSDATE) + 2/24,
	REPEAT_INTERVAL => 'FREQ=DAILY',
	ENABLED 	=> TRUE,
	COMMENTS	=> 'Daily archiving procedure');
END;


/*
 * dlf_stats_jobs (Populates table: dlf_jobstats)
 */
CREATE OR REPLACE PROCEDURE dlf_stats_jobs
AS

  -- Variables
  v_time	DATE;
  v_mode        NUMBER := 0;
  v_value       VARCHAR2(255);

BEGIN

  -- Job stats enabled ?
  SELECT value INTO v_value FROM dlf_settings WHERE name = 'ENABLE_JOBSTATS';
  v_mode := TO_NUMBER(v_value);

  IF v_mode = 1 THEN
    -- Set the nls_date_format
    EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "YYYYMMDDHH24MISS"';

    -- We define the time as a variable so that all data recorded will be synchronised
    v_time := TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS');

    FOR i IN 35..40 LOOP
      IF ((i = 35)  OR
          (i = 36)  OR
          (i = 37)  OR
          (i = 40)) THEN
            INSERT INTO dlf_jobstats VALUES(v_time, i, 0, 0, 0, 0, 0, 0, 900);
      END IF;
    END LOOP;

    -- Update the exiting statistics data
    FOR a IN (SELECT p.value, AVG(diff) avg, STDDEV_POP(diff) stddev, MIN(diff) min,
	  	     MAX(diff) max, COUNT(*) count
                   
              FROM (SELECT id, reqid, timestamp, msg_no,
                    LEAD(TO_NUMBER(timestamp), 1)
                      OVER (PARTITION BY reqid ORDER by timestamp, msg_no) next_timestamp,
                    LEAD(TO_NUMBER(timestamp), 1)
                      OVER (ORDER BY reqid) - TO_NUMBER(timestamp) diff
              FROM (

		  -- Intersection of request ids
		  SELECT * FROM dlf_messages WHERE reqid IN(
		    SELECT reqid FROM dlf_messages
                     WHERE timestamp >  TO_DATE(SYSDATE - 25/1440, 'YYYYMMDDHH24MISS')
                       AND timestamp <= TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
                       AND facility = 5
                       AND msg_no   = 15
                  INTERSECT
                    SELECT reqid FROM dlf_messages
                     WHERE timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
                       AND facility = 5
                       AND msg_no   = 12)
	 	  AND timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
		  AND facility = 5
		  AND ((msg_no = 12) OR (msg_no = 15))

		  ) ORDER BY reqid
	      ) m, dlf_num_param_values p

	      WHERE m.msg_no = 12
	        AND m.id = p.id
                AND p.timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
	        AND p.name = 'type'
	        AND p.value IN (35, 36, 37, 40)
	      GROUP BY p.value ORDER BY p.value)
    LOOP
      EXECUTE IMMEDIATE 'UPDATE dlf_jobstats SET
                                min_time  = '||a.min||',
                                max_time  = '||a.max||',
                                avg_time  = '||a.avg||',
                                std_dev   = '||a.stddev||',
                                exiting   = '||a.count||'
			  WHERE timestamp = '''||v_time||'''
			    AND type      = '||a.value;    
    END LOOP;
       
    -- Update the starting statistics data
    FOR a IN (SELECT p.value, COUNT(m.reqid) count 
                FROM dlf_messages m, dlf_num_param_values p
               WHERE m.timestamp >  TO_DATE(SYSDATE - 25/1440, 'YYYYMMDDHH24MISS')
                 AND m.timestamp <= TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
                 AND m.facility = 5
                 AND m.msg_no   = 12
                 AND m.id       = p.id
                 AND p.name     = 'type'
               GROUP BY p.value)
    LOOP
      EXECUTE IMMEDIATE 'UPDATE dlf_jobstats SET starting = '||a.count||'
                          WHERE timestamp = '''||v_time||'''
                            AND type = '||a.value;
    END LOOP;
  END IF;
END;


/*
 * dlf_stats_requests (Populates table: dlf_reqstats)
 */
CREATE OR REPLACE PROCEDURE dlf_stats_requests
AS

  -- Variables
  v_time	DATE;
  v_mode        NUMBER := 0;
  v_value       VARCHAR2(255);

BEGIN

  -- Request stats enabled ?
  SELECT value INTO v_value FROM dlf_settings WHERE name = 'ENABLE_REQSTATS';
  v_mode := TO_NUMBER(v_value);

  IF v_mode = 1 THEN
    -- Set the nls_date_format
    EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "YYYYMMDDHH24MISS"';

    -- We define the time as a variable so that all data recorded will be synchronised
    v_time := TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS');

    FOR i IN 35..40 LOOP
      IF ((i = 35)  OR
          (i = 36)  OR
          (i = 37)  OR
          (i = 40)) THEN
            INSERT INTO dlf_reqstats VALUES(v_time, i, 0, 0, 0, 0, 0, 900);    
      END IF;
    END LOOP;
  
    -- Update the statistics data
    FOR a IN (SELECT type, avg(proctime) avg, stddev_pop(proctime) stddev, min(proctime) min,
		     max(proctime) max, count(*) count
	      FROM (

		  SELECT m.msg_no, (p.value * 0.001) proctime,
			  LEAD (p.value, 1) 
			  OVER (PARTITION BY m.reqid ORDER BY m.timestamp, m.msg_no) type
		  FROM (

                    -- Intersection of request ids
                    SELECT * FROM dlf_messages WHERE reqid IN (
                      SELECT reqid FROM dlf_messages
                       WHERE timestamp >  TO_DATE(SYSDATE - 25/1440, 'YYYYMMDDHH24MISS')
                         AND timestamp <= TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
                         AND facility = 4
                         AND msg_no   = 10
                    INTERSECT
                      SELECT reqid FROM dlf_messages
                       WHERE timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
                         AND facility = 4
                         AND msg_no   = 12
                    )
		    AND timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
                    AND facility = 4
                    AND ((msg_no = 10) OR (msg_no = 12))		    

		  ) m, dlf_num_param_values p
		  WHERE p.timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
		    AND m.id = p.id
		    AND p.name IN ('Type', 'ProcTime')

	      )
	      WHERE msg_no = 10
	        AND type IN (35, 36, 37, 40)
	      GROUP BY type ORDER BY type)
    LOOP
      EXECUTE IMMEDIATE 'UPDATE dlf_reqstats SET
                                min_time  = '||a.min||',
                                max_time  = '||a.max||',
                                avg_time  = '||a.avg||',
                                std_dev   = '||a.stddev||',
                                req_count = '||a.count||'
			  WHERE timestamp = '''||v_time||'''
			    AND type      = '||a.type;    
    END LOOP;
  END IF;
END;


/*
 * statistics scheduler (15 minutes)
 */
BEGIN
DBMS_SCHEDULER.CREATE_JOB (
	JOB_NAME 	=> 'DLF_STATS_15MINS',
	JOB_TYPE 	=> 'PLSQL_BLOCK',
	JOB_ACTION 	=> 'BEGIN
				DLF_STATS_JOBS();
				DLF_STATS_REQUESTS();
			   END;',
	START_DATE 	=> SYSDATE,
	REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=15',
	ENABLED 	=> TRUE,
	COMMENTS	=> 'CASTOR2 Monitoring Statistics (15 Minute Frequency)');
END;


/*
 * dlf_stats_tape (Populates table: dlf_tapestats)
 */
CREATE OR REPLACE PROCEDURE dlf_stats_tape
AS

  -- Variables
  v_time        DATE;
  v_mode        NUMBER := 0;
  v_value       VARCHAR2(255);

BEGIN

  -- Tape stats enabled ?
  SELECT value INTO v_value FROM dlf_settings WHERE name = 'ENABLE_TAPESTATS';
  v_mode := TO_NUMBER(v_value);

  IF v_mode = 1 THEN
    -- Set the nls_date_format
    EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "YYYYMMDDHH24MISS"';

    -- We define the time as a variable so that all data recorded will be synchronised
    v_time := TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS');

    -- Update statistics
    FOR a IN (SELECT svcclass, tapepool, COUNT(*) count, SUM(filesize) filesize
              FROM (
                SELECT a.id,
                       MAX(DECODE(a.name, 'SVCCLASS',    a.value, NULL)) svcclass,
                       MAX(DECODE(a.name, 'TAPEPOOL',    a.value, NULL)) tapepool,
                       MAX(DECODE(b.name, 'FILESIZE',    b.value, NULL)) filesize
                 FROM dlf_str_param_values a, dlf_num_param_values b
                WHERE a.id = b.id
                  AND a.id IN (

                    -- Message ids of interest
                    SELECT id FROM dlf_messages
                     WHERE facility = 1
                       AND msg_no = 55
                       AND timestamp >  TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
                       AND timestamp <= TO_DATE(SYSDATE - 5/1440,  'YYYYMMDDHH24MISS')
                  )
                  AND a.timestamp >  TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
                  AND a.timestamp <= TO_DATE(SYSDATE - 5/1440,  'YYYYMMDDHH24MISS')
                  AND b.timestamp >  TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
                  AND b.timestamp <= TO_DATE(SYSDATE - 5/1440,  'YYYYMMDDHH24MISS')
                  AND a.name IN ('SVCCLASS', 'TAPEPOOL')
                  AND b.name IN ('FILESIZE', 'ELAPSEDTIME')
                GROUP BY a.id
              )
              GROUP BY svcclass, tapepool)
    LOOP
      EXECUTE IMMEDIATE 'INSERT INTO dlf_tapestats
                         VALUES ('''|| v_time ||''','''|| a.svcclass ||''','''|| a.tapepool ||''', 
                                '|| a.count ||','|| a.filesize ||', 300)';
    END LOOP;
  END IF;
END;


/*
 * statistics scheduler (5 minutes)
 */
BEGIN
DBMS_SCHEDULER.CREATE_JOB (
	JOB_NAME 	=> 'DLF_STATS_5MINS',
        JOB_TYPE        => 'STORED_PROCEDURE',
        JOB_ACTION      => 'DLF_STATS_TAPE',
	START_DATE 	=> SYSDATE,
	REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=5',
	ENABLED 	=> TRUE,
	COMMENTS	=> 'CASTOR2 Monitoring Statistics (5 Minute Frequency)');
END;


/*
 * Move partitions in the DLF_DATA tablespace to their own dedicated tablespace
 */
BEGIN

  -- Create missing tablespaces
  FOR a IN (SELECT DISTINCT(tablespace_name)
              FROM (SELECT CONCAT('DLF_', SUBSTR(partition_name, 3)) tablespace_name
                      FROM user_tab_partitions
                     WHERE tablespace_name = 'DLF_DATA'
                       AND partition_name <> 'MAX_VALUE')
             WHERE tablespace_name NOT IN (
                    SELECT tablespace_name FROM user_tablespaces))
  LOOP
    EXECUTE IMMEDIATE 'CREATE TABLESPACE '||a.tablespace_name||'
                       DATAFILE SIZE 50M
                       AUTOEXTEND ON NEXT 100M
                       MAXSIZE 10G
                       EXTENT MANAGEMENT LOCAL
                       SEGMENT SPACE MANAGEMENT AUTO';
  END LOOP;

  -- Move partitions into their dedicated tablespace areas
  FOR a IN (SELECT table_name, partition_name, CONCAT('DLF_', SUBSTR(partition_name, 3)) tablespace_name
              FROM user_tab_partitions
             WHERE tablespace_name = 'DLF_DATA'
               AND partition_name <> 'MAX_VALUE')
  LOOP
    EXECUTE IMMEDIATE 'ALTER TABLE '||a.table_name||'
                       MOVE PARTITION '||a.partition_name||'
                       TABLESPACE '||a.tablespace_name||'
                       UPDATE INDEXES';
  END LOOP;
END;

