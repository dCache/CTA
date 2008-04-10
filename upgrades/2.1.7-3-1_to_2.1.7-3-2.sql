/******************************************************************************
 *              2.1.7-3-1_to_2.1.7-3-2.sql
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
 * @(#)$RCSfile: 2.1.7-3-1_to_2.1.7-3-2.sql,v $ $Release: 1.2 $ $Release$ $Date: 2008/04/10 15:29:58 $ $Author: gtaur $
 *
 * This script upgrades a CASTOR v2.1.7-3-1 database into v2.1.7-3-2
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus */
WHENEVER SQLERROR EXIT FAILURE;

/* Version cross check and update */
DECLARE
  unused VARCHAR(100);
BEGIN
  SELECT release INTO unused FROM CastorVersion WHERE release = '2_1_7_3_1';
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Error, we can't apply this script
  raise_application_error(-20000, 'PL/SQL release mismatch. Please run previous upgrade scripts before this one.');
END;

UPDATE CastorVersion SET release = '2_1_7_3_2';
COMMIT;


CREATE OR REPLACE PROCEDURE attachTapeCopiesToStreams
(tapeCopyIds IN castor."cnumList",
 tapePoolIds IN castor."cnumList")
AS
  streamId NUMBER; -- stream attached to the tapepool
  counter NUMBER := 0;
  unused NUMBER;
BEGIN
  -- add choosen tapecopies to all Streams associated to the tapepool used by the policy 
  FOR i IN tapeCopyIds.FIRST .. tapeCopyIds.LAST LOOP
    BEGIN 
      UPDATE TapeCopy SET Status=2 WHERE Status=7 AND id = tapeCopyIds(i) RETURNING id into unused;
      FOR streamId IN (SELECT id FROM Stream WHERE Stream.tapepool= tapePoolIds(i)) LOOP
      	  DECLARE CONSTRAINT_VIOLATED EXCEPTION;
          PRAGMA EXCEPTION_INIT (CONSTRAINT_VIOLATED,-1);
          BEGIN 
          -- check if it is not been resurrected by the start of a new mighunter on the same svcclass
             INSERT INTO stream2tapecopy (parent ,child) VALUES (streamId.id, tapeCopyIds(i));
          EXCEPTION WHEN CONSTRAINT_VIOLATED THEN
      	     UPDATE tapecopy set status=1 where id=tapeCopyIds(i);
          END;
      END LOOP;
    EXCEPTION WHEN NO_DATA_FOUND THEN
     -- Go on the tapecopy has been resurrected or migrated
      NULL;
    END;
     
    counter := counter + 1;
    IF counter = 100 THEN
         counter := 0;
         COMMIT;
    END IF;
  END LOOP;
  COMMIT;
END;


