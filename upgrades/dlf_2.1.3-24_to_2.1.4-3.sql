/******************************************************************************
 *              dlf_2.1.3-24_to_2.1.4-1.sql
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
 * @(#)$RCSfile: dlf_2.1.3-24_to_2.1.4-3.sql,v $ $Release: 1.2 $ $Release$ $Date: 2007/08/28 14:37:42 $ $Author: sponcec3 $
 *
 * This script upgrades a CASTOR v2.1.3-24 DLF database to 2.1.4-1
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus and sql developer */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  found INTEGER;
BEGIN
  SELECT count(*) INTO found FROM dlf_version WHERE release = '2_1_3_24';
  IF found = 0 THEN
    -- Error, we can't apply this script
   raise_application_error(-20000, 'PL/SQL revision mismatch. Please run previous upgrade scripts before this one.');
  END IF;
END;

/* Upgrade dlf_version to new model */
UPDATE dlf_version SET release = '2_1_4_3';

/* Update facilities */
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (21, 'JobManager');
UPDATE dlf_facilities SET fac_name = 'RequestHandler' WHERE fac_name = 'RHLog';
UPDATE dlf_facilities SET fac_name = 'Cleaning' WHERE fac_name = 'cleaning';

/* Partition creation */
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
                           DATAFILE SIZE 100M
                           AUTOEXTEND ON NEXT 200M 
                           MAXSIZE 30G
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
