#pragma once
#include <QJsonObject>
#include <rdk.h>
#include <memory>
#include "ModelBoundary.h"

namespace NmsdkTools {
// Version-1 length-framed UTF8/binary64 encoding shared by identity and archive.
RDK_LIB_TYPE QString CanonicalModelDataSha256(const QJsonValue& data);
// Read-only semantic identity of a complete, ready graph. Version 1 is closed;
// version 2 records explicitly declared external matrix-double stimulus ports.
// Dynamic state, names/IDs of the root and presentation/diagnostic settings are
// excluded. Unsupported parameter types, unused ports or undeclared bindings invalidate it.
// The code/runtime identity and controller certificate remain separate records.
RDK_LIB_TYPE QJsonObject IdentifyModel(RDK::UEPtr<RDK::UContainer> model,
                         const ModelBoundarySources& boundarySources = {});
// Process-local read-only accelerator for the SAME semantic identity. Every
// Read checks actual typed parameters, bindings, named membership, child order,
// aliases, frequency, lifecycle and native caches. A change or forceFull uses
// IdentifyModel; no own-edit counter or rounded textual value is trusted.
// Callers must supply a live root and live declared ports, as for IdentifyModel.
// Single scheduler thread only; not a checkpoint and not a dynamic-state hash.
class RDK_LIB_TYPE ModelRevisionObserver {
public:
    ModelRevisionObserver();
    ~ModelRevisionObserver();
    ModelRevisionObserver(const ModelRevisionObserver&)=delete;
    ModelRevisionObserver& operator=(const ModelRevisionObserver&)=delete;
    QString Read(RDK::UEPtr<RDK::UContainer> model,const ModelBoundarySources& ports={},bool forceFull=false);
    void Clear();
    std::size_t FullReads() const;
    std::size_t CacheHits() const;
private:
    struct Impl;
    std::unique_ptr<Impl> implementation;
};
// Validate the archived identity itself, then compare against an independently
// captured native model. Never trust a stored hash without checking its data.
RDK_LIB_TYPE QJsonObject VerifyModelIdentity(const QJsonObject& expected, RDK::UEPtr<RDK::UContainer> model,
                               const ModelBoundarySources& boundarySources = {});
}
