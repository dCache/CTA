/******************************************************************************
 *                      Block.cpp
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
 * @(#)$RCSfile: Block.hpp,v $ $Revision: 1.16 $ $Release$ $Date: 2007/04/12 12:00:15 $ $Author: sponcec3 $
 *
 * A block of shared memory with incorporated memory allocation
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef SHAREDMEMORY_BLOCK_HPP
#define SHAREDMEMORY_BLOCK_HPP 1

#include <map>
#include <sys/types.h>
#include "castor/sharedMemory/BasicBlock.hpp"
#include "castor/sharedMemory/BlockKey.hpp"
#include "castor/exception/Exception.hpp"

namespace castor {

  namespace sharedMemory {

    // Convenience typedef
    typedef std::pair<void* const, size_t> SharedNode;

    /**
     * a class dealing with a Block of Shared memory.
     * The raw Block contains a sharedMemory::IBlock object
     * to manage itself and its own table of free and used areas
     * so that it's self contained.
     * The mapping of the memory of the Block is the following :
     * \verbatim
     *   Begin of Block
     *     Block object (this class)
     *     Extra reserved memory (for use by child classes)
     *     First and Second node of the Allocation Table
     *     Allocator of the allocation table
     *     Allocation Table
     *     Available memory
     *   End of Block
     * \endverbatim
     * Note that the 2 first items are not seen by this class.
     * Hence the pointer to the raw memory handled here points
     * to the "First and Second node of the Allocation Table"
     * @param A an allocator name that should be used for internal
     * memory allocation. Node that it must allocate
     * Block::SharedNode objects
     */
    template <typename A>
    class Block : public BasicBlock {

    public:

      /**
       * Constructor
       * @param key the key for this block
       * @param rawMem pointer to the raw memory to be used
       * objects
       */
      Block(BlockKey& key, void* rawMem)
        throw (castor::exception::Exception);

      /**
       * Destructor
       */
      ~Block() throw ();

      /**
       * allocates a new chunk of memory
       * @param obj the Block object to use (to be casted into Block*)
       * @param nbBytes the number of bytes needed
       */
      static void* malloc(BasicBlock* obj, size_t nbBytes)
        throw (castor::exception::Exception);

      /**
       * deallocate some memory
       * @param obj the Block object to use (to be casted into Block*)
       * @param pointer a pointer to the space to deallocate
       * @param nbBytes the number of bytes freed
       */
      static void free(BasicBlock* obj, void* pointer, size_t nbBytes)
        throw (castor::exception::Exception);

      /**
       * Outputs this object in a human readable format
       * @param stream The stream where to print this object
       * @param indent The indentation to use
       */
      void print(std::ostream& stream,
		 std::string indent) const throw();

    private:

      // Convenience typedef
      typedef typename std::map<void*,
                                size_t,
                                std::less<void*>,
                                A> SharedMap;

      /**
       * Are we initializing the Block ? And if yes (non 0) at which stage
       * of the initialization ? This is actually used for the management
       * of the m_freeRegions initialization.
       */
      int m_initializing;

      /**
       * map of free regions.
       */
      SharedMap* m_freeRegions;

    }; // end class Block

  } // namespace sharedMemory

} // namespace castor


////////////////////////////////////////////////////////////////////////////////
// Implementation of templated parts
////////////////////////////////////////////////////////////////////////////////

