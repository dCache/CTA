/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/repack/FileListHelper.hpp
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
 * @(#)$RCSfile: FileListHelper.hpp,v $ $Revision: 1.9 $ $Release$ $Date: 2007/03/20 08:11:23 $ $Author: gtaur $
 *
 * The Filelisthelper offers some little functions for getting the file 
 * information for a tape.
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_REPACK_FILELISTHELPER_HPP
#define CASTOR_REPACK_FILELISTHELPER_HPP

// Include Files

#include "castor/repack/RepackCommonHeader.hpp"
#include "osdep.h"
#include "Cns_api.h"
#include <common.h>
#include <map>
#include <vector>

namespace castor {

  namespace repack {

    /**
     * class FileListHelper
     */
    class FileListHelper {

    public:

      /**
       * Contructor, which initialises the FilelistHelper 
       * with a nameserver a parameter
       * @param nameserver The Nameserver to contact for file information
       */
      FileListHelper(std::string nameserver);

      /**
       * Empty Destructor
       */
      virtual ~FileListHelper();

      /**
       * Returns a vector with a filelist with full pathname.
       * @param rreq The Request of the Tape
       * @return pointer to vector of u_signed64
       */
      std::vector<u_signed64>* getFileList(
                                    castor::repack::RepackSubRequest *sreq) ;

      /**
       * Fills the Request with the segments on the tape
       * elements
       * @param sreq The RepackSubRequest to fill
       * @return -1 in case of an error, the error Message is written to DLF!
       */
      int getFileListSegs(castor::repack::RepackSubRequest *sreq);
      
      /**
        * Retrieves the filenames for the fileids given in the passed
        * RepackSubRequest.
        * @param subreq The RepackSubRequest with the fileids
        * @return pointer to vector of strings 
        */
      std::vector<std::string>* getFilePathnames(castor::repack::RepackSubRequest *subreq)
	                                               throw (castor::exception::Exception);
 
       /**
        * Print the information of a file using the fileid and copyno.
        * @param fileid and copyno.
        * @return void
        */

      void printFileInfo(u_signed64 fileid, int copyno);
      private:
		/**
		 * The nameserver this Class contacts
		 */
		std::string m_ns;
		
    }; // end of class FileListHelper

  }; // end of namespace repack

}; // end of namespace castor

#endif // CASTOR_REPACK_FILELISTHELPER_HPP
