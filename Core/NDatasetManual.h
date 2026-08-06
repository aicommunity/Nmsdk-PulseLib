#ifndef NDATASETMANUAL_H
#define NDATASETMANUAL_H

#include "NDatasetBase.h"

namespace NMSDK {

using namespace RDK;

/// Manual dataset: dims and matrices are editable parameters; dims resize MatrixData/MatrixClasses
class RDK_LIB_TYPE NDatasetManual: public NDatasetBase
{
public:
NDatasetManual(void);
virtual ~NDatasetManual(void);

virtual NDatasetManual* New(void);
static UComponent* NewStatic(void);

virtual bool SetNumFeatures(const int &value);
virtual bool SetNumSamples(const int &value);
virtual bool SetMaxSpikesPerFeature(const int &value);
virtual bool SetMatrixData(const MDMatrix<double> &value);
virtual bool SetMatrixClasses(const MDMatrix<int> &value);
virtual bool SetMatrixSpikeDelays(const MDMatrix<double> &value);

protected:
/// Promote dims and matrices to public parameters for GUI editing
void PromoteEditableProperties(void);

/// Resize MatrixData, MatrixClasses and MatrixSpikeDelays from dims
void ResizeMatricesFromDims(int num_samples, int num_features, int max_spikes);

virtual bool PrepareDataset(void);
virtual bool ADefault(void);
};

}
#endif
