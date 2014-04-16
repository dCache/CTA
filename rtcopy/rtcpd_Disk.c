/*
 * Copyright (C) 1999-2004 by CERN IT
 * All rights reserved
 */

/*
 * rtcpd_Disk.c - RTCOPY server disk IO thread 
 */

#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/types.h>                  /* Standard data types          */
#include <netdb.h>                      /* Network "data base"          */
#include <sys/socket.h>
#include <netinet/in.h>                 /* Internet data types          */
#include <sys/time.h>

#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>

#include <rtcp_xroot.h>

#include <pwd.h>
#include <Castor_limits.h>
#include <Cglobals.h>
#include <log.h>
#include <osdep.h>
#include <net.h>
#include <Cthread_api.h>
#include <Cpool_api.h>
#include <vdqm_api.h>
#include <Ctape_api.h>
#include <Cuuid.h>
#include <rtcp_constants.h>
#include <rtcpd_constants.h>
#include <rtcp.h>
#include <rtcp_server.h>
#include <serrno.h>
#include <u64subr.h>
#include "tplogger_api.h"

#define DK_STATUS(X) (diskIOstatus->current_activity = (X))
#define DK_SIZE(X)   (diskIOstatus->nbbytes = (X))
#define DEBUG_PRINT(X) {if ( debug == TRUE ) rtcp_log X ;}

extern char *getifnam (int);

typedef struct thread_arg {
    int client_socket;
    int pool_index;              /* Pool index of this thread */
    int start_indxp;             /* Index of start buffer */
    int start_offset;            /* Byte offset withint start buffer */
    int last_file;               /* TRUE if this is the last disk file */
    int end_of_tpfile;           /* TRUE if last buffer for this file
                                  * should be marked end_to_tpfile=TRUE */
    tape_list_t *tape;
    file_list_t *file;
    rtcpClientInfo_t *client;
} thread_arg_t;

thread_arg_t *thargs = NULL;

extern int Debug;

extern int nb_bufs;

extern int bufsz;

extern processing_cntl_t proc_cntl;

extern processing_status_t proc_stat;

extern buffer_table_t **databufs;

extern rtcpClientInfo_t *client;

extern int AbortFlag;

int success = 0;
int failure = -1;

/**
 * Convert the server:/path string to the CASTOR xroot
 * root://server:port//dummy?castor2fs.pfn1=path&castor2fs.pfn2=dummy
 * @param rtcpPath the path to be converted.
 * @param xrootFilePath the variable to return xroot path.
 * @return 0 if convertion succeed and -1 in error case.
 */
static int rtcpToCastorXroot(const char *const rtcpPath,
                             char *const xrootFilePath) {  
    const char *const xrootPort="1095";
    const char *pathWithoutServer;
    extern char *getconfent();
    *xrootFilePath='\0';
    if ( CA_MAXPATHLEN <
      (strlen("root://")+strlen(xrootPort)+strlen(rtcpPath)+
       strlen("//dummy?castor2fs.pfn1=&castor2fs.pfn2=dummy&")+
       strlen("streamout=xxx&streamin=xxx")) ) {
         return (-1);
    }
    if ( NULL == (pathWithoutServer=strchr(rtcpPath,'/'))) {
      return (-1);
    }
    /* protocol */
    strcat(xrootFilePath,"root://");
    /* server name */
    strncat(xrootFilePath,rtcpPath, pathWithoutServer-rtcpPath);
    /* port */
    strcat(xrootFilePath,xrootPort);
    /* castor2fs.pfn1 */
    strcat(xrootFilePath,"//dummy?castor2fs.pfn1=");
    strcat(xrootFilePath,pathWithoutServer);
    /* castor2fs.pfn2 */
    strcat(xrootFilePath,"&castor2fs.pfn2=dummy");
    return (0);
}

