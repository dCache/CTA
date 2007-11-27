/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/jobmanager/JobSubmissionRequest.hpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_JOBMANAGER_JOBSUBMISSIONREQUEST_HPP
#define CASTOR_JOBMANAGER_JOBSUBMISSIONREQUEST_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace jobmanager {

    /**
     * class JobSubmissionRequest
     * A class to store the information required to submit a job into the scheduler
     * through the job manager.
     */
    class JobSubmissionRequest : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      JobSubmissionRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~JobSubmissionRequest() throw();

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
       * Get the value of m_subReqId
       * The Cuuid identifying the SubRequest, stored as a human readable string
       * @return the value of m_subReqId
       */
      std::string subReqId() const {
        return m_subReqId;
      }

      /**
       * Set the value of m_subReqId
       * The Cuuid identifying the SubRequest, stored as a human readable string
       * @param new_var the new value of m_subReqId
       */
      void setSubReqId(std::string new_var) {
        m_subReqId = new_var;
      }

      /**
       * Get the value of m_protocol
       * The protocol that will be used to access the file
       * @return the value of m_protocol
       */
      std::string protocol() const {
        return m_protocol;
      }

      /**
       * Set the value of m_protocol
       * The protocol that will be used to access the file
       * @param new_var the new value of m_protocol
       */
      void setProtocol(std::string new_var) {
        m_protocol = new_var;
      }

      /**
       * Get the value of m_xsize
       * The expected size of the castor file
       * @return the value of m_xsize
       */
      u_signed64 xsize() const {
        return m_xsize;
      }

      /**
       * Set the value of m_xsize
       * The expected size of the castor file
       * @param new_var the new value of m_xsize
       */
      void setXsize(u_signed64 new_var) {
        m_xsize = new_var;
      }

      /**
       * Get the value of m_requestedFileSystems
       * The filesystems requested to fulfil the jobs resource requirements in the
       * scheduler
       * @return the value of m_requestedFileSystems
       */
      std::string requestedFileSystems() const {
        return m_requestedFileSystems;
      }

      /**
       * Set the value of m_requestedFileSystems
       * The filesystems requested to fulfil the jobs resource requirements in the
       * scheduler
       * @param new_var the new value of m_requestedFileSystems
       */
      void setRequestedFileSystems(std::string new_var) {
        m_requestedFileSystems = new_var;
      }

      /**
       * Get the value of m_reqId
       * The Cuuid identifying the Request, stored as a human readable string
       * @return the value of m_reqId
       */
      std::string reqId() const {
        return m_reqId;
      }

      /**
       * Set the value of m_reqId
       * The Cuuid identifying the Request, stored as a human readable string
       * @param new_var the new value of m_reqId
       */
      void setReqId(std::string new_var) {
        m_reqId = new_var;
      }

      /**
       * Get the value of m_clientVersion
       * The version of the client initiating the request
       * @return the value of m_clientVersion
       */
      u_signed64 clientVersion() const {
        return m_clientVersion;
      }

      /**
       * Set the value of m_clientVersion
       * The version of the client initiating the request
       * @param new_var the new value of m_clientVersion
       */
      void setClientVersion(u_signed64 new_var) {
        m_clientVersion = new_var;
      }

      /**
       * Get the value of m_fileId
       * The id of this castor file. This identifies it uniquely
       * @return the value of m_fileId
       */
      u_signed64 fileId() const {
        return m_fileId;
      }

      /**
       * Set the value of m_fileId
       * The id of this castor file. This identifies it uniquely
       * @param new_var the new value of m_fileId
       */
      void setFileId(u_signed64 new_var) {
        m_fileId = new_var;
      }

      /**
       * Get the value of m_nsHost
       * The name server hosting this castor file
       * @return the value of m_nsHost
       */
      std::string nsHost() const {
        return m_nsHost;
      }

      /**
       * Set the value of m_nsHost
       * The name server hosting this castor file
       * @param new_var the new value of m_nsHost
       */
      void setNsHost(std::string new_var) {
        m_nsHost = new_var;
      }

      /**
       * Get the value of m_svcClass
       * The name of the service class for the castor file
       * @return the value of m_svcClass
       */
      std::string svcClass() const {
        return m_svcClass;
      }

      /**
       * Set the value of m_svcClass
       * The name of the service class for the castor file
       * @param new_var the new value of m_svcClass
       */
      void setSvcClass(std::string new_var) {
        m_svcClass = new_var;
      }

      /**
       * Get the value of m_ipAddress
       * The address of the machine where the client is running
       * @return the value of m_ipAddress
       */
      unsigned long ipAddress() const {
        return m_ipAddress;
      }

      /**
       * Set the value of m_ipAddress
       * The address of the machine where the client is running
       * @param new_var the new value of m_ipAddress
       */
      void setIpAddress(unsigned long new_var) {
        m_ipAddress = new_var;
      }

      /**
       * Get the value of m_port
       * The port associated with the clients request used for callback
       * @return the value of m_port
       */
      unsigned short port() const {
        return m_port;
      }

      /**
       * Set the value of m_port
       * The port associated with the clients request used for callback
       * @param new_var the new value of m_port
       */
      void setPort(unsigned short new_var) {
        m_port = new_var;
      }

      /**
       * Get the value of m_requestType
       * The type of the request e.g. OBJ_StagePutRequest
       * @return the value of m_requestType
       */
      int requestType() const {
        return m_requestType;
      }

      /**
       * Set the value of m_requestType
       * The type of the request e.g. OBJ_StagePutRequest
       * @param new_var the new value of m_requestType
       */
      void setRequestType(int new_var) {
        m_requestType = new_var;
      }

      /**
       * Get the value of m_username
       * Name of the user that submitted the request
       * @return the value of m_username
       */
      std::string username() const {
        return m_username;
      }

      /**
       * Set the value of m_username
       * Name of the user that submitted the request
       * @param new_var the new value of m_username
       */
      void setUsername(std::string new_var) {
        m_username = new_var;
      }

      /**
       * Get the value of m_euid
       * The user id of the user that submitted the request
       * @return the value of m_euid
       */
      unsigned long euid() const {
        return m_euid;
      }

      /**
       * Set the value of m_euid
       * The user id of the user that submitted the request
       * @param new_var the new value of m_euid
       */
      void setEuid(unsigned long new_var) {
        m_euid = new_var;
      }

      /**
       * Get the value of m_egid
       * The group id of the user that submitted the request
       * @return the value of m_egid
       */
      unsigned long egid() const {
        return m_egid;
      }

      /**
       * Set the value of m_egid
       * The group id of the user that submitted the request
       * @param new_var the new value of m_egid
       */
      void setEgid(unsigned long new_var) {
        m_egid = new_var;
      }

      /**
       * Get the value of m_selectTime
       * The time that the job request was picked up from the database
       * @return the value of m_selectTime
       */
      u_signed64 selectTime() const {
        return m_selectTime;
      }

      /**
       * Set the value of m_selectTime
       * The time that the job request was picked up from the database
       * @param new_var the new value of m_selectTime
       */
      void setSelectTime(u_signed64 new_var) {
        m_selectTime = new_var;
      }

      /**
       * Get the value of m_submitStartTime
       * The time the request started to be submitted into the scheduler
       * @return the value of m_submitStartTime
       */
      u_signed64 submitStartTime() const {
        return m_submitStartTime;
      }

      /**
       * Set the value of m_submitStartTime
       * The time the request started to be submitted into the scheduler
       * @param new_var the new value of m_submitStartTime
       */
      void setSubmitStartTime(u_signed64 new_var) {
        m_submitStartTime = new_var;
      }

      /**
       * Get the value of m_clientType
       * The Id2type of the client
       * @return the value of m_clientType
       */
      u_signed64 clientType() const {
        return m_clientType;
      }

      /**
       * Set the value of m_clientType
       * The Id2type of the client
       * @param new_var the new value of m_clientType
       */
      void setClientType(u_signed64 new_var) {
        m_clientType = new_var;
      }

      /**
       * Get the value of m_openFlags
       * The direction of the transfer, e.g. read, write, read/write
       * @return the value of m_openFlags
       */
      std::string openFlags() const {
        return m_openFlags;
      }

      /**
       * Set the value of m_openFlags
       * The direction of the transfer, e.g. read, write, read/write
       * @param new_var the new value of m_openFlags
       */
      void setOpenFlags(std::string new_var) {
        m_openFlags = new_var;
      }

      /**
       * Get the value of m_sourceDiskCopyId
       * The id of the source diskcopy
       * @return the value of m_sourceDiskCopyId
       */
      u_signed64 sourceDiskCopyId() const {
        return m_sourceDiskCopyId;
      }

      /**
       * Set the value of m_sourceDiskCopyId
       * The id of the source diskcopy
       * @param new_var the new value of m_sourceDiskCopyId
       */
      void setSourceDiskCopyId(u_signed64 new_var) {
        m_sourceDiskCopyId = new_var;
      }

      /**
       * Get the value of m_askedHosts
       * A list of hosts to provide to LSF where the job could potentially run.
       * @return the value of m_askedHosts
       */
      std::string askedHosts() const {
        return m_askedHosts;
      }

      /**
       * Set the value of m_askedHosts
       * A list of hosts to provide to LSF where the job could potentially run.
       * @param new_var the new value of m_askedHosts
       */
      void setAskedHosts(std::string new_var) {
        m_askedHosts = new_var;
      }

      /**
       * Get the value of m_numAskedHosts
       * The number of hosts defined in the askedHosts attribute
       * @return the value of m_numAskedHosts
       */
      u_signed64 numAskedHosts() const {
        return m_numAskedHosts;
      }

      /**
       * Set the value of m_numAskedHosts
       * The number of hosts defined in the askedHosts attribute
       * @param new_var the new value of m_numAskedHosts
       */
      void setNumAskedHosts(u_signed64 new_var) {
        m_numAskedHosts = new_var;
      }

      /**
       * Get the value of m_destDiskCopyId
       * The id of the destination diskcopy
       * @return the value of m_destDiskCopyId
       */
      u_signed64 destDiskCopyId() const {
        return m_destDiskCopyId;
      }

      /**
       * Set the value of m_destDiskCopyId
       * The id of the destination diskcopy
       * @param new_var the new value of m_destDiskCopyId
       */
      void setDestDiskCopyId(u_signed64 new_var) {
        m_destDiskCopyId = new_var;
      }

      /**
       * Get the value of m_clientSecure
       * The clientSecure specifies if the Client runs in secure mode
       * @return the value of m_clientSecure
       */
      int clientSecure() const {
        return m_clientSecure;
      }

      /**
       * Set the value of m_clientSecure
       * The clientSecure specifies if the Client runs in secure mode
       * @param new_var the new value of m_clientSecure
       */
      void setClientSecure(int new_var) {
        m_clientSecure = new_var;
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

      /// The Cuuid identifying the SubRequest, stored as a human readable string
      std::string m_subReqId;

      /// The protocol that will be used to access the file
      std::string m_protocol;

      /// The expected size of the castor file
      u_signed64 m_xsize;

      /// The filesystems requested to fulfil the jobs resource requirements in the scheduler
      std::string m_requestedFileSystems;

      /// The Cuuid identifying the Request, stored as a human readable string
      std::string m_reqId;

      /// The version of the client initiating the request
      u_signed64 m_clientVersion;

      /// The id of this castor file. This identifies it uniquely
      u_signed64 m_fileId;

      /// The name server hosting this castor file
      std::string m_nsHost;

      /// The name of the service class for the castor file
      std::string m_svcClass;

      /// The address of the machine where the client is running
      unsigned long m_ipAddress;

      /// The port associated with the clients request used for callback
      unsigned short m_port;

      /// The type of the request e.g. OBJ_StagePutRequest
      int m_requestType;

      /// Name of the user that submitted the request
      std::string m_username;

      /// The user id of the user that submitted the request
      unsigned long m_euid;

      /// The group id of the user that submitted the request
      unsigned long m_egid;

      /// The time that the job request was picked up from the database
      u_signed64 m_selectTime;

      /// The time the request started to be submitted into the scheduler
      u_signed64 m_submitStartTime;

      /// The Id2type of the client
      u_signed64 m_clientType;

      /// The direction of the transfer, e.g. read, write, read/write
      std::string m_openFlags;

      /// The id of the source diskcopy
      u_signed64 m_sourceDiskCopyId;

      /// A list of hosts to provide to LSF where the job could potentially run.
      std::string m_askedHosts;

      /// The number of hosts defined in the askedHosts attribute
      u_signed64 m_numAskedHosts;

      /// The id of the destination diskcopy
      u_signed64 m_destDiskCopyId;

      /// The clientSecure specifies if the Client runs in secure mode
      int m_clientSecure;

      /// The id of this object
      u_signed64 m_id;

    }; /* end of class JobSubmissionRequest */

  } /* end of namespace jobmanager */

} /* end of namespace castor */

#endif // CASTOR_JOBMANAGER_JOBSUBMISSIONREQUEST_HPP
