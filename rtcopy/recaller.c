/******************************************************************************
 *                      recaller.c
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
 * @(#)$RCSfile: recaller.c,v $ $Revision: 1.2 $ $Release$ $Date: 2004/11/15 07:37:33 $ $Author: obarring $
 *
 * 
 *
 * @author Olof Barring
 *****************************************************************************/


#ifndef lint
static char sccsid[] = "@(#)$RCSfile: recaller.c,v $ $Revision: 1.2 $ $Release$ $Date: 2004/11/15 07:37:33 $ Olof Barring";
#endif /* not lint */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <winsock2.h>
extern char *geterr();
WSADATA wsadata;
#else /* _WIN32 */
#include <sys/types.h>                  /* Standard data types          */
#include <netdb.h>                      /* Network "data base"          */
#include <sys/socket.h>                 /* Socket interface             */
#include <netinet/in.h>                 /* Internet data types          */
#include <signal.h>
#include <sys/time.h>
#endif /* _WIN32 */
#include <sys/stat.h>
#include <errno.h>
#include <patchlevel.h>
#include <Castor_limits.h>
#include <log.h>
#include <net.h>
#include <osdep.h>
#include <trace.h>
#include <serrno.h>
#include <common.h>
#include <Cgetopt.h>
#include <Cinit.h>
#include <Cuuid.h>
#include <Cpwd.h>
#include <Cnetdb.h>
#include <Cpool_api.h>
#include <dlf_api.h>
#include <rtcp_constants.h>
#include <vdqm_api.h>
#include <vmgr_api.h>
#include <castor/stager/TapeStatusCodes.h>
#include <castor/stager/SegmentStatusCodes.h>
#include <rtcp.h>
#include <rtcp_server.h>
#include <rtcp_api.h>
#include <rtcpcld_constants.h>
#include <rtcpcld.h>
#include <rtcpcld_messages.h>
#include "castor/Constants.h"
extern int rtcpc_runReq_ext _PROTO((
                                    rtcpHdr_t *,
                                    rtcpc_sockets_t **,
                                    tape_list_t *,
                                    int (*)(void *(*)(void *), void *)
                                    ));
extern int (*rtcpc_ClientCallback) _PROTO((
                                           rtcpTapeRequest_t *, 
                                           rtcpFileRequest_t *
                                           ));

Cuuid_t childUuid, mainUuid;
static tape_list_t *tape = NULL;
static void *segmCountLock = NULL;
static int segmSubmitted = 0, segmCompleted = 0, segmFailed = 0;
static void *abortLock = NULL;
static int requestAborted = 0;
static int segmentFailed = 0;

int inChild = 1;

static int initLocks() 
{
  int rc;
  /*
   * Segment processing count lock
   */
  rc = Cthread_mutex_lock(&segmSubmitted);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock()");
    return(-1);
  }
  rc = Cthread_mutex_unlock(&segmSubmitted);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_unlock()");
    return(-1);
  }  
  segmCountLock = Cthread_mutex_lock_addr(&segmSubmitted);
  if ( segmCountLock == NULL ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock_addr()");
    return(-1);
  }

  /*
   * Abort lock
   */
  rc = Cthread_mutex_lock(&requestAborted);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock()");
    return(-1);
  }
  rc = Cthread_mutex_unlock(&requestAborted);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_unlock()");
    return(-1);
  }
  
  abortLock = Cthread_mutex_lock_addr(&requestAborted);
  if ( segmCountLock == NULL ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock_addr()");
    return(-1);
  }
  
  return(0);
}

static int updateSegmCount(
                           submitted,
                           completed,
                           failed
                           )
     int submitted,completed, failed;
{
  int rc;
  rc = Cthread_mutex_lock_ext(segmCountLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock_ext()");
    return(-1);
  }
  segmSubmitted += submitted;
  segmCompleted += completed;
  segmFailed += failed;
  (void)dlf_write(
                  childUuid,
                  DLF_LVL_DEBUG,
                  RTCPCLD_MSG_SEGMCNTS,
                  (struct Cns_fileid *)NULL,
                  RTCPCLD_NB_PARAMS+3,
                  "SUBM",
                  DLF_MSG_PARAM_INT,
                  segmSubmitted,
                  "COMPL",
                  DLF_MSG_PARAM_INT,
                  segmCompleted,
                  "FAILED",
                  DLF_MSG_PARAM_INT,
                  segmFailed,
                  RTCPCLD_LOG_WHERE
                  );
  rc = Cthread_mutex_unlock_ext(segmCountLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_unlock_ext()");
    return(-1);
  }
  return(0);
}

