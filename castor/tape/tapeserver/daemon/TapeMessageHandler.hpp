/******************************************************************************
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

#pragma once

#include "castor/legacymsg/VdqmProxy.hpp"
#include "castor/legacymsg/VmgrProxy.hpp"
#include "castor/log/Logger.hpp"
#include "castor/messages/Header.pb.h"
#include "castor/messages/Constants.hpp"
#include "castor/messages/Heartbeat.pb.h"
#include "castor/tape/reactor/ZMQPollEventHandler.hpp"
#include "castor/tape/reactor/ZMQReactor.hpp"
#include "castor/tape/tapeserver/daemon/DriveCatalogue.hpp"
#include "castor/tape/utils/ZmqMsg.hpp"
#include "castor/tape/utils/ZmqSocket.hpp"
#include "castor/utils/utils.hpp"

namespace castor     {
namespace tape       {
namespace tapeserver {
namespace daemon     {

/**
 * Handles the events of the socket listening for connection from the admin
 * commands.
 */
class TapeMessageHandler: public reactor::ZMQPollEventHandler {
public:

  /**
   * Constructor.
   *
   * @param reactor The reactor to which new Vdqm connection handlers are to
   * be registered.
   * @param log The object representing the API of the CASTOR logging system.
   * @param driveCatalogue The tape-drive catalogue.
   * @param hostName The name of the host.
   * @param vdqm Proxy object representing the vdqmd daemon.
   * @param vmgr Proxy object representing the vmgrd daemon.
   * @param zmqContext The ZMQ context.
   */
  TapeMessageHandler(
    reactor::ZMQReactor &reactor,
    log::Logger &log,
    DriveCatalogue &driveCatalogue,
    const std::string &hostName,
    castor::legacymsg::VdqmProxy & vdqm,
    castor::legacymsg::VmgrProxy & vmgr,
    void *const zmqContext);

  /**
   * Destructor.
   */
  ~TapeMessageHandler() throw();

  /**
   * Returns the human-readable name this event handler.
   */
  std::string getName() const throw();

  /**
   * Fills the specified poll file-descriptor ready to be used in a call to
   * poll().
   */
  void fillPollFd(zmq_pollitem_t &fd) throw();

  /**
   * Handles the specified event.
   *
   * @param fd The poll file-descriptor describing the event.
   * @return true if the event handler should be removed from and deleted by
   * the reactor.
   */
  bool handleEvent(const zmq_pollitem_t &fd) throw();
  
private:
  /**
   *  Send to the client a ReturnValue(0,"")
   */
  void sendSuccessReplyToClient();
  /**
   * Send to the client a ReturnValue(ex.code(),ex.getGetMessageValue())
   * @param e
   */
  void sendErrorReplyToClient(const castor::exception::Exception& e);

  /**
   * Send to the client a ReturnValue Message with the given parameters
   * @param returnValue the interger value to send (0 = everything ok, 
   * positive integer will trigger an exception
   * @param msg The string to display (should be empty if returnValue=0)
   */
  void sendReplyToClient(int returnValue,const std::string& msg);
  
  /**
   * Will try to parse a message of type T into msg from the data which are 
   * within blob
   * @param msg
   * @param blob
   */
  template <class T> void parseMsgBlob(T& msg, const tape::utils::ZmqMsg& blob) {
    if(!msg.ParseFromArray(blob.data(), blob.size())) {
      castor::exception::Exception ex;
      ex.getMessage() << "Failed to parse a " <<
        castor::utils::demangledNameOf(msg) << " message blob"
        ": ParseFromArray() returned false";
      throw ex;
    }
  }
  
   /**
   * The reactor to which new Vdqm connection handlers are to be registered.
   */
  reactor::ZMQReactor &m_reactor;

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;
  
  tape::utils::ZmqSocket m_socket;
  
    /**
   * The catalogue of tape drives controlled by the tape server daemon.
   */
  DriveCatalogue &m_driveCatalogue;
  
  /**
   * The name of the host on which tape daemon is running.
   */
  const std::string m_hostName;
  
  /** 
   * Reference to the VdqmProxy, allowing reporting of the drive status. It 
   * will be used by the StatusReporter 
   */
  castor::legacymsg::VdqmProxy & m_vdqm;

  /** 
   * Reference to the VmgrProxy, allowing reporting and checking tape status.
   * It is also used by the StatusReporter 
   */
  castor::legacymsg::VmgrProxy & m_vmgr;
  
  /**
   * Make sure the  zmq_pollitem_t's socket is the same as m_socket
   * Throw an exception if it is not the case
   * @param fd the poll item 
   */
  void checkSocket(const zmq_pollitem_t &fd);
  
  /**
   * Dispatches the appropriate handler method for the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void dispatchMsgHandler(castor::messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleHeartbeatMsg(const messages::Header& header, 
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleMigrationJobFromTapeGateway(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleMigrationJobFromWriteTp(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleRecallJobFromTapeGateway(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */  
  void handleRecallJobFromReadTp(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */  
  void handleTapeMountedForMigration(const messages::Header& header, 
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleTapeMountedForRecall(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleNotifyDriveTapeMountedMsg(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleTapeUnmountStarted(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Handles the specified message.
   *
   * @param header The header of the message.
   * @param bodyBlob The serialized body of the message.
   */
  void handleTapeUnmounted(const messages::Header& header,
    const tape::utils::ZmqMsg &bodyBlob);

  /**
   * Unserialize the blob and check the header
   * @param headerBlob The blob from the header
   */
  messages::Header buildHeader(tape::utils::ZmqMsg& headerBlob);

}; // class TapeMessageHandler

} // namespace daemon
} // namespace tapeserver
} // namespace tape
} // namespace castor
