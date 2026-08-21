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

#ifndef NAXONE_CHAIN_AND_DELAY_CPP
#define NAXONE_CHAIN_AND_DELAY_CPP

#include "NAxoneChainAndDelay.h"
#include <sstream>

namespace NMSDK {

NAxoneChainAndDelay::NAxoneChainAndDelay(void)
 : NumNodes("NumNodes",this,&NAxoneChainAndDelay::SetNumNodes),
   InternodeDelayTime("InternodeDelayTime",this,&NAxoneChainAndDelay::SetInternodeDelayTime),
   MembraneClassName("MembraneClassName",this,&NAxoneChainAndDelay::SetMembraneClassName),
   LTZoneClassName("LTZoneClassName",this,&NAxoneChainAndDelay::SetLTZoneClassName),
   PosGeneratorClassName("PosGeneratorClassName",this,&NAxoneChainAndDelay::SetPosGeneratorClassName)
{
}

NAxoneChainAndDelay::~NAxoneChainAndDelay(void)
{
}

std::string NAxoneChainAndDelay::SegmentName(int index_1based) const
{
 std::ostringstream os;
 os << "Segment" << index_1based;
 return os.str();
}

std::string NAxoneChainAndDelay::DelayName(int index_1based) const
{
 std::ostringstream os;
 os << "Delay" << index_1based;
 return os.str();
}

void NAxoneChainAndDelay::PropagateSegmentParams(UEPtr<NAxoneSegment> seg)
{
 if(!seg)
  return;
 seg->MembraneClassName = MembraneClassName.GetData();
 seg->LTZoneClassName = LTZoneClassName.GetData();
 seg->PosGeneratorClassName = PosGeneratorClassName.GetData();
}

bool NAxoneChainAndDelay::SetNumNodes(const int &value)
{
 if(value < 1)
  return false;
 Ready=false;
 return true;
}

bool NAxoneChainAndDelay::SetInternodeDelayTime(const double &value)
{
 if(value <= 0.0)
  return false;
 Ready=false;
 const int n = NumNodes.GetData();
 for(int i=1;i<n;++i)
 {
  UEPtr<NAxoneDelay> d =
   dynamic_pointer_cast<NAxoneDelay>(GetComponent(DelayName(i), true));
  if(d)
   d->DelayTime = value;
 }
 return true;
}

bool NAxoneChainAndDelay::SetMembraneClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 const int n = NumNodes.GetData();
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(seg)
   seg->MembraneClassName = value;
 }
 return true;
}

bool NAxoneChainAndDelay::SetLTZoneClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 const int n = NumNodes.GetData();
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(seg)
   seg->LTZoneClassName = value;
 }
 return true;
}

bool NAxoneChainAndDelay::SetPosGeneratorClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 const int n = NumNodes.GetData();
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(seg)
   seg->PosGeneratorClassName = value;
 }
 return true;
}

NAxoneChainAndDelay* NAxoneChainAndDelay::New(void)
{
 return new NAxoneChainAndDelay;
}

bool NAxoneChainAndDelay::CheckComponentType(UEPtr<UContainer> comp) const
{
 if(dynamic_pointer_cast<NAxoneSegment>(comp))
  return true;
 if(dynamic_pointer_cast<NAxoneDelay>(comp))
  return true;
 return false;
}

bool NAxoneChainAndDelay::ADefault(void)
{
 NumNodes=1;
 InternodeDelayTime=0.001;
 MembraneClassName="NPMembraneBio";
 LTZoneClassName="NPulseLTZoneThreshold";
 PosGeneratorClassName="NPNeuronPosCGenerator";
 return NAxoneCommon::ADefault();
}

bool NAxoneChainAndDelay::ABuild(void)
{
 return BuildStructure();
}

bool NAxoneChainAndDelay::BuildStructure(void)
{
 bool res=true;
 const int n = NumNodes.GetData();
 if(n < 1)
  return false;

 for(int i=1;i<=n;++i)
 {
  const std::string seg_name = SegmentName(i);
  UEPtr<NAxoneSegment> seg =
   AddMissingComponent<NAxoneSegment>(seg_name, "NAxoneSegment");
  if(!seg)
  {
   LogMessage(RDK_EX_WARNING,
    std::string("NAxoneChainAndDelay::BuildStructure failed to create ")+seg_name);
   return false;
  }
  PropagateSegmentParams(seg);
  // Equal gaps: [Segment ~18][gap 2][Delay ~4][gap 2][Segment ...]
  const double seg_w = 18.0;
  const double del_w = 4.0;
  const double gap = 2.0;
  const double period = seg_w + gap + del_w + gap;
  seg->SetCoord(MVector<double,3>((i - 1) * period, 0.0, 0.0));

  if(i < n)
  {
   const std::string d_name = DelayName(i);
   UEPtr<NAxoneDelay> d =
    AddMissingComponent<NAxoneDelay>(d_name, "NAxoneDelay");
   if(!d)
   {
    LogMessage(RDK_EX_WARNING,
     std::string("NAxoneChainAndDelay::BuildStructure failed to create ")+d_name);
    return false;
   }
   d->DelayTime = InternodeDelayTime.GetData();
   d->SetCoord(MVector<double,3>((i - 1) * period + seg_w + gap, 4.5, 0.0));
  }
 }

 for(int i=n+1;;++i)
 {
  if(!GetComponent(SegmentName(i), true))
   break;
  DelComponent(SegmentName(i));
 }
 for(int i=n;;++i)
 {
  if(!GetComponent(DelayName(i), true))
   break;
  DelComponent(DelayName(i));
 }

 for(int i=1;i<n;++i)
 {
  UEPtr<NAxoneSegment> seg_a =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  UEPtr<NAxoneDelay> d =
   dynamic_pointer_cast<NAxoneDelay>(GetComponent(DelayName(i), true));
  UEPtr<NAxoneSegment> seg_b =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i+1), true));
  if(!seg_a || !d || !seg_b)
   return false;
  res &= CreateLink(seg_a->GetLongName(this), "Output",
                    d->GetLongName(this), "Input");
  res &= CreateLink(d->GetLongName(this), "Output",
                    seg_b->GetLongName(this), "Input");
 }
 return res;
}

bool NAxoneChainAndDelay::AReset(void)
{
 return NAxoneCommon::AReset();
}

bool NAxoneChainAndDelay::Calculate(void)
{
 UEPtr<NAxoneSegment> first =
  dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(1), true));
 if(first)
  *first->Input = *Input;
 return NAxoneCommon::Calculate();
}

bool NAxoneChainAndDelay::ACalculate(void)
{
 const int n = NumNodes.GetData();
 UEPtr<NAxoneSegment> last =
  dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(n), true));
 if(last)
  *Output = *last->Output;
 else
  Output.ToZero();
 return true;
}

}
#endif