static int DiskIOstarted() {
    int rc;

    rc = Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
    if ( rc == -1 ) {
        rtcp_log(LOG_ERR,"DiskIOstarted(): Cthread_mutex_lock_ext(proc_cntl): %s\n",
            sstrerror(serrno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"       , TL_MSG_PARAM_STR, "DiskIOstarted",
                         "Message"    , TL_MSG_PARAM_STR, "Cthread_mutex_lock_ext(proc_cntl)",
                         "Client Name", TL_MSG_PARAM_STR, sstrerror(serrno) );
        return(-1);
    }
    proc_cntl.diskIOstarted = 1;
    rtcp_log(LOG_DEBUG,"DiskIOstarted() nb active disk IO threads=%d\n",
             proc_cntl.nb_diskIOactive);
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                     "func"   , TL_MSG_PARAM_STR, "DiskIOstarted",
                     "Message", TL_MSG_PARAM_STR, "nb active disk IO threads",
                     "Number" , TL_MSG_PARAM_INT, proc_cntl.nb_diskIOactive );
    rc = Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
    if ( rc == -1 ) {
        rtcp_log(LOG_ERR,"DiskIOstarted(): Cthread_cond_broadcast_ext(proc_cntl): %s\n",
            sstrerror(serrno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"   , TL_MSG_PARAM_STR, "DiskIOstarted",
                         "Message", TL_MSG_PARAM_STR, "Cthread_cond_broadcast_ext(proc_cntl)",
                         "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
        (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
        return(-1);
    }
    rc = Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
    if ( rc == -1 ) {
        rtcp_log(LOG_ERR,"DiskIOstarted(): Cthread_mutex_unlock_ext(proc_cntl): %s\n",
            sstrerror(serrno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"   , TL_MSG_PARAM_STR, "DiskIOstarted",
                         "Message", TL_MSG_PARAM_STR, "Cthread_mutex_unlock_ext(proc_cntl)",
                         "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
        return(-1);
    }
    return(0);
}

static void DiskIOfinished() {
    (void)Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
    proc_cntl.nb_diskIOactive--;
    rtcp_log(LOG_DEBUG,"DiskIOfinished() nb active disk IO threads=%d\n",
             proc_cntl.nb_diskIOactive);
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                     "func"   , TL_MSG_PARAM_STR, "DiskIOfinished",
                     "Message", TL_MSG_PARAM_STR, "nb active disk IO threads",
                     "Number" , TL_MSG_PARAM_INT, proc_cntl.nb_diskIOactive );
    (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
    (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
    return;
}

static int LockForAppend(const int lock) {
    static int nb_waiters = 0;
    static int next_entry = 0;
    static int *wait_list = NULL;

    rtcp_log(LOG_DEBUG,"LockForAppend(%d) current_lock=%d\n",
             lock,proc_cntl.DiskFileAppend);
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                     "func"        , TL_MSG_PARAM_STR, "LockForAppend",
                     "Message"     , TL_MSG_PARAM_STR, "current_lock",
                     "Lock"        , TL_MSG_PARAM_INT, lock,
                     "Current Lock", TL_MSG_PARAM_INT, proc_cntl.DiskFileAppend );
    return(rtcpd_SerializeLock(lock,&proc_cntl.DiskFileAppend,
           proc_cntl.DiskFileAppend_lock,&nb_waiters,&next_entry,&wait_list));
}

static int DiskFileOpen(int pool_index, 
                        tape_list_t *tape,
                        file_list_t *file) {
    rtcpTapeRequest_t *tapereq;
    rtcpFileRequest_t *filereq;
    int rc, irc, save_errno;
    int disk_fd, flags, severity;
    diskIOstatus_t *diskIOstatus = NULL;
    int binmode = 0;

    rc = irc = 0;
    disk_fd = -1;
    if ( tape == NULL || file == NULL ) return(-1);

    diskIOstatus = &proc_stat.diskIOstatus[pool_index];
    tapereq = &tape->tapereq;
    filereq = &file->filereq;
    umask((mode_t)filereq->umask);

    /*
     * If this file is concatenating or is going to be
     * concatenated we must serialize the access to it.
     */
    if ( (tapereq->mode == WRITE_DISABLE) && 
         ( (file->next->filereq.concat & CONCAT) != 0 ||
           (filereq->concat & (CONCAT|CONCAT_TO_EOD)) != 0 )) {
        rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
        if ( (severity & RTCP_EOD) != 0 ) return(-1);
        (*logfunc)(LOG_DEBUG,"DiskFileOpen(%s) lock file for concatenation\n",
            filereq->file_path);
        rc = LockForAppend(1);
        rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
        if ( rc == -1 ) {
             rtcp_log(LOG_ERR,"DiskFileOpen(%s) LockForAppend(0): %s\n",
                     filereq->file_path,sstrerror(serrno));
             tl_rtcpd.tl_log( &tl_rtcpd, 3, 4, 
                              "func"     , TL_MSG_PARAM_STR, "DiskFileOpen",
                              "Message"  , TL_MSG_PARAM_STR, "LockForAppend",
                              "File Path", TL_MSG_PARAM_STR, filereq->file_path,
                              "Error"    , TL_MSG_PARAM_STR, sstrerror(serrno) );
            return(-1);
        }
        if ( (severity & RTCP_EOD) != 0 ) return(-1);
    }

        /*
         * Normal Formatted file option. 
         * Open the disk file as byte-stream.
         */
        flags = O_RDONLY | binmode;
        if ( tapereq->mode == WRITE_DISABLE ) {
            if ( (filereq->concat & (CONCAT | CONCAT_TO_EOD)) != 0 ) {
                /*
                 * Appending to previous disk file. If concat to EOD we
                 * must check if it is the first file in sequence so that
                 * we don't append to an existing file....
                 */
                if ( (filereq->concat & CONCAT_TO_EOD) != 0 &&
                     (filereq->concat & CONCAT) == 0 &&
                     ((file->prev->filereq.concat & CONCAT_TO_EOD) == 0 ||
                      file == tape->file) ) {
                    flags = O_CREAT | O_WRONLY | O_TRUNC | binmode;
                } else {
                  flags = O_CREAT | O_WRONLY | O_APPEND | binmode;
                }
            } else {
                /*
                 * New disk file unless an offset has been specified.
                 * New stager will specify concat==OPEN_NOTRUNC in order
                 * to allow for efficient recall of multi-segment files
                 * (segments can be copied in any order).
                 */
                if ( (filereq->offset > 0) || 
                     (filereq->concat == OPEN_NOTRUNC) ) {
                    flags = O_CREAT| O_WRONLY | binmode;
                } else {
                  flags = O_CREAT | O_WRONLY | O_TRUNC | binmode;
                }
            }
        }
        errno = 0;
        rtcp_log(LOG_DEBUG,"DiskFileOpen() open(%s,0x%x)\n",filereq->file_path,
            flags);
        {
                char __flags[32];
                sprintf( __flags, "0x%x", flags );
                tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                                 "func"     , TL_MSG_PARAM_STR, "DiskFileOpen",
                                 "Message"  , TL_MSG_PARAM_STR, "open",
                                 "File Path", TL_MSG_PARAM_STR, filereq->file_path,
                                 "Flags"    , TL_MSG_PARAM_STR, __flags );
        }
        DK_STATUS(RTCP_PS_OPEN);

        char xrootFilePath[CA_MAXPATHLEN+1];
        if ( -1 == rtcpToCastorXroot(filereq->file_path,xrootFilePath) ) {
            errno = EFAULT; /* sets  "Bad address" errno */
            rc = -1;
        } else {
            rc = rtcp_xroot_open(xrootFilePath, flags, 0666); 
            rtcp_log(LOG_DEBUG,"rtcp_xroot_open for %s\n",xrootFilePath);
        }  

        DK_STATUS(RTCP_PS_NOBLOCKING);
        if ( rc == -1 ) { 
            save_errno = errno;
            rtcp_log(LOG_ERR,
                "DiskFileOpen() open(%s,0x%x): errno = %d\n",
                filereq->file_path,flags,errno);
            {
                    char __flags[32];
                    sprintf( __flags, "0x%x", flags );
                    tl_rtcpd.tl_log( &tl_rtcpd, 3, 5, 
                                     "func"      , TL_MSG_PARAM_STR, "DiskFileOpen",
                                     "Message"   , TL_MSG_PARAM_STR, "open",
                                     "File Path" , TL_MSG_PARAM_STR, filereq->file_path,
                                     "Flags"     , TL_MSG_PARAM_STR, __flags,
                                     "errno"     , TL_MSG_PARAM_INT, errno );
            }
        } else {
            disk_fd = rc;
            rc = 0;
        }
        rtcp_log(LOG_DEBUG,"DiskFileOpen() open() returned fd=%d\n",
            disk_fd);
        tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                         "func"   , TL_MSG_PARAM_STR, "DiskFileOpen",
                         "Message", TL_MSG_PARAM_STR, "open returned",
                         "fd"     , TL_MSG_PARAM_INT, disk_fd );                         
        if ( rc == 0 && filereq->offset > 0 ) {
			char tmpbuf[21];
			char tmpbuf2[21];
			off64_t rc64;
            rtcp_log(LOG_DEBUG,"DiskFileOpen() attempt to set offset %s\n",
                     u64tostr((u_signed64) filereq->offset, tmpbuf, 0));
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                             "func"   , TL_MSG_PARAM_STR, "DiskFileOpen",
                             "Message", TL_MSG_PARAM_STR, "attempt to set offset",
                             "offset" , TL_MSG_PARAM_STR, u64tostr((u_signed64) filereq->offset, tmpbuf, 0) );
            errno = 0;

            rc64 = rtcp_xroot_lseek(disk_fd, (off64_t)filereq->offset,SEEK_SET);

            if ( rc64 == -1 ) {
                save_errno = errno;
                rtcp_log(LOG_ERR,
                 "DiskFileOpen() lseek64(%d,%s,0x%x): errno = %d\n",
                 disk_fd,u64tostr((u_signed64)filereq->offset,tmpbuf,0),SEEK_SET,errno);
                {
                        char __seek_set[32];
                        sprintf( __seek_set, "0x%x", SEEK_SET );
                        tl_rtcpd.tl_log( &tl_rtcpd, 3, 8, 
                                         "func"      , TL_MSG_PARAM_STR, "DiskFileOpen",
                                         "Message"   , TL_MSG_PARAM_STR, "lseek64",
                                         "disk_fd"   , TL_MSG_PARAM_INT, disk_fd,                                 
                                         "offset"    , TL_MSG_PARAM_STR, u64tostr((u_signed64) filereq->offset, tmpbuf, 0),
                                         "SEEK_SET"  , TL_MSG_PARAM_STR, SEEK_SET, 
                                         "errno"     , TL_MSG_PARAM_INT, errno );
                }
                rc = -1;
            } else if ( rc64 != (off64_t)filereq->offset ) {
                save_errno = errno;
                rtcp_log(LOG_ERR,"lseek64(%d,%s,%d) returned %s\n",
                         disk_fd,u64tostr((u_signed64)filereq->offset,tmpbuf,0),SEEK_SET,u64tostr((u_signed64)rc64,tmpbuf2,0));
                {
                        char __seek_set[32];
                        sprintf( __seek_set, "0x%x", SEEK_SET );
                        tl_rtcpd.tl_log( &tl_rtcpd, 3, 6, 
                                         "func"        , TL_MSG_PARAM_STR, "DiskFileOpen",
                                         "Message"     , TL_MSG_PARAM_STR, "lseek64 returned",
                                         "disk_fd"     , TL_MSG_PARAM_INT, disk_fd,                                 
                                         "offset"      , TL_MSG_PARAM_STR, u64tostr((u_signed64) filereq->offset, tmpbuf, 0),
                                         "SEEK_SET"    , TL_MSG_PARAM_STR, SEEK_SET, 
                                         "Return Value", TL_MSG_PARAM_STR, u64tostr((u_signed64)rc64,tmpbuf2,0) );
                }                
                if ( save_errno == 0 ) save_errno = SEINTERNAL;
                rc = -1;
            } else rc = 0;
        }

    if ( rc != 0 || irc != 0 ) {
        if ( tapereq->mode == WRITE_ENABLE ) 
            rtcpd_AppendClientMsg(NULL, file,RT110,"CPDSKTP", sstrerror(errno));
        else
            rtcpd_AppendClientMsg(NULL, file,RT110,"CPTPDSK", sstrerror(errno));
        switch (save_errno) {
        case ENOENT:
        case EISDIR:
        case EPERM:
        case EACCES:
            rtcpd_SetReqStatus(NULL,file,save_errno,RTCP_USERR | RTCP_FAILED);
            break;
        default:
            rtcpd_SetReqStatus(NULL,file,save_errno, RTCP_FAILED | RTCP_UNERR);
            break;
        }
        return(-1);
    }
    if ( disk_fd != -1 ) {
      strcpy(filereq->ifce,"Xrd");
      /* there is no way to know interface name for XrdPosix */
    }

    if (disk_fd>0) {

            tl_rtcpd.tl_log( &tl_rtcpd, 27, 4, 
                             "func"     , TL_MSG_PARAM_STR, "DiskFileOpen",
                             "Message"  , TL_MSG_PARAM_STR, "successful",
                             "File Path", TL_MSG_PARAM_STR, filereq->file_path,
                             "fd"       , TL_MSG_PARAM_INT, disk_fd );                                     
    }

    return(disk_fd);
}

static int DiskFileClose(int disk_fd,
                         int pool_index,
                         tape_list_t *tape,
                         file_list_t *file) {
    rtcpFileRequest_t *filereq;
    int rc = 0;
    int save_errno;

    if ( file == NULL ) return(-1);
    filereq = &file->filereq;

    rtcp_log(LOG_DEBUG,"DiskFileClose(%s) close file descriptor %d\n",
             filereq->file_path,disk_fd);
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                     "func"           , TL_MSG_PARAM_STR, "DiskFileClose",
                     "Message"        , TL_MSG_PARAM_STR, "close file descriptor",
                     "File Path"      , TL_MSG_PARAM_STR, filereq->file_path,
                     "File Descriptor", TL_MSG_PARAM_INT, disk_fd );

    rc = rtcp_xroot_close(disk_fd);
    save_errno = errno;
    if ( rc == -1 ) {
      rtcpd_AppendClientMsg(NULL, file,RT108,"CPTPDSK",sstrerror(errno)); /* 0 for Xrd */

        rtcp_log(LOG_ERR,"xroot_close(): errno = %d\n",save_errno);
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"      , TL_MSG_PARAM_STR, "DiskFileClose",
                         "Message"   , TL_MSG_PARAM_STR, "xroot_close()",
                         "errno"     , TL_MSG_PARAM_INT, save_errno );

        if (save_errno == ENOSPC) {
            rtcp_log(LOG_DEBUG,"DiskFileClose(%s) ENOSPC detected\n", filereq->file_path);
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                             "func"     , TL_MSG_PARAM_STR, "DiskFileClose",
                             "Message"  , TL_MSG_PARAM_STR, "ENOSPC detected",
                             "File Path", TL_MSG_PARAM_STR, filereq->file_path );
            rtcpd_SetReqStatus(NULL,file,save_errno,RTCP_FAILED);
        } else {
            rtcpd_SetReqStatus(NULL,file,save_errno,RTCP_FAILED);
        }
    }

    serrno = 0;
    if ( (tape->tapereq.mode == WRITE_DISABLE) &&
         ( (file->next->filereq.concat & CONCAT) != 0 ||
           (filereq->concat & (CONCAT|CONCAT_TO_EOD)) != 0 ) ) {
        rtcp_log(LOG_DEBUG,"DiskFileClose(%s) unlock file for concatenation\n",
                 filereq->file_path);
        tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                         "func"     , TL_MSG_PARAM_STR, "DiskFileClose",
                         "Message"  , TL_MSG_PARAM_STR, "unlock file for concatenation",
                         "File Path", TL_MSG_PARAM_STR, filereq->file_path );
        rc = LockForAppend(0);
        if ( rc == -1 && serrno > 0 ) {
            rtcp_log(LOG_ERR,"DiskFileClose(%s) LockForAppend(0): %s\n",
                     filereq->file_path,sstrerror(serrno));
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 4, 
                             "func"     , TL_MSG_PARAM_STR, "DiskFileClose",
                             "Message"  , TL_MSG_PARAM_STR, "LockForAppend(0)",
                             "File Path", TL_MSG_PARAM_STR, filereq->file_path,
                             "Error"    , TL_MSG_PARAM_STR, sstrerror(serrno) );
        }
    }
    filereq->TEndTransferDisk = (int)time(NULL);

    if (0==rc) {

            tl_rtcpd.tl_log( &tl_rtcpd, 38, 3, 
                             "func"     , TL_MSG_PARAM_STR, "DiskFileClose",
                             "Message"  , TL_MSG_PARAM_STR, "successful",
                             "File Path", TL_MSG_PARAM_STR, filereq->file_path );
    }

    return(rc);
}

/*
 * Copy routines
 */
