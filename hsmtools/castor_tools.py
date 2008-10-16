#/******************************************************************************
# *                   castor_tools.py
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
# * @(#)$RCSfile: castor_tools.py,v $ $Revision: 1.5 $ $Release$ $Date: 2008/10/16 18:29:14 $ $Author: itglp $
# *
# * utility functions for castor tools written in python
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

import os, sys

def checkValueFound(name, value, instance, configFile):
    if len(value) == 0:
        raise ValueError, "No " + name + " found for " + instance + " in " + configFile

def getStagerDBConnectParams():
    # find out the instance to use
    full_name = "DbCnvSvc"
    inst = "default"
    if os.environ.has_key('CASTOR_INSTANCE'):
        inst = os.environ['CASTOR_INSTANCE']
        full_name = full_name + '_' + inst
        inst = "'" + inst + "' stager"
    # go through the lines of ORASTAGERCONFIG
    user = ""
    passwd = ""
    dbname = ""
    for l in open ('/etc/castor/ORASTAGERCONFIG').readlines():
        try:
            instance, entry, value = l.split()
            if instance == full_name:
                if entry == 'user':
                    user = value
                elif entry == 'passwd':
                    passwd = value
                elif entry == 'dbName':
                    dbname = value
        except ValueError:
            # ignore line
            pass
    checkValueFound("user name", user, inst, 'ORASTAGERCONFIG')
    checkValueFound("password", passwd, inst, 'ORASTAGERCONFIG')
    checkValueFound("DB name", dbname, inst, 'ORASTAGERCONFIG')
    return user, passwd, dbname

def getNSDBConnectParam():
    line = open('/etc/castor/NSCONFIG').readline()
    line = line[0:len(line)-1] #drop trailing \n
    sl = line.find('/')
    if sl == -1:
        raise ValueError, 'Invalid connection string in /etc/castor/NSCONFIG'
    ar = line.find('@',sl)
    if ar == -1:
        raise ValueError, 'Invalid connection string in /etc/castor/NSCONFIG'
    user = line[0:sl]
    passwd = line[sl+1:ar]
    dbname = line[ar+1:]
    checkValueFound("user name", user, 'nameserver', 'NSCONFIG')
    checkValueFound("password", passwd, 'nameserver', 'NSCONFIG')
    checkValueFound("DB name", dbname, 'nameserver', 'NSCONFIG')
    return user, passwd, dbname

def importOracle():
    global cx_Oracle
    try:
        import cx_Oracle
    except Exception:
        raise Exception, '''Fatal: could not load module cx_Oracle.
Make sure it is installed and $PYTHONPATH includes the directory where cx_Oracle.so resides and that
your ORACLE environment is setup properly.'''

def connectToDB(user, passwd, dbname):
    importOracle()
    return cx_Oracle.Connection(user + '/' + passwd + '@' + dbname)

def connectToStager():
    user, passwd, dbname = getStagerDBConnectParams()
    return connectToDB(user, passwd, dbname)

def connectToNS():
    user, passwd, dbname = getNSDBConnectParam()
    return connectToDB(user, passwd, dbname)

def disconnectDB(connection):
  connection.close()

DiskCopyStatus = ["DISKCOPY_STAGED",
                  "DISKCOPY_WAITDISK2DISKCOPY",
                  "DISKCOPY_WAITTAPERECALL",
                  "DISKCOPY_DELETED",
                  "DISKCOPY_FAILED",
                  "DISKCOPY_WAITFS",
                  "DISKCOPY_STAGEOUT",
                  "DISKCOPY_INVALID",
                  "DISKCOPY_GCCANDIDATE",
                  "DISKCOPY_BEINGDELETED",
                  "DISKCOPY_CANBEMIGR",
                  "DISKCOPY_WAITFS_SCHEDULING"]

