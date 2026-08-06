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
  MatrixSpikeDelays("MatrixSpikeDelays", this, &NDatasetBase::SetMatrixSpikeDelays),
  MatrixDelay("MatrixDelay", this),
  Iteration("Iteration", this),
  Tay("Tay", this),
  Delay("Delay", this, &NDatasetBase::SetDelay),
  StateGeneration("StateGeneration", this),
  TimeGeneration("TimeGeneration", this),
  OperatingTime("OperatingTime", this),
  ResetDelay("ResetDelay", this),
  SpikesFrequency("SpikesFrequency", this, &NDatasetBase::SetSpikesFrequency),
  NumClasses("NumClasses", this),
  SampleStartTime(0.0),
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

bool NDatasetBase::SetSpikesFrequency(const double &value)
{
    if(IsSpikeTrainMode())
        return true;

    for(size_t i = 0; i < Generators.size(); i++)
    {
     Generators[i]->Frequency = value;
     Generators[i]->Reset();
    }
    return true;
}

bool NDatasetBase::SetDelay(const double &value)
{
 if(IsSpikeTrainMode())
  return true;

 for(int i = 0; i < int(Generators.size()); i++)
 {
  if(Iteration >= 0 && Iteration < NumSamples && i < NumFeatures)
   Generators[i]->Delay = value + MatrixDelay(Iteration, i);
  else
   Generators[i]->Delay = value;
  Generators[i]->Reset();
 }
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

bool NDatasetBase::SetMatrixSpikeDelays(const MDMatrix<double> &value)
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
    Tay = float(0.01);
    Iteration = 0;
    NumClasses = 0;
    NumFeatures = 0;
    NumSamples = 0;
    MaxSpikesPerFeature = 1;
    MatrixData.Resize(0, 0);
    MatrixClasses.Resize(0, 0);
    MatrixSpikeDelays.Resize(0, 0);
    MatrixDelay.Resize(0, 0);
    StateGeneration = 2;
    SpikesFrequency = 5;
    Delay = 0;
    OperatingTime = 0;
    ResetDelay = true;
    TimeGeneration = 5;
    PulseGeneratorClassName = "NPulseGeneratorTransit";
    Generators.clear();
    SampleSpikeAbsTimes.clear();
    NextSpikeIndex.clear();
    OneShotEndTime.clear();
    SampleStartTime = 0.0;
    LastPlayedIteration = -1;
    return true;
}

bool NDatasetBase::IsSpikeTrainMode(void) const
{
    const int max_spikes = MaxSpikesPerFeature;
    if(max_spikes < 1)
        return false;

    const int rows = MatrixSpikeDelays.GetRows();
    const int cols = MatrixSpikeDelays.GetCols();
    if(rows <= 0 || cols <= 0)
        return false;

    if(max_spikes > 1)
        return true;

    // MaxSpikes == 1: use spike-train path if any non-sentinel delay exists
    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            if(MatrixSpikeDelays(r, c) >= 0.0)
                return true;
        }
    }
    return false;
}

void NDatasetBase::RebuildSpikeAbsTimesForIteration(void)
{
    const int features = NumFeatures;
    const int max_spikes = MaxSpikesPerFeature;
    SampleSpikeAbsTimes.assign(features > 0 ? features : 0, std::vector<double>());
    NextSpikeIndex.assign(features > 0 ? features : 0, 0);
    OneShotEndTime.assign(features > 0 ? features : 0, 0.0);

    if(features <= 0 || max_spikes < 1)
        return;
    if(Iteration < 0 || Iteration >= NumSamples)
        return;
    if(MatrixSpikeDelays.GetRows() <= Iteration)
        return;
    if(MatrixSpikeDelays.GetCols() < features * max_spikes)
        return;

    for(int f = 0; f < features; f++)
    {
        double t = 0.0;
        std::vector<double> &abs_times = SampleSpikeAbsTimes[f];
        abs_times.clear();
        for(int s = 0; s < max_spikes; s++)
        {
            const double d = MatrixSpikeDelays(Iteration, f * max_spikes + s);
            if(d < 0.0)
                continue;
            t += d;
            abs_times.push_back(t);
        }
        NextSpikeIndex[f] = 0;
        OneShotEndTime[f] = 0.0;
    }
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
    double freq = SpikesFrequency;
    if(freq <= 0.0)
    {
        const double pulse_len = gen->PulseLength > 0.0 ? double(gen->PulseLength) : 0.001;
        freq = 1.0 / pulse_len;
    }
    gen->Frequency = freq;
    gen->Reset();

    const double pulse_len = gen->PulseLength > 0.0 ? double(gen->PulseLength) : 0.001;
    if(feature_index >= 0 && feature_index < int(OneShotEndTime.size()))
        OneShotEndTime[feature_index] = now + pulse_len;
}

