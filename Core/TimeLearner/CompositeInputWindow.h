#pragma once
namespace NMSDK::TimeLearner {
struct CompositeInputWindow {
    double beginOffset=0.,endOffset=0.;
    bool operator==(const CompositeInputWindow&) const=default;
};
}
