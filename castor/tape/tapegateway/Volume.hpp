/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/tape/tapegateway/Volume.hpp
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

#ifndef CASTOR_TAPE_TAPEGATEWAY_VOLUME_HPP
#define CASTOR_TAPE_TAPEGATEWAY_VOLUME_HPP

// Include Files
#include "castor/tape/tapegateway/GatewayMessage.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  namespace tape {

    namespace tapegateway {

      /**
       * class Volume
       */
      class Volume : public virtual GatewayMessage {

      public:

        /**
         * Empty Constructor
         */
        Volume() throw();

        /**
         * Empty Destructor
         */
        virtual ~Volume() throw();

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
         * Get the value of m_vid
         * @return the value of m_vid
         */
        std::string vid() const {
          return m_vid;
        }

        /**
         * Set the value of m_vid
         * @param new_var the new value of m_vid
         */
        void setVid(std::string new_var) {
          m_vid = new_var;
        }

        /**
         * Get the value of m_mode
         * @return the value of m_mode
         */
        int mode() const {
          return m_mode;
        }

        /**
         * Set the value of m_mode
         * @param new_var the new value of m_mode
         */
        void setMode(int new_var) {
          m_mode = new_var;
        }

        /**
         * Get the value of m_density
         * @return the value of m_density
         */
        std::string density() const {
          return m_density;
        }

        /**
         * Set the value of m_density
         * @param new_var the new value of m_density
         */
        void setDensity(std::string new_var) {
          m_density = new_var;
        }

        /**
         * Get the value of m_label
         * @return the value of m_label
         */
        std::string label() const {
          return m_label;
        }

        /**
         * Set the value of m_label
         * @param new_var the new value of m_label
         */
        void setLabel(std::string new_var) {
          m_label = new_var;
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

        std::string m_vid;

        int m_mode;

        std::string m_density;

        std::string m_label;

        /// The id of this object
        u_signed64 m_id;

      }; /* end of class Volume */

    } /* end of namespace tapegateway */

  } /* end of namespace tape */

} /* end of namespace castor */

#endif // CASTOR_TAPE_TAPEGATEWAY_VOLUME_HPP
