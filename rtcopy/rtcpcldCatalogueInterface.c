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
 * @(#)$RCSfile: rtcpcldCatalogueInterface.c,v $ $Revision: 1.84 $ $Release$ $Date: 2004/11/26 13:58:56 $ $Author: obarring $
 *
 * 
 *
 * @author Olof Barring
 *****************************************************************************/


#ifndef lint
static char sccsid[] = "@(#)$RCSfile: rtcpcldCatalogueInterface.c,v $ $Revision: 1.84 $ $Release$ $Date: 2004/11/26 13:58:56 $ Olof Barring";
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
#include <unistd.h>
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
#include <Cmutex.h>
#include <u64subr.h>
#include <Cglobals.h>
#include <serrno.h>
#if defined(VMGR)
#include <vmgr_api.h>
#endif /* VMGR */
#include <Ctape_constants.h>
#include <castor/stager/Tape.h>
#include <castor/stager/Segment.h>
#include <castor/stager/Stream.h>
#include <castor/stager/TapeCopy.h>
#include <castor/stager/TapeCopyForMigration.h>
#include <castor/stager/DiskCopy.h>
#include <castor/stager/DiskCopyForRecall.h>
#include <castor/stager/DiskCopyStatusCodes.h>
#include <castor/stager/CastorFile.h>
#include <castor/stager/TapePool.h>
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
#include <Cthread_api.h>
#include <vdqm_api.h>
#include <rtcp.h>
#include <rtcp_api.h>
#include <rtcpcld_constants.h>
#include <rtcpcld_messages.h>
#include <rtcpcld.h>

/** Global needed for determine which uuid to log
 */
int inChild;
/** uuid's set by calling process (rtcpclientd:mainUuid, VidWorker:childUuid)
 */
Cuuid_t childUuid, mainUuid;

static unsigned char nullblkid[4] = {'\0', '\0', '\0', '\0'};

/**
 * Create/get the DB services. The call is wrapped in this routine to
 * avoid repeating the same log statment upon failure.
 *
 * \note Thread safeness and concurrent access is managed by C_Services
 */
static int getDbSvc(
                    dbSvc
                    )
     struct C_Services_t ***dbSvc;
{
  static int svcsKey = -1;
  struct C_Services_t **svc;
  int rc, save_serrno;

  if ( dbSvc == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  svc = NULL;
  rc = Cglobals_get(&svcsKey,(void **)&svc,sizeof(struct C_Services_t **));
  if ( rc == -1 || svc == NULL ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_SYSCALL),
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
      LOG_DBCALL_ERR("C_Services_create()","(null)");
      serrno = save_serrno;
      return(-1);
    }
  }
  *dbSvc = svc;
  
  return(0);
}

/**
 * Create/get the stager DB services. The call is wrapped in this routine to
 * avoid repeating the same log statment upon failure.
 *
 * \note Thread safeness and concurrent access is managed by Cstager_IStagerSvc
 */
static int getStgSvc(
                     stgSvc
                     )
     struct Cstager_IStagerSvc_t ***stgSvc;
{
  struct C_Services_t **svcs = NULL;
  struct C_IService_t *iSvc = NULL;
  static int svcsKey = -1;
  struct Cstager_IStagerSvc_t **svc;
  int rc, save_serrno;

  if ( stgSvc == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  svc = NULL;
  rc = Cglobals_get(&svcsKey,(void **)&svc,sizeof(struct Cstager_IStagerSvc_t **));
  if ( rc == -1 || svc == NULL ) {
    save_serrno = serrno;
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_SYSCALL),
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
    rc = getDbSvc(&svcs);
    if ( rc == -1 ) return(-1);
    
    rc = C_Services_service(*svcs,"OraStagerSvc",SVC_ORASTAGERSVC, &iSvc);
    if ( rc == -1 || iSvc == NULL ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_service()",
                     C_Services_errorMsg(*svcs));
      serrno = save_serrno;
      return(-1);
    }
    *svc = Cstager_IStagerSvc_fromIService(iSvc);
  }
  *stgSvc = svc;
  return(0);
}

/**
 * Update the memory copy of the tape and all attached segments from the database.
 */
static int updateTapeFromDB(
                            tape
                            )
     tape_list_t *tape;
{
  struct Cdb_DbAddress_t *dbAddr;
  struct C_Services_t **svcs = NULL;
  struct Cstager_IStagerSvc_t **stgSvc = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Tape_t *tp;
  int rc = 0, save_serrno;
  ID_TYPE key;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = getStgSvc(&stgSvc);
  if ( rc == -1 || stgSvc == NULL || *stgSvc == NULL ) return(-1);

  if ( tape->dbRef == NULL ) {
    tape->dbRef = (RtcpDBRef_t *)calloc(1,sizeof(RtcpDBRef_t));
    if ( tape->dbRef == NULL ) {
      serrno = errno;
      LOG_SYSCALL_ERR("calloc()");
      return(-1);
    }
  }
  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  key = tape->dbRef->key;

  if ( (tp == NULL) && (key != 0) ) {
    rc = Cdb_DbAddress_create(key,"OraCnvSvc",SVC_ORACNV,&dbAddr);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      RTCPCLD_LOG_MSG(RTCPCLD_MSG_SYSCALL),
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
  } else {
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) return(-1);
    if ( tp != NULL ) Cstager_Tape_id(tp,&key);
  }

  iAddr = C_BaseAddress_getIAddress(baseAddr);
  if ( (tp == NULL) && (key != 0) ) {
    rc = C_Services_createObj(*svcs,iAddr,&iObj);
    if ( rc == 0 ) tp = Cstager_Tape_fromIObject(iObj);
  } else if ( tp != NULL ) { 
    iObj = Cstager_Tape_getIObject(tp);
    rc = C_Services_updateObj(*svcs,iAddr,iObj);
  } else {
    rc = Cstager_IStagerSvc_selectTape(
                                       *stgSvc,
                                       &tp,
                                       tape->tapereq.vid,
                                       tape->tapereq.side,
                                       tape->tapereq.mode
                                       );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("Cstager_IStagerSvc_selectTape()",
                     Cstager_IStagerSvc_errorMsg(*stgSvc));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
    /*
     * selectTape() locks the Tape in DB. We commit immediately since
     * we don't intend to update the DB here.
     */
    (void)C_Services_commit(*svcs,iAddr);
  }

  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR((tp == NULL ? "C_Services_createObj()" : "C_Services_updateObj()"),
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  Cstager_Tape_id(tp,&key);
  tape->dbRef->key = key;
  tape->dbRef->row = (void *)tp;
  if ( tape->tapereq.mode == WRITE_ENABLE ) {
    iObj = Cstager_Tape_getIObject(tp);
    /*
     * Get the stream if any
     */
    rc = C_Services_fillObj(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_Stream
                            );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_fillObj()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
  }
  C_IAddress_delete(iAddr);
  return(0);
}

int rtcpcld_updateTapeFromDB(
                             tape
                             )
     tape_list_t *tape;
{
  return(updateTapeFromDB(tape));
}

/**
 * Update the memory copy of the segment from the database.
 */
static int updateSegmentFromDB(
                               file
                               )
     file_list_t *file;
{
  struct Cdb_DbAddress_t *dbAddr;
  struct C_Services_t **svcs = NULL;
  struct Cstager_IStagerSvc_t **stgSvc = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Segment_t *segm;
  int rc = 0, save_serrno;
  ID_TYPE key = 0;

  if ( (file == NULL) || (file->dbRef == NULL) ||
         ((file->dbRef->row == NULL) && (file->dbRef->key == 0)) ) {
    serrno = EINVAL;
    return(-1);
  }
  
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = getStgSvc(&stgSvc);
  if ( rc == -1 || stgSvc == NULL || *stgSvc == NULL  ) return(-1);

  key = file->dbRef->key;
  segm = (struct Cstager_Segment_t *)file->dbRef->row;

  if ( (segm == NULL) && (key != 0) ) {
    rc = Cdb_DbAddress_create(key,"OraCnvSvc",SVC_ORACNV,&dbAddr);
    if ( rc == -1 ) {
      save_serrno = serrno;
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      RTCPCLD_LOG_MSG(RTCPCLD_MSG_SYSCALL),
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
  } else {
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) return(-1);
    if ( segm != NULL ) Cstager_Segment_id(segm,&key);
  }

  iAddr = C_BaseAddress_getIAddress(baseAddr);
  iObj = Cstager_Segment_getIObject(segm);

  rc = C_Services_updateObj(*svcs,iAddr,iObj);  
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_updateObj()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  file->dbRef->row = (void *)segm;
  file->dbRef->key = key;
  C_IAddress_delete(iAddr);
  return(0);
}

int rtcpcld_updateSegmentFromDB(
                                file
                                )
     file_list_t *file;
{
  return(updateSegmentFromDB(file));
}

/**
 * Verify that tape really match the requested tape entry. If
 * the tape doesn't match it means that the rtcpclientd or somebody
 * else has started the VidWorker with an incorrect database key specified
 * using the -k option.
 */
static int verifyTape(
                      tape
                      )
     tape_list_t *tape;
{
  char *vid;
  int rc, mode, side;
  struct Cstager_Tape_t *tp = NULL;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  
  if ( (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
    rc = updateTapeFromDB(tape);
    if ( rc == -1 ) return(-1);
  }
  
  if ( (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
    serrno = ENOENT;
    return(-1);
  }

  tp = tape->dbRef->row;
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
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_WRONG_TAPE),
                    (struct Cns_fileid *)NULL,
                    2,
                    "VID",
                    DLF_MSG_PARAM_TPVID,
                    tape->tapereq.vid,
                    "DBENTRY",
                    DLF_MSG_PARAM_INT64,
                    tape->dbRef->key
                    );
    serrno = SEINTERNAL;
    return(-1);
  }
  return(0);
}

/**
 * Externalised version of getDbSvc(). Used in rtcpcldapi.c
 */
int rtcpcld_getDbSvc(
                     svcs
                     )
     struct C_Services_t ***svcs;
{
  return(getDbSvc(svcs));
}

/**
 * Externalised version of getStgSvc(). Used in rtcpcldapi.c
 */
int rtcpcld_getStgSvc(
                     stgSvc
                     )
     struct Cstager_IStagerSvc_t ***stgSvc;
{
  return(getStgSvc(stgSvc));
}

/**
 * This method is only called from methods used by the rtcpclientd parent.
 * Called by rtcpclientd to find out work to do. The internal active tapes
 * list is updated with new entries if any. It is the caller's responsability
 * to free the returned tapeArray and all its tape_list_t members.
 */
