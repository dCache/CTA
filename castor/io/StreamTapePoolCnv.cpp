/******************************************************************************
 *                      castor/io/StreamTapePoolCnv.cpp
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
 * @author Sebastien Ponce, sebastien.ponce@cern.ch
 *****************************************************************************/

// Include Files
#include "StreamTapePoolCnv.hpp"
#include "castor/CnvFactory.hpp"
#include "castor/IAddress.hpp"
#include "castor/IConverter.hpp"
#include "castor/IFactory.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectCatalog.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/exception/Exception.hpp"
#include "castor/exception/Internal.hpp"
#include "castor/io/StreamAddress.hpp"
#include "castor/stager/TapePool.hpp"
#include "osdep.h"
#include <string>

//------------------------------------------------------------------------------
// Instantiation of a static factory class
//------------------------------------------------------------------------------
static castor::CnvFactory<castor::io::StreamTapePoolCnv> s_factoryStreamTapePoolCnv;
const castor::IFactory<castor::IConverter>& StreamTapePoolCnvFactory = 
  s_factoryStreamTapePoolCnv;

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::io::StreamTapePoolCnv::StreamTapePoolCnv() :
  StreamBaseCnv() {}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::io::StreamTapePoolCnv::~StreamTapePoolCnv() throw() {
}

//------------------------------------------------------------------------------
// ObjType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapePoolCnv::ObjType() {
  return castor::stager::TapePool::TYPE();
}

//------------------------------------------------------------------------------
// objType
//------------------------------------------------------------------------------
const unsigned int castor::io::StreamTapePoolCnv::objType() const {
  return ObjType();
}

//------------------------------------------------------------------------------
// createRep
//------------------------------------------------------------------------------
void castor::io::StreamTapePoolCnv::createRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              castor::ObjectSet& alreadyDone,
                                              bool autocommit,
                                              bool recursive)
  throw (castor::exception::Exception) {
  castor::stager::TapePool* obj = 
    dynamic_cast<castor::stager::TapePool*>(object);
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  ad->stream() << obj->type();
  ad->stream() << obj->name();
  ad->stream() << obj->id();
  // Mark object as done
  alreadyDone.insert(obj);
}

//------------------------------------------------------------------------------
// updateRep
//------------------------------------------------------------------------------
void castor::io::StreamTapePoolCnv::updateRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              castor::ObjectSet& alreadyDone,
                                              bool autocommit,
                                              bool recursive)
  throw (castor::exception::Exception) {
  castor::exception::Internal ex;
  ex.getMessage() << "Cannot update representation in case of streaming."
                  << std::endl;
  throw ex;
}

//------------------------------------------------------------------------------
// deleteRep
//------------------------------------------------------------------------------
void castor::io::StreamTapePoolCnv::deleteRep(castor::IAddress* address,
                                              castor::IObject* object,
                                              castor::ObjectSet& alreadyDone,
                                              bool autocommit)
  throw (castor::exception::Exception) {
  castor::exception::Internal ex;
  ex.getMessage() << "Cannot delete representation in case of streaming."
                  << std::endl;
  throw ex;
}

//------------------------------------------------------------------------------
// createObj
//------------------------------------------------------------------------------
castor::IObject* castor::io::StreamTapePoolCnv::createObj(castor::IAddress* address,
                                                          castor::ObjectCatalog& newlyCreated,
                                                          bool recursive)
  throw (castor::exception::Exception) {
  StreamAddress* ad = 
    dynamic_cast<StreamAddress*>(address);
  // create the new Object
  castor::stager::TapePool* object = new castor::stager::TapePool();
  // Now retrieve and set members
  std::string name;
  ad->stream() >> name;
  object->setName(name);
  u_signed64 id;
  ad->stream() >> id;
  object->setId(id);
  newlyCreated.insert(object);
  return object;
}

//------------------------------------------------------------------------------
// updateObj
//------------------------------------------------------------------------------
void castor::io::StreamTapePoolCnv::updateObj(castor::IObject* obj,
                                              castor::ObjectCatalog& alreadyDone)
  throw (castor::exception::Exception) {
  castor::exception::Internal ex;
  ex.getMessage() << "Cannot update object in case of streaming."
                  << std::endl;
  throw ex;
}

