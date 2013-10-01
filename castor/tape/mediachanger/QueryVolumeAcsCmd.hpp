/******************************************************************************
 *                 castor/tape/mediachanger/QueryVolumeAcsCmd.hpp
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
 * @author Steven.Murray@cern.ch
 *****************************************************************************/

#ifndef CASTOR_TAPE_MEDIACHANGER_QUERYVOLUMEACSCMD_HPP
#define CASTOR_TAPE_MEDIACHANGER_QUERYVOLUMEACSCMD_HPP 1

#include "castor/exception/Internal.hpp"
#include "castor/exception/InvalidArgument.hpp"
#include "castor/exception/MissingOperand.hpp"
#include "castor/exception/QueryVolumeFailed.hpp"
#include "castor/tape/mediachanger/AcsCmd.hpp"
#include "castor/tape/mediachanger/QueryVolumeAcsCmdLine.hpp"

#include <stdint.h>

namespace castor  {
namespace tape    {
namespace mediachanger {

/**
 * The class implementing the mount command.
 */
class QueryVolumeAcsCmd: public AcsCmd {
public:

  /**
   * Constructor.
   *
   * @param inStream Standard input stream.
   * @param outStream Standard output stream.
   * @param errStream Standard error stream.
   * @param acs Wrapper around the ACSLS C-API.
   */
  QueryVolumeAcsCmd(std::istream &inStream, std::ostream &outStream,
    std::ostream &errStream, Acs &acs) throw();

  /**
   * Destructor.
   */
  virtual ~QueryVolumeAcsCmd() throw();

  /**
   * The entry function of the command.
   *
   * @param argc The number of command-line arguments.
   * @param argv The command-line arguments.
   */
  int main(const int argc, char *const *const argv) throw();

protected:

  /**
   * Parses the specified command-line arguments.
   *
   * @param argc Argument count from the executable's entry function: main().
   * @param argv Argument vector from the executable's entry function: main().
   * @return The parsed command-line.
   */
  QueryVolumeAcsCmdLine parseCmdLine(const int argc, char *const *const argv)
    throw(castor::exception::Internal, castor::exception::InvalidArgument,
      castor::exception::MissingOperand);

  /**
   * Writes the command-line usage message of to the specified output stream.
   *
   * @param os Output stream to be written to.
   */
  void usage(std::ostream &os) const throw();

  /**
   * Queries ACS for information about the volume identifier specified on the
   * command-line.
   *
   * This method does not return until the information has been successfully
   * retrieved, an error has occurred or the specified timeout has been
   * reached.
   *
   * @return The volume status of the volume identifier specified on the
   * command-line.
   */
  QU_VOL_STATUS syncQueryVolume() throw(castor::exception::QueryVolumeFailed);

  /**
   * Sends the query volume  request to ACSLS.
   *
   * @param seqNumber The sequence number to be used in the request.
   */
  void sendQueryVolumeRequest(const SEQ_NO seqNumber)
    throw (castor::exception::QueryVolumeFailed);

  /**
   * Sends a request for a response to the ACSLS.
   *
   * @param timeout The timeout.
   * @param requestSeqNumber The sequemce number that was sent in the initial
   * request to the ACSLS.
   * @param buf Output parameter.  The response message if there is one.
   * @return The type of the response message if there is one or RT_NONE if
   * there isn't one.
   */
  ACS_RESPONSE_TYPE requestResponse(const int timeout,
    const SEQ_NO requestSeqNumber,
    ALIGNED_BYTES (&buf)[MAX_MESSAGE_SIZE / sizeof(ALIGNED_BYTES)])
    throw(castor::exception::QueryVolumeFailed);

  /**
   * Throws castor::exception::QueryVolumeFailed if the specified request and
   * response sequence-numbers do not match.
   *
   * @param requestSeqNumber Request sequence-number.
   * @param responseSeqNumber Response sequence-number.
   */
  void checkResponseSeqNumber(const SEQ_NO requestSeqNumber,
    const SEQ_NO responseSeqNumber) throw(castor::exception::QueryVolumeFailed);

  /**
   * Extracts the volume status from the specified query-response message.
   *
   * @param buf The query-response message.
   */
  QU_VOL_STATUS processQueryResponse(
    ALIGNED_BYTES (&buf)[MAX_MESSAGE_SIZE / sizeof(ALIGNED_BYTES)])
    throw(castor::exception::QueryVolumeFailed);

  /**
   * Writes a human readable representation of the specified volume status to
   * the specified output stream.
   *
   * @param os The output stream.
   * @param s The volume status.
   */
  void writeVolumeStatus(std::ostream &os, const QU_VOL_STATUS &s) throw();

private:

  /**
   * The parsed command-line.
   *
   * The value of this member variable is set within the main() method of this
   * class.
   */
  QueryVolumeAcsCmdLine m_cmdLine;

  /**
   * The default time in seconds to wait between queries to ACS for responses.
   */
  const int m_defaultQueryInterval;

  /**
   * The default timeout value in seconds for the query to conclude either
   * success or failure.
   */
  const int m_defaultTimeout;

}; // class QueryVolumeAcsCmd

} // namespace mediachanger
} // namespace tape
} // namespace castor

#endif // CASTOR_TAPE_MEDIACHANGER_QUERYVOLUMEACSCMD_HPP
