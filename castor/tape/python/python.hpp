/******************************************************************************
 *                      castor/tape/python/python.hpp
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
 * @author Giulia Taurelli, Nicola Bessone and Steven Murray
 *****************************************************************************/

#ifndef CASTOR_TAPE_PYTHON_PYTHON_HPP
#define CASTOR_TAPE_PYTHON_PYTHON_HPP

// Include Python.h before any standard headers because Python.h may define
// some pre-processor definitions which affect the standard headers
#include <Python.h>

#include "castor/exception/Exception.hpp"


namespace castor {
namespace tape   {
namespace python {

/**
 * Initializes the embedded Python interpreter for multi-threaded use and
 * append the CASTOR_POLICIES_DIRECTORY to the PYTHONPATH environment
 * variable.
 *
 * This function should be once and only once for the entire duration of the
 * calling program.
 *
 * When this function returns there will be no lock taken on the global Python
 * interpeter.  All threads, including the main thread must therefore take a
 * lock using a ScopedPythonLock object before acsessing the API of the
 * embedded Python interpreter.
 */
void initializePython() throw(castor::exception::Exception);

/**
 * Finalizes the embedded Python interpreter.
 *
 * Please note that the calling thread must NOT have a lock on the global
 * Python interpreter.
 */
void finalizePython() throw();

/**
 * Imports a CASTOR-policy implemented as a Python module from the
 * Python-module search path which includes the
 * castor::tape::python::CASTOR_POLICIES_DIRECTORY directory.
 *
 * Please note that initPython() must be called before this function is called.
 *
 * Please not that the calling thread MUST have a lock on the global Python
 * interpreter through a call to PyGILState_Ensure() or through the strongly
 * recommended use of a ScopedPythonLock object.
 *
 * @param moduleName The name of the CASTOR-policy Python-module.
 * @return           The Python dictionary object of the imported library.  The
 *                   documentation of the embedded Python-interpreter describes
 *                   the return value as being a "borrowed reference".  This
 *                   means the caller does not need to call Py_XDECREF when the
 *                   dictionary is no longer required.
 */
PyObject* importPolicyPythonModule(const char *const moduleName) 
  throw(castor::exception::Exception);
    
/**
 * Get the Python function object for the specified function within the
 * specified Python dictionary.
 *
 * Please note that initPython() must be called before this function is called.
 *
 * Please not that the calling thread MUST have a lock on the global Python
 * interpreter through a call to PyGILState_Ensure() or through the strongly
 * recommended use of a ScopedPythonLock object.
 *
 * @param pyDict       The Python dictionary in which the specified function is
 *                     to be found.
 * @param functionName The name of the Python function.
 * @return             The Python function object representing the specified
 *                     function or NULL if the named function is not in the
 *                     specified dictionary.  The documentation of the embedded
 *                     Python-interpreter describes the return value as being a
 *                     "borrowed reference".  This means the caller does not
 *                     need to call Py_XDECREF when the function is no longer
 *                     required.
 */
PyObject* getPythonFunction(PyObject *const pyDict,
  const char *const functionName) throw(castor::exception::Exception);
  
} // namespace python
} // namespace tape
} // namespace castor

#endif // CASTOR_TAPE_PYTHON_PYTHON_HPP
