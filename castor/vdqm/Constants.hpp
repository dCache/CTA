/******************************************************************************
 *                      Constants.hpp
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
 * @author castor dev team
 *****************************************************************************/

#ifndef CASTOR_VDQM_CONSTANTS_HPP
#define CASTOR_VDQM_CONSTANTS_HPP 1


namespace castor { namespace vdqm {
  /**
   * The schema version with which this release of the VDQM is compatible with.
   */
  const std::string VDQMSCHEMAVERSION = "2_1_8_3";

  /**
   * Default number of request handler threads.
   */
  const int REQUESTHANDLERDEFAULTTHREADNUMBER = 20;

  /**
   * Default number of remote tape copy job submitter threads.
   */
  const int RTCPJOBSUBMITTERDEFAULTTHREADNUMBER = 5;

  /**
   * Default number of scheduler threads.
   */
  const int SCHEDULERDEFAULTTHREADNUMBER = 1;
} } // namespace vdqm - namespace castor


#endif // CASTOR_VDQM_CONSTANTS_HPP
