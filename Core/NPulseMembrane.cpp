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

// ������
// --------------------------
// ������������ � �����������
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
// ������ ���������� ���������� ����������
// --------------------------
// ������ ��������� �������������
size_t NPulseMembrane::GetNumPosChannels(void) const
{
 return ExcitatoryChannels.size();
}

NPulseChannelCommon* NPulseMembrane::GetPosChannel(size_t i)
{
 // SAFETY: Check bounds and validate pointer
 if(i >= ExcitatoryChannels.size())
  return nullptr;
 
 NPulseChannelCommon* channel = ExcitatoryChannels[i];
 if(!channel)
  return nullptr;
 
 // Try to verify channel still exists in components
 try {
  // Get all channels and check if this pointer is still valid
  vector<NameT> channel_buffer;
  GetComponentsNameByClassType<NPulseChannelCommon>(channel_buffer, GetThisAsSharedContainer());
  for(size_t j=0; j<channel_buffer.size(); j++)
  {
   std::shared_ptr<NPulseChannelCommon> ch = GetComponentL<NPulseChannelCommon>(channel_buffer[j], true);
   if(ch && ch.get() == channel)
    return channel; // Pointer is still valid
  }
  // Pointer not found in components - it's been deleted
  return nullptr;
 } catch (...) {
  // If we can't verify, return nullptr to be safe
  return nullptr;
 }
}

// ������ ��������� ����������������
size_t NPulseMembrane::GetNumNegChannels(void) const
{
 return InhibitoryChannels.size();
}

NPulseChannelCommon* NPulseMembrane::GetNegChannel(size_t i)
{
 // SAFETY: Check bounds and validate pointer
 if(i >= InhibitoryChannels.size())
  return nullptr;
 
 NPulseChannelCommon* channel = InhibitoryChannels[i];
 if(!channel)
  return nullptr;
 
 // Try to verify channel still exists in components
 try {
  vector<NameT> channel_buffer;
  GetComponentsNameByClassType<NPulseChannelCommon>(channel_buffer, GetThisAsSharedContainer());
  for(size_t j=0; j<channel_buffer.size(); j++)
  {
   std::shared_ptr<NPulseChannelCommon> ch = GetComponentL<NPulseChannelCommon>(channel_buffer[j], true);
   if(ch && ch.get() == channel)
    return channel; // Pointer is still valid
  }
  return nullptr;
 } catch (...) {
  return nullptr;
 }
}

// ������������ �������
size_t NPulseMembrane::GetNumExcitatorySynapses(void) const
{
 return ExcitatorySynapses.size();
}

NPulseSynapseCommon* NPulseMembrane::GetExcitatorySynapses(size_t i)
{
 // SAFETY: Check if this pointer is valid
 // According to backtrace, this can be nullptr (0x0)
 if(!this)
 {
  LOG(WARNING) << "NPulseMembrane::GetExcitatorySynapses - this pointer is null!";
  return nullptr;
 }
 
 // SAFETY: Check bounds and validate pointer
 if(i >= ExcitatorySynapses.size())
  return nullptr;
 
 NPulseSynapseCommon* synapse = ExcitatorySynapses[i];
 if(!synapse)
  return nullptr;
 
 // Try to verify synapse still exists in components
 try {
  vector<NameT> synapse_buffer;
  GetComponentsNameByClassType<NPulseSynapseCommon>(synapse_buffer, GetThisAsSharedContainer());
  for(size_t j=0; j<synapse_buffer.size(); j++)
  {
   std::shared_ptr<NPulseSynapseCommon> syn = GetComponentL<NPulseSynapseCommon>(synapse_buffer[j], true);
   if(syn && syn.get() == synapse)
    return synapse; // Pointer is still valid
  }
  return nullptr;
 } catch (...) {
  return nullptr;
 }
}

// ��������� �������
size_t NPulseMembrane::GetNumInhibitorySynapses(void) const
{
 return InhibitorySynapses.size();
}

NPulseSynapseCommon* NPulseMembrane::GetInhibitorySynapses(size_t i)
{
 // SAFETY: Check bounds and validate pointer
 if(i >= InhibitorySynapses.size())
  return nullptr;
 
 NPulseSynapseCommon* synapse = InhibitorySynapses[i];
 if(!synapse)
  return nullptr;
 
 // Try to verify synapse still exists in components
 try {
  vector<NameT> synapse_buffer;
  GetComponentsNameByClassType<NPulseSynapseCommon>(synapse_buffer, GetThisAsSharedContainer());
  for(size_t j=0; j<synapse_buffer.size(); j++)
  {
   std::shared_ptr<NPulseSynapseCommon> syn = GetComponentL<NPulseSynapseCommon>(synapse_buffer[j], true);
   if(syn && syn.get() == synapse)
    return synapse; // Pointer is still valid
  }
  return nullptr;
 } catch (...) {
  return nullptr;
 }
}

