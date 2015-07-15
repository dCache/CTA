/******************************************************************************
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#pragma once

#include "castor/server/BlockingQueue.hpp"
#include "castor/server/Threading.hpp"
#include "castor/log/LogContext.hpp"

namespace castor {
namespace exception {
// Forward declaration  
class Exception;    
}
namespace tape {
namespace tapeserver {
namespace daemon {
// Forward declaration  
class MemBlock;
/**
 * The memory manager is responsible for allocating memory blocks and distributing
 * the free ones around to any class in need.
 */
class RecallMemoryManager {
public:
  
  /**
   * Constructor
   * @param numberOfBlocks: number of blocks to allocate
   * @param blockSize: size of each block
   */
  RecallMemoryManager(const size_t numberOfBlocks, const size_t blockSize,
          castor::log::LogContext& lc);
  
  /**
   * Are all sheep back to the farm?
   * @return 
   */
  bool areBlocksAllBack() throw();
  
  /**
   * Takes back a block which has been released by one of the clients
   * @param mb: the pointer to the block
   */
  void releaseBlock(MemBlock *mb);
  
  /**
   * Pop a free block from the free block queue of the memory manager
   * @return pointer to a free block
   */
  MemBlock* getFreeBlock();
  
  /**
   * Destructor
   */
  ~RecallMemoryManager();
  
private:
  /**
   * Total number of allocated memory blocks
   */
  size_t m_totalNumberOfBlocks;
  
  /**
   * Container for the free blocks
   */
  castor::server::BlockingQueue<MemBlock*> m_freeBlocks;
  
  /**
   * Logging. The class is not threaded, so it can be shared with its parent
   */
  castor::log::LogContext& m_lc;
};

}}}}

