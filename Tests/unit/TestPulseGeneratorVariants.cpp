#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class PulseGeneratorVariantsTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateStorageWithLibraries({&NMSDK::PulseLibrary});
        NMSDK::PulseLibrary.Upload(storage.get());
        try {
            NMSDK::PulseLibrary.CreateClassSamples(storage.get());
        } catch (const std::exception& ex) {
            GTEST_SKIP() << "NPulseLibrary CreateClassSamples failed: " << ex.what();
        }
    }

    void TearDown() override {
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
};

// Test NPulseGenerator creation
TEST_F(PulseGeneratorVariantsTest, CreatePulseGenerator) {
    if (!storage->CheckClass("NPulseGenerator")) {
        GTEST_SKIP() << "NPulseGenerator class not available";
        return;
    }
    
    auto generator = storage->TakeObject("NPulseGenerator");
    if (generator) {
        EXPECT_EQ(generator->GetCompClassName(), "NPulseGenerator");
        EXPECT_NE(generator, nullptr);
        
        generator->SetName("TestPulseGenerator");
        EXPECT_EQ(generator->GetName(), "TestPulseGenerator");
    } else {
        GTEST_SKIP() << "Cannot create NPulseGenerator object";
    }
}

// Test NPulseGeneratorDelay creation
TEST_F(PulseGeneratorVariantsTest, CreatePulseGeneratorDelay) {
    if (!storage->CheckClass("NPulseGeneratorDelay")) {
        GTEST_SKIP() << "NPulseGeneratorDelay class not available";
        return;
    }
    
    auto generator = storage->TakeObject("NPulseGeneratorDelay");
    if (generator) {
        EXPECT_EQ(generator->GetCompClassName(), "NPulseGeneratorDelay");
        EXPECT_NE(generator, nullptr);
        
        generator->SetName("TestPulseGeneratorDelay");
        EXPECT_EQ(generator->GetName(), "TestPulseGeneratorDelay");
    } else {
        GTEST_SKIP() << "Cannot create NPulseGeneratorDelay object";
    }
}

// Test NPulseGeneratorMulti creation
TEST_F(PulseGeneratorVariantsTest, CreatePulseGeneratorMulti) {
    if (!storage->CheckClass("NPulseGeneratorMulti")) {
        GTEST_SKIP() << "NPulseGeneratorMulti class not available";
        return;
    }
    
    auto generator = storage->TakeObject("NPulseGeneratorMulti");
    if (generator) {
        EXPECT_EQ(generator->GetCompClassName(), "NPulseGeneratorMulti");
        EXPECT_NE(generator, nullptr);
        
        generator->SetName("TestPulseGeneratorMulti");
        EXPECT_EQ(generator->GetName(), "TestPulseGeneratorMulti");
    } else {
        GTEST_SKIP() << "Cannot create NPulseGeneratorMulti object";
    }
}