class castorObject(dict):
  '''a base object for CASTOR items.
  This includes clever printing and case insensitive member access'''
  def __init__(self, name):
    self.name = name
  def __str__(self):
    res = '[# ' + self.name + ' #]\n'
    for s in self.keys():
      if isinstance(self[s], type([])):
        res = res + s.lower() + " :\n"
        i = 0
        for t in self[s]:
          res = res + "  " + str(i) + " :\n"
          for l in str(t).split('\n'):
            if len(l) > 0:
              res = res + '    ' + l + '\n'
          i = i + 1
      else:
        res = res + s.lower() + " : " + str(self[s]) + "\n"
    return res
  def __getattr__(self, name):
    return self[name.upper()]

def getObject(stcur, table, key, value):
  '''Gets an object from the DB'''
  stmt = 'SELECT * FROM ' + table + ' WHERE ' + key + '='
  if type(value) == type(''): stmt = stmt + "'"
  stmt = stmt + value
  if type(value) == type(''): stmt = stmt + "'"  
  stcur.execute(stmt)
  rawobj = stcur.fetchone()
  if None == rawobj:
    raise ValueError, "Found no " + table + " with " + key + " : '" + value + "'"
  obj = castorObject(table)
  i = 0
  for d in stcur.description:
    obj[d[0]] = rawobj[i]
    i = i + 1
  return obj

def fillObjectgeneric(stcur, obj, entry, table, stmt):
  '''Fill an object with the children given by a DB stmt'''
  stcur.execute(stmt)
  rawobjs = stcur.fetchall()
  obj[entry] = []
  for r in rawobjs:
    child = castorObject(table)
    i = 0
    for d in stcur.description:
      child[d[0]] = r[i]
      i = i + 1
    obj[entry].append(child)

def fillObject12n(stcur, obj, entry, table, key):
  '''Fill an object following a 1->n relation'''
  stmt = 'SELECT * FROM ' + table + ' WHERE ' + key + '=' + str(obj.id)
  fillObjectgeneric(stcur, obj, entry, table, stmt)

def fillObjectn21(stcur, obj, entry, table):
  '''Fill an object following a n->1 relation'''
  stmt = 'SELECT * FROM ' + table + ' WHERE id =' + str(obj[entry.upper()])
  fillObjectgeneric(stcur, obj, entry.upper(), table, stmt)

def fillObjectn2n(stcur, obj, entry, table):
  '''Fill an object following a n->n relation'''
  if obj.name < table:
    jointable = obj.name + '2' + table
    key1 = 'child'
    key2 = 'parent'
  else:
    jointable = table + '2' + obj.name
    key1 = 'parent'
    key2 = 'child'
  stmt = 'SELECT ' + table + '.* FROM ' + table + ',' + jointable + ' WHERE ' + jointable + '.' + key1 + '=' + table + '.id AND ' + jointable + '.' + key2 + '=' + str(obj.id)
  fillObjectgeneric(stcur, obj, entry, table, stmt)

def getSvcClass(svcClassName):
    '''Gets the content of a given service class'''
    try:
        stconn = connectToStager()
        stcur = stconn.cursor()
        stcur.arraysize = 50
        svcClass = getObject(stcur, 'SvcClass', 'name', svcClassName)
        fillObjectn2n(stcur, svcClass, 'DiskPools', 'DiskPool')
        fillObjectn2n(stcur, svcClass, 'TapePools', 'TapePool')
        fillObjectn21(stcur, svcClass, 'forcedFileClass', 'FileClass');
        disconnectDB(stconn)
        return svcClass
    except Exception, e:
        print e
        sys.exit(-1)

def getFileClass(fileClassName):
    '''Gets the content of a given file class'''
    try:
        stconn = connectToStager()
        stcur = stconn.cursor()
        fileClass = getObject(stcur, 'FileClass', 'name', fileClassName)
        disconnectDB(stconn)
        return fileClass
    except Exception, e:
        print e
        sys.exit(-1)

