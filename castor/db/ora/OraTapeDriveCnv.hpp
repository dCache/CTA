/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/ora/OraTapeDriveCnv.hpp
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

#ifndef CASTOR_DB_ORA_TAPEDRIVE_HPP
#define CASTOR_DB_ORA_TAPEDRIVE_HPP

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
    class TapeDrive;

  }; // end of namespace vdqm

  namespace db {

    namespace ora {

      /**
       * class OraTapeDriveCnv
       * A converter for storing/retrieving TapeDrive into/from an Oracle database
       */
      class OraTapeDriveCnv : public OraBaseCnv {

      public:

        /**
         * Constructor
         */
        OraTapeDriveCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~OraTapeDriveCnv() throw();

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
         * Fill the foreign representation with some of the objects referred by a given C++
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
        virtual void fillRepTape(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeRequest refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeRequest(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type ErrorHistory refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepErrorHistory(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeDriveDedication refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeDriveDedication(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeDriveCompatibility refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeDriveCompatibility(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepDeviceGroupName(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception, oracle::occi::SQLException);

        /**
         * Fill the database with objects of type TapeServer refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeServer(castor::vdqm::TapeDrive* obj)
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
        virtual void fillObjTape(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeRequest refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeRequest(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type ErrorHistory refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjErrorHistory(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeDriveDedication refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeDriveDedication(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeDriveCompatibility refered by a
         * given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeDriveCompatibility(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjDeviceGroupName(castor::vdqm::TapeDrive* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeServer refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeServer(castor::vdqm::TapeDrive* obj)
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

        /// SQL select statement for member runningTapeReq
        static const std::string s_selectTapeRequestStatementString;

        /// SQL select statement object for member runningTapeReq
        oracle::occi::Statement *m_selectTapeRequestStatement;

        /// SQL delete statement for member runningTapeReq
        static const std::string s_deleteTapeRequestStatementString;

        /// SQL delete statement object for member runningTapeReq
        oracle::occi::Statement *m_deleteTapeRequestStatement;

        /// SQL remote update statement for member runningTapeReq
        static const std::string s_remoteUpdateTapeRequestStatementString;

        /// SQL remote update statement object for member runningTapeReq
        oracle::occi::Statement *m_remoteUpdateTapeRequestStatement;

        /// SQL checkExist statement for member runningTapeReq
        static const std::string s_checkTapeRequestExistStatementString;

        /// SQL checkExist statement object for member runningTapeReq
        oracle::occi::Statement *m_checkTapeRequestExistStatement;

        /// SQL update statement for member runningTapeReq
        static const std::string s_updateTapeRequestStatementString;

        /// SQL update statement object for member runningTapeReq
        oracle::occi::Statement *m_updateTapeRequestStatement;

        /// SQL select statement for member errorHistory
        static const std::string s_selectErrorHistoryStatementString;

        /// SQL select statement object for member errorHistory
        oracle::occi::Statement *m_selectErrorHistoryStatement;

        /// SQL delete statement for member errorHistory
        static const std::string s_deleteErrorHistoryStatementString;

        /// SQL delete statement object for member errorHistory
        oracle::occi::Statement *m_deleteErrorHistoryStatement;

        /// SQL remote update statement for member errorHistory
        static const std::string s_remoteUpdateErrorHistoryStatementString;

        /// SQL remote update statement object for member errorHistory
        oracle::occi::Statement *m_remoteUpdateErrorHistoryStatement;

        /// SQL select statement for member tapeDriveDedication
        static const std::string s_selectTapeDriveDedicationStatementString;

        /// SQL select statement object for member tapeDriveDedication
        oracle::occi::Statement *m_selectTapeDriveDedicationStatement;

        /// SQL delete statement for member tapeDriveDedication
        static const std::string s_deleteTapeDriveDedicationStatementString;

        /// SQL delete statement object for member tapeDriveDedication
        oracle::occi::Statement *m_deleteTapeDriveDedicationStatement;

        /// SQL remote update statement for member tapeDriveDedication
        static const std::string s_remoteUpdateTapeDriveDedicationStatementString;

        /// SQL remote update statement object for member tapeDriveDedication
        oracle::occi::Statement *m_remoteUpdateTapeDriveDedicationStatement;

        /// SQL insert statement for member tapeDriveCompatibilities
        static const std::string s_insertTapeDriveCompatibilityStatementString;

        /// SQL insert statement object for member tapeDriveCompatibilities
        oracle::occi::Statement *m_insertTapeDriveCompatibilityStatement;

        /// SQL delete statement for member tapeDriveCompatibilities
        static const std::string s_deleteTapeDriveCompatibilityStatementString;

        /// SQL delete statement object for member tapeDriveCompatibilities
        oracle::occi::Statement *m_deleteTapeDriveCompatibilityStatement;

        /// SQL select statement for member tapeDriveCompatibilities
        static const std::string s_selectTapeDriveCompatibilityStatementString;

        /// SQL select statement object for member tapeDriveCompatibilities
        oracle::occi::Statement *m_selectTapeDriveCompatibilityStatement;

        /// SQL checkExist statement for member deviceGroupName
        static const std::string s_checkDeviceGroupNameExistStatementString;

        /// SQL checkExist statement object for member deviceGroupName
        oracle::occi::Statement *m_checkDeviceGroupNameExistStatement;

        /// SQL update statement for member deviceGroupName
        static const std::string s_updateDeviceGroupNameStatementString;

        /// SQL update statement object for member deviceGroupName
        oracle::occi::Statement *m_updateDeviceGroupNameStatement;

        /// SQL checkExist statement for member tapeServer
        static const std::string s_checkTapeServerExistStatementString;

        /// SQL checkExist statement object for member tapeServer
        oracle::occi::Statement *m_checkTapeServerExistStatement;

        /// SQL update statement for member tapeServer
        static const std::string s_updateTapeServerStatementString;

        /// SQL update statement object for member tapeServer
        oracle::occi::Statement *m_updateTapeServerStatement;

      }; // end of class OraTapeDriveCnv

    }; // end of namespace ora

  }; // end of namespace db

}; // end of namespace castor

#endif // CASTOR_DB_ORA_TAPEDRIVE_HPP
