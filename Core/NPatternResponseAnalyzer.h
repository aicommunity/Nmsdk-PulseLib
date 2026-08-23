#ifndef NPATTERNRESPONSEANALYZER_H
#define NPATTERNRESPONSEANALYZER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

#include <fstream>
#include <string>
#include <vector>

namespace NMSDK {

class NPulseLTZoneCommon;
class NNeuronTimeLearner;

using namespace RDK;

class RDK_LIB_TYPE NPatternResponseAnalyzer : public UNet
{
public:
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> StimulusInputs;
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> NeuronOutputs;
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> TargetClassInput;
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> SomaAmplitudeInput;

UProperty<std::string, NPatternResponseAnalyzer, ptPubParameter> LearnerComponentName;

UProperty<double, NPatternResponseAnalyzer, ptPubParameter> PostPatternWindow;
 /// Extra watch after PostPatternWindow for late LTZone spikes (until this delay
 /// after last stim, or until the next trial starts). Must be >= PostPatternWindow.
 UProperty<double, NPatternResponseAnalyzer, ptPubParameter> LateResponseWindow;
UProperty<double, NPatternResponseAnalyzer, ptPubParameter> PulseDetectThreshold;
UProperty<std::string, NPatternResponseAnalyzer, ptPubParameter> SavePath;
UProperty<std::string, NPatternResponseAnalyzer, ptPubParameter> FileName;
UProperty<bool, NPatternResponseAnalyzer, ptPubParameter> AppendMode;
UProperty<bool, NPatternResponseAnalyzer, ptPubParameter> Enable;

/// 0 = LTZ rising-edge match (default); 1 = ISI template vs trial-0 training pattern
UProperty<int, NPatternResponseAnalyzer, ptPubParameter> MatchMode;

UProperty<int, NPatternResponseAnalyzer, ptPubState> TrialIndex;
UProperty<int, NPatternResponseAnalyzer, ptOutput | ptPubState> LastFired;
UProperty<int, NPatternResponseAnalyzer, ptOutput | ptPubState> LastMatch;
UProperty<int, NPatternResponseAnalyzer, ptOutput | ptPubState> LastLateFired;
UProperty<std::string, NPatternResponseAnalyzer, ptOutput | ptPubState> LastErrorClass;
UProperty<double, NPatternResponseAnalyzer, ptOutput | ptPubState> LastNeuronDelay;
UProperty<double, NPatternResponseAnalyzer, ptOutput | ptPubState> LastLateNeuronDelay;
UProperty<MDMatrix<double>, NPatternResponseAnalyzer, ptOutput | ptPubState> LastIsi;

NPatternResponseAnalyzer(void);
virtual ~NPatternResponseAnalyzer(void);

virtual NPatternResponseAnalyzer* New(void);
static UComponent* NewStatic(void);

protected:
virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);

bool EnsureCsvReady(void);
bool IsIsiTemplateMatch(void) const;
void CloseTrial(double now);
void BeginTrial(double now);
void ResetTrialState(void);
static const char *ClassifyError(int target, int fired, int late_fired);
bool DetectRisingEdge(const std::vector<MDMatrix<double> > &inputs,
                      std::vector<double> &prev_values) const;
double ReadScalar(const MDMatrix<double> &m) const;
double ReadTargetClass(void) const;
void UpdateTrialMetrics(double now);
void ReadSomaAmplitudes(double soma_out[4], double &soma_sum) const;
double ReadLtzPotential(void) const;
double EffectiveLateWindow(void) const;

bool trial_active_;
bool trial_window_closed_;
bool playback_stopped_;
bool csv_header_written_;
int trial_target_class_;
int trial_neuron_fired_;
int trial_late_fired_;
double trial_t_first_stim_;
double trial_t_last_stim_;
double trial_t_neuron_;
double trial_t_late_neuron_;
double trial_ltz_potential_max_;
double trial_soma_amp_max_[4];
double trial_soma_amp_sum_max_;
NPulseLTZoneCommon *ltz_source_;
std::vector<double> trial_stim_times_;
std::vector<double> prev_stimulus_;
std::vector<double> prev_neuron_;
std::string csv_full_path_;
};

}
#endif
