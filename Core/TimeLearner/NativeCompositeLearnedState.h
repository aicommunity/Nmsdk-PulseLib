#pragma once
#include "NativeLearnedState.h"
#include "CompositePortableTrainingProof.h"
#include "CandidateWindowPortableTrainingProof.h"

namespace NMSDK::TimeLearner {
bool RequiresCandidateWindowRecoveryBoundary(const NativeTrainingService&);
// Completed/stopped data boundary. Physical native import and full readback
// precede installation; live owner authority is created only by a later Start.
QJsonObject EncodeNativeCompositeTrainingState(const NativeTrainingService&,const std::string& actualModelRevision,
    const std::vector<InputParameters>& actualParameters);
QJsonObject EncodeNativeCandidateWindowTrainingState(const NativeTrainingService&,const std::string& actualModelRevision,
    const std::vector<InputParameters>& actualParameters);
// Summary8 retains supplied recovery delivery DATA; not a controller checkpoint.
QJsonObject EncodeNativeCandidateWindowRecoveryTrainingState(const NativeTrainingService&,const std::string& actualModelRevision,
    const std::vector<InputParameters>& actualParameters);
class NativeCompositeStateFactory {
public:
    static QJsonObject Export(RDK::UNet&,const NativeModelSpec&,const NativeTrainingService&,
        const std::vector<InputParameters>&,int controllerVersion,int calculateMode,int structureBuildMode,
        const std::string& implementationId,const NativeCapacitanceCorrection*);
    static std::unique_ptr<PreparedNativeLearnedState> Prepare(const QJsonObject&,
        RDK::UEPtr<RDK::UStorage>,RDK::UEPtr<RDK::UEnvironment>,const std::string& implementationId);
private:
    static void Restore(NativeTrainingService&,CompositePortableTrainingProof,NativeTrainingPlan,
        QJsonObject initialConfiguration,std::size_t sessions,TrainingStatus,const std::string&,bool completed);
};
// Explicit package6/protocol5 path. Closed DATA and physical import precede
// installation; neither this factory nor package bytes restore live authority.
class NativeCandidateWindowStateFactory {
public:
    static QJsonObject Export(RDK::UNet&,const NativeModelSpec&,const NativeTrainingService&,
        const std::vector<InputParameters>&,int controllerVersion,int calculateMode,int structureBuildMode,
        const std::string& implementationId,const NativeCapacitanceCorrection*);
    static std::unique_ptr<PreparedNativeLearnedState> Prepare(const QJsonObject&,
        RDK::UEPtr<RDK::UStorage>,RDK::UEPtr<RDK::UEnvironment>,const std::string& implementationId);
    // Explicit package7 boundary. Legacy Export/Prepare remain package6-only.
    static QJsonObject ExportRecovery(RDK::UNet&,const NativeModelSpec&,const NativeTrainingService&,
        const std::vector<InputParameters>&,int controllerVersion,int calculateMode,int structureBuildMode,
        const std::string& implementationId,const NativeCapacitanceCorrection*);
    static NativeTrainingExport ExportBoundary(RDK::UNet&,const NativeModelSpec&,const NativeTrainingService&,
        const std::vector<InputParameters>&,int controllerVersion,int calculateMode,int structureBuildMode,
        const std::string& implementationId,const NativeCapacitanceCorrection*);
    static std::unique_ptr<PreparedNativeLearnedState> PrepareRecovery(const QJsonObject&,
        RDK::UEPtr<RDK::UStorage>,RDK::UEPtr<RDK::UEnvironment>,const std::string& implementationId);
private:
    static QJsonObject ExportImpl(RDK::UNet&,const NativeModelSpec&,const NativeTrainingService&,
        const std::vector<InputParameters>&,int,int,int,const std::string&,const NativeCapacitanceCorrection*,bool recovery,QJsonObject* trainingSummary=nullptr);
    static std::unique_ptr<PreparedNativeLearnedState> PrepareImpl(const QJsonObject&,
        RDK::UEPtr<RDK::UStorage>,RDK::UEPtr<RDK::UEnvironment>,const std::string&,bool recovery);
    static void Restore(NativeTrainingService&,CandidateWindowPortableTrainingProof,NativeTrainingPlan,
        QJsonObject initialConfiguration,std::size_t sessions,TrainingStatus,const std::string&,bool completed);
};
}
