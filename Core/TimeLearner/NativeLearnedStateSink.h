#pragma once
#include <QJsonObject>
#include <functional>

namespace NMSDK::TimeLearner {
// A checked closed model is delivered before derived summary/publication work.
// The sink may persist DATA or throw; it must not edit the owner or its control.
// A delivered package is not the completion marker for the surrounding job.
using NativeLearnedStateSink=std::function<void(const QJsonObject&)>;
}
