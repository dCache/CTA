/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_STAGER_STAGEPREPARETOUPDATEREQUEST_HPP
#define CASTOR_STAGER_STAGEPREPARETOUPDATEREQUEST_HPP

// Include Files
#include "castor/stager/FileRequest.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace stager {

    /**
     * class StagePrepareToUpdateRequest
     */
    class StagePrepareToUpdateRequest : public virtual FileRequest {

    public:

      /**
       * Empty Constructor
       */
      StagePrepareToUpdateRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~StagePrepareToUpdateRequest() throw();

      /**
       * Outputs this object in a human readable format
       * @param stream The stream where to print this object
       * @param indent The indentation to use
       * @param alreadyPrinted The set of objects already printed.
       * This is to avoid looping when printing circular dependencies
       */
      virtual void print(std::ostream& stream,
                         std::string indent,
                         castor::ObjectSet& alreadyPrinted) const;

      /**
       * Outputs this object in a human readable format
       */
      virtual void print() const;

      /**
       * Gets the type of this kind of objects
       */
      static int TYPE();

      /********************************************/
      /* Implementation of IObject abstract class */
      /********************************************/
      /**
       * Gets the type of the object
       */
      virtual int type() const;

      /**
       * virtual method to clone any object
       */
      virtual castor::IObject* clone();

      /*********************************/
      /* End of IObject abstract class */
      /*********************************/
      /**
       * Get the value of m_id
       * The id of this object
       * @return the value of m_id
       */
      u_signed64 id() const {
        return m_id;
      }

      /**
       * Set the value of m_id
       * The id of this object
       * @param new_var the new value of m_id
       */
      void setId(u_signed64 new_var) {
        m_id = new_var;
      }

    private:

      /// The id of this object
      u_signed64 m_id;

    }; // end of class StagePrepareToUpdateRequest

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_STAGEPREPARETOUPDATEREQUEST_HPP
