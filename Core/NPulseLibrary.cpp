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


// --------------------------
// ������������ � �����������
// --------------------------
NPulseLibrary::NPulseLibrary(void)
 : ULibrary("PulseLibrary","1.0")
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
 UContainer *cont;


 cont=new NPulseLTZoneThreshold;
 cont->SetName("PLTZone");
 cont->Default();
 UploadClass("NPulseLTZoneThreshold",cont);

 cont=new NPulseLTZoneThreshold;
 cont->SetName("PLTZone");
 cont->Default();
 {
  NPulseLTZoneThreshold *ltzonet=dynamic_cast<NPulseLTZoneThreshold *>(cont);
  ltzonet->Threshold=-0.055;
  ltzonet->ThresholdOff=-0.1;
 }
 UploadClass("NPulseLTZoneThresholdBio",cont);

 cont=new NPulseLTZone;
 cont->SetName("PLTZone");
 cont->Default();
 UploadClass("NPLTZone",cont);

 cont=new NContinuesLTZone;
 cont->SetName("CLTZone");
 cont->Default();
 UploadClass("NCLTZone",cont);

 cont=new NPulseSimpleLTZone;
 cont->SetName("PLTZone");
 cont->Default();
 UploadClass("NPSimpleLTZone",cont);

 cont=new NContinuesSimpleLTZone;
 cont->SetName("CLTZone");
 cont->Default();
 UploadClass("NCSimpleLTZone",cont);

 cont=new NPulseChannel;
 cont->SetName("PChannel");
 cont->Default();
 UploadClass("NPChannel",cont);

 cont=new NPulseSynapse;
 cont->SetName("PSynapse");
 cont->Default();
 UploadClass("NPSynapse",cont);

 NPulseSynapse *syn=dynamic_pointer_cast<NPulseSynapse>(dynamic_cast<UStorage*>(storage)->TakeObject("NPSynapse"));
 syn->Resistance=2e7*4.3;
 syn->DissociationTC=0.005;
 UploadClass("NPSynapseBio",syn);


 cont=new NPulseSynChannel;
 cont->SetName("PChannel");
 cont->Default();
 UploadClass("NPSynChannel",cont);

 cont=new NContinuesSynChannel;
 cont->SetName("CChannel");
 cont->Default();
 UploadClass("NCSynChannel",cont);

 cont=new NPulseHebbSynapse;
 cont->SetName("Synapse");
 cont->Default();
 UploadClass("NPHebbSynapse",cont);

 UEPtr<NPulseChannel> ch, ch_pos, ch_neg;
 // ������ ������� ��������� ������� �������� ��������
 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NPExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NPInhChannel",ch_neg);

 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 ch_pos->FBResistance=1e7;
 UploadClass("NPExcChannelBio",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 ch_neg->FBResistance=1e7;
 UploadClass("NPInhChannelBio",ch_neg);


 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPSynChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NPSynExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPSynChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NPSynInhChannel",ch_neg);

 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NCSynChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NCSynExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NCSynChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NCSynInhChannel",ch_neg);

 // ������ ������� ��������� �������������� ����
 ch=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch->Capacity=1e-8;
 ch->RestingResistance=1e6;
 ch->SetName("PChannel");
 UploadClass("NPLTChannel",ch);

 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPLTChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NPLTExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPLTChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NPLTInhChannel",ch_neg);

 ch=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPSynChannel"));
 ch->Capacity=1e-8;
 ch->RestingResistance=1e6;
 ch->SetName("PChannel");
 UploadClass("NPLTSynChannel",ch);

 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPLTSynChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NPLTSynExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPLTSynChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NPLTSynInhChannel",ch_neg);

 // ������������ ������� ��������
 UEPtr<NPulseMembrane> membr;
 cont=new NPulseMembrane;
 cont->SetName("PMembrane");
 cont->Default();
 UploadClass("NPMembrane",cont);

 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NPExcChannelBio";
 membr->SynapseClassName="NPSynapseBio";
 membr->InhChannelClassName="NPInhChannelBio";
 UploadClass("NPMembraneBio",membr);

 // ������� �������� �������������� ����
 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("LTMembrane");
 membr->ExcChannelClassName="NPLTExcChannel";
 membr->InhChannelClassName="NPLTInhChannel";
 UploadClass("NPLTZoneNeuronMembrane",membr);

 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->ExcChannelClassName="NPLTSynExcChannel";
 membr->InhChannelClassName="NPLTSynInhChannel";
 UploadClass("NPLTZoneSynNeuronMembrane",membr);

 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NPSynExcChannel";
 membr->InhChannelClassName="NPSynInhChannel";
 UploadClass("NPSynNeuronMembrane",membr);

 // ������� �������� ������������ �������
 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NCSynExcChannel";
 membr->InhChannelClassName="NCSynInhChannel";
 UploadClass("NCSynNeuronMembrane",membr);

 // ��������� ������� ����� � ������� �������� � ����
 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("PMembrane");
 membr->Default();
 membr->SynapseClassName="NPHebbSynapse";
 membr->Build();
 UploadClass("NPNeuronHebbMembrane",membr);

 cont=new NEyeMuscle;
 cont->SetName("EyeMuscle");
 cont->Default();
 UploadClass("NEyeMuscle",cont);

 cont=new NMuscle;
 cont->SetName("Muscle");
 cont->Default();
 UploadClass("NMuscle",cont);

 cont=new NPac;
 cont->SetName("Pac");
 cont->Default();
 UploadClass("NPac",cont);

 cont=new NReceptor;
 cont->SetName("Receptor");
 cont->Default();
 UploadClass("NReceptor",cont);

 cont=new NCPac;
 cont->SetName("Pac");
 cont->Default();
 dynamic_pointer_cast<NCPac>(UEPtr<UContainer>(cont))->TCMode=0;
 UploadClass("NCPac",cont);

 // ���������� ������ ���������� PulseItemsLibrary
 cont=dynamic_pointer_cast<UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 cont->SetName("PNeuronNegCGenerator");
 dynamic_cast<NConstGenerator*>(cont)->Amplitude=-1;
 UploadClass("NPNeuronNegCGenerator",cont);

 cont=dynamic_pointer_cast<UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 cont->SetName("PNeuronPosCGenerator");
 dynamic_cast<NConstGenerator*>(cont)->Amplitude=1;
 UploadClass("NPNeuronPosCGenerator",cont);

 cont=dynamic_pointer_cast<UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 cont->SetName("PNeuronNegCGenerator");
 dynamic_cast<NConstGenerator*>(cont)->Amplitude=-2;
 UploadClass("NCNeuronNegCGenerator",cont);

 cont=dynamic_pointer_cast<UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 cont->SetName("PNeuronPosCGenerator");
 dynamic_cast<NConstGenerator*>(cont)->Amplitude=2;
 UploadClass("NCNeuronPosCGenerator",cont);

 cont=dynamic_pointer_cast<UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 cont->SetName("NegGenerator");
 dynamic_cast<NConstGenerator*>(cont)->Amplitude=-1;
 UploadClass("NPNeuronNegCGeneratorBio",cont);

 cont=dynamic_pointer_cast<UContainer>(dynamic_cast<UStorage*>(storage)->TakeObject("NCGenerator"));
 cont->SetName("PosGenerator");
 dynamic_cast<NConstGenerator*>(cont)->Amplitude=0.93;
 UploadClass("NPNeuronPosCGeneratorBio",cont);

 cont=new NPulseNeuron;
 cont->SetName("PNeuron");
 cont->Default();
 UploadClass("NPNeuron",cont);

 cont=new NPulseNeuron;
 cont->SetName("PHebbNeuron");
 cont->Default();
 dynamic_cast<NPulseNeuron*>(cont)->MembraneClassName="NPNeuronHebbMembrane";
 UploadClass("NPHebbNeuron",cont);

 cont=new NPulseNeuron;
 cont->Default();
 dynamic_cast<NPulseNeuron*>(cont)->MembraneClassName="NCSynNeuronMembrane";
 dynamic_cast<NPulseNeuron*>(cont)->LTMembraneClassName="";
 dynamic_cast<NPulseNeuron*>(cont)->LTZoneClassName="NCLTZone";
 dynamic_cast<NPulseNeuron*>(cont)->ExcGeneratorClassName="NCNeuronNegCGenerator";
 dynamic_cast<NPulseNeuron*>(cont)->InhGeneratorClassName="NCNeuronPosCGenerator";
 cont->SetName("CNeuron");
 UploadClass("NCNeuron",cont);


/* cont=new NPulseLifeNeuron("PLifeNeuron");
 cont->Default();
 UploadClass("NPLifeNeuron",cont);
  */
 cont=new NAfferentNeuron;
 cont->SetName("AfferentNeuron");
 cont->Default();
 UploadClass("NAfferentNeuron",cont);

 cont=new NPulseSynapseStdp;
 cont->SetName("SynapseStdp");
 cont->Default();
 UploadClass("NPulseSynapseStdp",cont);
}
// --------------------------


}

#endif

