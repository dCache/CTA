/******************************************************************************
 *                 castor/tape/tpcp/FilenameList.hpp
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
 * @author Nicola.Bessone@cern.ch Steven.Murray@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_TPCP_FILENAMELIST_HPP
#define CASTOR_TAPE_TPCP_FILENAMELIST_HPP 1

#include <list>
#include <ostream>
#include <string>

namespace castor {
namespace tape   {
namespace tpcp   {

/**
 * A list of filenames.
 */
class FilenameList : public std::list<std::string> {
};

} // namespace tpcp
} // namespace tape
} // namespace castor


/**
 * ostream << operator for castor::tape::tpcp::FilenameList
 */
std::ostream &operator<<(std::ostream &os,
  const castor::tape::tpcp::FilenameList &list);

#endif // CASTOR_TAPE_TPCP_FILENAMELIST_HPP
