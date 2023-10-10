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

#ifndef NCONST_GENERATOR_CPP
#define NCONST_GENERATOR_CPP

#include "NConstGenerator.h"



namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NConstGenerator::NConstGenerator(void)
//: NSource(name),
 : Amplitude("Amplitude",this,&NConstGenerator::SetAmplitude)
{
 UpdateOutputFlag=false;
}

NConstGenerator::~NConstGenerator(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
// ������������� ��������� ���������
bool NConstGenerator::SetAmplitude(const double &value)
{
 if(Amplitude.v != value)
  UpdateOutputFlag=true;
 return true;
}

// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NConstGenerator* NConstGenerator::New(void)
{
 return new NConstGenerator;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NConstGenerator::ADefault(void)
{
 Amplitude=100.0;
 return NSource::ADefault();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NConstGenerator::ABuild(void)
{
 return true;
}

// ����� �������� �����.
bool NConstGenerator::AReset(void)
{
 UpdateOutputFlag=true;
 return NSource::AReset();
}

// ��������� ������ ����� �������
bool NConstGenerator::ACalculate(void)
{
 if(UpdateOutputFlag)
 {
  Output.Assign(1,1,Amplitude.v);
  UpdateOutputFlag=false;
 }
 return true;
}
// --------------------------
}
#endif
