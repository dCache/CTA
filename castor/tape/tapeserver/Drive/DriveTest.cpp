/******************************************************************************
 *                      DriveTest.cpp
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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include <gtest/gtest.h>
#include <gmock/gmock-cardinalities.h>
#include "../SCSI/Device.hpp"
#include "../System/Wrapper.hpp"
#include "Drive.hpp"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::_;
using ::testing::An;

namespace UnitTests {

TEST(castor_tape_drives_Drive, OpensCorrectly) {
  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapper;
  sysWrapper.fake.setupSLC5();
  sysWrapper.delegateToFake();
  
  /* We expect the following calls: */
  EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
  EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, realpath(_, _)).Times(3);
  EXPECT_CALL(sysWrapper, open(_, _)).Times(21);
  EXPECT_CALL(sysWrapper, read(_, _, _)).Times(38);
  EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
  EXPECT_CALL(sysWrapper, ioctl(_,_,An<mtget*>())).Times(2);
  EXPECT_CALL(sysWrapper, close(_)).Times(21);
  EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(3);
  EXPECT_CALL(sysWrapper, stat(_,_)).Times(7);
  
  /* Test: detect devices, then open the device files */
  castor::tape::SCSI::DeviceVector dl(sysWrapper);
  for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin();
      i != dl.end(); i++) {
    if (castor::tape::SCSI::Types::tape == i->type) {
      castor::tape::drives::Drive drive(*i, sysWrapper);
    }
  }
}

TEST(castor_tape_drives_Drive, getPositionInfoAndPositionToLogicalObject) {
  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapper;
  sysWrapper.fake.setupSLC5();
  sysWrapper.delegateToFake();
  
  /* We expect the following calls: */
  EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
  EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, realpath(_, _)).Times(3);
  EXPECT_CALL(sysWrapper, open(_, _)).Times(21);
  EXPECT_CALL(sysWrapper, read(_, _, _)).Times(38);
  EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
  EXPECT_CALL(sysWrapper, ioctl(_,_,An<mtget*>())).Times(2);
  EXPECT_CALL(sysWrapper, close(_)).Times(21);
  EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(3);
  EXPECT_CALL(sysWrapper, stat(_,_)).Times(7);
  
  /* Test: detect devices, then open the device files */
  castor::tape::SCSI::DeviceVector dl(sysWrapper);
  for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin();
      i != dl.end(); i++) {
    if (castor::tape::SCSI::Types::tape == i->type) {
      castor::tape::drives::Drive dContainer(*i, sysWrapper);
      /* Compiler cannot implicitly use the conversion operator. Create an 
       * intermediate reference*/
      castor::tape::drives::DriveGeneric & drive = dContainer;
      castor::tape::drives::positionInfo posInfo;
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(1);      
      posInfo = drive.getPositionInfo();

      ASSERT_EQ(0xABCDEF12U,posInfo.currentPosition);
      ASSERT_EQ(0x12EFCDABU,posInfo.oldestDirtyObject);
      ASSERT_EQ(0xABCDEFU,posInfo.dirtyObjectsCount);
      ASSERT_EQ(0x12EFCDABU,posInfo.dirtyBytesCount);
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(1);      
      drive.positionToLogicalObject(0xABCDEF0);
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(1);
      posInfo = drive.getPositionInfo();
      
      ASSERT_EQ(0xABCDEF0U,posInfo.currentPosition);
      ASSERT_EQ(0xABCDEF0U,posInfo.oldestDirtyObject);
      ASSERT_EQ(0x0U,posInfo.dirtyObjectsCount);
      ASSERT_EQ(0x0U,posInfo.dirtyBytesCount);
    }
  }
} 
TEST(castor_tape_drives_Drive, setDensityAndCompression) {
  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapper;
  sysWrapper.fake.setupSLC5();
  sysWrapper.delegateToFake();
  
  /* We expect the following calls: */
  EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
  EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, realpath(_, _)).Times(3);
  EXPECT_CALL(sysWrapper, open(_, _)).Times(21);
  EXPECT_CALL(sysWrapper, read(_, _, _)).Times(38);
  EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
  EXPECT_CALL(sysWrapper, ioctl(_,_,An<mtget*>())).Times(2);
  EXPECT_CALL(sysWrapper, close(_)).Times(21);
  EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(3);
  EXPECT_CALL(sysWrapper, stat(_,_)).Times(7);
  
  /* Test: detect devices, then open the device files */
  castor::tape::SCSI::DeviceVector dl(sysWrapper);
  for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin();
      i != dl.end(); i++) {
    if (castor::tape::SCSI::Types::tape == i->type) {
      castor::tape::drives::Drive dContainer(*i, sysWrapper);
      /* Compiler cannot implicitly use the conversion operator. Create an 
       * intermediate reference*/
      castor::tape::drives::DriveGeneric & drive = dContainer;

      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(2);      
      drive.setDensityAndCompression();
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(2);      
      drive.setDensityAndCompression(true);
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(2);      
      drive.setDensityAndCompression(false);

      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(2);      
      drive.setDensityAndCompression(0x42,true);
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(2);      
      drive.setDensityAndCompression(0x46,false);
    }
  }
}

