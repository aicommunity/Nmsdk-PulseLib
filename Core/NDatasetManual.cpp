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
    ChangeLookupPropertyType("MaxSpikesPerFeature", ptPubParameter);
    ChangeLookupPropertyType("MatrixData", ptPubParameter);
    ChangeLookupPropertyType("MatrixClasses", ptPubParameter);
}

void NDatasetManual::ResizeMatricesFromDims(int num_samples, int num_features, int max_spikes)
{
    if(num_samples < 0)
        num_samples = 0;
    if(num_features < 0)
        num_features = 0;
    if(max_spikes < 1)
        max_spikes = 1;

    MatrixData.Resize(num_samples, num_features * max_spikes, -1.0);
    MatrixClasses.Resize(num_samples > 0 ? 1 : 0, num_samples);
}

void NDatasetManual::SyncMatricesToDimParams(void)
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
    NumFeatures.SetDataDirect(value);
    SyncMatricesToDimParams();
    Ready = false;
    return true;
}

bool NDatasetManual::SetNumSamples(const int &value)
{
    if(value < 0)
        return false;
    NumSamples.SetDataDirect(value);
    SyncMatricesToDimParams();
    Ready = false;
    return true;
}

bool NDatasetManual::SetMaxSpikesPerFeature(const int &value)
{
    if(value < 1)
        return false;
    MaxSpikesPerFeature.SetDataDirect(value);
    SyncMatricesToDimParams();
    Ready = false;
    return true;
}

bool NDatasetManual::SetMatrixData(const MDMatrix<double> &value)
{
    const int max_spikes = MaxSpikesPerFeature < 1 ? 1 : int(MaxSpikesPerFeature);
    if(value.GetCols() > 0 && value.GetCols() % max_spikes != 0)
        return false;

    NumSamples.SetDataDirect(value.GetRows());
    if(value.GetCols() > 0)
        NumFeatures.SetDataDirect(value.GetCols() / max_spikes);
    else
        NumFeatures.SetDataDirect(0);

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
    if(value.GetRows() == 1 && value.GetCols() > 0)
        NumClasses = CalcNumClasses(value);
    else
        NumClasses = 0;
    Ready = false;
    return true;
}

bool NDatasetManual::ADefault(void)
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

bool NDatasetManual::AReset(void)
{
    SyncMatricesToDimParams();
    return NDatasetBase::AReset();
}

bool NDatasetManual::PrepareDataset(void)
{
    SyncMatricesToDimParams();

    const int max_spikes = MaxSpikesPerFeature;
    if(max_spikes < 1)
        return false;
    if(NumSamples <= 0 || NumFeatures <= 0)
        return false;

    if(MatrixData.GetRows() != NumSamples ||
       MatrixData.GetCols() != NumFeatures * max_spikes)
        return false;
    if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != NumSamples)
        return false;
    return true;
}

}
#endif
