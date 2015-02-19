#include "MockLogicalLibraryDatabase.hpp"

#include <gtest/gtest.h>

namespace unitTests {

class cta_client_MockLogicalLibraryDatabaseTest: public ::testing::Test {
protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(cta_client_MockLogicalLibraryDatabaseTest, createLogicalLibrary_new) {
  using namespace cta;

  MockLogicalLibraryDatabase db;
  const SecurityIdentity requester;

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }

  const std::string libraryName = "TestLogicalLibrary";
  const std::string libraryComment = "Comment";
  ASSERT_NO_THROW(db.createLogicalLibrary(requester, libraryName,
    libraryComment));

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_EQ(1, libraries.size());

    LogicalLibrary logicalLibrary;
    ASSERT_NO_THROW(logicalLibrary = libraries.front());
    ASSERT_EQ(libraryName, logicalLibrary.getName());
    ASSERT_EQ(libraryComment, logicalLibrary.getComment());
  }
}

TEST_F(cta_client_MockLogicalLibraryDatabaseTest,
  createLogicalLibrary_already_existing) {
  using namespace cta;

  MockLogicalLibraryDatabase db;
  const SecurityIdentity requester;

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }

  const std::string libraryName = "TestLogicalLibrary";
  const std::string libraryComment = "Comment";
  ASSERT_NO_THROW(db.createLogicalLibrary(requester, libraryName,
    libraryComment));

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_EQ(1, libraries.size());

    LogicalLibrary logicalLibrary;
    ASSERT_NO_THROW(logicalLibrary = libraries.front());
    ASSERT_EQ(libraryName, logicalLibrary.getName());
    ASSERT_EQ(libraryComment, logicalLibrary.getComment());
  }
  
  ASSERT_THROW(db.createLogicalLibrary(requester, libraryName,
    libraryComment), std::exception);
}

TEST_F(cta_client_MockLogicalLibraryDatabaseTest,
  createLogicalLibrary_lexicographical_order) {
  using namespace cta;

  MockLogicalLibraryDatabase db;
  const SecurityIdentity requester;

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }

  ASSERT_NO_THROW(db.createLogicalLibrary(requester, "d", "Comment d"));
  ASSERT_NO_THROW(db.createLogicalLibrary(requester, "b", "Comment b"));
  ASSERT_NO_THROW(db.createLogicalLibrary(requester, "a", "Comment a"));
  ASSERT_NO_THROW(db.createLogicalLibrary(requester, "c", "Comment c"));
  
  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_EQ(4, libraries.size());

    ASSERT_EQ(std::string("a"), libraries.front().getName());
    libraries.pop_front();
    ASSERT_EQ(std::string("b"), libraries.front().getName());
    libraries.pop_front();
    ASSERT_EQ(std::string("c"), libraries.front().getName());
    libraries.pop_front();
    ASSERT_EQ(std::string("d"), libraries.front().getName());
  }
}

TEST_F(cta_client_MockLogicalLibraryDatabaseTest, deleteLogicalLibrary_existing) {
  using namespace cta;

  MockLogicalLibraryDatabase db;
  const SecurityIdentity requester;

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }

  const std::string libraryName = "TestLogicalLibrary";
  const std::string libraryComment = "Comment";
  ASSERT_NO_THROW(db.createLogicalLibrary(requester, libraryName,
    libraryComment));

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_EQ(1, libraries.size());
  
    LogicalLibrary logicalLibrary;
    ASSERT_NO_THROW(logicalLibrary = libraries.front());
    ASSERT_EQ(libraryName, logicalLibrary.getName());
    ASSERT_EQ(libraryComment, logicalLibrary.getComment());

    ASSERT_NO_THROW(db.deleteLogicalLibrary(requester, libraryName));
  }

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }
}

TEST_F(cta_client_MockLogicalLibraryDatabaseTest,
  deleteLogicalLibrary_non_existing) {
  using namespace cta;

  MockLogicalLibraryDatabase db;
  const SecurityIdentity requester;

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }

  const std::string libraryName = "TestLogicalLibrary";
  ASSERT_THROW(db.deleteLogicalLibrary(requester, libraryName),
    std::exception);

  {
    std::list<LogicalLibrary> libraries;
    ASSERT_NO_THROW(libraries = db.getLogicalLibraries(requester));
    ASSERT_TRUE(libraries.empty());
  }
}

} // namespace unitTests