int rtcpcld_getTapesToDo(
                         tapeArray, 
                         cnt
                         )
     tape_list_t ***tapeArray;
     int *cnt;
{
  struct C_IObject_t *iObj = NULL;
  struct C_IAddress_t *iAddr;
  struct C_BaseAddress_t *baseAddr;
  struct Cstager_Tape_t **tpArray = NULL, *tp;
  struct Cstager_Segment_t *segm;
  struct Cstager_Stream_t **streamArray = NULL;
  struct Cstager_IStagerSvc_t **stgsvc = NULL;
  struct C_Services_t **dbSvc;
  struct Cstager_TapePool_t *tapePool;
  rtcpTapeRequest_t *tapereq;
  tape_list_t *tmpTapeArray, *tape = NULL, *tl;
  char *vid, *tapePoolName;
  int i, rc = 0, nbTpItems = 0, nbStreamItems = 0, nbItems = 0;
  int save_serrno, mode, side = 0;
  ID_TYPE key = 0;
  u_signed64 sizeToTransfer = 0;

  if ( (tapeArray == NULL) || (cnt == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }

  *cnt = 0;

  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL || *stgsvc == NULL) return(-1);
  rc = Cstager_IStagerSvc_tapesToDo(
                                    *stgsvc,
                                    &tpArray,
                                    &nbTpItems
                                    );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("Cstager_IStagerSvc_tapesToDo()",
                   Cstager_IStagerSvc_errorMsg(*stgsvc));
    serrno = save_serrno;
    return(-1);
  } else if ( nbTpItems > 0 ) {
    for (i=0; i<nbTpItems; i++) {
      tl = NULL;
      Cstager_Tape_vid(tpArray[i],(CONST char **)&vid);
      if ( vid == NULL ) continue;
      Cstager_Tape_tpmode(tpArray[i],&mode);
      Cstager_Tape_side(tpArray[i],&side);
      rc = rtcp_NewTapeList(&tape,&tl,mode);
      if ( rc == -1 ) break;
      tl->dbRef = (RtcpDBRef_t *)calloc(1,sizeof(RtcpDBRef_t *));
      if ( tl->dbRef == NULL ) {
        save_serrno = errno;
        LOG_SYSCALL_ERR("calloc()");
        CLIST_DELETE(tape,tl);
        free(tl);
        continue;
      }
      Cstager_Tape_id(tpArray[i],&(tl->dbRef->key));
      tl->dbRef->row = (void *)tpArray[i];
      tapereq = &(tl->tapereq);
      strncpy(tapereq->vid,vid,sizeof(tapereq->vid)-1);
      tapereq->mode = mode;
      tapereq->side = side;
      rc = rtcpcld_tapeOK(tl);
      if ( rc == -1 ) {
        save_serrno = errno;
        LOG_SYSCALL_ERR("rtcpcld_tapeOK()");
        CLIST_DELETE(tape,tl);
        free(tl);
        continue;
      }
    }
    free(tpArray);
  }

  /*
   * Now do the streams. For each stream we need to call
   * vmgr_gettape() to get the VID to be used for the stream.
   */
  rc = Cstager_IStagerSvc_streamsToDo(
                                      *stgsvc,
                                      &streamArray,
                                      &nbStreamItems
                                      );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("Cstager_IStagerSvc_streamsToDo()",
                   Cstager_IStagerSvc_errorMsg(*stgsvc));
    serrno = save_serrno;
  } else if ( nbStreamItems > 0 ) {
    iAddr = NULL;
    rc = getDbSvc(&dbSvc);
    if ( rc != -1 ) {
      rc = C_BaseAddress_create("OraCnvSvc", SVC_ORACNV, &baseAddr);
      if ( rc != -1 ) iAddr = C_BaseAddress_getIAddress(baseAddr);
    }
    for (i=0; i<nbStreamItems; i++) {
      Cstager_Stream_tapePool(streamArray[i],&tapePool);
      if ( tapePool == NULL ) {
        /*
         * Try with a fillObj(), just in case
         */
        iObj = Cstager_Stream_getIObject(streamArray[i]);
        if ( rc != -1 ) rc = C_Services_fillObj(
                                                *dbSvc,
                                                iAddr,
                                                iObj,
                                                OBJ_TapePool
                                                );
        Cstager_Stream_tapePool(streamArray[i],&tapePool);
      }
      if ( tapePool == NULL ) {
        Cstager_Stream_id(streamArray[i],&key);
        (void)dlf_write(
                        (inChild == 0 ? mainUuid : childUuid),
                        RTCPCLD_LOG_MSG(RTCPCLD_MSG_NOTAPEPOOL),
                        (struct Cns_fileid *)NULL,
                        RTCPCLD_NB_PARAMS+1,
                        "DBKEY",
                        DLF_MSG_PARAM_INT64,
                        key,
                        RTCPCLD_LOG_WHERE
                        );
        continue;
      }
      Cstager_Stream_initialSizeToTransfer(streamArray[i],&sizeToTransfer);
      Cstager_TapePool_name(tapePool,(CONST char **)&tapePoolName);
      tl = NULL;
      rc = rtcpcld_gettape(
                           tapePoolName,
                           sizeToTransfer,
                           &tl
                           );
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_SYSCALL_ERR("rtcpcld_gettape()");
        if ( tl != NULL ) free(tl);
        continue;
      }
      /*
       * OK, we got the tape. The tape is now BUSY in VMGR.
       * Need to create the Tape in DB to facilitate the cleanup
       * in case something goes wrong.
       */
      rc = updateTapeFromDB(tl);
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_SYSCALL_ERR("updateTapeFromDB()");
        (void)rtcpcld_updateTape(tl,NULL,1,0);
        if ( tl != NULL ) free(tl);
        continue;
      }
      rc = rtcpcld_tapeOK(tl);
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_SYSCALL_ERR("rtcpcld_tapeOK()");
        (void)rtcpcld_updateTape(tl,NULL,1,0);
        if ( tl != NULL ) free(tl);
        continue;
      }
      /*
       * Connect the tape to this stream
       */
      tp = (struct Cstager_Tape_t *)tl->dbRef->row;
      Cstager_Stream_setTape(streamArray[i],tp);
      Cstager_Tape_setStream(tp,streamArray[i]);
      iObj = Cstager_Stream_getIObject(streamArray[i]);
      rc = C_Services_fillRep(
                              *dbSvc,
                              iAddr,
                              iObj,
                              OBJ_Tape,
                              1
                              );
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_DBCALL_ERR("C_Services_fillRep()",
                       C_Services_errorMsg(*dbSvc));
        (void)rtcpcld_updateTape(tl,NULL,1,0);
        if ( tl != NULL ) free(tl);
        continue;
      }
      /*
       * We should set the tape status to reflect the status of the stream.
       * This is not only for informative purpose but also to avoid that
       * the tape is returned in a tapesToDo().
       */
      Cstager_Tape_setStatus(tp,TAPE_WAITDRIVE);
      iObj = Cstager_Tape_getIObject(tp);
      rc = C_Services_updateRep(
                                *dbSvc,
                                iAddr,
                                iObj,
                                1
                                );
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_SYSCALL_ERR("C_Services_updateRep()");
        (void)rtcpcld_updateTape(tl,NULL,1,0);
        if ( tl != NULL ) free(tl);
        continue;
      }
      CLIST_INSERT(tape,tl);
    }
    if ( iAddr != NULL ) C_IAddress_delete(iAddr);

    free(streamArray);
  }

  /*
   * Count the number of usable tape request we found
   */
  nbItems = 0;
  CLIST_ITERATE_BEGIN(tape,tl) 
    {
      nbItems++;
    }
  CLIST_ITERATE_END(tape,tl);

  if ( nbItems > 0 ) {
    *tapeArray = (tape_list_t **)calloc(nbItems,sizeof(tape_list_t *));
    if ( *tapeArray == NULL ) {
      save_serrno = errno;
      LOG_SYSCALL_ERR("Cthread_mutex_lock(currentTapeFseq)");
      rc = -1;
    }
    
    i = 0;
    while ( (tl = tape) != NULL ) {
      CLIST_DELETE(tape,tl);
      (*tapeArray)[i++] = tl;
    }
  }

  if ( rc == -1 ) {
    serrno = save_serrno;
  } else {
    *cnt = nbItems;
  }
  return(rc);
}

void rtcpcld_cleanupTape(
                         tape
                         )
     tape_list_t *tape;
{
  struct Cstager_Tape_t *tp;
  struct Cstager_Segment_t *segm;
  file_list_t *file;
  
  if ( tape == NULL ) return;

  while ( (file = tape->file) != NULL ) {
    if ( file->dbRef != NULL ) {
      segm = (struct Cstager_Segment_t *)file->dbRef->row;
      if ( segm != NULL ) Cstager_Segment_delete(segm);
      free(file->dbRef);
    }
    CLIST_DELETE(tape->file,file);
    free(file);
  }
  if ( tape->dbRef != NULL ) {
    tp = (struct Cstager_Tape_t *)tape->dbRef->row;
    if ( tp != NULL ) Cstager_Tape_delete(tp);
    free(tape->dbRef);
  }
  free(tape);
  return;
}

void rtcpcld_cleanupFile(
                         file
                         )
     file_list_t *file;
{
  struct Cstager_Segment_t *segment = NULL;
  struct Cstager_TapeCopy_t *tapeCopy = NULL;
  struct Cstager_CastorFile_t *castorFile = NULL;
  struct Cstager_DiskCopy_t **diskCopyArray = NULL;
  int i, nbCopies = 0;

  if ( file == NULL ) return;
  if ( file->dbRef != NULL ) {
    segment = (struct Cstager_Segment_t *)file->dbRef->row;
    if ( segment != NULL ) {
      Cstager_Segment_copy(segment,&tapeCopy);
      if ( tapeCopy != NULL ) {
        Cstager_TapeCopy_castorFile(tapeCopy,&castorFile);
        if ( castorFile != NULL ) {
          Cstager_CastorFile_diskCopies(
                                        castorFile,
                                        &diskCopyArray,
                                        &nbCopies
                                        );
          for ( i=0; i<nbCopies; i++ ) {
            Cstager_DiskCopy_delete(diskCopyArray[i]);
          }
          if ( diskCopyArray != NULL ) free(diskCopyArray);
          /*
           * Destroying the castor file will also destroy
           * all the attached tape copies
           */
          Cstager_CastorFile_delete(castorFile);
        }
      }
    }
    free(file->dbRef);
  }
  CLIST_DELETE(file->tape->file,file);
  free(file);
  return;
}  

static int validPosition(
                         segment
                         )
     struct Cstager_Segment_t *segment;
{                         
  int fseq;
  unsigned char *blockid;

