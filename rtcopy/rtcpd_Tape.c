/*
 * Copyright (C) 1999 by CERN IT-PDP/DM
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile: rtcpd_Tape.c,v $ $Revision: 1.1 $ $Date: 1999/11/29 11:22:04 $ CERN IT-PDP/DM Olof Barring";
#endif /* not lint */

/*
 * rtcpd_MainCntl.c - RTCOPY server main control thread
 */


#if defined(_WIN32)
#include <winsock2.h>
#include <io.h>
extern char *geterr();
#else /* _WIN32 */
#include <sys/param.h>
#include <sys/types.h>                  /* Standard data types          */
#include <netdb.h>                      /* Network "data base"          */
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>                 /* Internet data types          */
#endif /* _WIN32 */

#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>

#include <pwd.h>
#include <Castor_limits.h>
#include <Cglobals.h>
#include <log.h>
#include <osdep.h>
#include <net.h>
#include <Cthread_api.h>
#include <vdqm_api.h>
#include <Ctape_api.h>
#include <rtcp_constants.h>
#include <rtcp.h>
#include <rtcp_server.h>
#include <serrno.h>

#define TP_STATUS(X) (proc_stat.tapeIOstatus.current_activity = (X))
#define TP_SIZE(X)   (proc_stat.tapeIOstatus.nbbytes += (u_signed64)(X))

typedef struct thread_arg {
    SOCKET client_socket;
    tape_list_t *tape;
} thread_arg_t;

extern int nb_bufs;

extern int bufsz;

extern processing_cntl_t proc_cntl;
extern processing_status_t proc_stat;

extern buffer_table_t **databufs;

extern rtcpClientInfo_t *client;

extern int success;
extern int failure;

static int last_block_done = 0;


/*
 * Read from tape to memory
 */
