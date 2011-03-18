/******************************************************************************
 *                 castor/db/switchToRtcpclientd.sql
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
 *
 *
 * @author Giulia Taurelli, Nicola Bessone and Steven Murray
 *****************************************************************************/

/* Stop on errors - this only works from sqlplus */
WHENEVER SQLERROR EXIT FAILURE;

BEGIN
  -- Do nothing and rise an exception if the database is already compatible 
  -- with the rtcpclientd daemon
  IF rtcpclientdIsRunning THEN
    raise_application_error(-20000,
      'PL/SQL switchToRTCPClientd: RTCPClientd already running.');
  END IF;
END;
/

-- The database is about to be modified and is therefore not compatible with
-- either the rtcpclientd daemon or the tape gateway daemon
UPDATE CastorConfig
  SET value = 'SwitchingToRTCPClientd'
  WHERE
    class = 'tape' AND
    key   = 'interfaceDaemon';
COMMIT;

-- The major reset procedure
-- Tape copies go back to to be migrated and we restart from basically scratch for most of the states.
-- From TAPECOPY_CREATED, leave.
-- From TAPECOPY_TOBEMIGRATED, leave.
-- From TAPECOPY_WAITINSTREAMS, leave.
-- From TAPECOPY_SELECTED, move back to TAPECOPY_TOBEMIGRATED, remove VID, no segments expected.
-- From TAPECOPY_TOBERECALLED, leave.
-- From TAPECOPY_STAGED, reset to TOBEMIGRATED. (will be remigrated by rtcpclientd.)
-- From TAPECOPY_FAILED, leave.
-- From TAPECOPY_WAITPOLICY, leave.
-- From TAPECOPY_REC_RETRY, move to TAPECOPY_TOBERECALLED, segment is left as is.
-- From TAPECOPY_MIG_RETRY, move back to TO BE MIGRATED.

BEGIN
  -- Deal with Migrations
  -- 1) Ressurect tapecopies for migration
  UPDATE TapeCopy tc SET tc.status = TCONST.TAPECOPY_TOBEMIGRATED,
                         tc.VID = NULL
    WHERE tc.status IN (TCONST.TAPECOPY_WAITPOLICY, TCONST.TAPECOPY_WAITINSTREAMS,
                        TCONST.TAPECOPY_SELECTED, TCONST.TAPECOPY_MIG_RETRY);
                        -- STAGED and FAILED can stay the same, other states are for recalls.
END;
/

-- Convert all the tapecopies error counts and error codes for tapecopies with a
-- non-zero error count into a collection of RETRIED segments all with the same
-- code. (one segment per retry).
DECLARE
  varSegmentType  INTEGER;
BEGIN
  -- Get the segment's type tragic number
  SELECT O2T.Type INTO varSegmentType 
    FROM Type2Obj O2T
   WHERE O2T.Object = 'Segment';
  -- Find all tape copies with a non NULL and non-zero error count.
  FOR varTc IN (
    SELECT TC.id Id FROM TapeCopy TC
     WHERE TC.nbretry IS NOT NULL
       AND TC.nbretry > 0
  ) LOOP
    DECLARE
      varNbretry   INTEGER;
      varErrorCode INTEGER;
    BEGIN
      -- For each tapecopy found, get NbRetry and ErrorCode
      SELECT TC.nbretry, TC.errorCode
        INTO varNbretry, varErrorCode
        FROM TapeCopy TC WHERE TC.id=varTc.Id;
      -- Create nbRetried RETRIED segments for this tapecopy. We cannot do better than having them identical.
      FOR i IN 0..(varNbretry - 1) LOOP
        DECLARE
          varSegID  INTEGER;
        BEGIN
          -- Determine the ID for the segment
          SELECT ids_seq.nextval INTO varSegId FROM DUAL;
          -- Create the segment.
          INSERT INTO Segment (Id,          ErrorCode, Copy,
                                  Status,                 ErrMsgTxt) 
          VALUES              (varSegId, varErrorCode, varTc.Id, 
                                  TConst.SEGMENT_RETRIED, 'Converted error segment from tape gateway');
          INSERT INTO Id2Type (Id,       Type)
          VALUES              (varSegId, varSegmentType);
        END;
      END LOOP;
      UPDATE TapeCopy TC
         SET TC.ErrorCode = NULL,
             TC.NbRetry = NULL
       WHERE TC.Id = varTc.Id;
    END;
  END LOOP;
END;
/

-- Streams do not need to be kept. The mighunter will recreate them all.
DELETE FROM Stream2TapeCopy;
DELETE FROM STREAM;