static int MemoryToDisk(int disk_fd, int pool_index,
                        int *indxp, int *offset,
                        int *last_file, int *end_of_tpfile,
                        tape_list_t *tape,
                        file_list_t *file) {
    int rc, i, blksiz, lrecl, save_serrno;
    int last_errno = 0;
    int nb_bytes, proc_err, severity, SendStartSignal;
    register int debug = Debug;
    register int concat;
    diskIOstatus_t *diskIOstatus = NULL;
    char *bufp;
    rtcpFileRequest_t *filereq = NULL;

    if ( disk_fd < 0 || indxp == NULL || offset == NULL ||
         last_file == NULL || end_of_tpfile == NULL ||
         tape == NULL || file == NULL ) {
        serrno = EINVAL;
        return(-1);
    }

    filereq = &file->filereq;
    diskIOstatus = &proc_stat.diskIOstatus[pool_index];
    diskIOstatus->disk_fseq = filereq->disk_fseq;
    blksiz = lrecl = -1;
    concat = filereq->concat;

    /*
     * Main write loop. End with EOF on tape file or error condition
     */
    *end_of_tpfile = FALSE;
    file->diskbytes_sofar = filereq->startsize;
    DK_SIZE(file->diskbytes_sofar);
    save_serrno = 0;
    proc_err = 0;
    SendStartSignal = TRUE;
    for (;;) {
        i = *indxp;
        /*
         * Synchronize access to next buffer
         */
        DK_STATUS(RTCP_PS_WAITMTX);
        rc = Cthread_mutex_lock_ext(databufs[i]->lock);
        DK_STATUS(RTCP_PS_NOBLOCKING);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"MemoryToDisk() Cthread_mutex_lock_ext(): %s\n",
                sstrerror(serrno));
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                             "Message", TL_MSG_PARAM_STR, "Cthread_mutex_lock_ext",
                             "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
            return(-1);
        }
        /*
         * Wait until it is full
         */
        while ( databufs[i]->flag == BUFFER_EMPTY ) {
            rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
            if ( (proc_err = ((severity | rtcpd_CheckProcError()) & 
                  RTCP_FAILED)) != 0 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                break;
            }

            DEBUG_PRINT((LOG_DEBUG,"MemoryToDisk() wait on buffer[%d]->flag=%d\n",
                    i,databufs[i]->flag));
            databufs[i]->nb_waiters++;
            DK_STATUS(RTCP_PS_WAITCOND);
            rc = Cthread_cond_wait_ext(databufs[i]->lock);
            DK_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToDisk() Cthread_cond_wait_ext(): %s\n",
                    sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                                 "Message", TL_MSG_PARAM_STR, "Cthread_cond_wait_ext",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }

            databufs[i]->nb_waiters--;

        } /* while (databufs[i]->flag == BUFFER_EMPTY) */

        if ( databufs[i]->flag == BUFFER_FULL ) {
            rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
            if ( (proc_err = ((severity | rtcpd_CheckProcError()) & 
                RTCP_FAILED)) != 0 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            }
        }

        /*
         * At this point we know that the tape->memory has
         * started. Thus it is now safe to take the blocksize and
         * record length. Before this was not the case because
         * for tape read the call to Ctape_info() in the
         * tape IO thread is not synchronised with the start up
         * of the disk IO thread.
         */
        if ( blksiz < 0 ) {
            blksiz = filereq->blocksize;
            lrecl = filereq->recordlength;
            if ( lrecl <= 0 ) {
                lrecl = blksiz;
            }
            filereq->TStartTransferDisk = (int)time(NULL);
        }

        /*
         * Check if reached an allowed end-of-tape
         */
        if ( (proc_err & RTCP_FAILED) == 0 && 
             (concat & (NOCONCAT_TO_EOD|CONCAT_TO_EOD)) != 0 ) { 
            rtcpd_CheckReqStatus(file->tape,file,NULL,&proc_err);
            if ( (proc_err = (proc_err & RTCP_EOD)) != 0 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            }
        }
        if ( proc_err != 0 ) break;
        DEBUG_PRINT((LOG_DEBUG,"MemoryToDisk() buffer %d full\n",i));
        if ( SendStartSignal == TRUE ) {
            /*
             * Signal to StartDiskIO() that we are starting to empty the
             * first buffer.
             */
            DK_STATUS(RTCP_PS_WAITMTX);
            rc = DiskIOstarted();
            DK_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
            SendStartSignal = FALSE;
        }

        /*
         * Should never happen unless there is a bug.
         */
        if ( (databufs[i]->data_length > databufs[i]->maxlength) ||
             (databufs[i]->data_length > databufs[i]->length) ) {
            rtcp_log(LOG_ERR,"Buffer overflow!! databuf %d, (%d,%d,%d)\n",
                     i,databufs[i]->data_length,databufs[i]->length,
                     databufs[i]->maxlength);
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 6, 
                             "func"       , TL_MSG_PARAM_STR, "MemoryToDisk",
                             "Message"    , TL_MSG_PARAM_STR, "Buffer overflow",
                             "Data Buffer", TL_MSG_PARAM_INT, i, 
                             "Data Length", TL_MSG_PARAM_INT, databufs[i]->data_length, 
                             "Length"     , TL_MSG_PARAM_INT, databufs[i]->length, 
                             "Max Length" , TL_MSG_PARAM_INT, databufs[i]->maxlength );
            rtcpd_AppendClientMsg(NULL,file,"Internal error. %s: buffer overflow\n",
                                  filereq->file_path);
            rtcpd_SetReqStatus(NULL,file,SEINTERNAL,RTCP_FAILED);
            (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
            (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            return(-1);
        }
        /*
         * Verify that actual buffer size matches block size
         */
        if ( (databufs[i]->length % blksiz) != 0 || blksiz < 0 ) {
            rtcp_log(LOG_ERR,"MemoryToDisk() blocksize mismatch\n");
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 2, 
                             "func"       , TL_MSG_PARAM_STR, "MemoryToDisk",
                             "Message"    , TL_MSG_PARAM_STR, "blocksize mismatch" );
            rtcpd_AppendClientMsg(NULL, file, "Internal error. %s: blocksize mismatch (%d,%d)\n",
                        filereq->file_path,databufs[i]->length,blksiz);
            rtcpd_SetReqStatus(NULL,file,SEINTERNAL,RTCP_FAILED);
            (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
            (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            return(-1);
        }
        /*
         * Check if this is the last buffer of the tape file
         * which implies that we should return.
         */
        *end_of_tpfile = databufs[i]->end_of_tpfile;
        /*
         * Copy the data from memory to disk. Note that offset should
         * always be zero for tape->disk copy because new files will
         * begin with a new buffer.
         */
        if ( databufs[i]->data_length > 0 ) {
            nb_bytes = databufs[i]->data_length;
            /*
             * Check that the total size does not exceed maxsize specified
             * by user. If so, reduce the nb bytes accordingly. The tape IO
             * should already have stopped in this buffer but data_length
             * is a multiple of blocksize and may therefore exceed maxsize.
             */
            if ( (filereq->maxsize > 0) && (file->diskbytes_sofar + 
                  (u_signed64)nb_bytes > filereq->maxsize) ) {
                nb_bytes = (int)(filereq->maxsize - file->diskbytes_sofar);
                databufs[i]->data_length = nb_bytes;
            }
            if ( nb_bytes > 0 ) {
                /*
                 * >>>>>>>>>>> write to disk <<<<<<<<<<<<<
                 */
                    bufp = databufs[i]->buffer;
                    DK_STATUS(RTCP_PS_WRITE);
                    if ( nb_bytes > 0 ) {
                      rc = rtcp_xroot_write(disk_fd,bufp,nb_bytes);
                    } else rc = nb_bytes;
                    DK_STATUS(RTCP_PS_NOBLOCKING);
                    if ( rc == -1 || rc != nb_bytes ) {
                        last_errno = errno;
                        rtcp_log(LOG_ERR,"write(): errno = %d\n",last_errno);
                        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                         "func"      , TL_MSG_PARAM_STR, "MemoryToDisk",
                                         "Message"   , TL_MSG_PARAM_STR, "write",
                                         "last_errno", TL_MSG_PARAM_INT, last_errno );
                    }
                if ( rc != nb_bytes ) {
                    /*
                     * In case of ENOSPC we will have to return
                     * to ask the stager for a new path
                     */
                    rtcpd_AppendClientMsg(NULL, file,RT115,"CPTPDSK",sstrerror(errno));

                    if (last_errno == ENOSPC) {
                         save_serrno = ENOSPC;
                         rtcp_log(LOG_DEBUG,"MemoryToDisk(%s) ENOSPC detected\n",
                                  filereq->file_path);
                         tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                          "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                                          "Message", TL_MSG_PARAM_STR, "ENOSPC detected",
                                          "Path"   , TL_MSG_PARAM_STR, filereq->file_path ); 
                         rtcpd_SetReqStatus(NULL,file,save_serrno, RTCP_FAILED);
                    }
                    if ( save_serrno != ENOSPC ) {
                        rtcpd_SetReqStatus(NULL,file,save_serrno, RTCP_FAILED);
                    }
                    (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                    (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                    serrno = save_serrno;
                    return(-1);
                }
            } else {
                /*
                 * Max size exceeded. Just skip over the buffer
                 * to assure that it is marked as free for the
                 * Tape IO thread. 
                 */
                rc = databufs[i]->data_length;
            }
        } else {
            rc = 0;
        }
        databufs[i]->data_length -= rc;

        file->diskbytes_sofar += (u_signed64)rc;
        DK_SIZE(file->diskbytes_sofar);
        /*
         * Reset the buffer semaphore only if the
         * full buffer has been succesfully written.
         */
        if ( databufs[i]->data_length == 0 ) {
            databufs[i]->bufendp = 0;
            databufs[i]->data_length = 0;
            databufs[i]->end_of_tpfile = FALSE;
            databufs[i]->last_buffer = FALSE;
            databufs[i]->flag = BUFFER_EMPTY;
        }

        /*
         * Signal and release this buffer
         */
        if ( databufs[i]->nb_waiters > 0 ) {
            rc = Cthread_cond_broadcast_ext(databufs[i]->lock);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToDisk() Cthread_cond_broadcast_ext(): %s\n",
                    sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                                 "Message", TL_MSG_PARAM_STR, "Cthread_cond_broadcast_ext",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
        }
        rc = Cthread_mutex_unlock_ext(databufs[i]->lock);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"MemoryToDisk() Cthread_mutex_unlock_ext(): %s\n",
                sstrerror(serrno));
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                             "Message", TL_MSG_PARAM_STR, "Cthread_mutex_unlock_ext",
                             "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
            return(-1);
        }

        if ( *end_of_tpfile == TRUE ) {
            /*
             * End of tape file reached. Close disk file and tell the
             * main control thread that we exit
             */
            DEBUG_PRINT((LOG_DEBUG,"MemoryToDisk() close disk file fd=%d\n",
                         disk_fd));
            rc = DiskFileClose(disk_fd,pool_index,tape,file);
            save_serrno = errno;
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToDisk() DiskFileClose(%d): %s\n",
                         disk_fd,sstrerror(errno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 4, 
                                 "func"       , TL_MSG_PARAM_STR, "MemoryToDisk",
                                 "Message"    , TL_MSG_PARAM_STR, "DiskFileClose",
                                 "disk_fd"    , TL_MSG_PARAM_INT, disk_fd,
                                 "error"      , TL_MSG_PARAM_STR, sstrerror(errno) );

                if ( save_serrno == ENOSPC ) {
                    rtcp_log(LOG_DEBUG,"MemoryToDisk(%s) ENOSPC detected\n",
                        filereq->file_path);
                    tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                     "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                                     "Message", TL_MSG_PARAM_STR, "ENOSPC detected",
                                     "Path"   , TL_MSG_PARAM_STR, filereq->file_path );
                    rtcpd_SetReqStatus(NULL,file,save_serrno,RTCP_FAILED);
                }
                serrno = save_serrno;
                return(-1);
            }
            break;
        }
        *indxp = (*indxp + 1) % nb_bufs;
        *offset = 0;

        /*
         * Has something fatal happened while we were occupied
         * reading from the disk? 
         */
        rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
        if ( (proc_err = ((severity | rtcpd_CheckProcError()) & 
              RTCP_FAILED)) != 0 ) break;
    } /* for (;;) */
    
    if ( proc_err != 0 ) DiskFileClose(disk_fd,pool_index,tape,file);
    return(0);
}
static int DiskToMemory(int disk_fd, int pool_index,
                        int *indxp, int *offset,
                        int *last_file, int *end_of_tpfile,
                        tape_list_t *tape,
                        file_list_t *file) {
    int rc, irc, i, blksiz, lrecl, end_of_dkfile, current_bufsz;
    int nb_bytes, SendStartSignal, save_errno, proc_err, severity;
    register int debug = Debug;
    diskIOstatus_t *diskIOstatus = NULL;
    char *bufp;
    rtcpFileRequest_t *filereq = NULL;

    if ( disk_fd < 0 || indxp == NULL || offset == NULL ||
         tape == NULL || file == NULL ) {
        serrno = EINVAL;
        return(-1);
    }
    filereq = &file->filereq;
    diskIOstatus = &proc_stat.diskIOstatus[pool_index];
    diskIOstatus->disk_fseq = filereq->disk_fseq;

    blksiz = filereq->blocksize;
    lrecl = filereq->recordlength;
    if ( lrecl <= 0 ) {
        lrecl = blksiz;
    }
    
    /*
     * Calculate new actual buffer length
     */
    current_bufsz = rtcpd_CalcBufSz(tape,file);
    if ( current_bufsz <= 0 ) return(-1);

    /*
     * Main read loop. End with EOF or error condition (or
     * limited by size).
     */
    end_of_dkfile = FALSE;
    SendStartSignal = TRUE;
    proc_err = 0;
    file->diskbytes_sofar = filereq->startsize;
    DK_SIZE(file->diskbytes_sofar);
    filereq->TStartTransferDisk = (int)time(NULL);
    for (;;) {
        i = *indxp;
        /*
         * Synchronize access to next buffer
         */
        DK_STATUS(RTCP_PS_WAITMTX);
        rc = Cthread_mutex_lock_ext(databufs[i]->lock);
        DK_STATUS(RTCP_PS_NOBLOCKING);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"DiskToMemory() Cthread_mutex_lock_ext(): %s\n",
                sstrerror(serrno));
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"   , TL_MSG_PARAM_STR, "DiskToMemory",
                             "Message", TL_MSG_PARAM_STR, "Cthread_mutex_lock_ext",
                             "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
            return(-1);
        }
        /*
         * Wait until it is empty. 
         */
        while ( databufs[i]->flag == BUFFER_FULL ) {
            rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
            if ( (proc_err = ((severity | rtcpd_CheckProcError()) & 
                  RTCP_FAILED)) != 0 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                break;
            }

            databufs[i]->nb_waiters++;
            DK_STATUS(RTCP_PS_WAITCOND);
            rc = Cthread_cond_wait_ext(databufs[i]->lock);
            DK_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"DiskToMemory() Cthread_cond_wait_ext(): %s\n",
                    sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "DiskToMemory",
                                 "Message", TL_MSG_PARAM_STR, "Cthread_cond_wait_ext",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
            databufs[i]->nb_waiters--;

        } /* while ( databufs[i]->flag == BUFFER_FULL ) */

        if ( databufs[i]->flag == BUFFER_EMPTY ) {
            rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
            if ( (proc_err = ((severity | rtcpd_CheckProcError()) & 
                RTCP_FAILED)) != 0 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            }
        }

        if ( proc_err != 0 ) break;
        DEBUG_PRINT((LOG_DEBUG,"DiskToMemory() buffer %d empty\n",i));
        if ( SendStartSignal == TRUE ) {
            /*
             * Signal to StartDiskIO() that we are starting to fill the
             * first buffer. 
             */
            DK_STATUS(RTCP_PS_WAITMTX);
            rc = DiskIOstarted();
            DK_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
            SendStartSignal = FALSE;
        }
        /*
         * Set the actual buffer size to match current block size
         */
        databufs[i]->length = current_bufsz;
        /*
         * Copy the data from disk to memory
         */
        nb_bytes = databufs[i]->length-*offset;
        /*
         * Check that the total size does not exceed maxsize specified
         * by user. If so, reduce the nb bytes accordingly.
         */
        if ( (filereq->maxsize > 0) && (file->diskbytes_sofar +
              (u_signed64)nb_bytes > filereq->maxsize) ) {
            nb_bytes = (int)(filereq->maxsize - file->diskbytes_sofar);
            end_of_dkfile = TRUE;
        }

        /*
         * >>>>>>>>>>> read from disk <<<<<<<<<<<<<
         */
        DEBUG_PRINT((LOG_DEBUG,"DiskToMemory() read %d bytes from %s\n",
            nb_bytes,filereq->file_path));
        rc = irc = 0;
        errno = 0;
            bufp = databufs[i]->buffer + *offset;
            DK_STATUS(RTCP_PS_READ);
            if ( nb_bytes > 0 ) {
              rc = rtcp_xroot_read(disk_fd,bufp,nb_bytes); 
            } else rc = nb_bytes;
            DK_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) save_errno = errno;
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"DiskToMemory() read(): errno = %d\n",errno);
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"      , TL_MSG_PARAM_STR, "DiskToMemory",
                             "Message"   , TL_MSG_PARAM_STR, "read",
                             "errno"     , TL_MSG_PARAM_INT, errno );
            rtcpd_AppendClientMsg(NULL, file,RT112,"CPDSKTP", sstrerror(save_errno));
            rtcpd_SetReqStatus(NULL,file,save_errno,RTCP_FAILED);
            (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
            (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            return(-1);
        }
        DEBUG_PRINT((LOG_DEBUG,"DiskToMemory() got %d bytes from %s\n",
            rc,filereq->file_path));
        databufs[i]->data_length += rc;
        file->diskbytes_sofar += (u_signed64)rc;
        DK_SIZE(file->diskbytes_sofar);
        if ( file->diskbytes_sofar - filereq->startsize > filereq->bytes_in ) {
            /*
             * This can happen if the user still writes to the file after
             * having submit the tape write request for it.
             */
            rtcp_log(LOG_ERR,"File %s: size changed during request!\n",
                     filereq->file_path);
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"   , TL_MSG_PARAM_STR, "MemoryToDisk",
                             "Message", TL_MSG_PARAM_STR, "File size changed during request!",
                             "File"   , TL_MSG_PARAM_STR, filereq->file_path );
            rtcpd_AppendClientMsg(NULL,file,RT150,"CPDSKTP",filereq->file_path);
            rtcpd_SetReqStatus(NULL,file,SEWOULDBLOCK,RTCP_USERR | RTCP_FAILED);
            (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
            (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            serrno = SEWOULDBLOCK;
            return(-1);
        }

        if ( (end_of_dkfile == TRUE) || (rc < nb_bytes)) {
            DEBUG_PRINT((LOG_DEBUG,"DiskToMemory() End of file %s reached in buffer %d\n",
                filereq->file_path,i));
            if ( databufs[i]->end_of_tpfile == FALSE ) 
                databufs[i]->end_of_tpfile = *end_of_tpfile;
            databufs[i]->last_buffer = *last_file;
            if ( *offset + rc > databufs[i]->bufendp )
                databufs[i]->bufendp = *offset + rc;
            end_of_dkfile = TRUE;
        } else {
            databufs[i]->bufendp = databufs[i]->length;
        }

        /*
         * Mark the buffer as full if:
         * 1) it is full (!), i.e. data_length == length
         * 2) it is the last buffer of a tape file and
         *    all data are available (i.e. data length ==
         *    buffer end).
         * The condition 2) is complicated because of concatenation
         * of disk files into a single tape file. Note that it is
         * not enough to know that the disk IO thread for the last
         * disk file has finished (i.e. the end_of_tpfile or
         * last_file flags are set), there can be other disk IO
         * threads still active writing to another piece of this
         * buffer. This can typically happen when concatenating
         * very small disk files that all fit into a single
         * memory buffer.
         */
        if ( (databufs[i]->data_length == databufs[i]->length) ||
            ((databufs[i]->data_length == databufs[i]->bufendp) &&
             (databufs[i]->end_of_tpfile == TRUE ||
              databufs[i]->last_buffer == TRUE)) ) {
            databufs[i]->flag = BUFFER_FULL;
        }
        if ( databufs[i]->flag == BUFFER_FULL ) (void)rtcpd_nbFullBufs(1);
        /*
         * Signal and release this buffer
         */
        if ( databufs[i]->nb_waiters > 0 ) {
            rc = Cthread_cond_broadcast_ext(databufs[i]->lock);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"DiskToMemory() Cthread_cond_broadcast_ext(): %s\n",
                    sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "DiskToMemory",
                                 "Message", TL_MSG_PARAM_STR, "Cthread_cond_broadcast_ext",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
        }

        rc = Cthread_mutex_unlock_ext(databufs[i]->lock);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"DiskToMemory() Cthread_mutex_unlock_ext(): %s\n",
                sstrerror(serrno));
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"   , TL_MSG_PARAM_STR, "DiskToMemory",
                             "Message", TL_MSG_PARAM_STR, "Cthread_mutex_unlock_ext",
                             "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
            return(-1);
        }

        if ( end_of_dkfile ) {
            /*
             * End of disk file reached. Close it and return to pool.
             */
            DEBUG_PRINT((LOG_DEBUG,"DiskToMemory() close disk file fd=%d\n",
                disk_fd));
            rc = DiskFileClose(disk_fd,pool_index,tape,file);
            save_errno = errno;
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"DiskToMemory() DiskFileClose(%d), file=%s: %s\n",
                         disk_fd,filereq->file_path,sstrerror(save_errno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 5, 
                                 "func"   , TL_MSG_PARAM_STR, "DiskToMemory",
                                 "Message", TL_MSG_PARAM_STR, "DiskFileClose",
                                 "disk_fd", TL_MSG_PARAM_INT, disk_fd,
                                 "Path"   , TL_MSG_PARAM_STR, filereq->file_path,
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(save_errno) );
                rtcpd_AppendClientMsg(NULL, file,RT108,"CPDSKTP",sstrerror(save_errno));
                rtcpd_SetReqStatus(NULL,file,errno,RTCP_FAILED);
                return(-1);
            }
            break;
        }
        *indxp = (*indxp + 1) % nb_bufs;
        *offset = 0;

        /*
         * Has something fatal happened while we were occupied
         * reading from the disk?
         */
        rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
        if ( (proc_err = ((severity | rtcpd_CheckProcError()) & 
              RTCP_FAILED)) != 0 ) break;
    } /* for (;;) */
    
    if ( proc_err != 0 ) DiskFileClose(disk_fd,pool_index,tape,file);
    return(0);
}

