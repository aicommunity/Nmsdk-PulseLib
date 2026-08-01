#include "UPackPulseDocs.h"

#include <algorithm>
#include <cctype>

namespace RDK::LLM {

namespace {

std::string toLowerAscii(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool looksLikePulseDomain(const std::string& text)
{
    const std::string lower = toLowerAscii(text);
    return lower.find("pulse") != std::string::npos || lower.find("nspneuron") != std::string::npos
           || lower.find("npulseneuron") != std::string::npos || lower.find("stdp") != std::string::npos
           || lower.find("synapse") != std::string::npos || lower.find("snn") != std::string::npos
           || text.find("синапс") != std::string::npos;
}

} // namespace

PackMatch UPackPulseDocs::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    if(!looksLikePulseDomain(snap.req->user_text) && !looksLikePulseDomain(snap.planning_text))
        return m;
    m.score = 0.55f;
    m.reason = "pulse_domain_hint";
    return m;
}

PackHintContribution UPackPulseDocs::hints(const PackTurnSnapshot& snap) const
{
    (void)snap;
    PackHintContribution h;
    h.ephemeral_markdown =
        "## Pulse library\n"
        "For SNN / Pulse neuron / STDP questions, prefer `search_pulse_docs` and "
        "`list_pulse_component_classes` before inventing class names.";
    h.extra_tool_names = {"search_pulse_docs", "list_pulse_component_classes", "describe_class",
                          "search_project_docs"};
    h.act_or_clarify_recovery_tools = {"search_pulse_docs", "list_pulse_component_classes",
                                       "ask_user"};
    return h;
}

} // namespace RDK::LLM
