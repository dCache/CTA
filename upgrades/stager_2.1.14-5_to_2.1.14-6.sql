/******************************************************************************
 *                 stager_2.1.14-5_to_2.1.14-6.sql
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
 * This script upgrades a CASTOR v2.1.14-5 STAGER database to v2.1.14-6
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors */
WHENEVER SQLERROR EXIT FAILURE
BEGIN
  -- If we have encountered an error rollback any previously non committed
  -- operations. This prevents the UPDATE of the UpgradeLog from committing
  -- inconsistent data to the database.
  ROLLBACK;
  UPDATE UpgradeLog
     SET failureCount = failureCount + 1
   WHERE schemaVersion = '2_1_14_2'
     AND release = '2_1_14_6'
     AND state != 'COMPLETE';
  COMMIT;
END;
/

/* Verify that the script is running against the correct schema and version */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM CastorVersion
   WHERE schemaName = 'STAGER'
     AND release LIKE '2_1_14_5%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we cannot apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts for the STAGER before this one.');
END;
/

INSERT INTO UpgradeLog (schemaVersion, release, type)
VALUES ('2_1_14_2', '2_1_14_6', 'NON TRANSPARENT');
COMMIT;

/* Job management */
BEGIN
  FOR a IN (SELECT * FROM user_scheduler_jobs)
  LOOP
    -- Stop any running jobs
    IF a.state = 'RUNNING' THEN
      dbms_scheduler.stop_job(a.job_name, force=>TRUE);
    END IF;
    -- Schedule the start date of the job to 15 minutes from now. This
    -- basically pauses the job for 15 minutes so that the upgrade can
    -- go through as quickly as possible.
    dbms_scheduler.set_attribute(a.job_name, 'START_DATE', SYSDATE + 15/1440);
  END LOOP;
END;
/

-- Fix constraint - transparent thanks to the NOVALIDATE clause
ALTER TABLE DiskCopy DROP CONSTRAINT CK_DiskCopy_GCType;
ALTER TABLE DiskCopy
  ADD CONSTRAINT CK_DiskCopy_GcType
  CHECK (gcType IN (0, 1, 2, 3, 4, 5, 6, 7)) ENABLE NOVALIDATE;

-- Draining schema change. Dropping the content because of the NOT NULL constraint.
TRUNCATE TABLE DrainingErrors;
ALTER TABLE DrainingErrors ADD (diskCopy INTEGER, timeStamp NUMBER CONSTRAINT NN_DrainingErrors_TimeStamp NOT NULL);

CREATE INDEX I_DrainingErrors_DC ON DrainingErrors (diskCopy);
ALTER TABLE DrainingErrors
  ADD CONSTRAINT FK_DrainingErrors_DC
    FOREIGN KEY (diskCopy)
    REFERENCES DiskCopy (id);

ALTER TABLE Disk2DiskCopyJob RENAME COLUMN replacedDcId TO srcDcId;
ALTER TABLE Disk2DiskCopyJob ADD (dropSource INTEGER DEFAULT 0 CONSTRAINT NN_Disk2DiskCopyJob_dropSource NOT NULL);
ALTER TABLE Disk2DiskCopyJob ADD CONSTRAINT FK_Disk2DiskCopyJob_SrcDcId
  FOREIGN KEY (srcDcId) REFERENCES DiskCopy(id);
CREATE INDEX I_Disk2DiskCopyJob_SrcDC ON Disk2DiskCopyJob(srcDcId);

/* Draining sensitivity */
UPDATE CastorConfig
   SET key = 'Sensitivity',
       description = 'The rebalancing sensitivity (in percent) : if a fileSystem is at least this percentage fuller than the average of the diskpool where it lives, rebalancing will fire.'
 WHERE key = 'Sensibility';
        
/* Bug #103792: RFE: provide the ability to selectively enable/disable protocols */
INSERT INTO CastorConfig
  VALUES ('Stager', 'Protocols', 'rfio rfio3 root gsiftp xroot', 'The list of protocols accepted by the system.')

/* For deleteDiskCopy */
DROP TABLE DeleteDiskCopyHelper;
CREATE GLOBAL TEMPORARY TABLE DeleteDiskCopyHelper
  (dcId INTEGER CONSTRAINT PK_DDCHelper_dcId PRIMARY KEY, fileId INTEGER, fStatus CHAR(1), rc INTEGER)
  ON COMMIT PRESERVE ROWS;
CREATE INDEX I_DDCHelper_FileId ON DeleteDiskCopyHelper(fileId);


XXX add disk2DiskCopyEnded PL/SQL procedure

/* Bug #103715: Requests for disk-to-disk copies wait forever in status WAITTAPERECALL.
 * Cleanup old requests stuck in status 4. To avoid a storm in the stager, the old ones
 * are archived straight, and only the new ones are restarted. New = more recent than 24h.
 */
UPDATE SubRequest SET status = 1 WHERE status = 4 AND creationTime > getTime() - 86400;
COMMIT;
BEGIN
  FOR s in (SELECT id FROM SubRequest WHERE status = 4) LOOP
    archiveSubReq(s.id, dconst.SUBREQUEST_FINISHED);
  END LOOP;
  COMMIT;
END;
/

XXX TODO add ALL remaining PL/SQL code


/* Recompile all invalid procedures, triggers and functions */
/************************************************************/
BEGIN
  recompileAll();
END;
/

/* Flag the schema upgrade as COMPLETE */
/***************************************/
UPDATE UpgradeLog SET endDate = systimestamp, state = 'COMPLETE'
 WHERE release = '2_1_14_6';
COMMIT;
