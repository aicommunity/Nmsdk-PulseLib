#ifndef RDK_LLM_PACK_PULSE_DOCS_H
#define RDK_LLM_PACK_PULSE_DOCS_H

#include "../../../Rdk/LLM/Core/Packs/ILLMCapabilityPack.h"

namespace RDK::LLM {

/// Library pack adapter (TD-171): Pulse docs / class discovery hints.
class UPackPulseDocs : public ILLMCapabilityPack {
public:
    const char* id() const override { return "pulse_docs"; }
    PackMatch match(const PackTurnSnapshot& snap) const override;
    PackHintContribution hints(const PackTurnSnapshot& snap) const override;
};

} // namespace RDK::LLM

#endif
