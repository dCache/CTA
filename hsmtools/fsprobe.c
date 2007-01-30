/******************************************************************************
 *                      fsprobe.c
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2007  CERN
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
 * @(#)$RCSfile: fsprobe.c,v $ $Revision: 1.16 $ $Release$ $Date: 2007/01/30 10:53:47 $ $Author: fuji $
 *
 * 
 *
 * @author Olof Barring, Peter Kelemen
 *****************************************************************************/

#define _GNU_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <syslog.h>
#include <getopt.h>

extern char *optarg;
extern int optind, opterr, optopt;

unsigned char *buffer = NULL;
unsigned char *readBuffer = NULL;
char *directoryName = NULL;
char *pathName = NULL;
char *mailTo = NULL;
char *logFileName = "/tmp/fsprobe.log";
size_t cycle, dumpCount;
size_t bufferSize = 1024*1024;
off64_t fileSize = (2*((off64_t)1024*1024*1024));
off64_t globalBufCount = 0;
off64_t bufCountStartPerFile;
int sleepTime = 3600;
int sleepBetweenBuffers = 1;
int runInForeground = 0;
int useRndBuf = 0;
int useSyslog = 0;
size_t nbLoops = 0;
int help_flag = 0;
int dumpBuffers = 0;
int continueOnDiff = 0;
int rndWait = 0;
int useSync = 0;
int forceCacheFlush = 0;
int useMultiPattern = 0;
int useBufCount = 0;
int useDirectIO = 0;

#define MAXTIMEBUFLEN 128
char timebuf[MAXTIMEBUFLEN];
size_t timebuflen;
time_t now;
struct tm timestamp;

#define BUFCNTLEN	(sizeof(globalBufCount))

const enum RunOptions
{
	Noop,
	PathName,
	LogFile,
	BufferSize,
	FileSize,
	NbLoops,
	SleepTime,
	IOSleepTime,
	Foreground,
	RndBuf,				/* Use buffers w/random data	*/
	MailTo,				/* Notify who?			*/
	Syslog,				/* Mark corruption in syslog	*/
	DumpBuffers,			/* Dump both buffers on diff	*/
	ContinueOnDiff,			/* Continue checking until EOF	*/
	RndWait,			/* Wait random 0-30s at startup	*/
	Sync,				/* Use fdatasync()/fsync()	*/
	ForceCacheFlush,		/* malloc() how many megabytes	*/
	MultiPattern,			/* use more than 2 bit patterns */
	BufCount,			/* use buffer counters		*/
	DirectIO			/* O_DIRECT			*/
} runOptions;

const struct option longopts[] = 
{
	{"help",no_argument,&help_flag,'h'},
	{"PathName",required_argument,NULL,PathName},
	{"LogFile",required_argument,NULL,LogFile},
	{"BufferSize",required_argument,NULL,BufferSize},
	{"FileSize",required_argument,NULL,FileSize},
	{"NbLoops",required_argument,NULL,NbLoops},
	{"SleepTime",required_argument,NULL,SleepTime},
	{"IOSleepTime",required_argument,NULL,IOSleepTime},
	{"Foreground",no_argument,&runInForeground,Foreground},
	{"RndBuf",no_argument,&useRndBuf,RndBuf},
	{"MailTo",required_argument,NULL,MailTo},
	{"Syslog",no_argument,&useSyslog,Syslog},
	{"DumpBuffers",no_argument,&dumpBuffers,DumpBuffers},
	{"ContinueOnDiff",no_argument,&continueOnDiff,ContinueOnDiff},
	{"RndWait",no_argument,&rndWait,RndWait},
	{"Sync",no_argument,&useSync,Sync},
	{"ForceCacheFlush",required_argument,NULL,ForceCacheFlush},
	{"MultiPattern",no_argument,&useMultiPattern,MultiPattern},
	{"BufCount",no_argument,&useBufCount,BufCount},
	{"DirectIO",no_argument,&useDirectIO,DirectIO},
	{NULL, 0, NULL, 0}
};

void usage(char *cmd)
{
	int i;
	
	fprintf(stdout,"Usage: %s \n",cmd);
	for (i=0; longopts[i].name != NULL; i++) {
		fprintf(stdout,"\t--%s %s\n",longopts[i].name,
		(longopts[i].has_arg == no_argument ? "" : longopts[i].name));
	}
	return;
}