static int MemoryToTape(int tape_fd, int *indxp, int *firstblk,
                        tape_list_t *tape, 
                        file_list_t *file) {
    int nb_bytes, rc, i, j, last_sz, blksiz, lrecl;
    int end_of_tpfile, buf_done;
    rtcpTapeRequest_t *tapereq = NULL;
    rtcpFileRequest_t *filereq = NULL;

    if ( tape_fd < 0 || indxp == NULL || firstblk == NULL ||
         tape == NULL || file == NULL ) {
        serrno = EINVAL;
        return(-1);
    }
    tapereq = &tape->tapereq;
    filereq = &file->filereq;
    if ( last_block_done != 0 ) {
        rtcp_log(LOG_ERR,"MemoryToTape() called beyond last block!!!\n");
        return(-1);
    }

    blksiz = filereq->blocksize;
    lrecl = filereq->recordlength;
    if ( lrecl <= 0 ) lrecl = blksiz;
    /*
     * Write loop to break out of
     */
    end_of_tpfile = FALSE;
    for (;;) {
        i = *indxp;
        buf_done = FALSE;
        /*
         * Synchronize access to next buffer
         */
        TP_STATUS(RTCP_PS_WAITMTX);
        rc = Cthread_mutex_lock_ext(databufs[i]->lock);
        TP_STATUS(RTCP_PS_NOBLOCKING);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"MemoryToTape() Cthread_mutex_lock_ext(): %s\n",
                sstrerror(serrno));
            return(-1);
        }
        /*
         * Wait until it is full
         */
        while (databufs[i]->flag == BUFFER_EMPTY) {
            TP_STATUS(RTCP_PS_WAITCOND);
            rc = Cthread_cond_wait_ext(databufs[i]->lock);
            TP_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToTape() Cthread_cond_wait_ext(): %s\n",
                    sstrerror(serrno));
                return(-1);
            }
            if ( rtcpd_CheckProcError() & RTCP_FAILED ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
        }
        rtcp_log(LOG_DEBUG,"MemoryToTape() buffer %d full\n",i);

        /*
         * Verify that actual buffer size matches block size
         */
        if ( (databufs[i]->length % blksiz) != 0 ) {
            rtcp_log(LOG_ERR,"MemoryToTape() blocksize mismatch\n");
            (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
            (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
            return(-1);
        }
        /*
         * Check if this is the last buffer of the tape file
         */
        end_of_tpfile = databufs[i]->end_of_tpfile;
        if ( end_of_tpfile == TRUE ) 
            rtcp_log(LOG_DEBUG,"MemoryToTape() end of tape file in buffer %d\n",i);
        /*
         * Copy the data to tape
         */
        last_sz = 0;
        if ( lrecl > 0 && filereq->maxnbrec > 0 ) {
            /*
             * Check if we reached the number of records limit
             * and, if so, reduce the data_length accordingly.
             */
            if ( filereq->nbrecs + databufs[i]->data_length/lrecl >
                filereq->maxnbrec ) {
                databufs[i]->data_length = (int)(filereq->maxnbrec -
                    filereq->nbrecs) * lrecl;
                rtcpd_SetReqStatus(NULL,file,EFBIG,RTCP_OK | RTCP_LIMBYSZ);
           }
        }
        rtcp_log(LOG_DEBUG,"MemoryToTape() write %d bytes to tape\n",
            databufs[i]->data_length);
        for (j=*firstblk; j*blksiz < databufs[i]->data_length; j++) {
            /*
             * Last block of file may be partial
             */
            nb_bytes = databufs[i]->data_length - j*blksiz;
            if ( nb_bytes > blksiz ) nb_bytes = blksiz;

            /*
             * >>>>>>>>>>> write to tape <<<<<<<<<<<<<
             */
            TP_STATUS(RTCP_PS_WRITE);
            rc = twrite(tape_fd,
                        databufs[i]->buffer + j*blksiz,
                        nb_bytes,
                        tape,
                        file);
            TP_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 || (rtcpd_CheckProcError() & RTCP_FAILED) ) {
                if ( rc == -1 ) 
                    rtcp_log(LOG_ERR,"MemoryToTape() tape write error\n");
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
            if ( rc == 0 ) {
                /*
                 * EOV reached
                 */
                if ( file->eovflag == 1 ) *firstblk = j;
                break;
            }
            last_sz += rc;
            file->tapebytes_sofar += rc;
            TP_SIZE(rc);
        } /* End of for (j=...) */
        rtcp_log(LOG_DEBUG,"MemoryToTape() wrote %d bytes to tape\n",
            last_sz);
        databufs[i]->data_length -= last_sz;

        if ( lrecl > 0 ) {
            filereq->nbrecs += last_sz / lrecl;
            if ( (last_sz % lrecl) != 0 ) filereq->nbrecs++;
        }

        /*
         * Reset the buffer semaphore only if the
         * full buffer has been succesfully written.
         */
        if ( databufs[i]->data_length == 0 ) {
            /*
             * Check if this is the request end.
             */
            last_block_done = databufs[i]->last_buffer;
            if ( last_block_done != 0 )
                rtcp_log(LOG_DEBUG,"MemoryToTape() last block done = %d\n",
                    last_block_done);
            /*
             * Reset all switches so that the buffer can be reused
             */
            databufs[i]->bufendp = 0;
            databufs[i]->data_length = 0;
            databufs[i]->end_of_tpfile = FALSE;
            databufs[i]->last_buffer = FALSE;
            databufs[i]->flag = BUFFER_EMPTY;
            buf_done = TRUE;
            /*
             * Increment the circular data buffer index and reset the
             * tape block index.
             */
            *indxp = (*indxp+1) % nb_bufs;
            *firstblk = 0;
        }
        /*
         * Release lock on this buffer
         */
        rc = Cthread_cond_broadcast_ext(databufs[i]->lock);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"MemoryToTape() Cthread_cond_broadcast_ext(): %s\n",
                sstrerror(serrno));
            return(-1);
        }
        rc = Cthread_mutex_unlock_ext(databufs[i]->lock);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"MemoryToTape() Cthread_mutex_unlock_ext(): %s\n",
                sstrerror(serrno));
            return(-1);
        }

        if ( buf_done == TRUE ) {
            /*
             * Decrement the number of reserved buffers.
             */
            rc = Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToTape() Cthread_mutex_lock_ext(): %s\n",
                    sstrerror(serrno));
                return(-1);
            }
            proc_cntl.nb_reserved_bufs--;

            /*
             * Signal to main thread that we changed cntl info
             */
            rc = Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToTape() Cthread_cond_broadcast_ext(proc_cntl): %s\n",
                    sstrerror(serrno));
                return(-1);
            }

            rc = Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"MemoryToTape() Cthread_mutex_unlock_ext(): %s\n",
                    sstrerror(serrno));
                return(-1);
            }
        }

        /*
         * Break out to main tape IO loop if we have hit 
         * the end of the current tape file
         */
        if ( end_of_tpfile == TRUE ) break;
        /*
         * Check if we reached end of volume. If so, return to
         * let the main tapeIO loop iterate one step in the tape
         * request list. This must be the last file request (in
         * a multi-volume request only the last file can span). Note
         * that we have to start on the same indxp since the last
         * buffer has not been completely written out.
         */
        if ( file->eovflag == 1 ) break;
        /*
         * Check if last block
         */
        if ( last_block_done != 0 ) break;
        /*
         * Has something fatal happened while we were occupied
         * writing to the tape?
         */
        if ( rtcpd_CheckProcError() & RTCP_FAILED ) return(-1);
    } /* End of for (;;) */

    TP_STATUS(RTCP_PS_CLOSE);
    rc = tclose(tape_fd,tape,file);
    TP_STATUS(RTCP_PS_NOBLOCKING);
    return(rc);
}

