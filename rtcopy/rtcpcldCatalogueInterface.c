/******************************************************************************
 *                      rtcpcldCatalogueInterface.c
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
 * @(#)$RCSfile: rtcpcldCatalogueInterface.c,v $ $Revision: 1.4 $ $Release$ $Date: 2004/06/09 16:18:47 $ $Author: obarring $
 *
 * 
 *
 * @author Olof Barring
 *****************************************************************************/


#ifndef lint
static char sccsid[] = "@(#)$RCSfile: rtcpcldCatalogueInterface.c,v $ $Revision: 1.4 $ $Release$ $Date: 2004/06/09 16:18:47 $ Olof Barring";
#endif /* not lint */

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>                  /* Standard data types          */
#include <sys/stat.h>
#include <errno.h>
#if defined(_WIN32)
#include <io.h>
#include <winsock2.h>
extern char *geterr();
WSADATA wsadata;
#else /* _WIN32 */
#include <netdb.h>                      /* Network "data base"          */
#include <sys/socket.h>                 /* Socket interface             */
#include <netinet/in.h>                 /* Internet data types          */
#include <signal.h>
#include <sys/time.h>
#endif /* _WIN32 */
#include <patchlevel.h>
#include <Castor_limits.h>
#include <log.h>
#include <net.h>
#include <osdep.h>
#include <trace.h>
#include <serrno.h>
#include <Cgetopt.h>
#include <Cpwd.h>
#include <Cns_api.h>
#include <dlf_api.h>
#include <Cnetdb.h>
#include <Cuuid.h>
#include <u64subr.h>
#include <serrno.h>
#include <vmgr_api.h>
#include <Ctape_constants.h>
#include <castor/stager/Tape.h>
#include <castor/stager/Segment.h>
#include <castor/stager/TapeStatusCodes.h>
#include <castor/stager/SegmentStatusCodes.h>
#include <castor/stager/IStagerSvc.h>
#include <castor/Services.h>
#include <castor/BaseAddress.h>
#include <castor/db/DbAddress.h>
#include <castor/IAddress.h>
#include <castor/IObject.h>
#include <castor/IClient.h>
#include <castor/Constants.h>
#include <rtcp_constants.h>
#include <vdqm_api.h>
#include <rtcp.h>
#include <rtcp_api.h>
#include <rtcpcld_constants.h>
#include <rtcpcld_messages.h>
#include <rtcpcld.h>

int inChild;
Cuuid_t childUuid, mainUuid;

static int dbSvcKey = -1;
static int iSvcKey = -1;
static unsigned char nullblkid[4] = {'\0', '\0', '\0', '\0'};
static RtcpcldTapeList_t *tpList = NULL;

/*
 * We can affort this to be thread-unsafe since the
 * key will only be set once, just in the beginning of
 * the VidWorker process
 */
static ID_TYPE tapeKey(
                       _key
                       )
     ID_TYPE *_key;
{
  static ID_TYPE key;
  
  if ( _key != NULL ) key = *_key;
  return(key);
}

void rtcpcld_setTapeKey(
                        key
                        )
     ID_TYPE key;
{
  (void)tapeKey(&key);
  return;
}

ID_TYPE rtcpcld_getTapeKey() 
{
  return(tapeKey(NULL));
}

static int getDbSvc(
                    dbSvc
                    )
     struct C_Services_t **dbSvc;
{
  struct C_Services_t **svc;
  int rc, save_serrno;
  if ( dbSvc == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  svc = NULL;
  rc = Cglobals_get(&dbSvcKey,(void **)&svc,sizeof(struct C_Services_t **));
  if ( rc == -1 || svc == NULL ) {
      save_serrno = serrno;
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "Cglobals_get()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
    serrno = save_serrno;
    return(-1);
  }
  
  if ( *svc == NULL ) {
    rc = C_Services_create(svc);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_DBSVC,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "DBSVCCALL",
                      DLF_MSG_PARAM_STR,
                      "C_Services_create()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
  }
  *dbSvc = *svc;
  
  return(0);
}

static int getStgSvc(
                     stgSvc
                     )
     struct Cstager_IStagerSvc_t **stgSvc;
{
  struct C_Services_t *svcs = NULL;
  struct C_IService_t **iSvc = NULL;
  int rc, save_serrno;
  

