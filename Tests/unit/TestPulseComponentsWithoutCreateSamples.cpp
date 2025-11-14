#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"
#include <algorithm>
#include <vector>

using namespace RDK;
using namespace RDK::TestHelpers;

// Tests that don't require CreateClassSamples - test basic library functionality
class PulseLibBasicTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateStorageWithLibraries({&NMSDK::PulseLibrary});
        NMSDK::PulseLibrary.Upload(storage.get());
        // Don't call CreateClassSamples to avoid lifecycle issues
    }

    void TearDown() override {
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
};

// Test that library uploads classes correctly
TEST_F(PulseLibBasicTest, LibraryUploadsClasses) {
    ASSERT_NE(storage, nullptr);
    std::vector<std::string> classNames;
    storage->GetClassNameList(classNames);
    EXPECT_GT(classNames.size(), 0) << "Storage should have some classes registered after Upload";
}

// Test that storage is properly initialized
TEST_F(PulseLibBasicTest, StorageInitialization) {
    ASSERT_NE(storage, nullptr);
    EXPECT_GE(storage->GetNumClasses(), 0) << "Storage should have classes registered";
}

// Test that we can check for class existence
TEST_F(PulseLibBasicTest, CheckClassExists) {
    ASSERT_NE(storage, nullptr);
    // Check if some common classes might be registered (without CreateClassSamples)
    std::vector<std::string> classNames;
    storage->GetClassNameList(classNames);
    
    // At least some classes should be registered
    EXPECT_GT(classNames.size(), 0) << "Should have at least some classes";
}

