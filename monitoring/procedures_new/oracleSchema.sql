/******************************************************************************
 *              oracleSchema.sql
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
 * @(#)RCSfile: oracleCreate.sql,v  Release: 1.2  Release Date: 2009/03/26 13:14:27  Author: waldron 
 *
 * This script create a new Monitoring schema
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus */
WHENEVER SQLERROR EXIT FAILURE;

/* Determine the DLF schema that the monitoring procedures should run against */
UNDEF dlfschema
ACCEPT dlfschema DEFAULT castor_dlf PROMPT 'Enter the DLF schema to run monitoring queries against: (castor_dlf) ';
SET VER OFF

/* Check that the executing accounting can see the DLF base tables */
DECLARE
  unused VARCHAR2(2048);
BEGIN
  -- Check that the user exists
  BEGIN
    SELECT username INTO unused
      FROM all_users
     WHERE username = upper('&&dlfschema');
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20000, 'User &dlfschema does not exist');
  END;
  -- Check that the correct grants are present
  BEGIN
    SELECT owner INTO unused
      FROM all_tables
     WHERE owner = upper('&&dlfschema')
       AND table_name = 'DLF_VERSION';
  EXCEPTION WHEN NO_DATA_FOUND THEN
    raise_application_error(-20001, 'Unable to access the &dlfschema..dlf_version table. Check that the correct grants have been issued!');
  END;  
END;
/


/***** EXISTING/OLD MONITORING *****/

