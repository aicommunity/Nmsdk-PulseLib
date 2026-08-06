#ifndef NDATASET_H
#define NDATASET_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

/// Dataset component: loads feature/class matrices from file and drives one pulse generator per feature
class RDK_LIB_TYPE NDataset: public UNet
{
/// Class name used to create child pulse generators
UProperty<std::string, NDataset, ptPubParameter> PulseGeneratorClassName;

/// Number of child generators (equals NumFeatures after successful load)
UProperty<int,NDataset, ptPubState> NumGenerators;

/// Number of features (columns) derived from the dataset file
UProperty<int,NDataset, ptPubState> NumFeatures;

/// Number of samples (rows) derived from the dataset file
UProperty<int,NDataset, ptPubState> NumSamples;

/// Feature matrix loaded from file (rows = samples, cols = features)
UProperty<MDMatrix<double>,NDataset, ptPubState> MatrixData;

/// Class labels matrix loaded from file (1 x NumSamples)
UProperty<MDMatrix<int>,NDataset, ptPubState> MatrixClasses;

/// Request dataset reload from file (cleared after successful load)
UProperty<bool,NDataset, ptPubParameter> ReloadDataset;

/// Computed generator start delays from MatrixData and Tay
UProperty<MDMatrix<double>,NDataset, ptPubState> MatrixDelay;

/// Current sample index
UProperty<int,NDataset, ptPubParameter> Iteration;

/// Time-scale parameter for delay normalization
UProperty<float,NDataset, ptPubParameter> Tay;

/// Base delay before generation starts (sec)
UProperty<double, NDataset, ptPubParameter> Delay;

/// Generation mode: 0 = off, 1 = timed, 2 = continuous
UProperty<int, NDataset, ptPubState> StateGeneration;

/// Timed generation duration (sec), used when StateGeneration == 1
UProperty<double, NDataset, ptPubState> TimeGeneration;

/// Timestamp when timed generation started
UProperty<double, NDataset, ptPubState> OperatingTime;

/// Flag to re-apply delays/frequencies to generators
UProperty<bool, NDataset, ptPubState> ResetDelay;

/// Spike frequency applied to child generators (Hz)
UProperty<double, NDataset, ptPubParameter> SpikesFrequency;

/// Number of distinct classes derived from MatrixClasses
UProperty<int,NDataset, ptPubState> NumClasses;

/// Child generators (one per feature)
vector<NPulseGeneratorTransit*> Generators;

/// Dataset file path (absolute, or relative to Config/Work per flags)
UProperty<std::string,NDataset, ptPubParameter> FileName;

/// Interpret FileName relative to the project config/data directory
UProperty<bool,NDataset, ptPubParameter> UseRelativePathFromConfig;

/// Interpret FileName relative to the application work directory
UProperty<bool,NDataset, ptPubParameter> UseRelativePathFromWorkDir;

protected:

public:
// --------------------------
// Constructors / destructor
// --------------------------
NDataset(void);
virtual ~NDataset(void);
// --------------------------

// --------------------------
// Parameter setters
// --------------------------

/// Pulse generator class name
bool SetPulseGeneratorClassName(const std::string &value);

/// Spike frequency (Hz)
bool SetSpikesFrequency(const double &value);

/// Base delay (sec)
bool SetDelay(const double &value);

/// Dataset file path
bool SetFileName(const std::string &value);

/// Relative path from config/data dir
bool SetUseRelativePathFromConfig(const bool &value);

/// Relative path from application work dir
bool SetUseRelativePathFromWorkDir(const bool &value);

/// Called after a child component was successfully added
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);

/// Called before a child component is removed
virtual bool ADelComponent(UEPtr<UContainer> comp);

// --------------------------
// Object factory
// --------------------------
virtual NDataset* New(void);
static UComponent* NewStatic(void);
// --------------------------


// --------------------------
// Simulation hooks
// --------------------------
protected:
/// Load and parse dataset file into matrices/sizes
bool TreatDataFromFile(void);

/// Resolve FileName using absolute / Config / Work rules
std::string CalcActualSourceFilePath(const std::string &file_name);

/// Create/remove child generators to match NumGenerators
bool SyncGenerators(void);

virtual bool ADefault(void);

virtual bool ABuild(void);

virtual bool AReset(void);

virtual bool ACalculate(void);
// --------------------------
};

}
#endif
