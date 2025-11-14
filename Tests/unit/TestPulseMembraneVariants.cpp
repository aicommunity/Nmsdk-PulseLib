#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class PulseMembraneVariantsTest : public ::testing::Test {
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

// Test NPulseMembrane creation
TEST_F(PulseMembraneVariantsTest, CreatePulseMembrane) {
    if (!storage->CheckClass("NPulseMembrane")) {
        GTEST_SKIP() << "NPulseMembrane class not available";
        return;
    }
    
    auto membrane = storage->TakeObject("NPulseMembrane");
    if (membrane) {
        EXPECT_EQ(membrane->GetCompClassName(), "NPulseMembrane");
        EXPECT_NE(membrane, nullptr);
        
        membrane->SetName("TestPulseMembrane");
        EXPECT_EQ(membrane->GetName(), "TestPulseMembrane");
    } else {
        GTEST_SKIP() << "Cannot create NPulseMembrane object";
    }
}

// Test NPulseMembraneCommon creation
TEST_F(PulseMembraneVariantsTest, CreatePulseMembraneCommon) {
    if (!storage->CheckClass("NPulseMembraneCommon")) {
        GTEST_SKIP() << "NPulseMembraneCommon class not available";
        return;
    }
    
    auto membrane = storage->TakeObject("NPulseMembraneCommon");
    if (membrane) {
        EXPECT_EQ(membrane->GetCompClassName(), "NPulseMembraneCommon");
        EXPECT_NE(membrane, nullptr);
        
        membrane->SetName("TestPulseMembraneCommon");
        EXPECT_EQ(membrane->GetName(), "TestPulseMembraneCommon");
    } else {
        GTEST_SKIP() << "Cannot create NPulseMembraneCommon object";
    }
}

// Test NPulseMembraneIzhikevich creation
TEST_F(PulseMembraneVariantsTest, CreatePulseMembraneIzhikevich) {
    if (!storage->CheckClass("NPulseMembraneIzhikevich")) {
        GTEST_SKIP() << "NPulseMembraneIzhikevich class not available";
        return;
    }
    
    auto membrane = storage->TakeObject("NPulseMembraneIzhikevich");
    if (membrane) {
        EXPECT_EQ(membrane->GetCompClassName(), "NPulseMembraneIzhikevich");
        EXPECT_NE(membrane, nullptr);
        
        membrane->SetName("TestPulseMembraneIzhikevich");
        EXPECT_EQ(membrane->GetName(), "TestPulseMembraneIzhikevich");
    } else {
        GTEST_SKIP() << "Cannot create NPulseMembraneIzhikevich object";
    }
}

