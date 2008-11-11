/******************************************************************************
 *              repack_2.1.8-2_to_2.1.8-3.sql
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
 * @(#)$RCSfile: repack_2.1.8-2_to_2.1.8-3.sql,v $ $Release: 1.2 $ $Release$ $Date: 2008/11/11 10:49:03 $ $Author: waldron $
 *
 * This script upgrades a CASTOR v2.1.8-2 REPACK database into v2.1.8-3
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM CastorVersion WHERE release LIKE '2_1_8_2%';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts before this one.');
END;
/

UPDATE CastorVersion SET release = '2_1_8_3';
COMMIT;

/* Recompile all procedures */
/***************************/
BEGIN
  FOR a IN (SELECT object_name, object_type
              FROM all_objects
             WHERE object_type = 'PROCEDURE'
               AND status = 'INVALID')
  LOOP
    EXECUTE IMMEDIATE 'ALTER PROCEDURE '||a.object_name||' COMPILE';
  END LOOP;
END;
/
