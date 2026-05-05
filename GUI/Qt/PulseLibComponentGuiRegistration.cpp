#include "../../../../Rdk/GUI/Qt/UComponentFormRegistry.h"
#include "../../../../Rdk/GUI/Qt/UGenericComponentControllerWidget.h"

namespace
{
UComponentFormDescriptor MakePulseDescriptor(const QString& id, const QString& title)
{
    UComponentFormDescriptor descriptor;
    descriptor.formId = id;
    descriptor.title = title;
    descriptor.singleInstance = true;
    descriptor.factory = [id, title](RDK::UApplication* app) -> UVisualControllerWidget*
    {
        return new UGenericComponentControllerWidget(id, title, nullptr, app);
    };
    return descriptor;
}
}

void RegisterPulseLibComponentGuiForms()
{
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();
    registry.registerFormFactory("NNeuronTrainer",
                                 MakePulseDescriptor("pulse.neuron.trainer", "PulseLib: Neuron Trainer"));
    registry.registerFormFactory("NNeuronLearner",
                                 MakePulseDescriptor("pulse.neuron.learner", "PulseLib: Neuron Learner"));
}