  Cstager_Segment_fseq(segment,&fseq);
  Cstager_Segment_blockid(segment,(CONST unsigned char **)&blockid);

  if ( (fseq <= 0) &&
       (blockid == NULL || memcmp(blockid,nullblkid,sizeof(nullblkid)) == 0) ) {
    return(0);
  } else {
    return(1);
  }
}

static int validPath(
                     segment
                     )
     struct Cstager_Segment_t *segment;
{
  char *path = NULL;

  if ( (path == NULL) || (*path == '\0') || (strcmp(path,".") == 0) ) {
    return(0);
  } else {
    return(1);
  }
}

static int validSegment(
                        segment
                        )
     struct Cstager_Segment_t *segment;
{
  if ( segment == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  if ( (validPosition(segment) == 1) &&
       (validPath(segment) == 1) ) {
    return(1);
  } else {
    return(0);
  }
}

/*
 * Externalised version for rtcpcldapi.c
 */
int rtcpcld_validSegment(
                         segment
                         )
     struct Cstager_Segment_t *segment;
{
  return(validSegment(segment));
}

static int compareSegments(
                           arg1,
                           arg2
                           )
     CONST void *arg1, *arg2;
{
  struct Cstager_Segment_t **segm1, **segm2;
  int fseq1, fseq2, rc;
  
  segm1 = (struct Cstager_Segment_t **)arg1;
  segm2 = (struct Cstager_Segment_t **)arg2;
  if ( segm1 == NULL || segm2 == NULL ||
       *segm1 == NULL || *segm2 == NULL ) return(0);
  Cstager_Segment_fseq(*segm1,&fseq1);
  Cstager_Segment_fseq(*segm2,&fseq2);
  rc = 0;
  if ( fseq1 < fseq2 ) rc = -1;
  if ( fseq1 > fseq2 ) rc = 1;

  return(rc);
}

static int alreadySelected(
                           tape,
                           segment
                           )
     tape_list_t *tape;
     struct Cstager_Segment_t *segment;
{
  struct Cstager_Segment_t *compSegment = NULL, segm = NULL;
  file_list_t *fl;
  
  if ( (tape == NULL) || (segment == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }
  /*
   * Check last element first
   */
  segm = segment;
  fl = tape->file->prev;
  if ( fl->dbRef != NULL ) {
    compSegment = fl->dbRef->row;
    if ( compareSegments((void *)&segm,(void *)&compSegment) == 0 ) return(1);
  }

  CLIST_ITERATE_BEGIN(tape->file,fl) 
    {
      if ( fl->dbRef != NULL ) {
        compSegment = fl->dbRef->row;
        if ( compareSegments((void *)&segm,(void *)&compSegment) == 0 ) return(1);
      }
    }
  CLIST_ITERATE_END(tape->file,fl);
  return(0);
}

/**
 * This method is only called from methods used by the recaller.
 * The catalogue is queried for new segments to be processed for the tape
 * using the segmentsForTape() method. New file list items are created for the
 * new segments. The newly file list items are incomplete (no path) and
 * caller should do a subsequent call to bestFileSystemForSegment() in order
 * complete the path.
 */
static int procSegmentsForTape(
                               tape
                               )
     tape_list_t *tape;
{
  struct Cstager_Tape_t *tp = NULL;
  struct Cstager_IStagerSvc_t **stgsvc = NULL;
  struct Cstager_Segment_t **segmArray = NULL;
  enum Cstager_SegmentStatusCodes_t cmpStatus;
  file_list_t *fl = NULL;
  tape_list_t *tl = NULL;
  unsigned char *blockid;
  ID_TYPE key;
  int rc, i, nbItems = 0, save_serrno, fseq, newFileReqs = 0;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_DISABLE) ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL || *stgsvc == NULL ) return(-1);

  tp = (struct Cstager_Tape_t *)tape->dbRef->row;

  rc = Cstager_IStagerSvc_segmentsForTape(
                                          *stgsvc,
                                          tp,
                                          &segmArray,
                                          &nbItems
                                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("Cstager_IStagerSvc_segmentsForSegment()",
                   Cstager_IStagerSvc_errorMsg(*stgsvc));
    serrno = save_serrno;
    return(-1);
  }

  if ( nbItems == 0 ) {
    serrno = ENOENT;
    return(-1);
  }

  /*
   * The segments returned by the catalogue are not sorted
   * in any particular order. Let's sort them in tape fseq
   * order so that at least consecutive tape files are positioned
   * efficiently.
   */
  qsort(
        (void *)segmArray,
        (size_t)nbItems,
        sizeof(struct Cstager_Segment_t *),
        compareSegments
        );

  tl = tape;
  for ( i=0; i<nbItems; i++ ) {
    Cstager_Segment_status(segmArray[i],&cmpStatus);
    if ( (cmpStatus == SEGMENT_UNPROCESSED) &&
         (alreadySelected(tape,segmArray[i]) == 0) ) {
      Cstager_Segment_blockid(
                              segmArray[i],
                              (CONST unsigned char **)&blockid
                              );
      Cstager_Segment_fseq(
                           segmArray[i],
                           &fseq
                           );
      rc = rtcp_NewFileList(&tl,&fl,tl->tapereq.mode);
      if ( rc == -1 ) {
        save_serrno = serrno;
        free(segmArray);
        (void)dlf_write(
                        (inChild == 0 ? mainUuid : childUuid),
                        RTCPCLD_LOG_MSG(RTCPCLD_MSG_SYSCALL),
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
      (void)dlf_write(
                      childUuid,
                      RTCPCLD_LOG_MSG(RTCPCLD_MSG_FILEREQ),
                      (struct Cns_fileid *)NULL,
                      1,
                      "FSEQ",
                      DLF_MSG_PARAM_INT,
                      fseq
                      );
      newFileReqs = 1;
      fl->filereq.concat = NOCONCAT;
      fl->filereq.convert = ASCCONV;
      strcpy(fl->filereq.recfm,"F");
      fl->filereq.tape_fseq = fseq;
      fl->filereq.def_alloc = 0;
      fl->filereq.disk_fseq = ++(fl->prev->filereq.disk_fseq);
      fl->dbRef = (RtcpDBRef_t *)calloc(1,sizeof(RtcpDBRef_t));
      if ( fl->dbRef == NULL ) {
        LOG_SYSCALL_ERR("calloc()");
        serrno = SESYSERR;
        return(-1);
      }
      fl->dbRef->row = segmArray[i];
      Cstager_Segment_id(segmArray[i],&key);
      fl->dbRef->key = key;
    }
    memcpy(fl->filereq.blockid,blockid,sizeof(fl->filereq.blockid));
    if ( memcmp(fl->filereq.blockid,nullblkid,sizeof(nullblkid)) == 0 ) 
      fl->filereq.position_method = TPPOSIT_FSEQ;
    else fl->filereq.position_method = TPPOSIT_BLKID;
    fl->filereq.proc_status = RTCP_WAITING;
    /*
     * Temporary hack until rtcpd_MainCntl.c fix has been deployed
     */
    fl->filereq.blocksize = 32760;
    
    /*
     * Path is set when this segment is ready for processing.
     */
    strcpy(fl->filereq.file_path,".");
    
    Cstager_Segment_bytes_in(
                             segmArray[i],
                             &(fl->filereq.bytes_in)
                             );
    Cstager_Segment_bytes_out(
                              segmArray[i],
                              &(fl->filereq.bytes_out)
                              );
    Cstager_Segment_offset(
                           segmArray[i],
                           &(fl->filereq.offset)
                           );
  }

  if ( segmArray != NULL ) free(segmArray);
  if ( newFileReqs == 0 ) {
    serrno = EAGAIN;
    return(-1);
  }
  return(0);
}

/** Assign path to the next incomplete segment to recall
 */
static int nextSegmentToRecall(
                               tape,
                               file
                               )
     tape_list_t *tape;
     file_list_t **file;
{
  struct Cstager_IStagerSvc_t **stgsvc = NULL;
  struct C_IObject_t *iObj;
  struct Cstager_Segment_t *segment = NULL;
  struct Cstager_DiskCopyForRecall_t *recallCandidate = NULL;
  struct Cstager_DiskCopy_t *diskCopy = NULL;
  struct Cstager_CastorFile_t *castorFile = NULL;
  char *diskServerName = NULL, *mountPointName = NULL, *pathName = NULL;
  char *nsHost = NULL;
  file_list_t *fl = NULL;
  int rc, save_serrno;
  rtcpFileRequest_t *filereq = NULL;
  struct Cns_fileid *fileid = NULL;

  /*
   * Only recalls, please!
   */  
  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_DISABLE) ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = rtcpcld_nextFileToProcess(tape,&fl);
  if ( (rc != 1) || (fl == NULL) ) {
    rc = procSegmentsForTape(tape);
    if ( rc == -1 ) {
      if ( serrno != EAGAIN ) LOG_SYSCALL_ERR("procSegmentsForTape()");
      return(-1);
    }
    fl = NULL;
    rc = rtcpcld_nextFileToProcess(tape,&fl);
    if ( (rc != 1) || (fl == NULL) ) {
      serrno = ENOENT;
      return(-1);
    }
  }

  filereq = &(fl->filereq);

  if ( fl->dbRef == NULL ) {
    rc = updateSegmentFromDB(fl);
    if ( rc == -1 ) return(-1);
    if ( (fl->dbRef == NULL) || (fl->dbRef->row == NULL) ) {
      serrno = SEINTERNAL;
      return(-1);
    }
  }
  segment = (struct Cstager_Segment_t *)fl->dbRef->row;

  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL || *stgsvc == NULL ) return(-1);
  
