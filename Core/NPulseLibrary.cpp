/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2009.
E-mail:            alexab@ailab.ru
url:            http://ailab.ru
Version:        1.0.0

This file - part of the project: NMSDK

File License:        BSD License
Project License:    GNU GPLv3 License
See file license.txt for more information
*********************************************************** */
#ifndef NPULSE_LIBRARY_CPP
#define NPULSE_LIBRARY_CPP

#include "NPulseLibrary.h"

namespace NMSDK {   

NPulseLibrary PulseLibrary;


	  /*
bool CreateNeuronLifeObject(std::shared_ptr<NNeuronLife> &life, UStorage *storage)
{
 if(life || !storage)
  return false;

 life=static_pointer_cast<NNeuronLife>(storage->TakeObject("NEmptyNeuronLife"));
 if(!life)
  return false;

 std::shared_ptr<NSum> sum=static_pointer_cast<NSum>(storage->TakeObject("NSum"));
 if(!sum)
  return false;

 sum->SetName("ExternalEnergyBonus");

 if(life->AddComponent(sum,&life->ExternalEnergyBonus) == ForbiddenId)
  return false;

 return true;
}         */


// --------------------------
// ������������ � �����������
// --------------------------
NPulseLibrary::NPulseLibrary(void)
 : ULibrary("PulseLibrary","1.0", GetGlobalVersion())
{
}
// --------------------------

// --------------------------
// ������ ����������� �����������
// --------------------------
// ��������� ������ ClassSamples �������� ������������ �������� � �� �������.
// �� ��������� ��������������� ������� ������� � ������ ������.
void NPulseLibrary::CreateClassSamples(UStorage *storage)
{
 if(!storage)
  return;

 {
 std::shared_ptr<RDK::UContainer> cont;

 cont=std::make_shared<NNet>();
 cont->SetName("Net");
 cont->SetStorage(storage);
 cont->Default();
 UploadClass("NNet",cont);

 cont=std::make_shared<NModel>();
 cont->SetName("Model");
 cont->SetStorage(storage);
 cont->Default();
 UploadClass("NModel",cont);

 cont=std::make_shared<NNeuron>();
 cont->SetName("Neuron");
 cont->SetStorage(storage);
 cont->Default();
 UploadClass("NNeuron",cont);

 cont=std::make_shared<NSource>();
 cont->SetName("Source");
 cont->SetStorage(storage);
 cont->Default();
 UploadClass("NSource",cont);

 cont=std::make_shared<NReceiver>();
 cont->SetStorage(storage);
 cont->SetName("Receiver");
 cont->Default();
 UploadClass("NReceiver",cont);


 cont=std::make_shared<NMExtrapolator>();
 cont->SetStorage(storage);
 cont->SetName("MExtrapolator");
 cont->Default();
 UploadClass("NMExtrapolator",cont);

 cont=std::make_shared<NSum>();
 cont->SetStorage(storage);
 cont->SetName("Sum");
 cont->Default();
 UploadClass("NSum",cont);



  cont=std::make_shared<NPulseGenerator>();
 cont->SetStorage(storage);
 cont->SetName("PGenerator");
 cont->Default();
 UploadClass("NPGenerator",cont);

 cont=std::make_shared<NConstGenerator>();
 cont->SetStorage(storage);
 cont->SetName("CGenerator");
 cont->Default();
 UploadClass("NCGenerator",cont);

 cont=std::make_shared<NSinusGenerator>();
 cont->SetStorage(storage);
 cont->SetName("SinusGenerator");
 cont->Default();
 UploadClass("NSinusGenerator",cont);

 cont=std::make_shared<NFileGenerator>();
 cont->SetStorage(storage);
 cont->SetName("FileGenerator");
 cont->Default();
 UploadClass("NFileGenerator",cont);

 cont=std::make_shared<NPulseGeneratorTransit>();
 cont->SetStorage(storage);
 cont->SetName("PGenerator");
 cont->Default();
 UploadClass("NPulseGeneratorTransit",cont);

 cont=std::make_shared<NPulseGeneratorMulti>();
 cont->SetStorage(storage);
 cont->SetName("PGeneratorMulti");
 cont->Default();
 UploadClass("NPulseGeneratorMulti",cont);

 cont=std::make_shared<NPattern>();
 cont->SetStorage(storage);
 cont->SetName("Pattern");
 cont->Default();
 UploadClass("NPattern",cont);

 cont=std::make_shared<NPulseGeneratorDelay>();
 cont->SetStorage(storage);
 cont->SetName("PGeneratorDelay");
 cont->Default();
 UploadClass("NPGeneratorDelay",cont);

 cont=std::make_shared<NPulseDelay>();
 cont->SetStorage(storage);
 cont->SetName("PDelay");
 cont->Default();
 UploadClass("NPDelay",cont);




 cont=std::make_shared<NPulseLTZoneThreshold>();
 cont->SetStorage(storage);
 cont->SetName("PLTZone");
 cont->Default();
 UploadClass("NPulseLTZoneThreshold",cont);

 cont=std::make_shared<NPulseLTZoneThreshold>();
 cont->SetStorage(storage);
 cont->SetName("PLTZone");
 cont->Default();
 {
  NPulseLTZoneThreshold *ltzonet=dynamic_cast<NPulseLTZoneThreshold *>(cont.get());
  ltzonet->Threshold=-0.055;
  ltzonet->ThresholdOff=-0.1;
 }
 UploadClass("NPulseLTZoneThresholdBio",cont);

 cont=std::make_shared<NPulseLTZoneThreshold>();
 cont->SetStorage(storage);
 cont->SetName("PLTZone");
 cont->Default();
 {
  NPulseLTZoneThreshold *ltzonet=dynamic_cast<NPulseLTZoneThreshold *>(cont.get());
  ltzonet->Threshold=-0.055;
  ltzonet->ThresholdOff=-0.07;
 }
 UploadClass("NPulseLTZoneThresholdBio2",cont);

 cont=std::make_shared<NPulseLTZone>();
 cont->SetStorage(storage);
 cont->SetName("PLTZone");
 cont->Default();
 UploadClass("NPLTZone",cont);

 cont=std::make_shared<NContinuesLTZone>();
 cont->SetStorage(storage);
 cont->SetName("CLTZone");
 cont->Default();
 UploadClass("NCLTZone",cont);

 cont=std::make_shared<NPulseSimpleLTZone>();
 cont->SetStorage(storage);
 cont->SetName("PLTZone");
 cont->Default();
 UploadClass("NPSimpleLTZone",cont);

 cont=std::make_shared<NContinuesSimpleLTZone>();
 cont->SetStorage(storage);
 cont->SetName("CLTZone");
 cont->Default();
 UploadClass("NCSimpleLTZone",cont);

 cont=std::make_shared<NPulseChannel>();
 cont->SetStorage(storage);
 cont->SetName("PChannel");
 cont->Default();
 UploadClass("NPChannel",cont);

 cont=std::make_shared<NPulseSynapse>();
 cont->SetStorage(storage);
 cont->SetName("PSynapse");
 cont->Default();
 UploadClass("NPSynapse",cont);

 std::shared_ptr<NPulseSynapse> syn=std::make_shared<NPulseSynapse>();
 syn->SetStorage(storage);
 syn->SetName("PSynapseBio");
 syn->Default();
 syn->Resistance=2e7*4.3;
 syn->DissociationTC=0.005;
 UploadClass("NPSynapseBio",syn);

 syn=std::make_shared<NPulseSynapse>();
 syn->SetStorage(storage);
 syn->SetName("PSynapseBio2");
 syn->Default();
 syn->Resistance=86000000;
 syn->DissociationTC=0.005;
 UploadClass("NPSynapseBio2",syn);

 cont=std::make_shared<NPulseSynChannel>();
 cont->SetStorage(storage);
 cont->SetName("PChannel");
 cont->Default();
 UploadClass("NPSynChannel",cont);

 cont=std::make_shared<NContinuesSynChannel>();
 cont->SetStorage(storage);
 cont->SetName("CChannel");
 cont->Default();
 UploadClass("NCSynChannel",cont);

 cont=std::make_shared<NPulseHebbSynapse>();
 cont->SetStorage(storage);
 cont->SetName("Synapse");
 cont->Default();
 UploadClass("NPHebbSynapse",cont);

 std::shared_ptr<NPulseChannel> ch, ch_pos, ch_neg;
 // ������ ������� ��������� ������� �������� ��������
 // REFACTORED: Use make_shared instead of TakeObject+PopObject to simplify lifecycle
 // Object is not added to ObjectsStorage, so no need to call PopObject
 ch_pos=std::make_shared<NPulseChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->Default();
 UploadClass("NPExcChannel",ch_pos);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_neg=std::make_shared<NPulseChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->Default();
 UploadClass("NPInhChannel",ch_neg);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_pos=std::make_shared<NPulseChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->FBResistance=1e7;
 ch_pos->Default();
 UploadClass("NPExcChannelBio",ch_pos);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_pos=std::make_shared<NPulseChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->FBResistance=3000000;
 ch_pos->Resistance=16000000;
 ch_pos->RestingResistance=3000000;
 ch_pos->Capacity = 2.5e-10;
 ch_pos->Default();
 UploadClass("NPExcChannelBio2",ch_pos);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_neg=std::make_shared<NPulseChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->FBResistance=1e7;
 ch_neg->Default();
 UploadClass("NPInhChannelBio",ch_neg);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_neg=std::make_shared<NPulseChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->FBResistance=3000000;
 ch_neg->Resistance=16000000;
 ch_neg->RestingResistance=3000000;
 ch_neg->Capacity = 2.5e-10;
 ch_neg->Default();
 UploadClass("NPInhChannelBio2",ch_neg);


 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_pos=std::make_shared<NPulseSynChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->Default();
 UploadClass("NPSynExcChannel",ch_pos);

 ch_neg=std::make_shared<NPulseSynChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->Default();
 UploadClass("NPSynInhChannel",ch_neg);

 ch_pos=std::make_shared<NContinuesSynChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->Default();
 UploadClass("NCSynExcChannel",ch_pos);

 ch_neg=std::make_shared<NContinuesSynChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->Default();
 UploadClass("NCSynInhChannel",ch_neg);

 // ������ ������� ��������� �������������� ����
 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch=std::make_shared<NPulseChannel>();
 ch->SetStorage(storage);
 ch->Capacity=1e-8;
 ch->RestingResistance=1e6;
 ch->SetName("PChannel");
 ch->Default();
 UploadClass("NPLTChannel",ch);

 ch_pos=std::make_shared<NPulseChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->Capacity=1e-8;
 ch_pos->RestingResistance=1e6;
 ch_pos->Default();
 UploadClass("NPLTExcChannel",ch_pos);

 ch_neg=std::make_shared<NPulseChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->Capacity=1e-8;
 ch_neg->RestingResistance=1e6;
 ch_neg->Default();
 UploadClass("NPLTInhChannel",ch_neg);

 ch=std::make_shared<NPulseSynChannel>();
 ch->SetStorage(storage);
 ch->Capacity=1e-8;
 ch->RestingResistance=1e6;
 ch->SetName("PChannel");
 ch->Default();
 UploadClass("NPLTSynChannel",ch);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 ch_pos=std::make_shared<NPulseSynChannel>();
 ch_pos->SetStorage(storage);
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->Capacity=1e-8;
 ch_pos->RestingResistance=1e6;
 ch_pos->Default();
 UploadClass("NPLTSynExcChannel",ch_pos);

 ch_neg=std::make_shared<NPulseSynChannel>();
 ch_neg->SetStorage(storage);
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->Capacity=1e-8;
 ch_neg->RestingResistance=1e6;
 ch_neg->Default();
 UploadClass("NPLTSynInhChannel",ch_neg);

 // ������������ ������� ��������
 std::shared_ptr<NPulseMembrane> membr;
 cont=std::make_shared<NPulseMembrane>();
 cont->SetStorage(storage);
 cont->SetName("PMembrane");
 cont->Default();
 UploadClass("NPMembrane",cont);

 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NPExcChannelBio";
 membr->SynapseClassName="NPSynapseBio";
 membr->InhChannelClassName="NPInhChannelBio";
 membr->Default();
 UploadClass("NPMembraneBio",membr);

 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NPExcChannelBio2";
 membr->SynapseClassName="NPSynapseBio2";
 membr->InhChannelClassName="NPInhChannelBio2";
 membr->FeedbackGain = 0.02;
 membr->Default();
 UploadClass("NPMembraneBio2",membr);

 // ������� �������� �������������� ����
 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->SetName("LTMembrane");
 membr->ExcChannelClassName="NPLTExcChannel";
 membr->InhChannelClassName="NPLTInhChannel";
 membr->Default();
 UploadClass("NPLTZoneNeuronMembrane",membr);

 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->ExcChannelClassName="NPLTSynExcChannel";
 membr->InhChannelClassName="NPLTSynInhChannel";
 membr->Default();
 UploadClass("NPLTZoneSynNeuronMembrane",membr);

 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NPSynExcChannel";
 membr->InhChannelClassName="NPSynInhChannel";
 membr->Default();
 UploadClass("NPSynNeuronMembrane",membr);

 // ������� �������� ������������ �������
 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NCSynExcChannel";
 membr->InhChannelClassName="NCSynInhChannel";
 membr->Default();
 UploadClass("NCSynNeuronMembrane",membr);

 // ��������� ������� ����� � ������� �������� � ����
 // REFACTORED: Use make_shared instead of TakeObject+PopObject
 membr=std::make_shared<NPulseMembrane>();
 membr->SetStorage(storage);
 membr->SetName("PMembrane");
 membr->Default();
 membr->SynapseClassName="NPHebbSynapse";
 membr->Build();
 UploadClass("NPNeuronHebbMembrane",membr);

 cont=std::make_shared<NEyeMuscle>();
 cont->SetStorage(storage);
 cont->SetName("EyeMuscle");
 cont->Default();
 UploadClass("NEyeMuscle",cont);

 cont=std::make_shared<NMuscle>();
 cont->SetStorage(storage);
 cont->SetName("Muscle");
 cont->Default();
 UploadClass("NMuscle",cont);

 cont=std::make_shared<NPac>();
 cont->SetStorage(storage);
 cont->SetName("Pac");
 cont->Default();
 UploadClass("NPac",cont);

 cont=std::make_shared<NReceptor>();
 cont->SetStorage(storage);
 cont->SetName("Receptor");
 cont->Default();
 UploadClass("NReceptor",cont);

 cont=std::make_shared<NCPac>();
 cont->SetStorage(storage);
 cont->SetName("Pac");
 cont->Default();
 dynamic_pointer_cast<NCPac>(std::shared_ptr<RDK::UContainer>(cont))->TCMode=0;
 UploadClass("NCPac",cont);

 // ���������� ������ ���������� PulseItemsLibrary
 // REFACTORED: Using weak_ptr in factories - objects stay in ObjectsStorage, no need for PopObject
 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("PNeuronNegCGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPNeuronNegCGenerator",cont);
 }

 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("PNeuronPosCGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPNeuronPosCGenerator",cont);
 }

 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("PNeuronNegCGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NCNeuronNegCGenerator",cont);
 }

 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("PNeuronPosCGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NCNeuronPosCGenerator",cont);
 }

 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("NegGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPNeuronNegCGeneratorBio",cont);
 }

 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("PosGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPNeuronPosCGeneratorBio",cont);
 }

 cont=dynamic_pointer_cast<RDK::UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 if(cont)
 {
  cont->SetName("PNeuronPosCGenerator");
  // storage->PopObject(cont);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPNeuronPosCGeneratorCable",cont);
 }

 cont=std::make_shared<NPulseNeuron>();
 cont->SetStorage(storage);
 cont->SetName("PNeuron");
 cont->Default();
 UploadClass("NPNeuron",cont);

 cont=std::make_shared<NPulseNeuron>();
 cont->SetStorage(storage);
 cont->SetName("PHebbNeuron");
 cont->Default();
 dynamic_cast<NPulseNeuron*>(cont.get())->MembraneClassName="NPNeuronHebbMembrane";
 UploadClass("NPHebbNeuron",cont);

 cont=std::make_shared<NPulseNeuron>();
 cont->SetStorage(storage);
 cont->Default();
 dynamic_cast<NPulseNeuron*>(cont.get())->MembraneClassName="NCSynNeuronMembrane";
 dynamic_cast<NPulseNeuron*>(cont.get())->LTMembraneClassName="";
 dynamic_cast<NPulseNeuron*>(cont.get())->LTZoneClassName="NCLTZone";
 dynamic_cast<NPulseNeuron*>(cont.get())->ExcGeneratorClassName="NCNeuronNegCGenerator";
 dynamic_cast<NPulseNeuron*>(cont.get())->InhGeneratorClassName="NCNeuronPosCGenerator";
 cont->SetName("CNeuron");
 UploadClass("NCNeuron",cont);


