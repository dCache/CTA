#!/usr/bin/python
#/******************************************************************************
# *                      fixFileSize
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
# * @(#)$RCSfile: fixFileSize.py,v $ $Revision: 1.2 $ $Release$ $Date: 2008/10/07 14:38:28 $ $Author: sponcec3 $
# *
# * fixes the size of a file both in the nameserver and in the stager
# * This tool overwrites any existing size, including the one of all existing
# * segments. To be used with care !
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

import sys
import getopt
import castor_tools

# usage function
def usage(exitCode):
  print 'Usage : ' + sys.argv[0] + ' [-h|--help] fileId:size [...]'
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

# If no arg, display usage
if len(args) == 0:
    print "missing argument\n"
    usage(1)

# build the list of fileids
fileids = []
for arg in args:
  try:
      fid, fs = arg.split(':')
      fileids.append((int(fid), int(fs)))
  except ValueError:
      print 'Invalid argument : "' + arg + '"'
      usage(1)

# logging
ans = raw_input(str(len(fileids)) + " files to fix. Shall I proceed [Yn] ? ")
if len(ans) > 0 and ans.lower() != 'y':
    print "Ok, giving up"
    sys.exit(0)

# connect to the nameserver and fix files
try:
    nsconn = castor_tools.connectToNS()
    nscur = nsconn.cursor()
    nscur.arraysize = 50
    nsfixSQL = '''DECLARE
      unused NUMBER;
    BEGIN
      SELECT parent_fileid INTO unused
        FROM cns_file_metadata
       WHERE fileid = 
         (SELECT parent_fileid FROM cns_file_metadata
           WHERE fileid = :fid
             AND bitand(filemode, 32768) = 32768) FOR UPDATE;
      UPDATE cns_file_metadata SET fileSize = :fs WHERE fileid = :fid;
      UPDATE cns_seg_metadata SET segsize = :fs WHERE s_fileid = :fid;
    END;'''
    print "Fixing nameserver..."
    for f in fileids:
        nscur.execute(nsfixSQL, fid=f[0], fs=f[1])
    nsconn.commit()
    castor_tools.disconnectDB(nsconn)
    print "Done"
except Exception, e:
    print e
    sys.exit(-1)

# connect to the stager and fix files
try:
    stconn = castor_tools.connectToStager()
    stcur = stconn.cursor()
    stcur.arraysize = 50
    stfixSQL = 'UPDATE Castorfile SET fileSize = :fs WHERE fileid = :fid'
    print "Fixing stager..."
    for f in fileids:
        stcur.execute(stfixSQL, fid=f[0], fs=f[1])
    stconn.commit()
    castor_tools.disconnectDB(stconn)
    print "Done"
except Exception, e:
    print e
    sys.exit(-1)

