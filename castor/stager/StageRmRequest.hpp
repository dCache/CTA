#ifndef CASTOR_STAGER_STAGERMREQUEST_HPP
#define CASTOR_STAGER_STAGERMREQUEST_HPP

// Include Files
#include "castor/stager/Request.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    /**
     * class StageRmRequest
     * 
     */
    class StageRmRequest : public virtual Request {

    public:

      /**
       * Empty Constructor
       */
      StageRmRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~StageRmRequest() throw();

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
       * Sets the id of the object
       * @param id The new id
       */
      virtual void setId(u_signed64 id);

      /**
       * gets the id of the object
       */
      virtual u_signed64 id() const;

      /**
       * Gets the type of the object
       */
      virtual int type() const;

      /*********************************/
      /* End of IObject abstract class */
      /*********************************/
    private:

    private:

      /// The id of this object
      u_signed64 m_id;

    }; // end of class StageRmRequest

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_STAGERMREQUEST_HPP
