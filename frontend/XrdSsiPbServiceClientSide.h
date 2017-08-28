#ifndef __XRD_SSI_PB_SERVICE_CLIENT_SIDE_H
#define __XRD_SSI_PB_SERVICE_CLIENT_SIDE_H

#include <XrdSsi/XrdSsiProvider.hh>
#include <XrdSsi/XrdSsiService.hh>
#include "XrdSsiException.h"
#include "XrdSsiPbRequest.h"



// Constants

const unsigned int DefaultResponseBufferSize = 2097152;    // Default size for the response buffer in bytes = 2 Mb
const unsigned int DefaultServerTimeout      = 15;         // Maximum XRootD reply timeout in secs
const unsigned int DefaultShutdownTimeout    = 30;         // Maximum time to wait for the Service to shut down in secs



// XrdSsiProviderClient is instantiated and managed by the SSI library

extern XrdSsiProvider *XrdSsiProviderClient;



// Convenience object to manage the XRootD SSI service on the client side

template <typename RequestType, typename ResponseType, typename MetadataType, typename AlertType>
class XrdSsiPbServiceClientSide
{
public:
   // Service object constructor for the client side

   XrdSsiPbServiceClientSide() = delete;

   XrdSsiPbServiceClientSide(const std::string &hostname, unsigned int port, const std::string &resource,
                           unsigned int response_bufsize = DefaultResponseBufferSize,
                           unsigned int server_tmo       = DefaultServerTimeout,
                           unsigned int shutdown_tmo     = DefaultShutdownTimeout) :
      m_resource(resource),
      m_response_bufsize(response_bufsize),
      m_server_tmo(server_tmo),
      m_shutdown_tmo(shutdown_tmo)
   {
      XrdSsiErrInfo eInfo;

      if(!(m_server_ptr = XrdSsiProviderClient->GetService(eInfo, hostname + ":" + std::to_string(port))))
      {
         throw XrdSsiException(eInfo);
      }
   }

   // Service object destructor for the client side

   virtual ~XrdSsiPbServiceClientSide();

   // Send a Request to the Service

   void send(const RequestType &request);

private:
   XrdSsiResource m_resource;            // Requests are bound to this resource

   XrdSsiService *m_server_ptr;          // Pointer to XRootD Server object

   unsigned int   m_response_bufsize;    // Buffer size for responses from the XRootD SSI server
   unsigned int   m_server_tmo;          // Timeout for a response from the server
   unsigned int   m_shutdown_tmo;        // Timeout to shut down the server in the destructor
};



// Destructor

template <typename RequestType, typename ResponseType, typename MetadataType, typename AlertType>
XrdSsiPbServiceClientSide<RequestType, ResponseType, MetadataType, AlertType>::~XrdSsiPbServiceClientSide()
{
   std::cerr << "Stopping XRootD SSI service...";

   // The XrdSsiService object cannot be explicitly deleted. The Stop() method deletes the object if
   // it is safe to do so.

   while(!m_server_ptr->Stop() && m_shutdown_tmo--)
   {
      sleep(1);
      std::cerr << ".";
   }

   if(m_shutdown_tmo > 0)
   {
      std::cerr << "done." << std::endl;
   }
   else
   {
      // Timeout reached and there are still outstanding requests
      //
      // A service object can only be deleted after all requests handed to the object have completed.
      // It is possible to take back control of Requests by calling each Request's Finished() method:
      // this cancels the Request and the object can then be deleted.
      //
      // However, the current interface does not provide a way to recover a list of outstanding
      // Requests. I have raised this with Andy.
      //
      // Until this is solved, deleting the Service may leak memory, so don't delete Service objects
      // unless the client is shutting down!

      std::cerr << "failed." << std::endl;
   }
}



// Send a Request to the Service

template <typename RequestType, typename ResponseType, typename MetadataType, typename AlertType>
void XrdSsiPbServiceClientSide<RequestType, ResponseType, MetadataType, AlertType>::send(const RequestType &request)
{
   // Serialize the request object

   std::string request_str;

   if(!request.SerializeToString(&request_str))
   {
      throw XrdSsiException("request.SerializeToString() failed");
   }

   // Requests are always executed in the context of a service. They need to correspond to what the service allows.

   XrdSsiRequest *request_ptr =
      new XrdSsiPbRequest<RequestType, ResponseType, MetadataType, AlertType>(request_str, m_response_bufsize, m_server_tmo);

   // Transfer ownership of the request to the service object
   // TestSsiRequest handles deletion of the request buffer, so we can allow the pointer to go out-of-scope

   m_server_ptr->ProcessRequest(*request_ptr, m_resource);

   // Note: it is safe to delete the XrdSsiResource object after ProcessRequest() returns. I don't delete it because
   // I am assuming I can reuse it, but I need to check if that is a safe assumption. Perhaps I need to create a new
   // resource object for each request?
   //
   // See SSI ref p.10 on configuring a resource to be reusable
   // Do I need more than one resource? I could have a single resource called "/default" with some default options and
   // if necessary I can add other resources with other options later.
   //
   // Possible useful options:
   //
   // For specifying the tapeserver callback:
   // XrdSsiResource::rInfo
   // This option allows you to send additional out-of-band information to the
   // server that will be executing the request. The information should be specified
   // in CGI format (i.e. key=value[&key=value[...]]). This information is supplied
   // to the server-side service in its corresponding request resource object. Note
   // that restrictions apply for reusable resources.
   //
   // XrdSsiResource::rUser
   // This is an arbitrary string that is meant to further identify the request. The
   // SSI framework normally uses this information to tag log messages. It is also
   // supplied to the server-side service in its corresponding request resource
   // object.
}

#endif
