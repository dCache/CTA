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
#include "common/log/Logger.hpp"
#include "common/utils/Utils.hpp"

#include <string>
#include <map>

namespace cta { namespace common {
    
    /**
     * represents a category from the CTA configuration file
     */
    typedef std::map<std::string, std::string> ConfCategory;

    /**
     * a class representing the configuration of castor.
     * This configurations is obtained from the local file given in the
     * constructor and will be updated regularly. The time between two
     * updates is taken from the Config/ExpirationDelay entry of the
     * configuration itself and defaults to 5mn if no such entry is found
     */
    class Configuration {

    public:
      
      /**
       * Private exceptions for this 
       */
      CTA_GENERATE_EXCEPTION_CLASS(InvalidConfigEntry);
      CTA_GENERATE_EXCEPTION_CLASS(NoEntry);
        
    public:
      
      /**
       * constructor
       * @param fileName the file that should be used to build the configuration
       */
      Configuration(std::string fileName = "/etc/cta/cta.conf");

      /**
       * copy constructor
       * @param other instance of CastorConfiguration class
       */
      Configuration(const Configuration & other);

      /**
       * destructor
       */
      virtual ~Configuration();

      /**
       * assignment operator
       * @param other instance of CastorConfiguration class
       */
      Configuration & operator=(const Configuration & other);

      /**
       * Retrieves a configuration entry.
       *
       * If this method is passed a logger object then it will log the value
       * of the configuration entry together with an indication of whether the
       * value was found in the castor configuration file or whether the
       * specified default value was used instead.
       *
       * @param category the category of the entry
       * @param key the key of the entry
       * @param defaultValue the value to be returned if the configuration entry
       * is not in the configuration file
       * @param log pointer to NULL or an optional logger object
       */
      const std::string& getConfEntString(const std::string &category,
        const std::string &key, const std::string &defaultValue,
        log::Logger *const log = NULL);

      /**
       * Retrieves a configuration entry.
       *
       * Besides other possible exceptions, this method throws a
       * castor::exception::NoEntry exception if the specified configuration
       * entry is not in the configuration file.
       *
       * If this method is passed a logger object then this method will log the
       * the value of the configuration entry.
       *
       * @param category the category of the entry
       * @param key the key of the entry
       * @param log pointer to NULL or an optional logger object
       */
      const std::string& getConfEntString(const std::string &category,
        const std::string &key, log::Logger *const log = NULL);

      /**
       * Retrieves a configuration entry as an integer.
       *
       * If this method is passed a logger object then it will log the value
       * of the configuration entry together with an indication of whether the
       * value was found in the castor configuration file or whether the
       * specified default value was used instead.
       *
       * @param category category of the configuration parameter
       * @param name category of the configuration parameter
       * @param defaultValue the value to be returned if the configuration entry
       * is not in the configuration file
       * @param log pointer to NULL or an optional logger object
       * @return the integer value
       */
      template<typename T> T getConfEntInt(const std::string &category,
        const std::string &key, const T defaultValue,
        log::Logger *const log = NULL)  {
        std::string strValue;
        try {
          strValue = getConfEntString(category, key);
        } catch(cta::exception::Exception &ex) {
          if(NULL != log) {
            std::list<log::Param> params = {
              log::Param("category", category),
              log::Param("key", key),
              log::Param("value", defaultValue),
              log::Param("source", "DEFAULT")};
            (*log)(log::INFO, "Configuration entry", params);
          }
          return defaultValue;
        }

        if (!Utils::isValidUInt(strValue.c_str())) {
          InvalidConfigEntry ex(category.c_str(),
            key.c_str(), strValue.c_str());
          ex.getMessage() << "Failed to get configuration entry " << category <<
            ":" << key << ": Value is not a valid unsigned integer: value=" <<
            strValue;
          throw ex;
        }

        T value;
        std::stringstream ss;
        ss << strValue.c_str();
        ss >> value;

        if(NULL != log) {
          std::list<log::Param> params = {
            log::Param("category", category),
            log::Param("key", key),
            log::Param("value", value),
            log::Param("source", m_fileName)};
          (*log)(log::INFO, "Configuration entry", params);
        }

        return value;
      }

      /**
       * Retrieves a configuration entry as an integer.
       *
       * Besides other possible exceptions, this method throws a
       * castor::exception::NoEntry exception if the specified configuration
       * entry is not in the configuration file.
       *
       * @param category category of the configuration parameter
       * @param name category of the configuration parameter
       * @param log pointer to NULL or an optional logger object
       * @return the integer value
       */
      template<typename T> T getConfEntInt(const std::string &category,
        const std::string &key, log::Logger *const log = NULL)  {
        const std::string strValue = getConfEntString(category, key);

        if (!Utils::isValidUInt(strValue.c_str())) {
          InvalidConfigEntry ex(category.c_str(),
            key.c_str(), strValue.c_str());
          ex.getMessage() << "Failed to get configuration entry " << category <<
            ":" << key << ": Value is not a valid unsigned integer: value=" <<
            strValue;
          throw ex;
        }

        T value;
        std::stringstream ss;
        ss << strValue.c_str();
        ss >> value;

        if(NULL != log) {
          std::list<log::Param> params = {
            log::Param("category", category),
            log::Param("key", key),
            log::Param("value", value),
            log::Param("source", m_fileName)};
          (*log)(log::INFO, "Configuration entry", params);
        }

        return value;
      }

    private:

      /**
       * check whether the configuration should be renewed
       */
      bool isStale() ;

      /**
       * tries to renew the configuration.
       * That is : take the write lock to do it, check whether it's needed
       * and do it only if needed before releasing the lock
       */
      void tryToRenewConfig() ;

      /**
       * gets current timeout value (in seconds)
       * this function does not take any lock while reading the
       * configuration. So it should never be called without holding
       * a read or a write lock
       */
      int getTimeoutNolock() ;

      /**
       * renews the configuration
       * this function does not take any lock while renewing the
       * configuration. So it should never be called without holding
       * the write lock
       */
      void renewConfigNolock() ;

    private:

      /**
       * fileName to be used when updating the configuration
       */
      std::string m_fileName;

      /**
       * last time we've updated the configuration
       */
      time_t m_lastUpdateTime;

      /**
       * the dictionnary of configuration items
       * actually a dictionnary of ConfCategories, which are dictionnaries of entries
       */
      std::map<std::string, ConfCategory> m_config;

      /**
       * lock to garantee safe access to the configuration, lastUpdateTime and timeout
       */
      pthread_rwlock_t m_lock;

    };

  } // namespace common
} // namespace castor  

