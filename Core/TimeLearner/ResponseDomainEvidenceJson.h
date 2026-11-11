#pragma once
#include "NativeResponseDomainCapture.h"
#include <QByteArray>
#include <QJsonObject>
#include <QString>

namespace NMSDK::TimeLearner {
// Canonical primary waveform, with actual source values and all soma samples.
// Fails for an unverified/partial capture. The caller owns NewOnly file writing,
// flush/readback and publishing; this codec does not perform filesystem I/O.
QByteArray EncodeNativeResponseDomainTrace(const NativeResponseDomainCapture& capture);
// Binds CSV bytes exactly to this capture and derives analysis from these same
// samples. The caller cannot pass an unrelated native closure or analysis.
QJsonObject EncodeNativeResponseDomainEvidence(const NativeResponseDomainCapture& capture,
    const ResponseDomainSpec& domains,const QString& relativeTracePath,const QByteArray& trace);
}
