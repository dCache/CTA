#ifndef CASTOR_IO_STAGEUPDATEFILESTATUSREQUEST_HPP
#define CASTOR_IO_STAGEUPDATEFILESTATUSREQUEST_HPP

// Include Files
#include "castor/Constants.hpp"
#include "castor/IAddress.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/io/StreamBaseCnv.hpp"

namespace castor {

  // Forward declarations
  class ObjectSet;
  class ObjectCatalog;
  class IObject;

  namespace io {

    // Forward declarations
    class StreamAddress;

    /**
     * class StreamStageUpdateFileStatusRequestCnv
     * A converter for marshalling/unmarshalling StageUpdateFileStatusRequest into/from
     * stl streams
     */
    class StreamStageUpdateFileStatusRequestCnv : public StreamBaseCnv {

    public:

      /**
       * Constructor
       */
      StreamStageUpdateFileStatusRequestCnv();

      /**
       * Destructor
       */
      virtual ~StreamStageUpdateFileStatusRequestCnv() throw();

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
       * Marshals an object using a StreamAddress.
       * If the object is in alreadyDone, just marshal its id. Otherwise, call createRep
       * and recursively marshal the refered objects.
       * @param object the object to marshal
       * @param address the address where to marshal
       * @param alreadyDone the list of objects already marshalled
       * @exception Exception throws an Exception in case of error
       */
      virtual void marshalObject(castor::IObject* object,
                                 StreamAddress* address,
                                 castor::ObjectSet& alreadyDone)
        throw (castor::exception::Exception);

      /**
       * Unmarshals an object from a StreamAddress.
       * @param stream the stream from which to unmarshal
       * @param newlyCreated a set of already created objects
       * that is used in case of circular dependencies
       * @return a pointer to the new object. If their was some
       * memory allocation (creation of a new object), the caller
       * is responsible for its deallocation
       * @exception Exception throws an Exception in case of error
       */
      virtual castor::IObject* unmarshalObject(castor::io::biniostream& stream,
                                               castor::ObjectCatalog& newlyCreated)
        throw (castor::exception::Exception);

    }; // end of class StreamStageUpdateFileStatusRequestCnv

  }; // end of namespace io

}; // end of namespace castor

#endif // CASTOR_IO_STAGEUPDATEFILESTATUSREQUEST_HPP
