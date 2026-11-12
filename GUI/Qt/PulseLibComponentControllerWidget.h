#ifndef PULSELIBCOMPONENTCONTROLLERWIDGET_H
#define PULSELIBCOMPONENTCONTROLLERWIDGET_H

#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>

#include "../../../../Rdk/GUI/Qt/UVisualControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/IComponentControllerWidget.h"
#include "../../../../Rdk/GUI/Qt/UComponentGuiContext.h"

class PulseLibComponentControllerWidget : public UVisualControllerWidget, public IComponentControllerWidget
{
    Q_OBJECT
public:
    explicit PulseLibComponentControllerWidget(QWidget* parent = nullptr, RDK::UApplication* app = nullptr);

    void setComponentContext(const UComponentGuiContext& context) override;
    void refreshFromModel(bool force) override;
    QString componentGuiId() const override;

private slots:
    void onResetClicked();
    void onCalculateClicked();

private:
    UComponentGuiContext m_context;
    QLabel* m_titleLabel;
    QPlainTextEdit* m_details;
    QPushButton* m_resetButton;
    QPushButton* m_calculateButton;
};

#endif // PULSELIBCOMPONENTCONTROLLERWIDGET_H