  if ( stgSvc == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  
  rc = getDbSvc(&svcs);
  if ( rc == -1 ) return(-1);

  iSvc = NULL;
  rc = Cglobals_get(&iSvcKey,(void **)&iSvc,sizeof(struct C_IServices_t *));
  if ( rc == -1 || iSvc == NULL ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "SYSCALL",
                      DLF_MSG_PARAM_STR,
                    "Cglobals_get()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }
  rc = C_Services_service(svcs,"OraStagerSvc",SVC_ORASTAGERSVC, iSvc);
  if ( rc == -1 || *iSvc == NULL ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_DBSVC,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+3,
                    "DBSVCCALL",
                    DLF_MSG_PARAM_STR,
                    "C_IServices_service()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    "DB_ERROR",
                    DLF_MSG_PARAM_STR,
                    C_Services_errorMsg(svcs),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }

  *stgSvc = Cstager_IStagerSvc_fromIService(*iSvc);
  return(0);
}

static int cmpTape(
                   tl1,
                   tl2
                   )
     tape_list_t *tl1, *tl2;
{
  if ( tl1 == NULL || tl2 == NULL ) return(0);
  else if ( tl1 == tl2 ) return(1);
  else {
    if ( tl1->tapereq.mode != tl2->tapereq.mode ) return(0);
    if ( tl1->tapereq.side != tl2->tapereq.side ) return(0);
    if ( strcmp(tl1->tapereq.vid,tl2->tapereq.vid) != 0 ) return(0);
  }
  return(1);
}

static int findTape(
                    tapereq,
                    tp
                    )
     rtcpTapeRequest_t *tapereq;
     RtcpcldTapeList_t **tp;
{
  RtcpcldTapeList_t *tpIterator;
  tape_list_t *tl;
  int found = 0;

  if ( tapereq == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  if ( (*tapereq->vid == '\0') ||
       (tapereq->mode != 0 && tapereq->mode != 1) ) {
    serrno = ENOENT;
    return(-1);
  }
  if ( tpList == NULL ) return(0);
  CLIST_ITERATE_BEGIN(tpList,tpIterator) 
    {
      tl = tpIterator->tape;
      if ( (tl->tapereq.mode == tapereq->mode) &&
           (tl->tapereq.side == tapereq->side) &&
           (strcmp(tl->tapereq.vid,tapereq->vid) == 0) ) {
        found = 1;
        break;
      }
    }
  CLIST_ITERATE_END(tpList,tpIterator);
  if ( found == 1 && tp != NULL ) *tp = tpIterator;
  return(found);
}

static int findSegment(
                       filereq,
                       segment
                       )
     rtcpFileRequest_t *filereq;
     RtcpcldSegmentList_t **segment;
{
  RtcpcldTapeList_t *tpIterator;
  RtcpcldSegmentList_t *segmItem;
  file_list_t *fl;
  int found = 0;

  if ( filereq == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  if ( (filereq->tape_fseq <= 0) && 
       (memcmp(filereq->blockid,nullblkid,4) == 0) &&
       ((*filereq->file_path == '\0') || 
        (strcmp(filereq->file_path,".") == 0)) ) {
    serrno = ENOENT;
    return(-1);
  }

  if ( tpList == NULL || tpList->segments == NULL ) return(0);

  CLIST_ITERATE_BEGIN(tpList,tpIterator) 
    {
      CLIST_ITERATE_BEGIN(tpIterator->segments,segmItem) 
        {
          fl = segmItem->file;
          if ( (fl->filereq.tape_fseq > 0) ||
               (memcmp(fl->filereq.blockid,nullblkid,4) != 0) ||
               ((*fl->filereq.file_path != '\0') &&
                (strcmp(fl->filereq.file_path,".") != 0)) ) {
            /*
             * Either the tape position MUST correspond ...
             */
            if ( ((fl->filereq.tape_fseq > 0) && 
                  (fl->filereq.tape_fseq == filereq->tape_fseq)) ||
                 ((memcmp(fl->filereq.blockid,nullblkid,4) != 0) &&
                  (memcmp(fl->filereq.blockid,filereq->blockid,4) == 0)) ) {
              found = 1;
              break;
            } else if ( (fl->filereq.tape_fseq <= 0) &&
                        (memcmp(fl->filereq.blockid,nullblkid,4) == 0) &&
                        (strcmp(fl->filereq.file_path,filereq->file_path) == 0) ) {
              /*
               * ... or, the tape position is not known and the disk file paths match
               */
              found = 1;
              break;
            }
          }
        }
      CLIST_ITERATE_END(tpIterator->segments,segmItem);
      if ( found == 1 ) break;
    }
  CLIST_ITERATE_END(tpList,tpIterator);
  if ( found == 1 || segment != NULL ) *segment = segmItem;
  return(found);
}
static int lockTpList(
                      tape
                      )
     tape_list_t *tape;
{
  ID_TYPE key;
  struct Cdb_DbAddress_t *dbAddr;
  struct C_BaseAddress_t *baseAddr;
  struct C_IAddress_t *iAddr;
  struct C_Services_t *svcs;
  struct C_IObject_t *object;
  struct Cstager_Tape_t *tp;
  struct Cstager_Segment_t **tpSegmentArray = NULL;
  RtcpcldTapeList_t *tpIterator = NULL;
  RtcpcldSegmentList_t *segmIterator = NULL;
  char *vid;
  int rc, i, nbItems = 0, save_serrno, mode, side;

  rc = Cmutex_lock(&tpList);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "SYSCALL",
                    DLF_MSG_PARAM_STR,
                    "Cmutex_lock()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }
  
  if ( tpList == NULL ) {
    if ( tape == NULL ) {
      (void)Cmutex_unlock(&tpList);
      serrno = EINVAL;
      return(-1);
    }

    key = rtcpcld_getTapeKey();

    rc = getDbSvc(&svcs);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)Cmutex_unlock(&tpList);
      serrno = save_serrno;
      /* error already logged in getDbSvc */
      return(-1); 
    }
    
    rc = Cdb_DbAddress_create(key,"OraCnvSvc",SVC_ORACNV,&dbAddr);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)Cmutex_unlock(&tpList);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "Cdb_DbAddress_create()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(save_serrno),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    baseAddr = Cdb_DbAddress_getBaseAddress(dbAddr);
    iAddr = C_BaseAddress_getIAddress(baseAddr);
    
    rc = C_Services_createObj(svcs,iAddr,&object);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)Cmutex_unlock(&tpList);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_DBSVC,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+3,
                      "DBSVCCALL",
                      DLF_MSG_PARAM_STR,
                      "C_Services_createObj()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(save_serrno),
                      "DB_ERROR",
                      DLF_MSG_PARAM_STR,
                      C_Services_errorMsg(svcs),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    tp = Cstager_Tape_fromIObject(object);
    tpIterator = (RtcpcldTapeList_t *)calloc(1,sizeof(RtcpcldTapeList_t));
    if ( tpIterator == NULL ) {
      save_serrno = serrno;
      (void)Cmutex_unlock(&tpList);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "malloc()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(save_serrno),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    CLIST_INSERT(tpList,tpIterator);
    tpIterator->tp = tp;
    tpIterator->tape = tape;
    Cstager_Tape_status(tp,&tpIterator->oldStatus);
    /*
     * Cross check that we really got hold of the correct request
     */
    Cstager_Tape_vid(tp,(CONST char **)&vid);
    Cstager_Tape_tpmode(tp,&mode);
    Cstager_Tape_side(tp,&side);
    if ( (tape->tapereq.mode != mode) ||
         (tape->tapereq.side != side) ||
         (strcmp(tape->tapereq.vid,vid) != 0) ) {
      /*
       * This is really quite serious...
       */
      (void)Cmutex_unlock(&tpList);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ALERT,
                      RTCPCLD_MSG_WRONG_TAPE,
                      (struct Cns_fileid *)NULL,
                      2,
                      "VID",
                      DLF_MSG_PARAM_TPVID,
                      tape->tapereq.vid,
                      "DBENTRY",
                      DLF_MSG_PARAM_INT64,
                      (u_signed64)key
                      );
      serrno = SEINTERNAL;
      return(-1);
    }
  }
  return(0);
}

static int unlockTpList() 
{
  return(Cmutex_unlock(&tpList));
}

int rtcpcld_getVIDsToDo(
                        tapeArray, 
                        cnt
                        )
     tape_list_t ***tapeArray;
     int *cnt;
{
  struct Cstager_Tape_t **tpArray = NULL;
  struct Cstager_IStagerSvc_t *stgsvc = NULL;
  rtcpTapeRequest_t tapereq;
  char vmgr_error_buffer[512];
  struct vmgr_tape_info vmgr_tapeinfo;
  tape_list_t *tape;
  char *vid;
  int createNew, i, rc, nbItems = 0, save_serrno, mode;
  int side = 0;

  if ( tapeArray == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL ) return(-1);
  rc = Cstager_IStagerSvc_tapesToDo(
                                    stgsvc,
                                    &tpArray,
                                    &nbItems
                                    );
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_DBSVC,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+3,
                    "DBSVCCALL",
                    DLF_MSG_PARAM_STR,
                    "Cstager_IStagerSvc_tapesTodo()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    "DB_ERROR",
                    DLF_MSG_PARAM_STR,
                    Cstager_IStagerSvc_errorMsg(stgsvc),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }

