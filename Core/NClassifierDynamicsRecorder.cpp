#include "NClassifierDynamicsRecorder.h"

#include <filesystem>
#include <iomanip>
#include <sstream>

namespace NMSDK {

NClassifierDynamicsRecorder::NClassifierDynamicsRecorder(void)
 : Signals("Signals", this),
   SignalNamesCsv("SignalNamesCsv", this),
   SavePath("SavePath", this),
   FileName("FileName", this),
   SampleInterval("SampleInterval", this),
   FlushInterval("FlushInterval", this),
   AppendMode("AppendMode", this),
   Enable("Enable", this),
   LastError("LastError", this),
   SamplesWritten("SamplesWritten", this),
   last_sample_time_(-1.0),
   last_flush_time_(0.0)
{
}

NClassifierDynamicsRecorder::~NClassifierDynamicsRecorder(void)
{
 CloseCsv();
}

NClassifierDynamicsRecorder* NClassifierDynamicsRecorder::New(void)
{
 return new NClassifierDynamicsRecorder;
}

UComponent* NClassifierDynamicsRecorder::NewStatic(void)
{
 return new NClassifierDynamicsRecorder;
}

bool NClassifierDynamicsRecorder::ADefault(void)
{
 CloseCsv();
 SignalNamesCsv = "";
 SavePath = "ClassifierDynamics";
 FileName = "signals.csv";
 SampleInterval = 0.0005;
 FlushInterval = 0.01;
 AppendMode = false;
 Enable = false;
 LastError = "";
 SamplesWritten = 0;
 last_sample_time_ = -1.0;
 last_flush_time_ = 0.0;
 return true;
}

bool NClassifierDynamicsRecorder::ABuild(void)
{
 return true;
}

std::string NClassifierDynamicsRecorder::FullCsvPath(void) const
{
 std::string base = Environment ? Environment->GetCurrentDataDir() : std::string();
 const std::string relative = std::string(SavePath);
 if(!relative.empty())
 {
  if(!base.empty() && base[base.size() - 1] != '/' && base[base.size() - 1] != '\\')
   base += "/";
  base += relative;
 }
 if(!base.empty() && base[base.size() - 1] != '/' && base[base.size() - 1] != '\\')
  base += "/";
 return base + std::string(FileName);
}

std::vector<std::string> NClassifierDynamicsRecorder::SignalNames(void) const
{
 std::vector<std::string> names;
 std::stringstream input{std::string(SignalNamesCsv)};
 std::string name;
 while(std::getline(input, name, ','))
 {
  const size_t first = name.find_first_not_of(" \t\r\n");
  const size_t last = name.find_last_not_of(" \t\r\n");
  names.push_back(first == std::string::npos ? std::string() : name.substr(first, last - first + 1));
 }
 return names;
}

bool NClassifierDynamicsRecorder::EnsureCsvReady(void)
{
 if(csv_.is_open())
  return true;

 csv_full_path_ = FullCsvPath();
 try
 {
  const std::filesystem::path path(csv_full_path_);
  if(path.has_parent_path())
   std::filesystem::create_directories(path.parent_path());
 }
 catch(const std::exception &e)
 {
  LastError = std::string("Cannot create recorder directory: ") + e.what();
  return false;
 }

 const bool append = bool(AppendMode);
 const bool existing_nonempty = append
  && std::filesystem::exists(csv_full_path_)
  && std::filesystem::file_size(csv_full_path_) > 0;
 csv_.open(csv_full_path_.c_str(), std::ios::out | (append ? std::ios::app : std::ios::trunc));
 if(!csv_.is_open())
 {
  LastError = std::string("Cannot open recorder file: ") + csv_full_path_;
  return false;
 }

 csv_ << std::setprecision(17);
 if(!existing_nonempty)
 {
  csv_ << "model_time";
  const std::vector<std::string> names = SignalNames();
  const size_t count = Signals.GetData().size();
  for(size_t i = 0; i < count; ++i)
  {
   std::string name = (i < names.size() && !names[i].empty()) ? names[i] : "signal_" + std::to_string(i + 1);
   // Keep the source path beside the user label. The connected-output order is
   // the same order used by Signals.GetData(), while hand-maintained labels can
   // become stale when a saved model restores links in a different order.
   const std::string source_name = Signals.GetItemFullName(static_cast<int>(i));
   const std::string source_property = Signals.GetItemOutputName(static_cast<int>(i));
   if(!source_name.empty())
   {
    name += " [" + source_name;
    if(!source_property.empty())
     name += "." + source_property;
    name += "]";
   }
   for(size_t pos = 0; pos < name.size(); ++pos)
    if(name[pos] == '"')
     name.insert(pos++, 1, '"');
   csv_ << ",\"" << name << "\"";
  }
  csv_ << "\n";
 }
 LastError = "";
 return true;
}

void NClassifierDynamicsRecorder::CloseCsv(void)
{
 if(csv_.is_open())
 {
  csv_.flush();
  csv_.close();
 }
}

bool NClassifierDynamicsRecorder::AReset(void)
{
 CloseCsv();
 last_sample_time_ = -1.0;
 last_flush_time_ = Environment ? Environment->GetTime().GetDoubleTime() : 0.0;
 SamplesWritten = 0;
 LastError = "";
 csv_full_path_.clear();
 return true;
}

bool NClassifierDynamicsRecorder::ACalculate(void)
{
 if(!Enable)
  return true;

 const double now = Environment ? Environment->GetTime().GetDoubleTime() : 0.0;
 if(last_sample_time_ >= 0.0 && SampleInterval > 0.0
    && now - last_sample_time_ + 1e-12 < SampleInterval)
  return true;
 if(!EnsureCsvReady())
  return true;

 csv_ << now;
 const std::vector<MDMatrix<double> > &signals = Signals.GetData();
 for(size_t i = 0; i < signals.size(); ++i)
 {
  const MDMatrix<double> &signal = signals[i];
  csv_ << ',';
  if(signal.GetRows() > 0 && signal.GetCols() > 0)
   csv_ << signal(0, 0);
  else
   csv_ << 0.0;
 }
 csv_ << "\n";
 last_sample_time_ = now;
 SamplesWritten = SamplesWritten + 1;

 if(FlushInterval <= 0.0 || now - last_flush_time_ + 1e-12 >= FlushInterval)
 {
  csv_.flush();
  last_flush_time_ = now;
 }
 return true;
}

}
