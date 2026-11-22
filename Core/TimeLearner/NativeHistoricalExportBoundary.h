#pragma once
#include "NativeModelBuilder.h"

namespace NMSDK::TimeLearner {
class NativeTrainingService;
// Read-only source-graph export context. Only the versioned terminal factory
// may create it; it grants no edit, probe, scheduler or continuation authority.
// Historical measurement evidence is exported against its own physical source
// and epoch, never against the selected destination's new physical epoch.
class NativeHistoricalExportBoundary {
public:
    void Validate(RDK::UNet& owner,const NativeTrainingService&,
        const std::vector<InputParameters>&) const;
    std::size_t ModelEpoch() const {return epoch;}
private:
    friend class NativeTerminalStateFactory;
    NativeHistoricalExportBoundary(const NativeTrainingService&,QJsonObject neuron,
        QJsonObject dataset,std::vector<InputParameters> parameters);
    const NativeTrainingService* service=nullptr;
    QJsonObject neuronIdentity,datasetIdentity;
    std::vector<InputParameters> parameters;
    std::size_t epoch=0,session=0;
    bool certified=false;
    TrainingStatus outcome;
    std::string reason;
};
}