  if ( nbItems <= 0 || tpArray == NULL ) return(0);
  *tapeArray = (tape_list_t **)calloc(nbItems,sizeof(tape_list_t *));
  if ( *tapeArray != NULL ) {
    vmgr_seterrbuf(vmgr_error_buffer,sizeof(vmgr_error_buffer));
    i=0;
    for (i=0; i<nbItems; i++) {
      tape = NULL;
      Cstager_Tape_vid(tpArray[i],(CONST char **)&vid);
      if ( vid == NULL ) continue;
      Cstager_Tape_tpmode(tpArray[i],&mode);
      tapereq.mode = mode;
      tapereq.side = 0;
      strncpy(tapereq.vid,vid,sizeof(tapereq.vid)-1);
      rc = findTape(tapereq,NULL);
      if ( rc == 1 ) continue;
      rc = rtcp_NewTapeList(&tape,NULL,mode);
      if ( rc == -1 ) return(-1);
      (*tapeArray)[i] = tape;
      strcpy(tape->tapereq.vid,vid);
      memset(&vmgr_tapeinfo,'\0',sizeof(vmgr_tapeinfo));
      rc = vmgr_querytape (
                           tape->tapereq.vid,
                           side,
                           &vmgr_tapeinfo,
                           tape->tapereq.dgn
                           );
      if ( rc == -1 ) {
        (void)dlf_write(
                        (inChild == 0 ? mainUuid : childUuid),
                        DLF_LVL_ERROR,
                        RTCPCLD_MSG_SYSCALL,
                        (struct Cns_fileid *)NULL,
                        RTCPCLD_NB_PARAMS+2,
                        "SYSCALL",
                        DLF_MSG_PARAM_STR,
                        "vmgr_querytape()",
                        "ERROR_STR",
                        DLF_MSG_PARAM_STR,
                        sstrerror(serrno),
                        RTCPCLD_LOG_WHERE
                        );
      }
      
      if ( ((vmgr_tapeinfo.status & DISABLED) == DISABLED) ||
           ((vmgr_tapeinfo.status & EXPORTED) == EXPORTED) ||
           ((vmgr_tapeinfo.status & ARCHIVED) == ARCHIVED) ) {
        errno = EACCES;
        return(-1);
      }
      if ( rc != -1 ) {
        strcpy(tape->tapereq.density, vmgr_tapeinfo.density);
        strcpy(tape->tapereq.label, vmgr_tapeinfo.lbltype);
        tape->tapereq.side = side;
      }
      Cstager_Tape_delete(tpArray[i]);
    };
    rc = 0;
    if ( cnt != NULL ) *cnt = nbItems;
  } else {
    serrno = errno;
    rc = -1;
  }
  return(0);
}

