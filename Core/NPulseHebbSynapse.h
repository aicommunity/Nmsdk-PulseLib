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

#ifndef NPULSE_HEBB_SYNAPSE_H
#define NPULSE_HEBB_SYNAPSE_H

#include "../../Kernel/NBios.h"
#include "NPulseSynapse.h"


namespace NMSDK {

class NPulseHebbSynapse: public NPulseSynapse
{
public: // ������������� ��������
RDK::ULProperty<real,NPulseHebbSynapse> Min;
RDK::ULProperty<real,NPulseHebbSynapse> Mout;
RDK::ULProperty<real,NPulseHebbSynapse> Md;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse> ActiveMs;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse> PassiveMs;
RDK::UCLProperty<vector<real>,NPulseHebbSynapse> Kmot;
RDK::ULProperty<real,NPulseHebbSynapse> Kin;
RDK::ULProperty<real,NPulseHebbSynapse> Kout;

// �-� �������� ������������ �����
RDK::ULProperty<real,NPulseHebbSynapse> GdGain;

// �-� �������� ����������� �����
RDK::ULProperty<real,NPulseHebbSynapse> GsGain;


protected: // �������� ��������

protected: // ��������� ����������
RDK::ULStateProperty<real,NPulseHebbSynapse> G;
RDK::ULStateProperty<real,NPulseHebbSynapse> Gd;
RDK::UCLStateProperty<vector<real>,NPulseHebbSynapse> Gs;
RDK::ULStateProperty<real,NPulseHebbSynapse> Win;
RDK::ULStateProperty<real,NPulseHebbSynapse> Wout;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseHebbSynapse(void);
virtual ~NPulseHebbSynapse(void);
// --------------------------

protected:
// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
//bool SetPulseAmplitude(real value);

// ���������� ������� ��������� ���������
//bool SetSecretionTC(real value);

// ���������� ������� ������� ���������
//bool SetDissociationTC(real value);

// ����������� ����������������� ����������
//bool SetInhibitionCoeff(real value);

// ��� (������������� �������) �������
//bool SetResistance(real value);
// --------------------------

public:
// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
virtual NPulseHebbSynapse* New(void);
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
