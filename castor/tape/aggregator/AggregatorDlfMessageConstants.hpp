/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Fri Jan 23 11:26:28 CET 2009
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
AGGREGATOR_VDQM_CONNECTION_WITHOUT_INFO=6, /* "Received a connection from VDQM without peer host and port information" */
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
AGGREGATOR_RTCPD_CONNECTION_WITH_INFO=20, /* "Received a connection from RTCPD" */
AGGREGATOR_RTCPD_CONNECTION_WITHOUT_INFO=21, /* "Received a connection from RTCPD without peer host and port information" */
AGGREGATOR_GOT_VOLREQID=22, /* "Got volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_GET_VOLREQID=23, /* "Failed to get volume request ID from RTCPD" */
AGGREGATOR_FAILED_TO_MARSHALL_RCP_JOB_REPLY_MESSAGE=24, /* "Failed to marshall RCP job reply message" */
AGGREGATOR_GAVE_VOLUME_INFO=25, /* "Gave volume information to RTCPD" */
AGGREGATOR_FAILED_TO_GIVE_VOLUME_INFO=26, /* "Failed to give volume information to RTCPD" */
AGGREGATOR_GAVE_FILE_INFO=27, /* "Gave file information to RTCPD" */
AGGREGATOR_FAILED_TO_GIVE_FILE_INFO=28, /* "Failed to give file information to RTCPD" */
AGGREGATOR_SIGNALLED_NO_MORE_REQUESTS=29, /* "Signalled no more requests to RTCPD" */
AGGREGATOR_FAILED_TO_SIGNAL_NO_MORE_REQUESTS=30 /* "Failed to signal no more requests to RTCPD" */
}; // enum AggregatorDlfMessages
} // namespace aggregator
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_AGGREGATOR_AGGREGATORDLFMESSAGECONSTANTS_HPP
