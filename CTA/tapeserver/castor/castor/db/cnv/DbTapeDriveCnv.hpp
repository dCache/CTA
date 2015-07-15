/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

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
    class TapeDrive;

  } /* end of namespace vdqm */

  namespace db {

    namespace cnv {

      /**
       * class DbTapeDriveCnv
       * A converter for storing/retrieving TapeDrive into/from a generic database
       */
      class DbTapeDriveCnv : public DbBaseCnv {

      public:

        /**
         * Constructor
         */
        DbTapeDriveCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~DbTapeDriveCnv() throw();

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
          ;

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
          ;

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
          ;

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
          ;

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
          ;

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
          ;

        /**
         * Updates C++ object from its foreign representation.
         * @param obj the object to deal with
         * @exception Exception throws an Exception in case of error
         */
        virtual void updateObj(castor::IObject* obj)
          ;

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
          ;

        /**
         * Fill the database with objects of type VdqmTape refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepVdqmTape(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Fill the database with objects of type TapeRequest refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeRequest(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Fill the database with objects of type TapeDriveDedication refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeDriveDedication(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Fill the database with objects of type TapeDriveCompatibility refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeDriveCompatibility(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Fill the database with objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepDeviceGroupName(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Fill the database with objects of type TapeServer refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeServer(castor::vdqm::TapeDrive* obj)
          ;

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
          ;

        /**
         * Retrieve from the database objects of type VdqmTape refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjVdqmTape(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Retrieve from the database objects of type TapeRequest refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeRequest(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Retrieve from the database objects of type TapeDriveDedication refered by a
         * given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeDriveDedication(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Retrieve from the database objects of type TapeDriveCompatibility refered by a
         * given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeDriveCompatibility(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Retrieve from the database objects of type DeviceGroupName refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjDeviceGroupName(castor::vdqm::TapeDrive* obj)
          ;

        /**
         * Retrieve from the database objects of type TapeServer refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeServer(castor::vdqm::TapeDrive* obj)
          ;

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

        /// SQL select statement for member 
        static const std::string s_selectTape2DriveDedicationStatementString;

        /// SQL select statement object for member 
        castor::db::IDbStatement *m_selectTape2DriveDedicationStatement;

        /// SQL delete statement for member 
        static const std::string s_deleteTape2DriveDedicationStatementString;

        /// SQL delete statement object for member 
        castor::db::IDbStatement *m_deleteTape2DriveDedicationStatement;

        /// SQL remote update statement for member 
        static const std::string s_remoteUpdateTape2DriveDedicationStatementString;

        /// SQL remote update statement object for member 
        castor::db::IDbStatement *m_remoteUpdateTape2DriveDedicationStatement;

        /// SQL checkExist statement for member tape
        static const std::string s_checkVdqmTapeExistStatementString;

        /// SQL checkExist statement object for member tape
        castor::db::IDbStatement *m_checkVdqmTapeExistStatement;

        /// SQL update statement for member tape
        static const std::string s_updateVdqmTapeStatementString;

        /// SQL update statement object for member tape
        castor::db::IDbStatement *m_updateVdqmTapeStatement;

        /// SQL select statement for member runningTapeReq
        static const std::string s_selectTapeRequestStatementString;

        /// SQL select statement object for member runningTapeReq
        castor::db::IDbStatement *m_selectTapeRequestStatement;

        /// SQL delete statement for member runningTapeReq
        static const std::string s_deleteTapeRequestStatementString;

        /// SQL delete statement object for member runningTapeReq
        castor::db::IDbStatement *m_deleteTapeRequestStatement;

        /// SQL remote update statement for member runningTapeReq
        static const std::string s_remoteUpdateTapeRequestStatementString;

        /// SQL remote update statement object for member runningTapeReq
        castor::db::IDbStatement *m_remoteUpdateTapeRequestStatement;

        /// SQL checkExist statement for member runningTapeReq
        static const std::string s_checkTapeRequestExistStatementString;

        /// SQL checkExist statement object for member runningTapeReq
        castor::db::IDbStatement *m_checkTapeRequestExistStatement;

        /// SQL update statement for member runningTapeReq
        static const std::string s_updateTapeRequestStatementString;

        /// SQL update statement object for member runningTapeReq
        castor::db::IDbStatement *m_updateTapeRequestStatement;

        /// SQL select statement for member tapeDriveDedication
        static const std::string s_selectTapeDriveDedicationStatementString;

        /// SQL select statement object for member tapeDriveDedication
        castor::db::IDbStatement *m_selectTapeDriveDedicationStatement;

        /// SQL delete statement for member tapeDriveDedication
        static const std::string s_deleteTapeDriveDedicationStatementString;

        /// SQL delete statement object for member tapeDriveDedication
        castor::db::IDbStatement *m_deleteTapeDriveDedicationStatement;

        /// SQL remote update statement for member tapeDriveDedication
        static const std::string s_remoteUpdateTapeDriveDedicationStatementString;

        /// SQL remote update statement object for member tapeDriveDedication
        castor::db::IDbStatement *m_remoteUpdateTapeDriveDedicationStatement;

        /// SQL insert statement for member tapeDriveCompatibilities
        static const std::string s_insertTapeDriveCompatibilityStatementString;

        /// SQL insert statement object for member tapeDriveCompatibilities
        castor::db::IDbStatement *m_insertTapeDriveCompatibilityStatement;

        /// SQL delete statement for member tapeDriveCompatibilities
        static const std::string s_deleteTapeDriveCompatibilityStatementString;

        /// SQL delete statement object for member tapeDriveCompatibilities
        castor::db::IDbStatement *m_deleteTapeDriveCompatibilityStatement;

        /// SQL select statement for member tapeDriveCompatibilities
        static const std::string s_selectTapeDriveCompatibilityStatementString;

        /// SQL select statement object for member tapeDriveCompatibilities
        castor::db::IDbStatement *m_selectTapeDriveCompatibilityStatement;

        /// SQL checkExist statement for member deviceGroupName
        static const std::string s_checkDeviceGroupNameExistStatementString;

        /// SQL checkExist statement object for member deviceGroupName
        castor::db::IDbStatement *m_checkDeviceGroupNameExistStatement;

        /// SQL update statement for member deviceGroupName
        static const std::string s_updateDeviceGroupNameStatementString;

        /// SQL update statement object for member deviceGroupName
        castor::db::IDbStatement *m_updateDeviceGroupNameStatement;

        /// SQL checkExist statement for member tapeServer
        static const std::string s_checkTapeServerExistStatementString;

        /// SQL checkExist statement object for member tapeServer
        castor::db::IDbStatement *m_checkTapeServerExistStatement;

        /// SQL update statement for member tapeServer
        static const std::string s_updateTapeServerStatementString;

        /// SQL update statement object for member tapeServer
        castor::db::IDbStatement *m_updateTapeServerStatement;

      }; // end of class DbTapeDriveCnv

    } /* end of namespace cnv */

  } /* end of namespace db */

} /* end of namespace castor */

