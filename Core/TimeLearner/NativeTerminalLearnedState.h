#pragma once
#include "NativeLearnedState.h"

namespace NMSDK::TimeLearner {
// Schema8 separates original search evidence from the actual terminal graph.
// Neither its DATA journal nor importing a package recreates live authority.
class NativeTerminalStateFactory {
public:
    static NativeTrainingExport Export(RDK::UNet&,const NativeModelSpec&,const NativeTrainingService&,
        const std::vector<InputParameters>&,int,int,int,const std::string&,const NativeCapacitanceCorrection*);
    static std::unique_ptr<PreparedNativeLearnedState> Prepare(const QJsonObject&,
        RDK::UEPtr<RDK::UStorage>,RDK::UEPtr<RDK::UEnvironment>,const std::string&);
    static QJsonObject Summary(const NativeTrainingService&,const std::string&,
        const std::vector<InputParameters>&);
};
}
