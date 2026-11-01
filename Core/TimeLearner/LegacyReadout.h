#ifndef NMSDK_TIMELEARNER_LEGACY_READOUT_H
#define NMSDK_TIMELEARNER_LEGACY_READOUT_H

#include <stdexcept>
#include <limits>

namespace NMSDK { namespace TimeLearner {

constexpr const char* LegacyReadoutShapeFailure="legacy_readout_shape_requires_build";

// These are learner observations, not physical membrane parameters or a
// dynamic checkpoint. Prepare them at Build, including full-graph mode0.
template<class Learner> bool LegacyReadoutsReady(Learner& owner)
{
 const int count=owner.NumInputDendrite.GetData();
 if(count<1 || count==std::numeric_limits<int>::max())return false;
 const int rows=count+1;
 return owner.DendriteNeuronAmplitude.GetRows()==rows &&
  owner.DendriteNeuronAmplitude.GetCols()==1 && owner.SomaNeuronAmplitude.GetRows()==rows &&
  owner.SomaNeuronAmplitude.GetCols()==1;
}

template<class Learner> void PrepareLegacyReadouts(Learner& owner)
{
 const int count=owner.NumInputDendrite.GetData();
 if(count<1)throw std::invalid_argument("legacy_input_count_must_be_positive");
 if(count==std::numeric_limits<int>::max())throw std::invalid_argument("legacy_readout_rows_not_representable");
 if(owner.DendriteNeuronAmplitude.GetRows()!=count+1 || owner.DendriteNeuronAmplitude.GetCols()!=1)
  owner.DendriteNeuronAmplitude.Assign(count+1,1,0.0);
 if(owner.SomaNeuronAmplitude.GetRows()!=count+1 || owner.SomaNeuronAmplitude.GetCols()!=1)
  owner.SomaNeuronAmplitude.Assign(count+1,1,0.0);
}

}}
#endif
