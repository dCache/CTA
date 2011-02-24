/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbTapeRequestCnv.hpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_DB_CNV_TAPEREQUEST_HPP
#define CASTOR_DB_CNV_TAPEREQUEST_HPP

// Include Files
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/db/cnv/DbBaseCnv.hpp"
#include "castor/exception/Exception.hpp"
#include <vector>

namespace castor {

  // Forward declarations
  class IObject;
  class ICnvSvc;

  // Forward declarations
  namespace vdqm {

    // Forward declarations
    class TapeRequest;

  } /* end of namespace vdqm */

  namespace db {

    namespace cnv {

      /**
       * class DbTapeRequestCnv
       * A converter for storing/retrieving TapeRequest into/from a generic database
       */
      class DbTapeRequestCnv : public DbBaseCnv {

      public:

        /**
         * Constructor
         */
        DbTapeRequestCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~DbTapeRequestCnv() throw();

        /**
         * Gets the object type.
         * That is the type of object this converter can convert
         */
        static unsigned int ObjType();

        /**
         * Gets the object type.
         * That is the type of object this converter can convert
         */
        virtual unsigned int objType() const;

        /**
         * Creates foreign representation from a C++ Object.
         * @param address where to store the representation of
         * the object
         * @param object the object to deal with
         * @param endTransaction whether the changes to the database
         * should be commited or not
         * @param type if not OBJ_INVALID, the ids representing
         * the links to objects of this type will not set to 0
         * as is the default.
         * @exception Exception throws an Exception in case of error
         */
        virtual void createRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool endTransaction,
                               unsigned int type = castor::OBJ_INVALID)
          throw (castor::exception::Exception);

        /**
         * Creates foreign representation from a set of C++ Objects.
         * @param address where to store the representation of
         * the objects
         * @param objects the list of objects to deal with
         * @param endTransaction whether the changes to the database
         * should be commited or not
         * @param type if not OBJ_INVALID, the ids representing
         * the links to objects of this type will not set to 0
         * as is the default.
         * @exception Exception throws an Exception in case of error
         */
        virtual void bulkCreateRep(castor::IAddress* address,
                               std::vector<castor::IObject*> &objects,
                               bool endTransaction,
                               unsigned int type = castor::OBJ_INVALID)
          throw (castor::exception::Exception);

        /**
         * Updates foreign representation from a C++ Object.
         * @param address where the representation of
         * the object is stored
         * @param object the object to deal with
         * @param endTransaction whether the changes to the database
         * should be commited or not
         * @exception Exception throws an Exception in case of error
         */
        virtual void updateRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool endTransaction)
          throw (castor::exception::Exception);

        /**
         * Deletes foreign representation of a C++ Object.
         * @param address where the representation of
         * the object is stored
         * @param object the object to deal with
         * @param endTransaction whether the changes to the database
         * should be commited or not
         * @exception Exception throws an Exception in case of error
         */
        virtual void deleteRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool endTransaction)
          throw (castor::exception::Exception);

        /**
         * Creates C++ object from foreign representation
         * @param address the place where to find the foreign
         * representation
         * @return the C++ object created from its reprensentation
         * or 0 if unsuccessful. Note that the caller is responsible
         * for the deallocation of the newly created object
         * @exception Exception throws an Exception in case of error
         */
        virtual castor::IObject* createObj(castor::IAddress* address)
          throw (castor::exception::Exception);

        /**
         * create C++ objects from foreign representations
         * @param address the place where to find the foreign
         * representations
         * @return the C++ objects created from the representations
         * or empty vector if unsuccessful. Note that the caller is
         * responsible for the deallocation of the newly created objects
         * @exception Exception throws an Exception in case of error
         */
        virtual std::vector<castor::IObject*> bulkCreateObj(castor::IAddress* address)
          throw (castor::exception::Exception);

        /**
         * Updates C++ object from its foreign representation.
         * @param obj the object to deal with
         * @exception Exception throws an Exception in case of error
         */
        virtual void updateObj(castor::IObject* obj)
          throw (castor::exception::Exception);

        /**
         * Reset the converter statements.
         */
        void reset() throw ();