static int nbRunningSegms() 
{
  int rc, nbRunning;
  rc = Cthread_mutex_lock_ext(segmCountLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock_ext()");
    return(-1);
  }
  if ( segmFailed > 0 ) nbRunning = 0;
  else nbRunning = segmSubmitted - segmCompleted;
  (void)dlf_write(
                  childUuid,
                  DLF_LVL_DEBUG,
                  RTCPCLD_MSG_SEGMCNTS,
                  (struct Cns_fileid *)NULL,
                  RTCPCLD_NB_PARAMS+3,
                  "SUBM",
                  DLF_MSG_PARAM_INT,
                  segmSubmitted,
                  "COMPL",
                  DLF_MSG_PARAM_INT,
                  segmCompleted,
                  "FAILED",
                  DLF_MSG_PARAM_INT,
                  segmFailed,
                  RTCPCLD_LOG_WHERE
                  );
  rc = Cthread_mutex_unlock_ext(segmCountLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_unlock_ext()");
    return(-1);
  }
  return(nbRunning);
}

static int checkAborted(
                        segmFailed
                        )
     int *segmFailed;
{
  int aborted = 0, rc;
  rc = Cthread_mutex_lock_ext(abortLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock_ext()");
    return(-1);
  }
  aborted = requestAborted;
  if ( segmFailed != NULL ) *segmFailed = segmentFailed;

  rc = Cthread_mutex_unlock_ext(abortLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_unlock_ext()");
    return(-1);
  }
  return(aborted);
}

static void setAborted(
                       segmFailed
                       )
     int segmFailed;
{
  int rc;
  rc = Cthread_mutex_lock_ext(abortLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_lock_ext()");
  }
  requestAborted = 1;
  if ( segmFailed > segmentFailed ) segmentFailed = segmFailed;

  rc = Cthread_mutex_unlock_ext(abortLock);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("Cthread_mutex_unlock_ext()");
  }
  return;
}


int recallerCallbackFileCopied(
                               tapereq,
                               filereq
                               )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  file_list_t *file = NULL;
  int rc, save_serrno, tapeCopyNb = 0;
  struct Cns_fileid *castorFileId = NULL;
  char *blkid;

  if ( (tapereq == NULL) || (filereq == NULL) ) {
    (void)dlf_write(
                    childUuid,
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "Invalid parameter",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = EINVAL;
    return(-1);
  }

  if ( rtcpcld_lockTape() == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_lockTape()");
    return(-1);
  }
  
  rc = rtcpcld_findFile(tape,filereq,&file);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_findFile()");
    return(-1);
  }
  file->filereq = *filereq;

  if ( rtcpcld_unlockTape() == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_unlockTape()");
    return(-1);
  }

  
  (void)rtcpcld_getFileId(file,&castorFileId);
  blkid = rtcp_voidToString(
                            (void *)filereq->blockid,
                            sizeof(filereq->blockid)
                            );
  if ( blkid == NULL ) blkid = strdup("unknown");

  if ( (filereq->cprc == 0) && (filereq->proc_status == RTCP_FINISHED) ) {
    rc = updateSegmCount(0,1,0);
    if ( rc == -1 ) {
      LOG_SYSCALL_ERR("updateSegmCount()");
    }
    rc = rtcpcld_updcFileRecalled(
                                  tape,
                                  file
                                  );
    if ( rc == -1 ) {
      LOG_SYSCALL_ERR("rtcpcld_updcFileRecalled()");
      return(-1);
    }
    (void)rtcpcld_setatime(file);
  } else {
    /*
     * Segment failed
     */
    rc = rtcpcld_updcRecallFailed(
                                  tape,
                                  file
                                  );
    if ( rc == -1 ) {
      LOG_SYSCALL_ERR("rtcpcld_updcRecallFailed()");
      return(-1);
    }
    setAborted(1);
  }

  return(0);
}