/* cont=new NPulseLifeNeuron("PLifeNeuron");
 cont->Default();
 UploadClass("NPLifeNeuron",cont);
  */
 cont=std::make_shared<NAfferentNeuron>();
 cont->SetStorage(storage);
 cont->SetName("AfferentNeuron");
 cont->Default();
 UploadClass("NAfferentNeuron",cont);

 cont=std::make_shared<NPulseSynapseStdp>();
 cont->SetStorage(storage);
 cont->SetName("SynapseStdp");
 cont->Default();
 UploadClass("NPulseSynapseStdp",cont);


  cont=std::make_shared<NPulseChannelIzhikevich>();
 cont->SetStorage(storage);
 cont->SetName("Channel");
 cont->Default();
 UploadClass("NPulseChannelIzhikevich",cont);

 cont=std::make_shared<NPulseSynapse>();
 cont->SetStorage(storage);
 cont->SetName("Synapse");
 cont->Default();
 static_cast<NPulseSynapse*>(cont.get())->SecretionTC=0.001;
 static_cast<NPulseSynapse*>(cont.get())->DissociationTC=0.005;
 static_cast<NPulseSynapse*>(cont.get())->UsePresynapticInhibition=false;
 static_cast<NPulseSynapse*>(cont.get())->Resistance=86000000;
 UploadClass("NSynapseIaF",cont);

 cont=std::make_shared<NPulseSynapse>();
 cont->SetStorage(storage);
 cont->SetName("Synapse");
 cont->Default();
 static_cast<NPulseSynapse*>(cont.get())->SecretionTC=0.001;
 static_cast<NPulseSynapse*>(cont.get())->DissociationTC=0.005;
 static_cast<NPulseSynapse*>(cont.get())->UsePresynapticInhibition=false;
 static_cast<NPulseSynapse*>(cont.get())->Resistance=86000000;
 UploadClass("NSynapseCable",cont);

 cont=std::make_shared<NPulseSynapse>();
 cont->SetStorage(storage);
 cont->SetName("Synapse");
 cont->Default();
 static_cast<NPulseSynapse*>(cont.get())->SecretionTC=0.001;
 static_cast<NPulseSynapse*>(cont.get())->DissociationTC=0.005;
 static_cast<NPulseSynapse*>(cont.get())->UsePresynapticInhibition=false;
 static_cast<NPulseSynapse*>(cont.get())->Resistance=86000000;
 UploadClass("NSynapseCableMulti",cont);

 cont=std::make_shared<NPulseChannelIaF>();
 cont->SetStorage(storage);
 cont->SetName("Channel");
 cont->Default();
 UploadClass("NPulseChannelIaF",cont);

 cont=std::make_shared<NPulseLTZoneThreshold>();
 cont->SetStorage(storage);
 cont->SetName("PulseLTZone");
 cont->Default();
 dynamic_cast<NLTZone*>(cont.get())->Threshold=30;
 UploadClass("NPulseLTZoneIzhikevich",cont);

 cont=std::make_shared<NPulseLTZoneThreshold>();
 cont->SetStorage(storage);
 cont->SetName("PulseLTZone");
 cont->Default();
 dynamic_cast<NPulseLTZoneThreshold*>(cont.get())->Threshold=-0.055;
 dynamic_cast<NPulseLTZoneThreshold*>(cont.get())->ThresholdOff=-0.07;
 static_cast<NPulseLTZoneThreshold*>(cont.get())->NumChannelsInGroup=1;
 UploadClass("NPulseLTZoneIaF",cont);

 cont=std::make_shared<NPulseLTZoneThreshold>();
 cont->SetStorage(storage);
 cont->SetName("PulseLTZone");
 cont->Default();
 dynamic_cast<NPulseLTZoneThreshold*>(cont.get())->Threshold=-0.055;
 dynamic_cast<NPulseLTZoneThreshold*>(cont.get())->ThresholdOff=-0.07;
 static_cast<NPulseLTZoneThreshold*>(cont.get())->NumChannelsInGroup=1;
 UploadClass("NPulseLTZoneCable",cont);

 cont=std::make_shared<NPulseMembrane>();
 cont->SetStorage(storage);
 cont->Default();
 cont->SetName("PulseMembrane");
 dynamic_cast<NPulseMembrane*>(cont.get())->InhChannelClassName="";
 dynamic_cast<NPulseMembrane*>(cont.get())->SynapseClassName="NPulseSynapseStdp";
 cont->Build();
 UploadClass("NPulseMembraneIzhikevich",cont);

 cont=std::make_shared<NPulseMembrane>();
 cont->SetStorage(storage);
 cont->Default();
 cont->SetName("PulseMembrane");
 dynamic_cast<NPulseMembrane*>(cont.get())->InhChannelClassName="";
 dynamic_cast<NPulseMembrane*>(cont.get())->SynapseClassName="NSynapseIaF";
 cont->Build();
 UploadClass("NPulseMembraneIaF",cont);


 cont=std::make_shared<NPulseChannelCable>();
 cont->SetStorage(storage);
 cont->SetName("Channel");
 cont->Default();
 //dynamic_cast<NPulseChannelCable*>(cont.get())->CalcMode = true;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->Cm = 1.0e-9;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->CompartmentR = 1;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->D = 0.00002;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->EL = -0.07;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->ModelMaxLength = 0.0002;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->Ri = 100000;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->Rm = 1000;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->SynapticR = 1.0;
