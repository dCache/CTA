/*
 *
 * Copyright (C) 2004 by CERN/IT/ADC/CA
 * All rights reserved
 *
 * @(#)$RCSfile: VidWorker.c,v $ $Revision: 1.1 $ $Release$ $Date: 2004/05/18 14:49:56 $ $Author: obarring $
 *
 *
 *
 * @author Olof Barring
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: VidWorker.c,v $ $Revision: 1.1 $ $Release$ $Date: 2004/05/18 14:49:56 $ Olof Barring";
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
#include <Cgetopt.h>
#include <Cinit.h>
#include <Cuuid.h>
#include <Cpwd.h>
#include <Cnetdb.h>
#include <Cthread_api.h>
#include <dlf_api.h>
#include <rtcp_constants.h>
#include <vdqm_api.h>
#include <vmgr_api.h>
#include <rtcp.h>
#include <rtcp_server.h>
#include <rtcp_api.h>
#include <rtcpcldCatalog.h>
#include <rtcpcld_constants.h>
#include <rtcpcld.h>
#include <rtcpcld_messages.h>
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
tape_list_t *vidChildTape = NULL;

int inChild = 1;

static int processTapePositionCallback(
                                       tapereq,
                                       filereq
                                       )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  return(0);
}

static int processFileCopyCallback(
                                   tapereq,
                                   filereq
                                   )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  return(0);
}


static int processGetMoreWorkCallback(
                                      tapereq,
                                      filereq
                                      )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  /*
   * Since we do not have access to the full request list we need a
   * static variable for remembering the number of requests we have
   * already sent off for processing. This is OK even if the
   * FILE request callbacks are multithreaded because RTCOPY will
   * always serialize the RQST_REQUEST_MORE_WORK callbacks.
   */
  static int nbInProgress = 0;
  int rc, found;
  file_list_t *fl = NULL;

  if ( tapereq == NULL || filereq == NULL ) {
    serrno = EINVAL;
    return(-1);
  }  

  /*
   * We are called in a loop file-by-file. Since ach Catalogue lookup
   * may return more than one file we must maintain an internal
   * list of unprocessed files. The first thing to do is to
   * check if that list.
   */
  if ( vidChildTape == NULL ) {
    rc = rtcp_NewTapeList(
                          &vidChildTape,
                          NULL,
                          tapereq->mode
                          );
    if ( rc == -1 ) {
      (void)dlf_write(
                      childUuid,
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "rtcp_NewTapeList()",
                      "ERROR_STRING",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
      return(-1);
    }
    vidChildTape->tapereq = *tapereq;
  }
  found = FALSE;
  CLIST_ITERATE_BEGIN(vidChildTape->file,fl) {
    if ( fl->filereq.proc_status == RTCP_WAITING ) {
      found = TRUE;
      break;
    }
  } CLIST_ITERATE_END(vidChildTape->file,fl);

  if ( found == FALSE ) {
    /*
     * Internal list empty or has no waiting requests (however that
     * happened?). Get more file requests from Catalogue.
     */
    rc = rtcpcld_getReqsForVID(
                               vidChildTape
                               );
    if ( rc == -1 ) {
      (void)dlf_write(
                      childUuid,
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "rtcpcld_getReqsForVID()",
                      "ERROR_STRING",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
      return(-1);
    }
    /*
     * Check if any new requests were found in Catalogue
     */
    CLIST_ITERATE_BEGIN(vidChildTape->file,fl) {
      if ( fl->filereq.proc_status == RTCP_WAITING ) {
        found = TRUE;
        break;
      }
    } CLIST_ITERATE_END(vidChildTape->file,fl);
  }

  if ( (fl == NULL) || (found == FALSE) ) {
    /*
     * Nothing more found... Give up (for the moment)
     */
    rc = -1;
  } else if ( (*fl->filereq.file_path == '\0') || 
              (*fl->filereq.file_path == '.') ) {
    /*
     * A waiting request was found but no path has been assigned yet.
     * Iterate until we get a path or an error occurred
     */
    while ( (*fl->filereq.file_path == '\0') ||
            (*fl->filereq.file_path == '.') ) {
      rc = rtcpcld_getPhysicalPath(
                                   &vidChildTape->tapereq, 
                                   &fl->filereq
                                   );
      if ( (rc != -1) &&
           (*fl->filereq.file_path != '\0') &&
           (*fl->filereq.file_path != '.') ) {
        /*
         * Valid path found. Go on with the file copying.
         */
        nbInProgress++;
        filereq->proc_status = RTCP_WAITING;
        rc = rtcpcld_setFileStatus(
                                   &fl->filereq,
                                   TPFILEINFO_COPYRUNNING
                                   );
        if ( rc == -1 ) {
          (void)dlf_write(
                          childUuid,
                          DLF_LVL_ERROR,
                          RTCPCLD_MSG_SYSCALL,
                          (struct Cns_fileid *)NULL,
                          RTCPCLD_NB_PARAMS+2,
                          "SYSCALL",
                          DLF_MSG_PARAM_STR,
                          "rtcpcld_setFileStatus()",
                          "ERROR_STRING",
                          DLF_MSG_PARAM_STR,
                          sstrerror(serrno),
                          RTCPCLD_LOG_WHERE
                          );
          return(-1);
        }
        break;
      } else {
        if ( (rc == -1) && (serrno == ENOENT) ) {
          /*
           * Oops, our waiting request has disappeared.
           * Give up searching (for the moment).
           */
          break;
        } else if ( (rc == -1) && (serrno == EAGAIN) ) {
          /*
           * There might be a path coming soon. Wait for
           * it unless there is already some stuff to do.
           */
          if ( nbInProgress > 0 ) break;
          sleep(1);
          continue;
        } else {
          /*
           * Something strange happened. Give up immediately.
           */
          (void)dlf_write(
                          childUuid,
                          DLF_LVL_ERROR,
                          RTCPCLD_MSG_SYSCALL,
                          (struct Cns_fileid *)NULL,
                          RTCPCLD_NB_PARAMS+4,
                          "SYSCALL",
                          DLF_MSG_PARAM_STR,
                          "rtcpcld_getPhysicalPath()",
                          "RETVAL",
                          DLF_MSG_PARAM_INT,
                          rc,
                          "FILE_PATH",
                          DLF_MSG_PARAM_STR,
                          fl->filereq.file_path,
                          "ERROR_STRING",
                          DLF_MSG_PARAM_STR,
                          sstrerror(serrno),
                          RTCPCLD_LOG_WHERE
                          );
          return(-1);
        }
      }
    }
  } else {
    /*
     * Path is OK. Go on with the copying
     */
    nbInProgress++;
    rc = rtcpcld_setFileStatus(
                               &fl->filereq,
                               TPFILEINFO_COPYRUNNING
                               );
    rc = 0;
  }
  
  if ( rc == 0 ) {
    /*
     * Pop off request by request until all has been passed back 
     * through callback. Make sure to retain the original tape
     * path, given by server 
     */
    strncpy(
            fl->filereq.tape_path,
            filereq->tape_path,
            sizeof(fl->filereq.tape_path)-1
            );
    if ( fl->filereq.proc_status == RTCP_WAITING ) *filereq = fl->filereq;
    CLIST_DELETE(vidChildTape->file,fl);
    free(fl);
  } else {
    if ( nbInProgress > 0 ) {
      /*
       * There is already something to do. Go ahead with the processing
       * and we will be called again once it has finished. Reset the
       * counter for next callback.
       */
      nbInProgress = 0;
    } else {
      /*
       * No more files to copy. Flag the file request as finished
       * in order to stop the processing (any value different
       * from RTCP_REQUEST_MORE_WORK or RTCP_WAITING would do).
       */
      filereq->proc_status = RTCP_FINISHED;
    }
  }
  return(0);
}
  
  
/** rtcpcld_Callback() - VidWorker processing of RTCOPY callbacks
 *
 */
int rtcpcld_Callback(
                     tapereq,
                     filereq
                     )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  char *func, *vid = "", *blkid = NULL, *disk_path = ".";
  int fseq = -1, proc_status = -1, msgNo = -1, rc, status, getMoreWork = 0;
  struct Cns_fileid fileId;
  Cuuid_t rtcpUuid, stgUuid;
  file_list_t *fl = NULL;

  if ( tapereq == NULL || filereq == NULL ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
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

  if ( tapereq != NULL ) {
    vid = tapereq->vid;
    rtcpUuid = tapereq->rtcpReqId;
  }

  if ( filereq != NULL ) {
    if ( filereq->proc_status == RTCP_POSITIONED ) {
      msgNo = RTCPCLD_MSG_CALLBACK_POS;
      func = "processTapePositionCallback";
      rc = processTapePositionCallback(
                                       tapereq,
                                       filereq
                                       );
    } else if ( filereq->proc_status == RTCP_REQUEST_MORE_WORK ) {
      getMoreWork = 1;
      msgNo = RTCPCLD_MSG_CALLBACK_GETW;
      func = "processGetMoreWorkCallback";
      rc = processGetMoreWorkCallback(
                                      tapereq,
                                      filereq
                                      );
    } else {
      msgNo = RTCPCLD_MSG_CALLBACK_CP;
      func = "processFileCopyCallback";
      rc = processFileCopyCallback(
                                       tapereq,
                                       filereq
                                       );
    }
    if ( rc == -1 ) {
      (void)dlf_write(
                      childUuid,
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      func,
                      "ERROR_STRING",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
      return(-1);
    }

    /*
     * Only update catalogue for real FILE callbacks. Note
     * that for a RTCP_REQUSET_MORE_WORK callback, the status
     * is changed to RTCP_FINISHED when there is nothing more
     * to do.
     */
    if ( getMoreWork == 0 && filereq->proc_status == RTCP_FINISHED ) {
      if ( filereq->cprc == 0 ) status = TPFILEINFO_FILECOPIED;
      else status = TPFILEINFO_FAILED;
      rc = rtcpcld_setFileStatus(
                                 filereq,
                                 status
                                 );
      if ( rc == -1 ) {
        (void)dlf_write(
                        childUuid,
                        DLF_LVL_ERROR,
                        RTCPCLD_MSG_SYSCALL,
                        (struct Cns_fileid *)NULL,
                        RTCPCLD_NB_PARAMS+2,
                        "SYSCALL",
                        DLF_MSG_PARAM_STR,
                        "rtcpcld_setFileStatus()",
                        "ERROR_STRING",
                        DLF_MSG_PARAM_STR,
                        sstrerror(serrno),
                        RTCPCLD_LOG_WHERE
                        );
        return(-1);
      }
    }
    fseq = filereq->tape_fseq;
    blkid = rtcp_voidToString(
                              (void *)filereq->blockid,
                              sizeof(filereq->blockid)
                              );
    if ( blkid == NULL ) blkid = strdup("unknown");
    fileId.fileid = filereq->castorSegAttr.castorFileId;
    strncpy(
            fileId.server,
            filereq->castorSegAttr.nameServerHostName,
            sizeof(fileId.server)-1
            );
    stgUuid = filereq->stgReqId;
    proc_status = filereq->proc_status;
    disk_path = filereq->file_path;
  } /* if ( filereq != NULL ) */

  if ( msgNo > 0 ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_SYSTEM,
                    msgNo,
                    &fileId,
                    6,
                    "",
                    DLF_MSG_PARAM_TPVID,
                    vid,
                    "",
                    DLF_MSG_PARAM_UUID,
                    rtcpUuid,
                    "",
                    DLF_MSG_PARAM_UUID,
                    stgUuid,
                    "FSEQ",
                    DLF_MSG_PARAM_INT,
                    fseq,
                    "BLKID",
                    DLF_MSG_PARAM_STR,
                    (blkid != NULL ? blkid : "unknown"),
                    "STATUS",
                    DLF_MSG_PARAM_INT,
                    proc_status
                    );
  }
  if ( blkid != NULL ) free(blkid);
  return(0);
}

/** myDispatch() - FILE request dispatch function to be used by RTCOPY API
 *
 * @param func - function to be dispatched
 * @param arg - pointer to opaque function argument
 *
 * myDispatch() creates a thread for dispatching the processing of FILE request
 * callbacks in the extended RTCOPY API. This allows for handling weakly
 * blocking callbacks like tppos to not be serialized with potentially strongly
 * blocking callbascks like filcp or request for more work.
 * 
 * Note that the Cthread_create_detached() API cannot be passed directly in
 * the RTCOPY API because it is a macro and not a proper function prototype.
 *
 * @return -1 = error otherwise the Cthread id (>=0)
 */
int myDispatch(
               func,
               arg
               )
     void *(*func)(void *);
     void *arg;
{
  return(Cthread_create_detached(
                                 func,
                                 arg
                                 ));  
}

static int updateClientInfo(
                            origSocket,
                            port,
                            tape
                            )
     SOCKET *origSocket;
     int port;
     tape_list_t *tape;
{
  vdqmVolReq_t volReq;
  vdqmDrvReq_t drvReq;
  struct passwd *pw;
  int rc;
  uid_t myUid;
  gid_t myGid;

  if ( origSocket == NULL || 
       *origSocket == INVALID_SOCKET || 
       port < 0 || 
       tape == NULL ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "invalid parameter",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = EINVAL;
    return(-1);
  }
  memset(&volReq,'\0',sizeof(volReq));
  memset(&drvReq,'\0',sizeof(drvReq));

  errno = serrno = 0;
  myUid = geteuid();
  myGid = getegid();
  pw = Cgetpwuid(myUid);
  if ( pw == NULL ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_PWUID,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    return(-1);
  }
  strncpy(
          volReq.client_name,
          pw->pw_name,
          sizeof(volReq.client_name)-1
          );
  volReq.clientUID = myUid;
  volReq.clientGID = myGid;
  (void)gethostname(
                    volReq.client_host,
                    sizeof(volReq.client_host)-1
                    );
  volReq.client_port = port;
  strncpy(
          drvReq.dgn,
          tape->tapereq.dgn,
          sizeof(drvReq.dgn)-1
          );
  volReq.VolReqID = tape->tapereq.VolReqID;
  strncpy(
          drvReq.drive,
          tape->tapereq.unit,
          sizeof(drvReq.drive)-1
          );
  errno = serrno = 0;
  rc = vdqm_SendToRTCP(
                       *origSocket,
                       &volReq,
                       &drvReq
                       );
  if ( rc == -1 ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    return(-1);
  }
  closesocket(
              *origSocket
              );
  *origSocket = INVALID_SOCKET;
  return(0);
}



static int vidWorker(
                     origSocket,
                     tape
                     )
     SOCKET *origSocket;
     tape_list_t *tape;
{
  rtcpc_sockets_t *socks;
  rtcpHdr_t hdr;
  int rc, port = -1, reqId;

  if ( tape == NULL || 
       *tape->tapereq.vid == '\0' ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
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

  (void)dlf_write(
                  mainUuid,
                  DLF_LVL_SYSTEM,
                  RTCPCLD_MSG_VIDWORKER_STARTED,
                  (struct Cns_fileid *)NULL,
                  5,
                  "",
                  DLF_MSG_PARAM_UUID,
                  childUuid,
                  "",
                  DLF_MSG_PARAM_TPVID,
                  tape->tapereq.vid,
                  "MODE",
                  DLF_MSG_PARAM_INT,
                  tape->tapereq.mode,
                  "VOLREQID",
                  DLF_MSG_PARAM_INT,
                  tape->tapereq.VolReqID,
                  "PID",
                  DLF_MSG_PARAM_INT,
                  getpid()
                  );

  /*
   * Check that there still are any requests for the VID
   */
  errno = serrno = 0;
  rc = rtcpcld_anyReqsForVID(
                             tape
                             );
  if ( rc == -1 ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    return(-1);
  }

  if ( rc == 0 ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_SYSTEM,
                    RTCPCLD_MSG_NOREQS,
                    (struct Cns_fileid *)NULL,
                    0
                    );
    return(0);
  }

  /*
   * Check the request and initialise our private listen socket
   * that we will use for further communication with rtcpd
   */
  errno = serrno = 0;
  rc = rtcpc_InitReq(
                     &socks,
                     &port,
                     tape
                     );
  if ( rc == -1 ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    return(-1);
  }

  if ( origSocket != NULL ) {
    /*
     * Send the new address to the rtcpd server. The original vdqm request
     * contained the rtcpclientd port, which we cannot use.
     */
    errno = serrno = 0;
    rc = updateClientInfo(
                          origSocket,
                          port,
                          tape
                          );
    if ( rc == -1 ) return(-1);  /* error already logged */
  } else {
    /*
     * VDQM not yet called when running in standalone mode.
     */
    rc = vdqm_SendVolReq(
                         NULL,
                         &(tape->tapereq.VolReqID),
                         tape->tapereq.vid,
                         tape->tapereq.dgn,
                         NULL,
                         NULL,
                         tape->tapereq.mode,
                         port
                         );
    if ( rc == -1 ) {
      (void)dlf_write(
                      childUuid,
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_VDQM,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+1,
                      "ERROR_STR", 
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
      return(-1);
    }
  }
  
  /*
   * Run the request;
   */
  rtcpc_ClientCallback = rtcpcld_Callback;

  reqId = tape->tapereq.VolReqID;
  rc = rtcpc_SelectServer(
                          &socks,
                          tape,
                          NULL,
                          port,
                          &reqId
                          );
  if ( rc == -1 ) return(-1);
  vidChildTape = NULL;
  hdr.magic = RTCOPY_MAGIC;
  rc = rtcpc_sendReqList(
                         &hdr,
                         &socks,
                         tape
                         );
  if ( rc == -1 ) return(-1);
  rc = rtcpc_sendEndOfReq(
                          &hdr,
                          &socks,
                          tape
                          );
  if ( rc == -1 ) return(-1);
  rc = rtcpc_runReq_ext(
                        &hdr,
                        &socks,
                        tape,
                        myDispatch
                        );
  return(rc);
}

static int checkArgs(
                     vid,
                     dgn,
                     lbltype,
                     density,
                     unit,
                     vdqmVolReqID
                     )
     char *vid, *dgn, *lbltype, *density, *unit;
     int vdqmVolReqID;
{
  int rc = 0;

  if ( vid == NULL || strlen(vid) > CA_MAXVIDLEN ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "VID wrongly specified",
                    "VALUE",
                    DLF_MSG_PARAM_STR,
                    (vid != NULL ? vid : "(null)"),
                    RTCPCLD_LOG_WHERE
                    );
    rc = -1;
  }
  if ( (dgn == NULL && vdqmVolReqID > 0) || 
       (dgn != NULL && strlen(dgn) > CA_MAXDGNLEN) ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "DGN wrongly specified",
                    "VALUE",
                    DLF_MSG_PARAM_STR,
                    (dgn != NULL ? dgn : "(null)"),
                    RTCPCLD_LOG_WHERE
                    );
    rc = -1;
  }
  if ( (lbltype == NULL && vdqmVolReqID > 0) || 
       (lbltype != NULL && strlen(lbltype) > CA_MAXLBLTYPLEN) ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "Label type wrongly specified",
                    "VALUE",
                    DLF_MSG_PARAM_STR,
                    (lbltype != NULL ? lbltype : "(null)"),
                    RTCPCLD_LOG_WHERE
                    );
    rc = -1;
  }
  if ( (density == NULL && vdqmVolReqID > 0) || 
       (density != NULL && strlen(density) > CA_MAXDENLEN) ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "Density wrongly specified",
                    "VALUE",
                    DLF_MSG_PARAM_STR,
                    (density != NULL ? density : "(null)"),
                    RTCPCLD_LOG_WHERE
                    );
    rc = -1;
  }

  if ( (unit == NULL && vdqmVolReqID > 0) || 
       (unit != NULL && strlen(unit) > CA_MAXUNMLEN) ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "ERROR_STRING",
                    DLF_MSG_PARAM_STR,
                    "Tape unit wrongly specified",
                    "VALUE",
                    DLF_MSG_PARAM_STR,
                    (unit != NULL ? unit : "(null)"),
                    RTCPCLD_LOG_WHERE
                    );
    rc = -1;
  }

  return(rc);
}