int recallerCallbackMoreWork(
                             tapereq,
                             filereq
                             )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  int rc, save_serrno, totalWaittime = 0;
  time_t tBefore, tNow;
  file_list_t *file = NULL;
  
  if ( rtcpcld_lockTape() == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_lockTape()");
    return(-1);
  }

  tBefore = time(NULL);
  for (;;) {
    if ( checkAborted(NULL) != 0 ) {
      (void)rtcpcld_unlockTape();
      return(-1);
    }
    file = NULL;
    rc = rtcpcld_getSegmentToDo(tape,&file);
    if ( rc == -1 ) {
      if ( (serrno == EAGAIN) &&
           (nbRunningSegms() > 0) ) {
        tNow = time(NULL);
        totalWaittime = (int)(tNow-tBefore);
        (void)dlf_write(
                        childUuid,
                        RTCPCLD_LOG_MSG(RTCPCLD_MSG_WAITSEGMS),
                        (struct Cns_fileid *)NULL,
                        1,
                        "WAITTIME",
                        DLF_MSG_PARAM_INT,
                        totalWaittime
                        );
        if ( rtcpcld_unlockTape() == -1 ) {
          LOG_SYSCALL_ERR("rtcpcld_unlockTape()");
          return(-1);
        }
        if ( totalWaittime > (RTCP_NETTIMEOUT*3)/4 ) {
          (void)dlf_write(
                          childUuid,
                          RTCPCLD_LOG_MSG(RTCPCLD_MSG_WAITTIMEOUT),
                          (struct Cns_fileid *)NULL,
                          1,
                          "WAITTIME",
                          DLF_MSG_PARAM_INT,
                          totalWaittime
                          );
          filereq->proc_status = RTCP_FINISHED;
          return(0);
        }
        sleep(2);
        if ( rtcpcld_lockTape() == -1 ) {
          LOG_SYSCALL_ERR("rtcpcld_lockTape()");
          return(-1);
        }
        continue;
      } else {
        /*
         * serrno != EAGAIN
         */
        save_serrno = serrno;
        LOG_SYSCALL_ERR("rtcpcld_getSegmentsToDo()");
        (void)rtcpcld_unlockTape();
        serrno = save_serrno;
        return(-1);
      }
    }
  }
  /*
   * We are here because there is at least one segment to process
   */
  
  return(0);
}

int recallerCallback(
                     tapereq,
                     filereq
                     )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  int rc = 0, save_serrno, msgNo, level = DLF_LVL_SYSTEM;
  struct Cns_fileid *castorFileId = NULL;
  file_list_t *file = NULL;
  char *blkid = NULL, *func = NULL;

  if ( tapereq == NULL || filereq == NULL ) {
    (void)dlf_write(
                    childUuid,
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "Invalid parameter",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = EINVAL;
    return(-1);
  }

  if ( rtcpcld_lockTape() == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_lockTape()");
    return(-1);
  }

  rc = rtcpcld_findFile(tape,filereq,&file);
  if ( (rc != -1) && (file != NULL) ) {
    (void)rtcpcld_getFileId(file,&castorFileId);
  }

  if ( rtcpcld_unlockTape() == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_unlockTape()");
    return(-1);
  }


  blkid = rtcp_voidToString(
                            (void *)filereq->blockid,
                            sizeof(filereq->blockid)
                            );
  if ( blkid == NULL ) blkid = strdup("unknown");

  switch (filereq->proc_status) {
  case RTCP_POSITIONED:
    msgNo = RTCPCLD_MSG_CALLBACK_POS;
    func = "processTapePositionCallback";
    if ( (tapereq->tprc != 0) ||
         (filereq->cprc != 0) ) {
      msgNo = RTCPCLD_MSG_CALLBACK_CP;
      func = "processFileCopyCallback";
      rc = recallerCallbackFileCopied(
                                      tapereq,
                                      filereq
                                      );
    }
    break;
  case RTCP_FINISHED:
    msgNo = RTCPCLD_MSG_CALLBACK_CP;
    func = "processFileCopyCallback";
    rc = recallerCallbackFileCopied(
                                    tapereq,
                                    filereq
                                    );
    break;
  case RTCP_REQUEST_MORE_WORK:
    msgNo = RTCPCLD_MSG_CALLBACK_GETW;
    func = "processGetMoreWorkCallback";
    if ( filereq->cprc == 0 ) {
      rc = recallerCallbackMoreWork(
                                    tapereq,
                                    filereq
                                    );
    }
    break;
  default:
    msgNo = RTCPCLD_MSG_INTERNAL;
    level = DLF_LVL_ERROR;
    func = "unprocessedCallback";
    if ( (tapereq->tprc != 0) ||
         (filereq->cprc != 0) ) {
      msgNo = RTCPCLD_MSG_CALLBACK_CP;
      func = "processFileCopyCallback";
      rc = recallerCallbackFileCopied(
                                      tapereq,
                                      filereq
                                      );
    }
    break;
  }

  if ( msgNo > 0 ) {
    if ( (tapereq->tprc == 0) && (filereq->cprc == 0) ) {
      (void)dlf_write(
                      childUuid,
                      level,
                      msgNo,
                      castorFileId,
                      9,
                      "",
                      DLF_MSG_PARAM_TPVID,
                      tapereq->vid,
                      "TPSERV",
                      DLF_MSG_PARAM_STR,
                      tapereq->server,
                      "TPDRIVE",
                      DLF_MSG_PARAM_STR,
                      tapereq->unit,
                      "",
                      DLF_MSG_PARAM_UUID,
                      tapereq->rtcpReqId,
                      "",
                      DLF_MSG_PARAM_UUID,
                      filereq->stgReqId,
                      "FSEQ",
                      DLF_MSG_PARAM_INT,
                      filereq->tape_fseq,
                      "BLKID",
                      DLF_MSG_PARAM_STR,
                      (blkid != NULL ? blkid : "unknown"),
                      "PATH",
                      DLF_MSG_PARAM_STR,
                      filereq->file_path,
                      "STATUS",
                      DLF_MSG_PARAM_INT,
                      filereq->proc_status
                      );
    } else {
      int tmpRC;
      rtcpErrMsg_t *err;
      if ( filereq->cprc != 0 ) {
        tmpRC = filereq->cprc;
        err = &(filereq->err);
      } else {
        tmpRC = tapereq->tprc;
        err = &(tapereq->err);
      }
      (void)dlf_write(
                      childUuid,
                      RTCPCLD_LOG_MSG(msgNo),
                      castorFileId,
                      12,
                      "",
                      DLF_MSG_PARAM_TPVID,
                      tapereq->vid,
                     "TPSERV",
                      DLF_MSG_PARAM_STR,
                      tapereq->server,
                      "TPDRIVE",
                      DLF_MSG_PARAM_STR,
                      tapereq->unit,
                      "",
                      DLF_MSG_PARAM_UUID,
                      tapereq->rtcpReqId,
                      "",
                      DLF_MSG_PARAM_UUID,
                      filereq->stgReqId,
                      "FSEQ",
                      DLF_MSG_PARAM_INT,
                      filereq->tape_fseq,
                      "BLKID",
                      DLF_MSG_PARAM_STR,
                      (blkid != NULL ? blkid : "unknown"),
                      "STATUS",
                      DLF_MSG_PARAM_INT,
                      filereq->proc_status,
                      (filereq->cprc != 0 ? "CPRC" : "TPRC"),
                      DLF_MSG_PARAM_INT,
                      tmpRC,
                      "ERROR_CODE",
                      DLF_MSG_PARAM_INT,
                      err->errorcode,
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      err->errmsgtxt,
                      "RTCP_SEVERITY",
                      DLF_MSG_PARAM_INT,
                      err->severity
                      );
    }
  }
  if ( blkid != NULL ) free(blkid);
  if ( castorFileId != NULL ) free(castorFileId);
  return(rc);
}

