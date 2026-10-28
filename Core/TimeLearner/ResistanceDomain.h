#pragma once
#include "TrainingParameters.h"
#include <cstddef>
#include <string>
#include <vector>
namespace NMSDK::TimeLearner {
enum class ResistanceDomainStatus { Available, Unavailable, Unsupported, Failed };
struct ResistanceDomainChannel {
    std::string path;
    double variableCoefficient=0.,fixedConductance=0.,minimumResistance=0.;
};
struct ResistanceDomainControl {
    std::string synapsePath;
    std::vector<std::size_t> inputIndices;
};
// Derived evidence for one candidate geometry and one variable input. Declared
// search bounds remain separate. No simulator state or owning pointers escape.
struct ResistanceDomainAssessment {
    ResistanceDomainStatus status=ResistanceDomainStatus::Unsupported;
    std::string reason,baseModelRevision,previewModelRevision;
    std::size_t inputIndex=0;
    std::vector<InputParameters> geometry;
    double minimumResistance=0.,maximumResistance=0.;
    bool lockedBySharedBank=false,modelMayHaveChanged=false;
    std::vector<ResistanceDomainChannel> channels;
    // Immutable preview identity and declared ownership allow an independent
    // auditor to derive conductances from real child physics and connections.
    // JSON stays opaque here: the numerical controller has no Qt dependency.
    std::string previewModelJson;
    std::vector<ResistanceDomainControl> controls;
    double sourceAmplitude=0.,maximumResistanceRatio=0.,configuredMinimumResistance=0.;
    bool branch=false;
};
const char* ResistanceDomainStatusName(ResistanceDomainStatus status);
}
