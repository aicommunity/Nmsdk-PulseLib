#pragma once
#include "CandidateWindowTrainingDeclarations.h"
#include "FinalVerification.h"
#include <QByteArray>

namespace NMSDK::TimeLearner {
// DATA codecs do not create any opaque reference, live or replay authority.
QJsonObject EncodeCandidateWindowProposalData(const CandidateWindowProposalRecord&);
CandidateWindowProposalRecord DecodeCandidateWindowProposalData(const QJsonObject&);
CandidateWindowProposalRecord DecodeCandidateWindowProposalData(const QByteArray&);
QJsonObject CandidateWindowRecipeContent(const CandidateWindowProposalRecord&);
QJsonObject CandidateWindowInstanceContent(const CandidateWindowProposalRecord&);

class CandidateWindowProposalReplayFactory {
public:
    static ReplayedCandidateWindowBinding CheckCandidateBinding(const CandidateWindowReferenceSet&,
        const VerificationProtocol&,const QJsonObject& binding);
    // Compute is DATA only. A caller must run the independent Replay path;
    // construction of a predicted window never admits a measured maximum.
    static CandidateWindowProposalRecord Compute(const CandidateWindowReferenceSet&,
        const VerificationProtocol&,const ReplayedCandidateWindowBinding&,
        const VerificationRequest&,const std::string& attemptId,const std::string& groupId,
        CompositeMeasurementPurpose);
    // expected is supplied by the owner or by the strict archive/native group
    // context. This result is still portable DATA, not scheduler authority or
    // admission of the group's native/geometry/measurement result.
    static ReplayedCandidateWindowProposal Replay(const CandidateWindowReferenceSet&,
        const VerificationProtocol&,const ReplayedCandidateWindowBinding&,
        const VerificationRequest& expected,const std::string& attemptId,const std::string& groupId,
        CompositeMeasurementPurpose,const CandidateWindowProposalRecord&);
private:
    static void CheckReferences(const CandidateWindowReferenceSet&,const VerificationProtocol&);
};
}
