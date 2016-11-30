/*
 * The CERN Tape Archive(CTA) project
 * Copyright(C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "mediachanger/ZmqSocket.hpp"
#include "mediachanger/ZmqSocketST.hpp"

#include <mutex>
#include <pthread.h>
#include <string>
#include <zmq.h>

namespace cta {
namespace mediachanger {

/**
 * A "Multi-Threaded" ZMQ socket.
 *
 * This concrete C++ class wraps a ZMQ socket and provides a thread safe
 * interface by acting as a monitor.  If a single-threaded use is required then
 * the ZmqSocketST class should be used instead.
 *
 * Please note that the getZmqSocket() method is not thread safe.
 */
class ZmqSocketMT: public ZmqSocket {
public:
    
  /**
   * Constructor.
   *
   * @param zmqContext The ZMQ context.
   * @param socketType The type of the ZMQ socket.
   */
  ZmqSocketMT(void *const zmqContext, const int socketType);
    
  /**
   * Destructor.
   */
  ~ZmqSocketMT() throw();
    
  /**
   * Closes the ZMQ socket.
   */
  void close();
    
  /**
   * Binds the ZMQ socket to the specified endpoint.
   *
   * @param endpoint The endpoint to bind to.
   */
  void bind(const std::string &endpoint);
    
  /**
   * Connects the socket to the spedicied endpoint.
   *
   * @param endpoint The endpoint to connect to.
   */ 
  void connect(const std::string &endpoint);

  /**
   * Sends the specified ZMQ message over the socket.
   *
   * @param msg The ZMQ messge to be sent.
   * @param flags See manual page of  zmq_msg_send().
   */
  void send(ZmqMsg &msg, const int flags = 0);
    
  /**
   * Sends the specified ZMQ message over the socket.
   *
   * @param msg The ZMQ messge to be sent.
   * @param flags See manual page of  zmq_msg_send().
   */
  void send(zmq_msg_t *const msg, const int flags = 0);
    
  /**
   * Receives a ZMQ mesage from the socket.
   *
   * @param msg Output parameter: The received ZMQ messge.
   * @param flags See manual page of  zmq_msg_send().
   */
  void recv(ZmqMsg &msg, const int flags = 0);
    
  /**
   * Receives a ZMQ mesage from the socket.
   *
   * @param msg Output parameter: The received ZMQ messge.
   * @param flags See manual page of  zmq_msg_send().
   */
  void recv(zmq_msg_t *const msg, const int flags = 0);

  /**
   * Returns the ZMQ socket wrappeed by this class.
   *
   * Please note that the getZmqSocket() method is not thread safe.
   *
   * @return The ZMQ socket wrappeed by this class.
   */
  void *getZmqSocket() const throw();

private:

  /**
   * Mutex used to implement a critical section around the enclosed
   * ZMQ socket.
   */
  std::mutex m_mutex;

  /**
   * The non thread-safe socket to be protected.
   */
  ZmqSocketST m_socket;

}; // class ZmqSocketMT

} // namespace mediachanger
} // namespace cta
