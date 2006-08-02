/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

#ifndef CASTOR_RH_REQUESTQUERYRESPONSE_H
#define CASTOR_RH_REQUESTQUERYRESPONSE_H

// Include Files and Forward declarations for the C world
#include "osdep.h"
struct C_IObject_t;
struct Crh_RequestQueryResponse_t;
struct Crh_Response_t;

//------------------------------------------------------------------------------
// This defines a C interface to the following class
// class RequestQueryResponse
// Response to the RequestQueryRequest
//------------------------------------------------------------------------------

/**
 * Empty Constructor
 */
int Crh_RequestQueryResponse_create(struct Crh_RequestQueryResponse_t** obj);

/**
 * Empty Destructor
 */
int Crh_RequestQueryResponse_delete(struct Crh_RequestQueryResponse_t* obj);

/**
 * Cast into Response
 */
struct Crh_Response_t* Crh_RequestQueryResponse_getResponse(struct Crh_RequestQueryResponse_t* obj);

/**
 * Dynamic cast from Response
 */
struct Crh_RequestQueryResponse_t* Crh_RequestQueryResponse_fromResponse(struct Crh_Response_t* obj);

/**
 * Cast into IObject
 */
struct C_IObject_t* Crh_RequestQueryResponse_getIObject(struct Crh_RequestQueryResponse_t* obj);

/**
 * Dynamic cast from IObject
 */
struct Crh_RequestQueryResponse_t* Crh_RequestQueryResponse_fromIObject(struct C_IObject_t* obj);

/**
 * Outputs this object in a human readable format
 */
int Crh_RequestQueryResponse_print(struct Crh_RequestQueryResponse_t* instance);

/**
 * Gets the type of this kind of objects
 */
int Crh_RequestQueryResponse_TYPE(int* ret);

/*********************************************/
/* Implementation of Response abstract class */
/*********************************************/

/**
 * Get the value of errorCode
 * The error code in case of error
 */
int Crh_RequestQueryResponse_errorCode(struct Crh_RequestQueryResponse_t* instance, unsigned int* var);

/**
 * Set the value of errorCode
 * The error code in case of error
 */
int Crh_RequestQueryResponse_setErrorCode(struct Crh_RequestQueryResponse_t* instance, unsigned int new_var);

/**
 * Get the value of errorMessage
 * The error message in case of error
 */
int Crh_RequestQueryResponse_errorMessage(struct Crh_RequestQueryResponse_t* instance, const char** var);

/**
 * Set the value of errorMessage
 * The error message in case of error
 */
int Crh_RequestQueryResponse_setErrorMessage(struct Crh_RequestQueryResponse_t* instance, const char* new_var);

/***************************************/
/* Implementation of IObject interface */
/***************************************/

/**
 * Gets the type of the object
 */
int Crh_RequestQueryResponse_type(struct Crh_RequestQueryResponse_t* instance,
                                  int* ret);

/**
 * virtual method to clone any object
 */
int Crh_RequestQueryResponse_clone(struct Crh_RequestQueryResponse_t* instance,
                                   struct C_IObject_t* ret);

/**
 * Get the value of reqId
 * The Cuuid identifying the request given as a human readable string
 */
int Crh_RequestQueryResponse_reqId(struct Crh_RequestQueryResponse_t* instance, const char** var);

/**
 * Set the value of reqId
 * The Cuuid identifying the request given as a human readable string
 */
int Crh_RequestQueryResponse_setReqId(struct Crh_RequestQueryResponse_t* instance, const char* new_var);

/**
 * Get the value of status
 * The status of the request
 */
int Crh_RequestQueryResponse_status(struct Crh_RequestQueryResponse_t* instance, unsigned int* var);

/**
 * Set the value of status
 * The status of the request
 */
int Crh_RequestQueryResponse_setStatus(struct Crh_RequestQueryResponse_t* instance, unsigned int new_var);

/**
 * Get the value of creationTime
 * Time of the request creation
 */
int Crh_RequestQueryResponse_creationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of creationTime
 * Time of the request creation
 */
int Crh_RequestQueryResponse_setCreationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64 new_var);

/**
 * Get the value of modificationTime
 * Time of the last modification of the request
 */
int Crh_RequestQueryResponse_modificationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of modificationTime
 * Time of the last modification of the request
 */
int Crh_RequestQueryResponse_setModificationTime(struct Crh_RequestQueryResponse_t* instance, u_signed64 new_var);

/**
 * Get the value of id
 * The id of this object
 */
int Crh_RequestQueryResponse_id(struct Crh_RequestQueryResponse_t* instance, u_signed64* var);

/**
 * Set the value of id
 * The id of this object
 */
int Crh_RequestQueryResponse_setId(struct Crh_RequestQueryResponse_t* instance, u_signed64 new_var);

#endif // CASTOR_RH_REQUESTQUERYRESPONSE_H
