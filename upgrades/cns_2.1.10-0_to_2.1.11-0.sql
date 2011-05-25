/******************************************************************************
 *                 cns_2.1.10-0_to_2.1.11-0.sql
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
 * This script upgrades a CASTOR v2.1.10-0 CNS database to v2.1.11-0
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors */
WHENEVER SQLERROR EXIT FAILURE
BEGIN
  -- If we've encountered an error rollback any previously non committed
  -- operations. This prevents the UPDATE of the UpgradeLog from committing
  -- inconsistent data to the database.
  ROLLBACK;
  UPDATE UpgradeLog
     SET failureCount = failureCount + 1
   WHERE schemaVersion = '2_1_9_3'
     AND release = '2_1_11_0'
     AND state != 'COMPLETE';
  COMMIT;
END;
/

/* Verify that the script is running against the correct schema and version */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM CastorVersion
   WHERE schemaName = 'CNS'
     AND release LIKE '2_1_10_0%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts for the CNS before this one.');
END;
/

INSERT INTO UpgradeLog (schemaVersion, release, type)
VALUES ('2_1_9_3', '2_1_11_0', 'TRANSPARENT');
COMMIT;

/* Schema changes go here */
/**************************/

/* #79320: CM: Remove VIRTUAL ID functionality to improve code maintainability */
DECLARE
  tableName VARCHAR2(30);
BEGIN
  SELECT table_name INTO tableName
    FROM user_tables WHERE table_name = 'CNS_USERINFO';
  EXECUTE IMMEDIATE 'DROP TABLE CNS_UserInfo';
EXCEPTION WHEN NO_DATA_FOUND THEN
  NULL;
END;
/

DECLARE
  tableName VARCHAR2(30);
BEGIN
  SELECT table_name INTO tableName
    FROM user_tables WHERE table_name = 'CNS_GROUPINFO';
  EXECUTE IMMEDIATE 'DROP TABLE CNS_GroupInfo';
EXCEPTION WHEN NO_DATA_FOUND THEN
  NULL;
END;
/

/* #71565 (RFE: provide weighted compression rate in nslisttape --summarize) */
DECLARE
  unused VARCHAR2(30);
BEGIN
  SELECT index_name INTO unused
    FROM user_indexes
   WHERE index_name = 'I_SEG_METADATA_TAPESUM';
EXCEPTION WHEN NO_DATA_FOUND THEN
  EXECUTE IMMEDIATE 'CREATE INDEX I_seg_metadata_tapesum
                     ON Cns_seg_metadata (vid, s_fileid, segsize, compression)';
  EXECUTE IMMEDIATE 'DROP INDEX I_seg_metadata_vid_fid_segsize';
END;
/

/* Recompile all invalid procedures, triggers and functions */
/************************************************************/
BEGIN
  FOR a IN (SELECT object_name, object_type
              FROM user_objects
             WHERE object_type IN ('PROCEDURE', 'TRIGGER', 'FUNCTION')
               AND status = 'INVALID')
  LOOP
    IF a.object_type = 'PROCEDURE' THEN
      EXECUTE IMMEDIATE 'ALTER PROCEDURE '||a.object_name||' COMPILE';
    ELSIF a.object_type = 'TRIGGER' THEN
      EXECUTE IMMEDIATE 'ALTER TRIGGER '||a.object_name||' COMPILE';
    ELSE
      EXECUTE IMMEDIATE 'ALTER FUNCTION '||a.object_name||' COMPILE';
    END IF;
  END LOOP;
END;
/

/* Flag the schema upgrade as COMPLETE */
/***************************************/
UPDATE UpgradeLog SET endDate = sysdate, state = 'COMPLETE'
 WHERE release = '2_1_11_0';
COMMIT;
