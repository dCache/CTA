/******************************************************************************
 *                castor/tape/mighunter/ora/OraMigHunterSvc.hpp
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
 * @(#)$RCSfile: OraMigHunterSvc.hpp,v $ $Revision: 1.1 $ $Release$ $Date: 2009/01/19 17:26:15 $ $Author: gtaur $
 *
 * Implementation of the IMigHunterSvc for Oracle
 *
 * @author Giulia Taurelli
 *****************************************************************************/

#ifndef ORA_ORAMIGHUNTERSVC_HPP
#define ORA_ORAMIGHUNTERSVC_HPP 

#include "occi.h"

#include "castor/BaseSvc.hpp"

#include "castor/db/newora/OraCommonSvc.hpp"

#include "castor/tape/mighunter/IMigHunterSvc.hpp"
#include "castor/tape/mighunter/MigrationPolicyElement.hpp"
#include "castor/tape/mighunter/StreamPolicyElement.hpp"

#include <list>
#include <string>

namespace castor    {
namespace tape      {
namespace mighunter {
namespace ora       {

/**
 * Implementation of the IPolicySvc for Oracle
 */
class OraMigHunterSvc : public castor::db::ora::OraCommonSvc, public virtual castor::tape::mighunter::IMigHunterSvc {

public:

  /**
   * default constructor
   */
  OraMigHunterSvc(const std::string name);

  /**
   * default destructor
   */
  virtual ~OraMigHunterSvc() throw();

  /**
   * Get the service id
   */
  virtual inline unsigned int id() const;

  /**
   * Get the service id
   */
  static unsigned int ID();

  /**
   * Reset the converter statements.
   */
  void reset() throw();
	
  /**
   * Gets the tape-copies to be attached to streams for the specified service
   * class.  After this method returns, each tape-copy to be attached will have
   * its status set and commited in the database to TAPECOPY_WAITPOLICY (tragic
   * number 7).
   *
   * Besides possibly throwing other types of castor exception, this method
   * throws an InvalidConfiguration exception when the specified service class
   * has no tapes-pools.  In this particular case the tape-copies to be
   * attached will be left in the commited status TAPECOPY_WAITPOLICY (tragic
   * number 7).  A restart of the migration hunter daemon will cause it to call
   * the migHunterCleanUp PL/SQL procedure which in turn will resurrect these
   * zombie tape-copies by setting their status to TAPECOPY_TOBEMIGRATED
   * (tragic number 1).
   */
  virtual void inputForMigrationPolicy(std::string svcClassName,
    u_signed64* byteThres,
    std::list<castor::tape::mighunter::MigrationPolicyElement>& candidates)
    throw(castor::exception::Exception);
              
  /**
   * createOrUpdateStream 
   */ 
  virtual int createOrUpdateStream(std::string svcClassId,
    u_signed64 initialSizeToTransfer, u_signed64 volumeThreashold,
    u_signed64 initialSizeCeiling, bool doClone, int tapeCopyNb)
    throw(castor::exception::Exception);

  /**
   * See documentation for castor::tape::mighunter::IMigHunterSvc.
   */
  virtual void getCreatedStreamsWithTapeCopies(
    const std::string     &svcClassName,
    std::list<u_signed64> &streamIds)
    throw(castor::exception::Exception);

  /**
   * See documentation for castor::tape::mighunter::IMigHunterSvc.
   */
  virtual void changeCreatedStreamToStopped(
    const u_signed64 streamId)
    throw(castor::exception::Exception);

  /**
   * See documentation for castor::tape::mighunter::IMigHunterSvc.
   */
  virtual void streamsForStreamPolicy(
    const std::string         &svcClassName,
    u_signed64                &svcClassId,
    std::string               &streamPolicyName,
    u_signed64                &nbDrives,
    StreamForStreamPolicyList &streamsForPolicy)
    throw (castor::exception::Exception);

  /**
   * See documentation for castor::tape::mighunter::IMigHunterSvc.
   */
  virtual void tapePoolsForStreamPolicy(
    const u_signed64               svcClassId,
    IdToTapePoolForStreamPolicyMap &tapePoolsForPolicy)
    throw (castor::exception::Exception);

  /**
   * startChosenStreams 
   */
  virtual void startChosenStreams(
    const std::list<castor::tape::mighunter::StreamPolicyElement>&
    outputFromStreamPolicy) throw (castor::exception::Exception);

  /**
   * See documentation for castor::tape::mighunter::IMigHunterSvc.
   */
  virtual void startChosenStreams(const std::list<u_signed64> &streamIds)
    throw (castor::exception::Exception);

