/******************************************************************************
 *                      castor/db/ora/OraDiskServerCnv.hpp
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

#ifndef CASTOR_DB_ORA_DISKSERVER_HPP
#define CASTOR_DB_ORA_DISKSERVER_HPP

// Include Files
#include "castor/IAddress.hpp"
#include "castor/db/ora/OraBaseCnv.hpp"
#include "castor/exception/Exception.hpp"

namespace castor {

  // Forward declarations
  class IObject;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class DiskServer;

  }; // end of namespace stager

  namespace db {

    namespace ora {

      /**
       * class OraDiskServerCnv
       * A converter for storing/retrieving DiskServer into/from an Oracle database
       */
      class OraDiskServerCnv : public OraBaseCnv {

      public:

        /**
         * Constructor
         */
        OraDiskServerCnv();

        /**
         * Destructor
         */
        virtual ~OraDiskServerCnv() throw();

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
         * @exception Exception throws an Exception in cas of error
         */
        virtual void createRep(castor::IAddress* address,
                               castor::IObject* object,
                               bool autocommit)
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
         * Fill the database with objects of type FileSystem refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepFileSystem(castor::stager::DiskServer* obj)
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
         * Retrieve from the database objects of type FileSystem refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjFileSystem(castor::stager::DiskServer* obj)
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

        /// SQL insert statement for member fileSystems
        static const std::string s_insertDiskServer2FileSystemStatementString;

        /// SQL insert statement object for member fileSystems
        oracle::occi::Statement *m_insertDiskServer2FileSystemStatement;

        /// SQL delete statement for member fileSystems
        static const std::string s_deleteDiskServer2FileSystemStatementString;

        /// SQL delete statement object for member fileSystems
        oracle::occi::Statement *m_deleteDiskServer2FileSystemStatement;

        /// SQL select statement for member fileSystems
        static const std::string s_DiskServer2FileSystemStatementString;

        /// SQL select statement object for member fileSystems
        oracle::occi::Statement *m_DiskServer2FileSystemStatement;

        /// SQL insert statement for member status
        static const std::string s_insertDiskServer2DiskServerStatusCodeStatementString;

        /// SQL insert statement object for member status
        oracle::occi::Statement *m_insertDiskServer2DiskServerStatusCodeStatement;

        /// SQL delete statement for member status
        static const std::string s_deleteDiskServer2DiskServerStatusCodeStatementString;

        /// SQL delete statement object for member status
        oracle::occi::Statement *m_deleteDiskServer2DiskServerStatusCodeStatement;

        /// SQL select statement for member status
        static const std::string s_DiskServer2DiskServerStatusCodeStatementString;

        /// SQL select statement object for member status
        oracle::occi::Statement *m_DiskServer2DiskServerStatusCodeStatement;

      }; // end of class OraDiskServerCnv

    }; // end of namespace ora

  }; // end of namespace db

}; // end of namespace castor

#endif // CASTOR_DB_ORA_DISKSERVER_HPP
