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

#ifndef NPULSE_LIFE_NEURON_CPP
#define NPULSE_LIFE_NEURON_CPP

#include "NPulseLifeNeuron.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseLifeNeuron::NPulseLifeNeuron(void)
//: NPulseNeuron(name),
 : SummaryPosGd("SummaryPosGd",this),
  SummaryPosGs("SummaryPosGs",this),
  SummaryPosG("SummaryPosG",this),
  SummaryNegGd("SummaryNegGd",this),
  SummaryNegGs("SummaryNegGs",this),
  SummaryNegG("SummaryNegG",this),
 OutputSummaryPosGd("OutputSummaryPosGd",this),
 OutputSummaryPosGdNorm("OutputSummaryPosGdNorm",this),
 OutputSummaryPosGs("OutputSummaryPosGs",this),
 OutputSummaryPosGsNorm("OutputSummaryPosGsNorm",this),
 OutputSummaryPosG("OutputSummaryPosG",this),
 OutputSummaryPosGNorm("OutputSummaryPosGNorm",this),

 OutputSummaryNegGd("OutputSummaryNegGd",this),
 OutputSummaryNegGdNorm("OutputSummaryNegGdNorm",this),
 OutputSummaryNegGs("OutputSummaryNegGs",this),
 OutputSummaryNegGsNorm("OutputSummaryNegGsNorm",this),
 OutputSummaryNegG("OutputSummaryNegG",this),
 OutputSummaryNegGNorm("OutputSummaryNegGNorm",this),
 NeuronLife("NeuronLife",this)
{
 NeuronLife=0;

  fstream stream;
}

NPulseLifeNeuron::~NPulseLifeNeuron(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ��������� ����������
// --------------------------
// ���������� ��������� �� ������ ����������������
NNeuronLife* NPulseLifeNeuron::GetNeuronLife(void)
{
 return NeuronLife;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseLifeNeuron* NPulseLifeNeuron::New(void)
{
 return new NPulseLifeNeuron;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseLifeNeuron::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(NPulseNeuron::CheckComponentType(comp) || dynamic_pointer_cast<NNeuronLife>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseLifeNeuron::ADefault(void)
{
 if(!NPulseNeuron::ADefault())
  return false;

 OutputSummaryPosGd.Assign(1,1,0.0);
 OutputSummaryPosGdNorm.Assign(1,1,0.0);
 OutputSummaryPosGs.Assign(1,1,0.0);
 OutputSummaryPosGsNorm.Assign(1,1,0.0);
 OutputSummaryPosG.Assign(1,1,0.0);
 OutputSummaryPosGNorm.Assign(1,1,0.0);

 OutputSummaryNegGd.Assign(1,1,0.0);
 OutputSummaryNegGdNorm.Assign(1,1,0.0);
 OutputSummaryNegGs.Assign(1,1,0.0);
 OutputSummaryNegGsNorm.Assign(1,1,0.0);
 OutputSummaryNegG.Assign(1,1,0.0);
 OutputSummaryNegGNorm.Assign(1,1,0.0);
   /*
 SetNumOutputs(NumOutputs+12);
 for(int i=NumOutputs-12;i<NumOutputs;i++)
  SetOutputDataSize(i,MMatrixSize(1,1));
	*/
 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseLifeNeuron::ABuild(void)
{
 if(!NPulseNeuron::ABuild())
  return false;

 bool res(true);

 UEPtr<NNeuronLife> nlife=AddMissingComponent<NNeuronLife>("NeuronLife","NNeuronLife",&NeuronLife);
 if(LTZone)
  res&=CreateLink(LTZone->GetLongName(this),"Output",nlife->GetLongName(this),"Input1");

 return true;
}

// ����� �������� �����.
bool NPulseLifeNeuron::AReset(void)
{
 if(!NPulseNeuron::AReset())
  return false;

 // ��������� ������������ ���
 SummaryPosGd=0;
 SummaryNegGd=0;

 // ��������� ����������� ���
 SummaryPosGs=0;
 SummaryNegGs=0;

 // ��������� �������������� ���
 SummaryPosG=0;
 SummaryNegG=0;

 OutputSummaryPosGd.ToZero();
 OutputSummaryPosGdNorm.ToZero();
 OutputSummaryPosGs.ToZero();
 OutputSummaryPosGsNorm.ToZero();
 OutputSummaryPosG.ToZero();
 OutputSummaryPosGNorm.ToZero();

 OutputSummaryNegGd.ToZero();
 OutputSummaryNegGdNorm.ToZero();
 OutputSummaryNegGs.ToZero();
 OutputSummaryNegGsNorm.ToZero();
 OutputSummaryNegG.ToZero();
 OutputSummaryNegGNorm.ToZero();
 return true;
}

// ��������� ������ ����� �������
bool NPulseLifeNeuron::ACalculate(void)
{
 if(/*!LTZone ||*/ !NeuronLife)
  return false;

 if(!NPulseNeuron::ACalculate())
  return false;

 LTZone->Threshold=NeuronLife->OutputThreshold(0,0);

 // ��������� ������������ ��� ������������ �����
 OutputSummaryPosGd(0,0)=SummaryPosGd;
 if(ActivePosInputs(0,0) > 0)
  OutputSummaryPosGdNorm(0,0)=SummaryPosGd/ActivePosInputs(0,0);
 else
  OutputSummaryPosGdNorm(0,0)=0;
 SummaryPosGd=0;

 // ��������� ����������� ��� ������������ �����
 OutputSummaryPosGs(0,0)=SummaryPosGs;
 if(ActivePosInputs(0,0) > 0)
  OutputSummaryPosGsNorm(0,0)=SummaryPosGs/ActivePosInputs(0,0);
 else
  OutputSummaryPosGsNorm(0,0)=0;
 SummaryPosGs=0;

 // ��������� �������������� ��� ������������ �����
 OutputSummaryPosG(0,0)=SummaryPosG;
 if(ActivePosInputs(0,0) > 0)
  OutputSummaryPosGNorm(0,0)=SummaryPosG/ActivePosInputs(0,0);
 else
  OutputSummaryPosGNorm(0,0)=0;
 SummaryPosG=0;

 // ��������� ������������ ��� ��������� �����
 OutputSummaryNegGd(0,0)=SummaryNegGd;
 if(ActiveNegInputs(0,0) > 0)
  OutputSummaryNegGdNorm(0,0)=SummaryNegGd/ActiveNegInputs(0,0);
 else
  OutputSummaryNegGdNorm(0,0)=0;
 SummaryNegGd=0;

 // ��������� ����������� ��� ��������� �����
 OutputSummaryNegGs(0,0)=SummaryNegGs;
 if(ActiveNegInputs(0,0) > 0)
  OutputSummaryNegGsNorm(0,0)=SummaryNegGs/ActiveNegInputs(0,0);
 else
  OutputSummaryNegGsNorm(0,0)=0;
 SummaryNegGs=0;

 // ��������� �������������� ��� ��������� �����
 OutputSummaryNegG(0,0)=SummaryNegG;
 if(ActiveNegInputs(0,0) != 0)
  OutputSummaryNegGNorm(0,0)=SummaryNegG/ActiveNegInputs(0,0);
 else
  OutputSummaryNegGNorm(0,0)=0;
 SummaryNegG=0;

 return true;
}
// --------------------------

}
#endif
