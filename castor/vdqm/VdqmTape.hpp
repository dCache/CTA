/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/vdqm/VdqmTape.hpp
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

#ifndef CASTOR_VDQM_VDQMTAPE_HPP
#define CASTOR_VDQM_VDQMTAPE_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/vdqm/TapeStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace vdqm {

    /**
     * class VdqmTape
     */
    class VdqmTape : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      VdqmTape() throw();

      /**
       * Empty Destructor
       */
      virtual ~VdqmTape() throw();

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
       * Tape visual identifier
       * @return the value of m_vid
       */
      std::string vid() const {
        return m_vid;
      }

      /**
       * Set the value of m_vid
       * Tape visual identifier
       * @param new_var the new value of m_vid
       */
      void setVid(std::string new_var) {
        m_vid = new_var;
      }

      /**
       * Get the value of m_side
       * Side (for future support of two sided media)
       * @return the value of m_side
       */
      int side() const {
        return m_side;
      }

      /**
       * Set the value of m_side
       * Side (for future support of two sided media)
       * @param new_var the new value of m_side
       */
      void setSide(int new_var) {
        m_side = new_var;
      }

      /**
       * Get the value of m_tpmode
       * Tape access mode (WRITE_DISABLE or WRITE_ENABLE)
       * @return the value of m_tpmode
       */
      int tpmode() const {
        return m_tpmode;
      }

      /**
       * Set the value of m_tpmode
       * Tape access mode (WRITE_DISABLE or WRITE_ENABLE)
       * @param new_var the new value of m_tpmode
       */
      void setTpmode(int new_var) {
        m_tpmode = new_var;
      }

      /**
       * Get the value of m_errMsgTxt
       * @return the value of m_errMsgTxt
       */
      std::string errMsgTxt() const {
        return m_errMsgTxt;
      }

      /**
       * Set the value of m_errMsgTxt
       * @param new_var the new value of m_errMsgTxt
       */
      void setErrMsgTxt(std::string new_var) {
        m_errMsgTxt = new_var;
      }

      /**
       * Get the value of m_errorCode
       * RTCOPY serrno if status == SEGMENT_FAILED
       * @return the value of m_errorCode
       */
      int errorCode() const {
        return m_errorCode;
      }

      /**
       * Set the value of m_errorCode
       * RTCOPY serrno if status == SEGMENT_FAILED
       * @param new_var the new value of m_errorCode
       */
      void setErrorCode(int new_var) {
        m_errorCode = new_var;
      }

      /**
       * Get the value of m_severity
       * @return the value of m_severity
       */
      int severity() const {
        return m_severity;
      }

      /**
       * Set the value of m_severity
       * @param new_var the new value of m_severity
       */
      void setSeverity(int new_var) {
        m_severity = new_var;
      }

      /**
       * Get the value of m_vwAddress
       * Vid worker address for killing requests
       * @return the value of m_vwAddress
       */
      std::string vwAddress() const {
        return m_vwAddress;
      }

      /**
       * Set the value of m_vwAddress
       * Vid worker address for killing requests
       * @param new_var the new value of m_vwAddress
       */
      void setVwAddress(std::string new_var) {
        m_vwAddress = new_var;
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
       * Get the value of m_status
       * @return the value of m_status
       */
      TapeStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(TapeStatusCodes new_var) {
        m_status = new_var;
      }

    private:

      /// Tape visual identifier
      std::string m_vid;

      /// Side (for future support of two sided media)
      int m_side;

      /// Tape access mode (WRITE_DISABLE or WRITE_ENABLE)
      int m_tpmode;

      std::string m_errMsgTxt;

      /// RTCOPY serrno if status == SEGMENT_FAILED
      int m_errorCode;

      int m_severity;

      /// Vid worker address for killing requests
      std::string m_vwAddress;

      /// The id of this object
      u_signed64 m_id;

      TapeStatusCodes m_status;

    }; /* end of class VdqmTape */

  } /* end of namespace vdqm */

} /* end of namespace castor */

#endif // CASTOR_VDQM_VDQMTAPE_HPP
