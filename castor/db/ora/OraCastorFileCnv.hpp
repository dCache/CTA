/******************************************************************************
 *                      castor/db/ora/OraCastorFileCnv.hpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

#ifndef CASTOR_DB_ORA_CASTORFILE_HPP
#define CASTOR_DB_ORA_CASTORFILE_HPP

// Include Files
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/db/ora/OraBaseCnv.hpp"
#include "castor/exception/Exception.hpp"

namespace castor {

  // Forward declarations
  class IObject;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class CastorFile;

  }; // end of namespace stager

  namespace db {

    namespace ora {

      /**
       * class OraCastorFileCnv
       * A converter for storing/retrieving CastorFile into/from an Oracle database
       */
      class OraCastorFileCnv : public OraBaseCnv {

      public:

        /**
         * Constructor
         */
        OraCastorFileCnv();

        /**
         * Destructor
         */
        virtual ~OraCastorFileCnv() throw();

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
         * Fill the database with objects of type SvcClass refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepSvcClass(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type FileClass refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepFileClass(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type DiskCopy refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepDiskCopy(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type TapeCopy refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepTapeCopy(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

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
         * Retrieve from the database objects of type SvcClass refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjSvcClass(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type FileClass refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjFileClass(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type DiskCopy refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjDiskCopy(castor::stager::CastorFile* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type TapeCopy refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjTapeCopy(castor::stager::CastorFile* obj)
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

        /// SQL checkExist statement for member svcClass
        static const std::string s_checkSvcClassExistStatementString;

        /// SQL checkExist statement object for member svcClass
        oracle::occi::Statement *m_checkSvcClassExistStatement;

        /// SQL update statement for member svcClass
        static const std::string s_updateSvcClassStatementString;

        /// SQL update statement object for member svcClass
        oracle::occi::Statement *m_updateSvcClassStatement;

        /// SQL checkExist statement for member fileClass
        static const std::string s_checkFileClassExistStatementString;

        /// SQL checkExist statement object for member fileClass
        oracle::occi::Statement *m_checkFileClassExistStatement;

        /// SQL update statement for member fileClass
        static const std::string s_updateFileClassStatementString;

        /// SQL update statement object for member fileClass
        oracle::occi::Statement *m_updateFileClassStatement;

        /// SQL select statement for member diskCopies
        static const std::string s_selectDiskCopyStatementString;

        /// SQL select statement object for member diskCopies
        oracle::occi::Statement *m_selectDiskCopyStatement;

        /// SQL delete statement for member diskCopies
        static const std::string s_deleteDiskCopyStatementString;

        /// SQL delete statement object for member diskCopies
        oracle::occi::Statement *m_deleteDiskCopyStatement;

        /// SQL remote update statement for member diskCopies
        static const std::string s_remoteUpdateDiskCopyStatementString;

        /// SQL remote update statement object for member diskCopies
        oracle::occi::Statement *m_remoteUpdateDiskCopyStatement;

        /// SQL select statement for member tapeCopies
        static const std::string s_selectTapeCopyStatementString;

        /// SQL select statement object for member tapeCopies
        oracle::occi::Statement *m_selectTapeCopyStatement;

        /// SQL delete statement for member tapeCopies
        static const std::string s_deleteTapeCopyStatementString;

        /// SQL delete statement object for member tapeCopies
        oracle::occi::Statement *m_deleteTapeCopyStatement;

        /// SQL remote update statement for member tapeCopies
        static const std::string s_remoteUpdateTapeCopyStatementString;

        /// SQL remote update statement object for member tapeCopies
        oracle::occi::Statement *m_remoteUpdateTapeCopyStatement;

      }; // end of class OraCastorFileCnv

    }; // end of namespace ora

  }; // end of namespace db

}; // end of namespace castor

#endif // CASTOR_DB_ORA_CASTORFILE_HPP
