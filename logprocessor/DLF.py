#!/usr/bin/env python
#-------------------------------------------------------------------------------
# Author: Lukasz Janyst <ljanyst@cern.ch>
# Date:   10.08.2009
# File:   DLF.py
# Desc:   Implements the DLF syslog message processing and storing in the
#         database
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Imports
#-------------------------------------------------------------------------------
import sys
import re
import time
import os
import LoggingCommon
from datetime import datetime as dt

try:
    import cx_Oracle
except ImportError:
    print "Please install cx_Oracle"
    sys.exit( 1 )

#-------------------------------------------------------------------------------
class DLFDbDest(LoggingCommon.MsgDestination):
    """
    This message destination stores the DLF messages in the database
    """

    #---------------------------------------------------------------------------
    def __init__( self ):
        pass

    #---------------------------------------------------------------------------
    def initialize( self, config ):
        """
        connString - the database connection string
        """

        #-----------------------------------------------------------------------
        # Connect to the database
        #-----------------------------------------------------------------------
        if not config.has_key( 'connection_string' ):
            connString = 'file:///etc/castor/DLFCONFIG'
        else:
            connString = config['connection_string']

        #-----------------------------------------------------------------------
        # Read the connect string from a file
        #-----------------------------------------------------------------------
        if connString[0:7] == 'file://':
            try:
                f = open( connString[7:] , 'r' )
                for line in f.readlines():
                    if line[0:1] not in '#':
                        connString = line.rstrip( '\n' )
                f.close
            except:
                raise LoggingCommon.ConfigError( 'Unable to parse connect '
                                                 'string file: ' + connString )

        #-----------------------------------------------------------------------
        # Try and connect to the database
        #-----------------------------------------------------------------------
        try:
            self.__conn = cx_Oracle.Connection( connString )
            self.__curs = self.__conn.cursor()
        except cx_Oracle.DatabaseError, e:
            raise RuntimeError( str(e) )

        #-----------------------------------------------------------------------
        # Set up the bulk count
        #-----------------------------------------------------------------------
        if config.has_key( 'bulk_count' ):
            self.bulkCount = int(config['bulk_count'])
        else:
            self.bulkCount = 1000

        #-----------------------------------------------------------------------
        # Set up the domain name
        #-----------------------------------------------------------------------
        if config.has_key( 'domain_name' ):
            self.domainName = config['domain_name']
        else:
            self.domainName = ''

        #-----------------------------------------------------------------------
        # Set up the flush interval
        #-----------------------------------------------------------------------
        if config.has_key( 'flush_interval' ):
            self.flushInterval = int(config['flush_interval'])
        else:
            self.flushInterval = 60
        self.lastFlush = time.time()

        #-----------------------------------------------------------------------
        # Check the if the database schema version matches the one we expect
        #-----------------------------------------------------------------------
        self.__curs.execute( "SELECT schemaversion FROM DLF_VERSION" )
        ver = self.__curs.fetchall()[0][0]
        if ver != '2_1_9_0':
            raise RuntimeError( 'Database schema does not match the expected ' +
                                'one' )

        self.__msgQueue = []
        self.__intQueue = []
        self.__strQueue = []

        #-----------------------------------------------------------------------
        # Fill the caches
        #-----------------------------------------------------------------------
        self.reloadHostCache()
        self.reloadFacCache()
        self.reloadSevCache()
        self.reloadTextsCache()
        self.reloadNsHostCache()

    #---------------------------------------------------------------------------
    def reloadHostCache( self ):
        self.__hostmap = {}
        self.__curs.execute( "SELECT hostid, hostname FROM DLF_HOST_MAP" )
        data = self.__curs.fetchall()
        for row in data:
            self.__hostmap[row[1]] = row[0]

    #---------------------------------------------------------------------------
    def reloadFacCache( self ):
        self.__facmap = {}
        self.__curs.execute( "SELECT fac_no, fac_name FROM DLF_FACILITIES" )
        data = self.__curs.fetchall()
        for row in data:
            self.__facmap[row[1]] = row[0]

    #---------------------------------------------------------------------------
    def reloadSevCache( self ):
        self.__sevmap = {}
        self.__curs.execute( """SELECT sev_no, sev_name
                                FROM DLF_SEVERITIES ORDER BY sev_no DESC""" )
        data = self.__curs.fetchall()
        for row in data:
            self.__sevmap[row[1]] = row[0]

    #---------------------------------------------------------------------------
    def reloadTextsCache( self ):
        self.__textmap = {}
        self.__curs.execute( """SELECT fac_no, msg_no, msg_text
                                FROM DLF_MSG_TEXTS""" )
        data = self.__curs.fetchall()
        for row in data:
            tmap = None
            if row[0] in self.__textmap.keys():
                tmap = self.__textmap[row[0]]
            else:
                tmap = {}
                self.__textmap[row[0]] = tmap
            tmap[row[2]] = row[1]

    #---------------------------------------------------------------------------
    def reloadTextsCacheFac( self, facility ):
        #-----------------------------------------------------------------------
        # Fetch the data
        #-----------------------------------------------------------------------
        self.__curs.execute( """SELECT msg_no, msg_text FROM DLF_MSG_TEXTS
                                WHERE fac_no = :fac""",
                             fac = facility)
        data = self.__curs.fetchall()

        #-----------------------------------------------------------------------
        # Insert the records to the cache
        #-----------------------------------------------------------------------
        tmap = {}
        for row in data:
            tmap[row[1]] = row[0]
        self.__textmap[facility] = tmap

    #---------------------------------------------------------------------------
    def reloadNsHostCache( self ):
        self.__nshostmap = {}
        self.__curs.execute( "SELECT nshostid, nshostname FROM DLF_NSHOST_MAP" )
        data = self.__curs.fetchall()
        for row in data:
            self.__nshostmap[row[1]] = row[0]

    #---------------------------------------------------------------------------
    def flushQueues( self ):
        self.lastFlush = time.time()
        #-----------------------------------------------------------------------
        # Insert with returning doesn't work in cx_Oracle with bulk insertions
        # so we first have to get the ids and then insert the records
        #-----------------------------------------------------------------------
        self.__curs.execute( """SELECT rownum, ids_seq.nextval
                                FROM DUAL CONNECT BY rownum <= :num""",
                             num = len(self.__msgQueue) )
        ids = self.__curs.fetchall()

        #-----------------------------------------------------------------------
        # Insert the messages
        #-----------------------------------------------------------------------
        for i in range(len(self.__msgQueue)):
            msg = self.__msgQueue[i]
            msg['id'] = ids[i][1]

        try:
            self.__curs.prepare( """INSERT /*+ APPEND */ INTO
                                    DLF_MESSAGES
                                    VALUES(:id, :timestamp,
                                           :timeusec, :reqid, :subreqid,
                                           :hostid, :facility, :severity,
                                           :msg_no, :pid, :tid, :nshostid,
                                           :nsfileid, :tapevid, :userid,
                                           :groupid, :sec_type, :sec_name)""" )

            self.__curs.setinputsizes( id        = cx_Oracle.NUMBER,
                                       timestamp = cx_Oracle.DATETIME,
                                       timeusec  = cx_Oracle.NUMBER,
                                       reqid     = cx_Oracle.STRING,
                                       subreqid  = cx_Oracle.STRING,
                                       hostid    = cx_Oracle.NUMBER,
                                       facility  = cx_Oracle.NUMBER,
                                       severity  = cx_Oracle.NUMBER,
                                       msg_no    = cx_Oracle.NUMBER,
                                       pid       = cx_Oracle.NUMBER,
                                       tid       = cx_Oracle.NUMBER,
                                       nshostid  = cx_Oracle.NUMBER,
                                       nsfileid  = cx_Oracle.NUMBER,
                                       tapevid   = cx_Oracle.STRING,
                                       userid    = cx_Oracle.NUMBER,
                                       groupid   = cx_Oracle.NUMBER,
                                       sec_type  = cx_Oracle.STRING,
                                       sec_name  = cx_Oracle.STRING )

            self.__curs.executemany( None, self.__msgQueue )

            #-------------------------------------------------------------------
            # Assign the ids to the key value pairs
            #-------------------------------------------------------------------
            for kv in self.__strQueue:
                kv['id'] = ids[kv['id']][1]

            for kv in self.__intQueue:
                kv['id'] = ids[kv['id']][1]

            #-------------------------------------------------------------------
            # Insert the key-value pairs
            #-------------------------------------------------------------------
            if len(self.__intQueue):
                self.__curs.executemany( """INSERT /*+ APPEND */ INTO
                                            DLF_NUM_PARAM_VALUES
                                            VALUES(:id, :timestamp, :name,
                                               :value)""",
                                            self.__intQueue )
            if len(self.__strQueue):
                self.__curs.executemany( """INSERT /*+ APPEND */ INTO
                                            DLF_STR_PARAM_VALUES
                                            VALUES(:id, :timestamp, :name,
                                               :value)""",
                                            self.__strQueue )

            #-------------------------------------------------------------------
            # Clean the buffers and commit the transaction
            #-------------------------------------------------------------------
            self.__msgQueue = []
            self.__intQueue = []
            self.__strQueue = []

            self.__conn.commit()

        except cx_Oracle.DatabaseError, e:
            #-------------------------------------------------------------------
            # Clean the buffers and rollback the transaction.
            #-------------------------------------------------------------------
            self.__msgQueue = []
            self.__intQueue = []
            self.__strQueue = []

            self.__conn.rollback()
            raise RuntimeError( str(e) )

    #---------------------------------------------------------------------------
    def finalize( self ):
        self.flushQueues()

    #---------------------------------------------------------------------------
    def insertMsgText( self, message ):

        #-----------------------------------------------------------------------
        # Check if we know this facility
        #-----------------------------------------------------------------------
        if not self.__facmap.has_key( message['facility'] ):
            raise ValueError( 'Unrecognized type of facility' )
        facility = self.__facmap[message['facility']]

        #-----------------------------------------------------------------------
        # Process the message
        #-----------------------------------------------------------------------
        msg = {}
        msg['fac'] = facility
        msg['no'] = int( message['msgno'])
        msg['txt'] = message['msgtext']

        #-----------------------------------------------------------------------
        # Check if we already have this message in the cache
        #-----------------------------------------------------------------------
        if self.__textmap.has_key( facility ) and self.__textmap[facility
           ].has_key( msg['txt'] ):
            return

        #-----------------------------------------------------------------------
        # Update the database
        #-----------------------------------------------------------------------
        try:
            self.__curs.execute( """MERGE INTO DLF_MSG_TEXTS A
                                    USING
                                     (SELECT fac_no, msg_no
                                        FROM (SELECT fac_no, msg_no
                                                FROM DLF_MSG_TEXTS
                                               WHERE fac_no = :fac
                                                 AND msg_no = :no
                                               UNION ALL
                                              SELECT -1, -1 FROM DUAL)
                                       WHERE rownum = 1) B
                                          ON (A.fac_no = B.fac_no
                                         AND  A.msg_no = B.msg_no)
                                    WHEN MATCHED THEN
                                      UPDATE SET A.msg_text = :txt
                                    WHEN NOT MATCHED THEN
                                      INSERT (fac_no, msg_no, msg_text)
                                      VALUES (:fac, :no, :txt)""", msg )
            self.__conn.commit()
        except cx_Oracle.DatabaseError, e:
            # Somebody have already inserted this message text
            if e.message.code != 1:
                raise RuntimeError(str(e))

        #-----------------------------------------------------------------------
        # Update the cache
        #-----------------------------------------------------------------------
        tmap = None
        if self.__textmap.has_key( msg['fac'] ):
            tmap = self.__textmap[msg['fac']]
        else:
            tmap = {}
            self.__textmap[msg['fac']] = tmap
        tmap[msg['txt']] = msg['no']

    #---------------------------------------------------------------------------
    def insertHost( self, host ):
        #-----------------------------------------------------------------------
        # Try to insert the hostname
        #-----------------------------------------------------------------------
        try:
            hostidVar = self.__curs.var( cx_Oracle.NUMBER )
            self.__curs.execute( """INSERT INTO
                                    DLF_HOST_MAP(hostid, hostname)
                                    VALUES(ids_seq.nextval, :name)
                                    RETURNING hostid INTO :id""",
                                    name = host, id = hostidVar )
            self.__conn.commit()
            hostid = hostidVar.getvalue()
            self.__hostmap[host] = int( hostid )
            return int( hostid )
        except cx_Oracle.DatabaseError, e:
            # Somebody might have already inserted this host
            if e.message.code != 1:
                raise RuntimeError( 'Unable to insert new record to host map' )

        #-----------------------------------------------------------------------
        # Update the host cache
        #-----------------------------------------------------------------------
        reloadHostCache()
        return self.__hostmap[host]

    #---------------------------------------------------------------------------
    def insertNsHost( self, host ):
        #-----------------------------------------------------------------------
        # Try to insert the hostname
        #-----------------------------------------------------------------------
        try:
            hostidVar = self.__curs.var( cx_Oracle.NUMBER )
            self.__curs.execute( """INSERT INTO
                                    DLF_NSHOST_MAP(nshostid, nshostname)
                                    VALUES(ids_seq.nextval, :name)
                                    RETURNING nshostid into :id""",
                                    name = host, id = hostidVar )
            self.__conn.commit()
            hostid = hostidVar.getvalue()
            self.__nshostmap[host] = int( hostid )
            return int( hostid )
        except cx_Oracle.DatabaseError, e:
            # Somebody might have already inserted this host
            if e.message.code != 1:
                raise RuntimeError( 'Unable to insert new record to host map' )

        #-----------------------------------------------------------------------
        # Update the host cache
        #-----------------------------------------------------------------------
        reloadNsHostCache()
        return self.__nshostmap[host]

    #---------------------------------------------------------------------------
    def insertMessage( self, message ):
        #-----------------------------------------------------------------------
        # Initialize the message
        #-----------------------------------------------------------------------
        msg = {}
        for i in ['id', 'timestamp', 'timeusec', 'reqid', 'subreqid', 'hostid',
                  'facility', 'severity', 'msg_no', 'pid', 'tid', 'nshostid',
                  'nsfileid', 'tapevid', 'userid', 'groupid', 'sec_type',
                  'sec_name']:
            msg[i] = None

        #-----------------------------------------------------------------------
        # Initialize message defaults
        #-----------------------------------------------------------------------
        msg['reqid']    = '00000000-0000-0000-0000-000000000000'
        msg['subreqid'] = '00000000-0000-0000-0000-000000000000'
        msg['userid']   = -1
        msg['groupid']  = -1
        msg['sec_type'] = 'N/A'
        msg['sec_name'] = 'N/A'
        msg['tapevid']  = 'N/A'

        if not message['nshostname']:
            message['nshostname'] = 'N/A'
        if not message['nsfileid']:
            message['nsfileid']   = 0

        #-----------------------------------------------------------------------
        # Create the timestamp - parsing fractional seconds is not supported
        # in python <= 2.5 which is a default but should be supported in
        # later versions. Hence this workaround.
        #
        # For some reason the fractional seconds do not work with the
        # cx_Oracle version I am using
        #-----------------------------------------------------------------------
        tsplit  = message['time'].split('.')
        tstring = message['date'] + ' ' + tsplit[0]
        tmptm   = time.strptime( tstring, '%Y-%m-%d %H:%M:%S' )
        tm      = dt( tmptm.tm_year, tmptm.tm_mon, tmptm.tm_mday, tmptm.tm_hour,
                      tmptm.tm_min, tmptm.tm_sec, int( tsplit[1] ) )
        msg['timestamp'] = tm
        msg['timeusec']  = int( tsplit[1] )

        #-----------------------------------------------------------------------
        # Insert the hostid
        #-----------------------------------------------------------------------
        hostid = None
        hostname = message['host']
        if self.domainName != '':
            hostname += '.'
            hostname += self.domainName

        if not self.__hostmap.has_key( hostname ):
            hostid = self.insertHost( hostname )
        else:
            hostid = self.__hostmap[hostname]
        msg['hostid'] = hostid

        #-----------------------------------------------------------------------
        # Insert facility
        #-----------------------------------------------------------------------
        facility = message['ident']
        facid    = None
        if self.__facmap.has_key( facility ):
            facid = self.__facmap[facility]
        else:
            raise ValueError( 'Malformed message: unrecognized facility: ' +
                              facility )
        msg['facility'] = facid

        #-----------------------------------------------------------------------
        # Insert severity
        #-----------------------------------------------------------------------
        severity = message['level']
        sevid    = None
        if self.__sevmap.has_key( severity ):
            sevid = self.__sevmap[severity]
        else:
            raise ValueError( 'Malformed message: unrecognized severity: ' +
                              severity )
        msg['severity'] = sevid

        #-----------------------------------------------------------------------
        # Insert other mandatory stuff
        #-----------------------------------------------------------------------
        msg['pid'] = int(message['pid'])
        msg['tid'] = int(message['tid'])

        #-----------------------------------------------------------------------
        # Insert the message id
        #-----------------------------------------------------------------------
        reloaded = False
        if not self.__textmap.has_key( facid ):
            self.reloadTextsCacheFac( facid )
            reloaded = True

        if not self.__textmap.has_key( facid ):
            raise ValueError( 'Malformed message: no msg texts known for ' +
                              'facility: ' + facility )

        facmsgs = self.__textmap[facid]

        if not facmsgs.has_key( message['msg'] ) and not reloaded:
            self.reloadTextsCacheFac( facid )

        if not facmsgs.has_key( message['msg'] ):
            raise ValueError( 'Malformed message: unknown message text: ' +
                              message['msg'] + ' for facility: ' + facility)

        msg['msg_no'] = facmsgs[message['msg']]

        #-----------------------------------------------------------------------
        # Insert the nshostid and nsfileid
        #-----------------------------------------------------------------------
        if message['nshostname']:
            nshostid = None
            nshostname = message['nshostname']
            if self.domainName != '':
                nshostname += '.'
                nshostname += self.domainName

            if not self.__nshostmap.has_key( nshostname ):
                nshostid = self.insertNsHost( nshostname )
            else:
                nshostid = self.__nshostmap[nshostname]
            msg['nshostid'] = nshostid

        msg['nsfileid'] = int(message['nsfileid'])

        #-----------------------------------------------------------------------
        # Reqid
        #-----------------------------------------------------------------------
        if message['reqid']:
            msg['reqid'] = message['reqid']

        #-----------------------------------------------------------------------
        # Process the key-value pairs
        #-----------------------------------------------------------------------
        id     = len(self.__msgQueue)
        kv_str = []
        kv_int = []
        kvdict = message['key-value']
        for kv in kvdict:
            if kv == 'TPVID':
                msg['tapevid'] = kvdict[kv]
            elif kv == 'UID':
                msg['userid'] = kvdict[kv]
            elif kv == 'GID':
                msg['groupid'] = kvdict[kv]
            elif kv == 'SNAME':
                msg['sec_name'] = kvdict[kv]
            elif kv == 'STYPE':
                msg['sec_type'] = kvdict[kv]
            elif kv == 'SUBREQID':
                msg['subreqid'] = kvdict[kv]
            else:
                intval = None
                rec = {}
                rec['id']        = id
                rec['timestamp'] = msg['timestamp']
                rec['name']      = kv

                if kvdict[kv] == None:
                    rec['value'] = ""
                    kv_str.append( rec )
                    continue

                # Integers
                try:
                   rec['value'] = int( kvdict[kv] )
                   kv_int.append( rec )
                   continue
                except ValueError:
                   pass

                # Floats
                try:
                   rec['value'] = float( kvdict[kv] )
                   kv_int.append( rec )
                   continue
                except ValueError:
                   pass

                # Strings
                rec['value'] = kvdict[kv]
                kv_str.append( rec )

        self.__strQueue += kv_str
        self.__intQueue += kv_int
        self.__msgQueue.append( msg )

        if (self.bulkCount <= len( self.__msgQueue )) or (len( self.__msgQueue ) > 0 and (time.time() - self.lastFlush) > self.flushInterval):
            self.flushQueues()

    #---------------------------------------------------------------------------
    def sendMessage( self, msg ):
        if msg['type'] == 'log':
            self.insertMessage( msg )
        elif msg['type'] == 'msgadd':
            self.insertMsgText( msg )
        else:
            raise ValueError( 'Unrecognized type of message' )

