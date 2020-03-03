/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "statistics/MysqlStatisticsSchema.hpp"

namespace cta {
namespace statistics {

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
MysqlStatisticsSchema::MysqlStatisticsSchema(): StatisticsSchema(
  // CTA_SQL_SCHEMA - The contents of mysql_statistics_schema.cpp go here
  ) {
}

} // namespace statistics
} // namespace cta