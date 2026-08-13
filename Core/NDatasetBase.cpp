#ifndef NDATASETBASE_CPP
#define NDATASETBASE_CPP

#include "NDatasetBase.h"

#include <vector>
#include <algorithm>

namespace NMSDK {

NDatasetBase::NDatasetBase(void)
: PulseGeneratorClassName("PulseGeneratorClassName", this, &NDatasetBase::SetPulseGeneratorClassName),
  NumFeatures("NumFeatures", this, &NDatasetBase::SetNumFeatures),
  NumSamples("NumSamples", this, &NDatasetBase::SetNumSamples),
  MaxSpikesPerFeature("MaxSpikesPerFeature", this, &NDatasetBase::SetMaxSpikesPerFeature),
  MatrixData("MatrixData", this, &NDatasetBase::SetMatrixData),
  MatrixClasses("MatrixClasses", this, &NDatasetBase::SetMatrixClasses),
  Iteration("Iteration", this),
  Delay("Delay", this, &NDatasetBase::SetDelay),
  AdvanceSampleAfterBurst("AdvanceSampleAfterBurst", this),
  LoopSamples("LoopSamples", this),
  CurrentClass("CurrentClass", this),
  CurrentClassMatrix("CurrentClassMatrix", this),
  StateGeneration("StateGeneration", this),
  TimeGeneration("TimeGeneration", this),
  OperatingTime("OperatingTime", this),
  ResetDelay("ResetDelay", this),
  NumClasses("NumClasses", this),
  SampleStartTime(0.0),
  SampleBurstEndRel(0.0),
  LastPlayedIteration(-1)
{
  Generators.clear();
}

NDatasetBase::~NDatasetBase(void)
{
}

bool NDatasetBase::SetPulseGeneratorClassName(const std::string &value)
{
    Ready = false;
    return true;
}

bool NDatasetBase::SetDelay(const double &value)
{
    (void)value;
    return true;
}

bool NDatasetBase::SetNumFeatures(const int &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetBase::SetNumSamples(const int &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetBase::SetMaxSpikesPerFeature(const int &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetBase::SetMatrixData(const MDMatrix<double> &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetBase::SetMatrixClasses(const MDMatrix<int> &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetBase::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
    UEPtr<NPulseGeneratorTransit> generator = dynamic_pointer_cast<NPulseGeneratorTransit>(comp);
    if(generator)
    {
      if(find(Generators.begin(), Generators.end(), generator) == Generators.end())
       Generators.push_back(generator);
    }
    return true;
}

bool NDatasetBase::ADelComponent(UEPtr<UContainer> comp)
{
    UEPtr<NPulseGeneratorTransit> generator = dynamic_pointer_cast<NPulseGeneratorTransit>(comp);
    if(generator)
    {
     vector<NPulseGeneratorTransit*>::iterator I;
     I = find(Generators.begin(), Generators.end(), generator.Get());
     if(I != Generators.end())
      Generators.erase(I);
    }
    return true;
}

bool NDatasetBase::AReset(void)
{
    ResetDelay = true;
    return true;
}

bool NDatasetBase::ADefault(void)
{
    Iteration = 0;
    NumClasses = 0;
    NumFeatures = 0;
    NumSamples = 0;
    MaxSpikesPerFeature = 1;
    MatrixData.Resize(0, 0);
    MatrixClasses.Resize(0, 0);
    StateGeneration = 2;
    Delay = 0;
    AdvanceSampleAfterBurst = false;
    LoopSamples = true;
    CurrentClass = 0;
    CurrentClassMatrix.Resize(1, 1, 0.0);
    OperatingTime = 0;
    ResetDelay = true;
    TimeGeneration = 5;
    PulseGeneratorClassName = "NPulseGeneratorTransit";
    Generators.clear();
    SampleSpikeAbsTimes.clear();
    NextSpikeIndex.clear();
    OneShotEndTime.clear();
    SampleStartTime = 0.0;
    SampleBurstEndRel = 0.0;
    LastPlayedIteration = -1;
    return true;
}

int NDatasetBase::SampleSlotRow(int sample, int spike_slot) const
{
    return sample * int(MaxSpikesPerFeature) + spike_slot;
}

void NDatasetBase::RebuildSpikeAbsTimesForIteration(void)
{
    const int features = NumFeatures;
    const int max_spikes = MaxSpikesPerFeature;
    const int num_samples = NumSamples;
    SampleSpikeAbsTimes.assign(features > 0 ? features : 0, std::vector<double>());
    NextSpikeIndex.assign(features > 0 ? features : 0, 0);
    OneShotEndTime.assign(features > 0 ? features : 0, 0.0);
    SampleBurstEndRel = 0.0;

    if(features <= 0 || max_spikes < 1)
        return;
    if(Iteration < 0 || Iteration >= num_samples)
        return;

    const int base = SampleSlotRow(Iteration, 0);
    if(MatrixData.GetCols() < features)
        return;
    if(MatrixData.GetRows() < base + max_spikes)
        return;

    double max_abs = 0.0;
    bool any_spike = false;
    for(int f = 0; f < features; f++)
    {
        double t = 0.0;
        std::vector<double> &abs_times = SampleSpikeAbsTimes[f];
        abs_times.clear();
        for(int s = 0; s < max_spikes; s++)
        {
            const double d = MatrixData(base + s, f);
            if(d < 0.0)
                continue;
            t += d;
            abs_times.push_back(t);
            any_spike = true;
            if(t > max_abs)
                max_abs = t;
        }
        NextSpikeIndex[f] = 0;
        OneShotEndTime[f] = 0.0;
    }

    double max_pulse = 0.001;
    for(int i = 0; i < int(Generators.size()); i++)
    {
        if(!Generators[i])
            continue;
        const double pl = Generators[i]->PulseLength > 0.0 ? double(Generators[i]->PulseLength) : 0.001;
        if(pl > max_pulse)
            max_pulse = pl;
    }
    SampleBurstEndRel = any_spike ? (max_abs + max_pulse) : 0.0;
}

void NDatasetBase::SilenceGenerators(void)
{
    for(int i = 0; i < int(Generators.size()); i++)
    {
        Generators[i]->Frequency = 0.0;
        Generators[i]->Reset();
    }
}

void NDatasetBase::FireOneShot(int feature_index, NPulseGeneratorTransit* gen, double now)
{
    if(!gen)
        return;

    gen->Delay = 0.0;
    const double pulse_len = gen->PulseLength > 0.0 ? double(gen->PulseLength) : 0.001;
    gen->Frequency = 1.0 / pulse_len;
    gen->Reset();

    if(feature_index >= 0 && feature_index < int(OneShotEndTime.size()))
        OneShotEndTime[feature_index] = now + pulse_len;
}

void NDatasetBase::BeginSpikeTrainSample(double now)
{
    if(NumSamples > 0 && Iteration >= NumSamples)
        Iteration.SetDataDirect(0);
    if(Iteration < 0)
        Iteration.SetDataDirect(0);

    SampleStartTime = now;
    LastPlayedIteration = Iteration;

    int cls = 0;
    if(MatrixClasses.GetRows() >= 1 && MatrixClasses.GetCols() > int(Iteration)
       && int(Iteration) >= 0)
        cls = MatrixClasses(0, int(Iteration));
    CurrentClass.SetDataDirect(cls);
    CurrentClassMatrix.Resize(1, 1, 0.0);
    CurrentClassMatrix(0, 0) = double(cls);

    RebuildSpikeAbsTimesForIteration();
    SilenceGenerators();
}

bool NDatasetBase::IsBurstFullyComplete(void) const
{
    const int features = std::min(int(NumFeatures), int(SampleSpikeAbsTimes.size()));
    for(int f = 0; f < features; f++)
    {
        if(f < int(OneShotEndTime.size()) && OneShotEndTime[f] > 0.0)
            return false;
        if(f < int(NextSpikeIndex.size()) &&
           NextSpikeIndex[f] < int(SampleSpikeAbsTimes[f].size()))
            return false;
    }
    return true;
}

void NDatasetBase::UpdateSpikeTrainPlayback(double now)
{
    const double t_rel = now - SampleStartTime;
    const int features = std::min(int(NumFeatures), int(Generators.size()));

    for(int f = 0; f < features; f++)
    {
        NPulseGeneratorTransit* gen = Generators[f];
        if(!gen)
            continue;

        if(f < int(OneShotEndTime.size()) && OneShotEndTime[f] > 0.0)
        {
            if(now < OneShotEndTime[f])
                continue;
            gen->Frequency = 0.0;
            OneShotEndTime[f] = 0.0;
            if(f < int(NextSpikeIndex.size()))
                NextSpikeIndex[f]++;
        }

        if(f >= int(SampleSpikeAbsTimes.size()) || f >= int(NextSpikeIndex.size()))
            continue;

        const std::vector<double> &abs_times = SampleSpikeAbsTimes[f];
        int &next_idx = NextSpikeIndex[f];
        if(next_idx < 0 || next_idx >= int(abs_times.size()))
            continue;

        if(f < int(OneShotEndTime.size()) && OneShotEndTime[f] > 0.0)
            continue;

        const double t_fire = abs_times[next_idx];
        if(t_rel + 1e-12 >= t_fire)
            FireOneShot(f, gen, now);
    }

    const double repeat_at = SampleBurstEndRel + (Delay > 0.0 ? double(Delay) : 0.0);
    if(IsBurstFullyComplete() && t_rel + 1e-12 >= repeat_at)
    {
        if(AdvanceSampleAfterBurst && NumSamples > 0)
        {
            const int next = int(Iteration) + 1;
            if(next >= int(NumSamples))
            {
                if(!LoopSamples)
                {
                    StateGeneration = 0;
                    SilenceGenerators();
                    return;
                }
                Iteration.SetDataDirect(0);
            }
            else
                Iteration.SetDataDirect(next);
        }
        BeginSpikeTrainSample(now);
    }
}

int NDatasetBase::CalcNumClasses(const MDMatrix<int> &matrix_classes) const
{
    std::vector<int> uniq;
    for(int i = 0; i < matrix_classes.GetCols(); i++)
    {
        bool flag_insert = true;
        for(int j = 0; j < int(uniq.size()); j++)
        {
            if(matrix_classes(0, i) == uniq[j])
                flag_insert = false;
        }
        if(flag_insert)
            uniq.push_back(matrix_classes(0, i));
    }
    return int(uniq.size());
}

bool NDatasetBase::ApplyFromMatrices(void)
{
    const int max_spikes = MaxSpikesPerFeature;
    if(max_spikes < 1)
        return false;

    const int rows = MatrixData.GetRows();
    const int cols = MatrixData.GetCols();
    if(rows <= 0 || cols <= 0)
        return false;
    if(rows % max_spikes != 0)
        return false;

    const int num_samples = rows / max_spikes;
    const int num_features = cols;
    if(num_samples <= 0 || num_features <= 0)
        return false;
    if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != num_samples)
        return false;

    NumSamples = num_samples;
    NumFeatures = num_features;
    NumClasses = CalcNumClasses(MatrixClasses);
    LastPlayedIteration = -1;
    return true;
}

bool NDatasetBase::SyncGenerators(void)
{
    int old_ex_generators = int(Generators.size());
    for(int i = NumFeatures; i < old_ex_generators; i++)
    {
     UEPtr<UContainer> gen = GetComponentL(std::string("Generator")+sntoa(i+1), true);
     if(gen)
      DelComponent(gen, true);
    }

    for(int i = 0; i < NumFeatures; i++)
    {
     UEPtr<NPulseGeneratorTransit> generator =
         AddMissingComponent<NPulseGeneratorTransit>(std::string("Generator")+sntoa(i+1), PulseGeneratorClassName);
     if(generator)
      generator->SetCoord(MVector<double,3>(5+i*6, 1.7, 0));
    }
    return true;
}

bool NDatasetBase::ABuild(void)
{
    if(!PrepareDataset())
    {
        const std::string msg = std::string("NDatasetBase::ABuild failed: PrepareDataset rejected data on ")
            + GetName();
        LogMessageEx(RDK_EX_ERROR, __FUNCTION__, msg);
        RDK_THROW(EStringError(msg));
    }

    // Empty matrices = idle (e.g. file source not present yet); valid built state.
    if(MatrixData.GetRows() <= 0 || MatrixData.GetCols() <= 0)
    {
        NumSamples = 0;
        NumFeatures = 0;
        NumClasses = 0;
        LastPlayedIteration = -1;
        return SyncGenerators();
    }

    if(!ApplyFromMatrices())
    {
        const std::string msg = std::string("NDatasetBase::ABuild failed: ApplyFromMatrices rejected data on ")
            + GetName();
        LogMessageEx(RDK_EX_ERROR, __FUNCTION__, msg);
        RDK_THROW(EStringError(msg));
    }
    return SyncGenerators();
}

bool NDatasetBase::ACalculate(void)
{
    if(StateGeneration == 0)
    {
        SilenceGenerators();
        return true;
    }

    const double now = Environment->GetTime().GetDoubleTime();
    const bool iteration_changed = (LastPlayedIteration != int(Iteration));

    if(StateGeneration == 2)
    {
        if(ResetDelay || iteration_changed)
        {
            BeginSpikeTrainSample(now);
            ResetDelay = false;
        }
        UpdateSpikeTrainPlayback(now);
        return true;
    }

    if(StateGeneration == 1)
    {
        if(ResetDelay || iteration_changed)
        {
            BeginSpikeTrainSample(now);
            OperatingTime = now;
            ResetDelay = false;
        }
        if(now - OperatingTime > TimeGeneration)
        {
            SilenceGenerators();
            return true;
        }
        UpdateSpikeTrainPlayback(now);
        return true;
    }
    return false;
}

}
#endif
