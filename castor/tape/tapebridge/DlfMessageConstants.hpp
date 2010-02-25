/* This file was generated by ./DlfMessagesCodeGenerator on Wed Feb 24 10:30:09 CET 2010
 */

/******************************************************************************
 *             castor/tape/tapeserver/DlfMessageConstants.hpp
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

#ifndef CASTOR_TAPE_TAPESERVER_TAPESERVERDLFMESSAGECONSTANTS_HPP
#define CASTOR_TAPE_TAPESERVER_TAPESERVERDLFMESSAGECONSTANTS_HPP 1


namespace castor {
namespace tape {
namespace tapeserver {
enum TapeServerDlfMessages {
TAPESERVER_NULL=0, /* " - " */
TAPESERVER_STARTED=1, /* "tapeserverd started" */
TAPESERVER_FAILED_TO_START=2, /* "tapeserverd failed to start" */
TAPESERVER_RECEIVED_VDQM_CONNECTION=3, /* "Received a connection from VDQM" */
TAPESERVER_HANDLE_VDQM_REQUEST_EXCEPT=4, /* "Exception raised when handling a request from the VDQM" */
TAPESERVER_FAILED_TO_READ_MESSAGE_HEADER=5, /* "Failed to read message header" */
TAPESERVER_UNKNOWN_MAGIC=6, /* "Unknown magic number" */
TAPESERVER_UNKNOWN_REQUEST_TYPE=7, /* "Unknown request type" */
TAPESERVER_FAILED_TO_READ_MESSAGE_BODY=8, /* "Failed to read message body" */
TAPESERVER_RECEIVE_RTCOPY_JOB=9, /* "Receiving remote-copy job" */
TAPESERVER_RECEIVED_RTCOPY_JOB=10, /* "Received remote-copy job" */
TAPESERVER_SUBMITTING_RTCOPY_JOB_TO_RTCPD=11, /* "Submitting remote-copy job to RTCPD" */
TAPESERVER_FAILED_TO_SUBMIT_JOB_TO_RTCPD=12, /* "Failed to submit remote-copy job to RTCPD" */
TAPESERVER_FAILED_TO_MARSHALL_RTCP_ACKN=13, /* "Failed to marshall RTCP acknowledge message" */
TAPESERVER_FAILED_TO_SEND_RTCOPY_JOB_REPLY_TO_VDQM=14, /* "Failed to send remote-copy job reply to VDQM" */
TAPESERVER_FAILED_TO_PROCESS_RCOPY_JOB_SUBMISSION=15, /* "Failed to process remote-copy job submission message" */
TAPESERVER_RTCPD_HANDLER_SOCKET_IS_NULL=16, /* "The RtcpdHandlerThread has been passed a NULL socket pointer" */
TAPESERVER_INITIAL_RTCPD_CALLBACK=17, /* "Received initial callback connection from RTCPD" */
TAPESERVER_RTCPD_CALLBACK=18, /* "Received a callback connection from RTCPD" */
TAPESERVER_GOT_VOLREQID=19, /* "Got volume request ID from RTCPD" */
TAPESERVER_FAILED_TO_GET_VOLREQID=20, /* "Failed to get volume request ID from RTCPD" */
TAPESERVER_FAILED_TO_MARSHALL_RTCOPY_JOB_REPLY_MESSAGE=21, /* "Failed to marshall remote-copy job reply message" */
TAPESERVER_SIGNALLED_NO_MORE_REQUESTS=22, /* "Signalled no more requests to RTCPD" */
TAPESERVER_FAILED_TO_COORDINATE_REMOTE_COPY=23, /* "Failed to coordinate remote-copy" */
TAPESERVER_TRANSFER_FAILED=24, /* "Transfer failed" */
TAPESERVER_GAVE_REQUEST_FOR_MORE_WORK=25, /* "Gave request for more work to RTCPD" */
TAPESERVER_DATA_ON_INITIAL_RTCPD_CONNECTION=26, /* "Data has arrived on the initial RTCPD connection" */
TAPESERVER_RECEIVED_RTCP_ENDOF_REQ=27, /* "Received RTCP_ENDOF_REQ" */
TAPESERVER_TAPE_POSITIONED=28, /* "Tape positioned" */
TAPESERVER_FILE_TRANSFERED=29, /* "File transfered" */
TAPESERVER_GET_REQUEST_INFO_FROM_RTCPD=30, /* "Getting request information from RTCPD" */
TAPESERVER_GOT_REQUEST_INFO_FROM_RTCPD=31, /* "Got request information from RTCPD" */
TAPESERVER_TOLD_CLIENT_TO_START_TRANSFER=32, /* "Told client to start the transfer protocol" */
TAPESERVER_GET_VOLUME_FROM_CLIENT=33, /* "Getting volume from client" */
TAPESERVER_GOT_VOLUME_FROM_CLIENT=34, /* "Got volume from client" */
TAPESERVER_GOT_NO_MORE_FILES_FROM_CLIENT=35, /* "Got no more files from client" */
TAPESERVER_GET_FILE_TO_MIGRATE_FROM_CLIENT=36, /* "Getting file to migrate from client" */
TAPESERVER_GOT_FILE_TO_MIGRATE_FROM_CLIENT=37, /* "Got file to migrate from client" */
TAPESERVER_GET_FILE_TO_RECALL_FROM_CLIENT=38, /* "Getting file to recall from client" */
TAPESERVER_GOT_FILE_TO_RECALL_FROM_CLIENT=39, /* "Got file to recall from client" */
TAPESERVER_GIVE_VOLUME_TO_RTCPD=40, /* "Giving volume to RTCPD" */
TAPESERVER_GAVE_VOLUME_TO_RTCPD=41, /* "Gave volume to RTCPD" */
TAPESERVER_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK=42, /* "Asking RTCPD to request more recall work" */
TAPESERVER_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK=43, /* "Asked RTCPD to request more recall work" */
TAPESERVER_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK=44, /* "Asking RTCPD to request more migrate work" */
TAPESERVER_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK=45, /* "Asked RTCPD to request more migrate work" */
TAPESERVER_TELL_RTCPD_DUMP_TAPE=46, /* "Telling RTCPD to dump tape" */
TAPESERVER_TOLD_RTCPD_DUMP_TAPE=47, /* "Told RTCPD to dump tape" */
TAPESERVER_TELL_RTCPD_END_OF_FILE_LIST=48, /* "Telling RTCPD end of file list" */
TAPESERVER_TOLD_RTCPD_END_OF_FILE_LIST=49, /* "Told RTCPD end of file list" */
TAPESERVER_TELL_RTCPD_TO_ABORT=50, /* "Telling RTCPD to abort" */
TAPESERVER_TOLD_RTCPD_TO_ABORT=51, /* "Told RTCPD to abort" */
TAPESERVER_GIVE_RECALL_FILE_TO_RTCPD=52, /* "Giving recall file to RTCPD" */
TAPESERVER_GAVE_RECALL_FILE_TO_RTCPD=53, /* "Gave recall file to RTCPD" */
TAPESERVER_GIVE_MIGRATE_FILE_TO_RTCPD=54, /* "Giving migrate file to RTCPD" */
TAPESERVER_GAVE_MIGRATE_FILE_TO_RTCPD=55, /* "Gave migrate file to RTCPD" */
TAPESERVER_RECEIVE_MSGBODY_FROM_RTCPD=56, /* "Receiving message body from RTCPD" */
TAPESERVER_RECEIVED_MSGBODY_FROM_RTCPD=57, /* "Received message body from RTCPD" */
TAPESERVER_RECEIVE_FILERQSTBODY_FROM_RTCPD=58, /* "Receiving FileRqstBody from RTCPD" */
TAPESERVER_RECEIVED_FILERQSTBODY_FROM_RTCPD=59, /* "Received FileRqstBody from RTCPD" */
TAPESERVER_RECEIVE_FILERQSTERRBODY_FROM_RTCPD=60, /* "Receiving FileRqstErrBody from RTCPD" */
TAPESERVER_RECEIVED_FILERQSTERRBODY_FROM_RTCPD=61, /* "Received FileRqstErrBody from RTCPD" */
TAPESERVER_PROCESSING_TAPE_DISK_RQST=62, /* "Processing a tape/disk IO request from RTCPD" */
TAPESERVER_PING_RTCPD=63, /* "Pinging RTCPD" */
TAPESERVER_PINGED_RTCPD=64, /* "Pinged RTCPD" */
TAPESERVER_SEND_ACK_TO_RTCPD=65, /* "Sending acknowledge to RTCPD" */
TAPESERVER_SENT_ACK_TO_RTCPD=66, /* "Sent acknowledge to RTCPD" */
TAPESERVER_SEND_HEADER_TO_RTCPD=67, /* "Sending message header to RTCPD" */
TAPESERVER_SENT_HEADER_TO_RTCPD=68, /* "Sent message header to RTCPD" */
TAPESERVER_RECEIVE_TAPERQSTBODY=69, /* "Receiving tape request message body" */
TAPESERVER_RECEIVED_TAPERQSTBODY=70, /* "Received tape request message body" */
TAPESERVER_RECEIVE_GIVEOUTPBODY=71, /* "Receiving GIVE_OUTP message body" */
TAPESERVER_RECEIVED_GIVEOUTPBODY=72, /* "Received GIVE_OUTP message body" */
TAPESERVER_RECEIVE_TAPERQSTERRBODY=73, /* "Receiving tape request message body with error appendix" */
TAPESERVER_RECEIVED_TAPERQSTERRBODY=74, /* "Received tape request message body with error appendix" */
TAPESERVER_SEND_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=75, /* "Sending delayed acknowledge of request for more work to RTCPD" */
TAPESERVER_SENT_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=76, /* "Sent delayed acknowledge of request for more work to RTCPD" */
TAPESERVER_CREATED_RTCPD_CALLBACK_PORT=77, /* "Created TCP/IP port for RTCPD callbacks" */
TAPESERVER_CONNECTION_CLOSED_BY_RTCPD=78, /* "Connection closed by RTCPD" */
TAPESERVER_CLOSED_CONNECTION=79, /* "Connection closed by tapeserver" */
TAPESERVER_NOTIFY_CLIENT_FILE_MIGRATED=80, /* "Notifying client file migrated" */
TAPESERVER_NOTIFIED_CLIENT_FILE_MIGRATED=81, /* "Notified client file migrated" */
TAPESERVER_NOTIFY_CLIENT_FILE_RECALLED=82, /* "Notifying client file recalled" */
TAPESERVER_NOTIFIED_CLIENT_FILE_RECALLED=83, /* "Notified client file recalled" */
TAPESERVER_NOTIFY_CLIENT_END_OF_SESSION=84, /* "Notifying client end of session" */
TAPESERVER_NOTIFIED_CLIENT_END_OF_SESSION=85, /* "Notified client end of session" */
TAPESERVER_GET_DUMP_PARAMETERS_FROM_CLIENT=86, /* "Getting dump parameters from client" */
TAPESERVER_GOT_DUMP_PARAMETERS_FROM_CLIENT=87, /* "Got dump parameters from client" */
TAPESERVER_NOTIFY_CLIENT_DUMP_MESSAGE=88, /* "Notifying client dump message" */
TAPESERVER_NOTIFIED_CLIENT_DUMP_MESSAGE=89, /* "Notified client dump message" */
TAPESERVER_FAILED_TO_NOTIFY_CLIENT_END_OF_SESSION=90, /* "Failed to notify client end of session" */
TAPESERVER_NOTIFY_CLIENT_END_OF_FAILED_SESSION=91, /* "Notifying client end of failed session" */
TAPESERVER_NOTIFIED_CLIENT_END_OF_FAILED_SESSION=92, /* "Notified client end of failed session" */
TAPESERVER_FAILED_TO_NOTIFY_CLIENT_END_OF_FAILED_SESSION=93, /* "Failed to notify client end of failed session" */
TAPESERVER_SELECT_INTR=94, /* "Select interrupted" */
TAPESERVER_ENTERING_BRIDGE_MODE=95, /* "Entering bridge mode" */
TAPESERVER_PING_CLIENT=96, /* "Pinging client" */
TAPESERVER_PINGED_CLIENT=97, /* "Pinged client" */
TAPESERVER_CLOSED_INITIAL_CALLBACK_CONNECTION=98, /* "Closed initial callback connection" */
TAPESERVER_RECEIVED_TAPE_INFO_FROM_VMGR=99, /* "Received tape information from VMGR" */
TAPESERVER_SENT_FILETOMIGRATEREQUEST=100, /* "Sent FileToMigrateRequest to client" */
TAPESERVER_RECEIVED_FILETOMIGRATE=101, /* "Received FileToMigrate from client" */
TAPESERVER_SENT_FILETORECALLREQUEST=102, /* "Sent FileToRecallRequest to client" */
TAPESERVER_RECEIVED_FILETORECALL=103, /* "Received FileToRecall from client" */
TAPESERVER_SENT_FILEMIGRATEDNOTIFICATION=104, /* "Sent FileMigratedNotification to client" */
TAPESERVER_SENT_FILERECALLEDNOTIFICATION=105, /* "Sent FileRecalledNotification to client" */
TAPESERVER_RECEIVED_NOMOREFILES=106, /* "Recieved NoMoreFiles from client" */
TAPESERVER_RECEIVED_ACK_OF_NOTIFICATION=107, /* "Recieved acknowledge of notification from client" */
TAPESERVER_RECEIVED_ENDNOTIFCATIONERRORREPORT=108, /* "Received EndNotificationErrorReport from client" */
TAPESERVER_RECEIVED_NOTIFCATIONACKNOWLEDGE=109, /* "Received NotificationAcknowledge from client" */
TAPESERVER_SEND_END_OF_SESSION_TO_RTCPD=110, /* "Sending end-of-session to RTCPD" */
TAPESERVER_FAILED_TO_PARSE_TPCONFIG=111, /* "Failed to parse TPCONFIG file" */
TAPESERVER_PARSED_TPCONFIG=112, /* "Parsed TPCONFIG" */
TAPESERVER_CLOSED_RTCPD_DISK_TAPE_CONNECTION_DUE_TO_PEER=113, /* "Closed rtcpd disk/tape IO control connection due to rtcpd closing its end" */
TAPESERVER_TOO_MANY_DRIVES_IN_TPCONFIG=114 /* "There are more drives in the TPCONFIG file than there were when tapeserverd was started.  Please restart tapeserverd." */
}; // enum TapeServerDlfMessages
} // namespace tapeserver
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_TAPESERVER_TAPESERVERDLFMESSAGECONSTANTS_HPP