/*
 * This horrible macro prevents us to always repeate the same code.
 * In addition to the return code from any blocking (or non-blocking) 
 * call we need to check the setting of the global processing error
 * to see if e.g. a tape IO thread has failed.
 * We have to be careful in setting processing error: if we are writing
 * a tape the tape IO is behind us and we have to let it finish with
 * previous file(s). Once it reach the current file it will stop because
 * the file request status is FAILED. On tape read at the other hand
 * we are behind the tape IO so we can safely interrupt everything in
 * case of an error.
 */
#define CHECK_PROC_ERR(X,Y,Z) { \
    save_errno = errno; \
    rtcpd_CheckReqStatus((X),(Y),NULL,&severity); \
    if ( rc == -1 || (severity & RTCP_FAILED) != 0 || \
        (rtcpd_CheckProcError() & RTCP_FAILED) != 0 ) { \
        rtcp_log(LOG_ERR,"diskIOthread() %s, rc=%d, severity=%d, errno=%d\n",\
        (Z),rc,severity,save_errno); \
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 5, \
                         "func"    , TL_MSG_PARAM_STR, "diskIOthread", \
                         "Message" , TL_MSG_PARAM_STR, (Z), \
                         "rc"      , TL_MSG_PARAM_INT, rc, \
                         "severity", TL_MSG_PARAM_INT, severity, \
                         "errno"   , TL_MSG_PARAM_INT, save_errno ); \
        if ( mode == WRITE_DISABLE && \
          (rc == -1 || (severity & RTCP_FAILED) != 0) && \
          (rtcpd_CheckProcError() & RTCP_FAILED) == 0 ) { \
            (void)rtcpd_WaitCompletion(tape,file); \
            if ( (severity & RTCP_FAILED) != 0 ) \
                rtcpd_SetProcError(severity); \
            else \
                rtcpd_SetProcError(RTCP_FAILED); \
            rtcp_log(LOG_DEBUG,"diskIOthread() return RC=-1 to client\n"); \
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 2, \
                             "func"   , TL_MSG_PARAM_STR, "diskIOthread", \
                             "Message", TL_MSG_PARAM_STR, "return RC=-1 to client" ); \
            if ( rc == 0 && AbortFlag != 0 && (severity & RTCP_FAILED) == 0 ) \
                rtcpd_SetReqStatus(X,Y,(AbortFlag == 1 ? ERTUSINTR : ERTOPINTR),rtcpd_CheckProcError()); \
            (void) tellClient(&client_socket,X,Y,-1); \
        } \
        if ( disk_fd != -1 ) \
            (void)DiskFileClose(disk_fd,pool_index,tape,file); \
        if ( AbortFlag == 0 ) rtcpd_BroadcastException(); \
        DiskIOfinished(); \
        if ( rc == -1 ) return((void *)&failure); \
        else return((void *)&success); \
    }}

