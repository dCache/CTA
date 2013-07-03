// ----------------------------------------------------------------------
// File: Drive/Drive.hh
// Author: Eric Cano - CERN
// ----------------------------------------------------------------------

/************************************************************************
 * Tape Server                                                          *
 * Copyright (C) 2013 CERN/Switzerland                                  *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 ************************************************************************/

#pragma once

#include "../SCSI/Device.hh"
#include "../System/Wrapper.hh"
#include "../Exception/Exception.hh"

/**
 * Class wrapping the tape server. Has to be templated (and hence fully in .hh)
 * to allow unit testing against system wrapper.
 */
namespace Tape {

  /**
   * Class abstracting the tape drives. Gets initialized from 
   */
  template <class sysWrapperClass>
  class Drive {
  public:
    Drive(SCSI::DeviceInfo di, sysWrapperClass & sw): m_sysWrapper(sw),
            m_SCSIInfo(di), m_tapeFD(-1), m_genericFD(-1)
    {
      /* Open the device files */
      m_tapeFD = m_sysWrapper.open(m_SCSIInfo.st_dev.c_str(), 0);
      if (-1 == m_tapeFD)
        throw Tape::Exceptions::Errnum(std::string("Could not open device file: "+ m_SCSIInfo.st_dev));
      m_genericFD = m_sysWrapper.open(m_SCSIInfo.sg_dev.c_str(), 0);
      if (-1 == m_genericFD)
        throw Tape::Exceptions::Errnum(std::string("Could not open device file: "+ m_SCSIInfo.sg_dev));
      /* Read drive status */
      if (-1 == m_sysWrapper.ioctl(m_tapeFD, MTIOCGET, &m_mtInfo))
        throw Tape::Exceptions::Errnum(std::string("Could not read drive status: "+ m_SCSIInfo.st_dev));
      /* Read Generic SCSI information (INQUIRY) */
      
    }
    virtual ~Drive() {
      if(-1 != m_tapeFD)
        m_sysWrapper.close(m_tapeFD);
      if (-1 != m_genericFD)
        m_sysWrapper.close(m_genericFD);
    }
  private:
    SCSI::DeviceInfo m_SCSIInfo;
    int m_tapeFD;
    int m_genericFD;
#undef ConvenientCoding
#ifdef ConvenientCoding
    syntax error here!!!; /* So we don't compile in this configuration */
    /* This makes code completion more efficient in editors */
    Tape::System::fakeWrapper & m_sysWrapper;
#else
    sysWrapperClass & m_sysWrapper;
#endif
    struct mtget m_mtInfo;
  };
}
