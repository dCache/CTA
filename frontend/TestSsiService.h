#ifndef __TEST_SSI_SERVICE_H
#define __TEST_SSI_SERVICE_H

#include <iostream>
#include <stdexcept>

#include <XrdSsi/XrdSsiProvider.hh>
#include <XrdSsi/XrdSsiService.hh>
#include "TestSsiRequest.h"

// Probably we want to allow multiple resources, e.g. streaming and non-streaming versions of the service
// Can this be defined in the protobuf definition?

const std::string TestSsiResource("/test");



// Class to convert a XRootD error into a std::exception

class XrdSsiException : public std::exception
{
public:
   XrdSsiException(const std::string &err_msg) : error_msg(err_msg)     {}
   XrdSsiException(const XrdSsiErrInfo &eInfo) : error_msg(eInfo.Get()) {}

   const char* what() const noexcept { return error_msg.c_str(); }

private:
   std::string error_msg;
};



// XrdSsiProviderClient is instantiated and managed by the SSI library

extern XrdSsiProvider *XrdSsiProviderClient;



template <typename RequestType, typename ResponseType>
class TestSsiService
{
public:
   TestSsiService() = delete;

   TestSsiService(std::string hostname, int port, int to = 15) : resource(TestSsiResource), timeout(to)
   {
      XrdSsiErrInfo eInfo;

      if(!(serverP = XrdSsiProviderClient->GetService(eInfo, hostname + ":" + std::to_string(port))))
      {
         throw XrdSsiException(eInfo);
      }
   }

   ~TestSsiService()
   {
      // The XrdSsiService object cannot be explicitly deleted. The Stop() method deletes the object if
      // it is safe to do so. A service object can only be deleted after all requests handed to the object
      // have completed, i.e. Finished() has been called on each request.

      if(serverP->Stop())
      {
         std::cerr << "Stopped SSI service" << std::endl;
      }
      else
      {
         std::cerr << "Failed to stop SSI service" << std::endl;

         // Unpalatable choice here between passing back control to the calling scope with a possible memory leak,
         // or trying to force all requests to Finish with possible blocking behaviour...
      }
   }

   void send(RequestType request_msg)
   {
      // Requests are always executed in the context of a service. They need to correspond to what the service allows.

      XrdSsiRequest *requestP;

      // Serialize the request object

      std::string data;

      if(!request_msg.SerializeToString(&data))
      {
         throw XrdSsiException("SerializeToString() failed");
      }

      requestP = new TestSsiRequest(data, timeout);

      // Transfer ownership of the request to the service object
      // TestSsiRequest handles deletion of the data buffer, so we can allow the pointer to go out-of-scope

      serverP->ProcessRequest(*requestP, resource);

      // Note: it is safe to delete the XrdSsiResource object after ProcessRequest() returns. I don't delete it because
      // I am assuming I can reuse it, but I need to check if that is a safe assumption. Perhaps I need to create a new
      // resource object for each request?
   }

private:
   XrdSsiResource resource;          // Requests are bound to this resource

   XrdSsiService *serverP;           // Pointer to XRootD Server object

   int timeout;                      // Server timeout
};

#endif
