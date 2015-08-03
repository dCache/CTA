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
  private:

    /**
     * Constructor.
     *
     * @param dbMount The database representation of this mount.
     */
    RetrieveMount(std::unique_ptr<cta::SchedulerDatabase::TapeMount> dbMount);

  public:

    CTA_GENERATE_EXCEPTION_CLASS(WrongMountType);
    CTA_GENERATE_EXCEPTION_CLASS(NotImplemented);

    /**
     * Returns The type of this tape mount.
     *
     * @return The type of this tape mount.
     */
    virtual MountType::Enum getMountType() const throw();

    /**
     * Returns the volume identifier of the tape to be mounted.
     *
     * @return The volume identifier of the tape to be mounted.
     */
    virtual std::string getVid() const throw();

    /**
     * Notifies the scheduler that the session is finished 
     */
    virtual void finish();
    
    /**
     * Job factory
     *
     * @return A unique_ptr to the next archive job or NULL if there are no more
     * archive jobs left for this tape mount.
     */
    std::unique_ptr<RetrieveJob> getNextJob();
    
    /**
     * Destructor.
     */
    virtual ~RetrieveMount() throw();

  private:

    /**
     * The database representation of this mount.
     */
    std::unique_ptr<cta::SchedulerDatabase::RetrieveMount> m_dbMount;

  }; // class RetrieveMount

} // namespace cta
