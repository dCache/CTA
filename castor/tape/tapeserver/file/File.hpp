/******************************************************************************
 *                      File.hpp
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
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once
#include <string>
#include "Structures.hpp"
#include "../exception/Exception.hpp"

namespace castor {
  namespace tape {
    /**
     * Class managing the reading and writing of files to and from tape.
     */
    namespace AULFile {
      /**
       * Class containing all the information related to a file being migrated to 
       * tape.
       */
      class Information { //no information about path and filename here as it cannot be used nor checked on tape
      public:
        uint32_t checksum;
        uint64_t nsFileId;
        uint64_t size;
        uint32_t fseq; //this is the payload (i.e. real file) sequence number, not the tape file sequence number (which would include headers and trailers as well)
        uint32_t blockId;
      };

      class BufferTooSmall: public Exception {
      public:
        BufferTooSmall(const std::string & what): Exception(what) {}
      };

      class WrongChecksum: public Exception {
      public:
        WrongChecksum(const std::string & what): Exception(what) {}
      };

      class WrongSize: public Exception {
      public:
        WrongSize(const std::string & what): Exception(what) {}
      };

      class NotReadingAFile: public Exception {
      public:
        NotReadingAFile(const std::string & what): Exception(what) {}
      };
      
      class TapeFormatError: public Exception {
      public:
        TapeFormatError(const std::string & what): Exception(what) {}
      };
      
      class TapeMediaError: public Exception {
      public:
        TapeMediaError(const std::string & what): Exception(what) {}
      };

      /**
       * Class keeping track of a whole tape read session over an AUL formated
       * tape. The session will keep track of the overall coherency of the session
       * and check for everything to be coherent. The tape should be mounted in
       * the drive before the AULReadSession is started (i.e. constructed).
       * Likewise, tape unmount is the business of the user.
       */
      class ReadSession{
      public:
        /**
         * Constructor of the AULReadSession. It will rewind the tape, and check the 
         * VSN value. Throws an exception in case of mismatch.
         * @param drive
         * @param VSN
         */
        ReadSession(drives::DriveGeneric & dg, std::string volId) throw (Exception);
        
        /**
         * Positions the tape for reading the file. Depending on the previous activity,
         * it is the duty of this function to determine how to best move to the next
         * file. The positioning will then be verified (header will be read). 
         * As usual, exception is thrown if anything goes wrong.
         * @param fileInfo: all relevant information passed by the stager about
         * the file.
         */
        void position(const Information &fileInfo) throw (Exception);
        
        /**
         * After positioning at the beginning of a file for readings, this function
         * allows the reader to know which block sizes to provide.
         * @return the block size in bytes.
         */
        size_t getBlockSize() throw (Exception);
        
        /**
         * Read data from the file. The buffer should equal to or bigger than the 
         * block size. Will try to actually fill up the provided buffer (this
         * function can trigger several read on the tape side).
         * This function will throw exceptions when problems arise (especially
         * at end of file in case of size or checksum mismatch.
         * After end of file, a new call to read without a call to position
         * will throw NotReadingAFile.
         * @param buff pointer to the data buffer
         * @param len size of the buffer
         * @return The amount of data actually copied. Zero at end of file.
         */
        size_t read(void * buff, size_t len) throw (Exception);
      
      private:  
        /**
         * checks the volume label to make sure the label is valid and that we
         * have the correct tape (checks VSN). Leaves the tape at the end of the
         * first header block (i.e. right before the first data block) in case
         * of success, or rewinds the tape in case of volume label problems.
         * Might also leave the tape in unknown state in case any of the st
         * operations fail.
         */
        void checkVOL1() throw (Exception);
        
        /**
         * DriveGeneric object referencing the drive used during this read session
         */
        drives::DriveGeneric & dg;
        std::string VSN;
        size_t current_block_size;
      };

      /**
       TODO
       */
      class WriteSession{

      };
    };
  }
}
