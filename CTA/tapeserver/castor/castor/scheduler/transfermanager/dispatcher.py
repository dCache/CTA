#!/usr/bin/python
#/******************************************************************************
# *                   dispatcher.py
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
# *
# * dispatcher class of the transfer manager of the CASTOR project
# * this class is responsible for polling the DB for transfers to dispatch and
# * effectively dispatch them on the relevant diskservers.
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

'''dispatcher module of the transfer manager daemon.
Handles the polling from the stager DB for new request
and their dispatching to the proper diskservers'''

import time
import threading
import cx_Oracle
import socket
import castor_tools, connectionpool
import Queue
import dlf
from transfermanagerdlf import msgs
from transfer import D2DTransfer, Transfer, TransferType
import copy

class WorkerThread(threading.Thread):
  '''Worker thread, responsible for scheduling effectively the transfers on the diskservers'''

  def __init__(self, workqueue):
    '''constructor'''
    super(WorkerThread, self).__init__(name='Worker')
    # the queue to work with
    self.workqueue = workqueue
    # whether to continue running
    self.running = True
    # start the thread
    self.setDaemon(True)
    self.start()

  def stop(self):
    '''Stops the thread processing'''
    self.running = False

  def run(self):
    '''main method to the threads. Only get work from the queue and do it'''
    while self.running:
      try:
        func, args = self.workqueue.get(True)
        # func may be None in case we wanted to exit the blocking get in order to close the service
        if func:
          func(*args)
      except Queue.Empty:
        # we've timed out, let's just retry. We only use the timeout so that this
        # thread can stop even if there is nothing in the queue
        pass
      except Exception, e:
        # "Caught exception in Worker thread" message
        dlf.writeerr(msgs.WORKEREXCEPTION, Type=str(e.__class__), Message=str(e))

class DBUpdaterThread(threading.Thread):
  '''Worker thread, responsible for updating DB asynchronously and in bulk after the transfer scheduling'''

  def __init__(self, workqueue):
    '''constructor'''
    super(DBUpdaterThread, self).__init__(name='DBUpdater')
    # whether we are connected to the stager DB
    self.stagerConnection = None
    # the queue to work with
    self.workqueue = workqueue
    # whether to continue running
    self.running = True
    # start the thread
    self.setDaemon(True)
    self.start()

  def stop(self):
    '''Stops the thread processing'''
    self.running = False

  def dbConnection(self):
    '''returns a connection to the stager DB.
    The connection is cached and reconnections are handled'''
    if self.stagerConnection == None:
      self.stagerConnection = castor_tools.connectToStager()
      self.stagerConnection.autocommit = True
    return self.stagerConnection

  def run(self):
    '''main method to the threads. Only get work from the queue and do it'''
    try:
      while self.running:
        # get something from the queue and then empty the queue and list all the updates to be done in one bulk
        failures = []
        # check whether there is something to do: we don't use timeouts because they cause spin locks,
        # thus we rely on the stopper to push a None entry (cf. Dispatcher.join)...
        transferId, fileId, errcode, errmsg, reqid = self.workqueue.get(True)
        # ...in case we wanted to exit the blocking get in order to close the service
        if transferId:
          failures.append((transferId, fileId, errcode, errmsg, reqid))
        else:
          # we got None, we're about to exit
          continue
        # empty the queue so that we go only once to the DB
        try:
          while True:
            transferId, fileId, errcode, errmsg, reqid = self.workqueue.get(False)
            # skip None, this is a fake message because we are about to exit, so don't process
            if transferId:
              failures.append((transferId, fileId, errcode, errmsg, reqid))
        except Queue.Empty:
          # we are over, the queue is empty
          pass
        # Now call the DB for failures
        data = zip(*failures)
        transferIds = data[0]
        errcodes = data[2]
        errmsgs = data[3]
        try:
          stcur = self.dbConnection().cursor()
          try:
            stcur.execute("BEGIN transferFailedLockedFile(:1, :2, :3); END;", [list(transferIds), list(errcodes), list(errmsgs)])
            for transferId, fileId, errcode, errmsg, reqid in failures:
              # 'Failed transfer' message
              dlf.writenotice(msgs.FAILEDTRANSFER, subreqid=transferId, reqid=reqid, fileId=fileId, ErrorCode=errcode, ErrorMessage=errmsg)
          finally:
            stcur.close()
        except Exception, e:
          for transferId, fileId, errcode, errmsg, reqid in failures:
            # 'Exception caught while failing transfer' message
            dlf.writeerr(msgs.FAILINGTRANSFEREXCEPTION, subreqid=transferId, reqid=reqid, fileId=fileId, Type=str(e.__class__), Message=str(e))
          # check whether we should reconnect to DB, and do so if needed
          self.dbConnection().checkForReconnection(e)
    finally:
      if self.stagerConnection != None:
        try:
          castor_tools.disconnectDB(self.stagerConnection)
        except Exception:
          pass

