#pragma once

#include "cta/ConfigurationItem.hpp"

#include <string>

namespace cta {

/**
 * Class representing a tape pool.
 */
class TapePool: public ConfigurationItem {
public:

  /**
   * Constructor.
   */
  TapePool();

  /**
   * Destructor.
   */
  ~TapePool() throw();

  /**
   * Constructor.
   *
   * @param name The name of the tape pool.
   * @param nbPartialTapes The maximum number of tapes that can be partially
   * full at any moment in time.
   * @param creator The identity of the user that created this configuration
   * item.
   * @param comment The comment describing this configuration item.
   * @param creationTime Optionally the absolute time at which this
   * configuration item was created.  If no value is given then the current
   * time is used.
   */
  TapePool(
    const std::string &name,
    const uint32_t nbPartialTapes,
    const UserIdentity &creator,
    const std::string &comment,
    const time_t creationTime = time(NULL));

  /**
   * Less than operator.
   *
   * @param rhs The right-hand side of the operator.
   */
  bool operator<(const TapePool &rhs) const throw();

  /**
   * Returns the name of the tape pool.
   *
   * @return The name of the tape pool.
   */
  const std::string &getName() const throw();

  /**
   * Returns the maximum number of tapes that can be partially full at any
   * moment in time.
   *
   * @return The maximum number of tapes that can be partially full at any
   * moment in time.
   */
  uint32_t getNbPartialTapes() const throw();

private:

  /**
   * The name of the tape pool.
   */
  std::string m_name;

  /**
   * The maximum number of tapes that can be partially full at any moment in
   * time.
   */
  uint32_t m_nbPartialTapes;

}; // class TapePool

} // namespace cta
