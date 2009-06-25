/******************************************************************************
 *                 castor/tape/tpcp/TapeFseqSequence.hpp
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

#ifndef CASTOR_TAPE_TPCP_TAPEFSEQRANGELISTSEQUENCE_HPP
#define CASTOR_TAPE_TPCP_TAPEFSEQRANGELISTSEQUENCE_HPP 1

#include "castor/exception/Exception.hpp"
#include "castor/tape/tpcp/TapeFseqRangeList.hpp"
#include "castor/tape/tpcp/TapeFseqRangeSequence.hpp"

namespace castor {
namespace tape   {
namespace tpcp   {

/**
 * Generates a sequence of tape file sequence numbers from a list of tape file
 * sequence ranges.
 */
class TapeFseqRangeListSequence {
public:

  /**
   * Constructor.
   *
   * @param list The list of tape file sequence ranges from which the sequence
   * of tape file sequence numbers is to be generated.
   */
  TapeFseqRangeListSequence(TapeFseqRangeList &list)
    throw(castor::exception::Exception);

  /**
   * Returns true if there is another tape file sequence number in the
   * sequence.
   */
  bool hasMore() throw();

  /**
   * Returns the next  tape file sequence number in the sequence, or throws an
   * exception if there isn't one.
   */
  uint32_t next() throw(castor::exception::Exception);


private:

  /**
   * The list of tape file sequence ranges.
   */
  TapeFseqRangeList &m_list;

  /**
   * Iterator pointing to the current range of tape file sequence numbers.
   */
  TapeFseqRangeList::iterator m_rangeItor;

  /**
   * The current sequence of the tape file sequence numbers.
   */
  TapeFseqRangeSequence m_nbSequence;

}; // class TapeFseqRangeListSequence

} // namespace tpcp
} // namespace tape
} // namespace castor


#endif // CASTOR_TAPE_TPCP_TAPEFSEQRANGELISTSEQUENCE_HPP
