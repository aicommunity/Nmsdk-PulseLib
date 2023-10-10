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

#ifndef NNEURON_LIFE_CPP
#define NNEURON_LIFE_CPP

#include "NNeuronLife.h"
#include "../../Nmsdk-PulseLib/Core/NPulseChannel.h"


namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NNeuronLife::NNeuronLife(void)
 : Threshold("Threshold",this,&NNeuronLife::SetThreshold),
  CriticalEnergy("CriticalEnergy",this),
  MaxPotentialGradient("MaxPotentialGradient",this,&NNeuronLife::SetMaxPotentialGradient),
  WearOutConstPositive("WearOutConstPositive",this),
  WearOutConstNegative("WearOutConstNegative",this),
  Kw("Kw",this),
  Emax("Emax",this,&NNeuronLife::SetEmax),
  En("En",this,&NNeuronLife::SetEn),
  Kq("Kq",this),
  Ee0("Ee0",this),
  Es("Es",this),
  Eh0("Eh0",this),
  Econst("Econst",this),
  Ecr("Ecr",this),
  WearOutcr("WearOutcr",this),
  EnergyWearOutCritical("EnergyWearOutCritical",this),
  EyConst("EyConst",this),
  EyBonusPos("EyBonusPos",this),
  EyBonusNeg("EyBonusNeg",this),
  EnergyComprehensibility("EnergyComprehensibility",this),
  Kdp("Kdp",this),
  Pdmax("Pdmax",this,&NNeuronLife::SetPdmax),
  Qd("Qd",this),
  Khp0("Khp0",this),
  Khp1("Khp1",this),
  Qsummax("Qsummax",this,&NNeuronLife::SetQsummax),
  EnergyBonus("EnergyBonus",this),
  Energy("Energy",this),
  Input1("Input1",this),
  Input2("Input2",this),
  Input3("Input3",this),
  Output1("Output1",this),
  Output2("Output2",this),
  Output3("Output3",this),
  OutputThreshold("OutputThreshold",this),
  Output5("Output5",this),
  Output6("Output6",this),
  Output7("Output7",this),
  Output8("Output8",this),

  WearOut("WearOut",this),
  ThresholdLife("ThresholdLife",this),
  ThresholdWearOut("ThresholdWearOut",this),
  ThresholdFeel("ThresholdFeel",this),
  Feel("Feel",this),
  Qsum("Qsum",this),
  Esum("Esum",this),
  EsumOld("EsumOld",this),
  EsumProizv("EsumProizv",this),
  Ea("Ea",this),
  Ey("Ey",this),
  Ee("Ee",this),
  Eh("Eh",this),
  dEa("dEa",this),
  dEy("dEy",this),
  dEe("dEe",this),
  dEh("dEh",this),
  dE("dE",this),

  Usum("Usum",this),
  ExternalEnergyBonus("ExternalEnergyBonus",this)

{
}

