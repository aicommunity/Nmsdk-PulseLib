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

#ifndef NPULSE_HEBB_SYNAPSE_CPP
#define NPULSE_HEBB_SYNAPSE_CPP

#include "NPulseHebbSynapse.h"
//#include "../../Nmsdk-NeuronLifeLib/Core/NPulseLifeNeuron.h"
#include "NPulseChannel.h"
#include "NPulseNeuron.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPulseHebbSynapse::NPulseHebbSynapse(void)
//: NPulseSynapse(name),
:
Min("Min",this),
Mout("Mout",this),
Md("Md",this),
ActiveMs("ActiveMs",this),
PassiveMs("PassiveMs",this),
Kmot("Kmot",this),
Kin("Kin",this),
Kout("Kout",this),
GdGain("GdGain",this),
GsGain("GsGain",this),

InputLTZoneFeedbackSignal("InputLTZoneFeedbackSignal",this),
InputMotivation("InputMotivation",this),
Output1("Output1",this),
Output2("Output2",this),
Output3("Output3",this),
Output4("Output4",this),
Output5("Output5",this),
Output6("Output6",this),

G("G",this),
Gd("Gd",this),
Gs("Gs",this),
GsSum("GsSum",this),
Win("Win",this),
Wout("Wout",this)

{
}

