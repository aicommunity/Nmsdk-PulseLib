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

#ifndef NMUSCLE_CPP
#define NMUSCLE_CPP

#include "NMuscle.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NMuscle::NMuscle(void)
 : MulCoeffs("MulCoeffs",this),
  Param("Param",this),
  TC("TC",this),
  Mass("Mass",this),
  Threshold("Threshold",this),
  G("G",this),
  Input("Input",this),
  LengthInput("LengthInput",this),
  Output("Output",this)
{
 for(size_t i=0;i<15;i++)
 {
  y[i]=0;
  yOld[i]=0;
 }
 for(size_t i=0;i<7;i++)
 {
  mr[i]=0;
  mrOld[7]=0;
 }
 vereteno1=0;
 vereteno2=0;
 goldgi=0;
 MaxActionPower=0;
 FirstStep=true;
}

NMuscle::~NMuscle(void)
{
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NMuscle* NMuscle::New(void)
{
 return new NMuscle;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NMuscle::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(//dynamic_cast<const NItem*>(comp) ||
	dynamic_pointer_cast<NReceptor>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NMuscle::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 return true;
}


// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NMuscle::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NMuscle::ADefault(void)
{
 vector<double> value;
 value.resize(18);
 value[0]=0.74;//0.09;//0.3;//0.45;
 value[1]=0.75;//0.25;//0.5;
 value[2]=0.5;//0.015;//0.02;
// value[3]=0.6;
// value[4]=0.004;
// value[5]=0.6;
// value[6]=0.004;

 value[3]=0.1;
 value[4]=0.6;
 value[5]=0.1;
 value[6]=0.6;

 value[7]=1;//37;
 value[8]=0.001;//30;
 value[9]=0.005;

 value[10]=0.5;

 value[11]=1;
 value[12]=0.005;
 value[13]=0.01;

 value[14]=0.1;
 value[15]=0.1;
 value[16]=22;//19.12;
 value[17]=1;

 MulCoeffs=value;

 value.resize(4);
 value[0]=0.039;
 value[1]=2.127;
 value[2]=0.153;
 value[3]=1.719;
 Param=value;

 value.resize(3);
 value[0]=0.003;//0.002;//0.003;
 value[1]=0.0055;//0.0092;//0.0004;//0.001513;
 value[2]=0.0298;//0.0327;//0.03;//0.00386;
 TC=value;


 Threshold = 0.2;//0.5;
 Mass 	   = 1;
 G 	   = 9.8;

// TimeStep=0.000001;

 Output.Assign(1,1,0.0);
 Input->Assign(1,1,0.0);
 LengthInput->Assign(1,1,0.0);

 return true;
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NMuscle::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NMuscle::AReset(void)
{
 for (int i=0; i<7; i++)
  mrOld[i] = mr[i] = 0;

 for (int i=0; i<15; i++)
  yOld[i] = y[i] = 0;

 goldgi    = 0;
 vereteno1 = 0;
 vereteno2 = 0;
 MaxActionPower = 0;
 FirstStep = true;

 return true;
}

// ��������� ������ ����� �������
bool NMuscle::ACalculate(void)
{
 for (int i=0; i<15; i++)
  yOld[i] = y[i];

 y[0]=(*Input)(0,0);
 y[8]=(*LengthInput)(0,0);


/* if(ExternalForce)
 {
  for(size_t i=0;i<ExternalForce->GetNumInputs();i++)
   for(size_t j=0;j<ExternalForce->GetInputSize(i);j++)
   {
   //if(k >= OutputSize)
   //	break;

	y[8]=ExternalForce->GetInputData(i)->Double[j];

	++k;
	break;
   }
 }
 else
  y[8]=0;
		 */
 y[1] = yOld[0] + yOld[11] - yOld[13] - yOld[6] + yOld[4];
// y[1] = yOld[0] + yOld[14] - yOld[13] - yOld[6] + yOld[4];
// y[1] = yOld[0] - yOld[6] + yOld[4];
// y[1] = yOld[0] + yOld[6] - yOld[4];
 y[2] = CompareThreshold( yOld[1] );
 y[3] = MuscularReduction( yOld[2] )*MulCoeffs[16];
 y[4] = MuscleFeedback(y[3], yOld[3], yOld[4],
					  1, MulCoeffs[3], MulCoeffs[4]);
 //AperiodicSection( y[3], yOld[3], MulCoeffs[3], MulCoeffs[4]);
 y[5] = yOld[3] * yOld[3];
 y[6] = MuscleFeedback(y[5], yOld[5], yOld[6],
					  1, MulCoeffs[5], MulCoeffs[6]);
 //AperiodicSection( y[5], yOld[5], MulCoeffs[5], MulCoeffs[6]);
 y[7] = yOld[3] + yOld[13];

 y[11]=MuscleFeedback(y[8], yOld[8], yOld[11],
					  MulCoeffs[11], MulCoeffs[12], MulCoeffs[13]);

 //y[11] = yOld[11]*(1.0-TimeStep/MulCoeffs[11])+yOld[8]*TimeStep/MulCoeffs[11];
 y[12] = yOld[8] * yOld[8]*((yOld[8]>=0)?1:-1);
 y[13] = MuscleFeedback(y[12], yOld[12], yOld[13],
					  MulCoeffs[7], MulCoeffs[8], MulCoeffs[9]);
// y[14] = AperiodicSection( y[11], yOld[11], MulCoeffs[9], MulCoeffs[10]);

/* y[11] = yOld[11]*(1.0-TimeStep/MulCoeffs[11])+yOld[8]*TimeStep/MulCoeffs[11];
 y[12] = yOld[11] * yOld[11]*((yOld[11]>=0)?1:-1);
 y[13] = (AperiodicSection( y[12], yOld[12], MulCoeffs[7], MulCoeffs[8]))*MulCoeffs[17];
 y[14] = AperiodicSection( y[11], yOld[11], MulCoeffs[9], MulCoeffs[10]);
  */

// (y[3] > MaxActionPower) ? (MaxActionPower = y[3]) : 0;
// (y[13] > MaxActionPower) ? (y[11] = yOld[11]) : 0;

// goldgi = AperiodicSection( y[7], yOld[7], MulCoeffs[11], MulCoeffs[12]);
// vereteno1 = AperiodicSection( y[13], yOld[13], MulCoeffs[13], MulCoeffs[14]);
// vereteno2 = AperiodicSection( y[14], yOld[14], MulCoeffs[15], MulCoeffs[16]);
  /*
 // ��������� �� �������������� ������
 double res;
 // �������� ����
 if(ActivePower)
 {
  res=y[3];
  ActivePower->SetOutput(&res,1);
 }

 // �����
 if(Length)
 {
  res=y[11];
  Length->SetOutput(&res,1);
 }

 // ��������
 if(Spindle1)
 {
  res=vereteno1;
  Spindle1->SetOutput(&res,1);
 }

 if(Spindle2)
 {
  res=vereteno2;
  Spindle2->SetOutput(&res,1);
 }

 // ����������� ����� �������
 if(Tendon)
 {
  res=goldgi;
  Tendon->SetOutput(&res,1);
 }
          */
 double res;
 res=y[11];
 Output(0,0)=res;

 FirstStep = false;

 return true;
}
// --------------------------


// --------------------------
// �������������� ������� ������ ���������� ������
// --------------------------
// �������������� �����
double NMuscle::AperiodicSection(double in, double inOld, double param1, double param2)
{
 if ( FirstStep == true )
   return param1*in;

/* if(in-inOld>10e6)
  return param1*in+10e6;

 if(in-inOld<-10e6)
  return param1*in-10e6;
  */
 return param1*in+param2*(in-inOld)*TimeStep;
}

// �����
double NMuscle::CompareThreshold(double in)
{
 return ((in>Threshold) ? (in-Threshold) : 0);
}

// ���������� ����������
double NMuscle::MuscularReduction(double in)
{
 for (int i=0; i<7; i++)
  mrOld[i] = mr[i];

 mr[0] = in;
 mr[1] = mrOld[0] - MulCoeffs[2] * mrOld[6];
 mr[2] = mrOld[1] - MulCoeffs[0] * mrOld[3];
 if(mrOld[2] || mrOld[3])
  mr[3] = 1.0 / ((TC[0] * mrOld[2] + mrOld[3])*TimeStep);
 mr[4] = mrOld[3] - MulCoeffs[1] * mrOld[5];
 if(mrOld[4] || mrOld[5])
  mr[5] = 1.0 / ((TC[1] * mrOld[4] + mrOld[5])*TimeStep);
 if(mrOld[5] || mrOld[6])
  mr[6] = 1.0 / ((TC[2] * mrOld[5] + mrOld[6])*TimeStep);

 return mr[6];
}


// ����������� ������� �� �����
double NMuscle::MuscleFeedback(double x_new, double x_old, double y_old,
					  double k, double T1, double T2)
{
 double res=0;

 res=(1.0-1.0/(T2*TimeStep))*y_old+(k/TimeStep-T1)*x_old/T2+T1*x_new/T2;

 return res;
}

// --------------------------
}
//---------------------------------------------------------------------------

#endif
