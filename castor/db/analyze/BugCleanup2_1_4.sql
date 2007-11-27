-- For logging
--DROP TABLE CleanupLogTable;
CREATE TABLE CleanupLogTable (fac NUMBER, message VARCHAR2(256), logDate NUMBER);

-- Cleanup of subrequests stuck in status WAITSUBREQ and
-- which are waiting on failed requests due to bug in the
-- handling of manual job kills
DECLARE
  totalCount NUMBER;
  done NUMBER := 0;
BEGIN
 INSERT INTO CleanupLogTable VALUES (8, 'Cleaning SubRequests stuck in WAITSUBREQ 0% done', getTime());
 COMMIT;
 SELECT count(s1.id) INTO totalCount
   FROM SubRequest s1, SubRequest s2
  WHERE s1.status = 5
    AND s1.parent = s2.id
    AND s2.status = 9;
 -- fail subrequests
 UPDATE SubRequest SET status = 7 -- FAILED
  WHERE id IN (SELECT s1.id FROM SubRequest s1, SubRequest s2
                WHERE s1.status = 5 AND s1.parent = s2.id
                  AND s2.status = 9);
 COMMIT;
 UPDATE CleanupLogTable
    SET message = 'SubRequests stuck in WAITSUBREQ were cleaned - ' || TO_CHAR(totalCount) || ' entries', logDate = getTime()
  WHERE fac = 8;
 COMMIT;
END;

-- Cleanup of subrequests stuck in status WAITSUBREQ and
-- which are waiting on nothing (null parent). This is due
-- to bug #30392
DECLARE
  nothingLeft NUMBER;
  totalCount NUMBER;
  done NUMBER := 0;
BEGIN
 INSERT INTO CleanupLogTable VALUES (9, 'Cleaning SubRequests stuck in WAITSUBREQ (#30392) 0% done', getTime());
 COMMIT;
 SELECT count(UNIQUE CastorFile) INTO totalCount FROM SubRequest
  WHERE status = 5 AND parent IS NULL;
 LOOP
   nothingLeft := 1;
   -- do it 50 by 50
   FOR cf IN (SELECT UNIQUE CastorFile FROM SubRequest
               WHERE status = 5 AND parent IS NULL AND ROWNUM < 50) LOOP
     -- drop bad diskcopy
     UPDATE diskcopy SET status = 7 WHERE status = 2 AND castorfile = cf.CastorFile;
     -- restart SubRequests waiting
     UPDATE SubRequest SET status = 1 WHERE status = 5 AND parent is NULL AND castorfile = cf.CastorFile;
     nothingLeft := 0;
   END LOOP;
   -- commit between each bunch of 50 and update monitoring
   done := done + 50;
   UPDATE CleanupLogTable
      SET message = 'Cleaning SubRequests stuck in WAITSUBREQ' ||
          TO_CHAR(100*done/(totalCount+1), '999.99') || '% done', logDate = getTime()
    WHERE fac = 9;
   COMMIT;
   IF nothingLeft = 1 THEN
     UPDATE CleanupLogTable
        SET message = 'SubRequests stuck in WAITSUBREQ were cleaned - ' || TO_CHAR(totalCount) || ' entries', logDate = getTime()
      WHERE fac = 9;
     COMMIT;
     EXIT;
   END IF;
 END LOOP;
END;

-- Cleanup for diskcopies in STAGIN without associated TAPECOPY. 

DECLARE 
    cfIds "numList";
    sIds "numList";
    totalIds NUMBER;
BEGIN
	INSERT INTO CleanupLogTable VALUES (10, 'Cleaning STAGIN diskcopy without tapecopy 0 done', getTime());
	COMMIT; 
        -- info for log   
	SELECT count(*) INTO totalIds FROM diskcopy WHERE castorfile NOT IN ( SELECT castorfile FROM tapecopy) AND status=2;  
	-- diskcopy as invalid
	UPDATE diskcopy SET status=7 WHERE castorfile NOT IN ( SELECT castorfile FROM tapecopy) AND status=2 RETURNING castorfile BULK COLLECT INTO cfIds;
	-- restart the subrequests
	UPDATE subrequest SET status=0 WHERE status IN (4,5) AND castorfile MEMBER OF cfIds;  
        COMMIT;
        -- log 
        UPDATE CleanupLogTable SET message = 'STAGIN diskcopy without tapecopy: cleaned - ' || TO_CHAR(totalIds) || ' entries', logDate = getTime() WHERE fac = 10;
	COMMIT;
END;


-- Cleanup for orphan segment.
DECLARE
	sIds "numList";
 	totalIds NUMBER;
BEGIN
	INSERT INTO CleanupLogTable VALUES (11, 'Cleaning segments without tapecopy 0 done', getTime());
	COMMIT;
        -- info for log
	SELECT count(*) INTO totalIds FROM segment WHERE status IN (6, 8) AND copy NOT IN( SELECT id FROM tapecopy);

	-- delete segments
	DELETE FROM segment WHERE status IN (6, 8) AND copy NOT IN (SELECT id FROM tapecopy) RETURNING id BULK COLLECT INTO sIds;
	DELETE FROM Id2type WHERE id MEMBER OF sIds;
	COMMIT;
        -- log
        UPDATE CleanupLogTable SET message = 'Segments without tapecopy: cleaned - ' || TO_CHAR(totalIds) || ' entries', logDate = getTime() WHERE fac = 11;
	COMMIT;
END; 

-- Optional steps follow
INSERT INTO CleanupLogTable VALUES (16, 'Shrinking tables', getTime());
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
ALTER TABLE SubRequest SHRINK SPACE CASCADE;
ALTER TABLE Segment SHRINK SPACE CASCADE;
ALTER TABLE id2type SHRINK SPACE CASCADE;

UPDATE CleanupLogTable
   SET message = 'All tables were shrunk', logDate = getTime()
 WHERE fac = 16;
COMMIT;

-- To provide a summary of the performed cleanup
SELECT * FROM CleanupLogTable;