TEST(castor_tape_drives_Drive, setStDriverOptions) {
  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapper;
  sysWrapper.fake.setupSLC5();
  sysWrapper.delegateToFake();
  
  /* We expect the following calls: */
  EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
  EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, realpath(_, _)).Times(3);
  EXPECT_CALL(sysWrapper, open(_, _)).Times(21);
  EXPECT_CALL(sysWrapper, read(_, _, _)).Times(38);
  EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
  EXPECT_CALL(sysWrapper, ioctl(_,_,An<mtget*>())).Times(2);
  EXPECT_CALL(sysWrapper, close(_)).Times(21);
  EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(3);
  EXPECT_CALL(sysWrapper, stat(_,_)).Times(7);
  
  /* Test: detect devices, then open the device files */
  castor::tape::SCSI::DeviceVector dl(sysWrapper);
  for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin(); i != dl.end(); i++) {
    if (castor::tape::SCSI::Types::tape == i->type) {
      castor::tape::drives::Drive dContainer(*i, sysWrapper);
      /* Compiler cannot implicitly use the conversion operator. Create an 
       * intermediate reference*/
      castor::tape::drives::DriveGeneric & drive = dContainer;
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<struct mtop *>())).Times(1);
      drive.setSTBufferWrite(true);
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<struct mtop *>())).Times(1);
      drive.setSTBufferWrite(false);
    }
  }
}

TEST(castor_tape_drives_Drive, getDeviceInfo) {
  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapper;
  sysWrapper.fake.setupSLC5();
  sysWrapper.delegateToFake();
  
  /* We expect the following calls: */
  EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
  EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, realpath(_, _)).Times(3);
  EXPECT_CALL(sysWrapper, open(_, _)).Times(21);
  EXPECT_CALL(sysWrapper, read(_, _, _)).Times(38);
  EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
  EXPECT_CALL(sysWrapper, ioctl(_,_,An<mtget*>())).Times(2);
  EXPECT_CALL(sysWrapper, close(_)).Times(21);
  EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(3);
  EXPECT_CALL(sysWrapper, stat(_,_)).Times(7);
  
  /* Test: detect devices, then open the device files */
  castor::tape::SCSI::DeviceVector dl(sysWrapper);
  for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin();
      i != dl.end(); i++) {
    if (castor::tape::SCSI::Types::tape == i->type) {
      castor::tape::drives::Drive dContainer(*i, sysWrapper);
      /* Compiler cannot implicitly use the conversion operator. Create an 
       * intermediate reference*/
      castor::tape::drives::DriveGeneric & drive = dContainer;
      castor::tape::drives::deviceInfo devInfo;
      
      EXPECT_CALL(sysWrapper, ioctl(_,_,An<sg_io_hdr_t*>())).Times(2);      
      devInfo = drive.getDeviceInfo();

      ASSERT_EQ("STK     ",devInfo.vendor);
      ASSERT_EQ("T10000B         ",devInfo.product);
      ASSERT_EQ("0104",devInfo.productRevisionLevel );
      ASSERT_EQ("XYZZY_A2  ",devInfo.serialNumber );
    }
  }
}

