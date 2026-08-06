#ifndef NDATASETMATRIX_H
#define NDATASETMATRIX_H

#include "NDatasetBase.h"

namespace NMSDK {

using namespace RDK;

/// Matrix dataset: dims resize MatrixData/MatrixClasses; MatrixData holds ISI spike trains
class RDK_LIB_TYPE NDatasetMatrix: public NDatasetBase
{
public:
NDatasetMatrix(void);
virtual ~NDatasetMatrix(void);

virtual NDatasetMatrix* New(void);
static UComponent* NewStatic(void);

virtual bool SetNumFeatures(const int &value);
virtual bool SetNumSamples(const int &value);
virtual bool SetMaxSpikesPerFeature(const int &value);
virtual bool SetMatrixData(const MDMatrix<double> &value);
virtual bool SetMatrixClasses(const MDMatrix<int> &value);

protected:
void PromoteEditableProperties(void);
void ResizeMatricesFromDims(int num_samples, int num_features, int max_spikes);
void SyncMatricesToDimParams(void);

virtual bool PrepareDataset(void);
virtual bool ADefault(void);
virtual bool AReset(void);
};

}
#endif