static int TapeToMemory(int tape_fd, int *indxp, int *firstblk,
                        tape_list_t *tape, 
                        file_list_t *file) {
    int nb_bytes, rc, i, j, last_sz, blksiz, current_bufsz;
    int lrecl, end_of_tpfile, break_and_return, severity;
    int nb_skipped_blks = 0;
    rtcpTapeRequest_t *tapereq = NULL;
    rtcpFileRequest_t *filereq = NULL;

    if ( tape_fd < 0 || indxp == NULL || firstblk == NULL ||
         tape == NULL || file == NULL ) {
        serrno = EINVAL;
        return(-1);
    }
    tapereq = &tape->tapereq;
    filereq = &file->filereq;
    if ( last_block_done != 0 ) {
        rtcp_log(LOG_ERR,"TapeToMemory() internal error called beyond last block!!!\n");
        return(-1);
    }

    /*
     * In the case when startsize > maxsize, i.e. we are concatenating
     * tape files to a single disk file and the max size has been
     * exceeded, we still need to enter the loop in order to flag a
     * buffer full to wake up the disk IO thread. However no data
     * will be read from tape since tapebytes_sofar exceeds maxsize
     * already from the beginning. See notice further down (immediately
     * before the tread()).
     */
    file->tapebytes_sofar = filereq->startsize;
    blksiz = filereq->blocksize;
    lrecl = filereq->recordlength;
    if ( lrecl <= 0 ) lrecl = blksiz;

    /*
     * Calculate new actual buffer length
     */
    current_bufsz = rtcpd_CalcBufSz(tape,file);
    if ( current_bufsz == -1 ) return(-1);
    /*
     * Tape read loop to break out of. We must assure that there
     * are at least one empty buffer available for next disk IO
     * thread before signaling end of tape file to main control
     * thread. The reason for this is to avoid conflicting access
     * between two disk IO thread on the same buffer in case the
     * network or remote disk is much slower than the tape IO. Thus
     * we must assure that the buffer following the last tape file
     * buffer is empty which means that the currently active disk IO
     * thread has finished with it.
     */
    end_of_tpfile = FALSE;
    break_and_return = FALSE;
    for (;;) {
        i = *indxp;
        /*
         * Synchronize access to next buffer
         */
        TP_STATUS(RTCP_PS_WAITMTX);
        rc = Cthread_mutex_lock_ext(databufs[i]->lock);
        TP_STATUS(RTCP_PS_NOBLOCKING);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"TapeToMemory() Cthread_mutex_lock_ext(): %s\n",
                sstrerror(serrno));
            return(-1);
        }
        /*
         * Wait until it is empty.
         */
        while (databufs[i]->flag == BUFFER_FULL) {
            rtcp_log(LOG_DEBUG,"TapeToMemory() wait on buffer[%d]->flag=%d\n",
                i,databufs[i]->flag);
            TP_STATUS(RTCP_PS_WAITCOND);
            rc = Cthread_cond_wait_ext(databufs[i]->lock);
            TP_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) {
                rtcp_log(LOG_ERR,"TapeToMemory() Cthread_cond_wait_ext(): %s\n",
                    sstrerror(serrno));
                return(-1);
            }
            if ( rtcpd_CheckProcError() & RTCP_FAILED ) {
                (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                return(-1);
            }
        }
        rtcp_log(LOG_DEBUG,"TapeToMemory() buffer %d empty\n",i);
        if ( end_of_tpfile == FALSE ) {
            /*
             * Set the actual buffer size to match current block size
             */
            databufs[i]->length = current_bufsz;
            /*
             * Reset switches (may remain old settings from a previous file).
             */
            databufs[i]->end_of_tpfile = FALSE;
            databufs[i]->last_buffer = FALSE;
            /*
             * Copy the data from tape
             */
            last_sz = 0;
            for (j=*firstblk; j*blksiz < databufs[i]->length; j++) {
                /*
                 * Max size may have been exceeded even before we
                 * read any data at all. This happens when we are 
                 * concatenating on disk and max size is already 
                 * exceeded already at entry (i.e. startsize > maxsize).
                 * We still need to flag the buffer full to wake up
                 * the disk IO thread.
                 * Note that in the normal case (where we already have 
                 * read some data) the max size limit is tested after 
                 * the read. In that case the following test is always
                 * false.
                 */
                if ( (filereq->maxsize > 0) && 
                     (file->tapebytes_sofar >= filereq->maxsize) ) {
                    /*
                     * Nothing will be copied!
                     */
                    filereq->bytes_out = 0;
                    end_of_tpfile = TRUE;
                    continue;
                }

                /*
                 * Read a block
                 */
                end_of_tpfile = FALSE;
                nb_bytes = blksiz;

                /*
                 * >>>>>>>>>>> read from tape <<<<<<<<<<<<<
                 */
                TP_STATUS(RTCP_PS_READ);
                rc = tread(tape_fd,
                           databufs[i]->buffer + j*blksiz,
                           nb_bytes,
                           tape,
                           file);
                TP_STATUS(RTCP_PS_NOBLOCKING);
                if ( rc == -1 || (rtcpd_CheckProcError() & RTCP_FAILED) ) {
                    if ( rc == -1 ) 
                        rtcp_log(LOG_ERR,"TapeToMemory() tape read error\n");
                    (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                    (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                    return(-1);
                }
                if ( rc == 0 ) {
                    rtcpd_CheckReqStatus(NULL,file,NULL,&severity);
                    if ( severity & RTCP_NEXTRECORD ) {
                        /*
                         * Bad block skipped
                         */
                        nb_skipped_blks++;
                    } else if ( file->eovflag == 1 ) {
                        /*
                         * EOV reached
                         */
                    } else {
                        /*
                         * EOF reached
                         */
                        end_of_tpfile = TRUE;
                    }
                    break;
                }
                last_sz += rc;
                file->tapebytes_sofar += rc;
                TP_SIZE(rc);
            } /* End of for (j=...) */
            /*  
             * Set the actual data length. Should be the
             * same as the current buffer length except for
             * the last buffer of the file.
             */
            databufs[i]->data_length += last_sz;
            databufs[i]->bufendp = last_sz;
            if ( lrecl > 0 ) {
                filereq->nbrecs += last_sz / lrecl;
                if ( (last_sz % lrecl) != 0 ) filereq->nbrecs++;
                /*
                 * Check if we already reached max number of
                 * records. If so we substract buffer sizes accordingly
                 * and signal end of file with this buffer
                 */
                if ( (filereq->maxnbrec > 0) &&
                     (filereq->nbrecs > filereq->maxnbrec) ) {
                    databufs[i]->data_length = databufs[i]->data_length -
                        (int)(filereq->nbrecs - filereq->maxnbrec)*lrecl;
                    file->tapebytes_sofar -= 
                        (filereq->nbrecs - filereq->maxnbrec)*lrecl;
                    filereq->nbrecs = filereq->maxnbrec;
                    end_of_tpfile = TRUE;
                    severity = RTCP_OK | RTCP_LIMBYSZ;
                    rtcpd_SetReqStatus(NULL,file,EFBIG,severity);
                }
            }
            /*
             * Check if maxsize has been reached for this file
             */
            if ((filereq->maxsize > 0) &&
                (filereq->maxsize <= file->tapebytes_sofar) ) {
                severity = RTCP_OK | RTCP_LIMBYSZ;
                rtcpd_SetReqStatus(NULL,file,EFBIG,severity);
                end_of_tpfile = TRUE;
            }

            if ( end_of_tpfile == TRUE ) {
                databufs[i]->end_of_tpfile = TRUE;
                /*
                 * Need to close here to get compression
                 * statistics etc. before the disk IO
                 * thread send the info. to the client
                 */
                TP_STATUS(RTCP_PS_CLOSE);
                rc = tclose(tape_fd,tape,file);
                TP_STATUS(RTCP_PS_NOBLOCKING);
                if ( rc == -1 ) {
                    rtcp_log(LOG_ERR,"TapeToMemory() tclose(): %s\n",
                        sstrerror(serrno));
                    (void)Cthread_cond_broadcast_ext(databufs[i]->lock);
                    (void)Cthread_mutex_unlock_ext(databufs[i]->lock);
                    return(-1);
                }
                tape_fd = -1;
            }

            /*
             * Set the buffer semaphore
             */
            databufs[i]->flag = BUFFER_FULL;
            /*
             * Always start next iteration with a new buffer boundary
             */
            *indxp = (*indxp+1) % nb_bufs;
            *firstblk = 0;
        } else {
            /*
             * End of tape file was reached in previous iteration.
             * At least one empty buffer follows the last written
             * buffer for this tape file. We can therefore signal
             * to the control thread that a new disk IO thread can
             * be started.
             */
             /*
              * Note down the actual file size to 
              * allow for start up of next disk IO thread.
              */
             if ((filereq->maxsize > 0) &&
                 (filereq->maxsize <= file->tapebytes_sofar) ) {
                 /*
                  * Truncated due to user specified max size. 
                  * Re-calculate number of bytes out and number
                  * of tape records.
                  */
                 filereq->bytes_out = filereq->maxsize - filereq->startsize;
                 if ( lrecl > 0 )
                     filereq->nbrecs = filereq->bytes_out / lrecl;
             } else
                 filereq->bytes_out = file->tapebytes_sofar - 
                     filereq->startsize;

             /*
              * Tell main control thread that we finished by
              * incrementing the number of reserved buffers.
              */
             TP_STATUS(RTCP_PS_WAITMTX);
             rc = Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
             TP_STATUS(RTCP_PS_NOBLOCKING);
             if ( rc == -1 ) {
                 rtcp_log(LOG_ERR,"TapeToMemory() Cthread_mutex_lock_ext(proc_cntl): %s\n",
                     sstrerror(serrno));
                 return(-1);
             }
             proc_cntl.nb_reserved_bufs = 0;
             /*
              * Signal to main thread that we changed cntl info
              */
             rc = Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
             if ( rc == -1 ) {
                 rtcp_log(LOG_ERR,"TapeToMemory() Cthread_cond_broadcast_ext(proc_cntl): %s\n",
                     sstrerror(serrno));
                 return(-1);
             }
             rc = Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
             if ( rc == -1 ) {
                 rtcp_log(LOG_ERR,"TapeToMemory() Cthread_mutex_unlock_ext(proc_cntl): %s\n",
                     sstrerror(serrno));
                 return(-1);
             }
             /*
              * Set switch to break out and return after having
              * signalled and released the buffer.
              */
             break_and_return = TRUE;
        }
        if ( databufs[i]->flag == BUFFER_FULL ) 
            rtcp_log(LOG_DEBUG,"TapeToMemory() flag buffer %d full\n",i);
        rc = Cthread_cond_broadcast_ext(databufs[i]->lock);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"TapeToMemory() Cthread_cond_broadcast_ext(): %s\n",
                sstrerror(serrno));
            return(-1);
        }
        rc = Cthread_mutex_unlock_ext(databufs[i]->lock);
        if ( rc == -1 ) {
            rtcp_log(LOG_ERR,"TapeToMemory() Cthread_mutex_unlock_ext(): %s\n",
                sstrerror(serrno));
            return(-1);
        }

        if ( break_and_return == TRUE ) break;

        /*
         * Report if there were skipped blocks
         */
        if ( nb_skipped_blks > 0 ) {
            if ( (severity & RTCP_LIMBYSZ) != 0  ) {
                severity = RTCP_OK | RTCP_TPE_LSZ;
                rtcpd_SetReqStatus(NULL,file,EFBIG,severity);
            } else {
                severity = RTCP_OK | RTCP_BLKSKPD;
                rtcpd_SetReqStatus(NULL,file,EIO,severity);
            }
        }

        /*
         * Break out to return if there were too many skipped blocks
         */
        if ( nb_skipped_blks > MAXNBSKIPS ) {
            severity = RTCP_FAILED | RTCP_MNYPARY | RTCP_USERR;
            rtcpd_SetReqStatus(NULL,file,EIO,severity);
            return(-1);
        }

        /*
         * If end of volume, break out from inifinite loop
         * and return to main tape I/O thread
         */
        if ( file->eovflag == 1 ) break;

        /*
         * Has something fatal happened while we were occupied
         * reading from the tape?
         */
        if ( rtcpd_CheckProcError() & RTCP_FAILED ) return(-1);
    } /* for (;;) */
    if ( tape_fd >= 0 ) {
        TP_STATUS(RTCP_PS_CLOSE);
        rc = tclose(tape_fd,tape,file);
        TP_STATUS(RTCP_PS_NOBLOCKING);
    }
    return(rc);
}

