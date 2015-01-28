#include "MockAPI.hpp"

#include <gtest/gtest.h>

namespace unitTests {

class cta_client_MockAPITest: public ::testing::Test {
protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(cta_client_MockAPITest, createStorageClass_new) {
  using namespace cta;

  MockAPI api;

  ASSERT_TRUE(api.getStorageClasses().empty());

  const std::string name = "TestStorageClass";
  const uint8_t nbCopies = 2;
  ASSERT_NO_THROW(api.createStorageClass(name, nbCopies));

  ASSERT_EQ(1, api.getStorageClasses().size());
  cta::StorageClass storageClass;
  ASSERT_NO_THROW(storageClass = api.getStorageClasses().front());
  ASSERT_EQ(name, storageClass.name);
  ASSERT_EQ(nbCopies, storageClass.nbCopies);
}

TEST_F(cta_client_MockAPITest, createStorageClass_already_existing) {
  using namespace cta;

  MockAPI api;

  ASSERT_TRUE(api.getStorageClasses().empty());
  
  const std::string name = "TestStorageClass";
  const uint8_t nbCopies = 2;
  ASSERT_NO_THROW(api.createStorageClass(name, nbCopies));
  
  ASSERT_EQ(1, api.getStorageClasses().size());
  cta::StorageClass storageClass;
  ASSERT_NO_THROW(storageClass = api.getStorageClasses().front());
  ASSERT_EQ(name, storageClass.name);
  ASSERT_EQ(nbCopies, storageClass.nbCopies);

  ASSERT_THROW(api.createStorageClass(name, nbCopies), std::exception);
}

TEST_F(cta_client_MockAPITest, createStorageClass_lexicographical_order) {
  using namespace cta;

  MockAPI api;

  ASSERT_TRUE(api.getStorageClasses().empty());
  
  ASSERT_NO_THROW(api.createStorageClass("d", 1));
  ASSERT_NO_THROW(api.createStorageClass("b", 1));
  ASSERT_NO_THROW(api.createStorageClass("a", 1));
  ASSERT_NO_THROW(api.createStorageClass("c", 1));
  
  ASSERT_EQ(4, api.getStorageClasses().size());

  cta::StorageClassList storageClasses = api.getStorageClasses();

  ASSERT_EQ(std::string("a"), storageClasses.front().name);
  storageClasses.pop_front();
  ASSERT_EQ(std::string("b"), storageClasses.front().name);
  storageClasses.pop_front();
  ASSERT_EQ(std::string("c"), storageClasses.front().name);
  storageClasses.pop_front();
  ASSERT_EQ(std::string("d"), storageClasses.front().name);
}

TEST_F(cta_client_MockAPITest, deleteStorageClass_existing) {
  using namespace cta;

  MockAPI api;

  ASSERT_TRUE(api.getStorageClasses().empty());
  
  const std::string name = "TestStorageClass";
  const uint8_t nbCopies = 2;
  ASSERT_NO_THROW(api.createStorageClass(name, nbCopies));
  
  ASSERT_EQ(1, api.getStorageClasses().size());
  cta::StorageClass storageClass;
  ASSERT_NO_THROW(storageClass = api.getStorageClasses().front());
  ASSERT_EQ(name, storageClass.name);
  ASSERT_EQ(nbCopies, storageClass.nbCopies);

  ASSERT_NO_THROW(api.deleteStorageClass(name));

  ASSERT_TRUE(api.getStorageClasses().empty());
}

TEST_F(cta_client_MockAPITest, deleteStorageClass_non_existing) {
  using namespace cta;

  MockAPI api;

  ASSERT_TRUE(api.getStorageClasses().empty());

  const std::string name = "TestStorageClass";
  ASSERT_THROW(api.deleteStorageClass(name), std::exception);

  ASSERT_TRUE(api.getStorageClasses().empty());
}

TEST_F(cta_client_MockAPITest, getDirectoryIterator_empty) {
  using namespace cta;

  MockAPI api;

  ASSERT_NO_THROW(api.getDirectoryIterator("/"));
}

} // namespace unitTests
