#!/usr/bin/python
import os
import sys
import getopt
import castor_tools

nsHost = "castorns"          # write your nameserver host alias here

# usage function
def usage(exitCode):
  print 'Usage : ' + sys.argv[0] + ' [-h]'
  sys.exit(exitCode)

## main()
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

# connect to DB
try:
    stconn = castor_tools.connectToStager()
    castor_tools.parseNsListClass()
    dbcursor = stconn.cursor()
    # create temp procedure
    sql = '''
CREATE OR REPLACE PROCEDURE reinsertCastorFile(
        dcId IN NUMBER,
        fid IN NUMBER,
        nh IN VARCHAR2,
        fcName IN VARCHAR2,
        fileSize IN NUMBER,
        fname IN VARCHAR2) AS
 scId NUMBER;
 cfId NUMBER;
 unused NUMBER;
 fcId NUMBER;
BEGIN
  SELECT d2s.child INTO scId
    FROM diskcopy, filesystem, diskpool2svcclass d2s 
   WHERE diskcopy.id = dcId 
     and diskcopy.filesystem = filesystem.id
     and filesystem.diskpool = d2s.parent;
  SELECT id INTO fcId
    FROM FileClass
   WHERE name = fcname;
  selectCastorFile(fid, nh, scId, fcId, fileSize, fname, cfId, unused);
  UPDATE DiskCopy set castorFile = cfId, status = 7 WHERE id = dcId;
  COMMIT;
END;'''
    dbcursor.execute(sql)
    # Get list of files to fix and do it
    sqlList = '''
SELECT diskcopy.id, substr(path, instr(path, '/',1,1)+1, instr(path,'@',1,1)-instr(path, '/',1,1)-1)
  FROM DiskCopy
 WHERE (castorFile = 0 or castorFile is null)'''
    dbcursor.execute(sqlList)
    files = dbcursor.fetchall()
    if len(files) == 0:
        print 'Found no diskcopy to fix, exiting'
    else:
        print 'Found %d diskcopies to be updated, starting...' % len(files)
    for f in files:
        namefd = os.popen('nsgetpath castorns ' + str(f[1]))
        name = namefd.read().strip('\n')
        rc = namefd.close()
        if rc == None:
            print 'fileid %d' % f[1]
            lsfd = os.popen('nsls -l --class ' + name)
            ls = lsfd.read().strip('\n').split()
            #print [f[0], f[1], nsHost, nsFileCl[ls[0]], ls[5], ls[9]]
            dbcursor.callproc('reinsertCastorFile', ([f[0], f[1], nsHost, castor_tools.nsFileCl[ls[0]], ls[5], ls[9]]));
    #   else the file does not exist: we can probably drop everything then
    
    # cleanup
    sql = 'DROP PROCEDURE reinsertCastorFile'
    dbcursor.execute(sql)
    print 'Update completed'
except Exception, e:
    print e
    sys.exit(-1)
