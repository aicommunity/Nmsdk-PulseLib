#pragma once
#include "NativeMatchedOmissionGroup.h"
#include <QByteArray>
#include <QJsonObject>
#include <QString>
#include <map>

namespace NMSDK::TimeLearner {
using MatchedOmissionPrimary=std::map<QString,QByteArray>;
// Strict native replay precedes serialization. Canonical CSV is the existing
// native-probe-csv-v1: time,stimulus,soma0,..., classic locale, binary64 17g, LF.
QByteArray EncodeMatchedOmissionPrimaryTrace(const MatchedOmissionTrace& trace,
                                            const NativeProbeResult& native);
// Closed complete N+2 groups only. Trace order is fullBefore, omit0,...,
// omitN-1, fullAfter. No filesystem I/O, partial group or Train certificate.
// Assignment is recomputed; a supplied stale assignment is an error.
QJsonObject EncodeMatchedOmissionEvidence(const NativeMatchedOmissionResult& result,
    const std::vector<QString>& relativeTracePaths,const MatchedOmissionPrimary& primary);
// Replays each imported CSV through Arm/Append/CloseNative, validates complete
// source/policy/context bindings, and compares fresh assignment with the JSON.
NativeMatchedOmissionResult DecodeMatchedOmissionEvidence(const QJsonObject& object,
                                                         const MatchedOmissionPrimary& primary);
// Use the byte overload at an untrusted JSON boundary: duplicate keys and raw
// JSON syntax cannot be recovered after constructing an ordinary QJsonObject.
NativeMatchedOmissionResult DecodeMatchedOmissionEvidence(const QByteArray& document,
                                                         const MatchedOmissionPrimary& primary);
}