  recallCandidate = NULL;
  rc = Cstager_IStagerSvc_bestFileSystemForSegment(
                                                   *stgsvc,
                                                   segment,
                                                   &recallCandidate
                                                   );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("Cstager_IStagerSvc_bestFileSystemForSegment()",
                   Cstager_IStagerSvc_errorMsg(*stgsvc));
    serrno = save_serrno;
    return(-1);
  }

  if ( recallCandidate == NULL ) {
    LOG_DBCALL_ERR("Cstager_IStagerSvc_bestFileSystemForSegment()",
                   "Unexpected successful return without candidate");
    serrno = ENOENT;
    return(-1);
  }
  
  diskCopy = Cstager_DiskCopyForRecall_getDiskCopy(
                                                   recallCandidate
                                                   );
  Cstager_DiskCopy_path(
                        diskCopy,
                        (CONST char **)&pathName
                        );
  Cstager_DiskCopy_castorFile(
                              diskCopy,
                              &castorFile
                              );
  Cstager_DiskCopyForRecall_mountPoint(
                                       recallCandidate,
                                       (CONST char **)&mountPointName
                                       );
  Cstager_DiskCopyForRecall_diskServer(
                                       recallCandidate,
                                       (CONST char **)&diskServerName
                                       );
  if ( castorFile != NULL ) {
    Cstager_CastorFile_nsHost(
                              castorFile,
                              (CONST char **)&nsHost
                              );
    if ( nsHost != NULL ) {
      strncpy(
              filereq->castorSegAttr.nameServerHostName,
              nsHost,
              sizeof(filereq->castorSegAttr.nameServerHostName)
              );
    }
    Cstager_CastorFile_fileId(
                              castorFile,
                              &(filereq->castorSegAttr.castorFileId)
                              );
    (void)rtcpcld_getFileId(
                            fl,
                            &fileid
                            );
  }
  
  if ( (pathName == NULL) || (*pathName == '\0') ||
       (mountPointName == NULL) || (*mountPointName == '\0') ||
       (diskServerName == NULL) || (*diskServerName == '\0') ||
       ((strlen(pathName)+
         strlen(mountPointName)+1+
         strlen(diskServerName)+1) > sizeof(filereq->file_path)-1) ) {
    if ( (strlen(pathName)+
          strlen(mountPointName)+1+
          strlen(diskServerName)+1) > sizeof(filereq->file_path)-1 ) {
      save_serrno = E2BIG;
    } else {
      save_serrno = SEINTERNAL;
    }
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_BADPATH),
                    (struct Cns_fileid *)fileid,
                    RTCPCLD_NB_PARAMS+3,
                    "DISKSRV",
                    DLF_MSG_PARAM_STR,
                    (diskServerName == NULL ? "(null)" : diskServerName),
                    "MNTPNT",
                    DLF_MSG_PARAM_STR,
                    (mountPointName == NULL ? "(null)" : mountPointName),
                    "PATHNAME",
                    DLF_MSG_PARAM_STR,
                    (pathName == NULL ? "(null)" : pathName),
                    RTCPCLD_LOG_WHERE
                    );
    serrno = save_serrno;
    return(-1);
  }

  sprintf(
          filereq->file_path,
          "%s:%s/%s",
          diskServerName,
          mountPointName,
          pathName
          );
  if ( file != NULL ) *file = fl;
  return(0);
}

int nextSegmentToMigrate(
                         tape,
                         file
                         )
     tape_list_t *tape;
     file_list_t **file;
{
  struct Cstager_Tape_t *tp = NULL;
  struct Cstager_TapeCopy_t *tpCp = NULL;
  struct Cstager_Stream_t *stream = NULL;
  struct C_Services_t **svcs = NULL;
  struct Cstager_IStagerSvc_t **stgsvc = NULL;
  struct Cstager_TapeCopyForMigration_t *nextMigrCandidate = NULL;
  struct Cstager_CastorFile_t *castorFile = NULL;
  struct Cstager_DiskCopy_t **dskCps = NULL, *dkCp = NULL;
  struct Cstager_Segment_t *segment = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  rtcpFileRequest_t *filereq = NULL;
  tape_list_t *tl = NULL;
  file_list_t *fl = NULL;
  char *diskServer = NULL, *mountPoint = NULL, *relPath = NULL, *nsHost = NULL;
  unsigned char *blockid;
  struct Cns_fileid *fileid;
  int rc, i, nbItems = 0, save_serrno, nbDskCps = 0;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_ENABLE) ) {
    serrno = EINVAL;
    return(-1);
  }

  if ( (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
    serrno = SEINTERNAL;
    return(-1);
  }  

  rc = getStgSvc(&stgsvc);
  if ( rc != -1 ) getDbSvc(&svcs);
  if ( rc == -1 || stgsvc == NULL || *stgsvc == NULL ||
       svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  Cstager_Tape_stream(tp,&stream);

  if ( stream == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "No stream found for tape",
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);
  }

  /*
   * \todo call getBestTapeCopyForStream() and create the
   * corresponding segment (in memory only).
   */
  rc = Cstager_IStagerSvc_bestTapeCopyForStream(
                                                *stgsvc,
                                                stream,
                                                &nextMigrCandidate
                                                );
  if ( rc == -1 ) {
    save_serrno = serrno;
    if ( save_serrno == ENOENT ) {
      C_IAddress_delete(iAddr);
      serrno = ENOENT;
      return(-1);
    } else {
      LOG_DBCALL_ERR("Cstager_IStagerSvc_bestTapeCopyForSegment()",
                     Cstager_IStagerSvc_errorMsg(*stgsvc));
    }
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  /*
   * Now, we need to get the TapeCopy and create a Segment
   * associated with it. We never commit the Segment in the
   * DB unless there was an error when writing it to tape.
   */
  tpCp = Cstager_TapeCopyForMigration_getTapeCopy(nextMigrCandidate);

  /*
   * If there was previous retries there are already Segments
   * associated with this TapeCopy. Let's get them so that
   * we don't risk to remove them at next updc.
   */
  iObj = Cstager_TapeCopy_getIObject(tpCp);
  rc = C_Services_fillObj(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_Segment
                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillObj()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  /*
   * Now add our own Segment
   */
  Cstager_Segment_create(&segment);
  if ( segment == NULL ) {
    LOG_SYSCALL_ERR("Cstager_Segment_create()");
    C_IAddress_delete(iAddr);
    serrno = SESYSERR;
    return(-1);
  }
  Cstager_Segment_setTape(segment,tp);
  Cstager_Tape_addSegments(tp,segment);
  Cstager_Segment_setCopy(segment,tpCp);
  Cstager_TapeCopy_addSegments(tpCp,segment);

  Cstager_TapeCopyForMigration_diskServer(
                                          nextMigrCandidate,
                                          (CONST char **)&diskServer
                                          );

  Cstager_TapeCopyForMigration_mountPoint(
                                    nextMigrCandidate,
                                    (CONST char **)&mountPoint
                                    );
  if ( mountPoint == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "No mountpoint set for selected migration candidate",
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);
  }
  Cstager_TapeCopy_castorFile(tpCp,&castorFile);

  Cstager_CastorFile_diskCopies(
                                castorFile,
                                &dskCps,
                                &nbDskCps
                                );
  if ( dskCps != NULL ) {
    dkCp = dskCps[0];
    free(dskCps);
  } else {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "No disk copy set for selected migration candidate",
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);
  }

  Cstager_DiskCopy_path(
                        dkCp,
                        (CONST char **)&relPath
                        );
  
  if ( relPath == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "No path set for selected migration candidate",
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);
  }
  if ( (strlen(diskServer)+strlen(mountPoint)+strlen(relPath)+2) > 
       (sizeof(filereq->file_path)-1) ) {
    C_IAddress_delete(iAddr);
    serrno = E2BIG;
    return(-1);
  }

  /*
   * Now we have everything we need to construct a valid filereq
   */
  serrno = 0;
  tl = tape;
  rc = rtcpcld_nextFileToProcess(tape,&fl);
  if ( rc == 0 ) {
    rc = rtcp_NewFileList(&tl,&fl,tape->tapereq.mode);
  }
  if ( rc == -1 ) {
    if ( serrno != 0 ) save_serrno = serrno;
    else save_serrno = errno;
    LOG_SYSCALL_ERR("rtcp_NewFileList()");
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  fl->dbRef = (RtcpDBRef_t *)calloc(1,sizeof(RtcpDBRef_t));
  if ( fl->dbRef == NULL ) {
    LOG_SYSCALL_ERR("calloc()");
    C_IAddress_delete(iAddr);
    serrno = SESYSERR;
    return(-1);
  }
  fl->dbRef->row = (void *)segment;

  /*
   * Initialize the usual defaults
   */
  filereq = &(fl->filereq);
  filereq->concat = NOCONCAT;
  filereq->convert = ASCCONV;
  filereq->position_method = TPPOSIT_FSEQ;
  filereq->VolReqID = tape->tapereq.VolReqID;
  strcpy(filereq->recfm,"F");
  filereq->def_alloc = 0;
  filereq->proc_status = RTCP_WAITING;

  if ( diskServer != NULL ) {
    sprintf(filereq->file_path,"%s:%s/%s",diskServer,mountPoint,relPath);
  } else {
    sprintf(filereq->file_path,"%s/%s",mountPoint,relPath);
  }

  Cstager_CastorFile_nsHost(
                            castorFile,
                            (CONST char **)&nsHost
                            );

  if ( nsHost != NULL ) {
    if ( strlen(nsHost) > sizeof(filereq->castorSegAttr.nameServerHostName)-1 ) {
      C_IAddress_delete(iAddr);
      serrno = E2BIG;
      return(-1);
    }
    strcpy(filereq->castorSegAttr.nameServerHostName,nsHost);
  }
  Cstager_CastorFile_fileId(
                            castorFile,
                            &(filereq->castorSegAttr.castorFileId)
                            );
  Cstager_CastorFile_fileSize(
                              castorFile,
                              &(filereq->bytes_in)
                              );
  (void)u64tohexstr(
                    filereq->castorSegAttr.castorFileId,
                    filereq->fid,
                    -sizeof(filereq->fid)
                    );

  C_IAddress_delete(iAddr);  
  if ( rtcpcld_checkDualCopies(fl) == -1 ) {
    save_serrno = serrno;
    if ( serrno == EEXIST ) {
      struct Cns_fileid *fileid = NULL;
      (void)rtcpcld_getFileId(fl,&fileid);
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      RTCPCLD_LOG_MSG(RTCPCLD_MSG_CPEXIST),
                      (struct Cns_fileid *)fileid,
                      RTCPCLD_NB_PARAMS+1,
                      "",
                      DLF_MSG_PARAM_TPVID,
                      tape->tapereq.vid
                      );
    } else {
      LOG_SYSCALL_ERR("rtcpcld_checkDualCopy()");
    }
    serrno = save_serrno;
    return(-1);
  }
  if ( file != NULL ) *file = fl;
  return(0);
}

/**
 * Externalised entry to procSegmentsForTape() 
 * or procTapeCopiesForStream()
 */
int rtcpcld_getSegmentToDo(
                           tape,
                           file
                           )
     tape_list_t *tape;
     file_list_t **file;
{
  int rc;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = verifyTape(tape);
  if ( rc == -1 ) return(-1);
  
  if ( tape->tapereq.mode == WRITE_DISABLE ) {
    return(nextSegmentToRecall(tape,file));
  } else if ( tape->tapereq.mode == WRITE_ENABLE ) {
    return(nextSegmentToMigrate(tape,file));
  } else {
    serrno = EINVAL;
    return(-1);
  }
}