#-------------------------------------------------------------------------------
class DLFMsgParser:
    """
    Message parser

    Parse the input messages according to given regular expressions
    """

    #---------------------------------------------------------------------------
    def __init__( self ):
        self.exp = re.compile( r"""
           ^                                         # beginning of the string
           (?P<date>     \d{4}-\d\d-\d\d)        T   # date
           (?P<time>     \d\d:\d\d:\d\d[.]\d{6})     # time
           (?P<timezone> [+\-]\d\d:\d\d)         \s+ # timezone
           (?P<host>     [\w\-]+)                \s+ # host
           (?P<ident>    [\w\-]+)                    # process identifier

           \[
           (?P<pid>      \d+)                        # pid
           \] \s*

           :                                     \s* # colon
           LVL=(?P<level> \w+)                   \s+ # level
           TID=(?P<tid> \d+)                     \s+ # tid
           MSG="(?P<msg> .*?)" # message

           # Some optional fields that must come in the right order
           (?: \s+ REQID=(?P<reqid> [\w\-]+)           )? # reqid
           (?: \s+ NSHOSTNAME=(?P<nshostname> [\w\-]+) )? # nshostname
           (?: \s+ NSFILEID=(?P<nsfileid> [\d]+)       )? # nsfileid

           (?P<message> .*)                               # message
           $                                              # the end
              """, re.VERBOSE )

        self.mexp = re.compile( r"""
           ^                                         # beginning of the string
           \s*                                       # white spaces
           (\w+)                                     # key
           \s*                                       # white spaces
           =                                         # equals
           \s*                                       # white spaces
           (?:"(.*?)" | (\S+))                       # value
              """, re.VERBOSE )

        self.maddexp = re.compile( r"""
           ^                                         # beginning of the string
           (?P<date>     \d{4}-\d\d-\d\d)        T   # date
           (?P<time>     \d\d:\d\d:\d\d[.]\d{6}) \+  # time
           (?P<timezone> \d\d:\d\d)              \s+ # timezone
           (?P<host>     [\w\-]+)                \s+ # host
           (?P<ident>    [\w\-]+)                    # process identifier

           \[
           (?P<pid>      \d+)                        # pid
           \] \s*

           :                                     \s* # colon
           MSGNO=(?P<msgno> \w+)                 \s+ # level
           MSGTEXT="(?P<msgtext> .*?)"               # message
           \s*                                       # whitespaces
           $                                         # the end
              """, re.VERBOSE )


    #---------------------------------------------------------------------------
    def __call__( self, msg ):
        """
        Parse the input string
        """
        #-----------------------------------------------------------------------
        # Parse the message
        #-----------------------------------------------------------------------
        types = [('log',self.exp), ('msgadd', self.maddexp)]
        type  = None
        res   = None

        for t in types:
            res = t[1].match( msg )
            if res:
                type = t[0]
                break

        if not res:
            raise ValueError( 'Message: "' + msg[:30] + '..." is malformed' )

        result = {}

        #-----------------------------------------------------------------------
        # We have a normal log message
        #-----------------------------------------------------------------------
        result['type'] = type
        if type == 'log':
            for g in res.groupdict().keys():
                if g == 'message':
                    continue
                result[g] = res.group( g )

            if result.has_key( 'nsfileid' ) and not result.has_key( 'nshostname' ):
                raise ValueError( 'Message: "' + msg[:30] +
                                  '..." is malformed' )

            #-------------------------------------------------------------------
            # Parse the key-value pairs
            #-------------------------------------------------------------------
            message = res.group( 'message' )
            vals = []
            start = 0
            while True:
                res = self.mexp.match( message[start:] )
                if not res:
                    break
                start += res.span()[1]
                if res.group(2):
                    val = res.group(2)
                else:
                    val = res.group(3)
                vals.append((res.group(1), val))

            result['key-value'] = dict(vals)

        #-----------------------------------------------------------------------
        # We have a message text insertion message
        #-----------------------------------------------------------------------
        elif type == 'msgadd':
            result['facility'] = res.group( 'ident' )
            result['msgno']    = res.group( 'msgno' )
            result['msgtext']  = res.group( 'msgtext' )

        return result


#-------------------------------------------------------------------------------
class DLFLogFile(LoggingCommon.MsgSource):
    #---------------------------------------------------------------------------
    def __init__( self ):
        self.source = LoggingCommon.PipeSource()


    #---------------------------------------------------------------------------
    def notify( self ):
        self.source.notify()

    #---------------------------------------------------------------------------
    def getMessage( self ):
        return self.source.getMessage()

    #---------------------------------------------------------------------------
    def initialize( self, config ):
        if not config.has_key( 'type' ):
            raise LoggingCommon.ConfigError( 'DLFLogFile: type of input ' +
                                             'was not specified' )

        if config['type'] != 'pipe' and config['type'] != 'file':
            raise LoggingCommon.ConfigError( 'DLFLogFile: unknown input ' +
                                             'type: ' + config['type'] )

        if config['type'] == 'file':
            self.source = LoggingCommon.FileSource()

        self.source.transform = DLFMsgParser()
        self.source.initialize( config )

    #---------------------------------------------------------------------------
    def finalize( self ):
        self.source.finalize()
