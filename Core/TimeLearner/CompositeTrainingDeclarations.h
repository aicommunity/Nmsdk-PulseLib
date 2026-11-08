#pragma once
#include "NativeCompositeTrainingRecord.h"
#include <QJsonObject>

namespace NMSDK::TimeLearner {
QJsonObject EncodeCompositeContextPolicy(const MatchedOmissionPolicy&);
MatchedOmissionPolicy DecodeCompositeContextPolicy(const QJsonObject&);
QJsonObject EncodeCompositeGeometryPolicy(const DeclaredWindowBasinPolicy&);
DeclaredWindowBasinPolicy DecodeCompositeGeometryPolicy(const QJsonObject&);
QJsonObject EncodeCompositeMeasurementContract(const CompositeMeasurementContract&);
CompositeMeasurementContract DecodeCompositeMeasurementContract(const QJsonObject&);
QJsonObject EncodeNativeCompositeTrainingPolicy(const NativeCompositeTrainingPolicy&);
NativeCompositeTrainingPolicy DecodeNativeCompositeTrainingPolicy(const QJsonObject&);
// The same semantic method binding is used by live production and portable
// proof replay. Artifact paths are absent; anti-replay execution IDs remain.
QJsonObject CompositeTrainingProtocolBinding(const VerificationProtocol&);
}
