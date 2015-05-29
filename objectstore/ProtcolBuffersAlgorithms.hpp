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

#include <google/protobuf/repeated_field.h>
#include "common/exception/Exception.hpp"

namespace cta { namespace objectstore { namespace serializers {
void removeString(::google::protobuf::RepeatedPtrField< ::std::string>* field, 
  const std::string & value);

template <class C1, class C2>
void removeOccurences(::google::protobuf::RepeatedPtrField<C1>* field,
  const C2 & value) {
  bool found;
  do {
    found=false;
    for (size_t i=0; i<(size_t)field->size(); i++) {
      if (value == field->Get(i)) {
        found = true;
        field->SwapElements(i, field->size()-1);
        field->RemoveLast();
        break;
      }
    }
  } while (found);
}
class NotFound: public cta::exception::Exception {
  public:
    NotFound(const std::string & w): cta::exception::Exception(w) {}
  };
  
size_t findString(::google::protobuf::RepeatedPtrField< ::std::string>* field, 
  const std::string & value);

size_t findStringFrom(::google::protobuf::RepeatedPtrField< ::std::string>* field,
  size_t fromIndex, const std::string & value);

template <class C1, class C2>
C1 & findElement(::google::protobuf::RepeatedPtrField<C1>* field, const C2 & value) {
  for (auto i=field->begin(); i!= field->end(); i++) {
    if (value == *i) {
      return *i;
    }
  }
  throw NotFound("In cta::objectsotre::serializers::findElement(non-const): element not found");
}

template <class C1, class C2>
const C1 & findElement(const ::google::protobuf::RepeatedPtrField<C1>& field, const C2 & value) {
  for (auto i=field.begin(); i!= field.end(); i++) {
    if (value == *i) {
      return *i;
    }
  }
  throw NotFound("In cta::objectsotre::serializers::findElement(const): element not found");
}

template <class C1, class C2>
bool isElementPresent(const ::google::protobuf::RepeatedPtrField<C1>& field, const C2 & value) {
  for (auto i=field.begin(); i!= field.end(); i++) {
    if (value == *i) {
      return true;
    }
  }
  return false;
}

}}}