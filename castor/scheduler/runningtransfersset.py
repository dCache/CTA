#/******************************************************************************
# *                   runningtransferset.py
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
# * runningtransferset class of the transfer manager of the CASTOR project
# * this class is responsible for managing a local set of running transfers and check for their
# * termination when called on the poll method
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

import os, pwd
import threading
import subprocess
import time
import dlf
from diskmanagerdlf import msgs
import castor_tools

class RunningTransfersSet:
  '''handles a list of running transfers and is able to poll them regularly and list the ones that ended'''

  def __init__(self, connections, fake=False):
    '''constructor'''
    # connection pool to be used for sending messages to schedulers
    self.connections = connections
    # do we run in fake mode ?
    self.fake = fake
    # standard transfers and disk2disk copy destinations
    self._transfers = set()
    # global lock for the 2 sets
    self._lock = threading.Lock()
    # get configuration
    self.config = castor_tools.castorConf()
    # list transfers already running on the node, left over from the last time we ran
    self.leftOverTransfers = self.populate()

  def populate(self):
    '''populates the list of ongoing transfers from the system.
    note that this is linux specific code'''
    # 'populating running scripts from system' message
    dlf.writedebug(msgs.POPULATING)
    # loop over all processes
    pids = [pid for pid in os.listdir('/proc') if pid.isdigit()]
    leftOvers = {}
    for pid in pids:
      cmdline = open(os.path.sep+os.path.join('proc', pid, 'cmdline'), 'rb').read()
      # find out the ones concerning us
      if cmdline.startswith('/usr/bin/stagerjob') or cmdline.startswith('/usr/bin/d2dtransfer'):
          # get all details we need
          args = cmdline.split('\0')
          transferid = args[4]
          notifFileName = args[-1][7:] # drop the leading 'file://'
          transfertype = 'standard'
          if args[0] == '/usr/bin/d2dtransfer': transfertype = 'd2ddest'
          try:
            arrivalTime = os.stat(os.path.sep+os.path.join('proc', pid, 'cmdline'))[-2]
            startTime = arrivalTime
            # create the entry in the list of running transfers, associated to the first scheduler we find
            self._transfers.add((transferid, self.config['DiskManager']['ServerHosts'].split()[0], tuple(args), notifFileName, None, transfertype, arrivalTime, startTime))
            # keep in memory that this was a rebuilt entry
            leftOvers[transferid] = int(pid)
            # 'Found transfer already running' message
            fileid = (args[8],int(args[6]))
            dlf.write(msgs.FOUNDTRANSFERALREADYRUNNING, subreqid=transferid, fileid=fileid)
          except Exception:
            # could not stat the proc file, it means that this process has ended in the meantime, so ignore it
            pass
    return leftOvers

  def add(self, transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, startTime=None):
    '''add a new running transfer to the list'''
    self._lock.acquire()
    try:
      if startTime == None:
        startTime = time.time()
      self._transfers.add((transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, startTime))
    finally:
      self._lock.release()

  def nbTransfers(self, reqUser=None, detailed=False):
    '''returns number of running transfers and number of running slots, plus details
    per protocol if the detailed paremeter is true.
    The exact format of the returned tuple if detailed is False is :
     (nbRunningTransfers, nbRunningSlots)
    If detailed is True, then it is :
     (nbRunningTransfers, (('proto1', nbRunningTransfersForProto1), ...),
      nbRunningSlots, (('proto1', nbRunningSlotsForProto1), ...)) '''
    n = 0
    nproto = {}
    ns = 0
    nsproto = {}
    for transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, runTime in self._transfers:
      if transfertype == 'd2dsource':
        protocol = 'd2dsrc'
        user = 'stage'
      elif transfertype == 'd2ddest':
        protocol = 'd2ddest'
        user = 'stage'
      else:
        protocol = transfer[10]
        try:
          user = pwd.getpwuid(int(transfer[20]))[0]
        except KeyError:
          user = transfer[20]
      if not reqUser or reqUser == user:
        n = n + 1
        ns = ns + self.config.getValue('DiskManager', protocol+'Weight', None, int)
        if protocol not in nproto: nproto[protocol] = 0
        nproto[protocol] = nproto[protocol] + 1
        if protocol not in nsproto: nsproto[protocol] = 0
        nsproto[protocol] = nsproto[protocol] + 1
    if detailed:
      return n, tuple(nproto.items()), ns, tuple(nsproto.items())
    else:
      return n, ns

  def nbUsedSlots(self):
    '''returns number of slots occupied by running transfers'''
    n = 0
    for transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, runTime in self._transfers:
      if transfertype == 'd2dsource':
        protocol = 'd2dsrc'
      elif transfertype == 'd2ddest':
        protocol = 'd2ddest'
      else:
        protocol = transfer[10]
      n = n + self.config.getValue('DiskManager', protocol+'Weight', None, int)
    return n

  def poll(self):
    '''checks for finished transfers and clean them up'''
    sourcesToBeInformed = []
    killedTransfers = {}
    self._lock.acquire()
    try:
      ended = []
      for transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, runTime in self._transfers:
        # check whether the job is over
        isEnded = False
        if jobid in self.leftOverJobs:
          # special care for left over jobs, we use a signla 0 for them as they are not our children
          try :
            pid = self.leftOverJobs[jobid]
            os.kill(pid, 0)
            # a process with this pid exists, now is it really our guy or something new ?
            cmdline = open(os.path.sep+os.path.join('proc', str(pid), 'cmdline'), 'rb').read().split('\0')
            if len(cmdline) < 5 or jobid != cmdline[4]:
              # it's a new one, not our guy
              isEnded = True
          except OSError:
            # process is dead
            isEnded = True
          if isEnded:
            rc = -1
            del self.leftOverJobs[jobid]
        elif process != None:
          # check regular jobs (non left over), except for d2dsource jobs as they have no process
          # for this case, the job is our child, so we can poll
          if self.fake:
            isEnded = True
          else:
            rc = process.poll(-1)
            isEnded = (rc!=None)
        if isEnded:
          # get fileid
          fileid = (transfer[8], int(transfer[6]))
          # "transfer ended message"
          dlf.writedebug(msgs.TRANSFERENDED, subreqid=transferid, fileid=fileid, rc=rc)
          # remove the notification file
          try:
            os.remove(notifyFileName)
          except OSError:
            # already removed ? that's fine
            pass
          # append to list of ended transfers
          ended.append(transferid)
          # in case of disk to disk copy, remember to inform source
          if transfertype == 'd2ddest':
            sourcesToBeInformed.append((scheduler, transferid, fileid))
          # in case of transfers killed by a signal, remember to inform the DB
          if rc < 0:
            if scheduler not in killedTransfers: killedTransfers[scheduler] = []
            killedTransfers[scheduler].append((transferid, fileid, rc, 'Transfer has been killed'))
      # cleanup ended transfers
      self._transfers = set(transfer for transfer in self._transfers if transfer[0] not in ended)
    finally:
      self._lock.release()
    # inform schedulers of disk to disk transfers that are over
    for scheduler, transferid, fileid in sourcesToBeInformed:
      try:
        self.connections.d2dend(scheduler, transferid)
      except Exception, e:
        # "Informing scheduler that d2d transfer is over failed" message
        dlf.writeerr(msgs.INFORMTRANSFERISOVERFAILED, scheduler=scheduler, subreqid=transferid, fileid=fileid, error=str(e))
    # inform schedulers of transfers killed
    for scheduler in killedTransfers:
      try:
        self.connections.transfersKilled(scheduler, tuple(killedTransfers[scheduler]))
      except Exception, e:
        for transferid, fileid, rc, msg in killedTransfers[scheduler]:
          # "Informing scheduler that transfers were killed by signals failed" message
          dlf.writeerr(msgs.INFORMTRANSFERKILLEDFAILED, scheduler=scheduler, subreqid=transferid, fileid=fileid)

  def d2dend(self, transferid):
    '''called when a disk to disk copy ends and we are the source of it'''
    self._lock.acquire()
    try:
      self._transfers = set(transfer for transfer in self._transfers if transfer[0] != transferid)
    finally:
      self._lock.release()

  def listTransfers(self, reqUser=None):
    '''lists running transfers'''
    res = []
    n = 0
    for transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, runTime in self._transfers:
      if transfertype == 'standard':
        try:
          user = pwd.getpwuid(int(transfer[20]))[0]
        except KeyError:
          user = transfer[20]
      else:
        user = 'stage'
      if not reqUser or user == reqUser:
        res.append((transferid, scheduler, user, 'RUN', transfertype, arrivalTime, runTime))
        n = n + 1
        if n >= 1000: # give up with full listing if too many transfers
          break
    return res

  def transferset(self):
    '''Lists all pending and running transfers'''
    return set([transferid for transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, runTime in self._transfers])

  def listRunningD2dSources(self):
    '''lists running d2dsource transfers'''
    return [(transferid, transfer, arrivalTime) for transferid, scheduler, transfer, notifyFileName, process, transfertype, arrivalTime, runTime in self._transfers if transfertype == 'd2dsource']