        /**
         * Fill the foreign representation with some of the objects.refered by a given C++
         * object.
         * @param address the place where to find the foreign representation
         * @param object the original C++ object
         * @param type the type of the refered objects to store
         * @param endTransaction whether the changes to the database
         * should be commited or not
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRep(castor::IAddress* address,
                             castor::IObject* object,
                             unsigned int type,
                             bool endTransaction)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type VdqmTape refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepVdqmTape(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type TapeAccessSpecification refered by a
         * given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeAccessSpecification(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type TapeServer refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeServer(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type TapeDrive refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeDrive(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepDeviceGroupName(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type ClientIdentification refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepClientIdentification(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database some of the objects refered by a given object.
         * @param object the original object
         * @param type the type of the refered objects to retrieve
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObj(castor::IAddress* address,
                             castor::IObject* object,
                             unsigned int type,
                             bool endTransaction)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type VdqmTape refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjVdqmTape(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeAccessSpecification refered by a
         * given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeAccessSpecification(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeServer refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeServer(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeDrive refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeDrive(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjDeviceGroupName(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type ClientIdentification refered by a
         * given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjClientIdentification(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception);

      private:

        /// SQL statement for request insertion
        static const std::string s_insertStatementString;

        /// SQL statement object for request insertion
        castor::db::IDbStatement *m_insertStatement;

        /// SQL statement for request bulk insertion
        static const std::string s_bulkInsertStatementString;

        /// SQL statement object for request bulk insertion
        castor::db::IDbStatement *m_bulkInsertStatement;

        /// SQL statement for request deletion
        static const std::string s_deleteStatementString;

        /// SQL statement object for request deletion
        castor::db::IDbStatement *m_deleteStatement;

        /// SQL statement for request selection
        static const std::string s_selectStatementString;

        /// SQL statement object for request selection
        castor::db::IDbStatement *m_selectStatement;

        /// SQL statement for request bulk selection
        static const std::string s_bulkSelectStatementString;

        /// SQL statement object for request bulk selection
        castor::db::IDbStatement *m_bulkSelectStatement;

        /// SQL statement for request update
        static const std::string s_updateStatementString;

        /// SQL statement object for request update
        castor::db::IDbStatement *m_updateStatement;

        /// SQL statement for type storage 
        static const std::string s_storeTypeStatementString;

        /// SQL statement object for type storage
        castor::db::IDbStatement *m_storeTypeStatement;

        /// SQL statement for type deletion 
        static const std::string s_deleteTypeStatementString;

        /// SQL statement object for type deletion
        castor::db::IDbStatement *m_deleteTypeStatement;

        /// SQL checkExist statement for member tape
        static const std::string s_checkVdqmTapeExistStatementString;

        /// SQL checkExist statement object for member tape
        castor::db::IDbStatement *m_checkVdqmTapeExistStatement;

        /// SQL update statement for member tape
        static const std::string s_updateVdqmTapeStatementString;

        /// SQL update statement object for member tape
        castor::db::IDbStatement *m_updateVdqmTapeStatement;

        /// SQL checkExist statement for member tapeAccessSpecification
        static const std::string s_checkTapeAccessSpecificationExistStatementString;

        /// SQL checkExist statement object for member tapeAccessSpecification
        castor::db::IDbStatement *m_checkTapeAccessSpecificationExistStatement;

        /// SQL update statement for member tapeAccessSpecification
        static const std::string s_updateTapeAccessSpecificationStatementString;

        /// SQL update statement object for member tapeAccessSpecification
        castor::db::IDbStatement *m_updateTapeAccessSpecificationStatement;

        /// SQL checkExist statement for member requestedSrv
        static const std::string s_checkTapeServerExistStatementString;

        /// SQL checkExist statement object for member requestedSrv
        castor::db::IDbStatement *m_checkTapeServerExistStatement;

        /// SQL update statement for member requestedSrv
        static const std::string s_updateTapeServerStatementString;

        /// SQL update statement object for member requestedSrv
        castor::db::IDbStatement *m_updateTapeServerStatement;

        /// SQL select statement for member tapeDrive
        static const std::string s_selectTapeDriveStatementString;

        /// SQL select statement object for member tapeDrive
        castor::db::IDbStatement *m_selectTapeDriveStatement;

        /// SQL delete statement for member tapeDrive
        static const std::string s_deleteTapeDriveStatementString;

        /// SQL delete statement object for member tapeDrive
        castor::db::IDbStatement *m_deleteTapeDriveStatement;

        /// SQL remote update statement for member tapeDrive
        static const std::string s_remoteUpdateTapeDriveStatementString;

        /// SQL remote update statement object for member tapeDrive
        castor::db::IDbStatement *m_remoteUpdateTapeDriveStatement;

        /// SQL checkExist statement for member tapeDrive
        static const std::string s_checkTapeDriveExistStatementString;

        /// SQL checkExist statement object for member tapeDrive
        castor::db::IDbStatement *m_checkTapeDriveExistStatement;

        /// SQL update statement for member tapeDrive
        static const std::string s_updateTapeDriveStatementString;

        /// SQL update statement object for member tapeDrive
        castor::db::IDbStatement *m_updateTapeDriveStatement;

        /// SQL checkExist statement for member deviceGroupName
        static const std::string s_checkDeviceGroupNameExistStatementString;

        /// SQL checkExist statement object for member deviceGroupName
        castor::db::IDbStatement *m_checkDeviceGroupNameExistStatement;

        /// SQL update statement for member deviceGroupName
        static const std::string s_updateDeviceGroupNameStatementString;

        /// SQL update statement object for member deviceGroupName
        castor::db::IDbStatement *m_updateDeviceGroupNameStatement;

        /// SQL checkExist statement for member client
        static const std::string s_checkClientIdentificationExistStatementString;

        /// SQL checkExist statement object for member client
        castor::db::IDbStatement *m_checkClientIdentificationExistStatement;

        /// SQL update statement for member client
        static const std::string s_updateClientIdentificationStatementString;

        /// SQL update statement object for member client
        castor::db::IDbStatement *m_updateClientIdentificationStatement;

      }; // end of class DbTapeRequestCnv

    } /* end of namespace cnv */

  } /* end of namespace db */

} /* end of namespace castor */

#endif // CASTOR_DB_CNV_TAPEREQUEST_HPP
