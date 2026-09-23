#ifndef NPATTERNRESPONSEANALYZER_CPP
#define NPATTERNRESPONSEANALYZER_CPP

#include "NPatternResponseAnalyzer.h"
#include "NNeuronTimeLearner.h"
#include "NDatasetBase.h"
#include "NDatasetMatrix.h"
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
 "trial,target_class,stim_count,isi0,isi1,isi2,isi3,"
 "neuron_fired,neuron_t_rel,match,"
 "late_fired,late_t_rel,error_class,"
 "ltz_potential_max,soma_amp_0,soma_amp_1,soma_amp_2,soma_amp_3,soma_amp_sum,"
 "neuron_spike_count,neuron_spike_times,response_class\n";

}

std::string NPatternResponseAnalyzer::ClassifyResponseMorphology(
    const std::vector<double> &spike_rel_times,
    const std::vector<double> &stim_times)
{
 const size_t spike_count = spike_rel_times.size();
 if(spike_count == 0)
  return "single"; // silence — not a morphology fault by itself

 bool burst = false;
 if(spike_count >= 2)
 {
  for(size_t i = 1; i < spike_count; ++i)
  {
   if(spike_rel_times[i] - spike_rel_times[i - 1] <= kBurstIsiMax + 1e-12)
   {
    burst = true;
    break;
   }
  }
 }

 bool per_stim = false;
 const size_t stim_count = stim_times.size();
 if(stim_count >= 3 && spike_count >= stim_count)
  per_stim = true;
 else if(stim_count >= 1 && spike_count >= 1)
 {
  double min_isi = kPerStimWindowFloor;
  if(stim_count >= 2)
  {
   min_isi = stim_times[1] - stim_times[0];
   for(size_t i = 2; i < stim_count; ++i)
    min_isi = std::min(min_isi, stim_times[i] - stim_times[i - 1]);
  }
  const double win = std::max(2.0 * min_isi, kPerStimWindowFloor);
  // Injective matching: each spike covers at most one stim window.
  std::vector<bool> spike_used(spike_count, false);
  size_t matched = 0;
  for(size_t s = 0; s < stim_count; ++s)
  {
   const double t0 = stim_times[s] - stim_times[0];
   for(size_t k = 0; k < spike_count; ++k)
   {
    if(spike_used[k])
     continue;
    if(spike_rel_times[k] + 1e-12 >= t0
       && spike_rel_times[k] <= t0 + win + 1e-12)
    {
     spike_used[k] = true;
     ++matched;
     break;
    }
   }
  }
  const size_t need = static_cast<size_t>(std::ceil(0.75 * double(stim_count)));
  per_stim = matched >= need;
 }

 if(burst && per_stim)
  return "multi";
 if(burst)
  return "burst";
 if(per_stim)
  return "per_stim";
 if(spike_count == 1)
  return "single";
 return "multi";
}

