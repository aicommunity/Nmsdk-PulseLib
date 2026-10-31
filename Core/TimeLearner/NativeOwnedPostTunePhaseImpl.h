#pragma once
#include "NativeOwnedPostTunePhase.h"
#include "NativePostTuneTransaction.h"
#include <set>

namespace NMSDK::TimeLearner {
template<class Owner> class NativeOwnedPostTunePhaseImpl final : public NativeOwnedPostTunePhase {
public:
    NativeOwnedPostTunePhaseImpl(Owner& owner,NativePostTuneTransactionSpec spec,const std::string& implementation):
        transaction(owner,std::move(spec),implementation),frequency(owner.GetTimeStep()){}
    void BeforeTick(double time) override {
        if(Done())return;
        if(!Detail::Finite(time) || (!times.isEmpty() && time<=times.last().toDouble())) {
            Interrupt("owned_post_tune_nonmonotonic_tick");return;
        }
        times.append(time);transaction.Advance();
    }
    bool Done() const override {return transaction.Done();}
    void Cancel(const std::string& reason) override {transaction.Cancel(reason);}
    void Interrupt(const std::string& reason) override {transaction.InterruptOwnerLifecycle(reason);}
    void InternalLifecycle(const std::string& reason) override {
        internal[QString::fromStdString(reason)]=internal[QString::fromStdString(reason)].toInt()+1;
    }
    QJsonObject Report() const override {return transaction.Report();}
    QJsonObject Execution() const override {
        return {{"kind","native_owned_post_tune_execution"},{"schema_version",1},
            {"advance_phase","owner_before_children"},{"frequency_hz",double(frequency)},
            {"advance_times",times},{"internal_lifecycle",internal},{"done",Done()},
            {"dynamic_checkpoint",false},{"evaluation_verdict",QJsonValue(QJsonValue::Null)}};
    }
private:
    NativePostTuneTransaction<Owner> transaction;
    unsigned frequency;
    QJsonArray times;
    QJsonObject internal;
};

template<class Owner> void ValidateNativeOwnedPhaseFrequency(Owner& owner)
{
    const auto frequency=owner.GetTimeStep();std::set<RDK::UContainer*> seen;
    const auto visit=[&](auto&& self,RDK::UContainer* node)->void {
        if(!node || !seen.insert(node).second || !frequency || node->GetTimeStep()!=frequency)
            throw std::invalid_argument("native_owned_post_tune_requires_uniform_frequency_tree");
        std::vector<RDK::NameT> names;node->GetComponentsList(names);
        for(const auto& name:names)self(self,node->GetComponent(name,true).Get());
    };
    visit(visit,&owner);
}
}
