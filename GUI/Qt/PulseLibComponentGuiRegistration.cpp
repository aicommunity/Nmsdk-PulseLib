#include "../../../../Rdk/GUI/Qt/UComponentFormRegistry.h"
#include "PulseLibComponentControllerWidget.h"

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
        Q_UNUSED(id);
        Q_UNUSED(title);
        return new PulseLibComponentControllerWidget(nullptr, app);
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
