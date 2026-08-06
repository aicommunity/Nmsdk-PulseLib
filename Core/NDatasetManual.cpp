#ifndef NDATASETMANUAL_CPP
#define NDATASETMANUAL_CPP

#include "NDatasetManual.h"

namespace NMSDK {

NDatasetManual::NDatasetManual(void)
{
    PromoteMatricesToParameters();
}

NDatasetManual::~NDatasetManual(void)
{
}

void NDatasetManual::PromoteMatricesToParameters(void)
{
    ChangeLookupPropertyType("MatrixData", ptPubParameter);
    ChangeLookupPropertyType("MatrixClasses", ptPubParameter);
}

NDatasetManual* NDatasetManual::New(void)
{
    return new NDatasetManual;
}

UComponent* NDatasetManual::NewStatic(void)
{
    return new NDatasetManual;
}

bool NDatasetManual::ADefault(void)
{
    NDatasetBase::ADefault();
    PromoteMatricesToParameters();
    return true;
}

bool NDatasetManual::PrepareDataset(void)
{
    if(MatrixData.GetRows() <= 0 || MatrixData.GetCols() <= 0)
        return false;
    if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != MatrixData.GetRows())
        return false;
    return true;
}

}
#endif
