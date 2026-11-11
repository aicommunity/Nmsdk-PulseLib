#include "NativeCompositeTrainingRecord.h"
#include "NumericChecks.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>

namespace NMSDK::TimeLearner {
namespace {void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);}}
QByteArray EncodeCompositeRawPrimary(const NativeCompositeRawTrial& trial) {
    const auto& s=trial.actual;const ProbeRecorder validate(s);
    Require(trial.prepared.effectiveModelRevision==s.modelRevision && trial.samples.size()<=s.maximumSamples,
        "composite_primary_context_or_budget_changed");
    std::ostringstream stream;stream.imbue(std::locale::classic());stream<<std::setprecision(17)<<"time,stimulus";
    for(std::size_t i=0;i<s.baseline.size();++i)stream<<",soma"<<i;stream<<'\n';
    std::optional<double> previous;
    for(const auto& f:trial.samples) {
        Require(f.effectiveModelRevision==s.modelRevision && std::isfinite(f.time) && std::isfinite(f.stimulus) &&
            f.potentials.size()==s.baseline.size() && (!previous || (f.time>*previous &&
            !Detail::IntervalAfter(f.time,*previous,s.maximumSampleGap))),"invalid_composite_primary_frame");
        stream<<f.time<<','<<f.stimulus;
        for(double value:f.potentials){Require(std::isfinite(value),"nonfinite_composite_primary");stream<<','<<value;}
        stream<<'\n';previous=f.time;
    }
    Require(bool(stream),"composite_primary_encoding_failed");return QByteArray::fromStdString(stream.str());
}
std::size_t CompositeRecordPrimaryBytes(const NativeCompositeMeasurementRecord& record) {
    std::size_t total=0;for(const auto& t:record.rawTrials) {
        const auto size=std::size_t(EncodeCompositeRawPrimary(t).size());
        Require(size<=std::numeric_limits<std::size_t>::max()-total,"composite_primary_size_overflow");total+=size;
    }
    return total;
}
}
