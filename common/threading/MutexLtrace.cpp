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

#include "Mutex.hpp"
#include "MutexLocker.hpp"
#include <mutex>

/*
 * A small test used to show that std::mutex calls pthread_mutex_destroy. This
 * causes confusion in helgrind when detecting race conditions as a several mutex can 
 * land on the same address, and helgrind cannot see they are different.
 * 
 * The proof is show below (libXrd static structures destructors ellipsed. 
 * Second scope does not call pthread_mutex_destroy:
 * 
[eric@localhost CTA (master)]$ ltrace -n 2 -C -i -e pthread*  ../CTA-build/common/mutexLtrace  2>&1 | grep -v libXrd
[0x7f6a67a440fd] libctacommon.so->pthread_mutexattr_init(0x7ffe915f2310, 0x7f6a67a61f3b, 0x7f6a661ee49e, 0x1211508) = 0
[0x7f6a67a44157] libctacommon.so->pthread_mutexattr_settype(0x7ffe915f2310, 2, 0x7f6a661ee31e, 0x1211508) = 0
[0x7f6a67a441b3] libctacommon.so->pthread_mutex_init(0x7ffe915f2350, 0x7ffe915f2310, 0x7ffe915f2310, 0x1211578) = 0
[0x7f6a67a44208] libctacommon.so->pthread_mutexattr_destroy(0x7ffe915f2310, 0x7f6a67a6200e, 0x7f6a661ee31e, 0x1211508) = 0
[0x7f6a67a443ab] libctacommon.so->pthread_mutex_lock(0x7ffe915f2350, 0x7f6a67a62010, 0x7f6a661ee6f2, 0x1211578) = 0
[0x7f6a67a44465] libctacommon.so->pthread_mutex_unlock(0x7ffe915f2350, 0x7f6a67a62092, 0x7f6a661ee52e, 0x1211508) = 0
[0x7f6a67a44349] libctacommon.so->pthread_mutex_destroy(0x7ffe915f2350, 0xffffffff, 0x7f6a6645d790, 0) = 0
[0x40109f] mutexLtrace->pthread_mutex_lock(0x7ffe915f2350, 0x7ffe915f2350, 0, 0) = 0
[0x4010f1] mutexLtrace->pthread_mutex_unlock(0x7ffe915f2350, 0, 0, 0) = 0
[0xffffffffffffffff] +++ exited (status 0) +++
 * 
 * This problem is also described here:
 * https://www.spinics.net/lists/ceph-devel/msg32724.html
 */

int main (void) {
  {
    cta::threading::Mutex m;
    cta::threading::MutexLocker ml(m);
  }
  {
    std::mutex m;
    std::lock_guard<std::mutex> lg(m);
  }
}