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
 * A parameter for the CASTOR log system
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include <sstream>
#include <string.h>
#include <cstdio>

namespace cta {
namespace log {

/**
 * A name/value parameter for the CASTOR logging system.
 */
class Param {

public:

  /**
   * Constructor.
   *
   * @param name The name of the parameter.
   * @param value The value of the parameter that will be converted to a string
   * using std::ostringstream.
   */
  template <typename T> Param(const std::string &name, const T &value) throw():
    m_name(name) {
    std::ostringstream oss;
    oss << value;
    m_value = oss.str();
  }
  
  /**
   * Constructor.
   *
   * @param name The name of the parameter.
   * @param value The value of the parameter that will be converted to a string
   * using snprintf for doubles
   */
  Param (const std::string &name, const double value) throw():
  m_name(name) {
    char buf[100];
    std::snprintf(buf, sizeof(buf), "%f", value);
    // Just in case we overflow
    buf[sizeof(buf)-1]='\0';
    m_value = buf;
  }
    
  /**
   * Value changer. Useful for log contexts.
   * @param value
   */
  template <typename T>
  void setValue (const T &value) throw() {
    std::stringstream oss;
    oss << value;
    m_value = oss.str();
  }

  /**
   * Returns a const reference to the name of the parameter.
   */
  const std::string &getName() const throw();

  /**
   * Returns a const reference to the value of the parameter.
   */
  const std::string &getValue() const throw();

protected:

  /**
   * Name of the parameter
   */
  std::string m_name;

  /**
   * The value of the parameter.
   */
  std::string m_value;

}; // class Param

/**
 * An helper class allowing the construction of a Param class with sprintf
 * formatting for a double.
 */
class ParamDoubleSnprintf: public Param {
public:
  ParamDoubleSnprintf(const std::string &name, const double value);
}; // class ParamDoubleSnprintf

} // namespace log
} // namespace cta

