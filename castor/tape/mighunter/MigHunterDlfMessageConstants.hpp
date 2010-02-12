/* This file was generated by ./MigHunterDlfMessagesCodeGenerator on Fri Feb 12 15:34:09 CET 2010
 */

/******************************************************************************
 *             castor/tape/mighunter/MighunterDlfMessageConstants.hpp
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

#ifndef CASTOR_TAPE_MIGHUNTER_MIGHUNTERDLFMESSAGECONSTANTS_HPP
#define CASTOR_TAPE_MIGHUNTER_MIGHUNTERDLFMESSAGECONSTANTS_HPP 1


namespace castor    {
namespace tape   {
namespace mighunter {
enum MighunterDlfMessages {
NOT_USED_1=0, /* "Service shutdown" */
DAEMON_STOP=1, /* "Service shutdown" */
FATAL_ERROR=2, /* "Fatal Error" */
NO_POLICY=3, /* "No Policy file available" */
PARSING_OPTIONS=4, /* "parameters option parsed" */
NO_TAPECOPIES=5, /* "No tapecopy found" */
TAPECOPIES_FOUND=6, /* "No migration candidate found" */
NO_TAPEPOOLS=7, /* "No tapepool found" */
NOT_ENOUGH=8, /* "not enough data to create streams" */
NO_DRIVES=9, /* "no drive assigned to this service class" */
POLICY_INPUT=10, /* "input to call the migration policy" */
ALLOWED_WITHOUT_POLICY=11, /* "allowed without policy" */
ALLOWED_BY_POLICY=12, /* "allowed by policy" */
NOT_ALLOWED=13, /* "not allowed" */
POLICY_RESULT=14, /* "summary of migration policy results" */
ATTACHED_TAPECOPIES=15, /* "attaching tapecopies to streams" */
DB_ERROR=16, /* "db error" */
RESURRECT_TAPECOPIES=17, /* "resurrecting tapecopies" */
INVALIDATE_TAPECOPIES=18, /* "invalidating tapecopies" */
NS_ERROR=19, /* "Error retrieving the file stat from the nameserver" */
NO_STREAM=20, /* "No stream found" */
STREAMS_FOUND=21, /* "Streams found" */
STREAM_INPUT=22, /* "input to call stream policy" */
START_WITHOUT_POLICY=23, /* "started without policy" */
START_BY_POLICY=24, /* "started by policy" */
NOT_STARTED=25, /* "stopped" */
STREAM_POLICY_RESULT=26, /* "summary of stream policy results" */
STARTED_STREAMS=27, /* "stream started" */
STOP_STREAMS=28, /* "stream stopped" */
MIGHUNTER_FAILED_TO_START=29, /* "mighunterd failed to start" */
FAILED_TO_APPLY_MIGRATION_POLICY=30, /* "Failed to apply migration policy, attaching tape copy" */
FAILED_TO_APPLY_STREAM_POLICY=31, /* "Failed to apply stream policy, starting stream" */
ALLOWED_BY_MIGRATION_POLICY=32, /* "Allowed by migration policy" */
NOT_ALLOWED_BY_MIGRATION_POLICY=33, /* "Not allowed by migration policy" */
ALLOWED_BY_STREAM_POLICY=34, /* "Allowed by stream policy" */
NOT_ALLOWED_BY_STREAM_POLICY=35, /* "Not allowed by stream policy" */
MIGRATION_POLICY_FUNCTION_NOT_IN_MODULE=36, /* "Migration-policy Python-function not in migration-policy Python-module" */
STREAM_POLICY_FUNCTION_NOT_IN_MODULE=37, /* "Stream-policy Python-function not in stream-policy Python-module" */
MIGRATION_POLICY_NOT_CONFIGURED=38, /* "Migration policy not configured" */
STREAM_POLICY_NOT_CONFIGURED=39, /* "Stream policy not configured" */
MIGHUNTERTHREAD_SKIPPING_SRVCCLASS=40, /* "MigHunter thread skipping service class because there are no candidate tape copies" */
DAEMON_START=41 /* "Service started" */
}; // enum MighunterDlfMessages
} // namespace mighunter
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_MIGHUNTER_MIGHUNTERDLFMESSAGECONSTANTS_HPP
