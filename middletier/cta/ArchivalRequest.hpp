#pragma once

#include "cta/UserRequest.hpp"

#include <stdint.h>
#include <string>

namespace cta {

/**
 * Abstract class representing a user request to archive some data.
 */
class ArchivalRequest: public UserRequest {
public:

  /**
   * Constructor.
   */
  ArchivalRequest();

  /**
   * Destructor.
   */
  virtual ~ArchivalRequest() throw() = 0;

  /**
   * Constructor.
   *
   * @param storageClassName The name of the storage class.
   * @param priority The priority of the request.
   */
  ArchivalRequest(const std::string &storageClassName, const uint64_t priority);

  /**
   * Returns the name of the storage class.
   *
   * @return The name of the storage class.
   */
  const std::string &getStorageClassName() const throw();

  /**
   * Returns the priority of the request.
   *
   * @return The priority of the request.
   */
  uint64_t getPriority() const throw();

private:

  /**
   * The name of the storage class.
   */
  std::string m_storageClassName;

  /**
   * The priority of the request.
   */
  uint64_t m_priority;

}; // class ArchivalRequest

} // namespace cta
