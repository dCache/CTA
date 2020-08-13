/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2019  CERN
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
#include <vector>
#include <memory>
#include "castor/tape/tapeserver/RAO/RAOConfig.hpp"
#include "castor/tape/tapeserver/drive/DriveInterface.hpp"
#include "castor/tape/tapeserver/SCSI/Structures.hpp"
#include "scheduler/RetrieveJob.hpp"
#include "common/optional.hpp"
#include "RAOAlgorithmFactory.hpp"
#include "common/log/LogContext.hpp"

namespace castor { namespace tape { namespace tapeserver { namespace rao {
  
/**
 * This class will be used to manage everything that is linked to RAO.
 * It centralizes all the RAO-related task that are executed during the
 * RecallTaskInjector lifecycle.
 */
class RAOManager {
public:
  /**
   * Default constructor, disable the RAO
   */
  RAOManager();
  
  /**
   * Copy constructor
   * @param manager the RAOManager to copy
   */
  RAOManager(const RAOManager & manager);
  
  /**
   * Constructor of a RAO manager
   * @param config the configuration of the RAO to manage
   * @param drive the DriveInterface of the drive that is mounting
   */
  RAOManager(const RAOConfig & config, castor::tape::tapeserver::drive::DriveInterface * drive);
  
  /**
   * Assignment operator
   * */
  RAOManager & operator=(const RAOManager & manager);
  
  /**
   * Returns true if RAO will be used, false otherwise
   */
  bool useRAO() const;
  
  /**
   * Returns true if the manager has informations about the drive's User Data Segments limits to
   * perform RAO for enteprise drive
   */
  bool hasUDS() const;
  
  /**
   * Returns true if the manager can ask for an Enterprise RAO Algorithm
   * false otherwise
   */
  bool isDriveEnterpriseEnabled() const;
  
  /**
   * Returns the pointer to the interface of that is mounting
   */
  castor::tape::tapeserver::drive::DriveInterface * getDrive() const;
  
  /**
   * Returns the number of files that will be supported by the RAO algorithm
   */
  cta::optional<uint64_t> getMaxFilesSupported() const;
  
  /**
   * Returns the RAOConfig that is used by this RAOManager
   */
  RAOConfig getConfig() const;
  
  /**
   * Disable the RAO algorithm
   */
  void disableRAO();
  
  /**
   * Set the enterprise RAO User Data Segments limits
   * that will be used by this manager to perform the Enterprise RAO query on the drive
   * @param raoLimits the enterprise RAO user data segments limits
   */
  void setEnterpriseRAOUdsLimits(const SCSI::Structures::RAO::udsLimits & raoLimits);
  
  /**
   * Query the RAO of the files passed in parameter
   * @param jobs the vector of jobs to query the RAO
   * @param lc the log context
   * @return the vector with re-arranged indexes of the jobs passed in parameter
   * It does not returns the fseqs, but a vector of indexes that will be used by the recall task injector to pick
   * the correct job after RAO has been done
   */
  std::vector<uint64_t> queryRAO(const std::vector<std::unique_ptr<cta::RetrieveJob>> & jobs, cta::log::LogContext & lc);
  
  virtual ~RAOManager();
  
private:
  RAOConfig m_config;
  /** Enterprise Drive-specific RAO parameters */
  SCSI::Structures::RAO::udsLimits m_enterpriseRaoLimits;
  //Is true if the drive have been able to get the RAO UDS limits numbers
  bool m_hasUDS = false;
  //The maximum number of files that will be considered for RAO
  cta::optional<uint64_t> m_maxFilesSupported;
  //Pointer to the drive interface of the drive currently used by the tapeserver
  castor::tape::tapeserver::drive::DriveInterface * m_drive;
  bool m_isDriveEnterpriseEnabled = false;
};

}}}}
