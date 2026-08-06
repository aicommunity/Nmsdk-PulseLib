#ifndef NDATASETBASE_CPP
#define NDATASETBASE_CPP

#include "NDatasetBase.h"

#include <vector>
#include <algorithm>

namespace NMSDK {

NDatasetBase::NDatasetBase(void)
: PulseGeneratorClassName("PulseGeneratorClassName", this, &NDatasetBase::SetPulseGeneratorClassName),
  NumFeatures("NumFeatures", this),
  NumSamples("NumSamples", this),
  MatrixData("MatrixData", this, &NDatasetBase::SetMatrixData),
  MatrixClasses("MatrixClasses", this, &NDatasetBase::SetMatrixClasses),
  MatrixDelay("MatrixDelay", this),
  Iteration("Iteration", this),
  Tay("Tay", this),
  Delay("Delay", this, &NDatasetBase::SetDelay),
  StateGeneration("StateGeneration", this),
  TimeGeneration("TimeGeneration", this),
  OperatingTime("OperatingTime", this),
  ResetDelay("ResetDelay", this),
  SpikesFrequency("SpikesFrequency", this, &NDatasetBase::SetSpikesFrequency),
  NumClasses("NumClasses", this)
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
    for(size_t i = 0; i < Generators.size(); i++)
    {
     Generators[i]->Frequency = value;
     Generators[i]->Reset();
    }
    return true;
}

bool NDatasetBase::SetDelay(const double &value)
{
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

bool NDatasetBase::SetMatrixData(const MDMatrix<double> &value)
{
    Ready = false;
    return true;
}

bool NDatasetBase::SetMatrixClasses(const MDMatrix<int> &value)
{
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
    MatrixData.Resize(0, 0);
    MatrixClasses.Resize(0, 0);
    MatrixDelay.Resize(0, 0);
    StateGeneration = 2;
    SpikesFrequency = 5;
    Delay = 0;
    OperatingTime = 0;
    ResetDelay = true;
    TimeGeneration = 5;
    PulseGeneratorClassName = "NPulseGeneratorTransit";
    Generators.clear();
    return true;
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
        return false;
    if(!ApplyFromMatrices())
        return false;
    return SyncGenerators();
}

bool NDatasetBase::ACalculate(void)
{
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