// dynamic_cast<NPulseChannelCable*>(cont.get())->dt = 1.0e-8;
 //dynamic_cast<NPulseChannelCable*>(cont.get())->dx = 1.0e-5;
 UploadClass("NPulseChannelCable",cont);

 cont=std::make_shared<NPulseChannelCable>();
 cont->SetStorage(storage);
 cont->SetName("Channel");
 cont->Default();
 UploadClass("NPulseChannelCableMulti",cont);

 cont=std::make_shared<NPulseMembrane>();
 cont->SetStorage(storage);
 cont->Default();
 cont->SetName("PulseMembrane");
 dynamic_cast<NPulseMembrane*>(cont.get())->InhChannelClassName="";
 dynamic_cast<NPulseMembrane*>(cont.get())->SynapseClassName="NSynapseCable";
 dynamic_cast<NPulseMembrane*>(cont.get())->FeedbackGain=0.7;
 cont->Build();
 UploadClass("NPulseMembraneCable",cont);

 cont=std::make_shared<NPulseMembrane>();
 cont->SetStorage(storage);
 cont->Default();
 cont->SetName("PulseMembrane");
 dynamic_cast<NPulseMembrane*>(cont.get())->InhChannelClassName="";
 dynamic_cast<NPulseMembrane*>(cont.get())->SynapseClassName="NSynapseCableMulti";
 dynamic_cast<NPulseMembrane*>(cont.get())->FeedbackGain=0.7;
 cont->Build();
 UploadClass("NPulseMembraneCableMulti",cont);

 cont=std::make_shared<NSynapseTrainerStdp>();
 cont->SetStorage(storage);
 cont->SetName("Trainer");
 cont->Default();
 UploadClass("NSynapseTrainerStdp",cont);

 cont=std::make_shared<NSynapseTrainerStdpTD>();
 cont->SetStorage(storage);
 cont->SetName("TrainerTD");
 cont->Default();
 UploadClass("NSynapseTrainerStdpTD",cont);

 cont=std::make_shared<NSynapseTrainerStdpWD>();
 cont->SetStorage(storage);
 cont->SetName("TrainerWD");
 cont->Default();
 UploadClass("NSynapseTrainerStdpWD",cont);

 cont=std::make_shared<NSynapseTrainerStdpLobov>();
 cont->SetStorage(storage);
 cont->SetName("TrainerLobov");
 cont->Default();
 UploadClass("NSynapseTrainerStdpLobov",cont);

 cont=std::make_shared<NSynapseTrainerStdpClassicDiscrete>();
 cont->SetStorage(storage);
 cont->SetName("TrainerClassicDiscrete");
 cont->Default();
 UploadClass("NSynapseTrainerStdpClassicDiscrete",cont);

 cont=std::make_shared<NSynapseTrainerStdpClassicIntegrated>();
 cont->SetStorage(storage);
 cont->SetName("TrainerClassicIntegrated");
 cont->Default();
 UploadClass("NSynapseTrainerStdpClassicIntegrated",cont);

 cont=std::make_shared<NSynapseTrainerStdpTriplet>();
 cont->SetStorage(storage);
 cont->SetName("TrainerTriplet");
 cont->Default();
 UploadClass("NSynapseTrainerStdpTriplet",cont);

 cont=std::make_shared<NSynapseTrainerStdpMirror>();
 cont->SetStorage(storage);
 cont->SetName("TrainerMirror");
 cont->Default();
 UploadClass("NSynapseTrainerStdpMirror",cont);

 cont=std::make_shared<NSynapseTrainerStdpProbabilistic>();
 cont->SetStorage(storage);
 cont->SetName("TrainerProbabilistic");
 cont->Default();
 UploadClass("NSynapseTrainerStdpProbabilistic",cont);

 cont=std::make_shared<NSynapseTrainerStdpStable>();
 cont->SetStorage(storage);
 cont->SetName("TrainerStable");
 cont->Default();
 UploadClass("NSynapseTrainerStdpStable",cont);


 std::shared_ptr<NPulseSynapse> sinStdp=dynamic_pointer_cast<NPulseSynapse>(storage->TakeObject("NPSynapse"));
 if(sinStdp)
 {
  sinStdp->TrainerClassName="NSynapseTrainerStdp";
  // storage->PopObject(sinStdp);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPSynapseStdp",sinStdp);
 }

 std::shared_ptr<NPulseMembrane> mem=dynamic_pointer_cast<NPulseMembrane>(storage->TakeObject("NPulseMembraneIaF"));
 if(mem)
 {
  mem->SynapseClassName="NPSynapseStdp";
  mem->Build();
  // storage->PopObject(mem);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPulseMembraneIaFStdp",mem);
 }

 cont=std::make_shared<NIntegrateAndFireNeuron>();
 cont->SetStorage(storage);
 cont->SetName("PulseIntegrateAndFireNeuron");
 cont->Default();
 UploadClass("NIntegrateAndFireNeuron",cont);

 cont=std::make_shared<NSynapseStdp>();
 cont->SetStorage(storage);
 cont->SetName("SynapseStdp");
 cont->Default();
 UploadClass("NSynapseStdp",cont);

 cont=std::make_shared<NSynapseClassic>();
 cont->SetStorage(storage);
 cont->SetName("Synapse");
 cont->Default();
 UploadClass("NSynapseClassic",cont);

 cont=std::make_shared<NSynapseClassicSlv>();
 cont->SetStorage(storage);
 cont->SetName("Synapse");
 cont->Default();
 UploadClass("NSynapseClassicSlv",cont);

 std::shared_ptr<NPulseNeuron> n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
  n->Default();
  n->MembraneClassName="NPulseMembraneIzhikevich";
  n->LTMembraneClassName="";
  n->LTZoneClassName="NPulseLTZoneIzhikevich";
  n->ExcGeneratorClassName="";
  n->InhGeneratorClassName="";
  n->NumSomaMembraneParts=1;
  n->SetName("IzhikevichNeuron");
  n->Build();
  // Remove from ObjectsStorage before using as prototype
  // Child components created via Build() remain in Storage, which is fine
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPulseNeuronIzhikevich",n);
 }

 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
  n->Default();
  n->MembraneClassName="NPulseMembraneIaF";
  n->LTMembraneClassName="";
  n->LTZoneClassName="NPulseLTZoneIaF";
  n->ExcGeneratorClassName="";
  n->InhGeneratorClassName="";
  n->NumSomaMembraneParts=1;
  n->SetName("IaFNeuron");
  n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPulseNeuronIaF",n);
 }

 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPulseNeuronIaF"));
 if(n)
 {
  n->MembraneClassName="NPulseMembraneIaFStdp";
  n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPulseNeuronIaFStdp",n);
 }

 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
  n->Default();
  n->MembraneClassName="NPulseMembraneCable";
  n->LTMembraneClassName="";
  n->LTZoneClassName="NPulseLTZoneCable";
  n->ExcGeneratorClassName="";
  n->InhGeneratorClassName="NPNeuronPosCGeneratorCable";
  n->NumSomaMembraneParts=1;
  n->SetName("CableNeuron");
  n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPulseNeuronCable",n);
 }

 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
  n->Default();
  n->MembraneClassName="NPulseMembraneCableMulti";
  n->LTMembraneClassName="";
  n->LTZoneClassName="NPulseLTZoneCable";
  n->ExcGeneratorClassName="";
  n->InhGeneratorClassName="NPNeuronPosCGeneratorCable";
  n->NumSomaMembraneParts=1;
  n->SetName("CableNeuron");
  n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NPulseNeuronCableMulti",n);
 }


