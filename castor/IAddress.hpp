/******************************************************************************
 *                      castor/IAddress.hpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_IADDRESS_HPP
#define CASTOR_IADDRESS_HPP

// Include Files
#include "castor/IObject.hpp"
#include <iostream>
#include <string>

namespace castor {

  // Forward declarations
  class ObjectSet;

  /**
   * class IAddress
   *  Base class for all addresses. An address allows to find a foreign representation
   * of an object. Fully empty except for an id telling the type of the address and
   * the infrastructure for printing any address
   */
  class IAddress : public virtual IObject {

  public:

    /**
     * Empty Destructor
     */
    virtual ~IAddress() throw() {};

    /**
     * gets the object type, that is the type of object whose representation is pointed
     * to by this address
     */
    virtual unsigned int objType() const = 0;

    /**
     * sets the object type, that is the type of object whose representation is pointed
     * to by this address.
     * @param type The new type of this address
     */
    virtual void setObjType(unsigned int type) = 0;

    /**
     * gets the name of the conversion service able to deal with this address
     */
    virtual std::string cnvSvcName() const = 0;

    /**
     * gets the type of the conversion service able to deal with this address
     */
    virtual unsigned int cnvSvcType() const = 0;

    /**
     * prints the address into an output stream
     * @param s The stream where to print
     */
    virtual void print(std::ostream& s) const = 0;

    /**
     * Outputs this object in a human readable format
     * @param stream The stream where to print this object
     * @param indent The indentation to use
     * @param alreadyPrinted The set of objects already printed.
     * This is to avoid looping when printing circular dependencies
     */
    virtual void print(std::ostream& stream,
                       std::string indent,
                       castor::ObjectSet& alreadyPrinted) const = 0;

    /**
     * Outputs this object in a human readable format
     */
    virtual void print() const = 0;

  private:

  }; // end of class IAddress

}; // end of namespace castor

#endif // CASTOR_IADDRESS_HPP
