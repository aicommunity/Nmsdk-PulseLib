#include "../../../../Rdk/GUI/Qt/UComponentFormRegistry.h"
#include "PulseLibComponentControllerWidget.h"
#include "PulseLibNeuronLearnerComponentControllerWidget.h"
#include "PulseLibNeuronTrainerComponentControllerWidget.h"

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

UComponentFormDescriptor MakePulseNeuronTrainerDescriptor(const QString& id, const QString& title)
{
    UComponentFormDescriptor descriptor;
    descriptor.formId = id;
    descriptor.title = title;
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget*
    {
        return new PulseLibNeuronTrainerComponentControllerWidget(nullptr, app);
    };
    return descriptor;
}

UComponentFormDescriptor MakePulseNeuronLearnerDescriptor(const QString& id, const QString& title)
{
    UComponentFormDescriptor descriptor;
    descriptor.formId = id;
    descriptor.title = title;
    descriptor.singleInstance = true;
    descriptor.factory = [](RDK::UApplication* app) -> UVisualControllerWidget*
    {
        return new PulseLibNeuronLearnerComponentControllerWidget(nullptr, app);
    };
    return descriptor;
}
}

void RegisterPulseLibComponentGuiForms()
{
    UComponentFormRegistry& registry = UComponentFormRegistry::instance();
    registry.registerFormFactory("NNeuronTrainer",
                                 MakePulseNeuronTrainerDescriptor("pulse.neuron.trainer", "PulseLib: Neuron Trainer"));
    registry.registerFormFactory("NNeuronLearner",
                                 MakePulseNeuronLearnerDescriptor("pulse.neuron.learner", "PulseLib: Neuron Learner"));
}
