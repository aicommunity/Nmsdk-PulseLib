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

#ifndef NPULSE_MEMBRANE_CPP
#define NPULSE_MEMBRANE_CPP

#include <algorithm>
#include "NPulseMembrane.h"

namespace NMSDK {

namespace {
// Default Modern Diagram layout (Dendrite1_85 exemplar for N=1).
// Synapses in a left column; channel to the right at mid-Y of that column.
constexpr double kMemSynX0     = 0.3;
constexpr double kMemBlockW    = 7.0;   // ~one diagram card width in Coord
// Was Syn→Channel ΔX≈7; add 1.5 blocks so multi-synapse links stay readable.
constexpr double kMemChannelX  = kMemSynX0 + kMemBlockW + 1.5 * kMemBlockW; // 17.8
constexpr double kMemExcRowY0  = 1.6;
constexpr double kMemInhRowY0  = 5.25;
constexpr double kMemSynPitchY = 3.5;

inline MVector<double,3> membraneSynapseCoord(double rowY0, int index)
{
 return MVector<double,3>(kMemSynX0, rowY0 + index * kMemSynPitchY, 0.0);
}

inline MVector<double,3> membraneChannelCoord(double rowY0, int numSynapses)
{
 const int n = numSynapses > 0 ? numSynapses : 1;
 return MVector<double,3>(kMemChannelX, rowY0 + (n - 1) * kMemSynPitchY * 0.5, 0.0);
}

inline double membraneInhRowY0(int numExcSynapses)
{
 const int nExc = numExcSynapses > 0 ? numExcSynapses : 0;
 return std::max(kMemInhRowY0, kMemExcRowY0 + nExc * kMemSynPitchY);
}
} // namespace

// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NPulseMembrane::NPulseMembrane(void)
 : FeedbackGain("FeedbackGain",this,&NPulseMembrane::SetFeedbackGain),
  ResetAvailable("ResetAvailable",this,&NPulseMembrane::SetResetAvailable),
  SynapseClassName("SynapseClassName",this,&NPulseMembrane::SetSynapseClassName),
  ExcChannelClassName("ExcChannelClassName",this,&NPulseMembrane::SetExcChannelClassName),
  InhChannelClassName("InhChannelClassName",this,&NPulseMembrane::SetInhChannelClassName),
  NumExcitatorySynapses("NumExcitatorySynapses",this,&NPulseMembrane::SetNumExcitatorySynapses),
  NumInhibitorySynapses("NumInhibitorySynapses",this,&NPulseMembrane::SetNumInhibitorySynapses),
  InputFeedbackSignal("InputFeedbackSignal",this)

{
}

NPulseMembrane::~NPulseMembrane(void)
{
}
// --------------------------

// --------------------------
// Методы управления временными перменными
// --------------------------
// Ионные механизмы деполяризации
// GetPosChannel возвращает каналы с именем "ExcChannel" (Type=-1, возбуждающие)
// Семантически "положительные" по эффекту на потенциал мембраны (деполяризация)
size_t NPulseMembrane::GetNumPosChannels(void) const
{
 return ExcitatoryChannels.size();
}

NPulseChannelCommon* NPulseMembrane::GetPosChannel(size_t i)
{
 return ExcitatoryChannels[i];
}

// Ионные механизмы гиперполяризации
// GetNegChannel возвращает каналы с именем "InhChannel" (Type=1, тормозные)
// Семантически "отрицательные" по эффекту на потенциал мембраны (гиперполяризация)
size_t NPulseMembrane::GetNumNegChannels(void) const
{
 return InhibitoryChannels.size();
}

NPulseChannelCommon* NPulseMembrane::GetNegChannel(size_t i)
{
 return InhibitoryChannels[i];
}

// Возбуждающие синапсы
size_t NPulseMembrane::GetNumExcitatorySynapses(void) const
{
 return ExcitatorySynapses.size();
}

NPulseSynapseCommon* NPulseMembrane::GetExcitatorySynapses(size_t i)
{
 return ExcitatorySynapses[i];
}

// Тормозные синапсы
size_t NPulseMembrane::GetNumInhibitorySynapses(void) const
{
 return InhibitorySynapses.size();
}

NPulseSynapseCommon* NPulseMembrane::GetInhibitorySynapses(size_t i)
{
 return InhibitorySynapses[i];
}

bool NPulseMembrane::UpdateChannelData(UEPtr<NPulseChannelCommon> channel, UEPtr<UIPointer> pointer)
{
  vector<NPulseChannelCommon* >::iterator I;
  if(channel->Type() < 0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel) == ExcitatoryChannels.end())
	ExcitatoryChannels.push_back(channel);
   if((I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel)) != InhibitoryChannels.end())
	InhibitoryChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel) == InhibitoryChannels.end())
	InhibitoryChannels.push_back(channel);
   if((I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel)) != ExcitatoryChannels.end())
	ExcitatoryChannels.erase(I);
  }

 else
  return false;

 return true;
}

