#ifndef NPATTERNRESPONSEANALYZER_CPP
#define NPATTERNRESPONSEANALYZER_CPP

#include "NPatternResponseAnalyzer.h"
#include "NNeuronTimeLearner.h"
#include "NPulseLTZoneCommon.h"
#include "NPulseNeuron.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace NMSDK {

namespace {

std::string JoinPath(const std::string &base, const std::string &sub)
{
 if(base.empty())
  return sub;
 if(sub.empty())
  return base;
 const char last = base[base.size() - 1];
 if(last == '/' || last == '\\')
  return base + sub;
 return base + "/" + sub;
}

const char *kCsvHeader =
 "trial,target_class,stim_count,isi0,isi1,isi2,isi3,neuron_fired,neuron_t_rel,match,"
 "ltz_potential_max,soma_amp_0,soma_amp_1,soma_amp_2,soma_amp_3,soma_amp_sum\n";

}

NPatternResponseAnalyzer::NPatternResponseAnalyzer(void)
: StimulusInputs("StimulusInputs", this),
  NeuronOutputs("NeuronOutputs", this),
  TargetClassInput("TargetClassInput", this),
  SomaAmplitudeInput("SomaAmplitudeInput", this),
  LearnerComponentName("LearnerComponentName", this),
  PostPatternWindow("PostPatternWindow", this),
  PulseDetectThreshold("PulseDetectThreshold", this),
  SavePath("SavePath", this),
  FileName("FileName", this),
  AppendMode("AppendMode", this),
  Enable("Enable", this),
  TrialIndex("TrialIndex", this),
  LastFired("LastFired", this),
  LastMatch("LastMatch", this),
  LastNeuronDelay("LastNeuronDelay", this),
  LastIsi("LastIsi", this),
  trial_active_(false),
  playback_stopped_(false),
  csv_header_written_(false),
  trial_target_class_(0),
  trial_neuron_fired_(0),
  trial_t_first_stim_(0.0),
  trial_t_last_stim_(0.0),
  trial_t_neuron_(-1.0),
  trial_ltz_potential_max_(0.0),
  ltz_source_(nullptr)
{
 trial_soma_amp_max_[0] = trial_soma_amp_max_[1] = 0.0;
 trial_soma_amp_max_[2] = trial_soma_amp_max_[3] = 0.0;
 trial_soma_amp_sum_max_ = 0.0;
}

NPatternResponseAnalyzer::~NPatternResponseAnalyzer(void)
{
}

NPatternResponseAnalyzer* NPatternResponseAnalyzer::New(void)
{
 return new NPatternResponseAnalyzer;
}

UComponent* NPatternResponseAnalyzer::NewStatic(void)
{
 return new NPatternResponseAnalyzer;
}

bool NPatternResponseAnalyzer::ADefault(void)
{
 PostPatternWindow = 0.5;
 PulseDetectThreshold = 0.0;
 SavePath = "SelectivityLog";
 FileName = "results.csv";
 AppendMode = false;
 Enable = true;
 LearnerComponentName = "NeuronTimeLearner";
 TrialIndex = 0;
 LastFired = 0;
 LastMatch = 0;
 LastNeuronDelay = -1.0;
 LastIsi.Resize(0, 0);
 trial_active_ = false;
 playback_stopped_ = false;
 csv_header_written_ = false;
 prev_stimulus_.clear();
 prev_neuron_.clear();
 trial_stim_times_.clear();
 csv_full_path_.clear();
 trial_ltz_potential_max_ = 0.0;
 trial_soma_amp_max_[0] = trial_soma_amp_max_[1] = 0.0;
 trial_soma_amp_max_[2] = trial_soma_amp_max_[3] = 0.0;
 trial_soma_amp_sum_max_ = 0.0;
 ltz_source_ = nullptr;
 return true;
}

bool NPatternResponseAnalyzer::ABuild(void)
{
 ltz_source_ = nullptr;
 if(UEPtr<UContainer> parent = GetOwner())
 {
  const std::string learner_name = std::string(LearnerComponentName);
  if(UEPtr<NNeuronTimeLearner> learner =
      parent->GetComponentL<NNeuronTimeLearner>(learner_name, true))
  {
   if(UEPtr<NPulseNeuron> neuron =
       learner->GetComponentL<NPulseNeuron>(std::string("Neuron"), true))
   {
    if(UEPtr<NPulseLTZoneCommon> ltz =
        neuron->GetComponentL<NPulseLTZoneCommon>(std::string("LTZone"), true))
     ltz_source_ = ltz.Get();
   }
  }
 }
 return true;
}

double NPatternResponseAnalyzer::ReadTargetClass(void) const
{
 const std::vector<MDMatrix<double> > &classes = TargetClassInput.GetData();
 for(size_t i = 0; i < classes.size(); ++i)
 {
  if(classes[i].GetRows() > 0 && classes[i].GetCols() > 0)
   return classes[i](0, 0);
 }
 return 0.0;
}