void *diskIOthread(void *arg) {
    tape_list_t *tape, *tl;
    file_list_t *file, *fl;
    rtcpTapeRequest_t *tapereq;
    rtcpFileRequest_t *filereq;
    u_signed64 nbbytes;
    int client_socket;
    char *p, u64buf[22];
    int pool_index = -1;
    int indxp = 0;
    int offset = 0;
    int disk_fd = -1;
    int last_file = FALSE;
    int end_of_tpfile = FALSE;
    int rc, save_rc, mode, severity, save_errno;
    extern char *u64tostr (u_signed64, char *, int);
    extern int ENOSPC_occurred;

    rtcp_log(LOG_DEBUG,"diskIOthread() started\n");
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 2, 
                     "func"   , TL_MSG_PARAM_STR, "diskIOthread",
                     "Message", TL_MSG_PARAM_STR, "started" );
    if ( arg == NULL ) {
        rtcp_log(LOG_ERR,"diskIOthread() received NULL argument\n");
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 2, 
                         "func"   , TL_MSG_PARAM_STR, "diskIOthread",
                         "Message", TL_MSG_PARAM_STR, "received NULL argument" );
        rtcpd_SetProcError(RTCP_FAILED);
        DiskIOfinished();
        return((void *)&failure);
    }

    tape = ((thread_arg_t *)arg)->tape;
    file = ((thread_arg_t *)arg)->file;
    client_socket = ((thread_arg_t *)arg)->client_socket;
    pool_index = ((thread_arg_t *)arg)->pool_index;
    indxp = ((thread_arg_t *)arg)->start_indxp;
    offset = ((thread_arg_t *)arg)->start_offset;
    last_file = ((thread_arg_t *)arg)->last_file;
    end_of_tpfile = ((thread_arg_t *)arg)->end_of_tpfile;

    if ( file == NULL || tape == NULL ) {
        rtcp_log(LOG_ERR,"diskIOthread() received NULL tape/file element\n");
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 2, 
                         "func"   , TL_MSG_PARAM_STR, "diskIOthread",
                         "Message", TL_MSG_PARAM_STR, "received NULL tape/file element" );        
        rtcpd_SetProcError(RTCP_FAILED);
        DiskIOfinished();
        return((void *)&failure);
    }
    tapereq = &tape->tapereq;
    filereq = &file->filereq;
    mode = tapereq->mode;

    /*
     * Open the disk file given the specified record format (U/F).
     */
    rc = 0;
    disk_fd = -1;
    severity = RTCP_OK;

    /*
     * EOD on read is processed later (not always an error).
     */
    if ( (mode == WRITE_ENABLE) || ((severity & RTCP_EOD) == 0) ) {
        if ( mode == WRITE_DISABLE ) {
            if ( ENOSPC_occurred == TRUE ) {
                rtcp_log(LOG_INFO,"diskIOthread() exit for synchronization due to ENOSPC\n");
                tl_rtcpd.tl_log( &tl_rtcpd, 10, 2, 
                                 "func"   , TL_MSG_PARAM_STR, "diskIOthread",
                                 "Message", TL_MSG_PARAM_STR, "exit for synchronization due to ENOSPC" );        
                filereq->proc_status = RTCP_WAITING;
                DiskIOfinished();
                return((void *)&success);
            }
        }

        rc = DiskFileOpen(pool_index,tape,file);
        disk_fd = rc;
        rtcpd_CheckReqStatus(file->tape,file,NULL,&severity);
        /*
         * EOD on read is processed later (not always an error).
         * Note: DiskFileOpen() cannot return non-error status in case of
         * a RTCP_EOD severity. This can happen if the open is blocked
         * because of synchronisation for append. Thus, we must check
         * if RTCP_EOD severity before checking the return status to assure
         * proper EOD processing (otherwise it will always be considered as an
         * error).
         */
        if ( (mode == WRITE_ENABLE) || ((severity & RTCP_EOD) == 0) ) {
            CHECK_PROC_ERR(file->tape,file,"DiskFileOpen() error");

            /*
             * Note that MemoryToDisk() and DiskToMemory() close the
             * disk file descriptor if the transfer was successful.
             */
            if ( mode == WRITE_DISABLE ) {
                rc = MemoryToDisk(disk_fd,pool_index,&indxp,&offset,
                                  &last_file,&end_of_tpfile,tape,file);
                if ( rc == 0 ) disk_fd = -1;
                CHECK_PROC_ERR(file->tape,file,"MemoryToDisk() error");
            } else {
                rc = DiskToMemory(disk_fd,pool_index,&indxp,&offset,
                                  &last_file,&end_of_tpfile,tape,file);
                if ( rc == 0 ) disk_fd = -1;
                CHECK_PROC_ERR(file->tape,file,"DiskToMemory() error");
            }
        }
    }

    save_rc = rc;

    if ( mode == WRITE_ENABLE && (filereq->concat & VOLUME_SPANNING) != 0 ) {
        /*
         * If disk files are concatenated into a single tape file that
         * spanns several volumes we should update the disk IO
         * specific data for all this disk fseq on all volumes.
         */
        CLIST_ITERATE_BEGIN(tape,tl) {
            CLIST_ITERATE_BEGIN(tl->file,fl) {
                if ( tl != tape && fl != file && 
                     (fl->filereq.concat & VOLUME_SPANNING) != 0 &&
                     fl->filereq.disk_fseq == filereq->disk_fseq &&
                     strcmp(fl->filereq.file_path,filereq->file_path) == 0 ) {
                    fl->filereq.TEndTransferDisk = filereq->TEndTransferDisk;
                    fl->filereq.TStartTransferDisk = filereq->TStartTransferDisk;
                    strcpy(fl->filereq.ifce,filereq->ifce);
                }
             } CLIST_ITERATE_END(tl->file,fl);
        } CLIST_ITERATE_END(tape,tl);
    }

    /*
     * Update request status if we are behind tape IO (i.e. tape read).
     */
    if ( mode == WRITE_DISABLE ) {
        nbbytes = file->diskbytes_sofar;
        if ( (filereq->concat & CONCAT) != 0 ) nbbytes -= filereq->startsize;
        if ( (filereq->concat & CONCAT_TO_EOD) != 0 ) {
            if ( (severity & RTCP_EOD) == 0 ) {
                nbbytes -= filereq->startsize;
                filereq->proc_status = RTCP_PARTIALLY_FINISHED;
            } else {
                filereq->proc_status = RTCP_FINISHED;
            }
        } else if ( (filereq->concat & VOLUME_SPANNING) != 0 ) { 
            nbbytes = 0;
            CLIST_ITERATE_BEGIN(tape,tl) {
                CLIST_ITERATE_BEGIN(tl->file,fl) {
                    if ( (fl->filereq.concat & VOLUME_SPANNING) != 0 &&
                         fl->filereq.disk_fseq == filereq->disk_fseq &&
                        strcmp(fl->filereq.file_path,filereq->file_path) == 0 &&
                         fl->filereq.proc_status < RTCP_PARTIALLY_FINISHED ) {
                        nbbytes += fl->diskbytes_sofar;
                        /*
                         * The disk transfer time per file section is not
                         * measured since there is only one disk IO thread
                         * for the whole file. The best we can do is to
                         * tape transfer times. 
                         */
                        fl->filereq.TStartTransferDisk = fl->filereq.TStartTransferTape; 
                        fl->filereq.TEndTransferDisk = fl->filereq.TEndTransferTape;
                        strcpy(fl->filereq.ifce,filereq->ifce);
                        fl->filereq.cprc = filereq->cprc;
                        if ( tl->next == tape && fl->next == tl->file ) {
                            fl->filereq.proc_status = RTCP_FINISHED;
                            file = fl; 
                            filereq = &fl->filereq;
                        } else {
                            fl->filereq.proc_status = RTCP_EOV_HIT;
                            rc = tellClient(&client_socket,NULL,fl,save_rc);
                            CHECK_PROC_ERR(NULL,fl,"tellClient() error");
                        }
                    }
                } CLIST_ITERATE_END(tl->file,fl);
            } CLIST_ITERATE_END(tape,tl);
        } else filereq->proc_status = RTCP_FINISHED;
        u64tostr(nbbytes,u64buf,0);
        p = strchr(u64buf,' ');
        if ( p != NULL ) p = '\0';
        if ( nbbytes > 0 ) { 
            rtcp_log(LOG_INFO,
                     "network interface for data transfer (%s bytes) is %s\n",
                     u64buf,filereq->ifce);
            tl_rtcpd.tl_log( &tl_rtcpd, 10, 4, 
                             "func"   , TL_MSG_PARAM_STR, "diskIOthread",
                             "Message", TL_MSG_PARAM_STR, "network interface for data transfer",
                             "bytes"  , TL_MSG_PARAM_STR, u64buf,
                             "ifce"   , TL_MSG_PARAM_STR, filereq->ifce );        
        }

        fl = NULL;

        rtcp_log(LOG_DEBUG,
           "diskIOthread() send %d status for FSEQ %d, FSEC %d on volume %s\n",
           filereq->proc_status,filereq->tape_fseq,
           file->tape_fsec,tapereq->vid);
        tl_rtcpd.tl_log( &tl_rtcpd, 11, 6, 
                         "func"   , TL_MSG_PARAM_STR, "diskIOthread",
                         "Message", TL_MSG_PARAM_STR, "send status",
                         "status" , TL_MSG_PARAM_INT, filereq->proc_status,
                         "FSEQ"   , TL_MSG_PARAM_INT, filereq->tape_fseq,
                         "FSEC"   , TL_MSG_PARAM_INT, file->tape_fsec,
                         "volume" , TL_MSG_PARAM_STR, tapereq->vid );        

        rc = tellClient(&client_socket,NULL,file,save_rc);
        CHECK_PROC_ERR(NULL,file,"tellClient() error");

        rtcp_log(LOG_DEBUG,"diskIOthread() fseq %d <-> %s copied %d bytes, rc=%d, proc_status=%d severity=%d\n",
            filereq->tape_fseq,filereq->file_path,
            (unsigned long)file->diskbytes_sofar,filereq->cprc,
            filereq->proc_status,severity);
        tl_rtcpd.tl_log( &tl_rtcpd, 11, 8, 
                         "func"       , TL_MSG_PARAM_STR,   "diskIOthread",
                         "Message"    , TL_MSG_PARAM_STR,   "copied bytes",
                         "FSEQ"       , TL_MSG_PARAM_INT,   filereq->tape_fseq,
                         "Path"       , TL_MSG_PARAM_STR,   filereq->file_path,
                         "Bytes"      , TL_MSG_PARAM_INT64, file->diskbytes_sofar, 
                         "rc"         , TL_MSG_PARAM_INT,   filereq->cprc,
                         "proc_status", TL_MSG_PARAM_INT,   filereq->proc_status,
                         "severity"   , TL_MSG_PARAM_INT,   severity );

    } /* if ( mode == WRITE_DISABLE ) */

    DiskIOfinished();
    return((void *)&success);
}


