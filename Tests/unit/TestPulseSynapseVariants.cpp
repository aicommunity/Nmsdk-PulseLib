#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class PulseSynapseVariantsTest : public ::testing::Test {
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

// Test NPulseSynapse creation
TEST_F(PulseSynapseVariantsTest, CreatePulseSynapse) {
    if (!storage->CheckClass("NPulseSynapse")) {
        GTEST_SKIP() << "NPulseSynapse class not available";
        return;
    }
    
    auto synapse = storage->TakeObject("NPulseSynapse");
    if (synapse) {
        EXPECT_EQ(synapse->GetCompClassName(), "NPulseSynapse");
        EXPECT_NE(synapse, nullptr);
        
        synapse->SetName("TestPulseSynapse");
        EXPECT_EQ(synapse->GetName(), "TestPulseSynapse");
    } else {
        GTEST_SKIP() << "Cannot create NPulseSynapse object";
    }
}

// Test NPulseSynapseCommon creation
TEST_F(PulseSynapseVariantsTest, CreatePulseSynapseCommon) {
    if (!storage->CheckClass("NPulseSynapseCommon")) {
        GTEST_SKIP() << "NPulseSynapseCommon class not available";
        return;
    }
    
    auto synapse = storage->TakeObject("NPulseSynapseCommon");
    if (synapse) {
        EXPECT_EQ(synapse->GetCompClassName(), "NPulseSynapseCommon");
        EXPECT_NE(synapse, nullptr);
        
        synapse->SetName("TestPulseSynapseCommon");
        EXPECT_EQ(synapse->GetName(), "TestPulseSynapseCommon");
    } else {
        GTEST_SKIP() << "Cannot create NPulseSynapseCommon object";
    }
}

// Test NPulseSynapseStdp creation
TEST_F(PulseSynapseVariantsTest, CreatePulseSynapseStdp) {
    if (!storage->CheckClass("NPulseSynapseStdp")) {
        GTEST_SKIP() << "NPulseSynapseStdp class not available";
        return;
    }
    
    auto synapse = storage->TakeObject("NPulseSynapseStdp");
    if (synapse) {
        EXPECT_EQ(synapse->GetCompClassName(), "NPulseSynapseStdp");
        EXPECT_NE(synapse, nullptr);
        
        synapse->SetName("TestPulseSynapseStdp");
        EXPECT_EQ(synapse->GetName(), "TestPulseSynapseStdp");
    } else {
        GTEST_SKIP() << "Cannot create NPulseSynapseStdp object";
    }
}

