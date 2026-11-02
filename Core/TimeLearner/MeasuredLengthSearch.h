#pragma once
#include <optional>
#include <vector>

namespace NMSDK::TimeLearner {
struct MeasuredLengthPoint {
    int length=1;
    double timingResidualLow=0.,timingResidualHigh=0.;
};

// Proposal only. The caller selects completed observations for one input,
// unchanged Ns/Rs and unchanged other input parameters. Revisions remain
// attached to the source observations; no peak or certificate is reused.
// A measured monotone trend may suggest an integer outside the nearest mesh.
// Nonmonotone/inconsistent data leave the bounded enumeration in charge.
std::optional<int> ProposeMeasuredLength(std::vector<MeasuredLengthPoint> points,
    int minimumLength,int maximumLength,double syncTolerance);
}
