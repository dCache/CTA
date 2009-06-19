/*******************************************************************
 * @(#)$RCSfile: oracleMonitoring.schema.sql,v $ $Revision: 1.2 $ $Date: 2009/06/19 12:27:14 $ $Author: waldron $
 * Schema creation code for Monitoring tables
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *******************************************************************/

/* SQL statement for table MonDiskCopyStats */
CREATE TABLE MonDiskCopyStats
  (timestamp DATE, interval NUMBER, diskServer VARCHAR2(255), mountPoint VARCHAR2(255), dsStatus VARCHAR2(20), fsStatus VARCHAR2(20), available VARCHAR2(2), status VARCHAR2(100), totalSize NUMBER, nbFiles NUMBER);

/* SQL statement for table MonWaitTapeMigrationStats */
CREATE TABLE MonWaitTapeMigrationStats
  (timestamp DATE, interval NUMBER, svcClass VARCHAR2(255), status VARCHAR2(10), minFileAge NUMBER, maxFileAge NUMBER, avgFileAge NUMBER, minFileSize NUMBER, maxFileSize NUMBER, avgFileSize NUMBER, bin_LT_1 NUMBER, bin_1_To_6 NUMBER, bin_6_To_12 NUMBER, bin_12_To_24 NUMBER, bin_24_To_48 NUMBER, bin_GT_48 NUMBER, totalSize NUMBER, nbFiles NUMBER);

/* SQL statement for table MonWaitTapeRecallStats  */
CREATE TABLE MonWaitTapeRecallStats
  (timestamp DATE, interval NUMBER, svcClass VARCHAR2(255), minFileAge NUMBER, maxFileAge NUMBER, avgFileAge NUMBER, minFileSize NUMBER, maxFileSize NUMBER, avgFileSize NUMBER, bin_LT_1 NUMBER, bin_1_To_6 NUMBER, bin_6_To_12 NUMBER, bin_12_To_24 NUMBER, bin_24_To_48 NUMBER, bin_GT_48 NUMBER, totalSize NUMBER, nbFiles NUMBER);

/* SQL statement for table ObjStatus (To be generated by gencastor!!) */
CREATE TABLE ObjStatus (object VARCHAR2(100), statusCode NUMBER, statusName VARCHAR2(100) CONSTRAINT NN_ObjStatus_statusName NOT NULL, CONSTRAINT UN_ObjStatus_ObjectCode UNIQUE (object, statusCode));

/* Fill ObjStatus table */
INSERT INTO ObjStatus VALUES ('DiskCopy', 0,  'STAGED');
INSERT INTO ObjStatus VALUES ('DiskCopy', 1,  'WAITDISK2DISKCOPY');
INSERT INTO ObjStatus VALUES ('DiskCopy', 2,  'WAITTAPERECALL');
INSERT INTO ObjStatus VALUES ('DiskCopy', 3,  'DELETED');
INSERT INTO ObjStatus VALUES ('DiskCopy', 4,  'FAILED');
INSERT INTO ObjStatus VALUES ('DiskCopy', 5,  'WAITFS');
INSERT INTO ObjStatus VALUES ('DiskCopy', 6,  'STAGEOUT');
INSERT INTO ObjStatus VALUES ('DiskCopy', 7,  'INVALID');
INSERT INTO ObjStatus VALUES ('DiskCopy', 9,  'BEINGDELETED');
INSERT INTO ObjStatus VALUES ('DiskCopy', 10, 'CANBEMIGR');
INSERT INTO ObjStatus VALUES ('DiskCopy', 11, 'WAITFS_SCHEDULING');

INSERT INTO ObjStatus VALUES ('DiskServer', 0, 'PRODUCTION');
INSERT INTO ObjStatus VALUES ('DiskServer', 1, 'DRAINING');
INSERT INTO ObjStatus VALUES ('DiskServer', 2, 'DISABLED');

INSERT INTO ObjStatus VALUES ('FileSystem', 0, 'PRODUCTION');
INSERT INTO ObjStatus VALUES ('FileSystem', 1, 'DRAINING');
INSERT INTO ObjStatus VALUES ('FileSystem', 2, 'DISABLED');