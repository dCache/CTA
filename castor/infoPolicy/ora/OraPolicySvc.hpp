/******************************************************************************
 *                castor/db/ora/OraPolicySvc.hpp
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
 * @(#)$RCSfile: OraPolicySvc.hpp,v $ $Revision: 1.2 $ $Release$ $Date: 2007/11/14 16:53:31 $ $Author: gtaur $
 *
 * Implementation of the ITapeSvc for Oracle
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef ORA_ORAPOLICYSVC_HPP
#define ORA_ORAPOLICYSVC_HPP 

// Include Files
#include "castor/BaseSvc.hpp"
#include "castor/db/newora/OraCommonSvc.hpp"
#include "castor/infoPolicy/PolicyObj.hpp"
#include "occi.h"
#include <vector>
#include <string>
#include "castor/infoPolicy/IPolicySvc.hpp"

namespace castor {

  namespace infoPolicy {

    namespace ora {

      /**
       * Implementation of the IPolicySvc for Oracle
       */
      class OraPolicySvc : public castor::db::ora::OraCommonSvc, public virtual castor::infoPolicy::IPolicySvc {

      public:

        /**
         * default constructor
         */
        OraPolicySvc(const std::string name);

        /**
         * default destructor
         */
        virtual ~OraPolicySvc() throw();

        /**
         * Get the service id
         */
        virtual inline const unsigned int id() const;

        /**
         * Get the service id
         */
        static const unsigned int ID();

        /**
         * Reset the converter statements.
         */
        void reset() throw ();

      public:
	
         /**
         *  inputForMigrationPolicy
         */

	virtual std::vector<castor::infoPolicy::PolicyObj*> castor::infoPolicy::ora::OraPolicySvc::inputForMigrationPolicy(std::string  svcClassName, u_signed64* byteThres) throw (castor::exception::Exception); 
              
         /**
          * createOrUpdateStream 
          */ 

	virtual int createOrUpdateStream(std::string svcClassId, u_signed64 initialSizeToTransfer, u_signed64 volumeThreashold, u_signed64 initialSizeCeiling,bool doClone, std::vector<PolicyObj*> tapeCopyIds) throw (castor::exception::Exception);

         /**
         * inputForStreamPolicy,
         */

        virtual std::vector<castor::infoPolicy::PolicyObj*> inputForStreamPolicy(std::string svcClassName)throw (castor::exception::Exception);

         /**
         * startChosenStreams 
         */

	virtual void startChosenStreams(std::vector<PolicyObj*> outputFromStreamPolicy,u_signed64 initialSize) throw (castor::exception::Exception);


	/**                    
	 * inputForRecallPolicy 
	 */

        virtual std::vector<castor::infoPolicy::PolicyObj*>  inputForRecallPolicy() throw (castor::exception::Exception);

        /**
         * Resurrect Tapes
         */

	virtual void resurrectTapes(std::vector<u_signed64> eligibleTapeIds) throw (castor::exception::Exception);

	/** 
	 * Attach TapeCopies To Streams
	 */
	
	virtual void  castor::infoPolicy::ora::OraPolicySvc::attachTapeCopiesToStreams(std::vector<PolicyObj*> outputFromMigrationPolicy) throw (castor::exception::Exception);
	
	/**
	 * resurrect tape copies 
	 */

	virtual void   castor::infoPolicy::ora::OraPolicySvc::resurrectTapeCopies(std::vector<PolicyObj*> tapeCopiesInfo) throw (castor::exception::Exception);

      private:

        /// SQL statement for inputForMigrationPolicy 
        static const std::string s_inputForMigrationPolicyStatementString;

        /// SQL statement object for function inputForMigrationPolicy
	oracle::occi::Statement *m_inputForMigrationPolicyStatement;
          
         /// SQL statement for createOrUpdateStream
        static const std::string s_createOrUpdateStreamStatementString;

         /// SQL statement object for function createOrUpdateStream
        oracle::occi::Statement *m_createOrUpdateStreamStatement;

        /// SQL statement for inputForStreamPolicy 
        static const std::string s_inputForStreamPolicyStatementString;

        /// SQL statement object for function inputForStreamPolicy
	oracle::occi::Statement *m_inputForStreamPolicyStatement;
       
        /// SQL statement for startChosenStreams
        static const std::string s_startChosenStreamsStatementString;

        /// SQL statement object for function startChosenStreams 
	oracle::occi::Statement *m_startChosenStreamsStatement;

        /// SQL statement for inputForRecallPolicy  
        static const std::string s_inputForRecallPolicyStatementString;

        /// SQL statement object for function inputForRecallPolicy 
	oracle::occi::Statement *m_inputForRecallPolicyStatement;

        /// SQL statement for resurrectTapes
        static const std::string s_resurrectTapesStatementString;

        /// SQL statement object for function resurrectTapes
	oracle::occi::Statement *m_resurrectTapesStatement;

        /// SQL statement select TapepoolsName
        static const std::string s_selectTapePoolNamesStatementString;

        /// SQL statement object for TapePoolsNameStatementString
	oracle::occi::Statement *m_selectTapePoolNamesStatement;

        /// SQL statement resurrect candidates
        static const std::string s_resurrectCandidatesStatementString;

        /// SQL statement object for resurrectCandidatesNameStatementString
	oracle::occi::Statement *m_resurrectCandidatesStatement;

        /// SQL statement attach tape copies to streams
        static const std::string s_attachTapeCopiesToStreamsStatementString;

        /// SQL statement object attach tape copies to Stream
	oracle::occi::Statement *m_attachTapeCopiesToStreamsStatement;
	
      }; // end of class OraPolicySvc

    } // end of namespace ora

  } // end of namespace db

} // end of namespace castor

#endif // ORA_ORAPOLICYSVC_HPP
