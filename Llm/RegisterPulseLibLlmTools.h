#ifndef NMSDK_REGISTER_PULSE_LIB_LLM_TOOLS_H
#define NMSDK_REGISTER_PULSE_LIB_LLM_TOOLS_H

namespace RDK::LLM {
class ILLMProjectContextProvider;
class URdkDomainAccess;
class ULLMToolRegistry;
}

void RegisterPulseLibLlmTools(RDK::LLM::ULLMToolRegistry& registry,
                              RDK::LLM::ILLMProjectContextProvider* project_context,
                              RDK::LLM::URdkDomainAccess& domain);

#endif