/*
 cont=std::make_shared<NPulseHebbLifeSynapse>();
 cont->SetName("Synapse");
 UploadClass("NPHebbLifeSynapse",cont);

 cont=std::make_shared<NNeuronLife>();
 cont->SetName("NeuronLife");
 UploadClass("NNeuronLife",cont);

 cont=std::make_shared<NLifeNet>();
 cont->SetName("LifeNet");
 UploadClass("NLifeNet",cont);

 // ��������� ������� ����� � ������� �������� � ����
 std::shared_ptr<NPulseMembrane> membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 if(membr)
 {
   membr->SetName("PMembrane");
   membr->Default();
   membr->SynapseClassName="NPHebbLifeSynapse";
   membr->Build();
   UploadClass("NPNeuronHebbLifeMembrane",membr);
 }

 cont=std::make_shared<NPulseLifeNeuron>();
 cont->SetName("PLifeNeuron");
 dynamic_cast<NPulseNeuron*>(cont.get())->MembraneClassName="NPNeuronHebbLifeMembrane";
 UploadClass("NPLifeNeuron",cont);
*/
	/*
 std::shared_ptr<NNeuronLife> life;
 if(CreateNeuronLifeObject(life,dynamic_cast<UStorage*>(storage)))
 {
  life->Default();
  UploadClass("NNeuronLife",life);
 }           */

 }

{
 std::shared_ptr<RDK::UContainer> cont;

 // ������������ ����������� ������
 std::shared_ptr<NAfferentNeuron> an=dynamic_pointer_cast<NAfferentNeuron>(storage->TakeObject("NAfferentNeuron"));
 if(an)
 {
  an->StructureBuildMode=1;
  an->Build();
  // storage->PopObject(an);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NSAfferentNeuron",an);
 }

 // ������������ ����������� ������� � ����������� �������
 an=dynamic_pointer_cast<NAfferentNeuron>(storage->TakeObject("NAfferentNeuron"));
 if(an)
 {
  an->MembraneClassName="NCSynNeuronMembrane";
  an->LTZoneClassName="NCLTZone";
  an->ExcGeneratorClassName="NCNeuronNegCGenerator";
  an->StructureBuildMode=1;
  an->Build();
  std::shared_ptr<NReceptor> receptor=dynamic_pointer_cast<NReceptor>(an->GetComponent("Receptor"));
  if(receptor)
  {
   receptor->ExpCoeff=100;
   receptor->Gain=2;
   receptor->SumCoeff=2;
   receptor->MaxOutputRange=1;
   receptor->OutputAdaptationMode=4;
   receptor->InputAdaptationMode=0;
  }
  // storage->PopObject(an);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NContinuesSAfferentNeuron",an);
 }

 // ������� ����������� ������
 an=dynamic_pointer_cast<NAfferentNeuron>(storage->TakeObject("NAfferentNeuron"));
 if(an)
 {
  an->LTZoneClassName="NPSimpleLTZone";
  an->StructureBuildMode=2;
  an->Build();
  std::shared_ptr<NReceptor> receptor=dynamic_pointer_cast<NReceptor>(an->GetComponent("Receptor"));
  if(receptor)
  {
   receptor->ExpCoeff=10e-5;
   receptor->Gain=1;
   receptor->SumCoeff=2;
   receptor->MaxOutputRange=100;
   receptor->OutputAdaptationMode=0;
   receptor->InputAdaptationMode=0;
  }
  // storage->PopObject(an);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NSimpleAfferentNeuron",an);
 }

 // ������� ����������� ������� � ����������� �������
 an=dynamic_pointer_cast<NAfferentNeuron>(storage->TakeObject("NAfferentNeuron"));
 if(an)
 {
  an->LTZoneClassName="NCSimpleLTZone";
  an->StructureBuildMode=2;
  an->Build();
  std::shared_ptr<NReceptor> receptor=dynamic_pointer_cast<NReceptor>(an->GetComponent("Receptor"));
  if(receptor)
  {
   receptor->ExpCoeff=100;
   receptor->Gain=2;
   receptor->SumCoeff=2;
   receptor->MaxOutputRange=1;
   receptor->OutputAdaptationMode=4;
   receptor->InputAdaptationMode=0;
  }
  // storage->PopObject(an);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NContinuesSimpleAfferentNeuron",an);
 }

 // ������� ������ ������
 std::shared_ptr<NPulseNeuron> n;
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
  n->LTMembraneClassName="";
  n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
  UploadClass("NSPNeuron",n);
 }

 // ������� ������� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NLPNeuron",n);
 }

 // ������� ����������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=3;
   n->SetName("Motoneuron");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NMotoneuron",n);
 }

 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=1;
   n->SetName("RenshowCell");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NRenshowCell",n);
 }

 // ������� � ����������������� ���������
 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=1;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSynSPNeuron",n);
 }

 // ������� ������� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSynLPNeuron",n);
 }

 // ������� ����������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=3;
   n->SetName("Motoneuron");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSynMotoneuron",n);
 }

 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=1;
   n->SetName("RenshowCell");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSynRenshowCell",n);
 }

 // ������� ������ ������ � ��������� �����
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPHebbNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=1;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSPHebbNeuron",n);
 }

 // ������� ������� ������ � ��������� �����
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPHebbNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NLPHebbNeuron",n);
 }

 // ============================================================================
 // ����� �������
 // ----------------------------------------------------------------------------
 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewSPNeuron",n);
 }

 // ������� ������� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewLPNeuron",n);
 }

 // ������� ����������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->SetName("Motoneuron");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewMotoneuron",n);
 }

 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->SetName("RenshowCell");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewRenshowCell",n);
 }

 // ������� � ����������������� ���������
 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=1;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewSynSPNeuron",n);
 }

 // ������� ������� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewSynLPNeuron",n);
 }

 // ������� ����������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=3;
   n->SetName("Motoneuron");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewSynMotoneuron",n);
 }

 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->MembraneClassName="NPSynNeuronMembrane";
   n->NumSomaMembraneParts=1;
   n->SetName("RenshowCell");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewSynRenshowCell",n);
 }
 // ����� �������� � ���������������� ���������

  // ������� ������ ������ � ��������� �����
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPHebbNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewSPHebbNeuron",n);
 }

 // ������� ������� ������ � ��������� �����
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPHebbNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NNewLPHebbNeuron",n);
 }
 // ============================================================================


  // ����������� ������� � ����������������� ���������
 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NCNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NContinuesSynSPNeuron",n);
 }

 // ������� ������� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NCNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NContinuesSynLPNeuron",n);
 }

 // ������� ����������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NCNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->SetName("Motoneuron");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NContinuesSynMotoneuron",n);
 }

 // ������� ������ ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NCNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->SetName("RenshowCell");
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NContinuesSynRenshowCell",n);
 }
 // ����� ����������� �������� � ���������������� ���������

 // ������� ������� � ���������� ����������� �������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPMembraneBio";
   n->LTZoneClassName="NPulseLTZoneThreshold";
  // n->ExcGeneratorClassName="NPNeuronPosCGeneratorBio";
  // n->InhGeneratorClassName="NPNeuronNegCGeneratorBio";
   n->Build();
   n->LTZone->Threshold=0.0117;
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSPNeuronGen",n);
 }

 // ������� ������ ������ � ������������ ��������������� �����������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPMembraneBio";
   n->LTZoneClassName="NPulseLTZoneThresholdBio";
   n->ExcGeneratorClassName="NPNeuronPosCGeneratorBio";
   n->InhGeneratorClassName="NPNeuronNegCGeneratorBio";
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSPNeuronBio",n);
 }

 // ������� ������ ������ � ������������ ��������������� �����������
 // ������ 2, ����������� � ��������� �������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->MembraneClassName="NPMembraneBio2";
   n->LTZoneClassName="NPulseLTZoneThresholdBio2";
   n->ExcGeneratorClassName="NPNeuronPosCGeneratorBio";
   n->InhGeneratorClassName="NPNeuronNegCGeneratorBio";
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NSPNeuronBio2",n);
 }

 // ������� ������ ������ � ������������ ��������������� �����������
/* n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->LTZoneClassName="NPulseLTZoneThreshold";
   n->Build();
   n->PosGenerator->Amplitude=0.93;
   NPulseLTZoneThreshold *ltzonet=dynamic_cast<NPulseLTZoneThreshold *>(n->LTZone);
   ltzonet->Threshold=-0.055;
   ltzonet->ThresholdOff=-0.1;
   UploadClass("NGenBioNeuron",n);
 }
  */

 // ����� �������� � ���������� ����������� �������

 // ============================================================================
 // ������� ��� � �������
 // ============================================================================
 /*
 {
  int paper_neurons_dl[] = {4,1,1,1};
  std::vector<int> papers_dendrit_length (paper_neurons_dl, paper_neurons_dl + sizeof(paper_neurons_dl) / sizeof(int) );
  n=CreateCustomSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
  "NPNeuronPosCGenerator","NPNeuronNegCGenerator",4,1,1,papers_dendrit_length);
  n->SetName("Neuron");
  UploadClass("NPNeuron4x1",n);
 }               */

 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=4;
   n->NumDendriteMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NPNeuron4x4",n);
 }

 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPNeuron"));
 if(n)
 {
   n->LTMembraneClassName="";
   n->NumSomaMembraneParts=1;
   n->NumDendriteMembraneParts=3;
   n->Build();
  // storage->PopObject(n);  // REMOVED: Objects stay in ObjectsStorage when factories use weak_ptr
   UploadClass("NPNeuron1x4",n);
 }
 // ============================================================================
