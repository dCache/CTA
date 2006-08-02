/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_STAGER_TAPEPOOL_HPP
#define CASTOR_STAGER_TAPEPOOL_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class SvcClass;
    class Stream;

    /**
     * class TapePool
     * A Pool of tapes
     */
    class TapePool : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      TapePool() throw();

      /**
       * Empty Destructor
       */
      virtual ~TapePool() throw();

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
       * Get the value of m_name
       * Name of this pool
       * @return the value of m_name
       */
      std::string name() const {
        return m_name;
      }

      /**
       * Set the value of m_name
       * Name of this pool
       * @param new_var the new value of m_name
       */
      void setName(std::string new_var) {
        m_name = new_var;
      }

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

      /**
       * Add a SvcClass* object to the m_svcClassesVector list
       */
      void addSvcClasses(SvcClass* add_object) {
        m_svcClassesVector.push_back(add_object);
      }

      /**
       * Remove a SvcClass* object from m_svcClassesVector
       */
      void removeSvcClasses(SvcClass* remove_object) {
        for (unsigned int i = 0; i < m_svcClassesVector.size(); i++) {
          SvcClass* item = m_svcClassesVector[i];
          if (item == remove_object) {
            std::vector<SvcClass*>::iterator it = m_svcClassesVector.begin() + i;
            m_svcClassesVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of SvcClass* objects held by m_svcClassesVector
       * @return list of SvcClass* objects held by m_svcClassesVector
       */
      std::vector<SvcClass*>& svcClasses() {
        return m_svcClassesVector;
      }

      /**
       * Add a Stream* object to the m_streamsVector list
       */
      void addStreams(Stream* add_object) {
        m_streamsVector.push_back(add_object);
      }

      /**
       * Remove a Stream* object from m_streamsVector
       */
      void removeStreams(Stream* remove_object) {
        for (unsigned int i = 0; i < m_streamsVector.size(); i++) {
          Stream* item = m_streamsVector[i];
          if (item == remove_object) {
            std::vector<Stream*>::iterator it = m_streamsVector.begin() + i;
            m_streamsVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of Stream* objects held by m_streamsVector
       * @return list of Stream* objects held by m_streamsVector
       */
      std::vector<Stream*>& streams() {
        return m_streamsVector;
      }

    private:

      /// Name of this pool
      std::string m_name;

      /// The id of this object
      u_signed64 m_id;

      std::vector<SvcClass*> m_svcClassesVector;

      std::vector<Stream*> m_streamsVector;

    }; // end of class TapePool

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_TAPEPOOL_HPP
