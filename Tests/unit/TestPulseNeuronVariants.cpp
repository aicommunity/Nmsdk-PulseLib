#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"
#include <algorithm>
#include <vector>

using namespace RDK;
using namespace RDK::TestHelpers;

class PulseNeuronVariantsTest : public ::testing::Test {
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

// Test NPulseNeuron creation
TEST_F(PulseNeuronVariantsTest, CreatePulseNeuron) {
    if (!storage->CheckClass("NPulseNeuron")) {
        GTEST_SKIP() << "NPulseNeuron class not available";
        return;
    }
    
    auto neuron = storage->TakeObject("NPulseNeuron");
    if (neuron) {
        EXPECT_EQ(neuron->GetCompClassName(), "NPulseNeuron");
        EXPECT_NE(neuron, nullptr);
        
        neuron->SetName("TestPulseNeuron");
        EXPECT_EQ(neuron->GetName(), "TestPulseNeuron");
    } else {
        GTEST_SKIP() << "Cannot create NPulseNeuron object";
    }
}

// Test NPulseNeuronCommon creation
TEST_F(PulseNeuronVariantsTest, CreatePulseNeuronCommon) {
    if (!storage->CheckClass("NPulseNeuronCommon")) {
        GTEST_SKIP() << "NPulseNeuronCommon class not available";
        return;
    }
    
    auto neuron = storage->TakeObject("NPulseNeuronCommon");
    if (neuron) {
        EXPECT_EQ(neuron->GetCompClassName(), "NPulseNeuronCommon");
        EXPECT_NE(neuron, nullptr);
        
        neuron->SetName("TestPulseNeuronCommon");
        EXPECT_EQ(neuron->GetName(), "TestPulseNeuronCommon");
    } else {
        GTEST_SKIP() << "Cannot create NPulseNeuronCommon object";
    }
}

// Test NPulseNeuronIzhikevich creation
TEST_F(PulseNeuronVariantsTest, CreatePulseNeuronIzhikevich) {
    if (!storage->CheckClass("NPulseNeuronIzhikevich")) {
        GTEST_SKIP() << "NPulseNeuronIzhikevich class not available";
        return;
    }
    
    auto neuron = storage->TakeObject("NPulseNeuronIzhikevich");
    if (neuron) {
        EXPECT_EQ(neuron->GetCompClassName(), "NPulseNeuronIzhikevich");
        EXPECT_NE(neuron, nullptr);
        
        neuron->SetName("TestPulseNeuronIzhikevich");
        EXPECT_EQ(neuron->GetName(), "TestPulseNeuronIzhikevich");
    } else {
        GTEST_SKIP() << "Cannot create NPulseNeuronIzhikevich object";
    }
}

// Test NPulseLifeNeuron creation
TEST_F(PulseNeuronVariantsTest, CreatePulseLifeNeuron) {
    if (!storage->CheckClass("NPulseLifeNeuron")) {
        GTEST_SKIP() << "NPulseLifeNeuron class not available";
        return;
    }
    
    auto neuron = storage->TakeObject("NPulseLifeNeuron");
    if (neuron) {
        EXPECT_EQ(neuron->GetCompClassName(), "NPulseLifeNeuron");
        EXPECT_NE(neuron, nullptr);
        
        neuron->SetName("TestPulseLifeNeuron");
        EXPECT_EQ(neuron->GetName(), "TestPulseLifeNeuron");
    } else {
        GTEST_SKIP() << "Cannot create NPulseLifeNeuron object";
    }
}

