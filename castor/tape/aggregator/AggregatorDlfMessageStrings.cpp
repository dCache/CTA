/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Tue Jan 19 12:06:10 CET 2010
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
{AGGREGATOR_FAILED_TO_START, "aggregatord failed to start"},
{AGGREGATOR_RECEIVED_VDQM_CONNECTION, "Received a connection from VDQM"},
{AGGREGATOR_HANDLE_VDQM_REQUEST_EXCEPT, "Exception raised when handling a request from the VDQM"},
{AGGREGATOR_FAILED_TO_READ_MESSAGE_HEADER, "Failed to read message header"},
{AGGREGATOR_UNKNOWN_MAGIC, "Unknown magic number"},
{AGGREGATOR_UNKNOWN_REQUEST_TYPE, "Unknown request type"},
{AGGREGATOR_FAILED_TO_READ_MESSAGE_BODY, "Failed to read message body"},
{AGGREGATOR_RECEIVE_RTCOPY_JOB, "Receiving remote-copy job"},
{AGGREGATOR_RECEIVED_RTCOPY_JOB, "Received remote-copy job"},
{AGGREGATOR_SUBMITTING_RTCOPY_JOB_TO_RTCPD, "Submitting remote-copy job to RTCPD"},
{AGGREGATOR_FAILED_TO_SUBMIT_JOB_TO_RTCPD, "Failed to submit remote-copy job to RTCPD"},
{AGGREGATOR_FAILED_TO_MARSHALL_RTCP_ACKN, "Failed to marshall RTCP acknowledge message"},
{AGGREGATOR_FAILED_TO_SEND_RTCOPY_JOB_REPLY_TO_VDQM, "Failed to send remote-copy job reply to VDQM"},
{AGGREGATOR_FAILED_TO_PROCESS_RCOPY_JOB_SUBMISSION, "Failed to process remote-copy job submission message"},
{AGGREGATOR_RTCPD_HANDLER_SOCKET_IS_NULL, "The RtcpdHandlerThread has been passed a NULL socket pointer"},
{AGGREGATOR_INITIAL_RTCPD_CALLBACK, "Received initial callback connection from RTCPD"},
{AGGREGATOR_RTCPD_CALLBACK, "Received a callback connection from RTCPD"},
{AGGREGATOR_GOT_VOLREQID, "Got volume request ID from RTCPD"},
{AGGREGATOR_FAILED_TO_GET_VOLREQID, "Failed to get volume request ID from RTCPD"},
{AGGREGATOR_FAILED_TO_MARSHALL_RTCOPY_JOB_REPLY_MESSAGE, "Failed to marshall remote-copy job reply message"},
{AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS, "Signalled no more requests to RTCPD"},
{AGGREGATOR_FAILED_TO_COORDINATE_REMOTE_COPY, "Failed to coordinate remote-copy"},
{AGGREGATOR_TRANSFER_FAILED, "Transfer failed"},
{AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK, "Gave request for more work to RTCPD"},
{AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION, "Data has arrived on the initial RTCPD connection"},
{AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ, "Received RTCP_ENDOF_REQ"},
{AGGREGATOR_TAPE_POSITIONED, "Tape positioned"},
{AGGREGATOR_FILE_TRANSFERED, "File transfered"},
{AGGREGATOR_GET_REQUEST_INFO_FROM_RTCPD, "Getting request information from RTCPD"},
{AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD, "Got request information from RTCPD"},
{AGGREGATOR_TOLD_CLIENT_TO_START_TRANSFER, "Told client to start the transfer protocol"},
{AGGREGATOR_GET_VOLUME_FROM_CLIENT, "Getting volume from client"},
{AGGREGATOR_GOT_VOLUME_FROM_CLIENT, "Got volume from client"},
{AGGREGATOR_GOT_NO_MORE_FILES_FROM_CLIENT, "Got no more files from client"},
{AGGREGATOR_GET_FILE_TO_MIGRATE_FROM_CLIENT, "Getting file to migrate from client"},
{AGGREGATOR_GOT_FILE_TO_MIGRATE_FROM_CLIENT, "Got file to migrate from client"},
{AGGREGATOR_GET_FILE_TO_RECALL_FROM_CLIENT, "Getting file to recall from client"},
{AGGREGATOR_GOT_FILE_TO_RECALL_FROM_CLIENT, "Got file to recall from client"},
{AGGREGATOR_GIVE_VOLUME_TO_RTCPD, "Giving volume to RTCPD"},
{AGGREGATOR_GAVE_VOLUME_TO_RTCPD, "Gave volume to RTCPD"},
{AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK, "Asking RTCPD to request more recall work"},
{AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK, "Asked RTCPD to request more recall work"},
{AGGREGATOR_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK, "Asking RTCPD to request more migrate work"},
{AGGREGATOR_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK, "Asked RTCPD to request more migrate work"},
{AGGREGATOR_TELL_RTCPD_DUMP_TAPE, "Telling RTCPD to dump tape"},
{AGGREGATOR_TOLD_RTCPD_DUMP_TAPE, "Told RTCPD to dump tape"},
{AGGREGATOR_TELL_RTCPD_END_OF_FILE_LIST, "Telling RTCPD end of file list"},
{AGGREGATOR_TOLD_RTCPD_END_OF_FILE_LIST, "Told RTCPD end of file list"},
{AGGREGATOR_TELL_RTCPD_TO_ABORT, "Telling RTCPD to abort"},
{AGGREGATOR_TOLD_RTCPD_TO_ABORT, "Told RTCPD to abort"},
{AGGREGATOR_GIVE_RECALL_FILE_TO_RTCPD, "Giving recall file to RTCPD"},
{AGGREGATOR_GAVE_RECALL_FILE_TO_RTCPD, "Gave recall file to RTCPD"},
{AGGREGATOR_GIVE_MIGRATE_FILE_TO_RTCPD, "Giving migrate file to RTCPD"},
{AGGREGATOR_GAVE_MIGRATE_FILE_TO_RTCPD, "Gave migrate file to RTCPD"},
{AGGREGATOR_RECEIVE_MSGBODY_FROM_RTCPD, "Receiving message body from RTCPD"},
{AGGREGATOR_RECEIVED_MSGBODY_FROM_RTCPD, "Received message body from RTCPD"},
{AGGREGATOR_RECEIVE_FILERQSTBODY_FROM_RTCPD, "Receiving FileRqstBody from RTCPD"},
{AGGREGATOR_RECEIVED_FILERQSTBODY_FROM_RTCPD, "Received FileRqstBody from RTCPD"},
{AGGREGATOR_RECEIVE_FILERQSTERRBODY_FROM_RTCPD, "Receiving FileRqstErrBody from RTCPD"},
{AGGREGATOR_RECEIVED_FILERQSTERRBODY_FROM_RTCPD, "Received FileRqstErrBody from RTCPD"},
{AGGREGATOR_PROCESSING_TAPE_DISK_RQST, "Processing a tape/disk IO request from RTCPD"},
{AGGREGATOR_PING_RTCPD, "Pinging RTCPD"},
{AGGREGATOR_PINGED_RTCPD, "Pinged RTCPD"},
{AGGREGATOR_SEND_ACK_TO_RTCPD, "Sending acknowledge to RTCPD"},
{AGGREGATOR_SENT_ACK_TO_RTCPD, "Sent acknowledge to RTCPD"},
{AGGREGATOR_SEND_HEADER_TO_RTCPD, "Sending message header to RTCPD"},
{AGGREGATOR_SENT_HEADER_TO_RTCPD, "Sent message header to RTCPD"},
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
{AGGREGATOR_NOTIFY_CLIENT_FILE_MIGRATED, "Notifying client file migrated"},
{AGGREGATOR_NOTIFIED_CLIENT_FILE_MIGRATED, "Notified client file migrated"},
{AGGREGATOR_NOTIFY_CLIENT_FILE_RECALLED, "Notifying client file recalled"},
{AGGREGATOR_NOTIFIED_CLIENT_FILE_RECALLED, "Notified client file recalled"},
{AGGREGATOR_NOTIFY_CLIENT_END_OF_SESSION, "Notifying client end of session"},
{AGGREGATOR_NOTIFIED_CLIENT_END_OF_SESSION, "Notified client end of session"},
{AGGREGATOR_GET_DUMP_PARAMETERS_FROM_CLIENT, "Getting dump parameters from client"},
{AGGREGATOR_GOT_DUMP_PARAMETERS_FROM_CLIENT, "Got dump parameters from client"},
{AGGREGATOR_NOTIFY_CLIENT_DUMP_MESSAGE, "Notifying client dump message"},
{AGGREGATOR_NOTIFIED_CLIENT_DUMP_MESSAGE, "Notified client dump message"},
{AGGREGATOR_FAILED_TO_NOTIFY_CLIENT_END_OF_SESSION, "Failed to notify client end of session"},
{AGGREGATOR_NOTIFY_CLIENT_END_OF_FAILED_SESSION, "Notifying client end of failed session"},
{AGGREGATOR_NOTIFIED_CLIENT_END_OF_FAILED_SESSION, "Notified client end of failed session"},
{AGGREGATOR_FAILED_TO_NOTIFY_CLIENT_END_OF_FAILED_SESSION, "Failed to notify client end of failed session"},
{AGGREGATOR_SELECT_INTR, "Select interrupted"},
{AGGREGATOR_ENTERING_BRIDGE_MODE, "Entering bridge mode"},
{AGGREGATOR_PING_CLIENT, "Pinging client"},
{AGGREGATOR_PINGED_CLIENT, "Pinged client"},
{AGGREGATOR_CLOSED_INITIAL_CALLBACK_CONNECTION, "Closed initial callback connection"},
{AGGREGATOR_RECEIVED_TAPE_INFO_FROM_VMGR, "Received tape information from VMGR"},
{AGGREGATOR_SENT_FILETOMIGRATEREQUEST, "Sent FileToMigrateRequest to client"},
{AGGREGATOR_RECEIVED_FILETOMIGRATE, "Received FileToMigrate from client"},
{AGGREGATOR_SENT_FILETORECALLREQUEST, "Sent FileToRecallRequest to client"},
{AGGREGATOR_RECEIVED_FILETORECALL, "Received FileToRecall from client"},
{AGGREGATOR_SENT_FILEMIGRATEDNOTIFICATION, "Sent FileMigratedNotification to client"},
{AGGREGATOR_SENT_FILERECALLEDNOTIFICATION, "Sent FileRecalledNotification to client"},
{AGGREGATOR_RECEIVED_NOMOREFILES, "Recieved NoMoreFiles from client"},
{AGGREGATOR_RECEIVED_ACK_OF_NOTIFICATION, "Recieved acknowledge of notification from client"},
{AGGREGATOR_RECEIVED_ENDNOTIFCATIONERRORREPORT, "Received EndNotificationErrorReport from client"},
{AGGREGATOR_RECEIVED_NOTIFCATIONACKNOWLEDGE, "Received NotificationAcknowledge from client"},
{AGGREGATOR_SEND_END_OF_SESSION_TO_RTCPD, "Sending end-of-session to RTCPD"},
{AGGREGATOR_FAILED_TO_PARSE_TPCONFIG, "Failed to parse TPCONFIG file"},
{AGGREGATOR_PARSED_TPCONFIG, "Parsed TPCONFIG"},
{AGGREGATOR_ADDED_TAPE_SESSION_TO_CATLAOGUE, "Added tape session to catalogue"},
{AGGREGATOR_REMOVED_TAPE_SESSION_FROM_CATALOGUE, "Removed tape session from catalogue"},
{AGGREGATOR_CLOSED_RTCPD_DISK_TAPE_CONNECTION_DUE_TO_PEER, "Closed rtcpd disk/tape IO control connection due to rtcpd closing its end"},
{-1, ""}};