NPatternResponseAnalyzer::NPatternResponseAnalyzer(void)
: StimulusInputs("StimulusInputs", this),
  NeuronOutputs("NeuronOutputs", this),
  TargetClassInput("TargetClassInput", this),
  SomaAmplitudeInput("SomaAmplitudeInput", this),
  LearnerComponentName("LearnerComponentName", this),
  DatasetComponentName("DatasetComponentName", this),
  StimulusFeatureIndex("StimulusFeatureIndex", this),
  PostPatternWindow("PostPatternWindow", this),
  LateResponseWindow("LateResponseWindow", this),
  PulseDetectThreshold("PulseDetectThreshold", this),
  SavePath("SavePath", this),
  FileName("FileName", this),
  AppendMode("AppendMode", this),
  Enable("Enable", this),
  MatchMode("MatchMode", this),
  TrialIndex("TrialIndex", this),
  LastFired("LastFired", this),
  LastMatch("LastMatch", this),
  LastLateFired("LastLateFired", this),
  LastErrorClass("LastErrorClass", this),
  LastNeuronDelay("LastNeuronDelay", this),
  LastLateNeuronDelay("LastLateNeuronDelay", this),
  LastIsi("LastIsi", this),
  LastSpikeCount("LastSpikeCount", this),
  LastResponseClass("LastResponseClass", this),
  trial_active_(false),
  trial_window_closed_(false),
  playback_stopped_(false),
  csv_header_written_(false),
  trial_target_class_(0),
  trial_sample_id_(-1),
  trial_expected_stims_(0),
  trial_neuron_fired_(0),
  trial_late_fired_(0),
  trial_t_first_stim_(0.0),
  trial_t_last_stim_(0.0),
  trial_t_neuron_(-1.0),
  trial_t_late_neuron_(-1.0),
  trial_ltz_potential_max_(0.0),
  trial_sample_start_abs_(0.0),
  trial_expected_last_stim_abs_(-1.0),
  trial_observe_until_(-1.0),
  trial_pattern_complete_(false),
  trial_neu_edge_consumed_(false),
  ltz_source_(nullptr),
  dataset_source_(nullptr)
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
 // Cover long Branch cables (settle ≈ 0.01*L); still ends before next pattern if gap is larger.
 LateResponseWindow = 1.5;
 PulseDetectThreshold = 0.0;
 SavePath = "SelectivityLog";
 FileName = "results.csv";
 AppendMode = false;
 Enable = true;
 MatchMode = 0;
 LearnerComponentName = "NeuronTimeLearner";
 DatasetComponentName = "DatasetMatrix";
 StimulusFeatureIndex = 0;
 TrialIndex = 0;
 LastFired = 0;
 LastMatch = 0;
 LastLateFired = 0;
 LastErrorClass = "ok";
 LastNeuronDelay = -1.0;
 LastLateNeuronDelay = -1.0;
 LastIsi.Resize(0, 0);
 LastSpikeCount = 0;
 LastResponseClass = "single";
 ResetTrialState();
 playback_stopped_ = false;
 csv_header_written_ = false;
 prev_stimulus_.clear();
 prev_neuron_.clear();
 csv_full_path_.clear();
 ltz_source_ = nullptr;
 dataset_source_ = nullptr;
 return true;
}

void NPatternResponseAnalyzer::RecordNeuronSpike(double now)
{
 if(!trial_active_)
  return;
 if(trial_neuron_spike_times_.size() >= kMaxSpikeTimesCsv)
  return;
 trial_neuron_spike_times_.push_back(now);
}

void NPatternResponseAnalyzer::ResetTrialState(void)
{
 trial_active_ = false;
 trial_window_closed_ = false;
 trial_stim_times_.clear();
 trial_neuron_spike_times_.clear();
 trial_neuron_fired_ = 0;
 trial_late_fired_ = 0;
 trial_sample_id_ = -1;
 trial_expected_stims_ = 0;
 trial_t_first_stim_ = 0.0;
 trial_t_last_stim_ = 0.0;
 trial_t_neuron_ = -1.0;
 trial_t_late_neuron_ = -1.0;
 trial_ltz_potential_max_ = 0.0;
 trial_soma_amp_max_[0] = trial_soma_amp_max_[1] = 0.0;
 trial_soma_amp_max_[2] = trial_soma_amp_max_[3] = 0.0;
 trial_soma_amp_sum_max_ = 0.0;
 trial_sample_start_abs_ = 0.0;
 trial_expected_last_stim_abs_ = -1.0;
 trial_observe_until_ = -1.0;
 trial_pattern_complete_ = false;
 trial_neu_edge_consumed_ = false;
}

double NPatternResponseAnalyzer::EffectiveLateWindow(void) const
{
 const double post = double(PostPatternWindow);
 const double late = double(LateResponseWindow);
 return std::max(post, late);
}

const char *NPatternResponseAnalyzer::ClassifyError(int target, int fired, int late_fired)
{
 if(target != 0)
 {
  if(fired)
   return "ok";
  if(late_fired)
   return "late_fn";
  return "fn";
 }
 if(fired)
  return "fp";
 if(late_fired)
  return "late_fp";
 return "ok";
}

