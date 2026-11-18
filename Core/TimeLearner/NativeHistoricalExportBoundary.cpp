#include "NativeHistoricalExportBoundary.h"
#include "NativeTrainingService.h"
#include "ModelIdentity.h"
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
}
NativeHistoricalExportBoundary::NativeHistoricalExportBoundary(const NativeTrainingService& source,
    QJsonObject neuron,QJsonObject dataset,std::vector<InputParameters> values)
    :service(&source),neuronIdentity(std::move(neuron)),datasetIdentity(std::move(dataset)),
     parameters(std::move(values)),epoch(source.TranscriptModelEpoch()),session(source.SessionAttempt()),
     outcome(source.Outcome()),reason(source.Reason())
{
    Require(source.RequiresTerminalSelectionArchive() && source.State()==NativeTrainingState::Finished &&
        session>0 && !source.Certificate(),"historical_export_requires_stopped_terminal_scope");
    Require(neuronIdentity.value("valid")==QJsonValue(true) && datasetIdentity.value("valid")==QJsonValue(true),
        "historical_export_requires_valid_source_graphs");
    if(source.Search())Require(neuronIdentity.value("sha256")==QJsonValue(QString::fromStdString(source.Search()->ModelRevision())) &&
        parameters==source.Search()->Parameters(),"historical_export_search_source_mismatch");
}
void NativeHistoricalExportBoundary::Validate(RDK::UNet& owner,const NativeTrainingService& source,
    const std::vector<InputParameters>& actual) const
{
    Require(service==&source && source.State()==NativeTrainingState::Finished &&
        source.RequiresTerminalSelectionArchive() && source.SessionAttempt()==session &&
        source.TranscriptModelEpoch()==epoch && source.Outcome()==outcome && source.Reason()==reason &&
        !source.Certificate() && actual==parameters,"historical_export_boundary_changed");
    auto neuron=owner.GetComponentL<NPulseNeuron>("Neuron",true);
    auto dataset=owner.GetComponentL<NDatasetMatrix>("DatasetMatrix",true);
    auto generator=owner.GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);
    Require(neuron && dataset && generator &&
        NmsdkTools::IdentifyModel(neuron.Get(),{{&generator->Output,"burst"}})==neuronIdentity &&
        NmsdkTools::IdentifyModel(dataset.Get())==datasetIdentity,"historical_export_physical_source_changed");
}
}