  /**
   * stopChosenStreams 
   */
  virtual void stopChosenStreams(
    const std::list<castor::tape::mighunter::StreamPolicyElement>&
    outputFromStreamPolicy) throw (castor::exception::Exception);

  /**
   * See documentation for castor::tape::mighunter::IMigHunterSvc.
   */
  virtual void stopChosenStreams(const std::list<u_signed64> &streamIds)
    throw (castor::exception::Exception);

  /** 
   * Attach TapeCopies To Streams
   */
  virtual void  attachTapeCopiesToStreams(
    const std::list<castor::tape::mighunter::MigrationPolicyElement>&
    outputFromMigrationPolicy) throw (castor::exception::Exception);

  /**
   * resurrect tape copies 
   */
  virtual void resurrectTapeCopies(
    const std::list<castor::tape::mighunter::MigrationPolicyElement>&
    tapeCopiesInfo) throw (castor::exception::Exception);

  /** 
   * invalidate tapecopies
   */
  virtual void invalidateTapeCopies(
    const std::list<castor::tape::mighunter::MigrationPolicyElement>&
    tapeCopiesInfo) throw (castor::exception::Exception);

  /** 
   * migHunterCleanUp
   */
  virtual void migHunterCleanUp(std::string svcClassName)
    throw (castor::exception::Exception);

private:

  /// SQL statement for inputForMigrationPolicy 
  static const std::string s_inputForMigrationPolicyStatementString;

  /// SQL statement object for function inputForMigrationPolicy
  oracle::occi::Statement *m_inputForMigrationPolicyStatement;
          
  /// SQL statement for createOrUpdateStream
  static const std::string s_createOrUpdateStreamStatementString;

  /// SQL statement object for function createOrUpdateStream
  oracle::occi::Statement *m_createOrUpdateStreamStatement;

  /// SQL statement for function getCreatedStreamsWithTapeCopies 
  static const std::string s_getCreatedStreamsWithTapeCopiesString;

  /// SQL statement object for function getCreatedStreamsWithTapeCopies
  oracle::occi::Statement *m_getCreatedStreamsWithTapeCopiesStatement;

  /// SQL statement for function changeCreatedStreamToStopped 
  static const std::string s_changeCreatedStreamToStoppedString;

  /// SQL statement object for function changeCreatedStreamToStopped
  oracle::occi::Statement *m_changeCreatedStreamToStoppedStatement;

  /// SQL statement for function streamsForStreamPolicy 
  static const std::string s_streamsForStreamPolicyStatementString;

  /// SQL statement object for function streamsForStreamPolicy
  oracle::occi::Statement *m_streamsForStreamPolicyStatement;

  /// SQL statement for function tapePoolsForStreamPolicy 
  static const std::string s_tapePoolsForStreamPolicyStatementString;

  /// SQL statement object for function tapePoolsForStreamPolicy
  oracle::occi::Statement *m_tapePoolsForStreamPolicyStatement;
       
  /// SQL statement for startChosenStreams
  static const std::string s_startChosenStreamsStatementString;

  /// SQL statement object for function startChosenStreams 
  oracle::occi::Statement *m_startChosenStreamsStatement;

  /// SQL statement for stopChosenStreams
  static const std::string s_stopChosenStreamsStatementString;

  /// SQL statement object for function stopChosenStreams 
  oracle::occi::Statement *m_stopChosenStreamsStatement;

  /// SQL statement resurrect candidates
  static const std::string s_resurrectCandidatesStatementString;

  /// SQL statement object for resurrectCandidatesNameStatementString
  oracle::occi::Statement *m_resurrectCandidatesStatement;

  /// SQL statement invalidate tapecopies
  static const std::string s_invalidateTapeCopiesStatementString;

  /// SQL statement object for invalidateTapeCopiesStatementString
  oracle::occi::Statement *m_invalidateTapeCopiesStatement;

  /// SQL statement select TapepoolsName
  static const std::string s_selectTapePoolNamesStatementString;

  /// SQL statement object for TapePoolsNameStatementString
  oracle::occi::Statement *m_selectTapePoolNamesStatement;

  /// SQL statement attach tape copies to streams
  static const std::string s_attachTapeCopiesToStreamsStatementString;

  /// SQL statement object attach tape copies to Stream
  oracle::occi::Statement *m_attachTapeCopiesToStreamsStatement;
	
  /// SQL statement migHunterCleanUp
  static const std::string s_migHunterCleanUpStatementString;

  /// SQL statement object clean up the db at the startup
  oracle::occi::Statement *m_migHunterCleanUpStatement;
}; // class OraMigHunterSvc
      
} // namespace ora
} // namespace mighunter
} // namespace tape
} // namespace castor

#endif // ORA_ORAMIGHUNTERSVC_HPP