int rtcpcld_getTapeCopyNumber(
                              file,
                              tapeCopyNb
                              )
     file_list_t *file;
     int *tapeCopyNb;
{
  struct Cstager_Segment_t *segment = NULL;
  struct Cstager_TapeCopy_t *tapeCopy = NULL;
  int copyNb = 0;
  
  if ( (file == NULL) || (file->dbRef == NULL) || (file->dbRef->row == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }
  segment = (struct Cstager_Segment_t *)file->dbRef->row;
  Cstager_Segment_copy(segment,&tapeCopy);
  if ( tapeCopy == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  Cstager_TapeCopy_copyNb(tapeCopy,&copyNb);
  if ( tapeCopyNb != NULL ) {
    if ( VALID_COPYNB(copyNb) ) *tapeCopyNb = copyNb;
    else *tapeCopyNb = 0;
  }
  
  return(0);
}

/**
 * This method is only called from methods used by the VidWorker childs.
 * Query database to check if there are any segments to process for this
 * tape. This method is called by the VidWorker at startup to avoid unnecessary
 * tape mounts.
 */
int rtcpcld_anyReqsForTape(
                           tape
                           )
     tape_list_t *tape;
{
  struct Cstager_Tape_t *tp;
  struct Cstager_Stream_t *stream;
  struct Cstager_IStagerSvc_t **stgsvc = NULL;
  int rc, nbItems, save_serrno;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }

  rc = getStgSvc(&stgsvc);
  if ( rc == -1 || stgsvc == NULL || *stgsvc == NULL ) return(-1);

  rc = verifyTape(tape);
  if ( rc == -1 ) {
    return(-1);
  }
  
  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  if ( tape->tapereq.mode == WRITE_DISABLE ) {
    rc = Cstager_IStagerSvc_anySegmentsForTape(*stgsvc,tp);
  } else {
    Cstager_Tape_stream(tp,&stream);
    rc = Cstager_IStagerSvc_anyTapeCopyForStream(*stgsvc,stream);
  }
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR((tape->tapereq.mode == WRITE_DISABLE ?
                    "Cstager_anySegmentsForTape()" :
                    "Cstager_anyTapeCopiesForStream()"),
                   Cstager_IStagerSvc_errorMsg(*stgsvc));
    serrno = save_serrno;
    return(-1);
  }
  nbItems = rc;

  return(nbItems);
}

int deleteSegmentFromDB(
                        segment,
                        tp,
                        tapeCopy
                        )
     struct Cstager_Tape_t *tp;
     struct Cstager_Segment_t *segment;
     struct Cstager_TapeCopy_t *tapeCopy;
{
  struct Cstager_Tape_t *tmpTp = NULL;
  struct Cstager_TapeCopy_t *tmpTapeCopy = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct C_Services_t **svcs = NULL;
  int rc = 0, nbTapeCopies, save_serrno, i;
  ID_TYPE key;

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  if ( segment == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  iObj = Cstager_Segment_getIObject(segment);
  
  if ( tp == NULL ) {
    Cstager_Segment_tape(segment,&tmpTp);
    if ( tmpTp == NULL ) {
      rc = C_Services_fillObj(
                              *svcs,
                              iAddr,
                              iObj,
                              OBJ_Tape
                              );
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_DBCALL_ERR("C_Services_fillObj()",
                       C_Services_errorMsg(*svcs));
        C_IAddress_delete(iAddr);
        serrno = save_serrno;
        return(-1);
      }
    }
    Cstager_Segment_tape(segment,&tmpTp);
  } else {
    tmpTp = tp;
  }
  

  if ( tapeCopy == NULL ) {
    Cstager_Segment_copy(segment,&tmpTapeCopy);
    if ( tmpTapeCopy == NULL ) {
      rc = C_Services_fillObj(
                              *svcs,
                              iAddr,
                              iObj,
                              OBJ_TapeCopy
                              );
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_DBCALL_ERR("C_Services_fillObj()",
                       C_Services_errorMsg(*svcs));
        C_IAddress_delete(iAddr);
        serrno = save_serrno;
        return(-1);
      }
    }
    Cstager_Segment_copy(segment,&tmpTapeCopy);
  } else {
    tmpTapeCopy = tapeCopy;
  }

  Cstager_Segment_id(segment,&key);
  Cstager_Segment_setTape(segment,NULL);
  Cstager_Segment_setCopy(segment,NULL);
  if ( tmpTp != NULL ) {
    Cstager_Tape_removeSegments(tmpTp,segment);
    rc = C_Services_fillRep(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_Tape,
                            0
                            );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_fillRep()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
  }
  if ( tmpTapeCopy != NULL ) {
    Cstager_TapeCopy_removeSegments(tmpTapeCopy,segment);
    rc = C_Services_fillRep(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_TapeCopy,
                            0
                            );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_fillRep()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
  }
  rc = C_Services_deleteRep(
                            *svcs,
                            iAddr,
                            iObj,
                            1
                            );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_deleteRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  C_IAddress_delete(iAddr);
  return(0);
}

int deleteTapeCopyFromDB(
                         tapeCopy
                         )
     struct Cstager_TapeCopy_t *tapeCopy;
{
  struct Cstager_Stream_t *stream = NULL, **streamArray;
  struct Cstager_CastorFile_t *castorFile = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct C_Services_t **svcs = NULL;
  int rc = 0, nbStreams, save_serrno, i;
  ID_TYPE key;

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  if ( tapeCopy == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  iObj = Cstager_TapeCopy_getIObject(tapeCopy);

  Cstager_TapeCopy_id(tapeCopy,&key);
  rc = C_Services_fillObj(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_Stream
                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillObj()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  
  Cstager_TapeCopy_stream(tapeCopy,&streamArray,&nbStreams);
  for ( i=0; i<nbStreams; i++ ) {
    Cstager_TapeCopy_removeStream(
                                  tapeCopy,
                                  streamArray[i]
                                  );
    Cstager_Stream_removeTapeCopy(
                                  streamArray[i],
                                  tapeCopy
                                  );
    rc = C_Services_fillRep(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_Stream,
                            0
                            );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_fillRep()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      free(streamArray);
      serrno = save_serrno;
      return(-1);
    }
  }
  if ( streamArray != NULL ) free(streamArray);
  
  Cstager_TapeCopy_setCastorFile(tapeCopy,NULL);
  rc = C_Services_fillRep(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_CastorFile,
                          0
                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  
  rc = C_Services_deleteRep(
                            *svcs,
                            iAddr,
                            iObj,
                            1
                            );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  return(0);
}

/** Catalogue update after failed migration
 *
 * @param tape - the tape list to which the file request belongs
 * @param filereq - the file request that failed
 *
 * Catalogue update after failed migration implies:
 *  - Set the failure info fields of the corresponding segment
 *  - Set the Segment status to SEGMENT_FAILED
 *  - Commit the Segment to DB (for migration, Segments are usually transient in memory)
 *  - Leave the TapeCopy in TAPECOPY_SELECTED status
 *  - Cut the link between TapeCopy and Stream
 *  - Don't update the DiskCopy (will remain in DISKCOPY_STAGEOUT until the
 *    migration control process decides to do no further retries)
 * Note that the migrator will never attempt to do any retries by itself.
 * All retries has to be decided by a separate process scanning the catalogue
 * for SELECTED TapeCopies associated with FAILED Segments. The number of
 * failed retries == the number of FAILED Segments associated with the TapeCopy
 * (one should never remove a FAILED Segment until the TapeCopy itself is removed).
 */
int rtcpcld_updcMigrFailed(
                           tape,
                           file
                           )
     tape_list_t *tape;
     file_list_t *file;
{
  struct Cstager_TapeCopy_t *tapeCopy, **tapeCopyArray;
  enum Cstager_TapeCopyStatusCodes_t tapeCopyStatus;
  struct Cstager_Stream_t **streamArray = NULL;
  struct Cstager_Segment_t **segmentArray, *segment;
  struct Cstager_Tape_t *tp;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct C_Services_t **svcs = NULL;
  rtcpFileRequest_t *filereq;
  struct Cns_fileid *fileid;
  int rc = 0, nbTapeCopies, nbStreams, save_serrno, i, j;
  ID_TYPE key;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_ENABLE) ||
       (file == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }

  filereq = &(file->filereq);

  fileid = NULL;
  (void)rtcpcld_getFileId(file,&fileid);

  rc = verifyTape(tape);
  if ( rc == -1 ) {
    return(-1);
  }

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  segment = (struct Cstager_Segment_t *)file->dbRef->row;
  if ( segment != NULL ) Cstager_Segment_copy(segment,&tapeCopy);
  if ( (segment == NULL) || (tapeCopy == NULL) ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)fileid,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    ((segment == NULL) ? 
                     "No segment found for filereq" :
                     "No tape copy found for filereq" ),
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);    
  }
  /*
   * Update Segment error information
   */
  Cstager_Segment_setErrMsgTxt(
                               segment,
                               filereq->err.errmsgtxt
                               );
  Cstager_Segment_setErrorCode(
                               segment,
                               filereq->err.errorcode
                               );
  Cstager_Segment_setSeverity(
                              segment,
                              filereq->err.severity
                              );
  Cstager_Segment_setStatus(
                            segment,
                            SEGMENT_FAILED
                            );
  /*
   * Remove the stream associations
   */
  streamArray = NULL;
  Cstager_TapeCopy_stream(
                          tapeCopy,
                          &streamArray,
                          &nbStreams
                          );
  for ( i=0; i<nbStreams; i++ ) {
    Cstager_TapeCopy_removeStream(
                                  tapeCopy,
                                  streamArray[i]
                                  );
  }
  if ( streamArray != NULL ) free(streamArray);

  /*
   * Update DB
   */
  iObj = Cstager_TapeCopy_getIObject(tapeCopy);
  rc = C_Services_fillRep(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_Segment,
                          0
                          );
  if ( rc != -1 ) {
    rc = C_Services_fillRep(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_Stream,
                            1
                            );
  }
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  /*
   * Make sure that the tape link is also created (the
   * memory objects were linked in tapeCopiesForStream)
   */
  iObj = Cstager_Segment_getIObject(segment);
  rc = C_Services_fillRep(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_Tape,
                          1
                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  C_IAddress_delete(iAddr);
  return(0);
}

/** Catalogue update after failed recall
 *
 * @param tape - the tape list to which the file request belongs
 * @param filereq - the file request that failed
 *
 * Catalogue update after failed migration implies:
 *  - Set the failure info fields of the corresponding segment
 *  - Set the Segment status to SEGMENT_FAILED
 *  - Leave the TapeCopy in TAPECOPY_SELECTED status
 *  - Don't update the DiskCopy (will remain in DISKCOPY_STAGEOUT until the
 *    recall control process decides to do no further retries)
 * Note that the trvsllrt will never attempt to do any retries by itself.
 * All retries has to be decided by a separate process scanning the catalogue
 * for SELECTED TapeCopies associated with FAILED Segments. The number of
 * failed retries == the number of FAILED Segments associated with the TapeCopy
 * (one should never remove a FAILED Segment until the TapeCopy itself is removed).
 */

