#ifndef NSYNAPSE_TRAINER_STDP_CPP
#define NSYNAPSE_TRAINER_STDP_CPP

#include "NSynapseTrainerStdp.h"

namespace NMSDK {

///������������ ����� STDP
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdp::NSynapseTrainerStdp(void)
: APlus("APlus",this,&NSynapseTrainerStdp::SetAPlus),
  AMinus("AMinus",this,&NSynapseTrainerStdp::SetAMinus),
  WMin("WMin",this,&NSynapseTrainerStdp::SetWMin),
  WMax("WMax",this,&NSynapseTrainerStdp::SetWMax),
  IsTrainEnable("IsTrainEnable",this),
  TPre("TPre",this),
  TPost("TPost",this),
  TPreOld("TPreOld",this),
  TPostOld("TPostOld",this),
  TDiff("TDiff",this),
  XYDiff("XYDiff",this),
  IsInputPulseActive("IsInputPulseActive",this),
  IsOutputPulseActive("IsOutputPulseActive",this)
{
}

NSynapseTrainerStdp::~NSynapseTrainerStdp(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdp* NSynapseTrainerStdp::New(void)
{
 return new NSynapseTrainerStdp;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NSynapseTrainerStdp::SetAPlus(const double &value)
{
 return true;
}

bool NSynapseTrainerStdp::SetAMinus(const double &value)
{
 return true;
}

bool NSynapseTrainerStdp::SetWMin(const double &value)
{
 return true;
}

bool NSynapseTrainerStdp::SetWMax(const double &value)
{
 return true;
}
// --------------------------
//������ � ����
bool NSynapseTrainerStdp::WriteIntoFile(double deltaT, double deltaW)
{
 ofstream fileDeltaW;
 ofstream fileDeltaT;
 fileDeltaW.open("C:\\Users\\kirik\\Desktop\\DeltaW.txt", ios::app);
 fileDeltaT.open("C:\\Users\\kirik\\Desktop\\DeltaT.txt", ios::app);
 if (!fileDeltaW || !fileDeltaT)
 {
     return true;
 }
 fileDeltaW << deltaW << endl;
 fileDeltaT << deltaT << endl;
 fileDeltaW.close();
 fileDeltaT.close();
 return true;
}


// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdp::ADefault(void)
{
 WeightOutput.Assign(1,1,1.0);
 IsTrainEnable=true;
 TPre=0.0;
 TPost=0.0;
 TPreOld=0.0;
 TPostOld=0.0;
 TDiff=0.0;
 XYDiff=0.0;
 WMin=-2.0;
 WMax=2.0;
 return true;
}


bool NSynapseTrainerStdp::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdp::AReset(void)
{
 TPre=0.0;
 TPost=0.0;
 TPreOld=0.0;
 TPostOld=0.0;
 TDiff=0.0;
 XYDiff=0.0;
 WeightOutput(0,0) = WMin + (double)rand()/RAND_MAX*(WMax-WMin);
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdp::ACalculate(void)
{

 IsInputPulseActive = false;
 IsOutputPulseActive = false;

 if(!IsTrainEnable)
  return true;

 if(!PreSynInput.IsConnected() || PreSynInput->GetCols()<=0)
 {
  WeightOutput(0,0)=1;
  return true;
 }

 if(!PostSynInput.IsConnected() || PostSynInput->GetCols()<=0)
 {
  WeightOutput(0,0)=1;
  return true;
 }

 if((*PreSynInput)(0,0)>0 && Environment->GetTime().GetDoubleTime()>TPre+(1.0/TimeStep))
 {
  IsInputPulseActive=true;
  TPreOld.v=TPre.v;
  TPre=Environment->GetTime().GetDoubleTime();
 }

 if((*PostSynInput)(0,0)>0 && Environment->GetTime().GetDoubleTime()>TPre+(1.0/TimeStep))
 {
   IsOutputPulseActive=true;
   TPostOld.v=TPost.v;
   TPost=Environment->GetTime().GetDoubleTime();
 }
 return true;
}
// --------------------------

///STDP, ��������� �� ������� ��������
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpTD::NSynapseTrainerStdpTD(void)
: TauX("TauX",this, &NSynapseTrainerStdpTD::SetTauX),
  TauY("TauY",this, &NSynapseTrainerStdpTD::SetTauY),
  TauMinus("TauMinus",this,&NSynapseTrainerStdpTD::SetTauMinus),
  TauPlus("TauPlus",this,&NSynapseTrainerStdpTD::SetTauPlus),
  XAvg("XAvg",this,&NSynapseTrainerStdpTD::SetXAvg),
  YAvg("YAvg",this,&NSynapseTrainerStdpTD::SetYAvg),
  WRange("WRange",this)
{
}

NSynapseTrainerStdpTD::~NSynapseTrainerStdpTD(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpTD* NSynapseTrainerStdpTD::New(void)
{
 return new NSynapseTrainerStdpTD;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NSynapseTrainerStdpTD::SetTauX(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpTD::SetTauY(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpTD::SetTauMinus(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpTD::SetTauPlus(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpTD::SetXAvg(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpTD::SetYAvg(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpTD::ADefault(void)
{
 NSynapseTrainerStdp::ADefault();
 XAvg=0.0;
 YAvg=0.0;
 WRange=WMax-WMin;
 return true;
}


bool NSynapseTrainerStdpTD::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpTD::AReset(void)
{
 NSynapseTrainerStdp::AReset();
 XAvg=0.0;
 YAvg=0.0;
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpTD::ACalculate(void)
{
 NSynapseTrainerStdp::ACalculate();
 return true;
}
// --------------------------

///STDP, ��������� �� ���� ��������
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpWD::NSynapseTrainerStdpWD(void)
{
}

NSynapseTrainerStdpWD::~NSynapseTrainerStdpWD(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpWD* NSynapseTrainerStdpWD::New(void)
{
 return new NSynapseTrainerStdpWD;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpWD::ADefault(void)
{
 NSynapseTrainerStdp::ADefault();
 return true;
}


bool NSynapseTrainerStdpWD::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpWD::AReset(void)
{
 NSynapseTrainerStdp::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpWD::ACalculate(void)
{
 NSynapseTrainerStdp::ACalculate();
 return true;
}
// --------------------------

/// STDP - ����������� (�� ��������, �� ������)
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpLobov::NSynapseTrainerStdpLobov(void)
: XModCoeff("XModCoeff",this, &NSynapseTrainerStdpLobov::SetXModCoeff),
  YModCoeff("YModCoeff",this, &NSynapseTrainerStdpLobov::SetYModCoeff),
  StartWeight("StartWeight",this,&NSynapseTrainerStdpLobov::SetStartWeight)
{
}

NSynapseTrainerStdpLobov::~NSynapseTrainerStdpLobov(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpLobov* NSynapseTrainerStdpLobov::New(void)
{
 return new NSynapseTrainerStdpLobov;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NSynapseTrainerStdpLobov::SetXModCoeff(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpLobov::SetYModCoeff(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpLobov::SetStartWeight(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpLobov::ADefault(void)
{
 NSynapseTrainerStdpTD::ADefault();
 XModCoeff=1;
 YModCoeff=1;
 APlus=5;
 AMinus=1;
 TauX=1e-2;
 TauY=5e-3;
 return true;
}


bool NSynapseTrainerStdpLobov::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpLobov::AReset(void)
{
 NSynapseTrainerStdpTD::AReset();
 StartWeight = WeightOutput(0,0);
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpLobov::ACalculate(void)
{
 NSynapseTrainerStdpTD::ACalculate();

 if(IsOutputPulseActive)
  XAvg.v+=(XModCoeff.v-XAvg.v)/(TauX.v*TimeStep);
 else
  XAvg.v-=XAvg.v/(TauX.v*TimeStep);

 if(IsInputPulseActive)
  YAvg.v+=(YModCoeff.v-YAvg.v)/(TauY.v*TimeStep);
 else
  YAvg.v-=YAvg.v/(TauY.v*TimeStep);

 double x_avg_res=(IsOutputPulseActive)?XAvg.v*APlus.v:0;
 double y_avg_res=(IsInputPulseActive)?YAvg.v*AMinus.v:0;;

 XYDiff.v+=(x_avg_res-y_avg_res)/TimeStep;

 double oldW=WeightOutput(0,0);

 WeightOutput(0,0)=(StartWeight-XYDiff.v);
 if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
 if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

 // ������ � ����
 if ((IsOutputPulseActive || IsInputPulseActive) && (WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
 {
  NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
 }

 return true;
}
// --------------------------

///STDP - ������������, ������� ����������
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpClassicDiscrete::NSynapseTrainerStdpClassicDiscrete(void)
: Mu("Mu",this)
{
}

NSynapseTrainerStdpClassicDiscrete::~NSynapseTrainerStdpClassicDiscrete(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpClassicDiscrete* NSynapseTrainerStdpClassicDiscrete::New(void)
{
 return new NSynapseTrainerStdpClassicDiscrete;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpClassicDiscrete::ADefault(void)
{
 NSynapseTrainerStdpTD::ADefault();
 Mu=10;
 APlus=5.0;
 AMinus=-5.0;
 TauPlus=0.01;
 TauMinus=0.02;
 return true;
}


bool NSynapseTrainerStdpClassicDiscrete::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpClassicDiscrete::AReset(void)
{
 NSynapseTrainerStdpTD::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpClassicDiscrete::ACalculate(void)
{
 NSynapseTrainerStdpTD::ACalculate();

 if((IsOutputPulseActive || IsInputPulseActive) && TPre!=0 && TPost!=0)
 {
  TDiff=TPost-TPre;

  if(TDiff>0)
  {
    XYDiff.v=APlus.v*pow((1-((WeightOutput(0,0)-WMin)/WRange)),Mu)*exp(-TDiff/TauPlus.v);
  }
  if(TDiff<0)
  {
    XYDiff.v=AMinus.v*pow(((WeightOutput(0,0)-WMin)/WRange),Mu)*exp(TDiff/TauMinus.v);
  }

  double oldW=WeightOutput(0,0);

  WeightOutput(0,0)+=XYDiff.v;
  if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
  if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

  if ((WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
  {
   NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
  }
 }
 return true;
}
// --------------------------

///STDP - ������������, ��������������
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpClassicIntegrated::NSynapseTrainerStdpClassicIntegrated(void)
: MuPlus("MuPlus",this),
  MuMinus("MuMinus",this)
{
}

NSynapseTrainerStdpClassicIntegrated::~NSynapseTrainerStdpClassicIntegrated(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpClassicIntegrated* NSynapseTrainerStdpClassicIntegrated::New(void)
{
 return new NSynapseTrainerStdpClassicIntegrated;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpClassicIntegrated::ADefault(void)
{
 NSynapseTrainerStdpTD::ADefault();
 MuPlus=0.5;
 MuMinus=-4;
 TauX=1e-2;
 TauY=5e-3;
 APlus=5.0;
 AMinus=1.0;
 return true;
}


bool NSynapseTrainerStdpClassicIntegrated::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpClassicIntegrated::AReset(void)
{
 NSynapseTrainerStdpTD::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpClassicIntegrated::ACalculate(void)
{
 NSynapseTrainerStdpTD::ACalculate();

 if(IsInputPulseActive)
  XAvg.v+=1;
 XAvg.v-=XAvg.v/(TauX.v*TimeStep);

 if(IsOutputPulseActive)
  YAvg.v+=1;
 YAvg.v-=YAvg.v/(TauY.v*TimeStep);

 double oldW=WeightOutput(0,0);

 WeightOutput(0,0) += (APlus.v*pow((1-((WeightOutput(0,0)-WMin)/WRange)),MuPlus)*XAvg.v*((IsOutputPulseActive)?1:0)-AMinus.v*pow((WeightOutput(0,0)-WMin+0.0001)/WRange,MuMinus)*YAvg.v*((IsInputPulseActive)?1:0))/TimeStep;
 if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
 if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

 if ((IsOutputPulseActive || IsInputPulseActive) && (WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
 {
  NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
 }
 return true;
}
// --------------------------

///STDP Triplet
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpTriplet::NSynapseTrainerStdpTriplet(void)
: APlus3("XModCoeff",this, &NSynapseTrainerStdpTriplet::SetAPlus3),
  AMinus3("YModCoeff",this, &NSynapseTrainerStdpTriplet::SetAMinus3)
{
}

NSynapseTrainerStdpTriplet::~NSynapseTrainerStdpTriplet(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpTriplet* NSynapseTrainerStdpTriplet::New(void)
{
 return new NSynapseTrainerStdpTriplet;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NSynapseTrainerStdpTriplet::SetAPlus3(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpTriplet::SetAMinus3(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpTriplet::ADefault(void)
{
 NSynapseTrainerStdpTD::ADefault();
 APlus=0.01;
 AMinus=0.01;
 APlus3=0.01;
 AMinus3=0.01;
 TauPlus=0.01;
 TauMinus=0.01;
 TauX=0.01;
 TauY=0.01;
 return true;
}


bool NSynapseTrainerStdpTriplet::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpTriplet::AReset(void)
{
 NSynapseTrainerStdpTD::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpTriplet::ACalculate(void)
{
 NSynapseTrainerStdpTD::ACalculate();

 if((IsInputPulseActive || IsOutputPulseActive) && TPreOld!=0 && TPostOld!=0)
 {
  double delta_t1=TPost-TPre;
  double delta_t2=TPost-TPostOld;
  double delta_t3=TPre-TPreOld;

  double oldW=WeightOutput(0,0);

  if(IsOutputPulseActive)
   WeightOutput(0,0)+=exp(-delta_t1/TauPlus)*(APlus+APlus3*exp(-delta_t2/TauY));
  if(IsInputPulseActive)
   WeightOutput(0,0)-=exp(delta_t1/TauMinus)*(AMinus+AMinus3*exp(-delta_t3/TauX));

  if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
  if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

  if ((WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
  {
   NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
  }
  }
  return true;
 }
// --------------------------

///STDP Mirror
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpMirror::NSynapseTrainerStdpMirror(void)
: APlus3("XModCoeff",this, &NSynapseTrainerStdpMirror::SetAPlus3),
  AMinus3("YModCoeff",this, &NSynapseTrainerStdpMirror::SetAMinus3),
  o1("o1",this),
  o2("o2",this),
  r1("r1",this),
  r2("r2",this)
{
}

NSynapseTrainerStdpMirror::~NSynapseTrainerStdpMirror(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpMirror* NSynapseTrainerStdpMirror::New(void)
{
 return new NSynapseTrainerStdpMirror;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NSynapseTrainerStdpMirror::SetAPlus3(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpMirror::SetAMinus3(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpMirror::ADefault(void)
{
 NSynapseTrainerStdpTD::ADefault();
 o1=0;
 o2=0;
 r1=0;
 r2=0;
 APlus=0.046;
 AMinus=0.03;
 APlus3=0.091;
 AMinus3=0.075;
 TauPlus=0.0168;
 TauMinus=0.0337;
 TauX=0.575;
 TauY=0.047;
 return true;
}


bool NSynapseTrainerStdpMirror::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpMirror::AReset(void)
{
 NSynapseTrainerStdpTD::AReset();
 o1=0;
 o2=0;
 r1=0;
 r2=0;
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpMirror::ACalculate(void)
{
 NSynapseTrainerStdpTD::ACalculate();
 o1.v-=(1/(TimeStep*TauMinus))*o1.v;
 o2.v-=(1/(TimeStep*TauY))*o2.v;
 r1.v-=(1/(TimeStep*TauPlus))*r1.v;
 r2.v-=(1/(TimeStep*TauX))*r2.v;

 double oldW=WeightOutput(0,0);

 if(IsInputPulseActive)
 {
  WeightOutput(0,0)-=o1.v*(AMinus+AMinus3*r2.v);
  r1.v+=1;
  r2.v+=1;
 }
 if(IsOutputPulseActive)
 {
  WeightOutput(0,0)-=r1.v*(APlus+APlus3*o2.v);
  o1.v+=1;
  o2.v+=1;
 }
 if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
 if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

 if ((IsOutputPulseActive || IsInputPulseActive) && (WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
 {
  NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
 }

 return true;
}
// --------------------------

///������������� STDP
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpProbabilistic::NSynapseTrainerStdpProbabilistic(void)
{
}

NSynapseTrainerStdpProbabilistic::~NSynapseTrainerStdpProbabilistic(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpProbabilistic* NSynapseTrainerStdpProbabilistic::New(void)
{
 return new NSynapseTrainerStdpProbabilistic;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpProbabilistic::ADefault(void)
{
 NSynapseTrainerStdpWD::ADefault();
 APlus=0.01;
 AMinus=0.01;
 return true;
}


bool NSynapseTrainerStdpProbabilistic::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpProbabilistic::AReset(void)
{
 NSynapseTrainerStdpWD::AReset();
 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpProbabilistic::ACalculate(void)
{
 NSynapseTrainerStdpWD::ACalculate();

 if((IsInputPulseActive || IsOutputPulseActive) && TPre>0 && TPost>0)
 {
  TDiff=TPost-TPre;

  if(abs(TDiff)>=0.1)
   return true;

  if(TDiff>0)
  {
   XYDiff.v=APlus*exp(-WeightOutput(0,0));
  }
  if(TDiff<0)
  {
   XYDiff.v=-AMinus;
  }

  double oldW=WeightOutput(0,0);

  WeightOutput(0,0)+=XYDiff.v;
  if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
  if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

  if ((WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
  {
   NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
  }
 }

 return true;
}
// --------------------------

///���������� STDP
// ������
// --------------------------
// ������������ � �����������
// --------------------------
NSynapseTrainerStdpStable::NSynapseTrainerStdpStable(void)
: TauLTP("TauLTP",this, &NSynapseTrainerStdpStable::SetTauLTP),
  TauLTD("TauLTD",this, &NSynapseTrainerStdpStable::SetTauLTD)
{
}

NSynapseTrainerStdpStable::~NSynapseTrainerStdpStable(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NSynapseTrainerStdpStable* NSynapseTrainerStdpStable::New(void)
{
 return new NSynapseTrainerStdpStable;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
bool NSynapseTrainerStdpStable::SetTauLTP(const double &value)
{
 return true;
}

bool NSynapseTrainerStdpStable::SetTauLTD(const double &value)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NSynapseTrainerStdpStable::ADefault(void)
{
 NSynapseTrainerStdpWD::ADefault();
 APlus=0.01;
 AMinus=0.01;
 TauLTP=0.02;
 TauLTD=0.0;
 return true;
}


bool NSynapseTrainerStdpStable::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NSynapseTrainerStdpStable::AReset(void)
{
 NSynapseTrainerStdpWD::AReset();

 return true;
}

// ��������� ������ ����� �������
bool NSynapseTrainerStdpStable::ACalculate(void)
{
 NSynapseTrainerStdpWD::ACalculate();

 if((IsInputPulseActive || IsOutputPulseActive) && TPre>0 && TPost>0)
 {
  double oldW=WeightOutput(0,0);

  if(TPost-TPre<TauLTP && TPost-TPre > TauLTD)
   WeightOutput(0,0)+=APlus*exp(-WeightOutput(0,0));
  if(TPost-TPre>TauLTP || TPre-TPost < TauLTD)
   WeightOutput(0,0)-=AMinus*exp(WeightOutput(0,0));

  if (WeightOutput(0,0) < WMin) WeightOutput(0,0)=WMin;
  if (WeightOutput(0,0) > WMax) WeightOutput(0,0)=WMax;

  if ((WeightOutput(0,0)-oldW)!=0 && WeightOutput(0,0) != WMin && WeightOutput(0,0) !=WMax)
  {
   NSynapseTrainerStdp::WriteIntoFile(TPost-TPre, WeightOutput(0,0)-oldW);
  }
 }

 return true;
}
// --------------------------

}
#endif



