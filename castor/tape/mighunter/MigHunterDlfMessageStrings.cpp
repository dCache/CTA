/* This file was generated by ./MigHunterDlfMessagesCodeGenerator on Tue Feb 23 18:06:44 CET 2010
 */

/******************************************************************************
 *           castor/tape/mighunter/MighunterDlfMessageStrings.cpp
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

#include "castor/tape/mighunter/MigHunterDlfMessageConstants.hpp"
#include "castor/tape/mighunter/MigHunterDaemon.hpp"

castor::dlf::Message castor::tape::mighunter::MigHunterDaemon::s_dlfMessages[] = {
{NOT_USED_1, "Service shutdown"},
{DAEMON_STOP, "Service shutdown"},
{FATAL_ERROR, "Fatal Error"},
{NO_POLICY, "No Policy file available"},
{PARSING_OPTIONS, "parameters option parsed"},
{NO_TAPECOPIES, "No tapecopy found"},
{TAPECOPIES_FOUND, "No migration candidate found"},
{NO_TAPEPOOLS, "No tapepool found"},
{NOT_ENOUGH, "not enough data to create streams"},
{NO_DRIVES, "no drive assigned to this service class"},
{POLICY_INPUT, "input to call the migration policy"},
{ALLOWED_WITHOUT_POLICY, "allowed without policy"},
{ALLOWED_BY_POLICY, "allowed by policy"},
{NOT_ALLOWED, "not allowed"},
{MIGRATION_POLICY_RESULT, "summary of migration policy results"},
{ATTACHED_TAPECOPIES, "attaching tapecopies to streams"},
{DB_ERROR, "db error"},
{RESURRECT_TAPECOPIES, "resurrecting tapecopies"},
{INVALIDATE_TAPECOPIES, "invalidating tapecopies"},
{NS_ERROR, "Error retrieving the file stat from the nameserver"},
{NO_STREAM, "No stream found"},
{STREAMS_FOUND, "Streams found"},
{STREAM_INPUT, "input to call stream policy"},
{START_WITHOUT_POLICY, "started without policy"},
{START_BY_POLICY, "started by policy"},
{NOT_STARTED, "stopped"},
{STREAM_POLICY_RESULT, "summary of stream policy results"},
{STARTED_STREAMS, "stream started"},
{STOP_STREAMS, "stream stopped"},
{MIGHUNTER_FAILED_TO_START, "mighunterd failed to start"},
{FAILED_TO_APPLY_MIGRATION_POLICY, "Failed to apply migration policy, attaching tape copy"},
{FAILED_TO_APPLY_STREAM_POLICY, "Failed to apply stream policy, starting stream"},
{ALLOWED_BY_MIGRATION_POLICY, "Allowed by migration policy"},
{NOT_ALLOWED_BY_MIGRATION_POLICY, "Not allowed by migration policy"},
{ALLOWED_BY_STREAM_POLICY, "Allowed by stream policy"},
{NOT_ALLOWED_BY_STREAM_POLICY, "Not allowed by stream policy"},
{MIGRATION_POLICY_FUNCTION_NOT_IN_MODULE, "Migration-policy Python-function not in migration-policy Python-module"},
{STREAM_POLICY_FUNCTION_NOT_IN_MODULE, "Stream-policy Python-function not in stream-policy Python-module"},
{MIGRATION_POLICY_NOT_CONFIGURED, "Migration policy not configured"},
{STREAM_POLICY_NOT_CONFIGURED, "Stream policy not configured"},
{MIGHUNTERTHREAD_SKIPPING_SRVCCLASS, "MigHunter thread skipping service class because there are no candidate tape copies"},
{DAEMON_START, "Service started"},
{-1, ""}};
