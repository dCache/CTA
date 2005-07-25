/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraTapeRequestCnv.hpp
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

#ifndef CASTOR_DB_ORA_TAPEREQUEST_HPP
#define CASTOR_DB_ORA_TAPEREQUEST_HPP

// Include Files
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/db/ora/OraBaseCnv.hpp"
#include "castor/exception/Exception.hpp"

namespace castor {

  // Forward declarations
  class IObject;
  class ICnvSvc;

  // Forward declarations
  namespace vdqm {

    // Forward declarations
    class TapeRequest;

  }; // end of namespace vdqm

  namespace db {

    namespace ora {

      /**
       * class OraTapeRequestCnv
       * A converter for storing/retrieving TapeRequest into/from an Oracle database
       */
      class OraTapeRequestCnv : public OraBaseCnv {

      public:

        /**
         * Constructor
         */
        OraTapeRequestCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~OraTapeRequestCnv() throw();

        /**
         * Gets the object type.
         * That is the type of object this converter can convert
         */
        static const unsigned int ObjType();

        /**
         * Gets the object type.
         * That is the type of object this converter can convert
         */
        virtual const unsigned int objType() const;

        /**
         * Creates foreign representation from a C++ Object.
         * @param address where to store the representation of
         * the object
         * @param object the object to deal with
         * @param autocommit whether the changes to the database
         * should be commited or not
         * @param type if not OBJ_INVALID, the ids representing
         * the links to objects of this type will not set to 0
         * as is the default.
         * @exception Exception throws an Exception in cas of error
         */
        virtual void createRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool autocommit,
                               unsigned int type = castor::OBJ_INVALID)
          throw (castor::exception::Exception);

        /**
         * Updates foreign representation from a C++ Object.
         * @param address where the representation of
         * the object is stored
         * @param object the object to deal with
         * @param autocommit whether the changes to the database
         * should be commited or not
         * @exception Exception throws an Exception in cas of error
         */
        virtual void updateRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool autocommit)
          throw (castor::exception::Exception);

        /**
         * Deletes foreign representation of a C++ Object.
         * @param address where the representation of
         * the object is stored
         * @param object the object to deal with
         * @param autocommit whether the changes to the database
         * should be commited or not
         * @exception Exception throws an Exception in cas of error
         */
        virtual void deleteRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool autocommit)
          throw (castor::exception::Exception);

        /**
         * Creates C++ object from foreign representation
         * @param address the place where to find the foreign
         * representation
         * @return the C++ object created from its reprensentation
         * or 0 if unsuccessful. Note that the caller is responsible
         * for the deallocation of the newly created object
         * @exception Exception throws an Exception in cas of error
         */
        virtual castor::IObject* createObj(castor::IAddress* address)
          throw (castor::exception::Exception);

