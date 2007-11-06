/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/infoPolicy/PolicyObj.cpp
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

// Include Files
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "castor/ObjectSet.hpp"
#include "castor/infoPolicy/CnsInfoPolicy.hpp"
#include "castor/infoPolicy/DbInfoPolicy.hpp"
#include "castor/infoPolicy/PolicyObj.hpp"
#include "osdep.h"
#include <iostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
castor::infoPolicy::PolicyObj::PolicyObj() throw() :
  m_policyName(""),
  m_svcClassName(""),
  m_id(0) {
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
castor::infoPolicy::PolicyObj::~PolicyObj() throw() {
  for (unsigned int i = 0; i < m_dbInfoPolicyVector.size(); i++) {
    m_dbInfoPolicyVector[i]->setPolicyObject(0);
  }
  m_dbInfoPolicyVector.clear();
  for (unsigned int i = 0; i < m_cnsInfoPolicyVector.size(); i++) {
    m_cnsInfoPolicyVector[i]->setPolicyObject(0);
  }
  m_cnsInfoPolicyVector.clear();
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::infoPolicy::PolicyObj::print(std::ostream& stream,
                                          std::string indent,
                                          castor::ObjectSet& alreadyPrinted) const {
  stream << indent << "[# PolicyObj #]" << std::endl;
  if (alreadyPrinted.find(this) != alreadyPrinted.end()) {
    // Circular dependency, this object was already printed
    stream << indent << "Back pointer, see above" << std::endl;
    return;
  }
  // Output of all members
  stream << indent << "policyName : " << m_policyName << std::endl;
  stream << indent << "svcClassName : " << m_svcClassName << std::endl;
  stream << indent << "id : " << m_id << std::endl;
  alreadyPrinted.insert(this);
  {
    stream << indent << "DbInfoPolicy : " << std::endl;
    int i;
    std::vector<DbInfoPolicy*>::const_iterator it;
    for (it = m_dbInfoPolicyVector.begin(), i = 0;
         it != m_dbInfoPolicyVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
  {
    stream << indent << "CnsInfoPolicy : " << std::endl;
    int i;
    std::vector<CnsInfoPolicy*>::const_iterator it;
    for (it = m_cnsInfoPolicyVector.begin(), i = 0;
         it != m_cnsInfoPolicyVector.end();
         it++, i++) {
      stream << indent << "  " << i << " :" << std::endl;
      (*it)->print(stream, indent + "    ", alreadyPrinted);
    }
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
void castor::infoPolicy::PolicyObj::print() const {
  castor::ObjectSet alreadyPrinted;
  print(std::cout, "", alreadyPrinted);
}

//------------------------------------------------------------------------------
// TYPE
//------------------------------------------------------------------------------
int castor::infoPolicy::PolicyObj::TYPE() {
  return OBJ_PolicyObj;
}

//------------------------------------------------------------------------------
// type
//------------------------------------------------------------------------------
int castor::infoPolicy::PolicyObj::type() const {
  return TYPE();
}

//------------------------------------------------------------------------------
// clone
//------------------------------------------------------------------------------
castor::IObject* castor::infoPolicy::PolicyObj::clone() {
  return new PolicyObj(*this);
}

