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

#include "ValueCountMap.hpp"
#include "common/exception/Exception.hpp"

#include <algorithm>

namespace cta { namespace objectstore {

ValueCountMap::ValueCountMap(google::protobuf::RepeatedPtrField<serializers::ValueCountPair>* valueCountMap): 
  m_valueCountMap(*valueCountMap) { }

void ValueCountMap::decCount(uint64_t value) {
  // Find the entry for this value. Failing is an error.
  auto counter = std::find_if(m_valueCountMap.begin(), m_valueCountMap.end(),
      [&](decltype(*m_valueCountMap.begin()) pair) {
        return pair.value() == value;
      });
  if (counter == m_valueCountMap.end()) {
    std::stringstream err;
    err << "In ValueCountMap::decCount: no entry found for value=" << value;
    throw  cta::exception::Exception(err.str());
  }
  // Decrement the value and remove the entry if needed.
  if (counter->count() < 1) {
    std::stringstream err;
    err << "In ValueCountMap::decCount: entry with wrong count value=" << value << " count=" << counter->count();
    throw  cta::exception::Exception(err.str());
  }
  counter->set_count(counter->count()-1);
  if (!counter->count()) {
    auto size=m_valueCountMap.size();
    counter->Swap(&(*(m_valueCountMap.end()-1)));
    m_valueCountMap.RemoveLast();
    // Cross check that the size has decreased.
    if (size -1 != m_valueCountMap.size()) {
      std::stringstream err;
      err << "In ValueCountMap::decCount: unexpected size after trimming empty entry. expectedSize=" << size -1 << " newSize=" << m_valueCountMap.size();
      throw  cta::exception::Exception(err.str());
    }
    // Cross check we cannot find the value.
    auto counter2 = std::find_if(m_valueCountMap.begin(), m_valueCountMap.end(), 
          [&](decltype(*m_valueCountMap.begin()) pair) { return pair.value() == value; });
    if (m_valueCountMap.end() != counter2) {
      std::stringstream err;
      err << "In ValueCountMap::decCount: still found the value after trimming empty entry. value=" << counter2->value() << " count=" << counter2->count();
      throw  cta::exception::Exception(err.str());
    }
  }
}

void ValueCountMap::incCount(uint64_t value) {
  // Find the entry for this value (might fail)
  auto counter = std::find_if(m_valueCountMap.begin(), m_valueCountMap.end(),
      [&](decltype(*m_valueCountMap.begin()) pair) {
        return pair.value() == value; 
      });
  if (counter != m_valueCountMap.end()) {
    if (counter->count() < 1) {
      std::stringstream err;
      err << "In ValueCountMap::incCount: unexpected count value=" << counter->value() << " count=" << counter->count();
      throw  cta::exception::Exception(err.str());
    } else {
      counter->set_count(counter->count()+1);
    }
  } else {
    // Create the new entry if necessary.
    auto newCounter = m_valueCountMap.Add();
    newCounter->set_value(value);
    newCounter->set_count(1);
  }
}

uint64_t ValueCountMap::maxValue() {
  if (!m_valueCountMap.size())
    throw  cta::exception::Exception("In ValueCountMap::maxValue: empty map");
  uint64_t ret = std::numeric_limits<uint64_t>::min();
  std::for_each(m_valueCountMap.begin(), m_valueCountMap.end(),
      [&](decltype(*m_valueCountMap.begin()) pair) {
        if (ret<pair.value()) ret=pair.value();
      });
  return ret;
}

uint64_t ValueCountMap::minValue() {
  if (!m_valueCountMap.size()) throw  cta::exception::Exception("In ValueCountMap::minValue: empty map");
  uint64_t ret = std::numeric_limits<uint64_t>::max();
  std::for_each(m_valueCountMap.begin(), m_valueCountMap.end(),
      [&](decltype(*m_valueCountMap.begin()) pair) {
        if (ret>pair.value()) ret=pair.value();
      });
  return ret;
}

uint64_t ValueCountMap::total() {
  uint64_t ret = 0;
  std::for_each(m_valueCountMap.begin(), m_valueCountMap.end(), 
      [&](decltype(*m_valueCountMap.begin()) pair) {
        if (pair.count()<1) {
          std::stringstream err;
          err << "In ValueCountMap::total: unexpected count value=" << pair.value() << " count=" << pair.count();
          throw  cta::exception::Exception(err.str());
        }
        ret += pair.count();
      });
  return ret;
}



}} // namespace cta::objectstore