bool NPatternResponseAnalyzer::ABuild(void)
{
 ltz_source_ = nullptr;
 dataset_source_ = nullptr;
 if(UEPtr<UContainer> parent = GetOwner())
 {
  const std::string learner_name = std::string(LearnerComponentName);
  const std::string ds_name = std::string(DatasetComponentName);
  UEPtr<UContainer> learner_c = parent->GetComponentL(learner_name, true);
  // StructTrain Branch clones name the learner NeuronTimeLearnerBranch.
  if(!learner_c)
   learner_c = parent->GetComponentL(std::string("NeuronTimeLearnerBranch"), true);
  if(!learner_c)
   learner_c = parent->GetComponentL(std::string("NeuronTimeLearner"), true);
  if(learner_c)
  {
   if(UEPtr<NPulseNeuron> neuron =
       learner_c->GetComponentL<NPulseNeuron>(std::string("Neuron"), true))
   {
    if(UEPtr<NPulseLTZoneCommon> ltz =
        neuron->GetComponentL<NPulseLTZoneCommon>(std::string("LTZone"), true))
     ltz_source_ = ltz.Get();
   }
   if(UEPtr<NDatasetMatrix> ds =
       learner_c->GetComponentL<NDatasetMatrix>(ds_name, true))
    dataset_source_ = ds.Get();
   if(!dataset_source_)
   {
    if(UEPtr<NDatasetMatrix> ds =
        learner_c->GetComponentL<NDatasetMatrix>(std::string("DatasetMatrix"), true))
     dataset_source_ = ds.Get();
   }
  }
  if(!dataset_source_)
  {
   if(UEPtr<NDatasetMatrix> ds =
       parent->GetComponentL<NDatasetMatrix>(ds_name, true))
    dataset_source_ = ds.Get();
  }
  if(!dataset_source_)
  {
   // Last resort: DatasetMatrix nested under a sibling learner.
   if(UEPtr<NDatasetMatrix> ds = parent->GetComponentL<NDatasetMatrix>(
           std::string("NeuronTimeLearnerBranch.DatasetMatrix"), true))
    dataset_source_ = ds.Get();
   if(!dataset_source_)
   {
    if(UEPtr<NDatasetMatrix> ds = parent->GetComponentL<NDatasetMatrix>(
            std::string("NeuronTimeLearner.DatasetMatrix"), true))
     dataset_source_ = ds.Get();
   }
  }
 }
 return true;
}

