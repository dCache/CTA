/*
 * @project      The CERN Tape Archive (CTA)
 * @copyright    Copyright © 2022 CERN
 * @license      This program is free software, distributed under the terms of the GNU General Public
 *               Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING". You can
 *               redistribute it and/or modify it under the terms of the GPL Version 3, or (at your
 *               option) any later version.
 *
 *               This program is distributed in the hope that it will be useful, but WITHOUT ANY
 *               WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 *               PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 *               In applying this licence, CERN does not waive the privileges and immunities
 *               granted to it by virtue of its status as an Intergovernmental Organization or
 *               submit itself to any jurisdiction.
 */

#include "RetrieveJob.hpp"
#include "common/exception/Exception.hpp"

namespace cta {

PostgresSchedDB::RetrieveJob::RetrieveJob()
{
   throw cta::exception::Exception("Not implemented");
}

void PostgresSchedDB::RetrieveJob::asyncSetSuccessful()
{
   throw cta::exception::Exception("Not implemented");
}

void PostgresSchedDB::RetrieveJob::failTransfer(const std::string &failureReason, log::LogContext &lc)
{
   throw cta::exception::Exception("Not implemented");
}

void PostgresSchedDB::RetrieveJob::failReport(const std::string &failureReason, log::LogContext &lc)
{
   throw cta::exception::Exception("Not implemented");
}

void PostgresSchedDB::RetrieveJob::abort(const std::string &abortReason, log::LogContext &lc)
{
   throw cta::exception::Exception("Not implemented");
}

void PostgresSchedDB::RetrieveJob::fail()
{
   throw cta::exception::Exception("Not implemented");
}

} //namespace cta
