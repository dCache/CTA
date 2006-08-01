/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_DB_CNV_DISKPOOLQUERY_HPP
#define CASTOR_DB_CNV_DISKPOOLQUERY_HPP

// Include Files
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/db/cnv/DbBaseCnv.hpp"
#include "castor/exception/Exception.hpp"

namespace castor {

  // Forward declarations
  class IObject;
  class ICnvSvc;

  // Forward declarations
  namespace query {

    // Forward declarations
    class DiskPoolQuery;

  }; // end of namespace query

  namespace db {

    namespace cnv {

      /**
       * class DbDiskPoolQueryCnv
       * A converter for storing/retrieving DiskPoolQuery into/from a generic database
       */
      class DbDiskPoolQueryCnv : public DbBaseCnv {

      public:

        /**
         * Constructor
         */
        DbDiskPoolQueryCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~DbDiskPoolQueryCnv() throw();

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
        virtual void fillRepSvcClass(castor::query::DiskPoolQuery* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type IClient refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepIClient(castor::query::DiskPoolQuery* obj)
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
                             bool autocommit)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type SvcClass refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjSvcClass(castor::query::DiskPoolQuery* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type IClient refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjIClient(castor::query::DiskPoolQuery* obj)
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

        /// SQL statement for request update
        static const std::string s_updateStatementString;

        /// SQL statement object for request update
        castor::db::IDbStatement *m_updateStatement;

        /// SQL statement for request insertion into newRequests table
        static const std::string s_insertNewReqStatementString;

        /// SQL statement object for request status insertion into newRequests table
        castor::db::IDbStatement *m_insertNewReqStatement;

        /// SQL statement for type storage 
        static const std::string s_storeTypeStatementString;

        /// SQL statement object for type storage
        castor::db::IDbStatement *m_storeTypeStatement;

        /// SQL statement for type deletion 
        static const std::string s_deleteTypeStatementString;

        /// SQL statement object for type deletion
        castor::db::IDbStatement *m_deleteTypeStatement;

        /// SQL checkExist statement for member svcClass
        static const std::string s_checkSvcClassExistStatementString;

        /// SQL checkExist statement object for member svcClass
        castor::db::IDbStatement *m_checkSvcClassExistStatement;

        /// SQL update statement for member svcClass
        static const std::string s_updateSvcClassStatementString;

        /// SQL update statement object for member svcClass
        castor::db::IDbStatement *m_updateSvcClassStatement;

        /// SQL update statement for member client
        static const std::string s_updateIClientStatementString;

        /// SQL update statement object for member client
        castor::db::IDbStatement *m_updateIClientStatement;

      }; // end of class DbDiskPoolQueryCnv

    }; // end of namespace cnv

  }; // end of namespace db

}; // end of namespace castor

#endif // CASTOR_DB_CNV_DISKPOOLQUERY_HPP
