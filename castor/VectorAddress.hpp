/******************************************************************************
 *                      castor/VectorAddress.hpp
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
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

// Include Files
#include "castor/IAddress.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;
  class IObject;

  /**
   * class VectorAddress
   * The most basic address : only a type.
   */
  class VectorAddress : public virtual IAddress {

  public:

    /**
     * Empty Constructor
     */
    VectorAddress() throw();

    /**
     * Empty Destructor
     */
    virtual ~VectorAddress() throw();

    /**
     * Outputs this object in a human readable format
     * @param stream The stream where to print this object
     * @param indent The indentation to use
     * @param alreadyPrinted The set of objects already printed.
     * This is to avoid looping when printing circular dependencies
     */
    virtual void print(std::ostream& stream,
                       std::string indent,
                       castor::ObjectSet& alreadyPrinted) const;

    /**
     * Outputs this object in a human readable format
     */
    virtual void print() const;

    /**
     * Gets the type of this kind of objects
     */
    static int TYPE();

    /********************************************/
    /* Implementation of IObject abstract class */
    /********************************************/
    /**
     * Gets the type of the object
     */
    virtual int type() const;

    /**
     * virtual method to clone any object
     */
    virtual IObject* clone();

    /*********************************/
    /* End of IObject abstract class */
    /*********************************/
    /**
     * Get the value of m_objType
     * the object type of this address
     * @return the value of m_objType
     */
    unsigned int objType() const {
      return m_objType;
    }

    /**
     * Set the value of m_objType
     * the object type of this address
     * @param new_var the new value of m_objType
     */
    void setObjType(unsigned int new_var) {
      m_objType = new_var;
    }

    /**
     * Get the value of m_cnvSvcName
     * the name of the conversion service able to deal with this address
     * @return the value of m_cnvSvcName
     */
    std::string cnvSvcName() const {
      return m_cnvSvcName;
    }

    /**
     * Set the value of m_cnvSvcName
     * the name of the conversion service able to deal with this address
     * @param new_var the new value of m_cnvSvcName
     */
    void setCnvSvcName(std::string new_var) {
      m_cnvSvcName = new_var;
    }

    /**
     * Get the value of m_cnvSvcType
     * the type of the conversion service able to deal with this address
     * @return the value of m_cnvSvcType
     */
    unsigned int cnvSvcType() const {
      return m_cnvSvcType;
    }

    /**
     * Set the value of m_cnvSvcType
     * the type of the conversion service able to deal with this address
     * @param new_var the new value of m_cnvSvcType
     */
    void setCnvSvcType(unsigned int new_var) {
      m_cnvSvcType = new_var;
    }

    /**
     * Get the value of m_target
     * The id of the object this address points to, if any
     * @return the value of m_target
     */
    std::vector<u_signed64> target() const {
      return m_target;
    }

    /**
     * Set the value of m_target
     * The id of the object this address points to, if any
     * @param new_var the new value of m_target
     */
    void setTarget(std::vector<u_signed64> &new_var) {
      m_target = new_var;
    }

    /**
     * Get the value of m_id
     * The id of this object
     * @return the value of m_id
     */
    u_signed64 id() const {
      return m_id;
    }

    /**
     * Set the value of m_id
     * The id of this object
     * @param new_var the new value of m_id
     */
    void setId(u_signed64 new_var) {
      m_id = new_var;
    }

  private:

    /// the object type of this address
    unsigned int m_objType;

    /// the name of the conversion service able to deal with this address
    std::string m_cnvSvcName;

    /// the type of the conversion service able to deal with this address
    unsigned int m_cnvSvcType;

    /// The id of the object this address points to, if any
    std::vector<u_signed64> m_target;

    /// The id of this object
    u_signed64 m_id;

  }; /* end of class VectorAddress */

} /* end of namespace castor */

