/******************************************************************************
 *                 castor/db/oracleTapeConstants.sql
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
 * @author Nicola Bessone and Steven Murray
 *****************************************************************************/

/**
 * Package containing the definition of all tape-archieve related PL/SQL 
 * constants.
 */
CREATE OR REPLACE PACKAGE tconst
AS

  -- TPMODE
  WRITE_DISABLE CONSTANT PLS_INTEGER :=  0;
  WRITE_ENABLE  CONSTANT PLS_INTEGER :=  1;

  SEGMENT_UNPROCESSED CONSTANT PLS_INTEGER := 0;
  SEGMENT_FILECOPIED  CONSTANT PLS_INTEGER := 5;
  SEGMENT_FAILED      CONSTANT PLS_INTEGER := 6;
  SEGMENT_SELECTED    CONSTANT PLS_INTEGER := 7;
  SEGMENT_RETRIED     CONSTANT PLS_INTEGER := 8;

  STREAM_PENDING    CONSTANT PLS_INTEGER := 0;
  STREAM_WAITDRIVE  CONSTANT PLS_INTEGER := 1;
  STREAM_WAITMOUNT  CONSTANT PLS_INTEGER := 2;
  STREAM_RUNNING    CONSTANT PLS_INTEGER := 3;
  STREAM_WAITSPACE  CONSTANT PLS_INTEGER := 4;
  STREAM_CREATED    CONSTANT PLS_INTEGER := 5;
  STREAM_STOPPED    CONSTANT PLS_INTEGER := 6;
  STREAM_WAITPOLICY CONSTANT PLS_INTEGER := 7;

  TAPE_UNUSED     CONSTANT PLS_INTEGER := 0;
  TAPE_PENDING    CONSTANT PLS_INTEGER := 1;
  TAPE_WAITDRIVE  CONSTANT PLS_INTEGER := 2;
  TAPE_WAITMOUNT  CONSTANT PLS_INTEGER := 3;
  TAPE_MOUNTED    CONSTANT PLS_INTEGER := 4;
  TAPE_FINISHED   CONSTANT PLS_INTEGER := 5;
  TAPE_FAILED     CONSTANT PLS_INTEGER := 6;
  TAPE_UNKNOWN    CONSTANT PLS_INTEGER := 7;
  TAPE_WAITPOLICY CONSTANT PLS_INTEGER := 8;

  TAPECOPY_CREATED      CONSTANT PLS_INTEGER := 0;
  TAPECOPY_TOBEMIGRATED CONSTANT PLS_INTEGER := 1;
  TAPECOPY_WAITSTREAM   CONSTANT PLS_INTEGER := 2;
  TAPECOPY_SELECTED     CONSTANT PLS_INTEGER := 3;
  TAPECOPY_TOBERECALLED CONSTANT PLS_INTEGER := 4;
  TAPECOPY_STAGED       CONSTANT PLS_INTEGER := 5;
  TAPECOPY_FAILED       CONSTANT PLS_INTEGER := 6;
  TAPECOPY_WAITPOLICY   CONSTANT PLS_INTEGER := 7;
  TAPECOPY_REC_RETRY    CONSTANT PLS_INTEGER := 8;
  TAPECOPY_MIG_RETRY    CONSTANT PLS_INTEGER := 9;

  TG_REQUEST_TO_BE_RESOLVED     CONSTANT PLS_INTEGER := 0;
  TG_REQUEST_TO_BE_SENT_TO_VDQM CONSTANT PLS_INTEGER := 1;
  TG_REQUEST_WAITING_TAPESERVER CONSTANT PLS_INTEGER := 2;
  TG_REQUEST_ONGOING            CONSTANT PLS_INTEGER := 3;

  TAPEREQUEST_PENDING        CONSTANT PLS_INTEGER := 0;
  TAPEREQUEST_BEINGSUBMITTED CONSTANT PLS_INTEGER := 2;
  TAPEREQUEST_SUBMITTED      CONSTANT PLS_INTEGER := 3;
  TAPEREQUEST_FAILED         CONSTANT PLS_INTEGER := 4;

END tconst;
/


/**
 * Package containing the definition of all disk related PL/SQL constants.
 */
CREATE OR REPLACE PACKAGE dconst
AS

  DISKCOPY_STAGED            CONSTANT PLS_INTEGER :=  0;
  DISKCOPY_WAITDISK2DISKCOPY CONSTANT PLS_INTEGER :=  1;
  DISKCOPY_WAITTAPERECALL    CONSTANT PLS_INTEGER :=  2;
  DISKCOPY_DELETED           CONSTANT PLS_INTEGER :=  3;
  DISKCOPY_FAILED            CONSTANT PLS_INTEGER :=  4;
  DISKCOPY_WAITFS            CONSTANT PLS_INTEGER :=  5;
  DISKCOPY_STAGEOUT          CONSTANT PLS_INTEGER :=  6;
  DISKCOPY_INVALID           CONSTANT PLS_INTEGER :=  7;
  DISKCOPY_BEINGDELETED      CONSTANT PLS_INTEGER :=  9;
  DISKCOPY_CANBEMIGR         CONSTANT PLS_INTEGER := 10;
  DISKCOPY_WAITFS_SCHEDULING CONSTANT PLS_INTEGER := 11;

  DISKSERVER_PRODUCTION CONSTANT PLS_INTEGER := 0;
  DISKSERVER_DRAINING   CONSTANT PLS_INTEGER := 1;
  DISKSERVER_DISABLED   CONSTANT PLS_INTEGER := 2;

  FILESYSTEM_PRODUCTION CONSTANT PLS_INTEGER := 0;
  FILESYSTEM_DRAINING   CONSTANT PLS_INTEGER := 1;
  FILESYSTEM_DISABLED   CONSTANT PLS_INTEGER := 2;

  SUBREQUEST_START            CONSTANT PLS_INTEGER :=  0;
  SUBREQUEST_RESTART          CONSTANT PLS_INTEGER :=  1;
  SUBREQUEST_RETRY            CONSTANT PLS_INTEGER :=  2;
  SUBREQUEST_WAITSCHED        CONSTANT PLS_INTEGER :=  3;
  SUBREQUEST_WAITTAPERECALL   CONSTANT PLS_INTEGER :=  4;
  SUBREQUEST_WAITSUBREQ       CONSTANT PLS_INTEGER :=  5;
  SUBREQUEST_READY            CONSTANT PLS_INTEGER :=  6;
  SUBREQUEST_FAILED           CONSTANT PLS_INTEGER :=  7;
  SUBREQUEST_FINISHED         CONSTANT PLS_INTEGER :=  8;
  SUBREQUEST_FAILED_FINISHED  CONSTANT PLS_INTEGER :=  9;
  SUBREQUEST_FAILED_ANSWERING CONSTANT PLS_INTEGER := 10;
  SUBREQUEST_ARCHIVED         CONSTANT PLS_INTEGER := 11;
  SUBREQUEST_REPACK           CONSTANT PLS_INTEGER := 12;
  SUBREQUEST_READUFORSCHED    CONSTANT PLS_INTEGER := 13;
  SUBREQUEST_BEINGSCHED       CONSTANT PLS_INTEGER := 14;

END dconst;
/

