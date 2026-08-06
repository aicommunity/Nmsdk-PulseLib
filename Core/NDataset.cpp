#ifndef NDATASET_CPP
#define NDATASET_CPP

#include "NDataset.h"

#include <fstream>
#include <sstream>

namespace NMSDK {

NDataset::NDataset(void)
: ReloadDataset("ReloadDataset", this),
  FileName("FileName", this, &NDataset::SetFileName),
  UseRelativePathFromConfig("UseRelativePathFromConfig", this, &NDataset::SetUseRelativePathFromConfig),
  UseRelativePathFromWorkDir("UseRelativePathFromWorkDir", this, &NDataset::SetUseRelativePathFromWorkDir)
{
}

NDataset::~NDataset(void)
{
}

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

NDataset* NDataset::New(void)
{
    return new NDataset;
}

UComponent* NDataset::NewStatic(void)
{
    return new NDataset;
}

bool NDataset::ADefault(void)
{
    NDatasetBase::ADefault();
    ReloadDataset = true;
    FileName = "input_data.txt";
    UseRelativePathFromConfig = true;
    UseRelativePathFromWorkDir = false;
    return true;
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

    MatrixData = matrix_data;
    MatrixClasses = matrix_classes;
    return true;
}

bool NDataset::PrepareDataset(void)
{
    const bool ok = TreatDataFromFile();
    if(ok)
        ReloadDataset = false;
    return ok;
}

bool NDataset::ACalculate(void)
{
    if(ReloadDataset)
    {
        if(!TreatDataFromFile())
            return false;
        if(!ApplyFromMatrices())
            return false;
        ReloadDataset = false;
        SyncGenerators();
        ResetDelay = true;
    }
    return NDatasetBase::ACalculate();
}

}
#endif