int rtcpcld_procReqsForVID(
                           tape,
                           keepLock
                           )
     tape_list_t *tape;
     int keepLock;
{
  struct Cstager_IStagerSvc_t *stgsvc = NULL;
  struct C_Services_t *svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Segment_t **segmArray = NULL;
  enum Cstager_SegmentStatusCodes_t cmpStatus;
  RtcpcldTapeList_t *tpIterator = NULL;
  RtcpcldSegmentList_t *segmIterator = NULL;
  rtcpFileRequest_t filereq;
  file_list_t *fl = NULL;
  tape_list_t *tl = NULL;
  char *vid, *diskPath;
  unsigned char *blockid;
  int rc, i, nbItems = 0, save_serrno, fseq, updated = 0;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL ) return(-1);

  rc = lockTpList(tape);
  if ( rc == -1 ) return(-1);

  rc = findTape(tape->tapereq,&tpIterator);
  if ( rc != 1 || tpIterator == NULL ) {
    (void)unlockTpList();
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "Tape request could not be found in internal list",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = SEINTERNAL;
    return(-1);
  }

  rc = Cstager_IStagerSvc_segmentsForTape(
                                          stgsvc,
                                          tpIterator->tp,
                                          &segmArray,
                                          &nbItems
                                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)unlockTpList();
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_DBSVC,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+3,
                    "DBSVCCALL",
                    DLF_MSG_PARAM_STR,
                    "Cstager_segmentsForTape()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    "DB_ERROR",
                    DLF_MSG_PARAM_STR,
                    Cstager_IStagerSvc_errorMsg(stgsvc),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }

  tl = tape;
  for ( i=0; i<nbItems; i++ ) {
    Cstager_Segment_status(segmArray[i],&cmpStatus);
    if ( (cmpStatus == SEGMENT_UNPROCESSED) ||
         (cmpStatus == SEGMENT_WAITFSEQ) ||
         (cmpStatus == SEGMENT_WAITPATH) ||
         (cmpStatus == SEGMENT_WAITCOPY) ) {
      Cstager_Segment_blockid(
                              segmArray[i],
                              (CONST unsigned char **)&blockid
                              );
      Cstager_Segment_fseq(
                           segmArray[i],
                           &fseq
                           );
      Cstager_Segment_diskPath(
                               segmArray[i],
                               (CONST char **)&diskPath
                               );
      if ( blockid != NULL ) memcpy(
                                    filereq.blockid,
                                    blockid,
                                    sizeof(filereq.blockid)
                                    );
      if ( diskPath != NULL ) strncpy(
                                     filereq.file_path,
                                     diskPath,
                                     sizeof(filereq.file_path)-1
                                     );
      filereq.tape_fseq = fseq;
      segmIterator = NULL;
      rc = findSegment(&filereq,&segmIterator);
      if ( rc != 1 || segmIterator == NULL || segmIterator->file == NULL ) {
        rc = rtcp_NewFileList(&tl,&fl,tl->tapereq.mode);
        if ( rc == -1 ) {
          save_serrno = serrno;
          (void)unlockTpList();
          (void)dlf_write(
                          (inChild == 0 ? mainUuid : childUuid),
                          DLF_LVL_ERROR,
                          RTCPCLD_MSG_SYSCALL,
                          (struct Cns_fileid *)NULL,
                          RTCPCLD_NB_PARAMS+2,
                          "SYSCALL",
                          DLF_MSG_PARAM_STR,
                          "rtcp_NewFileList()",
                          "ERROR_STR",
                          DLF_MSG_PARAM_STR,
                          sstrerror(save_serrno),
                          RTCPCLD_LOG_WHERE
                          );
          serrno = save_serrno;
          return(-1);
        }

        fl->filereq.concat = NOCONCAT;
        strcpy(fl->filereq.recfm,"F");
        fl->filereq.tape_fseq = fseq;
        fl->filereq.def_alloc = 0;
        fl->filereq.disk_fseq = ++(fl->prev->filereq.disk_fseq);
        if ( segmIterator == NULL ) {
          segmIterator = (RtcpcldSegmentList_t *)
            calloc(1,sizeof(RtcpcldSegmentList_t));
          if ( segmIterator == NULL ) {
            save_serrno = serrno;
            (void)unlockTpList();
            (void)dlf_write(
                            (inChild == 0 ? mainUuid : childUuid),
                            DLF_LVL_ERROR,
                            RTCPCLD_MSG_SYSCALL,
                            (struct Cns_fileid *)NULL,
                            RTCPCLD_NB_PARAMS+2,
                            "SYSCALL",
                            DLF_MSG_PARAM_STR,
                            "calloc()",
                            "ERROR_STR",
                            DLF_MSG_PARAM_STR,
                            sstrerror(save_serrno),
                            RTCPCLD_LOG_WHERE
                            );
            serrno = save_serrno;
            return(-1);
          }
          CLIST_INSERT(tpList->segments,segmIterator);
        }
        segmIterator->file = fl;
        segmIterator->tp = tpIterator;
        segmIterator->segment = segmArray[i];
      } else {
        fl = segmIterator->file;
      }
      memcpy(fl->filereq.blockid,blockid,sizeof(fl->filereq.blockid));
      if ( memcmp(fl->filereq.blockid,nullblkid,sizeof(nullblkid)) == 0 ) 
        fl->filereq.position_method = TPPOSIT_FSEQ;
      else fl->filereq.position_method = TPPOSIT_BLKID;
      fl->filereq.proc_status = RTCP_WAITING;
      strcpy(fl->filereq.file_path,".");
      Cstager_Segment_diskPath(segmArray[i],(CONST char **)&diskPath);
      if ( (diskPath == NULL) && (*diskPath != '.') && 
           (*diskPath != '\0') ) {
        strcpy(fl->filereq.file_path,diskPath);          
      }
      if ( ((fseq > 0) || 
            ((blockid != NULL) && 
             (memcmp(blockid,nullblkid,sizeof(nullblkid)) != 0))) &&
           ((diskPath != NULL) && (*diskPath != '\0') &&
            (*diskPath != '.')) ) {
        if ( cmpStatus != SEGMENT_WAITCOPY ) {
          updated = 1;
          Cstager_Segment_setStatus(segmArray[i],SEGMENT_WAITCOPY);
        }
      } else if ( ((fseq > 0) ||
                   ((blockid != NULL) &&
                    (memcmp(blockid,nullblkid,sizeof(nullblkid)) != 0))) ) {
        if ( cmpStatus != SEGMENT_WAITPATH ) {
          updated = 1;
          Cstager_Segment_setStatus(segmArray[i],SEGMENT_WAITPATH);
        }
      } else {
        if ( cmpStatus != SEGMENT_WAITFSEQ ) {
          updated = 1;
          Cstager_Segment_setStatus(segmArray[i],SEGMENT_WAITFSEQ);
        }
      }
    }
  }
  if ( updated == 1 ) {
    rc = getDbSvc(&svcs);
    if ( rc == -1 || svcs == NULL ) return(-1);
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)unlockTpList();
      serrno = save_serrno;
      return(-1);
    }
    iAddr = C_BaseAddress_getIAddress(baseAddr);
    iObj = Cstager_Tape_getIObject(tpIterator->tp);
    rc = C_Services_updateObj(svcs,iAddr,iObj);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)unlockTpList();
      C_IAddress_delete(iAddr);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_DBSVC,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+3,
                      "DBSVCCALL",
                      DLF_MSG_PARAM_STR,
                      "C_Services_updateObj()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      "DB_ERROR",
                      DLF_MSG_PARAM_STR,
                      C_Services_errorMsg(svcs),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    C_IAddress_delete(iAddr);
  }
  if ( keepLock == 0 ) (void)unlockTpList();
  return(0);
}