TEST(castor_tape_drives_Drive, getCompressionAndClearCompressionStats) {
  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapper;
  sysWrapper.fake.setupSLC5();
  sysWrapper.delegateToFake();
  
  /* We expect the following calls: */
  EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
  EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
  EXPECT_CALL(sysWrapper, realpath(_, _)).Times(3);
  EXPECT_CALL(sysWrapper, open(_, _)).Times(25);
  EXPECT_CALL(sysWrapper, read(_, _, _)).Times(38);
  EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
  EXPECT_CALL(sysWrapper, ioctl(_,_,An<mtget*>())).Times(6);
  EXPECT_CALL(sysWrapper, close(_)).Times(25);
  EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(3);
  EXPECT_CALL(sysWrapper, stat(_,_)).Times(7);
  
  /* Test: detect devices, then open the device files */
  castor::tape::SCSI::DeviceVector dl(sysWrapper);
  for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin();
      i != dl.end(); i++) {
    if (castor::tape::SCSI::Types::tape == i->type) {
      castor::tape::drives::DriveGeneric *drive;
      castor::tape::drives::compressionStats comp;
      
        {
          drive = new castor::tape::drives::DriveT10000(*i, sysWrapper);

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          comp = drive->getCompression();
          ASSERT_EQ(0xABCDEF1122334455ULL, comp.fromHost);
          ASSERT_EQ(0x2233445566778899ULL, comp.toHost);
          ASSERT_EQ(0x99AABBCCDDEEFF11ULL, comp.fromTape);
          ASSERT_EQ(0x1122334455667788ULL, comp.toTape);

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          drive->clearCompressionStats();

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          comp = drive->getCompression();
          ASSERT_EQ(0U, comp.fromHost);
          ASSERT_EQ(0U, comp.toHost);
          ASSERT_EQ(0U, comp.fromTape);
          ASSERT_EQ(0U, comp.toTape);

          delete drive;
        }
        {
          drive = new castor::tape::drives::DriveIBM3592(*i, sysWrapper);

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          comp = drive->getCompression();
          ASSERT_EQ(0x4488CD1000ULL, comp.fromHost);
          ASSERT_EQ(0x15599DE2000ULL, comp.toHost);
          ASSERT_EQ(0x377BBFC4400ULL, comp.fromTape);
          ASSERT_EQ(0x266AAEF3000ULL, comp.toTape);

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          drive->clearCompressionStats();

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          comp = drive->getCompression();
          ASSERT_EQ(0U, comp.fromHost);
          ASSERT_EQ(0U, comp.toHost);
          ASSERT_EQ(0U, comp.fromTape);
          ASSERT_EQ(0U, comp.toTape);

          delete drive;
        }
        {
          drive = new castor::tape::drives::DriveLTO(*i, sysWrapper);

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          comp = drive->getCompression();
          ASSERT_EQ(0x209DB2BE187D9ULL, comp.fromHost);
          ASSERT_EQ(0x105707026D088ULL, comp.toHost);
          ASSERT_EQ(0x928C54DFC8A11ULL, comp.fromTape);
          ASSERT_EQ(0xA2D3009B64262ULL, comp.toTape);

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          drive->clearCompressionStats();

          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          comp = drive->getCompression();
          ASSERT_EQ(0U, comp.fromHost);
          ASSERT_EQ(0U, comp.toHost);
          ASSERT_EQ(0U, comp.fromTape);
          ASSERT_EQ(0U, comp.toTape);

          delete drive;
        }
    }
  }
}

TEST(castor_tape_drives_Drive, getTapeAlerts) {

  /**
   * "Local function" allowing the test to be run twice (for SLC5 and then for 
   * SLC6).
   */
  struct {
    void operator() (castor::tape::System::mockWrapper & sysWrapper) {
      /* We expect the following calls: */
      EXPECT_CALL(sysWrapper, opendir(_)).Times(AtLeast(3));
      EXPECT_CALL(sysWrapper, readdir(_)).Times(AtLeast(30));
      EXPECT_CALL(sysWrapper, closedir(_)).Times(AtLeast(3));
      EXPECT_CALL(sysWrapper, realpath(_, _)).Times(AtLeast(3));
      EXPECT_CALL(sysWrapper, open(_, _)).Times(AtLeast(21));
      EXPECT_CALL(sysWrapper, read(_, _, _)).Times(AtLeast(38));
      EXPECT_CALL(sysWrapper, write(_, _, _)).Times(0);
      EXPECT_CALL(sysWrapper, ioctl(_, _, An<mtget*>())).Times(2);
      EXPECT_CALL(sysWrapper, close(_)).Times(AtLeast(21));
      EXPECT_CALL(sysWrapper, readlink(_, _, _)).Times(AtLeast(3));
      EXPECT_CALL(sysWrapper, stat(_, _)).Times(AtLeast(7));

      /* Test: detect devices, then open the device files */
      castor::tape::SCSI::DeviceVector dl(sysWrapper);
      for (std::vector<castor::tape::SCSI::DeviceInfo>::iterator i = dl.begin();
          i != dl.end(); i++) {
        if (castor::tape::SCSI::Types::tape == i->type) {
          castor::tape::drives::Drive dContainer(*i, sysWrapper);
          /* Compiler cannot implicitly use the conversion operator. Create an 
           * intermediate reference*/
          castor::tape::drives::DriveGeneric & drive = dContainer;
          EXPECT_CALL(sysWrapper, ioctl(_, _, An<sg_io_hdr_t*>())).Times(1);
          std::vector<std::string> alerts = drive.getTapeAlerts();
          ASSERT_EQ(3U, alerts.size());
          ASSERT_FALSE(alerts.end() == 
              find(alerts.begin(), alerts.end(), 
              std::string("Unexpected tapeAlert code: 0x41")));
          ASSERT_FALSE(alerts.end() == find(alerts.begin(), alerts.end(), 
              std::string("Obsolete tapeAlert code: 0x28")));
          ASSERT_FALSE(alerts.end() == find(alerts.begin(), alerts.end(), 
              std::string("Forced eject")));
        }
      }
    }
  } test_functor;


  /* Prepare the test harness */
  castor::tape::System::mockWrapper sysWrapperSLC5;
  castor::tape::System::mockWrapper sysWrapperSLC6;
  sysWrapperSLC5.fake.setupSLC5();
  sysWrapperSLC6.fake.setupSLC6();
  sysWrapperSLC5.delegateToFake();
  sysWrapperSLC6.delegateToFake();
  test_functor(sysWrapperSLC5);
  test_functor(sysWrapperSLC6);
}

}
