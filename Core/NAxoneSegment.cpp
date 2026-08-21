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

#ifndef NAXONE_SEGMENT_CPP
#define NAXONE_SEGMENT_CPP

#include "NAxoneSegment.h"

namespace NMSDK {

NAxoneSegment::NAxoneSegment(void)
 : MembraneClassName("MembraneClassName",this,&NAxoneSegment::SetMembraneClassName),
   LTZoneClassName("LTZoneClassName",this,&NAxoneSegment::SetLTZoneClassName),
   PosGeneratorClassName("PosGeneratorClassName",this,&NAxoneSegment::SetPosGeneratorClassName)
{
 Inertial=0;
 Soma=0;
 LTZone=0;
 PosGenerator=0;
}

NAxoneSegment::~NAxoneSegment(void)
{
}

bool NAxoneSegment::SetMembraneClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 return true;
}

bool NAxoneSegment::SetLTZoneClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 return true;
}

bool NAxoneSegment::SetPosGeneratorClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 return true;
}

NAxoneSegment* NAxoneSegment::New(void)
{
 return new NAxoneSegment;
}

bool NAxoneSegment::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(!comp)
  return false;
 if(dynamic_pointer_cast<NAperiodicLink>(comp))
  return true;
 if(dynamic_pointer_cast<NPulseMembrane>(comp))
  return true;
 if(dynamic_pointer_cast<NLTZone>(comp))
  return true;
 if(dynamic_pointer_cast<NConstGenerator>(comp))
  return true;
 return false;
}

bool NAxoneSegment::ADefault(void)
{
 MembraneClassName="NPMembraneBio";
 LTZoneClassName="NPulseLTZoneThreshold";
 PosGeneratorClassName="NPNeuronPosCGenerator";
 Inertial=0;
 Soma=0;
 LTZone=0;
 PosGenerator=0;
 return NAxoneCommon::ADefault();
}

bool NAxoneSegment::ABuild(void)
{
 return BuildStructure();
}

bool NAxoneSegment::BuildStructure(void)
{
 bool res=true;

 std::string membr_class = MembraneClassName.GetData();
 if(membr_class.empty())
  membr_class="NPMembraneBio";

 std::string ltz_class = LTZoneClassName.GetData();
 if(ltz_class.empty())
  ltz_class="NPulseLTZoneThreshold";

 std::string pos_class = PosGeneratorClassName.GetData();
 if(pos_class.empty())
  pos_class="NPNeuronPosCGenerator";

 // Drop legacy InputBridge from older models
 if(GetComponent("InputBridge", true))
  DelComponent("InputBridge");

 Inertial = AddMissingComponent<NAperiodicLink>("Inertial", "NAperiodicLink");
 if(!Inertial)
 {
  LogMessage(RDK_EX_WARNING,
   "NAxoneSegment::BuildStructure failed to create Inertial (NAperiodicLink)");
  return false;
 }
 Inertial->SetCoord(MVector<double,3>(2.0, 6.5, 0.0));

 PosGenerator = AddMissingComponent<NConstGenerator>("PosGenerator", pos_class);
 if(!PosGenerator)
 {
  LogMessage(RDK_EX_WARNING,
   std::string("NAxoneSegment::BuildStructure failed to create PosGenerator: ")+pos_class);
  return false;
 }
 PosGenerator->SetCoord(MVector<double,3>(2.0, 2.0, 0.0));

 Soma = AddMissingComponent<NPulseMembrane>("Soma", membr_class);
 if(!Soma)
 {
  LogMessage(RDK_EX_WARNING,
   std::string("NAxoneSegment::BuildStructure failed to create Soma: ")+membr_class);
  return false;
 }
 Soma->SetCoord(MVector<double,3>(8.0, 4.5, 0.0));
 if(!Soma->Build())
 {
  LogMessage(RDK_EX_WARNING, "NAxoneSegment::BuildStructure: Soma->Build failed");
  return false;
 }

 LTZone = AddMissingComponent<NLTZone>("LTZone", ltz_class);
 if(!LTZone)
 {
  LogMessage(RDK_EX_WARNING,
   std::string("NAxoneSegment::BuildStructure failed to create LTZone: ")+ltz_class);
  return false;
 }
 LTZone->SetCoord(MVector<double,3>(16.0, 4.5, 0.0));

 NPulseChannelCommon *exc = Soma->GetPosChannel(0);
 NPulseChannelCommon *inh = Soma->GetNegChannel(0);
 if(!exc || !inh)
 {
  LogMessage(RDK_EX_WARNING,
   "NAxoneSegment::BuildStructure: Soma missing ExcChannel/InhChannel");
  return false;
 }

 // Instead of NegGenerator(-1): Inertial.Output (y+Bias) → ExcChannel
 res &= CreateLink(Inertial->GetLongName(this), "Output",
                   exc->GetLongName(this), "ChannelInputs");
 // PosGenerator(+1) → InhChannel (as in NPulseNeuron)
 res &= CreateLink(PosGenerator->GetLongName(this), "Output",
                   inh->GetLongName(this), "ChannelInputs");
 // Channels → LTZone
 res &= CreateLink(exc->GetLongName(this), "Output",
                   LTZone->GetLongName(this), "Inputs");
 res &= CreateLink(inh->GetLongName(this), "Output",
                   LTZone->GetLongName(this), "Inputs");
 // Feedback LTZone → Soma
 res &= CreateLink(LTZone->GetLongName(this), "Output",
                   Soma->GetLongName(this), "InputFeedbackSignal");

 return res;
}

bool NAxoneSegment::AReset(void)
{
 return NAxoneCommon::AReset();
}

bool NAxoneSegment::Calculate(void)
{
 if(!Inertial)
  Inertial = dynamic_pointer_cast<NAperiodicLink>(GetComponent("Inertial", true));
 if(Inertial)
  *Inertial->Input = *Input;
 return NAxoneCommon::Calculate();
}

bool NAxoneSegment::ACalculate(void)
{
 if(!LTZone)
  LTZone = dynamic_pointer_cast<NLTZone>(GetComponent("LTZone", true));
 if(LTZone)
  *Output = *LTZone->Output;
 else
  Output.ToZero();
 return true;
}

}
#endif
