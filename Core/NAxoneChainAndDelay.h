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

#ifndef NAXONE_CHAIN_AND_DELAY_H
#define NAXONE_CHAIN_AND_DELAY_H

#include "NAxoneSegment.h"
#include "NAxoneDelay.h"

namespace NMSDK {

/// Myelinated-style axon: Segment1 - Delay1 - Segment2 - ... - SegmentN.
class RDK_LIB_TYPE NAxoneChainAndDelay: public NAxoneCommon
{
public:
/// Number of regenerative nodes (>=1)
UProperty<int,NAxoneChainAndDelay, ptPubParameter> NumNodes;

/// Delay between consecutive nodes [s]
UProperty<double,NAxoneChainAndDelay, ptPubParameter> InternodeDelayTime;

/// Propagated to each Segment*
UProperty<std::string,NAxoneChainAndDelay, ptPubParameter> MembraneClassName;

/// Propagated to each Segment*
UProperty<std::string,NAxoneChainAndDelay, ptPubParameter> LTZoneClassName;

/// Propagated to each Segment*
UProperty<std::string,NAxoneChainAndDelay, ptPubParameter> PosGeneratorClassName;

public:
NAxoneChainAndDelay(void);
virtual ~NAxoneChainAndDelay(void);

protected:
bool SetNumNodes(const int &value);
bool SetInternodeDelayTime(const double &value);
bool SetMembraneClassName(const std::string &value);
bool SetLTZoneClassName(const std::string &value);
bool SetPosGeneratorClassName(const std::string &value);

public:
virtual NAxoneChainAndDelay* New(void);
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
std::string DelayName(int index_1based) const;

void ApplyDiagramLayout(void);

public:
virtual bool Build(void);
};

}
#endif
