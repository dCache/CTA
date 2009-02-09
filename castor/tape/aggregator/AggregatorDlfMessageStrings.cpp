/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Mon Feb  9 15:22:58 CET 2009
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
{AGGREGATOR_HANDLE_JOB_MESSAGE, "Handle job submission message VDQM_CLIENTINFO:RTCOPY_MAGIC_OLD0"},
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
{AGGREGATOR_GAVE_VOLUME_INFO, "Gave volume information to RTCPD"},
{AGGREGATOR_FAILED_TO_START_GATEWAY_PROTOCOL, "Failed to start prototcol with tape gateway"},
{AGGREGATOR_GAVE_FILE_INFO, "Gave file information to RTCPD"},
{AGGREGATOR_FAILED_TO_GIVE_FILE_INFO, "Failed to give file information to RTCPD"},
{AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS, "Signalled no more requests to RTCPD"},
{AGGREGATOR_FAILED_TO_SIGNAL_NO_MORE_REQUESTS, "Failed to signal no more requests to RTCPD"},
{AGGREGATOR_FAILED_TO_COORDINATE_REMOTE_COPY, "Failed to coordinate remote copy"},
{AGGREGATOR_TRANSFER_FAILED, "Transfer failed"},
{AGGREGATOR_PROCESS_RTCPD_CONNECTIONS_FAILED, "Failed to process RTCPD connections"},
{AGGREGATOR_PINGED_RTCPD, "Pinged RTCPD"},
{AGGREGATOR_MAIN_SELECT_FAILED, "The select of the main select loop encountered an error other than an interruption"},
{AGGREGATOR_GAVE_REQUEST_FOR_MORE_WORK, "Gave request for more work to RTCPD"},
{AGGREGATOR_DATA_ON_INITIAL_RTCPD_CONNECTION, "Data has arrived on the initial RTCPD connection"},
{AGGREGATOR_RECEIVED_RTCP_ENDOF_REQ, "Received RTCP_ENDOF_REQ"},
{AGGREGATOR_TAPE_POSITIONED_FILE_REQ, "Tape positioned"},
{AGGREGATOR_TAPE_POSITIONED_TAPE_REQ, "Tape positioned (tape request)"},
{AGGREGATOR_FILE_TRANSFERED, "File transfered"},
{AGGREGATOR_TOLD_GATEWAY_TO_START_TRANSFER, "Told tape gateway to start transfer"},
{AGGREGATOR_GOT_REQUEST_INFO_FROM_RTCPD, "Got request information from RTCP"},
{-1, ""}};
