#pragma once
#include "NativeTrainingService.h"
#include "MatchedOmissionEvidenceJson.h"

namespace NMSDK::TimeLearner {
struct NativeCompositeArchiveDocument {
    QJsonObject document;
    MatchedOmissionPrimary primary;
};
class CompositePortableTrainingProof;
class NativeCompositeTrainingAttempt;
// Closed portable data, not a live lease or a claim that execution happened.
// Complete groups replay native closure and the current selector from exact
// CSV bytes. Partials prove no native closure and remain nonpromotable.
class DecodedNativeCompositeArchive {
public:
    DecodedNativeCompositeArchive(const DecodedNativeCompositeArchive&)=default;
    DecodedNativeCompositeArchive& operator=(const DecodedNativeCompositeArchive&)=default;
    // The explicit copy constructor otherwise suppresses implicit moves and
    // turns proof/factory ownership transfers into deep copies of raw traces.
    DecodedNativeCompositeArchive(DecodedNativeCompositeArchive&&)=default;
    DecodedNativeCompositeArchive& operator=(DecodedNativeCompositeArchive&&)=default;
    const std::vector<NativeCompositeMeasurementRecord>& Records() const {return records;}
    const VerificationProtocol& Protocol() const {return protocol;}
    const QJsonObject& SourceBinding() const {return sourceBinding;}
    const std::string& SourceOrigin() const {return sourceOrigin;}
    const std::string& OwnerFamilyBinding() const {return ownerFamilyBinding;}
    const std::string& CurrentAttemptId() const {return currentAttemptId;}
    const std::vector<std::string>& ReferenceGroups() const {return referenceGroups;}
    const std::vector<std::string>& FinalGroups() const {return finalGroups;}
    const QJsonObject& Document() const {return document;}
private:
    DecodedNativeCompositeArchive()=default;
    VerificationProtocol protocol;
    QJsonObject sourceBinding,document;
    std::string sourceOrigin,ownerFamilyBinding,currentAttemptId;
    std::vector<NativeCompositeMeasurementRecord> records;
    std::vector<std::string> referenceGroups,finalGroups;
    friend class CompositePortableTrainingProof;
    friend class NativeTrainingService;
    friend DecodedNativeCompositeArchive DecodeNativeCompositeTrainingArchive(const QJsonObject&,
        const std::function<QByteArray(const QString&)>&);
};
NativeCompositeArchiveDocument EncodeNativeCompositeTrainingArchive(const NativeTrainingService&,
    const QString& primaryPrefix="composite-measurements");
// Explicit service5 accepted journal; remains separate from actor-only STEP2
// export and from a future completed learned6/certificate3 portable package.
NativeCompositeArchiveDocument EncodeNativeCandidateWindowServiceArchive(const NativeTrainingService&,
    const QString& primaryPrefix="composite-measurements");
// Explicit archive2 export of protocol5 factory history at a group boundary.
// It does not enable the service, search/final admission or certificates.
NativeCompositeArchiveDocument EncodeNativeCandidateWindowTrainingArchive(const NativeCompositeTrainingAttempt&,
    const QString& primaryPrefix="candidate-window-measurements");
DecodedNativeCompositeArchive DecodeNativeCompositeTrainingArchive(const QJsonObject&,
    const std::function<QByteArray(const QString&)>& externalPrimaryResolver={});
DecodedNativeCompositeArchive DecodeNativeCompositeTrainingArchive(const QByteArray&,
    const std::function<QByteArray(const QString&)>& externalPrimaryResolver={});
QJsonObject EncodeNativeCompositeSelection(const NativeDeclaredWindowBasinResult&);
struct CompositeSemanticProofContent {
    const std::string& sourceOrigin;
    const std::string& candidateWitnessId;
    const std::string& liveAttemptBinding;
    const CompositeAttemptLimits& attemptLimits;
    double maximumGroupModelSeconds,maximumGroupWallSeconds;
    const std::string& logicalTrialId;
    const std::string& groupId;
    CompositeMeasurementPurpose purpose;
    const NativeMatchedOmissionResult& nativeGroup;
    const ResponseMaximum& actualPeak;
};
std::string CompositeTrainingSemanticProofId(const VerificationProtocol&,const CompositeSemanticProofContent&);
std::string CompositeTrainingSemanticProofId(const VerificationProtocol&,const NativeCompositeMeasurementRecord&);
}
