#ifndef NPATTERNRESPONSEANALYZER_H
#define NPATTERNRESPONSEANALYZER_H

#include "../../../Rdk/Deploy/Include/rdk.h"

#include <fstream>
#include <string>
#include <vector>

namespace NMSDK {

using namespace RDK;

class RDK_LIB_TYPE NPatternResponseAnalyzer : public UNet
{
public:
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> StimulusInputs;
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> NeuronOutputs;
UProperty<std::vector<MDMatrix<double> >, NPatternResponseAnalyzer, ptInput | ptPubState> TargetClassInput;

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

bool trial_active_;
bool playback_stopped_;
bool csv_header_written_;
int trial_target_class_;
int trial_neuron_fired_;
double trial_t_first_stim_;
double trial_t_last_stim_;
double trial_t_neuron_;
std::vector<double> trial_stim_times_;
std::vector<double> prev_stimulus_;
std::vector<double> prev_neuron_;
std::string csv_full_path_;
};

}
#endif
