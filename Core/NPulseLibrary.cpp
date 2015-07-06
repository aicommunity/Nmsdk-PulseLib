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
/*
#include "NAfferentNeuron.cpp"
#include "NEyeMuscle.cpp"
#include "NMuscle.cpp"
#include "NPulseChannel.cpp"
#include "NPulseSynChannel.cpp"
#include "NPulseHebbSynapse.cpp"
#include "NPulseLTZone.cpp"
#include "NPulseMembrane.cpp"
#include "NPulseNeuron.cpp"
#include "NPulseSynapse.cpp"
#include "NPac.cpp"
#include "NReceptor.cpp"
*/

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

 cont=new NPulseNeuron;
 cont->SetName("PNeuron");
 cont->Default();
 UploadClass("NPNeuron",cont);

/* cont=new NPulseLifeNeuron("PLifeNeuron");
 cont->Default();
 UploadClass("NPLifeNeuron",cont);
  */
 cont=new NAfferentNeuron;
 cont->SetName("AfferentNeuron");
 cont->Default();
 UploadClass("NAfferentNeuron",cont);

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


 cont=new NPulseMembrane;
 cont->SetName("PMembrane");
 cont->Default();
 UploadClass("NPMembrane",cont);

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
 cont->SetName("PHebbSynapse");
 cont->Default();
 UploadClass("NPHebbSynapse",cont);

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
}
// --------------------------


}

#endif

