#ifndef NDATASETMANUAL_CPP
#define NDATASETMANUAL_CPP

#include "NDatasetManual.h"

namespace NMSDK {

NDatasetManual::NDatasetManual(void)
{
    PromoteEditableProperties();
}

NDatasetManual::~NDatasetManual(void)
{
}

void NDatasetManual::PromoteEditableProperties(void)
{
    ChangeLookupPropertyType("NumFeatures", ptPubParameter);
    ChangeLookupPropertyType("NumSamples", ptPubParameter);
    ChangeLookupPropertyType("MatrixData", ptPubParameter);
    ChangeLookupPropertyType("MatrixClasses", ptPubParameter);
}

void NDatasetManual::ResizeMatricesFromDims(int num_samples, int num_features)
{
    if(num_samples < 0)
        num_samples = 0;
    if(num_features < 0)
        num_features = 0;

    MatrixData.Resize(num_samples, num_features);
    MatrixClasses.Resize(num_samples > 0 ? 1 : 0, num_samples);
}

NDatasetManual* NDatasetManual::New(void)
{
    return new NDatasetManual;
}

UComponent* NDatasetManual::NewStatic(void)
{
    return new NDatasetManual;
}

bool NDatasetManual::SetNumFeatures(const int &value)
{
    if(value < 0)
        return false;
    ResizeMatricesFromDims(NumSamples, value);
    Ready = false;
    return true;
}

bool NDatasetManual::SetNumSamples(const int &value)
{
    if(value < 0)
        return false;
    ResizeMatricesFromDims(value, NumFeatures);
    Ready = false;
    return true;
}

bool NDatasetManual::SetMatrixData(const MDMatrix<double> &value)
{
    NumSamples.SetDataDirect(value.GetRows());
    NumFeatures.SetDataDirect(value.GetCols());
    if(value.GetRows() > 0)
    {
        if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != value.GetRows())
            MatrixClasses.Resize(1, value.GetRows());
    }
    else
    {
        MatrixClasses.Resize(0, 0);
    }
    Ready = false;
    return true;
}

bool NDatasetManual::SetMatrixClasses(const MDMatrix<int> &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetManual::ADefault(void)
{
    NDatasetBase::ADefault();
    PromoteEditableProperties();
    NumSamples.SetDataDirect(1);
    NumFeatures.SetDataDirect(1);
    ResizeMatricesFromDims(1, 1);
    Ready = false;
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