bool NPulseMembrane::UpdateSynapseData(UEPtr<NPulseSynapseCommon> synapse, UEPtr<UIPointer> pointer)
{
    vector<NPulseSynapseCommon* >::iterator I;
    if(synapse->Type() < 0)
    {
     if(find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse) == ExcitatorySynapses.end())
      ExcitatorySynapses.push_back(synapse);
     if((I=find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse)) != InhibitorySynapses.end())
      InhibitorySynapses.erase(I);
    }
    else
    if(synapse->Type() > 0)
    {
     if(find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse) == InhibitorySynapses.end())
      InhibitorySynapses.push_back(synapse);
     if((I=find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse)) != ExcitatorySynapses.end())
      ExcitatorySynapses.erase(I);
    }

   else
    return false;

   return true;
}
// --------------------------

// --------------------------
// Методы управления общедоступными свойствами
// --------------------------
/// Наличие механизма сброса
bool NPulseMembrane::SetResetAvailable(const bool &value)
{
 return true;
}

// Коэффициент обратной связи
bool NPulseMembrane::SetFeedbackGain(const double &value)
{
 if(value < 0)
  return false;

 return true;
}

bool NPulseMembrane::SetSynapseClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetExcChannelClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetInhChannelClassName(const std::string &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetNumExcitatorySynapses(const int &value)
{
 Ready=false;
 return true;
}

bool NPulseMembrane::SetNumInhibitorySynapses(const int &value)
{
 Ready=false;
 return true;
}
// --------------------------

// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NPulseMembrane* NPulseMembrane::New(void)
{
 return new NPulseMembrane;
}

// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NPulseMembrane::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NPulseMembraneCommon::AAddComponent(comp,pointer))
  return false;

 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  if(channel->Type <0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel) == ExcitatoryChannels.end())
    ExcitatoryChannels.push_back(channel);
  }
  else
  if(channel->Type >0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel) == InhibitoryChannels.end())
    InhibitoryChannels.push_back(channel);
  }
 }

 UEPtr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  if(synapse->Type < 0)
  {
   if(find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse) == ExcitatorySynapses.end())
    ExcitatorySynapses.push_back(synapse);
  }
  else
  if(synapse->Type > 0)
  {
   if(find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse) == InhibitorySynapses.end())
    InhibitorySynapses.push_back(synapse);
  }
 }

 return true;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NPulseMembrane::ADelComponent(UEPtr<UContainer> comp)
{

 UEPtr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  vector<NPulseChannelCommon*>::iterator I;
  I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel);
  if(I != ExcitatoryChannels.end())
   ExcitatoryChannels.erase(I);

  I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel);
  if(I != InhibitoryChannels.end())
   InhibitoryChannels.erase(I);
 }

 UEPtr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  vector<NPulseSynapseCommon*>::iterator I;
  I=find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse);
  if(I != ExcitatorySynapses.end())
   ExcitatorySynapses.erase(I);

  I=find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse);
  if(I != InhibitorySynapses.end())
   InhibitorySynapses.erase(I);
 }

 return NPulseMembraneCommon::ADelComponent(comp);
}
// --------------------------

// --------------------------
// Методы доступа к компонентам
// --------------------------
// Метод проверяет на допустимость объекта данного типа
// в качестве компоненты данного объекта
// Метод возвращает 'true' в случае допустимости
// и 'false' в случае некорректного типа
bool NPulseMembrane::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseChannelCommon>(comp) ||
    dynamic_pointer_cast<NPulseSynapseCommon>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NPulseMembrane::ADefault(void)
{
 if(!NPulseMembraneCommon::ADefault())
  return false;
 FeedbackGain=2;
 ResetAvailable=true;
 SynapseClassName="NPSynapse";
 ExcChannelClassName="NPExcChannel";
 InhChannelClassName="NPInhChannel";
 NumExcitatorySynapses=1;
 NumInhibitorySynapses=1;

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NPulseMembrane::ABuild(void)
{
 if(!Storage)
  return true;
 UEPtr<NPulseChannelCommon> exc_channel;
 UEPtr<NPulseChannelCommon> inh_channel;
 bool res=true;

 const int num_exc = int(NumExcitatorySynapses);
 const int num_inh = int(NumInhibitorySynapses);

 if(!ExcChannelClassName->empty())
 {
  exc_channel=AddMissingComponent<NPulseChannelCommon>("ExcChannel", ExcChannelClassName);
  if(!exc_channel)
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
     std::string("Failed to create ExcChannel with class: ")+ExcChannelClassName.GetData());
   return false;
  }
//  ExcitatoryChannels.resize(1);
//  ExcitatoryChannels[0]=exc_channel;
  exc_channel->SetCoord(membraneChannelCoord(kMemExcRowY0, num_exc));

  int old_ex_synapses=int(ExcitatorySynapses.size());

  for(int i=NumExcitatorySynapses;i<old_ex_synapses;i++)
  {
   UEPtr<UContainer> syn = GetComponentL(std::string("ExcSynapse")+sntoa(i+1), true);
   if(syn)
    DelComponent(syn, true);
//    GetStorage()->ReturnObject(syn);
  }
//  ExcitatorySynapses.clear(); // Это НЕ ошибка. Мы удаляем только ненужные, а ниже проходим
							  // по всем синапсам используя AddMissingComponent
  for(int i=0;i<NumExcitatorySynapses;i++)
  {
   UEPtr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>(std::string("ExcSynapse")+sntoa(i+1), SynapseClassName);
   if(!synapse)
   {
    LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
      std::string("Failed to create ExcSynapse")+sntoa(i+1)+std::string(" with class: ")+SynapseClassName.GetData());
    return false;
   }
   synapse->Type = -1;
//   ExcitatorySynapses.push_back(synapse);
   res&=CreateLink(synapse->GetName(),"Output","ExcChannel","SynapticInputs");
   synapse->SetCoord(membraneSynapseCoord(kMemExcRowY0, i));
   synapse->RebuildInternalLinks();
  }
 }

 if(!InhChannelClassName->empty())
 {
  inh_channel=AddMissingComponent<NPulseChannelCommon>("InhChannel", InhChannelClassName);
  if(!inh_channel)
  {
   LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
     std::string("Failed to create InhChannel with class: ")+InhChannelClassName.GetData());
   return false;
  }

