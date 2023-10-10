// ===========================================================
// Version:        1.0.0
// ===========================================================
/* ***********************************************************
@Copyright Alexander V. Bakhshiev, 2010.
E-mail:		alexab@ailab.ru
url:            http://ailab.ru

This file - part of the project: Neuro Modeler Software Developer Kit (NMSDK)

File License:        BSD License
Project License:     BSD License
See file license.txt for more information
*********************************************************** */

#ifndef NLIFE_NET_CPP
#define NLIFE_NET_CPP

#include "NLifeNet.h"
#include "../../Nmsdk-PulseLib/Core/NPulseMembrane.h"
#include "../../Nmsdk-PulseLib/Core/NPulseLTZone.h"
#include "../../Nmsdk-PulseLib/Core/NPulseChannel.h"
#include <algorithm>

namespace NMSDK {


// ------------------------
// Операторы
// ------------------------
bool operator < (const NEnsembleLink &link1,const NEnsembleLink &link2)
{
 return (link1.Power<link2.Power);
}
// ------------------------


// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
NLifeNet::NLifeNet(void)
//: NNet(name),
 : AvailableEnergy("AvailableEnergy",this),
  CellDivisionFlag("CellDivisionFlag",this),
  CellDivisionEnergy("CellDivisionEnergy",this),
  EnsembleThreshold("EnsembleThreshold",this),
  EnsembleLinkThreshold("EnsembleLinkThreshold",this),
  EnsembleThresholdMode("EnsembleThresholdMode",this),
  EnsembleCalculationStep("EnsembleCalculationStep",this),
  EnergyBalanceMode("EnergyBalanceMode",this),

  Output1("Output1",this),
  Output2("Output2",this),
  Output3("Output3",this),
  Output4("Output4",this),
  Output5("Output5",this),
  Output6("Output6",this),
  Output7("Output7",this),
  Output8("Output8",this),
  Output9("Output9",this),
  Output10("Output10",this),

