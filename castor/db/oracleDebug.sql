/*******************************************************************
 *
 * @(#)$RCSfile: oracleDebug.sql,v $ $Revision: 1.8 $ $Date: 2008/07/02 15:20:07 $ $Author: sponcec3 $
 *
 * Some SQL code to ease support and debugging
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

CREATE OR REPLACE PACKAGE castor_debug AS
  TYPE DiskCopyDebug_typ IS RECORD (
    id INTEGER,
    diskPool VARCHAR2(2048),
    location VARCHAR2(2048),
    status NUMBER,
    creationtime DATE);
  TYPE DiskCopyDebug IS TABLE OF DiskCopyDebug_typ;
  TYPE SubRequestDebug IS TABLE OF SubRequest%ROWTYPE;
  TYPE RequestDebug_typ IS RECORD (
    creationtime DATE,
    SubReqId NUMBER,
    SubReqParentId NUMBER,
    Status NUMBER,
    username VARCHAR2(2048),
    machine VARCHAR2(2048),
    svcClassName VARCHAR2(2048),
    ReqId NUMBER,
    ReqType VARCHAR2(20));
  TYPE RequestDebug IS TABLE OF RequestDebug_typ;
  TYPE TapeCopyDebug_typ IS RECORD (
    TCId NUMBER,
    TCStatus NUMBER,
    SegId NUMBER,
    SegStatus NUMBER,
    SegErrCode NUMBER,
    VID VARCHAR2(2048),
    tpMode NUMBER,
    TapeStatus NUMBER,
    nbStreams NUMBER,
    SegErr VARCHAR2(2048));
  TYPE TapeCopyDebug IS TABLE OF TapeCopyDebug_typ;
END;


/* Return the castor file id associated with the reference number */
CREATE OR REPLACE FUNCTION getCF(ref NUMBER) RETURN NUMBER AS
  t NUMBER;
  cfId NUMBER;
BEGIN
  SELECT type INTO t FROM id2Type WHERE id = ref;
  IF (t = 2) THEN -- CASTORFILE
    RETURN ref;
  ELSIF (t = 5) THEN -- DiskCopy
    SELECT castorFile INTO cfId FROM DiskCopy WHERE id = ref;
  ELSIF (t = 27) THEN -- SubRequest
    SELECT castorFile INTO cfId FROM SubRequest WHERE id = ref;
  ELSIF (t = 30) THEN -- TapeCopy
    SELECT castorFile INTO cfId FROM TapeCopy WHERE id = ref;
  ELSIF (t = 18) THEN -- Segment
    SELECT castorFile INTO cfId FROM TapeCopy, Segment WHERE Segment.id = ref AND TapeCopy.id = Segment.copy;
  END IF;
  RETURN cfId;
EXCEPTION WHEN NO_DATA_FOUND THEN -- fileid ?
  SELECT id INTO cfId FROM CastorFile WHERE fileId = ref;
  RETURN cfId;
END;


/* Get the diskcopys associated with the reference number */
CREATE OR REPLACE FUNCTION getDCs(ref number) RETURN castor_debug.DiskCopyDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT diskCopy.id,
                   diskPool.name AS diskpool,
                   diskServer.name || ':' || fileSystem.mountPoint || diskCopy.path AS location,
                   diskCopy.status AS status,
                   to_date('01011970','ddmmyyyy') + 1/24/60/60 * creationtime AS creationtime
              FROM DiskCopy, FileSystem, DiskServer, DiskPool
             WHERE DiskCopy.fileSystem = FileSystem.id(+)
               AND FileSystem.diskServer = diskServer.id(+)
               AND DiskPool.id(+) = fileSystem.diskPool
               AND DiskCopy.castorfile = getCF(ref)) LOOP
     PIPE ROW(d);
  END LOOP;
END;


/* Get the tapecopys, segments and streams associated with the reference number */
CREATE OR REPLACE FUNCTION getTCs(ref number) RETURN castor_debug.TapeCopyDebug PIPELINED AS
BEGIN
  FOR t IN (SELECT TapeCopy.id AS TCId, TapeCopy.status AS TCStatus,
                   Segment.Id, Segment.status AS SegStatus, Segment.errorCode AS SegErrCode,
                   Tape.vid AS VID, Tape.tpMode AS tpMode, Tape.Status AS TapeStatus,
                   CASE WHEN Stream2TapeCopy.child IS NULL THEN 0 ELSE count(*) END AS nbStreams,
                   Segment.errMsgTxt AS SegErr
              FROM TapeCopy, Segment, Tape, Stream2TapeCopy
             WHERE TapeCopy.id = Segment.copy(+)
               AND Segment.tape = Tape.id(+)
               AND TapeCopy.castorfile = getCF(ref)
               AND Stream2TapeCopy.child(+) = TapeCopy.id
              GROUP BY TapeCopy.id, TapeCopy.status, Segment.id, Segment.status, Segment.errorCode,
                       Tape.vid, Tape.tpMode, Tape.Status, Segment.errMsgTxt, Stream2TapeCopy.child) LOOP
     PIPE ROW(t);
  END LOOP;
END;


/* Get the subrequests associated with the reference number. (By castorfile/diskcopy/
 * subrequest/tapecopy or fileid
 */
CREATE OR REPLACE FUNCTION getSRs(ref number) RETURN castor_debug.SubRequestDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT * FROM SubRequest WHERE castorfile = getCF(ref)) LOOP
     PIPE ROW(d);
  END LOOP;
END;


/* Get the requests associated with the reference number. (By castorfile/diskcopy/
 * subrequest/tapecopy or fileid
 */
CREATE OR REPLACE FUNCTION getRs(ref number) RETURN castor_debug.RequestDebug PIPELINED AS
BEGIN
  FOR d IN (SELECT to_date('01011970','ddmmyyyy') + 1/24/60/60 * creationtime AS creationtime,
                   SubRequest.id AS SubReqId, SubRequest.parent AS SubReqParentId, SubRequest.Status,
                   username, machine, svcClassName, Request.id AS ReqId, Request.type AS ReqType
              FROM SubRequest,
                    (SELECT id, username, machine, svcClassName, 'Get' AS type FROM StageGetRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PGet' AS type FROM StagePrepareToGetRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'Put' AS type FROM StagePutRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PPut' AS type FROM StagePrepareToPutRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'Upd' AS type FROM StageUpdateRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PUpd' AS type FROM StagePrepareToUpdateRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'Repack' AS type FROM StageRepackRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'GetNext' AS type FROM StageGetNextRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'UpdNext' AS type FROM StageUpdateNextRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'PutDone' AS type FROM StagePutDoneRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'DCRepl' AS type FROM StageDiskCopyReplicaRequest UNION ALL
                     SELECT id, username, machine, svcClassName, 'SetFileGCWeight' AS type FROM SetFileGCWeight) Request
             WHERE castorfile = getCF(ref)
               AND Request.id = SubRequest.request) LOOP
     PIPE ROW(d);
  END LOOP;
END;
