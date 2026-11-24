#pragma once
#include "BoundedTraining.h"
#include <QJsonObject>
#include <QStringList>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace NMSDK::TimeLearner {
// An orthogonal versioned declaration, shared by cold, runtime and evidence
// codecs. Omission means Sequential. An explicit no-op Sequential object is
// not canonical, and must not be advertised as an active experiment factor.
inline void AppendInputTraversalDeclaration(QJsonObject& search,const BoundedTrainingConfig& configuration)
{
    ValidateNativeInputTraversalConfiguration(configuration);
    if(search.contains("input_traversal"))
        throw std::invalid_argument("duplicate_input_traversal_declaration");
    if(configuration.inputTraversal==InputTraversalPolicy::Sequential)return;
    search["input_traversal"]=QJsonObject{{"kind","timelearner_input_traversal"},{"schema_version",1},
        {"policy","classic_candidate_round_robin"},
        {"candidates_per_visit",double(configuration.candidatesPerInputVisit)},
        {"maximum_seed_elements",double(configuration.maximumTraversalSeedElements)}};
}

inline void TakeInputTraversalDeclaration(QJsonObject& search,BoundedTrainingConfig& configuration)
{
    if(!search.contains("input_traversal"))
    {
        configuration.inputTraversal=InputTraversalPolicy::Sequential;
        configuration.maximumTraversalSeedElements=DefaultTraversalSeedElements;
        configuration.candidatesPerInputVisit=DefaultCandidatesPerInputVisit;return;
    }
    const auto value=search.value("input_traversal");
    if(!value.isObject())throw std::invalid_argument("invalid_input_traversal_declaration");
    const auto declaration=value.toObject();
    QStringList fields{"kind","schema_version","policy","candidates_per_visit","maximum_seed_elements"};fields.sort();
    const auto version=declaration.value("schema_version"),limit=declaration.value("maximum_seed_elements");
    const auto quantum=declaration.value("candidates_per_visit");
    if(declaration.keys()!=fields || declaration.value("kind").toString()!="timelearner_input_traversal" ||
       !version.isDouble() || version.toDouble()!=1. ||
       declaration.value("policy").toString()!="classic_candidate_round_robin" ||
       !limit.isDouble() || !std::isfinite(limit.toDouble()) || limit.toDouble()<1. ||
       limit.toDouble()>9007199254740991. || limit.toDouble()>double(std::numeric_limits<std::size_t>::max()) ||
       std::floor(limit.toDouble())!=limit.toDouble() ||
       !quantum.isDouble() || !std::isfinite(quantum.toDouble()) || quantum.toDouble()<1. ||
       quantum.toDouble()>9007199254740991. || quantum.toDouble()>double(std::numeric_limits<std::size_t>::max()) ||
       std::floor(quantum.toDouble())!=quantum.toDouble())
        throw std::invalid_argument("invalid_input_traversal_declaration");
    // Commit only after validation; malformed data cannot partly change a
    // caller's configuration or disappear from its unconsumed field set.
    configuration.inputTraversal=InputTraversalPolicy::ClassicCandidateRoundRobin;
    configuration.maximumTraversalSeedElements=std::size_t(limit.toDouble());
    configuration.candidatesPerInputVisit=std::size_t(quantum.toDouble());
    search.remove("input_traversal");
}
}