int rtcpcld_getReqsForVID(
                          tape
                          )
     tape_list_t *tape;
{
  return(rtcpcld_procReqsForVID(tape,0));
}


int rtcpcld_anyReqsForVID(
                          tape
                          )
     tape_list_t *tape;
{
  struct Cstager_IStagerSvc_t *stgsvc = NULL;
  RtcpcldTapeList_t *tpItem = NULL;
  int rc, nbItems, save_serrno;
  tape_list_t *tl;
  file_list_t *fl;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL ) return(-1);

  rc = lockTpList(tape);
  if ( rc == -1 ) return(-1);

  rc = findTape(tape->tapereq,&tpItem);
  if ( rc != 1 || tpItem == NULL ) {
    (void)unlockTpList();
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "Tape request could not be found in internal list",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = SEINTERNAL;
    return(-1);
  }

  rc = Cstager_IStagerSvc_anySegmentsForTape(stgsvc,tpItem->tp);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)unlockTpList();
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_DBSVC,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+3,
                    "DBSVCCALL",
                    DLF_MSG_PARAM_STR,
                    "Cstager_anySegmentsForTape()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    "DB_ERROR",
                    DLF_MSG_PARAM_STR,
                    Cstager_IStagerSvc_errorMsg(stgsvc),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }
  (void)unlockTpList();

  nbItems = rc;
  tl = tape;
  fl = tl->file;
  if ( fl == NULL ) {
    rc = rtcp_NewFileList(&tl,&fl,tl->tapereq.mode);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_SYSCALL,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+2,
                      "SYSCALL",
                      DLF_MSG_PARAM_STR,
                      "rtcp_NewFileList()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(save_serrno),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    
    fl->filereq.concat = NOCONCAT;
    strcpy(fl->filereq.recfm,"F");
    fl->filereq.proc_status = RTCP_REQUEST_MORE_WORK;
  }

  return(0);
}

