/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/Stream.hpp
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

#ifndef CASTOR_STAGER_STREAM_HPP
#define CASTOR_STAGER_STREAM_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/stager/StreamStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class TapePool;
    class TapeCopy;
    class Tape;

    /**
     * class Stream
     * A logical stream for the writing of DiskCopies on Tapes
     */
    class Stream : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      Stream() throw();

      /**
       * Empty Destructor
       */
      virtual ~Stream() throw();

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
       * Get the value of m_initialSizeToTransfer
       * Initial data volume to be migrated (needed by vmgr_gettape())
       * @return the value of m_initialSizeToTransfer
       */
      u_signed64 initialSizeToTransfer() const {
        return m_initialSizeToTransfer;
      }

      /**
       * Set the value of m_initialSizeToTransfer
       * Initial data volume to be migrated (needed by vmgr_gettape())
       * @param new_var the new value of m_initialSizeToTransfer
       */
      void setInitialSizeToTransfer(u_signed64 new_var) {
        m_initialSizeToTransfer = new_var;
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
       * Add a TapeCopy* object to the m_tapeCopyVector list
       */
      void addTapeCopy(TapeCopy* add_object) {
        m_tapeCopyVector.push_back(add_object);
      }

      /**
       * Remove a TapeCopy* object from m_tapeCopyVector
       */
      void removeTapeCopy(TapeCopy* remove_object) {
        for (unsigned int i = 0; i < m_tapeCopyVector.size(); i++) {
          TapeCopy* item = m_tapeCopyVector[i];
          if (item == remove_object) {
            std::vector<TapeCopy*>::iterator it = m_tapeCopyVector.begin() + i;
            m_tapeCopyVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of TapeCopy* objects held by m_tapeCopyVector
       * @return list of TapeCopy* objects held by m_tapeCopyVector
       */
      std::vector<TapeCopy*>& tapeCopy() {
        return m_tapeCopyVector;
      }

      /**
       * Get the value of m_tape
       * @return the value of m_tape
       */
      Tape* tape() const {
        return m_tape;
      }

      /**
       * Set the value of m_tape
       * @param new_var the new value of m_tape
       */
      void setTape(Tape* new_var) {
        m_tape = new_var;
      }

      /**
       * Get the value of m_tapePool
       * @return the value of m_tapePool
       */
      TapePool* tapePool() const {
        return m_tapePool;
      }

      /**
       * Set the value of m_tapePool
       * @param new_var the new value of m_tapePool
       */
      void setTapePool(TapePool* new_var) {
        m_tapePool = new_var;
      }

      /**
       * Get the value of m_status
       * @return the value of m_status
       */
      StreamStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(StreamStatusCodes new_var) {
        m_status = new_var;
      }

    private:

      /// Initial data volume to be migrated (needed by vmgr_gettape())
      u_signed64 m_initialSizeToTransfer;

      /// The id of this object
      u_signed64 m_id;

      std::vector<TapeCopy*> m_tapeCopyVector;

      Tape* m_tape;

      TapePool* m_tapePool;

      StreamStatusCodes m_status;

    }; /* end of class Stream */

  } /* end of namespace stager */

} /* end of namespace castor */

#endif // CASTOR_STAGER_STREAM_HPP