/* SQL statement for table LatencyStats */
CREATE TABLE LatencyStats (timestamp DATE CONSTRAINT NN_LatencyStats_ts NOT NULL, interval NUMBER, type VARCHAR2(255), started NUMBER, minTime NUMBER(*,4), maxTime NUMBER(*,4), avgTime NUMBER(*,4), stddevTime NUMBER(*,4), medianTime NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table QueueTimeStats */
CREATE TABLE QueueTimeStats (timestamp DATE CONSTRAINT NN_QueueTimeStats_ts NOT NULL, interval NUMBER, type VARCHAR2(255), svcclass VARCHAR2(255), dispatched NUMBER, minTime NUMBER(*,4), maxTime NUMBER(*,4), avgTime NUMBER(*,4), stddevTime NUMBER(*,4), medianTime NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table GarbageCollectionStats */
CREATE TABLE GarbageCollectionStats (timestamp DATE CONSTRAINT NN_GarbageCollectionStats_ts NOT NULL, interval NUMBER, diskserver VARCHAR2(255), type VARCHAR2(255), deleted NUMBER, totalSize NUMBER, minFileAge NUMBER(*,4), maxFileAge NUMBER(*,4), avgFileAge NUMBER(*,4), stddevFileAge NUMBER(*,4), medianFileAge NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table RequestStats */
CREATE TABLE RequestStats (timestamp DATE CONSTRAINT NN_RequestStats_ts NOT NULL, interval NUMBER, type VARCHAR2(255), hostname VARCHAR2(255), euid VARCHAR2(255), requests NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table DiskCacheEfficiencyStats */
CREATE TABLE DiskCacheEfficiencyStats (timestamp DATE CONSTRAINT NN_DiskCacheEfficiencyStats_ts NOT NULL, interval NUMBER, type VARCHAR2(255), svcclass VARCHAR2(255), wait NUMBER, d2d NUMBER, recall NUMBER, staged NUMBER, total NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table FilesMigratedStats */
CREATE TABLE FilesMigratedStats (timestamp DATE CONSTRAINT NN_LFilesMigratedStats_ts NOT NULL, interval NUMBER, svcclass VARCHAR2(255), tapepool VARCHAR2(255), totalFiles NUMBER, totalSize NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table ReplicationStats */
CREATE TABLE ReplicationStats (timestamp DATE CONSTRAINT NN_ReplicationStats_ts NOT NULL, interval NUMBER, sourceSvcClass VARCHAR2(255), destSvcClass VARCHAR2(255), transferred NUMBER, totalSize NUMBER, minSize NUMBER(*,4), maxSize NUMBER(*,4), avgSize NUMBER(*,4), stddevSize NUMBER(*,4), medianSize NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table TapeRecalledStats */
CREATE TABLE TapeRecalledStats (timestamp DATE CONSTRAINT NN_TapeRecalledStats_ts NOT NULL, interval NUMBER, type VARCHAR2(255), username VARCHAR2(255), groupname VARCHAR2(255), tapeVid VARCHAR2(255), tapeStatus VARCHAR2(255), files NUMBER, totalSize NUMBER, mountsPerDay NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table ProcessingTimeStats */
CREATE TABLE ProcessingTimeStats (timestamp DATE CONSTRAINT NN_ProcessingTimeStats_ts NOT NULL, interval NUMBER, daemon VARCHAR2(255), type VARCHAR2(255), requests NUMBER, minTime NUMBER(*,4), maxTime NUMBER(*,4), avgTime NUMBER(*,4), stddevTime NUMBER(*,4), medianTime NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table ClientVersionStats */
CREATE TABLE ClientVersionStats (timestamp DATE CONSTRAINT NN_ClientVersionStats_ts NOT NULL, interval NUMBER, clientVersion VARCHAR2(255), requests NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for temporary table CacheEfficiencyHelper */
CREATE GLOBAL TEMPORARY TABLE CacheEfficiencyHelper (reqid CHAR(36))
  ON COMMIT DELETE ROWS;

/* SQL statement for table TapeMountsHelper */
CREATE TABLE TapeMountsHelper (timestamp DATE CONSTRAINT NN_TapeMountsHelper_ts NOT NULL, tapevid VARCHAR2(20))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for a view on the DLF_Monitoring table */
CREATE OR REPLACE VIEW DLFStats AS
  SELECT * FROM &dlfschema..dlf_monitoring;

/* SQL statement for a view on the DLF_Config table */
CREATE OR REPLACE VIEW DLFConfig AS
  SELECT * FROM &dlfschema..dlf_config;


/***** NEW MONITORING *****/

/* SQL statement for table ConfigSchema */
CREATE TABLE ConfigSchema (expiry NUMBER, runMaxTime DATE);
INSERT INTO ConfigSchema VALUES (90, SYSDATE);

/* SQL statement for table Requests */
CREATE TABLE Requests (subReqId CHAR(36) CONSTRAINT NN_Requests_subReqId NOT NULL CONSTRAINT PK_Requests_subReqId PRIMARY KEY, timestamp DATE CONSTRAINT NN_Requests_timestamp NOT NULL, reqId CHAR(36) CONSTRAINT NN_Requests_reqId NOT NULL, nsFileId NUMBER CONSTRAINT NN_Requests_nsFileId NOT NULL, type VARCHAR2(255), svcclass VARCHAR2(255), username VARCHAR2(255), state VARCHAR2(255), filename VARCHAR2(2048), filesize NUMBER) 
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statements for indexes on the Requests table */
CREATE INDEX I_Requests_reqId ON Requests (reqId) LOCAL;

/* SQL statement for table InternalDiskCopy */
CREATE TABLE InternalDiskCopy (timestamp DATE CONSTRAINT NN_InternalDiskCopy_ts NOT NULL, svcclass VARCHAR2(255), copies NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table TotalLatency */
CREATE TABLE TotalLatency (subReqId CHAR(36) CONSTRAINT NN_TotalLatency_subReqId NOT NULL CONSTRAINT PK_TotalLatency_subReqId PRIMARY KEY, timestamp DATE CONSTRAINT NN_TotalLatency_ts NOT NULL, nsFileId NUMBER CONSTRAINT NN_TotalLatency_nsFileId NOT NULL, totalLatency NUMBER) 
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statements for indexes on the TotalLatency table */
CREATE INDEX I_TotalLatency_totalLatency ON TotalLatency (totalLatency) LOCAL;

/* SQL statement for table TapeRecall */
CREATE TABLE TapeRecall (subReqId CHAR(36) CONSTRAINT NN_TapeRecall_subReqId NOT NULL CONSTRAINT PK_TapeRecall_subReqId PRIMARY KEY, timestamp DATE CONSTRAINT NN_TapeRecall_ts NOT NULL, tapeId VARCHAR2(255 BYTE), tapeMountState VARCHAR2(255 BYTE), readLatency INTEGER, copyLatency INTEGER, CONSTRAINT FK_TapeRecall_subReqId FOREIGN KEY (subReqId) REFERENCES Requests (subReqid) ON DELETE CASCADE)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table DiskCopy */
CREATE TABLE DiskCopy (nsFileId NUMBER CONSTRAINT NN_DiskCopy_nsFileId NOT NULL, timestamp DATE CONSTRAINT NN_DiskCopy_ts NOT NULL, originalPool VARCHAR2(255), targetPool VARCHAR2(255), readLatency INTEGER, copyLatency INTEGER, numCopiesInPools INTEGER, srcHost VARCHAR2(255), destHost VARCHAR2(255))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table GCFiles */
CREATE TABLE GcFiles (timestamp DATE CONSTRAINT NN_GCFiles_ts NOT NULL, nsFileId NUMBER CONSTRAINT NN_GCFiles_nsFileId NOT NULL, fileSize NUMBER, fileAge NUMBER, lastAccessTime NUMBER, nbAccesses NUMBER, gcType VARCHAR2(255), svcClass VARCHAR2(255))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table Migration */
CREATE TABLE Migration (subReqId CHAR(36) CONSTRAINT NN_Migration_subReqId NOT NULL CONSTRAINT PK_Migration_subReqId PRIMARY KEY, timestamp DATE CONSTRAINT NN_Migration_ts NOT NULL, reqId CHAR(36) CONSTRAINT NN_Migration_reqId NOT NULL, nsfileid NUMBER CONSTRAINT NN_Migration_nsFileId NOT NULL, type VARCHAR2(255), svcclass VARCHAR2(255), username VARCHAR2(255), state VARCHAR2(255), filename VARCHAR2(2048), totalLatency NUMBER, filesize NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statements for indexes on the Migration table */
CREATE INDEX I_Migration_reqId ON Migration (reqId) LOCAL;

/* SQL statement for creation of the Errors materialized view */
CREATE MATERIALIZED VIEW Errors_MV
  REFRESH FORCE ON DEMAND 
  START WITH SYSDATE NEXT SYSDATE + 5/1440
AS
  SELECT fac.fac_name, txt.msg_text, count(*) errorsum
    FROM &dlfschema..dlf_messages mes, &dlfschema..dlf_msg_texts txt, 
         &dlfschema..dlf_facilities fac
   WHERE mes.msg_no = txt.msg_no
     AND fac.fac_no = mes.facility
     AND mes.facility = txt.fac_no
     AND mes.severity = 3  -- Errors
     AND mes.timestamp >= sysdate - 15/1440 
     AND mes.timestamp < sysdate - 5/1440
   GROUP BY fac.fac_name, txt.msg_text;

/* SQL statement for creation of the ReqDel materialized view */
CREATE MATERIALIZED VIEW ReqDel_MV
  REFRESH FORCE ON DEMAND
  START WITH SYSDATE NEXT SYSDATE + 10/1440
AS
  SELECT req.timestamp, round((req.timestamp - gcFiles.timestamp) * 24, 5) dif
    FROM Requests req, GcFiles gcFiles
   WHERE req.nsFileId = gcFiles.nsFileId
     AND req.state = 'TapeRecall'
     AND req.timestamp > gcFiles.timestamp
     AND req.timestamp - gcFiles.timestamp <= 1;

CREATE INDEX I_ReqDel_MV_dif ON ReqDel_MV (dif);

/* SQL statement for creation of the GcMonitor materialized view */
CREATE MATERIALIZED VIEW GcMonitor_MV
 REFRESH COMPLETE ON DEMAND
 START WITH SYSDATE NEXT SYSDATE + 5/1440
AS 
  SELECT a.tot total, round(a.avgage / 3600, 2) avg_age, 
         round((a.avgage - b.avgage) / b.avgage, 4) age_per, 
         round(a.avgsize / 102400, 4) avg_size, 
         round((a.avgsize - b.avgsize) / b.avgsize, 4) size_per
    FROM (SELECT count(*) tot, avg(fileAge) avgage, avg(fileSize) avgsize
            FROM GcFiles
           WHERE timestamp > sysdate - 10/1440
             AND timestamp <= sysdate - 5/1440) a,
         (SELECT avg(fileAge) avgage, avg(fileSize) avgsize
            FROM GcFiles
           WHERE timestamp > sysdate - 15/1440
             AND timestamp <= sysdate - 10/1140) b;

/* SQL statement for creation of the MainTableCounters materialized view */
CREATE MATERIALIZED VIEW MainTableCounters_MV 
  REFRESH COMPLETE ON DEMAND 
  START WITH SYSDATE NEXT SYSDATE + 5/1140
AS 
  SELECT a.svcclass, a.state state, a.num num, 
         round((a.num - b.num) / b.num, 2) per
    FROM (SELECT svcclass, state, count(*) num
              FROM Requests 
           WHERE timestamp > sysdate - 10/1440 
             AND timestamp <= sysdate -5/1440 
             GROUP BY svcclass,state) a ,
         (SELECT svcclass, state, count(*) num
            FROM Requests 
           WHERE timestamp > sysdate - 15/1440
             AND timestamp <= sysdate - 10/1440 
           GROUP BY svcclass, state) b
   WHERE a.svcclass = b.svcclass
     AND a.state = b.state
   ORDER BY a.svcclass;

/* SQL statement for creation of the MigMonitor_MV materialized view */
CREATE MATERIALIZED VIEW MigMonitor_MV
  REFRESH COMPLETE ON DEMAND
  START WITH SYSDATE NEXT SYSDATE + 5/1140
AS
  SELECT svcclass, count(*) migs 
    FROM Migration
   WHERE timestamp > sysdate - 10/1440 
     AND timestamp <= sysdate - 5/1440
   GROUP BY svcclass;

/* SQL statement for creation of the SvcClassMap_MV materialized view */
CREATE MATERIALIZED VIEW SvcClassMap_MV
  REFRESH FORCE ON DEMAND 
  START WITH SYSDATE + 10/1440 NEXT SYSDATE + 1
AS 
  SELECT DISTINCT(svcclass) FROM Requests;

/* SQL statement to rename the NOT NULL constraints of the materialized view 
 * created above.
 */
BEGIN
  FOR a IN (SELECT constraint_name, table_name, column_name
              FROM user_cons_columns
             WHERE table_name LIKE '%_MV'
               AND constraint_name LIKE 'SYS_%')
  LOOP
    EXECUTE IMMEDIATE 'ALTER MATERIALIZED VIEW '||a.table_name||' RENAME 
                      CONSTRAINT '||a.constraint_name||
                      ' TO NN_'||a.table_name||'_'|| a.column_name;
  END LOOP;
END;
/

/* Error log tables */
BEGIN
  DBMS_ERRLOG.CREATE_ERROR_LOG ('Requests',     'Err_Requests');
  DBMS_ERRLOG.CREATE_ERROR_LOG ('Migration',    'Err_Migration');
  DBMS_ERRLOG.CREATE_ERROR_LOG ('DiskCopy',     'Err_DiskCopy');
  DBMS_ERRLOG.CREATE_ERROR_LOG ('TapeRecall',   'Err_TapeRecall');
  DBMS_ERRLOG.CREATE_ERROR_LOG ('TotalLatency', 'Err_Latency');
END;
/
