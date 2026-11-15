#include "CandidateWindowLogicalContext.h"
#include "CandidateWindowContractValidation.h"
#include <algorithm>
#include <charconv>
#include <stdexcept>
#include <system_error>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v,const char* reason){if(!v)throw std::invalid_argument(reason);}
bool Decimal(const std::string& s) {
    if(s.empty() || (s!="0" && s.front()=='0') || !std::all_of(s.begin(),s.end(),[](char c){return c>='0' && c<='9';}))return false;
    std::uint64_t value=0;const auto result=std::from_chars(s.data(),s.data()+s.size(),value);
    return result.ec==std::errc{} && result.ptr==s.data()+s.size() && value<=CandidateWindowMaximumExactCount;
}
}
void ValidateCandidateWindowLogicalContext(const VerificationProtocol& p,const VerificationRequest& r,
    const std::string& attempt,const std::string& group,CompositeMeasurementPurpose purpose) {
    ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);
    Require(!attempt.empty() && !r.runId.empty() && !r.trialId.empty() && !group.empty() && r.inputIndex<p.inputs.size() &&
        r.inputId==p.inputs[r.inputIndex].id && r.sampleId==p.sampleId && r.patternHash==p.patternHash && r.protocolRevision==p.revision &&
        r.routing==p.routing && !r.baseModelRevision.empty() && r.epoch<=CandidateWindowMaximumExactCount,"candidate_window_logical_context");
    Require(purpose==CompositeMeasurementPurpose::Reference || purpose==CompositeMeasurementPurpose::Search || purpose==CompositeMeasurementPurpose::Final,
        "candidate_window_proposal_purpose");
    if(purpose==CompositeMeasurementPurpose::Search)Require(r.runId==attempt,"candidate_window_attempt_namespace");
    else {const auto prefix=attempt+(purpose==CompositeMeasurementPurpose::Reference?"/capture/":"/final/");
        Require(r.runId.starts_with(prefix) && Decimal(r.runId.substr(prefix.size())),"candidate_window_attempt_namespace");}
    const auto trialPrefix=purpose==CompositeMeasurementPurpose::Reference?r.runId+"/":
        r.runId+(purpose==CompositeMeasurementPurpose::Search?"/search/":"/verify/"+std::to_string(r.epoch)+"/");
    Require(r.trialId.starts_with(trialPrefix) && Decimal(r.trialId.substr(trialPrefix.size())),"candidate_window_trial_namespace");
    const auto groupPrefix=r.trialId+"/composite/";
    Require(group.starts_with(groupPrefix) && Decimal(group.substr(groupPrefix.size())),"candidate_window_group_namespace");
}
}
