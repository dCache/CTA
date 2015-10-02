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

#include "common/exception/Exception.hpp"
#include "scheduler/RetrieveJob.hpp"
#include "scheduler/RetrieveMount.hpp"
#include "scheduler/SchedulerDatabase.hpp"
#include "scheduler/TapeMount.hpp"

#include <memory>

namespace cta {
  
  /**
   * The class driving a retrieve mount.
   * The class only has private constructors as it is instanciated by
   * the Scheduler class.
   */
  class RetrieveMount: public TapeMount {
    friend class Scheduler;
  protected:
    /**
     * Trivial constructor
     */
    RetrieveMount();
    
    /**
     * Constructor.
     *
     * @param dbMount The database representation of this mount.
     */
    RetrieveMount(std::unique_ptr<cta::SchedulerDatabase::RetrieveMount> dbMount);

  public:

    CTA_GENERATE_EXCEPTION_CLASS(WrongMountType);
    CTA_GENERATE_EXCEPTION_CLASS(NotImplemented);

    /**
     * Returns The type of this tape mount.
     *
     * @return The type of this tape mount.
     */
    virtual MountType::Enum getMountType() const;

    /**
     * Returns the volume identifier of the tape to be mounted.
     *
     * @return The volume identifier of the tape to be mounted.
     */
    virtual std::string getVid() const;
    
    /**
     * Returns the density of the tape to be mounted.
     *
     * @return The density of the tape to be mounted.
     */
    virtual std::string getDensity() const;
    
    /**
     * Returns the mount transaction id.
     *
     * @return The mount transaction id.
     */
    virtual std::string getMountTransactionId() const;

    /**
     * Indicates that the mount was completed.
     *
     * @param checksumOfTransfer The adler-32 checksum of the file as calculated
     * during the execution of the job.
     * @param fileSizeOfTransfer The size of the file as calculated during the
     * execution of the job.
     */
    virtual void complete();

    CTA_GENERATE_EXCEPTION_CLASS(SessionNotRunning);
    /**
     * Job factory
     *
     * @return A unique_ptr to the next archive job or NULL if there are no more
     * archive jobs left for this tape mount.
     */
    virtual std::unique_ptr<RetrieveJob> getNextJob();
    
    /**
     * Destructor.
     */
    virtual ~RetrieveMount() throw();

  private:

    /**
     * The database representation of this mount.
     */
    std::unique_ptr<cta::SchedulerDatabase::RetrieveMount> m_dbMount;
    
    /**
     * Internal tracking of the session completion
     */
    bool m_sessionRunning;

  }; // class RetrieveMount

} // namespace cta
