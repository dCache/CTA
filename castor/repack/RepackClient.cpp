/******************************************************************************
 *                      RepackClient.cpp
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
 * @(#)$RCSfile: RepackClient.cpp,v $ $Revision: 1.13 $ $Release$ $Date: 2006/05/22 07:05:33 $ $Author: felixehm $
 *
 * The Repack Client.
 * Creates a RepackRequest and send it to the Repack server, specified in the 
 * castor config file, or from the enviroment.
 * 
 * For the enviroment serveral possibilities are possible:
 * REPACK_HOST and REPACK_HOST_ALT , the REPACK_HOST_ALT  is an alternative, if 
 * no server entry is found in the config file or REPACK_HOST is not set.
 * REPACK_PORT and REPACK_PORT_ALT : same as for the REPACK_HOST
 *
 * 
 * @author Felix Ehm
 *****************************************************************************/


/* Client  includes */
#include "castor/repack/RepackClient.hpp"



/** 
 * By including the Header file, the Factory is automatically active !! 
 */
#include "castor/io/StreamRepackRequestCnv.hpp"
#include "castor/io/StreamRepackSubRequestCnv.hpp"
#include "castor/io/StreamRepackAckCnv.hpp"






//------------------------------------------------------------------------------
// main method
//------------------------------------------------------------------------------
int main(int argc, char *argv[]) 
{
   castor::repack::RepackClient *client = new castor::repack::RepackClient();
   try{
      client->run(argc, argv);
   } 
   catch (castor::exception::Exception ex) {
      std::cout << ex.getMessage().str() << std::endl;
   }

}



namespace castor {

 namespace repack {