int rtcpcld_updcRecallFailed(
                             tape,
                             file
                             )
     tape_list_t *tape;
     file_list_t *file;
{
  struct Cstager_TapeCopy_t *tapeCopy, **tapeCopyArray;
  enum Cstager_TapeCopyStatusCodes_t tapeCopyStatus;
  struct Cstager_Stream_t **streamArray = NULL;
  struct Cstager_Segment_t **segmentArray, *segment;
  struct Cstager_Tape_t *tp;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct C_Services_t **svcs = NULL;
  rtcpFileRequest_t *filereq;
  struct Cns_fileid *fileid;
  int rc = 0, nbTapeCopies, nbStreams, save_serrno, i, j;
  ID_TYPE key;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_ENABLE) ||
       (file == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }

  filereq = &(file->filereq);

  fileid = NULL;
  (void)rtcpcld_getFileId(file,&fileid);

  rc = verifyTape(tape);
  if ( rc == -1 ) {
    return(-1);
  }

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  segment = (struct Cstager_Segment_t *)file->dbRef->row;
  if (segment == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)fileid,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "No segment found for filereq",
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);    
  }
  /*
   * Update Segment error information
   */
  Cstager_Segment_setErrMsgTxt(
                               segment,
                               filereq->err.errmsgtxt
                               );
  Cstager_Segment_setErrorCode(
                               segment,
                               filereq->err.errorcode
                               );
  Cstager_Segment_setSeverity(
                              segment,
                              filereq->err.severity
                              );
  Cstager_Segment_setStatus(
                            segment,
                            SEGMENT_FAILED
                            );
  /*
   * Update DB
   */
  iObj = Cstager_Segment_getIObject(segment);
  rc = C_Services_updateRep(
                            *svcs,
                            iAddr,
                            iObj,
                            1
                            );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_updateRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  C_IAddress_delete(iAddr);
  
  return(rc);
}

int rtcpcld_updcFileRecalled(
                             tape,
                             file
                             )
     tape_list_t *tape;
     file_list_t *file;
{
  struct Cstager_TapeCopy_t *tapeCopy, **tapeCopyArray;
  enum Cstager_TapeCopyStatusCodes_t tapeCopyStatus;
  struct Cstager_CastorFile_t *castorFile;
  struct Cstager_DiskCopy_t **diskCopyArray, *diskCopy;
  enum Cstager_DiskCopyStatusCodes_t diskCopyStatus;
  struct Cstager_Segment_t **segmentArray, *segment;
  enum Cstager_SegmentStatusCodes_t segmentStatus;
  struct Cstager_Tape_t *tp;
  struct Cstager_Stream_t **streamArray;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct C_Services_t **svcs = NULL;
  struct Cstager_IStagerSvc_t **stgSvc = NULL;
  rtcpFileRequest_t *filereq;
  struct Cns_fileid *fileid;
  int rc = 0, nbSegments = 0, nbTapeCopies = 0, doUpdate = 1;
  int save_serrno, i, j;
  ID_TYPE key;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_DISABLE) ||
       (file == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }

  filereq = &(file->filereq);

  fileid = NULL;
  (void)rtcpcld_getFileId(file,&fileid);

  rc = verifyTape(tape);
  if ( rc == -1 ) {
    return(-1);
  }

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  segment = (struct Cstager_Segment_t *)file->dbRef->row;
  free(file->dbRef);
  file->dbRef = NULL;
  if ( segment != NULL ) Cstager_Segment_copy(segment,&tapeCopy);
  if ( tapeCopy != NULL ) Cstager_TapeCopy_castorFile(tapeCopy,&castorFile);
  if ( (segment == NULL) || (tapeCopy == NULL) || (castorFile == NULL) ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)fileid,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    ((segment == NULL) ? 
                     "No segment found for filereq" :
                     (tapeCopy == NULL ?
                      "No tape copy found for filereq" :
                      "No castor file found for filereq")),
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);    
  }

  /*
   * Get all segments to check if they are all copied before updating
   * the catalogue
   */
  iObj = Cstager_TapeCopy_getIObject(tapeCopy);
  rc = C_Services_fillObj(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_Segment
                          );
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillObj()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  Cstager_TapeCopy_segments(
                            tapeCopy,
                            &segmentArray,
                            &nbSegments
                            );
  /*
   * Update the status of this segment (otherwise the check
   * below would always fail since in the DB the status of
   * this segment is still SEGMENT_UNPROCESSED)
   */
  Cstager_Segment_setStatus(segment,SEGMENT_FILECOPIED);
  for ( i=0; i<nbSegments; i++ ) {
    Cstager_Segment_status(
                           segmentArray[i],
                           &segmentStatus
                           );
    if ( segmentStatus != SEGMENT_FILECOPIED ) {
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      RTCPCLD_LOG_MSG(RTCPCLD_MSG_REMAININGSEGMS),
                      (struct Cns_fileid *)fileid,
                      0
                      );
      doUpdate = 0;
      break;
    }
  }
  free(segmentArray);

  if ( doUpdate == 0 ) {
    /*
     * OK, there are still incomplete segments for this file.
     * We just update our own.
     */
    Cstager_Segment_setStatus(
                              segment,
                              SEGMENT_FILECOPIED
                              );
    iObj = Cstager_Segment_getIObject(segment);
    rc = C_Services_updateRep(
                              *svcs,
                              iAddr,
                              iObj,
                              1
                              );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_updateRep()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
    C_IAddress_delete(iAddr);
  } else {
    C_IAddress_delete(iAddr);
    /*
     * Yes, the whole file is really staged. Update the
     * disk copy status and Get rid of the segments and
     * tape copy.
     */
    rc = getStgSvc(&stgSvc);
    if ( rc == -1 || stgSvc == NULL || *stgSvc == NULL ) return(-1);
    rc = Cstager_IStagerSvc_fileRecalled(
                                         *stgSvc,
                                         tapeCopy
                                         );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("Cstager_IStagerSvc_fileRecalled()",
                     Cstager_IStagerSvc_errorMsg(*stgSvc));
      serrno = save_serrno;
      return(-1);
    }
    rc = deleteSegmentFromDB(
                             segment,
                             tp,
                             tapeCopy
                             );
    if ( rc == -1 ) return(-1);
    rc = deleteTapeCopyFromDB(
                              tapeCopy
                              );
    if ( rc == -1 ) return(-1);
  }
  
  return(0);
}

int rtcpcld_updcFileMigrated(
                             tape,
                             file
                             )
     tape_list_t *tape;
     file_list_t *file;
{
  struct Cstager_TapeCopy_t *tapeCopy, **tapeCopyArray;
  enum Cstager_TapeCopyStatusCodes_t tapeCopyStatus;
  struct Cstager_CastorFile_t *castorFile;
  struct Cstager_DiskCopy_t **diskCopyArray, *diskCopy;
  enum Cstager_DiskCopyStatusCodes_t diskCopyStatus;
  struct Cstager_Segment_t **segmentArray, *segment;
  struct Cstager_Tape_t *tp;
  struct Cstager_Stream_t **streamArray;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct C_Services_t **svcs = NULL;
  rtcpFileRequest_t *filereq;
  struct Cns_fileid *fileid;
  int rc = 0, nbTapeCopies, nbDiskCopies, nbSegments, nbStreams;
  int save_serrno, i, j;
  ID_TYPE key;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_ENABLE) ||
       (file == NULL) ) {
    serrno = EINVAL;
    return(-1);
  }

  filereq = &(file->filereq);

  fileid = NULL;
  (void)rtcpcld_getFileId(file,&fileid);

  rc = verifyTape(tape);
  if ( rc == -1 ) {
    return(-1);
  }

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }
  iAddr = C_BaseAddress_getIAddress(baseAddr);

  tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  segment = (struct Cstager_Segment_t *)file->dbRef->row;
  free(file->dbRef);
  file->dbRef = NULL;
  if ( segment != NULL ) Cstager_Segment_copy(segment,&tapeCopy);
  if ( tapeCopy != NULL ) Cstager_TapeCopy_castorFile(tapeCopy,&castorFile);
  if ( (segment == NULL) || (tapeCopy == NULL) || (castorFile == NULL) ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)fileid,
                    RTCPCLD_NB_PARAMS+1,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    ((segment == NULL) ? 
                     "No segment found for filereq" :
                     (tapeCopy == NULL ?
                      "No tape copy found for filereq" :
                      "No castor file found for filereq")),
                    RTCPCLD_LOG_WHERE
                    );
    C_IAddress_delete(iAddr);
    serrno = SEINTERNAL;
    return(-1);    
  }

  /*
   * Get all disk and tape copies from DB
   */
  iObj = Cstager_CastorFile_getIObject(castorFile);
  rc = C_Services_fillObj(
                          *svcs,
                          iAddr,
                          iObj,
                          OBJ_TapeCopy
                          );
  if ( rc != -1 ) {
    rc = C_Services_fillObj(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_DiskCopy
                            );
  }

  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_fillObj()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }

  /*
   * We can only update catalogue if all tape copies associated
   * with this castor file has been written to tape.
   */
  Cstager_CastorFile_tapeCopies(
                                castorFile,
                                &tapeCopyArray,
                                &nbTapeCopies
                                );
  
  /*
   * Update status of this tape copy (otherwise the check below
   * would always fail since in DB the status of this tape copy 
   * is still TAPECOPY_SELECTED)
   */
  Cstager_TapeCopy_setStatus(tapeCopy,TAPECOPY_STAGED);

  for ( i=0; i<nbTapeCopies; i++ ) {
    Cstager_TapeCopy_status(
                            tapeCopyArray[i],
                            &tapeCopyStatus
                            );
    if ( tapeCopyStatus != TAPECOPY_STAGED ) {
      (void)dlf_write(
                      (inChild == 0 ? mainUuid : childUuid),
                      RTCPCLD_LOG_MSG(RTCPCLD_MSG_REMAININGTPCPS),
                      (struct Cns_fileid *)fileid,
                      0
                      );
      if ( tapeCopyArray != NULL ) free(tapeCopyArray);
      C_IAddress_delete(iAddr);
      return(0);
    }
  }
  /*
   * All copies are STAGED. Go ahead and update the DiskCopy
   * status from STAGEOUT to STAGED 
   */
  Cstager_CastorFile_diskCopies(
                                castorFile,
                                &diskCopyArray,
                                &nbDiskCopies
                                );

  for ( i=0; i<nbDiskCopies; i++ ) {
    Cstager_DiskCopy_status(diskCopyArray[i],&diskCopyStatus);
    if ( diskCopyStatus == DISKCOPY_STAGEOUT ) {
      Cstager_DiskCopy_setStatus(diskCopyArray[i],DISKCOPY_STAGED);
      iObj = Cstager_DiskCopy_getIObject(diskCopyArray[i]);
      rc = C_Services_updateRep(
                                *svcs,
                                iAddr,
                                iObj,
                                0
                                );
      if ( rc == -1 ) {
        save_serrno = serrno;
        LOG_DBCALL_ERR("C_Services_updateRep()",
                       C_Services_errorMsg(*svcs));
        if ( diskCopyArray != NULL ) free(diskCopyArray);
        if ( tapeCopyArray != NULL ) free(tapeCopyArray);
        C_Services_rollback(*svcs,iAddr);
        C_IAddress_delete(iAddr);
        serrno = save_serrno;
        return(-1);
      }
    }
    Cstager_DiskCopy_delete(diskCopyArray[i]);
  }
  if ( diskCopyArray != NULL ) free(diskCopyArray);

  rc = C_Services_commit(*svcs,iAddr);
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_commit()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  if ( iAddr != NULL ) C_IAddress_delete(iAddr);


  /*
   * Now we don't need the tape copies anymore. Remove them from
   * the database. We first must remove the segments one by one
   * (on a migration there is only one at the most), then we must
   * detach the Stream and CastorFile and finally delete the
   * TapeCopy.
   */
  for ( i=0; i<nbTapeCopies; i++ ) {
    rc = deleteTapeCopyFromDB(tapeCopyArray[i]);
    if ( rc == -1 ) {
      free(tapeCopyArray);
      return(-1);
    }
  }
  if ( tapeCopyArray != NULL ) free(tapeCopyArray);  
  /*
   * We don't need CastorFile anymore. Because of the composition
   * relationship, removing the CastorFile from memory will automatically 
   * also remove the associated TapeCopy(s) and Segment(s)
   */
  Cstager_CastorFile_delete(castorFile);

  return(0);
}

