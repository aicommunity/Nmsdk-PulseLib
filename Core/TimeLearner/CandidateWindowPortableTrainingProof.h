#pragma once
#include "NativeCompositeTrainingEvidence.h"
#include <utility>

namespace NMSDK::TimeLearner {
// Closed protocol5 DATA proof for a saved boundary. The archive decoder and
// portable factories replay primary/native/context/geometry/proposal bindings;
// this class replays complete original targets and the selected fresh final
// residuals. It restores neither a live lease nor controller traversal/history.
class CandidateWindowPortableTrainingProof {
public:
    static CandidateWindowPortableTrainingProof Replay(DecodedNativeCompositeArchive,
        const VerificationTolerances&,const std::string& modelRevision,std::size_t modelEpoch);
    const DecodedNativeCompositeArchive& Archive() const {return archive;}
    const std::optional<CapturedReferences>& References() const {return references;}
    const std::optional<CandidateWindowReferenceSet>& WindowReferences() const {return windowReferences;}
    const std::optional<TrainingCertificate>& Certificate() const {return certificate;}
    const std::string& ModelRevision() const {return modelRevision;}
    std::size_t ModelEpoch() const {return modelEpoch;}
    // Exact mathematical comparison includes schema3 identities, residuals
    // and tolerances. Archive DATA does not encode the final service outcome:
    // even a matching proof does not admit a stopped package as completed.
    // The future package6 owner must independently require its actual completed
    // boundary/live certificate, or preserve stopped diagnostic/null claims.
    void RequireCertificate(const QJsonObject& claimed) const;
private:
    explicit CandidateWindowPortableTrainingProof(DecodedNativeCompositeArchive value):archive(std::move(value)){}
    DecodedNativeCompositeArchive archive;
    std::optional<CapturedReferences> references;
    std::optional<CandidateWindowReferenceSet> windowReferences;
    std::optional<TrainingCertificate> certificate;
    std::string modelRevision;
    std::size_t modelEpoch=0;
};
}
