#ifndef NSYNAPSE_TRAINER_STDP_H
#define NSYNAPSE_TRAINER_STDP_H

#include "../../Nmsdk-PulseLib/Core/NSynapseTrainer.h"

/// NSynapseTrainerStdp
/// NSynapseTrainerStdpTD
/// NSynapseTrainerStdpWD
/// NSynapseTrainerStdpLobov
/// NSynapseTrainerStdpClassicDiscrete
/// NSynapseTrainerStdpClassicIntegrated
/// NSynapseTrainerStdpTriplet
/// NSynapseTrainerStdpMirror
/// NSynapseTrainerStdpProbabilistic
/// NSynapseTrainerStdpStable



namespace NMSDK {

///  STDP
class NSynapseTrainerStdp: public NSynapseTrainer
{
public:

UProperty<double,NSynapseTrainerStdp, ptPubParameter> APlus;

UProperty<double,NSynapseTrainerStdp, ptPubParameter> AMinus;

UProperty<double,NSynapseTrainerStdp, ptPubParameter> WMin;

UProperty<double,NSynapseTrainerStdp, ptPubParameter> WMax;

UProperty<bool,NSynapseTrainerStdp, ptPubState> IsTrainEnable;

public: //  
UProperty<double,NSynapseTrainerStdp, ptPubState> TPre;

UProperty<double,NSynapseTrainerStdp, ptPubState> TPost;

UProperty<double,NSynapseTrainerStdp, ptPubState> TPreOld;

UProperty<double,NSynapseTrainerStdp, ptPubState> TPostOld;

UProperty<double,NSynapseTrainerStdp, ptPubState> TDiff;

UProperty<double,NSynapseTrainerStdp, ptPubState> XYDiff;

UProperty<bool,NSynapseTrainerStdp, ptPubState> IsInputPulseActive;

UProperty<bool,NSynapseTrainerStdp, ptPubState> IsOutputPulseActive;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdp(void);
virtual ~NSynapseTrainerStdp(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdp* New(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetAPlus(const double &value);

bool SetAMinus(const double &value);

bool SetWMin(const double &value);

bool SetWMax(const double &value);
// --------------------------

bool WriteIntoFile(double deltaT, double deltaW);

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

///STDP,    
class NSynapseTrainerStdpTD: public NSynapseTrainerStdp
{
public:

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauX;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauY;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauMinus;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> TauPlus;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> XAvg;

UProperty<double,NSynapseTrainerStdpTD, ptPubParameter> YAvg;

UProperty<double,NSynapseTrainerStdpTD, ptPubState> WRange;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpTD(void);
virtual ~NSynapseTrainerStdpTD(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpTD* New(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetTauX(const double &value);

bool SetTauY(const double &value);

bool SetTauMinus(const double &value);

bool SetTauPlus(const double &value);

bool SetXAvg(const double &value);

bool SetYAvg(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

///STDP,    
class NSynapseTrainerStdpWD: public NSynapseTrainerStdp
{
public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpWD(void);
virtual ~NSynapseTrainerStdpWD(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpWD* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

/// STDP -  ( ,  )
class NSynapseTrainerStdpLobov: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> XModCoeff;

UProperty<double,NSynapseTrainerStdpLobov, ptPubParameter> YModCoeff;

UProperty<double,NSynapseTrainerStdpLobov,ptPubParameter> StartWeight;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpLobov(void);
virtual ~NSynapseTrainerStdpLobov(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpLobov* New(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetXModCoeff(const double &value);

bool SetYModCoeff(const double &value);

bool SetStartWeight(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};


///STDP - ,  
class NSynapseTrainerStdpClassicDiscrete: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpClassicDiscrete, ptPubState> Mu;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpClassicDiscrete(void);
virtual ~NSynapseTrainerStdpClassicDiscrete(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpClassicDiscrete* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

///STDP - , 
class NSynapseTrainerStdpClassicIntegrated: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuPlus;
UProperty<double,NSynapseTrainerStdpClassicIntegrated, ptPubState> MuMinus;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpClassicIntegrated(void);
virtual ~NSynapseTrainerStdpClassicIntegrated(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpClassicIntegrated* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};


///STDP Triplet
class NSynapseTrainerStdpTriplet: public NSynapseTrainerStdpTD
{
public:

UProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> APlus3;

UProperty<double,NSynapseTrainerStdpTriplet, ptPubParameter> AMinus3;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpTriplet(void);
virtual ~NSynapseTrainerStdpTriplet(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpTriplet* New(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetAPlus3(const double &value);

bool SetAMinus3(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};


///STDP Mirror
class NSynapseTrainerStdpMirror: public NSynapseTrainerStdpTD
{
public:
UProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> APlus3;

UProperty<double,NSynapseTrainerStdpMirror, ptPubParameter> AMinus3;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> o1;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> o2;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> r1;

UProperty<double,NSynapseTrainerStdpMirror, ptPubState> r2;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpMirror(void);
virtual ~NSynapseTrainerStdpMirror(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpMirror* New(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetAPlus3(const double &value);

bool SetAMinus3(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};


/// STDP
class NSynapseTrainerStdpProbabilistic: public NSynapseTrainerStdpWD
{
public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpProbabilistic(void);
virtual ~NSynapseTrainerStdpProbabilistic(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpProbabilistic* New(void);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

/// STDP
class NSynapseTrainerStdpStable: public NSynapseTrainerStdpWD
{
public:
UProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTP;

UProperty<double,NSynapseTrainerStdpStable, ptPubParameter> TauLTD;

public: // 
// --------------------------
//   
// --------------------------
NSynapseTrainerStdpStable(void);
virtual ~NSynapseTrainerStdpStable(void);
// --------------------------

// --------------------------
//    
// --------------------------
//         
virtual NSynapseTrainerStdpStable* New(void);
// --------------------------

protected:
// --------------------------
//    
// --------------------------
bool SetTauLTP(const double &value);

bool SetTauLTD(const double &value);
// --------------------------

// --------------------------
//    
// --------------------------
protected:
//        
virtual bool ADefault(void);

//     
virtual bool ABuild(void);

//      
virtual bool AReset(void);

//    
virtual bool ACalculate(void);
// --------------------------
};

}

#endif

