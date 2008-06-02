/******************************************************************************
 *              dlf_2.1.7-7_to_2.1.7-8.sql
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
 * @(#)$RCSfile: dlf_2.1.7-7_to_2.1.7-10.sql,v $ $Release: 1.2 $ $Release$ $Date: 2008/06/02 13:43:36 $ $Author: waldron $
 *
 * This script upgrades a CASTOR v2.1.7-7 DLF database to 2.1.7-8
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus and sql developer */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM dlf_version WHERE release LIKE '2_1_7_7%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL revision mismatch. Please run previous upgrade scripts before this one.');
END;

UPDATE dlf_version SET release = '2_1_7_8';
COMMIT;

/* SQL statement for table ClientVersionStats */
CREATE TABLE ClientVersionStats (timestamp DATE NOT NULL, interval NUMBER, clientVersion VARCHAR2(255), requests NUMBER)
  PARTITION BY RANGE (timestamp) (PARTITION MAX_VALUE VALUES LESS THAN (MAXVALUE));


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


/* Modify the scheduler job responsible for statistic creation */
BEGIN
  -- Drop the job
  DBMS_SCHEDULER.DROP_JOB('STATISTICJOB', TRUE);

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