/*
 * This horrible macro prevents us to always repeate the same code
 * In addition to the return code from any blocking (or non-blocking) 
 * call we need to check the setting of the global processing error
 * to see if e.g. a disk IO thread has failed.
 */
#define CHECK_PROC_ERR(X,Y,Z) { \
    if ( rc == -1 || (rtcpd_CheckProcError() & RTCP_FAILED) != 0 ) { \
        if ( rc == -1 ) {\
            (void) tellClient(&client_socket,X,Y,rc); \
            rtcp_log(LOG_ERR,"tapeIOthread() %s\n",Z); \
            rtcpd_SetProcError(RTCP_FAILED); \
        }\
        if ( tape_fd != -1 ) tcloserr(tape_fd,X,Y); \
        if ( (rtcpd_CheckProcError() & RTCP_LOCAL_RETRY) == 0 ) \
            (void)rtcpd_Release((X),NULL); \
        rtcp_CloseConnection(&client_socket); \
        (void) rtcpd_CtapeFree(); \
        return((void *)&failure); \
    }}

void *tapeIOthread(void *arg) {
    tape_list_t *tape, *nexttape;
    file_list_t *nextfile, *tmpfile;
    SOCKET client_socket;
    u_signed64 totsz;
    int indxp = 0;
    int firstblk = 0;
    int tape_fd = -1;
    int rc,BroadcastInfo;

    if ( arg == NULL ) {
        rtcp_log(LOG_ERR,"tapeIOthread() received NULL argument\n");
        rtcpd_SetProcError(RTCP_FAILED);
        return((void *)&failure);
    }
    tape = ((thread_arg_t *)arg)->tape;
    client_socket = ((thread_arg_t *)arg)->client_socket;
    free(arg);

    /*
     * Initialize the tapeIO processing status
     */
    proc_stat.tapeIOstatus.nbbytes = 0;
    TP_STATUS(RTCP_PS_NOBLOCKING);

    /*
     * Reset flag for last block. On UNIX we are normally in a newly
     * forked process but this is not the case on NT (where we currently
     * only support one request at the time).
     */
    last_block_done = 0;

    if ( tape->local_retry == 0 ) {
        /*
         * Initialize Ctape error message buffer
         */
        rc = rtcpd_CtapeInit();
        if ( rc == -1 ) {
            CHECK_PROC_ERR(tape,NULL,"rtcpd_CtapeInit() error");
        }
        /*
         * Reserve the unit (client should have made sure that all
         * requested volumes are in same dgn).
         */
        TP_STATUS(RTCP_PS_RESERVE);
        rc = rtcpd_Reserve(tape);
        TP_STATUS(RTCP_PS_NOBLOCKING);
        if ( rc == -1 ) {
            (void)rtcpd_Deassign(-1,&tape->tapereq);
        }
        CHECK_PROC_ERR(tape,NULL,"rtcpd_Reserv() error");
    }
    /*
     * Main loop over all tapes requested
     */
    totsz = 0;
    CLIST_ITERATE_BEGIN(tape,nexttape) {
        /*
         * Mount the volume
         */
        if ( tape->local_retry == 0 ) {
            TP_STATUS(RTCP_PS_MOUNT);
            rc = rtcpd_Mount(nexttape);
            TP_STATUS(RTCP_PS_NOBLOCKING);
            if ( rc == -1 ) (void)rtcpd_Deassign(-1,&tape->tapereq);
            CHECK_PROC_ERR(nexttape,NULL,"rtcpd_Mount() error");
        }

        CLIST_ITERATE_BEGIN(nexttape->file,nextfile) {
            /*
             * If we are concatenating to tape all involved file
             * requests are done in a single iteration. This has to
             * be so to avoid partial blocks in the middle of tape
             * files.
             */
            if ( (nextfile->filereq.proc_status != RTCP_FINISHED) &&
                 ((nexttape->tapereq.mode == WRITE_DISABLE) ||
                  ((nexttape->tapereq.mode == WRITE_ENABLE) &&
                   (nextfile->filereq.concat == NOCONCAT))) ) {
                /*
                 * Position the volume
                 */
                TP_STATUS(RTCP_PS_POSITION);
                rc = rtcpd_Position(nexttape,nextfile);
                TP_STATUS(RTCP_PS_NOBLOCKING);
                CHECK_PROC_ERR(NULL,nextfile,"rtcpd_Position() error");

                /*
                 * Fill in missing information (if any). In case
                 * we are writing and the blocksize info is not
                 * specified by user (i.e. we need to take it from
                 * label) we must to synchronise with the disk IO 
                 * thread before updating the information.
                 */
                BroadcastInfo = FALSE;
                if ( nexttape->tapereq.mode == WRITE_ENABLE &&
                    nextfile->filereq.blocksize < 0 ) {
                    TP_STATUS(RTCP_PS_WAITMTX);
                    rc = Cthread_mutex_lock_ext(proc_cntl.cntl_lock);
                    TP_STATUS(RTCP_PS_NOBLOCKING);
                    CHECK_PROC_ERR(NULL,nextfile,"Cthread_mutex_lock_ext() error");
                    BroadcastInfo = TRUE;
                }
                TP_STATUS(RTCP_PS_INFO);
                rc = rtcpd_Info(nexttape,nextfile);
                TP_STATUS(RTCP_PS_NOBLOCKING);
                CHECK_PROC_ERR(NULL,nextfile,"rtcpd_Info() error");
                if ( BroadcastInfo == TRUE ) {
                    /*
                     * We are concatenating to tape and blocksize was
                     * not specified by user. Either a default or
                     * the blocksize of the tape file to be overwritten
                     * has been returned by Ctape_info(). We must now update
                     * all subsequent file requests within this concatenation
                     * since there will be no call to Ctape_info() for them.
                     */
                    CLIST_ITERATE_BEGIN(nextfile,tmpfile) {
                        if ( tmpfile->filereq.tape_fseq !=
                             nextfile->filereq.tape_fseq ) break;
                        tmpfile->filereq.blocksize = nextfile->filereq.blocksize;
                        tmpfile->filereq.recordlength = nextfile->filereq.recordlength;
                        tmpfile->filereq.proc_status == RTCP_POSITIONED;
                        tmpfile->filereq.TStartPosition = nextfile->filereq.TStartPosition;
                        tmpfile->filereq.TEndPosition = nextfile->filereq.TEndPosition;
                    } CLIST_ITERATE_END(nextfile,tmpfile);
                    rc = Cthread_cond_broadcast_ext(proc_cntl.cntl_lock);
                    CHECK_PROC_ERR(NULL,nextfile,"Cthread_cond_broadcast_ext() error");
                    rc = Cthread_mutex_unlock_ext(proc_cntl.cntl_lock);
                    CHECK_PROC_ERR(NULL,nextfile,"Cthread_mutex_unlock_ext() error");
                }

                nextfile->filereq.proc_status = RTCP_POSITIONED;

                tellClient(&client_socket,nexttape,NULL,rc);
                tellClient(&client_socket,NULL,nextfile,rc);
                TP_STATUS(RTCP_PS_STAGEUPDC);
                rc = rtcpd_stageupdc(nexttape,nextfile);
                TP_STATUS(RTCP_PS_NOBLOCKING);

                /*
                 * Open the tape file
                 */
                proc_stat.tapeIOstatus.tape_fseq = 
                    nextfile->filereq.tape_fseq;
                TP_STATUS(RTCP_PS_OPEN);
                rc = topen(nexttape,nextfile);
                TP_STATUS(RTCP_PS_NOBLOCKING);
                CHECK_PROC_ERR(NULL,nextfile,"topen() error");
                tape_fd = rc;

                if ( nexttape->tapereq.mode == WRITE_ENABLE ) {
                    rc = MemoryToTape(tape_fd,&indxp,&firstblk,
                                      nexttape,nextfile);
                    CHECK_PROC_ERR(NULL,nextfile,"MemoryToTape() error");
                } else {
                    rc = TapeToMemory(tape_fd,&indxp,&firstblk,
                                      nexttape,nextfile);
                    CHECK_PROC_ERR(NULL,nextfile,"TapeToMemory() error");
                }
                /*
                 * Tape file is closed in MemoryToTape() or TapeToMemor()
                 */
                tape_fd = -1;

                if ( nextfile->eovflag == 1 ) {
                    /*
                     * File is spanning two volumes. It must
                     * be the last file of the request. Release
                     * to unload but keep drive reservation.
                     */
                    if ( nextfile->next != nexttape->file ||
                         nexttape->next == tape ) {
                        if ( nexttape->next == tape ) {
                            /*
                             * No second volume specified
                             */
                            rtcpd_AppendClientMsg(NULL,nextfile,RT124,
                                                  "CPDSKTP");
                            if ( nexttape->tapereq.mode == WRITE_ENABLE )
                                serrno = ENOSPC;
                            else
                                serrno = EFBIG;
                        } else {
                           rtcpd_AppendClientMsg(NULL,nextfile,RT127,
                                                  "CPDSKTP");
                           serrno = EINVAL;
                        }
                        rtcpd_SetReqStatus(NULL,nextfile,serrno,
                            RTCP_FAILED | RTCP_USERR);
                        tellClient(&client_socket,NULL,nextfile,-1);
                        TP_STATUS(RTCP_PS_STAGEUPDC);
                        rc = rtcpd_stageupdc(nexttape,nextfile);
                        TP_STATUS(RTCP_PS_NOBLOCKING);
                        rtcpd_SetProcError(RTCP_FAILED);
                        TP_STATUS(RTCP_PS_RELEASE);
                        rtcpd_Release(nexttape,NULL);
                        TP_STATUS(RTCP_PS_NOBLOCKING);
                        (void) rtcpd_CtapeFree();
                        return((void *)&failure);
                    }
                    break;
                }

                /*
                 * Keep track of total size stored with this file 
                 * request element. If we concatenate on tape the 
                 * subsequent file request elements involved in the 
                 * concatenation have been implicitly processed 
                 * with this file request.
                 */
                totsz = nextfile->tapebytes_sofar;

                /*
                 * When concatenate on disk, make sure that the
                 * maxsize and maxnbrec are taken into account. We
                 * also check that the maxsizes are set consistently
                 * between the file requests. If not, the error is
                 * logged and the maxsize is corrected to the first
                 * specified value.
                 */
                if ((nexttape->tapereq.mode == WRITE_DISABLE) &&
                    (nextfile->filereq.disk_fseq ==
                     nextfile->next->filereq.disk_fseq) ) {
                    if ( nextfile->filereq.maxsize > 0 ) {
                        if ( nextfile->filereq.maxsize !=
                            nextfile->next->filereq.maxsize ) {
                            rtcp_log(LOG_INFO,"tapeIOthread() inconsistent maxsize for concat to disk. Repaired.\n");
                            nextfile->next->filereq.maxsize = 
                                nextfile->filereq.maxsize;
                        }
                        if ( nextfile->tapebytes_sofar <
                             nextfile->filereq.maxsize ) 
                             nextfile->next->filereq.startsize = 
                                 nextfile->tapebytes_sofar;
                        else
                             nextfile->next->filereq.startsize = 
                                 nextfile->filereq.maxsize;
                    }
                }
            } /* !FINISHED && NOCONCAT */
            if ( nexttape->tapereq.mode == WRITE_ENABLE ) {
                /*
                 * Decrement the total (concatenated) size and check
                 * if it is consistent with sum of the individual
                 * file sizes. This may happen if the max number
                 * of records limit was specified by the user. In
                 * this case we must make sure the copied file size
                 * is updated.
                 */
                if ( totsz > nextfile->filereq.bytes_in ) {
                    totsz -= nextfile->filereq.bytes_in;
                } else {
                    nextfile->filereq.bytes_in = totsz;
                    totsz = 0;
                }
                if ( nextfile->filereq.concat == CONCAT ) {
                    /*
                     * When concatenating to tape all disk file
                     * requests are clumped together and treated
                     * in a single call to MemoryToTape(). This means
                     * that the subsequent requests are not passed
                     * to topen() and tclose() and hence the start
                     * and end transfer time for each individual file
                     * is unknown. We simply set it to the total (all
                     * concatenated files together) start and end.
                     */
                    nextfile->filereq.TStartTransferTape = 
                        nextfile->prev->filereq.TStartTransferTape;
                    nextfile->filereq.TEndTransferTape = 
                        nextfile->prev->filereq.TEndTransferTape;
                    nextfile->filereq.nbrecs = 
                        nextfile->prev->filereq.nbrecs;
                }
                nextfile->filereq.proc_status = RTCP_FINISHED;
                tellClient(&client_socket,NULL,nextfile,0);
                TP_STATUS(RTCP_PS_STAGEUPDC);
                rc = rtcpd_stageupdc(nexttape,nextfile);
                TP_STATUS(RTCP_PS_NOBLOCKING);
            }
        } CLIST_ITERATE_END(nexttape->file,nextfile);
        if ( nexttape->next != nexttape ) {
            TP_STATUS(RTCP_PS_RELEASE);
            rc = rtcpd_Release(nexttape,nexttape->file);
            TP_STATUS(RTCP_PS_NOBLOCKING);
            CHECK_PROC_ERR(nexttape,NULL,"rtcpd_Release() error");
        }
    } CLIST_ITERATE_END(tape,nexttape);

    TP_STATUS(RTCP_PS_RELEASE);
    rtcpd_Release(nexttape,NULL);
    TP_STATUS(RTCP_PS_NOBLOCKING);
    tellClient(&client_socket,NULL,NULL,0);
    (void) rtcp_CloseConnection(&client_socket);
    (void) rtcpd_CtapeFree();
    return((void *)&success);
}

