/* This file was generated by ./RecHandlerDlfMessagesCodeGenerator on Fri Jan 29 10:32:46 CET 2010
 */

/******************************************************************************
 *             castor/tape/rechandler/RecHandlerDlfMessageConstants.hpp
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

#ifndef CASTOR_TAPE_RECHANDLER_RECHANDLERDLFMESSAGECONSTANTS_HPP
#define CASTOR_TAPE_RECHANDLER_RECHANDLERDLFMESSAGECONSTANTS_HPP 1


namespace castor    {
namespace tape   {
namespace rechandler {
enum RecHandlerDlfMessages {
DAEMON_START=1, /* "Starting RecHandler Daemon done" */
DAEMON_STOP=2, /* "Stopped  RecHandler Daemon done" */
NO_POLICY=3, /* "Wrong policy configuration" */
FATAL_ERROR=4, /* "Fatal Error" */
TAPE_NOT_FOUND=5, /* "No tape found" */
TAPE_FOUND=6, /* "Tape found" */
POLICY_INPUT=7, /* "Input given to the policy" */
ALLOWED_WITHOUT_POLICY=8, /* "Recall allowed without policy" */
ALLOWED_BY_POLICY=9, /* "Recall allowed by policy/sending vdqm priority" */
PRIORITY_SENT=10, /* "Priority sent to vdqm" */
NOT_ALLOWED=11, /* "Recall not allowed" */
PYTHON_ERROR=12, /* "Python error" */
RESURRECT_TAPES=13, /* "Result save into db" */
RECHANDLER_FAILED_TO_START=14 /* "RecHandler daemon failed to start" */
}; // enum RecHandlerDlfMessages
} // namespace rechandler
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_RECHANDLER_RECHANDLERDLFMESSAGECONSTANTS_HPP