/*
 // ����� �������
 // ������� ������ ����� ������
  // ������� ������ ������ � ��������� �����
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPLifeNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->SetName("SPLifeNeuron");
   n->MembraneClassName="NPMembrane";
   n->LTMembraneClassName="";
   n->Build();
   UploadClass("NSPLifeNeuron",n);
 }


 // ������� ������� ����� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPLifeNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->SetName("LPLifeNeuron");
   n->MembraneClassName="NPMembrane";
   n->LTMembraneClassName="";
   n->Build();
   UploadClass("NLPLifeNeuron",n);
 }

 // ������� ������ ����� ������ � ��������� ����� //������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPLifeNeuron"));
 if(n)
 {
   n->SetName("SPLifeHebbNeuron");
   n->LTMembraneClassName="";
   n->Build();
   UploadClass("NSPLifeHebbNeuron",n);
 }

 // ������� ������ ����� ������ � ��������� ����� � ���������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPLifeNeuron"));
 if(n)
 {
   n->NumDendriteMembraneParts=3;
   n->LTMembraneClassName="";
   n->SetName("SPDendriteLifeHebbNeuron");
   n->Build();
   UploadClass("NSPDendriteLifeHebbNeuron",n);
 }

 // ������� ������� ����� ������ � ��������� �����
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPLifeNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=3;
   n->LTMembraneClassName="";
   n->SetName("LPLifeHebbNeuron");
   n->Build();
   UploadClass("NLPLifeHebbNeuron",n);
 }

  // ������� ������ ����� ������
 n=dynamic_pointer_cast<NPulseNeuron>(storage->TakeObject("NPLifeNeuron"));
 if(n)
 {
   n->NumSomaMembraneParts=1;
   n->SetName("SPLifeNeuron");
   n->Build();
   UploadClass("NNewSPLifeNeuron",n);
 }
*/
   // ������� ������ ����������� ��������
 cont=std::make_shared<NNeuronFreqGroup>();
 cont->SetName("NeuronFreqGroup");
 cont->Build();
 UploadClass("NNeuronFreqGroup",cont);

	// ������� ���� ����� ����������� ��������
 cont=std::make_shared<NNeuronFreqGroupLayer>();
 cont->SetName("NeuronFreqGroupLayer");
 cont->Build();
 UploadClass("NNeuronFreqGroupLayer",cont);

   // ������� ������, ��������� �������������� �������� ��� ������������� ��������� �� ��������� ������
cont=std::make_shared<NNeuronLearner>();
cont->SetName("NeuronLearner");
cont->Build();
UploadClass("NNeuronLearner",cont);

	// ������� ���� ��������
 cont=std::make_shared<NNeuronsLayer>();
 cont->SetName("NeuronsLayer");
 cont->Build();
 UploadClass("NNeuronsLayer",cont);

 std::shared_ptr<NNeuronsLayer> layer=dynamic_pointer_cast<NNeuronsLayer>(storage->TakeObject("NNeuronsLayer"));
 if(layer)
 {
   layer->NeuronsClassName="NPulseNeuronIaFStdp";
   layer->Build();
   UploadClass("NNeuronsLayerIaF",layer);
 }

	// ������� ���������� ����������
 cont=std::make_shared<NPulsePerseptron>();
 cont->SetName("PulsePerseptron");
 cont->Build();
 UploadClass("NPulsePerseptron",cont);

 std::shared_ptr<NPulsePerseptron> pers=dynamic_pointer_cast<NPulsePerseptron>(storage->TakeObject("NPulsePerseptron"));
 if(pers)
 {
   pers->NeuronsLayerClassName="NNeuronsLayerIaF";
   pers->Build();
   UploadClass("NPulsePerseptronIaF",pers);
 }

	// ������� ������ �� ����������, �������������� ��� ������������� ��������� �������� ���������
 cont=std::make_shared<NNeuronTrainer>();
 cont->SetName("NeuronTrainer");
 cont->Build();
 UploadClass("NNeuronTrainer",cont);

	// ������� ������ ��������� �������� ��� ������������� ��������� �������� ���������
 cont=std::make_shared<NSpikeClassifier>();
 cont->SetName("SpikeClassifier");
 cont->Build();
 UploadClass("NSpikeClassifier",cont);

	// ������� �������������
 cont=std::make_shared<NClassifier>();
 cont->SetName("Classifier");
 cont->Build();
 UploadClass("NClassifier",cont);

 // ������� ������ �������� ��� ������������� ��������� ��������
 cont=std::make_shared<NConditionedReflex>();
 cont->SetName("ConditionedReflex");
 cont->Build();
 UploadClass("NConditionedReflex",cont);

 // ������� ������ �������� ��� ������������� ����������� �������� ��������
cont=std::make_shared<NPainReflexSimple>();
 cont->SetName("PainReflexSimple");
 cont->Build();
 UploadClass("NPainReflexSimple",cont);


	// ������� ������ �������� ��� ������������� ������������ ������������� ������
cont=std::make_shared<NAssociationFormer>();
 cont->SetName("AssociationTrainer");
 cont->Build();
 UploadClass("NAssociationFormer",cont);

 // ������� ������ �������� ��� ������������� �������� �������� (���������� ��)
cont=std::make_shared<NLogicalNot>();
cont->SetName("LogicalNot");
cont->Build();
UploadClass("NLogicalNot",cont);

// ������� ������� ���������
cont=std::make_shared<NPredictor>();
cont->SetName("Predictor");
cont->Build();
UploadClass("NPredictor",cont);

    // ������� ����� ������ ��������
 cont=std::make_shared<NDataset>();
 cont->SetName("Dataset");
 cont->Build();
 UploadClass("NDataset",cont);

// ������� ��������� ���������
cont=std::make_shared<NStatePredictor>();
cont->SetName("StatePredictor");
cont->Build();
UploadClass("NStatePredictor",cont);

// ������� ������������� � �������������� PCA
cont=std::make_shared<NPCAClassifier>();
cont->SetName("PCAClassifier");
cont->Build();
UploadClass("NPCAClassifier",cont);

#ifdef RDK_USE_ODESOLVER
cont=std::make_shared<NOdeSolver>();
cont->SetName("OdeSolver");
cont->Build();
UploadClass("NOdeSolver",cont);

