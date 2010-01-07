/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Thu Jan  7 15:58:11 CET 2010
 */

/******************************************************************************
 *             castor/tape/aggregator/AggregatorDlfMessageConstants.hpp
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

#ifndef CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP
#define CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP 1


namespace castor {
namespace tape {
namespace aggregator {
enum AggregatorDlfMessages {
AGGREGATOR_NULL=0, /* " - " */
AGGREGATOR_STARTED=1, /* "aggregatord started" */
AGGREGATOR_FAILED_TO_PARSE_COMMAND_LINE=2, /* "Failed to parse the command line" */
AGGREGATOR_FAILED_TO_INIT_DB_SERVICE=3, /* "Failed to initialise database service" */
AGGREGATOR_VDQM_CONNECTION_WITH_INFO=4, /* "Received a connection from VDQM" */
AGGREGATOR_HANDLE_VDQM_REQUEST_EXCEPT=5, /* "Exception raised when handling a request from the VDQM" */
AGGREGATOR_FAILED_TO_READ_MESSAGE_HEADER=6, /* "Failed to read message header" */
AGGREGATOR_UNKNOWN_MAGIC=7, /* "Unknown magic number" */
AGGREGATOR_UNKNOWN_REQUEST_TYPE=8, /* "Unknown request type" */
AGGREGATOR_FAILED_TO_READ_MESSAGE_BODY=9, /* "Failed to read message body" */
AGGREGATOR_RECEIVE_RCP_JOB_RQST=10, /* "Receiving RCP job request" */
AGGREGATOR_RECEIVED_RCP_JOB_RQST=11, /* "Received RCP job request" */
AGGREGATOR_SUBMITTING_JOB_TO_RTCPD=12, /* "Submitting remote copy job to RTCPD" */
AGGREGATOR_FAILED_TO_SUBMIT_JOB_TO_RTCPD=13, /* "Failed to submit remote copy job to RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RTCP_ACKN=14, /* "Failed to marshall RTCP acknowledge message" */
AGGREGATOR_FAILED_TO_SEND_RCP_JOB_REPLY_TO_VDQM=15, /* "Failed to send RCP job reply to VDQM" */
AGGREGATOR_FAILED_TO_PROCESS_RCP_JOB_SUBMISSION=16, /* "Failed to process RCP job submission message" */
AGGREGATOR_RTCPD_HANDLER_SOCKET_IS_NULL=17, /* "The RtcpdHandlerThread has been passed a NULL socket pointer" */
AGGREGATOR_INITIAL_RTCPD_CALLBACK=18, /* "Received initial callback connection from RTCPD" */
AGGREGATOR_RTCPD_CALLBACK=19, /* "Received a callback connection from RTCPD" */
AGGREGATOR_GOT_VOLREQID=20, /* "Got volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_GET_VOLREQID=21, /* "Failed to get volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RCP_JOB_REPLY_MESSAGE=22, /* "Failed to marshall RCP job reply message" */
AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS=23, /* "Signalled no more requests to RTCPD" */
AGGREGATOR_FAILED_TO_COORDINATE_REMOTE_COPY=24, /* "Failed to coordinate remote copy" */
AGGREGATOR_TRANSFER_FAILED=25, /* "Transfer failed" */
AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK=26, /* "Gave request for more work to RTCPD" */
AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION=27, /* "Data has arrived on the initial RTCPD connection" */
AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ=28, /* "Received RTCP_ENDOF_REQ" */
AGGREGATOR_TAPE_POSITIONED=29, /* "Tape positioned" */
AGGREGATOR_FILE_TRANSFERED=30, /* "File transfered" */
AGGREGATOR_GET_REQUEST_INFO_FROM_RTCPD=31, /* "Getting request information from RTCPD" */
AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD=32, /* "Got request information from RTCPD" */
AGGREGATOR_TOLD_CLIENT_TO_START_TRANSFER=33, /* "Told client to start the transfer protocol" */
AGGREGATOR_GET_VOLUME_FROM_CLIENT=34, /* "Getting volume from client" */
AGGREGATOR_GOT_VOLUME_FROM_CLIENT=35, /* "Got volume from client" */
AGGREGATOR_GOT_NO_MORE_FILES_FROM_CLIENT=36, /* "Got no more files from client" */
AGGREGATOR_GET_FILE_TO_MIGRATE_FROM_CLIENT=37, /* "Getting file to migrate from client" */
AGGREGATOR_GOT_FILE_TO_MIGRATE_FROM_CLIENT=38, /* "Got file to migrate from client" */
AGGREGATOR_GET_FILE_TO_RECALL_FROM_CLIENT=39, /* "Getting file to recall from client" */
AGGREGATOR_GOT_FILE_TO_RECALL_FROM_CLIENT=40, /* "Got file to recall from client" */
AGGREGATOR_GIVE_VOLUME_TO_RTCPD=41, /* "Giving volume to RTCPD" */
AGGREGATOR_GAVE_VOLUME_TO_RTCPD=42, /* "Gave volume to RTCPD" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK=43, /* "Asking RTCPD to request more recall work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK=44, /* "Asked RTCPD to request more recall work" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK=45, /* "Asking RTCPD to request more migrate work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK=46, /* "Asked RTCPD to request more migrate work" */
AGGREGATOR_TELL_RTCPD_DUMP_TAPE=47, /* "Telling RTCPD to dump tape" */
AGGREGATOR_TOLD_RTCPD_DUMP_TAPE=48, /* "Told RTCPD to dump tape" */
AGGREGATOR_TELL_RTCPD_END_OF_FILE_LIST=49, /* "Telling RTCPD end of file list" */
AGGREGATOR_TOLD_RTCPD_END_OF_FILE_LIST=50, /* "Told RTCPD end of file list" */
AGGREGATOR_TELL_RTCPD_TO_ABORT=51, /* "Telling RTCPD to abort" */
AGGREGATOR_TOLD_RTCPD_TO_ABORT=52, /* "Told RTCPD to abort" */
AGGREGATOR_GIVE_RECALL_FILE_TO_RTCPD=53, /* "Giving recall file to RTCPD" */
AGGREGATOR_GAVE_RECALL_FILE_TO_RTCPD=54, /* "Gave recall file to RTCPD" */
AGGREGATOR_GIVE_MIGRATE_FILE_TO_RTCPD=55, /* "Giving migrate file to RTCPD" */
AGGREGATOR_GAVE_MIGRATE_FILE_TO_RTCPD=56, /* "Gave migrate file to RTCPD" */
AGGREGATOR_RECEIVE_MSGBODY_FROM_RTCPD=57, /* "Receiving message body from RTCPD" */
AGGREGATOR_RECEIVED_MSGBODY_FROM_RTCPD=58, /* "Received message body from RTCPD" */
AGGREGATOR_RECEIVE_FILERQSTBODY_FROM_RTCPD=59, /* "Receiving FileRqstBody from RTCPD" */
AGGREGATOR_RECEIVED_FILERQSTBODY_FROM_RTCPD=60, /* "Received FileRqstBody from RTCPD" */
AGGREGATOR_RECEIVE_FILERQSTERRBODY_FROM_RTCPD=61, /* "Receiving FileRqstErrBody from RTCPD" */
AGGREGATOR_RECEIVED_FILERQSTERRBODY_FROM_RTCPD=62, /* "Received FileRqstErrBody from RTCPD" */
AGGREGATOR_PROCESSING_TAPE_DISK_RQST=63, /* "Processing a tape/disk IO request from RTCPD" */
AGGREGATOR_PING_RTCPD=64, /* "Pinging RTCPD" */
AGGREGATOR_PINGED_RTCPD=65, /* "Pinged RTCPD" */
AGGREGATOR_SEND_ACK_TO_RTCPD=66, /* "Sending acknowledge to RTCPD" */
AGGREGATOR_SENT_ACK_TO_RTCPD=67, /* "Sent acknowledge to RTCPD" */
AGGREGATOR_SEND_HEADER_TO_RTCPD=68, /* "Sending message header to RTCPD" */
AGGREGATOR_SENT_HEADER_TO_RTCPD=69, /* "Sent message header to RTCPD" */
AGGREGATOR_RECEIVE_TAPERQSTBODY=70, /* "Receiving tape request message body" */
AGGREGATOR_RECEIVED_TAPERQSTBODY=71, /* "Received tape request message body" */
AGGREGATOR_RECEIVE_GIVEOUTPBODY=72, /* "Receiving GIVE_OUTP message body" */
AGGREGATOR_RECEIVED_GIVEOUTPBODY=73, /* "Received GIVE_OUTP message body" */
AGGREGATOR_RECEIVE_TAPERQSTERRBODY=74, /* "Receiving tape request message body with error appendix" */
AGGREGATOR_RECEIVED_TAPERQSTERRBODY=75, /* "Received tape request message body with error appendix" */
AGGREGATOR_SEND_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=76, /* "Sending delayed acknowledge of request for more work to RTCPD" */
AGGREGATOR_SENT_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=77, /* "Sent delayed acknowledge of request for more work to RTCPD" */
AGGREGATOR_CREATED_RTCPD_CALLBACK_PORT=78, /* "Created TCP/IP port for RTCPD callbacks" */
AGGREGATOR_CONNECTION_CLOSED_BY_RTCPD=79, /* "Connection closed by RTCPD" */
AGGREGATOR_CLOSED_CONNECTION=80, /* "Connection closed by aggregator" */
AGGREGATOR_NOTIFY_CLIENT_FILE_MIGRATED=81, /* "Notifying client file migrated" */
AGGREGATOR_NOTIFIED_CLIENT_FILE_MIGRATED=82, /* "Notified client file migrated" */
AGGREGATOR_NOTIFY_CLIENT_FILE_RECALLED=83, /* "Notifying client file recalled" */
AGGREGATOR_NOTIFIED_CLIENT_FILE_RECALLED=84, /* "Notified client file recalled" */
AGGREGATOR_NOTIFY_CLIENT_END_OF_SESSION=85, /* "Notifying client end of session" */
AGGREGATOR_NOTIFIED_CLIENT_END_OF_SESSION=86, /* "Notified client end of session" */
AGGREGATOR_GET_DUMP_PARAMETERS_FROM_CLIENT=87, /* "Getting dump parameters from client" */
AGGREGATOR_GOT_DUMP_PARAMETERS_FROM_CLIENT=88, /* "Got dump parameters from client" */
AGGREGATOR_NOTIFY_CLIENT_DUMP_MESSAGE=89, /* "Notifying client dump message" */
AGGREGATOR_NOTIFIED_CLIENT_DUMP_MESSAGE=90, /* "Notified client dump message" */
AGGREGATOR_FAILED_TO_NOTIFY_CLIENT_END_OF_SESSION=91, /* "Failed to notify gateway end of session" */
AGGREGATOR_NOTIFY_CLIENT_END_OF_FAILED_SESSION=92, /* "Notifying client end of failed session" */
AGGREGATOR_NOTIFIED_CLIENT_END_OF_FAILED_SESSION=93, /* "Notified client end of failed session" */
AGGREGATOR_FAILED_TO_NOTIFY_CLIENT_END_OF_FAILED_SESSION=94, /* "Failed to notify gateway end of failed session" */
AGGREGATOR_SELECT_INTR=95, /* "Select interrupted" */
AGGREGATOR_ENTERING_BRIDGE_MODE=96, /* "Entering bridge mode" */
AGGREGATOR_PING_CLIENT=97, /* "Pinging client" */
AGGREGATOR_PINGED_CLIENT=98, /* "Pinged client" */
AGGREGATOR_CLOSED_INITIAL_CALLBACK_CONNECTION=99, /* "Closed initial callback connection" */
AGGREGATOR_RECEIVED_TAPE_INFO_FROM_VMGR=100, /* "Received tape information from VMGR" */
AGGREGATOR_SENT_FILETOMIGRATEREQUEST=101, /* "Sent FileToMigrateRequest to client" */
AGGREGATOR_RECEIVED_FILETOMIGRATE=102, /* "Received FileToMigrate from client" */
AGGREGATOR_SENT_FILETORECALLREQUEST=103, /* "Sent FileToRecallRequest to client" */
AGGREGATOR_RECEIVED_FILETORECALL=104, /* "Received FileToRecall from client" */
AGGREGATOR_SENT_FILEMIGRATEDNOTIFICATION=105, /* "Sent FileMigratedNotification to client" */
AGGREGATOR_SENT_FILERECALLEDNOTIFICATION=106, /* "Sent FileRecalledNotification to client" */
AGGREGATOR_RECEIVED_NOMOREFILES=107, /* "Recieved NoMoreFiles from client" */
AGGREGATOR_RECEIVED_ACK_OF_NOTIFICATION=108, /* "Recieved acknowledge of notification from client" */
AGGREGATOR_RECEIVED_ENDNOTIFCATIONERRORREPORT=109, /* "Received EndNotificationErrorReport from client" */
AGGREGATOR_RECEIVED_NOTIFCATIONACKNOWLEDGE=110, /* "Received NotificationAcknowledge from client" */
AGGREGATOR_SEND_END_OF_SESSION_TO_RTCPD=111, /* "Sending end-of-session to RTCPD" */
AGGREGATOR_FAILED_TO_PARSE_TPCONFIG=112, /* "Failed to parse TPCONFIG file" */
AGGREGATOR_PARSED_TPCONFIG=113 /* "Parsed TPCONFIG" */
}; // enum AggregatorDlfMessages
} // namespace aggregator
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP
