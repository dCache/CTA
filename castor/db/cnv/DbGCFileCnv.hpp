/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/db/cnv/DbGCFileCnv.hpp
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

#ifndef CASTOR_DB_CNV_GCFILE_HPP
#define CASTOR_DB_CNV_GCFILE_HPP

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
  namespace stager {

    // Forward declarations
    class GCFile;

  } /* end of namespace stager */

  namespace db {

    namespace cnv {

      /**
       * class DbGCFileCnv
       * A converter for storing/retrieving GCFile into/from a generic database
       */
      class DbGCFileCnv : public DbBaseCnv {

      public:

        /**
         * Constructor
         */
        DbGCFileCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~DbGCFileCnv() throw();

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
         * Fill the database with objects of type GCFileList refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepGCFileList(castor::stager::GCFile* obj)
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
         * Retrieve from the database objects of type GCFileList refered by a given
         * object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjGCFileList(castor::stager::GCFile* obj)
          throw (castor::exception::Exception);

      private:

        /// SQL statement for request insertion
        static const std::string s_insertStatementString;

        /// SQL statement object for request insertion
        castor::db::IDbStatement *m_insertStatement;

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

        /// SQL update statement for member request
        static const std::string s_updateGCFileListStatementString;

        /// SQL update statement object for member request
        castor::db::IDbStatement *m_updateGCFileListStatement;

      }; // end of class DbGCFileCnv

    } /* end of namespace cnv */

  } /* end of namespace db */

} /* end of namespace castor */

#endif // CASTOR_DB_CNV_GCFILE_HPP