#endif

 return;

 // ������� ����� ������ ������
 /*
 std::shared_ptr<NPulseNeuron> n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,1,1);
 n->SetName("SPNeuron");
 n->DelComponent("PNeuronMembrane.PosChannel.Synapse2");
 n->DelComponent("PNeuronMembrane.PosChannel.Synapse3");
 n->DelComponent("PNeuronMembrane.NegChannel.Synapse2");
 n->DelComponent("PNeuronMembrane.NegChannel.Synapse3");
 UploadClass("NSPMinNeuron",n);
          */
 // ������� ������ ������
 /*
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("SPNeuron");
 UploadClass("NSPNeuron",n);

 // ������� ������� ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,1,1);
 n->SetName("LPNeuron");
 UploadClass("NLPNeuron",n);

 // ������� ����������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,10,10);
 n->SetName("Motoneuron");
 UploadClass("NMotoneuron",n);

 // ������� ������ ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("RenshowCell");
 UploadClass("NRenshowCell",n);

 // ������� � ����������������� ���������
 // ������� ������ ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("SPNeuron");
 UploadClass("NSynSPNeuron",n);

 // ������� ������� ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,1,1);
 n->SetName("LPNeuron");
 UploadClass("NSynLPNeuron",n);
                         */

			   /*
 // ������� ����������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,10,10);
 n->SetName("Motoneuron");
 UploadClass("NSynMotoneuron",n);

 // ������� ������ ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("RenshowCell");
 UploadClass("NSynRenshowCell",n);
 // ����� �������� � ���������������� ���������
                  */
							/*
 // ����������� ������� � ����������������� ���������
 // ������� ������ ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NCSynNeuronMembrane","NCLTZone",
 "NCNeuronPosCGenerator","NCNeuronNegCGenerator",1,5,5);
 n->SetName("SPNeuron");
 UploadClass("NContinuesSynSPNeuron",n);

 // ������� ������� ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NCSynNeuronMembrane","NCLTZone",
 "NCNeuronPosCGenerator","NCNeuronNegCGenerator",3,1,1);
 n->SetName("LPNeuron");
 UploadClass("NContinuesSynLPNeuron",n);

 // ������� ����������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NCSynNeuronMembrane","NCLTZone",
 "NCNeuronPosCGenerator","NCNeuronNegCGenerator",3,10,10);
 n->SetName("Motoneuron");
 UploadClass("NContinuesSynMotoneuron",n);

 // ������� ������ ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NCSynNeuronMembrane","NCLTZone",
 "NCNeuronPosCGenerator","NCNeuronNegCGenerator",1,5,5);
 n->SetName("RenshowCell");
 UploadClass("NContinuesSynRenshowCell",n);
 // ����� ����������� �������� � ���������������� ���������
 */
			 /*
 // ������� ������ ������ � ��������� �����
 n=CreateSimplePulseHebbNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 1,1,1);
 n->SetName("SPHebbNeuron");
 UploadClass("NSPHebbNeuron",n);

 // ������� ������� ������ � ��������� �����
 n=CreateSimplePulseHebbNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 3,1,1);
 n->SetName("LPHebbNeuron");
 UploadClass("NLPHebbNeuron",n);
				  */
				  /*
 // ������� ������ ����� ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPLifeNeuron","NPNeuronMembrane","NPLTZone","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 1,1,1);
 n->SetName("SPLifeNeuron");
 UploadClass("NSPLifeNeuron",n);


 // ������� ������� ����� ������
 n=CreateSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPLifeNeuron","NPNeuronMembrane","NPLTZone","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 3,1,1);
 n->SetName("LPLifeNeuron");
 UploadClass("NLPLifeNeuron",n);

 // ������� ������ ����� ������ � ��������� �����
 n=CreateSimplePulseHebbNeuron(dynamic_cast<UStorage*>(storage),"NPLifeNeuron","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 1,1,1);
 n->SetName("SPLifeHebbNeuron");
 UploadClass("NSPLifeHebbNeuron",n);

 // ������� ������ ����� ������ � ��������� ����� � ���������
 n=CreateSimplePulseHebbNeuron(dynamic_cast<UStorage*>(storage),"NPLifeNeuron","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 1,1,1,4);
 n->SetName("SPDendriteLifeHebbNeuron");
 UploadClass("NSPDendriteLifeHebbNeuron",n);

 // ������� ������� ����� ������ � ��������� �����
 n=CreateSimplePulseHebbNeuron(dynamic_cast<UStorage*>(storage),"NPLifeNeuron","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 3,1,1);
 n->SetName("LPLifeHebbNeuron");
 UploadClass("NLPLifeHebbNeuron",n);
			   */
 /*
 // ============================================================================
 // ����� �������
 // ----------------------------------------------------------------------------
 // ������� ������ ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewNeuronMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("SPNeuron");
 UploadClass("NNewSPNeuron",n);

 // ������� ������� ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewNeuronMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,1,1);
 n->SetName("LPNeuron");
 UploadClass("NNewLPNeuron",n);

 // ������� ����������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewNeuronMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,10,10);
 n->SetName("Motoneuron");
 UploadClass("NNewMotoneuron",n);

 // ������� ������ ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewNeuronMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("RenshowCell");
 UploadClass("NNewRenshowCell",n);
				 */
				 /*
  // ������� ������ ����� ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPLifeNeuron","NPNewNeuronMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator", 1,1,1);
 n->SetName("SPLifeNeuron");
 UploadClass("NNewSPLifeNeuron",n);      */
					   /*
 // ������� � ����������������� ���������
 // ������� ������ ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewSynNeuronMembrane","NPLTZoneSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("SPNeuron");
 UploadClass("NNewSynSPNeuron",n);

 // ������� ������� ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewSynNeuronMembrane","NPLTZoneSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,1,1);
 n->SetName("LPNeuron");
 UploadClass("NNewSynLPNeuron",n);
								 */
  //������ ������ ��� ������ TCN
 int dl[] = {4,1,1,1};
 std::vector<int> dendrit_length (dl, dl + sizeof(dl) / sizeof(int) );
 n=CreateCustomSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",4,1,1,dendrit_length);
 n->SetName("TCNNeuron");
 UploadClass("NSynTCNNeuron",n);

 //������ ������ ��� ������ TCN
 int new_dl[] = {4,1,1,1};
 std::vector<int> new_dendrit_length (new_dl, new_dl + sizeof(new_dl) / sizeof(int) );
 n=CreateCustomNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewSynNeuronMembrane","NPLTZoneSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",4,1,1,new_dendrit_length);
 n->SetName("TCNNeuron");
 UploadClass("NNewSynTCNNeuron",n);
  /*
 // ������� ����������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewSynNeuronMembrane","NPLTZoneSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,10,10);
 n->SetName("Motoneuron");
 UploadClass("NNewSynMotoneuron",n);

 // ������� ������ ������
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNewSynNeuronMembrane","NPLTZoneSynNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("RenshowCell");
 UploadClass("NNewSynRenshowCell",n);

  // ������� ������ ������ � ��������� �����
  n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronHebbMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,5,5);
 n->SetName("SPNeuron");
 UploadClass("NNewSPHebbNeuron",n);

 // ������� ������� ������ � ��������� �����
 n=CreateNewSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronHebbMembrane","NPLTZoneNeuronMembrane","NPLTZone",
 "NPNeuronPosCGenerator","NPNeuronNegCGenerator",3,1,1);
 n->SetName("LPNeuron");
 UploadClass("NNewLPHebbNeuron",n);

 // ����� �������� � ���������������� ���������
 // ============================================================================
*/
 // ============================================================================
 // ������� ��� � �������
 // ============================================================================
/*
 {
  int paper_neurons_dl[] = {4,1,1,1};
  std::vector<int> papers_dendrit_length (paper_neurons_dl, paper_neurons_dl + sizeof(paper_neurons_dl) / sizeof(int) );
  n=CreateCustomSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
  "NPNeuronPosCGenerator","NPNeuronNegCGenerator",4,1,1,papers_dendrit_length);
  n->SetName("Neuron");
  UploadClass("NPNeuron4x1",n);
 }

 {
  int paper_neurons_dl[] = {4,4,4,4};
  std::vector<int> papers_dendrit_length (paper_neurons_dl, paper_neurons_dl + sizeof(paper_neurons_dl) / sizeof(int) );
  n=CreateCustomSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
  "NPNeuronPosCGenerator","NPNeuronNegCGenerator",4,1,1,papers_dendrit_length);
  n->SetName("Neuron");
  UploadClass("NPNeuron4x4",n);
 }

 {
  int paper_neurons_dl[] = {4};
  std::vector<int> papers_dendrit_length (paper_neurons_dl, paper_neurons_dl + sizeof(paper_neurons_dl) / sizeof(int) );
  n=CreateCustomSimplePulseNeuron(dynamic_cast<UStorage*>(storage),"NPNeuron","NPNeuronMembrane","NPLTZone",
  "NPNeuronPosCGenerator","NPNeuronNegCGenerator",1,1,1,papers_dendrit_length);
  n->SetName("Neuron");
  UploadClass("NPNeuron1x4",n);
 }                                 */
 // ============================================================================
          /*
 // ������� ����������� ������
 an=CreateAfferentNeuron(dynamic_cast<UStorage*>(storage),"NPNeuronMembrane","NPLTZone","NPNeuronPosCGenerator","NPNeuronNegCGenerator",
 1);
 an->SetName("AfferentNeuron");
 UploadClass("NSAfferentNeuron",an);

 an=CreateAfferentNeuron(dynamic_cast<UStorage*>(storage),"NCSynNeuronMembrane","NCLTZone","NCNeuronPosCGenerator","NCNeuronNegCGenerator",
 1);
 an->SetName("AfferentNeuron");
 receptor=dynamic_pointer_cast<NReceptor>(an->GetComponent("Receptor"));
 if(receptor)
 {
  receptor->ExpCoeff=100;
  receptor->Gain=2;
  receptor->MaxOutputRange=1;
  receptor->OutputAdaptationMode=4;
  receptor->InputAdaptationMode=0;
 }
 UploadClass("NContinuesSAfferentNeuron",an);

 an=CreateSimpleAfferentNeuron(dynamic_cast<UStorage*>(storage),"NPSimpleLTZone",200);
 an->SetName("AfferentNeuron");
 receptor=dynamic_pointer_cast<NReceptor>(an->GetComponent("Receptor"));
 if(receptor)
 {
  receptor->ExpCoeff=10e-5;
  receptor->Gain=1;
  receptor->MaxOutputRange=100;
  receptor->OutputAdaptationMode=0;
  receptor->InputAdaptationMode=0;
 }
 UploadClass("NSimpleAfferentNeuron",an);
							 */
							 /*
 an=CreateSimpleAfferentNeuron(dynamic_cast<UStorage*>(storage),"NCSimpleLTZone",1);
 an->SetName("AfferentNeuron");
 receptor=dynamic_pointer_cast<NReceptor>(an->GetComponent("Receptor"));
 if(receptor)
 {
  receptor->ExpCoeff=100;
  receptor->Gain=2;
  receptor->MaxOutputRange=1;
  receptor->OutputAdaptationMode=4;
  receptor->InputAdaptationMode=0;
 }
 UploadClass("NContinuesSimpleAfferentNeuron",an);
                    */

}

}
// --------------------------

// --------------------------
// ������ �������� ��������� �������
// --------------------------
// ����� ������������ ������ �������� ����������� �������
std::shared_ptr<NPulseNeuron> NPulseLibrary::CreateNewSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzonemembraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, int dendrite_length)
{
 std::shared_ptr<RDK::UContainer> membr=0,ltmembr=0;
 std::shared_ptr<NPulseChannel> channel1, channel2, ltchannel1,ltchannel2;
 std::shared_ptr<NPulseNeuron> n;
 std::shared_ptr<NNet> ltzone;
 bool res;
 RDK::ULinkSide item,conn;
 vector<std::shared_ptr<NPulseHebbSynapse> > synapse_list;


 if(!storage)
  return 0;

 n=static_pointer_cast<NPulseNeuron>(storage->TakeObject(neuronclass));
 if(!n)
  return 0;


 ltzone=static_pointer_cast<NNet>(storage->TakeObject(ltzone_class));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");

 std::shared_ptr<NConstGenerator> gen_pos,gen_neg;
 gen_pos=static_pointer_cast<NConstGenerator>(storage->TakeObject(pos_gen_class));
 res=n->AddComponent(gen_pos);
 gen_neg=static_pointer_cast<NConstGenerator>(storage->TakeObject(neg_gen_class));
 n->AddComponent(gen_neg);

 ltmembr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(ltzonemembraneclass));
 ltmembr->SetName("LTMembrane");
 res=n->AddComponent(ltmembr);

 ltchannel1=static_pointer_cast<NPulseChannel>(ltmembr->GetComponent("PosChannel"));
 ltchannel2=static_pointer_cast<NPulseChannel>(ltmembr->GetComponent("NegChannel"));
 item.Index=0;
 conn.Index=-1;

 // ������������� �������� �����
 item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 conn.Id=ltmembr->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);

 // ������������� ����� ���������� � ���������
 item.Id=ltchannel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 conn.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);
 item.Id=ltchannel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);


 synapse_list.clear();
 for(int i=0;i<num_membranes;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));
  for(int j=0;j<channel1->GetNumComponents();j++)
  {
   std::shared_ptr<NPulseHebbSynapse> hebb_syn=dynamic_pointer_cast<NPulseHebbSynapse>(channel1->GetComponentByIndex(j));
   if(hebb_syn)
	synapse_list.push_back(hebb_syn);
  }

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));
  for(int j=0;j<channel2->GetNumComponents();j++)
  {
   std::shared_ptr<NPulseHebbSynapse> hebb_syn=dynamic_pointer_cast<NPulseHebbSynapse>(channel2->GetComponentByIndex(j));
   if(hebb_syn)
	synapse_list.push_back(hebb_syn);
  }
  item.Index=0;
  conn.Index=-1;

  // ������������� ����� ���������� � ���������
  item.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltchannel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
  item.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltchannel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  item.Id=gen_neg->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  item.Id=gen_pos->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
 }


 for(int i=1;i<dendrite_length;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));
  for(int j=0;j<channel1->GetNumComponents();j++)
  {
   std::shared_ptr<NPulseHebbSynapse> hebb_syn=dynamic_pointer_cast<NPulseHebbSynapse>(channel1->GetComponentByIndex(j));
   if(hebb_syn)
	synapse_list.push_back(hebb_syn);
  }

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));
  for(int j=0;j<channel2->GetNumComponents();j++)
  {
   std::shared_ptr<NPulseHebbSynapse> hebb_syn=dynamic_pointer_cast<NPulseHebbSynapse>(channel2->GetComponentByIndex(j));
   if(hebb_syn)
	synapse_list.push_back(hebb_syn);
  }
 }

 UStringLinkSide sitem, sconn;
 if(!synapse_list.empty())
 {
   // ������������� ����� ���������� � ��������� �����
  sitem.Id=ltzone->GetLongName(n);
  sitem.Name="DataOutput0";
  sconn.Name="DataInput1";
  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;
  conn.Index=1;
  for(size_t j=0;j<synapse_list.size();j++)
  {
   if(synapse_list[j])
   {
	sconn.Id=synapse_list[j]->GetLongName(n);
	conn.Id=synapse_list[j]->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
	res=n->CreateLink(item,conn);
   }
  }
 }
/*
 std::shared_ptr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(n);
 if(lifeneuron)
 {
  std::shared_ptr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(storage->TakeObject("NNeuronLife"));
  lifeneuron->AddComponent(nlife);

  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;//1;
  conn.Id=nlife->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  res=n->CreateLink(item,conn);
 }
*/
 if(!res)
  return 0;

 return dynamic_pointer_cast<NPulseNeuron>(n);
}
// ����� ������������ ������ �������� ����������� ������� � ������������ ���������� ����������
std::shared_ptr<NPulseNeuron> NPulseLibrary::CreateCustomNewSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzonemembraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, vector<int> &dendrite_length)
{
 std::shared_ptr<RDK::UContainer> membr=0,ltmembr=0;
 std::shared_ptr<NPulseChannel> channel1, channel2, ltchannel1,ltchannel2, channel1temp,channel2temp;
 std::shared_ptr<NPulseNeuron> n;
 std::shared_ptr<NNet> ltzone;
 bool res;
 RDK::ULinkSide item,conn;


 if(!storage)
  return 0;

 n=static_pointer_cast<NPulseNeuron>(storage->TakeObject(neuronclass));
 if(!n)
  return 0;


 ltzone=static_pointer_cast<NNet>(storage->TakeObject(ltzone_class));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");

 std::shared_ptr<NConstGenerator> gen_pos,gen_neg;
 gen_pos=static_pointer_cast<NConstGenerator>(storage->TakeObject(pos_gen_class));
 res=n->AddComponent(gen_pos);
 gen_neg=static_pointer_cast<NConstGenerator>(storage->TakeObject(neg_gen_class));
 n->AddComponent(gen_neg);

 ltmembr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(ltzonemembraneclass));
 ltmembr->SetName("LTMembrane");
 res=n->AddComponent(ltmembr);

 ltchannel1=static_pointer_cast<NPulseChannel>(ltmembr->GetComponent("PosChannel"));
 ltchannel2=static_pointer_cast<NPulseChannel>(ltmembr->GetComponent("NegChannel"));
 item.Index=0;
 conn.Index=-1;

 // ������������� �������� �����
 item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 conn.Id=ltmembr->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);

 // ������������� ����� ���������� � ���������
 item.Id=ltchannel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 conn.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);
 item.Id=ltchannel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);



 for(int i=0;i<num_membranes;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

  item.Index=0;
  conn.Index=-1;

  // ������������� ����� ���������� � ���������
  item.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltchannel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
  item.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltchannel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  for(int j=1;j<dendrite_length[i];j++)
  {
   membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
   res=n->AddComponent(membr);

   channel1temp=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));
   channel2temp=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

   item.Id=channel1temp->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   res=n->CreateLink(item,conn);
   item.Id=channel2temp->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   res=n->CreateLink(item,conn);

   channel1 = channel1temp;
   channel2 = channel2temp;

  }

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  item.Id=gen_neg->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  item.Id=gen_pos->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
 }
/*

 std::shared_ptr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(n);
 if(lifeneuron)
 {
  std::shared_ptr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(storage->TakeObject("NNeuronLife"));
  lifeneuron->AddComponent(nlife);

  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;//1;
  conn.Id=nlife->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  res=n->CreateLink(item,conn);
 }*/

 if(!res)
  return 0;

 return dynamic_pointer_cast<NPulseNeuron>(n);
}

// ����� ������������ ������ �������� ����������� �������
std::shared_ptr<NPulseNeuron> NPulseLibrary::CreateSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, int dendrite_length)
{
 std::shared_ptr<RDK::UContainer> membr=0;
 std::shared_ptr<NPulseChannel> channel1, channel2;
 std::shared_ptr<NPulseNeuron> n;
 std::shared_ptr<NNet> ltzone;
 bool res;
 RDK::ULinkSide item,conn;


 if(!storage)
  return 0;

 n=static_pointer_cast<NPulseNeuron>(storage->TakeObject(neuronclass));
 if(!n)
  return 0;


 ltzone=static_pointer_cast<NNet>(storage->TakeObject(ltzone_class));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");

 std::shared_ptr<NConstGenerator> gen_pos,gen_neg;
 gen_pos=static_pointer_cast<NConstGenerator>(storage->TakeObject(pos_gen_class));
 res=n->AddComponent(gen_pos);
 gen_neg=static_pointer_cast<NConstGenerator>(storage->TakeObject(neg_gen_class));
 n->AddComponent(gen_neg);

 for(int i=0;i<num_membranes;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

  item.Index=0;
  conn.Index=-1;

  // ������������� �������� �����
  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=membr->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  // ������������� ����� ���������� � ���������
  item.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
  item.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  item.Id=gen_neg->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  item.Id=gen_pos->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
 }

 for(int i=1;i<dendrite_length;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

 }
/*
 std::shared_ptr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(n);
 if(lifeneuron)
 {
  std::shared_ptr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(storage->TakeObject("NNeuronLife"));
  lifeneuron->AddComponent(nlife);

  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;//1;
  conn.Id=nlife->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  res=n->CreateLink(item,conn);
 }
*/
 if(!res)
  return 0;

 return dynamic_pointer_cast<NPulseNeuron>(n);
}

// ����� ������������ ������ �������� ����������� ������� � ������������ ���������� ����������
std::shared_ptr<NPulseNeuron> NPulseLibrary::CreateCustomSimplePulseNeuron(UStorage *storage, const string &neuronclass, const string &membraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, std::vector<int> dendrite_length)
{
 std::shared_ptr<RDK::UContainer> membr=0;
 std::shared_ptr<NPulseChannel> channel1, channel2, channel1temp, channel2temp;
 std::shared_ptr<NPulseNeuron> n;
 std::shared_ptr<NNet> ltzone;
 bool res;
 RDK::ULinkSide item,conn;


 if(!storage)
  return 0;

 n=static_pointer_cast<NPulseNeuron>(storage->TakeObject(neuronclass));
 if(!n)
  return 0;


 ltzone=static_pointer_cast<NNet>(storage->TakeObject(ltzone_class));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");

 std::shared_ptr<NConstGenerator> gen_pos,gen_neg;
 gen_pos=static_pointer_cast<NConstGenerator>(storage->TakeObject(pos_gen_class));
 res=n->AddComponent(gen_pos);
 gen_neg=static_pointer_cast<NConstGenerator>(storage->TakeObject(neg_gen_class));
 n->AddComponent(gen_neg);

 for(int i=0;i<num_membranes;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

  item.Index=0;
  conn.Index=-1;

  // ������������� �������� �����
  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=membr->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  // ������������� ����� ���������� � ���������
  item.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
  item.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  for(int j=1;j<dendrite_length[i];j++)
  {
   membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass/*"NPNeuronMembrane"*/));
   res=n->AddComponent(membr);

   channel1temp=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));
   channel2temp=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

   item.Id=channel1temp->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   res=n->CreateLink(item,conn);
   item.Id=channel2temp->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   res=n->CreateLink(item,conn);

   channel1 = channel1temp;
   channel2 = channel2temp;

  }

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  item.Id=gen_neg->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);

  item.Id=gen_pos->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  res=n->CreateLink(item,conn);
 }

/*
 std::shared_ptr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(n);
 if(lifeneuron)
 {
  std::shared_ptr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(storage->TakeObject("NNeuronLife"));
  lifeneuron->AddComponent(nlife);

  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;//1;
  conn.Id=nlife->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  res=n->CreateLink(item,conn);
 }
*/
 if(!res)
  return 0;

 return dynamic_pointer_cast<NPulseNeuron>(n);
}

// ����� ������������ ������ �������� ����������� ������� � ��������� �����
std::shared_ptr<NPulseNeuron> NPulseLibrary::CreateSimplePulseHebbNeuron(UStorage *storage, const string &neuronclass, const string &pos_gen_class, const string &neg_gen_class, int num_membranes,
					int num_stimulates, int num_arresting, int dendrite_length)
{
 std::shared_ptr<RDK::UContainer> membr;
 std::shared_ptr<NPulseChannel> channel1, channel2;
 std::shared_ptr<NPulseChannel> dchannel1, dchannel2;

 std::shared_ptr<NPulseNeuron> n;
 std::shared_ptr<NPulseLTZone> ltzone;
 bool res;
 RDK::ULinkSide item,conn;
 vector<std::shared_ptr<NPulseHebbSynapse> > synapse_list;

 if(!storage)
  return 0;

 n=static_pointer_cast<NPulseNeuron>(storage->TakeObject(neuronclass));
 if(!n)
  return 0;


 ltzone=static_pointer_cast<NPulseLTZone>(storage->TakeObject("NPLTZone"));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");

 std::shared_ptr<NConstGenerator> gen_pos, gen_neg;
 gen_pos=static_pointer_cast<NConstGenerator>(storage->TakeObject(pos_gen_class));
 res=n->AddComponent(gen_pos);
 gen_neg=static_pointer_cast<NConstGenerator>(storage->TakeObject(neg_gen_class));
 n->AddComponent(gen_neg);

 synapse_list.clear();
 for(int i=0;i<num_membranes;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject("NPNeuronHebbMembrane"));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));

  for(int j=0;j<channel1->GetNumComponents();j++)
   synapse_list.push_back(dynamic_pointer_cast<NPulseHebbSynapse>(channel1->GetComponentByIndex(j)));

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

  for(int j=0;j<channel2->GetNumComponents();j++)
   synapse_list.push_back(dynamic_pointer_cast<NPulseHebbSynapse>(channel2->GetComponentByIndex(j)));


  item.Index=0;
  conn.Index=-1;

  // ������������� �������� �����
  item.Id=ltzone->GetLongId(n);
  conn.Id=membr->GetLongId(n);
  res=n->CreateLink(item,conn);

  // ������������� ����� ���������� � ���������
  item.Id=channel1->GetLongId(n);
  conn.Id=ltzone->GetLongId(n);
  res=n->CreateLink(item,conn);
  item.Id=channel2->GetLongId(n);
  res=n->CreateLink(item,conn);

  if(dendrite_length == 1 || i != 0)
  {
   // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
   item.Id=gen_neg->GetLongId(n);
   conn.Id=channel1->GetLongId(n);
   res=n->CreateLink(item,conn);

   item.Id=gen_pos->GetLongId(n);
   conn.Id=channel2->GetLongId(n);
   res=n->CreateLink(item,conn);
  }

  if(i == 0)
  {
   dchannel1=channel1;
   dchannel2=channel2;
  }
 }

 for(int i=1;i<dendrite_length;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject("NPNeuronHebbMembrane"));
  res=n->AddComponent(membr);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));

  for(int j=0;j<channel1->GetNumComponents();j++)
   synapse_list.push_back(dynamic_pointer_cast<NPulseHebbSynapse>(channel1->GetComponentByIndex(j)));


  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));

  for(int j=0;j<channel2->GetNumComponents();j++)
   synapse_list.push_back(dynamic_pointer_cast<NPulseHebbSynapse>(channel2->GetComponentByIndex(j)));

   // ����� ����� �������� � �������
   item.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   conn.Id=dchannel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   res=n->CreateLink(item,conn);

   item.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   conn.Id=dchannel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
   res=n->CreateLink(item,conn);

   dchannel1=channel1;
   dchannel2=channel2;
 }

 // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
 item.Id=gen_neg->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);

 item.Id=gen_pos->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
 res=n->CreateLink(item,conn);


  // ������������� ����� ���������� � ��������� �����
  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;
  conn.Index=1;
  for(size_t j=0;j<synapse_list.size();j++)
  {
   if(synapse_list[j])
   {
    conn.Id=synapse_list[j]->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
    res=n->CreateLink(item,conn);
   }
  }
/*
 std::shared_ptr<NPulseLifeNeuron> lifeneuron=dynamic_pointer_cast<NPulseLifeNeuron>(n);
 if(lifeneuron)
 {
  std::shared_ptr<NNeuronLife> nlife=dynamic_pointer_cast<NNeuronLife>(storage->TakeObject("NNeuronLife"));
  lifeneuron->AddComponent(nlife);

  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  item.Index=0;//1;
  conn.Id=nlife->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  res=n->CreateLink(item,conn);
 }*/

 if(!res)
  return 0;

 return dynamic_pointer_cast<NPulseNeuron>(n);
}

// ����� ������������ ������ ������������ �������
std::shared_ptr<NAfferentNeuron> NPulseLibrary::CreateAfferentNeuron(UStorage *storage, const string &membraneclass, const string &ltzone_class, const string &pos_gen_class, const string &neg_gen_class, int num_membranes)
{
 std::shared_ptr<RDK::UContainer> membr;
 std::shared_ptr<NPulseChannel> channel1, channel2;
 std::shared_ptr<NAfferentNeuron> n;
 std::shared_ptr<NLTZone> ltzone;
 std::shared_ptr<NReceptor> receptor;
 bool res;
 RDK::ULinkSide item,conn;

 item.Index=0;
 conn.Index=-1;

 if(!storage)
  return 0;

 n=static_pointer_cast<NAfferentNeuron>(storage->TakeObject("NAfferentNeuron"));
 if(!n)
  return 0;

 ltzone=static_pointer_cast<NLTZone>(storage->TakeObject(ltzone_class));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");
 ltzone->Threshold=0;

 std::shared_ptr<NConstGenerator> gen_pos,gen_neg;
 gen_pos=static_pointer_cast<NConstGenerator>(storage->TakeObject(pos_gen_class));
 res=n->AddComponent(gen_pos);
 gen_neg=static_pointer_cast<NConstGenerator>(storage->TakeObject(neg_gen_class));
 n->AddComponent(gen_neg);

 bool linkres=true;
 for(int i=0;i<num_membranes;i++)
 {
  membr=static_pointer_cast<NPulseMembrane>(storage->TakeObject(membraneclass));
  res=n->AddComponent(membr);

  receptor=static_pointer_cast<NReceptor>(storage->TakeObject("NReceptor"));
  receptor->ExpCoeff=0.01;
  receptor->Gain=1;
  res=n->AddComponent(receptor);

  channel1=static_pointer_cast<NPulseChannel>(membr->GetComponent("PosChannel"));//(storage->TakeObject("NPChannel"));
//  channel1->SetNumInputs(2);

  channel2=static_pointer_cast<NPulseChannel>(membr->GetComponent("NegChannel"));//(storage->TakeObject("NPChannel"));
  // ������������� �������� �����
  item.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=membr->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  linkres&=n->CreateLink(item,conn);

  // ������������� ����� ���������� � ���������
  item.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  linkres&=n->CreateLink(item,conn);
  item.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
//  conn.Index=1;
  linkres&=res=n->CreateLink(item,conn);
//  conn.Index=0;

  // ����� ����� ���������� ���������� ��������� ������ ������� � ��������
  item.Id=gen_neg->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel1->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  linkres&=n->CreateLink(item,conn);

  item.Id=receptor->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=0;
  linkres&=n->CreateLink(item,conn);

//  item.Id=gen_pos->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
//  conn.Id=channel2->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
//  linkres=n->CreateLink(item,conn);
 }

 if(!linkres)
  return 0;

 if(!res)
  return 0;

 return dynamic_pointer_cast<NAfferentNeuron>(n);
}


// ����� ������������ ������ �������� ������������ �������
std::shared_ptr<NAfferentNeuron> NPulseLibrary::CreateSimpleAfferentNeuron(UStorage *storage, const string &ltzone_class, double max_output)
{
 std::shared_ptr<RDK::UContainer> membr;
 std::shared_ptr<NPulseChannel> channel1, channel2;
 std::shared_ptr<NAfferentNeuron> n;
 std::shared_ptr<NLTZone> ltzone;
 std::shared_ptr<NReceptor> receptor;
 bool res;
 RDK::ULinkSide item,conn;

 item.Index=0;
 conn.Index=-1;

 if(!storage)
  return 0;

 n=static_pointer_cast<NAfferentNeuron>(storage->TakeObject("NAfferentNeuron"));
 if(!n)
  return 0;

 ltzone=static_pointer_cast<NLTZone>(storage->TakeObject(ltzone_class));
 n->AddComponent(ltzone);//,&n->LTZone);
 ltzone->SetName("LTZone");
 ltzone->Threshold=0;

 bool linkres=true;

  receptor=static_pointer_cast<NReceptor>(storage->TakeObject("NReceptor"));
  receptor->Gain=1;
  receptor->OutputAdaptationMode=0;
  receptor->InputAdaptationMode=0;
  receptor->MinOutputRange=0;
  receptor->MaxOutputRange=max_output;
  res=n->AddComponent(receptor);

  item.Id=receptor->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Id=ltzone->GetLongId(std::shared_ptr<RDK::UContainer>(n.get()));
  conn.Index=-1;
  linkres&=n->CreateLink(item,conn);

 if(!linkres)
  return 0;

 if(!res)
  return 0;

 return dynamic_pointer_cast<NAfferentNeuron>(n);
}

// --------------------------


}

#endif

