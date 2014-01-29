/******************************************************************************
 *                      Drive.hpp
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

#include "File.hpp"
#include "castor/exception/Errnum.hpp"

using namespace castor::tape::AULFile;
/**
 * Constructor of the AULReadSession. It will rewind the tape, and check the 
 * VSN value. Throws an exception in case of mismatch.
 * @param drive
 * @param VSN
 */
ReadSession::ReadSession(drives::Drive & drive, std::string VSN) throw (Exception) {
  castor::tape::drives::DriveGeneric & dg = drive;
  dg.rewind();
  
}

/**
 * Positions the tape for reading the file. Depending on the previous activity,
 * it is the duty of this function to determine how to best move to the next
 * file. The positioning will then be verified (header will be read). 
 * As usual, exception is thrown if anything goes wrong.
 * @param fileInfo: all relevant information passed by the stager about
 * the file.
 */
void ReadSession::position(Information fileInfo) throw (Exception) {
}

/**
 * After positioning at the beginning of a file for readings, this function
 * allows the reader to know which block sizes to provide.
 * If called before the end of a file read, the file reading will be 
 * interrupted and positioning to the new file will occur.
 * @return the block size in bytes.
 */
size_t ReadSession::getBlockSize() throw (Exception) {
  return 0;
}

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
size_t ReadSession::read(void * buff, size_t len) throw (Exception) {
  return 0;
}
