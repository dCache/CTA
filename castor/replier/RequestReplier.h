/******************************************************************************
 *                      RequestReplier.h
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef REPLIER_REQUESTREPLIER_H 
#define REPLIER_REQUESTREPLIER_H 1

/// Forward declarations for the C world
struct Creplier_RequestReplier_t;
struct C_IClient_t;
struct C_IObject_t;

/**
 * Returns the instance of the RequestReplier singleton.
 * Note that the instance should be released by the caller.
 * @return A pointer to the RequestReplier instance
 */
struct Creplier_RequestReplier_t *Creplier_RequestReplier_getInstance();

/**
 * Adds a client to the queue of clients waiting for a response.
 * @param rr the request replier used
 * @param client the client object indicating the client address
 * @param response the response to send
 * @param isLastResponse whether this will be the last response
 * to this client.
 * @return 0 : call was successful,
 * -1 : an error occurred and serrno is set to the corresponding error code
 * A detailed error message can be retrieved by calling
 * Creplier_RequestReplier_errorMsg
 */
int Creplier_RequestReplier_replyToClient(struct Creplier_RequestReplier_t *rr,
                                          struct C_IClient_t *client,
                                          struct C_IObject_t *response,
                                          int isLastResponse);


/**
 * Adds a client to the queue of clients waiting for a response.
 * @param rr the request replier used
 * @param client the client object indicating the client address
 * @param response the response to send
 * @param isLastResponse whether this will be the last response
 * to this client.
 * @return 0 : call was successful,
 * -1 : an error occurred and serrno is set to the corresponding error code
 * A detailed error message can be retrieved by calling
 * Creplier_RequestReplier_errorMsg
 */
int Creplier_RequestReplier_sendResponse(struct Creplier_RequestReplier_t *rr,
					 struct C_IClient_t *client,
					 struct C_IObject_t *response,
					 int isLastResponse);


/**
 * Adds a client to the queue of clients waiting for a response.
 * @param rr the request replier used
 * @param client the client object indicating the client address
 * to this client.
 * @return 0 : call was successful,
 * -1 : an error occurred and serrno is set to the corresponding error code
 * A detailed error message can be retrieved by calling
 * Creplier_RequestReplier_errorMsg
 */
int Creplier_RequestReplier_sendEndResponse(struct Creplier_RequestReplier_t *rr,
					    struct C_IClient_t *client);


/**
 * Releases an instance of the RequestReplier singleton
 * @param rr the request replier instance to release
 */
void Creplier_RequestReplier_release(struct Creplier_RequestReplier_t *rr);

/**
 * Returns the error message associated to the last error.
 * Note that the error message string should be deallocated
 * by the caller.
 * @param rr the request replier used
 * @return the error message
 */
const char* Creplier_RequestReplier_errorMsg
(struct Creplier_RequestReplier_t* rr);

#endif // REPLIER_REQUESTREPLIER_H