int rtcpd_InitDiskIO(int *poolsize) {
    extern char *getenv();
    extern char *getconfent();
    char *p;
    int rc;
    
    if ( poolsize == NULL ) {
        serrno = EINVAL;
        return(-1);
    }

    if ( (p = getenv("RTCPD_THREAD_POOL")) != (char *)NULL ) {
        *poolsize = atoi(p);
    } else if ( ( p = getconfent("RTCPD","THREAD_POOL",0)) != (char *)NULL ) {
        *poolsize = atoi(p);
    } else {
        *poolsize = RTCPD_THREAD_POOL;
    }
    
    rc = Cpool_create(*poolsize,poolsize);
    rtcp_log(LOG_DEBUG,"rtcpd_InitDiskIO() thread pool (id=%d): pool size = %d\n",
        rc,*poolsize);
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                     "func"     , TL_MSG_PARAM_STR, "rtcpd_InitDiskIO",
                     "Message"  , TL_MSG_PARAM_STR, "thread pool",
                     "ID"       , TL_MSG_PARAM_INT, rc,
                     "Pool size", TL_MSG_PARAM_INT, *poolsize );
    /*
     * Create the diskIOstatus array in the processing status structure
     */
    proc_stat.diskIOstatus = (diskIOstatus_t *)calloc(*poolsize,sizeof(diskIOstatus_t));
    if ( proc_stat.diskIOstatus == NULL ) {
        rtcp_log(LOG_ERR,"rtcpd_InitDiskIO() calloc(%d,%d): %s\n",
            *poolsize,sizeof(diskIOstatus_t),sstrerror(errno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 5, 
                         "func"   , TL_MSG_PARAM_STR, "rtcpd_InitDiskIO",
                         "Message", TL_MSG_PARAM_STR, "calloc",
                         "nmemb"  , TL_MSG_PARAM_INT, *poolsize,
                         "size"   , TL_MSG_PARAM_INT, sizeof(diskIOstatus_t),
                         "Error"  , TL_MSG_PARAM_STR, sstrerror(errno) );
        rc = -1;
    }
    proc_stat.nb_diskIO = *poolsize;

    return(rc);
}

int rtcpd_CleanUpDiskIO(int poolsize) {
    int thIndex;
    for ( thIndex = 0; thIndex < poolsize; thIndex++ ) {
        tellClient(&thargs[thIndex].client_socket,NULL,NULL,0);
        rtcp_CloseConnection(&thargs[thIndex].client_socket);
    }
    if ( thargs != NULL ) free(thargs);
    return(0);
}

