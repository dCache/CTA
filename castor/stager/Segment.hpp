/******************************************************************************
 *                      castor/stager/Segment.hpp
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

#ifndef CASTOR_STAGER_SEGMENT_HPP
#define CASTOR_STAGER_SEGMENT_HPP

// Include Files
#include "castor/IObject.hpp"
#include "castor/stager/SegmentStatusCodes.hpp"
#include "osdep.h"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class TapeCopy;
    class Tape;

    /**
     * class Segment
     * Segment Information  The Segment object contains all tape file (segment)
     * information required to be stored in the stager request catalog. The remaining
     * tape file parameters such as record format and blocksize are either defined by
     * defaults or taken from the tape label.
     */
    class Segment : public virtual castor::IObject {

    public:

      /**
       * Empty Constructor
       */
      Segment() throw();

      /**
       * Empty Destructor
       */
      virtual ~Segment() throw();

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
       * Sets the id of the object
       * @param id The new id
       */
      virtual void setId(u_signed64 id);

      /**
       * gets the id of the object
       */
      virtual u_signed64 id() const;

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
       * Get the value of m_blockid
       * @return the value of m_blockid
       */
      const unsigned char* blockid() const {
        return m_blockid;
      }

      /**
       * Set the value of m_blockid
       * @param new_var the new value of m_blockid
       */
      void setBlockid(const unsigned char* new_var) {
        memcpy(m_blockid, new_var, 4 * sizeof(unsigned char));
      }

      /**
       * Get the value of m_fseq
       * Tape media file sequence number (if no locate)
       * @return the value of m_fseq
       */
      int fseq() const {
        return m_fseq;
      }

      /**
       * Set the value of m_fseq
       * Tape media file sequence number (if no locate)
       * @param new_var the new value of m_fseq
       */
      void setFseq(int new_var) {
        m_fseq = new_var;
      }

      /**
       * Get the value of m_offset
       * Start offset in the disk file (if segmented file)
       * @return the value of m_offset
       */
      u_signed64 offset() const {
        return m_offset;
      }

      /**
       * Set the value of m_offset
       * Start offset in the disk file (if segmented file)
       * @param new_var the new value of m_offset
       */
      void setOffset(u_signed64 new_var) {
        m_offset = new_var;
      }

      /**
       * Get the value of m_bytes_in
       * Size of the tape file (segment) to be copied
       * @return the value of m_bytes_in
       */
      u_signed64 bytes_in() const {
        return m_bytes_in;
      }

      /**
       * Set the value of m_bytes_in
       * Size of the tape file (segment) to be copied
       * @param new_var the new value of m_bytes_in
       */
      void setBytes_in(u_signed64 new_var) {
        m_bytes_in = new_var;
      }

      /**
       * Get the value of m_bytes_out
       * @return the value of m_bytes_out
       */
      u_signed64 bytes_out() const {
        return m_bytes_out;
      }

      /**
       * Set the value of m_bytes_out
       * @param new_var the new value of m_bytes_out
       */
      void setBytes_out(u_signed64 new_var) {
        m_bytes_out = new_var;
      }

      /**
       * Get the value of m_host_bytes
       * @return the value of m_host_bytes
       */
      u_signed64 host_bytes() const {
        return m_host_bytes;
      }

      /**
       * Set the value of m_host_bytes
       * @param new_var the new value of m_host_bytes
       */
      void setHost_bytes(u_signed64 new_var) {
        m_host_bytes = new_var;
      }

      /**
       * Get the value of m_segmCksumAlgorithm
       * @return the value of m_segmCksumAlgorithm
       */
      std::string segmCksumAlgorithm() const {
        return m_segmCksumAlgorithm;
      }

      /**
       * Set the value of m_segmCksumAlgorithm
       * @param new_var the new value of m_segmCksumAlgorithm
       */
      void setSegmCksumAlgorithm(std::string new_var) {
        m_segmCksumAlgorithm = new_var;
      }

      /**
       * Get the value of m_segmCksum
       * @return the value of m_segmCksum
       */
      unsigned long segmCksum() const {
        return m_segmCksum;
      }

      /**
       * Set the value of m_segmCksum
       * @param new_var the new value of m_segmCksum
       */
      void setSegmCksum(unsigned long new_var) {
        m_segmCksum = new_var;
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
       * Get the value of m_copy
       * @return the value of m_copy
       */
      TapeCopy* copy() const {
        return m_copy;
      }

      /**
       * Set the value of m_copy
       * @param new_var the new value of m_copy
       */
      void setCopy(TapeCopy* new_var) {
        m_copy = new_var;
      }

      /**
       * Get the value of m_status
       * @return the value of m_status
       */
      SegmentStatusCodes status() const {
        return m_status;
      }

      /**
       * Set the value of m_status
       * @param new_var the new value of m_status
       */
      void setStatus(SegmentStatusCodes new_var) {
        m_status = new_var;
      }

    private:

    private:

      unsigned char m_blockid[4];

      /// Tape media file sequence number (if no locate)
      int m_fseq;

      /// Start offset in the disk file (if segmented file)
      u_signed64 m_offset;

      /// Size of the tape file (segment) to be copied
      u_signed64 m_bytes_in;

      u_signed64 m_bytes_out;

      u_signed64 m_host_bytes;

      std::string m_segmCksumAlgorithm;

      unsigned long m_segmCksum;

      std::string m_errMsgTxt;

      /// RTCOPY serrno if status == SEGMENT_FAILED
      int m_errorCode;

      int m_severity;

      /// The id of this object
      u_signed64 m_id;

      Tape* m_tape;

      TapeCopy* m_copy;

      SegmentStatusCodes m_status;

    }; // end of class Segment

  }; // end of namespace stager

}; // end of namespace castor

#endif // CASTOR_STAGER_SEGMENT_HPP
