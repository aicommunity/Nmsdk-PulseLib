#pragma once
#include "NativeTrainingService.h"
#include "ModelIdentity.h"

namespace NMSDK::TimeLearner {
double NativeTrainingWallTime();

template<class Learner> NativeTrainingAdapter BindNativeTraining(Learner& owner,NativeProbeExecutor& probe,unsigned& editDepth)
{
    NativeTrainingAdapter result;
    result.trainingRequested=[&owner]{return owner.IsNeedToTrain.GetData();};
    result.readParameters=[&owner]{return owner.ReadNativeParameters();};
    auto observer=std::make_shared<NmsdkTools::ModelRevisionObserver>();
    const auto revision=[&owner,observer](bool full) {
        auto neuron=owner.template GetComponentL<NPulseNeuron>("Neuron",true);
        auto source=owner.template GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);
        if(!neuron || !source)throw std::runtime_error("missing_native_training_graph");
        return observer->Read(neuron,{{&source->Output,"burst"}},full).toStdString();
    };
    result.readModelRevision=[revision]{return revision(false);};
    result.readFullModelRevision=[revision]{return revision(true);};
    result.apply=[&owner,&editDepth](const auto& parameters) {
        NativeLifecycleScope scope(editDepth);return owner.ApplyNativeParameters(parameters);
    };
    result.assessResistanceDomain=[&owner,&editDepth](const auto& parameters,std::size_t input) {
        NativeLifecycleScope scope(editDepth);return owner.AssessNativeResistanceDomain(parameters,input);
    };
    result.queue=[&probe](NativeProbeRequest request){probe.Queue(std::move(request));};
    result.probeState=[&probe]{return probe.Status();};
    result.takeResult=[&probe]{return probe.TakeResult();};
    result.cancel=[&probe](const std::string& reason){probe.Cancel(reason);};
    return result;
}
inline bool NativeTrainingIsActive(const NativeTrainingService& service)
{return service.State()==NativeTrainingState::CapturingReferences || service.State()==NativeTrainingState::Searching;}
template<class Pattern> bool NativeTrainingPatternEqual(const Pattern& a,const Pattern& b)
{
    if(a.GetRows()!=b.GetRows() || a.GetCols()!=b.GetCols())return false;
    for(int row=0;row<a.GetRows();++row)for(int column=0;column<a.GetCols();++column)
        if(a(row,column)!=b(row,column))return false;
    return true;
}
}
