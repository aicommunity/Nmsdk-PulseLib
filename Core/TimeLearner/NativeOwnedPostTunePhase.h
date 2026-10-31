#pragma once
#include <QJsonObject>
#include <string>

namespace NMSDK::TimeLearner {
struct NativePostTuneTransactionSpec;
// Type-erased owner phase; Report/Execution never dereference the owner.
class NativeOwnedPostTunePhase {
public:
    virtual ~NativeOwnedPostTunePhase()=default;
    virtual void BeforeTick(double modelTime)=0;
    virtual bool Done() const=0;
    virtual void Cancel(const std::string& reason)=0;
    virtual void Interrupt(const std::string& reason)=0;
    virtual void InternalLifecycle(const std::string& reason)=0;
    virtual QJsonObject Report() const=0;
    virtual QJsonObject Execution() const=0;
};
}
