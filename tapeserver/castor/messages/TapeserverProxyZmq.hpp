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

#include "castor/log/Logger.hpp"
#include "castor/messages/Frame.hpp"
#include "castor/messages/Mutex.hpp"
#include "castor/messages/TapeserverProxy.hpp"
#include "castor/messages/ZmqSocketST.hpp"

namespace castor {
namespace messages {

/**
 * A concrete implementation of the interface to the internal network
 * communications of the tapeserverd daemon.
 */
class TapeserverProxyZmq: public TapeserverProxy {
public:

  /**
   * Constructor.
   *
   * @param log The object representing the API of the CASTOR logging system.
   * @param serverPort The TCP/IP port on which the tapeserverd daemon is
   * listening for internal ZMQ message.
   * @param zmqContext The ZMQ context.
   */
  TapeserverProxyZmq(log::Logger &log, const unsigned short serverPort,
    void *const zmqContext) throw();
  
  /**
   * Notifies the tapeserverd daemon that the mount-session child-process got
   * an archive job from CTA
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   * @return The number of files currently stored on the tape
   */
  virtual uint32_t gotArchiveJobFromCTA(const std::string &vid,
    const std::string &unitName);
  
  /**
   * Notifies the tapeserverd daemon that the mount-session child-process got
   * a retrieve job from CTA
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  virtual void gotRetrieveJobFromCTA(const std::string &vid,
    const std::string &unitName);

  /**
   * Notifies the tapeserverd daemon that the mount-session child-process got
   * a recall job from the tapegatewayd daemon.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  void gotRecallJobFromTapeGateway(const std::string &vid,
    const std::string &unitName);

  /**
   * Notifies the tapeserverd daemon that the mount-session child-process got
   * a recall job from the readtp command-line tool.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  void gotRecallJobFromReadTp(const std::string &vid,
    const std::string &unitName);

  /**
   * Notifies the tapeserverd daemon that the mount-session child-process got
   * a migration job from the tapegatewayd daemon.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   * @return The number of files currently stored on the tape as given by the
   * vmgrd daemon.
   */
  uint32_t gotMigrationJobFromTapeGateway(const std::string &vid,
    const std::string &unitName);

  /**
   * Notifies the tapeserverd daemon that the mount-session child-process got
   * a migration job from the writetp command-line tool.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   * @return The number of files currently stored on the tape as given by the
   * vmgrd daemon.
   */
  uint32_t gotMigrationJobFromWriteTp(const std::string &vid,
    const std::string &unitName);

  /**
   * Notifies the tapeserverd daemon that the specified tape has been mounted.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  void tapeMountedForRecall(const std::string &vid,
    const std::string &unitName);

  /**
   * Notifies the tapeserverd daemon that the specified tape has been mounted.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  void tapeMountedForMigration(const std::string &vid, 
    const std::string &unitName);
  
  /**
   * Notifies the tapeserverd daemon that the specified tape is unmounting.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  void tapeUnmountStarted(const std::string &vid,
    const std::string &unitName);
 
  /**
   * Notifies the tapeserverd daemon that the specified tape has been unmounted.
   *
   * @param vid The tape to be mounted for recall.
   * @param unitName The unit name of the tape drive.
   */
  void tapeUnmounted(const std::string &vid,
    const std::string &unitName);
 
  /**
   * Notifies the tapeserverd daemon that the data-transfer session is still
   * alive and gives an indication of how much data has been moved.
   *
   * @param unitName The unit name of the tape drive.
   * @param nbBlocksMoved Delta value giving the number of blocks moved
   * since the last heartbeat message.
   */
  void notifyHeartbeat(const std::string &unitName,
    const uint64_t nbBlocksMoved);
  
  /**
   * Sends a new set of parameters, to be logged by the mother process when the
   * transfer session is over.
   * @param params: a vector of log parameters
   */
  virtual void addLogParams(const std::string &unitName,
    const std::list<castor::log::Param> & params);
  
  /**
   * Sends a list of parameters to remove from the end of session logging.
   */
  virtual void deleteLogParams(const std::string &unitName,
    const std::list<std::string> & paramNames);
  
  /**
   * Notifies the tapeserverd daemon that a label session has encountered the
   * specified error.
   *
   * @param unitName The unit name of the tape drive.
   * @param message The error message.
   */
  void labelError(const std::string &unitName, const std::string &message);

private:

  /**
   * Mutex used to implement a critical section around the enclosed
   * ZMQ socket.
   */
  Mutex m_mutex;

  /**
   * The object representing the API of the CASTOR logging system.
   */
  log::Logger &m_log;

  /**
   * The name of the host on which the vdqmd daemon is running.
   */
  const std::string m_tapeserverHostName;

  /**
   * The TCP/IP port on which the vdqmd daemon is listening.
   */
  const unsigned short m_serverPort;
  
  /**
   * Socket connecting this tape server proxy to the tape server daemon.
   */
  ZmqSocketST m_serverSocket;
  
  /**
   * Creates a frame containing a ArchiveJobFromCTA message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createArchiveJobFromCTAFrame(const std::string &vid,
    const std::string &unitName);
          
  /**
   * Creates a frame containing a RetrieveJobFromCTA message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createRetrieveJobFromCTAFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a RecallJobFromTapeGateway message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createRecallJobFromTapeGatewayFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a RecallJobFromReadTp message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createRecallJobFromReadTpFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a MigrationJobFromTapeGateway message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createMigrationJobFromTapeGatewayFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a MigrationJobFromWriteTp message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createMigrationJobFromWriteTpFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a TapeMountedForRecall message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createTapeMountedForRecallFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a TapeMountedForMigration message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createTapeMountedForMigrationFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a TapeUnmountStarted message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createTapeUnmountStartedFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a TapeUnmounted message.
   *
   * @param vid The volume identifier of the tape.
   * @param unitName The unit name of the tape drive.
   * @return The frame.
   */
  Frame createTapeUnmountedFrame(const std::string &vid,
    const std::string &unitName);

  /**
   * Creates a frame containing a Heartbeat message.
   *
   * @param unitName The unit name of the tape drive.
   * @param nbBlocksMoved Delta value giving the number of blocks moved
   * since the last heartbeat message.
   * @return The frame.
   */
  Frame createHeartbeatFrame(const std::string &unitName,
    const uint64_t nbBlocksMoved);
  
  /**
   * Creates a frame containing an add log params.
   *
   * @param unitName The unit name of the tape drive.
   * @param params Values of the log parameters to be added or overwritten
   * in the status that the session will have in the main process's logs.
   * @return The frame.
   */
  Frame createAddLogParamsFrame(const std::string &unitName,
    const std::list<castor::log::Param> & params);

  /**
   * Creates a frame containing a delete log params.
   *
   * @param unitName The unit name of the tape drive.
   * @param paramNames names of the log parameters to remove
   * @return The frame.
   */
  Frame createDeleteLogParamsFrame(const std::string &unitName,
    const std::list<std::string> & paramNames);
  
  /**
   * Creates a frame containing a LabelError message.
   *
   * @param unitName The unit name of the tape drive.
   * @param message The error message.
   * @return The frame.
   */
  Frame createLabelErrorFrame(const std::string &unitName,
    const std::string &message);

}; // class TapeserverProxyZmq

} // namespace messages
} // namespace castor

