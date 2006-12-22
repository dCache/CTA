/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/TapeAccessSpecification.hpp
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

#ifndef CASTOR_VDQM_TAPEACCESSSPECIFICATION_HPP
#define CASTOR_VDQM_TAPEACCESSSPECIFICATION_HPP

// Include Files
#include "castor/IObject.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace vdqm {

    /**
     * class TapeAccessSpecification
     * This table contains all existing tape models, the densities and their access
     * modes.  A tape drive can be indirectly associated with several entries, if it is
     * capable to support them. Please note that this table has only static entries!
     */
    class TapeAccessSpecification : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      TapeAccessSpecification() throw();

      /**
       * Empty Destructor
       */
      virtual ~TapeAccessSpecification() throw();

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
       * Get the value of m_accessMode
       * WRITE_ENABLE/WRITE_DISABLE from Ctape_constants.h
       * @return the value of m_accessMode
       */
      int accessMode() const {
        return m_accessMode;
      }

      /**
       * Set the value of m_accessMode
       * WRITE_ENABLE/WRITE_DISABLE from Ctape_constants.h
       * @param new_var the new value of m_accessMode
       */
      void setAccessMode(int new_var) {
        m_accessMode = new_var;
      }

      /**
       * Get the value of m_density
       * The density of a tape
       * @return the value of m_density
       */
      std::string density() const {
        return m_density;
      }

      /**
       * Set the value of m_density
       * The density of a tape
       * @param new_var the new value of m_density
       */
      void setDensity(std::string new_var) {
        m_density = new_var;
      }

      /**
       * Get the value of m_tapeModel
       * the model of a tape
       * @return the value of m_tapeModel
       */
      std::string tapeModel() const {
        return m_tapeModel;
      }

      /**
       * Set the value of m_tapeModel
       * the model of a tape
       * @param new_var the new value of m_tapeModel
       */
      void setTapeModel(std::string new_var) {
        m_tapeModel = new_var;
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

      /// WRITE_ENABLE/WRITE_DISABLE from Ctape_constants.h
      int m_accessMode;

      /// The density of a tape
      std::string m_density;

      /// the model of a tape
      std::string m_tapeModel;

      /// The id of this object
      u_signed64 m_id;

    }; // end of class TapeAccessSpecification

  }; // end of namespace vdqm

}; // end of namespace castor

#endif // CASTOR_VDQM_TAPEACCESSSPECIFICATION_HPP
