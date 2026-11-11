#include "CandidateWindowPortableReferences.h"
#include "CandidateWindowProposalReplay.h"
#include "NativeCompositeTrainingEvidence.h"
#include "CandidateWindowMeasurementEvidence.h"
#include "CompositeSourceValidation.h"
#include "ConfiguredNativeTraining.h"
#include "ProbeEvidenceJson.h"
#include "ModelIdentity.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool v,const char* why){if(!v)throw std::invalid_argument(why);}
int Positive(const QJsonValue& v) {Require(v.isDouble() && std::isfinite(v.toDouble()) && v.toDouble()>0. &&
    v.toDouble()<=std::numeric_limits<int>::max() && std::floor(v.toDouble())==v.toDouble(),"candidate_window_portable_parameter");return int(v.toDouble());}
std::string Hash(const QJsonValue& v){return NmsdkTools::CanonicalModelDataSha256(v).toStdString();}
}
CandidateWindowReferenceSet CandidateWindowPortableReferenceFactory::Replay(const DecodedNativeCompositeArchive& archive) {
    const auto& p=archive.Protocol();ValidateVerificationProtocol(p);ValidateCandidateWindowMeasurementContract(p);
    Require(archive.Document()["schema_version"]==QJsonValue(2) && archive.ReferenceGroups().size()==p.inputs.size(),
        "candidate_window_portable_complete_reference_set_required");
    const auto& source=archive.SourceBinding();CompositeEvidenceDetail::ValidateInitialSource(p,source,archive.SourceOrigin());
    const auto initial=source["initial_model"].toObject();const auto initialEpoch=source["initial_epoch"].toDouble();
    std::optional<TrainingCapacitance> c0;if(!source["optional_C0_witness"].isNull())
        c0=DecodeNativeTrainingCapacitanceWitness(source["optional_C0_witness"].toObject()).parameters;
    std::vector<CandidateWindowReferenceSeed> seeds;std::vector<QJsonObject> bindings;std::vector<double> targets;std::set<std::string> seen;
    for(std::size_t i=0;i<p.inputs.size();++i) {
        const auto& id=archive.ReferenceGroups()[i];const auto found=std::find_if(archive.Records().begin(),archive.Records().end(),[&](const auto& r){return r.groupId==id;});
        Require(found!=archive.Records().end() && seen.insert(id).second,"candidate_window_portable_reference_group");const auto& r=*found;
        Require(r.purpose==CompositeMeasurementPurpose::Reference && r.logical.inputIndex==i && r.consumed && r.trainingRequestedAtConsume==true &&
            r.sourceOrigin==archive.SourceOrigin() && r.ownerFamilyBinding==archive.OwnerFamilyBinding() &&
            r.logical.baseModelRevision==initial["sha256"].toString().toStdString() && double(r.logical.epoch)==initialEpoch &&
            r.candidateWindowSpecification && r.candidateWindowSpecification->coordinateMode==CandidateWindowCoordinateMode::ReferenceTime && !r.windowProposal &&
            r.selection && r.selection->geometry && r.selection->geometry->measuredMaximum &&
            r.semanticProofId==CandidateWindowTrainingSemanticProofId(p,r),"candidate_window_portable_reference_binding");
        auto binding=ParseStrictNativeJsonObject(r.candidateBindingJson,128u*1024u*1024u);
        Require(binding["model_identity"]==QJsonValue(initial),"candidate_window_portable_reference_not_original_model");
        const auto values=binding["physical_readback"].toObject().value("parameters");
        Require(values.isArray() && std::size_t(values.toArray().size())==p.inputs.size(),"candidate_window_portable_P0_dimensions");
        CandidateWindowArithmeticSeed seed;seed.inputIndex=i;const auto& peak=*r.selection->geometry->measuredMaximum;
        seed.firstFrame=peak.firstSample;seed.lastFrame=peak.lastSample;
        targets.push_back(peak.rawPotential);
        seed.referenceFrameCount=r.group.witness.fullBefore.observation.channels.at(r.candidateWindowSpecification->channel).samples.size();
        for(const auto& value:values.toArray()) {
            Require(value.isObject(),"candidate_window_portable_P0_parameter");const auto item=value.toObject();const auto resistance=item["resistance"];
            Require(resistance.isDouble() && std::isfinite(resistance.toDouble()) && resistance.toDouble()>0.,"candidate_window_portable_P0_parameter");
            seed.initialParameters.push_back({Positive(item["length"]),Positive(item["synapses"]),resistance.toDouble()});
        }
        seed.initialCapacitance=c0;seed.grid.frequencyHz=std::uint64_t(p.stimulusProfile->frequencyHz);
        seed.grid.sourceOrigin=archive.SourceOrigin();seed.grid.sourcePatternBinding=p.patternHash;
        seed.grid.sourceProfileBinding=Hash(source["actual_source_profile"]);seed.grid.resetBinding=r.group.specification.resetBinding;
        seeds.push_back(CandidateWindowReferenceSeed(std::move(seed),r.semanticProofId,archive.OwnerFamilyBinding()));bindings.push_back(std::move(binding));
    }
    CandidateWindowReferenceSet result(std::move(seeds),archive.SourceOrigin(),archive.OwnerFamilyBinding(),archive.ReferenceGroups(),
        Hash(EncodeVerificationProtocol(p)),QJsonDocument(source).toJson(QJsonDocument::Compact).toStdString(),
        std::make_shared<const VerificationProtocol>(p),initial["sha256"].toString().toStdString(),std::size_t(initialEpoch),std::move(targets));
    // Re-derive every controlled value and bank/link membership from the FULL
    // original semantic graph. Public physical_readback numbers are not proof.
    for(std::size_t i=0;i<bindings.size();++i) {
        const auto checked=CandidateWindowProposalReplayFactory::CheckCandidateBinding(result,p,bindings[i]);
        Require(checked.Facts().parameters==result.seeds[i].arithmetic.initialParameters &&
            checked.Facts().capacitance==result.seeds[i].arithmetic.initialCapacitance,"candidate_window_portable_P0_graph_mismatch");
    }
    return result;
}
}
