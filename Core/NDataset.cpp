#ifndef NDATASET_CPP
#define NDATASET_CPP

#include "NDataset.h"

#include <fstream>
#include <vector>
#include <charconv>
#include <cmath>
#include <limits>
#include <system_error>

namespace NMSDK {

namespace {

bool IsSpace(char value)
{
    return value==' ' || value=='\t' || value=='\r' || value=='\n' || value=='\f' || value=='\v';
}

std::string TrimToken(const std::string& value)
{
    const auto first=value.find_first_not_of(" \t\r\n\f\v");
    if(first==std::string::npos)return {};
    return value.substr(first,value.find_last_not_of(" \t\r\n\f\v")-first+1);
}

bool ParseInteger(std::string token,int& value)
{
    token=TrimToken(token);
    if(!token.empty() && token[0]=='+')
    {
        token.erase(0,1);
        if(!token.empty() && (token[0]=='+' || token[0]=='-'))return false;
    }
    if(token.empty())return false;
    const auto parsed=std::from_chars(token.data(),token.data()+token.size(),value);
    return parsed.ec==std::errc{} && parsed.ptr==token.data()+token.size();
}

bool ParseFiniteDouble(std::string token,double& value)
{
    token=TrimToken(token);
    if(!token.empty() && token[0]=='+')
    {
        token.erase(0,1);
        if(!token.empty() && (token[0]=='+' || token[0]=='-'))return false;
    }
    if(token.empty())return false;
    const auto parsed=std::from_chars(token.data(),token.data()+token.size(),value,std::chars_format::general);
    return parsed.ec==std::errc{} && parsed.ptr==token.data()+token.size() && std::isfinite(value);
}

int CountDelimiter(const std::string &line, char delimiter)
{
    int count = 0;
    for(size_t i = 0; i < line.length(); i++)
    {
        if(line[i] == delimiter)
        {
            if(count==std::numeric_limits<int>::max())return -1;
            count++;
        }
    }
    return count;
}

bool ParseMaxSpikesFromMeta(const std::string &line, int &max_spikes)
{
    auto text=TrimToken(line);
    if(text.empty() || text[0]!='#')return false;
    text=TrimToken(text.substr(1));
    const std::string name="NDataset",key="MaxSpikes=";
    if(text.compare(0,name.size(),name)!=0)return false;
    text=text.substr(name.size());
    if(text.empty())return false;
    if(text[0]==';')text=TrimToken(text.substr(1));
    else if(IsSpace(text[0]))text=TrimToken(text);
    else return false;
    if(text.compare(0,key.size(),key)!=0)return false;
    return ParseInteger(text.substr(key.size()),max_spikes) && max_spikes>=1;
}

/// Parse one data row: optional leading class token, then exactly num_features doubles.
/// Slot 0 rows: "class;v0;v1;..." — class_out set when parse_class is true.
/// Later slots: ";v0;v1;..." (empty class) or "v0;v1;..." — class ignored.
bool ParseFeatureRow(const std::string &line, char delimiter, int num_features,
                     bool parse_class, int &class_out, std::vector<double> &values)
{
    values.clear();
    values.reserve(num_features);

    // Preserve empty terminal fields: stringstream/getline alone would hide
    // a trailing delimiter. Every supplied field must have a declared owner.
    std::vector<std::string> fields;
    size_t begin=0;
    for(;;)
    {
        const auto end=line.find(delimiter,begin);
        fields.push_back(line.substr(begin,end==std::string::npos?end:end-begin));
        if(end==std::string::npos)break;
        begin=end+1;
    }
    size_t offset=0;
    if(parse_class)
    {
        if(fields.empty() || !ParseInteger(fields[0],class_out))return false;
        offset=1;
    }
    else if(fields.size()==size_t(num_features)+1 && TrimToken(fields[0]).empty())offset=1;
    if(fields.size()!=size_t(num_features)+offset)return false;
    for(size_t index=offset;index<fields.size();++index)
    {
        double value=0.;
        if(!ParseFiniteDouble(fields[index],value))return false;
        values.push_back(value);
    }
    return true;
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

NDataset::FileLoadResult NDataset::TreatDataFromFile(void)
{
    const std::string path = CalcActualSourceFilePath(FileName);
    if(path.empty())
    {
        MatrixData.Resize(0, 0);
        MatrixClasses.Resize(0, 0);
        return FileLoadResult::Unavailable;
    }

    std::ifstream file_dataset(path.c_str());
    if(!file_dataset)
    {
        MatrixData.Resize(0, 0);
        MatrixClasses.Resize(0, 0);
        return FileLoadResult::Unavailable;
    }

    std::string line;
    const char delimiter = ';';
    if(!std::getline(file_dataset, line))
        return FileLoadResult::Invalid;

    int max_spikes = 1;
    // UTF-8 BOM and ASCII whitespace are transport formatting, not data.
    if(line.compare(0,3,"\xEF\xBB\xBF")==0)line.erase(0,3);
    line=TrimToken(line);
    if(!line.empty() && line[0] == '#')
    {
        if(!ParseMaxSpikesFromMeta(line, max_spikes))
            return FileLoadResult::Invalid;
        if(!std::getline(file_dataset, line))
            return FileLoadResult::Invalid;
    }

    line=TrimToken(line);
    if(line.empty() || line[0]=='#')return FileLoadResult::Invalid;

    // Header: semicolon count == NumFeatures
    const int num_features = CountDelimiter(line, delimiter);
    if(num_features <= 0)
        return FileLoadResult::Invalid;

    std::vector<std::string> data_lines;
    while(std::getline(file_dataset, line))
    {
        if(!TrimToken(line).empty())
            data_lines.push_back(line);
    }
    file_dataset.close();

    if(data_lines.empty())
        return FileLoadResult::Invalid;
    // MDMatrix dimensions/indexing are ints; validate products before casting
    // or allocating. This is a representation bound, not an experiment limit.
    if(data_lines.size()>size_t(std::numeric_limits<int>::max())/size_t(num_features))
        return FileLoadResult::Invalid;
    if(int(data_lines.size()) % max_spikes != 0)
        return FileLoadResult::Invalid;

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
                return FileLoadResult::Invalid;

            if(k == 0)
                matrix_classes(0, s) = class_value;

            const int row = s * max_spikes + k;
            for(int f = 0; f < num_features; f++)
                matrix_data(row, f) = values[f];
        }
    }