int rtcpcld_updateVIDStatus(
                            tape, 
                            fromStatus,
                            toStatus
                            )
     tape_list_t *tape;
     enum Cstager_TapeStatusCodes_t fromStatus;
     enum Cstager_TapeStatusCodes_t toStatus;
{
  struct C_Services_t *svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  RtcpcldTapeList_t *tapeItem;
  enum Cstager_TapeStatusCodes_t cmpStatus;  
  int rc = 0, save_serrno;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL ) return(-1);

  rc = lockTpList(tape);
  if ( rc == -1 ) return(-1);

  rc = findTape(tape->tapereq,&tapeItem);
  if ( rc != 1 || tapeItem == NULL ) {
    (void)unlockTpList();
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "Tape request could not be found in internal list",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = SEINTERNAL;
    return(-1);
  }
  
  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)unlockTpList();
    serrno = save_serrno;
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);
  iObj = Cstager_Tape_getIObject(tapeItem->tp);
  rc = C_Services_updateObj(svcs,iAddr,iObj);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)unlockTpList();
    C_IAddress_delete(iAddr);
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_DBSVC,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+3,
                    "DBSVCCALL",
                    DLF_MSG_PARAM_STR,
                    "C_Services_updateObj()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    "DB_ERROR",
                    DLF_MSG_PARAM_STR,
                    C_Services_errorMsg(svcs),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }
  Cstager_Tape_status(tapeItem->tp,&cmpStatus);
  if ( fromStatus == cmpStatus ) {
    Cstager_Tape_setStatus(tapeItem->tp,toStatus);
    rc = C_Services_updateRep(svcs,iAddr,iObj,1);
    if ( rc == -1 ) {
      save_serrno = serrno;
      C_IAddress_delete(iAddr);
      (void)unlockTpList();
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_DBSVC,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+3,
                      "DBSVCCALL",
                      DLF_MSG_PARAM_STR,
                      "C_Services_updateRep()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      "DB_ERROR",
                      DLF_MSG_PARAM_STR,
                      C_Services_errorMsg(svcs),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
  }
  C_IAddress_delete(iAddr);
  (void)unlockTpList();
  return(0);
}

