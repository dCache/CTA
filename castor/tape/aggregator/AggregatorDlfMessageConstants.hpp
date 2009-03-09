/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Fri Mar  6 17:04:50 CET 2009
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
AGGREGATOR_HANDLE_JOB_MESSAGE=12, /* "Handle job submission message VDQM_CLIENTINFO:RTCOPY_MAGIC_OLD0" */
AGGREGATOR_SUBMITTING_JOB_TO_RTCPD=13, /* "Submitting remote copy job to RTCPD" */
AGGREGATOR_FAILED_TO_SUBMIT_JOB_TO_RTCPD=14, /* "Failed to submit remote copy job to RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RTCP_ACKN=15, /* "Failed to marshall RTCP acknowledge message" */
AGGREGATOR_FAILED_TO_SEND_RCP_JOB_REPLY_TO_VDQM=16, /* "Failed to send RCP job reply to VDQM" */
AGGREGATOR_FAILED_TO_PROCESS_RCP_JOB_SUBMISSION=17, /* "Failed to process RCP job submission message" */
AGGREGATOR_RECEIVED_RTCPD_ERROR_MESSAGE=18, /* "Received error message from RTCPD" */
AGGREGATOR_RTCPD_HANDLER_SOCKET_IS_NULL=19, /* "The RtcpdHandlerThread has been passed a NULL socket pointer" */
AGGREGATOR_INITIAL_RTCPD_CALLBACK_WITH_INFO=20, /* "Received initial callback connection from RTCPD" */
AGGREGATOR_INITIAL_RTCPD_CALLBACK_WITHOUT_INFO=21, /* "Received initial callback connection from RTCPD without peer information" */
AGGREGATOR_RTCPD_CALLBACK_WITH_INFO=22, /* "Received a callback connection from RTCPD" */
AGGREGATOR_RTCPD_CALLBACK_WITHOUT_INFO=23, /* "Received a callback connection from RTCPD without peer information" */
AGGREGATOR_GOT_VOLREQID=24, /* "Got volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_GET_VOLREQID=25, /* "Failed to get volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RCP_JOB_REPLY_MESSAGE=26, /* "Failed to marshall RCP job reply message" */
AGGREGATOR_GAVE_VOLUME_INFO_TO_RTCPD=27, /* "Gave volume information to RTCPD" */
AGGREGATOR_GAVE_FILE_INFO_TO_RTCPD=28, /* "Gave file information to RTCPD" */
AGGREGATOR_FAILED_TO_GIVE_FILE_INFO_TO_RTCPD=29, /* "Failed to give file information to RTCPD" */
AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS=30, /* "Signalled no more requests to RTCPD" */
AGGREGATOR_FAILED_TO_COORDINATE_REMOTE_COPY=31, /* "Failed to coordinate remote copy" */
AGGREGATOR_TRANSFER_FAILED=32, /* "Transfer failed" */
AGGREGATOR_PINGED_RTCPD=33, /* "Pinged RTCPD" */
AGGREGATOR_MAIN_SELECT_FAILED=34, /* "The select of the main select loop encountered an error other than an interruption" */
AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK=35, /* "Gave request for more work to RTCPD" */
AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION=36, /* "Data has arrived on the initial RTCPD connection" */
AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ=37, /* "Received RTCP_ENDOF_REQ" */
AGGREGATOR_TAPE_POSITIONED_FILE_REQ=38, /* "Tape positioned" */
AGGREGATOR_TAPE_POSITIONED_TAPE_REQ=39, /* "Tape positioned (tape request)" */
AGGREGATOR_FILE_TRANSFERED=40, /* "File transfered" */
AGGREGATOR_GET_REQUEST_INFO_FROM_RTCPD=41, /* "Getting request information from RTCPD" */
AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD=42, /* "Got request information from RTCPD" */
AGGREGATOR_TOLD_GATEWAY_TO_START_TRANSFER=43, /* "Told tape gateway to start the transfer protocol" */
AGGREGATOR_GET_VOLUME_FROM_GATEWAY=44, /* "Getting volume from tape gateway" */
AGGREGATOR_GOT_VOLUME_FROM_GATEWAY=45, /* "Got volume from tape gateway" */
AGGREGATOR_GOT_NO_VOLUME_FROM_GATEWAY=46, /* "Asked for, but did not get a volume from the tape gateway" */
AGGREGATOR_GET_FIRST_FILE_TO_MIGRATE_FROM_GATEWAY=47, /* "Getting first file to migrate from tape gateway" */
AGGREGATOR_GOT_FIRST_FILE_TO_MIGRATE_FROM_GATEWAY=48, /* "Got first file to migrate from tape gateway" */
AGGREGATOR_GOT_NO_FIRST_FILE_TO_MIGRATE_FROM_GATEWAY=49, /* "Asked for, but did not get the first file to migrate from tape gateway" */
AGGREGATOR_GIVE_VOLUME_TO_RTCPD=50, /* "Giving volume to RTCPD" */
AGGREGATOR_GAVE_VOLUME_TO_RTCPD=51, /* "Gave volume to RTCPD" */
AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_WORK=52, /* "Asking RTCPD to request more work" */
AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_WORK=53, /* "Asked RTCPD to request more work" */
AGGREGATOR_TELL_RTCPD_END_OF_FILE_LIST=54, /* "Telling RTCPD end of file list" */
AGGREGATOR_TOLD_RTCPD_END_OF_FILE_LIST=55, /* "Told RTCPD end of file list" */
AGGREGATOR_GIVE_FILE_TO_RTCPD=56, /* "Giving file to RTCPD" */
AGGREGATOR_GAVE_FILE_TO_RTCPD=57, /* "Gave file to RTCPD" */
AGGREGATOR_NO_MORE_FILES_TO_MIGRATE=58, /* "No more files to migrate" */
AGGREGATOR_NO_MORE_FILES_TO_RECALL=59, /* "No more files to recall" */
AGGREGATOR_FILE_TO_MIGRATE=60, /* "File to migrate" */
AGGREGATOR_FILE_TO_RECALL=61 /* "File to recall" */
}; // enum AggregatorDlfMessages
} // namespace aggregator
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP
