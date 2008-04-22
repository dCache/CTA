/******************************************************************************
 *              dlf_2.1.7-3_to_2.1.7-4.sql
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
 * @(#)$RCSfile: dlf_2.1.7-3_to_2.1.7-4.sql,v $ $Release: 1.2 $ $Release$ $Date: 2008/04/22 06:19:57 $ $Author: waldron $
 *
 * This script upgrades a CASTOR v2.1.7-3 DLF database to 2.1.7-4
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus and sql developer */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM dlf_version WHERE release LIKE '2_1_7_3%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL revision mismatch. Please run previous upgrade scripts before this one.');
END;

UPDATE dlf_version SET release = '2_1_7_4';
COMMIT;


/* Drop all jobs*/
BEGIN
  FOR a IN (SELECT job_name FROM user_scheduler_jobs)
  LOOP
    DBMS_SCHEDULER.DROP_JOB(a.job_name, TRUE);
  END LOOP;
END;

/* Drop old statistics procedures */
DROP PROCEDURE dlf_stats_tape;
DROP PROCEDURE dlf_stats_requests;
DROP PROCEDURE dlf_stats_jobs;

/* Drop old statistics tables */
DROP TABLE dlf_jobstats;
DROP TABLE dlf_reqstats;
DROP TABLE dlf_tapestats;

/* Drop old maintenance procedures */
DROP PROCEDURE dlf_archive;
DROP PROCEDURE dlf_partition;

/* Drop unused tables */
DROP TABLE dlf_settings;
DROP TABLE dlf_mode;

/* SQL statement for table LatencyStats */
CREATE TABLE LatencyStats (timestamp DATE NOT NULL, interval NUMBER, type NUMBER, started NUMBER, minimum NUMBER(*,4), maximum NUMBER(*,4), average NUMBER(*,4), stddev NUMBER(*,4), median NUMBER(*,4)) 
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table QueueTimeStats */
CREATE TABLE QueueTimeStats (timestamp DATE NOT NULL, interval NUMBER, dispatched NUMBER, minimum NUMBER(*,4), maximum NUMBER(*,4), average NUMBER(*,4), stddev NUMBER(*,4), median NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table GarbageCollectionStats */
CREATE TABLE GarbageCollectionStats (timestamp DATE NOT NULL, interval NUMBER, diskserver VARCHAR(255), deleted NUMBER, totalSize NUMBER, minimum NUMBER(*,4), maximum NUMBER(*,4), average NUMBER(*,4), stddev NUMBER(*,4), median NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table RequestStats */
CREATE TABLE RequestStats (timestamp DATE NOT NULL, interval NUMBER, type VARCHAR2(255), euid VARCHAR2(255), requests NUMBER) 
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table DiskCacheEfficiencyStats */
CREATE TABLE DiskCacheEfficiencyStats (timestamp DATE NOT NULL, interval NUMBER, wait NUMBER, d2d NUMBER, recall NUMBER, staged NUMBER, total NUMBER) 
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table FilesMigratedStats */
CREATE TABLE FilesMigratedStats (timestamp DATE NOT NULL, interval NUMBER, svcclass VARCHAR2(255), tapepool VARCHAR2(255), files NUMBER, totalSize NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));


/* PL/SQL method implementing statsLatency */
CREATE OR REPLACE PROCEDURE statsLatency (now IN DATE) AS
BEGIN
  -- Stats table: LatencyStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT type, count(*) started, min(waitTime) min, max(waitTime) max, 
           avg(waitTime) avg, stddev_pop(waitTime) stddev, median(waitTime) median
      FROM (
        SELECT nvl(substr(value, instr(value, '@', 1, 2) + 1, 2), 133) type, waitTime
          FROM (
            -- Extract the totalWaitTime for all stagerJobs or diskCopyTransfers 
            -- which have started.
            SELECT params.id, params.value waitTime
              FROM dlf_messages messages, dlf_num_param_values params
             WHERE messages.id = params.id
               AND messages.severity = 8 -- System
               AND ((messages.facility = 5  AND messages.msg_no = 12)  -- Job started
                OR  (messages.facility = 23 AND messages.msg_no = 25)) -- DiskCopyTransfer started
               AND messages.timestamp >  now - 10/1440
               AND messages.timestamp <= now - 5/1440
               AND params.name = 'TotalWaitTime'
               AND params.timestamp >  now - 10/1440
               AND params.timestamp <= now - 5/1440
          ) results
        -- For facility 23 (DiskCopyTransfer) we can assume that the request type
        -- associated with the transfer is 133 (StageDiskCopyReplicaRequest). 
        -- However, for normal jobs we must parse the Arguments attribute of the
        -- start message to determine the request type. Hence the left join, 
        -- NULL's are 133!!
        LEFT JOIN dlf_str_param_values params
          ON results.id = params.id
         AND params.name = 'Arguments'
         AND params.timestamp >  now - 10/1440
         AND params.timestamp <= now - 5/1440)
       GROUP BY type 
       ORDER BY to_number(type)
  )
  LOOP
    INSERT INTO LatencyStats
    VALUES (now - 5/1440, 300, a.type, a.started, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;


/* PL/SQL method implementing statsQueueTime */
CREATE OR REPLACE PROCEDURE statsQueueTime (now in DATE) AS
BEGIN
  -- Stats table: QueueTimeStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT count(*) dispatched, 
           nvl(min(params.value), 0) min, 
           nvl(max(params.value), 0) max, 
           nvl(avg(params.value), 0) avg, 
           nvl(stddev_pop(params.value), 0) stddev, 
           nvl(median(params.value), 0) median
      FROM dlf_messages messages, dlf_num_param_values params
     WHERE messages.id = params.id
       AND messages.severity = 8 -- System
       AND messages.facility = 9 -- Scheduler
       AND messages.msg_no = 34  -- Wrote notification file
       AND messages.timestamp >  now - 10/1440
       AND messages.timestamp <= now - 5/1440
       AND params.name = 'QueueTime'
       AND params.timestamp >  now - 10/1440
       AND params.timestamp <= now - 5/1440
  )
  LOOP
    INSERT INTO QueueTimeStats
    VALUES (now - 5/1440, 300, a.dispatched, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;


/* PL/SQL method implementing statsGarbageCollection */
CREATE OR REPLACE PROCEDURE statsGarbageCollection (now IN DATE) AS
BEGIN
  -- Stats table: GarbageCollectionStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT hostname diskserver, type, count(*) deleted,
         nvl(sum(params.value), -1) totalSize, 
         nvl(min(params.value), -1) min,
         nvl(max(params.value), -1) max, 
         nvl(avg(params.value), -1) avg, 
         nvl(stddev_pop(params.value), -1) stddev, 
         nvl(median(params.value), -1) median
    FROM (
      -- Extract the number of files deleted through name server and stager
      -- catalog sychronisation.
      SELECT messages.id, messages.hostid, decode(messages.msg_no, 27, 142, 149) type, 0 fileage
        FROM dlf_messages messages
       WHERE messages.severity = 8 -- System
         AND messages.facility = 8 -- GC
         AND (messages.msg_no = 27 OR -- Deleting ... nameserver
              messages.msg_no = 36)   -- Deleting ... stager catalog
         AND messages.timestamp >  now - 10/1440
         AND messages.timestamp <= now - 5/1440
      UNION ALL
      -- Extract the file age of all files successfully removed across all
      -- diskservers.
      SELECT messages.id, messages.hostid, 73 type, params.value fileAge
        FROM dlf_messages messages, dlf_num_param_values params
       WHERE messages.id = params.id
         AND messages.severity = 8 -- System
         AND messages.facility = 8 -- GC
         AND messages.msg_no = 11  -- Removed file successfully
         AND messages.timestamp >  now - 10/1440
         AND messages.timestamp <= now - 5/1440
         AND params.name = 'FileAge'
         AND params.timestamp >  now - 10/1440
         AND params.timestamp <= now - 5/1440
    ) results
    -- Attach the file size value from the same message to the result form the
    -- inner select above. As a result we'll have one row per file with its 
    -- corresponding age and size.
    LEFT JOIN dlf_num_param_values params
      ON results.id = params.id
     AND params.name = 'FileSize'
     AND params.timestamp >  now - 10/1440
     AND params.timestamp <= now - 5/1440
   -- Resolve the host ids to names
   INNER JOIN dlf_host_map hosts
      ON results.hostid = hosts.hostid
   GROUP BY hostname, type
   ORDER BY hostname, type
  )
  LOOP
    INSERT INTO GarbageCollectionStats
    VALUES (now - 5/1440, 300, a.diskserver, a.deleted, a.totalSize, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;


/* PL/SQL method implementing statsRequest */
CREATE OR REPLACE PROCEDURE statsRequest (now IN DATE) AS
BEGIN
  -- Stats table: RequestStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT type, euid, requests FROM (
      -- For each request type display the top 5 users + the total number of requests
      SELECT type, euid, requests,
             RANK() OVER (PARTITION BY type ORDER BY requests DESC, euid ASC) rank
        FROM(
          -- Determine the number of requests made for each request type over the
          -- last sampling interval.
          SELECT params.value type, '-' euid, count(*) requests
            FROM dlf_messages messages, dlf_str_param_values params
           WHERE messages.id = params.id
             AND messages.severity = 10 -- Monitoring
             AND messages.facility = 4  -- RequestHandler
             AND messages.msg_no = 10   -- Reply sent to client
             AND messages.timestamp >  now - 10/1440
             AND messages.timestamp <= now - 5/1440
             AND params.name = 'Type'
             AND params.timestamp >  now - 10/1440
             AND params.timestamp <= now - 5/1440
           GROUP BY params.value
          -- Join the user and summary/aggregate level breakdowns together. Note: 
          -- this could probably be done using an analaytical function or grouping 
          -- set!!!
           UNION
          -- Determine the number of requests made for each request type and per
          -- user over the last sampling period. This gives us a user level breakdown.
          SELECT results.value type, TO_CHAR(params.value) euid, count(*) requests
            FROM (
              SELECT params.id, params.value
                FROM dlf_messages messages, dlf_str_param_values params
               WHERE messages.id = params.id
                 AND messages.severity = 10 -- Monitoring
                 AND messages.facility = 4  -- RequestHandler
                 AND messages.msg_no = 10   -- Reply sent to client
                 AND messages.timestamp >  now - 10/1440
                 AND messages.timestamp <= now - 5/1440
                 AND params.name = 'Type'
                 AND params.timestamp >  now - 10/1440
                 AND params.timestamp <= now - 5/1440
            ) results
      -- Determine the uid of the user associated with each request
      INNER JOIN dlf_num_param_values params
         ON results.id = params.id
        AND params.name = 'Euid'
        AND params.timestamp >  now - 10/1440
        AND params.timestamp <= now - 5/1440
      GROUP BY results.value, params.value)
      ) WHERE rank < 6
     ORDER BY type, requests DESC
  )
  LOOP
    INSERT INTO RequestStats
    VALUES (now - 5/1440, 300, a.type, a.euid, a.requests);
  END LOOP;
END;


/* PL/SQL method implementing statsDiskCachEfficiency */
CREATE OR REPLACE PROCEDURE statsDiskCacheEfficiency (now IN DATE) AS
BEGIN
  -- Stats table: DiskCacheEfficiencyStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT nvl(sum(CASE WHEN msg_no = 53 THEN requests ELSE 0 END), 0) Wait,
           nvl(sum(CASE WHEN msg_no = 56 THEN requests ELSE 0 END), 0) D2D,
           nvl(sum(CASE WHEN msg_no = 57 THEN requests ELSE 0 END), 0) Recall,
           nvl(sum(CASE WHEN msg_no = 60 THEN requests ELSE 0 END), 0) Staged,
           nvl(sum(requests), 0) total
      FROM (
        SELECT msg_no, count(*) requests
          FROM (
            -- Get the first message issued for all subrequests of interest. This
            -- indicates to us whether the request was a hit or miss.
            SELECT sum(messages.id) KEEP (DENSE_RANK FIRST 
                   ORDER BY messages.timestamp ASC, messages.timeusec ASC) id
              FROM dlf_messages messages
             WHERE messages.severity = 8  -- System
               AND messages.facility = 22 -- Stager
               AND messages.msg_no IN (53, 56, 57, 60)
               AND messages.reqid IN (
                 -- Extract all new requests entering the system through the request
                 -- handler that may result in a read request. This result set will 
                 -- be used in outer select statement to prevent if from picking up
                 -- subrequests which have been restarted but did not enter the 
                 -- system within the required timeframe.
                 SELECT messages.reqid
                   FROM dlf_messages messages, dlf_str_param_values params
                  WHERE messages.id = params.id
                    AND messages.severity = 10 -- Monitoring
                    AND messages.facility = 4  -- RequestHandler
                    AND messages.msg_no = 10   -- Reply sent to client
                    AND messages.timestamp >  now - 10/1440
                    AND messages.timestamp <= now - 5/1440
                    AND params.name = 'Type'
                    AND params.value IN ('StageGetRequest', 
                                         'StagePrepareToGetRequest', 
                                         'StageUpdateRequest')
                    AND params.timestamp >  now - 10/1440
                    AND params.timestamp <= now - 5/1440
               )
               AND messages.timestamp >  now - 10/1440
               AND messages.timestamp <= now - 5/1440
             GROUP BY messages.subreqid
          ) results
         INNER JOIN dlf_messages messages
            ON messages.id = results.id
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
         GROUP BY msg_no)
  )
  LOOP
    INSERT INTO DiskCacheEfficiencyStats
    VALUES (now - 5/1440, 300, a.wait, a.d2d, a.recall, a.staged, a.total);
  END LOOP;