def inttoip(n):
  '''converts a integer into human readble IP address'''
  if n < 0:
    n = (1<<32) + n
  return socket.inet_ntoa(hex(n)[2:].zfill(8).decode('hex'))

class AbstractDispatcherThread(threading.Thread):
  '''abstract version of a scheduling thread.
     Contains all the threading code, the db connection code and the scheduling code.
     Implementations should only implement the run method to connect to the stager,
     get transfers and call the _schedule method.'''

  def __init__(self, queueingTransfers, name, nbWorkers=5):
    '''constructor of this thread. Arguments are the connection pool and the transfer queue to use'''
    super(AbstractDispatcherThread, self).__init__(name=name)
    # whether we should continue running
    self.running = True
    # the list of queueing transfers
    self.queueingTransfers = queueingTransfers
    # our own name
    self.hostname = socket.getfqdn()
    # a counter of number of scheduled transfers in the current second
    self.nbTransfersScheduled = 0
    # the current second, so that we can reset the previous counter when it changes
    self.currentSecond = 0
    # whether we are connected to the stager DB
    self.stagerConnection = None
    # a queue of work to be done by the workers
    self.workToDispatch = Queue.Queue(2*nbWorkers)
    # a queue of updates to be done in the DB
    self.updateDBQueue = Queue.Queue()
    # a thread pool of Schedulers
    self.workers = []
    for i in range(nbWorkers):          # pylint: disable=W0612
      t = WorkerThread(self.workToDispatch)
      self.workers.append(t)
    # a DBUpdater thread
    self.dbthread = DBUpdaterThread(self.updateDBQueue)
    # start the thread
    self.setDaemon(True)
    self.start()

  def join(self, timeout=None):
    # put None values to the worker queue so that workers go out of their blocking call to get
    for i_unused in range(len(self.workers)):
      self.workToDispatch.put((None, None))
    # join the worker threads
    for w in self.workers:
      w.join(timeout)
    # put None values to the updateDBQueue so that dbthread goes out of its blocking call to get
    self.updateDBQueue.put((None, (None, None), None, None, None))
    # join the db thread
    self.dbthread.join(timeout)
    # join the master thread
    threading.Thread.join(self, timeout)

  def dbConnection(self):
    '''returns a connection to the stager DB.
    The connection is cached and reconnections are handled'''
    if self.stagerConnection == None:
      self.stagerConnection = castor_tools.connectToStager()
      self.stagerConnection.autocommit = True
    return self.stagerConnection

  def _schedule(self, transferId, reqId, fileId, transferList, transferType, errorCode, errorMessage):
    '''schedules a given transfer on the given set of machine and handles errors.
    Returns whether the scheduling was successful'''
    # put transfers in the queue of pending transfers
    # Note that we have to do this before even attempting to schedule the
    # transfers for real, as a job may start very fast after the scheduling
    # on the first machine, and it expects the queue to be up to date.
    # the consequence is that we may have to amend the list in case we
    # could not schedule everywhere.
    for transfer in transferList:
      self.queueingTransfers.put(transfer)
    # send the transfers to the appropriate diskservers
    # not that we will retry up to 3 times if we do not manage to schedule anywhere
    # we then give up
    nbRetries = 0
    scheduleHosts = []
    while not scheduleHosts and nbRetries < 3:
      nbRetries = nbRetries + 1
      for transfer in transferList:
        try:
          connectionpool.connections.scheduleTransfer(transfer.diskServer, self.hostname, transfer.asTuple())
          scheduleHosts.append(transfer.diskServer)
        except Exception, e:
          # 'Failed to schedule xxx' message
          dlf.writenotice(errorCode, subreqid=transferId, reqid=reqId, fileId=fileId,
                          DiskServer=transfer.diskServer, Type=str(e.__class__), Message=str(e))
    # we are over, check whether we could schedule at all
    if not scheduleHosts:
      # we could not schedule anywhere.... so fail the transfer in the DB
      self.updateDBQueue.put((transferId, fileId, 1721, errorMessage, reqId)) # 1721 = ESTSCHEDERR
      # and remove it from the server queue
      self.queueingTransfers.remove([(transferId, transferType)])
      return False
    else:
      # see where we could not schedule
      failedHosts = set([transfer.diskServer for transfer in transferList]) - set(scheduleHosts)
      # We could scheduler at least on one host
      if failedHosts:
        # but we have failed on others : inform server queue of the failures
        if self.queueingTransfers.transfersStartingFailed([transfer for transfer in transferList
                                                           if transfer.diskServer in failedHosts]):
          # It seems that finally we have not been able to schedule anywhere....
          # This may seem in contradiction with the last but one comment but it actually
          # only means that the machines to which we've managed to schedule have already
          # tried to start the job in the mean time and have all failed, e.g because
          # they have no space.
          # So in practice, we will not start the job and we have to inform the DB
          self.updateDBQueue.put((transferId, fileId, 1721, errorMessage, reqId)) # 1721 = ESTSCHEDERR
          return False
      # 'Marking transfer as scheduled' message
      dlf.write(msgs.TRANSFERSCHEDULED, subreqid=transferId, reqid=reqId, fileId=fileId, type=transferType, hosts=str(scheduleHosts))
      return True

  def stop(self):
    '''Stops processing of this thread'''
    # first stop the acitivity of taking new jobs from the DB
    self.running = False
    # now wait that the internal queue is empty
    # note that this should be implemented with Queue.join and Queue.task_done if we would have python 2.5
    while not self.workToDispatch.empty():
      time.sleep(0.1)
    # then stop the workers
    for w in self.workers:
      w.running = False
    # and finally stop the DB thread
    self.dbthread.running = False