double NPatternResponseAnalyzer::ReadScalar(const MDMatrix<double> &m) const
{
 if(m.GetRows() > 0 && m.GetCols() > 0)
  return m(0, 0);
 return 0.0;
}

bool NPatternResponseAnalyzer::DetectRisingEdge(const std::vector<MDMatrix<double> > &inputs,
                                                std::vector<double> &prev_values) const
{
 const double thr = double(PulseDetectThreshold);
 if(int(prev_values.size()) != int(inputs.size()))
  prev_values.assign(inputs.size(), 0.0);

 bool rising = false;
 for(size_t i = 0; i < inputs.size(); ++i)
 {
  const double now = ReadScalar(inputs[i]);
  const double prev = prev_values[i];
  if(prev <= thr && now > thr)
   rising = true;
  prev_values[i] = now;
 }
 return rising;
}

double NPatternResponseAnalyzer::ReadLtzPotential(void) const
{
 if(!ltz_source_ && GetOwner())
 {
  const std::string learner_name = std::string(LearnerComponentName);
  if(UEPtr<NNeuronTimeLearner> learner =
      GetOwner()->GetComponentL<NNeuronTimeLearner>(learner_name, true))
  {
   if(UEPtr<NPulseNeuron> neuron =
       learner->GetComponentL<NPulseNeuron>(std::string("Neuron"), true))
   {
    if(UEPtr<NPulseLTZoneCommon> ltz =
        neuron->GetComponentL<NPulseLTZoneCommon>(std::string("LTZone"), true))
     const_cast<NPatternResponseAnalyzer *>(this)->ltz_source_ = ltz.Get();
   }
  }
 }
 if(ltz_source_)
  return ltz_source_->Potential.GetData();
 return 0.0;
}

void NPatternResponseAnalyzer::ReadSomaAmplitudes(double soma_out[4], double &soma_sum) const
{
 soma_sum = 0.0;
 for(int i = 0; i < 4; ++i)
  soma_out[i] = 0.0;

 const std::vector<MDMatrix<double> > &amps = SomaAmplitudeInput.GetData();
 if(amps.empty())
  return;

 const MDMatrix<double> &m = amps[0];
 if(m.GetRows() <= 0 || m.GetCols() <= 0)
  return;

 if(m.GetRows() > 0)
  soma_sum = m(0, 0);
 for(int i = 0; i < 4; ++i)
 {
  const int row = i + 1;
  if(row < m.GetRows())
   soma_out[i] = m(row, 0);
 }
}

void NPatternResponseAnalyzer::UpdateTrialMetrics(double /*now*/)
{
 const double ltz = ReadLtzPotential();
 if(ltz > trial_ltz_potential_max_)
  trial_ltz_potential_max_ = ltz;

 double soma[4] = {0.0, 0.0, 0.0, 0.0};
 double soma_sum = 0.0;
 ReadSomaAmplitudes(soma, soma_sum);
 if(soma_sum > trial_soma_amp_sum_max_)
  trial_soma_amp_sum_max_ = soma_sum;
 for(int i = 0; i < 4; ++i)
 {
  if(soma[i] > trial_soma_amp_max_[i])
   trial_soma_amp_max_[i] = soma[i];
 }
}

bool NPatternResponseAnalyzer::EnsureCsvReady(void)
{
 if(csv_full_path_.empty())
 {
  std::string base;
  if(Environment)
   base = Environment->GetCurrentDataDir();
  const std::string dir = JoinPath(base, std::string(SavePath));
  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  csv_full_path_ = JoinPath(dir, std::string(FileName));
 }

 if(!AppendMode.GetData() && !csv_header_written_)
 {
  std::ofstream out(csv_full_path_.c_str(), std::ios::trunc);
  if(!out)
   return false;
  out << kCsvHeader;
  csv_header_written_ = true;
  return true;
 }

 if(AppendMode.GetData() && !csv_header_written_)
 {
  std::ifstream probe(csv_full_path_.c_str());
  if(!probe.good())
  {
   std::ofstream out(csv_full_path_.c_str(), std::ios::trunc);
   if(!out)
    return false;
   out << kCsvHeader;
  }
  csv_header_written_ = true;
 }
 return true;
}