  SummaryFeeling("SummaryFeeling",this),
  SummaryEnergy("SummaryEnergy",this),
  SummaryWearOut("SummaryWearOut",this),
  SummaryFrequency("SummaryFrequency",this),
  SummaryLiveNeurons("SummaryLiveNeurons",this)
{
 EnsembleCalculationCounter=0;
}

NLifeNet::~NLifeNet(void)
{
}
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Возвращает число систем жизнеобеспечения нейронов
size_t NLifeNet::GetNumNeuronsLife(void) const
{
 return NeuronsLife.size();
}

// Возвращает заданную систему жизнеобеспечения нейрона
NNeuronLife* NLifeNet::GetNeuronLife(size_t i)
{
 return NeuronsLife[i];
}

// Возвращает число бета-ансамблей нейронов
size_t NLifeNet::GetNumBetaEnsembles(void) const
{
 return BetaEnsembles.size();
}

// Возвращает заданный бета-ансамбль
const NEnsemble& NLifeNet::GetBetaEnsemble(size_t i)
{
 return BetaEnsembles[i].second;
}

// Возвращает мощность заданного бета-ансамбля
double NLifeNet::GetBetaEnsemblePower(size_t i) const
{
 return BetaEnsembles[i].first;
}

// Возвращает число ансамблей нейронов
size_t NLifeNet::GetNumEnsembles(void) const
{
 return Ensembles.size();
}

// Возвращает заданный ансамбль
const NEnsemble& NLifeNet::GetEnsemble(size_t i)
{
 return Ensembles[i].second;
}

// Возвращает мощность заданного ансамбля
double NLifeNet::GetEnsemblePower(size_t i) const
{
 return Ensembles[i].first;
}

// Возвращает имя ансамбля по имени составляющих нейронов
NameT NLifeNet::GetEnsembleName(size_t i) const
{
 NameT name;
 for(size_t j=0;j<Ensembles[i].second.size();j++)
 {
  name+=Ensembles[i].second[j]->GetName();
  if(j<Ensembles[i].second.size()-1)
   name+="+";
 }
 return name;
}

// Возвращает абсолютное значение активности ансамбля
// (средняя частота всех нейронов ансамбля)
double NLifeNet::GetEnsembleActivity(size_t i) const
{
 double eactivity=0;
 for(size_t j=0;j<Ensembles[i].second.size();j++)
  eactivity+=Ensembles[i].second[j]->GetLTZone()->GetOutputData(2).Double[0];

 if(Ensembles[i].second.size())
  return eactivity/Ensembles[i].second.size();

 return 0;
}

double NLifeNet::GetRelativeEnsembleActivity(size_t i) const
{
 double eactivity=GetEnsembleActivity(i);
// return (eactivity>POutputData[9].Double[0])?(eactivity-POutputData[9].Double[0]):0;
 return eactivity-Output10(0,0);
}

// Возвращает число связей между ансамблями
size_t NLifeNet::GetNumEnsembleLinks(void) const
{
 return EnsembleLinks.size();
}

// Возвращает связь между ансамблями
const NEnsembleLink& NLifeNet::GetEnsembleLink(size_t i) const
{
 return EnsembleLinks[i];
}
// --------------------------


// --------------------------
// Системные методы управления объектом
// --------------------------
// Выделяет память для новой чистой копии объекта этого класса
NLifeNet* NLifeNet::New(void)
{
 return new NLifeNet;
}
// --------------------------

// --------------------------
// Скрытые методы управления компонентами
// --------------------------
// Выполняет завершающие пользовательские действия
// при добавлении дочернего компонента в этот объект
// Метод будет вызван только если comp был
// успешно добавлен в список компонент
bool NLifeNet::AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer)
{
 if(!NNet::AAddComponent(comp,pointer))
  return false;

 bool res=true;

 UEPtr<NPulseLifeNeuron> n=dynamic_pointer_cast<NPulseLifeNeuron>(comp);
 UEPtr<NNeuronLife> life;
 UEPtr<NSum> pebonus;
 if(n)
 {
  life=n->GetNeuronLife();
   if(life)
   {
	if(life->ExternalEnergyBonus)
	 pebonus=&*life->ExternalEnergyBonus;
	NeuronsLife.push_back(life);
	res&=Connect(life,0,2);
	res&=Connect(life,1,1);
   }

  // Устанавливает первичные связи нейрона
  for(int i=0;i<GetNumComponents();i++)
  {
   UEPtr<NPulseLifeNeuron> nn=dynamic_pointer_cast<NPulseLifeNeuron>(GetComponentByIndex(i));
   UEPtr<NNeuronLife> life2;
   if(nn && nn != n)
   {
	life2=nn->GetNeuronLife();
	if(life2)
	{
	 res&=life2->Connect(pebonus,7);
	}
	RDK::MMatrix<double,3,1> dist=n->GetCoord()-nn->GetCoord();
	if((!dist)<=LinkDistance) // Фиксированное расстояние
	{
	 NPulseMembrane *cont=dynamic_cast<NPulseMembrane*>(n->BranchDendrite("PNeuronHebbMembrane4",true));
	 NPulseMembrane *cont2=dynamic_cast<NPulseMembrane*>(nn->BranchDendrite("PNeuronHebbMembrane4",true));
	 RDK::ULinkSide item,conn;

	 // Подключаем возбуждающие синапсы
	 RDK::MVector<double,3> diff1(n->GetCoord()-nn->GetCoord()),
						  diff2(nn->GetCoord()-n->GetCoord());
	 double mul1=diff1*RDK::MVector<double,3>(1,0,0);
	 double mul2=diff2*RDK::MVector<double,3>(1,0,0);
	 if(mul1>0)
	 {
	  item.Id=n->GetLTZone()->GetLongId(this);
	  item.Index=0;
	  for(size_t k=0;k<cont2->GetNumPosChannels();k++)
	  {
	   NPulseChannel* channel=dynamic_cast<NPulseChannel*>(cont2->GetPosChannel(k));
	   if(channel)
		for(int l=0;l<channel->GetNumSynapses();l++)
		{
		 conn.Id=channel->GetSynapse(l)->GetLongId(this);
		 conn.Index=0;
		 res&=CreateLink(item,conn);
	    }
	  }

	  item.Id=nn->GetLTZone()->GetLongId(this);
	  item.Index=0;
	  for(size_t k=0;k<cont->GetNumPosChannels();k++)
	  {
	   NPulseChannel* channel=dynamic_cast<NPulseChannel*>(cont->GetPosChannel(k));
	   if(channel)
		for(int l=0;l<channel->GetNumSynapses();l++)
		{
		 conn.Id=channel->GetSynapse(l)->GetLongId(this);
		 conn.Index=0;
		 res&=CreateLink(item,conn);
	    }
	  }
	 }

	 if(mul2<0)
	 {
	  cont=dynamic_cast<NPulseMembrane*>(n->BranchDendrite("PNeuronHebbMembrane3",true));
	  cont2=dynamic_cast<NPulseMembrane*>(nn->BranchDendrite("PNeuronHebbMembrane3",true));
	  // Подключаем тормозные синапсы
	  item.Id=n->GetLTZone()->GetLongId(this);
	  item.Index=0;
	  for(size_t k=0;k<cont2->GetNumNegChannels();k++)
	  {
 	   NPulseChannel* channel=dynamic_cast<NPulseChannel*>(cont2->GetNegChannel(k));
	   if(channel)
		for(int l=0;l<channel->GetNumSynapses();l++)
		{
		 conn.Id=channel->GetSynapse(l)->GetLongId(this);
		 conn.Index=0;
		 res&=CreateLink(item,conn);
	    }
	  }

	  item.Id=nn->GetLTZone()->GetLongId(this);
	  item.Index=0;
	  for(size_t k=0;k<cont->GetNumNegChannels();k++)
	  {
	   NPulseChannel* channel=dynamic_cast<NPulseChannel*>(cont->GetNegChannel(k));
	   if(channel)
		for(int l=0;l<channel->GetNumSynapses();l++)
		{
		 conn.Id=channel->GetSynapse(l)->GetLongId(this);
		 conn.Index=0;
		 res&=CreateLink(item,conn);
	    }
	  }
	 }
	}
   }
  }
 }


 return res;
}

