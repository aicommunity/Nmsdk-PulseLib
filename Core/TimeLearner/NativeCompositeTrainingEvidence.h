#pragma once
#include "NativeTrainingService.h"
#include "MatchedOmissionEvidenceJson.h"

namespace NMSDK::TimeLearner {
struct NativeCompositeArchiveDocument {
    QJsonObject document;
    MatchedOmissionPrimary primary;
};
class CompositePortableTrainingProof;
// Closed portable data, not a live lease or a claim that execution happened.
// Complete groups replay native closure and the current selector from exact
// CSV bytes. Partials prove no native closure and remain nonpromotable.
class DecodedNativeCompositeArchive {
public:
    DecodedNativeCompositeArchive(const DecodedNativeCompositeArchive&)=default;
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
