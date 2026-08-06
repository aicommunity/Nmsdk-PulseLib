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
virtual bool SetMatrixData(const MDMatrix<double> &value);
virtual bool SetMatrixClasses(const MDMatrix<int> &value);

protected:
/// Promote dims and matrices to public parameters for GUI editing
void PromoteEditableProperties(void);

/// Resize MatrixData (samples x features) and MatrixClasses (1 x samples)
void ResizeMatricesFromDims(int num_samples, int num_features);

virtual bool PrepareDataset(void);
virtual bool ADefault(void);
};

}
#endif