-- From STREAM_PENDING, Leave as is
-- From STREAM_WAITDRIVE, Set to pending
-- From STREMA_WAITMOUNT, set to pending
-- From STREAM_RUNING, set to pending
-- From STREAM_WAITSPACE, Leave as is.
-- From STREAM_CREATED, Leave as is.
-- From STREAM_STOPPED, Leave as is.
-- From STREAM_WAITPOLICY, Leave as is.
-- From STREAM_TOBESENTTOVDQM, we have a busy tape attached and they have to be free.

-- Segments (the gateway is not going to use)
-- From SEGMENT_UNPROCESSED, Leave as is.
-- From SEGMENT_FILECOPIED, (apparently unused)
-- From SEGMENT_FAILED, Reset to unprocessed.
-- From SEGMENT_SELECTED, Move to unprocessed
-- From SEGMENT_RETRIED, Delete segment from the database.

  -- Deal with Recalls
  -- Segments
BEGIN
  UPDATE Segment SET status = TCONST.SEGMENT_UNPROCESSED
    WHERE status IN (TCONST.SEGMENT_SELECTED); -- Resurrect selected segments
END;
/

-- Tapes
-- From TAPE_UNSED, Leave as is.
-- From TAPE_PENDING, Leave as is.
-- From TAPE_WAITDRIVE, reset to PENDING
-- From TAPE_WAITMOUNT, reset to PENDING
-- From TAPE_MOUNTED, reset to PENDING
-- From TAPE_FINISHED, Leave as is. (Assuming it is an end state).
-- From TAPE_FAILED, Leave as is.
-- From TAPE_UNKNOWN, Leave as is. (Assuming it is an end state).
-- From TAPE_WAITPOLICY, Leave as is. (For the rechandler to take).

BEGIN
  -- Write tape mounts can be removed.
  DELETE FROM Tape T WHERE T.tpMode = TCONST.TPMODE_WRITE;
  -- Unreferenced read tape mounts can be dumped. (at the end)
  
   -- Read tape mounts that are not referenced by any segment are dropped
   DELETE FROM Tape T
    WHERE T.tpMode = TCONST.TPMODE_READ AND (
       NOT EXISTS ( SELECT Seg.id FROM Segment Seg
      WHERE Seg.tape = T.id));  -- Resurrect the tapes running for recall
  -- Reset active read tape mounts to pending (re-do)
  UPDATE Tape SET status = TCONST.TAPE_PENDING
    WHERE tpmode = TCONST.TPMODE_READ AND 
          status IN (TCONST.TAPE_WAITDRIVE, TCONST.TAPE_WAITMOUNT, 
          TCONST.TAPE_MOUNTED);
  -- Resurrect tapes with UNPROCESSED segments (preserving WAITPOLICY state)
  UPDATE Tape T SET T.status = TCONST.TAPE_PENDING
    WHERE T.status NOT IN (TCONST.TAPE_WAITPOLICY, TCONST.TAPE_PENDING) AND EXISTS
    ( SELECT Seg.id FROM Segment Seg
      WHERE Seg.status = TCONST.SEGMENT_UNPROCESSED AND Seg.tape = T.id);
   -- We keep the tapes referenced by failed segments and move them to UNUSED. Those are the ones WITHOUT
   -- unprocessed segments
   UPDATE Tape T SET T.status = TCONST.TAPE_UNUSED
    WHERE NOT EXISTS
    ( SELECT Seg.id FROM Segment Seg
      WHERE Seg.status = TCONST.SEGMENT_UNPROCESSED AND Seg.tape = T.id);
END;
/

-- Reset all tape gateway columns to NULL
UPDATE TapeCopy SET
   fSeq                 = NULL,
   tapeGatewayRequestId = NULL,
   vid                  = NULL,
   fileTransactionId    = NULL;
UPDATE Tape SET
   startTime            = NULL,
   lastVdqmPingTime     = NULL,
   vdqmVolReqId         = NULL,
   lastFseq             = NULL,
   tapeGatewayRequestId = NULL;
UPDATE Stream SET
   vdqmVolReqId         = NULL,
   tapeGatewayRequestId = NULL;


-- The database is now compatible with the rtcpclientd daemon
UPDATE CastorConfig
  SET value = 'rtcpclientd'
  WHERE
    class = 'tape' AND
    key   = 'interfaceDaemon';
COMMIT;

BEGIN
  -- Start the restartStuckRecallsJob
  DBMS_SCHEDULER.CREATE_JOB(
      JOB_NAME        => 'RESTARTSTUCKRECALLSJOB',
      JOB_TYPE        => 'PLSQL_BLOCK',
      JOB_ACTION      => 'BEGIN restartStuckRecalls(); END;',
      JOB_CLASS       => 'CASTOR_JOB_CLASS',
      START_DATE      => SYSDATE + 60/1440,
      REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=60',
      ENABLED         => TRUE,
      COMMENTS        => 'Workaround to restart stuck recalls');
END;
/

COMMIT;

