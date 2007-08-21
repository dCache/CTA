/******************************************************************************
 *              dlf_2.1.3-24_to_2.1.4-0.sql
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
 * @(#)$RCSfile: dlf_2.1.3-24_to_2.1.4-0.sql,v $ $Release: 1.2 $ $Release$ $Date: 2007/08/21 15:50:45 $ $Author: itglp $
 *
 * This script upgrades a CASTOR v2.1.3-24 DLF database to 2.1.4.0
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
UPDATE dlf_version SET release = '2_1_4_0';

/* Update facilities */
INSERT INTO dlf_facilities (fac_no, fac_name) VALUES (21, 'JobManager');
UPDATE dlf_facilities SET fac_name = 'RequestHandler' WHERE fac_name = 'RHLog';
UPDATE dlf_facilities SET fac_name = 'Cleaning' WHERE fac_name = 'cleaning';
COMMIT;

