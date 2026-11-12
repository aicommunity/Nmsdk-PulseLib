#ifndef NDATASETBASE_H
#define NDATASETBASE_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

#include <vector>

namespace NMSDK {

using namespace RDK;

/// Shared base for file and matrix spike-dataset components (not registered in Storage)
class RDK_LIB_TYPE NDatasetBase: public UNet
{
public:
/// Class name used to create child pulse generators
UProperty<std::string, NDatasetBase, ptPubParameter> PulseGeneratorClassName;

/// Number of features (MatrixData columns); also count of child Generator1..N
UProperty<int, NDatasetBase, ptPubState> NumFeatures;

/// Number of samples (MatrixData has NumSamples * MaxSpikesPerFeature rows)
UProperty<int, NDatasetBase, ptPubState> NumSamples;

/// Spike slots per sample block (MatrixData rows = NumSamples * MaxSpikesPerFeature)
UProperty<int, NDatasetBase, ptPubParameter> MaxSpikesPerFeature;

/// ISI: (NumSamples * MaxSpikesPerFeature) rows x NumFeatures cols; -1 = skip
UProperty<MDMatrix<double>, NDatasetBase, ptPubState> MatrixData;

/// Class labels matrix (1 x NumSamples)
UProperty<MDMatrix<int>, NDatasetBase, ptPubState> MatrixClasses;

/// Current sample index
UProperty<int, NDatasetBase, ptPubParameter> Iteration;

/// Pause after a spike burst before next play (sec)
UProperty<double, NDatasetBase, ptPubParameter> Delay;

/// After burst+Delay: if true, Iteration = (Iteration+1) % NumSamples
UProperty<bool, NDatasetBase, ptPubParameter> AdvanceSampleAfterBurst;

/// If true, wrap Iteration to 0 after last sample; if false, stop after last sample
UProperty<bool, NDatasetBase, ptPubParameter> LoopSamples;

/// Class label of the active sample (from MatrixClasses at Iteration)
UProperty<int, NDatasetBase, ptOutput | ptPubState> CurrentClass;

/// Same as CurrentClass as 1×1 matrix for links to analyzers
UProperty<MDMatrix<double>, NDatasetBase, ptOutput | ptPubState> CurrentClassMatrix;

/// Generation mode: 0 = off, 1 = timed, 2 = continuous (burst -> Delay -> burst)
UProperty<int, NDatasetBase, ptPubState> StateGeneration;

/// Timed generation duration (sec), used when StateGeneration == 1
UProperty<double, NDatasetBase, ptPubState> TimeGeneration;

/// Timestamp when timed generation started
UProperty<double, NDatasetBase, ptPubState> OperatingTime;

/// Flag to restart burst schedule
UProperty<bool, NDatasetBase, ptPubState> ResetDelay;

/// Number of distinct classes derived from MatrixClasses
UProperty<int, NDatasetBase, ptPubState> NumClasses;

/// Child generators (one per feature)
vector<NPulseGeneratorTransit*> Generators;

public:
NDatasetBase(void);
virtual ~NDatasetBase(void);

bool SetPulseGeneratorClassName(const std::string &value);
bool SetDelay(const double &value);

/// Dimensional setters (virtual: Matrix leaf resizes matrices)
virtual bool SetNumFeatures(const int &value);
virtual bool SetNumSamples(const int &value);
virtual bool SetMaxSpikesPerFeature(const int &value);
virtual bool SetMatrixData(const MDMatrix<double> &value);
virtual bool SetMatrixClasses(const MDMatrix<int> &value);

virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);
virtual bool ADelComponent(UEPtr<UContainer> comp);

protected:
/// Fill or validate MatrixData/MatrixClasses from the concrete source
virtual bool PrepareDataset() = 0;

/// Derive NumSamples/NumFeatures/NumClasses from MatrixData/MatrixClasses
bool ApplyFromMatrices(void);

/// Create/remove child generators to match NumFeatures
bool SyncGenerators(void);

/// Row index in MatrixData for sample + spike slot
int SampleSlotRow(int sample, int spike_slot) const;

/// Absolute spike times for current Iteration from MatrixData ISI slots
void RebuildSpikeAbsTimesForIteration(void);

/// Start a single pulse on generator
void FireOneShot(int feature_index, NPulseGeneratorTransit* gen, double now);

/// Silence all child generators
void SilenceGenerators(void);

/// Begin spike-train schedule for current sample
void BeginSpikeTrainSample(double now);

/// Advance one-shot schedule; after burst+Delay restart or advance Iteration
void UpdateSpikeTrainPlayback(double now);

/// True when all scheduled spikes for the sample have been fired and pulses ended
bool IsBurstFullyComplete(void) const;

/// Number of distinct class labels
int CalcNumClasses(const MDMatrix<int> &matrix_classes) const;

virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);

/// Absolute spike times for current Iteration, per feature
std::vector<std::vector<double> > SampleSpikeAbsTimes;

/// Next spike index per feature
std::vector<int> NextSpikeIndex;

/// End time of active one-shot pulse per feature (0 = idle)
std::vector<double> OneShotEndTime;

/// Wall-clock start of current sample playback
double SampleStartTime;

/// Relative end of burst (last spike abs time + pulse length), from SampleStart
double SampleBurstEndRel;

/// Last Iteration value used for playback schedule
int LastPlayedIteration;
};

}
#endif
