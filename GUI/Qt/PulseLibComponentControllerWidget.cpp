#include "PulseLibComponentControllerWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "../../../../Rdk/Deploy/Include/rdk_init.h"

PulseLibComponentControllerWidget::PulseLibComponentControllerWidget(QWidget* parent, RDK::UApplication* app)
    : UVisualControllerWidget(parent, app)
    , m_titleLabel(new QLabel(this))
    , m_details(new QPlainTextEdit(this))
    , m_resetButton(new QPushButton("Reset component", this))
    , m_calculateButton(new QPushButton("Calculate component", this))
{
    setAccessibleName("PulseLibComponentControllerWidget");

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(4);
    rootLayout->addWidget(m_titleLabel);
    rootLayout->addWidget(m_details);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_resetButton);
    buttonsLayout->addWidget(m_calculateButton);
    rootLayout->addLayout(buttonsLayout);

    m_details->setReadOnly(true);
    connect(m_resetButton, &QPushButton::clicked, this, &PulseLibComponentControllerWidget::onResetClicked);
    connect(m_calculateButton, &QPushButton::clicked, this, &PulseLibComponentControllerWidget::onCalculateClicked);

    refreshFromModel(true);
}

void PulseLibComponentControllerWidget::setComponentContext(const UComponentGuiContext& context)
{
    m_context = context;
}

void PulseLibComponentControllerWidget::refreshFromModel(bool force)
{
    Q_UNUSED(force);
    const QString title = QString("PulseLib component: %1").arg(m_context.componentLongName);
    m_titleLabel->setText(title);
    setWindowTitle(title);

    QString details;
    details += "Class: " + m_context.componentClassName + "\n";
    details += "Channel: " + QString::number(m_context.channelIndex) + "\n";
    details += "Purpose: runtime control entrypoint for PulseLib component.\n";
    details += "Actions: reset/calculate via component API.\n";
    m_details->setPlainText(details);
}

QString PulseLibComponentControllerWidget::componentGuiId() const
{
    return "pulse.component.controller";
}

void PulseLibComponentControllerWidget::onResetClicked()
{
    if(m_context.componentLongName.isEmpty())
        return;
    Env_Reset(m_context.componentLongName.toLocal8Bit().constData());
    refreshFromModel(true);
}

void PulseLibComponentControllerWidget::onCalculateClicked()
{
    if(m_context.componentLongName.isEmpty())
        return;
    Env_Calculate(m_context.componentLongName.toLocal8Bit().constData());
    refreshFromModel(true);
}
