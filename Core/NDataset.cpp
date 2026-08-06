#ifndef NDATASET_CPP
#define NDATASET_CPP

#include "NDataset.h"

#include <fstream>
#include <sstream>
#include <cstring>

namespace NMSDK {

namespace {

int CountDelimiter(const std::string &line, char delimiter)
{
    int count = 0;
    for(size_t i = 0; i < line.length(); i++)
    {
        if(line[i] == delimiter)
            count++;
    }
    return count;
}

bool ParseMaxSpikesFromMeta(const std::string &line, int &max_spikes)
{
    // Expected: #NDataset;MaxSpikes=N  (extra fields allowed after)
    const char *key = "MaxSpikes=";
    const size_t pos = line.find(key);
    if(pos == std::string::npos)
        return false;
    max_spikes = atoi(line.c_str() + pos + std::strlen(key));
    return max_spikes >= 1;
}

} // namespace

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

    int max_spikes = 1;
    bool is_v2 = false;
    if(!line.empty() && line[0] == '#' && line.find("NDataset") != std::string::npos)
    {
        if(!ParseMaxSpikesFromMeta(line, max_spikes))
            return false;
        is_v2 = true;
        if(!std::getline(file_dataset, line))
            return false;
    }

    const int header_semicolons = CountDelimiter(line, delimiter);
    if(header_semicolons <= 0)
        return false;

    int num_features = 0;
    int delay_cols = 0;
    if(is_v2)
    {
        if(header_semicolons % max_spikes != 0)
            return false;
        num_features = header_semicolons / max_spikes;
        delay_cols = header_semicolons;
    }
    else
    {
        num_features = header_semicolons;
        delay_cols = 0;
    }

    if(num_features <= 0)
        return false;

    int num_samples = 0;
    while(std::getline(file_dataset, line))
        num_samples++;

    if(num_samples <= 0)
        return false;

    file_dataset.close();
    file_dataset.clear();
    file_dataset.open(path.c_str());
    if(!file_dataset)
        return false;

    // Skip meta (v2) and header
    if(!std::getline(file_dataset, line))
        return false;
    if(is_v2)
    {
        if(!std::getline(file_dataset, line))
            return false;
    }

    MDMatrix<double> matrix_data;
    MDMatrix<double> matrix_spike_delays;
    MDMatrix<int> matrix_classes;
    matrix_classes.Resize(1, num_samples);

    if(is_v2)
    {
        matrix_spike_delays.Resize(num_samples, delay_cols, -1.0);
        matrix_data.Resize(0, 0);
    }
    else
    {
        matrix_data.Resize(num_samples, num_features);
        matrix_spike_delays.Resize(0, 0);
    }

    int row = 0;
    while(std::getline(file_dataset, line) && row < num_samples)
    {
        std::stringstream stream(line);
        std::string number;
        if(!std::getline(stream, number, delimiter))
            return false;
        matrix_classes(0, row) = atoi(number.c_str());

        if(is_v2)
        {
            int col = 0;
            std::string count;
            while(std::getline(stream, count, delimiter) && col < delay_cols)
            {
                matrix_spike_delays(row, col) = atof(count.c_str());
                col++;
            }
            if(col != delay_cols)
                return false;
        }
        else
        {
            int col = 0;
            std::string count;
            while(std::getline(stream, count, delimiter) && col < num_features)
            {
                matrix_data(row, col) = atof(count.c_str());
                col++;
            }
        }
        row++;
    }
    file_dataset.close();

    if(row != num_samples)
        return false;

    MaxSpikesPerFeature = max_spikes;
    MatrixClasses = matrix_classes;
    if(is_v2)
    {
        MatrixSpikeDelays = matrix_spike_delays;
        MatrixData.Resize(0, 0);
    }
    else
    {
        MatrixData = matrix_data;
        MatrixSpikeDelays.Resize(0, 0);
    }

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
