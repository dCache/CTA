/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_STAGER_PUTDONESTART_H
#define CASTOR_STAGER_PUTDONESTART_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_PutDoneStart_t;
struct Cstager_Request_t;
struct Cstager_StartRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class PutDoneStart
// Internal request used when a putDone job has just started.
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_PutDoneStart_create(struct Cstager_PutDoneStart_t** obj);

/**
 * Empty Destructor
 */
int Cstager_PutDoneStart_delete(struct Cstager_PutDoneStart_t* obj);

/**
 * Cast into StartRequest
 */
struct Cstager_StartRequest_t* Cstager_PutDoneStart_getStartRequest(struct Cstager_PutDoneStart_t* obj);

/**
 * Dynamic cast from StartRequest
 */
struct Cstager_PutDoneStart_t* Cstager_PutDoneStart_fromStartRequest(struct Cstager_StartRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_PutDoneStart_getRequest(struct Cstager_PutDoneStart_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_PutDoneStart_t* Cstager_PutDoneStart_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_PutDoneStart_getIObject(struct Cstager_PutDoneStart_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_PutDoneStart_t* Cstager_PutDoneStart_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_PutDoneStart_print(struct Cstager_PutDoneStart_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_PutDoneStart_TYPE(int* ret);

/*************************************************/
/* Implementation of StartRequest abstract class */
/*************************************************/

/**
 * Get the value of subreqId
 * The id of the subRequest that should be scheduled
 */
int Cstager_PutDoneStart_subreqId(struct Cstager_PutDoneStart_t* instance, u_signed64* var);

/**
 * Set the value of subreqId
 * The id of the subRequest that should be scheduled
 */
int Cstager_PutDoneStart_setSubreqId(struct Cstager_PutDoneStart_t* instance, u_signed64 new_var);

/**
 * Get the value of diskServer
 * The name of the diskserver on which the selected filesystem for the given
 */
int Cstager_PutDoneStart_diskServer(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of diskServer
 * The name of the diskserver on which the selected filesystem for the given
 */
int Cstager_PutDoneStart_setDiskServer(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/**
 * Get the value of fileSystem
 * The mount point of the selected filesystem for the given SubRequest
 */
int Cstager_PutDoneStart_fileSystem(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of fileSystem
 * The mount point of the selected filesystem for the given SubRequest
 */
int Cstager_PutDoneStart_setFileSystem(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_PutDoneStart_flags(struct Cstager_PutDoneStart_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_PutDoneStart_setFlags(struct Cstager_PutDoneStart_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_PutDoneStart_userName(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_PutDoneStart_setUserName(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_PutDoneStart_euid(struct Cstager_PutDoneStart_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_PutDoneStart_setEuid(struct Cstager_PutDoneStart_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_PutDoneStart_egid(struct Cstager_PutDoneStart_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_PutDoneStart_setEgid(struct Cstager_PutDoneStart_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_PutDoneStart_mask(struct Cstager_PutDoneStart_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_PutDoneStart_setMask(struct Cstager_PutDoneStart_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_PutDoneStart_pid(struct Cstager_PutDoneStart_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_PutDoneStart_setPid(struct Cstager_PutDoneStart_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_PutDoneStart_machine(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_PutDoneStart_setMachine(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_PutDoneStart_svcClassName(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_PutDoneStart_setSvcClassName(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cstager_PutDoneStart_userTag(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cstager_PutDoneStart_setUserTag(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_PutDoneStart_reqId(struct Cstager_PutDoneStart_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_PutDoneStart_setReqId(struct Cstager_PutDoneStart_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_PutDoneStart_creationTime(struct Cstager_PutDoneStart_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_PutDoneStart_setCreationTime(struct Cstager_PutDoneStart_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_PutDoneStart_lastModificationTime(struct Cstager_PutDoneStart_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_PutDoneStart_setLastModificationTime(struct Cstager_PutDoneStart_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_PutDoneStart_svcClass(struct Cstager_PutDoneStart_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_PutDoneStart_setSvcClass(struct Cstager_PutDoneStart_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_PutDoneStart_client(struct Cstager_PutDoneStart_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_PutDoneStart_setClient(struct Cstager_PutDoneStart_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_PutDoneStart_type(struct Cstager_PutDoneStart_t* instance,
                              int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_PutDoneStart_clone(struct Cstager_PutDoneStart_t* instance,
                               struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_PutDoneStart_id(struct Cstager_PutDoneStart_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_PutDoneStart_setId(struct Cstager_PutDoneStart_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_PUTDONESTART_H
