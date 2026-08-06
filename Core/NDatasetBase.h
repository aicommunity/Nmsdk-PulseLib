#ifndef NDATASETBASE_H
#define NDATASETBASE_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

/// Shared base for file and manual spike-dataset components (not registered in Storage)
class RDK_LIB_TYPE NDatasetBase: public UNet
{
/// Class name used to create child pulse generators
UProperty<std::string, NDatasetBase, ptPubParameter> PulseGeneratorClassName;

/// Number of child generators (equals NumFeatures after ApplyFromMatrices)
UProperty<int, NDatasetBase, ptPubState> NumGenerators;

/// Number of features (columns) derived from MatrixData
UProperty<int, NDatasetBase, ptPubState> NumFeatures;

/// Number of samples (rows) derived from MatrixData
UProperty<int, NDatasetBase, ptPubState> NumSamples;

/// Feature matrix (rows = samples, cols = features)
UProperty<MDMatrix<double>, NDatasetBase, ptPubState> MatrixData;

/// Class labels matrix (1 x NumSamples)
UProperty<MDMatrix<int>, NDatasetBase, ptPubState> MatrixClasses;

/// Computed generator start delays from MatrixData and Tay
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
bool SetMatrixData(const MDMatrix<double> &value);
bool SetMatrixClasses(const MDMatrix<int> &value);

virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);
virtual bool ADelComponent(UEPtr<UContainer> comp);

protected:
/// Fill or validate MatrixData/MatrixClasses from the concrete source
virtual bool PrepareDataset() = 0;

/// Derive Num*/MatrixDelay/NumGenerators from MatrixData/MatrixClasses
bool ApplyFromMatrices(void);

/// Create/remove child generators to match NumGenerators
bool SyncGenerators(void);

/// Matrix of generator start delays from normalized feature values
MDMatrix<double> CalcMatrixDelay(int num_samples, int num_features,
                                 const MDMatrix<double> &matrix_data, double tay) const;

/// Number of distinct class labels
int CalcNumClasses(const MDMatrix<int> &matrix_classes) const;

virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);
};

}
#endif
