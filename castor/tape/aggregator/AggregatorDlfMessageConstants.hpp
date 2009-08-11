/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Tue Aug 11 19:03:32 CEST 2009
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
AGGREGATOR_VDQM_REQUEST_HANDLER_SOCKET_IS_NULL=4, /* "The VdqmRequestHandlerThread has been passed a NULL socket pointer" */
AGGREGATOR_VDQM_CONNECTION_WITH_INFO=5, /* "Received a connection from VDQM" */
AGGREGATOR_VDQM_CONNECTION_WITHOUT_INFO=6, /* "Received a connection from VDQM without peer information" */
AGGREGATOR_HANDLE_VDQM_REQUEST_EXCEPT=7, /* "Exception raised when handling a request from the VDQM" */
AGGREGATOR_FAILED_TO_READ_MESSAGE_HEADER=8, /* "Failed to read message header" */
AGGREGATOR_UNKNOWN_MAGIC=9, /* "Unknown magic number" */
AGGREGATOR_UNKNOWN_REQUEST_TYPE=10, /* "Unknown request type" */
AGGREGATOR_FAILED_TO_READ_MESSAGE_BODY=11, /* "Failed to read message body" */
AGGREGATOR_RECEIVE_RCP_JOB_RQST=12, /* "Receiving RCP job request" */
AGGREGATOR_RECEIVED_RCP_JOB_RQST=13, /* "Received RCP job request" */
AGGREGATOR_SUBMITTING_JOB_TO_RTCPD=14, /* "Submitting remote copy job to RTCPD" */
AGGREGATOR_FAILED_TO_SUBMIT_JOB_TO_RTCPD=15, /* "Failed to submit remote copy job to RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RTCP_ACKN=16, /* "Failed to marshall RTCP acknowledge message" */
AGGREGATOR_FAILED_TO_SEND_RCP_JOB_REPLY_TO_VDQM=17, /* "Failed to send RCP job reply to VDQM" */
AGGREGATOR_FAILED_TO_PROCESS_RCP_JOB_SUBMISSION=18, /* "Failed to process RCP job submission message" */
AGGREGATOR_RECEIVED_RTCPD_ERROR_MESSAGE=19, /* "Received error message from RTCPD" */
AGGREGATOR_RTCPD_HANDLER_SOCKET_IS_NULL=20, /* "The RtcpdHandlerThread has been passed a NULL socket pointer" */
AGGREGATOR_INITIAL_RTCPD_CALLBACK_WITH_INFO=21, /* "Received initial callback connection from RTCPD" */
AGGREGATOR_INITIAL_RTCPD_CALLBACK_WITHOUT_INFO=22, /* "Received initial callback connection from RTCPD without peer information" */
AGGREGATOR_RTCPD_CALLBACK_WITH_INFO=23, /* "Received a callback connection from RTCPD" */
AGGREGATOR_RTCPD_CALLBACK_WITHOUT_INFO=24, /* "Received a callback connection from RTCPD without peer information" */
AGGREGATOR_GOT_VOLREQID=25, /* "Got volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_GET_VOLREQID=26, /* "Failed to get volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RCP_JOB_REPLY_MESSAGE=27, /* "Failed to marshall RCP job reply message" */
AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS=28, /* "Signalled no more requests to RTCPD" */
AGGREGATOR_FAILED_TO_COORDINATE_REMOTE_COPY=29, /* "Failed to coordinate remote copy" */
AGGREGATOR_TRANSFER_FAILED=30, /* "Transfer failed" */
AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK=31, /* "Gave request for more work to RTCPD" */
AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION=32, /* "Data has arrived on the initial RTCPD connection" */
AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ=33, /* "Received RTCP_ENDOF_REQ" */
AGGREGATOR_TAPE_POSITIONED=34, /* "Tape positioned" */
AGGREGATOR_FILE_TRANSFERED=35, /* "File transfered" */
AGGREGATOR_GET_REQUEST_INFO_FROM_RTCPD=36, /* "Getting request information from RTCPD" */
AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD=37, /* "Got request information from RTCPD" */
AGGREGATOR_TOLD_CLIENT_TO_START_TRANSFER=38, /* "Told client to start the transfer protocol" */
AGGREGATOR_GET_VOLUME_FROM_CLIENT=39, /* "Getting volume from client" */
AGGREGATOR_GOT_VOLUME_FROM_CLIENT=40, /* "Got volume from client" */
AGGREGATOR_GOT_NO_MORE_FILES_FROM_CLIENT=41, /* "Got no more files from client" */
AGGREGATOR_GET_FILE_TO_MIGRATE_FROM_CLIENT=42, /* "Getting file to migrate from client" */
AGGREGATOR_GOT_FILE_TO_MIGRATE_FROM_CLIENT=43, /* "Got file to migrate from client" */
AGGREGATOR_GET_FILE_TO_RECALL_FROM_CLIENT=44, /* "Getting file to recall from client" */
AGGREGATOR_GOT_FILE_TO_RECALL_FROM_CLIENT=45, /* "Got file to recall from client" */
AGGREGATOR_GIVE_VOLUME_TO_RTCPD=46, /* "Giving volume to RTCPD" */
AGGREGATOR_GAVE_VOLUME_TO_RTCPD=47, /* "Gave volume to RTCPD" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK=48, /* "Asking RTCPD to request more recall work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK=49, /* "Asked RTCPD to request more recall work" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK=50, /* "Asking RTCPD to request more migrate work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK=51, /* "Asked RTCPD to request more migrate work" */
AGGREGATOR_TELL_RTCPD_DUMP_TAPE=52, /* "Telling RTCPD to dump tape" */
AGGREGATOR_TOLD_RTCPD_DUMP_TAPE=53, /* "Told RTCPD to dump tape" */
AGGREGATOR_TELL_RTCPD_END_OF_FILE_LIST=54, /* "Telling RTCPD end of file list" */
AGGREGATOR_TOLD_RTCPD_END_OF_FILE_LIST=55, /* "Told RTCPD end of file list" */
AGGREGATOR_TELL_RTCPD_TO_ABORT=56, /* "Telling RTCPD to abort" */
AGGREGATOR_TOLD_RTCPD_TO_ABORT=57, /* "Told RTCPD to abort" */
AGGREGATOR_GIVE_RECALL_FILE_TO_RTCPD=58, /* "Giving recall file to RTCPD" */
AGGREGATOR_GAVE_RECALL_FILE_TO_RTCPD=59, /* "Gave recall file to RTCPD" */
AGGREGATOR_GIVE_MIGRATE_FILE_TO_RTCPD=60, /* "Giving migrate file to RTCPD" */
AGGREGATOR_GAVE_MIGRATE_FILE_TO_RTCPD=61, /* "Gave migrate file to RTCPD" */
AGGREGATOR_RECEIVE_MSGBODY_FROM_RTCPD=62, /* "Receiving message body from RTCPD" */
AGGREGATOR_RECEIVED_MSGBODY_FROM_RTCPD=63, /* "Received message body from RTCPD" */
AGGREGATOR_RECEIVE_FILERQSTBODY_FROM_RTCPD=64, /* "Receiving FileRqstBody from RTCPD" */
AGGREGATOR_RECEIVED_FILERQSTBODY_FROM_RTCPD=65, /* "Received FileRqstBody from RTCPD" */
AGGREGATOR_RECEIVE_FILERQSTERRBODY_FROM_RTCPD=66, /* "Receiving FileRqstErrBody from RTCPD" */
AGGREGATOR_RECEIVED_FILERQSTERRBODY_FROM_RTCPD=67, /* "Received FileRqstErrBody from RTCPD" */
AGGREGATOR_PROCESSING_TAPE_DISK_RQST=68, /* "Processing a tape/disk IO request from RTCPD" */
AGGREGATOR_PING_RTCPD=69, /* "Pinging RTCPD" */
AGGREGATOR_PINGED_RTCPD=70, /* "Pinged RTCPD" */
AGGREGATOR_SEND_ACK_TO_RTCPD=71, /* "Sending acknowledge to RTCPD" */
AGGREGATOR_SENT_ACK_TO_RTCPD=72, /* "Sent acknowledge to RTCPD" */
AGGREGATOR_SEND_HEADER_TO_RTCPD=73, /* "Sending message header to RTCPD" */
AGGREGATOR_SENT_HEADER_TO_RTCPD=74, /* "Sent message header to RTCPD" */
AGGREGATOR_RECEIVE_TAPERQSTBODY=75, /* "Receiving tape request message body" */
AGGREGATOR_RECEIVED_TAPERQSTBODY=76, /* "Received tape request message body" */
AGGREGATOR_RECEIVE_GIVEOUTPBODY=77, /* "Receiving GIVE_OUTP message body" */
AGGREGATOR_RECEIVED_GIVEOUTPBODY=78, /* "Received GIVE_OUTP message body" */
AGGREGATOR_RECEIVE_TAPERQSTERRBODY=79, /* "Receiving tape request message body with error appendix" */
AGGREGATOR_RECEIVED_TAPERQSTERRBODY=80, /* "Received tape request message body with error appendix" */
AGGREGATOR_SEND_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=81, /* "Sending delayed acknowledge of request for more work to RTCPD" */
AGGREGATOR_SENT_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD=82, /* "Sent delayed acknowledge of request for more work to RTCPD" */
AGGREGATOR_CREATED_RTCPD_CALLBACK_PORT=83, /* "Created TCP/IP port for RTCPD callbacks" */
AGGREGATOR_CONNECTION_CLOSED_BY_RTCPD=84, /* "Connection closed by RTCPD" */
AGGREGATOR_CLOSED_CONNECTION=85, /* "Connection closed by aggregator" */
AGGREGATOR_NOTIFY_CLIENT_FILE_MIGRATED=86, /* "Notifying client file migrated" */
AGGREGATOR_NOTIFIED_CLIENT_FILE_MIGRATED=87, /* "Notified client file migrated" */
AGGREGATOR_NOTIFY_CLIENT_FILE_RECALLED=88, /* "Notifying client file recalled" */
AGGREGATOR_NOTIFIED_CLIENT_FILE_RECALLED=89, /* "Notified client file recalled" */
AGGREGATOR_NOTIFY_CLIENT_END_OF_SESSION=90, /* "Notifying client end of session" */
AGGREGATOR_NOTIFIED_CLIENT_END_OF_SESSION=91, /* "Notified client end of session" */
AGGREGATOR_GET_DUMP_PARAMETERS_FROM_CLIENT=92, /* "Getting dump parameters from client" */
AGGREGATOR_GOT_DUMP_PARAMETERS_FROM_CLIENT=93, /* "Got dump parameters from client" */
AGGREGATOR_NOTIFY_CLIENT_DUMP_MESSAGE=94, /* "Notifying client dump message" */
AGGREGATOR_NOTIFIED_CLIENT_DUMP_MESSAGE=95, /* "Notified client dump message" */
AGGREGATOR_FAILED_TO_NOTIFY_CLIENT_END_OF_SESSION=96, /* "Failed to notify gateway end of session" */
AGGREGATOR_NOTIFY_CLIENT_END_OF_FAILED_SESSION=97, /* "Notifying client end of failed session" */
AGGREGATOR_NOTIFIED_CLIENT_END_OF_FAILED_SESSION=98, /* "Notified client end of failed session" */
AGGREGATOR_FAILED_TO_NOTIFY_CLIENT_END_OF_FAILED_SESSION=99, /* "Failed to notify gateway end of failed session" */
AGGREGATOR_FAILED_TO_PROCESS_RTCPD_SOCKETS=100, /* "Failed to process RTCPD sockets" */
AGGREGATOR_SELECT_INTR=101, /* "Select interrupted" */
AGGREGATOR_ENTERING_BRIDGE_MODE=102, /* "Entering bridge mode" */
AGGREGATOR_PING_CLIENT=103, /* "Pinging client" */
AGGREGATOR_PINGED_CLIENT=104, /* "Pinged client" */
AGGREGATOR_CLOSED_INITIAL_CALLBACK_CONNECTION=105 /* "Closed initial callback connection" */
}; // enum AggregatorDlfMessages
} // namespace aggregator
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP
