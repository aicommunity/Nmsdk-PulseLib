#include "RegisterPulseLibLlmTools.h"

#include <filesystem>
#include <memory>

#include "../../../Rdk/LLM/Core/Context/ILLMProjectContextProvider.h"
#include "../../../Rdk/LLM/Core/Context/UDocSearchIndex.h"
#include "../../../Rdk/LLM/Core/Context/ULLMDocOpenPolicy.h"
#include "../../../Rdk/LLM/Core/Domain/URdkDomainAccess.h"
#include "../../../Rdk/LLM/Core/LlmPublicApi.h"
#include "../../../Rdk/LLM/Core/Tools/ULLMToolRegistry.h"
#include "UPackPulseDocs.h"

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
                nlohmann::json row = {{"path", s.path},
                                      {"title", s.title},
                                      {"excerpt", s.excerpt},
                                      {"score", s.score}};
                if(project_context)
                    RDK::LLM::enrichSnippetDocUri(row, s.path,
                                                  project_context->paths().repository_root);
                r.result["snippets"].push_back(std::move(row));
            }
            r.ok = true;
            return r;
        });

    registry.registerTool(
        makeReadDef("list_pulse_component_classes",
                    "Lists pulse library component class names from the live registry",
                    {{"type", "object"}, {"additionalProperties", false}}),
        [&domain](const nlohmann::json& args) -> RDK::LLM::ToolGatewayResult {
            (void)args;
            RDK::LLM::ToolGatewayResult r;
            nlohmann::json out;
            if(domain.listRegisteredClasses(out, "PulseLibrary").ok())
                r.result = std::move(out);
            else
                r.result["classes"] = nlohmann::json::array();
            r.result["docs_hint"] = "Libraries/Nmsdk-PulseLib/Docs/README.md";
            r.result["mutation_hint"] = "Use add_component with class_name from this list.";
            r.ok = true;
            return r;
        });

    // TD-171: library capability pack adapter (hints band; tools registered above).
    if(RDK::LLM::LLMServices::instance().isInitialized())
    {
        RDK::LLM::LLMServices::instance().packs().registerPack(
            std::make_unique<RDK::LLM::UPackPulseDocs>());
    }
}