END;


/* PL/SQL method implementing statsMigratedFiles */
CREATE OR REPLACE PROCEDURE statsMigratedFiles (now IN DATE) AS
BEGIN
  -- Stats table: FilesMigratedStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT svcclass, tapepool, count(*) files, sum(filesize) totalSize
      FROM (
        SELECT a.id, max(decode(a.name, 'SVCCLASS', a.value, NULL)) svcclass,
                     max(decode(a.name, 'TAPEPOOL', a.value, NULL)) tapepool,
                     max(decode(b.name, 'FILESIZE', b.value, NULL)) filesize
          FROM dlf_str_param_values a, dlf_num_param_values b
         WHERE a.id = b.id
           AND a.id IN (
             -- Extract the message ids of interest
             SELECT messages.id FROM dlf_messages messages
              WHERE messages.severity = 8 -- System
                AND messages.facility = 1 -- migrator
                AND messages.msg_no = 55  -- File staged
                AND messages.timestamp >  now - 10/1440
                AND messages.timestamp <= now - 5/1440
             )
             AND a.timestamp >  now - 10/1440
             AND a.timestamp <= now - 5/1440
             AND b.timestamp >  now - 10/1440
             AND b.timestamp <= now - 5/1440
             AND a.name IN ('SVCCLASS', 'TAPEPOOL')
             AND b.name IN ('FILESIZE', 'ELAPSEDTIME')
           GROUP BY a.id)
     GROUP BY svcclass, tapepool
  )
  LOOP
    INSERT INTO FilesMigratedStats
    VALUES (now - 5/1440, 300, a.svcclass, a.tapepool, a.files, a.totalSize);
  END LOOP;
END;


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
             WHERE table_name LIKE 'DLF_%'
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


/* PL/SQL method implementing archiveData */
CREATE OR REPLACE PROCEDURE archiveData (expiry IN NUMBER)
AS
  username VARCHAR2(2048);
BEGIN
  -- Extract the name of the current user running the PL/SQL procedure. This name
  -- will be used within the tablespace names.
  SELECT SYS_CONTEXT('USERENV', 'CURRENT_USER') 
    INTO username
    FROM dual;

  -- Drop partitions across all DLF_ tables
  FOR a IN (SELECT *
              FROM user_tab_partitions
             WHERE partition_name < concat('P_', TO_CHAR(SYSDATE - expiry, 'YYYYMMDD'))
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
               AND tablespace_name < 'DLF_'||TO_CHAR(SYSDATE - expiry, 'YYYYMMDD')||'_'||username
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


/* Remove scheduler jobs before recreation */
BEGIN
  FOR a IN (SELECT job_name FROM user_scheduler_jobs)
  LOOP
    DBMS_SCHEDULER.DROP_JOB(a.job_name, TRUE);
  END LOOP;
END;


BEGIN
  -- Create a db job to be run every day and create new partitions
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'partitionCreationJob',
      JOB_TYPE        => 'STORED_PROCEDURE',
      JOB_ACTION      => 'createPartitions',
      START_DATE      => TRUNC(SYSDATE) + 1/24,
      REPEAT_INTERVAL => 'FREQ=DAILY',
      ENABLED         => TRUE,
      COMMENTS        => 'Daily partitioning creation');

  -- Create a db job to be run every day and drop old data from the database
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'archiveDataJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN archiveData(90); END;',
      START_DATE      => TRUNC(SYSDATE) + 2/24,
      REPEAT_INTERVAL => 'FREQ=DAILY',
      ENABLED         => TRUE,
      COMMENTS        => 'Daily data archiving');

  -- Create a job to execute the procedures that create statistical information
  DBMS_SCHEDULER.CREATE_JOB (
      JOB_NAME        => 'statisticJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'DECLARE
                            now DATE;
                          BEGIN
                            now := SYSDATE;
                            statsLatency(now);
		  	    statsQueueTime(now);
			    statsGarbageCollection(now);
			    statsRequest(now);
			    statsDiskCacheEfficiency(now);
			    statsMigratedFiles(now);
                          END;',
      START_DATE      => SYSDATE,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=5',
      ENABLED         => TRUE,
      COMMENTS        => 'CASTOR2 Monitoring Statistics (5 Minute Frequency)');
END;


/* Trigger the initial creation of partitions */
BEGIN
  createPartitions();
END;


/* End-of-File */
