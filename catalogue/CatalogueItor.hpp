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

#include "catalogue/CatalogueItorImpl.hpp"
#include "common/exception/Exception.hpp"

namespace cta {
namespace catalogue {

/**
 * A wrapper around an object that iterators over a list of items retrieved
 * from the CTA catalogue.
 *
 * This wrapper permits the user of the Catalogue API to use different
 * iterator implementations whilst only using a single iterator type.
 */
template <typename Item>
class CatalogueItor {
public:

  typedef CatalogueItorImpl<Item> Impl;

  /**
   * Constructor.
   */
  CatalogueItor():
    m_impl(nullptr) {
  }

  /**
   * Constructor.
   *
   * @param impl The object actually implementing this iterator.
   */
  CatalogueItor(Impl *const impl):
    m_impl(impl) {
    if(nullptr == impl) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: Pointer to implementation object is null");
    }
  }

  /**
   * Deletion of copy constructor.
   */
  CatalogueItor(const CatalogueItor &) = delete;

  /**
   * Move constructor.
   *
   * @param other The other object to be moved.
   */
  CatalogueItor(CatalogueItor &&other):
    m_impl(other.m_impl) {
    other.m_impl = nullptr;
  }

  /**
   * Destructor.
   */
  ~CatalogueItor() {
    delete m_impl;
  }

  /**
   * Deletion of copy assignment.
   */
  CatalogueItor &operator=(const CatalogueItor &) = delete;

  /**
   * Move assignment.
   */
  CatalogueItor &operator=(CatalogueItor &&rhs) {
    // Protect against self assignment
    if(this != &rhs) {
      // Avoid memory leak
      delete m_impl;

      m_impl = rhs.m_impl;
      rhs.m_impl = nullptr;
    }
    return *this;
  }

  /**
   * Returns true if a call to next would return another item.
   */
  bool hasMore() const {
    if(nullptr == m_impl) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: This iterator is invalid");
    }
    return m_impl->hasMore();
  }

  /**
   * Returns the next item or throws an exception if there isn't one.
   */
  Item next() {
    if(nullptr == m_impl) {
      throw exception::Exception(std::string(__FUNCTION__) + " failed: This iterator is invalid");
    }
    return m_impl->next();
  }

private:

  /**
   * The object actually implementing this iterator.
   */
  Impl *m_impl;

}; // class CatalogueItor

} // namespace catalogue
} // namespace cta