  const char* HOST_ENV_ALT = "REPACK_HOST_ALT";
  const char* HOST_ENV = "REPACK_HOST";
  const char* PORT_ENV_ALT = "REPACK_PORT_ALT";
  const char* PORT_ENV = "REPACK_PORT";
  const char* CATEGORY_CONF = "REPACK";
  const char* HOST_CONF = "HOST";
  const char* PORT_CONF = "PORT";

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
RepackClient::RepackClient()
{
  /* the default server port */
  m_defaultport = CSP_REPACKSERVER_PORT;
  /* the default repackserver host */
  m_defaulthost = "localhost";
  std::string clientname = "RepackClient";

  cp.vid =  NULL;
  cp.pool = NULL;

  svc = svcs()->cnvService("StreamCnvSvc", castor::SVC_STREAMCNV);
  if (0 == svc) {
      // "Could not get Conversion Service for Streaming" message
	std::cerr << "Could not get Conversion Service for Streaming" << std::endl;
	return;
  }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
RepackClient::~RepackClient() throw()
{
  svc->release();
}





//------------------------------------------------------------------------------
// parseInput
//------------------------------------------------------------------------------
bool RepackClient::parseInput(int argc, char** argv)
{
  const char* cmdParams = "S:sR:V:P:h";
  if (argc == 1){
    usage();
    return false;
  }
  struct Coptions longopts[] = {
    {"status", 0,NULL, 's' },
	 {"status", REQUIRED_ARGUMENT,NULL, 'S' },
    {"delete", REQUIRED_ARGUMENT,NULL, 'R' },
    {"volumeid", REQUIRED_ARGUMENT, NULL, 'V'},
    {"poolid", REQUIRED_ARGUMENT, NULL, 'P'},
    /*{"library", REQUIRED_ARGUMENT, 0, OPT_LIBRARY_NAME},
    {"min_free", REQUIRED_ARGUMENT, 0, 'm'},
    {"model", REQUIRED_ARGUMENT, 0, OPT_MODEL},
    {"nodelete", NO_ARGUMENT, &nodelete_flag, 1},
    {"output_tppool", REQUIRED_ARGUMENT, 0, 'o'},
    {"otp", REQUIRED_ARGUMENT, 0, 'o'},*/
    {"help", NO_ARGUMENT,NULL, 'h' },
    {NULL, 0, NULL, 0}
  };

  
  Coptind = 1;
  Copterr = 0;
  char c;

  while ((c = Cgetopt_long(argc, argv, (char*)cmdParams, longopts, NULL)) != -1) {
    switch (c) {
    case 'h':
      help();
      exit(0);
      break;
    case 'V':
      cp.vid = Coptarg; // store it for later use in building Request
      cp.command = REPACK;
      break;
    case 'P':
      cp.pool = Coptarg; // store it for later use in building Request
      break;
    case 'R':
      cp.vid = Coptarg;
      cp.command = REMOVE_TAPE;
      break;
	 case 'S':
		cp.vid = Coptarg;
		cp.command = GET_STATUS;
		return true;
	case 's':
		cp.command = GET_STATUS_ALL;
		return true;
    default:
      break;
    }
  }
  
  return ( cp.pool != NULL || cp.vid != NULL );
}




//------------------------------------------------------------------------------
// usage
//------------------------------------------------------------------------------
void RepackClient::usage() 
{
	std::cout << "Usage: repack -V [VolumeID] -P [PoolID] | -h " << std::endl;
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
void RepackClient::help(){
	std::cout << "The RepackClient" << std::endl
	<< "This client sends a request to the repack server with tapes or one pool to be repacked. "<< std::endl 
	<< "Several tapes can be added by sperating them by ':'.It is also possible to repack a tape pool."<< std::endl 
	<< "Here, only the name of one tape pool is allowed." << std::endl;

	std::cout << "The hostname and port can be changed easily by changing them in your castor " << std::endl 
	<< "config file or by defining them in your enviroment." << std::endl << std::endl 
	<< "The enviroment variables are: "<< std::endl 
	<< "REPACK_PORT or REPACK_PORT_ALT "<< std::endl 
	<< "REPACK_HOST or REPACK_HOST_ALT "<< std::endl << std::endl 
	<< "The castor config file :" << std::endl
	<< "REPACK PORT <port number> "<< std::endl
	<< "REPACK HOST <hostname> "<< std::endl<< std::endl
	<< "If the enviroment has no vaild entries the castor configuration file" << std::endl
	<< "is read." << std::endl << std::endl;
		
        usage();
}



//------------------------------------------------------------------------------
// addTapes
//------------------------------------------------------------------------------
int RepackClient::addTapes(RepackRequest *rreq)
{
	char* vid;
  /* add the given tapes as SubRequests */
	if ( cp.vid != NULL ) {
	  for (vid = strtok (cp.vid, ":"); vid;  vid = strtok (NULL, ":")) {
	  	RepackSubRequest *sreq = new RepackSubRequest();
	  	sreq->setVid(vid);
	  	sreq->setRequestID(rreq);
	  	rreq->addSubRequest(sreq);
	  }
  }	
}



//------------------------------------------------------------------------------
// buildRequest
//------------------------------------------------------------------------------
castor::repack::RepackRequest* RepackClient::buildRequest() throw ()
{
  setRemotePort();
  setRemoteHost();
  char* vid;
  char cName[CA_MAXHOSTNAMELEN];
  struct passwd *pw = Cgetpwuid(getuid());

  RepackRequest* rreq = new RepackRequest();
  
  if ( gethostname(cName, CA_MAXHOSTNAMELEN) != 0 ){
  	std::cerr << "Cannot get hostname ! Aborting.." << std::endl;
 	delete rreq;
  	return NULL;
  }

  addTapes(rreq);
  rreq->setCommand(cp.command);
  
  /* or, we want to repack a pool */
  if ( cp.pool != NULL ) {
  	if ( !rreq->subRequest().size() )
       rreq->setPool(cp.pool);
    else
    {
    	std::cerr << "You must specify either a pool name or one or more volume ids" 
    			  << std::endl;
    	usage();
    	delete rreq;
    	return NULL;  	
     }
  }
  
  rreq->setPid(getpid());
  rreq->setUserName(pw->pw_name);
  rreq->setCreationTime(time(NULL));
  rreq->setMachine(cName);
  
  return rreq;
  
}

//------------------------------------------------------------------------------
// run
//------------------------------------------------------------------------------
void RepackClient::run(int argc, char** argv) 
{

  try {
    // parses the command line
    if (!parseInput(argc, argv)) {
      return;
    }
    // builds a request and prints it
    castor::repack::RepackRequest* req = buildRequest();
    
    if ( req == NULL )
    	return;
   // req->print();

    // creates a socket
    castor::io::ClientSocket s(m_defaultport, m_defaulthost);
    s.connect();
    // sends the request
    s.sendObject(*req);
	castor::IObject* obj = s.readObject();
	
	RepackAck* ack = dynamic_cast<castor::repack::RepackAck*>(obj);
	if ( ack != 0 ){
		handleResponse(ack);
	}
	
    // free memory
    delete req;
    delete ack;
  } catch (castor::exception::Exception ex) {
    	std::cerr << ex.getMessage().str() << std::endl;
  }
}




//------------------------------------------------------------------------------
// handleResponse
//------------------------------------------------------------------------------
void RepackClient::handleResponse(RepackAck* ack) {
	
	std::map<int,std::string> statuslist;
	statuslist[SUBREQUEST_READYFORSTAGING] = "START";
	statuslist[SUBREQUEST_STAGING] = "STAGING";
	statuslist[SUBREQUEST_MIGRATING] = "MIGRATING";
	statuslist[SUBREQUEST_READYFORCLEANUP] = "CLEANUP";
	statuslist[SUBREQUEST_DONE] = "FINISHED";
	
	if ( ack->errorCode() ){
			std::cerr << "Repackserver respond :" << std::endl
					  << ack->errorMessage() << std::endl;
			return;
		}

	if ( ack->request().size() > 0 ){
	RepackRequest* rreq = ack->request().at(0);
	std::cout << "============================================================================" << std::endl;
	std::cout << " Server response " << std::endl;
	switch ( rreq->command() ){
		case GET_STATUS_ALL : 
    case GET_STATUS : 
		case REPACK :
		{
			std::cout << "VID\tMIGRATING\tSTAGING\tTOTAL\tSTATUS\t\tCUUID" <<std::endl;
			
			std::vector<RepackSubRequest*>::iterator tape = rreq->subRequest().begin();
			while ( tape != rreq->subRequest().end() ){
				std::cout << (*tape)->vid() 
					<< "\t" << (*tape)->filesMigrating()
          << "\t\t" << (*tape)->filesStaging()
          << "\t" << (*tape)->files()
					<< "\t" << statuslist[(*tape)->status()]
          << "\t" << (*tape)->cuuid()
					<< std::endl;
				tape++;
			}
			break;
		}

	}
	}


}



//------------------------------------------------------------------------------
// setRemotePort()
//------------------------------------------------------------------------------
void RepackClient::setRemotePort()
  throw (castor::exception::Exception) {
  char* port;
  // Repack server port. Can be given through the environment
  // variable REPACK_PORT or in the castor.conf file as a
  // REPACK_PORT entry. If none is given, default is used
  if ((port = getenv (PORT_ENV)) != 0 
      || (port = getenv ((char*)PORT_ENV_ALT)) != 0 ) {
    char* dp = port;
    errno = 0;
    int iport = strtoul(port, &dp, 0);
    if (*dp != 0) {
      castor::exception::Exception e(errno);
      e.getMessage() << "Bad port value." << std::endl;
      throw e;
    }
    if (iport > 65535) {
      castor::exception::Exception e(errno);
      e.getMessage()
        << "Invalid port value : " << iport
        << ". Must be < 65535." << std::endl;
      throw e;
    }
    m_defaultport = iport;
  }
  else {
		stage_trace(3,"No repack server port in enviroment found using default(%d)! ", CSP_REPACKSERVER_PORT);
  }
  
  stage_trace(3, "Setting up Server Port - Using %d", m_defaultport);
}

//------------------------------------------------------------------------------
// setRhHost
//------------------------------------------------------------------------------
void RepackClient::setRemoteHost()
  throw (castor::exception::Exception) {
  // RH server host. Can be passed given through the
  // RH_HOST environment variable or in the castor.conf
  // file as a RH/HOST entry
  char* host;
  if ((host = getenv (HOST_ENV)) != 0
      || (host = getenv (HOST_ENV_ALT)) != 0
      || (host = getconfent((char*)CATEGORY_CONF,(char *)HOST_CONF,0)) != 0) {
    m_defaulthost = host;
  } else {
    castor::exception::Exception e(serrno);
      e.getMessage()
        << "No repack server hostname in config file or in enviroment found! " << std::endl;
      throw e;
  }
  stage_trace(3, "Looking up Server Host - Using %s", m_defaulthost.c_str());
}




      }  //end of namespace repack

}     //end of namespace castor
