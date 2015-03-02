#pragma once

#include "SecurityIdentity.hpp"
#include "TapePool.hpp"

#include <list>
#include <map>
#include <string>

namespace cta {

/**
 * Mock database-table of tape pools.
 */
class MockTapePoolTable {
public:

  /**
   * Creates a tape pool with the specifed name.
   *
   * @param requester The identity of the user requesting the creation of the
   * tape pool.
   * @param name The name of the tape pool.
   * @param nbDrives The maximum number of drives that can be concurrently
   * assigned to this pool independent of whether they are archiving or
   * retrieving files.
   * @param nbPartialTapes The maximum number of tapes that can be partially
   * full at any moment in time.
   * @param comment The comment describing the tape pool.
   */
  void createTapePool(
    const SecurityIdentity &requester,
    const std::string &name,
    const uint16_t nbDrives,
    const uint32_t nbPartialTapes,
    const std::string &comment);

  /**
   * Throws an exception if the specified tape pool already exists.
   *
   * @param name The name of the tape pool.
   */
  void checkTapePoolDoesNotAlreadyExist(const std::string &name) const;

  /**
   * Throws an exception if the specified tape pool does not exist.
   *
   * @param name The name of the tape pool.
   */
  void checkTapePoolExists(const std::string &name) const;

  /**
   * Delete the tape pool with the specifed name.
   *
   * @param requester The identity of the user requesting the deletion of the
   * tape pool.
   * @param name The name of the tape pool.
   */
  void deleteTapePool(
    const SecurityIdentity &requester,
    const std::string &name);

  /**
   * Gets the current list of tape pools in lexicographical order.
   *
   * @param requester The identity of the user requesting the list.
   * @return The current list of tape pools in lexicographical order.
   */
  std::list<TapePool> getTapePools(
    const SecurityIdentity &requester) const;

  /**
   * Returns the specified tape pool.
   *
   * @param name The name of the tape pool.
   * @return The specified tape pool.
   */
  const TapePool &getTapePool(const std::string &name) const;

private:

  /**
   * Mapping from tape pool name to tape pool.
   */
  std::map<std::string, TapePool> m_tapePools;

}; // class MockTapePoolTable

} // namespace cta
