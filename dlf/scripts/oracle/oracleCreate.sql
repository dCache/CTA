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
 * @(#)$RCSfile: oracleCreate.sql,v $ $Release: 1.2 $ $Release$ $Date: 2008/11/06 13:20:07 $ $Author: waldron $
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

/* SQL statement for table LatencyStats */
CREATE TABLE LatencyStats (timestamp DATE NOT NULL, interval NUMBER, type VARCHAR2(255), started NUMBER, minTime NUMBER(*,4), maxTime NUMBER(*,4), avgTime NUMBER(*,4), stddevTime NUMBER(*,4), medianTime NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table QueueTimeStats */
CREATE TABLE QueueTimeStats (timestamp DATE NOT NULL, interval NUMBER, type VARCHAR2(255), svcclass VARCHAR2(255), dispatched NUMBER, minTime NUMBER(*,4), maxTime NUMBER(*,4), avgTime NUMBER(*,4), stddevTime NUMBER(*,4), medianTime NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table GarbageCollectionStats */
CREATE TABLE GarbageCollectionStats (timestamp DATE NOT NULL, interval NUMBER, diskserver VARCHAR2(255), type VARCHAR2(255), deleted NUMBER, totalSize NUMBER, minFileAge NUMBER(*,4), maxFileAge NUMBER(*,4), avgFileAge NUMBER(*,4), stddevFileAge NUMBER(*,4), medianFileAge NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table RequestStats */
CREATE TABLE RequestStats (timestamp DATE NOT NULL, interval NUMBER, type VARCHAR2(255), hostname VARCHAR2(255), euid VARCHAR2(255), requests NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table DiskCacheEfficiencyStats */
CREATE TABLE DiskCacheEfficiencyStats (timestamp DATE NOT NULL, interval NUMBER, type VARCHAR2(255), svcclass VARCHAR2(255), wait NUMBER, d2d NUMBER, recall NUMBER, staged NUMBER, total NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table FilesMigratedStats */
CREATE TABLE FilesMigratedStats (timestamp DATE NOT NULL, interval NUMBER, svcclass VARCHAR2(255), tapepool VARCHAR2(255), totalFiles NUMBER, totalSize NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table ReplicationStats */
CREATE TABLE ReplicationStats (timestamp DATE NOT NULL, interval NUMBER, sourceSvcClass VARCHAR2(255), destSvcClass VARCHAR2(255), transferred NUMBER, totalSize NUMBER, minSize NUMBER(*,4), maxSize NUMBER(*,4), avgSize NUMBER(*,4), stddevSize NUMBER(*,4), medianSize NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table TapeRecalledStats */
CREATE TABLE TapeRecalledStats (timestamp DATE NOT NULL, interval NUMBER, type VARCHAR2(255), username VARCHAR2(255), groupname VARCHAR2(255), tapeVid VARCHAR2(255), tapeStatus VARCHAR2(255), files NUMBER, totalSize NUMBER, mountsPerDay NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table ProcessingTimeStats */
CREATE TABLE ProcessingTimeStats (timestamp DATE NOT NULL, interval NUMBER, daemon VARCHAR2(255), type VARCHAR2(255), requests NUMBER, minTime NUMBER(*,4), maxTime NUMBER(*,4), avgTime NUMBER(*,4), stddevTime NUMBER(*,4), medianTime NUMBER(*,4))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));

/* SQL statement for table ClientVersionStats */
CREATE TABLE ClientVersionStats (timestamp DATE NOT NULL, interval NUMBER, clientVersion VARCHAR2(255), requests NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));


/* SQL statement for temporary table CacheEfficiencyHelper */
CREATE GLOBAL TEMPORARY TABLE CacheEfficiencyHelper (reqid CHAR(36))
  ON COMMIT DELETE ROWS;

/* SQL statement for table TapeMountsHelper */
CREATE TABLE TapeMountsHelper (timestamp DATE NOT NULL, tapevid VARCHAR2(20))
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));


/* PL/SQL method implementing statsLatency
 *
 * Provides statistics on the amount of time a user has had to wait since their
 * request was entered into the system and it actually being served. The returned
 * data is broken down by request type.
 */
CREATE OR REPLACE PROCEDURE statsLatency (now IN DATE) AS
BEGIN
  -- Stats table: LatencyStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT type, count(*) started, min(waitTime) min, max(waitTime) max,
           avg(waitTime) avg, stddev_pop(waitTime) stddev, median(waitTime) median
      FROM (
        SELECT nvl(value, 'StageDiskCopyReplicaRequest') type, waitTime
          FROM (
            -- Extract the totalWaitTime for all stagerJobs or diskCopyTransfers
            -- which have started.
            SELECT params.id, params.value waitTime
              FROM dlf_messages messages, dlf_num_param_values params
             WHERE messages.id = params.id
               AND messages.severity = 8 -- System
               AND ((messages.facility = 26 AND messages.msg_no = 20)  -- Job started
                OR  (messages.facility = 23 AND messages.msg_no = 25)) -- DiskCopyTransfer started
               AND messages.timestamp >  sysdate - 10/1440
               AND messages.timestamp <= sysdate - 5/1440
               AND params.name = 'TotalWaitTime'
               AND params.timestamp >  sysdate - 10/1440
               AND params.timestamp <= sysdate - 5/1440
          ) results
      -- For facility 23 (DiskCopyTransfer) we can assume that the request type
      -- associated with the transfer is 133 (StageDiskCopyReplicaRequest).
      -- However, for normal jobs we must parse the Arguments attribute of the
      -- start message to determine the request type. Hence the left join,
      -- NULL's are 133!!
      LEFT JOIN dlf_str_param_values params
        ON results.id = params.id
       AND params.name = 'Type'
       AND params.timestamp >  sysdate - 10/1440
       AND params.timestamp <= sysdate - 5/1440)
     GROUP BY type
     ORDER BY type
  )
  LOOP
    INSERT INTO LatencyStats
      (timestamp, interval, type, started, minTime, maxTime, avgTime, stddevTime, medianTime)
    VALUES (now - 5/1440, 300, a.type, a.started, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;
/


/* PL/SQL method implementing statsQueueTime
 *
 * Provides statistics on the queue time of requests in LSF broken down by request
 * type and service class.
 */
CREATE OR REPLACE PROCEDURE statsQueueTime (now in DATE) AS
BEGIN
  -- Stats table: QueueTimeStats
  -- Frequency: 5 minutes
  FOR a IN (
     SELECT type, svcclass, count(*) dispatched,
           nvl(min(params.value), 0) min,
           nvl(max(params.value), 0) max,
           nvl(avg(params.value), 0) avg,
           nvl(stddev_pop(params.value), 0) stddev,
           nvl(median(params.value), 0) median
       FROM (
         -- Extract the type and service class for all jobs dispatched by LSF
         SELECT messages.id,
                max(decode(params.name, 'Type',     params.value, NULL)) type,
                max(decode(params.name, 'SvcClass', params.value, NULL)) svcclass
           FROM dlf_messages messages, dlf_str_param_values params
          WHERE messages.id = params.id
            AND messages.severity = 8 -- System
            AND messages.facility = 9 -- Scheduler
            AND messages.msg_no = 34  -- Wrote notification file
            AND messages.timestamp >  now - 10/1440
            AND messages.timestamp <= now - 5/1440
            AND params.name IN ('Type', 'SvcClass')
            AND params.timestamp >  now - 10/1440
            AND params.timestamp <= now - 5/1440
          GROUP BY messages.id
       ) results
      -- Attach the QueueTime attribute to the results previously collected.
      -- After this we will have a line for each started job detailing the
      -- service class the job is destined for, the request type and the number
      -- of seconds it spent queued.
      INNER JOIN dlf_num_param_values params
         ON results.id = params.id
        AND params.name = 'QueueTime'
        AND params.timestamp >  now - 10/1440
        AND params.timestamp <= now - 5/1440
      GROUP BY type, svcclass
      ORDER BY type, svcclass
  )
  LOOP
    INSERT INTO QueueTimeStats
      (timestamp, interval, type, svcclass, dispatched, minTime, maxTime, avgTime, stddevTime, medianTime)
    VALUES (now - 5/1440, 300, a.type, a.svcclass, a.dispatched, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;
/


/* PL/SQL method implementing statsGarbageCollection
 *
 * Provides an overview of the garbage collection process which includes the number
 * of files removed during the last interval, the total volume of reclaimed space
 * and statistical information e.g. avg on the fileage of the files deleted.
 */
CREATE OR REPLACE PROCEDURE statsGarbageCollection (now IN DATE) AS
BEGIN
  -- Stats table: GarbageCollectionStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT hostname diskserver, type, count(*) deleted,
           sum(params.value) totalSize,
           min(fileAge) min,
           max(fileAge) max,
           avg(fileAge) avg,
           stddev_pop(fileAge) stddev,
           median(fileAge) median
     FROM (
       -- Extract the file age of all files successfully removed across all
       -- diskservers.
       SELECT messages.id, messages.hostid,
              decode(messages.msg_no, 11, 'Files2Delete',
              decode(messages.msg_no, 27, 'NsFilesDeletd', 'StgFilesDeleted')) type,
              params.value fileAge
         FROM dlf_messages messages, dlf_num_param_values params
        WHERE messages.id = params.id
          AND messages.severity = 8 -- System
          AND messages.facility = 8 -- GC
          AND (messages.msg_no = 11 OR -- Removed file successfully
               messages.msg_no = 27 OR -- Deleting ... nameserver
               messages.msg_no = 36)   -- Deleting ... stager catalog
          AND messages.timestamp >  now - 10/1440
          AND messages.timestamp <= now - 5/1440
          AND params.name = 'FileAge'
          AND params.timestamp >  now - 10/1440
          AND params.timestamp <= now - 5/1440
     ) results
    -- Attach the file size value from the same message to the result form the
    -- inner select above. As a result we'll have one row per file with its
    -- corresponding age and size.
    INNER JOIN dlf_num_param_values params
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
      (timestamp, interval, diskserver, type, deleted, totalSize, minFileAge, maxFileAge, avgFileAge, stddevFileAge, medianFileAge)
    VALUES (now - 5/1440, 300, a.diskserver, a.type, a.deleted, a.totalsize, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;
/


/* PL/SQL method implementing statsRequest
 *
 * Provides statistical information on the types of requests recorded by the request
 * handler, the total for all users and a break down of the top 5 users per request
 * type
 */
CREATE OR REPLACE PROCEDURE statsRequest (now IN DATE) AS
BEGIN
  -- Stats table: RequestStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT type, hostname, euid, requests FROM (
      -- For each request type display the top 5 users + the total number of requests
      SELECT type, euid, hostname, requests,
             RANK() OVER (PARTITION BY type ORDER BY requests DESC, euid ASC) rank
        FROM(
          SELECT params.value type, hosts.hostname, '-' euid, count(*) requests
            FROM dlf_messages messages, dlf_str_param_values params, dlf_host_map hosts
           WHERE messages.id = params.id
             AND messages.severity = 10 -- Monitoring
             AND messages.facility = 4  -- RequestHandler
             AND messages.msg_no = 10   -- Reply sent to client
             AND messages.timestamp >  now - 10/1440
             AND messages.timestamp <= now - 5/1440
             AND params.name = 'Type'
             AND params.timestamp >  now - 10/1440
             AND params.timestamp <= now - 5/1440
             AND messages.hostid = hosts.hostid
           GROUP BY params.value, hosts.hostname
          -- Join the user and summary/aggregate level breakdowns together. Note:
          -- this could probably be done using an analytical function or grouping
          -- set!!!
           UNION
          -- Determine the number of requests made for each request type and per
          -- user over the last sampling period. This gives us a user level breakdown.
          SELECT results.value type, hostname, TO_CHAR(params.value) euid, count(*) requests
            FROM (
              SELECT params.id, params.value, hosts.hostname
                FROM dlf_messages messages, dlf_str_param_values params, dlf_host_map hosts
               WHERE messages.id = params.id
                 AND messages.severity = 10 -- Monitoring
                 AND messages.facility = 4  -- RequestHandler
                 AND messages.msg_no = 10   -- Reply sent to client
                 AND messages.timestamp >  now - 10/1440
                 AND messages.timestamp <= now - 5/1440
                 AND params.name = 'Type'
                 AND params.timestamp >  now - 10/1440
                 AND params.timestamp <= now - 5/1440
                 AND messages.hostid = hosts.hostid
            ) results
          -- Determine the uid of the user associated with each request
          INNER JOIN dlf_num_param_values params
             ON results.id = params.id
            AND params.name = 'Euid'
            AND params.timestamp >  now - 10/1440
            AND params.timestamp <= now - 5/1440
          GROUP BY results.value, hostname, params.value)
      ) WHERE rank < 6
     ORDER BY type, requests DESC
  )
  LOOP
    INSERT INTO RequestStats
      (timestamp, interval, type, hostname, euid, requests)
    VALUES (now - 5/1440, 300, a.type, a.hostname, a.euid, a.requests);
  END LOOP;
END;
/


/* PL/SQL method implementing statsDiskCachEfficiency
 *
 * Provides an overview of how effectively the disk cache is performing. For example,
 * the greater the number of recalls the less effective the cache is.
 *
 * Example output:
 *   Type            SvcClass      Wait D2D  Recall Staged Total
 *   StageGetRequest dteam         0    0    0      3      3
 *   StageGetRequest compasschunks 0    0    0      1      1
 *   StageGetRequest na48          0    0    0      71     71
 *   StageGetRequest compassmdst   0    0    0      1      1
 *   StageGetRequest compass004d   0    0    0      55     55
 *   StageGetRequest compasscdr    0    0    1      1      2
 *   StageGetRequest na48goldcmp   0    0    0      154    154
 *   StageGetRequest default       0    0    0      100    100
 */
CREATE OR REPLACE PROCEDURE statsDiskCacheEfficiency (now IN DATE) AS
BEGIN
  -- Stats table: DiskCacheEfficiencyStats
  -- Frequency: 5 minutes

  -- Collect a list of request ids that we are interested in. We dump this list into
  -- a temporary table so that the execution plan of the query afterwards is optimized
  INSERT INTO CacheEfficiencyHelper
    SELECT messages.reqid
      FROM dlf_messages messages
     WHERE messages.severity = 10 -- Monitoring
       AND messages.facility = 4  -- RequestHandler
       AND messages.msg_no = 10   -- Reply sent to client
       AND messages.timestamp >  now - 10/1440
       AND messages.timestamp <= now - 5/1440;

  -- Record results
  FOR a IN (
    SELECT type, svcclass,
           nvl(sum(decode(msg_no, 53, requests, 0)), 0) Wait,
           nvl(sum(decode(msg_no, 56, requests, 0)), 0) D2D,
           nvl(sum(decode(msg_no, 57, requests, 0)), 0) Recall,
           nvl(sum(decode(msg_no, 60, requests, 0)), 0) Staged,
           nvl(sum(requests), 0) total
      FROM (
        SELECT type, svcclass, msg_no, count(*) requests FROM (
          SELECT * FROM (
            -- Get the first message issued for all subrequests of interest. This
            -- will indicate to us whether the request was a hit or a miss
            SELECT msg_no, type, svcclass,
                   RANK() OVER (PARTITION BY subreqid
                          ORDER BY timestamp ASC, timeusec ASC) rank
              FROM (
                -- Extract all subrequests processed by the stager that resulted in
                -- read type access
                SELECT messages.reqid, messages.subreqid, messages.timestamp,
                       messages.timeusec, messages.msg_no,
                       max(decode(params.name, 'Type',     params.value, NULL)) type,
                       max(decode(params.name, 'SvcClass', params.value, 'default')) svcclass
                  FROM dlf_messages messages, dlf_str_param_values params
                 WHERE messages.id = params.id
                   AND messages.severity = 8  -- System
                   AND messages.facility = 22 -- Stager
                   AND messages.msg_no IN (53, 56, 57, 60)
                   AND messages.timestamp >  now - 10/1440
                   AND messages.timestamp <= now - 3/1440
                   AND params.name IN ('Type', 'SvcClass')
                   AND params.timestamp >  now - 10/1440
                   AND params.timestamp <= now - 3/1440
                 GROUP BY messages.reqid, messages.subreqid, messages.timestamp,
                          messages.timeusec, messages.msg_no
              ) results
             -- Filter the subrequests so that we only process requests which entered
             -- the system through the request handler in the last sampling interval.
             -- This stops us from recounting subrequests that were restarted
             INNER JOIN CacheEfficiencyHelper helper
                ON results.reqid = helper.reqid)
         ) WHERE rank = 1
       GROUP BY type, svcclass, msg_no)
     GROUP BY type, svcclass
  )
  LOOP
    INSERT INTO DiskCacheEfficiencyStats
      (timestamp, interval, wait, type, svcclass, d2d, recall, staged, total)
    VALUES (now - 5/1440, 300, a.wait, a.type, a.svcclass, a.d2d, a.recall, a.staged, a.total);
  END LOOP;
END;
/


/* PL/SQL method implementing statsMigratedFiles
 *
 * Provides statistical information on the number of files migrated to tape and the
 * total data volume transferred broken down by service class and tape pool.
 */
CREATE OR REPLACE PROCEDURE statsMigratedFiles (now IN DATE) AS
BEGIN
  -- Stats table: FilesMigratedStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT svcclass, tapepool, count(*) files, sum(params.value) totalsize
      FROM (
        -- Extract the messages to indicate when a file has been migrated
        SELECT messages.id,
               max(decode(params.name, 'SVCCLASS', params.value, NULL)) svcclass,
               max(decode(params.name, 'TAPEPOOL', params.value, NULL)) tapepool
          FROM dlf_messages messages, dlf_str_param_values params
         WHERE messages.id = params.id
           AND messages.severity = 8 -- System
           AND messages.facility = 1 -- migrator
           AND messages.msg_no = 55  -- File staged
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
           AND params.name IN ('SVCCLASS', 'TAPEPOOL')
           AND params.timestamp >  now - 10/1440
           AND params.timestamp <= now - 5/1440
         GROUP BY messages.id
      ) results
      -- Attach the filesize to the previously collected information
     INNER JOIN dlf_num_param_values params
        ON results.id = params.id
       AND params.name = 'FILESIZE'
       AND params.timestamp >  now - 10/1440
       AND params.timestamp <= now - 5/1440
     GROUP BY svcclass, tapepool
  )
  LOOP
    INSERT INTO FilesMigratedStats
      (timestamp, interval, svcclass, tapepool, totalFiles, totalSize)
    VALUES (now - 5/1440, 300, a.svcclass, a.tapepool, a.files, a.totalsize);
  END LOOP;
END;
/


/* PL/SQL method implementing statsReplication
 *
 * Provides statistical information on disk copy replication requests both across
 * service classes and internally within the same service class.
 */
CREATE OR REPLACE PROCEDURE statsReplication (now IN DATE) AS
BEGIN
  -- Stats table: ReplicationStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT src, dest, count(*) transferred, sum(params.value) totalsize,
           min(params.value) min, max(params.value) max, avg(params.value) avg,
           stddev_pop(params.value) stddev, median(params.value) median
      FROM (
        SELECT params.id,
               substr(params.value, 0, instr(params.value, '->', 1) - 2) src,
               substr(params.value, instr(params.value, '->', 1) + 3) dest
          FROM dlf_messages messages, dlf_str_param_values params
         WHERE messages.id = params.id
           AND messages.severity = 8  -- System
           AND messages.facility = 23 -- DiskCopyTransfer
           AND messages.msg_no = 39   -- DiskCopy Transfer successful
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
           AND params.name = 'Direction'
           AND params.timestamp >  now - 10/1440
           AND params.timestamp <= now - 5/1440
      ) results
     -- Attach the size of the file to each replication request. As a result of
     -- this we will have one line per request detailing the direction of the
     -- transfer and the amount of data transferred
     INNER JOIN dlf_num_param_values params
        ON results.id = params.id
       AND params.name = 'FileSize'
       AND params.timestamp >  now - 10/1440
       AND params.timestamp <= now - 5/1440
     GROUP BY src, dest
  )
  LOOP
    INSERT INTO ReplicationStats
      (timestamp, interval, sourceSvcClass, destSvcClass, transferred, totalSize, minSize, maxSize, avgSize, stddevSize, medianSize)
    VALUES (now - 5/1440, 300, a.src, a.dest, a.transferred, a.totalsize, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;
/


/* PL/SQL method implementing statsTapeRecalled
 *
 * Provides statistical information on who triggered a tape recall, how many files
 * were requested, the status of the tape as the request was processed and the type
 * of request that triggered the recall.
 *
 * Example output:
 *   Type                     Username Groupname TapeVID TapeStatus   Files MountsPerDay
 *   StagePrepareToGetRequest waldron  c3        I10488	 TAPE_PENDING 10    0
 *   StagePrepareToGetRequest waldron  c3        I10487	 TAPE_PENDING 8     0
 *   StagePrepareToGetRequest waldron  c3        I10486	 TAPE_PENDING 2     0
 *   StagePrepareToGetRequest waldron  c3        I06983	 TAPE_PENDING 854   0
 */
CREATE OR REPLACE PROCEDURE statsTapeRecalled (now IN DATE) AS
BEGIN
  -- Stats table: TapeRecalledStats
  -- Frequency: 5 minutes

  -- Populate the TapeMountsHelper table with a line for every recaller that has
  -- been started in the last sampling interval. This will act as a summary table
  INSERT INTO TapeMountsHelper
    SELECT messages.timestamp, messages.tapevid
      FROM dlf_messages messages
     WHERE messages.severity = 8 -- System
       AND messages.facility = 2 -- Recaller
       AND messages.msg_no = 13  -- Recaller started
       AND messages.subreqid <> '00000000-0000-0000-0000-000000000000'
       AND messages.timestamp >  now - 10/1440
       AND messages.timestamp <= now - 5/1440;

  -- Record results
  FOR a IN (
    SELECT type, username, groupname, results.tapevid, tapestatus,
           count(*) files, sum(params.value) totalsize,
           max(nvl(mounts.mounted, 0)) mounted
      FROM (
        -- Extract all requests from the stager which triggered a tape recall
        -- including the request type, username and groupname associated with
        -- that request
        SELECT messages.id, messages.tapevid,
               max(decode(params.name, 'Type',       params.value, NULL)) type,
               max(decode(params.name, 'Username',   params.value, NULL)) username,
               max(decode(params.name, 'Groupname',  params.value, NULL)) groupname,
               max(decode(params.name, 'TapeStatus', params.value, NULL)) tapestatus
          FROM dlf_messages messages, dlf_str_param_values params
         WHERE messages.id = params.id
           AND messages.severity = 8  -- System
           AND messages.facility = 22 -- Stager
           AND messages.msg_no = 57   -- Triggering Tape Recall
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
           AND params.name IN ('Type', 'Username', 'Groupname', 'TapeStatus')
           AND params.timestamp >  now - 10/1440
           AND params.timestamp <= now - 5/1440
         GROUP BY messages.id, messages.tapevid
      ) results
     -- Attach the file size to be recalled
     INNER JOIN dlf_num_param_values params
        ON results.id = params.id
       AND params.name = 'FileSize'
       AND params.timestamp >  now - 10/1440
       AND params.timestamp <= now - 5/1440
     -- Attached the number of mounts which took place for the tape over the
     -- last 24 hours
     LEFT JOIN (
       SELECT helper.tapevid, count(*) mounted
         FROM TapeMountsHelper helper
        WHERE helper.timestamp > (now - 1) - 5/1440
        GROUP BY helper.tapevid) mounts
        ON results.tapevid = mounts.tapevid
     GROUP BY type, username, groupname, results.tapevid, tapestatus
  )
  LOOP
    INSERT INTO TapeRecalledStats
      (timestamp, interval, type, username, groupname, tapeVid, tapeStatus, files, totalSize, mountsPerDay)
    VALUES (now - 5/1440, 300, a.type, a.username, a.groupname, a.tapevid, a.tapestatus, a.files, a.totalsize, a.mounted);
  END LOOP;
END;
/


/* PL/SQL method implementing statsProcessingTime
 *
 * Provides statistics on the processing time in seconds of requests in the Stager
 * and RequestHandler daemons
 */
CREATE OR REPLACE PROCEDURE statsProcessingTime (now IN DATE) AS
BEGIN
  -- Stats table: ProcessingTimeStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT facility.fac_name daemon, params.value type, count(*) requests,
           min(results.value) min,
           max(results.value) max,
           avg(results.value) avg,
           stddev_pop(results.value) stddev,
           median(results.value) median
      FROM (
        -- Extract all the processing time values for the Stager
        SELECT messages.id, messages.facility, params.value
          FROM dlf_messages messages, dlf_num_param_values params
         WHERE messages.id = params.id
           AND messages.severity = 10 -- Monitoring
           AND messages.facility = 22 -- Stager
           AND messages.msg_no = 25   -- Request processed
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
           AND params.name = 'ProcessingTime'
           AND params.timestamp >  now - 10/1440
           AND params.timestamp <= now - 5/1440
         UNION
        -- Extract all the processing time values for the RequestHandler
        SELECT messages.id, messages.facility, params.value
          FROM dlf_messages messages, dlf_num_param_values params
         WHERE messages.id = params.id
           AND messages.severity = 10 -- Monitoring
           AND messages.facility = 4  -- RequestHandler
           AND messages.msg_no = 10   -- Reply sent to client
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
           AND params.name = 'ElapsedTime'
           AND params.timestamp >  now - 10/1440
           AND params.timestamp <= now - 5/1440
      ) results
     -- Attach the request type
     INNER JOIN dlf_str_param_values params
        ON results.id = params.id
       AND params.name = 'Type'
       AND params.timestamp >  now - 10/1440
       AND params.timestamp <= now - 5/1440
     -- Resolve the facility number to a name
     INNER JOIN dlf_facilities facility
        ON results.facility = facility.fac_no
     GROUP BY facility.fac_name, params.value
  )
  LOOP
    INSERT INTO ProcessingTimeStats
      (timestamp, interval, daemon, type, requests, minTime, maxTime, avgTime, stddevTime, medianTime)
    VALUES (now - 5/1440, 300, a.daemon, a.type, a.requests, a.min, a.max, a.avg, a.stddev, a.median);
  END LOOP;
END;
/


/* PL/SQL method implementing statsClientVersion
 *
 * Provides statistics on the different client versions seen by the RequestHandler
 */
CREATE OR REPLACE PROCEDURE statsClientVersion (now IN DATE) AS
BEGIN
  -- Stats table: ClientVersionStats
  -- Frequency: 5 minutes
  FOR a IN (
    SELECT clientVersion, count(*) requests
      FROM (
        SELECT nvl(params.value, 'Unknown') clientVersion
          FROM dlf_messages messages, dlf_str_param_values params
         WHERE messages.id = params.id
           AND messages.severity = 10 -- Monitoring
           AND messages.facility = 4  -- RequestHandler
           AND messages.msg_no = 10   -- Reply sent to client
           AND messages.timestamp >  now - 10/1440
           AND messages.timestamp <= now - 5/1440
           AND params.name = 'ClientVersion'
           AND params.timestamp >  now - 10/1440
           AND params.timestamp <= now - 5/1440)
     GROUP BY clientVersion
  )
  LOOP
    INSERT INTO ClientVersionStats
      (timestamp, interval, clientVersion, requests)
    VALUES (now - 5/1440, 300, a.clientVersion, a.requests);
  END LOOP;
END;
/


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
      START_DATE      => TRUNC(SYSDATE) + 1/24,
      REPEAT_INTERVAL => 'FREQ=DAILY',
      ENABLED         => TRUE,
      COMMENTS        => 'Daily partitioning creation');

  -- Create a db job to be run every day and drop old data from the database
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'archiveDataJob',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN archiveData(-1); END;',
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
                            statsReplication(now);
                            statsTapeRecalled(now);
                            statsProcessingTime(now);
                            statsClientVersion(now);
                          END;',
      START_DATE      => SYSDATE,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=5',
      ENABLED         => TRUE,
      COMMENTS        => 'CASTOR2 Monitoring Statistics (5 Minute Frequency)');
END;
/


/* Trigger the initial creation of partitions */
BEGIN
  createPartitions();
END;
/


/* End-of-File */
