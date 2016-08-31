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
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include "common/exception/Exception.hpp"
#include "common/exception/Errnum.hpp"
#include <errno.h>

#include <gtest/gtest.h>
#include <gmock/gmock-cardinalities.h>

namespace unitTests {
  class Nested {
  public:
    void f1();
    void f2();
    Nested();
  };
  
  /* Prevent inlining: it makes this test fail! */
  void __attribute__((noinline)) Nested::f1() {
    throw cta::exception::Exception("Throwing in Nested's constructor");
  }
  
  /* Prevent inlining: it makes this test fail!
   * Even with that, f2 does not show up in the trace */
  void __attribute__((noinline)) Nested::f2() {
    f1();
  }
  
  /* Prevent inlining: it makes this test fail! */
  __attribute__((noinline))  Nested::Nested() {
    f2();
  }

  TEST(castor_exceptions, stacktrace_with_demangling) {
    try {
      Nested x;
    } catch (cta::exception::Exception & e) {
      std::string bt = e.backtrace();
      ASSERT_NE(std::string::npos, bt.find("Nested::f1"));
      ASSERT_NE(std::string::npos, bt.find("cta::exception::Backtrace::Backtrace"));
      ASSERT_EQ("Throwing in Nested's constructor", std::string(e.getMessageValue()));
      std::string fullWhat(e.what());
      ASSERT_NE(std::string::npos, fullWhat.find("Nested::f1"));
    }
  }
  
    TEST(castor_exceptions, stacktrace_in_std_exception) {
    try {
      Nested x;
    } catch (std::exception & e) {
      std::string fullWhat(e.what());
      ASSERT_NE(std::string::npos, fullWhat.find("Nested::f1"));
      ASSERT_NE(std::string::npos, fullWhat.find("Throwing in Nested's constructor"));
    }
  }
  
  TEST(cta_exceptions, errnum_throwing) {
    /* Mickey Mouse test as we had trouble which throwing Errnum (with errno=ENOENT)*/
    errno = ENOENT;
    try {
      throw cta::exception::Errnum("Test ENOENT");
    } catch (std::exception & e) {
      std::string temp = e.what();
      temp += " ";
    }
  }

  TEST(cta_exceptions, Errnum_throwers) {
    /* throwOnReturnedErrno */
    ASSERT_NO_THROW(cta::exception::Errnum::throwOnReturnedErrno(0, "Context"));
    ASSERT_THROW(cta::exception::Errnum::throwOnReturnedErrno(ENOSPC, "Context"),
      cta::exception::Errnum);
    
    /* throwOnNonZero */
    errno = ENOENT;
    ASSERT_NO_THROW(cta::exception::Errnum::throwOnNonZero(0, "Context"));
    ASSERT_THROW(cta::exception::Errnum::throwOnNonZero(-1, "Context"),
      cta::exception::Errnum);
    
    /* throwOnMinusOne */
    errno = ENOENT;
    ASSERT_NO_THROW(cta::exception::Errnum::throwOnMinusOne(0, "Context"));
    ASSERT_THROW(cta::exception::Errnum::throwOnMinusOne(-1, "Context"),
      cta::exception::Errnum);
    
    /* throwOnNegative */
    errno = ENOENT;
    ASSERT_NO_THROW(cta::exception::Errnum::throwOnNegative(0, "Context"));
    ASSERT_THROW(cta::exception::Errnum::throwOnNegative(-1, "Context"),
      cta::exception::Errnum); 

    /* throwOnNull */
    errno = ENOENT;
    ASSERT_NO_THROW(cta::exception::Errnum::throwOnNull(this, "Context"));
    ASSERT_THROW(cta::exception::Errnum::throwOnNull(NULL, "Context"),
      cta::exception::Errnum);

    /* throwOnZero */
    errno = ENOENT;
    ASSERT_NO_THROW(cta::exception::Errnum::throwOnZero(1, "Context"));
    ASSERT_THROW(cta::exception::Errnum::throwOnZero(0, "Context"),
      cta::exception::Errnum); 
  }
}
