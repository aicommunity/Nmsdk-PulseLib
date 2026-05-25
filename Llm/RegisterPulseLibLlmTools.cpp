#include "RegisterPulseLibLlmTools.h"

#include <filesystem>

#include "../../../Rdk/LLM/Core/Context/ILLMProjectContextProvider.h"
#include "../../../Rdk/LLM/Core/Context/UDocSearchIndex.h"
#include "../../../Rdk/LLM/Core/Tools/ULLMToolRegistry.h"

namespace fs = std::filesystem;

namespace {

RDK::LLM::LLMToolDefinition makeDef(const std::string& name, const std::string& desc,
                                    nlohmann::json input)
{
    RDK::LLM::LLMToolDefinition d;
    d.name = name;
    d.kind = RDK::LLM::LLMToolKind::Read;
    d.description = desc;
    d.input_schema = std::move(input);
    d.output_schema = {{"type", "object"}};
    return d;
}

fs::path pulseDocsRoot(RDK::LLM::ILLMProjectContextProvider* ctx)
{
    if(!ctx)
        return {};
    return ctx->paths().repository_root / "Libraries" / "Nmsdk-PulseLib" / "Docs";
}

} // namespace

void RegisterPulseLibLlmTools(RDK::LLM::ULLMToolRegistry& registry,
                              RDK::LLM::ILLMProjectContextProvider* project_context)
{
    registry.registerTool(
        makeDef("search_pulse_docs",
                "Search Nmsdk-PulseLib documentation (SNN neurons, synapses, trainers)",
                {{"type", "object"},
                 {"required", {"query"}},
                 {"properties",
                  {{"query", {{"type", "string"}}}, {"top_k", {{"type", "integer"}}}}},
                 {"additionalProperties", false}}),
        [project_context](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            RDK::LLM::ToolGatewayResult r;
            const std::string query = args.at("query").get<std::string>();
            const int top_k = args.value("top_k", 5);
            const fs::path root = pulseDocsRoot(project_context);
            std::vector<fs::path> roots;
            if(!root.empty())
                roots.push_back(root);
            auto snippets = RDK::LLM::searchDocsWithIndex(roots, query, top_k);
            r.result["snippets"] = nlohmann::json::array();
            r.result["library"] = "Nmsdk-PulseLib";
            for(const auto& s : snippets)
            {
                r.result["snippets"].push_back(
                    {{"path", s.path}, {"title", s.title}, {"excerpt", s.excerpt}, {"score", s.score}});
            }
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeDef("list_pulse_component_classes",
                "Lists pulse library component class names and short descriptions",
                {{"type", "object"}, {"additionalProperties", false}}),
        [](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            (void)args;
            RDK::LLM::ToolGatewayResult r;
            r.result["classes"] = nlohmann::json::array({
                {{"class_name", "NPulseNeuron"}, {"summary", "Pulse neuron model"}},
                {{"class_name", "NPulseSynapseStdp"}, {"summary", "STDP synapse"}},
                {{"class_name", "NNeuronTrainer"}, {"summary", "Neuron trainer (STDP params)"}},
                {{"class_name", "NNeuronLearner"}, {"summary", "Online learner"}},
                {{"class_name", "NPulseMembrane"}, {"summary", "Membrane dynamics"}},
            });
            r.result["docs_hint"] = "Libraries/Nmsdk-PulseLib/Docs/README.md";
            r.ok = true;
            return r;
        });
}
