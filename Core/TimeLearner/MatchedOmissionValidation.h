#pragma once
// Private production implementation detail shared by matched selectors.
// Not a native attestation, persistent proof format or public admission API.
#include "MatchedOmissionAssignment.h"
#include <memory>
#include <utility>

namespace NMSDK::TimeLearner::MatchedOmissionDetail {
using Validity=MatchedOmissionValidity;
struct Failure {Validity validity;const char* reason;};
void Require(bool condition,Validity validity,const char* reason);
double Difference(double a,double b);
double Tolerance(double absolute,double relative,double a,double b);
void CheckPolicy(const MatchedOmissionPolicy& policy,const StimulusProfile& source,std::size_t count);
struct CheckedTrace {
    const MatchedOmissionTrace* trace=nullptr;
    std::vector<std::size_t> eventFrames;
};
class CheckedContext;
std::unique_ptr<const CheckedContext> CheckMatchedOmissionContext(
    const MatchedOmissionSpecification&,const MatchedOmissionPolicy&,const MatchedOmissionWitness&);
// Factory owned and non-copyable. Borrowed observations are used only inside
// the calling selector; the input witness must remain alive and unchanged.
// Checks validate the declared records, never independently prove live reset
// ownership or the physical origin of arbitrary caller-constructed records.
class CheckedContext {
public:
    CheckedContext(const CheckedContext&)=delete;
    CheckedContext& operator=(const CheckedContext&)=delete;
    const CheckedTrace& Before() const {return before;}
    const CheckedTrace& After() const {return after;}
    const std::vector<CheckedTrace>& Controls() const {return controls;}
private:
    friend std::unique_ptr<const CheckedContext> CheckMatchedOmissionContext(
        const MatchedOmissionSpecification&,const MatchedOmissionPolicy&,const MatchedOmissionWitness&);
    CheckedContext(CheckedTrace first,CheckedTrace last,std::vector<CheckedTrace> omitted)
        :before(std::move(first)),after(std::move(last)),controls(std::move(omitted)) {}
    CheckedTrace before,after;
    std::vector<CheckedTrace> controls;
};
}
