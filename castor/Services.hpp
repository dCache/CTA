/******************************************************************************
 *                      Services.hpp
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
 * @(#)$RCSfile: Services.hpp,v $ $Revision: 1.1.1.1 $ $Release$ $Date: 2004/05/12 12:13:34 $ $Author: sponcec3 $
 *
 *
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef CASTOR_SERVICES_H
#define CASTOR_SERVICES_H 1

//Include Files
#include <map>
#include "Exception.hpp"

namespace castor {

  // Forward declarations
  class IService;
  class IAddress;
  class ICnvSvc;
  class IObject;

  /**
   * A class holding the list of available services.
   */
  class Services {

  public:

    /** Default constructor */
    Services();

    /** Default destructor */
    virtual ~Services();

    /**
     * gets a service by name.
     * If id not 0 and the service does not exist, it
     * is created.
     */
    IService* service (const std::string name,
                       const unsigned int id = 0);

    /**
     * gets a conversion service by name.
     * If id not 0 and the service does not exist, it
     * is created.
     */
    ICnvSvc* cnvService (const std::string name,
                         const unsigned int id = 0);

    /**
     * removes reference to a service. This means that it will be
     * recreated using the factory if ever needed
     */
    void removeService(const std::string name);

    /**
     * create foreign representation from a C++ Object
     * @param address where to store the representation of
     * the object
     * @param object the object to deal with
     * @param autocommit whether the changes to the database
     * should be commited or not. Default is yes.
     * @exception Exception throws an Exception in cas of error
     */
    void createRep(IAddress* address,
                   IObject* object,
                   bool autocommit = true) throw (Exception);

    /**
     * Updates foreign representation from a C++ Object.
     * @param address where the representation of
     * the object is stored
     * @param object the object to deal with
     * @param alreadyDone the set of objects which representation
     * was already updated. This is needed to avoid looping in case
     * of circular dependencies
     * @param autocommit whether the changes to the database
     * should be commited or not
     * @exception Exception throws an Exception in cas of error
     */
    void updateRep(IAddress* address,
                   IObject* object,
                   bool autocommit = true) throw (Exception);

    /**
     * deletes foreign representation of a C++ Object
     * @param address where the representation of
     * the object is stored
     * @param object the object to deal with
     * @param autocommit whether the changes to the database
     * should be commited or not. Default is not.
     * @exception Exception throws an Exception in case of error
     */
    void deleteRep(IAddress* address,
                   IObject* object,
                   bool autocommit = false) throw (Exception);

    /**
     * create C++ object from foreign representation
     * @param address the place where to find the foreign
     * representation
     * @return the C++ object created from its reprensentation
     * or 0 if unsuccessful. Note that the caller is responsible
     * for the deallocation of the newly created object
     * @exception Exception throws an Exception in cas of error
     */
    IObject* createObj(IAddress* address) throw (Exception);

  private:
    /**
     * gets a conversion service able to deal with a given
     * address
     */
    castor::ICnvSvc* cnvSvcFromAddress(castor::IAddress* address);

  private:
    /** the list of services, by name */
    std::map<const std::string, IService*> m_services;

  };

} // end of namespace castor

#endif // CASTOR_SERVICES_H
