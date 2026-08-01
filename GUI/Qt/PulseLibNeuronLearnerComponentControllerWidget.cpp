#include "PulseLibNeuronLearnerComponentControllerWidget.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include "../../../../Rdk/Deploy/Include/rdk_init.h"
#include "../../Core/NModel.h"
#include "../../Core/NNeuronLearner.h"

PulseLibNeuronLearnerComponentControllerWidget::PulseLibNeuronLearnerComponentControllerWidget(QWidget* parent,
                                                                                                 RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
{
    m_calculateModeCombo = new QComboBox(this);
    m_calculateModeCombo->addItem("Trainer-managed (mode 0)", 0);
    m_calculateModeCombo->addItem("Self-managed (mode 1)", 1);

    m_needToTrainCheck = new QCheckBox("Need to train", this);

    m_delaySpin = new QDoubleSpinBox(this);
    m_delaySpin->setRange(0.0, 1e9);
    m_delaySpin->setDecimals(6);
    m_delaySpin->setSingleStep(0.1);

    m_spikesFrequencySpin = new QDoubleSpinBox(this);
    m_spikesFrequencySpin->setRange(0.0, 1e9);
    m_spikesFrequencySpin->setDecimals(6);
    m_spikesFrequencySpin->setSingleStep(0.1);

    m_numInputDendriteSpin = new QSpinBox(this);
    m_numInputDendriteSpin->setRange(1, 1000000);

    m_maxDendriteLengthSpin = new QSpinBox(this);
    m_maxDendriteLengthSpin->setRange(1, 1000000);

    m_useFixedThresholdCheck = new QCheckBox("Use fixed LTZ threshold", this);
    m_thresholdSpin = new QDoubleSpinBox(this);
    m_thresholdSpin->setRange(-1e9, 1e9);
    m_thresholdSpin->setDecimals(6);
    m_thresholdSpin->setSingleStep(0.1);

    m_synapseResistanceStepSpin = new QDoubleSpinBox(this);
    m_synapseResistanceStepSpin->setRange(0.0, 1e9);
    m_synapseResistanceStepSpin->setDecimals(9);
    m_synapseResistanceStepSpin->setSingleStep(0.000001);

    m_experimentModeCheck = new QCheckBox("Experiment mode", this);
    m_experimentNumSpin = new QSpinBox(this);
    m_experimentNumSpin->setRange(0, 1000000);

    m_enableDebugCheck = new QCheckBox("Enable debug", this);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Calculate mode:", m_calculateModeCombo);
    formLayout->addRow("Need to train:", m_needToTrainCheck);
    formLayout->addRow("Delay:", m_delaySpin);
    formLayout->addRow("Spikes frequency:", m_spikesFrequencySpin);
    formLayout->addRow("Num input dendrite:", m_numInputDendriteSpin);
    formLayout->addRow("Max dendrite length:", m_maxDendriteLengthSpin);
    formLayout->addRow("LTZ threshold mode:", m_useFixedThresholdCheck);
    formLayout->addRow("LTZ threshold:", m_thresholdSpin);
    formLayout->addRow("Synapse resistance step:", m_synapseResistanceStepSpin);
    formLayout->addRow("Experiment mode:", m_experimentModeCheck);
    formLayout->addRow("Experiment number:", m_experimentNumSpin);
    formLayout->addRow("Enable debug:", m_enableDebugCheck);

    m_applyButton = new QPushButton("Apply parameters", this);
    m_resetButton = new QPushButton("Reset component", this);
    m_calculateButton = new QPushButton("Calculate component", this);

    connect(m_applyButton, &QPushButton::clicked, this, &PulseLibNeuronLearnerComponentControllerWidget::onApplyClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &PulseLibNeuronLearnerComponentControllerWidget::onResetClicked);
    connect(m_calculateButton, &QPushButton::clicked, this, &PulseLibNeuronLearnerComponentControllerWidget::onCalculateClicked);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_applyButton);
    buttonsLayout->addWidget(m_resetButton);
    buttonsLayout->addWidget(m_calculateButton);

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(4);
    rootLayout->addLayout(formLayout);
    rootLayout->addLayout(buttonsLayout);

    setAccessibleName("PulseLibNeuronLearnerComponentControllerWidget");
}

void PulseLibNeuronLearnerComponentControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
    refreshFromModel(true);
}

QString PulseLibNeuronLearnerComponentControllerWidget::componentGuiId() const
{
    return "pulse.neuron.learner.controller";
}

void PulseLibNeuronLearnerComponentControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    if(m_context.componentLongName.isEmpty())
        return;

    RDK::UELockPtr<NMSDK::NModel> model = RDK::GetModelLock<NMSDK::NModel>();
    if(!model)
        return;

    auto learner = RDK::dynamic_pointer_cast<NMSDK::NNeuronLearner>(
        model->GetComponentL(m_context.componentLongName.toLocal8Bit().constData()));
    if(!learner)
        return;

    QSignalBlocker b1(m_calculateModeCombo);
    QSignalBlocker b2(m_needToTrainCheck);
    QSignalBlocker b3(m_delaySpin);
    QSignalBlocker b4(m_spikesFrequencySpin);
    QSignalBlocker b5(m_numInputDendriteSpin);
    QSignalBlocker b6(m_maxDendriteLengthSpin);
    QSignalBlocker b7(m_useFixedThresholdCheck);
    QSignalBlocker b8(m_thresholdSpin);
    QSignalBlocker b9(m_synapseResistanceStepSpin);
    QSignalBlocker b10(m_experimentModeCheck);
    QSignalBlocker b11(m_experimentNumSpin);
    QSignalBlocker b12(m_enableDebugCheck);

    const int calculateMode = learner->CalculateMode.GetData();
    int comboIndex = m_calculateModeCombo->findData(calculateMode);
    if(comboIndex < 0)
        comboIndex = 0;
    m_calculateModeCombo->setCurrentIndex(comboIndex);
    m_needToTrainCheck->setChecked(learner->IsNeedToTrain.GetData());
    m_delaySpin->setValue(learner->Delay.GetData());
    m_spikesFrequencySpin->setValue(learner->SpikesFrequency.GetData());
    m_numInputDendriteSpin->setValue(learner->NumInputDendrite.GetData());
    m_maxDendriteLengthSpin->setValue(learner->MaxDendriteLength.GetData());
    const bool useFixed = learner->UseFixedLTZThreshold.GetData();
    m_useFixedThresholdCheck->setChecked(useFixed);
    m_thresholdSpin->setValue(useFixed ? learner->FixedLTZThreshold.GetData() : learner->TrainingLTZThreshold.GetData());
    m_synapseResistanceStepSpin->setValue(learner->SynapseResistanceStep.GetData());
    m_experimentModeCheck->setChecked(learner->ExperimentMode.GetData());
    m_experimentNumSpin->setValue(learner->ExperimentNum.GetData());
    m_enableDebugCheck->setChecked(learner->EnableDebug.GetData());
}

void PulseLibNeuronLearnerComponentControllerWidget::onApplyClicked()
{
    if(m_context.componentLongName.isEmpty())
        return;

    RDK::UELockPtr<NMSDK::NModel> model = RDK::GetModelLock<NMSDK::NModel>();
    if(!model)
        return;

    auto learner = RDK::dynamic_pointer_cast<NMSDK::NNeuronLearner>(
        model->GetComponentL(m_context.componentLongName.toLocal8Bit().constData()));
    if(!learner)
        return;

    learner->SetCalculateMode(m_calculateModeCombo->currentData().toInt());
    learner->SetIsNeedToTrain(m_needToTrainCheck->isChecked());
    learner->SetDelay(m_delaySpin->value());
    learner->SetSpikesFrequency(m_spikesFrequencySpin->value());
    learner->SetNumInputDendrite(m_numInputDendriteSpin->value());
    learner->SetMaxDendriteLength(m_maxDendriteLengthSpin->value());
    learner->SetUseFixedLTZThreshold(m_useFixedThresholdCheck->isChecked());
    if(m_useFixedThresholdCheck->isChecked())
        learner->SetFixedLTZThreshold(m_thresholdSpin->value());
    else
        learner->SetTrainingLTZThreshold(m_thresholdSpin->value());
    learner->SetSynapseResistanceStep(m_synapseResistanceStepSpin->value());
    learner->SetExperimentMode(m_experimentModeCheck->isChecked());
    learner->SetExperimentNum(m_experimentNumSpin->value());
    learner->SetEnableDebug(m_enableDebugCheck->isChecked());
    refreshFromModel(true);
}

void PulseLibNeuronLearnerComponentControllerWidget::onResetClicked()
{
    if(m_context.componentLongName.isEmpty())
        return;
    Env_Reset(m_context.componentLongName.toLocal8Bit().constData());
    refreshFromModel(true);
}

void PulseLibNeuronLearnerComponentControllerWidget::onCalculateClicked()
{
    if(m_context.componentLongName.isEmpty())
        return;
    Env_Calculate(m_context.componentLongName.toLocal8Bit().constData());
    refreshFromModel(true);
}
