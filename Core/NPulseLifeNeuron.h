// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_LIFE_NEURON_H
#define NPULSE_LIFE_NEURON_H

#include "../../Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include "NNeuronLife.h"

namespace NMSDK {

class RDK_LIB_TYPE NPulseLifeNeuron: public NPulseNeuron
{
public: // ������
/// ��������� ������������ ��� ������������ ������
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosGd;

/// ��������� ����������� ��� ������������ ������
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosGs;

/// ��������� �������������� ��� ������������ ������
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryPosG;

/// ��������� ������������ ��� ��������� ������
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegGd;

/// ��������� ����������� ��� ��������� ������
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegGs;

/// ��������� �������������� ��� ��������� ������
ULProperty<double, NPulseLifeNeuron,ptPubState> SummaryNegG;

public: // ����� � ������
/// �����: ��������� ������������ ��� ������������ ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGd;

/// �����: ��������� ������������ ��� ������������ ������ ���������� � ����� �������� ������������ ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGdNorm;

/// �����: ��������� ����������� ��� ������������ ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGs;

/// �����: ��������� ����������� ��� ������������ ������ ���������� � ����� �������� ������������ ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGsNorm;

/// ��������� �������������� ��� ������������ ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosG;

/// ��������� �������������� ��� ������������ ������ ���������� � ����� �������� ������������ ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryPosGNorm;

/// �����: ��������� ������������ ��� ��������� ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGd;

/// �����: ��������� ������������ ��� ��������� ������ ���������� � ����� �������� ��������� ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGdNorm;

/// �����: ��������� ����������� ��� ��������� ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGs;

/// �����: ��������� ����������� ��� ��������� ������ ���������� � ����� �������� ��������� ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGsNorm;

/// ��������� �������������� ��� ��������� ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegG;

/// ��������� �������������� ��� ��������� ������ ���������� � ����� �������� ��������� ������
UPropertyOutputData<MDMatrix<double>, NPulseLifeNeuron,ptOutput | ptPubState> OutputSummaryNegGNorm;

protected: // ��������� ����������

UEPointer<NNeuronLife,NPulseLifeNeuron> NeuronLife;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLifeNeuron(void);
virtual ~NPulseLifeNeuron(void);
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ ����������������
NNeuronLife* GetNeuronLife(void);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseLifeNeuron* New(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
protected:
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
