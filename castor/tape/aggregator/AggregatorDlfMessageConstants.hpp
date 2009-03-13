/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Fri Mar 13 19:04:00 CET 2009
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
AGGREGATOR_MAIN_SELECT_FAILED=31, /* "The select of the main select loop encountered an error other than an interruption" */
AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK=32, /* "Gave request for more work to RTCPD" */
AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION=33, /* "Data has arrived on the initial RTCPD connection" */
AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ=34, /* "Received RTCP_ENDOF_REQ" */
AGGREGATOR_TAPE_POSITIONED_FILE_REQ=35, /* "Tape positioned" */
AGGREGATOR_TAPE_POSITIONED_TAPE_REQ=36, /* "Tape positioned (tape request)" */
AGGREGATOR_FILE_TRANSFERED=37, /* "File transfered" */
AGGREGATOR_GET_REQUEST_INFO_FROM_RTCPD=38, /* "Getting request information from RTCPD" */
AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD=39, /* "Got request information from RTCPD" */
AGGREGATOR_TOLD_GATEWAY_TO_START_TRANSFER=40, /* "Told tape gateway to start the transfer protocol" */
AGGREGATOR_GET_VOLUME_FROM_GATEWAY=41, /* "Getting volume from tape gateway" */
AGGREGATOR_GOT_VOLUME_FROM_GATEWAY=42, /* "Got volume from tape gateway" */
AGGREGATOR_GOT_NO_VOLUME_FROM_GATEWAY=43, /* "Asked for, but did not get a volume from the tape gateway" */
AGGREGATOR_GET_FIRST_FILE_TO_MIGRATE_FROM_GATEWAY=44, /* "Getting first file to migrate from tape gateway" */
AGGREGATOR_GOT_FIRST_FILE_TO_MIGRATE_FROM_GATEWAY=45, /* "Got first file to migrate from tape gateway" */
AGGREGATOR_GOT_NO_FIRST_FILE_TO_MIGRATE_FROM_GATEWAY=46, /* "Asked for, but did not get the first file to migrate from tape gateway" */
AGGREGATOR_GIVE_VOLUME_TO_RTCPD=47, /* "Giving volume to RTCPD" */
AGGREGATOR_GAVE_VOLUME_TO_RTCPD=48, /* "Gave volume to RTCPD" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK=49, /* "Asking RTCPD to request more recall work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK=50, /* "Asked RTCPD to request more recall work" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK=51, /* "Asking RTCPD to request more migrate work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK=52, /* "Asked RTCPD to request more migrate work" */
AGGREGATOR_TELL_RTCPD_END_OF_FILE_LIST=53, /* "Telling RTCPD end of file list" */
AGGREGATOR_TOLD_RTCPD_END_OF_FILE_LIST=54, /* "Told RTCPD end of file list" */
AGGREGATOR_GIVE_RECALL_FILE_TO_RTCPD=55, /* "Giving recall file to RTCPD" */
AGGREGATOR_GAVE_RECALL_FILE_TO_RTCPD=56, /* "Gave recall file to RTCPD" */
AGGREGATOR_GIVE_MIGRATE_FILE_TO_RTCPD=57, /* "Giving migrate file to RTCPD" */
AGGREGATOR_GAVE_MIGRATE_FILE_TO_RTCPD=58, /* "Gave migrate file to RTCPD" */
AGGREGATOR_NO_MORE_FILES_TO_MIGRATE=59, /* "No more files to migrate" */
AGGREGATOR_NO_MORE_FILES_TO_RECALL=60, /* "No more files to recall" */
AGGREGATOR_FILE_TO_MIGRATE=61, /* "File to migrate" */
AGGREGATOR_FILE_TO_RECALL=62, /* "File to recall" */
AGGREGATOR_PROCESSING_TAPE_DISK_RQST=63, /* "Processing a tape/disk IO request from RTCPD" */
AGGREGATOR_PING_RTCPD=64, /* "Pinging RTCPD" */
AGGREGATOR_PINGED_RTCPD=65, /* "Pinged RTCPD" */
AGGREGATOR_SEND_ACK_TO_RTCPD=66, /* "Sending acknowledge to RTCPD" */
AGGREGATOR_SENT_ACK_TO_RTCPD=67, /* "Sent acknowledge to RTCPD" */
AGGREGATOR_RECEIVE_TAPERQSTERRBODY=68, /* "Receiving tape request message body with error appendix" */
AGGREGATOR_RECEIVED_TAPERQSTERRBODY=68 /* "Received tape request message body witherror appendix" */
}; // enum AggregatorDlfMessages
} // namespace aggregator
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP
