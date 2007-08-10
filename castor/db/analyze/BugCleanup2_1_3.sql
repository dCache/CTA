-- For logging
--DROP TABLE CleanupLogTable;
CREATE TABLE CleanupLogTable (fac NUMBER, message VARCHAR2(256), logDate NUMBER);

-- Cleanup of old subrequests (code from deleteOutOfDateRequests)

CREATE TABLE REQCLEANING
    ( ID NUMBER NOT NULL ENABLE,
      TYPE NUMBER(*,0) NOT NULL ENABLE
    )
 PCTFREE 0 TABLESPACE STAGER_DATA
 PARTITION BY LIST (TYPE)
  (PARTITION P_35  VALUES (35),
   PARTITION P_40  VALUES (40),
   PARTITION P_42  VALUES (42),
   PARTITION P_39  VALUES (39),
   PARTITION P_36  VALUES (36),
   PARTITION P_37  VALUES (37),
   PARTITION P_NOTDEFINED  VALUES (default)
  );

-- A little method to delete efficiently
CREATE OR REPLACE PROCEDURE efficientDelete
  (sel IN VARCHAR2, tab IN VARCHAR2) IS
BEGIN
  EXECUTE IMMEDIATE
  'DECLARE
    cursor s IS '||sel||'
    ids "numList";
  BEGIN
    OPEN s;
    LOOP
      FETCH s BULK COLLECT INTO ids LIMIT 10000;
      DELETE FROM '||tab||' WHERE id in (SELECT * FROM TABLE(ids));
      COMMIT;
      EXIT WHEN s%NOTFOUND;
    END LOOP;
  END;';
END;

-- A little method to delete efficiently Requests
CREATE OR REPLACE PROCEDURE deleteRequestEfficiently
  (tab IN VARCHAR2, typ IN VARCHAR2) IS
BEGIN
  -- delete client id2type
  efficientDelete(
    'SELECT client FROM '||tab||', ReqCleaning
       WHERE '||tab||'.id = ReqCleaning.id;',
    'Id2Type');
  -- delete client itself
  efficientDelete(
    'SELECT client FROM '||tab||', ReqCleaning
       WHERE '||tab||'.id = ReqCleaning.id;',
    'Client');
  -- delete request id2type
  efficientDelete(
    'SELECT id FROM ReqCleaning WHERE type = '||typ||';',
    'Id2Type');
  -- delete request itself
  efficientDelete(
    'SELECT id FROM ReqCleaning WHERE type = '||typ||';',
    tab);
END;

-- Using the APPEND hint to use Direct Option load mechanism
DECLARE
  totalCount NUMBER;
BEGIN
  -- For logging
  INSERT INTO CleanupLogTable VALUES (13, 'Cleaning all old subrequests', getTime());
  COMMIT;
  INSERT /*+ APPEND */ INTO ReqCleaning
    SELECT UNIQUE request, type 
    FROM SubRequest, id2type
    WHERE subrequest.request = id2type.id
    GROUP BY request, type
    HAVING min(status) >= 8
       AND max(status) <= 11
       AND max(lastModificationTime) < getTime() - 43200;
  COMMIT;
  SELECT count(*) INTO totalCount FROM ReqCleaning;
  -- Delete SubRequests
    -- Starting with id
  efficientDelete(
     'SELECT SubRequest.id FROM SubRequest, ReqCleaning
         WHERE SubRequest.request = ReqCleaning.id;',
     'Id2Type');
    -- Then the subRequests
  efficientDelete('
      SELECT SubRequest.id FROM SubRequest, ReqCleaning
       WHERE SubRequest.request = ReqCleaning.id;',
     'SubRequest');
  -- Delete Request + Clients 
    ---- Get ----
  deleteRequestEfficiently('StageGetRequest', '35');
    ---- Put ----
  deleteRequestEfficiently('StagePutRequest', '40');
    ---- Update ----
  deleteRequestEfficiently('StageUpdateRequest', '44');
    ---- Rm ----
  deleteRequestEfficiently('StageRmRequest', '42');
    ---- PutDone ----
  deleteRequestEfficiently('StagePutDoneRequest', '39');
    ---- PrepareToGet -----
  deleteRequestEfficiently('StagePrepareToGetRequest', '36');
    ---- PrepareToPut ----
  deleteRequestEfficiently('StagePrepareToPutRequest', '37');
  -- Last step is to commit the operations
  UPDATE CleanupLogTable
     SET message = 'Old subrequests were cleaned - ' || TO_CHAR(totalCount) || ' entries', logDate = getTime()
   WHERE fac = 13;
  COMMIT;
