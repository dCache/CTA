/* This file was generated by ./AggregatorDlfMessagesCodeGenerator on Fri Nov  7 18:42:23 CET 2008
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
{AGGREGATOR_REQUEST_HANDLER_SOCKET_IS_NULL, "The RequestHandlerThread has been passed a NULL socket pointer"},
{AGGREGATOR_HANDLE_REQUEST_EXCEPT, "Exception raised by castor::tape::aggregator::RequestHandlerThread::handleRequest"},
{AGGREGATOR_FAILED_TO_READ_MAGIC, "Failed to read magic number from socket"},
{-1, ""}};
