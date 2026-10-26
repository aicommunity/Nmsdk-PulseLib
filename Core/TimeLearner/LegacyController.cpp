#include "LegacyController.h"
#include <algorithm>
#include <cmath>

namespace NMSDK::TimeLearner::Legacy {
double ChooseDelay(double modelDelay, double measuredDelay, bool peakValid,
                   double measuredPeak, double expectedPulse, double syncTolerance,
                   double agreementMarginMinimum)
{
    const double margin=std::max(syncTolerance,agreementMarginMinimum);
    const double modelPeak=expectedPulse+modelDelay;
    const bool agrees=peakValid && (std::fabs(measuredDelay-modelDelay)<=margin)
        && (std::fabs(measuredPeak-modelPeak)<=margin);
    return agrees ? measuredDelay : modelDelay;
}

ResistanceDecision ProposeResistance(double oldResistance, double amplitude,
    double reference, double error, double configuredGain,
    std::optional<double> previousError, const ResistancePolicy& policy)
{
    double gain=configuredGain;
    if(gain<=0.) gain=policy.defaultGain;
    if(oldResistance<=0. || reference<=0. || amplitude<=policy.minimumMeasurableAmplitude)
        return {oldResistance,gain,false};
    const double ratio=amplitude/reference;
    if(previousError && std::fabs(*previousError)>1e-12)
    {
        if(*previousError*error<0.) gain*=policy.overshootGainFactor;
        else if(std::fabs(*previousError)>policy.undershootBoostRatio*std::fabs(error) && *previousError*error>0.)
            gain*=policy.undershootGainFactor;
    }
    gain=std::max(0.05,std::min(1.,gain));
    double stepRatio=1.+gain*(ratio-1.);
    if(stepRatio<=0.) stepRatio=0.05;
    return {oldResistance*stepRatio,gain,true};
}

void ComputePeakTimes(int count, double firstImpulse,
    const std::vector<bool>& valid, const std::vector<double>& absolutePeaks,
    const std::vector<double>& relativePulses, std::vector<double>& relativePeaks,
    std::vector<double>& delays)
{
    if(int(relativePeaks.size())!=count) relativePeaks.assign(static_cast<size_t>(count),0.);
    if(int(delays.size())!=count) delays.assign(static_cast<size_t>(count),0.);
    for(int i=0;i<count;++i)
    {
        if(i>=int(valid.size()) || !valid[i] || i>=int(absolutePeaks.size()))
        { relativePeaks[static_cast<size_t>(i)]=0.; delays[static_cast<size_t>(i)]=0.; continue; }
        const double pulse=i<int(relativePulses.size()) ? relativePulses[static_cast<size_t>(i)] : 0.;
        relativePeaks[static_cast<size_t>(i)]=absolutePeaks[static_cast<size_t>(i)]-firstImpulse;
        delays[static_cast<size_t>(i)]=absolutePeaks[static_cast<size_t>(i)]-(firstImpulse+pulse);
    }
}

int DetectPulses(int counter, double level, int& previousCounter, double& previousLevel)
{
    int count=0;
    if(counter>previousCounter) { count=counter-previousCounter; previousCounter=counter; }
    else if(level>0. && previousLevel<=0.) count=1;
    previousLevel=level;
    return count>0 ? count : 0;
}
}
