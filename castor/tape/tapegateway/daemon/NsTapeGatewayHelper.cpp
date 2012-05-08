/******************************************************************************
 *                      NsTapeGatewayHelper.cpp
 *
 * This file is part of the Castor project.
 * See http://castor.web.cern.ch/castor
 *
 * Copyright (C) 2003  CERN
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
 * @(#)$RCSfile: NsTapeGatewayHelper.cpp,v $ $Revision: 1.15 $ $Release$ 
 * $Date: 2009/08/10 22:07:12 $ $Author: murrayc3 $
 *
 *
 *
 * @author Castor Dev team, castor-dev@cern.ch
 *****************************************************************************/

#include <common.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "Cns_api.h"
#include "rfio_api.h"

#include "castor/exception/OutOfMemory.hpp"
#include "castor/exception/Internal.hpp"

#include "castor/tape/tapegateway/PositionCommandCode.hpp"

#include "castor/tape/tapegateway/daemon/NsTapeGatewayHelper.hpp"


void castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile
(tape::tapegateway::FileMigratedNotification& file, int copyNumber, std::string vid,
 u_signed64 lastModificationTime) throw (castor::exception::Exception){ 
  // fill in the castor file id structure
  int nbSegs=0;
  struct Cns_segattrs *nsSegAttrs = NULL;
  struct Cns_fileid castorFileId;
  memset(&castorFileId,'\0',sizeof(castorFileId));
  strncpy(
          castorFileId.server,
          file.nshost().c_str(),
          sizeof(castorFileId.server)-1
          );
  castorFileId.fileid = file.fileid();

  // let's get the number of copies allowed
  char castorFileName[CA_MAXPATHLEN+1];
  struct Cns_filestat statbuf;
  memset(&statbuf,'\0',sizeof(statbuf));
  *castorFileName = '\0';
  
  serrno=0;
  int rc = Cns_statx(castorFileName,&castorFileId,&statbuf);
  if ( rc == -1 ) {
    castor::exception::Exception ex(serrno);
    ex.getMessage()
      << "castor::tape::tapegateway::NsTapeGatewayHelper::checkMigratedFile:"
      << "impossible to stat the file";
    throw ex;
  }
  // get segments for this fileid
  serrno=0;
  rc = Cns_getsegattrs(NULL,&castorFileId,&nbSegs,&nsSegAttrs);
  if ( rc == -1 ) {
    free(nsSegAttrs);
    castor::exception::Exception ex(serrno);
    ex.getMessage()
	    << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
	    << "impossible to get the file";
    throw ex;
  }
  // check we are not migrating a new copy to the same tape as another one
  // (overwriting the same copy number of course is not an issue)
  for ( int i=0; i< nbSegs ;i++) {
    if ( nsSegAttrs[i].copyno != copyNumber) {
      if ( strcmp(nsSegAttrs[i].vid , vid.c_str()) == 0 ) { 
        free(nsSegAttrs);
        castor::exception::Exception ex(EEXIST);
        ex.getMessage()
	          << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
	          << "this file have already a copy on that tape";
        throw ex;
      }
    }
  }
  free(nsSegAttrs);
  // fill in the information of the tape file to update the nameserver
  nbSegs = 1; // NEVER more than one segment
  nsSegAttrs = (struct Cns_segattrs *)calloc(
                                             nbSegs,
                                             sizeof(struct Cns_segattrs)
                                             );
  if ( !nsSegAttrs ) {
    castor::exception::Exception ex(-1);
    ex.getMessage()
      << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
      << "calloc failed";
    throw ex;

  } 
  nsSegAttrs->copyno = copyNumber; 
  nsSegAttrs->fsec = 1; // always one
  u_signed64 nsSize=file.fileSize();
  nsSegAttrs->segsize = nsSize;
  u_signed64 compression = (file.fileSize() * 100 )/ file.compressedFileSize();
  nsSegAttrs->compression = compression;
  strncpy(nsSegAttrs->vid, vid.c_str(),CA_MAXVIDLEN);
  nsSegAttrs->side = 0; //HARDCODED
  nsSegAttrs->s_status = '-';
  //  convert the blockid
  nsSegAttrs->blockid[3]=file.blockId3(); 
  nsSegAttrs->blockid[2]=file.blockId2();
  nsSegAttrs->blockid[1]=file.blockId1();
  nsSegAttrs->blockid[0]=file.blockId0();
  nsSegAttrs->fseq = file.fseq();
  strncpy(
	  nsSegAttrs->checksum_name,
	  file.checksumName().c_str(),
	  CA_MAXCKSUMNAMELEN
	  );
  nsSegAttrs->checksum = file.checksum();
  int save_serrno=0;
  serrno=0;
  rc = Cns_setsegattrs(
		       (char *)NULL, // CASTOR file name 
		       &castorFileId,
		       nbSegs,
		       nsSegAttrs,
		       lastModificationTime
		       );
  free(nsSegAttrs); nsSegAttrs=NULL;
  if (rc ==0 ) { 
    //update fine
    return;
  }
  save_serrno = serrno; 
  switch ( save_serrno ) {
    case ENOENT:
    // check if the file is still there
    {
      char *castorFileName = '\0';
      struct Cns_filestat statbuf;
      rc = Cns_statx(castorFileName,&castorFileId,&statbuf);
      if (rc==0) {
        //the file is there the update failed
        castor::exception::Exception ex(EINVAL);
        ex.getMessage()
	    << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
	    << "false enoent received the update failed";
        throw ex;
      } else {
        // the file is not there the update is considered successfull
        return;
      }
    }
    break;

    case ENSCLASSNOSEGS:
    case ENSTOOMANYSEGS:
      /* We ignore ENSCLASSNOSEGS. This means that the segment created belongs
       * to a file that should not be on tape. Thus migration can be considered
       * as completed.
       * We ignore ENSTOOMANYSEGS. This means that the segment created belongs
       * to a file that already has enough copy on tape. So this segment is one too
       * much. Thus migration can be considered as completed. */
    {
      SuperfluousSegmentException ex(save_serrno);
      ex.getMessage()
        << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
        << "name server determined this segment was not necessary.";
      throw ex;
    }
      break;

    default:
     // all the other possible serrno cause the failure
    {
      castor::exception::Exception ex(save_serrno);
      ex.getMessage()
        << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
        << "Cns_setsegattrs failed";
      throw ex;
    }
  }
  // We should never get here (all case should be covered above).
  castor::exception::Internal ex;
  ex.getMessage()<< "Internal error: Uncovered case in castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
      "save_errno=" << save_serrno;
  throw ex;
}

void castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile(
    tape::tapegateway::FileMigratedNotification& file,
    int originalCopyNumber, std::string originalVid,
    int copyNumber, std::string vid, u_signed64 lastModificationTime)
  throw (castor::exception::Exception) {
  // fill in the castor file id structure
  struct Cns_fileid castorFileId;
  memset(&castorFileId,'\0',sizeof(castorFileId));
  strncpy(castorFileId.server,
          file.nshost().c_str(),
          sizeof(castorFileId.server)-1);
  castorFileId.fileid = file.fileid();

  // fill in the information of the tape file to update the nameserver
  struct Cns_segattrs nsSegAttrs;
  memset (&nsSegAttrs, '\0', sizeof (struct Cns_segattrs));

  nsSegAttrs.copyno = copyNumber;
  nsSegAttrs.fsec = 1; // always one
  u_signed64 compression = (file.fileSize() * 100 )/ file.compressedFileSize();
  nsSegAttrs.compression = compression;
  nsSegAttrs.segsize = file.fileSize();
  nsSegAttrs.s_status = '-';
  
  strncpy(nsSegAttrs.vid,vid.c_str(),CA_MAXVIDLEN);
  nsSegAttrs.vid[CA_MAXVIDLEN]='\0';
  nsSegAttrs.side = 0; // HARDCODED side
  //  convert the blockid
  nsSegAttrs.blockid[3]=file.blockId3();
  nsSegAttrs.blockid[2]=file.blockId2();
  nsSegAttrs.blockid[1]=file.blockId1();
  nsSegAttrs.blockid[0]=file.blockId0();
  nsSegAttrs.fseq = file.fseq();
  strncpy(nsSegAttrs.checksum_name,
          file.checksumName().c_str(),
          CA_MAXCKSUMNAMELEN);
  nsSegAttrs.checksum = file.checksum();
 
  // we are in repack case so the copy should be replaced
  int oldNbSegms = 0;
  struct Cns_segattrs *oldSegattrs = NULL;
  // get the old copy with the segments associated
  serrno = 0;
  int rc = Cns_getsegattrs( NULL, (struct Cns_fileid *)&castorFileId, &oldNbSegms, &oldSegattrs);
  if ( (rc == -1) || (oldNbSegms <= 0) || (oldSegattrs == NULL) )  {
    castor::exception::Exception ex(serrno);
    ex.getMessage()
      << "castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile:"
      << "impossible to get the old segment";
    if (oldSegattrs) free (oldSegattrs);
    throw ex;
  }

  // get the file information
  char castorFileName[CA_MAXPATHLEN+1]; *castorFileName = '\0';
  struct Cns_filestatcs nsFileAttrs;
  memset(&nsFileAttrs,'\0',sizeof(nsFileAttrs));
  serrno=0;
  rc = Cns_statcsx(castorFileName,&castorFileId,&nsFileAttrs);
  if ( rc == -1 ) {
    NoSuchFileException ex;
    ex.getMessage()
      << "castor::tape::tapegateway::NsTapeGatewayHelper::checkRepackedFile:"
      << "impossible to stat the file: rc=-1, serrno=" << serrno;
    throw ex;
  }
      
  // Check if there is already a copy of this file in the new tape.
  // We don't want the new copy to be on this tape in any case:
  // we would be migrating a second copy to the same tape (forbidden)
  // or repacking a tape to itself (this should not happen by construction
  // but we would block it at that level).
  for ( int i=0; i< oldNbSegms ;i++) {
    if ( strcmp(oldSegattrs[i].vid , vid.c_str()) == 0 ) {
      if (oldSegattrs) free(oldSegattrs);
      castor::exception::Exception ex(EEXIST);
      ex.getMessage()
	      << "castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile:"
	      << "this file have already a copy on that tape";
      throw ex;
    }
  }

  bool copyToOverwriteFound=false;
  // We have all the segments and copies of that file
  // We check that the copy we want to replace exists
  for(int i = 0; i < oldNbSegms; i++) {
    if (!strcmp(oldSegattrs[i].vid, originalVid.c_str()) &&
        oldSegattrs[i].copyno == originalCopyNumber) {
      // it is tape copy
      copyToOverwriteFound=true;
      break;
    }
  }
 
  if (!copyToOverwriteFound) {
    // The copy we expected to replace can't be found. We can't proceed further
    // with repack in any case. This situation can be a harmless update/removal
    // of the file during repack, or an internal problem.
    // We confirm the positive case and finish with this outdated request.
    // We return the conclusion to the caller in the form of an exception
    if (oldSegattrs) free(oldSegattrs);
    if (nsFileAttrs.mtime != (time_t)lastModificationTime) {
      FileMutatedException ex;
      ex.getMessage()
          << "castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile:"
          << " segment not found: file has changed.";
      throw ex;
    } else {
      FileMutationUnconfirmedException ex;
      ex.getMessage()
          << "castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile:"
          << " segment not found: file change not confirmed.";
      throw ex;
    }
  }

  // check the checksum
  if (oldSegattrs->checksum != nsSegAttrs.checksum) {
    free(oldSegattrs);
    oldSegattrs = NULL;
    castor::exception::Exception ex(SECHECKSUM);
    ex.getMessage()
      << "castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile:"
      << " invalid checksum";
    throw ex; 
  }  

  // replace the tapecopy: only one at a time
  rc = Cns_replaceormovetapecopy(&castorFileId, originalVid.c_str(), originalCopyNumber,
                                 &nsSegAttrs, lastModificationTime);
  if (oldSegattrs) free(oldSegattrs);
  oldSegattrs = NULL;
  if (rc<0) {
    int save_serrno = serrno;
    switch (save_serrno) {
      case ENSCLASSNOSEGS:
      case ENSTOOMANYSEGS:
        /* We ignore ENSCLASSNOSEGS. This means that the segment created belongs
         * to a file that should not be on tape. Thus migration can be considered
         * as completed.
         * We ignore ENSTOOMANYSEGS. This means that the segment created belongs
         * to a file that already has enough copy on tape. So this segment is one too
         * much. Thus migration can be considered as completed. */
      {
        SuperfluousSegmentException ex (save_serrno);
        ex.getMessage()
              << "castor::tape::tapegateway::NsTapeGatewayHelper::updateMigratedFile:"
              << "name server determined this segment was not necessary.";
        throw ex;
      }
        break;

      default:
      {
        castor::exception::Exception ex(serrno);
        ex.getMessage()
              << "castor::tape::tapegateway::NsTapeGatewayHelper::updateRepackedFile:"
              << " impossible to replace tapecopy originalcopynb=" << originalCopyNumber
              << " originalvid=" <<  originalVid << " newcopynb=" <<  copyNumber
              << " newvid=" << vid << " rc=" << rc;
        throw ex;
      }
        break;
    }
  }
}

