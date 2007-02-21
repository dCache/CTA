/**** This file has been autogenerated by gencastor from Umbrello UML model ***/

/******************************************************************************
 *                      castor/stager/QryRequest.hpp
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
 * @(#)$RCSfile$ $Revision$ $Release$ $Date$ $Author$
 *
 * 
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#ifndef CASTOR_STAGER_QRYREQUEST_HPP
#define CASTOR_STAGER_QRYREQUEST_HPP

// Include Files
#include "castor/stager/Request.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace castor {

  // Forward declarations
  class ObjectSet;

  namespace stager {

    // Forward declarations
    class QueryParameter;

    /**
     * class QryRequest
     * An abstract ancester for all query requests
     */
    class QryRequest : public virtual Request {

    public:

      /**
       * Empty Constructor
       */
      QryRequest() throw();

      /**
       * Empty Destructor
       */
      virtual ~QryRequest() throw();

      /**
       * Outputs this object in a human readable format
       * @param stream The stream where to print this object
       * @param indent The indentation to use
       * @param alreadyPrinted The set of objects already printed.
       * This is to avoid looping when printing circular dependencies
       */
      virtual void print(std::ostream& stream,
                         std::string indent,
                         castor::ObjectSet& alreadyPrinted) const;

      /**
       * Outputs this object in a human readable format
       */
      virtual void print() const;

      /**
       * Add a QueryParameter* object to the m_parametersVector list
       */
      void addParameters(QueryParameter* add_object) {
        m_parametersVector.push_back(add_object);
      }

      /**
       * Remove a QueryParameter* object from m_parametersVector
       */
      void removeParameters(QueryParameter* remove_object) {
        for (unsigned int i = 0; i < m_parametersVector.size(); i++) {
          QueryParameter* item = m_parametersVector[i];
          if (item == remove_object) {
            std::vector<QueryParameter*>::iterator it = m_parametersVector.begin() + i;
            m_parametersVector.erase(it);
            return;
          }
        }
      }

      /**
       * Get the list of QueryParameter* objects held by m_parametersVector
       * @return list of QueryParameter* objects held by m_parametersVector
       */
      std::vector<QueryParameter*>& parameters() {
        return m_parametersVector;
      }

    private:

      std::vector<QueryParameter*> m_parametersVector;

    }; /* end of class QryRequest */

  } /* end of namespace stager */

} /* end of namespace castor */

#endif // CASTOR_STAGER_QRYREQUEST_HPP
