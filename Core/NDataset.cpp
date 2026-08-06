#ifndef NDATASET_CPP
#define NDATASET_CPP

#include "NDataset.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace NMSDK {

namespace {

/// Matrix of generator start delays from normalized feature values
MDMatrix<double> CalcMatrixDelay(int num_samples, int num_features,
                                 const MDMatrix<double> &matrix_data, double tay)
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

/// Number of distinct class labels
int CalcNumClasses(const MDMatrix<int> &matrix_classes)
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

} // namespace

// --------------------------
// Constructors / destructor
// --------------------------
NDataset::NDataset(void)
: PulseGeneratorClassName("PulseGeneratorClassName",this,&NDataset::SetPulseGeneratorClassName),
  NumGenerators("NumGenerators",this),
  NumFeatures("NumFeatures",this),
  NumSamples("NumSamples",this),
  MatrixData("MatrixData",this),
  MatrixClasses("MatrixClasses",this),
  ReloadDataset("ReloadDataset",this),
  MatrixDelay("MatrixDelay",this),
  Iteration("Iteration",this),
  Tay("Tay",this),
  Delay("Delay",this,&NDataset::SetDelay),
  StateGeneration("StateGeneration",this),
  TimeGeneration("TimeGeneration",this),
  OperatingTime("OperatingTime",this),
  ResetDelay("ResetDelay",this),
  SpikesFrequency("SpikesFrequency",this,&NDataset::SetSpikesFrequency),
  NumClasses("NumClasses",this),
  FileName("FileName",this,&NDataset::SetFileName),
  UseRelativePathFromConfig("UseRelativePathFromConfig",this,&NDataset::SetUseRelativePathFromConfig),
  UseRelativePathFromWorkDir("UseRelativePathFromWorkDir",this,&NDataset::SetUseRelativePathFromWorkDir)
{
  Generators.clear();
}

NDataset::~NDataset(void)
{
}

// --------------------------
// Parameter setters
// --------------------------

bool NDataset::SetFileName(const std::string &value)
{
    ReloadDataset = true;
    Ready = false;
    return true;
}

bool NDataset::SetUseRelativePathFromConfig(const bool &value)
{
    UseRelativePathFromWorkDir = false;
    ReloadDataset = true;
    Ready = false;
    return true;
}

bool NDataset::SetUseRelativePathFromWorkDir(const bool &value)
{
    UseRelativePathFromConfig = false;
    ReloadDataset = true;
    Ready = false;
    return true;
}

bool NDataset::SetPulseGeneratorClassName(const std::string &value)
{
    Ready = false;
    return true;
}

bool NDataset::SetSpikesFrequency(const double &value)
{
    for(size_t i = 0; i < Generators.size(); i++)
    {
     Generators[i]->Frequency = value;
     Generators[i]->Reset();
    }
    return true;
}

bool NDataset::SetDelay(const double &value)
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

// --------------------------
// Object factory
// --------------------------
NDataset* NDataset::New(void)
{
    return new NDataset;
}

UComponent* NDataset::NewStatic(void)
{
    return new NDataset;
}

// --------------------------
// Child component hooks
// --------------------------
bool NDataset::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
    UEPtr<NPulseGeneratorTransit> generator=dynamic_pointer_cast<NPulseGeneratorTransit>(comp);
    if(generator)
    {
      if(find(Generators.begin(),Generators.end(),generator) == Generators.end())
       Generators.push_back(generator);
    }
    return true;
}

bool NDataset::ADelComponent(UEPtr<UContainer> comp)
{
    UEPtr<NPulseGeneratorTransit> generator=dynamic_pointer_cast<NPulseGeneratorTransit>(comp);
    if(generator)
    {
     vector<NPulseGeneratorTransit*>::iterator I;
     I=find(Generators.begin(),Generators.end(),generator.Get());
     if(I != Generators.end())
      Generators.erase(I);
    }
    return true;
}

// --------------------------
// Simulation hooks
// --------------------------
bool NDataset::AReset(void)
{
    ResetDelay = true;
    return true;
}

