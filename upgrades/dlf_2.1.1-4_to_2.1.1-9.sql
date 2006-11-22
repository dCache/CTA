/*                        ORACLE ENTERPRISE EDITION                         */
/*                                                                          */
/* This file contains SQL code that will upgrade the DLF database schema    */
/* from version 2.1.1-4 to 2.1.1-9                                          */

CREATE INDEX i_msg_timestamp ON dlf_messages (timestamp) LOCAL TABLESPACE dlf_indx;
CREATE INDEX i_msg_pid ON dlf_messages (pid) LOCAL TABLESPACE dlf_indx;

UPDATE dlf_severities SET sev_name = 'Monitoring' WHERE sev_no = 10;
INSERT INTO dlf_severities (sev_no, sev_name) VALUES ('11', 'Debug');

CREATE OR REPLACE PROCEDURE dlf_archive
AS
	-- cursors
	v_cur_partition	INTEGER;
	v_cur_drop	INTEGER;
	v_cur_setting	INTEGER;

	-- variables
	v_partname	USER_TAB_PARTITIONS.PARTITION_NAME%TYPE;
	v_tablename	USER_TABLES.TABLE_NAME%TYPE;
	v_rows		INTEGER;
	v_expire	INTEGER;

BEGIN

	-- set the nls_time_date for this session, the partitioning is sensitive to the
	-- date format reported by sysdate. (alternative: to_char sysdate)
	v_cur_setting := DBMS_SQL.OPEN_CURSOR;
	v_rows := 0;

	DBMS_SQL.PARSE(v_cur_setting,
		'ALTER SESSION
		 SET NLS_DATE_FORMAT = ''DD-MON-YYYY''',
		DBMS_SQL.NATIVE);
	v_rows := DBMS_SQL.EXECUTE(v_cur_setting);

	-- free resources
	DBMS_SQL.CLOSE_CURSOR(v_cur_setting);

	-- variables
	v_cur_partition := DBMS_SQL.OPEN_CURSOR;
	v_rows   := 0;
	v_expire := 30;

	-- determine partitions greater then 'x' days old
	DBMS_SQL.PARSE(v_cur_partition,
		'SELECT TABLE_NAME, PARTITION_NAME
		 FROM USER_TAB_PARTITIONS
		 WHERE PARTITION_NAME <> ''MAX_VALUE''
		 AND PARTITION_NAME < CONCAT(''P_'', TO_CHAR(SYSDATE - '||v_expire||', ''YYYYMMDD''))
		 ORDER BY PARTITION_NAME',
		DBMS_SQL.NATIVE);

	-- define columns, execute statement
	DBMS_SQL.DEFINE_COLUMN(v_cur_partition, 1, v_tablename, 30);
	DBMS_SQL.DEFINE_COLUMN(v_cur_partition, 2, v_partname, 30);
	v_rows := DBMS_SQL.EXECUTE(v_cur_partition);

	-- loop over results
	LOOP
		IF DBMS_SQL.FETCH_ROWS(v_cur_partition) = 0 THEN
			EXIT;
		END IF;
		DBMS_SQL.COLUMN_VALUE(v_cur_partition, 1, v_tablename);
		DBMS_SQL.COLUMN_VALUE(v_cur_partition, 2, v_partname);

		-- drop the partition
		v_cur_drop := DBMS_SQL.OPEN_CURSOR;
		v_rows := 0;

		DBMS_SQL.PARSE(v_cur_drop,
			'ALTER TABLE '||v_tablename||'
			 DROP PARTITION '||v_partname||'',
			DBMS_SQL.NATIVE);
		v_rows := DBMS_SQL.EXECUTE(v_cur_drop);

		-- free resources
		DBMS_SQL.CLOSE_CURSOR(v_cur_drop);

	END LOOP;

	-- free resources
	DBMS_SQL.CLOSE_CURSOR(v_cur_partition);

EXCEPTION
	WHEN OTHERS THEN

		-- close all open cursors
		IF DBMS_SQL.IS_OPEN(v_cur_partition) THEN
			DBMS_SQL.CLOSE_CURSOR(v_cur_partition);
		END IF;
		IF DBMS_SQL.IS_OPEN(v_cur_drop) THEN
			DBMS_SQL.CLOSE_CURSOR(v_cur_drop);
		END IF;
		IF DBMS_SQL.IS_OPEN(v_cur_setting) THEN
			DBMS_SQL.CLOSE_CURSOR(v_cur_setting);
		END IF;
END;


CREATE OR REPLACE 
PROCEDURE dlf_stats_jobs
AS
	-- query to determine statistics on exiting jobs
	CURSOR v_cur_exit IS
	    SELECT p.value, avg(diff) avg, stddev_pop(diff) stddev, min(diff) min,
		   max(diff) max, count(*) count
	    FROM (
		SELECT id, reqid, timestamp, msg_no,
			LEAD(TO_NUMBER(timestamp), 1)
				OVER (PARTITION BY reqid
				ORDER by timestamp, msg_no) next_timestamp,
			LEAD(TO_NUMBER(timestamp), 1)
				OVER (ORDER BY reqid) - TO_NUMBER(timestamp) diff
		FROM (

		-- intersection of request ids
		SELECT * FROM dlf_messages WHERE reqid IN(
		    SELECT reqid FROM dlf_messages
			WHERE timestamp >  TO_DATE(SYSDATE - 15/1440, 'YYYYMMDDHH24MISS')
			AND   timestamp <= TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
			AND   facility = 5
			AND   msg_no   = 15
		      INTERSECT
		    SELECT reqid FROM dlf_messages
			WHERE timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
			AND   facility = 5
			AND   msg_no   = 12)
		AND facility = 5
		AND ((msg_no = 12) OR (msg_no = 15))

		) ORDER BY reqid
	    ) m, dlf_num_param_values p

	    WHERE m.msg_no = 12
	    AND   m.id = p.id
	    AND   p.name = 'type'
	    AND   p.value IN (35, 36, 37, 40)
	    GROUP BY p.value ORDER BY p.value;

	-- query to determine statistics on starting jobs
        CURSOR v_cur_start IS
            SELECT p.value, count(m.reqid) count FROM dlf_messages m, dlf_num_param_values p
            WHERE  m.timestamp >  TO_DATE(SYSDATE - 15/1440, 'YYYYMMDDHH24MISS')
            AND    m.timestamp <= TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
            AND    m.facility = 5
            AND    m.msg_no   = 12
            AND    m.id       = p.id
            AND    p.name     = 'type'
            GROUP BY p.value;
            
	-- variables
	v_record_exit   v_cur_exit%ROWTYPE;
        v_record_start  v_cur_start%ROWTYPE;
	v_time	        DATE;

BEGIN

	-- set the nls_date_format
	EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "YYYYMMDDHH24MISS"';

	-- we define the time as a variable so that all data recorded will be synchronised
 	v_time := TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS');

	FOR i IN 35..40 LOOP
		IF ((i = 35)  OR
		    (i = 36)  OR
		    (i = 37)  OR
		    (i = 40)) THEN
			INSERT INTO dlf_jobstats VALUES(v_time, i, 0, 0, 0, 0, 0, 0, 300);
		END IF;
	END LOOP;

	-- update the exiting statistics data
	OPEN v_cur_exit;
	LOOP
		FETCH v_cur_exit INTO v_record_exit;
			EXIT WHEN v_cur_exit%NOTFOUND;

			UPDATE dlf_jobstats SET
				min_time = v_record_exit.min,
				max_time = v_record_exit.max,
				avg_time = v_record_exit.avg,
				std_dev  = v_record_exit.stddev,
				exiting  = v_record_exit.count
			WHERE timestamp = v_time
			AND   type      = v_record_exit.value;
	END LOOP;

	CLOSE v_cur_exit;
        
        -- update the start statistics data
        OPEN v_cur_start;
        LOOP
		FETCH v_cur_start INTO v_record_start;
			EXIT WHEN v_cur_start%NOTFOUND;
			IF ((v_record_start.value = 35)  OR
			    (v_record_start.value = 36)  OR 
			    (v_record_start.value = 37)  OR
			    (v_record_start.value = 40)) THEN        
				UPDATE dlf_jobstats SET
					starting = v_record_start.count
				WHERE timestamp = v_time
				AND   type      = v_record_start.value;
			END IF;
	END LOOP;
        
	COMMIT;

EXCEPTION
	WHEN OTHERS THEN
		CLOSE v_cur_exit;
                CLOSE v_cur_start;
		ROLLBACK;
END;


/*
 * dlf_stats_requests (Populates table: dlf_reqstats)
 */
CREATE OR REPLACE 
PROCEDURE dlf_stats_requests
AS
	-- query to determine statistics on exiting requests
	CURSOR v_cur_exit IS
	    SELECT type, avg(proctime) avg, stddev_pop(proctime) stddev, min(proctime) min,
		   max(proctime) max, count(*) count
	    FROM (

		SELECT m.msg_no, (p.value * 0.001) proctime,
			LEAD (p.value, 1) 
			OVER (PARTITION BY m.reqid ORDER BY m.timestamp, m.msg_no) type
		FROM (

		    -- intersection of request ids
		    SELECT * FROM dlf_messages WHERE reqid IN (
		        SELECT reqid FROM dlf_messages
			    WHERE timestamp >  TO_DATE(SYSDATE - 15/1440, 'YYYYMMDDHH24MISS')
			    AND   timestamp <= TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS')
			    AND   facility = 4
			    AND   msg_no   = 10
		          INTERSECT
		        SELECT reqid FROM dlf_messages
			    WHERE timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
			    AND   facility = 4
			    AND   msg_no   = 12
		    )
		    AND facility = 4
		    AND ((msg_no = 10) OR (msg_no = 12))		    

		) m, dlf_num_param_values p
		WHERE p.timestamp > TO_DATE(SYSDATE - 2, 'YYYYMMDDHH24MISS')
		AND   m.id = p.id
		AND   p.name IN ('Type', 'ProcTime')

	    )
	    WHERE msg_no = 10
	    AND   type IN (35, 36, 37, 40)
	    GROUP BY type ORDER BY type;
            
	-- variables
	v_record_exit   v_cur_exit%ROWTYPE;
	v_time	        DATE;

BEGIN

	-- set the nls_date_format
	EXECUTE IMMEDIATE 'ALTER SESSION SET NLS_DATE_FORMAT = "YYYYMMDDHH24MISS"';

	-- we define the time as a variable so that all data recorded will be synchronised
 	v_time := TO_DATE(SYSDATE - 10/1440, 'YYYYMMDDHH24MISS');

	FOR i IN 35..40 LOOP
		IF ((i = 35)  OR
		    (i = 36)  OR
		    (i = 37)  OR
		    (i = 40)) THEN
			INSERT INTO dlf_reqstats VALUES(v_time, i, 0, 0, 0, 0, 0, 300);
		END IF;
	END LOOP;

	-- update the statistics data
	OPEN v_cur_exit;
	LOOP
		FETCH v_cur_exit INTO v_record_exit;
			EXIT WHEN v_cur_exit%NOTFOUND;

			UPDATE dlf_reqstats SET
				min_time  = v_record_exit.min,
				max_time  = v_record_exit.max,
				avg_time  = v_record_exit.avg,
				std_dev   = v_record_exit.stddev,
				req_count = v_record_exit.count
			WHERE timestamp = v_time
			AND   type      = v_record_exit.type;
	END LOOP;
        
	COMMIT;

EXCEPTION
	WHEN OTHERS THEN
		CLOSE v_cur_exit;
		ROLLBACK;
END;


/*
 * statistics scheduler
 */
BEGIN
DBMS_SCHEDULER.CREATE_JOB (
	JOB_NAME 	=> 'DLF_STATS_5MINS',
	JOB_TYPE 	=> 'PLSQL_BLOCK',
	JOB_ACTION 	=> 'BEGIN
				DLF_STATS_JOBS();
				DLF_STATS_REQUESTS();
			   END;',
	START_DATE 	=> SYSDATE,
	REPEAT_INTERVAL => 'FREQ=MINUTELY; INTERVAL=5',
	ENABLED 	=> TRUE,
	COMMENTS	=> 'CASTOR2 Monitoring Statistics (5 Minute Frequency)');
END;


/** End-of-File **/