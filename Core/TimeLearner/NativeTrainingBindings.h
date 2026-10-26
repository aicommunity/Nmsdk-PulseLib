#pragma once
#include "NativeTrainingService.h"
#include "ModelIdentity.h"

namespace NMSDK::TimeLearner {
double NativeTrainingWallTime();

template<class Learner> NativeTrainingAdapter BindNativeTraining(Learner& owner,NativeProbeExecutor& probe,unsigned& editDepth)
{
    NativeTrainingAdapter result;
    result.readParameters=[&owner]{return owner.ReadNativeParameters();};
    result.readModelRevision=[&owner] {
        auto neuron=owner.template GetComponentL<NPulseNeuron>("Neuron",true);
        auto source=owner.template GetComponentL<NPulseGeneratorTransit>("DatasetMatrix.Generator1",true);
        if(!neuron || !source)throw std::runtime_error("missing_native_training_graph");
        const auto identity=NmsdkTools::IdentifyModel(neuron,{{&source->Output,"burst"}});
        if(!identity["valid"].toBool())throw std::runtime_error("invalid_native_training_model_identity");
        return identity["sha256"].toString().toStdString();
    };
    result.apply=[&owner,&editDepth](const auto& parameters) {
        NativeLifecycleScope scope(editDepth);return owner.ApplyNativeParameters(parameters);
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
