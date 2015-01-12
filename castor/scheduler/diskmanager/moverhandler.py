#!/usr/bin/python
#/******************************************************************************
# *                   moverhandler.py
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
# * the mover handler thread of the disk server manager daemon of CASTOR
# *
# * @author Castor Dev team, castor-dev@cern.ch
# *****************************************************************************/

"""mover handler thread of the disk server manager daemon of CASTOR."""

import threading, socket, time, Queue, ast, subprocess
import connectionpool, dlf
from diskmanagerdlf import msgs
from transfer import TransferType, TapeTransfer
from xrootiface import xrootTupleToTransfer

class MoverReqHandlerThread(threading.Thread):
  '''Worker thread handling each mover request'''

  def __init__(self, workQueue, runningTransfers, config):
    '''constructor'''
    super(MoverReqHandlerThread, self).__init__(name='MoverReqHandler')
    # the queue to work with
    self.workQueue = workQueue
    # get the context
    self.runningTransfers = runningTransfers
    self.config = config
    # start the thread
    self.setDaemon(True)
    self.start()

  def stop(self):
    '''Stops the thread processing'''
    # this makes sure we drain the queue and exit only when empty
    self.workQueue.put(None)

  def _failTransfer(self, scheduler, transfer, errCode, errMessage):
    '''Fail a transfer with the given error code and message. The transfer is assumed to have already been dropped
    from the list of running transfers, and thus no lock is taken for this operation.'''
    # get the admin timeout
    timeout = self.config.getValue('TransferManager', 'AdminTimeout', 5, float)
    closeTime = time.time()
    # call transferEnded to fail the transfer
    attempt = 0
    while True:
      try:
        # "Transfer ended" message
        dlf.write(msgs.TRANSFERENDED, subreqId=transfer.transferId, reqId=transfer.reqId, flags=transfer.flags, errCode=errCode, \
                  errMessage=errMessage, fileId=transfer.fileId, elapsedTime=(closeTime-transfer.creationTime), totalTime=(closeTime-transfer.submissionTime))
        rc_unused, msg_unused = connectionpool.connections.transferEnded(scheduler, \
                                                                         (transfer.transferId, transfer.reqId, transfer.fileId, \
                                                                          transfer.flags, 0, closeTime, '', '', errCode, errMessage),
                                                                         timeout=timeout)
        return
      except connectionpool.Timeout:
        # as long as we get a timeout, we retry up to 3 times
        attempt += 1
        if attempt < 3:
          time.sleep(attempt)
        else:
          # give up
          raise IOError('Timeout attempting to end transfer %s in scheduler %s' % (transfer.transferId, scheduler))

  def handleOpen(self, payload):
    '''handle an OPEN call. The protocol is as follows:
    - The mover connects to localhost:15511 [DiskManager/MoverHandlerPort in castor.conf]
    - It sends a single string like
      OPEN errorCode ('tident', 'physicalPath', 'transferType', 'isWriteFlag', 'transferId')
      where errorCode is non-0 in case the transfer is to be cancelled
            tident has the format: username.clientPid:fd@clientHost
            transferType is one of tape, user, d2duser, d2dinternal, d2ddraining, d2drebalance
            transferId is the UUID of the transfer
    - It synchronously waits for a single answer like
      <rc>[ <error message>]\n
      either "0" for success
      or "2 Transfer has disappeared from the scheduling system"
         in case the slot is not available any longer for a user transfer
      or "22 Invalid format"
         in case the tuple does not respect the correct format
    '''
    # parse payload, throw IndexError or TypeError on malformed input
    errCode = int(payload.split()[0])
    tident, physicalPath, transferType, isWriteFlag, transferid = ast.literal_eval(' '.join(payload.split()[1:]))
    if transferType == 'user' or transferType[0:3] == 'd2d':
      # user or disk-to-disk copy transfers:
      try:
        # yes, look it up for this transferid
        t = self.runningTransfers.get(transferid)
      except KeyError:
        # transfer not found: assume it already timed out, raise error
        # "Transfer slot timed out" message
        dlf.writenotice(msgs.TRANSFERTIMEDOUT, subreqId=transferid, transferType=transferType)
        raise
      if errCode != 0:
        # this is a user transfer that has to be failed as we got an error from xroot
        self.runningTransfers.remove(t)
        self._failTransfer(t.scheduler, t.transfer, errCode, 'Error while opening the file')
      else:
        # No xroot error and we found it: if this is a user transfer,
        # set process to something not None (see runningtransferset.py)
        if transferType == 'user':
          self.runningTransfers.setProcess(transferid, 0)
    elif transferType == 'tape':
      # tape transfers: convert the xroot tuple and take note of this tape transfer
      tTransfer = xrootTupleToTransfer(None, (tident, physicalPath, transferType, isWriteFlag, transferid))
      self.runningTransfers.addTapeTransfer(tTransfer)
    else:
      # any other transferType is unknown, this should never happen
      raise ValueError
    return 0

  def handleClose(self, payload):
    '''handle a CLOSE call. The protocol is as follows:
    - The mover connects to localhost:15511 [DiskManager/MoverHandlerPort in castor.conf]
    - It sends a single string like
      CLOSE <transferUUID> <fileSize> <cksumType> <cksumValue> <errorCode>[ <error message>]
      [In case of Get requests, fileSize, cksumType, and cksumValue are ignored]
    - It synchronously waits for a single answer like
      <rc>[ <error message>]\n
    '''
    # parse payload, throw IndexError on malformed input
    transferid, fSize, cksumType, cksumValue, errCode = payload.split()[0:5]
    if errCode != '0':
      errMessage = payload[payload.find(' '+ errCode +' ')+1:].split(' ', 1)[1]     # the error message is any string at the end
    else:
      errCode = 0
      errMessage = None
    # xrootd sends us ADLER32, map it to AD
    if cksumType == 'ADLER32':
      cksumType = 'AD'
    # find transfer in runningTransfers, raise KeyError if not found
    t = self.runningTransfers.get(transferid)
    if type(t) == TapeTransfer:
      # acknowledge that the transfer completed and return
      self.runningTransfers.remove(t)
      return 0
    if t.process == None or t.process == 0:
      # this transfer has no subprocess attached to it (case of xroot and d2d src transfers),
      # so it can be dropped from the list of running transfers at this time.
      # Others are kept so that runningTransfers.poll() 'bcan clean them up.
      self.runningTransfers.remove(t)
    # get the admin timeout
    timeout = self.config.getValue('TransferManager', 'AdminTimeout', 5, float)
    closeTime = time.time()
    # call transferEnded with the given arguments
    attempt = 0
    while True:
      try:
        # log "Transfer ended"
        dlf.write(msgs.TRANSFERENDED, subreqId=transferid, reqId=t.transfer.reqId, fileId=t.transfer.fileId, \
                  transferType=TransferType.toPreciseStr(t.transfer), fileSize=fSize, errCode=errCode, errMessage=errMessage, \
                  totalTime="%.6f" % (closeTime-t.transfer.creationTime), schedulerTime="%.6f" % (closeTime-t.transfer.submissionTime))
        if t.transfer.transferType == TransferType.STD:
          # user transfer
          rc, errMsg = connectionpool.connections.transferEnded(t.scheduler, \
                                                                (transferid, t.transfer.reqId, t.transfer.fileId, t.transfer.flags, \
                                                                 int(fSize), closeTime, cksumType, cksumValue, int(errCode), errMessage), \
                                                                timeout=timeout)
          t.ended = True
          if rc != 0:
            # log "Failed to end the transfer"
            dlf.writenotice(msgs.TRANSFERENDEDFAILED, subreqId=transferid, reqId=t.transfer.reqId, errCode=rc, errMessage=errMsg)
          else:
            dlf.writedebug(msgs.TRANSFERENDED, subreqId=transferid, errCode=0)
          return '%d %s' % (rc, errMsg)
        elif t.transfer.transferType == TransferType.D2DDST:
          # a destination disk-to-disk copy needs to be notified to the scheduler, with no return code
          connectionpool.connections.d2dEnded(t.scheduler,
                                              tuple([(transferid, t.transfer.reqId, t.transfer.fileId,
                                                      socket.getfqdn(), t.localPath, int(fSize), cksumValue, errCode, errMessage)]),
                                              timeout=timeout)
          t.ended = True
          return 0
        elif t.transfer.transferType == TransferType.D2DSRC:
          # nothing else to be done for d2d sources
          return 0
        else:
          raise ValueError('Invalid transfer type %d for transfer %s' % (t.transfer.transferType, transferid))
      except connectionpool.Timeout, e:
        # as long as we get a timeout, we retry up to 3 times
        attempt += 1
        if attempt < 3:
          dlf.writedebug(msgs.TRANSFERENDED, subreqId=transferid, errMessage='Timeout, sleeping %d seconds' % attempt)
          time.sleep(attempt)
        else:
          # give up, inform mover
          dlf.writeerr(msgs.MOVERHANDLEREXCEPTION, Message='Timeout attempting to end transfer %s in scheduler %s' % (transferid, t.scheduler))
          return '%d %s' % (1015, 'Error closing the file in the stager')
      except Exception, e:
        # any other error, we give up and inform the mover
        # "Caught exception in MoverHandler thread" message, error = SEINTERNAL
        dlf.writeerr(msgs.MOVERHANDLEREXCEPTION, Type=str(e.__class__), Message=str(e))
        # report error to the mover
        return '%d %s' % (1015, 'Error closing the file in the stager')

  def handleRequest(self, data):
    '''
    The requests handler for the mover open/close protocol.
    As this is a local protocol, there's no magic number protection
    '''
    # parse input, bail out on any parsing error
    try:
      key, payload = data.split(' ', 1)
      dlf.writedebug(msgs.MOVERCALL, operation=key, payload=payload)
      if key == 'OPEN':
        return str(self.handleOpen(payload))
      elif key == 'CLOSE':
        return str(self.handleClose(payload))
      else:
        raise ValueError
    except (ValueError, TypeError):
      # invalid format, error = EINVAL
      return '%d Invalid format in %s' % (22, data)
    except KeyError:
      # if not found, error = ENOENT
      return '%d Transfer has disappeared from the scheduling system' % (2)
    except IndexError:
      # thrown by split() when not enough parameters, error = EINVAL
      return '%d Not enough parameters in %s' % (22, data)
    except Exception, e:
      # something else went wrong, log "Caught exception in MoverHandler thread"
      dlf.writeerr(msgs.MOVERHANDLEREXCEPTION, Type=str(e.__class__), Message=str(e))
      # still return something, error = SEINTERNAL
      return '%d Internal error: %s' % (1015, str(e))

  def run(self):
    '''main method for the threads. Only get work from the queue and do it'''
    while True:
      try:
        clientsock = self.workQueue.get(True)
        # if None, we have been asked to close the service
        if clientsock == None:
          return
        req = clientsock.recv(1024)
        res = self.handleRequest(req)
        clientsock.send('%s\n' % res)
        clientsock.close()
      except Exception, e:
        # "Caught exception in Worker thread" message
        dlf.writeerr(msgs.WORKEREXCEPTION, Type=str(e.__class__), Message=str(e))


