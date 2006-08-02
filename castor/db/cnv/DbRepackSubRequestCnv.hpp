/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_DB_CNV_REPACKSUBREQUEST_HPP
#define CASTOR_DB_CNV_REPACKSUBREQUEST_HPP

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
  namespace repack {

    // Forward declarations
    class RepackSubRequest;

  }; // end of namespace repack

  namespace db {

    namespace cnv {

      /**
       * class DbRepackSubRequestCnv
       */
      class DbRepackSubRequestCnv : public DbBaseCnv {

      public:

        /**
         * Constructor
         */
        DbRepackSubRequestCnv(castor::ICnvSvc* cnvSvc);

        /**
         * Destructor
         */
        virtual ~DbRepackSubRequestCnv() throw();

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
         * Fill the database with objects of type RepackSegment refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepRepackSegment(castor::repack::RepackSubRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Fill the database with objects of type RepackRequest refered by a given object.
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillRepRepackRequest(castor::repack::RepackSubRequest* obj)
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
         * Retrieve from the database objects of type RepackSegment refered by a given
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjRepackSegment(castor::repack::RepackSubRequest* obj)
          throw (castor::exception::Exception);

        /**
         * Retrieve from the database objects of type RepackRequest refered by a given
         * @param obj the original object
         * @exception Exception throws an Exception in case of error
         */
        virtual void fillObjRepackRequest(castor::repack::RepackSubRequest* obj)
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

        /// SQL statement for type storage 
        static const std::string s_storeTypeStatementString;

        /// SQL statement object for type storage
        castor::db::IDbStatement *m_storeTypeStatement;

        /// SQL statement for type deletion 
        static const std::string s_deleteTypeStatementString;

        /// SQL statement object for type deletion
        castor::db::IDbStatement *m_deleteTypeStatement;

        /// SQL select statement for member segment
        static const std::string s_selectRepackSegmentStatementString;

        /// SQL select statement object for member segment
        castor::db::IDbStatement *m_selectRepackSegmentStatement;

        /// SQL delete statement for member segment
        static const std::string s_deleteRepackSegmentStatementString;

        /// SQL delete statement object for member segment
        castor::db::IDbStatement *m_deleteRepackSegmentStatement;

        /// SQL remote update statement for member segment
        static const std::string s_remoteUpdateRepackSegmentStatementString;

        /// SQL remote update statement object for member segment
        castor::db::IDbStatement *m_remoteUpdateRepackSegmentStatement;

        /// SQL checkExist statement for member requestID
        static const std::string s_checkRepackRequestExistStatementString;

        /// SQL checkExist statement object for member requestID
        castor::db::IDbStatement *m_checkRepackRequestExistStatement;

        /// SQL update statement for member requestID
        static const std::string s_updateRepackRequestStatementString;

        /// SQL update statement object for member requestID
        castor::db::IDbStatement *m_updateRepackRequestStatement;

      }; // end of class DbRepackSubRequestCnv

    }; // end of namespace cnv

  }; // end of namespace db

}; // end of namespace castor

#endif // CASTOR_DB_CNV_REPACKSUBREQUEST_HPP