/**
 * This method is called from methods used by both the recaller/migrator
 * and the rtcpclientd processes. Update the status of a tape (not its
 * segments). Mostly used to mark FAILED status since most other tape
 * statuses are atomically updated by Cstager_IStagerSvc methods.
 */
int rtcpcld_updateTapeStatus(
                             tape, 
                             fromStatus,
                             toStatus
                             )
     tape_list_t *tape;
     enum Cstager_TapeStatusCodes_t fromStatus;
     enum Cstager_TapeStatusCodes_t toStatus;
{
  struct C_Services_t **svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Tape_t *tapeItem = NULL;
  enum Cstager_TapeStatusCodes_t cmpStatus;  
  int rc = 0, save_serrno;

  if ( tape == NULL ) {
    serrno = EINVAL;
    return(-1);
  }
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  if ( (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
    rc = updateTapeFromDB(tape);
    if ( (rc == -1) || (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
      return(-1);
    }
  }
  tapeItem = (struct Cstager_Tape_t *)tape->dbRef->row;

  if ( tapeItem == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
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
  
  Cstager_Tape_status(tapeItem,&cmpStatus);
  if ( fromStatus == cmpStatus ) {
    ID_TYPE _key = 0;
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) {
      return(-1);
    }

    iAddr = C_BaseAddress_getIAddress(baseAddr);
    iObj = Cstager_Tape_getIObject(tapeItem);
    Cstager_Tape_id(tapeItem,&_key);
    Cstager_Tape_setStatus(tapeItem,toStatus);
    /*
     * Set error info, if TAPE_FAILED status is requested
     */
    if ( toStatus == TAPE_FAILED ) {
      if ( tape->tapereq.err.errorcode > 0 )
        Cstager_Tape_setErrorCode(tapeItem,tape->tapereq.err.errorcode);
      if ( *tape->tapereq.err.errmsgtxt != '\0' )
        Cstager_Tape_setErrMsgTxt(tapeItem,tape->tapereq.err.errmsgtxt);
      if ( tape->tapereq.err.severity != RTCP_OK )
        Cstager_Tape_setSeverity(tapeItem,tape->tapereq.err.severity);
    }
    
    if ( toStatus == TAPE_FAILED || toStatus == TAPE_FINISHED ) {
      char *vwAddress = "";
      Cstager_Tape_setVwAddress(tapeItem,vwAddress);
    }
    
    rc = C_Services_updateRep(*svcs,iAddr,iObj,1);
    save_serrno = serrno;
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_updateRep()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
    C_IAddress_delete(iAddr);
  }
  return(0);
}

/**
 * This method is called from methods used by both the migrator
 * and the rtcpclientd processes after finishing with a tape (either
 * FULL or some error). Detach the tape from this stream and reset the
 * stream status to STREAM_PENDING to allow for a subsequent streamsToDo()
 * to pick up this stream for further processing.
 */
int rtcpcld_returnStream(
                         tape
                        )
     tape_list_t *tape;
{
  struct C_Services_t **svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Tape_t *tp = NULL;
  struct Cstager_Stream_t *stream = NULL;
  enum Cstager_TapeStatusCodes_t tpStatus;  
  struct Cstager_TapeCopy_t **tapeCopyArray = NULL;
  struct Cstager_TapePool_t *tapePool = NULL;
  enum Cstager_TapeCopyStatusCodes_t tapeCopyStatus;
  int i, waitingFound = 0, rc = 0, save_serrno, nbTapeCopies = 0;
  ID_TYPE key = 0;
  int doCommit = 0;

  if ( (tape == NULL) || (tape->tapereq.mode != WRITE_ENABLE) ) {
    serrno = EINVAL;
    return(-1);
  }
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  if ( (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
    rc = updateTapeFromDB(tape);
    if ( (rc == -1) || (tape->dbRef == NULL) || (tape->dbRef->row == NULL) ) {
      return(-1);
    }
  }
  tp = (struct Cstager_Tape_t *)tape->dbRef->row;

  if ( tp == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
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
  
  Cstager_Tape_status(tp,&tpStatus);
  switch (tpStatus) {
  case TAPE_PENDING:
  case TAPE_WAITDRIVE:
  case TAPE_WAITMOUNT:
  case TAPE_MOUNTED:
  case TAPE_FINISHED:
  case TAPE_UNKNOWN:
    rc = rtcpcld_updateTapeStatus(tape,tpStatus,TAPE_UNUSED);
  default:
    break;
  }

  Cstager_Tape_stream(tp,&stream);
  if ( stream != NULL ) {
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) {
      return(-1);
    }

    /*
     * We detach the stream from the tape. Delete the stream
     * if there are no more tape copies waiting in stream, otherwise
     * reset the stream status to pending so that a new tape
     * can be selected.
     */
    iAddr = C_BaseAddress_getIAddress(baseAddr);
    iObj = Cstager_Stream_getIObject(stream);
    rc = C_Services_fillObj(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_TapeCopy
                            );
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR("C_Services_fillObj()",
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
    Cstager_Stream_tapeCopy(stream,&tapeCopyArray,&nbTapeCopies);     
    Cstager_Stream_id(stream,&key);
    for ( i=0; i<nbTapeCopies; i++ ) {
      Cstager_TapeCopy_status(tapeCopyArray[i],&tapeCopyStatus);
      if ( tapeCopyStatus == TAPECOPY_WAITINSTREAMS ) {
        waitingFound = 1;
        break;
      }
    }

    Cstager_Stream_setTape(stream,NULL);
    Cstager_Tape_setStream(tp,NULL);
    doCommit = 0;
    /*
     * Detach the tape (we keep the tape in DB)
     */
    rc = C_Services_fillRep(
                            *svcs,
                            iAddr,
                            iObj,
                            OBJ_Tape,
                            doCommit
                            );
    if ( rc == 0 ) {
      /*
       * Detach the tape pool (we keep the tape pool in DB)
       */
      rc = C_Services_fillRep(
                              *svcs,
                              iAddr,
                              iObj,
                              OBJ_TapePool,
                              doCommit
                              );
    }
    if ( rc == 0 ) {
      doCommit = 1;
      if ( waitingFound == 1 ) {
        Cstager_Stream_setStatus(stream,STREAM_PENDING);
        (void)dlf_write(
                        (inChild == 0 ? mainUuid : childUuid),
                        RTCPCLD_LOG_MSG(RTCPCLD_MSG_RESETSTREAM),
                        (struct Cns_fileid *)NULL,
                        2,
                        "",
                        DLF_MSG_PARAM_TPVID,
                        tape->tapereq.vid,
                        "DBKEY",
                        DLF_MSG_PARAM_INT64,
                        key
                        );
        rc = C_Services_updateRep(
                                  *svcs,
                                  iAddr,
                                  iObj,
                                  doCommit
                                  );
      } else {
        for ( i=0; i<nbTapeCopies; i++ ) {
          Cstager_Stream_removeTapeCopy(stream,tapeCopyArray[i]);
          Cstager_TapeCopy_removeStream(tapeCopyArray[i],stream);
        }
        rc = C_Services_fillRep(
                                *svcs,
                                iAddr,
                                iObj,
                                OBJ_TapeCopy,
                                0
                                );
        if ( rc == 0 ) {
          rc = C_Services_deleteRep(
                                    *svcs,
                                    iAddr,
                                    iObj,
                                    doCommit
                                    );
        }
      }
    }
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR((doCommit == 0 ? 
                      "C_Services_fillRep()" :
                      (waitingFound == 1 ? 
                       "C_Services_fillRep()" :
                       "C_Services_deleteRep()")),
                     C_Services_errorMsg(*svcs));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
  }
  C_IAddress_delete(iAddr);

  return(0);
}

/**
 * This method is called from methods used by both the migrator
 * and the rtcpclientd processes after finishing with a tape (either
 * FULL or some error). Reset all TapeCopy that are in TAPECOPY_SELECTED
 * state (returned by bestTapeCopyForStream()) but not yet processed,
 * allowing them to be picked up by another stream.
 */

int rtcpcld_restoreSelectedTapeCopies(
                                      tape
                                      )
     tape_list_t *tape;
{
  struct C_Services_t **svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Tape_t *tp = NULL;
  struct Cstager_Segment_t *segment;
  struct Cstager_TapeCopy_t *tapeCopy;
  enum Cstager_TapeCopyStatusCodes_t tapeCopyStatus;
  file_list_t *file;
  int rc, save_serrno, doCommit = 0;
  struct Cns_fileid *fileId = NULL;

  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  if ( tape->dbRef != NULL ) {
    tp = (struct Cstager_Tape_t *)tape->dbRef->row;
  }

  if ( tp == NULL ) {
    /*
     * Since the segments are normally only in memory there is
     * nothing we can do if the tape object is not available in
     * memory.
     */
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "",
                    DLF_MSG_PARAM_TPVID,
                    tape->tapereq.vid,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "Error getting tape from DB",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = SEINTERNAL;
    return(-1);
  }
  
  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }

  iAddr = C_BaseAddress_getIAddress(baseAddr);

  CLIST_ITERATE_BEGIN(tape->file,file) 
    {
      if ( (file->filereq.proc_status != RTCP_FINISHED) &&
           ((file->filereq.cprc == 0) ||
            (file->filereq.err.errorcode == ENOSPC)) ) {
        segment = NULL;
        if ( file->dbRef != NULL ) {
          segment = (struct Cstager_Segment_t *)file->dbRef->row;
          if ( segment != NULL ) {
            Cstager_Segment_copy(segment,&tapeCopy);
            if ( tapeCopy != NULL ) {
              Cstager_TapeCopy_setStatus(tapeCopy,TAPECOPY_WAITINSTREAMS);
              iObj = Cstager_TapeCopy_getIObject(tapeCopy);
              rc = C_Services_updateRep(
                                        *svcs,
                                        iAddr,
                                        iObj,
                                        0
                                        );
              if ( rc == -1 ) {
                LOG_DBCALL_ERR(
                               "C_Services_updateRep()",
                               C_Services_errorMsg(*svcs)
                               );
              } else {
                (void)rtcpcld_getFileId(file,&fileId);
                (void)dlf_write(
                                (inChild == 0 ? mainUuid : childUuid),
                                RTCPCLD_LOG_MSG(RTCPCLD_MSG_RESTORETPCP),
                                (struct Cns_fileid *)fileId,
                                0
                                );
                doCommit = 1;
              }
            }
          }
        }
      }
    }
  CLIST_ITERATE_END(tape->file,file);
  if ( doCommit == 1 ) {
    rc = C_Services_commit(
                           *svcs,
                           iAddr
                           );
    if ( rc == -1 ) {
      LOG_DBCALL_ERR(
                     "C_Services_commit()",
                     C_Services_errorMsg(*svcs)
                     );
      return(-1);
    }
  }
  return(0);
}
  

/**
 * This method is called from methods used by the VidWorker childs
 * Update the vwAddress (VidWorker address) for receiving RTCOPY kill
 * requests. 
 */
int rtcpcld_setVidWorkerAddress(
                                tape,
                                port
                                )
     tape_list_t *tape;
     int port;
{
  struct C_Services_t **svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Tape_t *tp = NULL;
  char myHost[CA_MAXHOSTNAMELEN+1], vwAddress[CA_MAXHOSTNAMELEN+12];
  int rc = 0, save_serrno;
  ID_TYPE _key = 0;

  if ( tape == NULL || port < 0 ) {
    serrno = EINVAL;
    return(-1);
  }
  (void)gethostname(myHost,sizeof(myHost)-1);
  sprintf(vwAddress,"%s:%d",myHost,port);
  
  rc = getDbSvc(&svcs);
  if ( rc == -1 || svcs == NULL || *svcs == NULL ) return(-1);

  if ( (tape->dbRef == NULL) || 
       ((tp = tape->dbRef->row) == NULL) ) {
    rc = updateTapeFromDB(tape);
    if ( (rc == 0) && (tape->dbRef != NULL) ) {
      tp = (struct Cstager_Tape_t *)tape->dbRef->row;
    }
  }

  if ( tp == NULL ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_INTERNAL),
                    (struct Cns_fileid *)NULL,
                    RTCPCLD_NB_PARAMS+2,
                    "",
                    DLF_MSG_PARAM_TPVID,
                    tape->tapereq.vid,
                    "REASON",
                    DLF_MSG_PARAM_STR,
                    "Error getting tape from DB",
                    RTCPCLD_LOG_WHERE
                    );
    serrno = SEINTERNAL;
    return(-1);
  }
  
  rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
  if ( rc == -1 ) {
    return(-1);
  }

  iAddr = C_BaseAddress_getIAddress(baseAddr);
  iObj = Cstager_Tape_getIObject(tp);
  Cstager_Tape_id(tp,&_key);
  rc = C_Services_updateObj(*svcs,iAddr,iObj);
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_updateObj()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  Cstager_Tape_setVwAddress(tp,vwAddress);
  Cstager_Tape_id(tp,&_key);
  rc = C_Services_updateRep(*svcs,iAddr,iObj,1);
  if ( rc == -1 ) {
    save_serrno = serrno;
    LOG_DBCALL_ERR("C_Services_updateRep()",
                   C_Services_errorMsg(*svcs));
    C_IAddress_delete(iAddr);
    serrno = save_serrno;
    return(-1);
  }
  C_IAddress_delete(iAddr);
  return(0);
}

/**
 * This method is only called from methods used by the VidWorker childs.
 * Update the status of a segments matching the passed filereq. If no
 * matching entry is found, the method returns -1 and sets serrno == ENOENT.
 */
int rtcpcld_setFileStatus(
                          tape,
                          file, 
                          newStatus,
                          notify
                          )
     tape_list_t *tape;
     file_list_t *file;
     enum Cstager_SegmentStatusCodes_t newStatus;
     int notify;
{
  struct C_Services_t **svcs = NULL;
  struct C_BaseAddress_t *baseAddr = NULL;
  struct C_IAddress_t *iAddr;
  struct C_IObject_t *iObj;
  struct Cstager_Segment_t *segmItem;
  enum Cstager_SegmentStatusCodes_t currentStatus;
  struct Cstager_Tape_t *dummyTp = NULL;
  rtcpFileRequest_t *filereq;
  ID_TYPE _key = 0;
  char *vid;
  int mode, side;
  struct Cns_fileid fileid;
  char *nsHost;
  Cuuid_t stgUuid;
  char *diskPath;
  unsigned char *blockid = NULL;
  int rc = 0, rcTmp, save_serrno, fseq;

  if ( (file == NULL) || (file->dbRef == NULL) || (file->dbRef->row == NULL) ||
       (*(file->filereq.file_path) == '\0') ||
       (*(file->filereq.file_path) == '.') ) {
    serrno = EINVAL;
    return(-1);
  }
  filereq = &(file->filereq);

  segmItem = (struct Cstager_Segment_t *)file->dbRef->row;


  svcs = NULL;
  rc = getDbSvc(&svcs);

  rc = updateSegmentFromDB(file);
  if ( rc == -1 ) {
    return(-1);
  }

  Cstager_Segment_status(
                         segmItem,
                         &currentStatus
                         );

  if ( (currentStatus == SEGMENT_FAILED) &&
       (newStatus != SEGMENT_FAILED) ) {
    (void)dlf_write(
                    (inChild == 0 ? mainUuid : childUuid),
                    RTCPCLD_LOG_MSG(RTCPCLD_MSG_SEGMFAILED),
                    (struct Cns_fileid *)&fileid,
                    RTCPCLD_NB_PARAMS+1,
                    "",
                    DLF_MSG_PARAM_UUID,
                    stgUuid,
                    RTCPCLD_LOG_WHERE
                    );
    serrno = EPERM;
    return(-1);
  }

  diskPath = NULL;
  Cstager_Segment_fseq(
                       segmItem,
                       &fseq
                       );
  Cstager_Segment_blockid(
                          segmItem,
                          (CONST unsigned char **)&blockid
                          );

  if ( ((diskPath != NULL) && 
        (strcmp(filereq->file_path,diskPath) == 0)) &&
       (((filereq->position_method == TPPOSIT_FSEQ) &&
         (filereq->tape_fseq == fseq)) ||
        ((filereq->position_method == TPPOSIT_BLKID) &&
         (memcmp(filereq->blockid,blockid,sizeof(filereq->blockid))==0))) ) {
    rc = C_BaseAddress_create("OraCnvSvc",SVC_ORACNV,&baseAddr);
    if ( rc == -1 ) {
      return(-1);
    }
    
    iAddr = C_BaseAddress_getIAddress(baseAddr);
    
    Cstager_Segment_setStatus(
                              segmItem,
                              newStatus
                              );
    if ( newStatus == SEGMENT_FILECOPIED ) {
      Cstager_Segment_setBlockid(
                                 segmItem,
                                 filereq->blockid
                                 );
      Cstager_Segment_setBytes_in(
                                  segmItem,
                                  filereq->bytes_in
                                  );
      Cstager_Segment_setBytes_out(
                                   segmItem,
                                   filereq->bytes_out
                                   );
      Cstager_Segment_setHost_bytes(
                                    segmItem,
                                    filereq->host_bytes
                                    );
      Cstager_Segment_setSegmCksumAlgorithm(
                                            segmItem,
                                            filereq->castorSegAttr.segmCksumAlgorithm
                                            );
      Cstager_Segment_setSegmCksum(
                                   segmItem,
                                   filereq->castorSegAttr.segmCksum
                                   );
    }
    if ( newStatus == SEGMENT_FAILED ) {
      if (filereq->err.errorcode <= 0)
        filereq->err.errorcode = SEINTERNAL;
      Cstager_Segment_setErrorCode(segmItem,
                                   filereq->err.errorcode);
      
      if (filereq->err.severity == RTCP_OK)
        filereq->err.severity = RTCP_FAILED|RTCP_UNERR;
      Cstager_Segment_setSeverity(segmItem,
                                  filereq->err.severity);
      
      if (*filereq->err.errmsgtxt == '\0')
        strncpy(filereq->err.errmsgtxt,
                sstrerror(filereq->err.errorcode),
                sizeof(filereq->err.errmsgtxt)-1);
      Cstager_Segment_setErrMsgTxt(segmItem,
                                   filereq->err.errmsgtxt);
    }

    iObj = Cstager_Segment_getIObject(segmItem);
    Cstager_Segment_id(segmItem,&_key);
    if ( rc != -1 && svcs != NULL && *svcs != NULL ) {
      rc = C_Services_updateRep(*svcs,iAddr,iObj,1);
      save_serrno = serrno;
    }
    
    if ( rc == -1 ) {
      save_serrno = serrno;
      LOG_DBCALL_ERR((*svcs == NULL ? "getDbSvcs()" : "C_Services_updateRep()"),
                     (*svcs == NULL ? "(null)" : C_Services_errorMsg(*svcs)));
      C_IAddress_delete(iAddr);
      serrno = save_serrno;
      return(-1);
    }
    C_IAddress_delete(iAddr);
  } else {
    serrno = ENOENT;
    return(-1);
  }

  return(0);
}

