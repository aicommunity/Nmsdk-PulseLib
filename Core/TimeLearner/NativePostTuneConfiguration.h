#pragma once
#include "NativePostTuneTransaction.h"

namespace NMSDK::TimeLearner {
// The nine explicit controls of the existing version-1 production job.
// No Rdk access or physical edits. Model/design validation still belongs to
// NativePostTuneTransaction and its native preview, before any live import.
// The caller supplies the declared calibration design separately; this codec
// never invents trials, thresholds, defaults or an independent partition.
QJsonObject EncodeNativePostTuneControls(const NativePostTuneTransactionSpec& specification);
NativePostTuneTransactionSpec DecodeNativePostTuneControls(const QJsonObject& controls,
    NativeThresholdGridSpec calibration);
// Explicit schema 2 adds finalization; schema 1 always means Calibrate.
// Serializing fixed finalization through the old codec is rejected.
QJsonObject EncodeNativePostTuneControlsV2(const NativePostTuneTransactionSpec& specification);
NativePostTuneTransactionSpec DecodeNativePostTuneControlsV2(const QJsonObject& controls,
    NativeThresholdGridSpec calibration);
}
