#!/usr/bin/python
#/******************************************************************************
# *                   badmin.py
# *
# * This file is part of the Castor project.
# * See http://castor.web.cern.ch/castor
# *
# * Copyright (C) 2003  CERN
# * This program is free software; you can redistribute it and/or
# * modify it under the terms of the GNU General Public License
# * as published by the Free Software Foundation; either version 2
# * of the License, or (at your option) any later version.
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# * You should have received a copy of the GNU General Public License
# * along with this program; if not, write to the Free Software
# * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# *
# * @(#)$RCSfile: castor_tools.py,v $ $Revision: 1.9 $ $Release$ $Date: 2009/03/23 15:47:41 $ $Author: sponcec3 $
# *
# * command line to reconfigure the scheduling
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

import sys
import getopt
import rpyc
import castor_tools

# usage function
def usage(exitCode):
  print 'Usage : ' + sys.argv[0] + ' [-h|--help]'
  sys.exit(exitCode)

# first parse the options
try:
    options, args = getopt.getopt(sys.argv[1:], 'h', ['help'])
except Exception, e:
    print e
    usage(1)
for f, v in options:
    if f == '-h' or f == '--help':
        usage(0)
    else:
        print "unknown option : " + f
        usage(1)

svcClass = None
if len(args) > 0:
    svcClass = args[0]
    if len(args) > 1:
      print "Too many arguments"
      usage(1)

# connect to server and gather numbers
conf = castor_tools.castorConf()
rpcconn = rpyc.connect(conf['JOBMANAGER']['HOST'], 2681)
counts = rpcconn.root.badmin(svcClass)
print 'reconfiguration done'

