/******************************************************************************
 *                      StagerJob.hpp
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
 * some useful declarations around the scheduler job implementation
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#pragma once

// Include Files
#include <string>
#include "castor/exception/Exception.hpp"

namespace castor {

  // Forward declaration
  class IClient;
  namespace stager {
    class IJobSvc;
  }
  namespace rh {
    class IOResponse;
  }

  namespace job {

    namespace stagerjob {

      // Forward declaration
      class IPlugin;

      /**
       * Internal struct holding the context passed to a plugin
       */
      struct PluginContext {
        int socket;
        std::string host;
        int port;
        mode_t mask;
        std::string fullDestPath;
        int childPid;
        castor::stager::IJobSvc* jobSvc;
      };

      /**
       * DLF messages defined for the stager job
       */
      enum StagerJobDlfMessages {

        // System call errors
        CREATFAILED =     1, /* Failed to create empty file */
        FCLOSEFAILED =    2, /* Failed to close file */
        SCLOSEFAILED =    3, /* Failed to close socket */
        CHDIRFAILED =     4, /* Failed to change directory to tmp */
        DUP2FAILED =      5, /* Failed to duplicate socket */
        MOVERNOTEXEC =    6, /* Mover program cannot be executed. Check permissions */
        EXECFAILED =      7, /* Failed to exec mover */

        // Informative logs
        JOBSTARTED =      20, /* Job Started */
        JOBENDED =        21, /* Job finished successfully */
        JOBFAILED =       22, /* Job failed */
        JOBORIGCRED =     23, /* Credentials at start time */
        JOBACTCRED =      24, /* Actual credentials used */
        JOBNOOP =         25, /* No operation performed */
        FORKMOVER =       26, /* Forking mover */
        REQCANCELED =     27, /* Request canceled */
        MOVERPORT =       28, /* Mover will use the following port */
        MOVERFORK =       29, /* Mover fork uses the following command line */
        ACCEPTCONN =      30, /* Client connected */
        JOBFAILEDNOANS =  31, /* Job failed before it could send an answer to client */
        TERMINATEMOVER =  68, /* Mover process still running, sending signal */
        KILLMOVER =       69, /* Mover process still running after 10 seconds, killing process */

        // Errors
        STAT64FAIL =      40, /* stat64 error */
        NODATAWRITTEN =   49, /* No data transferred */
        UNLINKFAIL =      50, /* unlink error */
        CHILDEXITED =     41, /* Child exited */
        CHILDSIGNALED =   42, /* Child exited due to uncaught signal */
        CHILDSTOPPED =    43, /* Child was stopped */
        NOANSWERSENT =    52, /* Could not send answer to client */
        GETATTRFAILED =   53, /* Failed to get checksum information from extended attributes */
        CSTYPENOTSOP =    54, /* Unsupported checksum type, ignoring checksum information */
        CLEANUPFAILED =   70, /* Unable to clean up stager DB for failed job */

        // Protocol specific. Should not be here if the plugins
        // were properly packaged in separate libs
        GSIBADPORT =      44, /* Invalid port range for GridFTP in config file, using default */
        GSIBADMINPORT =   45, /* Invalid lower bound for GridFTP port range in config file, using default */
        GSIBADMAXPORT =   46, /* Invalid upper bound for GridFTP port range in config file, using default */
        GSIBADMINVAL =    47, /* Lower bound for GridFTP port range not in valid range, using default */
        GSIBADMAXVAL =    48, /* Upper bound for GridFTP port range not in valid range, using default */
        GSIBADTIMEOUT =   66, /* Invalid value for GSIFTP/TIMEOUT option, using default */

        XROOTENOENT =     51, /* Xrootd is not installed */
        XROOTBADTIMEOUT = 67, /* Invalid value for XROOT/TIMEOUTS option, using default */
        XROOTFILEUPDATE = 71, /* File update detected, changing access mode to write */
       
        RFIODBADPORT =    55, /* Invalid port range for RFIOD in config file, using default */
        RFIODBADMINPORT = 56, /* Invalid lower bound for RFIOD port range in config file, using default */
        RFIODBADMAXPORT = 57, /* Invalid upper bound for RFIOD port range in config file, using default */
        RFIODBADMINVAL =  58, /* Lower bound for RFIOD port range not in valid range, using default */
        RFIODBADMAXVAL =  59, /* Upper bound for RFIOD port range not in valid range, using default */

        ROOTDBADPORT =    60, /* Invalid port range for ROOT in config file, using default */
        ROOTDBADMINPORT = 61, /* Invalid lower bound for ROOT port range in config file, using default */
        ROOTDBADMAXPORT = 62, /* Invalid upper bound for ROOT port range in config file, using default */
        ROOTDBADMINVAL =  63, /* Lower bound for ROOT port range not in valid range, using default */
        ROOTDBADMAXVAL =  64, /* Upper bound for ROOT port range not in valid range, using default */
        ROOTDBADTIMEOUT = 65, /* Invalid value for ROOT/TIMEOUT option, using default */
        ROOTRDWRSTATERR = 72  /* Failed to determine file modification time, file may not be closed properly */
      };

      /**
       * Gets the plugin for a given protocol
       * @param protocol the requested protocol
       */
      IPlugin* getPlugin(std::string protocol)
        throw (castor::exception::Exception);

      /**
       * Registers a new plugin
       * @param protocol the associatedprotocol
       * @param protocol the plugin to register
       */
      void registerPlugin(std::string protocol,
                          IPlugin* plugin)
        throw();

      /**
       * Sends a given response to a given client
       * @param client the client waiting for the response
       * @param response the response to be sent
       * @return whether the child failed
       */
      void sendResponse(castor::IClient *client,
                        castor::rh::IOResponse &response)
        throw (castor::exception::Exception);

    } // end of namespace stagerjob

  } // end of namespace job

} // end of namespace castor