int rtcpd_StartTapeIO(rtcpClientInfo_t *client, tape_list_t *tape) {
    int rc;
    thread_arg_t *tharg;

    if ( client == NULL || tape == NULL ) {
        serrno = EINVAL;
        return(-1);
    }

    tharg = (thread_arg_t *)malloc(sizeof(thread_arg_t));
    if ( tharg == NULL ) {
        rtcp_log(LOG_ERR,"rtcpd_StartTapeIO() malloc(): %s\n",
            sstrerror(errno));
        return(-1);
    }

    /*
     * Open a separate connection to client
     */
    tharg->client_socket = INVALID_SOCKET;
    rc = rtcpd_ConnectToClient(&tharg->client_socket,
                               client->clienthost,
                               &client->clientport);
    if ( rc == -1 ) {
        rtcp_log(LOG_ERR,"rtcpd_StartTapeIO() rtcp_ConnectToClient(%s,%d): %s\n",
                 client->clienthost,client->clientport,sstrerror(serrno));
        return(-1);
    }

    tharg->tape = tape;

    rc = Cthread_create(tapeIOthread,(void *)tharg);
    if ( rc == -1 )
        rtcp_log(LOG_ERR,"rtcpd_StartTapeIO() Cthread_create(): %s\n",
            sstrerror(serrno));
    else 
        proc_cntl.tapeIOthreadID = rc;

    return(rc);
}

int rtcpd_WaitTapeIO(int *status) {
    int rc,*_status;

    rtcp_log(LOG_INFO,"rtcpd_WaitTapeIO() waiting for tape I/O thread\n");

    _status = NULL;
    rc = Cthread_join(proc_cntl.tapeIOthreadID,&_status);
    if ( rc == -1 ) {
        rtcp_log(LOG_ERR,"rtcpd_WaitTapeIO() Cthread_joint(): %s\n",
            sstrerror(serrno));
    } else {
        if ( status != NULL && _status != NULL ) *status = *_status;
    }
    return(rc);
}