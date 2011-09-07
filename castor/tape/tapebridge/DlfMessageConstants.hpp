/* This file was generated by ./DlfMessagesCodeGenerator on Wed Sep  7 09:39:06 CEST 2011
 */

/******************************************************************************
 *             castor/tape/tapebridge/DlfMessageConstants.hpp
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
 * @author Steven Murray Steven.Murray@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_TAPEBRIDGE_TAPEBRIDGEDLFMESSAGECONSTANTS_HPP
#define CASTOR_TAPE_TAPEBRIDGE_TAPEBRIDGEDLFMESSAGECONSTANTS_HPP 1


namespace castor {
namespace tape {
namespace tapebridge {
enum TapeBridgeDlfMessages {
TAPEBRIDGE_NULL=0, /* " - " */
TAPEBRIDGE_STARTED=1, /* "tapebridged started" */
TAPEBRIDGE_FAILED_TO_START=2, /* "tapebridged failed to start" */
TAPEBRIDGE_RECEIVED_VDQM_CONNECTION=3, /* "Received a connection from VDQM" */
TAPEBRIDGE_HANDLE_VDQM_REQUEST_EXCEPT=4, /* "Exception raised when handling a request from the VDQM" */
TAPEBRIDGE_FAILED_TO_READ_MESSAGE_HEADER=5, /* "Failed to read message header" */
TAPEBRIDGE_UNKNOWN_MAGIC=6, /* "Unknown magic number" */
TAPEBRIDGE_UNKNOWN_REQUEST_TYPE=7, /* "Unknown request type" */
TAPEBRIDGE_FAILED_TO_READ_MESSAGE_BODY=8, /* "Failed to read message body" */
TAPEBRIDGE_RECEIVE_RTCOPY_JOB=9, /* "Receiving remote-copy job" */
TAPEBRIDGE_RECEIVED_RTCOPY_JOB=10, /* "Received remote-copy job" */
TAPEBRIDGE_SUBMITTING_RTCOPY_JOB_TO_RTCPD=11, /* "Submitting remote-copy job to RTCPD" */
TAPEBRIDGE_FAILED_TO_SUBMIT_JOB_TO_RTCPD=12, /* "Failed to submit remote-copy job to RTCPD" */
TAPEBRIDGE_FAILED_TO_MARSHALL_RTCP_ACKN=13, /* "Failed to marshall RTCP acknowledge message" */
TAPEBRIDGE_FAILED_TO_SEND_RTCOPY_JOB_REPLY_TO_VDQM=14, /* "Failed to send remote-copy job reply to VDQM" */
TAPEBRIDGE_FAILED_TO_PROCESS_RCOPY_JOB_SUBMISSION=15, /* "Failed to process remote-copy job submission message" */
TAPEBRIDGE_RTCPD_HANDLER_SOCKET_IS_NULL=16, /* "The RtcpdHandlerThread has been passed a NULL socket pointer" */
TAPEBRIDGE_INITIAL_RTCPD_CALLBACK=17, /* "Received initial callback connection from RTCPD" */
TAPEBRIDGE_RTCPD_CALLBACK=18, /* "Received a callback connection from RTCPD" */
TAPEBRIDGE_GOT_VOLREQID=19, /* "Got volume request ID from RTCPD" */
TAPEBRIDGE_FAILED_TO_GET_VOLREQID=20, /* "Failed to get volume request ID from RTCPD" */
TAPEBRIDGE_FAILED_TO_MARSHALL_RTCOPY_JOB_REPLY_MESSAGE=21, /* "Failed to marshall remote-copy job reply message" */
TAPEBRIDGE_SIGNALLED_NO_MORE_REQUESTS=22, /* "Signalled no more requests to RTCPD" */
TAPEBRIDGE_FAILED_TO_COORDINATE_REMOTE_COPY=23, /* "Failed to coordinate remote-copy" */
TAPEBRIDGE_TRANSFER_FAILED=24, /* "Transfer failed" */
TAPEBRIDGE_GAVE_REQUEST_FOR_MORE_WORK=25, /* "Gave request for more work to RTCPD" */
TAPEBRIDGE_DATA_ON_INITIAL_RTCPD_CONNECTION=26, /* "Data has arrived on the initial RTCPD connection" */
TAPEBRIDGE_RECEIVED_RTCP_ENDOF_REQ=27, /* "Received RTCP_ENDOF_REQ" */
TAPEBRIDGE_TAPE_POSITIONED=28, /* "Tape positioned" */
TAPEBRIDGE_FILE_TRANSFERED_DEPRECATED=29, /* "File transfered" */
TAPEBRIDGE_GET_REQUEST_INFO_FROM_RTCPD=30, /* "Getting request information from RTCPD" */
TAPEBRIDGE_GOT_REQUEST_INFO_FROM_RTCPD=31, /* "Got request information from RTCPD" */
TAPEBRIDGE_TOLD_CLIENT_TO_START_TRANSFER=32, /* "Told client to start the transfer protocol" */
TAPEBRIDGE_GET_VOLUME_FROM_CLIENT=33, /* "Getting volume from client" */
TAPEBRIDGE_GOT_VOLUME_FROM_CLIENT=34, /* "Got volume from client" */
TAPEBRIDGE_GOT_NO_MORE_FILES_FROM_CLIENT=35, /* "Got no more files from client" */
TAPEBRIDGE_GET_FILE_TO_MIGRATE_FROM_CLIENT=36, /* "Getting file to migrate from client" */
TAPEBRIDGE_GOT_FILE_TO_MIGRATE_FROM_CLIENT=37, /* "Got file to migrate from client" */
TAPEBRIDGE_GET_FILE_TO_RECALL_FROM_CLIENT=38, /* "Getting file to recall from client" */
TAPEBRIDGE_GOT_FILE_TO_RECALL_FROM_CLIENT=39, /* "Got file to recall from client" */
TAPEBRIDGE_GIVE_VOLUME_TO_RTCPD=40, /* "Giving volume to RTCPD" */
TAPEBRIDGE_GAVE_VOLUME_TO_RTCPD=41, /* "Gave volume to RTCPD" */
TAPEBRIDGE_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK=42, /* "Asking RTCPD to request more recall work" */
TAPEBRIDGE_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK=43, /* "Asked RTCPD to request more recall work" */
TAPEBRIDGE_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK=44, /* "Asking RTCPD to request more migrate work" */
TAPEBRIDGE_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK=45, /* "Asked RTCPD to request more migrate work" */
TAPEBRIDGE_TELL_RTCPD_DUMP_TAPE=46, /* "Telling RTCPD to dump tape" */
TAPEBRIDGE_TOLD_RTCPD_DUMP_TAPE=47, /* "Told RTCPD to dump tape" */
TAPEBRIDGE_TELL_RTCPD_END_OF_FILE_LIST=48, /* "Telling RTCPD end of file list" */
TAPEBRIDGE_TOLD_RTCPD_END_OF_FILE_LIST=49, /* "Told RTCPD end of file list" */
TAPEBRIDGE_TELL_RTCPD_TO_ABORT=50, /* "Telling RTCPD to abort" */
TAPEBRIDGE_TOLD_RTCPD_TO_ABORT=51, /* "Told RTCPD to abort" */
TAPEBRIDGE_GIVE_RECALL_FILE_TO_RTCPD=52, /* "Giving recall file to RTCPD" */
TAPEBRIDGE_GAVE_RECALL_FILE_TO_RTCPD=53, /* "Gave recall file to RTCPD" */
TAPEBRIDGE_GIVE_MIGRATE_FILE_TO_RTCPD=54, /* "Giving migrate file to RTCPD" */
TAPEBRIDGE_GAVE_MIGRATE_FILE_TO_RTCPD=55, /* "Gave migrate file to RTCPD" */
TAPEBRIDGE_RECEIVE_MSGBODY_FROM_RTCPD=56, /* "Receiving message body from RTCPD" */
TAPEBRIDGE_RECEIVED_MSGBODY_FROM_RTCPD=57, /* "Received message body from RTCPD" */
TAPEBRIDGE_RECEIVE_FILERQSTBODY_FROM_RTCPD=58, /* "Receiving FileRqstBody from RTCPD" */
TAPEBRIDGE_RECEIVED_FILERQSTBODY_FROM_RTCPD=59, /* "Received FileRqstBody from RTCPD" */
TAPEBRIDGE_RECEIVE_FILERQSTERRBODY_FROM_RTCPD=60, /* "Receiving FileRqstErrBody from RTCPD" */
TAPEBRIDGE_RECEIVED_FILERQSTERRBODY_FROM_RTCPD=61, /* "Received FileRqstErrBody from RTCPD" */
TAPEBRIDGE_PROCESSING_TAPE_DISK_RQST=62, /* "Processing a tape/disk IO request from RTCPD" */
TAPEBRIDGE_PING_RTCPD=63, /* "Pinging RTCPD" */
TAPEBRIDGE_PINGED_RTCPD=64, /* "Pinged RTCPD" */
TAPEBRIDGE_SEND_ACK_TO_RTCPD=65, /* "Sending acknowledge to RTCPD" */
TAPEBRIDGE_SENT_ACK_TO_RTCPD=66, /* "Sent acknowledge to RTCPD" */
TAPEBRIDGE_SEND_HEADER_TO_RTCPD=67, /* "Sending message header to RTCPD" */
TAPEBRIDGE_SENT_HEADER_TO_RTCPD=68, /* "Sent message header to RTCPD" */
TAPEBRIDGE_RECEIVE_TAPERQSTBODY=69, /* "Receiving tape request message body" */
TAPEBRIDGE_RECEIVED_TAPERQSTBODY=70, /* "Received tape request message body" */
TAPEBRIDGE_RECEIVE_GIVEOUTPBODY=71, /* "Receiving GIVE_OUTP message body" */
TAPEBRIDGE_RECEIVED_GIVEOUTPBODY=72, /* "Received GIVE_OUTP message body" */
TAPEBRIDGE_RECEIVE_TAPERQSTERRBODY=73, /* "Receiving tape request message body with error appendix" */
TAPEBRIDGE_RECEIVED_TAPERQSTERRBODY=74, /* "Received tape request message body with error appendix" */
TAPEBRIDGE_SEND_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=75, /* "Sending delayed acknowledge of request for more work to RTCPD" */
TAPEBRIDGE_SENT_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=76, /* "Sent delayed acknowledge of request for more work to RTCPD" */
TAPEBRIDGE_CREATED_RTCPD_CALLBACK_PORT=77, /* "Created TCP/IP port for RTCPD callbacks" */
TAPEBRIDGE_CONNECTION_CLOSED_BY_RTCPD=78, /* "Connection closed by RTCPD" */
TAPEBRIDGE_CLOSED_CONNECTION=79, /* "Connection closed by tapebridge" */
TAPEBRIDGE_NOTIFY_CLIENT_FILE_MIGRATED=80, /* "Notifying client file migrated" */
TAPEBRIDGE_NOTIFIED_CLIENT_FILE_MIGRATED=81, /* "Notified client file migrated" */
TAPEBRIDGE_NOTIFY_CLIENT_FILE_RECALLED=82, /* "Notifying client file recalled" */
TAPEBRIDGE_NOTIFIED_CLIENT_FILE_RECALLED=83, /* "Notified client file recalled" */
TAPEBRIDGE_NOTIFY_CLIENT_END_OF_SESSION=84, /* "Notifying client end of session" */
TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_SESSION=85, /* "Notified client end of session" */
TAPEBRIDGE_GET_DUMP_PARAMETERS_FROM_CLIENT=86, /* "Getting dump parameters from client" */
TAPEBRIDGE_GOT_DUMP_PARAMETERS_FROM_CLIENT=87, /* "Got dump parameters from client" */
TAPEBRIDGE_NOTIFY_CLIENT_DUMP_MESSAGE=88, /* "Notifying client dump message" */
TAPEBRIDGE_NOTIFIED_CLIENT_DUMP_MESSAGE=89, /* "Notified client dump message" */
TAPEBRIDGE_FAILED_TO_NOTIFY_CLIENT_END_OF_SESSION=90, /* "Failed to notify client end of session" */
TAPEBRIDGE_NOTIFY_CLIENT_END_OF_FAILED_SESSION=91, /* "Notifying client end of failed session" */
TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_FAILED_SESSION=92, /* "Notified client end of failed session" */
TAPEBRIDGE_FAILED_TO_NOTIFY_CLIENT_END_OF_FAILED_SESSION=93, /* "Failed to notify client end of failed session" */
TAPEBRIDGE_SELECT_INTR=94, /* "Select interrupted" */
TAPEBRIDGE_ENTERING_BRIDGE_MODE=95, /* "Entering bridge mode" */
TAPEBRIDGE_PING_CLIENT=96, /* "Pinging client" */
TAPEBRIDGE_PINGED_CLIENT=97, /* "Pinged client" */
TAPEBRIDGE_CLOSED_INITIAL_CALLBACK_CONNECTION=98, /* "Closed initial callback connection" */
TAPEBRIDGE_RECEIVED_TAPE_INFO_FROM_VMGR=99, /* "Received tape information from VMGR" */
TAPEBRIDGE_SENT_FILETOMIGRATEREQUEST=100, /* "Sent FileToMigrateRequest to client" */
TAPEBRIDGE_RECEIVED_FILETOMIGRATE=101, /* "Received FileToMigrate from client" */
TAPEBRIDGE_SENT_FILETORECALLREQUEST=102, /* "Sent FileToRecallRequest to client" */
TAPEBRIDGE_RECEIVED_FILETORECALL=103, /* "Received FileToRecall from client" */
TAPEBRIDGE_SENT_FILEMIGRATEDNOTIFICATION=104, /* "Sent FileMigratedNotification to client" */
TAPEBRIDGE_SENT_FILERECALLEDNOTIFICATION=105, /* "Sent FileRecalledNotification to client" */
TAPEBRIDGE_RECEIVED_NOMOREFILES=106, /* "Recieved NoMoreFiles from client" */
TAPEBRIDGE_RECEIVED_ACK_OF_NOTIFICATION=107, /* "Recieved acknowledge of notification from client" */
TAPEBRIDGE_RECEIVED_ENDNOTIFCATIONERRORREPORT=108, /* "Received EndNotificationErrorReport from client" */
TAPEBRIDGE_RECEIVED_NOTIFCATIONACKNOWLEDGE=109, /* "Received NotificationAcknowledge from client" */
TAPEBRIDGE_SEND_END_OF_SESSION_TO_RTCPD=110, /* "Sending end-of-session to RTCPD" */
TAPEBRIDGE_FAILED_TO_PARSE_TPCONFIG=111, /* "Failed to parse TPCONFIG file" */
TAPEBRIDGE_PARSED_TPCONFIG=112, /* "Parsed TPCONFIG" */
TAPEBRIDGE_CLOSED_RTCPD_DISK_TAPE_CONNECTION_DUE_TO_PEER=113, /* "Closed rtcpd disk/tape IO control connection due to rtcpd closing its end" */
TAPEBRIDGE_TOO_MANY_DRIVES_IN_TPCONFIG=114, /* "There are more drives in the TPCONFIG file than there were when tapebridged was started.  Please restart tapebridged." */
TAPEBRIDGE_RECEIVED_TAPEBRIDGE_FLUSHEDTOTAPE=115, /* "Received TAPEBRIDGE_FLUSHEDTOTAPE" */
TAPEBRIDGE_CONFIG_PARAM=116, /* "Determined value of configuration parameter" */
TAPEBRIDGE_NOTIFY_CLIENT_END_OF_FAILED_SESSION_DUE_TO_FILE=117, /* "Notifying client end of failed session due to failed copy of tape file" */
TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_FAILED_SESSION_DUE_TO_FILE=118, /* "Notified client end of failed session due to failed copy of tape file" */
TAPEBRIDGE_FILE_WRITTEN_WITHOUT_FLUSH=119, /* "File written without flush" */
TAPEBRIDGE_FILE_READ=120, /* "File read" */
TAPEBRIDGE_ADDED_PENDING_MIGRATION_TO_STORE=121, /* "Added pending file-migration to store" */
TAPEBRIDGE_MARKED_PENDING_MIGRATION_WRITTEN_WITHOUT_FLUSH=122, /* "Marked pending file-migration in store as written without flush" */
TAPEBRIDGE_SENT_FILESTOMIGRATELISTREQUEST=123, /* "Sent FilesToMigrateListRequest to client" */
TAPEBRIDGE_SENT_FILESTORECALLLISTREQUEST=124, /* "Sent FilesToRecallListRequest to client" */
TAPEBRIDGE_RECEIVED_FILESTOMIGRATELIST=125, /* "Received FilesToMigrateList from client" */
TAPEBRIDGE_RECEIVED_FILESTORECALLLIST=126, /* "Received FilesToRecallList from client" */
TAPEBRIDGE_SENT_FILEMIGRATIONREPORTLIST=127, /* "Sent FileMigrationReportList to client" */
TAPEBRIDGE_SENT_FILERECALLREPORTLIST=128, /* "Sent FileRecallReportList to client" */
TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_SESSION_DUE_TO_FAILED_MIGRATION=129, /* "Notified client of end of session due to failed migration" */
TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_SESSION_DUE_TO_FAILED_RECALL=130, /* "Notified client of end of session due to failed recall" */
TAPEBRIDGE_RECEIVED_RTCP_WAITING=131, /* "Received an RTCP_WAITING message" */
TAPEBRIDGE_RECEIVED_RTCP_WAITING_ERROR=132 /* "Received an RTCP_WAITING error message" */
}; // enum TapeBridgeDlfMessages
} // namespace tapebridge
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_TAPEBRIDGE_TAPEBRIDGEDLFMESSAGECONSTANTS_HPP