END;

DROP TABLE ReqCleaning;


-- cleaning up files stuck with multiple concurrent recalls
-- This can happen due to a race condition in castor < 2.1.4-0
CREATE OR REPLACE PROCEDURE stageRmForConcurrentRecalls (cfid IN INTEGER) AS
  segId INTEGER;
  unusedIds "numList";
BEGIN
  -- First lock all segments for the file
  SELECT segment.id BULK COLLECT INTO unusedIds
    FROM Segment, TapeCopy
   WHERE TapeCopy.castorfile = cfId
     AND TapeCopy.id = Segment.copy
  FOR UPDATE;
  -- Check whether we have any segment in SELECTED
  SELECT segment.id INTO segId
    FROM Segment, TapeCopy
   WHERE TapeCopy.castorfile = cfId
     AND TapeCopy.id = Segment.copy
     AND Segment.status = 7 -- SELECTED
     AND ROWNUM < 2;
  -- Something is running, so give up
EXCEPTION WHEN NO_DATA_FOUND THEN
  -- Nothing running
  FOR t IN (SELECT id FROM TapeCopy WHERE castorfile = cfId) LOOP
    FOR s IN (SELECT id FROM Segment WHERE copy = t.id) LOOP
      -- Delete the segment(s)
      DELETE FROM Id2Type WHERE id = s.id;
      DELETE FROM Segment WHERE id = s.id;
    END LOOP;
    -- Delete the TapeCopy
    DELETE FROM Id2Type WHERE id = t.id;
    DELETE FROM TapeCopy WHERE id = t.id;
  END LOOP;
  -- Delete the DiskCopies
  UPDATE DiskCopy
     SET status = 7  -- INVALID
   WHERE status = 2  -- WAITTAPERECALL
     AND castorFile = cfId;
  -- Mark the 'recall' SubRequests as failed
  -- so that clients get an answer
  UPDATE SubRequest SET status = 7   -- FAILED
   WHERE castorFile = cfId and status = 4;
END;
DECLARE
  totalCount NUMBER := 0;
BEGIN
  -- For logging
  INSERT INTO CleanupLogTable VALUES (14, 'Cleanup files stuck with multiple concurrent recalls', getTime());
  COMMIT;
  FOR cf IN (SELECT castorFile.id FROM
              (SELECT castorfile cfid, count(*) AS c FROM diskcopy
                WHERE status = 2 GROUP BY castorfile), CastorFile
             WHERE c > 1 AND cfid = castorfile.id) LOOP
    stageRmForConcurrentRecalls(cf.id);
    totalCount := totalCount + 1;
  END LOOP;
  UPDATE CleanupLogTable
     SET message = 'Cleanup files stuck with multiple concurrent recalls were cleaned - ' || TO_CHAR(totalCount) || ' entries', logDate = getTime()
   WHERE fac = 14;
  COMMIT;
END;
DROP PROCEDURE stageRmForConcurrentRecalls;

-- Optional steps follow

INSERT INTO CleanupLogTable VALUES (15, 'Shrinking tables', getTime());
COMMIT;

-- Shrinking space in the tables that have just been cleaned up.
-- This only works if tables have ROW MOVEMENT enabled, and it's
-- not a big problem if we don't do that. Newer Castor versions
-- do enable this feature in all involved tables.
ALTER TABLE Id2Type SHRINK SPACE CASCADE;
ALTER TABLE Client SHRINK SPACE CASCADE;
ALTER TABLE StagePrepareToPutRequest SHRINK SPACE CASCADE;
ALTER TABLE StagePrepareToGetRequest SHRINK SPACE CASCADE;
ALTER TABLE StagePutDoneRequest SHRINK SPACE CASCADE;
ALTER TABLE StageRmRequest SHRINK SPACE CASCADE;
ALTER TABLE StageGetRequest SHRINK SPACE CASCADE;
ALTER TABLE StagePutRequest SHRINK SPACE CASCADE;
ALTER TABLE DiskCopy SHRINK SPACE CASCADE;
ALTER TABLE SubRequest SHRINK SPACE CASCADE;

UPDATE CleanupLogTable
   SET message = 'All tables were shrunk', logDate = getTime()
 WHERE fac = 15;
COMMIT;

-- To provide a summary of the performed cleanup
SELECT * FROM CleanupLogTable;