void NPatternResponseAnalyzer::CloseTrial(double now)
{
 (void)now;
 if(!trial_active_)
  return;

 trial_target_class_ = int(ReadTargetClass());

 const int fired = trial_neuron_fired_;
 const int target = trial_target_class_;
 const int match = (target != 0) ? fired : (fired ? 0 : 1);

 LastFired.SetDataDirect(fired);
 LastMatch.SetDataDirect(match);
 if(fired && trial_t_neuron_ >= 0.0 && trial_t_first_stim_ >= 0.0)
  LastNeuronDelay.SetDataDirect(trial_t_neuron_ - trial_t_first_stim_);
 else
  LastNeuronDelay.SetDataDirect(-1.0);

 MDMatrix<double> isi_mat;
 if(trial_stim_times_.size() >= 2)
 {
  const int n = int(trial_stim_times_.size()) - 1;
  isi_mat.Resize(n, 1, 0.0);
  for(int i = 0; i < n; ++i)
   isi_mat(i, 0) = trial_stim_times_[static_cast<size_t>(i + 1)]
                    - trial_stim_times_[static_cast<size_t>(i)];
 }
 else
  isi_mat.Resize(0, 0);
 LastIsi.SetDataDirect(isi_mat);

 if(Enable.GetData())
 {
  if(EnsureCsvReady())
  {
   std::ofstream out(csv_full_path_.c_str(), std::ios::app);
   if(out)
   {
    out << int(TrialIndex) << ','
        << target << ','
        << int(trial_stim_times_.size()) << ',';
    for(int i = 0; i < 4; ++i)
    {
     if(i < isi_mat.GetRows())
      out << isi_mat(i, 0);
     if(i < 3)
      out << ',';
    }
    out << ','
        << fired << ',';
    if(fired && trial_t_neuron_ >= 0.0 && trial_t_first_stim_ >= 0.0)
     out << (trial_t_neuron_ - trial_t_first_stim_);
    else
     out << -1;
    out << ',' << match << ','
        << trial_ltz_potential_max_ << ',';
    for(int i = 0; i < 4; ++i)
    {
     out << trial_soma_amp_max_[i];
     if(i < 3)
      out << ',';
    }
    out << ',' << trial_soma_amp_sum_max_ << '\n';
   }
  }
 }

 TrialIndex.SetDataDirect(int(TrialIndex) + 1);
 trial_active_ = false;
 trial_neuron_fired_ = 0;
 trial_stim_times_.clear();
 trial_t_first_stim_ = 0.0;
 trial_t_last_stim_ = 0.0;
 trial_t_neuron_ = -1.0;
 trial_ltz_potential_max_ = 0.0;
 trial_soma_amp_max_[0] = trial_soma_amp_max_[1] = 0.0;
 trial_soma_amp_max_[2] = trial_soma_amp_max_[3] = 0.0;
 trial_soma_amp_sum_max_ = 0.0;
}

bool NPatternResponseAnalyzer::AReset(void)
{
 if(trial_active_)
  CloseTrial(Environment ? Environment->GetTime().GetDoubleTime() : 0.0);

 TrialIndex = 0;
 LastFired = 0;
 LastMatch = 0;
 LastNeuronDelay = -1.0;
 LastIsi.Resize(0, 0);
 trial_active_ = false;
 playback_stopped_ = false;
 prev_stimulus_.clear();
 prev_neuron_.clear();
 trial_stim_times_.clear();
 trial_neuron_fired_ = 0;
 trial_t_first_stim_ = 0.0;
 trial_t_last_stim_ = 0.0;
 trial_t_neuron_ = -1.0;

 if(!AppendMode.GetData())
 {
  csv_header_written_ = false;
  if(!csv_full_path_.empty())
  {
   std::ofstream out(csv_full_path_.c_str(), std::ios::trunc);
   if(out)
    out << kCsvHeader;
   csv_header_written_ = true;
  }
 }
 return true;
}

bool NPatternResponseAnalyzer::ACalculate(void)
{
 if(!Enable.GetData())
  return true;

 const double now = Environment->GetTime().GetDoubleTime();
 const std::vector<MDMatrix<double> > &stim = StimulusInputs.GetData();
 const std::vector<MDMatrix<double> > &neuron = NeuronOutputs.GetData();

 if(!playback_stopped_ && DetectRisingEdge(stim, prev_stimulus_))
 {
  if(!trial_active_)
  {
   trial_active_ = true;
   trial_target_class_ = int(ReadTargetClass());
   trial_neuron_fired_ = 0;
   trial_t_first_stim_ = now;
   trial_t_last_stim_ = now;
   trial_t_neuron_ = -1.0;
   trial_ltz_potential_max_ = 0.0;
   trial_soma_amp_max_[0] = trial_soma_amp_max_[1] = 0.0;
   trial_soma_amp_max_[2] = trial_soma_amp_max_[3] = 0.0;
   trial_soma_amp_sum_max_ = 0.0;
   trial_stim_times_.clear();
   trial_stim_times_.push_back(now);
  }
  else
  {
   trial_stim_times_.push_back(now);
   trial_t_last_stim_ = now;
  }
 }

 if(trial_active_)
  trial_target_class_ = int(ReadTargetClass());

 if(trial_active_)
  UpdateTrialMetrics(now);

 if(trial_active_ && !trial_neuron_fired_
    && DetectRisingEdge(neuron, prev_neuron_)
    && now <= trial_t_last_stim_ + double(PostPatternWindow))
 {
  trial_t_neuron_ = now;
  trial_neuron_fired_ = 1;
  UpdateTrialMetrics(now);
 }

 if(trial_active_ && now >= trial_t_last_stim_ + double(PostPatternWindow))
  CloseTrial(now);

 return true;
}

}
#endif
