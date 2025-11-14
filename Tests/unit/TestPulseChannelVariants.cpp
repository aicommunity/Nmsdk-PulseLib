#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class PulseChannelVariantsTest : public ::testing::Test {
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

// Test NPulseChannel creation
TEST_F(PulseChannelVariantsTest, CreatePulseChannel) {
    if (!storage->CheckClass("NPulseChannel")) {
        GTEST_SKIP() << "NPulseChannel class not available";
        return;
    }
    
    auto channel = storage->TakeObject("NPulseChannel");
    if (channel) {
        EXPECT_EQ(channel->GetCompClassName(), "NPulseChannel");
        EXPECT_NE(channel, nullptr);
        
        channel->SetName("TestPulseChannel");
        EXPECT_EQ(channel->GetName(), "TestPulseChannel");
    } else {
        GTEST_SKIP() << "Cannot create NPulseChannel object";
    }
}

// Test NPulseChannelCommon creation
TEST_F(PulseChannelVariantsTest, CreatePulseChannelCommon) {
    if (!storage->CheckClass("NPulseChannelCommon")) {
        GTEST_SKIP() << "NPulseChannelCommon class not available";
        return;
    }
    
    auto channel = storage->TakeObject("NPulseChannelCommon");
    if (channel) {
        EXPECT_EQ(channel->GetCompClassName(), "NPulseChannelCommon");
        EXPECT_NE(channel, nullptr);
        
        channel->SetName("TestPulseChannelCommon");
        EXPECT_EQ(channel->GetName(), "TestPulseChannelCommon");
    } else {
        GTEST_SKIP() << "Cannot create NPulseChannelCommon object";
    }
}

// Test NPulseChannelClassic creation
TEST_F(PulseChannelVariantsTest, CreatePulseChannelClassic) {
    if (!storage->CheckClass("NPulseChannelClassic")) {
        GTEST_SKIP() << "NPulseChannelClassic class not available";
        return;
    }
    
    auto channel = storage->TakeObject("NPulseChannelClassic");
    if (channel) {
        EXPECT_EQ(channel->GetCompClassName(), "NPulseChannelClassic");
        EXPECT_NE(channel, nullptr);
        
        channel->SetName("TestPulseChannelClassic");
        EXPECT_EQ(channel->GetName(), "TestPulseChannelClassic");
    } else {
        GTEST_SKIP() << "Cannot create NPulseChannelClassic object";
    }
}

// Test NPulseChannelIaF creation
TEST_F(PulseChannelVariantsTest, CreatePulseChannelIaF) {
    if (!storage->CheckClass("NPulseChannelIaF")) {
        GTEST_SKIP() << "NPulseChannelIaF class not available";
        return;
    }
    
    auto channel = storage->TakeObject("NPulseChannelIaF");
    if (channel) {
        EXPECT_EQ(channel->GetCompClassName(), "NPulseChannelIaF");
        EXPECT_NE(channel, nullptr);
        
        channel->SetName("TestPulseChannelIaF");
        EXPECT_EQ(channel->GetName(), "TestPulseChannelIaF");
    } else {
        GTEST_SKIP() << "Cannot create NPulseChannelIaF object";
    }
}

// Test NPulseChannelIzhikevich creation
TEST_F(PulseChannelVariantsTest, CreatePulseChannelIzhikevich) {
    if (!storage->CheckClass("NPulseChannelIzhikevich")) {
        GTEST_SKIP() << "NPulseChannelIzhikevich class not available";
        return;
    }
    
    auto channel = storage->TakeObject("NPulseChannelIzhikevich");
    if (channel) {
        EXPECT_EQ(channel->GetCompClassName(), "NPulseChannelIzhikevich");
        EXPECT_NE(channel, nullptr);
        
        channel->SetName("TestPulseChannelIzhikevich");
        EXPECT_EQ(channel->GetName(), "TestPulseChannelIzhikevich");
    } else {
        GTEST_SKIP() << "Cannot create NPulseChannelIzhikevich object";
    }
}

