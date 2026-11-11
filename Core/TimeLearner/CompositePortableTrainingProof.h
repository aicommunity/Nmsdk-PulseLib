#pragma once
#include "NativeCompositeTrainingEvidence.h"

namespace NMSDK::TimeLearner {
// Replays the accepted measured boundary and its residual math. This is not
// training traversal, invalid-budget/wall-history replay or live authority.
class CompositePortableTrainingProof {
public:
    static CompositePortableTrainingProof Replay(DecodedNativeCompositeArchive archive,
        const VerificationTolerances&,const std::string& modelRevision,std::size_t modelEpoch);
    const DecodedNativeCompositeArchive& Archive() const {return archive;}
    const std::optional<CapturedReferences>& References() const {return references;}
    const std::optional<TrainingCertificate>& Certificate() const {return certificate;}
    const std::string& ModelRevision() const {return modelRevision;}
    std::size_t ModelEpoch() const {return modelEpoch;}
private:
    explicit CompositePortableTrainingProof(DecodedNativeCompositeArchive value):archive(std::move(value)){}
    DecodedNativeCompositeArchive archive;
    std::optional<CapturedReferences> references;
    std::optional<TrainingCertificate> certificate;
    std::string modelRevision;
    std::size_t modelEpoch=0;
    friend class NativeTrainingService;
    friend class NativeCompositeStateFactory;
};
}
