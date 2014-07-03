/******************************************************************************
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
 * @author Giuseppe Lo Presti
 *****************************************************************************/

#pragma once

#include "castor/server/metrics/Counter.hpp"

#include <string>

namespace castor {
namespace rh {

class SvcClassCounter : public castor::server::metrics::Counter {

public:    
    
  /// Factory method
  static castor::server::metrics::Counter* instantiate(castor::IObject* obj);
      
  /// Default constructor
  SvcClassCounter(castor::IObject* obj);
      
  /// Default destructor
  ~SvcClassCounter() {};
    
  /// match
  virtual int match(castor::IObject* obj);
      
}; // class SvcClassCounter

} // namespace rh
} // namespace castor