class UserDispatcherThread(AbstractDispatcherThread):
  '''Dispatcher thread dedicated to user transfers'''

  def __init__(self, queueingTransfers, nbWorkers=5):
    '''constructor of this thread. Arguments are the connection pool and the transfer queue to use'''
    super(UserDispatcherThread, self).__init__(queueingTransfers, 'UserDispatcherThread', nbWorkers)

  def _scheduleStandard(self, transfer, destFilesystems):
    '''Schedules a disk to disk copy and handle issues '''
    # extract list of candidates where to schedule and log
    if None == destFilesystems:
      schedCandidates = []
    else:
      schedCandidates = [candidate.split(':') for candidate in destFilesystems.split('|')]
    # 'Scheduling standard transfer' message
    dlf.writedebug(msgs.SCHEDTRANSFER, subreqid=transfer.transferId, reqid=transfer.reqId,
                   fileId=transfer.fileId, DiskServers=str(schedCandidates))
    # build a list of transfers to schedule for each machine
    transferList = []
    for diskServer, mountPoint in schedCandidates:
      dtransfer = copy.copy(transfer)
      dtransfer.diskServer = diskServer
      dtransfer.mountPoint = mountPoint
      transferList.append(dtransfer)
    # effectively schedule the transfer
    self._schedule(transfer.transferId, transfer.reqId, transfer.fileId, transferList, 'standard',
                   msgs.SCHEDTRANSFERFAILED, 'Unable to schedule transfer')

  def run(self):
    '''goes to the stager DB and retrieves user jobs to schedule'''
    configuration = castor_tools.castorConf()
    while self.running:
      try:
        try:
          # prepare a cursor for database polling
          stcur = self.dbConnection().cursor()
          stcur.arraysize = 50
          srId = stcur.var(cx_Oracle.NUMBER)
          srSubReqId = stcur.var(cx_Oracle.STRING)
          srProtocol = stcur.var(cx_Oracle.STRING)
          destFilesystems = stcur.var(cx_Oracle.STRING)
          reqId = stcur.var(cx_Oracle.STRING)
          cfFileId = stcur.var(cx_Oracle.NUMBER)
          cfNsHost = stcur.var(cx_Oracle.STRING)
          reqSvcClass = stcur.var(cx_Oracle.STRING)
          reqType = stcur.var(cx_Oracle.NUMBER)
          reqEuid = stcur.var(cx_Oracle.NUMBER)
          reqEgid = stcur.var(cx_Oracle.NUMBER)
          srOpenFlags = stcur.var(cx_Oracle.STRING)
          clientIp = stcur.var(cx_Oracle.NUMBER)
          clientPort = stcur.var(cx_Oracle.NUMBER)
          reqCreationTime = stcur.var(cx_Oracle.NUMBER)
          stTransferToSchedule = 'BEGIN userTransferToSchedule(:srId, :srSubReqId , :srProtocol, :destFilesystems, :reqId, :cfFileId, :cfNsHost, :reqSvcClass, :reqType, :reqEuid, :reqEgid, :srOpenFlags, :clientIp, :clientPort, :reqCreationTime); END;' # pylint: disable=C0301
          # infinite loop over the polling of the DB
          while self.running:
            # see whether there is something to do
            # note that this will hang until something comes or the internal timeout is reached
            stcur.execute(stTransferToSchedule, (srId, srSubReqId, srProtocol, destFilesystems, reqId, cfFileId,
                                                 cfNsHost, reqSvcClass, reqType, reqEuid, reqEgid,
                                                 srOpenFlags, clientIp, clientPort, reqCreationTime))
            # in case of timeout, we may have nothing to do
            if srId.getvalue() != None:
              # errors are handled internally and there are no exception others than
              # the ones implying the end of the processing
              self.workToDispatch.put((self._scheduleStandard,
                                       (Transfer(srSubReqId.getvalue(), reqId.getvalue(),
                                                 (cfNsHost.getvalue(), int(cfFileId.getvalue())),
                                                 int(reqEuid.getvalue()), int(reqEgid.getvalue()),
                                                 reqSvcClass.getvalue(), reqCreationTime.getvalue(),
                                                 srProtocol.getvalue(), srId.getvalue(),
                                                 int(reqType.getvalue()), srOpenFlags.getvalue(),
                                                 inttoip(int(clientIp.getvalue())),
                                                 int(clientPort.getvalue())),
                                       destFilesystems.getvalue())))
              # if maxNbTransfersScheduledPerSecond is given, request throttling is active
              # What it does is keep a count of the number of scheduled request in the current second
              # and wait the rest of the second if it reached the limit
              maxNbTransfersScheduledPerSecond = configuration.getValue('TransferManager', 'MaxNbTransfersScheduledPerSecond', -1, int)
              if maxNbTransfersScheduledPerSecond >= 0:
                currentTime = time.time()
                currentSecond = int(currentTime)
                # reset the counters if we've changed second
                if currentSecond != self.currentSecond:
                  self.currentSecond = currentSecond
                  self.nbTransfersScheduled = 0
                # increase counter of number of transfers scheduled within the current second
                self.nbTransfersScheduled = self.nbTransfersScheduled + 1
                # did we reach our quota of requests for this second ?
                if self.nbTransfersScheduled >= maxNbTransfersScheduledPerSecond:
                  # check that the second is not just over, so that we do not sleep a negative time
                  if currentTime < self.currentSecond + 1:
                    # wait until the second is over
                    time.sleep(self.currentSecond + 1 - currentTime)
        finally:
          stcur.close()
      except Exception, e:
        # "Caught exception in Dispatcher for regular Job" message
        dlf.writeerr(msgs.DISPATCHJOBEXCEPTION, Type=str(e.__class__), Message=str(e))
        # check whether we should reconnect to DB, and do so if needed
        self.dbConnection().checkForReconnection(e)
        # then sleep a bit to not loop to fast on the error
        time.sleep(1)