int rtcpcld_updateVIDFileStatus(
                                tape, 
                                fromStatus, 
                                toStatus
                                )
     tape_list_t *tape;
     enum Cstager_SegmentStatusCodes_t fromStatus;
     enum Cstager_SegmentStatusCodes_t toStatus;
{
  struct C_Services_t *svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  RtcpcldTapeList_t *tapeItem;
  RtcpcldSegmentList_t *segmItem;
  enum Cstager_SegmentStatusCodes_t cmpStatus;  
  int rc = 0, updated = 0, save_serrno;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL ) return(-1);

  rc = lockTpList(tape);
  if ( rc == -1 ) return(-1);
  
  rc = findTape(tape->tapereq,&tapeItem);
  if ( rc != 1 || tapeItem == NULL ) {
    (void)unlockTpList();
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_INTERNAL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "Tape request could not be found in internal list",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = SEINTERNAL;
    return(-1);
  }

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)unlockTpList();
    serrno = save_serrno;
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);
  iObj = Cstager_Tape_getIObject(tapeItem->tp);
  rc = C_Services_updateObj(svcs,iAddr,iObj);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)unlockTpList();
    C_IAddress_delete(iAddr);
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_DBSVC,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+3,
                    "DBSVCCALL",
                    DLF_MSG_PARAM_STR,
                    "C_Services_updateObj()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    "DB_ERROR",
                    DLF_MSG_PARAM_STR,
                    C_Services_errorMsg(svcs),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }
  CLIST_ITERATE_BEGIN(tapeItem->segments,segmItem) 
    {
      Cstager_Segment_status(segmItem->segment,&cmpStatus);
      if ( cmpStatus == fromStatus ) {
        Cstager_Segment_setStatus(segmItem->segment,toStatus);
        updated = 1;
      }
    }
  CLIST_ITERATE_END(tapeItem->segments,segmItem);
  if ( updated == 1 ) {
    rc = C_Services_updateRep(svcs,iAddr,iObj,1);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)unlockTpList();
      C_IAddress_delete(iAddr);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_DBSVC,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+3,
                      "DBSVCCALL",
                      DLF_MSG_PARAM_STR,
                      "C_Services_updateRep()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      "DB_ERROR",
                      DLF_MSG_PARAM_STR,
                      C_Services_errorMsg(svcs),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    C_IAddress_delete(iAddr);
  }
  (void)unlockTpList();
  return(0);
}

int rtcpcld_setFileStatus(
                          filereq, 
                          newStatus
                          )
     rtcpFileRequest_t *filereq;
     enum Cstager_SegmentStatusCodes_t newStatus;
{
  struct C_Services_t *svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  RtcpcldTapeList_t *tapeItem;
  RtcpcldSegmentList_t *segmItem;
  enum Cstager_SegmentStatusCodes_t cmpStatus;
  char *diskPath;
  int rc = 0, updated = 0, save_serrno;

  if ( filereq == NULL || 
       *filereq->file_path == '\0' || 
       *filereq->file_path == '.' ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL ) return(-1);

  rc = lockTpList(NULL);
  if ( rc == -1 ) return(-1);
  
