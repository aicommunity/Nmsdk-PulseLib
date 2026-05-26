#include "RegisterPulseLibLlmTools.h"

#include <filesystem>
#include <unordered_set>

#include "../../../Rdk/LLM/Core/Context/ILLMProjectContextProvider.h"
#include "../../../Rdk/LLM/Core/Context/UDocSearchIndex.h"
#include "../../../Rdk/LLM/Core/Domain/URdkDomainAccess.h"
#include "../../../Rdk/LLM/Core/Tools/ULLMLibraryScopedWriteTools.h"
#include "../../../Rdk/LLM/Core/Tools/ULLMToolRegistry.h"

namespace fs = std::filesystem;

namespace {

RDK::LLM::LLMToolDefinition makeReadDef(const std::string& name, const std::string& desc,
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

const std::unordered_set<std::string>& pulseClassNames()
{
    static const std::unordered_set<std::string> k = {
        "NPulseNeuron",
        "NPulseSynapseStdp",
        "NNeuronTrainer",
        "NNeuronLearner",
        "NPulseMembrane",
    };
    return k;
}

fs::path pulseDocsRoot(RDK::LLM::ILLMProjectContextProvider* ctx)
{
    if(!ctx)
        return {};
    return ctx->paths().repository_root / "Libraries" / "Nmsdk-PulseLib" / "Docs";
}

} // namespace

void RegisterPulseLibLlmTools(RDK::LLM::ULLMToolRegistry& registry,
                              RDK::LLM::ILLMProjectContextProvider* project_context,
                              RDK::LLM::URdkDomainAccess& domain)
{
    registry.registerTool(
        makeReadDef("search_pulse_docs",
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
        makeReadDef("list_pulse_component_classes",
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

    RDK::LLM::URdkDomainAccess* domain_access = &domain;
    registry.registerTool(
        RDK::LLM::makeLibraryWriteDef(
            "add_pulse_component",
            "Add a Nmsdk-PulseLib component (NPulseNeuron, synapses, trainers)",
            {{"type", "object"},
             {"required", {"class_name", "parent_long_name", "short_name"}},
             {"properties",
              {{"class_name", {{"type", "string"}}},
               {"parent_long_name", {{"type", "string"}}},
               {"short_name", {{"type", "string"}}},
               {"channel_index", {{"type", "integer"}, {"minimum", 0}}}}},
             {"additionalProperties", false}},
            true),
        [domain_access](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            return RDK::LLM::invokeLibraryAddComponent(*domain_access, args, pulseClassNames(),
                                                       "Nmsdk-PulseLib");
        });

    registry.registerTool(
        RDK::LLM::makeLibraryWriteDef(
            "set_pulse_property",
            "Set a property on a pulse-library component (same safety rules as set_property)",
            {{"type", "object"},
             {"required", {"long_name", "property_name", "value"}},
             {"properties",
              {{"long_name", {{"type", "string"}}},
               {"property_name", {{"type", "string"}}},
               {"value", {{"type", "string"}}},
               {"channel_index", {{"type", "integer"}, {"minimum", 0}, {"default", 0}}}}},
             {"additionalProperties", false}},
            true),
        [domain_access](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            return RDK::LLM::invokeLibrarySetProperty(*domain_access, args, pulseClassNames(),
                                                      "Nmsdk-PulseLib");
        });
}