class D2DDispatcherThread(AbstractDispatcherThread):
  '''Dispatcher thread dedicated to disk to disk transfers'''

  def __init__(self, queueingTransfers, nbWorkers=5):
    '''constructor of this thread. Arguments are the connection pool and the transfer queue to use'''
    super(D2DDispatcherThread, self).__init__(queueingTransfers, 'D2DDispatcherThread', nbWorkers)

  def _scheduleD2d(self, srcTransfer, sourceFileSystems, destFilesystems):
    '''Schedules a disk to disk copy on the source and destinations and handle issues '''
    # check whether the sources are not empty
    if sourceFileSystems == None:
      # fail the transfer immediately as we have nowhere to go. This will log the error too
      self.updateDBQueue.put((srcTransfer.transferId, srcTransfer.fileId, 1721,   # 1721 = ESTSCHEDERR
                              'No source found', srcTransfer.reqId))
      return
    # check whether the destinations are not empty
    if destFilesystems == None:
      # fail the transfer immediately as we have nowhere to go. This will log the error too
      self.updateDBQueue.put((srcTransfer.transferId, srcTransfer.fileId, 1721,   # 1721 = ESTSCHEDERR
                              'No destination host found', srcTransfer.reqId))
      return
    # first schedule sources transfers
    schedSourceCandidates = [candidate.split(':') for candidate in sourceFileSystems.split('|')]
    # 'Scheduling d2d source' message
    dlf.writedebug(msgs.SCHEDD2DSRC, subreqid=srcTransfer.transferId, reqid=srcTransfer.reqId,
                   fileId=srcTransfer.fileId, DiskServers=str(schedSourceCandidates))
    transferList = []
    for diskServer, mountPoint in schedSourceCandidates:
      stransfer = D2DTransfer(**srcTransfer.__dict__)
      stransfer.diskServer = diskServer
      stransfer.mountPoint = mountPoint
      transferList.append(stransfer)
    # effectively schedule the transfer onto its source
    if not self._schedule(srcTransfer.transferId, srcTransfer.reqId, srcTransfer.fileId, transferList,
                          'd2dsrc', msgs.SCHEDD2DSRCFAILED, 'Unable to schedule on source host'):
      # source could not be scheduled. Give up. Note that the stagerDB has already been updated
      return

    # now schedule on all potential destinations
    schedDestCandidates = [candidate.split(':') for candidate in destFilesystems.split('|')]
    # 'Scheduling d2d destination' message
    dlf.writedebug(msgs.SCHEDD2DDEST, subreqid=srcTransfer.transferId, reqid=srcTransfer.reqId,
                   fileId=srcTransfer.fileId, DiskServers=str(schedDestCandidates))
    # build the list of hosts and transfers to launch
    transferList = []
    for diskServer, mountPoint in schedDestCandidates:
      dtransfer = D2DTransfer(**srcTransfer.__dict__)
      dtransfer.transferType = TransferType.D2DDST
      dtransfer.diskServer = diskServer
      dtransfer.mountPoint = mountPoint
      transferList.append(dtransfer)
    # effectively schedule the transfer onto its destination
    self._schedule(srcTransfer.transferId, srcTransfer.reqId, srcTransfer.fileId, transferList,
                   'd2ddest', msgs.SCHEDD2DDESTFAILED, 'Failed to schedule d2d destination')

  def run(self):
    '''main method, containing the infinite loop'''
    while self.running:
      try:
        try:
          # prepare a cursor for database polling
          stcur = self.dbConnection().cursor()
          stcur.arraysize = 50
          transferId = stcur.var(cx_Oracle.STRING)
          reqId = stcur.var(cx_Oracle.STRING)
          fileId = stcur.var(cx_Oracle.NUMBER)
          nsHost = stcur.var(cx_Oracle.STRING)
          euid = stcur.var(cx_Oracle.NUMBER)
          egid = stcur.var(cx_Oracle.NUMBER)
          svcClassName = stcur.var(cx_Oracle.STRING)
          creationTime = stcur.var(cx_Oracle.NUMBER)
          replicationType = stcur.var(cx_Oracle.NUMBER)
          destFileSystems = stcur.var(cx_Oracle.STRING)
          srcFileSystems = stcur.var(cx_Oracle.STRING)
          stTransferToSchedule = 'BEGIN D2dTransferToSchedule(:transferId, :reqId, :fileId, :nsHost, :euid, :egid, :svcClassName, :creationTime, :replicationType, :destFileSystems, :srcFileSystems); END;' # pylint: disable=C0301
          # infinite loop over the polling of the DB
          while self.running:
            # see whether there is something to do
            # not that this will hang until something comes or the internal timeout is reached
            stcur.execute(stTransferToSchedule, (transferId, reqId, fileId, nsHost, euid, egid,
                                                 svcClassName, creationTime, replicationType,
                                                 destFileSystems, srcFileSystems))
            # in case of timeout, we may have nothing to do
            if transferId.getvalue() != None:
              self.workToDispatch.put((self._scheduleD2d,
                                       (D2DTransfer(transferId.getvalue(), reqId.getvalue(),
                                                    (nsHost.getvalue(), int(fileId.getvalue())),
                                                    int(euid.getvalue()), int(egid.getvalue()),
                                                    svcClassName.getvalue(), creationTime.getvalue(),
                                                    TransferType.D2DSRC, replicationType.getvalue()),
                                        srcFileSystems.getvalue(), destFileSystems.getvalue())))
        finally:
          stcur.close()
      except Exception, e:
        # "Caught exception in Dispatcher thread" message
        dlf.writeerr(msgs.DISPATCHJOBEXCEPTION, Type=str(e.__class__), Message=str(e))
        # check whether we should reconnect to DB, and do so if needed
        self.dbConnection().checkForReconnection(e)
        # then sleep a bit to not loop to fast on the error
        time.sleep(1)