int main(
         argc,
         argv
         )
     int argc;
     char **argv;
{  
  char *recallerFacility = RECALLER_FACILITY_NAME, cmdline[CA_MAXLINELEN+1];
  int rc, c, i, save_serrno = 0;

  /*
   * If we are started by the rtcpclientd, the main accept socket has been
   * duplicated to file descriptor 0
   */
  SOCKET sock = 0;

  /* Initializing the C++ log */
  /* Necessary at start of program and after any fork */
  C_BaseObject_initLog("NewStagerLog", SVC_NOMSG);

  Cuuid_create(
               &childUuid
               );
  
  /*
   * Initialise DLF for our facility
   */
  (void)rtcpcld_initLogging(
                            recallerFacility
                            );
  cmdline[0] = '\0';
  c=0;
  for (i=0; (i<CA_MAXLINELEN) && (c<argc);) {
    strcat(cmdline,argv[c++]);
    strcat(cmdline," ");
    i = strlen(cmdline)+1;
  }
  
  (void)dlf_write(
                  childUuid,
                  RTCPCLD_LOG_MSG(RTCPCLD_MSG_RECALLER_STARTED),
                  (struct Cns_fileid *)NULL,
                  1,
                  "COMMAND",
                  DLF_MSG_PARAM_STR,
                  cmdline
                  );

  /*
   * Create our tape list
   */
  rc = rtcp_NewTapeList(&tape,NULL,WRITE_ENABLE);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("rtcp_NewTapeList()");
    return(1);
  }

  rc = rtcpcld_initLocks(tape);
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_initLocks()");
    return(1);
  }

  rc = rtcpcld_initNsInterface();
  if ( rc == -1 ) {
    LOG_SYSCALL_ERR("rtcpcld_initNsInterface()");
    return(1);
  }

  rc = rtcpcld_parseWorkerCmd(
                              argc,
                              argv,
                              tape,
                              &sock
                              );
  if ( rc == 0 ) {
    rc = rtcpcld_initThreadPool(WRITE_ENABLE);
    if ( rc == -1 ) {
      LOG_SYSCALL_ERR("initThreadPool()");
    } else {
      rc = rtcpcld_runWorker(
                             tape,
                             (tape->tapereq.VolReqID <= 0 ? NULL : &sock),
                             rtcpcld_myDispatch,
                             recallerCallback
                             );
    }
  }
  if ( rc == -1 ) save_serrno = serrno;

  rc = rtcpcld_workerFinished(
                              tape,
                              rc,
                              save_serrno
                              );
  return(rc);
}
