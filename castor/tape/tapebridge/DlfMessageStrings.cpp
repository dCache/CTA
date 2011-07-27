/* This file was generated by ./DlfMessagesCodeGenerator on Wed Jul 27 09:23:55 CEST 2011
 */

/******************************************************************************
 *           castor/tape/tapebridge/TapeBridgeDlfMessageStrings.cpp
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

#include "castor/tape/tapebridge/DlfMessageConstants.hpp"
#include "castor/tape/tapebridge/TapeBridgeDaemon.hpp"

castor::dlf::Message castor::tape::tapebridge::TapeBridgeDaemon::s_dlfMessages[] = {
{TAPEBRIDGE_NULL, " - "},
{TAPEBRIDGE_STARTED, "tapebridged started"},
{TAPEBRIDGE_FAILED_TO_START, "tapebridged failed to start"},
{TAPEBRIDGE_RECEIVED_VDQM_CONNECTION, "Received a connection from VDQM"},
{TAPEBRIDGE_HANDLE_VDQM_REQUEST_EXCEPT, "Exception raised when handling a request from the VDQM"},
{TAPEBRIDGE_FAILED_TO_READ_MESSAGE_HEADER, "Failed to read message header"},
{TAPEBRIDGE_UNKNOWN_MAGIC, "Unknown magic number"},
{TAPEBRIDGE_UNKNOWN_REQUEST_TYPE, "Unknown request type"},
{TAPEBRIDGE_FAILED_TO_READ_MESSAGE_BODY, "Failed to read message body"},
{TAPEBRIDGE_RECEIVE_RTCOPY_JOB, "Receiving remote-copy job"},
{TAPEBRIDGE_RECEIVED_RTCOPY_JOB, "Received remote-copy job"},
{TAPEBRIDGE_SUBMITTING_RTCOPY_JOB_TO_RTCPD, "Submitting remote-copy job to RTCPD"},
{TAPEBRIDGE_FAILED_TO_SUBMIT_JOB_TO_RTCPD, "Failed to submit remote-copy job to RTCPD"},
{TAPEBRIDGE_FAILED_TO_MARSHALL_RTCP_ACKN, "Failed to marshall RTCP acknowledge message"},
{TAPEBRIDGE_FAILED_TO_SEND_RTCOPY_JOB_REPLY_TO_VDQM, "Failed to send remote-copy job reply to VDQM"},
{TAPEBRIDGE_FAILED_TO_PROCESS_RCOPY_JOB_SUBMISSION, "Failed to process remote-copy job submission message"},
{TAPEBRIDGE_RTCPD_HANDLER_SOCKET_IS_NULL, "The RtcpdHandlerThread has been passed a NULL socket pointer"},
{TAPEBRIDGE_INITIAL_RTCPD_CALLBACK, "Received initial callback connection from RTCPD"},
{TAPEBRIDGE_RTCPD_CALLBACK, "Received a callback connection from RTCPD"},
{TAPEBRIDGE_GOT_VOLREQID, "Got volume request ID from RTCPD"},
{TAPEBRIDGE_FAILED_TO_GET_VOLREQID, "Failed to get volume request ID from RTCPD"},
{TAPEBRIDGE_FAILED_TO_MARSHALL_RTCOPY_JOB_REPLY_MESSAGE, "Failed to marshall remote-copy job reply message"},
{TAPEBRIDGE_SIGNALLED_NO_MORE_REQUESTS, "Signalled no more requests to RTCPD"},
{TAPEBRIDGE_FAILED_TO_COORDINATE_REMOTE_COPY, "Failed to coordinate remote-copy"},
{TAPEBRIDGE_TRANSFER_FAILED, "Transfer failed"},
{TAPEBRIDGE_GAVE_REQUEST_FOR_MORE_WORK, "Gave request for more work to RTCPD"},
{TAPEBRIDGE_DATA_ON_INITIAL_RTCPD_CONNECTION, "Data has arrived on the initial RTCPD connection"},
{TAPEBRIDGE_RECEIVED_RTCP_ENDOF_REQ, "Received RTCP_ENDOF_REQ"},
{TAPEBRIDGE_TAPE_POSITIONED, "Tape positioned"},
{TAPEBRIDGE_FILE_TRANSFERED_DEPRECATED, "File transfered"},
{TAPEBRIDGE_GET_REQUEST_INFO_FROM_RTCPD, "Getting request information from RTCPD"},
{TAPEBRIDGE_GOT_REQUEST_INFO_FROM_RTCPD, "Got request information from RTCPD"},
{TAPEBRIDGE_TOLD_CLIENT_TO_START_TRANSFER, "Told client to start the transfer protocol"},
{TAPEBRIDGE_GET_VOLUME_FROM_CLIENT, "Getting volume from client"},
{TAPEBRIDGE_GOT_VOLUME_FROM_CLIENT, "Got volume from client"},
{TAPEBRIDGE_GOT_NO_MORE_FILES_FROM_CLIENT, "Got no more files from client"},
{TAPEBRIDGE_GET_FILE_TO_MIGRATE_FROM_CLIENT, "Getting file to migrate from client"},
{TAPEBRIDGE_GOT_FILE_TO_MIGRATE_FROM_CLIENT, "Got file to migrate from client"},
{TAPEBRIDGE_GET_FILE_TO_RECALL_FROM_CLIENT, "Getting file to recall from client"},
{TAPEBRIDGE_GOT_FILE_TO_RECALL_FROM_CLIENT, "Got file to recall from client"},
{TAPEBRIDGE_GIVE_VOLUME_TO_RTCPD, "Giving volume to RTCPD"},
{TAPEBRIDGE_GAVE_VOLUME_TO_RTCPD, "Gave volume to RTCPD"},
{TAPEBRIDGE_ASK_RTCPD_TO_RQST_MORE_RECALL_WORK, "Asking RTCPD to request more recall work"},
{TAPEBRIDGE_ASKED_RTCPD_TO_RQST_MORE_RECALL_WORK, "Asked RTCPD to request more recall work"},
{TAPEBRIDGE_ASK_RTCPD_TO_RQST_MORE_MIGRATE_WORK, "Asking RTCPD to request more migrate work"},
{TAPEBRIDGE_ASKED_RTCPD_TO_RQST_MORE_MIGRATE_WORK, "Asked RTCPD to request more migrate work"},
{TAPEBRIDGE_TELL_RTCPD_DUMP_TAPE, "Telling RTCPD to dump tape"},
{TAPEBRIDGE_TOLD_RTCPD_DUMP_TAPE, "Told RTCPD to dump tape"},
{TAPEBRIDGE_TELL_RTCPD_END_OF_FILE_LIST, "Telling RTCPD end of file list"},
{TAPEBRIDGE_TOLD_RTCPD_END_OF_FILE_LIST, "Told RTCPD end of file list"},
{TAPEBRIDGE_TELL_RTCPD_TO_ABORT, "Telling RTCPD to abort"},
{TAPEBRIDGE_TOLD_RTCPD_TO_ABORT, "Told RTCPD to abort"},
{TAPEBRIDGE_GIVE_RECALL_FILE_TO_RTCPD, "Giving recall file to RTCPD"},
{TAPEBRIDGE_GAVE_RECALL_FILE_TO_RTCPD, "Gave recall file to RTCPD"},
{TAPEBRIDGE_GIVE_MIGRATE_FILE_TO_RTCPD, "Giving migrate file to RTCPD"},
{TAPEBRIDGE_GAVE_MIGRATE_FILE_TO_RTCPD, "Gave migrate file to RTCPD"},
{TAPEBRIDGE_RECEIVE_MSGBODY_FROM_RTCPD, "Receiving message body from RTCPD"},
{TAPEBRIDGE_RECEIVED_MSGBODY_FROM_RTCPD, "Received message body from RTCPD"},
{TAPEBRIDGE_RECEIVE_FILERQSTBODY_FROM_RTCPD, "Receiving FileRqstBody from RTCPD"},
{TAPEBRIDGE_RECEIVED_FILERQSTBODY_FROM_RTCPD, "Received FileRqstBody from RTCPD"},
{TAPEBRIDGE_RECEIVE_FILERQSTERRBODY_FROM_RTCPD, "Receiving FileRqstErrBody from RTCPD"},
{TAPEBRIDGE_RECEIVED_FILERQSTERRBODY_FROM_RTCPD, "Received FileRqstErrBody from RTCPD"},
{TAPEBRIDGE_PROCESSING_TAPE_DISK_RQST, "Processing a tape/disk IO request from RTCPD"},
{TAPEBRIDGE_PING_RTCPD, "Pinging RTCPD"},
{TAPEBRIDGE_PINGED_RTCPD, "Pinged RTCPD"},
{TAPEBRIDGE_SEND_ACK_TO_RTCPD, "Sending acknowledge to RTCPD"},
{TAPEBRIDGE_SENT_ACK_TO_RTCPD, "Sent acknowledge to RTCPD"},
{TAPEBRIDGE_SEND_HEADER_TO_RTCPD, "Sending message header to RTCPD"},
{TAPEBRIDGE_SENT_HEADER_TO_RTCPD, "Sent message header to RTCPD"},
{TAPEBRIDGE_RECEIVE_TAPERQSTBODY, "Receiving tape request message body"},
{TAPEBRIDGE_RECEIVED_TAPERQSTBODY, "Received tape request message body"},
{TAPEBRIDGE_RECEIVE_GIVEOUTPBODY, "Receiving GIVE_OUTP message body"},
{TAPEBRIDGE_RECEIVED_GIVEOUTPBODY, "Received GIVE_OUTP message body"},
{TAPEBRIDGE_RECEIVE_TAPERQSTERRBODY, "Receiving tape request message body with error appendix"},
{TAPEBRIDGE_RECEIVED_TAPERQSTERRBODY, "Received tape request message body with error appendix"},
{TAPEBRIDGE_SEND_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD, "Sending delayed acknowledge of request for more work to RTCPD"},
{TAPEBRIDGE_SENT_DELAYED_REQUEST_MORE_WORK_ACK_TO_RTCPD, "Sent delayed acknowledge of request for more work to RTCPD"},
{TAPEBRIDGE_CREATED_RTCPD_CALLBACK_PORT, "Created TCP/IP port for RTCPD callbacks"},
{TAPEBRIDGE_CONNECTION_CLOSED_BY_RTCPD, "Connection closed by RTCPD"},
{TAPEBRIDGE_CLOSED_CONNECTION, "Connection closed by tapebridge"},
{TAPEBRIDGE_NOTIFY_CLIENT_FILE_MIGRATED, "Notifying client file migrated"},
{TAPEBRIDGE_NOTIFIED_CLIENT_FILE_MIGRATED, "Notified client file migrated"},
{TAPEBRIDGE_NOTIFY_CLIENT_FILE_RECALLED, "Notifying client file recalled"},
{TAPEBRIDGE_NOTIFIED_CLIENT_FILE_RECALLED, "Notified client file recalled"},
{TAPEBRIDGE_NOTIFY_CLIENT_END_OF_SESSION, "Notifying client end of session"},
{TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_SESSION, "Notified client end of session"},
{TAPEBRIDGE_GET_DUMP_PARAMETERS_FROM_CLIENT, "Getting dump parameters from client"},
{TAPEBRIDGE_GOT_DUMP_PARAMETERS_FROM_CLIENT, "Got dump parameters from client"},
{TAPEBRIDGE_NOTIFY_CLIENT_DUMP_MESSAGE, "Notifying client dump message"},
{TAPEBRIDGE_NOTIFIED_CLIENT_DUMP_MESSAGE, "Notified client dump message"},
{TAPEBRIDGE_FAILED_TO_NOTIFY_CLIENT_END_OF_SESSION, "Failed to notify client end of session"},
{TAPEBRIDGE_NOTIFY_CLIENT_END_OF_FAILED_SESSION, "Notifying client end of failed session"},
{TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_FAILED_SESSION, "Notified client end of failed session"},
{TAPEBRIDGE_FAILED_TO_NOTIFY_CLIENT_END_OF_FAILED_SESSION, "Failed to notify client end of failed session"},
{TAPEBRIDGE_SELECT_INTR, "Select interrupted"},
{TAPEBRIDGE_ENTERING_BRIDGE_MODE, "Entering bridge mode"},
{TAPEBRIDGE_PING_CLIENT, "Pinging client"},
{TAPEBRIDGE_PINGED_CLIENT, "Pinged client"},
{TAPEBRIDGE_CLOSED_INITIAL_CALLBACK_CONNECTION, "Closed initial callback connection"},
{TAPEBRIDGE_RECEIVED_TAPE_INFO_FROM_VMGR, "Received tape information from VMGR"},
{TAPEBRIDGE_SENT_FILETOMIGRATEREQUEST, "Sent FileToMigrateRequest to client"},
{TAPEBRIDGE_RECEIVED_FILETOMIGRATE, "Received FileToMigrate from client"},
{TAPEBRIDGE_SENT_FILETORECALLREQUEST, "Sent FileToRecallRequest to client"},
{TAPEBRIDGE_RECEIVED_FILETORECALL, "Received FileToRecall from client"},
{TAPEBRIDGE_SENT_FILEMIGRATEDNOTIFICATION, "Sent FileMigratedNotification to client"},
{TAPEBRIDGE_SENT_FILERECALLEDNOTIFICATION, "Sent FileRecalledNotification to client"},
{TAPEBRIDGE_RECEIVED_NOMOREFILES, "Recieved NoMoreFiles from client"},
{TAPEBRIDGE_RECEIVED_ACK_OF_NOTIFICATION, "Recieved acknowledge of notification from client"},
{TAPEBRIDGE_RECEIVED_ENDNOTIFCATIONERRORREPORT, "Received EndNotificationErrorReport from client"},
{TAPEBRIDGE_RECEIVED_NOTIFCATIONACKNOWLEDGE, "Received NotificationAcknowledge from client"},
{TAPEBRIDGE_SEND_END_OF_SESSION_TO_RTCPD, "Sending end-of-session to RTCPD"},
{TAPEBRIDGE_FAILED_TO_PARSE_TPCONFIG, "Failed to parse TPCONFIG file"},
{TAPEBRIDGE_PARSED_TPCONFIG, "Parsed TPCONFIG"},
{TAPEBRIDGE_CLOSED_RTCPD_DISK_TAPE_CONNECTION_DUE_TO_PEER, "Closed rtcpd disk/tape IO control connection due to rtcpd closing its end"},
{TAPEBRIDGE_TOO_MANY_DRIVES_IN_TPCONFIG, "There are more drives in the TPCONFIG file than there were when tapebridged was started.  Please restart tapebridged."},
{TAPEBRIDGE_RECEIVED_TAPEBRIDGE_FLUSHEDTOTAPE, "Received TAPEBRIDGE_FLUSHEDTOTAPE"},
{TAPEBRIDGE_CONFIG_PARAM, "Determined value of configuration parameter"},
{TAPEBRIDGE_NOTIFY_CLIENT_END_OF_FAILED_SESSION_DUE_TO_FILE, "Notifying client end of failed session due to failed copy of tape file"},
{TAPEBRIDGE_NOTIFIED_CLIENT_END_OF_FAILED_SESSION_DUE_TO_FILE, "Notified client end of failed session due to failed copy of tape file"},
{TAPEBRIDGE_FILE_WRITTEN_WITHOUT_FLUSH, "File written without flush"},
{TAPEBRIDGE_FILE_READ, "File read"},
{TAPEBRIDGE_ADDED_PENDING_MIGRATION_TO_STORE, "Added pending file-migration to store"},
{TAPEBRIDGE_MARKED_PENDING_MIGRATION_WRITTEN_WITHOUT_FLUSH, "Marked pending file-migration in store as written without flush"},
{-1, ""}};
