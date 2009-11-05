/******************************************************************************
 *                castor/rtcopy/rechandler/OraRecHandlerSvc.hpp
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
 * @(#)$RCSfile: OraRecHandlerSvc.hpp,v $ $Revision: 1.1 $ $Release$ $Date: 2009/01/19 17:26:39 $ $Author: gtaur $
 *
 * Implementation of the IRecHandlerSvc for Oracle
 *
 * @author Giulia Taurelli
 *****************************************************************************/

#ifndef ORA_ORARECHANDLERSVC_HPP
#define ORA_ORARECHANDLERSVC_HPP 

// Include Files
#include "castor/BaseSvc.hpp"
#include "castor/db/newora/OraCommonSvc.hpp"
#include "castor/infoPolicy/RecallPolicyElement.hpp"
#include "occi.h"
#include <list>
#include <string>
#include "castor/rtcopy/rechandler/IRecHandlerSvc.hpp"

namespace castor {

  namespace rtcopy {
    
    namespace rechandler {
      
      namespace ora {

      /**
       * Implementation of the IRecHandlerSvc for Oracle
       */
      class OraRecHandlerSvc : public castor::db::ora::OraCommonSvc, public virtual castor::rtcopy::rechandler::IRecHandlerSvc {

      public:

        /**
         * default constructor
         */
        OraRecHandlerSvc(const std::string name);

        /**
         * default destructor
         */
        virtual ~OraRecHandlerSvc() throw();

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
	 * inputForRecallPolicy 
	 */

        virtual void  inputForRecallPolicy(std::list<castor::infoPolicy::RecallPolicyElement>& candidates) throw (castor::exception::Exception);

        /**
         * Resurrect Tapes
         */

	virtual void resurrectTapes(const std::list<u_signed64>& eligibleTapeIds) throw (castor::exception::Exception);


      private:

        /// SQL statement for inputForRecallPolicy  
        static const std::string s_inputForRecallPolicyStatementString;

        /// SQL statement object for function inputForRecallPolicy 
	oracle::occi::Statement *m_inputForRecallPolicyStatement;

        /// SQL statement for resurrectTapes
        static const std::string s_resurrectTapesStatementString;

        /// SQL statement object for function resurrectTapes
	oracle::occi::Statement *m_resurrectTapesStatement;
	
      }; // end of class OraRecHandlerSvc

      } // end of namespace ora
    } // end of namespace rechandler
  } // end of namespace rtcopy

} // end of namespace castor

#endif // ORA_ORARECHANDLERSVC_HPP