static int callVmgr(
                    vid,
                    side,
                    dgn,
                    density,
                    lbltype
                    )
     char *vid, **dgn, **density, **lbltype;
     int side;
{
  char vmgr_error_buffer[512], tmpdgn[CA_MAXDGNLEN+1];
  struct vmgr_tape_info vmgr_tapeinfo;
  int rc;
  
  vmgr_seterrbuf(
                 vmgr_error_buffer,
                 sizeof(vmgr_error_buffer)
                 );
  memset(
         &vmgr_tapeinfo,
         '\0',
         sizeof(vmgr_tapeinfo)
         );
  rc = vmgr_querytape (
                       vid,
                       side,
                       &vmgr_tapeinfo,
                       tmpdgn
                       );
  if ( ((vmgr_tapeinfo.status & DISABLED) == DISABLED) ||
       ((vmgr_tapeinfo.status & EXPORTED) == EXPORTED) ||
       ((vmgr_tapeinfo.status & ARCHIVED) == ARCHIVED) ) {
    serrno = EACCES;
    return(-1);
  }

  if ( *dgn == NULL ) *dgn = strdup(tmpdgn);
  else strcpy(*dgn,tmpdgn);
  if ( *density == NULL ) *density = strdup(vmgr_tapeinfo.density);
  else strcpy(*density,vmgr_tapeinfo.density);
  if ( *lbltype == NULL ) *lbltype = strdup(vmgr_tapeinfo.lbltype);
  else strcpy(*lbltype,vmgr_tapeinfo.lbltype);
  return(0);
}

