#ifndef PULSELIBNEURONTRAINERCOMPONENTCONTROLLERWIDGET_H
#define PULSELIBNEURONTRAINERCOMPONENTCONTROLLERWIDGET_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"
#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"

class PulseLibNeuronTrainerComponentControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget
{
    Q_OBJECT
public:
    explicit PulseLibNeuronTrainerComponentControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onApplyClicked();
    void onToggleEngineControlClicked();
    void onWatchWindowClicked();

private:
    UComponentGuiContext m_context;

    QCheckBox* m_needToTrainCheck = nullptr;
    QDoubleSpinBox* m_delaySpin = nullptr;
    QDoubleSpinBox* m_spikesFrequencySpin = nullptr;

    QCheckBox* m_useFixedThresholdCheck = nullptr;
    QDoubleSpinBox* m_thresholdSpin = nullptr;

    QPushButton* m_applyButton = nullptr;
    QPushButton* m_toggleEngineControlButton = nullptr;
    QPushButton* m_watchWindowButton = nullptr;
};

#endif // PULSELIBNEURONTRAINERCOMPONENTCONTROLLERWIDGET_H

