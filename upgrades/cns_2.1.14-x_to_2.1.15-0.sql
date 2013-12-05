/******************************************************************************
 *                 cns_2.1.14-x_to_2.1.15-0.sql
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
 * This script upgrades a CASTOR v2.1.14-x CNS database to v2.1.15-0
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
     AND release = '2_1_15_0'
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
     AND release LIKE '2_1_14%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we cannot apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts for the CNS before this one.');
END;
/

/* Verify that the open mode switch has been performed */
DECLARE
  openMode VARCHAR(100);
BEGIN
  SELECT value INTO openMode FROM CastorConfig
   WHERE key = 'openmode';
  IF openMode != 'N' THEN
    -- Error, we cannot apply this script
    raise_application_error(-20000, 'Nameserver Open mode value is '|| openMode ||', not the expected value N(ative). Please run the cns_2.1.14_switch-open-mode.sql script before this one.');
  END IF;
END;


INSERT INTO UpgradeLog (schemaVersion, release, type)
VALUES ('2_1_15_0', '2_1_15_0', 'NON TRANSPARENT');
COMMIT;

-- Not needed any longer
DELETE FROM CastorConfig WHERE key = 'openmode';
DROP PROCEDURE update2114Data;

-- enforce constraint on stagerTime. This was to be done as part of the post 2.1.14 upgrade phase, but it is not a transparent operation...
ALTER TABLE Cns_file_metadata MODIFY (stagerTime CONSTRAINT NN_File_stagerTime NOT NULL);


/* PL/SQL code update */


/* Flag the schema upgrade as COMPLETE */
/***************************************/

UPDATE UpgradeLog SET endDate = sysdate, state = 'COMPLETE'
 WHERE release = '2_1_15_0';
COMMIT;