NNeuronLife::~NNeuronLife(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NNeuronLife::SetThreshold(const double &value)
{
 return true;
}

// ������������� ��������� �������� ������������� ��������� ������ �������
bool NNeuronLife::SetMaxPotentialGradient(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;
 return true;
}

bool NNeuronLife::SetEmax(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;
 return true;
}

bool NNeuronLife::SetEn(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;
 return true;
}

bool NNeuronLife::SetPdmax(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;
 return true;
}

bool NNeuronLife::SetQsummax(const double &value)
{
 if(value <= 1)
  return false;

 Ready=false;
 return true;
}
// --------------------------


// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NNeuronLife* NNeuronLife::New(void)
{
 return new NNeuronLife;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NNeuronLife::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NSum>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------


// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NNeuronLife::ABuild(void)
{
 AddMissingComponent<NSum>("ExternalEnergyBonus","NSum",&ExternalEnergyBonus);
 return true;
}
 // �������������� �������� �� ��������� � ����� �������� �����
bool NNeuronLife::ADefault(void)
{
 Output1.Assign(1,4,0.0);
 Output2.Assign(1,1,0.0);
 Output3.Assign(1,4,0.0);
 OutputThreshold.Assign(1,1,0.0);
 Output5.Assign(1,5,0.0);
 Output6.Assign(1,5,0.0);
 Output7.Assign(1,1,0.0);
 Output8.Assign(1,1,0.0);


 // ��������� �������� ���� ����������
 Threshold=static_cast<double>(0.001);
 WearOutConstPositive = static_cast<double>(100);//0.01;
 WearOutConstNegative = 1;//0.1;
 MaxPotentialGradient=1;
 Usum = 1;
 En = static_cast<double>(0.1);
 Emax = 1;
 Esum = 1;
 Econst = 1;
 Ecr =static_cast<double>(500);//0.5
 WearOutcr = 10000;
 EnergyWearOutCritical = 5000;
 Ee0 =static_cast<double>(10);// 0.1
 Eh0 =static_cast<double>(180);// 0.1
 Es =1;
 Khp0 = static_cast<double>(0.1e-3);//0.1;
 Khp1 = Threshold/1000;//static_cast<double>(0.001);//0.2;
 EyConst=static_cast<double>(10000);//1;
 Qsummax=2;

 CriticalEnergy=static_cast<double>(0.5);

 Kq=1;
 Pdmax=0.001;//10;//2;
// NumInputs=2;

 Kw=log(float(2))/MaxPotentialGradient;

 Kq=-(1/Emax)*log(static_cast<double>(0.1));

 Kdp=1.0/1000;//1;
 Qd=1;//0.5;//3;
// Kdp = -(1/Qsummax)*log(0.1);
// Qd = -1.0/Kdp*log(Pdmax-1);

 EyBonusPos=1;
 EyBonusNeg=0.1;
 EnergyComprehensibility=1000;

 return true;
}
// ����� �������� �����.
bool NNeuronLife::AReset(void)
{
 // ����� ��������� ����������
// NeuralPotential=0;
// PrePotential=0;
// PulseCounter=0;
// UsumProizv=0;
 Usum=0;
 EsumProizv=0;

 Energy=EsumOld=static_cast<double>(100);
 Feel=0;

 Qsum=0;

 ThresholdWearOut=1;
 ThresholdFeel=1;
// ThresholdLife=(ThresholdWearOut.v+ThresholdFeel.v+1)*Threshold;
 ThresholdLife=(ThresholdWearOut.v+ThresholdFeel.v+Threshold.v);
 WearOut=static_cast<double>(0.1);

 dEa=0;
 dEy=0;
 dEe=0;
 dEh=0;
 dE=0;

 EnergyBonus=0;

 return true;
}

bool NNeuronLife::ACalcWearOut(void)
{
 // ��������� ������������� �������� � ������ � �������
 WearOut.v+=WearOutConstPositive/TimeStep;

 // �������� ���������� ������� �� ���� ���������
// WearOut-=WearOut*(1/(1+exp(-Kw*fabs(Usum)))-0.5)*WearOutConstNegative/TimeStep;
// WearOut-=fabs(Usum)*WearOutConstNegative/TimeStep;//(1.0/(1.0+exp(-Kw*fabs(Usum)))-0.5)*WearOutConstNegative/TimeStep;
 WearOut.v-=WearOut*fabs(Usum)*WearOutConstNegative/TimeStep;

 return true;
}
bool NNeuronLife::ACalcEnergy(void)
{
 double old_Energy=Energy;
 // ��������� ����� �������
// EnergyBonus+=fabs(Usum)*EyBonusPos/TimeStep;
// EnergyBonus-=EnergyBonus*EyBonusNeg/TimeStep;
 EnergyBonus.v+=fabs(Usum)*EyBonusPos;
 EnergyBonus.v-=EnergyBonus.v*EyBonusNeg.v;

 Energy.v+=Ea;

 dEy=fabs(Usum)*EyConst/TimeStep-Ey;
 Ey.v+=dEy;

 dEe=Ee0*exp((log(2.0)/Ecr)*Energy)/TimeStep-Ee;
 Ee.v+=dEe;

 dEh=Eh0*(1+exp((log(1.5)/EnergyWearOutCritical)*WearOut))/TimeStep-Eh;
 Eh.v+=dEh;

 Energy.v-=Ey.v+Ee.v+Eh.v;

 dE=(Energy-old_Energy)/TimeStep;

 return true;
}
bool NNeuronLife::ACalcFeel(void)
{
 EsumProizv = (Energy.v-EsumOld.v)*TimeStep;

 EsumOld = Energy;

 Feel=(Energy.v-En.v) + FeelDiff(Kq,EsumProizv,En);

 return true;
}
bool NNeuronLife::ACalcThresholdLife(void)
{
// ThresholdFeel = Pdmax/(1+exp(-Kdp*(CriticalEnergy-Energy)));
// ThresholdFeel = Pdmax/(1+exp(-Kdp*(Qsum.v-Qd.v)));
 ThresholdFeel = Pdmax/(1+exp(-Kdp*(Feel.v-Qd.v)));

 if (WearOut < WearOutcr)
 {
  if(WearOut.v<0.05)
   ThresholdWearOut=1e10;
  else
   ThresholdWearOut = exp(Khp0.v/WearOut.v) - exp(Khp0.v/WearOutcr.v);
 }
 else
  ThresholdWearOut = static_cast<double>(1.0-exp(Khp1*(WearOut.v - WearOutcr.v)));

// ThresholdLife = static_cast<double>((ThresholdWearOut.v + ThresholdFeel.v + 0.5)*Threshold);
 ThresholdLife = 0;//static_cast<double>(ThresholdWearOut.v + ThresholdFeel.v + Threshold);

 return true;
}

double NNeuronLife::FeelDiff(double kq, double ediff, double en)
{
 if(ediff>1e2)
//  return static_cast<double>(0.5*en);
  return static_cast<double>(en);
 if(ediff<-1e2)
  return 0;

// return static_cast<double>((1.0/(1.0+exp(-kq*ediff))-0.5)*en);
 return static_cast<double>((1.0/(1.0+exp(-kq*ediff))-0.5)*en*2.0);
}

// ��������� ������ ����� �������
bool NNeuronLife::ACalculate(void)
{
 if(!Input1.IsConnected() || !Input2.IsConnected() || !Input3.IsConnected() || Input1->GetCols()<1 || Input2->GetCols() || Input3->GetCols())
  return true;

 double potential=(*Input1)(0,0);
 double epos=(*Input2)(0,0);
 double qsum=(*Input3)(0,0);

// UsumProizv = (potential-Usum)*TimeStep;
 Usum=potential;
// UsumOld = Usum;

 Qsum=qsum;

 double ebonus=0;
 if(ExternalEnergyBonus && ExternalEnergyBonus->Output.GetCols()>0)
  ebonus=ExternalEnergyBonus->Output(0,0);

 ebonus+=EnergyBonus;

 if(epos>EnergyComprehensibility)
 {
  if(ebonus)
  {
   if(EnergyComprehensibility+ebonus<epos)
	epos=EnergyComprehensibility+ebonus;
  }
  else
   epos=EnergyComprehensibility;
 }
 dEa=epos/TimeStep-Ea;
 Ea.v+=dEa;

 if(!ACalcWearOut())
  return false;

 if(!ACalcEnergy())
  return false;

 if(!ACalcFeel())
  return false;

 if(!ACalcThresholdLife())
  return false;

 if(Energy.v < 0) // ������ 0 ������ ���� �������� ����������� ��������
 {
  UEPtr<UItem> item=dynamic_pointer_cast<UItem>(Owner);
  if(item)
  {
   item->SetActivity(false);
   //item->FillOutputData(0);
   Output1.ToZero();
   Output2.ToZero();
   Output3.ToZero();
   OutputThreshold.ToZero();
   Output5.ToZero();
   Output6.ToZero();
   Output7.ToZero();
   Output8.ToZero();
   return true;
  }
 }

 Output1(0,0)=Threshold.v;
 Output1(0,1)=ThresholdWearOut.v;
 Output1(0,2)=ThresholdFeel.v;
 Output1(0,3)=ThresholdLife.v;

 Output2(0,0)=Feel.v;

 Output3(0,0)=WearOut.v;

 OutputThreshold(0,0)=Ey.v;
 OutputThreshold(0,1)=Ee.v;
 OutputThreshold(0,2)=Eh.v;
 OutputThreshold(0,3)=Ey.v+Ee.v+Eh.v;

 Output5(0,0)=Energy.v;

 // ��������� ��������� ��� ������������ ��������
 Output6(0,0)=dEa*TimeStep;
 Output6(0,1)=-dEy;
 Output6(0,2)=-dEe*TimeStep;
 Output6(0,3)=dEh*TimeStep;
 Output6(0,4)=0;//dE*TimeStep;

 // ��������� ��������� ��� ��������� ��������
/* POutputData[6].Double[0]=dEa*TimeStep;
 POutputData[6].Double[1]=-dEy;
 POutputData[6].Double[2]=-dEe*TimeStep;
 POutputData[6].Double[3]=dEh*TimeStep;
 POutputData[6].Double[4]=0;//-dE*TimeStep;
  */
 Output7(0,0)=-dEa*TimeStep;
 Output7(0,1)=dEy;
 Output7(0,2)=dEe*TimeStep;
 Output7(0,3)=-dEh*TimeStep;
 Output7(0,4)=0;//-dE*TimeStep;

 // ��������� �������������� ����������� - �������������� �����
 Output8(0,0)=EnergyBonus.v;

 return true;
}
// --------------------------
}
#endif
