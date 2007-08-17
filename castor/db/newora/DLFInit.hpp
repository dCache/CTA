/******************************************************************************
 *                      DLFInit.hpp
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
 * @(#)$RCSfile: DLFInit.hpp,v $ $Revision: 1.1 $ $Release$ $Date: 2007/08/17 09:09:59 $ $Author: sponcec3 $
 *
 * Initialization of the DLF messages for the ORACLE part
 *
 * @author Sebastien Ponce
 *****************************************************************************/

#ifndef ORA_DLFINIT_HPP 
#define ORA_DLFINIT_HPP 1

namespace castor {

  namespace db {

    namespace ora {

      /**
       * dedicated DLF messages creation
       */
      class DLFInit {
      public:
	/**
	 * The constructor, initializing the messages
	 */
	DLFInit();
      };
    
    }  // namespace ora

  }  // namespace db

} // namespace castor

#endif // ORA_DLFINIT_HPP