static int initTapeReq(
                       tape,
                       vid,
                       side,
                       mode,
                       _dgn,
                       _density,
                       _lbltype,
                       unit,
                       vdqmVolReqID
                       )
     tape_list_t **tape;
     char *vid, *_dgn, *_density, *_lbltype, *unit;
     int side, mode, vdqmVolReqID;
{
  char *dgn, *density, *lbltype;
  int rc;
  
  dgn = _dgn;
  density = _density;
  lbltype = _lbltype;

  if ( tape == NULL || vid == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  
  rc = rtcp_NewTapeList(
                        tape,
                        NULL,
                        mode
                        );
  if ( rc == -1 || *tape == NULL ) {
    (void)dlf_write(
                    childUuid,
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "SYSCALL",
                    DLF_MSG_PARAM_STR,
                    "rtcp_NewTapeList()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    return(-1);
  }

  if ( dgn == NULL || density == NULL || lbltype == NULL ) {
    /*
     * Get missing tape parameters from VMGR
     */
    rc = callVmgr(
                  vid,
                  side,
                  &dgn,
                  &density,
                  &lbltype
                  );
    if ( rc == -1 || dgn == NULL || density == NULL || lbltype == NULL ) {
      (void)dlf_write(
                      childUuid,
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "callVmgr()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
      return(-1);
    }
  }

  strncpy(
          (*tape)->tapereq.vid,
          vid,
          sizeof((*tape)->tapereq.vid)-1
          );
  strncpy(
          (*tape)->tapereq.dgn,
          dgn,
          sizeof((*tape)->tapereq.dgn)-1
          );
  strncpy(
          (*tape)->tapereq.density,
          density,
          sizeof((*tape)->tapereq.density)-1
          );
  strncpy(
          (*tape)->tapereq.label,
          lbltype,
          sizeof((*tape)->tapereq.label)-1
          );
  if ( unit != NULL ) {
    strncpy(
            (*tape)->tapereq.unit,
            unit,
            sizeof((*tape)->tapereq.unit)-1
            );
  }
  
  (*tape)->tapereq.VolReqID = vdqmVolReqID;
  
  return(0);
}

int main(
         argc,
         argv
         )
     int argc;
     char **argv;
{
  tape_list_t *tape = NULL;
  char *vid = NULL, *dgn = NULL, *lbltype = NULL, *density = NULL;
  char  *unit = NULL, *mainUuidStr = NULL, *shiftMsg;
  char *vidChildFacility = RTCPCLIENTD_FACILITY_NAME, cmdline[CA_MAXLINELEN+1];
  int i, vdqmVolReqID = -1, c, rc, mode = WRITE_DISABLE, modeSet = 0, side = 0;
  int save_serrno, retval, tStartRequest = 0;
  /*
   * If we are started by the rtcpclientd, the main accept socket has been
   * duplicated to file descriptor 0
   */
  SOCKET origSocket = 0;
  
  Coptind = 1;
  Copterr = 1;
  vid = dgn = lbltype = density = NULL;

  Cuuid_create(
               &childUuid
               );
  /*
   * Initialise DLF
   */
  (void)rtcpcld_initLogging(
                            vidChildFacility
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
                  DLF_LVL_SYSTEM,
                  RTCPCLD_MSG_VIDWORKER_STARTED,
                  (struct Cns_fileid *)NULL,
                  1,
                  "COMMAND",
                  DLF_MSG_PARAM_STR,
                  cmdline
                  );

  while ( (c = Cgetopt(argc, argv, "V:g:i:l:d:s:S:U:u:T:rw")) != -1 ) {
    switch (c) {
    case 'V':
      /*
       * Tape VID. This argument is required
       */
      vid = Coptarg;
      break;
    case 'g':
      /* 
       * Device group name. If not provided, it will be taken from VMGR
       */
      dgn = Coptarg;
      break;
    case 'i':
      /*
       * VDQM Volume Request ID. This argument is passed by the rtcpclientd
       * daemon, which starts the VidWorker only when the tape request has
       * started on the tape server. If not provided, the VidWorker will
       * submit its own request to VDQM. This allows for stand-alone running
       * (and debugging) of the VidWorker program.
       */
      vdqmVolReqID = atoi(Coptarg);
      break;
    case 'l':
      /*
       * Tape label type. If not provided, it will be taken from VMGR
       */
      lbltype = Coptarg;
      break;
    case 'd':
      /*
       * Tape media density. If not provided, it will be taken from VMGR
       */
      density = Coptarg;
      break;
    case 's':
      /*
       * Media side (forseen for future support for two sided medias).
       * Defaults to 0
       */
      side = atoi(Coptarg);
      break;
    case 'S':
      /*
       * Parent decided to use a different socket than 0 .
       * Useful when running with Insure...
       */
      origSocket = atoi(Coptarg);
      break;
    case 'U':
      /*
       * UUID of the rtcpclientd daemon that started this VidWorker.
       * Used only for logging of UUID association.
       */
      mainUuidStr = Coptarg;
      serrno = 0;
      rc = rtcp_stringToVoid(
                             mainUuidStr,
                             &mainUuid,
                             sizeof(mainUuid)
                             );
      if ( rc == -1 ) {
        if ( serrno == 0 ) serrno = errno;
        (void)dlf_write(
                        childUuid,
                        DLF_LVL_ERROR,
                        RTCPCLD_MSG_SYSCALL,
                        (struct Cns_fileid *)NULL,
                        RTCPCLD_NB_PARAMS+2,
                        "SYSCALL",
                        DLF_MSG_PARAM_STR,
                        "rtcp_stringToVoid()",
                        "ERROR_STR",
                        DLF_MSG_PARAM_STR,
                        sstrerror(serrno),
                        RTCPCLD_LOG_WHERE
                        );
        return(1);
      }
      break;
    case 'u':
      unit = Coptarg;
      break;
    case 'T':
      tStartRequest = atoi(Coptarg);
      break;
    case 'r':
      /*
       * Read mode switch. Default if no tape access mode is specified
       */
      if ( modeSet != 0 ) {
        (void)dlf_write(
                        childUuid,
                        DLF_LVL_ERROR,
                        RTCPCLD_MSG_INTERNAL,
                        (struct Cns_fileid *)NULL,
                        RTCPCLD_NB_PARAMS+1,
                        "ERROR_STRING",
                        DLF_MSG_PARAM_STR,
                        "Duplicate or concurrent use of -r or -w switch",
                        RTCPCLD_LOG_WHERE
                        );
        return(2);
      }
      mode = WRITE_DISABLE;
      modeSet++;
      break;
    case 'w':
      /*
       * Write mode switch. Must be specified if tape write access is required.
       */
      if ( modeSet != 0 ) {
        (void)dlf_write(
                        childUuid,
                        DLF_LVL_ERROR,
                        RTCPCLD_MSG_INTERNAL,
                        (struct Cns_fileid *)NULL,
                        RTCPCLD_NB_PARAMS+1,
                        "ERROR_STRING",
                        DLF_MSG_PARAM_STR,
                        "Duplicate or concurrent use of -r or -w switch",
                        RTCPCLD_LOG_WHERE
                        );
        return(2);
      }
      mode = WRITE_ENABLE;
      modeSet++;
      break;
    default:
      (void)dlf_write(
                      childUuid,
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_INTERNAL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+1,
                      "ERROR_STRING",
                      DLF_MSG_PARAM_STR,
                      "Unknown option",
                      RTCPCLD_LOG_WHERE
                      );
      return(2);
      break;
    }
  }  

  /*
   * Fake a parent UUID in case we run stand-alone
   */
  if ( mainUuidStr == NULL ) Cuuid_create(
                                          &mainUuid
                                          );
  rc = checkArgs(
                 vid,
                 dgn,
                 lbltype,
                 density,
                 unit,
                 vdqmVolReqID
                 );
  if ( rc == -1 ) return(2);
  rc = initTapeReq(
                   &tape,
                   vid,
                   side,
                   mode,
                   dgn,
                   density,
                   lbltype,
                   unit,
                   vdqmVolReqID
                   );
  if ( rc == -1 ) return(2);  

  /*
   * Process the request
   */
  if ( tStartRequest <= 0 ) tStartRequest = (int)time(NULL);
  tape->tapereq.TStartRequest = tStartRequest;
  rc = vidWorker(
                 (vdqmVolReqID >= 0 ? &origSocket : NULL),
                 tape
                 );
  save_serrno = serrno;
  retval = 0;
  if ( rc == -1 ) {
    (void)rtcp_RetvalSHIFT(tape,NULL,&retval);
    if ( retval == 0 ) retval = UNERR;
  }
  switch (retval) {
  case 0:
    shiftMsg = "command successful";
    break;
  case RSLCT:
    shiftMsg = "Re-selecting another tape server";
    break;
  case BLKSKPD:
    shiftMsg = "command partially successful since blocks were skipped";
    break;
  case TPE_LSZ:
    shiftMsg = "command partially successful: blocks skipped and size limited by -s option";
    break;
  case MNYPARY:
    shiftMsg = "command partially successful: blocks skipped or too many errors on tape";
    break;
  case LIMBYSZ:
    shiftMsg = "command successful";
    break;
  default:
    shiftMsg = "command failed";
    break;
  }  
  (void)dlf_write(
                  mainUuid,
                  DLF_LVL_SYSTEM,
                  RTCPCLD_MSG_VIDWORKER_ENDED,
                  (struct Cns_fileid *)NULL,
                  7,
                  "",
                  DLF_MSG_PARAM_TPVID,
                  tape->tapereq.vid,
                  "MODE",
                  DLF_MSG_PARAM_INT,
                  tape->tapereq.mode,
                  "VOLREQID",
                  DLF_MSG_PARAM_INT,
                  tape->tapereq.VolReqID,
                  "PID",
                  DLF_MSG_PARAM_INT,
                  getpid(),
                  "rtcpRC",
                  DLF_MSG_PARAM_INT,
                  rc,
                  "serrno",
                  DLF_MSG_PARAM_INT,
                  save_serrno,
                  "SHIFTMSG",
                  DLF_MSG_PARAM_STR,
                  shiftMsg
                  );
  if ( rc == -1 ) {
    (void)rtcpcld_setVIDFailedStatus(tape);
  }
  
  return(retval);
}
