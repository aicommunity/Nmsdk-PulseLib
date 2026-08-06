#ifndef NDATASET_CPP
#define NDATASET_CPP

#include "NDataset.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

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
    const char *key = "MaxSpikes=";
    const size_t pos = line.find(key);
    if(pos == std::string::npos)
        return false;
    max_spikes = atoi(line.c_str() + pos + std::strlen(key));
    return max_spikes >= 1;
}

/// Parse one data row: optional leading class token, then exactly num_features doubles.
/// Slot 0 rows: "class;v0;v1;..." — class_out set when parse_class is true.
/// Later slots: ";v0;v1;..." (empty class) or "v0;v1;..." — class ignored.
bool ParseFeatureRow(const std::string &line, char delimiter, int num_features,
                     bool parse_class, int &class_out, std::vector<double> &values)
{
    values.clear();
    values.reserve(num_features);

    std::stringstream stream(line);
    std::string token;

    if(parse_class)
    {
        if(!std::getline(stream, token, delimiter))
            return false;
        class_out = atoi(token.c_str());
    }
    else
    {
        // Leading empty field before first ';' (";v0;v1") — consume and ignore
        if(!line.empty() && line[0] == delimiter)
        {
            if(!std::getline(stream, token, delimiter))
                return false;
        }
    }

    while(std::getline(stream, token, delimiter) && int(values.size()) < num_features)
        values.push_back(atof(token.c_str()));

    return int(values.size()) == num_features;
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
    bool has_meta = false;
    if(!line.empty() && line[0] == '#' && line.find("NDataset") != std::string::npos)
    {
        if(!ParseMaxSpikesFromMeta(line, max_spikes))
            return false;
        has_meta = true;
        if(!std::getline(file_dataset, line))
            return false;
    }

    // Header: semicolon count == NumFeatures
    const int num_features = CountDelimiter(line, delimiter);
    if(num_features <= 0)
        return false;

    std::vector<std::string> data_lines;
    while(std::getline(file_dataset, line))
    {
        if(!line.empty())
            data_lines.push_back(line);
    }
    file_dataset.close();

    if(data_lines.empty())
        return false;
    if(int(data_lines.size()) % max_spikes != 0)
        return false;

    const int num_samples = int(data_lines.size()) / max_spikes;

    MDMatrix<double> matrix_data;
    MDMatrix<int> matrix_classes;
    matrix_data.Resize(num_samples * max_spikes, num_features, -1.0);
    matrix_classes.Resize(1, num_samples);

    for(int s = 0; s < num_samples; s++)
    {
        for(int k = 0; k < max_spikes; k++)
        {
            const std::string &row_line = data_lines[s * max_spikes + k];
            std::vector<double> values;
            int class_value = 0;
            const bool parse_class = (k == 0);
            if(!ParseFeatureRow(row_line, delimiter, num_features, parse_class, class_value, values))
                return false;

            if(k == 0)
                matrix_classes(0, s) = class_value;

            const int row = s * max_spikes + k;
            for(int f = 0; f < num_features; f++)
                matrix_data(row, f) = values[f];
        }
    }

    (void)has_meta;
    MaxSpikesPerFeature = max_spikes;
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
