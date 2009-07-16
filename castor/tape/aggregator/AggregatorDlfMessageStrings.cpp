/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Thu Jul 16 13:36:42 CEST 2009
 */

/******************************************************************************
 *           castor/tape/aggregator/AggregatorDlfMessageStrings.cpp
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

#include "castor/tape/aggregator/AggregatorDlfMessageConstants.hpp"
#include "castor/tape/aggregator/AggregatorDaemon.hpp"

castor::dlf::Message castor::tape::aggregator::AggregatorDaemon::s_dlfMessages[] = {
{AGGREGATOR_NULL, " - "},
{AGGREGATOR_STARTED, "aggregatord started"},
{AGGREGATOR_FAILED_TO_PARSE_COMMAND_LINE, "Failed to parse the command line"},
{AGGREGATOR_FAILED_TO_INIT_DB_SERVICE, "Failed to initialise database service"},
{AGGREGATOR_VDQM_REQUEST_HANDLER_SOCKET_IS_NULL, "The VdqmRequestHandlerThread has been passed a NULL socket pointer"},
{AGGREGATOR_VDQM_CONNECTION_WITH_INFO, "Received a connection from VDQM"},
{AGGREGATOR_VDQM_CONNECTION_WITHOUT_INFO, "Received a connection from VDQM without peer information"},
{AGGREGATOR_HANDLE_VDQM_REQUEST_EXCEPT, "Exception raised when handling a request from the VDQM"},
{AGGREGATOR_FAILED_TO_READ_MESSAGE_HEADER, "Failed to read message header"},
{AGGREGATOR_UNKNOWN_MAGIC, "Unknown magic number"},
{AGGREGATOR_UNKNOWN_REQUEST_TYPE, "Unknown request type"},
{AGGREGATOR_FAILED_TO_READ_MESSAGE_BODY, "Failed to read message body"},
{AGGREGATOR_RECEIVE_RCP_JOB_RQST, "Receiving RCP job request"},
{AGGREGATOR_RECEIVED_RCP_JOB_RQST, "Received RCP job request"},
{AGGREGATOR_SUBMITTING_JOB_TO_RTCPD, "Submitting remote copy job to RTCPD"},
{AGGREGATOR_FAILED_TO_SUBMIT_JOB_TO_RTCPD, "Failed to submit remote copy job to RTCPD"},
{AGGREGATOR_FAILED_TO_MARSHALL_RTCP_ACKN, "Failed to marshall RTCP acknowledge message"},
{AGGREGATOR_FAILED_TO_SEND_RCP_JOB_REPLY_TO_VDQM, "Failed to send RCP job reply to VDQM"},
{AGGREGATOR_FAILED_TO_PROCESS_RCP_JOB_SUBMISSION, "Failed to process RCP job submission message"},
{AGGREGATOR_RECEIVED_RTCPD_ERROR_MESSAGE, "Received error message from RTCPD"},
{AGGREGATOR_RTCPD_HANDLER_SOCKET_IS_NULL, "The RtcpdHandlerThread has been passed a NULL socket pointer"},
{AGGREGATOR_INITIAL_RTCPD_CALLBACK_WITH_INFO, "Received initial callback connection from RTCPD"},
{AGGREGATOR_INITIAL_RTCPD_CALLBACK_WITHOUT_INFO, "Received initial callback connection from RTCPD without peer information"},
{AGGREGATOR_RTCPD_CALLBACK_WITH_INFO, "Received a callback connection from RTCPD"},
{AGGREGATOR_RTCPD_CALLBACK_WITHOUT_INFO, "Received a callback connection from RTCPD without peer information"},
{AGGREGATOR_GOT_VOLREQID, "Got volume request ID from RTCPD"},
{AGGREGATOR_FAILED_TO_GET_VOLREQID, "Failed to get volume request ID from RTCPD"},
{AGGREGATOR_FAILED_TO_MARSHALL_RCP_JOB_REPLY_MESSAGE, "Failed to marshall RCP job reply message"},
{AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS, "Signalled no more requests to RTCPD"},
{AGGREGATOR_FAILED_TO_COORDINATE_REMOTE_COPY, "Failed to coordinate remote copy"},
{AGGREGATOR_TRANSFER_FAILED, "Transfer failed"},
{AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK, "Gave request for more work to RTCPD"},
{AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION, "Data has arrived on the initial RTCPD connection"},
{AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ, "Received RTCP_ENDOF_REQ"},
{AGGREGATOR_TAPE_POSITIONED, "Tape positioned"},
{AGGREGATOR_FILE_TRANSFERED, "File transfered"},
{AGGREGATOR_GET_REQUEST_INFO_FROM_RTCPD, "Getting request information from RTCPD"},
{AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD, "Got request information from RTCPD"},
{AGGREGATOR_TOLD_GATEWAY_TO_START_TRANSFER, "Told tape gateway to start the transfer protocol"},
{AGGREGATOR_GET_VOLUME_FROM_GATEWAY, "Getting volume from tape gateway"},
{AGGREGATOR_GOT_VOLUME_FROM_GATEWAY, "Got volume from tape gateway"},
{AGGREGATOR_GOT_NO_VOLUME_FROM_GATEWAY, "Got no volume from tape gateway"},
{AGGREGATOR_GET_FILE_TO_MIGRATE_FROM_GATEWAY, "Getting file to migrate from tape gateway"},
{AGGREGATOR_GOT_FILE_TO_MIGRATE_FROM_GATEWAY, "Got file to migrate from tape gateway"},
{AGGREGATOR_NO_MORE_FILES_TO_MIGRATE, "No more files to migrate"},
{AGGREGATOR_GET_FILE_TO_RECALL_FROM_GATEWAY, "Getting file to recall from tape gateway"},
{AGGREGATOR_GOT_FILE_TO_RECALL_FROM_GATEWAY, "Got file to recall from tape gateway"},
{AGGREGATOR_NO_MORE_FILES_TO_RECALL, "No more files to recall"},
{AGGREGATOR_GIVE_VOLUME_TO_RTCPD, "Giving volume to RTCPD"},
{AGGREGATOR_GAVE_VOLUME_TO_RTCPD, "Gave volume to RTCPD"},
{AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK, "Asking RTCPD to request more recall work"},
{AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK, "Asked RTCPD to request more recall work"},
{AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK, "Asking RTCPD to request more migrate work"},
{AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK, "Asked RTCPD to request more migrate work"},
{AGGREGATOR_TELL_RTCPD_END_OF_FILE_LIST, "Telling RTCPD end of file list"},
{AGGREGATOR_TOLD_RTCPD_END_OF_FILE_LIST, "Told RTCPD end of file list"},
{AGGREGATOR_TELL_RTCPD_TO_ABORT, "Telling RTCPD to abort"},
{AGGREGATOR_TOLD_RTCPD_TO_ABORT, "Told RTCPD to abort"},
{AGGREGATOR_GIVE_RECALL_FILE_TO_RTCPD, "Giving recall file to RTCPD"},
{AGGREGATOR_GAVE_RECALL_FILE_TO_RTCPD, "Gave recall file to RTCPD"},
{AGGREGATOR_GIVE_MIGRATE_FILE_TO_RTCPD, "Giving migrate file to RTCPD"},
{AGGREGATOR_GAVE_MIGRATE_FILE_TO_RTCPD, "Gave migrate file to RTCPD"},
{AGGREGATOR_RECEIVE_FILERQSTBODY_FROM_RTCPD, "Receiving FileRqstBody from RTCPD"},
{AGGREGATOR_RECEIVED_FILERQSTBODY_FROM_RTCPD, "Received FileRqstBody from RTCPD"},
{AGGREGATOR_RECEIVE_FILERQSTERRBODY_FROM_RTCPD, "Receiving FileRqstErrBody from RTCPD"},
{AGGREGATOR_RECEIVED_FILERQSTERRBODY_FROM_RTCPD, "Received FileRqstErrBody from RTCPD"},
{AGGREGATOR_PROCESSING_TAPE_DISK_RQST, "Processing a tape/disk IO request from RTCPD"},
{AGGREGATOR_PING_RTCPD, "Pinging RTCPD"},
{AGGREGATOR_PINGED_RTCPD, "Pinged RTCPD"},
{AGGREGATOR_SEND_ACK_TO_RTCPD, "Sending acknowledge to RTCPD"},
{AGGREGATOR_SENT_ACK_TO_RTCPD, "Sent acknowledge to RTCPD"},
{AGGREGATOR_RECEIVE_TAPERQSTBODY, "Receiving tape request message body"},
{AGGREGATOR_RECEIVED_TAPERQSTBODY, "Received tape request message body"},
{AGGREGATOR_RECEIVE_GIVEOUTPBODY, "Receiving GIVE_OUTP message body"},
{AGGREGATOR_RECEIVED_GIVEOUTPBODY, "Received GIVE_OUTP message body"},
{AGGREGATOR_RECEIVE_TAPERQSTERRBODY, "Receiving tape request message body with error appendix"},
{AGGREGATOR_RECEIVED_TAPERQSTERRBODY, "Received tape request message body with error appendix"},
{AGGREGATOR_SEND_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD, "Sending delayed acknowledge of request for more work to RTCPD"},
{AGGREGATOR_SENT_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD, "Sent delayed acknowledge of request for more work to RTCPD"},
{AGGREGATOR_CREATED_RTCPD_CALLBACK_PORT, "Created TCP/IP port for RTCPD callbacks"},
{AGGREGATOR_CONNECTION_CLOSED_BY_RTCPD, "Connection closed by RTCPD"},
{AGGREGATOR_CLOSED_CONNECTION, "Connection closed by aggregator"},
{AGGREGATOR_NOTIFY_GATEWAY_FILE_MIGRATED, "Notifying tape gateway file migrated"},
{AGGREGATOR_NOTIFIED_GATEWAY_FILE_MIGRATED, "Notified tape gateway file migrated"},
{AGGREGATOR_NOTIFY_GATEWAY_FILE_RECALLED, "Notifying tape gateway file recalled"},
{AGGREGATOR_NOTIFIED_GATEWAY_FILE_RECALLED, "Notified tape gateway file recalled"},
{AGGREGATOR_NOTIFY_GATEWAY_END_OF_SESSION, "Notifying tape gateway end of session"},
{AGGREGATOR_NOTIFIED_GATEWAY_END_OF_SESSION, "Notified tape gateway end of session"},
{AGGREGATOR_NOTIFY_GATEWAY_DUMP_MESSAGE, "Notifying tape gateway dump message"},
{AGGREGATOR_NOTIFIED_GATEWAY_DUMP_MESSAGE, "Notified tape gateway dump message"},
{AGGREGATOR_FAILED_TO_NOTIFY_GATEWAY_END_OF_SESSION, "Failed to notify gateway end of session"},
{AGGREGATOR_NOTIFY_GATEWAY_END_OF_FAILED_SESSION, "Notifying tape gateway end of failed session"},
{AGGREGATOR_NOTIFIED_GATEWAY_END_OF_FAILED_SESSION, "Notified tape gateway end of failed session"},
{AGGREGATOR_FAILED_TO_NOTIFY_GATEWAY_END_OF_FAILED_SESSION, "Failed to notify gateway end of failed session"},
{AGGREGATOR_FAILED_TO_PROCESS_RTCPD_SOCKETS, "Failed to process RTCPD sockets"},
{-1, ""}};
