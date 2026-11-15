#pragma once
#include <optional>
#include <vector>
#include "TrainingParameters.h"

namespace NMSDK::TimeLearner {
struct MeasuredLengthPoint {
    int length=1;
    double timingResidualLow=0.,timingResidualHigh=0.;
};

// Proposal only. The caller selects completed observations for one input,
// unchanged Ns/Rs and a declared fixed geometry or Branch growth family. Revisions remain
// attached to the source observations; no peak or certificate is reused.
// A measured monotone trend may suggest an integer outside the nearest mesh.
// Nonmonotone/inconsistent data leave the bounded enumeration in charge.
std::optional<int> ProposeMeasuredLength(std::vector<MeasuredLengthPoint> points,
    int minimumLength,int maximumLength,double syncTolerance);

// Proposal eligibility only, not physical equivalence or measurement authority.
// Branch growth can move unvisited upstream attachments together. Admit only
// the ordinary or separated-bank family of one unchanged background; never
// change the anchor/committed downstream inputs or borrow another Rs/Ns.
bool SameBranchLengthProposalFamily(const std::vector<InputParameters>& background,std::size_t input,
    const std::vector<InputParameters>& previous,const std::vector<InputParameters>& current);
}
