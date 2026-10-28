#pragma once
#include "ModelIdentity.h"
#include <stdexcept>

namespace NmsdkTools {
// Same-storage native Save/Load construction for a speculative boundary edit.
// Preserves the physical graph/ports and initializes fresh dynamics. This is
// neither Alloc/Copy nor a dynamic checkpoint or a training-result certificate.
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> ClonePhysicalGraphForEdit(
    RDK::UEPtr<RDK::UContainer> model,const ModelBoundarySources& boundarySources = {});
class RDK_LIB_TYPE ModelArchiveError : public std::runtime_error
{
public:
    const std::string code;
    ModelArchiveError(const std::string& reason, const std::string& detail)
        : std::runtime_error(reason + ": " + detail), code(reason) {}
};

// Complete native physical graph plus immutable image dependency closure.
// Version 1 is closed; version 2 requires explicitly named external stimulus
// outputs. Source owners must be initialized/ready before native connection.
// metadata declares implementation_id, state_kind, training, calibration and
// conversion. This is not a physical/controller mid-probe checkpoint.
RDK_LIB_TYPE QJsonObject ExportModelArchive(RDK::UEPtr<RDK::UContainer> model, const QJsonObject& metadata,
                               const ModelBoundarySources& boundarySources = {},const std::vector<std::string>& additionalImageRoots = {});
// Validate/register the complete immutable dependency closure before creating
// caller-owned stimulus sources. Does not instantiate the archived neuron.
RDK_LIB_TYPE void PrepareModelArchive(const QJsonObject& archive,RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment,const QString& implementationId);
// Auxiliary closed DatasetMatrix graph, used only inside a complete learned
// state package whose neuron archive supplies its immutable image closure.
RDK_LIB_TYPE QJsonObject ExportDatasetSnapshot(RDK::UEPtr<RDK::UContainer> dataset);
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> ImportDatasetSnapshot(const QJsonObject& snapshot,
    RDK::UEPtr<RDK::UStorage> storage,RDK::UEPtr<RDK::UEnvironment> environment);

// Returns a newly acquired model only after native Load/Init and full identity
// verification. Caller owns the instance and must Free/ReturnObject it.
// Existing shared images are verified, never replaced. On failure, newly
// registered unused images may remain; discard that import storage before retry.
// Before returning a port-bound model or its sources, disconnect it through
// native UNet::BreakLinks; the live source pointers are caller-owned.
RDK_LIB_TYPE RDK::UEPtr<RDK::UContainer> ImportModelArchive(
    const QJsonObject& archive, RDK::UEPtr<RDK::UStorage> storage,
    RDK::UEPtr<RDK::UEnvironment> environment, const QString& implementationId,
    const ModelBoundarySources& boundarySources = {});
}