void prepareTimeStamp(void)
{
	now = time(NULL);
	if ( localtime_r(&now, &timestamp) ) {
		timebuflen = strftime(timebuf, MAXTIMEBUFLEN, "%F %T ", &timestamp);
	} else {
		timebuflen = strlen(strcpy(timebuf, "localtime() NULL??? "));
	}
}

void myLog(char *str) 
{
	int fd, dontClose = 0;
	
	if ( str == NULL ) return;
	prepareTimeStamp();
	if ( (runInForeground != 0) && (strcmp(logFileName,"stderr") == 0) ) {
		dontClose = 1;
		fd = 2;
	} else {
		fd = open(logFileName,O_WRONLY|O_CREAT|O_APPEND,0644);
	}

	if ( fd == -1 ) {
		fprintf(stderr, "%s %s", timebuf, str);
		return;
	}
	(void) write(fd, timebuf, timebuflen);
	write(fd,str,strlen(str));
	if ( dontClose == 0 ) close(fd);
	return;
}

int initBuffers()
{
	int fdRandom = -1, i, randSize, rcRandom;
	
	if ( useDirectIO ) {
		i = posix_memalign((void *)&buffer, getpagesize(), bufferSize);
		if ( i ) {
			fprintf(stderr, "posix_memalign(%u): error %d\n", bufferSize, i);
			return (-1);
		}
		
		i = posix_memalign((void *)&readBuffer, getpagesize(), bufferSize);
		if ( i ) {
			fprintf(stderr, "posix_memalign(%u): error %d\n", bufferSize, i);
			return (-1);
		}
	} else {
		buffer = (unsigned char *)malloc(bufferSize);
		if ( buffer == NULL ) {
			fprintf(stderr,"Cannot initialize buffer: malloc(%u) -> %s\n",bufferSize,
					    strerror(errno));
			return(-1);
		}
		readBuffer = (unsigned char *)malloc(bufferSize);
		if ( readBuffer == NULL ) {
			fprintf(stderr,"Cannot initialize buffer: malloc(%u) -> %s\n",bufferSize,
					    strerror(errno));
			return(-1);
		}
	}

	if ( useRndBuf ) {
		fdRandom = open("/dev/urandom",O_RDONLY,0644);
		if ( fdRandom < 0 ) {
			fprintf(stderr,"open(/dev/urandom): %s\n",strerror(errno));
			return(-1);
		}
		i = randSize = 0;
		
		while ( randSize < bufferSize ) {
			rcRandom = read(fdRandom,buffer+i*512,512);
			if ( rcRandom < 0 ) {
				fprintf(stderr,"read(): %s\n",strerror(errno));
				return(-1);
			}
			i++;
			randSize += 512;
		}
		close(fdRandom);
	}
	
	return(0);
}

int putInBackground() 
{
	pid_t pid;
	int fdnull = -1, i, maxfds;
	char logbuf[2048];

	if ( (pid = fork()) < 0 ) {
		fprintf(stderr,"failed to go to background, fork(): %s\n",strerror(errno));
		return(-1);
	} else {
		if ( pid > 0 ) {
			printf("starting in background, pid=%d...\n",(int)pid);
			exit(0);
		}
		fdnull = open("/dev/null",O_RDWR);
		maxfds = getdtablesize();
		setsid();
		close(0);
		dup2(fdnull,1);
		dup2(fdnull,2);
		for ( i=3; i<maxfds; i++ ) {
			if ( i != fdnull ) close(i);
		}
	}
	sprintf(logbuf, "fsprobe $Revision: 1.16 $ operational.\n");
	myLog(logbuf);

	sprintf(logbuf, "filesize %llu bufsize %u sleeptime %u iosleeptime %u loops %u\n",
		fileSize, bufferSize, sleepTime, sleepBetweenBuffers, nbLoops);
	myLog(logbuf);

	sprintf(logbuf, "rndbuf %u syslog %u dumpbuf %u cont %u rndwait %u sync %u flush %u\n",
		useRndBuf, useSyslog, dumpBuffers, continueOnDiff, rndWait, useSync, forceCacheFlush);
	myLog(logbuf);

	sprintf(logbuf, "multi %u bufcnt %u directio %u\n",
		useMultiPattern, useBufCount, useDirectIO);
	myLog(logbuf);
	return(0);
}

