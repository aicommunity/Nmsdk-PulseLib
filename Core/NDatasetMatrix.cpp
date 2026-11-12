#ifndef NDATASETMATRIX_CPP
#define NDATASETMATRIX_CPP

#include "NDatasetMatrix.h"

namespace NMSDK {

NDatasetMatrix::NDatasetMatrix(void)
{
    PromoteEditableProperties();
}

NDatasetMatrix::~NDatasetMatrix(void)
{
}

void NDatasetMatrix::PromoteEditableProperties(void)
{
    ChangeLookupPropertyType("NumFeatures", ptPubParameter);
    ChangeLookupPropertyType("NumSamples", ptPubParameter);
    ChangeLookupPropertyType("MaxSpikesPerFeature", ptPubParameter);
    ChangeLookupPropertyType("MatrixData", ptPubParameter);
    ChangeLookupPropertyType("MatrixClasses", ptPubParameter);
    ChangeLookupPropertyType("AdvanceSampleAfterBurst", ptPubParameter);
}

void NDatasetMatrix::ResizeMatricesFromDims(int num_samples, int num_features, int max_spikes)
{
    if(num_samples < 0)
        num_samples = 0;
    if(num_features < 0)
        num_features = 0;
    if(max_spikes < 1)
        max_spikes = 1;

    MatrixData.Resize(num_samples * max_spikes, num_features, -1.0);
    MatrixClasses.Resize(num_samples > 0 ? 1 : 0, num_samples);
}

void NDatasetMatrix::SyncMatricesToDimParams(void)
{
    int num_samples = NumSamples;
    int num_features = NumFeatures;
    int max_spikes = MaxSpikesPerFeature;

    if(num_samples < 0)
    {
        num_samples = 0;
        NumSamples.SetDataDirect(0);
    }
    if(num_features < 0)
    {
        num_features = 0;
        NumFeatures.SetDataDirect(0);
    }
    if(max_spikes < 1)
    {
        max_spikes = 1;
        MaxSpikesPerFeature.SetDataDirect(1);
    }

    ResizeMatricesFromDims(num_samples, num_features, max_spikes);

    if(MatrixClasses.GetRows() == 1 && MatrixClasses.GetCols() == num_samples && num_samples > 0)
        NumClasses = CalcNumClasses(MatrixClasses);
    else
        NumClasses = 0;
}

NDatasetMatrix* NDatasetMatrix::New(void)
{
    return new NDatasetMatrix;
}

UComponent* NDatasetMatrix::NewStatic(void)
{
    return new NDatasetMatrix;
}

bool NDatasetMatrix::SetNumFeatures(const int &value)
{
    if(value < 0)
        return false;
    NumFeatures.SetDataDirect(value);
    SyncMatricesToDimParams();
    Ready = false;
    return true;
}

bool NDatasetMatrix::SetNumSamples(const int &value)
{
    if(value < 0)
        return false;
    NumSamples.SetDataDirect(value);
    SyncMatricesToDimParams();
    Ready = false;
    return true;
}

bool NDatasetMatrix::SetMaxSpikesPerFeature(const int &value)
{
    if(value < 1)
        return false;
    MaxSpikesPerFeature.SetDataDirect(value);
    SyncMatricesToDimParams();
    Ready = false;
    return true;
}

bool NDatasetMatrix::SetMatrixData(const MDMatrix<double> &value)
{
    const int max_spikes = MaxSpikesPerFeature < 1 ? 1 : int(MaxSpikesPerFeature);
    if(value.GetRows() > 0 && value.GetRows() % max_spikes != 0)
        return false;

    NumFeatures.SetDataDirect(value.GetCols());
    NumSamples.SetDataDirect(value.GetRows() > 0 ? value.GetRows() / max_spikes : 0);

    const int num_samples = NumSamples;
    if(num_samples > 0)
    {
        if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != num_samples)
            MatrixClasses.Resize(1, num_samples);
    }
    else
    {
        MatrixClasses.Resize(0, 0);
    }

    // Store payload when SetMatrixData() is invoked directly (not only via UProperty::SetData).
    MatrixData.SetDataDirect(value);
    Ready = false;
    return true;
}

bool NDatasetMatrix::SetMatrixClasses(const MDMatrix<int> &value)
{
    MatrixClasses.SetDataDirect(value);
    if(value.GetRows() == 1 && value.GetCols() > 0)
        NumClasses = CalcNumClasses(value);
    else
        NumClasses = 0;
    Ready = false;
    return true;
}

bool NDatasetMatrix::ADefault(void)
{
    NDatasetBase::ADefault();
    PromoteEditableProperties();
    NumSamples.SetDataDirect(1);
    NumFeatures.SetDataDirect(1);
    MaxSpikesPerFeature.SetDataDirect(1);
    SyncMatricesToDimParams();
    MatrixData(0, 0) = 0.0;
    NumClasses = CalcNumClasses(MatrixClasses);
    Ready = false;
    return true;
}

bool NDatasetMatrix::AReset(void)
{
    SyncMatricesToDimParams();
    return NDatasetBase::AReset();
}

bool NDatasetMatrix::PrepareDataset(void)
{
    SyncMatricesToDimParams();

    const int max_spikes = MaxSpikesPerFeature;
    if(max_spikes < 1)
        return false;
    if(NumSamples <= 0 || NumFeatures <= 0)
        return false;

    if(MatrixData.GetRows() != NumSamples * max_spikes ||
       MatrixData.GetCols() != NumFeatures)
        return false;
    if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != NumSamples)
        return false;
    return true;
}

}
#endif