bool NPulseMembrane::UpdateChannelData(std::shared_ptr<NPulseChannelCommon> channel, std::shared_ptr<UIPointer> pointer)
{
  vector<NPulseChannelCommon* >::iterator I;
  if(channel->Type() < 0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel.get()) == ExcitatoryChannels.end())
	ExcitatoryChannels.push_back(channel.get());
   if((I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel.get())) != InhibitoryChannels.end())
	InhibitoryChannels.erase(I);
  }
  else
  if(channel->Type() > 0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel.get()) == InhibitoryChannels.end())
	InhibitoryChannels.push_back(channel.get());
   if((I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel.get())) != ExcitatoryChannels.end())
	ExcitatoryChannels.erase(I);
  }

 else
  return false;

 return true;
}

bool NPulseMembrane::UpdateSynapseData(std::shared_ptr<NPulseSynapseCommon> synapse, std::shared_ptr<UIPointer> pointer)
{
    vector<NPulseSynapseCommon* >::iterator I;
    if(synapse->Type() < 0)
    {
     if(find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse.get()) == ExcitatorySynapses.end())
      ExcitatorySynapses.push_back(synapse.get());
     if((I=find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse.get())) != InhibitorySynapses.end())
      InhibitorySynapses.erase(I);
    }
    else
    if(synapse->Type() > 0)
    {
     if(find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse.get()) == InhibitorySynapses.end())
      InhibitorySynapses.push_back(synapse.get());
     if((I=find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse.get())) != ExcitatorySynapses.end())
      ExcitatorySynapses.erase(I);
    }

   else
    return false;

   return true;
}
// --------------------------

// --------------------------
// ������ ���������� �������������� ����������
// --------------------------
/// ������� ��������� ������
bool NPulseMembrane::SetResetAvailable(const bool &value)
{
 return true;
}

// ����������� �������� �����
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
// ��������� ������ ���������� ��������
// --------------------------
// �������� ������ ��� ����� ������ ����� ������� ����� ������
NPulseMembrane* NPulseMembrane::New(void)
{
 return new NPulseMembrane;
}

// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
bool NPulseMembrane::AAddComponent(std::shared_ptr<UContainer> comp, std::shared_ptr<UIPointer> pointer)
{
 if(!NPulseMembraneCommon::AAddComponent(comp,pointer))
  return false;

 std::shared_ptr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  if(channel->Type <0)
  {
   if(find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel.get()) == ExcitatoryChannels.end())
    ExcitatoryChannels.push_back(channel.get());
  }
  else
  if(channel->Type >0)
  {
   if(find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel.get()) == InhibitoryChannels.end())
    InhibitoryChannels.push_back(channel.get());
  }
 }

 std::shared_ptr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  if(synapse->Type < 0)
  {
   if(find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse.get()) == ExcitatorySynapses.end())
    ExcitatorySynapses.push_back(synapse.get());
  }
  else
  if(synapse->Type > 0)
  {
   if(find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse.get()) == InhibitorySynapses.end())
    InhibitorySynapses.push_back(synapse.get());
  }
 }

 return true;
}

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
bool NPulseMembrane::ADelComponent(std::shared_ptr<UContainer> comp)
{

 std::shared_ptr<NPulseChannelCommon> channel=dynamic_pointer_cast<NPulseChannelCommon>(comp);
 if(channel)
 {
  vector<NPulseChannelCommon*>::iterator I;
  I=find(ExcitatoryChannels.begin(),ExcitatoryChannels.end(),channel.get());
  if(I != ExcitatoryChannels.end())
   ExcitatoryChannels.erase(I);

  I=find(InhibitoryChannels.begin(),InhibitoryChannels.end(),channel.get());
  if(I != InhibitoryChannels.end())
   InhibitoryChannels.erase(I);
 }

 std::shared_ptr<NPulseSynapseCommon> synapse=dynamic_pointer_cast<NPulseSynapseCommon>(comp);
 if(synapse)
 {
  vector<NPulseSynapseCommon*>::iterator I;
  I=find(ExcitatorySynapses.begin(),ExcitatorySynapses.end(),synapse.get());
  if(I != ExcitatorySynapses.end())
   ExcitatorySynapses.erase(I);

  I=find(InhibitorySynapses.begin(),InhibitorySynapses.end(),synapse.get());
  if(I != InhibitorySynapses.end())
   InhibitorySynapses.erase(I);
 }

 return NPulseMembraneCommon::ADelComponent(comp);
}
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
bool NPulseMembrane::CheckComponentType(std::shared_ptr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NPulseChannelCommon>(comp) ||
    dynamic_pointer_cast<NPulseSynapseCommon>(comp))
  return true;

 return false;
}
// --------------------------