NPulseHebbSynapse::~NPulseHebbSynapse(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseHebbSynapse* NPulseHebbSynapse::New(void)
{
 return new NPulseHebbSynapse;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// ���������� ������ ����� synapse � �������������� ���� �������-���������
// ���������� false ������ ���� ��������� ������ ��������� �����
// ���� synapse == 0, �� ���������� ��� ������� �����
bool NPulseHebbSynapse::InstallHebbianConnection(void)
{
 bool res=true;
 std::shared_ptr<NPulseNeuron> mowner=std::shared_ptr<NPulseNeuron>(dynamic_pointer_cast<NPulseNeuron>(MainOwner.lock()).get());

 if(mowner && mowner->GetLTZone())
 {
//	RDK::UStringLinkSide item,conn;
//	item.Id=mowner->GetLTZone()->GetLongName(mowner);
//	item.Name="Output";
//	conn.Id=GetLongName(mowner);
//	conn.Name="InputLTZoneFeedbackSignal";
//	res&=mowner->CreateLink(item,conn);
	res&=mowner->CreateLink(mowner->GetLTZone()->GetLongName(mowner),"Output",GetLongName(mowner),"InputLTZoneFeedbackSignal");
 }
 else
  return false;
	   /*
 std::shared_ptr<NPulseLifeNeuron> mlowner=dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner);

 if(mlowner && mlowner->GetNeuronLife())
 {
	RDK::UStringLinkSide item,conn;
//	item.Id=mlowner->GetNeuronLife()->GetLongName(mlowner);
#pragma warning
	if(Type.v>0)
	 item.Name="Output7";
	else
	 item.Name="Output6";
 //	conn.Id=GetLongName(mlowner);
 //	conn.Name="InputNeuronLifeSignal";
	res&=mlowner->CreateLink(mlowner->GetNeuronLife()->GetLongName(mlowner),"",GetLongName(mlowner), "InputNeuronLifeSignal");
 }                 */
 return res;
}

// �������������� �������� �� ��������� � ����� �������� �����
bool NPulseHebbSynapse::ADefault(void)
{
 if(!NPulseSynapse::ADefault())
  return false;

// SetNumOutputs(6);
// for(int i=0;i<NumOutputs;i++)
//  SetOutputDataSize(i,MMatrixSize(1,1));

	Min=10;
	Mout=10;
	Md=0.001;
	Kmot->resize(1);
	Kin=100;
	Kout=100;

	GdGain=1;
	GsGain=10;//10;

 Kmot->resize(5);
 Kmot[0]=1;
 Kmot[1]=1;
 Kmot[2]=10000;
 Kmot[3]=100000;
 Kmot[4]=100;

 Gs->resize(Kmot->size());

 ActiveMs->resize(Kmot->size());
 ActiveMs[0]=0.001;
 ActiveMs[1]=0.001;
 ActiveMs[2]=0.001;
 ActiveMs[3]=0.001;
 ActiveMs[4]=0.001;

 PassiveMs->resize(Kmot->size());
 PassiveMs[0]=0.1;
 PassiveMs[1]=0.1;
 PassiveMs[2]=0.1;
 PassiveMs[3]=0.1;
 PassiveMs[4]=0.1;

 // ��� (������������� �������) �������
 Resistance=1.0e10;
// Resistance=1.0e8;

Output1.Assign(1,1,0.0);
Output2.Assign(1,1,0.0);
Output3.Assign(1,1,0.0);
Output4.Assign(1,1,0.0);
Output5.Assign(1,1,0.0);
Output6.Assign(1,1,0.0);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseHebbSynapse::ABuild(void)
{
 return NPulseSynapse::ABuild();
}

// ����� �������� �����.
bool NPulseHebbSynapse::AReset(void)
{
 // ����� ��������� ����������
 Win=0;
 Wout=0;
 Gd=0;
 Gs->assign(Kmot->size(),0);
 G=0;
 GsSum=0;

 if(!InputLTZoneFeedbackSignal.IsConnected())
  InstallHebbianConnection();

 return NPulseSynapse::AReset();
}

// ��������� ������ ����� �������
bool NPulseHebbSynapse::ACalculate2(void)
{
 double input=0;
 double ltzoneoutput=0;
 vector<double> motivation;


 if(!NPulseSynapse::ACalculate2())
  return false;

 if(!Input.IsConnected() || !InputLTZoneFeedbackSignal.IsConnected() || Input->GetCols()<=0 || InputLTZoneFeedbackSignal->GetCols()<=0)
  return true;

 motivation.assign(Kmot->size(),0);
 input=(*Input)(0,0);
 ltzoneoutput=(*InputLTZoneFeedbackSignal)(0,0);

 // ��������� ��������� ���� ����
 if(InputMotivation.IsConnected() && InputMotivation->GetCols()>0)
 {
  int motmin=(int(Kmot->size())<InputMotivation->GetCols())?int(Kmot->size()):InputMotivation->GetCols();
  for(int i=0;i<motmin;i++)
   motivation[i]=(*InputMotivation)(0,i)*Kmot[i];
 }

 Win.v += (Kin.v*input - Min.v*Win.v)/TimeStep;
 Wout.v += (Kout.v*ltzoneoutput - Mout.v*Wout.v)/TimeStep;
// Wout.v=1;

 Gd.v += (Win.v*Wout.v - Md.v*Gd.v)/TimeStep;

// for(size_t i=0;i<Gs->size();i++)
//  Gs[i] += (motivation[i]*Gd.v - ActiveMs[i]*Gs[i])/TimeStep;

 for(int i=0;i<int(Gs->size());i++)
  if(motivation[i]>0)
//  if(motivation[i]*Gd.v > PassiveMs[i]*Gs[i])
   Gs[i] += (motivation[i]*Gd.v - ActiveMs[i]*Gs[i])/TimeStep;
  else
   Gs[i] += (motivation[i]*Gd.v - PassiveMs[i]*Gs[i])/TimeStep;

 double gs_res=0;
 for(int i=0;i<int(Gs->size());i++)
  gs_res+=Gs[i];
 GsSum=gs_res;

 G.v = (Gd.v*GdGain + GsSum.v*GsGain);
 Output(0,0)*=(1.0+G.v);
 Output1(0,0)=Output(0,0);
 Output2(0,0)=G.v;
 Output3(0,0)=Gd.v*GdGain;
 Output4(0,0)=GsSum.v*GsGain;
 Output5(0,0)=Win.v;
 Output6(0,0)=Wout.v;
   /*
 if(MainOwner && Owner)
 {
  std::shared_ptr<NPulseLifeNeuron> neuron=dynamic_pointer_cast<NPulseLifeNeuron>(MainOwner);
  if(neuron)
  {
   if(static_pointer_cast<NPulseChannel>(Owner)->Type()>0)
   {
	neuron->SummaryNegGs.v+=gs_res*GsGain.v;
	neuron->SummaryNegGd.v+=Gd.v*GdGain.v;
	neuron->SummaryNegG.v+=G;
   }
   else
   {
	neuron->SummaryPosGs.v+=gs_res*GsGain.v;
	neuron->SummaryPosGd.v+=Gd.v*GdGain.v;
	neuron->SummaryPosG.v+=G;
   }
  }
 } */

 return true;
}
// --------------------------
}

#endif