#include "castor/exception/OutOfMemory.hpp"
#include "castor/sharedMemory/BlockDict.hpp"

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
template <typename A>
castor::sharedMemory::Block<A>::Block
(BlockKey& key, void* rawMem)
  throw (castor::exception::Exception) :
  BasicBlock(key, rawMem), m_initializing(0) {
  // Register block in the dictionnary before it's fully
  // created since the SharedMap creation will use it
  castor::sharedMemory::BlockDict::insertBlock
    (key, this, this->malloc, this->free);
  // start of memory initialization
  m_initializing = 1;
  // now create the map of free regions (and its allocator)
  size_t offset = 2*sizeof(std::_Rb_tree_node<SharedNode>);
  void* allocatorRaw = (void*)((char*)m_sharedMemoryBlock + offset);
  offset += sizeof(A);
  void* freeRegionsRaw = (void*)((char*)m_sharedMemoryBlock + offset);
  // create the free region map and its allocator
  A* alloc = new(allocatorRaw) A();
  m_freeRegions = new(freeRegionsRaw) SharedMap(std::less<void*>(), *alloc);
  offset += sizeof(SharedMap);
  // note that malloc will be called here since a new cell
  // has to be created in the m_freeRegions container
  // However, since m_initializing is true, it won't allocate memory
  // but used the reserved node
  (*m_freeRegions)[(void*)((char*)m_sharedMemoryBlock + offset)] =
    m_key.size() - offset;
  // End of memory initialization
  m_initializing = 0;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
template <typename A>
castor::sharedMemory::Block<A>::~Block () throw () {}

//------------------------------------------------------------------------------
// malloc
//------------------------------------------------------------------------------
template <class A>
void* castor::sharedMemory::Block<A>::malloc
(BasicBlock* rawObj, size_t nbBytes)
  throw (castor::exception::Exception) {
  // This can only be done because no virtual inheritance
  // is implied and because there is no virtual table
  Block* obj = (Block*) rawObj;
  if (obj->m_initializing) {
    // Very special case where we are "recursively" called
    // for the creation of the first node of the map Btree.
    // We return straight the space dedicated to it, which
    // is located at the very beginning of the block
    void* res = (void*) (((char*)obj->m_sharedMemoryBlock) +
                         ((obj->m_initializing-1) *
			  sizeof(std::_Rb_tree_node<SharedNode>)));
    obj->m_initializing++;
    return res;
  }
  // loop over free regions to find a suitable one
  for (typename SharedMap::iterator it =
         obj->m_freeRegions->begin();
       it != obj->m_freeRegions->end();
       it++) {
    // Take the first one large enough
    if (it->second >= nbBytes) {
      // read everything before we screw the iterator
      void* pointer = it->first;
      // remove from the free regions
      if (it->second == nbBytes) {
        // the perfect match
        obj->m_freeRegions->erase(it);
	//std::cout << "allocated " << std::hex << nbBytes << " bytes at "
	//	  << std::hex << pointer << ". Free region dropped\n";
      } else {
        // Let's reduce the region
        it->second -= nbBytes;
        pointer = (void*)((char*)pointer + it->second);
	//std::cout << "allocated " << std::hex << nbBytes << " bytes at "
	//	  << std::hex << pointer << "\n";
      }
      return pointer;
    }
  }
  // none found, i.e. out of memory
  castor::exception::OutOfMemory e;
  e.getMessage() << "Out of Shared memory. Cannot allocate "
                 << nbBytes << " bytes";
  throw e;
}

//------------------------------------------------------------------------------
// free
//------------------------------------------------------------------------------
template <class A>
void castor::sharedMemory::Block<A>::free
(BasicBlock* rawObj, void* pointer, size_t nbBytes)
  throw (castor::exception::Exception) {
  // This can only be done because no virtual inheritance
  // is implied and because there is no virtual table
  Block* obj = (Block*) rawObj;
  // Update free space
  typename SharedMap::iterator it = obj->m_freeRegions->begin();
  // First find the surrounding free regions (if any)
  std::pair<void*, size_t> previousRegion(0, 0);
  it = obj->m_freeRegions->begin();
  while ((it != obj->m_freeRegions->end()) && // end of iteration
         (it->first <= pointer)) { // found the right one
    previousRegion = *it;
    it++;
  }
  //std::cout << "deallocating " << std::hex << nbBytes << " bytes at "
  //          << std::hex << pointer << "\n";
  // Merge with the previous one ?
  bool merged = false;
  size_t size = nbBytes;
  if (previousRegion.first > 0 &&
      ((void*)((char*)previousRegion.first +
               previousRegion.second)) == pointer) {
    pointer = previousRegion.first;
    size += previousRegion.second;
    (*obj->m_freeRegions)[pointer] = size;
    merged = true;
    //std::cout << "  Merged with region " << std::hex << pointer
    //	        << " now of size " << std::hex << size << " bytes\n";
  }
  // Merge with the next one ?
  if (it != obj->m_freeRegions->end() &&
      it->first == (void*)((char*)pointer + size)) {
    //std::cout << "  Also merged with region " << std::hex << it->first
    //	        << " of size " << std::hex << it->second
    //          << ". New region now of size " << std::hex
    //          << (size + it->second) << " bytes\n";
    (*obj->m_freeRegions)[pointer] = size + it->second;
    obj->m_freeRegions->erase(it);
    merged = true;
  }
  if (!merged) {
    // No merge, create a new free Region
    // Note that this will trigger an allocation of memory
    // for the cell of the new free region within the
    // obj->m_freeRegions container. Thus malloc will be called
    // at some point inside free !
    (*obj->m_freeRegions)[pointer] = size;
    //std::cout << "  New free region created\n";
  }
}

//------------------------------------------------------------------------------
// print
//------------------------------------------------------------------------------
template <class A>
void castor::sharedMemory::Block<A>::print
(std::ostream& stream, std::string indent) const throw() {
  castor::sharedMemory::BasicBlock::print(stream, indent);
  stream << indent << "[# Block #]" << "\n"
	 << indent << "  m_initializing = " << m_initializing << "\n"
	 << indent << "  m_freeRegions = " << std::hex
	 << m_freeRegions << " (" << m_freeRegions->size() << " items) :\n";
  for (typename SharedMap::iterator it =
         m_freeRegions->begin();
       it != m_freeRegions->end();
       it++) {
    stream << indent << "    "
	   << std::hex << "0x" << it->second << " bytes @ "
	   << std::hex << "0x" << it->first << "\n";
  }
  stream << std::flush;
}

#endif // SHAREDMEMORY_BLOCK_HPP
