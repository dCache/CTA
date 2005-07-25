/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeDriveCompability.hpp
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

#ifndef CASTOR_VDQM_TAPEDRIVECOMPABILITY_HPP
#define CASTOR_VDQM_TAPEDRIVECOMPABILITY_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace vdqm {

    // Forward declarations
    class TapeAccessSpecification;

    /**
     * class TapeDriveCompability
     * A static table which specifies for a tape drive model the priority list for tape
     * access
     */
    class TapeDriveCompability : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      TapeDriveCompability() throw();

      /**
       * Empty Destructor
       */
      virtual ~TapeDriveCompability() throw();

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
       * Get the value of m_tapeDriveModel
       * The tape drive model
       * @return the value of m_tapeDriveModel
       */
      std::string tapeDriveModel() const {
        return m_tapeDriveModel;
      }

      /**
       * Set the value of m_tapeDriveModel
       * The tape drive model
       * @param new_var the new value of m_tapeDriveModel
       */
      void setTapeDriveModel(std::string new_var) {
        m_tapeDriveModel = new_var;
      }

      /**
       * Get the value of m_priorityLevel
       * The priority Level for the specific TapeAccessSpecification
       * @return the value of m_priorityLevel
       */
      int priorityLevel() const {
        return m_priorityLevel;
      }

      /**
       * Set the value of m_priorityLevel
       * The priority Level for the specific TapeAccessSpecification
       * @param new_var the new value of m_priorityLevel
       */
      void setPriorityLevel(int new_var) {
        m_priorityLevel = new_var;
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
       * Get the value of m_tapeAccessSpecifications
       * @return the value of m_tapeAccessSpecifications
       */
      TapeAccessSpecification* tapeAccessSpecifications() const {
        return m_tapeAccessSpecifications;
      }

      /**
       * Set the value of m_tapeAccessSpecifications
       * @param new_var the new value of m_tapeAccessSpecifications
       */
      void setTapeAccessSpecifications(TapeAccessSpecification* new_var) {
        m_tapeAccessSpecifications = new_var;
      }

    private:

      /// The tape drive model
      std::string m_tapeDriveModel;

      /// The priority Level for the specific TapeAccessSpecification
      int m_priorityLevel;

      /// The id of this object
      u_signed64 m_id;

      TapeAccessSpecification* m_tapeAccessSpecifications;

    }; // end of class TapeDriveCompability

  }; // end of namespace vdqm

}; // end of namespace castor

#endif // CASTOR_VDQM_TAPEDRIVECOMPABILITY_HPP
