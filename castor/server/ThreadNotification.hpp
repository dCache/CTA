/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/server/ThreadNotification.hpp
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

#ifndef CASTOR_SERVER_THREADNOTIFICATION_HPP
#define CASTOR_SERVER_THREADNOTIFICATION_HPP

namespace castor {

  namespace server {

    /**
     * class ThreadNotification
     * Notification message sent via UDP to wake up threads. Used by the
     * NotificationDispatcher thread pool.
     */
    class ThreadNotification {

    public:

      /**
       * Empty Constructor
       */
      ThreadNotification() throw();

      /**
       * Empty Destructor
       */
      virtual ~ThreadNotification() throw();

      /**
       * Get the value of m_tpName
       * Name (initial) of the thread pool to be signaled
       * @return the value of m_tpName
       */
      unsigned char tpName() const {
        return m_tpName;
      }

      /**
       * Set the value of m_tpName
       * Name (initial) of the thread pool to be signaled
       * @param new_var the new value of m_tpName
       */
      void setTpName(unsigned char new_var) {
        m_tpName = new_var;
      }

      /**
       * Get the value of m_nbThreads
       * Number of threads to be woken up
       * @return the value of m_nbThreads
       */
      unsigned char nbThreads() const {
        return m_nbThreads;
      }

      /**
       * Set the value of m_nbThreads
       * Number of threads to be woken up
       * @param new_var the new value of m_nbThreads
       */
      void setNbThreads(unsigned char new_var) {
        m_nbThreads = new_var;
      }

    private:

      /// Name (initial) of the thread pool to be signaled
      unsigned char m_tpName;

      /// Number of threads to be woken up
      unsigned char m_nbThreads;

    }; /* end of class ThreadNotification */

  } /* end of namespace server */

} /* end of namespace castor */

#endif // CASTOR_SERVER_THREADNOTIFICATION_HPP
