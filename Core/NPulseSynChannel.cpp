// ===========================================================
// Version:        3.0.3
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NPULSE_SYN_CHANNEL_CPP
#define NPULSE_SYN_CHANNEL_CPP

#include "NPulseSynChannel.h"
#include "NPulseMembrane.h"
#include "NPulseNeuron.h"
#include "../../Nmsdk-NeuronLifeLib/Core/NPulseLifeNeuron.h"
#include "NReceptor.h"


namespace NMSDK {

// Методы NPulseSynChannel
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseSynChannel::NPulseSynChannel(void)
//: NPulseChannel(name),
: PulseAmplitude("PulseAmplitude",this,&NPulseSynChannel::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NPulseSynChannel::SetSecretionTC),
DissociationTC("DissociationTC",this,&NPulseSynChannel::SetDissociationTC),
InhibitionCoeff("InhibitionCoeff",this,&NPulseSynChannel::SetInhibitionCoeff),
SynapseResistance("SynapseResistance",this,&NPulseSynChannel::SetSynapseResistance),

PreOutput("PreOutput",this)
{
 VSecretionTC=1;
 VDissociationTC=1;
 OutputConstData=0;
 syn_output=0;
}

NPulseSynChannel::~NPulseSynChannel(void)
{
}
// --------------------------


// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
int NPulseSynChannel::GetNumSynapses(void) const
{
 return 0;
// return GetNumComponents();
}

// Возвращает синапс по индексу
NPulseSynapse* NPulseSynChannel::GetSynapse(size_t i)
{
// return static_cast<NPulseSynapse*>(GetComponentByIndex(i));
 return 0;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NPulseSynChannel::SetPulseAmplitude(const double &value)
{
 return true;
}

// Постоянная времени выделения медиатора
bool NPulseSynChannel::SetSecretionTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// Постоянная времени распада медиатора
bool NPulseSynChannel::SetDissociationTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// Коэффициент пресинаптического торможения
bool NPulseSynChannel::SetInhibitionCoeff(const double &value)
{
 if(SynapseResistance.v > 0)
 {
  if(value>0)
   OutputConstData=4.0*(value+1)/SynapseResistance.v;
  else
   OutputConstData=1.0/SynapseResistance.v;
 }
 else
  OutputConstData=0;

 return true;
}

// Вес (эффективность синапса) синапса
bool NPulseSynChannel::SetSynapseResistance(const double &value)
{
 if(value<=0)
  return false;

 if(InhibitionCoeff.v>0)
  OutputConstData=4.0*(InhibitionCoeff.v+1)/value;
 else
  OutputConstData=1.0/value;

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseSynChannel* NPulseSynChannel::New(void)
{
 return new NPulseSynChannel;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseSynChannel::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NPulseSynapse*>(comp))
//  return true;

 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
/*bool NPulseSynChannel::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}     */

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
/*bool NPulseSynChannel::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}                     */
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
double NPulseSynChannel::GetSynOutput()
{
    return syn_output;
}

bool NPulseSynChannel::ResetOut()
{
	POutputData[0].Double[0]=channel_input;
    return true;
}
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseSynChannel::ADefault(void)
{
 if(!NPulseChannel::ADefault())
  return false;

 // Начальные значения всем параметрам
 // Амплитуда входных импульсов
 PulseAmplitude=1;

 // Постоянная времени выделения медиатора
 SecretionTC=0.001;

 // Постоянная времени распада медиатора
 DissociationTC=0.01;

 // Коэффициент пресинаптического торможения
 InhibitionCoeff=0;

 // Вес (эффективность синапса) синапса
 SynapseResistance=1.e8;

// SetNumInputs(1);

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseSynChannel::ABuild(void)
{
 if(!NPulseChannel::ABuild())
  return false;

 VSecretionTC=SecretionTC*TimeStep;
 VDissociationTC=DissociationTC*TimeStep;
 return true;
}

// Сброс процесса счета.
bool NPulseSynChannel::AReset(void)
{
 if(!NPulseChannel::AReset())
  return false;

 // Сброс временных переменных
 if(Type>0)
 {
  Output(0,0)=1;
 }
 else
 if(Type<1)
 {
  Output(0,0)=-1;
 }
 else
 {
  Output.ToZero();
 }

 SynapseInputFlagsList.resize(ChannelInputs->size(),true);
 for(size_t n=0;n<ChannelInputs->size();n++)
 {
  UEPtr<UContainer> item=ChannelInputs.GetItem(n)->GetOwner();
  if(dynamic_pointer_cast<NPulseChannel>(item) ||
	 dynamic_pointer_cast<NReceptor>(item) ||
	 dynamic_pointer_cast<NConstGenerator>(item))
   SynapseInputFlagsList[n]=false;
 }

 return true;
}

// Выполняет расчет этого объекта
bool NPulseSynChannel::ACalculate2(void)
{
 /*double*/ channel_input=0;
 size_t num_connected_channels=0;
 double G=0;

 // Расчет синапсов
 double input=0;
 int num_connected_synapsis=0;
 /*double*/ syn_output=0;


 for(size_t n=0;n<ChannelInputs->size();n++)
 {
  size_t inpsize=ChannelInputs[n]->GetCols();
  if(inpsize == 0)
   continue;

  if(!SynapseInputFlagsList[n])
  {
	double *data=ChannelInputs[n]->Data;
	for(size_t j=0;j<inpsize;j++,++data)
	 channel_input+=*data;
	++num_connected_channels;
  }
  else // Остальные подключенные компоненты считаем входами синапсов
  {
   ++num_connected_synapsis;
   if(int(PreOutput->size())<num_connected_synapsis)
   {
	PreOutput->resize(num_connected_synapsis);
	PreOutput.v[num_connected_synapsis-1]=0;
   }

   input=(*ChannelInputs[n])(0,0);

   if(MainOwner && Owner)
   {
	if(Type() < 0)
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActivePosInputs.v;
	else
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveNegInputs.v;
   }

   if(input>0)
	PreOutput.v[num_connected_synapsis-1]+=(input/PulseAmplitude.v-PreOutput.v[num_connected_synapsis-1])/VSecretionTC;
   else
	PreOutput.v[num_connected_synapsis-1]-=PreOutput.v[num_connected_synapsis-1]/VDissociationTC;

   syn_output=OutputConstData*(1.0-InhibitionCoeff.v*PreOutput.v[num_connected_synapsis-1])*PreOutput.v[num_connected_synapsis-1];

   if(syn_output>0)
	G+=syn_output;
  }
 }
 NumConnectedSynapsis=num_connected_synapsis;

 if(UseAveragePotential && num_connected_channels>0)
  channel_input/=num_connected_channels;

 if(UseAverageSynapsis && num_connected_synapsis>0)
  G/=num_connected_synapsis;

 double feedback=static_pointer_cast<NPulseMembrane>(Owner)->Feedback;
 // Получение информации об обратной связи
 if(Owner)
  channel_input-=feedback;

 // Расчет
 double *out=&Output(0,0);
 double Ti(0.0),sum_u(0.0);

 // Проверяем необходимость сброса
// if(RestingFlag)
// {
//  if(G<RestingThreshold)
//  {
//   *out=channel_input;
//  }
// }

 if(!feedback)
 {
  double resistance(0.0);
  if((*out<channel_input && Type == 1) || (*out>channel_input && Type == -1))
   resistance=RestingResistance.v;
  else
   resistance=Resistance.v;

  Ti=Capacity/(G+1.0/resistance);
  sum_u=(1.0+G*resistance);
 }
 else
 {
  Ti=Capacity/(G+1.0/FBResistance);
  sum_u=(1.0+G*FBResistance);
 }

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------


// Методы NContinuesSynChannel
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NContinuesSynChannel::NContinuesSynChannel(void)
//: NPulseChannel(name),
: PulseAmplitude("PulseAmplitude",this,&NContinuesSynChannel::SetPulseAmplitude),
SecretionTC("SecretionTC",this,&NContinuesSynChannel::SetSecretionTC),
DissociationTC("DissociationTC",this,&NContinuesSynChannel::SetDissociationTC),
InhibitionCoeff("InhibitionCoeff",this,&NContinuesSynChannel::SetInhibitionCoeff),
SynapseResistance("SynapseResistance",this,&NContinuesSynChannel::SetSynapseResistance),

PreOutput("PreOutput",this)
{
 VSecretionTC=1;
 VDissociationTC=1;
 OutputConstData=0;
}

NContinuesSynChannel::~NContinuesSynChannel(void)
{
}
// --------------------------


// --------------------------
// Методы управления специфическими компонентами
// --------------------------
// Возвращает число синапсов
int NContinuesSynChannel::GetNumSynapses(void) const
{
 return 0;
// return GetNumComponents();
}

// Возвращает синапс по индексу
NPulseSynapse* NContinuesSynChannel::GetSynapse(size_t i)
{
// return static_cast<NPulseSynapse*>(GetComponentByIndex(i));
 return 0;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
// Устанавливает амплитуду импульсов
bool NContinuesSynChannel::SetPulseAmplitude(const double &value)
{
 return true;
}

// Постоянная времени выделения медиатора
bool NContinuesSynChannel::SetSecretionTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// Постоянная времени распада медиатора
bool NContinuesSynChannel::SetDissociationTC(const double &value)
{
 if(value <= 0)
  return false;

 Ready=false;

 return true;
}

// Коэффициент пресинаптического торможения
bool NContinuesSynChannel::SetInhibitionCoeff(const double &value)
{
 if(SynapseResistance.v > 0)
  OutputConstData=4.0*(value+1)/SynapseResistance.v;
 else
  OutputConstData=0;

 return true;
}

// Вес (эффективность синапса) синапса
bool NContinuesSynChannel::SetSynapseResistance(const double &value)
{
 if(value<=0)
  return false;

 OutputConstData=4.0*InhibitionCoeff.v/value;

 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NContinuesSynChannel* NContinuesSynChannel::New(void)
{
 return new NContinuesSynChannel;
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NContinuesSynChannel::CheckComponentType(UEPtr<UContainer> comp) const
{
// if(dynamic_cast<const NPulseSynapse*>(comp))
//  return true;

 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
/*bool NContinuesSynChannel::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 InstallHebbSynapses(comp);
 return true;
}     */

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
/*bool NContinuesSynChannel::ADelComponent(UEPtr<UContainer> comp)
{
 return true;
}                     */
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NContinuesSynChannel::ADefault(void)
{
 if(!NPulseChannel::ADefault())
  return false;

 // Начальные значения всем параметрам
 // Амплитуда входных импульсов
 PulseAmplitude=1;

 // Постоянная времени выделения медиатора
 SecretionTC=0.001;

 // Постоянная времени распада медиатора
 DissociationTC=0.01;

 // Коэффициент пресинаптического торможения
 InhibitionCoeff=1;

 // Вес (эффективность синапса) синапса
 SynapseResistance=1.0e8;

// SetNumInputs(1);

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NContinuesSynChannel::ABuild(void)
{
 if(!NPulseChannel::ABuild())
  return false;

 VSecretionTC=SecretionTC*TimeStep;
 VDissociationTC=DissociationTC*TimeStep;
 return true;
}

// Сброс процесса счета.
bool NContinuesSynChannel::AReset(void)
{
 if(!NPulseChannel::AReset())
  return false;

 // Сброс временных переменных
// PreOutput=0;
 PreOutput->assign(PreOutput->size(),0);

 FillOutputData();

 SynapseInputFlagsList.resize(ChannelInputs->size(),false);
 for(size_t n=0;n<ChannelInputs->size();n++)
 {
  UEPtr<UContainer> item=ChannelInputs.GetItem(n)->GetOwner();
  if(dynamic_pointer_cast<NPulseSynapseCommon>(item))
   SynapseInputFlagsList[n]=true;
 }

 return true;
}

// Выполняет расчет этого объекта
bool NContinuesSynChannel::ACalculate2(void)
{
 double channel_input=0;
 size_t num_connected_channels=0;
 double G=0;

 // Расчет синапсов
 double input=0;
 int num_connected_synapsis=0;
 double syn_output=0;


 for(size_t n=0;n<ChannelInputs->size();n++)
 {
  if(ChannelInputs[n]->GetCols() == 0)
   continue;
//  if(dynamic_cast<NPulseChannel*>(GetCItem(n).Item) ||
//     dynamic_cast<NReceptor*>(GetCItem(n).Item) ||
//	 dynamic_cast<NConstGenerator*>(GetCItem(n).Item))
  if(!SynapseInputFlagsList[n])
  {
   size_t inpsize=0;
   if((inpsize=ChannelInputs[n]->GetCols()) >0)
   {
	double *data=ChannelInputs[n]->Data;
	for(size_t j=0;j<inpsize;j++,++data)
	 channel_input+=*data;
	++num_connected_channels;
   }
  }
  else // Остальные подключенные компоненты считаем входами синапсов
  {
   ++num_connected_synapsis;
   if(int(PreOutput->size())<num_connected_synapsis)
   {
	PreOutput->resize(num_connected_synapsis);
    PreOutput.v[num_connected_synapsis-1]=0;
   }

   input=(*ChannelInputs[n])(0,0);

   if(MainOwner && Owner)
   {
	if(Type() < 0)
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActivePosInputs.v;
	else
	 ++static_pointer_cast<NPulseNeuron>(MainOwner)->NumActiveNegInputs.v;
   }

   if(input>0)
    PreOutput.v[num_connected_synapsis-1]+=(input/PulseAmplitude.v-PreOutput.v[num_connected_synapsis-1])/VSecretionTC;
   else
	PreOutput.v[num_connected_synapsis-1]-=PreOutput.v[num_connected_synapsis-1]/VDissociationTC;

   syn_output=PreOutput.v[num_connected_synapsis-1]/SynapseResistance.v;//OutputConstData*(1.0-InhibitionCoeff.v*PreOutput.v[num_connected_synapsis-1])*PreOutput.v[num_connected_synapsis-1];

   if(syn_output>0)
	G+=syn_output;
  }
 }
 NumConnectedSynapsis=num_connected_synapsis;

 if(UseAveragePotential && num_connected_channels>0)
  channel_input/=num_connected_channels;

 if(UseAverageSynapsis && num_connected_synapsis>0)
  G/=num_connected_synapsis;

 // Расчет
 double *out=&Output(0,0);
 double Ti(0.0),sum_u(0.0);


 // Проверяем необходимость сброса
// if(RestingFlag)
// {
//  if(G<RestingThreshold)
//  {
//   *out=channel_input;
//  }
// }

 double resistance(0.0);
 if((*out<channel_input && Type == 1) || (*out>channel_input && Type == -1))
  resistance=RestingResistance.v;
 else
  resistance=Resistance.v;

 Ti=Capacity/(G+1.0/resistance);
 sum_u=(1.0+G*resistance);

 *out+=(channel_input-(*out)*sum_u)/(Ti*TimeStep);

 return true;
}
// --------------------------
}

#endif