bool NDataset::ADefault(void)
{
    Tay = float(0.01);
    Iteration = 0;
    ReloadDataset = true;
    NumClasses = 0;
    NumFeatures = 0;
    NumSamples = 0;
    NumGenerators = 0;
    MatrixData.Resize(0, 0);
    MatrixClasses.Resize(0, 0);
    MatrixDelay.Resize(0, 0);
    StateGeneration = 2;
    SpikesFrequency = 5;
    Delay = 0;
    OperatingTime = 0;
    ResetDelay = true;
    FileName = "input_data.txt";
    UseRelativePathFromConfig = true;
    UseRelativePathFromWorkDir = false;
    TimeGeneration = 5;
    PulseGeneratorClassName = "NPulseGeneratorTransit";
    Generators.clear();
    return true;
}

bool NDataset::SyncGenerators(void)
{
    int old_ex_generators = int(Generators.size());
    for(int i = NumGenerators; i < old_ex_generators; i++)
    {
     UEPtr<UContainer> gen = GetComponentL(std::string("Generator")+sntoa(i+1), true);
     if(gen)
      DelComponent(gen, true);
    }

    for(int i = 0; i < NumGenerators; i++)
    {
     UEPtr<NPulseGeneratorTransit> generator =
         AddMissingComponent<NPulseGeneratorTransit>(std::string("Generator")+sntoa(i+1), PulseGeneratorClassName);
     if(generator)
      generator->SetCoord(MVector<double,3>(5+i*6, 1.7, 0));
    }
    return true;
}

bool NDataset::ABuild(void)
{
    if(!TreatDataFromFile())
        return false;

    ReloadDataset = false;
    return SyncGenerators();
}

std::string NDataset::CalcActualSourceFilePath(const std::string &file_name)
{
 std::string clean_file_name = trim_spaces(file_name);

 if(clean_file_name.empty())
  return clean_file_name;

 if(!UseRelativePathFromConfig && !UseRelativePathFromWorkDir)
  return clean_file_name;

 std::string file_path;

 if(clean_file_name.substr(0, 2) == "./" || clean_file_name.substr(0, 2) == ".\\")
  clean_file_name = clean_file_name.substr(2);

 if(UseRelativePathFromConfig)
 {
  if(GetEnvironment())
   file_path = GetEnvironment()->GetCurrentDataDir() + clean_file_name;
 }

 if(UseRelativePathFromWorkDir)
 {
  if(GetEnvironment())
   file_path = GetEnvironment()->GetSystemDir() + clean_file_name;
 }
 return file_path;
}

bool NDataset::TreatDataFromFile(void)
{
    const std::string path = CalcActualSourceFilePath(FileName);
    if(path.empty())
        return false;

    std::ifstream file_dataset(path.c_str());
    if(!file_dataset)
        return false;

    std::string line;
    const char delimiter = ';';
    if(!std::getline(file_dataset, line))
        return false;

    int num_features = 0;
    for(size_t i = 0; i < line.length(); i++)
    {
        if(line[i] == ';')
            num_features++;
    }

    int num_samples = 0;
    while(std::getline(file_dataset, line))
        num_samples++;

    if(num_features <= 0 || num_samples <= 0)
        return false;

    file_dataset.close();
    file_dataset.clear();
    file_dataset.open(path.c_str());
    if(!file_dataset)
        return false;

    MDMatrix<double> matrix_data;
    MDMatrix<int> matrix_classes;
    matrix_data.Resize(num_samples, num_features);
    matrix_classes.Resize(1, num_samples);

    if(!std::getline(file_dataset, line))
        return false;

    int row = 0;
    while(std::getline(file_dataset, line) && row < num_samples)
    {
        int col = 0;
        std::stringstream stream(line);
        std::string number;
        if(!std::getline(stream, number, delimiter))
            return false;
        matrix_classes(0, row) = atoi(number.c_str());
        std::string count;
        while(std::getline(stream, count, delimiter) && col < num_features)
        {
            matrix_data(row, col) = atof(count.c_str());
            col++;
        }
        row++;
    }
    file_dataset.close();

    if(row != num_samples)
        return false;

    MDMatrix<double> matrix_delay = CalcMatrixDelay(num_samples, num_features, matrix_data, Tay);
    const int num_classes = CalcNumClasses(matrix_classes);

    NumFeatures = num_features;
    NumSamples = num_samples;
    MatrixData = matrix_data;
    MatrixClasses = matrix_classes;
    MatrixDelay = matrix_delay;
    NumClasses = num_classes;
    NumGenerators = num_features;
    return true;
}

bool NDataset::ACalculate(void)
{
    if(ReloadDataset)
    {
        if(!TreatDataFromFile())
            return false;
        ReloadDataset = false;
        SyncGenerators();
        ResetDelay = true;
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
