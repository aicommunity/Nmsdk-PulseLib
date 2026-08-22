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

#ifndef NAXONE_CHAIN_H
#define NAXONE_CHAIN_H

#include "NAxoneSegment.h"

namespace NMSDK {

/// Structural axon: chain of NAxoneSegment. Length = NumSegments.
class RDK_LIB_TYPE NAxoneChain: public NAxoneCommon
{
public:
/// Number of regenerative segments (>=1)
UProperty<int,NAxoneChain, ptPubParameter> NumSegments;

/// Propagated to each Segment*
UProperty<std::string,NAxoneChain, ptPubParameter> MembraneClassName;

/// Propagated to each Segment*
UProperty<std::string,NAxoneChain, ptPubParameter> LTZoneClassName;

/// Propagated to each Segment*
UProperty<std::string,NAxoneChain, ptPubParameter> PosGeneratorClassName;

public:
NAxoneChain(void);
virtual ~NAxoneChain(void);

protected:
bool SetNumSegments(const int &value);
bool SetMembraneClassName(const std::string &value);
bool SetLTZoneClassName(const std::string &value);
bool SetPosGeneratorClassName(const std::string &value);

public:
/// Append one segment (structural elongation)
NAxoneSegment* Elongate(void);
/// Remove last segment if NumSegments > 1
bool Shorten(void);

virtual NAxoneChain* New(void);
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
virtual bool Calculate(void);

protected:
virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);

bool BuildStructure(void);
void PropagateSegmentParams(UEPtr<NAxoneSegment> seg);
std::string SegmentName(int index_1based) const;

void ApplyDiagramLayout(void);

public:
virtual bool Build(void);
};

}
#endif
