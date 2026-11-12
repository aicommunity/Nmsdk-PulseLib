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
UProperty<double, NPatternResponseAnalyzer, ptPubParameter> PulseDetectThreshold;
UProperty<std::string, NPatternResponseAnalyzer, ptPubParameter> SavePath;
UProperty<std::string, NPatternResponseAnalyzer, ptPubParameter> FileName;
UProperty<bool, NPatternResponseAnalyzer, ptPubParameter> AppendMode;
UProperty<bool, NPatternResponseAnalyzer, ptPubParameter> Enable;

UProperty<int, NPatternResponseAnalyzer, ptPubState> TrialIndex;
UProperty<int, NPatternResponseAnalyzer, ptOutput | ptPubState> LastFired;
UProperty<int, NPatternResponseAnalyzer, ptOutput | ptPubState> LastMatch;
UProperty<double, NPatternResponseAnalyzer, ptOutput | ptPubState> LastNeuronDelay;
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
void CloseTrial(double now);
bool DetectRisingEdge(const std::vector<MDMatrix<double> > &inputs,
                      std::vector<double> &prev_values) const;
double ReadScalar(const MDMatrix<double> &m) const;
double ReadTargetClass(void) const;
void UpdateTrialMetrics(double now);
void ReadSomaAmplitudes(double soma_out[4], double &soma_sum) const;
double ReadLtzPotential(void) const;

bool trial_active_;
bool playback_stopped_;
bool csv_header_written_;
int trial_target_class_;
int trial_neuron_fired_;
double trial_t_first_stim_;
double trial_t_last_stim_;
double trial_t_neuron_;
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