// This checker function will raise an exception if the Fseq forseen for writing
// is not strictly greater than the highest know Fseq for this tape in the name
// server (meaning an overwrite).
void castor::tape::tapegateway::NsTapeGatewayHelper::checkFseqForWrite (const std::string &vid, int Fseq)
     throw (castor::exception::Exception) {
  struct Cns_segattrs segattrs;
  memset (&segattrs, 0, sizeof(struct Cns_segattrs));
  int rc = Cns_lastfseq (vid.c_str(), 0, &segattrs); // side = 0 hardcoded
  // Read serrno only once as it points at a function hidden behind a macro.
  int save_serrno = serrno;
  // If the name server does not know about the tape, we're safe (return, done).
  if ((-1 == rc) && (ENOENT == save_serrno)) return;
  if (rc != 0) { // Failure to contact the name server and all other errors.
    castor::exception::Exception ex(save_serrno);
    ex.getMessage()
      << "castor::tape::tapegateway::NsTapeGatewayHelper::checkFseqForWrite:"
      << "Cns_lastfseq failed for VID="<<vid.c_str()<< " side="<< 0 << " : rc="
      << rc << " serrno =" << save_serrno << "(" << sstrerror(save_serrno) << ")";
    throw ex;
  }
  if (Fseq <= segattrs.fseq) { // Major error, we are about to overwrite an fseq
    // referenced in the name server.
    castor::exception::Exception ex(ERTWRONGFSEQ);
    ex.getMessage()
          << "castor::tape::tapegateway::NsTapeGatewayHelper::checkFseqForWrite:"
          << "Fseq check failed for VID="<<vid.c_str()<< " side="<< 0 << " Fseq="
          << Fseq << " last referenced segment in NS=" << segattrs.fseq;
    throw ex;
  }
}

