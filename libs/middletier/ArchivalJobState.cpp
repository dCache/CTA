#include "ArchivalJob.hpp"

//------------------------------------------------------------------------------
// toStr
//------------------------------------------------------------------------------
const char *cta::ArchivalJobState::toStr(const Enum enumValue) throw() {
  switch(enumValue) {
  case NONE   : return "NONE";
  case PENDING: return "PENDING";
  case SUCCESS: return "SUCCESS";
  case ERROR  : return "ERROR";
  default     : return "UNKNOWN";
  }
}
