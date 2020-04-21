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
// Конструкторы и деструкторы
// --------------------------
NPulseLibrary::NPulseLibrary(void)
 : ULibrary("PulseLibrary","1.0")
{
}
// --------------------------

// --------------------------
// Методы заполенения бибилиотеки
// --------------------------
// Заполняет массив ClassSamples готовыми экземплярами образцов и их именами.
// Не требуется предварительная очистка массива и уборка памяти.
void NPulseLibrary::CreateClassSamples(UStorage *storage)
{
 UContainer *cont;

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
 // Модели ионного механизма обычных участков мембраны
 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NPExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NPInhChannel",ch_neg);

 ch_pos=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPSynChannel"));
 ch_pos->SetName("ExcChannel");
 ch_pos->Type=-1;
 UploadClass("NPSynExcChannel",ch_pos);

 ch_neg=dynamic_pointer_cast<NPulseChannel>(dynamic_cast<UStorage*>(storage)->TakeObject("NPSynChannel"));
 ch_neg->SetName("InhChannel");
 ch_neg->Type=1;
 UploadClass("NPSynInhChannel",ch_neg);

 // Модели ионного механизма низкопороговой зоны
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
 UploadClass("NPLTInhChannel",ch_pos);

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

 // Классические участки мембраны
 UEPtr<NPulseMembrane> membr;
 cont=new NPulseMembrane;
 cont->SetName("PMembrane");
 cont->Default();
 UploadClass("NPMembrane",cont);

 // Участки мембраны низкопороговой зоны
 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("LTMembrane");
 membr->ExcChannelClassName="NPLTExcChannel";
 membr->InhChannelClassName="NPLTInhChannel";
 UploadClass("NPLTZoneNeuronMembrane",membr);

 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->ExcChannelClassName="NPLTSynExcChannel";
 membr->InhChannelClassName="NPLTSynInhChannel";
 UploadClass("NPLTZoneSynNeuronMembrane",membr);

 // Участок мембраны непрерывного нейрона
 membr=dynamic_pointer_cast<NPulseMembrane>(dynamic_cast<UStorage*>(storage)->TakeObject("NPMembrane"));
 membr->SetName("PMembrane");
 membr->ExcChannelClassName="NCSynChannel";
 membr->InhChannelClassName="NCSynChannel";
 UploadClass("NCSynNeuronMembrane",membr);

 // Формируем синапсы хебба и участки мембраны с ними
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



/*
 cont=new NNeuronLife("NeuronLife");
 cont->Default();
 UploadClass("NNeuronLife",cont);

 cont=new NLifeNet("LifeNet");
 cont->Default();
 UploadClass("NLifeNet",cont);
 */


 // Содержимое старой библиотеки PulseItemsLibrary
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

 cont=new NPulseNeuron;
 cont->SetName("PNeuron");
 cont->Default();
 UploadClass("NPNeuron",cont);

 cont=new NPulseNeuron;
 cont->SetName("PHebbNeuron");
 cont->Default();
 dynamic_cast<NPulseNeuron*>(cont)->MembraneClassName="NPNeuronHebbMembrane";
 UploadClass("NPHebbNeuron",cont);
 // UploadClass("NPNeuron","PNeuron",&NPulseNeuron::NewStatic);

/* cont=new NPulseLifeNeuron("PLifeNeuron");
 cont->Default();
 UploadClass("NPLifeNeuron",cont);
  */
 cont=new NAfferentNeuron;
 cont->SetName("AfferentNeuron");
 cont->Default();
 UploadClass("NAfferentNeuron",cont);
}
// --------------------------


}

#endif

