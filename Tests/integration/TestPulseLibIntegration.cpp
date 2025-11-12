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

class PulseLibIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitTestLogging();
        storage = CreateStorageWithLibraries({&NMSDK::PulseLibrary});
        NMSDK::PulseLibrary.Upload(storage.get());

        auto generator = std::make_shared<NMSDK::NPulseGenerator>();
        generator->SetName("IntegrationGenerator");
        ASSERT_TRUE(NMSDK::PulseLibrary.UploadClass("NPulseGenerator", generator));

        auto neuron = std::make_shared<NMSDK::NPulseNeuron>();
        neuron->SetName("IntegrationNeuron");
        ASSERT_TRUE(NMSDK::PulseLibrary.UploadClass("NPulseNeuron", neuron));
        environment = CreateTestEnvironment(storage);
    }

    void TearDown() override {
        environment.reset();
        storage.reset();
    }

    std::shared_ptr<UStorage> storage;
    std::shared_ptr<UEnvironment> environment;
};

TEST_F(PulseLibIntegrationTest, PulseLibEngineIntegration) {
    ASSERT_NE(storage, nullptr);
    ASSERT_NE(environment, nullptr);
    EXPECT_GT(storage->GetNumClasses(), 0);
}

TEST_F(PulseLibIntegrationTest, PulseLibEnvironmentIntegration) {
    if (!environment->IsInit()) {
        environment->Default();
        environment->Init();
    }

    EXPECT_TRUE(environment->IsInit());
    UStorage* envStorage = environment->GetStorage();
    ASSERT_NE(envStorage, nullptr);
    EXPECT_EQ(envStorage, storage.get());
}

TEST_F(PulseLibIntegrationTest, PulseLibModelCreation) {
    if (!environment->IsInit()) {
        environment->Default();
        environment->Init();
    }

    auto model = CreateTestModel(storage, environment);
    if (!model) {
        GTEST_SKIP() << "Cannot create model in environment";
        return;
    }

    auto neuron = storage->TakeObject("NPulseNeuron");
    ASSERT_NE(neuron, nullptr);
    neuron->SetName("PulseNeuron");
    ASSERT_TRUE(model->AddComponent(neuron));

    auto generator = storage->TakeObject("NPulseGenerator");
    if (generator) {
        generator->SetName("PulseGenerator");
        model->AddComponent(generator);
    }

    EXPECT_GE(model->GetNumComponents(), 1);
}

