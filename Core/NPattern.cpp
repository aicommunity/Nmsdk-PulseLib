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

#ifndef NPATTERN_CPP
#define NPATTERN_CPP

#include <cstdlib>
#include <cstdio>
#include "NPattern.h"

namespace NMSDK {

// ������
// --------------------------
// ������������ � �����������
// --------------------------
NPattern::NPattern(void)
 :
	PulseGeneratorClassName("PulseGeneratorClassName",this,&NPattern::SetPulseGeneratorClassName),
	NumGenerators("NumGenerators",this,&NPattern::SetNumGenerators),
	InputPattern("InputPattern",this,&NPattern::SetInputPattern),
	Frequency("Frequency",this,&NPattern::SetFrequency),
	PulseLength("PulseLength",this,&NPattern::SetPulseLength),
	Amplitude("Amplitude",this,&NPattern::SetAmplitude),
	Delay("Delay",this,&NPattern::SetDelay),
	FrequencyDeviation("FrequencyDeviation",this,&NPattern::SetFrequencyDeviation),
	UseTransitSignal("UseTransitSignal",this,&NPattern::SetUseTransitSignal)
{
 OldNumGenerators = 0;
 generators.clear();
}

NPattern::~NPattern(void)
{
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
/// ��� ������, ���������� ��������� ���������
bool NPattern::SetPulseGeneratorClassName(const std::string &value)
{
 Ready=false;
 return true;
}


// ������������� ������ �������� ��������
bool NPattern::SetNumGenerators(const int &value)
{
 Ready = false;

 OldNumGenerators = NumGenerators;
 InputPattern.Resize(value,1);

 return true;
}

// ������������� ������������ ������� ���������
bool NPattern::SetInputPattern(const MDMatrix<double> &value)
{
 Ready = false;

 for(int i = 0; i < int(generators.size()); i++)
 {
  generators[i]->Delay = Delay + value[i];
  generators[i]->Reset();
 }

 return true;
}

// ������������� ������� ���������
bool NPattern::SetFrequency(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->Frequency = Frequency;
    generators[i]->Reset();
 }

 return true;
}

// ������������� ������������ ��������
bool NPattern::SetPulseLength(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->PulseLength = PulseLength;
 }

 return true;
}

// ������������� ��������� ���������
bool NPattern::SetAmplitude(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->Amplitude = Amplitude;
 }

 return true;
}

// ������������� �������� ������� ����������
bool NPattern::SetDelay(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->Delay = Delay;
 }

 return true;
}

// ������������� ���������� �������
// (������ ��������, �� �������� ����������� �����������!)
bool NPattern::SetFrequencyDeviation(const double &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->FrequencyDeviation = FrequencyDeviation;
 }

 return true;
}

// ������������� ������������� �������� ������� �� �������� ���������
bool NPattern::SetUseTransitSignal(const bool &value)
{
 for(size_t i = 0; i < generators.size(); i++)
 {
	generators[i]->UseTransitSignal = UseTransitSignal;
 }

 return true;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPattern* NPattern::New(void)
{
 return new NPattern;
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------

// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool NPattern::ADefault(void)
{
 PulseGeneratorClassName="NPulseGeneratorTransit";
 NumGenerators = 1;
 InputPattern.Resize(1,1);
 Frequency = 0.5;
 PulseLength = 0.001;
 Amplitude = 1.0;
 Delay = 0.1;
 FrequencyDeviation = 0;
 UseTransitSignal = false;

 return NSource::ADefault();
}

// ����� �������� �����.
bool NPattern::AReset(void)
{
 OldNumGenerators = NumGenerators;

 for(int i = 0; i < int(generators.size()); i++)
 {
	generators[i]->Frequency = Frequency;
	generators[i]->PulseLength = PulseLength;
	generators[i]->Amplitude = Amplitude;
	generators[i]->Delay = Delay + InputPattern(i,0);
	generators[i]->FrequencyDeviation = FrequencyDeviation;
	generators[i]->UseTransitSignal = UseTransitSignal;
    generators[i]->Reset();
 }

 return NSource::AReset();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPattern::ABuild(void)
{
	// Проверяем наличие Storage перед созданием компонентов
	// Если Storage не установлен (например, при регистрации класса-образца),
	// пропускаем создание компонентов - они будут созданы позже при реальном использовании
	if(!Storage)
	{
		generators.clear();
		generators.resize(NumGenerators, nullptr);
		OldNumGenerators = 0;
		return true;
	}
	
	// ������� ������ ����������
	for(int i = NumGenerators; i < OldNumGenerators; i++)
	{
		DelComponent(std::string("Source")+sntoa(i+1));
	}
	// �������������� ���������� ���������
	generators.resize(NumGenerators);
	for(int i = 0; i < NumGenerators; i++)
	{
		generators[i] = AddMissingComponent<NPulseGeneratorTransit>(std::string("Source")+sntoa(i+1), PulseGeneratorClassName);
		if(generators[i])
		{
			generators[i]->SetCoord(MVector<double,3>(7.0,2.0+i*2,0));
		}
	}

	OldNumGenerators = 0;

	return true;
}

// ��������� ������ ����� �������
bool NPattern::ACalculate(void)
{
 return true;
}
// --------------------------


}
#endif
