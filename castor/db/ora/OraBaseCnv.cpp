/******************************************************************************
 *                      OraBaseCnv.cpp
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

// Include Files
#include "castor/exception/Internal.hpp"
#include "castor/db/ora/OraCnvSvc.hpp"
#include "castor/Constants.hpp"
#include "castor/IObject.hpp"
#include "OraBaseCnv.hpp"

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
castor::db::ora::OraBaseCnv::OraBaseCnv(castor::ICnvSvc* cs) :
  OraBaseObj(cs) {
  cnvSvc()->registerCnv(this);
}

// -----------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------
castor::db::ora::OraBaseCnv::~OraBaseCnv() throw() {
  cnvSvc()->unregisterCnv(this);
}

// -----------------------------------------------------------------------
// RepType
// -----------------------------------------------------------------------
const unsigned int castor::db::ora::OraBaseCnv::RepType() {
  return castor::REP_ORACLE;
}

// -----------------------------------------------------------------------
// repType
// -----------------------------------------------------------------------
inline const unsigned int castor::db::ora::OraBaseCnv::repType() const {
  return RepType();
}
