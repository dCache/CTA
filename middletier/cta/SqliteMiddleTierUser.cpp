#include "cta/Exception.hpp"
#include "cta/SqliteMiddleTierUser.hpp"
#include "cta/Utils.hpp"

#include <iostream>
#include <memory>
#include <sstream>

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
cta::SqliteMiddleTierUser::SqliteMiddleTierUser(Vfs &vfs, SqliteDatabase &db):
  m_db(db), m_vfs(vfs) {
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cta::SqliteMiddleTierUser::~SqliteMiddleTierUser() throw() {
}

//------------------------------------------------------------------------------
// createDir
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::createDir(const SecurityIdentity &requester,
  const std::string &dirPath) {
  m_vfs.createDir(requester, dirPath, 0777);
}

//------------------------------------------------------------------------------
// deleteDir
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::deleteDir(const SecurityIdentity &requester,
  const std::string &dirPath) {
  m_vfs.deleteDir(requester, dirPath);
}

//------------------------------------------------------------------------------
// getDirContents
//------------------------------------------------------------------------------
cta::DirIterator cta::SqliteMiddleTierUser::getDirContents(
  const SecurityIdentity &requester, const std::string &dirPath) const {
  return m_vfs.getDirContents(requester, dirPath);
}

//------------------------------------------------------------------------------
// stat
//------------------------------------------------------------------------------
cta::DirEntry cta::SqliteMiddleTierUser::stat(
  const SecurityIdentity &requester,
  const std::string path) const {
  return m_vfs.statDirEntry(requester, path);
}

//------------------------------------------------------------------------------
// setDirStorageClass
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::setDirStorageClass(
  const SecurityIdentity &requester,
  const std::string &dirPath,
  const std::string &storageClassName) {
  m_vfs.setDirStorageClass(requester, dirPath, storageClassName);
}

//------------------------------------------------------------------------------
// clearDirStorageClass
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::clearDirStorageClass(
  const SecurityIdentity &requester,
  const std::string &dirPath) {
  m_vfs.clearDirStorageClass(requester, dirPath);
}
  
//------------------------------------------------------------------------------
// getDirStorageClass
//------------------------------------------------------------------------------
std::string cta::SqliteMiddleTierUser::getDirStorageClass(
  const SecurityIdentity &requester,
  const std::string &dirPath) const {
  return m_vfs.getDirStorageClass(requester, dirPath);
}

//------------------------------------------------------------------------------
// archive
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::archive(const SecurityIdentity &requester,
  const std::list<std::string> &srcUrls, const std::string &dstPath) {
  if(m_vfs.isExistingDir(requester, dstPath)) {
    return archiveToDir(requester, srcUrls, dstPath);
  } else {
    return archiveToFile(requester, srcUrls, dstPath);
  }
}

//------------------------------------------------------------------------------
// archiveToDir
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::archiveToDir(
  const SecurityIdentity &requester,
  const std::list<std::string> &srcUrls,
  const std::string &dstDir) {
  if(0 == srcUrls.size()) {
    throw Exception("At least one source file should be provided");
  }
  std::string storageClassName = m_vfs.getDirStorageClass(requester, dstDir);
  cta::StorageClass storageClass = m_db.getStorageClassByName(requester, storageClassName);
  if(storageClass.getNbCopies()==0) {       
    std::ostringstream message;
    message << "archiveToDir() - Storage class " << storageClassName << " has 0 copies";
    throw(Exception(message.str()));
  }
  for(int i=1; i<=storageClass.getNbCopies(); i++) {
    cta::ArchivalRoute route = m_db.getArchivalRouteOfStorageClass(requester, storageClassName, i);
    for(std::list<std::string>::const_iterator itor = srcUrls.begin(); itor != srcUrls.end(); itor++) {
      const std::string &srcFileName = *itor;
      std::string dstPathname;
      if(dstDir.at(dstDir.length()-1) == '/') {
        dstPathname = dstDir+srcFileName;
      }
      else {
        dstPathname = dstDir+"/"+srcFileName;
      }
      m_db.insertArchivalJob(requester, route.getTapePoolName(), srcFileName, dstPathname);
    }
  }
  
  const std::list<std::string> dstFileNames = Utils::getEnclosedNames(srcUrls);

  for(std::list<std::string>::const_iterator itor = dstFileNames.begin(); itor != dstFileNames.end(); itor++) {
    const std::string &dstFileName = *itor;
    std::string dstPathname;
    if(dstDir.at(dstDir.length()-1) == '/') {
      dstPathname = dstDir+dstFileName;
    }
    else {
      dstPathname = dstDir+"/"+dstFileName;
    }
    m_vfs.createFile(requester, dstPathname, 0666);
  }
}

//------------------------------------------------------------------------------
// archiveToFile
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::archiveToFile(
  const SecurityIdentity &requester,
  const std::list<std::string> &srcUrls,
  const std::string &dstFile) {
  if(1 != srcUrls.size()) {
    throw Exception("One and only one source file must be provided when "
      "archiving to a single destination file");
  }
  
  const std::string &srcFileName = srcUrls.front();
  std::string storageClassName = m_vfs.getDirStorageClass(requester, cta::Utils::getEnclosingDirPath(dstFile));
  cta::StorageClass storageClass = m_db.getStorageClassByName(requester, storageClassName);
  if(storageClass.getNbCopies()==0) {       
    std::ostringstream message;
    message << "archiveToFile() - Storage class " << storageClassName << " has 0 copies";
    throw(Exception(message.str()));
  }
  for(int i=1; i<=storageClass.getNbCopies(); i++) {
    cta::ArchivalRoute route = m_db.getArchivalRouteOfStorageClass(requester, storageClassName, i);
    m_db.insertArchivalJob(requester, route.getTapePoolName(), srcFileName, dstFile);
  }
  
  m_vfs.createFile(requester, dstFile, 0666);
}

//------------------------------------------------------------------------------
// getArchivalJobs
//------------------------------------------------------------------------------
std::map<cta::TapePool, std::list<cta::ArchivalJob> >
  cta::SqliteMiddleTierUser::getArchivalJobs(
  const SecurityIdentity &requester) const {
  return m_db.selectAllArchivalJobs(requester);
}

//------------------------------------------------------------------------------
// getArchivalJobs
//------------------------------------------------------------------------------
std::list<cta::ArchivalJob> cta::SqliteMiddleTierUser::getArchivalJobs(
  const SecurityIdentity &requester,
  const std::string &tapePoolName) const {
  return (m_db.selectAllArchivalJobs(requester))[m_db.getTapePoolByName(requester, tapePoolName)];
}

//------------------------------------------------------------------------------
// deleteArchivalJob
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::deleteArchivalJob(
  const SecurityIdentity &requester,
  const std::string &dstPath) {
  m_db.deleteArchivalJob(requester, dstPath);
}

//------------------------------------------------------------------------------
// retrieve
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::retrieve(
  const SecurityIdentity &requester,
  const std::list<std::string> &srcPaths,
  const std::string &dstUrl) { //we consider only the case in which dstUrl is a directory so that we accept multiple source files
  for(std::list<std::string>::const_iterator it=srcPaths.begin(); it!=srcPaths.end(); it++) {
    std::string vid = m_vfs.getVidOfFile(requester, *it, 1); //we only consider 1st copy
    m_db.insertRetrievalJob(requester, vid, *it, dstUrl);
  }
}

//------------------------------------------------------------------------------
// getRetrievalJobs
//------------------------------------------------------------------------------
std::map<cta::Tape, std::list<cta::RetrievalJob> >
  cta::SqliteMiddleTierUser::getRetrievalJobs(
  const SecurityIdentity &requester) const {
  return m_db.selectAllRetrievalJobs(requester);
}

//------------------------------------------------------------------------------
// getRetrievalJobs
//------------------------------------------------------------------------------
std::list<cta::RetrievalJob> cta::SqliteMiddleTierUser::getRetrievalJobs(
  const SecurityIdentity &requester,
  const std::string &vid) const {
  return m_db.selectAllRetrievalJobs(requester)[m_db.getTapeByVid(requester, vid)];
}

//------------------------------------------------------------------------------
// deleteRetrievalJob
//------------------------------------------------------------------------------
void cta::SqliteMiddleTierUser::deleteRetrievalJob(
  const SecurityIdentity &requester,
  const std::string &dstUrl) {
  m_db.deleteRetrievalJob(requester, dstUrl);
}
