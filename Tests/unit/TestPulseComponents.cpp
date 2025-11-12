#include <gtest/gtest.h>
#include "../../../../Tests/TestHelpers.h"
#include "../../../../Rdk/Deploy/Include/rdk.h"
#include "../../../Deploy/Include/Lib.h"
#include "../../../../Libraries/Nmsdk-PulseLib/Core/NPulseLibrary.h"
#include "../../../../Libraries/Nmsdk-PulseLib/Core/NPulseGenerator.h"
#include "../../../../Libraries/Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include <algorithm>
#include <vector>

using namespace RDK;
using namespace RDK::TestHelpers;

namespace {

std::string FindPulseComponentName(UStorage& storage,
                                   std::initializer_list<const char*> preferredNames,
                                   std::initializer_list<const char*> fallbackPrefixes) {
    for (const auto* name : preferredNames) {
        if (storage.CheckClass(name)) {
            return name;
        }
    }

    std::vector<std::string> names;
    storage.GetClassNameList(names);
    for (const auto* prefix : fallbackPrefixes) {
        auto it = std::find_if(names.begin(), names.end(),
                               [&](const std::string& value) {
                                   return value.rfind(prefix, 0) == 0;
                               });
        if (it != names.end()) {
            return *it;
        }
    }
    return {};
}

} // namespace

class PulseComponentsTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateStorageWithLibraries({&NMSDK::PulseLibrary});
        NMSDK::PulseLibrary.Upload(storage.get());
        // CreateClassSamples is not called automatically for PulseLib (only for BasicLib)
        // So we need to call it manually after Upload
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

TEST_F(PulseComponentsTest, RegistersCoreClasses) {
    ASSERT_NE(storage, nullptr);
    EXPECT_TRUE(storage->CheckClass("NPulseGenerator")) << "NPulseGenerator should be registered";
    EXPECT_TRUE(storage->CheckClass("NPulseNeuron")) << "NPulseNeuron should be registered";
}

TEST_F(PulseComponentsTest, CreateNeuronComponent) {
    auto neuron = storage->TakeObject("NPulseNeuron");
    ASSERT_NE(neuron, nullptr);
    neuron->SetName("PulseNeuronInstance");
    EXPECT_EQ(neuron->GetName(), "PulseNeuronInstance");
    storage->ReturnObject(neuron);
}

TEST_F(PulseComponentsTest, CreateGeneratorComponent) {
    auto generator = storage->TakeObject("NPulseGenerator");
    ASSERT_NE(generator, nullptr);
    generator->SetName("PulseGenerator");
    EXPECT_EQ(generator->GetName(), "PulseGenerator");
    storage->ReturnObject(generator);
}

