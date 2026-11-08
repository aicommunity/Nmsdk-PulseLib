#pragma once
#include "BoundedTraining.h"

namespace NMSDK::TimeLearner::BoundedTrainingDetail {
// Private shared numeric preflight. Protocol/closed-reference admission stays
// with each caller; these checks never grant measurement or live authority.
void ValidateInputShape(std::size_t inputs,const BoundedTrainingConfig&,
    const std::vector<InputParameters>& initial);
void ValidateInputRanges(const BoundedTrainingConfig&);
}
