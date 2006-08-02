/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_RH_FILEQRYRESPONSE_HPP
#define CASTOR_RH_FILEQRYRESPONSE_HPP

// Include Files
#include "castor/rh/Response.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace rh {

    /**
     * class FileQryResponse
     * Response to the FileQueryRequest
     */
    class FileQryResponse : public virtual Response {

    public:

      /**
       * Empty Constructor
       */
      FileQryResponse() throw();

      /**
       * Empty Destructor
       */
      virtual ~FileQryResponse() throw();

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
       * Get the value of m_fileName
       * Name of the internal file
       * @return the value of m_fileName
       */
      std::string fileName() const {
        return m_fileName;
      }

      /**
       * Set the value of m_fileName
       * Name of the internal file
       * @param new_var the new value of m_fileName
       */
      void setFileName(std::string new_var) {
        m_fileName = new_var;
      }

      /**
       * Get the value of m_castorFileName
       * Name of the castor file as in the nameserver
       * @return the value of m_castorFileName
       */
      std::string castorFileName() const {
        return m_castorFileName;
      }

      /**
       * Set the value of m_castorFileName
       * Name of the castor file as in the nameserver
       * @param new_var the new value of m_castorFileName
       */
      void setCastorFileName(std::string new_var) {
        m_castorFileName = new_var;
      }

      /**
       * Get the value of m_fileId
       * Castor FileId for this file
       * @return the value of m_fileId
       */
      u_signed64 fileId() const {
        return m_fileId;
      }

      /**
       * Set the value of m_fileId
       * Castor FileId for this file
       * @param new_var the new value of m_fileId
       */
      void setFileId(u_signed64 new_var) {
        m_fileId = new_var;
      }

      /**
       * Get the value of m_status
       * Status of the file
       * @return the value of m_status
       */
      unsigned int status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * Status of the file
       * @param new_var the new value of m_status
       */
      void setStatus(unsigned int new_var) {
        m_status = new_var;
      }

      /**
       * Get the value of m_size
       * Size of the file
       * @return the value of m_size
       */
      u_signed64 size() const {
        return m_size;
      }

      /**
       * Set the value of m_size
       * Size of the file
       * @param new_var the new value of m_size
       */
      void setSize(u_signed64 new_var) {
        m_size = new_var;
      }

      /**
       * Get the value of m_poolName
       * Name of the pool containing the file
       * @return the value of m_poolName
       */
      std::string poolName() const {
        return m_poolName;
      }

      /**
       * Set the value of m_poolName
       * Name of the pool containing the file
       * @param new_var the new value of m_poolName
       */
      void setPoolName(std::string new_var) {
        m_poolName = new_var;
      }

      /**
       * Get the value of m_creationTime
       * Time of the file creation
       * @return the value of m_creationTime
       */
      u_signed64 creationTime() const {
        return m_creationTime;
      }

      /**
       * Set the value of m_creationTime
       * Time of the file creation
       * @param new_var the new value of m_creationTime
       */
      void setCreationTime(u_signed64 new_var) {
        m_creationTime = new_var;
      }

      /**
       * Get the value of m_accessTime
       * Time of the last access to the file
       * @return the value of m_accessTime
       */
      u_signed64 accessTime() const {
        return m_accessTime;
      }

      /**
       * Set the value of m_accessTime
       * Time of the last access to the file
       * @param new_var the new value of m_accessTime
       */
      void setAccessTime(u_signed64 new_var) {
        m_accessTime = new_var;
      }

      /**
       * Get the value of m_nbAccesses
       * Number of accesses to the file
       * @return the value of m_nbAccesses
       */
      unsigned int nbAccesses() const {
        return m_nbAccesses;
      }

      /**
       * Set the value of m_nbAccesses
       * Number of accesses to the file
       * @param new_var the new value of m_nbAccesses
       */
      void setNbAccesses(unsigned int new_var) {
        m_nbAccesses = new_var;
      }

      /**
       * Get the value of m_diskServer
       * @return the value of m_diskServer
       */
      std::string diskServer() const {
        return m_diskServer;
      }

      /**
       * Set the value of m_diskServer
       * @param new_var the new value of m_diskServer
       */
      void setDiskServer(std::string new_var) {
        m_diskServer = new_var;
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

    private:

      /// Name of the internal file
      std::string m_fileName;

      /// Name of the castor file as in the nameserver
      std::string m_castorFileName;

      /// Castor FileId for this file
      u_signed64 m_fileId;

      /// Status of the file
      unsigned int m_status;

      /// Size of the file
      u_signed64 m_size;

      /// Name of the pool containing the file
      std::string m_poolName;

      /// Time of the file creation
      u_signed64 m_creationTime;

      /// Time of the last access to the file
      u_signed64 m_accessTime;

      /// Number of accesses to the file
      unsigned int m_nbAccesses;

      std::string m_diskServer;

      /// The id of this object
      u_signed64 m_id;

    }; // end of class FileQryResponse

  }; // end of namespace rh

}; // end of namespace castor

#endif // CASTOR_RH_FILEQRYRESPONSE_HPP