void NPatternResponseAnalyzer::ResolveDatasetSourceIfNeeded(void)
{
 if(dataset_source_)
  return;
 ABuild();
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

bool NPatternResponseAnalyzer::IsIsiTemplateMatch(void) const
{
 if(!GetOwner())
  return false;

 const std::string learner_name = std::string(LearnerComponentName);
 UEPtr<NNeuronTimeLearner> learner =
  GetOwner()->GetComponentL<NNeuronTimeLearner>(learner_name, true);
 if(!learner)
  return false;

 UEPtr<NPulseNeuron> neuron =
  learner->GetComponentL<NPulseNeuron>(std::string("Neuron"), true);
 if(!neuron)
  return false;

 const MDMatrix<double> &ref = neuron->TrainingPattern;
 if(ref.GetRows() < 1 || trial_stim_times_.size() < 2)
  return false;

 const int n_gap = int(trial_stim_times_.size()) - 1;
 int ref_off = 0;
 if(ref.GetRows() == n_gap + 1)
  ref_off = 1;
 if(ref.GetRows() != n_gap + ref_off)
  return false;

 const double eps = 1e-5;
 for(int i = 0; i < n_gap; ++i)
 {
  const double trial_isi = trial_stim_times_[static_cast<size_t>(i + 1)]
                         - trial_stim_times_[static_cast<size_t>(i)];
  if(fabs(trial_isi - ref(i + ref_off, 0)) > eps)
   return false;
 }
 return true;
}

void NPatternResponseAnalyzer::BeginTrial(double now)
{
 ResolveDatasetSourceIfNeeded();
 trial_active_ = true;
 trial_window_closed_ = false;
 trial_pattern_complete_ = false;
 trial_neu_edge_consumed_ = false;
 trial_observe_until_ = -1.0;

 if(dataset_source_)
 {
  trial_sample_id_ = dataset_source_->GetCurrentSampleId();
  trial_expected_stims_ = dataset_source_->CountScheduledSpikes(
      trial_sample_id_, int(StimulusFeatureIndex));
  trial_target_class_ = dataset_source_->GetSampleClass(trial_sample_id_);
  trial_sample_start_abs_ = dataset_source_->GetSampleStartTime();
  const double last_rel = dataset_source_->GetExpectedLastSpikeAbsRel(
      trial_sample_id_, int(StimulusFeatureIndex));
  trial_expected_last_stim_abs_ = trial_sample_start_abs_ + last_rel;
 }
 else
 {
  trial_sample_id_ = -1;
  trial_expected_stims_ = 0;
  trial_target_class_ = int(ReadTargetClass());
  trial_sample_start_abs_ = now;
  trial_expected_last_stim_abs_ = -1.0;
 }

 trial_neuron_fired_ = 0;
 trial_late_fired_ = 0;
 trial_t_first_stim_ = now;
 trial_t_last_stim_ = now;
 trial_t_neuron_ = -1.0;
 trial_t_late_neuron_ = -1.0;
 trial_ltz_potential_max_ = 0.0;
 trial_soma_amp_max_[0] = trial_soma_amp_max_[1] = 0.0;
 trial_soma_amp_max_[2] = trial_soma_amp_max_[3] = 0.0;
 trial_soma_amp_sum_max_ = 0.0;
 trial_stim_times_.clear();
 trial_neuron_spike_times_.clear();
 trial_stim_times_.push_back(now);
 if(IsPatternComplete())
 {
  trial_pattern_complete_ = true;
  trial_observe_until_ = trial_t_last_stim_ + EffectiveLateWindow();
 }
}

void NPatternResponseAnalyzer::AppendStim(double now)
{
 trial_stim_times_.push_back(now);
 trial_t_last_stim_ = now;
 if(IsPatternComplete())
 {
  trial_pattern_complete_ = true;
  if(trial_observe_until_ < 0.0)
   trial_observe_until_ = trial_t_last_stim_ + EffectiveLateWindow();
 }
}

bool NPatternResponseAnalyzer::IsPatternComplete() const
{
 if(trial_expected_stims_ > 0)
  return int(trial_stim_times_.size()) >= trial_expected_stims_;
 // Legacy without dataset: never treat the first stim as a complete pattern (A01).
 // Historical packA-class patterns without a bound dataset need >=4 observed stims.
 return trial_stim_times_.size() >= 4;
}

void NPatternResponseAnalyzer::AttributeNeuronToActiveTrial(double now)
{
 if(!trial_active_ || trial_neu_edge_consumed_)
  return;
 RecordNeuronSpike(now);
 trial_neu_edge_consumed_ = true;
 MaybeClassifyFire(now, double(PostPatternWindow));
}

void NPatternResponseAnalyzer::MaybeClassifyFire(double now, double post_win)
{
 if(!trial_pattern_complete_)
  return;
 if(now + 1e-12 < trial_t_last_stim_)
  return;
 if(now <= trial_t_last_stim_ + post_win + 1e-12)
 {
  if(!trial_neuron_fired_)
  {
   trial_neuron_fired_ = 1;
   trial_t_neuron_ = now;
  }
 }
 else if(!trial_neuron_fired_ && !trial_late_fired_)
 {
  trial_late_fired_ = 1;
  trial_t_late_neuron_ = now;
 }
}

void NPatternResponseAnalyzer::CloseTrial(double now)
{
 (void)now;
 if(!trial_active_)
  return;

 // Immutable label from BeginTrial — do not re-read TargetClassInput.

 const int fired = trial_neuron_fired_;
 const int late = trial_late_fired_;
 const int target = trial_target_class_;
 int match;
 if(MatchMode.GetData() == 1)
 {
  const bool isi_ok = IsIsiTemplateMatch();
  match = (target != 0) ? (isi_ok ? 1 : 0) : (isi_ok ? 0 : 1);
 }
 else
  match = (target != 0) ? fired : (fired ? 0 : 1);
 const char *err = ClassifyError(target, fired, late);

 std::vector<double> spike_rel;
 spike_rel.reserve(trial_neuron_spike_times_.size());
 for(size_t i = 0; i < trial_neuron_spike_times_.size(); ++i)
  spike_rel.push_back(trial_neuron_spike_times_[i] - trial_t_first_stim_);
 const std::string resp =
  ClassifyResponseMorphology(spike_rel, trial_stim_times_);
 const int spike_count = int(trial_neuron_spike_times_.size());

 LastFired.SetDataDirect(fired);
 LastMatch.SetDataDirect(match);
 LastLateFired.SetDataDirect(late);
 LastErrorClass.SetDataDirect(std::string(err));
 LastSpikeCount.SetDataDirect(spike_count);
 LastResponseClass.SetDataDirect(resp);
 if(fired && trial_t_neuron_ >= 0.0 && trial_t_first_stim_ >= 0.0)
  LastNeuronDelay.SetDataDirect(trial_t_neuron_ - trial_t_first_stim_);
 else
  LastNeuronDelay.SetDataDirect(-1.0);
 if(late && trial_t_late_neuron_ >= 0.0 && trial_t_first_stim_ >= 0.0)
  LastLateNeuronDelay.SetDataDirect(trial_t_late_neuron_ - trial_t_first_stim_);
 else
  LastLateNeuronDelay.SetDataDirect(-1.0);

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
        << late << ',';
    if(late && trial_t_late_neuron_ >= 0.0 && trial_t_first_stim_ >= 0.0)
     out << (trial_t_late_neuron_ - trial_t_first_stim_);
    else
     out << -1;
    out << ',' << err << ','
        << trial_ltz_potential_max_ << ',';
    for(int i = 0; i < 4; ++i)
    {
     out << trial_soma_amp_max_[i];
     if(i < 3)
      out << ',';
    }
    out << ',' << trial_soma_amp_sum_max_ << ','
        << spike_count << ',';
    for(size_t i = 0; i < spike_rel.size(); ++i)
    {
     if(i)
      out << ';';
     out << spike_rel[i];
    }
    out << ',' << resp << '\n';
   }
  }
 }

 TrialIndex.SetDataDirect(int(TrialIndex) + 1);
 ResetTrialState();
}