// Выполняет предварительные пользовательские действия
// при удалении дочернего компонента из этого объекта
// Метод будет вызван только если comp
// существует в списке компонент
bool NLifeNet::ADelComponent(UEPtr<UContainer> comp)
{
 vector<NNeuronLife*>::iterator I;

 if(NeuronsLife.empty())
  return NNet::ADelComponent(comp);

 I=NeuronsLife.begin();
 do
 {
  if((*I)->GetOwner() == comp)
  {
   Disconnect((*I)->GetId());
   NeuronsLife.erase(I);
   I=NeuronsLife.begin();
  }
  else
   ++I;
 }while(I != NeuronsLife.end());

 return NNet::ADelComponent(comp);
}
// --------------------------

// --------------------------
// Скрытые методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool NLifeNet::ADefault(void)
{
/* SetNumOutputs(10);
 for(int i=0;i<NumOutputs;i++)
  SetOutputDataSize(i,MMatrixSize(1,1));
     */
// vector<size_t> size;
// size.assign(NumOutputs,1);
// SetOutputDataSize(size);

 AvailableEnergy=10;

 CellDivisionFlag=true;
 CellDivisionEnergy=0.5;
 EnsembleThreshold=0.0;
 EnsembleLinkThreshold=0.000;
 EnsembleThresholdMode=0;
 EnsembleCalculationStep=0.1;
 LinkDistance=1;

 EnergyBalanceMode=1;

 Output1.Assign(1,1,0.0);
 Output2.Assign(1,1,0.0);
 Output3.Assign(1,1,0.0);
 Output4.Assign(1,1,0.0);
 Output5.Assign(1,1,0.0);
 Output6.Assign(1,1,0.0);
 Output7.Assign(1,1,0.0);
 Output8.Assign(1,1,0.0);
 Output9.Assign(1,1,0.0);
 Output10.Assign(1,1,0.0);

 return true;
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool NLifeNet::ABuild(void)
{
 return true;
}

// Сброс процесса счета.
bool NLifeNet::AReset(void)
{
 EnsembleCalculationCounter=0;
 return true;
}

// Выполняет расчет этого объекта
bool NLifeNet::ACalculate(void)
{
 vector<NNeuronLife*> best_neurons;
 SummaryFeeling=0;
 SummaryWearOut=0;
 SummaryLiveNeurons=0;
 SummaryFrequency=0;
 SummaryEnergy=0;

 SynapticMap.clear();
 for(size_t i=0;i<NeuronsLife.size();i++)
 {
//  POutputData[10].Double[i]=0;
  SynapticMap[static_pointer_cast<NPulseLifeNeuron>(NeuronsLife[i]->GetOwner())];
  if(NeuronsLife[i]->GetActivity())
  {
   SummaryFeeling.v+=NeuronsLife[i]->Output2(0,0);
   SummaryWearOut.v+=NeuronsLife[i]->Output3(0,0);
   SummaryEnergy.v+=NeuronsLife[i]->Output5(0,0);
   UEPtr<NPulseLTZone> zone=static_pointer_cast<NPulseLTZone>(static_pointer_cast<UContainer>(NeuronsLife[i]->GetOwner())->GetComponent("LTZone"));
//   POutputData[10].Double[i]=zone->GetNumAConnectors(0);
   SummaryFrequency.v+=zone->OutputFrequency(0,0);
   ++SummaryLiveNeurons.v;
   if(CellDivisionFlag && NeuronsLife[i]->OutputThreshold(0,0)>CellDivisionEnergy)
	best_neurons.push_back(NeuronsLife[i]);
  }
 }

 // Нейроны способные делится - делятся
 for(size_t i=0;i<best_neurons.size();i++)
 {
  best_neurons[i]->Energy=best_neurons[i]->Energy/2;
  UEPtr<NPulseLifeNeuron> n=static_pointer_cast<NPulseLifeNeuron>(Storage->TakeObject(best_neurons[i]->GetMainOwner()->GetClass()));
  n->SetName("N");
  n->SetCoord(static_pointer_cast<NPulseLifeNeuron>(best_neurons[i]->GetMainOwner())->GetCoord());
  n->GetNeuronLife()->Energy=best_neurons[i]->Energy;
  AddComponent(n);
 }

 Output1(0,0)=SummaryFeeling.v;
 if(SummaryLiveNeurons.v>0)
 {
  switch(EnergyBalanceMode)
  {
  case 0:
   Output2(0,0)=AvailableEnergy.v/SummaryLiveNeurons.v;
  break;

  case 1:
   Output2(0,0)=AvailableEnergy.v/SummaryLiveNeurons.v;
  break;
  }
 }
 else
  Output2(0,0)=0;

 Output3(0,0)=SummaryWearOut.v;
 Output4(0,0)=SummaryEnergy.v;
 Output5(0,0)=SummaryFrequency.v;

 Output6(0,0)=SummaryLiveNeurons.v;
 if(SummaryLiveNeurons.v>0)
 {
  Output7(0,0)=SummaryFeeling.v/SummaryLiveNeurons.v;
  Output8(0,0)=SummaryWearOut.v/SummaryLiveNeurons.v;
  Output9(0,0)=SummaryEnergy.v/SummaryLiveNeurons.v;
  Output10(0,0)=SummaryFrequency.v/SummaryLiveNeurons.v;
 }
 else
 {
  Output7(0,0)=0;
  Output8(0,0)=0;
  Output9(0,0)=0;
  Output10(0,0)=0;
 }

 if(EnsembleCalculationStep.v == 0)
  return true;

 ++EnsembleCalculationCounter;
 if(double(EnsembleCalculationCounter)/double(TimeStep) >= EnsembleCalculationStep)
 {
  EnsembleCalculationCounter=0;
  if(!CalcEnsembles(EnsembleThreshold))
   return false;

  if(!CalcEnsembleLinks(EnsembleLinkThreshold))
   return false;
 }

 return true;
}

// Вычисляет ансамбли сети
bool NLifeNet::CalcEnsembles(double threshold)
{
 Ensembles.clear();
 BetaEnsembles.clear();

 double resthreshold=0;
 size_t numneurons=0;
 UEPtr<NPulseLifeNeuron> n;
 switch(EnsembleThresholdMode)
 {
 case 2:
  for(size_t i=0;i<NeuronsLife.size();i++)
  {
   n=static_pointer_cast<NPulseLifeNeuron>(NeuronsLife[i]->GetOwner());
   if(n->GetActivity())
   {
	resthreshold+=n->OutputSummaryPosGsNorm(0,0);
	++numneurons;
   }
  }
  if(numneurons)
   resthreshold/=numneurons;
 break;
 }

 for(size_t i=0;i<NeuronsLife.size();i++)
 {
  n=static_pointer_cast<NPulseLifeNeuron>(NeuronsLife[i]->GetOwner());
  pair<double, NEnsemble > ensemble;
  ensemble.second.push_back(n);
  ensemble.first=n->OutputSummaryPosGs(0,0);
  if(n->GetActivity())
  {
   for(int j=0;j<n->GetNumComponents();j++)
   {
	UEPtr<NPulseMembrane> membrane=dynamic_pointer_cast<NPulseMembrane>(n->GetComponentByIndex(j));
	if(membrane)
	{
	 for(int k=0;k<membrane->GetNumComponents();k++)
	 {
	  UEPtr<NPulseChannel> channel=dynamic_pointer_cast<NPulseChannel>(membrane->GetComponentByIndex(k));
	  if(channel)
	  {
	   for(int m=0;m<channel->GetNumComponents();m++)
	   {
		UEPtr<NPulseHebbSynapse> synapse=dynamic_pointer_cast<NPulseHebbSynapse>(channel->GetComponentByIndex(m));
		if(synapse)
		{
		 if(EnsembleThresholdMode == 1)
		  resthreshold=n->OutputSummaryPosGsNorm(0,0);
		 if(synapse->Output4(0,0) > resthreshold+threshold)
		 {
		  UEPtr<NPulseLifeNeuron> item=dynamic_pointer_cast<NPulseLifeNeuron>(synapse->GetCItem(0).Item->GetMainOwner());
		  if(item)
		  {
		   ensemble.first+=n->OutputSummaryPosGs(0,0);
		   ensemble.second.push_back(item);
		  }
		 }
		}
	   }
	  }
	 }
	}
   }

   if(ensemble.second.size()>1)
	BetaEnsembles.push_back(ensemble);
  }
 }

 // Формируем список ансамблей
 for(size_t i=0;i<BetaEnsembles.size();i++)
 {
  // Берем бета-ансабль...
  pair<double, NEnsemble > bensemble=BetaEnsembles[i];
  pair<double, NEnsemble > ensemble;
  vector<pair<double, NEnsemble > > egroup;

  egroup.push_back(bensemble);
  size_t ix=0;
  for(size_t j=i+1;j<BetaEnsembles.size();j++)
  {
   for(size_t k=0;k<bensemble.second.size();k++)
   {
	if(BetaEnsembles[j].second.size() == bensemble.second.size() &&
	   find(BetaEnsembles[j].second.begin(),BetaEnsembles[j].second.end(),bensemble.second[k]) ==
				BetaEnsembles[j].second.end())
	{
	 break;
	}
	else
	 ix=k;
   }
   if(ix == bensemble.second.size()-1)
   {
	egroup.push_back(BetaEnsembles[j]);
   }
  }

  // Сформировали egroup, проверяем его размер
  if(egroup.size() >= bensemble.second.size())
  {
   for(size_t m=1;m<egroup.size();m++)
	bensemble.first+=egroup[m].first;

   // Сортируем ансамбль
   vector<string> enames;
   enames.resize(bensemble.second.size());
   for(size_t m=0;m<enames.size();m++)
	enames[m]=bensemble.second[m]->GetName();

   sort(enames.begin(),enames.end());

   for(size_t m=0;m<enames.size();m++)
   {
	for(size_t mm=0;mm<bensemble.second.size();mm++)
	{
	 if(enames[m] == bensemble.second[mm]->GetName() && mm > m)
	  swap(bensemble.second[m],bensemble.second[mm]);
	}
   }
   // Конец сортировки


   size_t m;
   for(m=0;m<Ensembles.size();++m)
	if(Ensembles[m].second == bensemble.second)
	 break;

   if(m == Ensembles.size())
	Ensembles.push_back(bensemble);
  }
 }

 return true;
}

// Вычисляет связи между ансамблями сети
bool NLifeNet::CalcEnsembleLinks(double threshold)
{
 EnsembleLinks.clear();
 for(size_t i=0;i<Ensembles.size();i++)
 {
  for(size_t j=0;j<Ensembles[i].second.size();j++)
  {
	 for(int k=0;k<Ensembles[i].second[j]->GetLTZone()->GetNumOutputs();k++)
	  for(int l=0;l<Ensembles[i].second[j]->GetLTZone()->GetNumAConnectors(k);l++)
	  {
	   UEPtr<NPulseHebbSynapse> synapse=
	   	dynamic_pointer_cast<NPulseHebbSynapse>(Ensembles[i].second[j]->GetLTZone()->GetAConnectorByIndex(int(k), int(l)));
	   if(synapse)
	   {
		for(size_t n=0;n<Ensembles.size();n++)
		{
		  for(size_t m=0;m<Ensembles[n].second.size();m++)
		  {
		   if(n != i)
			if(synapse->CheckOwner(Ensembles[n].second[m]))
			 if(synapse->Output4(0,0) > threshold)
			 {
			  NEnsembleLink link;
			  link.In=GetEnsembleName(i);
			  link.Out=GetEnsembleName(n);
			  link.Power=synapse->Output4(0,0);

			  size_t p=0;
			  for(;p<EnsembleLinks.size();p++)
			   if(EnsembleLinks[p].In == link.In &&
				  EnsembleLinks[p].Out == link.Out)
			   {
				EnsembleLinks[p].Power+=link.Power;
				break;
			   }

			  if(p >= EnsembleLinks.size())
			   EnsembleLinks.push_back(link);
			 }
		  }
		}
	   }
	  }
  }
 }

 sort(EnsembleLinks.begin(),EnsembleLinks.end());
 return true;
}
// --------------------------

}
#endif

