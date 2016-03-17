/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2015  CERN
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "common/exception/Exception.hpp"
#include "tapeserver/daemon/ConfigurationFile.hpp"
#include "common/utils/utils.hpp"
#include "common/log/LogContext.hpp"
#include "common/log/Logger.hpp"
#include <limits>


namespace cta {
namespace tape {
namespace daemon {
/**
 * A templated class allowing the tracking of parameter with their source.
 * If the parameter is not set (implicitly defined as the source being
 * an empty string), access to the value will be denied (exception)
 */
template<class C>
class SourcedParameter {
public:
  CTA_GENERATE_EXCEPTION_CLASS(MandatoryParameterNotDefined);
  CTA_GENERATE_EXCEPTION_CLASS(UnsupportedParameterType);
  CTA_GENERATE_EXCEPTION_CLASS(BadlyFormattedInteger);
  CTA_GENERATE_EXCEPTION_CLASS(BadlyFormattedSizeFileLimit);
  
  /// Constructor for mandatory options (they do not have default values)
  SourcedParameter(const std::string & category, const std::string & key): 
  m_category(category), m_key(key) {
    if (std::is_arithmetic<C>::value) {
      m_value=std::numeric_limits<C>::max();
    }
  }
  
  /// Constructor for optional options (they have default values)
  SourcedParameter(const std::string & category, const std::string & key, 
    const C & value, const std::string & source):
    m_category(category), m_key(key), m_value(value), m_source(source), m_set(true) {}
  C operator() () {
    if (m_set) return m_value;
    throw MandatoryParameterNotDefined(std::string("In SourcedParameter::operator(): "
      "value not defined for parameter \'" + m_category + "\' :"));
  }
  
  /// Function setting the parameter from a string (with integer interpretation)
  void set(const std::string & value, const std::string & source);
  
  /// Try and find the entry from the configuration file. Throw an exception for missing mandatory options.
  void setFromConfigurationFile(ConfigurationFile & configFile, const std::string & configFilePath) {
    try {
      auto & entry = configFile.entries.at(m_category).at(m_key);
      std::stringstream source;
      source << configFilePath << ":" << entry.line;
      set(entry.value, source.str());
    } catch (std::out_of_range &) {
      // If the config entry has a default, it's fine. If not, throw an exception.
      if (m_set) return;
      std::stringstream err;
      err << "In SourcedParameter::setFromConfigurationFile: mandatory parameter not found: "
          << "category=" << m_category << " key=" << m_key << " configFilePath=" << configFilePath;
      throw MandatoryParameterNotDefined(err.str());
    }
  }
  
  const C & value() { return m_value; }
  const std::string & category() { return m_category; }
  const std::string & key() { return m_key; }
  const std::string & source() { return m_source; }
  
  void log(log::Logger & logger) {
    // We log each parameter from a fresh context
    log::LogContext lc(logger);
    addLogParams(lc);
    lc.log(log::INFO, "Configuration entry");
  }
  
private:
  std::string m_category;      ///< The category of the parameter
  std::string m_key;           ///< The key of the parameter
  C m_value;                   ///< The value of the parameter
  std::string m_source;        ///< The source from which the parameter was gotten.
  bool m_set = false;          ///< Flag checking if the parameter was ever set.
  
  /// The specific part for each value type.
  void addLogParamForValue(log::LogContext & lc);
  
  /// A log param list representation of the sourced parameter.
  void addLogParams(log::LogContext & lc) {
    if (m_category.size())
      lc.pushOrReplace({"category", m_category});
    if (m_key.size())
      lc.pushOrReplace({"key", m_key});
    addLogParamForValue(lc);
    lc.pushOrReplace({"source", m_source});
  }
};



}}} // namespace cta::tape::daemon