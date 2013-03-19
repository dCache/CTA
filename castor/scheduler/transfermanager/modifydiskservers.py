#/******************************************************************************
# *                   modifydiskservers.py
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
# * ModifyDiskServer is part of the transfer manager of the CASTOR project.
# * This module allows to modify the properties of a set of diskservers
# * (e.g. state or diskPool) from a remote location such as the diskserver itself.
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

'''This module allows to modify the properties of a set of diskservers
(e.g. state or diskPool) from a remote location such as the diskserver itself.'''

import sys
import castor_tools

def modifyDiskServers(dbconn, targets, state, mountPoints, diskPool):
    '''modifies the properties of a set of diskservers'''
    stcur = dbconn.cursor()
    # check target diskpool if any
    if diskPool:
        sqlStatement = "SELECT id, name FROM DiskPool WHERE name = :diskPool"
        stcur.execute(sqlStatement, diskPool=diskPool)
        rows = stcur.fetchall()
        if not rows:
            print 'DiskPool %d does not exist. Giving up' % diskPool
            try:
                castor_tools.disconnectDB(dbconn)
            except Exception:
                pass
            sys.exit(1)
        diskPoolId = rows[0][0]
    # check diskpools
    sqlStatement = 'SELECT id, name FROM DiskPool WHERE name = :dpname'
    diskPools = set([])
    diskPoolIds = []
    for target in targets:
        stcur.execute(sqlStatement, dpname=target)
        row = stcur.fetchone()
        if row:
            diskPoolIds.append(row[0])
            diskPools.add(row[1])
    # check diskservers
    sqlStatement = 'SELECT id, name FROM DiskServer WHERE name = :dsname'
    diskServers = set([])
    diskServerIds = []
    for target in targets:
        stcur.execute(sqlStatement, dsname=target)
        row = stcur.fetchone()
        if row:
            diskServerIds.append(row[0])
            diskServers.add(row[1])
    unknownTargets = set(t for t in targets) - diskPools - diskServers
    if not diskPools and not diskServers:
        returnMsg = 'None of the provided diskpools/diskservers could be found. Giving up'
        return returnMsg
    # go for the update
    if mountPoints:
        # check MountPoints
        sqlStatement = '''SELECT id, mountPoint FROM FileSystem
                           WHERE mountPoint = :mountPoint'''
        if targets:
            sqlStatement += ' AND ('
            if diskPoolIds:
                sqlStatement += 'diskPool IN (' + ', '.join([str(x) for x in diskPoolIds]) + ')'
            if diskServerIds:
                if diskPoolIds:
                    sqlStatement += ' OR '
                sqlStatement += 'diskServer IN (' + ', '.join([str(x) for x in diskServerIds]) + ')'
            sqlStatement += ')'
        mountPointIds = []
        existingMountPoints = set([])
        for mountPoint in mountPoints:
            stcur.execute(sqlStatement, mountPoint=mountPoint)
            row = stcur.fetchone()
            if row:
                mountPointIds.append(row[0])
                existingMountPoints.add(row[1])
        unknownMountPoints = set(m for m in mountPoints) - existingMountPoints
        # update mountPoints
        varDict = {}
        sqlStatement = 'UPDATE FileSystem SET '
        setClause = ''
        whereClause = ' WHERE id IN (' + ', '.join([str(x) for x in mountPointIds]) + ')'
        if state != None:
            setClause = 'status = :status'
            varDict['status'] = state
        if diskPool:
            if setClause:
                setClause += ','
            setClause += ' diskPool = :diskPool'
            varDict['diskPool'] = diskPoolId
        stcur.execute(sqlStatement + setClause + whereClause, varDict)
        # commit update and tell user
        dbconn.commit()
        returnMsg = 'Filesystem(s) modified successfully'
        # mention unknown mountPoints
        if mountPoints and unknownMountPoints:
            returnMsg += '\nWARNING : the following mountPoints could not be found : ' + ', '.join(unknownMountPoints)
    else:
        # check DiskServers
        if diskPools:
            sqlStatement = '''SELECT UNIQUE DiskServer.id FROM DiskServer, FileSystem
                               WHERE DiskServer.id = FileSystem.diskServer
                                 AND FileSystem.diskPool IN (''' + "'" + \
                           "', '".join([str(x) for x in diskPoolIds]) + "')"
            stcur.execute(sqlStatement)
            rows = stcur.fetchall()
            diskServerIds.extend([row[0] for row in rows])
            if not diskServerIds:
                returnMsg = 'No diskserver matching your request'
        # update diskServers for status
        if diskServerIds:
            if state != None:
                varDict = {}
                sqlStatement = '''UPDATE DiskServer SET status = :status
                                   WHERE id IN (''' + ', '.join([str(x) for x in diskServerIds]) + ')'
                varDict['status'] = state
                stcur.execute(sqlStatement, varDict)
            # update fileSystems for diskPool
            if diskPool:
                varDict = {}
                sqlStatement = '''UPDATE FileSystem SET diskPool = :diskPoolId
                                   WHERE diskServer IN (''' + ', '.join([str(x) for x in diskServerIds]) + ')'
                stcur.execute(sqlStatement, diskPoolId=diskPoolId)
            # commit update and tell user
            dbconn.commit()
            returnMsg = 'Diskserver(s) modified successfully'
    # mention unknown targets
    if unknownTargets:
        returnMsg += '\nWARNING : the following diskpools/diskservers do not exist : ' + ', '.join(unknownTargets)
    # return collected messages. If any exception, this is forwarded to the caller
    return returnMsg
