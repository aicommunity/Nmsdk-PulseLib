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

#ifndef NAXONE_SEGMENT_H
#define NAXONE_SEGMENT_H

#include "NAxoneCommon.h"
#include "NAperiodicLink.h"
#include "NConstGenerator.h"
#include "NPulseMembrane.h"
#include "NPulseLTZoneCommon.h"

namespace NMSDK {

/// Regenerative axon segment: Inertial (Bias=-1) + Soma + PosGenerator(+1) + LTZone feedback.
class RDK_LIB_TYPE NAxoneSegment: public NAxoneCommon
{
public:
/// Soma membrane class (default NPMembraneBio)
UProperty<std::string,NAxoneSegment, ptPubParameter> MembraneClassName;

/// LTZone class (default NPulseLTZoneThreshold)
UProperty<std::string,NAxoneSegment, ptPubParameter> LTZoneClassName;

/// +1 constant generator class (default NPNeuronPosCGenerator)
UProperty<std::string,NAxoneSegment, ptPubParameter> PosGeneratorClassName;

public:
UEPtr<NAperiodicLink> Inertial;
UEPtr<NPulseMembrane> Soma;
UEPtr<NLTZone> LTZone;
UEPtr<NConstGenerator> PosGenerator;

public:
NAxoneSegment(void);
virtual ~NAxoneSegment(void);

protected:
bool SetMembraneClassName(const std::string &value);
bool SetLTZoneClassName(const std::string &value);
bool SetPosGeneratorClassName(const std::string &value);

public:
virtual NAxoneSegment* New(void);
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
virtual bool Calculate(void);

protected:
virtual bool ADefault(void);
virtual bool ABuild(void);
virtual bool AReset(void);
virtual bool ACalculate(void);

/// Inertial + PosGenerator + Soma + LTZone via AddMissingComponent
bool BuildStructure(void);

public:
/// Re-apply diagram coords/threshold (also when Ready skips full Build).
void ApplyDiagramLayout(void);

public:
virtual bool Build(void);
};

}
#endif