// --------------------------
// ������� ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
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

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool NPulseMembrane::ABuild(void)
{
 if(!Storage)
  return true;
 std::shared_ptr<NPulseChannelCommon> exc_channel;
 std::shared_ptr<NPulseChannelCommon> inh_channel;
 bool res=true;

 if(!ExcChannelClassName->empty())
 {
  exc_channel=AddMissingComponent<NPulseChannelCommon>("ExcChannel", ExcChannelClassName);
//  ExcitatoryChannels.resize(1);
//  ExcitatoryChannels[0]=exc_channel;
  if(exc_channel)
  {
   exc_channel->SetCoord(MVector<double,3>(5,4,0));
  }

  int old_ex_synapses=int(ExcitatorySynapses.size());

  for(int i=NumExcitatorySynapses;i<old_ex_synapses;i++)
  {
   std::shared_ptr<UContainer> syn = GetComponentL(std::string("ExcSynapse")+sntoa(i+1), true);
   if(syn)
    DelComponent(syn, true);
//    GetStorage()->ReturnObject(syn);
  }
//  ExcitatorySynapses.clear(); // ��� �� ������. �� ������� ������ ��������, � ���� ��������
							  // �� ���� �������� ��������� AddMissingComponent
  for(int i=0;i<NumExcitatorySynapses;i++)
  {
   std::shared_ptr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>(std::string("ExcSynapse")+sntoa(i+1), SynapseClassName);
   if(synapse)
   {
    synapse->Type = -1;
//   ExcitatorySynapses.push_back(synapse);
    res&=CreateLink(synapse->GetName(),"Output","ExcChannel","SynapticInputs");
    synapse->SetCoord(MVector<double,3>(5+i*6,1.7,0));
    synapse->RebuildInternalLinks();
   }
  }
 }

 if(!InhChannelClassName->empty())
 {
  inh_channel=AddMissingComponent<NPulseChannelCommon>("InhChannel", InhChannelClassName);

//  InhibitoryChannels.resize(1);
//  InhibitoryChannels[0]=inh_channel;
  if(inh_channel)
  {
   inh_channel->SetCoord(MVector<double,3>(5,8,0));
  }

  int old_in_synapses=int(InhibitorySynapses.size());
  for(int i=NumInhibitorySynapses;i<old_in_synapses;i++)
  {
   std::shared_ptr<UContainer> syn = GetComponentL(std::string("InhSynapse")+sntoa(i+1), true);
   if(syn)
    DelComponent(syn, true);
    //GetStorage()->ReturnObject(syn);
  }

//  InhibitorySynapses.clear(); // ��� �� ������. �� ������� ������ ��������, � ���� ��������
							  // �� ���� �������� ��������� AddMissingComponent
  for(int i=0;i<NumInhibitorySynapses;i++)
  {
   std::shared_ptr<NPulseSynapseCommon> synapse=AddMissingComponent<NPulseSynapseCommon>(std::string("InhSynapse")+sntoa(i+1), SynapseClassName);
   if(synapse)
   {
    synapse->Type = 1;

 //  InhibitorySynapses.push_back(synapse);
    res&=CreateLink(synapse->GetName(),"Output","InhChannel","SynapticInputs");
    synapse->SetCoord(MVector<double,3>(5+i*6,10.6,0));
    synapse->RebuildInternalLinks();
   }
  }
 }

 if(!NPulseMembraneCommon::ABuild())
  return false;
 return true;
}

// ����� �������� �����.
bool NPulseMembrane::AReset(void)
{
 if(!NPulseMembraneCommon::AReset())
  return false;
 return true;
}

// ��������� ������ ����� �������
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

  for(int j=0;j<InputFeedbackSignal->GetRows();j++)
   for(int i=0;i<InputFeedbackSignal->GetCols();i++)
    Feedback.v+=(*InputFeedbackSignal)(j,i);

 Feedback.v*=FeedbackGain.v;

 return true;
}

// ������������� ���������� � ��������� ������� �������
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
