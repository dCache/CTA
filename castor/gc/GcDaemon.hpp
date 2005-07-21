/******************************************************************************
 *                      GcDaemon.hpp
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
 * @(#)GcDaemon.hpp,v 1.2 $Release$ 2005/03/16 10:37:01 jiltsov
 *
 * Garbage collector daemon handling the deletion of local
 * files on a filesystem. Makes remote calls to the stager's
 * GC service to know what to delete and to update the catalog
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef GC_GCDAEMON_HPP
#define GC_GCDAEMON_HPP 1

// Include Files
#include "castor/BaseObject.hpp"
#include "castor/exception/Exception.hpp"
#include "Cuuid.h"

namespace castor {

  namespace gc {

    /**
     * Castor garbage collection daemon.
     */
    class GcDaemon : public BaseObject {

    public:

      /**
       * constructor
       */
      GcDaemon();

      /*
       * destructor
       */
      virtual ~GcDaemon() throw();

      /**
       * Starts the server, as a daemon
       */
      virtual int start()
        throw (castor::exception::Exception);

      /**
       * parses a command line to set the server oprions
       */
      void 	GCparseCommandLine(int argc, char *argv[]);

    private:

      /**
       * get size (in bytes) of a file
       * @param gcfilepath the path of the file
       * @return the size (in bytes) of the file
       * @exception when the internal stat64 failed
       */
      u_signed64 gcGetFileSize (std::string& gcfilepath)
	throw (castor::exception::Exception);

      /**
       * actually remove the file from local filesystem.
       * @param gcfilepath the path of the file to remove
       * @return the size (in bytes) of the removed file
       * @exception when the removing failed
       */
      u_signed64 gcRemoveFilePath (std::string gcfilepath)
	throw (castor::exception::Exception);

    private:
      /**
       * UUID of the server (for log purposes)
       */
      Cuuid_t m_uuid;

      /**
       * Flag indicating whether the server should 
       * run in foreground or background mode.
       */
      bool m_foreground;

    };

  } // end of namespace gc

} // end of namespace castor

#endif // GC_GCDAEMON_HPP
