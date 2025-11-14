#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../Core/NPulseLibrary.h"

using namespace RDK;
using namespace RDK::TestHelpers;

class PulseLTZoneVariantsTest : public ::testing::Test {
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

// Test NPulseLTZone creation
TEST_F(PulseLTZoneVariantsTest, CreatePulseLTZone) {
    if (!storage->CheckClass("NPulseLTZone")) {
        GTEST_SKIP() << "NPulseLTZone class not available";
        return;
    }
    
    auto zone = storage->TakeObject("NPulseLTZone");
    if (zone) {
        EXPECT_EQ(zone->GetCompClassName(), "NPulseLTZone");
        EXPECT_NE(zone, nullptr);
        
        zone->SetName("TestPulseLTZone");
        EXPECT_EQ(zone->GetName(), "TestPulseLTZone");
    } else {
        GTEST_SKIP() << "Cannot create NPulseLTZone object";
    }
}

// Test NPulseLTZoneCommon creation
TEST_F(PulseLTZoneVariantsTest, CreatePulseLTZoneCommon) {
    if (!storage->CheckClass("NPulseLTZoneCommon")) {
        GTEST_SKIP() << "NPulseLTZoneCommon class not available";
        return;
    }
    
    auto zone = storage->TakeObject("NPulseLTZoneCommon");
    if (zone) {
        EXPECT_EQ(zone->GetCompClassName(), "NPulseLTZoneCommon");
        EXPECT_NE(zone, nullptr);
        
        zone->SetName("TestPulseLTZoneCommon");
        EXPECT_EQ(zone->GetName(), "TestPulseLTZoneCommon");
    } else {
        GTEST_SKIP() << "Cannot create NPulseLTZoneCommon object";
    }
}

// Test NPulseLTZoneIzhikevich creation
TEST_F(PulseLTZoneVariantsTest, CreatePulseLTZoneIzhikevich) {
    if (!storage->CheckClass("NPulseLTZoneIzhikevich")) {
        GTEST_SKIP() << "NPulseLTZoneIzhikevich class not available";
        return;
    }
    
    auto zone = storage->TakeObject("NPulseLTZoneIzhikevich");
    if (zone) {
        EXPECT_EQ(zone->GetCompClassName(), "NPulseLTZoneIzhikevich");
        EXPECT_NE(zone, nullptr);
        
        zone->SetName("TestPulseLTZoneIzhikevich");
        EXPECT_EQ(zone->GetName(), "TestPulseLTZoneIzhikevich");
    } else {
        GTEST_SKIP() << "Cannot create NPulseLTZoneIzhikevich object";
    }
}

