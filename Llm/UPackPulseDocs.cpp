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
           || text.find("синапс") != std::string::npos || text.find("нейрон") != std::string::npos
           || lower.find("neuron") != std::string::npos;
}

bool looksLikeNeuronModelInventory(const std::string& text)
{
    const std::string lower = toLowerAscii(text);
    const bool modelish = text.find("модел") != std::string::npos
                          || lower.find("model") != std::string::npos
                          || text.find("класс") != std::string::npos
                          || lower.find("class") != std::string::npos
                          || text.find("доступн") != std::string::npos
                          || text.find("какие") != std::string::npos
                          || text.find("список") != std::string::npos
                          || lower.find("available") != std::string::npos;
    const bool neuronish = text.find("нейрон") != std::string::npos
                           || lower.find("neuron") != std::string::npos;
    return modelish && neuronish;
}

} // namespace

PackMatch UPackPulseDocs::match(const PackTurnSnapshot& snap) const
{
    PackMatch m;
    if(!snap.req || snap.skip_pre_llm_funnel)
        return m;
    const std::string& user = snap.req->user_text;
    const std::string& plan = snap.planning_text;
    if(!looksLikePulseDomain(user) && !looksLikePulseDomain(plan))
        return m;
    if(looksLikeNeuronModelInventory(user) || looksLikeNeuronModelInventory(plan))
    {
        m.score = 0.9f;
        m.reason = "pulse_neuron_model_inventory";
        return m;
    }
    m.score = 0.55f;
    m.reason = "pulse_domain_hint";
    return m;
}

PackHintContribution UPackPulseDocs::hints(const PackTurnSnapshot& snap) const
{
    PackHintContribution h;
    const bool inventory =
        snap.req
        && (looksLikeNeuronModelInventory(snap.req->user_text)
            || looksLikeNeuronModelInventory(snap.planning_text));
    if(inventory)
    {
        h.ephemeral_markdown =
            "## Pulse neuron model families\n"
            "Canonical families (not a full class dump): **CSNM**, **iaF** / integrate-and-fire, "
            "**Izhikevich**, **cable**. Call `list_pulse_component_classes` for the complete "
            "registered list, then `search_pulse_docs` / cite `doc_uri` markdown links. "
            "Do not write a single-component essay.";
    }
    else
    {
        h.ephemeral_markdown =
            "## Pulse library\n"
            "For SNN / Pulse neuron / STDP questions, prefer `search_pulse_docs` and "
            "`list_pulse_component_classes` before inventing class names.";
    }
    h.extra_tool_names = {"search_pulse_docs", "list_pulse_component_classes", "describe_class",
                          "search_project_docs", "open_documentation", "open_class_docs"};
    h.act_or_clarify_recovery_tools = {"list_pulse_component_classes", "search_pulse_docs",
                                       "list_registered_classes", "open_documentation",
                                       "ask_user"};
    return h;
}

} // namespace RDK::LLM
