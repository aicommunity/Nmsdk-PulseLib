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
    ChangeLookupPropertyType("MatrixSpikeDelays", ptPubParameter);
}

void NDatasetManual::ResizeMatricesFromDims(int num_samples, int num_features, int max_spikes)
{
    if(num_samples < 0)
        num_samples = 0;
    if(num_features < 0)
        num_features = 0;
    if(max_spikes < 1)
        max_spikes = 1;

    MatrixData.Resize(num_samples, num_features);
    MatrixClasses.Resize(num_samples > 0 ? 1 : 0, num_samples);
    MatrixSpikeDelays.Resize(num_samples, num_features * max_spikes, -1.0);
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
    ResizeMatricesFromDims(NumSamples, value, MaxSpikesPerFeature);
    Ready = false;
    return true;
}

bool NDatasetManual::SetNumSamples(const int &value)
{
    if(value < 0)
        return false;
    ResizeMatricesFromDims(value, NumFeatures, MaxSpikesPerFeature);
    Ready = false;
    return true;
}

bool NDatasetManual::SetMaxSpikesPerFeature(const int &value)
{
    if(value < 1)
        return false;
    ResizeMatricesFromDims(NumSamples, NumFeatures, value);
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

    const int max_spikes = MaxSpikesPerFeature < 1 ? 1 : int(MaxSpikesPerFeature);
    const int expected_cols = value.GetCols() * max_spikes;
    if(MatrixSpikeDelays.GetRows() != value.GetRows() || MatrixSpikeDelays.GetCols() != expected_cols)
        MatrixSpikeDelays.Resize(value.GetRows(), expected_cols, -1.0);

    Ready = false;
    return true;
}

bool NDatasetManual::SetMatrixClasses(const MDMatrix<int> &value)
{
    (void)value;
    Ready = false;
    return true;
}

bool NDatasetManual::SetMatrixSpikeDelays(const MDMatrix<double> &value)
{
    const int max_spikes = MaxSpikesPerFeature < 1 ? 1 : int(MaxSpikesPerFeature);
    if(value.GetCols() > 0 && max_spikes > 0 && value.GetCols() % max_spikes == 0)
    {
        NumSamples.SetDataDirect(value.GetRows());
        NumFeatures.SetDataDirect(value.GetCols() / max_spikes);
        if(value.GetRows() > 0)
        {
            if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != value.GetRows())
                MatrixClasses.Resize(1, value.GetRows());
            if(MatrixData.GetRows() != value.GetRows() ||
               MatrixData.GetCols() != value.GetCols() / max_spikes)
                MatrixData.Resize(value.GetRows(), value.GetCols() / max_spikes);
        }
        else
        {
            MatrixClasses.Resize(0, 0);
            MatrixData.Resize(0, 0);
        }
    }
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
    ResizeMatricesFromDims(1, 1, 1);
    // One spike at sample start so Reset creates Generator1 and fires once in spike-train mode
    MatrixSpikeDelays(0, 0) = 0.0;
    Ready = false;
    return true;
}

bool NDatasetManual::PrepareDataset(void)
{
    const int max_spikes = MaxSpikesPerFeature;
    if(max_spikes < 1)
        return false;

    const int delay_rows = MatrixSpikeDelays.GetRows();
    const int delay_cols = MatrixSpikeDelays.GetCols();
    if(delay_rows > 0 && delay_cols > 0)
    {
        if(delay_cols % max_spikes != 0)
            return false;
        if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != delay_rows)
            return false;
        return true;
    }

    if(MatrixData.GetRows() <= 0 || MatrixData.GetCols() <= 0)
        return false;
    if(MatrixClasses.GetRows() != 1 || MatrixClasses.GetCols() != MatrixData.GetRows())
        return false;
    return true;
}

}
#endif
