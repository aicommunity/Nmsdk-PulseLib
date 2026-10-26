#pragma once
#include <QJsonObject>
#include <rdk.h>
#include "ModelBoundary.h"

namespace NmsdkTools {
enum class ModelInspectionDetail { Full, Structure };
// Readback of the actual graph. Does not Build, Reset, Calculate or modify it.
RDK_LIB_TYPE QJsonObject InspectModel(RDK::UEPtr<RDK::UContainer> model,
                        const ModelBoundarySources& boundarySources = {},
                        ModelInspectionDetail detail=ModelInspectionDetail::Full);
}
