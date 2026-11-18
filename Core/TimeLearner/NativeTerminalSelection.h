#pragma once
#include "NativeModelBuilder.h"
#include <cmath>
#include <functional>
#include <stdexcept>

namespace NMSDK::TimeLearner {
// Explicit native-plan capability. Cold JSON configuration does not yet enable
// it: portable terminal-transition export/import must be bound separately.
struct NativeTerminalSelectionPolicy {
    double maximumWallSeconds=30.;
    static constexpr std::size_t MaximumTransactions=2; // commit plus reserved rollback
};
inline void ValidateNativeTerminalSelectionPolicy(const NativeTerminalSelectionPolicy& policy)
{
    if(!std::isfinite(policy.maximumWallSeconds) || policy.maximumWallSeconds<=0.)
        throw std::invalid_argument("invalid_native_terminal_selection_budget");
}
// Process-local, short-lived authority minted by the stopped service. Neither
// a retained snapshot nor decoded DATA can construct a permit.
class NativeTerminalSelectionPermit {
public:
    NativeTerminalSelectionPermit(const NativeTerminalSelectionPermit&)=delete;
    void RequireScope() const {check();}
    void RequireCurrent() const {check();budget();}
private:
    friend class NativeTrainingService;
    NativeTerminalSelectionPermit(std::function<void()> value,std::function<void()> limit):check(std::move(value)),budget(std::move(limit)){}
    std::function<void()> check,budget;
};
struct NativeTerminalGraphResult {
    NativeModelBuildResult model;
    std::size_t transactions=0;
    bool rollbackAttempted=false,rollbackSucceeded=false;
};
}