int rtcpd_StartDiskIO(rtcpClientInfo_t *client,
                      tape_list_t *tape,
                      file_list_t *file,
                      int poolID, int poolsize) {

    tape_list_t *nexttape;
    file_list_t *nextfile, *prevfile;
    u_signed64 prev_filesz;
    rtcpFileRequest_t *filereq;
    diskIOstatus_t *diskIOstatus;
    thread_arg_t *tharg;
    int rc, save_serrno, indxp, offset, next_offset, last_file,end_of_tpfile;
    int prev_bufsz = 0;
    int next_nb_bufs, severity;
    int next_bufsz = 0;
    int thIndex, mode;
    int nb_diskIOactive = 0;

    if ( client == NULL || tape == NULL || file == NULL ||
        poolsize <= 0 ) {
        serrno = EINVAL;
        return(-1);
    }

    rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() called\n");
    tl_rtcpd.tl_log( &tl_rtcpd, 11, 2, 
                     "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                     "Message", TL_MSG_PARAM_STR, "called" );
    /*
     * Reserve a thread argument table
     */
    if ( thargs == NULL ) {
        thargs = (thread_arg_t *)malloc(poolsize * sizeof(thread_arg_t));
        if ( thargs == NULL ) {
            save_serrno = errno;
            rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() malloc(): %s\n",
                sstrerror(errno));
            tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                             "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "Message", TL_MSG_PARAM_STR, "malloc",
                             "Error"  , TL_MSG_PARAM_STR, sstrerror(errno) );
            serrno = save_serrno;
            return(-1);
        }
        for ( thIndex = 0; thIndex < poolsize; thIndex++ ) {
            /*
             * Open a separate connection to client. An unique
             * connection for each thread allows them to talk
             * independently to the client. The connections are
             * closed at cleanup after the full request has finished
             * (in rtcpd_CleanUpDiskIO()).
             */
            tharg = &thargs[thIndex];
            tharg->client_socket = -1;
            rc = rtcpd_ConnectToClient(&tharg->client_socket,
                                       client->clienthost,
                                       &client->clientport);
            if ( rc == -1 ) {
                save_serrno = serrno;
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() rtcpd_ConnectToClient(%s,%d): %s\n",
                    client->clienthost,client->clientport,sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 5, 
                                 "func"       , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message"    , TL_MSG_PARAM_STR, "rtcpd_ConnectToClient",
                                 "Client Host", TL_MSG_PARAM_STR, client->clienthost,
                                 "Client Port", TL_MSG_PARAM_INT, client->clientport,
                                 "Error"      , TL_MSG_PARAM_STR, sstrerror(errno) );
                serrno = save_serrno;
                return(-1);
            }
        }
    }

    /*
     * Loop over all file requests
     */
    indxp = offset = next_offset = 0;
    last_file = FALSE;
    prevfile = NULL;
    /*
     * We can safely set diskIOstarted flag here. It is only used
     * by us and the diskIOthread's we will start in the following.
     * It must be set to one to allow the first diskIOthread to
     * start.
     */
    proc_cntl.diskIOstarted = 1;
    proc_cntl.diskIOfinished = 0;
    proc_cntl.nb_diskIOactive = 0;
    proc_cntl.nb_reserved_bufs = 0;
    nexttape = tape;
    mode = tape->tapereq.mode;
    /*
     * We don't loop over volumes since volume spanning is only allowed
     * for last tape file in the request and the file must start in the 
     * first volume.
     */
    CLIST_ITERATE_BEGIN(nexttape->file,nextfile) {
        end_of_tpfile = FALSE;
        thIndex = -1;

        /*
         * Leading I/O thread must always check if clients wishes
         * to append more work. Trailing I/O thread must always
         * wait for leading I/O thread to do this check in case
         * they have reached the same file request.
         */
        if ( mode == WRITE_ENABLE ) {
            /*
             * Grab a free socket
             */
            thIndex = Cpool_next_index(poolID);
            if ( thIndex == -1 ) {
                save_serrno = serrno;
                rtcpd_AppendClientMsg(NULL, nextfile, "Error assigning thread: %s\n",
                    sstrerror(save_serrno));
                rtcpd_SetReqStatus(NULL,nextfile,save_serrno,
                                   RTCP_SYERR | RTCP_FAILED);

                (void)Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
                proc_cntl.diskIOfinished = 1;
                (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cpool_next_index(): %s\n",
                         sstrerror(save_serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "Cpool_next_index",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(save_serrno) );
                serrno = save_serrno;
                return(-1);
            }
            rtcp_log(LOG_INFO,"rtcpd_StartDiskIO() check with client for more work. Use socket %d\n",
                thargs[thIndex].client_socket);
            tl_rtcpd.tl_log( &tl_rtcpd, 10, 3, 
                             "func"      , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "Message"   , TL_MSG_PARAM_STR, "check with client for more work",
                             "Use socket", TL_MSG_PARAM_INT, thargs[thIndex].client_socket );
            rc = rtcpd_checkMoreWork(&(thargs[thIndex].client_socket),
                nexttape,nextfile);
            if ( rc == -1 ) {
                save_serrno = serrno;
                rtcpd_AppendClientMsg(NULL, nextfile, "Error requesting client for more work: %s\n",
                    sstrerror(save_serrno));
                rtcpd_SetReqStatus(NULL,nextfile,save_serrno,
                                   RTCP_USERR | RTCP_FAILED);
                (void)Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
                proc_cntl.diskIOfinished = 1;
                (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() rtcpd_checkMoreWork(): %s\n",
                         sstrerror(save_serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "rtcpd_checkMoreWork",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(save_serrno) );
                serrno = save_serrno;
                return(-1);
            }
            if ( rc == 1 ) break;
        } else {
            rtcp_log(LOG_INFO,"rtcpd_StartDiskIO() end of filereqs. Wait for tapeIO to check for more work\n");
            tl_rtcpd.tl_log( &tl_rtcpd, 10, 2, 
                             "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "Message", TL_MSG_PARAM_STR, "End of filereqs. Wait for tapeIO to check for more work" );
            rc = rtcpd_waitMoreWork(nextfile);
            if ( rc == -1 ) {
                save_serrno = serrno;
                rtcpd_AppendClientMsg(NULL, nextfile, "Error waiting for client to request more work: %s\n",
                    sstrerror(save_serrno));
                rtcpd_SetReqStatus(NULL,nextfile,save_serrno,
                                   RTCP_USERR | RTCP_FAILED);
                (void)Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
                proc_cntl.diskIOfinished = 1;
                (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() rtcpd_waitMoreWork(): %s\n",
                         sstrerror(save_serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "rtcpd_waitMoreWork",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(save_serrno) );
                serrno = save_serrno;
                return(-1);
            }
            rtcpd_CheckReqStatus(NULL,nextfile,NULL,&severity);
            if ( (severity & RTCP_FAILED) != 0 ) {
                (void)Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
                nb_diskIOactive = proc_cntl.nb_diskIOactive;
                if ( nb_diskIOactive <= 0 ) proc_cntl.diskIOfinished = 1;
                (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                if ( nb_diskIOactive <= 0 ) {
                    rtcpd_SetProcError(severity);
                    return(0);
                }
            }

            /*
             * Break out if there is nothing more to do
             */
            if ( rc == 1 ) break;
        }

        filereq = &nextfile->filereq;
        if ( nextfile->next == nexttape->file ) last_file = TRUE;
        if ( (last_file == TRUE) || (nextfile->next->filereq.concat & 
             (NOCONCAT | NOCONCAT_TO_EOD)) != 0 ) end_of_tpfile = TRUE;
        if ( nextfile->filereq.proc_status != RTCP_FINISHED ) {
            /*
             * Get control info
             */
            rc = Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
            if ( rc == -1 ) {
                save_serrno = serrno;
                rtcpd_AppendClientMsg(NULL, nextfile, "Cannot lock mutex: %s\n",
                    sstrerror(save_serrno));
                rtcpd_SetReqStatus(NULL,nextfile,save_serrno,
                                   RTCP_FAILED | RTCP_SYERR);
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cthread_mutex_lock_ext(): %s\n",
                    sstrerror(save_serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "Cthread_mutex_lock_ext",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(save_serrno) );
                serrno = save_serrno;
                return(-1);
            }

            /*
             * Check if we need to exit due to processing error
             */
            rc= rtcpd_CheckProcError();
            if ( rc != RTCP_OK ) {
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() processing error detected, severity=0x%x (%d)\n",rc,rc);
                {
                        char __severity[32];
                        sprintf( __severity, "0x%x", rc );
                        tl_rtcpd.tl_log( &tl_rtcpd, 3, 4, 
                                         "func"          , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                         "Message"       , TL_MSG_PARAM_STR, "processing error detected",
                                         "Severity (hex)", TL_MSG_PARAM_STR, __severity,
                                         "Severity (dec)", TL_MSG_PARAM_INT, rc );
                }
                proc_cntl.diskIOfinished = 1;
                (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                /* It's not our error */
                return(0);
            }
            /*
             * Did the tape end with previous file ?
             */
            if ( mode == WRITE_DISABLE && prevfile != NULL &&
                 (prevfile->filereq.concat & (CONCAT_TO_EOD|NOCONCAT_TO_EOD)) !=0 ) {
                rtcpd_CheckReqStatus(prevfile->tape,prevfile,NULL,&severity);
                if ( (severity & RTCP_EOD) != 0 ) {
                    proc_cntl.diskIOfinished = 1;
                    (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                    (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                    return(0);
                }
            }

            /*
             * Wait while buffers are overallocated. We also
             * have to make sure that the previously dispatched thread
             * started and locked its first buffer. For tape write
             * there are one additional conditions:
             * 1) we must check if we need to wait for the 
             *    Ctape_info() for more info. (i.e. blocksize)
             *    before starting the disk -> memory copy.
             * For tape read we must also check if the tapeIO has
             * finished, in which case we can just go on copying the
             * the remainin data in memory to disk.
             */
            rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO(): nb_reserver_bufs=%d, nb_bufs=%d, mode=%d, diskIOstarted=%d, tapeIOfinished=%d, blocksize=%d\n",
                    proc_cntl.nb_reserved_bufs,nb_bufs,mode,
                    proc_cntl.diskIOstarted,proc_cntl.tapeIOfinished,
                    filereq->blocksize);
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 7, 
                             "func"            , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "nb_reserver_bufs", TL_MSG_PARAM_INT, proc_cntl.nb_reserved_bufs ,
                             "nb_bufs"         , TL_MSG_PARAM_INT, nb_bufs,
                             "mode"            , TL_MSG_PARAM_INT, mode,
                             "diskIOstarted"   , TL_MSG_PARAM_INT, proc_cntl.diskIOstarted ,
                             "tapeIOfinished"  , TL_MSG_PARAM_INT, proc_cntl.tapeIOfinished ,
                             "blocksize"       , TL_MSG_PARAM_INT, filereq->blocksize );

            while ( (proc_cntl.diskIOstarted == 0) ||
                    (proc_cntl.nb_reserved_bufs >= nb_bufs &&
                     (mode == WRITE_ENABLE || 
                      (mode == WRITE_DISABLE &&
                       proc_cntl.tapeIOfinished == 0 &&
                       prevfile != NULL &&
                       prevfile->end_index < 0))) ||
                    ((mode == WRITE_ENABLE) &&
                     (filereq->blocksize <= 0) &&
                     ((filereq->concat & NOCONCAT) != 0)) ) {
                rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() waiting... (nb_reserved_bufs=%d, diskIOstarted=%d\n",
                         proc_cntl.nb_reserved_bufs,proc_cntl.diskIOstarted);
                tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                                 "func"            , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message"         , TL_MSG_PARAM_STR, "waiting ...",
                                 "nb_reserver_bufs", TL_MSG_PARAM_INT, proc_cntl.nb_reserved_bufs ,
                                 "diskIOstarted"   , TL_MSG_PARAM_INT, proc_cntl.diskIOstarted );
                rc = Cthread_cond_wait_ext(proc_cntl.cntl_lock);
                if ( rc == -1 ) {
                    save_serrno = serrno;
                    rtcpd_AppendClientMsg(NULL, nextfile, "Error on condition wait: %s\n",
                        sstrerror(save_serrno));
                    rtcpd_SetReqStatus(NULL,nextfile,save_serrno,
                                      RTCP_SYERR | RTCP_FAILED);
                    rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cthread_cond_wait_ext(proc_cntl): %s\n",
                        sstrerror(save_serrno));
                    tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                     "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                     "Message", TL_MSG_PARAM_STR, "Cthread_cond_wait_ext(proc_cntl)",
                                     "Error"  , TL_MSG_PARAM_STR, sstrerror(save_serrno) );
                    (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                    serrno = save_serrno;
                    return(-1);
                }
                /*
                 * Check if we need to exit due to processing error
                 */
                rc= rtcpd_CheckProcError();
                if ( rc != RTCP_OK ) {
                    rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() processing error detected, severity=0x%x (%d)\n",rc,rc);
                    {
                            char __severity[32];
                            sprintf( __severity, "0x%x", rc );
                            tl_rtcpd.tl_log( &tl_rtcpd, 3, 4, 
                                             "func"          , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                             "Message"       , TL_MSG_PARAM_STR, "processing error detected",
                                             "Severity (hex)", TL_MSG_PARAM_STR, __severity,
                                             "Severity (dec)", TL_MSG_PARAM_INT, rc );
                    }
                    proc_cntl.diskIOfinished = 1;
                    (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                    (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                    /* It's not our error */
                    return(0);
                }
                /*
                 * Did the tape end with previous file ?
                 */
                if ( mode == WRITE_DISABLE && prevfile != NULL &&
                     (prevfile->filereq.concat & (CONCAT_TO_EOD|NOCONCAT_TO_EOD)) !=0 ) {          
                    rtcpd_CheckReqStatus(prevfile->tape,prevfile,NULL,&severity);
                    if ( (severity & RTCP_EOD) != 0 ) {
                        proc_cntl.diskIOfinished = 1;
                        (void)Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                        (void)Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                        return(0);
                    }
                }
            }

            /*
             * We're going to start a new diskIO thread. 
             */
            proc_cntl.diskIOstarted = 0;
            proc_cntl.nb_diskIOactive++;
            /*
             * Calculate nb buffers to be used for the next file.
             * Don't touch this code unless you REALLY know what you are
             * doing. Any mistake here may in the best case result in deadlocks
             * and in the worst case .... data corruption!
             */
            if ( mode == WRITE_ENABLE ) {
                /*
                 * On tape write we know the next file size
                 */
                next_bufsz = rtcpd_CalcBufSz(nexttape,nextfile);
                if ( (filereq->concat & CONCAT) != 0 ) 
                    next_nb_bufs = (int)(((u_signed64)next_offset + 
                          nextfile->filereq.bytes_in) / (u_signed64)next_bufsz);
                else 
                    next_nb_bufs = (int)(nextfile->filereq.bytes_in / 
                                   (u_signed64)next_bufsz);
                /*
                 * Increment by one if not concatenating or we reached last
                 * file in the concatenation (subsequent file will start
                 * with a new buffer).
                 */
                if ( (nextfile->next->filereq.concat & NOCONCAT) != 0 ) 
                    next_nb_bufs++;
            } else {
                /*
                 * On tape read we don't know the file size. Make
                 * sure to overallocate until we know the exact size
                 * set by the tape IO thread at end of file.
                 */
                next_nb_bufs = nb_bufs + 1;
            }
            rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() number of reserved buffs %d + %d\n",
                proc_cntl.nb_reserved_bufs,next_nb_bufs);
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                             "func"                    , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "number of reserved buffs", TL_MSG_PARAM_INT, proc_cntl.nb_reserved_bufs,
                             "next number of buffs"    , TL_MSG_PARAM_INT, next_nb_bufs );
            proc_cntl.nb_reserved_bufs += next_nb_bufs;
            rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() new number of reserved buffs %d\n",
                proc_cntl.nb_reserved_bufs);
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 2, 
                             "func"                        , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "new number of reserved buffs", TL_MSG_PARAM_INT, proc_cntl.nb_reserved_bufs );
            rc = Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
            if ( rc == -1 ) {
                save_serrno = serrno;
                rtcpd_AppendClientMsg(NULL,nextfile,
                      "Cannot unlock CNTL mutex: %s\n",sstrerror(serrno));
                rtcpd_SetReqStatus(NULL,nextfile,serrno,
                                   RTCP_SYERR | RTCP_FAILED);
                rtcp_log(LOG_ERR,
                         "rtcpd_StartDiskIO() Cthread_mutex_unlock_ext(): %s\n",
                         sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "Cthread_mutex_unlock_ext",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                serrno = save_serrno;
                return(-1);
            }

            /* 
             * Update offset in buffer table.
             * Don't touch this code unless you REALLY know what you are
             * doing. Any mistake here may in the best case result in deadlocks
             * and in the worst case .... data corruption!
             */
            if ( prevfile == NULL ) {
                /*
                 * First file
                 */
                indxp = offset = next_offset = 0;
            } else {
                prev_bufsz = rtcpd_CalcBufSz(nexttape,prevfile);
                if ( mode == WRITE_ENABLE ) 
                    prev_filesz = prevfile->filereq.bytes_in;
                else 
                    prev_filesz = prevfile->filereq.bytes_out;
                rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() prev. file size %d, buffer sz %d, indxp %d\n",
                        (int)prev_filesz,prev_bufsz,indxp);
                tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                                 "func"           , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "prev. file size", TL_MSG_PARAM_INT, (int)prev_filesz,
                                 "buffer size"    , TL_MSG_PARAM_INT, prev_bufsz,
                                 "indxp"          , TL_MSG_PARAM_INT, indxp );                
                if ( mode == WRITE_ENABLE ) 
                    indxp = (indxp + (int)(((u_signed64)offset + prev_filesz) /
                            ((u_signed64)prev_bufsz)));
                else
                    indxp = prevfile->end_index;
                rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() new indxp %d\n",
                        indxp);
                tl_rtcpd.tl_log( &tl_rtcpd, 11, 2, 
                                 "func"     , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "new indxp", TL_MSG_PARAM_INT, indxp );                
                indxp = indxp % nb_bufs;
                if ( mode == WRITE_ENABLE ) {
                    if ( (filereq->concat & NOCONCAT) != 0) {
                        /*
                         * Not concatenating on tape. Start next file
                         * with a brand new buffer except if previous
                         * file was empty we re-use the previous buffer.
                         */
                        rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO(): no concatenate (%d != %d), indxp %d\n",
                            nextfile->filereq.tape_fseq,prevfile->filereq.tape_fseq,indxp);
                        tl_rtcpd.tl_log( &tl_rtcpd, 11, 4, 
                                         "func"     , TL_MSG_PARAM_STR, "no concatenate",
                                         "Next FSEQ", TL_MSG_PARAM_INT, nextfile->filereq.tape_fseq,
                                         "Prev FSEQ", TL_MSG_PARAM_INT, prevfile->filereq.tape_fseq,
                                         "indxp"    , TL_MSG_PARAM_INT, indxp );                
                        if ( prev_filesz != 0 ) 
                            indxp = (indxp + 1) % nb_bufs;
                        offset = next_offset = 0;
                        /*
                         * If next file is concatenated we must provide
                         * correct offset so that the next_nb_bufs calculation
                         * in next iteration is correct.
                         */
                        if ( (nextfile->next->filereq.concat & CONCAT) != 0 ) {
                            next_offset = (int)(filereq->bytes_in/((u_signed64)next_bufsz));
                        }
                    } else {
                        /*
                         * On tape write we need offset if we are
                         * concatenating on tape
                         */
                        rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO(): concatenate (%d == %d)\n",
                            nextfile->filereq.tape_fseq,prevfile->filereq.tape_fseq);
                        tl_rtcpd.tl_log( &tl_rtcpd, 11, 3, 
                                         "func"     , TL_MSG_PARAM_STR, "concatenate",
                                         "Next FSEQ", TL_MSG_PARAM_INT, nextfile->filereq.tape_fseq,
                                         "Prev FSEQ", TL_MSG_PARAM_INT, prevfile->filereq.tape_fseq );
                        offset = (int)(((u_signed64)offset + prev_filesz) %
                                       ((u_signed64)prev_bufsz));
                        next_offset = (int)(((u_signed64)offset + filereq->bytes_in) %
                                            ((u_signed64)next_bufsz));
                    }
                }
            }
            prevfile = nextfile;
            /*
             * Get next thread index and fill in thread args
             */
            if ( thIndex < 0 ) thIndex = Cpool_next_index(poolID);
            if ( thIndex == -1 ) {
                save_serrno = serrno;
                rtcpd_AppendClientMsg(NULL, nextfile, "Error assigning thread: %s\n",
                    sstrerror(save_serrno));
                rtcpd_SetReqStatus(NULL,nextfile,save_serrno,
                                   RTCP_SYERR | RTCP_FAILED);
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cpool_next_index(): %s\n",
                         sstrerror(save_serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "Cpool_next_index",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                serrno = save_serrno;
                return(-1);
            }
            tharg = &thargs[thIndex];

            tharg->tape = nexttape;
            tharg->file = nextfile;
            tharg->client = client;
            tharg->end_of_tpfile = end_of_tpfile;
            tharg->last_file = last_file;
            tharg->pool_index = thIndex;
            tharg->start_indxp = indxp;
            tharg->start_offset = offset;
            /*
             * Reset the diskIOstatus for assigned thread
             */
            diskIOstatus = &proc_stat.diskIOstatus[thIndex];
            DK_STATUS(RTCP_PS_NOBLOCKING);
            DK_SIZE(0);
            /*
             * Assign next thread and start the request
             */
            rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO(thIndex=%d,arg=0x%lx) start with indxp=%d, offset=%d, end_of_tpfile=%d\n",
                thIndex,tharg,indxp,offset,end_of_tpfile);
            {
                    char __arg[32];
                    sprintf( __arg, "%p", tharg );
                    tl_rtcpd.tl_log( &tl_rtcpd, 11, 6, 
                                     "func"         , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                     "thIndex"      , TL_MSG_PARAM_INT, thIndex,
                                     "arg"          , TL_MSG_PARAM_STR, __arg,
                                     "indxp"        , TL_MSG_PARAM_INT, indxp,
                                     "offset"       , TL_MSG_PARAM_INT, offset,
                                     "end_of_tpfile", TL_MSG_PARAM_INT, end_of_tpfile );
            }
            rc = Cpool_assign(poolID,diskIOthread,(void *)tharg,-1);
            if ( rc == -1 ) {
                save_serrno = serrno;
                rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cpool_assign(%d): %s\n",
                         poolID,sstrerror(serrno));
                tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                                 "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                                 "Message", TL_MSG_PARAM_STR, "Cpool_assign",
                                 "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
                serrno = save_serrno;
                return(-1);
            }
            /*
             * Have we reached End Of Data on tape in a CONCAT_TO_EOD or
             * NOCONCAT_TO_EOD request ?
             */
            if ( mode == WRITE_DISABLE &&
                (filereq->err.severity & RTCP_EOD) != 0 ) break;

        } else { /* if ( ... ) */
            rtcp_log(LOG_DEBUG,"rtcpd_StartDiskIO() skipping finished request (%d,%d,%s,concat:%d\n",
                     nextfile->filereq.tape_fseq,nextfile->filereq.disk_fseq,
                     nextfile->filereq.file_path,nextfile->filereq.concat);
            tl_rtcpd.tl_log( &tl_rtcpd, 11, 6, 
                             "func"     , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                             "Message"  , TL_MSG_PARAM_STR, "skipping finished request",
                             "Tape FSEQ", TL_MSG_PARAM_INT, nextfile->filereq.tape_fseq,
                             "Disk FSEQ", TL_MSG_PARAM_INT, nextfile->filereq.disk_fseq,
                             "Path"     , TL_MSG_PARAM_STR, nextfile->filereq.file_path,
                             "concat"   , TL_MSG_PARAM_INT, nextfile->filereq.concat );
        }
    } CLIST_ITERATE_END(nexttape->file,nextfile);
    /*
     * Tell the others that we've finished
     */
    rc = Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
    if ( rc == -1 ) {
        save_serrno = serrno;
        rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cthread_mutex_lock_ext(): %s\n",
                 sstrerror(serrno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                         "Message", TL_MSG_PARAM_STR, "Cthread_mutex_lock_ext",
                         "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );
        serrno = save_serrno;
        return(-1);
    }
    proc_cntl.diskIOfinished = 1;
    rc = Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
    if ( rc == -1 ) {
        save_serrno = serrno;
        rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cthread_cond_broadcast_ext(): %s\n",
                 sstrerror(serrno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                         "Message", TL_MSG_PARAM_STR, "Cthread_cond_broadcast_ext",
                         "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );       
        serrno = save_serrno;
        return(-1);
    }
    rc = Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
    if ( rc == -1 ) {
        save_serrno = serrno;
        rtcp_log(LOG_ERR,"rtcpd_StartDiskIO() Cthread_mutex_unlock_ext(): %s\n",
                 sstrerror(serrno));
        tl_rtcpd.tl_log( &tl_rtcpd, 3, 3, 
                         "func"   , TL_MSG_PARAM_STR, "rtcpd_StartDiskIO",
                         "Message", TL_MSG_PARAM_STR, "Cthread_mutex_unlock_ext",
                         "Error"  , TL_MSG_PARAM_STR, sstrerror(serrno) );       
        serrno = save_serrno;
        return(-1);
    }

    return(0);
}