        /**
         * Updates C++ object from its foreign representation.
         * @param obj the object to deal with
         * @exception Exception throws an Exception in cas of error
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
         * @param autocommit whether the changes to the database
         * should be commited or not
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRep(castor::IAddress* address,
                             castor::IObject* object,
                             unsigned int type,
                             bool autocommit)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type Tape refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTape(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeAccessSpecification refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeAccessSpecification(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeServer refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeServer(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeDrive refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeDrive(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepDeviceGroupName(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type ClientIdentification refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepClientIdentification(castor::vdqm::TapeRequest* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Retrieve from the database some of the objects refered by a given object.
         * @param object the original object
         * @param type the type of the refered objects to retrieve
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObj(castor::IAddress* address,
                             castor::IObject* object,
                             unsigned int type)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type Tape refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTape(castor::vdqm::TapeRequest* obj)
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
         * Retrieve from the database objects of type TapeServer refered by a given object.
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
        oracle::occi::Statement *m_insertStatement;

        /// SQL statement for request deletion
        static const std::string s_deleteStatementString;

        /// SQL statement object for request deletion
        oracle::occi::Statement *m_deleteStatement;

        /// SQL statement for request selection
        static const std::string s_selectStatementString;

        /// SQL statement object for request selection
        oracle::occi::Statement *m_selectStatement;

        /// SQL statement for request update
        static const std::string s_updateStatementString;

        /// SQL statement object for request update
        oracle::occi::Statement *m_updateStatement;

        /// SQL statement for type storage 
        static const std::string s_storeTypeStatementString;

        /// SQL statement object for type storage
        oracle::occi::Statement *m_storeTypeStatement;

        /// SQL statement for type deletion 
        static const std::string s_deleteTypeStatementString;

        /// SQL statement object for type deletion
        oracle::occi::Statement *m_deleteTypeStatement;

        /// SQL checkExist statement for member tape
        static const std::string s_checkTapeExistStatementString;

        /// SQL checkExist statement object for member tape
        oracle::occi::Statement *m_checkTapeExistStatement;

        /// SQL update statement for member tape
        static const std::string s_updateTapeStatementString;

        /// SQL update statement object for member tape
        oracle::occi::Statement *m_updateTapeStatement;

        /// SQL checkExist statement for member tapeAccessSpecificatioon
        static const std::string s_checkTapeAccessSpecificationExistStatementString;

        /// SQL checkExist statement object for member tapeAccessSpecificatioon
        oracle::occi::Statement *m_checkTapeAccessSpecificationExistStatement;

        /// SQL update statement for member tapeAccessSpecificatioon
        static const std::string s_updateTapeAccessSpecificationStatementString;

        /// SQL update statement object for member tapeAccessSpecificatioon
        oracle::occi::Statement *m_updateTapeAccessSpecificationStatement;

        /// SQL checkExist statement for member requestedSrv
        static const std::string s_checkTapeServerExistStatementString;

        /// SQL checkExist statement object for member requestedSrv
        oracle::occi::Statement *m_checkTapeServerExistStatement;

        /// SQL update statement for member requestedSrv
        static const std::string s_updateTapeServerStatementString;

        /// SQL update statement object for member requestedSrv
        oracle::occi::Statement *m_updateTapeServerStatement;

        /// SQL select statement for member tapeDrive
        static const std::string s_selectTapeDriveStatementString;

        /// SQL select statement object for member tapeDrive
        oracle::occi::Statement *m_selectTapeDriveStatement;

        /// SQL delete statement for member tapeDrive
        static const std::string s_deleteTapeDriveStatementString;

        /// SQL delete statement object for member tapeDrive
        oracle::occi::Statement *m_deleteTapeDriveStatement;

        /// SQL remote update statement for member tapeDrive
        static const std::string s_remoteUpdateTapeDriveStatementString;

        /// SQL remote update statement object for member tapeDrive
        oracle::occi::Statement *m_remoteUpdateTapeDriveStatement;

        /// SQL checkExist statement for member tapeDrive
        static const std::string s_checkTapeDriveExistStatementString;

        /// SQL checkExist statement object for member tapeDrive
        oracle::occi::Statement *m_checkTapeDriveExistStatement;

        /// SQL update statement for member tapeDrive
        static const std::string s_updateTapeDriveStatementString;

        /// SQL update statement object for member tapeDrive
        oracle::occi::Statement *m_updateTapeDriveStatement;

        /// SQL checkExist statement for member deviceGroupName
        static const std::string s_checkDeviceGroupNameExistStatementString;

        /// SQL checkExist statement object for member deviceGroupName
        oracle::occi::Statement *m_checkDeviceGroupNameExistStatement;

        /// SQL update statement for member deviceGroupName
        static const std::string s_updateDeviceGroupNameStatementString;

        /// SQL update statement object for member deviceGroupName
        oracle::occi::Statement *m_updateDeviceGroupNameStatement;

        /// SQL checkExist statement for member client
        static const std::string s_checkClientIdentificationExistStatementString;

        /// SQL checkExist statement object for member client
        oracle::occi::Statement *m_checkClientIdentificationExistStatement;

        /// SQL update statement for member client
        static const std::string s_updateClientIdentificationStatementString;

        /// SQL update statement object for member client
        oracle::occi::Statement *m_updateClientIdentificationStatement;

      }; // end of class OraTapeRequestCnv

    }; // end of namespace ora

  }; // end of namespace db

}; // end of namespace castor

#endif // CASTOR_DB_ORA_TAPEREQUEST_HPP
