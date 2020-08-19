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


#include "RAOManager.hpp"
#include "EnterpriseRAOAlgorithm.hpp"
#include "EnterpriseRAOAlgorithmFactory.hpp"
#include "NonConfigurableRAOAlgorithmFactory.hpp"
#include "RAOAlgorithmFactoryFactory.hpp"
#include "catalogue/Catalogue.hpp"

namespace castor { namespace tape { namespace tapeserver { namespace rao {
  
RAOManager::RAOManager() {

}
  
RAOManager::RAOManager(const RAOConfigurationData & config, drive::DriveInterface * drive, cta::catalogue::Catalogue * catalogue):m_raoConfigurationData(config), 
  m_drive(drive), m_catalogue(catalogue){}

RAOManager::RAOManager(const RAOManager& manager){
  if(this != &manager){
    m_catalogue = manager.m_catalogue;
    m_drive = manager.m_drive;
    m_enterpriseRaoLimits = manager.m_enterpriseRaoLimits;
    m_hasUDS = manager.m_hasUDS;
    m_isDriveEnterpriseEnabled = manager.m_isDriveEnterpriseEnabled;
    m_maxFilesSupported = manager.m_maxFilesSupported;
    m_raoConfigurationData = manager.m_raoConfigurationData;
  }
}

RAOManager& RAOManager::operator=(const RAOManager& manager) {
  if(this != &manager){
    m_catalogue = manager.m_catalogue;
    m_drive = manager.m_drive;
    m_enterpriseRaoLimits = manager.m_enterpriseRaoLimits;
    m_hasUDS = manager.m_hasUDS;
    m_isDriveEnterpriseEnabled = manager.m_isDriveEnterpriseEnabled;
    m_maxFilesSupported = manager.m_maxFilesSupported;
    m_raoConfigurationData = manager.m_raoConfigurationData;
  }
  return *this;
}


RAOManager::~RAOManager() {
}

bool RAOManager::useRAO() const{
  return m_raoConfigurationData.useRAO();
}

bool RAOManager::hasUDS() const {
  return m_hasUDS;
}

bool RAOManager::isDriveEnterpriseEnabled() const {
  return m_isDriveEnterpriseEnabled;
}

castor::tape::tapeserver::drive::DriveInterface* RAOManager::getDrive() const {
  return m_drive;
}

cta::catalogue::Catalogue* RAOManager::getCatalogue() const {
  return m_catalogue;
}


void RAOManager::disableRAO(){
  m_raoConfigurationData.disableRAO();
}

void RAOManager::setEnterpriseRAOUdsLimits(const SCSI::Structures::RAO::udsLimits& raoLimits) {
  m_enterpriseRaoLimits = raoLimits;
  m_maxFilesSupported = raoLimits.maxSupported;
  m_hasUDS = true;
  m_isDriveEnterpriseEnabled = true;
}

cta::optional<uint64_t> RAOManager::getMaxFilesSupported() const{
  return m_maxFilesSupported;
}

RAOConfigurationData RAOManager::getRAODataConfig() const {
  return m_raoConfigurationData;
}

std::vector<uint64_t> RAOManager::queryRAO(const std::vector<std::unique_ptr<cta::RetrieveJob>> & jobs, cta::log::LogContext & lc){
  RAOAlgorithmFactoryFactory raoAlgoFactoryFactory(*this,lc);
  std::unique_ptr<RAOAlgorithm> raoAlgo = raoAlgoFactoryFactory.createAlgorithmFactory()->createRAOAlgorithm();
  return raoAlgo->performRAO(jobs);
}


}}}}