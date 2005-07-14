/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/ErrorHistory.hpp
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

#ifndef CASTOR_VDQM_ERRORHISTORY_HPP
#define CASTOR_VDQM_ERRORHISTORY_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  // Forward declarations
  namespace stager {

    // Forward declarations
    class Tape;

  }; // end of namespace stager

  namespace vdqm {

    // Forward declarations
    class TapeDrive;

    /**
     * class ErrorHistory
     * This table holds all the errors, which occures for a specified tape drive with a
     * dedicated tape.
     */
    class ErrorHistory : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      ErrorHistory() throw();

      /**
       * Empty Destructor
       */
      virtual ~ErrorHistory() throw();

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
       * Get the value of m_errorMessage
       * The error message, which was thrown for the specified tape drive with the
       * specified mounted tape.
       * @return the value of m_errorMessage
       */
      std::string errorMessage() const {
        return m_errorMessage;
      }

      /**
       * Set the value of m_errorMessage
       * The error message, which was thrown for the specified tape drive with the
       * specified mounted tape.
       * @param new_var the new value of m_errorMessage
       */
      void setErrorMessage(std::string new_var) {
        m_errorMessage = new_var;
      }

      /**
       * Get the value of m_timeStamp
       * The time, when the error occured
       * @return the value of m_timeStamp
       */
      int timeStamp() const {
        return m_timeStamp;
      }

      /**
       * Set the value of m_timeStamp
       * The time, when the error occured
       * @param new_var the new value of m_timeStamp
       */
      void setTimeStamp(int new_var) {
        m_timeStamp = new_var;
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
       * Get the value of m_tapeDrive
       * @return the value of m_tapeDrive
       */
      TapeDrive* tapeDrive() const {
        return m_tapeDrive;
      }

      /**
       * Set the value of m_tapeDrive
       * @param new_var the new value of m_tapeDrive
       */
      void setTapeDrive(TapeDrive* new_var) {
        m_tapeDrive = new_var;
      }

      /**
       * Get the value of m_tape
       * @return the value of m_tape
       */
      castor::stager::Tape* tape() const {
        return m_tape;
      }

      /**
       * Set the value of m_tape
       * @param new_var the new value of m_tape
       */
      void setTape(castor::stager::Tape* new_var) {
        m_tape = new_var;
      }

    private:

      /// The error message, which was thrown for the specified tape drive with the specified mounted tape.
      std::string m_errorMessage;

      /// The time, when the error occured
      int m_timeStamp;

      /// The id of this object
      u_signed64 m_id;

      TapeDrive* m_tapeDrive;

      castor::stager::Tape* m_tape;

    }; // end of class ErrorHistory

  }; // end of namespace vdqm

}; // end of namespace castor

#endif // CASTOR_VDQM_ERRORHISTORY_HPP
