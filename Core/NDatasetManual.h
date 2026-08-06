#ifndef NDATASETMANUAL_H
#define NDATASETMANUAL_H

#include "NDatasetBase.h"

namespace NMSDK {

using namespace RDK;

/// Manual dataset: MatrixData/MatrixClasses are editable parameters (via ChangeLookupPropertyType)
class RDK_LIB_TYPE NDatasetManual: public NDatasetBase
{
public:
NDatasetManual(void);
virtual ~NDatasetManual(void);

virtual NDatasetManual* New(void);
static UComponent* NewStatic(void);

protected:
/// Promote MatrixData/MatrixClasses to public parameters for GUI editing
void PromoteMatricesToParameters(void);

virtual bool PrepareDataset(void);
virtual bool ADefault(void);
};

}
#endif
