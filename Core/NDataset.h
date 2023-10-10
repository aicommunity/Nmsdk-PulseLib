#ifndef NDATASET_H
#define NDATASET_H

#include "NPulseGenerator.h"
#include "../../../Rdk/Deploy/Include/rdk.h"

namespace NMSDK {

using namespace RDK;

// �����, ���������� �� �������
class RDK_LIB_TYPE NDataset: public UNet
{
/// ��� ������, ���������� ��������� ���������
ULProperty<std::string, NDataset, ptPubParameter> PulseGeneratorClassName;

///����� �����������
ULProperty<int,NDataset, ptPubParameter> NumGenerators;

///����� ��������� (�� �����)
ULProperty<int,NDataset, ptPubParameter> NumFeatures;

///����� �������� (�� �����)
ULProperty<int,NDataset, ptPubParameter> NumSamples;

///������� ��������� ����
ULProperty<MDMatrix<double>,NDataset, ptPubParameter> MatrixData;

///������� ��������� �������
ULProperty<MDMatrix<int>,NDataset, ptPubParameter> MatrixClasses;

///���� ������� ��������� �������� ������
ULProperty<bool,NDataset, ptPubParameter> ReloadDataset;

///������� ��������� ������� �������� �����������
ULProperty<MDMatrix<double>,NDataset, ptPubState> MatrixDelay;

///����� ������ � ��������
ULProperty<int,NDataset, ptPubParameter> Iteration;

///����������� �� ��������� ���������
ULProperty<float,NDataset, ptPubParameter> Tay;

/// ����� �������� ������ �������� ������������ ������ ������� (���)
ULProperty<double, NDataset, ptPubParameter> Delay;

///�������� ���������, ����� ������ ������: ���� =0, �� ���������� ���������
///���� =1, �� ���������� ����� ������ �������
///���� =2, �� �������� ����������� �����
ULProperty<int, NDataset, ptPubState> StateGeneration;

///����� ���������, � ��������
ULProperty<double, NDataset, ptPubState> TimeGeneration;

///����� ������
ULProperty<double, NDataset, ptPubState> OperatingTime;

///�������� ��������� ������ �����������
ULProperty<bool, NDataset, ptPubState> ResetDelay;

/// ������� ����������� (��)
ULProperty<double, NDataset, ptPubParameter> SpikesFrequency;

/// ����� ���������������� �������
ULProperty<int,NDataset, ptPubState> NumClasses;

/// ����������
vector<NPulseGeneratorTransit*> Generators;

///���� � ���������
ULProperty<string,NDataset, ptPubState> FileName;

protected:

 // <-------

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NDataset(void);
virtual ~NDataset(void);
// --------------------------

// --------------------------
// ������ ��������� �����������
// --------------------------

/// ��� ������, ���������� ��������� ���������
bool SetPulseGeneratorClassName(const std::string &value);

///����� �����������
bool SetNumGenerators(const int &value);

///����� ��������� (�� �����)
bool SetNumFeatures(const int &value);

///������� ��������� ����
bool SetMatrixData(const MDMatrix<double> &value);

///������� ��������� �������
bool SetMatrixClasses(const MDMatrix<int> &value);

/// ������� ����������� (��)
bool SetSpikesFrequency(const double &value);

/// ����� �������� ������ �������� ������������ ������ ������� (���)
bool SetDelay(const double &value);

/// ��������� ����������� ���������������� ��������
/// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ��� ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer = 0);

/// ��������� ��������������� ���������������� ��������
/// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NDataset* New(void);
static UComponent* NewStatic(void);
// --------------------------


// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
// ������� ��� ������ � �������.
// ������������ ������ ������� ������ �� �����,
// ��������� �����������
bool TreatDataFromFile(void);

// �������������� �������� �� ��������� � ����� �������� �����
virtual bool ADefault(void);

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
virtual bool ABuild(void);

// ����� �������� �����.
virtual bool AReset(void);

// ��������� ������ ����� �������
virtual bool ACalculate(void);
// --------------------------
};

}
#endif