  CLIST_ITERATE_BEGIN(tpList,tapeItem) 
    {
      CLIST_ITERATE_BEGIN(tapeItem->segments,segmItem) 
        {
          diskPath = NULL;
          Cstager_Segment_diskPath(segmItem->segment,(CONST char **)&diskPath);
          if ( (diskPath != NULL) && 
               (strcmp(filereq->file_path,diskPath) == 0) ) {
            Cstager_Segment_setStatus(segmItem->segment,newStatus);
            updated = 1;
            break;
          }
        }
      CLIST_ITERATE_END(tapeItem->segments,segmItem);
      if ( updated == 1 ) break;
    }
  CLIST_ITERATE_END(tpList,tapeItem);
  if ( updated == 1 ) {
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)unlockTpList();
      serrno = save_serrno;
      return(-1);
    }
    iAddr = C_BaseAddress_getIAddress(baseAddr);
    iObj = Cstager_Segment_getIObject(segmItem->segment);
    rc = C_Services_updateRep(svcs,iAddr,iObj,1);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)unlockTpList();
      C_IAddress_delete(iAddr);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      DLF_LVL_ERROR,
                      RTCPCLD_MSG_DBSVC,
                      (struct Cns_fileid *)NULL,
                      RTCPCLD_NB_PARAMS+3,
                      "DBSVCCALL",
                      DLF_MSG_PARAM_STR,
                      "Cstager_anySegmentsForTape()",
                      "ERROR_STR",
                      DLF_MSG_PARAM_STR,
                      sstrerror(serrno),
                      "DB_ERROR",
                      DLF_MSG_PARAM_STR,
                      C_Services_errorMsg(svcs),
                      RTCPCLD_LOG_WHERE
                      );
      serrno = save_serrno;
      return(-1);
    }
    C_IAddress_delete(iAddr);
  }

  (void)unlockTpList();
  return(0);
}

int rtcpcld_getPhysicalPath(
                            tapereq, 
                            filereq
                            )
     rtcpTapeRequest_t *tapereq;
     rtcpFileRequest_t *filereq;
{
  struct C_Services_t *svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  RtcpcldTapeList_t *tapeItem;
  RtcpcldSegmentList_t *segmItem;
  enum Cstager_SegmentStatusCodes_t cmpStatus;
  char *diskPath;
  unsigned char *blockid;
  int rc = 0, updated = 0, found = 0, waiting = 0, save_serrno, fseq, blkidSize;
  tape_list_t tape;
  file_list_t *file, *fl;

  if ( tapereq == NULL || filereq == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  /*
   * Disk path already assigned ?
   */
  if ( *filereq->file_path != '\0' && *filereq->file_path != '.' ) return(0);

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL ) return(-1);

  tape.tapereq = *tapereq;
  rc = rtcpcld_procReqsForVID(&tape,1);
  if ( rc == -1 ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    DLF_LVL_ERROR,
                    RTCPCLD_MSG_SYSCALL,
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "SYSCALL",
                    DLF_MSG_PARAM_STR,
                    "rtcpcld_checkReqsForVID()",
                    "ERROR_STR",
                    DLF_MSG_PARAM_STR,
                    sstrerror(serrno),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }

  fl = file = NULL;
  found = findSegment(filereq,&segmItem);
  if ( (found == 1) && (segmItem != NULL) && (segmItem->file != NULL) ) {
    fl = segmItem->file;
    if ( fl->filereq.proc_status == RTCP_WAITING ) {
      waiting = 1;
      if ( (*fl->filereq.file_path == '\0') ||
           (*fl->filereq.file_path == '.') ) {
        found = 0;
      }
    }
  }
  if ( found != 1 ) {
    /*
     * Nothing found yet. Search all segments to find one waiting with a path
     */
    file = segmItem->file;
    if ( file == NULL ) {
      rc = findTape(tapereq,&tapeItem);
      if ( (tapeItem != NULL) && (tapeItem->tape != NULL) )
        file = tapeItem->tape->file;
    }
    CLIST_ITERATE_BEGIN(file,fl) 
      {
        if ( (fl->filereq.proc_status == RTCP_WAITING) ) {
          waiting = 1;
          if ( (*fl->filereq.file_path != '\0') &&
                (*fl->filereq.file_path != '.') ) {
            found = 1;
            break;
          }
        }
      }
    CLIST_ITERATE_END(file,fl);
  }

  if ( (found == 0) && (waiting == 1) ) {
    /*
     * Nothing found yet. Return an error indicating it's worth doing a retry.
     */
    (void)unlockTpList();
    serrno = EAGAIN;
    return(-1);
  }
  if ( (fl == NULL) || ((found == 0) && (waiting == 0)) ) {
    (void)unlockTpList();
    serrno = ENOENT;
    return(-1);
  }
  *filereq = fl->filereq;
  (void)unlockTpList();

  return(0);
}
