/* This file was generated by ./TapeGatewayDlfMessagesCodeGenerator on Wed May  9 16:15:35 WEDT 2012
 */

/******************************************************************************
 *           castor/tape/tapegateway/TapeGatewayDlfMessageStrings.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "castor/tape/tapegateway/TapeGatewayDlfMessageConstants.hpp"
#include "castor/tape/tapegateway/daemon/TapeGatewayDaemon.hpp"

castor::dlf::Message castor::tape::tapegateway::TapeGatewayDaemon::s_dlfMessages[] = {
{DAEMON_START, "Service startup"},
{DAEMON_STOP, "Service shoutdown"},
{NO_RETRY_POLICY_FOUND, "Incomplete parameters for retry policy"},
{FATAL_ERROR, "Fatal error"},
{PRODUCER_GETTING_TAPE_5, "VdqmRequestsProducer: getting a tape to submit"},
{PRODUCER_NO_TAPE_REPL_6, "VdqmRequestsProducer: no tape retrieved"},
{PRODUCER_TAPE_FOUND_7, "VdqmRequestsProducer: no tape to submit"},
{PRODUCER_CANNOT_UPDATE_DB, "VdqmRequestsProducer: cannot update db"},
{PRODUCER_QUERYING_VMGR, "VdqmRequestsProducer: querying vmgr"},
{PRODUCER_VMGR_ERROR, "VdqmRequestsProducer: vmgr error"},
{PRODUCER_SUBMITTING_VDQM_11, "VdqmRequestsProducer: submitting to vdqm"},
{PRODUCER_VDQM_ERROR, "VdqmRequestsProducer: vdqm error"},
{PRODUCER_REQUEST_SAVED_13, "VdqmRequestsProducer: vdqm request id saved in the db"},
{CHECKER_GETTING_TAPES, "VdqmRequestsChecker: getting tapes to check"},
{CHECKER_NO_TAPE, "VdqmRequestsChecker: no tape to check"},
{CHECKER_QUERYING_VDQM, "VdqmRequestsChecker: querying vdqm"},
{CHECKER_LOST_VDQM_REQUEST, "VdqmRequestsChecker: request was lost or out of date"},
{CHECKER_CANNOT_UPDATE_DB, "VdqmRequestsChecker: cannot update db"},
{CHECKER_RELEASING_UNUSED_TAPE, "VdqmRequestsChecker: releasing BUSY tape"},
{CHECKER_VMGR_ERROR, "VdqmRequestsChecker: vmgr error,impossible to reset BUSY state"},
{CHECKER_TAPES_FOUND, "VdqmRequestsChecker: tapes to check found"},
{CHECKER_TAPES_RESURRECTED, "VdqmRequestsChecker: tapes resurrected"},
{LINKER_GETTING_STREAMS_REPL_23, "TapeStreamLinker: getting streams to resolve"},
{LINKER_NO_STREAM_REPL_24, "TapeStreamLinker: no stream to resolve"},
{LINKER_QUERYING_VMGR_REPL_25, "TapeStreamLinker: querying vmgr"},
{LINKER_LINKING_TAPE_STREAM_REPL_26, "TapeStreamLinker: association tape-stream done"},
{LINKER_CANNOT_UPDATE_DB_REPL_27, "TapeStreamLinker: cannot update db"},
{LINKER_RELEASED_BUSY_TAPE_REPL_28, "TapeStreamLinker: released BUSY tape"},
{LINKER_NO_TAPE_AVAILABLE_REPL_29, "TapeStreamLinker:No tape available in such tapepool"},
{LINKER_STREAMS_FOUND_REPL_30, "TapeStreamLinker: streams found"},
{LINKER_TAPES_ATTACHED_REPL_31, "TapeStreamLinker: tapes attached to streams"},
{MIG_ERROR_GETTING_FILES_REPL_32, "MigratorErrorHandlerThread: getting failed tapecopy"},
{MIG_ERROR_NO_TAPECOPY_REPL_33, "MigratorErrorHandlerThread: no tapecopy found"},
{MIG_ERROR_RETRY, "MigratorErrorHandlerThread: retry this migration"},
{MIG_ERROR_FAILED, "MigratorErrorHandlerThread: fail this migration"},
{MIG_ERROR_RETRY_BY_DEFAULT, "MigratorErrorHandlerThread: retry this migration without applying the policy"},
{MIG_ERROR_CANNOT_UPDATE_DB, "MigratorErrorHandlerThread: cannot update db"},
{MIG_ERROR_TAPECOPIES_FOUND_REPL_38, "MigratorErrorHandlerThread: tapecopies found"},
{MIG_ERROR_RESULT_SAVED, "MigratorErrorHandlerThread: result saved"},
{REC_ERROR_GETTING_FILES_REPL_40, "RecallerErrorHandlerThread: getting failed tapecopy"},
{REC_ERROR_NO_TAPECOPY_REPL_41, "RecallerErrorHandlerThread: no tapecopy found"},
{REC_ERROR_RETRY, "RecallerErrorHandlerThread: retry this recall"},
{REC_ERROR_FAILED, "RecallerErrorHandlerThread: fail this recall"},
{REC_ERROR_RETRY_BY_DEFAULT, "RecallerErrorHandlerThread: retry this recall without applying the policy"},
{REC_ERROR_CANNOT_UPDATE_DB, "RecallerErrorHandlerThread: cannot update db"},
{REC_ERROR_TAPECOPIES_FOUND_46, "RecallerErrorHandlerThread: tapecopies found"},
{REC_ERROR_RESULT_SAVED, "RecallerErrorHandlerThread: result saved"},
{WORKER_MESSAGE_RECEIVED, "Worker: received a message"},
{WORKER_UNKNOWN_CLIENT, "Worker: unknown client"},
{WORKER_INVALID_REQUEST, "Worker: invalid request"},
{WORKER_INVALID_CAST, "Worker: invalid cast"},
{WORKER_DISPATCHING, "Worker: dispatching request"},
{WORKER_RESPONDING, "Worker: responding to the Tapebridge"},
{WORKER_CANNOT_RESPOND, "Worker: cannot respond to the Tapebridge"},
{WORKER_CANNOT_RECEIVE, "Worker: cannot receive message"},
{WORKER_UNKNOWN_EXCEPTION, "Worker: received volume request"},
{WORKER_VOLUME_REQUESTED, "Worker: getting volume from db"},
{WORKER_GETTING_VOLUME, "Worker: getting volume from db"},
{WORKER_NO_VOLUME, "Worker: no volume found"},
{WORKER_NO_FILE, "Worker: no file found for such volume"},
{WORKER_VOLUME_FOUND, "Worker: volume found"},
{WORKER_RECALL_NOTIFIED, "Worker: received recall notification"},
{WORKER_RECALL_GET_DB_INFO, "Worker: getting data from db for recalled file"},
{WORKER_RECALL_FILE_NOT_FOUND, "Worker: recalled file not found"},
{WORKER_RECALL_NS_CHECK, "Worker: checking nameserver"},
{WORKER_RECALL_NS_FAILURE, "Worker: nameserver error for recalled file"},
{WORKER_RECALL_DB_UPDATE, "Worker: updating db after recall notification"},
{WORKER_RECALL_CANNOT_UPDATE_DB_REPL_68, "Worker: cannot update db for recalled"},
{WORKER_RECALL_CHECK_FILE_SIZE, "Worker: checking file size of recalled file"},
{WORKER_RECALL_WRONG_FILE_SIZE, "Worker: wrong file size for recalled file"},
{WORKER_RECALL_COMPLETED_UPDATE_DB, "Worker: update the db after full recall completed"},
{WORKER_MIGRATION_NOTIFIED, "Worker: received migration notification"},
{WORKER_MIGRATION_GET_DB_INFO, "Worker: getting data from db for migrated file"},
{WORKER_MIGRATION_FILE_NOT_FOUND, "Worker: migrated file not found"},
{WORKER_MIGRATION_VMGR_UPDATE, "Worker: updating vmgr for migrated file"},
{WORKER_MIGRATION_NS_UPDATE, "Worker: updating nameserver for migrated file"},
{WORKER_REPACK_NS_UPDATE, "Worker: updating nameserver for repacked file"},
{WORKER_MIGRATION_NS_FAILURE, "Worker: nameserver error for migrated/repacked file"},
{WORKER_MIGRATION_VMGR_FAILURE, "Worker: vmgr error for migrated/repacked file"},
{WORKER_MIGRATION_DB_UPDATE, "Worker: updating db for migrated file"},
{WORKER_MIGRATION_CANNOT_UPDATE_DB, "Worker: cannot update db for migrated file"},
{WORKER_RECALL_REQUESTED, "Worker: file to recall requested"},
{WORKER_RECALL_RETRIEVED, "Worker: file to recall retrieved from db"},
{WORKER_RECALL_RETRIEVING_DB_ERROR, "Worker: db error while retrieving file to recall"},
{WORKER_NO_FILE_TO_RECALL, "Worker: no more file to recall"},
{WORKER_MIGRATION_REQUESTED, "Worker: file to migrate requested"},
{WORKER_MIGRATION_RETRIEVED, "Worker: file to migrate retrieved from db"},
{WORKER_MIGRATION_RETRIEVING_DB_ERROR, "Worker: db error while retrieving file to migrate"},
{WORKER_NO_FILE_TO_MIGRATE, "Worker: no more file to migrate"},
{WORKER_END_NOTIFICATION, "Worker: received end transaction notification"},
{WORKER_END_DB_UPDATE, "Worker: updating db after end transaction"},
{WORKER_END_DB_ERROR, "Worker: db error while updating for end transaction"},
{WORKER_END_GET_TAPE_TO_RELEASE, "Worker: getting tape used"},
{WORKER_END_RELEASE_TAPE, "Worker: releasing BUSY tape after end transaction"},
{WORKER_CANNOT_RELEASE_TAPE, "Worker: cannot release BUSY tape after end transaction"},
{WORKER_TAPE_MAKED_FULL, "Worker: set tape as FULL"},
{WORKER_CANNOT_MARK_TAPE_FULL, "Worker: cannot set the tape as FULL"},
{WORKER_FAIL_NOTIFICATION, "Worker: received end notification error report"},
{WORKER_FAIL_DB_UPDATE, "Worker: updating db after end notification error report"},
{WORKER_FAIL_DB_ERROR, "Worker: db error while updating for end notification error report"},
{WORKER_FAIL_GET_TAPE_TO_RELEASE, "Worker: getting tape used during failure"},
{WORKER_FAIL_RELEASE_TAPE, "Worker: releasing BUSY tape after end notification error report"},
{ORA_FILE_TO_MIGRATE_NS_ERROR, "Worker: OraTapeGatewaySvc: failed check against the nameserver for file to migrate"},
{ORA_DB_ERROR, "Worker: OraTapeGatewaySvc: impossible to update db after failure"},
{ORA_IMPOSSIBLE_TO_SEND_RMMASTER_REPORT, "Worker: OraTapeGatewaySvc: impossible to send rmmaster report"},
{ORA_FILE_TO_RECALL_NS_ERROR, "Worker: OraTapeGatewaySvc: failed check against the nameserver for file to recall"},
{LINKER_NOT_POOL_REPL_107, "TapeStreamLinker: this tapepool does not exist"},
{TAPE_GATEWAY_FAILED_TO_START, "TapeGateway failed to start"},
{INTERNAL_ERROR, "Internal error"},
{VDQMREQUESTSPRODUCER_PROCESS_FAILED, "VdqmRequestsProducerThread::process() failed"},
{LINKER_VMGRSETTOREADONLY_FAILED_REPL_111, "TapeStreamLinker: failed to set the tape to read only"},
{LINKER_VMGR_NS_DISCREPANCY_REPL_112, "TapeStreamLinker: NS/VMGR mismatch for highest FSEQ. Putting the tape readonly"},
{WORKER_REPACK_STALE_FILE, "Worker: file changed during repack: nothing to do"},
{WORKER_REPACK_UNCONFIRMED_STALE_FILE, "Worker: segment to repack is gone, but file change unconfirmed"},
{WORKER_REPACK_FILE_REMOVED, "Worker: file no found on segment update for repack"},
{WORKER_FAIL_NOTIFICATION_FOR_FILE, "Worker: received end notification error report for file"},
{CHECKER_CANNOT_RELEASE_TAPE, "VdqmRequestsChecker: cannot release BUSY tape after end transaction"},
{VMGR_GATEWAY_HELPER_RETRYING, "VmgrTapeGatewayHelper::TapeInfo: will retry VMGR query"},
{WORKER_MIG_REPORT_LIST_RECEIVED, "Worker: received a migration report list"},
{WORKER_MIG_REPORT_LIST_PROCESSED, "Worker: finished processing a migration report list"},
{WORKER_REC_REPORT_LIST_RECEIVED, "Worker: received a recall report list"},
{WORKER_REC_REPORT_LIST_PROCESSED, "Worker: finished processing a recall report list"},
{WORKER_MIG_CANNOT_FIND_VID, "Worker: cannot find VID for migration mount"},
{WORKER_MIG_TAPE_RDONLY, "Worker: tape set to readonly after failed fSeq update"},
{WORKER_MIG_CANNOT_RDONLY, "Worker: failed to set tape to readonly after failed fSeq update"},
{WORKER_FILE_ERROR_REPORT_REPL_126, "Worker: Tape server reported a file error"},
{WORKER_FILE_ERROR_REPORT, "Worker: Tape server reported a file error"},
{PRODUCER_NO_TAPE, "VdqmRequestsProducer: no tape retrieved"},
{LINKER_GETTING_MIGRATION_MOUNTS, "TapeMigrationMountLinker: getting migration mounts to resolve"},
{LINKER_NO_MIGRATION_MOUNT, "TapeMigrationMountLinker: no migration mount to resolve"},
{LINKER_QUERYING_VMGR, "TapeMigrationMountLinker: querying vmgr"},
{LINKER_LINKING_TAPE_MOUNT, "TapeMigrationMountLinker: association tape-mount done"},
{LINKER_CANNOT_UPDATE_DB, "TapeMigrationMountLinker: cannot update db"},
{LINKER_RELEASED_BUSY_TAPE, "TapeMigrationMountLinker: released BUSY tape"},
{LINKER_NO_TAPE_AVAILABLE, "TapeMigrationMountLinker: No tape available in such tapepool"},
{LINKER_MOUNTS_FOUND, "TapeMigrationMountLinker: migration mounts found"},
{LINKER_TAPES_ATTACHED, "TapeMigrationMountLinker: tapes attached to migration mounts"},
{MIG_ERROR_GETTING_FILES, "MigratorErrorHandlerThread: getting failed migration job"},
{MIG_ERROR_NO_JOB, "MigratorErrorHandlerThread: no migration job found"},
{MIG_ERROR_JOBS_FOUND, "MigratorErrorHandlerThread: migration jobs found"},
{REC_ERROR_GETTING_FILES, "RecallerErrorHandlerThread: getting failed recall job"},
{REC_ERROR_NO_JOB, "RecallerErrorHandlerThread: no recall job found"},
{REC_ERROR_JOBS_FOUND, "RecallerErrorHandlerThread: recall jobs found"},
{LINKER_NOT_POOL, "TapeMigrationMountLinker: this tapepool does not exist"},
{LINKER_VMGRSETTOREADONLY_FAILED, "TapeMigrationMountLinker: failed to set the tape to read only"},
{LINKER_VMGR_NS_DISCREPANCY, "TapeMigrationMountLinker: NS/VMGR mismatch for highest FSEQ. Putting the tape readonly"},
{WORKER_SUPERFLUOUS_SEGMENT, "Worker: Name server declared segment was extra/unnecessary: considering migration done"},
{WORKER_RECALL_CANNOT_UPDATE_DB, "Worker: cannot update db for recalled file"},
{MOUNT_PRODUCER_START, "MigrationMountProducer: calling the DB procedure"},
{MOUNT_PRODUCER_DB_ERROR, "MigrationMountProducer: failure calling the DB procedure"},
{MOUNT_PRODUCER_REPORT, "MigrationMountProducer: created new migration mount"},
{MOUNT_PRODUCER_REPORT_NO_ACTION, "MigrationMountProducer: no need for new migration mount"},
{LINKER_CANNOT_RELEASE_TAPE, "TapeMigrationMountLinker: cannot release BUSY tape after failed DB update"},
{MIG_ERROR_DB_ERROR, "MigratorErrorHandlerThread: error fetching from the DB"},
{MOUNT_PRODUCER_REPORT_NO_FILE, "MigrationMountProducer: rolled back migration mount creation due to lack of jobs"},
{REC_ERROR_DB_ERROR, "RecallerErrorHandlerThread: error fetching from the DB"},
{WORKER_MIGRATION_LIST_RETRIEVED, "Worker: files to migrate retrieved in bulk from db"},
{PRODUCER_GETTING_TAPES, "VdqmRequestsProducer: getting tapes to submit"},
{PRODUCER_TAPE_FOUND, "VdqmRequestsProducer: found tapes to submit"},
{PRODUCER_REQUEST_SUBMITTED, "VdqmRequestsProducer: request submitted to vdqm successfully"},
{-1, ""}};