void NDatasetBase::BeginSpikeTrainSample(double now)
{
    SampleStartTime = now;
    LastPlayedIteration = Iteration;
    RebuildSpikeAbsTimesForIteration();
    SilenceGenerators();
}

void NDatasetBase::UpdateSpikeTrainPlayback(double now)
{
    const double t_rel = now - SampleStartTime;
    const double base_delay = Delay;
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

        // Wait until previous one-shot fully ended before arming the next
        if(f < int(OneShotEndTime.size()) && OneShotEndTime[f] > 0.0)
            continue;

        const double t_fire = base_delay + abs_times[next_idx];
        if(t_rel + 1e-12 >= t_fire)
            FireOneShot(f, gen, now);
    }
}

MDMatrix<double> NDatasetBase::CalcMatrixDelay(int num_samples, int num_features,
                                               const MDMatrix<double> &matrix_data,
                                               double tay) const
{
    MDMatrix<double> matrix_delay;
    matrix_delay.Resize(num_samples, num_features);
    if(num_samples <= 0 || num_features <= 0)
        return matrix_delay;

    std::vector<double> max_features(num_features);
    std::vector<double> min_features(num_features);
    for(int i = 0; i < num_features; i++)
    {
        max_features[i] = matrix_data(0, i);
        min_features[i] = matrix_data(0, i);
        for(int j = 1; j < num_samples; j++)
        {
            if(matrix_data(j, i) > max_features[i])
                max_features[i] = matrix_data(j, i);
            if(matrix_data(j, i) < min_features[i])
                min_features[i] = matrix_data(j, i);
        }
    }
    for(int i = 0; i < num_samples; i++)
    {
        for(int j = 0; j < num_features; j++)
        {
            const double range = max_features[j] - min_features[j];
            if(range == 0.0)
                matrix_delay(i, j) = 0.0;
            else
                matrix_delay(i, j) = (matrix_data(i, j) - min_features[j]) / range * tay;
        }
    }
    return matrix_delay;
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
    if(IsSpikeTrainMode())
    {
        const int max_spikes = MaxSpikesPerFeature;
        if(max_spikes < 1)
            return false;

        const int num_samples = MatrixSpikeDelays.GetRows();
        const int cols = MatrixSpikeDelays.GetCols();
        if(num_samples <= 0 || cols <= 0 || cols % max_spikes != 0)
            return false;

        const int num_features = cols / max_spikes;
        if(num_features <= 0)
            return false;
        if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != num_samples)
            return false;

        NumSamples = num_samples;
        NumFeatures = num_features;
        MaxSpikesPerFeature = max_spikes;
        MatrixDelay.Resize(0, 0);
        NumClasses = CalcNumClasses(MatrixClasses);
        LastPlayedIteration = -1;
        return true;
    }

    const int num_samples = MatrixData.GetRows();
    const int num_features = MatrixData.GetCols();
    if(num_samples <= 0 || num_features <= 0)
        return false;

    if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != num_samples)
        return false;

    NumSamples = num_samples;
    NumFeatures = num_features;
    MatrixDelay = CalcMatrixDelay(num_samples, num_features, MatrixData, Tay);
    NumClasses = CalcNumClasses(MatrixClasses);
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
    if(IsSpikeTrainMode())
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

    if(StateGeneration == 0)
    {
        for(int i = 0; i < int(Generators.size()); i++)
        {
         Generators[i]->Frequency = 0;
         Generators[i]->Reset();
        }
    }
    else if(StateGeneration == 2)
    {
        if(ResetDelay)
        {
            SetDelay(Delay);
            SetSpikesFrequency(SpikesFrequency);
            ResetDelay = false;
        }
    }
    else if(StateGeneration == 1)
    {
        if(ResetDelay)
        {
            SetDelay(Delay);
            SetSpikesFrequency(SpikesFrequency);
            OperatingTime = Environment->GetTime().GetDoubleTime();
            ResetDelay = false;
        }
        if(Environment->GetTime().GetDoubleTime() - OperatingTime > TimeGeneration)
        {
            for(int i = 0; i < int(Generators.size()); i++)
            {
             Generators[i]->Frequency = 0;
             Generators[i]->Reset();
            }
        }
    }
    else
        return false;
    return true;
}

}
#endif
