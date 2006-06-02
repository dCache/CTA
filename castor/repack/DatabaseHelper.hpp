
/******************************************************************************
 *                      castor/repack/DatabaseHelper.hpp
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
 * @(#)$RCSfile: DatabaseHelper.hpp,v $ $Revision: 1.11 $ $Release$ $Date: 2006/06/02 08:16:52 $ $Author: felixehm $
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_REPACK_DATABASEHELPER_HPP
#define CASTOR_REPACK_DATABASEHELPER_HPP

// Include Files
#include "castor/repack/RepackCommonHeader.hpp"
#include <vector>
#include "castor/BaseAddress.hpp"
#include "castor/Services.hpp"
#include "castor/db/DbBaseObj.hpp"
#include "castor/db/DbCnvSvc.hpp"
#include "castor/exception/SQLError.hpp"
#include "stager_client_api.h"


namespace castor {
	
  namespace repack {

    // Forward declarations
    class RepackRequest;
    class IObject;
	  class Services;
    

    /**
     * class DatabaseHelper
     * 
     */
    class DatabaseHelper  : public castor::db::DbBaseObj{

    public:

      /**
       * Empty Constructor
       */
      DatabaseHelper() ;

      /**
       * Empty Destructor
       */
      virtual ~DatabaseHelper() throw() ;

      /**
       * Stores a RepackRequest in the Database
       * @param rreq The RepackRequest
       */
      void storeRequest(castor::repack::RepackRequest* rreq) 
					throw(castor::exception::Internal);


      /**
	   * Selects the next request the Repack daemon should deal with.
	   * It returns a RepackSubRequest, so directly the tape to repack.
	   * The corresponding main Request can be fetched by TODO:: <other function>>
 	   * TODO: select statement
	   * 
	   * @return the Request to process
	   * @exception Exception in case of error
	   */
	  castor::repack::RepackSubRequest* requestToDo() 
					throw(castor::exception::Internal);



	  

	  /**
     * Resets the converter. In particular any prepared
     * statements are destroyed.
     */
    virtual void reset() throw();

    /**
     * updates the RepackSubRequest. For logging purpose the cuuid is given as
     * parameter.
     * @param obj The RepackSubRequest, which is to be updated
     * @param cuuid The Cuuid for following the dlf messages
     * @throws castor::exception::Internal in case of an error
     */
    void updateSubRequest(castor::repack::RepackSubRequest* obj, bool fill,Cuuid_t& cuuid)
              throw(castor::exception::Internal);

	  /**
	   * Checks the RepackDB for SubRequests in a certain status.
     * The returned Object is filled (all segments and the corresponding 
     * RepackRequest This means that the caller has to free the 
     * allocated memory.
     * @param status The status to be queried
     * @return RepackSubRequest a full RepackSubRequest Object
     * @throws castor::exception::Internal in case of an error
	   */
	  RepackSubRequest* checkSubRequestStatus(int status) 
	  					throw(castor::exception::Internal);
	  
    /**
     * Removes an RepackRequest,RepackSubRequest or even RepackSegment from DB
     * @param obj The object to be removed
     * @throws castor::exception::Internal in case of an error
     */
	  void remove(castor::IObject* obj) throw(castor::exception::Internal) ;


    /**
     * Retrieves and RepackSbu Request from DB by a given volume id
     * @param the volume name
     * @throws castor::exception::Internal in case of an error
     */
    RepackSubRequest* getSubRequestByVid(std::string vid) 
	  					throw(castor::exception::Internal);
    
    /**
     * Checks,wether a Tape is already stored in the RepackSubRequest Table.
     * This is needed before a Tape is inserted as a new repackjob.
     * @throws castor::exception::Internal in case of an error
     */
    bool is_stored(std::string vid) throw(castor::exception::Internal);

    /**
     * Gets all RepackSubRequests from the DB
     * @return an pointer to a vector of Repack SubRequests
     * @throws castor::exception::Internal in case of an error
     */
     std::vector<castor::repack::RepackSubRequest*>* 
	                   getAllSubRequests() throw (castor::exception::Internal);

     /**
     * Gets all RepackSubRequests in a certain status from the DB
     * @return an pointer to a vector of Repack SubRequests
     * @throws castor::exception::Internal in case of an error
     */
     std::vector<castor::repack::RepackSubRequest*>* 
                     getAllSubRequestsStatus(int status)
                                          throw (castor::exception::Internal);

     /**
      * Returns the already existing RepackSegment in the DB.
      * Note that even a file is multi segmented the file is being
      * repacked only for one segment.
      */
     RepackSegment* DatabaseHelper::getTapeCopy(RepackSegment* lookup)
                    throw (castor::exception::Internal);
    

     /** Archives the finished RepackSubRequests.
      * It just updates the DB for Requests in SUBREQUEST_DONE
      * to SUBREQUEST_ARCHIVED.
      * @throw castor::exception::Internal in case of an error
      */
     void archive() throw (castor::exception::Internal);
     


      private:
      
        /**
         * Little Helper to get a get a SubRequest from DB by the rowid.
         * @param sub_id the id of the RepackSubRequest
         * @throws castor::exception::Internal in case of an error
         */
        RepackSubRequest* DatabaseHelper::getSubRequest(u_signed64 sub_id) 
						throw(castor::exception::Internal);
      
       /**
        * Gets a bunch of RepackSubRequest from DB. This method is only used
        * internally.
        * @param castor::db::IDbStatement The Statement to get the SubRequests
        * @throw castor::exception::Internal in case of an error
        * @return pointer to vector of RepackSubRequests
        */
        std::vector<RepackSubRequest*>* DatabaseHelper::internalgetSubRequests
                                              (castor::db::IDbStatement* statement) 
                                              throw (castor::exception::Internal);



        static const std::string s_selectCheckStatementString;
        castor::db::IDbStatement *m_selectCheckStatement;
        
        static const std::string s_selectCheckSubRequestStatementString;
        castor::db::IDbStatement *m_selectCheckSubRequestStatement;
          
        static const std::string s_selectAllSubRequestsStatementString;
        castor::db::IDbStatement *m_selectAllSubRequestsStatement;

        static const std::string s_selectExistingSegmentsStatementString;
        castor::db::IDbStatement *m_selectExistingSegmentsStatement;

        static const std::string s_isStoredStatementString;
        castor::db::IDbStatement *m_isStoredStatement;
	
        static const std::string s_selectAllSubRequestsStatusStatementString;
        castor::db::IDbStatement *m_selectAllSubRequestsStatusStatement;

        static const std::string s_archiveStatementString;
        castor::db::IDbStatement  *m_archiveStatement;
        //oracle::occi::Statement  *m_archiveStatement;
        castor::BaseAddress ad;
      
    }; // end of class DatabaseHelper

  }; // end of namespace repack

}; // end of namespace castor

#endif // CASTOR_REPACK_DATABASEHELPER_HPP
