#ifndef NDATASETBASE_H
#define NDATASETBASE_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

#include <vector>

namespace NMSDK {

using namespace RDK;

/// Shared base for file and manual spike-dataset components (not registered in Storage)
class RDK_LIB_TYPE NDatasetBase: public UNet
{
public:
/// Class name used to create child pulse generators
UProperty<std::string, NDatasetBase, ptPubParameter> PulseGeneratorClassName;

/// Number of features (columns) derived from MatrixData; also count of child Generator1..N
UProperty<int, NDatasetBase, ptPubState> NumFeatures;

/// Number of samples (rows) derived from MatrixData
UProperty<int, NDatasetBase, ptPubState> NumSamples;

/// Max spike slots per feature in MatrixSpikeDelays (default 1 = legacy-compatible)
UProperty<int, NDatasetBase, ptPubParameter> MaxSpikesPerFeature;

/// Feature matrix (rows = samples, cols = features); legacy single-spike source
UProperty<MDMatrix<double>, NDatasetBase, ptPubState> MatrixData;

/// Class labels matrix (1 x NumSamples)
UProperty<MDMatrix<int>, NDatasetBase, ptPubState> MatrixClasses;

/// Wide ISI matrix: NumSamples x (NumFeatures * MaxSpikesPerFeature); -1 = skip slot
UProperty<MDMatrix<double>, NDatasetBase, ptPubState> MatrixSpikeDelays;

/// Computed generator start delays from MatrixData and Tay (legacy path)
UProperty<MDMatrix<double>, NDatasetBase, ptPubState> MatrixDelay;

/// Current sample index
UProperty<int, NDatasetBase, ptPubParameter> Iteration;

/// Time-scale parameter for delay normalization
UProperty<float, NDatasetBase, ptPubParameter> Tay;

/// Base delay before generation starts (sec)
UProperty<double, NDatasetBase, ptPubParameter> Delay;

/// Generation mode: 0 = off, 1 = timed, 2 = continuous
UProperty<int, NDatasetBase, ptPubState> StateGeneration;

/// Timed generation duration (sec), used when StateGeneration == 1
UProperty<double, NDatasetBase, ptPubState> TimeGeneration;

/// Timestamp when timed generation started
UProperty<double, NDatasetBase, ptPubState> OperatingTime;

/// Flag to re-apply delays/frequencies to generators
UProperty<bool, NDatasetBase, ptPubState> ResetDelay;

/// Spike frequency applied to child generators (Hz)
UProperty<double, NDatasetBase, ptPubParameter> SpikesFrequency;

/// Number of distinct classes derived from MatrixClasses
UProperty<int, NDatasetBase, ptPubState> NumClasses;

/// Child generators (one per feature)
vector<NPulseGeneratorTransit*> Generators;

public:
NDatasetBase(void);
virtual ~NDatasetBase(void);

bool SetPulseGeneratorClassName(const std::string &value);
bool SetSpikesFrequency(const double &value);
bool SetDelay(const double &value);

/// Dimensional setters (virtual: Manual resizes matrices)
virtual bool SetNumFeatures(const int &value);
virtual bool SetNumSamples(const int &value);
virtual bool SetMaxSpikesPerFeature(const int &value);
virtual bool SetMatrixData(const MDMatrix<double> &value);
virtual bool SetMatrixClasses(const MDMatrix<int> &value);
virtual bool SetMatrixSpikeDelays(const MDMatrix<double> &value);

virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);
virtual bool ADelComponent(UEPtr<UContainer> comp);

protected:
/// Fill or validate MatrixData/MatrixClasses from the concrete source
virtual bool PrepareDataset() = 0;

/// Derive NumSamples/NumFeatures/NumClasses/MatrixDelay from matrices
bool ApplyFromMatrices(void);

/// Create/remove child generators to match NumFeatures
bool SyncGenerators(void);

/// True when MatrixSpikeDelays drives playback (not legacy MatrixData/Tay path)
bool IsSpikeTrainMode(void) const;

/// Absolute spike times for current Iteration from MatrixSpikeDelays ISI slots
void RebuildSpikeAbsTimesForIteration(void);

/// Start a single pulse on generator (Frequency + Reset, then silence after PulseLength)
void FireOneShot(int feature_index, NPulseGeneratorTransit* gen, double now);

/// Silence all child generators
void SilenceGenerators(void);

/// Begin spike-train schedule for current sample
void BeginSpikeTrainSample(double now);

/// Advance one-shot schedule for current sample
void UpdateSpikeTrainPlayback(double now);

/// Matrix of generator start delays from normalized feature values
MDMatrix<double> CalcMatrixDelay(int num_samples, int num_features,
                                 const MDMatrix<double> &matrix_data, double tay) const;

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

/// Last Iteration value used for playback schedule
int LastPlayedIteration;
};

}
#endif
