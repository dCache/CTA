/******************************************************************************
 *                      RepackServer.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2004  CERN
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
 * @(#)$RCSfile: RepackServer.cpp,v $ $Revision: 1.13 $ $Release$ $Date: 2006/06/02 09:24:12 $ $Author: felixehm $
 *
 *
 *
 * @author Felix Ehm 
 *****************************************************************************/

// Include Files
#include "castor/repack/RepackServer.hpp"





//------------------------------------------------------------------------------
// main method
//------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

	try {

    castor::repack::RepackServer server;
	
    server.addThreadPool(
      new castor::server::ListenerThreadPool("Worker", new castor::repack::RepackWorker(), server.getListenPort()));
	  server.getThreadPool('W')->setNbThreads(1);
	
    server.addThreadPool(
      new castor::server::SignalThreadPool("Stager", new castor::repack::RepackFileStager(&server) ));
	  server.getThreadPool('S')->setNbThreads(1);
	  
	  server.addThreadPool(
      new castor::server::SignalThreadPool("Monitor", new castor::repack::RepackMonitor(&server),0, 60 ));
	  server.getThreadPool('M')->setNbThreads(1);
    
	  server.addThreadPool(
        new castor::server::SignalThreadPool("Cleaner", new castor::repack::RepackCleaner(&server),60 ));
	  server.getThreadPool('C')->setNbThreads(1);
   
    
    server.parseCommandLine(argc, argv);
    server.start();
    } catch (castor::exception::Internal i){
    	std::cerr << "Caught castor internal exception : "
			<< sstrerror(i.code()) << std::endl
			<< i.getMessage().str() << std::endl;
    }
	catch (castor::exception::Exception e) {
			std::cerr << "Caught castor exception : "
			<< sstrerror(e.code()) << std::endl
			<< e.getMessage().str() << std::endl;
	}
	catch (...) {
		std::cerr << "Caught general exception!" << std::endl;
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
// RepackServer Constructor
// also initialises the logging facility
//------------------------------------------------------------------------------
castor::repack::RepackServer::RepackServer() : 
	castor::server::BaseDaemon("RepackServer")
{
  // Initializes the DLF logging. This includes
  // defining the predefined messages
  
  castor::BaseObject::initLog("Repack", castor::SVC_STDMSG);
  castor::dlf::Message messages[] =
    {{ 0, " - "},
     { 1, "New Request Arrival"},
     { 2, "Could not get Conversion Service for Database"},
     { 3, "Could not get Conversion Service for Streaming"},
     { 4, "Exception caught : server is stopping"},
     { 5, "Exception caught : ignored"},
     { 6, "Invalid Request"},
     { 7, "Unable to read Request from socket"},
     { 8, "Processing Request"},
     { 9, "Exception caught"},
     {10, "Sending reply to client"},		// remove ?
     {11, "Unable to send Ack to client"},
     {12, "DatabaseHelper: Request stored in DB"},
     {13, "DatabaseHelper: Unable to store Request in DB"},
     {14, "FileListHelper: Fetching files from Nameserver"},							// FileListHelper::getFileList()
     {15, "FileListHelper: Cannot get file pathname"},									// FileListHelper::getFilePathnames
     {16, "RepackWorker : No such Tape!"},													// RepackWorker:getTapeInfo()
     {17, "RepackWorker : Tape has unkown status, repack abort for this tape!"},// RepackWorker:getTapeInfo()
     {18, "RepackWorker : Tape is marked as FREE, no repack to be done"},		// RepackWorker:getTapeInfo()
     {19, "RepackWorker : No such pool!"},													// RepackWorker:getPoolInfo()
     {20, "RepackWorker : Adding tapes for pool repacking!"},							// RepackWorker:getPoolInfo()
     {21, "RepackFileStager: Unable to stage files!"},									// RepackFileStager:stage_files
     {22, "RepackFileStager: New request for staging files"},							// RepackFileStager:run()
     {23, "RepackFileStager: Not enough space for this RepackRequest. Skipping..."},// RepackFileStager:stage_files
     {24, "FileListHelper: Retrieved segs for SubRequest."},							//FileListHelper:getFileListSegs()
     {25, "RepackFileStager: Updating Request to STAGING and add its segs."},		// RepackFileStager:stage_files
     {36, "RepackFileStager: No files found on tape."},               // RepackFileStager:stage_files
     {26, "RepackFileStager: Staging files."},												// RepackFileStager:stage_files
     {27, "DatabaseHelper: Unable to update SubRequest!"},
     {28, "DatabaseHelper: Tape already in repack que!"},
     {29, "RepackFileStager: Getting Segs for SubRequest!"},
     {30, "DatabaseHelper: SubRequest updated!"},
     {33, "RepackFileStager: Changing CUUID to stager one"},
     {34, "RepackCleaner: No files found for cleanup phase"},
     {35, "RepackCleaner: Cleaner started"},
     {40, "RepackMonitor: Changing status"},
     {41, "RepackMonitor: Stager query failed"},
     {99, "TODO::MESSAGE"},
     {-1, ""}};
  castor::dlf::dlf_init("Repack", messages);
  
  
  /* --------------------------------------------------------------------- */
  /** This gets the repack port configuration in the enviroment, this 
      is the only on, which is taken from the enviroment */
  char* tmp;

  if ( (tmp = getenv ("REPACK_PORT")) != 0 ) {
      char* dp = tmp;
      m_listenPort = strtoul(tmp, &dp, 0);
      
      if (*dp != 0) {
        castor::exception::Internal ex;
        ex.getMessage() << "Bad port value in enviroment variable " 
                        << tmp << std::endl;
        throw ex;
      }
      if ( m_listenPort > 65535 ){
        castor::exception::Internal ex;
        ex.getMessage() << "Given port no. in enviroment variable "
                        << "exceeds 65535 !"<< std::endl;
        throw ex;
    }
  }
  else
    m_listenPort = castor::repack::CSP_REPACKSERVER_PORT;
  free(tmp);

  /* --------------------------------------------------------------------- */

  /** the RepackServer reads the configuration for Nameserver, stager etc.
      at the beginning and keeps the information for the threads
    */
  char* tmp2;
  if ( !(tmp2 = getconfent("CNS", "HOST",0)) ){
    castor::exception::Internal ex;
    ex.getMessage() << "Unable to initialise RepackServer with nameserver "
                    << "entry in castor config file" << std::endl;
    throw ex; 
  }
  m_ns = new std::string(tmp2);


  /* --------------------------------------------------------------------- */

  /** the stager name 
  */
  if ( !(tmp2 = getconfent("STAGER", "HOST",0)) ){
    castor::exception::Internal ex;
    ex.getMessage() << "Unable to initialise RepackServer with stager "
                    << "entry in castor config file" << std::endl;
    throw ex; 
  }
  m_stager = new std::string(tmp2);


  /* --------------------------------------------------------------------- */

  /** Get the repack service class, which is used for staging in files 
    * The diskpool in this service class is used for recall process.
    */
  if ( !(tmp2 = getconfent("REPACK", "SVCCLASS",0)) ){
    castor::exception::Internal ex;
    ex.getMessage() << "Unable to initialise RepackServer with service class "
                    << "entry in castor config file" << std::endl;
    throw ex; 
  }
  m_serviceClass = new std::string(tmp2);
  

  /* --------------------------------------------------------------------- */

  /** Get the repack service class, which is used for staging in files 
    * The diskpool in this service class is used for recall process.
    */
  if ( !(tmp2 = getconfent("REPACK", "PROTOCOL",0)) ){
    castor::exception::Internal ex;
    ex.getMessage() << "Unable to initialise RepackServer with protocol "
                    << "entry in castor config file" << std::endl;
    throw ex; 
  }
  m_protocol = new std::string(tmp2);
  /* --------------------------------------------------------------------- */

}




castor::repack::RepackServer::~RepackServer() throw()
{
    if ( m_ns != NULL ) delete m_ns;
    if ( m_stager != NULL ) delete m_stager;
    if ( m_serviceClass != NULL ) delete m_serviceClass;
    if ( m_protocol != NULL ) delete m_protocol;
}