//  InhibitoryChannels.resize(1);
//  InhibitoryChannels[0]=inh_channel;
  const double inh_y0 = membraneInhRowY0(num_exc);
  inh_channel->SetCoord(membraneChannelCoord(inh_y0, num_inh));

  int old_in_synapses=int(InhibitorySynapses.size());
  for(int i=NumInhibitorySynapses;i<old_in_synapses;i++)
  {
   UEPtr<UContainer> syn = GetComponentL(std::string("InhSynapse")+sntoa(i+1), true);
   if(syn)
    DelComponent(syn, true);
    //GetStorage()->ReturnObject(syn);
  }

//  InhibitorySynapses.clear(); // Это НЕ ошибка. Мы удаляем только ненужные, а ниже проходим
							  // по всем синапсам используя AddMissingComponent
  for(int i=0;i<NumInhibitorySynapses;i++)
  {
   UEPtr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>(std::string("InhSynapse")+sntoa(i+1), SynapseClassName);
   if(!synapse)
   {
    LogMessageEx(RDK_EX_ERROR, __FUNCTION__,
      std::string("Failed to create InhSynapse")+sntoa(i+1)+std::string(" with class: ")+SynapseClassName.GetData());
    return false;
   }
   synapse->Type = 1;

 //  InhibitorySynapses.push_back(synapse);
   res&=CreateLink(synapse->GetName(),"Output","InhChannel","SynapticInputs");
   synapse->SetCoord(membraneSynapseCoord(inh_y0, i));
   synapse->RebuildInternalLinks();
  }
 }

 if(!NPulseMembraneCommon::ABuild())
  return false;
 return true;
}

// Сброс процесса счета.
bool NPulseMembrane::AReset(void)
{
 if(!NPulseMembraneCommon::AReset())
  return false;
 return true;
}

// Выполняет расчет этого объекта
bool NPulseMembrane::ACalculate2(void)
{
 if(!ExcitatoryChannels.empty() && !InhibitoryChannels.empty() && ResetAvailable)
 {
  NPulseChannel *exc_ch=dynamic_cast<NPulseChannel*>(ExcitatoryChannels[0]);
  NPulseChannel *inh_ch=dynamic_cast<NPulseChannel*>(InhibitoryChannels[0]);

  if(exc_ch && inh_ch)
  {
	if(exc_ch->GetSynOutput() > 1.0e-10)
	{
	 inh_ch->ResetOut();
	}
	else
	if(inh_ch->GetSynOutput() > 1.0e-10)
	{
	 exc_ch->ResetOut();
	}
  }
 }

 Feedback=0;

  double feedback = Feedback.GetData();
  for(int j=0;j<InputFeedbackSignal->GetRows();j++)
   for(int i=0;i<InputFeedbackSignal->GetCols();i++)
    feedback = feedback + (*InputFeedbackSignal)(j,i);

  const double feedbackGain = FeedbackGain.GetData();
  Feedback = feedback * feedbackGain;

 return true;
}

// Устанавливает компоненты в требуемый порядок расчета
void NPulseMembrane::UpdateComputationOrder(void)
{
 int position=0;
 for(size_t i=0;i<ExcitatorySynapses.size();i++)
  SetComponentPosition(ExcitatorySynapses[i]->GetName(),position++);
 for(size_t i=0;i<InhibitorySynapses.size();i++)
  SetComponentPosition(InhibitorySynapses[i]->GetName(),position++);
 for(size_t i=0;i<ExcitatoryChannels.size();i++)
  SetComponentPosition(ExcitatoryChannels[i]->GetName(),position++);
 for(size_t i=0;i<InhibitoryChannels.size();i++)
  SetComponentPosition(InhibitoryChannels[i]->GetName(),position++);
}
// --------------------------
}
#endif
