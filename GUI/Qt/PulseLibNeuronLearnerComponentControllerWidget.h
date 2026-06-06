#ifndef PULSELIBNEURONLEARNERCOMPONENTCONTROLLERWIDGET_H
#define PULSELIBNEURONLEARNERCOMPONENTCONTROLLERWIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class PulseLibNeuronLearnerComponentControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget
{
    Q_OBJECT
public:
    explicit PulseLibNeuronLearnerComponentControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onApplyClicked();
    void onResetClicked();
    void onCalculateClicked();

private:
    UComponentGuiContext m_context;

    QComboBox* m_calculateModeCombo = nullptr;
    QCheckBox* m_needToTrainCheck = nullptr;
    QDoubleSpinBox* m_delaySpin = nullptr;
    QDoubleSpinBox* m_spikesFrequencySpin = nullptr;
    QSpinBox* m_numInputDendriteSpin = nullptr;
    QSpinBox* m_maxDendriteLengthSpin = nullptr;
    QCheckBox* m_useFixedThresholdCheck = nullptr;
    QDoubleSpinBox* m_thresholdSpin = nullptr;
    QDoubleSpinBox* m_synapseResistanceStepSpin = nullptr;
    QCheckBox* m_experimentModeCheck = nullptr;
    QSpinBox* m_experimentNumSpin = nullptr;
    QCheckBox* m_enableDebugCheck = nullptr;

    QPushButton* m_applyButton = nullptr;
    QPushButton* m_resetButton = nullptr;
    QPushButton* m_calculateButton = nullptr;
};

#endif // PULSELIBNEURONLEARNERCOMPONENTCONTROLLERWIDGET_H
