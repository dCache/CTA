/******************************************************************************
 *                      DiskWriteFileTask.hpp
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

#include "castor/tape/tapeserver/daemon/DiskWriteTaskInterface.hpp"
#include "castor/tape/tapeserver/daemon/DataFifo.hpp"
#include "castor/tape/tapeserver/daemon/RecallMemoryManager.hpp"
#include "castor/tape/tapeserver/daemon/DataConsumer.hpp"
#include "castor/tape/tapeserver/file/File.hpp"
#include "castor/tape/tapegateway/FileToRecallStruct.hpp"
#include "castor/tape/tapeserver/daemon/ReportPackerInterface.hpp"

#include <memory>
namespace {
  
  uint32_t blockID(const castor::tape::tapegateway::FileToRecallStruct& ftr)
  {
    return (ftr.blockId0() << 24) | (ftr.blockId1() << 16) |  (ftr.blockId2() << 8) | ftr.blockId3();
  }
 
}
namespace castor {
namespace tape {
namespace tapeserver {
namespace daemon {
  
/**
 * The DiskWriteFileTask is responsible to write a single file onto disk as part of a recall
 * session. Being a consumer of memory blocks, it inherits from the DataConsumer class. It also
 * inherits several methods from the DiskWriteTask (TODO: do we really need this base class?).
 */
class DiskWriteTask: public DiskWriteTaskInterface, public DataConsumer {
public:
  /**
   * Constructor
   * @param file: All we need to know about the file we  are recalling
   * @param mm: memory manager of the session
   */
  DiskWriteTask(tape::tapegateway::FileToRecallStruct* file,RecallMemoryManager& mm);
  /**
   * Main routine: takes each memory block in the fifo and writes it to disk
   * @return true if the file has been successfully written false otherwise.
   */
  virtual bool execute(ReportPackerInterface<detail::Recall>& reporter,log::LogContext& lc) ;
  
  /**
   * Allows client code to return a reusable memory block. Should not been called
   * @return the pointer to the memory block that can be reused
   */
  virtual MemBlock *getFreeBlock() ;
  
  /**
   * Function used to enqueue a new memory block holding data to be written to disk
   * @param mb: corresponding memory block
   */
  virtual void pushDataBlock(MemBlock *mb);

  /**
   * Destructor (also waiting for the end of the write operation)
   */
  virtual ~DiskWriteTask();
  
private:

  void releaseAllBlock();
  /**
   * The fifo containing the memory blocks holding data to be written to disk
   */
  castor::tape::threading::BlockingQueue<MemBlock *> m_fifo;
  /** 
   * All we need to know about the file we are currently recalling
   */
  std::auto_ptr<tape::tapegateway::FileToRecallStruct> m_recallingFile;
    
  /**
   * Reference to the Memory Manager in use
   */
  RecallMemoryManager & m_memManager;
  
  /**
   * Mutex forcing serial access to the fifo
   */
  castor::tape::threading::Mutex m_producerProtection;
  
};

}}}}