int writeFile() 
{
	int fd, rc, flags;
	size_t j, bytesToWrite;
	off64_t bytesWritten;
	off64_t *bc;
	char logbuf[2048];
	
	flags = O_WRONLY|O_TRUNC|O_CREAT;
	if ( useDirectIO ) {
		flags |= O_DIRECT;
	}
	fd = open64(pathName, flags, 0644);
	if ( fd == -1 ) {
		sprintf(logbuf,"open(%s, %d) for write: %s\n",
			pathName, flags, strerror(errno));
		myLog(logbuf);
		return(-1);
	}

	bc = (off64_t *)buffer;

	bytesWritten = 0ULL;
	while ( bytesWritten < fileSize ) {
		if ( sleepBetweenBuffers ) {
			sleep(sleepBetweenBuffers);
		}
		bytesToWrite = bufferSize;
		if ( fileSize < bytesWritten+bufferSize ) {
			bytesToWrite = (size_t)(fileSize-bytesWritten);
			sprintf(logbuf, "tail write: cycle %u bufcnt %llu bytes %u\n",
				cycle, globalBufCount, bytesToWrite);
			myLog(logbuf);
		}
		j = 0;
		if ( useBufCount ) *bc = globalBufCount;
		while ( j < bytesToWrite ) {
			rc = write(fd, buffer+j, bytesToWrite-j);
			if ( rc == -1 ) {
				sprintf(logbuf,"write(%s): %s\n",pathName,strerror(errno));
				myLog(logbuf);
				close(fd);
				return(-1);
			}
			if ( rc != bytesToWrite-j ) {
				sprintf(logbuf, "partial write: cycle %u bufcnt %llu bufpos %u offset %llu req %u got %u\n",
					cycle, globalBufCount, j, bytesWritten+j, bytesToWrite-j, rc);
				myLog(logbuf);
			}
			j += rc;
			if ( useSync ) {
				rc = fdatasync(fd);
				if ( rc == -1 ) {
					sprintf(logbuf,"fdatasync(%s): %s\n",pathName,strerror(errno));
					myLog(logbuf);
					close(fd);
					return(-1);
				}
			}
		}
		bytesWritten += bytesToWrite;
		globalBufCount++;
	}
	if ( useSync ) {
		rc = fsync(fd);
		if ( rc == -1 ) {
			sprintf(logbuf,"fsync(%s): %s\n",pathName,strerror(errno));
			myLog(logbuf);
			close(fd);
			return(-1);
		}
	}
	rc = close(fd);
	if ( rc == -1 ) {
		sprintf(logbuf,"close(%s): %s\n",pathName,strerror(errno));
		myLog(logbuf);
		return(-1);
	}
	return(0);
}

/* NOTE(fuji): There is no guarantee that read() will not read cached data,
 * i.e. corruptions on disk are not guaranteed to be exposed if the kernel
 * cache doesn't have a high turnover rate, IOW I/O load.  Currently, we
 * assume `sleepTime' number of seconds elapsed between write and read are
 * sufficiently long for the cache to change.
 */