class MoverHandlerThread(threading.Thread):
  '''the mover handler thread.
  This thread is responsible for handling the open and close of the files as requested by the different movers.
  It internally uses a pool of ReqHandlerThread instances to do the actual job.
  '''

  def __init__(self, runningTransfers, config):
    '''constructor'''
    super(MoverHandlerThread, self).__init__(name='MoverHandler')
    self.alive = True
    # get context
    self.runningTransfers = runningTransfers
    self.config = config
    # create a work queue for the worker threads
    self.workQueue = Queue.Queue()
    # create a number of worker threads
    self.workers = []
    nbWorkers = self.config.getValue('DiskManager', 'NbMoverThreads', 5, int)
    for i_unused in range(nbWorkers):
      t = MoverReqHandlerThread(self.workQueue, self.runningTransfers, self.config)
      self.workers.append(t)
    # start the thread
    self.setDaemon(True)
    self.start()

  def stop(self):
    '''stops processing in this thread'''
    self.alive = False

  def run(self):
    '''main method, containing the infinite accept/serve loop'''
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
      sock.bind(('localhost', self.config.getValue('DiskManager', 'MoverHandlerPort', 15511)))
      sock.listen(5)
      sock.settimeout(1)
    except Exception, e:
      # "Caught exception in MoverHandler thread" message
      dlf.writeemerg(msgs.MOVERHANDLEREXCEPTION, \
                     error='Could not bind to the mover handler port: %s. Terminating' % str(e), \
                     port=self.config.getValue('DiskManager', 'MoverHandlerPort', 15511))
      # this is fatal as no mover would be able to close any file, therefore exit
      raise SystemExit

    while self.alive:
      try:
        # wait for a mover to send a message
        clientsock, addr_unused = sock.accept()
        # push this socket to the workQueue
        self.workQueue.put(clientsock)
      except socket.timeout:
        # accept timed out, go back
        pass
      except Exception, e:
        # "Caught exception in MoverHandler thread" message
        dlf.writeerr(msgs.MOVERHANDLEREXCEPTION, Type=str(e.__class__), Message=str(e))

  def join(self, timeout=None):
    # stop and drain the worker threads
    for t in self.workers:
      t.stop(timeout)
    # join the worker threads
    for t in self.workers:
      t.join(timeout)
    # join the master thread
    threading.Thread.join(self, timeout)
