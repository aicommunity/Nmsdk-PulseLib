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

// Coverage proposal after a rejected observation, not a timing estimate.
// Maximize distance to already issued lengths in one unchanged search family.
// Work is bounded by the recorded inventory, not the integer domain width.
std::optional<int> ProposeUnvisitedLength(std::vector<int> issued,
    int minimumLength,int maximumLength);

// Exploration around a measured timing-ready amplitude miss, not a timing or
// amplitude prediction. Prefer the nearest unissued integer, then the smaller
// length. Work is bounded by issued inventory even for INT_MAX-wide domains.
std::optional<int> ProposeNearestUnvisitedLength(std::vector<int> issued,
    int centerLength,int minimumLength,int maximumLength);

// Recovery proposal order only. First cover the legal half-to-double band
// around a fixed first-rejection center; if that finite band is exhausted,
// fall back to the unchanged full declared domain. No timing/validity claim.
// Bounded by the recorded inventory, including for INT_MAX-wide domains.
std::optional<int> ProposeModerateUnvisitedLength(std::vector<int> issued,
    int centerLength,int minimumLength,int maximumLength);

// Proposal eligibility only, not physical equivalence or measurement authority.
// Branch growth can move unvisited upstream attachments together. Admit only
// the ordinary or separated-bank family of one unchanged background; never
// change the anchor/committed downstream inputs or borrow another Rs/Ns.
bool SameBranchLengthProposalFamily(const std::vector<InputParameters>& background,std::size_t input,
    const std::vector<InputParameters>& previous,const std::vector<InputParameters>& current);
}
