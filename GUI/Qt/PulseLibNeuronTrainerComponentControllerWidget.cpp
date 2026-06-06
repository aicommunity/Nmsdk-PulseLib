#include "PulseLibNeuronTrainerComponentControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

#include "../../../../Rdk/Deploy/Include/rdk_init.h"

#include "../../Core/NNeuronTrainer.h"
#include "../../Core/NModel.h"
#include "../../../../Rdk/GUI/Qt/UGEngineControlWidget.h"

namespace
{
UGEngineControlWidget* resolveEngineControlWidget(QWidget* source)
{
    QObject* current = source;
    while(current)
    {
        if(auto* main = qobject_cast<UGEngineControlWidget*>(current))
            return main;
        current = current->parent();
    }
    return nullptr;
}
}

PulseLibNeuronTrainerComponentControllerWidget::PulseLibNeuronTrainerComponentControllerWidget(QWidget* parent,
                                                                                                 RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_needToTrainCheck = new QCheckBox("Need to train", this);
    m_delaySpin = new QDoubleSpinBox(this);
    m_delaySpin->setRange(0.0, 1e9);
    m_delaySpin->setDecimals(6);
    m_delaySpin->setSingleStep(0.1);

    m_spikesFrequencySpin = new QDoubleSpinBox(this);
    m_spikesFrequencySpin->setRange(0.0, 1e9);
    m_spikesFrequencySpin->setDecimals(6);
    m_spikesFrequencySpin->setSingleStep(0.1);

    m_useFixedThresholdCheck = new QCheckBox("Use fixed LTZ threshold", this);
    m_thresholdSpin = new QDoubleSpinBox(this);
    m_thresholdSpin->setRange(0.0, 1e9);
    m_thresholdSpin->setDecimals(6);
    m_thresholdSpin->setSingleStep(0.1);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Need to train:", m_needToTrainCheck);
    formLayout->addRow("Delay:", m_delaySpin);
    formLayout->addRow("Spikes frequency:", m_spikesFrequencySpin);
    formLayout->addRow("LTZ threshold mode:", m_useFixedThresholdCheck);
    formLayout->addRow("LTZ threshold:", m_thresholdSpin);

    m_applyButton = new QPushButton("Apply parameters", this);
    connect(m_applyButton, &QPushButton::clicked, this, &PulseLibNeuronTrainerComponentControllerWidget::onApplyClicked);

    m_toggleEngineControlButton = new QPushButton("Toggle engine control", this);
    connect(m_toggleEngineControlButton, &QPushButton::clicked, this, &PulseLibNeuronTrainerComponentControllerWidget::onToggleEngineControlClicked);

    m_watchWindowButton = new QPushButton("Open watch window", this);
    connect(m_watchWindowButton, &QPushButton::clicked, this, &PulseLibNeuronTrainerComponentControllerWidget::onWatchWindowClicked);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_applyButton);
    buttonsLayout->addWidget(m_toggleEngineControlButton);
    buttonsLayout->addWidget(m_watchWindowButton);

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->addLayout(formLayout);
    rootLayout->addLayout(buttonsLayout);

    setAccessibleName("PulseLibNeuronTrainerComponentControllerWidget");

    refreshFromModel(true);
}

void PulseLibNeuronTrainerComponentControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel(true);
}

QString PulseLibNeuronTrainerComponentControllerWidget::componentGuiId() const
{
    return "pulse.neuron.trainer.controller";
}

void PulseLibNeuronTrainerComponentControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);

    if(m_context.componentLongName.isEmpty())
        return;

    RDK::UELockPtr<NMSDK::NModel> model = RDK::GetModelLock<NMSDK::NModel>();
    if(!model)
        return;

    auto positionTrainer = RDK::dynamic_pointer_cast<NMSDK::NNeuronTrainer>(
        model->GetComponentL(m_context.componentLongName.toLocal8Bit().constData()));
    if(!positionTrainer)
        return;

    const bool needToTrain = positionTrainer->IsNeedToTrain.GetData();
    const double delay = positionTrainer->Delay.GetData();
    const double spikesFrequency = positionTrainer->SpikesFrequency.GetData();
    const bool useFixed = positionTrainer->UseFixedLTZThreshold.GetData();
    const double ltzThreshold = useFixed ? positionTrainer->FixedLTZThreshold.GetData() : positionTrainer->TrainingLTZThreshold.GetData();

    // Update UI without triggering apply-style changes
    QSignalBlocker b1(m_needToTrainCheck);
    QSignalBlocker b2(m_delaySpin);
    QSignalBlocker b3(m_spikesFrequencySpin);
    QSignalBlocker b4(m_useFixedThresholdCheck);
    QSignalBlocker b5(m_thresholdSpin);

    m_needToTrainCheck->setChecked(needToTrain);
    m_delaySpin->setValue(delay);
    m_spikesFrequencySpin->setValue(spikesFrequency);
    m_useFixedThresholdCheck->setChecked(useFixed);
    m_thresholdSpin->setValue(ltzThreshold);
}

void PulseLibNeuronTrainerComponentControllerWidget::onApplyClicked()
{
    if(m_context.componentLongName.isEmpty())
        return;

    RDK::UELockPtr<NMSDK::NModel> model = RDK::GetModelLock<NMSDK::NModel>();
    if(!model)
        return;

    auto positionTrainer = RDK::dynamic_pointer_cast<NMSDK::NNeuronTrainer>(
        model->GetComponentL(m_context.componentLongName.toLocal8Bit().constData()));
    if(!positionTrainer)
        return;

    const bool useFixed = m_useFixedThresholdCheck->isChecked();

    // Apply to component via setters to preserve internal consistency.
    positionTrainer->SetNeedToTrain(m_needToTrainCheck->isChecked());
    positionTrainer->SetDelay(m_delaySpin->value());
    positionTrainer->SetSpikesFrequency(m_spikesFrequencySpin->value());

    positionTrainer->SetUseFixedLTZThreshold(useFixed);
    if(useFixed)
        positionTrainer->SetFixedLTZThreshold(m_thresholdSpin->value());
    else
        positionTrainer->SetTrainingLTZThreshold(m_thresholdSpin->value());

    refreshFromModel(true);
}

void PulseLibNeuronTrainerComponentControllerWidget::onToggleEngineControlClicked()
{
    auto* main = resolveEngineControlWidget(this);
    if(!main)
    {
        QMessageBox::information(this, "Neuron Trainer", "Engine control widget is not found as parent.");
        return;
    }

    main->setVisible(!main->isVisible());
    main->raise();
}

void PulseLibNeuronTrainerComponentControllerWidget::onWatchWindowClicked()
{
    auto* main = resolveEngineControlWidget(this);
    if(!main)
        return;

    main->actionWatchWindow();
}

