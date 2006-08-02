/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_STAGER_STAGEPREPARETOUPDATEREQUEST_H
#define CASTOR_STAGER_STAGEPREPARETOUPDATEREQUEST_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IClient_t;
struct C_IObject_t;
struct Cstager_FileRequest_t;
struct Cstager_Request_t;
struct Cstager_StagePrepareToUpdateRequest_t;
struct Cstager_SubRequest_t;
struct Cstager_SvcClass_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class StagePrepareToUpdateRequest
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Cstager_StagePrepareToUpdateRequest_create(struct Cstager_StagePrepareToUpdateRequest_t** obj);

/**
 * Empty Destructor
 */
int Cstager_StagePrepareToUpdateRequest_delete(struct Cstager_StagePrepareToUpdateRequest_t* obj);

/**
 * Cast into FileRequest
 */
struct Cstager_FileRequest_t* Cstager_StagePrepareToUpdateRequest_getFileRequest(struct Cstager_StagePrepareToUpdateRequest_t* obj);

/**
 * Dynamic cast from FileRequest
 */
struct Cstager_StagePrepareToUpdateRequest_t* Cstager_StagePrepareToUpdateRequest_fromFileRequest(struct Cstager_FileRequest_t* obj);

/**
 * Cast into Request
 */
struct Cstager_Request_t* Cstager_StagePrepareToUpdateRequest_getRequest(struct Cstager_StagePrepareToUpdateRequest_t* obj);

/**
 * Dynamic cast from Request
 */
struct Cstager_StagePrepareToUpdateRequest_t* Cstager_StagePrepareToUpdateRequest_fromRequest(struct Cstager_Request_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Cstager_StagePrepareToUpdateRequest_getIObject(struct Cstager_StagePrepareToUpdateRequest_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Cstager_StagePrepareToUpdateRequest_t* Cstager_StagePrepareToUpdateRequest_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Cstager_StagePrepareToUpdateRequest_print(struct Cstager_StagePrepareToUpdateRequest_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Cstager_StagePrepareToUpdateRequest_TYPE(int* ret);

/************************************************/
/* Implementation of FileRequest abstract class */
/************************************************/

/**
 * Add a struct Cstager_SubRequest_t* object to the subRequests list
 */
int Cstager_StagePrepareToUpdateRequest_addSubRequests(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Remove a struct Cstager_SubRequest_t* object from subRequests
 */
int Cstager_StagePrepareToUpdateRequest_removeSubRequests(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct Cstager_SubRequest_t* obj);

/**
 * Get the list of struct Cstager_SubRequest_t* objects held by subRequests. Note
 */
int Cstager_StagePrepareToUpdateRequest_subRequests(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct Cstager_SubRequest_t*** var, int* len);

/********************************************/
/* Implementation of Request abstract class */
/********************************************/

/**
 * Get the value of flags
 */
int Cstager_StagePrepareToUpdateRequest_flags(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64* var);

/**
 * Set the value of flags
 */
int Cstager_StagePrepareToUpdateRequest_setFlags(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_userName(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char** var);

/**
 * Set the value of userName
 * Name of the user that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_setUserName(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char* new_var);

/**
 * Get the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_euid(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long* var);

/**
 * Set the value of euid
 * Id of the user that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_setEuid(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long new_var);

/**
 * Get the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_egid(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long* var);

/**
 * Set the value of egid
 * Id of the group of the user that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_setEgid(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long new_var);

/**
 * Get the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StagePrepareToUpdateRequest_mask(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long* var);

/**
 * Set the value of mask
 * Mask for accessing files in the user space
 */
int Cstager_StagePrepareToUpdateRequest_setMask(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long new_var);

/**
 * Get the value of pid
 * Process id of the user process
 */
int Cstager_StagePrepareToUpdateRequest_pid(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long* var);

/**
 * Set the value of pid
 * Process id of the user process
 */
int Cstager_StagePrepareToUpdateRequest_setPid(struct Cstager_StagePrepareToUpdateRequest_t* instance, unsigned long new_var);

/**
 * Get the value of machine
 * The machine that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_machine(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char** var);

/**
 * Set the value of machine
 * The machine that submitted the request
 */
int Cstager_StagePrepareToUpdateRequest_setMachine(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char* new_var);

/**
 * Get the value of svcClassName
 */
int Cstager_StagePrepareToUpdateRequest_svcClassName(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char** var);

/**
 * Set the value of svcClassName
 */
int Cstager_StagePrepareToUpdateRequest_setSvcClassName(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char* new_var);

/**
 * Get the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cstager_StagePrepareToUpdateRequest_userTag(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char** var);

/**
 * Set the value of userTag
 * This is a string that the user is free to use. It can be useful to classify and
 */
int Cstager_StagePrepareToUpdateRequest_setUserTag(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char* new_var);

/**
 * Get the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StagePrepareToUpdateRequest_reqId(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the Request, stored as a human readable string
 */
int Cstager_StagePrepareToUpdateRequest_setReqId(struct Cstager_StagePrepareToUpdateRequest_t* instance, const char* new_var);

/**
 * Get the value of creationTime
 * Time when the Request was created
 */
int Cstager_StagePrepareToUpdateRequest_creationTime(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time when the Request was created
 */
int Cstager_StagePrepareToUpdateRequest_setCreationTime(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StagePrepareToUpdateRequest_lastModificationTime(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64* var);

/**
 * Set the value of lastModificationTime
 * Time when the request was last modified
 */
int Cstager_StagePrepareToUpdateRequest_setLastModificationTime(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64 new_var);

/**
 * Get the value of svcClass
 */
int Cstager_StagePrepareToUpdateRequest_svcClass(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct Cstager_SvcClass_t** var);

/**
 * Set the value of svcClass
 */
int Cstager_StagePrepareToUpdateRequest_setSvcClass(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct Cstager_SvcClass_t* new_var);

/**
 * Get the value of client
 */
int Cstager_StagePrepareToUpdateRequest_client(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct C_IClient_t** var);

/**
 * Set the value of client
 */
int Cstager_StagePrepareToUpdateRequest_setClient(struct Cstager_StagePrepareToUpdateRequest_t* instance, struct C_IClient_t* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Cstager_StagePrepareToUpdateRequest_type(struct Cstager_StagePrepareToUpdateRequest_t* instance,
                                             int* ret);

/**
 * virtual method to clone any object
 */
int Cstager_StagePrepareToUpdateRequest_clone(struct Cstager_StagePrepareToUpdateRequest_t* instance,
                                              struct C_IObject_t* ret);

/**
 * Get the value of id
 * The id of this object
 */
int Cstager_StagePrepareToUpdateRequest_id(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Cstager_StagePrepareToUpdateRequest_setId(struct Cstager_StagePrepareToUpdateRequest_t* instance, u_signed64 new_var);

#endif // CASTOR_STAGER_STAGEPREPARETOUPDATEREQUEST_H