    MaxSpikesPerFeature = max_spikes;
    MatrixData = matrix_data;
    MatrixClasses = matrix_classes;
    return FileLoadResult::Loaded;
}

bool NDataset::PrepareDataset(void)
{
    const FileLoadResult result = TreatDataFromFile();
    if(result == FileLoadResult::Invalid)
    {
        // A rejected reload must not leave a previously high output visible
        // to consumers while native Build/Calculate report the failure. Keep
        // the last accepted matrix for inspection; it cannot drive playback
        // until a valid native rebuild resets and rearms the source.
        SilenceGenerators();
        return false;
    }
    if(result == FileLoadResult::Loaded)
        ReloadDataset = false;
    // Unavailable: empty matrices, ReloadDataset stays true for a later retry
    return true;
}

void NDataset::PreparePlaybackData(void)
{
    if(ReloadDataset)
    {
        // File loading changes matrices and the generator graph. Complete the
        // owner's native Build/AfterBuild/Reset transaction before arming this
        // tick. Applying matrices here and leaving Ready=false makes the next
        // tick's automatic Build reset playback again, shifting onset by dt.
        // ABuild owns parsing/validation and SyncGenerators owns initialization
        // of runtime-created children. Missing files remain idle/retryable.
        Ready = false;
        const auto marker=OperationFailureSequence();
        Build();
        if(!IsReady() || HasOperationFailureSince(marker))
            RDK_THROW(EStringError("dataset_playback_reload_build_failed"));
    }
}

}
#endif