bool NPatternResponseAnalyzer::AReset(void)
{
 if(trial_active_)
  CloseTrial(Environment ? Environment->GetTime().GetDoubleTime() : 0.0);

 TrialIndex = 0;
 LastFired = 0;
 LastMatch = 0;
 LastLateFired = 0;
 LastErrorClass = "ok";
 LastNeuronDelay = -1.0;
 LastLateNeuronDelay = -1.0;
 LastIsi.Resize(0, 0);
 LastSpikeCount = 0;
 LastResponseClass = "single";
 ResetTrialState();
 playback_stopped_ = false;
 prev_stimulus_.clear();
 prev_neuron_.clear();

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

 ResolveDatasetSourceIfNeeded();
 const double now = Environment->GetTime().GetDoubleTime();
 const std::vector<MDMatrix<double> > &stim = StimulusInputs.GetData();
 const std::vector<MDMatrix<double> > &neuron = NeuronOutputs.GetData();
 const double post_win = double(PostPatternWindow);
 const double late_win = EffectiveLateWindow();

 const bool stim_edge = DetectRisingEdge(stim, prev_stimulus_);
 bool neu_edge = DetectRisingEdge(neuron, prev_neuron_);
 trial_neu_edge_consumed_ = false;

 // Watchdog: dataset advanced to a new sample while previous trial still open.
 if(trial_active_ && dataset_source_
    && trial_sample_id_ >= 0
    && dataset_source_->GetCurrentSampleId() != trial_sample_id_
    && !stim_edge)
 {
  CloseTrial(now);
 }

 if(!playback_stopped_ && stim_edge)
 {
  if(!trial_active_)
  {
   BeginTrial(now);
   if(neu_edge)
    AttributeNeuronToActiveTrial(now);
  }
  else if(!trial_pattern_complete_)
  {
   AppendStim(now);
   if(neu_edge)
    AttributeNeuronToActiveTrial(now);
  }
  else
  {
   // Pattern already complete: this stim starts the next sample.
   CloseTrial(now);
   BeginTrial(now);
   if(neu_edge)
    AttributeNeuronToActiveTrial(now);
  }
 }

 if(trial_active_)
 {
  UpdateTrialMetrics(now);

  if(neu_edge && !trial_neu_edge_consumed_)
   AttributeNeuronToActiveTrial(now);

  if(trial_pattern_complete_ && trial_observe_until_ < 0.0)
   trial_observe_until_ = trial_t_last_stim_ + late_win;

  if(trial_pattern_complete_
     && now + 1e-12 >= trial_t_last_stim_ + post_win)
   trial_window_closed_ = true;

  if(trial_observe_until_ >= 0.0
     && now + 1e-12 >= trial_observe_until_)
   CloseTrial(now);
 }

 return true;
}

}
#endif
