#include "cta/DriveQuota.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::DriveQuota::DriveQuota():
  m_minDrives(0),
  m_maxDrives(0) {
}

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::DriveQuota::DriveQuota(const uint32_t minDrives, const uint32_t maxDrives):
  m_minDrives(minDrives),
  m_maxDrives(maxDrives) {
}

//------------------------------------------------------------------------------
// getMinDrives
//------------------------------------------------------------------------------
uint32_t cta::DriveQuota::getMinDrives() const throw() {
  return m_minDrives;
}

//------------------------------------------------------------------------------
// getMaxDrives
//------------------------------------------------------------------------------
uint32_t cta::DriveQuota::getMaxDrives() const throw() {
  return m_maxDrives;
}
