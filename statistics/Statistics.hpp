/*
 * The CERN Tape Archive (CTA) project
 * Copyright (C) 2019  CERN
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
#include <map>
#include "FileStatistics.hpp"
#include "rdbms/Rset.hpp"

namespace cta { namespace statistics {
    
/**
 * This class represents the statistics that we will get and compute from
 * the Catalogue database. It will also be used by the method that will insert the 
 * statistics in the Statistics database
 */    
class Statistics {
public:
  typedef std::map<std::string,FileStatistics> StatisticsPerVo;
  
  Statistics();
  Statistics(const Statistics &other);
  Statistics &operator=(const Statistics &other);
  
  /**
   * Insert statistics per-VO
   * This method will increase the total counters of files and bytes
   * @param vo the VO to which correspond the statistics we insert
   * @param fileStatistics the statistics of the files that belong to the VO
   */
  void insertPerVOStatistics(const std::string &vo, const FileStatistics &fileStatistics);
  
  /**
   * Get the statistics of all VOs
   * @return the StatisticsPerVo map 
   */
  const StatisticsPerVo &getAllVOStatistics() const;
  
  /**
   * Get the total number of MASTER files stored on the tapes
   * @return the total number of MASTER files stored on the tapes
   */
  uint64_t getTotalFiles() const;
  
  /**
   * Get the total amount of space (in bytes) used by the MASTER files stored on the tapes
   * @return the total amount of space (in bytes) used by the MASTER files stored on the tapes 
   */
  uint64_t getTotalBytes() const;
  
  /**
   * Get the total number of MASTER files that are copyNb 1 stored on the tapes
   * @return the total number of MASTER files that are copyNb 1 stored on the tapes
   */
  uint64_t getTotalFilesCopyNb1() const;
  
  /**
   * Get the total amount of space (in bytes) used by MASTER files that are copyNb 1 stored on the tapes
   * @return the total amount of space (in bytes) used by MASTER files that are copyNb 1 stored on the tapes
   */
  uint64_t getTotalBytesCopyNb1() const;
  
  /**
   * Get the total number of MASTER files that are copyNb greater than 1 stored on the tapes
   * @return the total number of MASTER files that are copyNb greater than 1 stored on the tapes
   */
  uint64_t getTotalFilesCopyNbGt1() const;
  
  /**
   * Get the total amount of space (in bytes) used by MASTER files that are copyNb greater than 1 stored on the tapes
   * @return the total amount of space (in bytes) used by MASTER files that are copyNb greater than 1 stored on the tapes
   */
  uint64_t getTotalBytesCopyNbGt1() const;
  
  /**
   * This builder class allows to build the statistics
   */
  class Builder {
  public:
    Builder();
    /**
     * Build the statistics with the Rset returned by the Catalogue database query
     * @param rset the Rset returned by the Catalogue database query
     * @return a unique_ptr that contains the built statistics
     */
    std::unique_ptr<Statistics> build(cta::rdbms::Rset & rset);
  private:
    /**
     * Holds the statististics that will be returned by the build() methods
     */
    std::unique_ptr<Statistics> m_statistics;
  };
  
private:
  /**
   * Currently, it's a map[VO][FileStatistics]
   * used to hold the statistics per-VO
   */
  StatisticsPerVo m_statisticsPerVo;
  
  /**
   * Total number of MASTER files in CTA
   */
  uint64_t m_totalFiles = 0;
  
  /**
   * Total of space used by the MASTER files in CTA
   */
  
  uint64_t m_totalBytes = 0;
  /**
   * Total number of MASTER files that are copyNb 1 in CTA
   */
  uint64_t m_totalFilesCopyNb1 = 0;
  
  /**
   * Total of space used by the MASTER files that are copyNb 1 in CTA
   */
  uint64_t m_totalBytesCopyNb1 = 0;
  
  /**
   * Total number of MASTER files that are copyNb greater than 1 in CTA
   */
  uint64_t m_totalFilesCopyNbGt1 = 0;
  
  /**
   * Total of space used by the MASTER files that are copyNb greater than 1 in CTA
   */
  uint64_t m_totalBytesCopyNbGt1 = 0;
};

/**
 * Overload of the stream << operator
 * The stream will contain the Statistics object in the JSON format
 * @param stream the stream that will contain the Statistics object in JSON
 * @param stats the Statistics object that will be represented in JSON
 * @return the stream containing the Statistics object in JSON
 */
std::ostream &operator <<(std::ostream &stream, Statistics stats);

}}