int checkFile() 
{
	int fd, rc, flags, dumpfd;
	size_t i, j, bytesToRead, diffCount;
	off64_t bytesRead, bufCount;
	off64_t *bc;
	char logbuf[2048];
	char dumpPathName[1024];
	int diffFound = 0;
	int bufStart;
	
	flags = O_RDONLY;
	if ( useDirectIO ) {
		flags |= O_DIRECT;
	}
	fd = open64(pathName, flags, 0644);
	if ( fd == -1 ) {
		sprintf(logbuf,"open(%s, %d) for read: %s\n",
			pathName, flags, strerror(errno));
		myLog(logbuf);
		return(-1);
	}

	bytesRead = 0ULL;
	bufCount = bufCountStartPerFile;
	while ( bytesRead < fileSize ) {
		if ( sleepBetweenBuffers ) {
			sleep(sleepBetweenBuffers);
		}
		
		bytesToRead = bufferSize;
		if ( fileSize < bytesRead+bufferSize ) {
			bytesToRead = (size_t)(fileSize-bytesRead);
			sprintf(logbuf, "tail read: cycle %u bufcnt %llu bytes %u\n",
				cycle, bufCount, bytesToRead);
			myLog(logbuf);
		}
		j = 0;
		while ( j < bytesToRead ) {
			rc = read(fd, readBuffer+j, bytesToRead-j);
			if ( rc == -1 ) {
				sprintf(logbuf,"read(%s): %s\n",pathName,strerror(errno));
				myLog(logbuf);
				close(fd);
				return(-1);
			}
			if ( rc != bytesToRead-j ) {
				sprintf(logbuf, "partial read: cycle %u bufcnt %llu bufpos %u offset %llu req %u got %u\n",
					cycle, bufCount, j, bytesRead+j, bytesToRead-j, rc);
				myLog(logbuf);
			}
			j += rc;
		}

		diffCount = 0;
		if ( useBufCount ) {
			bc = (off64_t *)readBuffer;
			if ( bufCount != *bc ) {
				sprintf(logbuf, "cntdiff: cycle %u expected %llu (0x%08llX) got %llu (0x%08llX)\n",
					cycle, bufCount, bufCount, *bc, *bc);
				myLog(logbuf);
				for (i = 0; i < BUFCNTLEN; i++) {
					if ( *(buffer+i) == *(readBuffer+i) ) {
						continue;
					}
					diffCount++;
				}
			}
			bufStart = BUFCNTLEN;	/* compare pattern bytes after the counter */
		} else {
			bufStart = 0;
		}
		for (i = bufStart; i < bytesToRead; i++) {
			if ( *(buffer+i) == *(readBuffer+i) ) {
				continue;
			}
			diffCount++;
			sprintf(logbuf, "diff: cycle %u bufcnt %llu bufpos %08X %u offset %08llX %llu expected 0x%02X got 0x%02X\n",
				cycle,
				bufCount,
				i,
				i,
				bytesRead+i, 
				bytesRead+i, 
				*(buffer+i), 
				*(readBuffer+i) );
			myLog(logbuf);
		}
		if ( diffCount ) {
			sprintf(logbuf, "total %u differing bytes found\n", diffCount);
			myLog(logbuf);
			diffFound++;
		}

		rc = memcmp(buffer+bufStart, readBuffer+bufStart, bytesToRead-bufStart);
		if ( rc != 0 ) {
			sprintf(logbuf,"Corruption found in %s after %llu bytes\n",
				      pathName,bytesRead);
			myLog(logbuf);
		}

		/* NOTE(fuji): Only do accounting here so that offsets are
		 * correct in any messages logged in above code. */
		bytesRead += bytesToRead;
		bufCount++;

		if ( rc == 0 && diffCount != 0 ) {
			sprintf(logbuf, "OUCH, memcmp() missed some differences!\n");
			myLog(logbuf);
		}
		if ( rc != 0 && diffCount == 0 ) {
			sprintf(logbuf, "OUCH, only memcmp() thinks there are differences!\n");
			myLog(logbuf);
		}

		if ( rc != 0 || diffCount != 0 ) {
			if ( useSyslog != 0 ) {
				syslog(LOG_ALERT,"fsprobe %s",logbuf);
			}
			if ( mailTo != NULL ) {
				sprintf(logbuf,
					"tail %s |"
					"mail -s corruption %s",
				        logFileName, mailTo);
				system(logbuf);
			}
			if ( dumpBuffers ) {
				dumpCount++;
				sprintf(dumpPathName, "%s.%u.%u.ob", pathName, cycle, dumpCount);
				dumpfd = open(dumpPathName, O_WRONLY|O_TRUNC|O_CREAT, 0644);
				if ( dumpfd ) {
					write(dumpfd, buffer, bufferSize);
					close(dumpfd);
				} else {
					sprintf(logbuf, "dump failed: %s, error %d (%s)\n",
						dumpPathName, errno, strerror(errno));
					myLog(logbuf);
				}
				sprintf(dumpPathName, "%s.%u.%u.rb", pathName, cycle, dumpCount);
				dumpfd = open(dumpPathName, O_WRONLY|O_TRUNC|O_CREAT, 0644);
				if ( dumpfd ) {
					write(dumpfd, readBuffer, bufferSize);
					close(dumpfd);
				} else {
					sprintf(logbuf, "dump failed: %s, error %d (%s)\n",
						dumpPathName, errno, strerror(errno));
					myLog(logbuf);
				}
			}
			if ( !continueOnDiff ) break;
		}

	}
	rc = close(fd);
	if ( rc == -1 ) {
		sprintf(logbuf,"close(%s): %s\n",pathName,strerror(errno));
		myLog(logbuf);
		if ( diffFound ) {
			return (-2);
		}
		return(-1);
	}
	if ( diffFound ) {
		return (-2);
	}
	return(0);
}

#define MAXPATTERN 6

void
prepareBitPatternBuffer(void)
{
	unsigned char patterns[MAXPATTERN] = { 0x55, 0xAA, 0x33, 0xCC, 0x0F, 0xF0 };

	if ( useMultiPattern ) {
		memset(buffer, patterns[cycle % MAXPATTERN], bufferSize);
	} else {
		if ( cycle & 1UL ) {
			memset(buffer, 0xAA, bufferSize);
		} else {
			memset(buffer, 0x55, bufferSize);
		}
	}
}

