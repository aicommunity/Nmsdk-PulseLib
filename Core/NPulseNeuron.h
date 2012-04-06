// ===========================================================
// Version:        3.0.2
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_NEURON_H
#define NPULSE_NEURON_H

#include "../BCL/NNeuron.h"
#include "NPulseLTZone.h"
#include "../SourceLib/NConstGenerator.h"
#include "NPulseMembrane.h"

namespace NMSDK {

class NPulseNeuron: public NNeuron
{
public: // ����������
// ����� ������ �������������� ���� �������� �� ������ (� ����)
RDK::ULStateProperty<real,NPulseNeuron> NumActiveOutputs;

// ����� ������������ ������ �������������� ������� ��������� �� ����
RDK::ULStateProperty<real,NPulseNeuron> NumActivePosInputs;

// ����� ��������� ������ �������������� ������� ��������� �� ����
RDK::ULStateProperty<real,NPulseNeuron> NumActiveNegInputs;

protected: // ��������� ����������
//NPulseLTZone *LTZone;

public:
RDK::UEPointer<NPulseLTZone,NPulseNeuron> LTZone;

NConstGenerator *PosGenerator,*NegGenerator;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseNeuron(void);
virtual ~NPulseNeuron(void);
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ �������������� ����
NPulseLTZone* GetLTZone(void);

// ���������� ��������� �� ������ ��������� ������������� ����������
NConstGenerator* GetPosGenerator(void);

// ���������� ��������� �� ������ ��������� ���������� ����������
NConstGenerator* GetNegGenerator(void);
// --------------------------

// --------------------------
// ������ ���������� ���������� �������
// --------------------------
// ��������� �������� ������� ��������, �������� � ���� ����� ������� ��������,
// � ���������� ����� ��������� ������� �� ����� ������
// ���������� ��������� �� ��������� �������
NPulseMembrane* ElongateDendrite(const UId &id, bool feedback=false);

// ����������� �������� ������� ��������, �������� � ����� ��� �����������
// ������������� ����� ������� ��������
// ���������� ��������� �� ��������� �������
NPulseMembrane* BranchDendrite(const UId &id, bool feedback=false);

// ������� �������� ������� ��������
// ���� full == true, �� ������� � ��� ������ �������, ������������ � ����
// ����� �������������� ����� �� ������ �� ���� ������
bool EraseDendrite(const UId &id);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseNeuron* New(void);
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UAContainer> comp);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<NAContainer> comp) const;
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
