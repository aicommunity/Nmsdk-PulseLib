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

#ifndef NAXONE_CHAIN_CPP
#define NAXONE_CHAIN_CPP

#include "NAxoneChain.h"
#include <sstream>

namespace NMSDK {

NAxoneChain::NAxoneChain(void)
 : NumSegments("NumSegments",this,&NAxoneChain::SetNumSegments),
   MembraneClassName("MembraneClassName",this,&NAxoneChain::SetMembraneClassName),
   LTZoneClassName("LTZoneClassName",this,&NAxoneChain::SetLTZoneClassName),
   PosGeneratorClassName("PosGeneratorClassName",this,&NAxoneChain::SetPosGeneratorClassName)
{
}

NAxoneChain::~NAxoneChain(void)
{
}

std::string NAxoneChain::SegmentName(int index_1based) const
{
 std::ostringstream os;
 os << "Segment" << index_1based;
 return os.str();
}

void NAxoneChain::PropagateSegmentParams(UEPtr<NAxoneSegment> seg)
{
 if(!seg)
  return;
 seg->MembraneClassName = MembraneClassName.GetData();
 seg->LTZoneClassName = LTZoneClassName.GetData();
 seg->PosGeneratorClassName = PosGeneratorClassName.GetData();
}

bool NAxoneChain::SetNumSegments(const int &value)
{
 if(value < 1)
  return false;
 Ready=false;
 return true;
}

bool NAxoneChain::SetMembraneClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 const int n = NumSegments.GetData();
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(seg)
   seg->MembraneClassName = value;
 }
 return true;
}

bool NAxoneChain::SetLTZoneClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 const int n = NumSegments.GetData();
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(seg)
   seg->LTZoneClassName = value;
 }
 return true;
}

bool NAxoneChain::SetPosGeneratorClassName(const std::string &value)
{
 if(value.empty())
  return false;
 Ready=false;
 const int n = NumSegments.GetData();
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(seg)
   seg->PosGeneratorClassName = value;
 }
 return true;
}

NAxoneSegment* NAxoneChain::Elongate(void)
{
 NumSegments = NumSegments.GetData() + 1;
 Ready=false;
 if(!Build())
 {
  NumSegments = NumSegments.GetData() - 1;
  return 0;
 }
 UEPtr<NAxoneSegment> seg =
  dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(NumSegments.GetData()), true));
 return seg;
}

bool NAxoneChain::Shorten(void)
{
 if(NumSegments.GetData() <= 1)
  return false;
 const int last = NumSegments.GetData();
 DelComponent(SegmentName(last));
 NumSegments = last - 1;
 Ready=false;
 Build();
 return true;
}

NAxoneChain* NAxoneChain::New(void)
{
 return new NAxoneChain;
}

bool NAxoneChain::CheckComponentType(UEPtr<UContainer> comp) const
{
 return dynamic_pointer_cast<NAxoneSegment>(comp) != 0;
}

bool NAxoneChain::ADefault(void)
{
 NumSegments=1;
 MembraneClassName="NPMembraneBio";
 LTZoneClassName="NPulseLTZoneThreshold";
 PosGeneratorClassName="NPNeuronPosCGenerator";
 return NAxoneCommon::ADefault();
}

bool NAxoneChain::ABuild(void)
{
 return BuildStructure();
}

bool NAxoneChain::Build(void)
{
 ApplyDiagramLayout();
 return UContainer::Build();
}

void NAxoneChain::ApplyDiagramLayout(void)
{
 const int n = NumSegments.GetData();
 if(n < 1)
  return;
 const double seg_pitch = 18.0;
 for(int i=1;i<=n;++i)
 {
  UEPtr<NAxoneSegment> seg =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  if(!seg)
   continue;
  seg->SetCoord(MVector<double,3>((i - 1) * seg_pitch, 0.0, 0.0));
  seg->ApplyDiagramLayout();
 }
}

bool NAxoneChain::BuildStructure(void)
{
 bool res=true;
 const int n = NumSegments.GetData();
 if(n < 1)
  return false;

 for(int i=1;i<=n;++i)
 {
  const std::string name = SegmentName(i);
  UEPtr<NAxoneSegment> seg =
   AddMissingComponent<NAxoneSegment>(name, "NAxoneSegment");
  if(!seg)
  {
   LogMessage(RDK_EX_WARNING,
    std::string("NAxoneChain::BuildStructure failed to create ")+name);
   return false;
  }
  PropagateSegmentParams(seg);
  // UContainer::Build walks existing children before ABuild; segments
  // created here must Build() explicitly (same pattern as Soma in NAxoneSegment).
  if(!seg->Build())
  {
   LogMessage(RDK_EX_WARNING,
    std::string("NAxoneChain::BuildStructure: ")+name+std::string("->Build failed"));
   return false;
  }
 }

 ApplyDiagramLayout();

 for(int i=n+1;;++i)
 {
  UEPtr<UContainer> extra = GetComponent(SegmentName(i), true);
  if(!extra)
   break;
  DelComponent(SegmentName(i));
 }

 for(int i=1;i<n;++i)
 {
  UEPtr<NAxoneSegment> a =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i), true));
  UEPtr<NAxoneSegment> b =
   dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(i+1), true));
  if(!a || !b)
   return false;
  res &= CreateLink(a->GetLongName(this), "Output",
                    b->GetLongName(this), "Input");
 }
 return res;
}

bool NAxoneChain::AReset(void)
{
 ApplyDiagramLayout();
 return NAxoneCommon::AReset();
}

bool NAxoneChain::Calculate(void)
{
 UEPtr<NAxoneSegment> first =
  dynamic_pointer_cast<NAxoneSegment>(GetComponent(SegmentName(1), true));
 if(first)
  *first->Input = *Input;
 return NAxoneCommon::Calculate();
}

bool NAxoneChain::ACalculate(void)
{
 const int n = NumSegments.GetData();
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