int main(int argc, char *argv[]) 
{
	struct stat64 st;
	int rc;
	char corruptPathName[1024];
	char *cmd, ch;
	void *cacheflush;

	optind = 1;
	opterr = 1;
	cmd = argv[0];
	while ((ch = getopt_long(argc,argv,"h",longopts,NULL)) != EOF) {
		switch (ch) {
			case PathName:
				pathName = strdup(optarg);
				break;
			case LogFile:
				logFileName = strdup(optarg);
				break;
			case BufferSize:
				bufferSize = atoi(optarg);
				break;
			case FileSize:
				fileSize = atoll(optarg);
				break;
			case NbLoops:
				nbLoops = atoi(optarg);
				break;
			case SleepTime:
				sleepTime = atoi(optarg);
				break;
			case IOSleepTime:
				sleepBetweenBuffers = atoi(optarg);
				break;
			case MailTo:
				mailTo = strdup(optarg);
				break;
			case ForceCacheFlush:
				forceCacheFlush = atoi(optarg);
				break;
			case 'h':
				usage(cmd);
			default:
				break;
		}
	}
	if ( pathName == NULL ) {
		fprintf(stderr,"Please provide a directory name with --PathName!!!\n");
		usage(cmd);
		exit(1);
	}

	if ( bufferSize < 2*BUFCNTLEN ) {
		fprintf(stderr, "BufferSize must be larger than %u bytes!\n",
			2*BUFCNTLEN);
		exit(1);
	}
	if ( fileSize % bufferSize != 0 ) {
		if ( fileSize % bufferSize < 2*BUFCNTLEN ) {
			fprintf(stderr, "FileSize(%llu) %% BufferSize(%u) < 2*%u\n",
				fileSize, bufferSize, BUFCNTLEN);
			exit(1);
		}
	}

	if ( useDirectIO ) {
		if ( (bufferSize % getpagesize()) != 0 ) {
			fprintf(stderr, "BufferSize(%u) must be multiple of page size (%u) for direct I/O!\n",
				bufferSize, getpagesize() );
			exit(1);
		}
		if ( (fileSize % getpagesize()) != 0 ) {
			fprintf(stderr, "FileSize(%llu) must be multiple of page size (%u) for direct I/O!\n",
				fileSize, getpagesize() );
			exit(1);
		}
	}

	/*
	 * Check that we can use the provided path
	 */
	rc = stat64(pathName,&st);
	if ( rc == 0 ) {
		if ( !S_ISREG(st.st_mode) ) {
			fprintf(stderr,"Cannot use path %s, not a regular filename\n",pathName);
			exit(1);
		}
	}
	rc = open64(pathName,O_WRONLY|O_CREAT,0644);
	if ( rc == -1 ) {
		fprintf(stderr,"Error opening path %s: %s\n",pathName,strerror(errno));
		exit(1);
	}
	close(rc);
	
	rc = unlink(pathName);
	if ( rc == -1 ) {
		fprintf(stderr,"Error removing path %s: %s\n",pathName,strerror(errno));
		exit(1);
	}

	/*
	 * Path is OK. Initialize...
	 */
	rc = initBuffers();
	if ( rc == -1 ) exit(1);

	if ( runInForeground == 0 ) {
		rc = putInBackground();
		if ( rc == -1 ) exit(1);
	}

	if ( rndWait ) {
		srandom(1);
		sleep(random() % 30);
	}

	dumpCount = 0;
	cycle = 0;
	while ( (nbLoops == 0) || (cycle < nbLoops) ) {
		if ( ! useRndBuf ) {
			prepareBitPatternBuffer();
		}
		bufCountStartPerFile = globalBufCount;
		rc = writeFile();
		if ( rc == 0 ) {
			rc = checkFile();
			if ( rc == -2 ) {      /* diff found */
				sprintf(corruptPathName,"%s.%u",pathName,cycle);
				(void)rename(pathName,corruptPathName);
			}
		}
		(void)unlink(pathName);
		if ( forceCacheFlush ) {
			cacheflush = malloc(forceCacheFlush*1024UL*1024);
			if ( cacheflush ) {
				memset(cacheflush, 0xFF, forceCacheFlush*1024UL*1024);
				free(cacheflush);
			} else {
				perror("malloc");
			}
		}
		if ( sleepTime ) {
			sleep(sleepTime);
		}
		cycle++;
	}
	exit(0);
}

/* vim: set number nowrap: */
/* End of file. */
