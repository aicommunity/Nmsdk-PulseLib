#include "ModelArchive.h"
#include "NativeOperationChecks.h"
#include "../../../../Rdk/Core/Engine/UClassImageRegistry.h"
#include "../../../../Rdk/Core/Engine/UComponentFactory.h"
#include "../NPulseNeuron.h"
#include "../NPulseMembrane.h"
#include "../NDatasetMatrix.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <typeinfo>

namespace NmsdkTools {
namespace {
using namespace RDK;
const char* GraphRoot = "TimeLearnerNativeGraph";

[[noreturn]] void Fail(const char* code, const QString& detail)
{ throw ModelArchiveError(code, detail.toStdString()); }

QString RequiredString(const QJsonObject& object, const char* key)
{
    if(!object[key].isString() || object[key].toString().trimmed().isEmpty()) Fail("invalid_archive_field", key);
    return object[key].toString();
}

struct Lease
{
    UEPtr<UStorage> storage;
    UEPtr<UContainer> object;
    bool hasBoundary = false;
    ~Lease()
    {
        if(object)
        {
            // Rehearsal/failing-import instances must not remain listeners of
            // a caller-owned stimulus after being returned to the pool.
            if(hasBoundary)
                if(auto net = dynamic_pointer_cast<UNet>(object)) net->BreakLinks();
            storage->ReturnObject(object);
        }
    }
    UEPtr<UContainer> Release() { auto result = object; object = 0; return result; }
};

UEPtr<UNet> Take(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment, const QString& name)
{
    if(!storage->CheckClass(name.toStdString())) Fail("missing_archive_class", name);
    auto object = dynamic_pointer_cast<UNet>(storage->TakeObject(name.toStdString()));
    if(!object) Fail("unsupported_archive_class", name);
    object->SetEnvironment(environment);
    return object;
}

QJsonObject Identity(UEPtr<UContainer> model, const ModelBoundarySources& boundarySources = {})
{
    auto identity = IdentifyModel(model, boundarySources);
    if(!identity["valid"].toBool()) Fail("invalid_model_identity", QString::fromStdString(model->GetName()) + ": " +
        QString::fromUtf8(QJsonDocument(identity["diagnostics"].toArray()).toJson(QJsonDocument::Compact)));
    return identity;
}

void RequireMatch(const QJsonObject& identity, UEPtr<UContainer> model,
                  const ModelBoundarySources& boundarySources = {})
{
    auto check = VerifyModelIdentity(identity, model, boundarySources);
    if(!check["valid"].toBool() || !check["match"].toBool())
    {
        QString detail=check["reason"].toString()+": "+check["first_difference"].toString();
        if(check["valid"].toBool() && check["first_difference"]==QJsonValue("model.bindings.size"))
        {
            // Keep identity strict. A bounded endpoint delta distinguishes a
            // missing native connection from a duplicate link after rebuild.
            const auto actual=IdentifyModel(model,boundarySources);
            if(actual["valid"].toBool())
            {
                const auto expectedBindings=identity["model"].toObject()["bindings"].toArray();
                const auto actualBindings=actual["model"].toObject()["bindings"].toArray();
                const auto counts=[](const QJsonArray& bindings) {
                    std::map<QByteArray,int> result;
                    for(const auto& binding:bindings)++result[QJsonDocument(binding.toObject()).toJson(QJsonDocument::Compact)];
                    return result;
                };
                const auto expected=counts(expectedBindings),observed=counts(actualBindings);
                const auto delta=[](const auto& first,const auto& second) {
                    QJsonArray entries;int total=0;
                    for(const auto& [binding,count]:first)
                    {
                        const auto other=second.find(binding);
                        const int missing=count-(other==second.end()?0:other->second);
                        if(missing<=0)continue;
                        total+=missing;
                        if(entries.size()<8)entries.append(QJsonObject{{"count",missing},
                            {"binding",QJsonDocument::fromJson(binding).object()}});
                    }
                    return QJsonObject{{"total",total},{"first_eight",entries}};
                };
                const QJsonObject diagnostic{{"expected_count",int(expectedBindings.size())},
                    {"actual_count",int(actualBindings.size())},{"missing",delta(expected,observed)},
                    {"unexpected",delta(observed,expected)}};
                detail+="; binding_delta="+QString::fromUtf8(QJsonDocument(diagnostic).toJson(QJsonDocument::Compact));
            }
        }
        Fail("archive_model_mismatch",detail);
    }
}

QString XmlDigest(const QString& xml)
{ return QString::fromLatin1(QCryptographicHash::hash(xml.toUtf8(), QCryptographicHash::Sha256).toHex()); }

// Cursor is the component node in a private serialization tree. StepDuration
// is measured wall-time telemetry, not physics or a continuation checkpoint.
// Keep scheduler limits, connection state and every other field intact. Never
// write the live model or traverse arbitrary nodes just because of their name.
void NormalizeSnapshotTelemetry(USerStorageXML& xml, UEPtr<UContainer> model)
{
    const auto fail = [&] { Fail("archive_serialization_failed", "native StepDuration/component layout"); };
    if(xml.GetNumNodes("State") != 1 || !xml.SelectNode("State")) fail();
    if(xml.GetNumNodes("StepDuration") != 1 || !xml.SelectNode("StepDuration")) fail();
    if(xml.GetNumNodes() != 0 || xml.GetNodeAttribute("PType") != std::to_string(ptPubSysState) ||
       !xml.SetNodeText("0")) fail();
    xml.SelectUp(); xml.SelectUp();
    if(xml.GetNumNodes("Components") != 1 || !xml.SelectNode("Components") ||
       xml.GetNumNodes() != model->GetNumComponents()) fail();
    for(int index = 0; index < model->GetNumComponents(); ++index)
    {
        const auto child = model->GetComponentByIndex(index);
        if(!child || xml.GetNumNodes(child->GetName()) != 1 || !xml.SelectNode(child->GetName())) fail();
        NormalizeSnapshotTelemetry(xml, child);
        xml.SelectUp();
    }
    xml.SelectUp();
}

QJsonObject Snapshot(UEPtr<UContainer> model, const ModelBoundarySources& boundarySources = {})
{
    auto net = dynamic_pointer_cast<UNet>(model);
    if(!net) Fail("unsupported_archive_graph", "UNet required");
    auto identity = Identity(model, boundarySources);
    USerStorageXML xml;
    if(!xml.Create(GraphRoot) || !net->SaveComponent(&xml, true, ptPubParameter | ptPubState | pgSystem))
        Fail("archive_serialization_failed", model->GetName().c_str());
    xml.SelectRoot();
    if(xml.GetNumNodes(model->GetName()) != 1 || !xml.SelectNode(model->GetName()))
        Fail("archive_serialization_failed", "native component root");
    NormalizeSnapshotTelemetry(xml, model);
    xml.SelectRoot();
    std::string bytes;
    if(!xml.Save(bytes)) Fail("archive_serialization_failed", "native XML Save");
    const auto text = QString::fromStdString(bytes);
    if(text.toUtf8().toStdString() != bytes) Fail("invalid_xml_encoding", "UTF8 required");
    QJsonObject snapshot;
    snapshot["component_name"] = QString::fromStdString(model->GetName());
    snapshot["component_class"] = QString::fromStdString(model->GetCompClassName());
    snapshot["native_xml"] = text; snapshot["native_xml_sha256"] = XmlDigest(text);
    snapshot["identity"] = identity;
    return snapshot;
}

std::set<QString> Classes(const QJsonObject& identity)
{
    std::set<QString> result;
    for(const auto& entry : identity["model"].toObject()["nodes"].toArray())
    {
        auto node = entry.toObject(); result.insert(RequiredString(node, "class"));
        auto parameters = node["parameters"].toObject();
        for(auto i = parameters.begin(); i != parameters.end(); ++i)
        {
            // Structural class selectors also govern future growth and need
            // definitions even when no instance of that class currently exists.
            if(!i.key().endsWith("ClassName")) continue;
            const auto data = i.value().toObject()["data"].toObject();
            if(data["type"].toString() == "utf8" && !data["value"].toString().isEmpty())
                result.insert(data["value"].toString());
        }
    }
    return result;
}

void ValidateMetadata(const QJsonObject& metadata, const QJsonObject& identity)
{
    QString rate;
    for(const auto& entry : identity["model"].toObject()["nodes"].toArray())
    {
        const auto node = entry.toObject(); const auto frequency = RequiredString(node,"frequency_hz");
        bool ok = false;
        if(frequency.toUInt(&ok) == 0 || !ok || (!rate.isEmpty() && rate != frequency))
            Fail("unsupported_multirate_archive", node["path"].toString());
        rate = frequency;
    }
    RequiredString(metadata, "implementation_id");
    const auto state = RequiredString(metadata, "state_kind");
    if(state != "model_spec" && state != "learned_solo" && state != "recognition" && state != "diagnostic" && state != "diagnostic_recognition") Fail("invalid_state_kind", state);
    const bool diagnostic=state=="diagnostic" || state=="diagnostic_recognition";
    const bool recognition=(state=="recognition" || state=="diagnostic_recognition") && metadata.contains("recognition_schema_version");
    if(state=="diagnostic_recognition" && !recognition)Fail("invalid_recognition_verification", "diagnostic conversion requires explicit recognition metadata");
    if(!metadata["training"].isObject() || !metadata["calibration"].isObject() || !metadata["conversion"].isObject())
        Fail("incomplete_model_metadata", "training/calibration/conversion required");
    const auto training = metadata["training"].toObject();
    const auto status = RequiredString(training, "status");
    if(state == "model_spec")
    {
        if(status != "not_trained") Fail("invalid_training_metadata", "model_spec must not claim training");
    }
    else
    {
        const std::set<QString> statuses=diagnostic?
            std::set<QString>{"search_unresolved","budget_exhausted","external_stop","invalid_model","invalid_observation"}:
            std::set<QString>{"exact","quantized","best_effort","search_unresolved","budget_exhausted","external_stop"};
        if(!statuses.count(status)) Fail("invalid_training_metadata", status);
        RequiredString(training, "target_id");
        const auto trainer = RequiredString(training, "trainer"), normalization = RequiredString(training, "normalization");
        if((trainer != "Classic" && trainer != "Branch") || (normalization != "N0" && normalization != "N1"))
            Fail("invalid_training_metadata", "trainer/normalization");
        const auto target = training["target_pattern"].toArray(), references = training["captured_per_input"].toArray();
        if(target.isEmpty() || (diagnostic?references.size()>target.size():target.size()!=references.size()))
            Fail("invalid_training_metadata", "target/reference dimension");
        for(const auto& value : target)
            if(!value.isDouble() || !std::isfinite(value.toDouble()) || value.toDouble() < 0.) Fail("invalid_training_metadata", "target value");
        for(const auto& value : references)
            if(!value.isDouble() || !std::isfinite(value.toDouble())) Fail("invalid_training_metadata", "reference value");
        if(diagnostic)
        {
            if(!training["certificate"].isNull() || !training["reference_complete"].isBool() ||
                training["reference_complete"].toBool()!=(target.size()==references.size()))
                Fail("invalid_training_metadata", "diagnostic reference/certificate claim");
            RequiredString(training,"reason");RequiredString(training,"capture_revision");
            const auto epoch=training["capture_epoch"];
            if(!epoch.isDouble() || !std::isfinite(epoch.toDouble()) || epoch.toDouble()<0. ||
                epoch.toDouble()>9007199254740991. || std::floor(epoch.toDouble())!=epoch.toDouble())
                Fail("invalid_training_metadata", "diagnostic capture epoch");
        }
        else
        {
            const auto certificate = training["certificate"].toObject();
            // Versioned recognition metadata retains the original solo proof;
            // it must not relabel that proof as verification of a scaled graph.
            const auto expectedIdentity=recognition?metadata["conversion"].toObject()["source_solo_identity"]:identity["sha256"];
            if(certificate["state_identity"] != expectedIdentity || certificate["status"] != training["status"])
                Fail("certificate_model_mismatch", "certificate does not identify the exported state");
        }
        if(recognition)
        {
            const auto verification=metadata["recognition_verification"].toObject();
            if(metadata["recognition_schema_version"]!=QJsonValue(1) || training["scope"]!=QJsonValue("source_solo") ||
               verification["status"]!=QJsonValue("unverified") || verification["model_identity"]!=identity["sha256"] ||
               !verification.contains("certificate") || !verification["certificate"].isNull() ||
               metadata["conversion"].toObject()["mode"]!=QJsonValue("parallel"))
                Fail("invalid_recognition_verification", "converted state requires separate verification");
        }
    }
    const auto calibration = metadata["calibration"].toObject();
    const auto calibrationStatus = RequiredString(calibration, "status");
    const std::set<QString> calibrationStatuses{"fixed","calibrated","nonseparable","invalid"};
    if(!calibrationStatuses.count(calibrationStatus)) Fail("invalid_calibration_metadata", calibrationStatus);
    RequiredString(calibration, "protocol");
    if(!calibration["threshold"].isDouble() || !std::isfinite(calibration["threshold"].toDouble()))
        Fail("invalid_calibration_metadata", "threshold");
    const auto ltPath = RequiredString(calibration, "ltzone_path");
    bool actualLtZone = false, matchedThreshold = false;
    for(const auto& entry : identity["model"].toObject()["caches"].toArray())
        actualLtZone = actualLtZone || entry.toObject()["cached_ltzone"].toString() == ltPath;
    for(const auto& entry : identity["model"].toObject()["nodes"].toArray())
    {
        const auto node = entry.toObject();
        if(node["path"].toString() != ltPath) continue;
        const auto data = node["parameters"].toObject()["Threshold"].toObject()["data"].toObject();
        const auto text = data["value"].toString(); bool ok = false;
        const std::uint64_t bits = text.toULongLong(&ok,16); double threshold;
        std::memcpy(&threshold,&bits,sizeof(threshold));
        matchedThreshold = ok && text.size() == 16 && data["type"].toString() == "binary64" &&
            threshold == calibration["threshold"].toDouble();
    }
    if(!actualLtZone || !matchedThreshold) Fail("calibration_model_mismatch", ltPath);
    const auto conversion = metadata["conversion"].toObject();
    const auto mode = RequiredString(conversion, "mode");
    if(mode != "none" && mode != "parallel") Fail("invalid_conversion_metadata", mode);
    if(state != "recognition" && state != "diagnostic_recognition" && mode != "none") Fail("invalid_conversion_metadata", "solo/spec cannot be converted");
    if(mode == "parallel")
    {
        if(RequiredString(conversion, "source_solo_identity").size() != 64 || conversion["factors"].toArray().isEmpty())
            Fail("invalid_conversion_metadata", "origin and factors required");
        for(const auto& value : conversion["factors"].toArray())
            if(!value.isDouble() || !std::isfinite(value.toDouble()) || value.toDouble() <= 0.) Fail("invalid_conversion_metadata", "factor");
    }
}

void CheckXmlNode(USerStorageXML& xml, const QString& path, const std::map<QString,QJsonObject>& nodes,
                  const std::set<QString>& inheritedFeedback = {})
{
    const auto found = nodes.find(path);
    if(found == nodes.end()) Fail("unexpected_archive_node", path);
    const auto expected = found->second;
    if(QString::fromStdString(xml.GetNodeAttribute("Class")) != expected["class"].toString())
        Fail("archive_node_class_mismatch", path);
    const auto parameters = expected["parameters"].toObject();
    for(auto i = parameters.begin(); i != parameters.end(); ++i)
    {
        bool present = false;
        for(const auto* section : {"Parameters", "State"})
        {
            if(xml.GetNumNodes(section) > 1) Fail("duplicate_archive_section", path);
            if(!xml.SelectNode(section)) continue;
            const auto count = xml.GetNumNodes(i.key().toStdString());
            if(count > 1) Fail("duplicate_archive_parameter", path + ":" + i.key());
            present = present || count == 1; xml.SelectUp();
        }
        if(!present && !(i.key()=="FeedbackGain" && inheritedFeedback.count(path)))
            Fail("missing_archive_parameter", path + ":" + i.key());
    }
    if(xml.GetNumNodes("Components") != 1 || !xml.SelectNode("Components")) Fail("missing_archive_components", path);
    const auto children = expected["child_order"].toArray();
    if(xml.GetNumNodes() != children.size()) Fail("archive_child_count_mismatch", path);
    for(int i = 0; i < children.size(); ++i)
    {
        if(!xml.SelectNode(i)) Fail("missing_archive_child", path);
        const auto name = QString::fromStdString(xml.GetNodeName());
        if(name != children[i].toString()) Fail("archive_child_order_mismatch", path);
        CheckXmlNode(xml, path + "." + name, nodes,inheritedFeedback); xml.SelectUp();
    }
    xml.SelectUp();
}

void CheckSnapshot(const QJsonObject& snapshot,const std::set<QString>& inheritedFeedback = {})
{
    const auto text = RequiredString(snapshot, "native_xml");
    if(snapshot["native_xml_sha256"].toString() != XmlDigest(text)) Fail("native_xml_digest_mismatch", "graph");
    const auto identity = snapshot["identity"].toObject();
    if(!identity["valid"].toBool() || (identity["schema_version"] != QJsonValue(1) && identity["schema_version"] != QJsonValue(2)) ||
       identity["model"].toObject()["schema_version"] != identity["schema_version"] ||
       identity["kind"].toString() != "semantic_model_identity" ||
       identity["canonical_encoding"].toString() != "length-framed-utf8-binary64-v1" ||
       !identity["diagnostics"].isArray() || !identity["diagnostics"].toArray().isEmpty() ||
       identity["sha256"].toString() != CanonicalModelDataSha256(identity["model"]))
        Fail("invalid_archive_identity", "graph");
    std::map<QString,QJsonObject> nodes;
    for(const auto& entry : identity["model"].toObject()["nodes"].toArray())
    {
        auto node = entry.toObject(); auto path = RequiredString(node, "path");
        if(!nodes.emplace(path,node).second) Fail("duplicate_archive_node", path);
    }
    if(!nodes.count("$") || nodes.at("$")["class"] != snapshot["component_class"])
        Fail("invalid_archive_root", "class");
    for(const auto& path:inheritedFeedback)
        if(!nodes.count(path) || !nodes.at(path)["parameters"].toObject().contains("FeedbackGain"))
            Fail("invalid_inherited_feedback_parameter",path);
    USerStorageXML xml;
    if(!xml.Load(text.toStdString(), GraphRoot) || xml.GetNumNodes() != 1 ||
       !xml.SelectNode(RequiredString(snapshot,"component_name").toStdString()))
        Fail("invalid_native_xml", "root");
    CheckXmlNode(xml, "$", nodes,inheritedFeedback);
}

std::map<QString, UIPropertyOutput*> BoundaryOutputs(const QJsonObject& identity,
                                                    const ModelBoundarySources& sources)
{
    std::map<QString, UIPropertyOutput*> outputs;
    for(const auto& source : sources)
    {
        auto output = dynamic_cast<const UIPropertyOutput*>(source.first);
        if(!output || !(source.first->GetType() & ptOutput) ||
           !dynamic_cast<const UVBaseDataProperty<MDMatrix<double>>*>(source.first) ||
           source.second.trimmed().isEmpty() || !outputs.emplace(source.second,const_cast<UIPropertyOutput*>(output)).second)
            Fail("invalid_boundary_sources", "distinct matrix-double outputs required");
    }
    QJsonArray expected;
    for(const auto& output : outputs) expected.append(QJsonObject{{"id",output.first},{"type","matrix_binary64"}});
    const auto semantic = identity["model"].toObject();
    if((outputs.empty() && identity["schema_version"] != QJsonValue(1)) ||
       (!outputs.empty() && (identity["schema_version"] != QJsonValue(2) || semantic["stimulus_ports"] != QJsonValue(expected))))
        Fail("boundary_sources_mismatch", "archive ports must match supplied outputs exactly");
    return outputs;
}

UEPtr<UContainer> LoadSnapshot(const QJsonObject& snapshot, UEPtr<UStorage> storage, UEPtr<UEnvironment> environment,
                              const ModelBoundarySources& boundarySources = {},const std::set<QString>& inheritedFeedback = {})
{
    CheckSnapshot(snapshot,inheritedFeedback);
    const auto outputs = BoundaryOutputs(snapshot["identity"].toObject(),boundarySources);
    auto object = Take(storage, environment, snapshot["component_class"].toString());
    Lease lease{storage, object,!boundarySources.empty()};
    object->UnInit();
    USerStorageXML xml;
    if(!xml.Load(snapshot["native_xml"].toString().toStdString(), GraphRoot) ||
       !xml.SelectNode(snapshot["component_name"].toString().toStdString())) Fail("invalid_native_xml", "load root");
    TProjectLoadDiagnostics diagnostics;
    if(!object->LoadComponent(&xml, true, &diagnostics) || !diagnostics.errors.empty() || !diagnostics.warnings.empty())
        Fail("native_model_load_failed", object->GetName().c_str());
    InitializeNative(object);
    const auto bindings = snapshot["identity"].toObject()["model"].toObject()["bindings"].toArray();
    std::set<QString> portTargets;
    for(const auto& entry : bindings)
        if(entry.toObject()["kind"].toString() == "stimulus_port") portTargets.insert(entry.toObject()["target"].toString());
    auto endpoint = [&](const QString& path) -> std::pair<UEPtr<UContainer>,std::string> {
        const auto separator = path.lastIndexOf(':'); const auto component = path.left(separator);
        if(separator < 1 || (component != "$" && !component.startsWith("$."))) Fail("invalid_boundary_binding",path);
        auto owner = component == "$" ? static_pointer_cast<UContainer>(object) : object->GetComponentL(component.mid(2).toStdString(),true);
        return {owner,path.mid(separator+1).toStdString()};
    };
    for(const auto& target : portTargets)
    {
        auto destination = endpoint(target); auto owner = destination.first; const auto& name = destination.second;
        auto property = owner ? owner->FindProperty(name) : UEPtr<UIProperty>();
        auto input = property ? dynamic_cast<UIPropertyInput*>(property.Get()) : nullptr;
        auto connector = dynamic_pointer_cast<UConnector>(owner);
        if(!input || !(property->GetType() & ptInput) || !connector || input->UsesExternalDataSource())
            Fail("boundary_binding_failed", target + ": invalid native endpoint");
        // Build may supply the profile's default source. This explicitly
        // declared input owns its complete archived slot list, including any
        // internal sources mixed with ports. Other inputs are left to native
        // Load/Build and the final complete graph comparison.
        std::vector<UCItem> previous; connector->GetCItem(name,previous);
        for(int i=int(previous.size())-1;i>=0;--i)
            previous[i].Item->Disconnect(connector,previous[i].Name,name,i);
        const auto direct = input->GetDataSources();
        for(int i=int(direct.size())-1;i>=0;--i)
            if(!input->ResetPointer(i,const_cast<UIPropertyOutput*>(dynamic_cast<const UIPropertyOutput*>(direct[i]))))
                Fail("boundary_binding_failed",target + ": cannot detach previous direct source");
        std::map<int,QJsonObject> expected;
        for(const auto& entry : bindings)
        {
            const auto binding = entry.toObject();
            if(binding["target"].toString() != target) continue;
            bool ok=false; const int slot=binding["slot"].toString().toInt(&ok);
            if(!ok || slot < 0 || slot >= bindings.size() || !expected.emplace(slot,binding).second)
                Fail("invalid_boundary_binding",target);
        }
        int next=0;
        for(const auto& entry : expected)
        {
            if(entry.first != next++) Fail("invalid_boundary_binding",target + ": noncontiguous slots");
            const auto binding=entry.second; UIPropertyOutput* output=nullptr;
            if(binding["kind"].toString() == "stimulus_port")
            {
                auto source=outputs.find(binding["source"].toString());
                if(source != outputs.end()) output=source->second;
            }
            else if(binding["kind"].toString() == "property_pointer")
            {
                auto source=endpoint(binding["source"].toString());
                if(source.first) output=dynamic_cast<UIPropertyOutput*>(source.first->FindProperty(source.second).Get());
            }
            auto sourceItem=output ? dynamic_cast<UItem*>(output->GetOwner()) : nullptr;
            if(!sourceItem || !sourceItem->IsReady() || !sourceItem->IsInit())
                Fail("boundary_binding_failed",target + ": missing/unready source");
            int actualSlot=entry.first;
            if(!sourceItem->Connect(connector,output->GetName(),name,actualSlot,true) || actualSlot != entry.first)
                Fail("boundary_binding_failed",target + ": native Connect slot mismatch");
        }
    }
    RequireMatch(snapshot["identity"].toObject(), object,boundarySources);
    return lease.Release();
}

struct ImageSource { QString kind, base; UEPtr<UContainer> prototype; };
ImageSource Source(UEPtr<UStorage> storage, const QString& name)
{
    if(!storage->CheckClass(name.toStdString())) Fail("missing_archive_class", name);
    auto factory = storage->GetComponentFactory(name.toStdString());
    auto image = dynamic_pointer_cast<UVirtualMethodFactory>(factory);
    if(!image) return {"native_required", {}, {}};
    auto prototype = image->GetComponent();
    if(!prototype) Fail("invalid_image_factory", name);
    if(prototype->GetClass() == ForbiddenId || prototype->GetClass() == factory->GetClassId())
        return {"native_required", {}, {}};
    return {"derived_image", QString::fromStdString(storage->FindClassName(prototype->GetClass())), prototype};
}

QJsonObject ImageSnapshot(UEPtr<UStorage> storage, UEPtr<UEnvironment> environment,
                         const QString& name, const ImageSource& source)
{
    UEPtr<UContainer> object;
    if(source.kind == "native_required") object = Take(storage, environment, name);
    else object = source.prototype->Alloc(storage, false);
    Lease lease{storage, object};
    if(!object) Fail("image_copy_failed", name);
    object->SetEnvironment(environment); InitializeNative(object);
    return Snapshot(object);
}
QJsonArray ImageDefinitions(UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,
    const QJsonObject& identity,const std::vector<std::string>& additionalImageRoots)
{
    QJsonArray definitions; std::set<QString> done, visiting;
    std::function<void(const QString&)> capture = [&](const QString& name) {
        if(done.count(name)) return;
        if(!visiting.insert(name).second) Fail("image_dependency_cycle", name);
        auto source = Source(storage, name);
        auto snapshot = ImageSnapshot(storage, environment, name, source); CheckSnapshot(snapshot);
        auto dependencies = Classes(snapshot["identity"].toObject());
        dependencies.erase(name);
        if(!source.base.isEmpty()) dependencies.insert(source.base);
        for(const auto& dependency : dependencies) capture(dependency);
        QJsonObject definition;
        definition["name"] = name; definition["kind"] = source.kind; definition["base_class"] = source.base;
        definition["snapshot"] = snapshot;
        QJsonArray dependenciesJson; for(const auto& dependency : dependencies) dependenciesJson.append(dependency);
        definition["dependencies"] = dependenciesJson;
        definitions.append(definition); visiting.erase(name); done.insert(name);
    };
    for(const auto& name : Classes(identity)) capture(name);
    for(const auto& name:additionalImageRoots)capture(QString::fromStdString(name));
    return definitions;
}
}

QJsonArray CaptureModelImageDefinitions(UEPtr<UContainer> model,const ModelBoundarySources& boundarySources,
    const std::vector<std::string>& additionalImageRoots)
{
    if(!model || !model->GetStorage() || !model->GetEnvironment())Fail("invalid_archive_context","model/storage/environment");
    return ImageDefinitions(model->GetStorage(),model->GetEnvironment(),Identity(model,boundarySources),additionalImageRoots);
}
QString ModelImageDefinitionsFingerprint(const QJsonArray& definitions)
{
    if(definitions.isEmpty() || definitions.size()>4096)Fail("invalid_image_definition_count","1..4096 required");
    QJsonObject semantic;std::set<QString> names;std::map<QString,std::set<QString>> edges;
    for(const auto& value:definitions)
    {
        if(!value.isObject())Fail("invalid_image_definition","object required");
        const auto definition=value.toObject();const auto name=RequiredString(definition,"name");
        const auto kind=RequiredString(definition,"kind");
        if(definition.size()!=5 || !names.insert(name).second || !definition["base_class"].isString() ||
            !definition["snapshot"].isObject() || !definition["dependencies"].isArray() ||
            (kind!="native_required" && kind!="derived_image"))Fail("invalid_image_definition",name);
        const auto snapshot=definition["snapshot"].toObject();CheckSnapshot(snapshot);
        auto expected=Classes(snapshot["identity"].toObject());expected.erase(name);
        const auto base=definition["base_class"].toString();if(!base.isEmpty())expected.insert(base);
        std::set<QString> actual;
        for(const auto& dependency:definition["dependencies"].toArray())
            if(!dependency.isString() || !actual.insert(dependency.toString()).second)Fail("invalid_image_dependencies",name);
        if(expected!=actual || (kind=="derived_image" && base.isEmpty()) ||
            (kind=="native_required" && !base.isEmpty()))Fail("invalid_image_dependencies",name);
        edges.emplace(name,std::move(actual));
        semantic[name]=QJsonObject{{"kind",kind},{"base_class",base},
            {"model",snapshot["identity"].toObject()["model"]}};
    }
    for(const auto& value:definitions)for(const auto& dependency:value.toObject()["dependencies"].toArray())
        if(!names.count(dependency.toString()))Fail("missing_image_dependency",dependency.toString());
    std::set<QString> done,visiting;
    std::function<void(const QString&,unsigned)> visit=[&](const QString& name,unsigned depth) {
        if(done.count(name))return;
        if(depth>128 || !visiting.insert(name).second)Fail("image_dependency_cycle_or_depth",name);
        for(const auto& next:edges.at(name))visit(next,depth+1);
        visiting.erase(name);done.insert(name);
    };
    for(const auto& name:names)visit(name,0);
    return CanonicalModelDataSha256(semantic);
}

QJsonObject ExportModelArchive(UEPtr<UContainer> model, const QJsonObject& metadata,
                               const ModelBoundarySources& boundarySources,const std::vector<std::string>& additionalImageRoots)
try
{
    if(!model || !model->GetStorage() || !model->GetEnvironment()) Fail("invalid_archive_context", "model/storage/environment");
    if(!dynamic_pointer_cast<NMSDK::NPulseNeuron>(model)) Fail("unsupported_archive_model", "NPulseNeuron graph required");
    auto storage = model->GetStorage(); auto environment = model->GetEnvironment();
    auto graph = Snapshot(model,boundarySources); CheckSnapshot(graph);
    ValidateMetadata(metadata, graph["identity"].toObject());
    // Prove the native serializer represents this graph, including direct links
    // regenerated by native Build, before emitting an apparently usable archive.
    Lease rehearsal{storage, LoadSnapshot(graph, storage, environment,boundarySources),!boundarySources.empty()};
    const auto definitions=ImageDefinitions(storage,environment,graph["identity"].toObject(),additionalImageRoots);
    QJsonObject archive;
    archive["schema_version"] = boundarySources.empty() ? 1 : 2; archive["kind"] = "timelearner_model_archive";
    archive["encoding"] = boundarySources.empty() ? "native-xml-and-model-identity-v1" : "native-xml-and-model-identity-v2";
    archive["dynamic_checkpoint"] = false;
    archive["metadata"] = metadata; archive["graph"] = graph; archive["class_definitions"] = definitions;
    archive["sha256"] = CanonicalModelDataSha256(archive);
    return archive;
}
catch(const UException& error) { Fail("native_archive_export_failed",QString::fromStdString(std::string(typeid(error).name())+": "+error.what())); }

UEPtr<UContainer> ClonePhysicalGraphForEdit(UEPtr<UContainer> model,const ModelBoundarySources& sources)
try
{
    if(!model || !model->GetStorage() || !model->GetEnvironment() || !dynamic_pointer_cast<NMSDK::NPulseNeuron>(model))
        Fail("invalid_edit_clone_context","native neuron/storage/environment required");
    auto snapshot=Snapshot(model,sources);CheckSnapshot(snapshot);
    return LoadSnapshot(snapshot,model->GetStorage(),model->GetEnvironment(),sources);
}
catch(const UException& error) { Fail("native_edit_clone_failed",QString::fromStdString(error.what())); }

QJsonObject ExportPhysicalGraphForEdit(UEPtr<UContainer> model,const ModelBoundarySources& sources)
{
    if(!model || !dynamic_pointer_cast<NMSDK::NPulseNeuron>(model))
        Fail("invalid_edit_snapshot_context","native neuron required");
    auto snapshot=Snapshot(model,sources);CheckSnapshot(snapshot);return snapshot;
}
UEPtr<UContainer> ImportPhysicalGraphForEdit(const QJsonObject& snapshot,UEPtr<UStorage> storage,
    UEPtr<UEnvironment> environment,const ModelBoundarySources& sources)
{
    if(!storage || !environment)Fail("invalid_edit_snapshot_context","storage/environment required");
    Lease candidate{storage,LoadSnapshot(snapshot,storage,environment,sources),!sources.empty()};
    if(!dynamic_pointer_cast<NMSDK::NPulseNeuron>(candidate.object))
        Fail("invalid_edit_snapshot_context","native neuron required");
    return candidate.Release();
}

UEPtr<UContainer> ImportFeedbackProfileGraphForEdit(const QJsonObject& snapshot,UEPtr<UStorage> storage,
    UEPtr<UEnvironment> environment,const std::map<std::string,std::string>& classRenames,
    std::optional<double> feedbackGain,const ModelBoundarySources& boundarySources)
{
    CheckSnapshot(snapshot);
    if(!storage || !environment || (feedbackGain && (!std::isfinite(*feedbackGain) || *feedbackGain<0.)))
        Fail("invalid_feedback_import_context","storage/environment/finite nonnegative gain required");
    // Validate the original XML against its own complete identity before
    // removing any serialized gain. A derived image must not hide a corrupt
    // or contradictory source snapshot.
    Lease rehearsal{storage,LoadSnapshot(snapshot,storage,environment,boundarySources),!boundarySources.empty()};
    std::set<std::string> destinations,used;
    for(const auto& [oldName,newName]:classRenames)
    {
        if(oldName.empty() || newName.empty() || oldName==newName || classRenames.count(newName) ||
           !destinations.insert(newName).second)
            Fail("invalid_feedback_class_substitution",QString::fromStdString(oldName));
        auto oldObject=Take(storage,environment,QString::fromStdString(oldName));
        Lease oldImage{storage,oldObject};
        auto newObject=Take(storage,environment,QString::fromStdString(newName));
        Lease newImage{storage,newObject};
        if(typeid(*oldImage.object)!=typeid(*newImage.object))
            Fail("feedback_image_native_type_mismatch",QString::fromStdString(newName));
    }
    const auto rename=[&](const QString& name) {
        const auto found=classRenames.find(name.toStdString());
        if(found==classRenames.end())return name;
        used.insert(found->first);return QString::fromStdString(found->second);
    };
    const std::set<QString> classParameters{"MembraneClassName","LTMembraneClassName","LTZoneClassName",
        "ExcGeneratorClassName","InhGeneratorClassName","ExcChannelClassName","InhChannelClassName",
        "SynapseClassName","ExcSynapseClassName","InhSynapseClassName"};
    auto identity=snapshot["identity"].toObject();auto model=identity["model"].toObject();
    auto nodes=model["nodes"].toArray();std::map<QString,QJsonObject> originals;
    std::set<QString> inheritedGains;
    for(int i=0;i<nodes.size();++i)
    {
        auto node=nodes[i].toObject();const auto path=RequiredString(node,"path");
        if(!originals.emplace(path,node).second)Fail("invalid_feedback_source_graph",path);
        const auto originalClass=RequiredString(node,"class");node["class"]=rename(originalClass);
        auto parameters=node["parameters"].toObject();
        for(auto prop=parameters.begin();prop!=parameters.end();++prop)
        {
            if(!classParameters.count(prop.key()))continue;
            auto property=prop.value().toObject();auto data=property["data"].toObject();
            if(property["binding"]!=QJsonValue(false) || data["type"]!=QJsonValue("utf8"))
                Fail("unsupported_feedback_class_binding",path+":"+prop.key());
            data["value"]=rename(data["value"].toString());property["data"]=data;prop.value()=property;
        }
        if(parameters.contains("FeedbackGain"))
        {
            auto property=parameters["FeedbackGain"].toObject();auto data=property["data"].toObject();
            bool ok=false;const auto text=data["value"].toString();
            const std::uint64_t bits=text.toULongLong(&ok,16);double previous;std::memcpy(&previous,&bits,sizeof(previous));
            if(property["binding"]!=QJsonValue(false) || data["type"]!=QJsonValue("binary64") ||
               !ok || text.size()!=16 || !std::isfinite(previous) || previous<0.)
                Fail("unsupported_feedback_gain",path);
            const bool replaced=classRenames.count(originalClass.toStdString());
            if(feedbackGain && !replaced)Fail("feedback_gain_requires_derived_image",path);
            if(replaced)
            {
                auto prototype=Take(storage,environment,node["class"].toString());
                Lease image{storage,prototype};
                auto membrane=dynamic_pointer_cast<NMSDK::NPulseMembrane>(image.object);
                double expected=feedbackGain.value_or(previous);
                if(expected==0.)expected=0.; // ModelIdentity canonical binary64 zero.
                const auto binding=membrane?dynamic_cast<const UIPropertyInput*>(&membrane->FeedbackGain):nullptr;
                if(!membrane || (binding && (binding->IsConnected() || binding->UsesExternalDataSource())) ||
                   membrane->FeedbackGain.GetData()!=expected)
                    Fail("feedback_image_gain_mismatch",path);
                // No per-instance uniform repair: omit the old serialized
                // value, so native TakeObject/Load inherits the derived image.
                inheritedGains.insert(path);
                std::uint64_t newBits;std::memcpy(&newBits,&expected,sizeof(newBits));
                data["value"]=QString::number(qulonglong(newBits),16).rightJustified(16,'0');
                property["data"]=data;parameters["FeedbackGain"]=property;
            }
        }
        node["parameters"]=parameters;nodes[i]=node;
    }
    if(feedbackGain && inheritedGains.empty())Fail("feedback_no_membranes","source graph");
    model["nodes"]=nodes;identity["model"]=model;identity["sha256"]=CanonicalModelDataSha256(model);
    USerStorageXML xml;
    if(!xml.Load(snapshot["native_xml"].toString().toStdString(),GraphRoot) ||
       xml.GetNumNodes(snapshot["component_name"].toString().toStdString())!=1 ||
       !xml.SelectNode(snapshot["component_name"].toString().toStdString()))
        Fail("invalid_feedback_native_xml","source root");
    std::function<void(const QString&)> rewrite=[&](const QString& path) {
        const auto found=originals.find(path);
        if(found==originals.end())Fail("invalid_feedback_native_xml",path);
        const auto& original=found->second;
        const auto oldClass=RequiredString(original,"class");
        if(xml.GetNodeAttribute("Class")!=oldClass.toStdString() ||
           !xml.SetNodeAttribute("Class",rename(oldClass).toStdString()) ||
           xml.GetNumNodes("Parameters")!=1 || !xml.SelectNode("Parameters"))
            Fail("invalid_feedback_native_xml",path);
        const auto parameters=original["parameters"].toObject();
        for(auto prop=parameters.begin();prop!=parameters.end();++prop)
        {
            if(!classParameters.count(prop.key()))continue;
            const auto name=prop.key().toStdString();
            const auto previous=prop.value().toObject()["data"].toObject()["value"].toString();
            if(xml.GetNumNodes(name)!=1 || !xml.SelectNode(name) || xml.GetNodeText()!=previous.toStdString() ||
               !xml.SetNodeText(rename(previous).toStdString()))Fail("invalid_feedback_native_xml",path+":"+prop.key());
            xml.SelectUp();
        }
        if(inheritedGains.count(path))
        {
            if(xml.GetNumNodes("FeedbackGain")!=1 || !xml.SelectNode("FeedbackGain"))
                Fail("invalid_feedback_native_xml",path+":FeedbackGain");
            xml.DelNode(); // native cursor returns to Parameters.
        }
        xml.SelectUp();
        if(xml.GetNumNodes("Components")!=1 || !xml.SelectNode("Components"))
            Fail("invalid_feedback_native_xml",path+":Components");
        if(xml.GetNumNodes()!=original["child_order"].toArray().size())
            Fail("invalid_feedback_native_xml",path+":child count");
        for(int index=0;index<xml.GetNumNodes();++index)
        {
            if(!xml.SelectNode(index))Fail("invalid_feedback_native_xml",path);
            const auto child=QString::fromStdString(xml.GetNodeName());rewrite(path+"."+child);xml.SelectUp();
        }
        xml.SelectUp();
    };
    rewrite("$");
    if(used.size()!=classRenames.size())Fail("unused_feedback_class_substitution","declared classes must occur");
    xml.SelectRoot();std::string bytes;
    if(!xml.Save(bytes))Fail("feedback_native_serialization_failed","native Save");
    auto derived=snapshot;const auto text=QString::fromStdString(bytes);
    derived["component_class"]=rename(snapshot["component_class"].toString());
    derived["native_xml"]=text;derived["native_xml_sha256"]=XmlDigest(text);derived["identity"]=identity;
    // LoadSnapshot initializes/reset native dynamics and independently compares
    // the entire effective graph, including every saved individual parameter.
    // Only this private, already image-validated derivation may omit these
    // exact gain fields. All public source/archive loads retain strict XML
    // completeness, and the resulting object's full identity is still checked.
    Lease candidate{storage,LoadSnapshot(derived,storage,environment,boundarySources,inheritedGains),!boundarySources.empty()};
    if(!dynamic_pointer_cast<NMSDK::NPulseNeuron>(candidate.object))Fail("unsupported_edit_graph","NPulseNeuron required");
    return candidate.Release();
}

QJsonObject ExportDatasetSnapshot(UEPtr<UContainer> dataset)
{
    if(!dataset || !dynamic_pointer_cast<NMSDK::NDatasetMatrix>(dataset))Fail("unsupported_auxiliary_graph","NDatasetMatrix required");
    auto snapshot=Snapshot(dataset);CheckSnapshot(snapshot);QJsonArray required;
    for(const auto& name:Classes(snapshot["identity"].toObject()))required.append(name);
    snapshot["required_class_names"]=required;return snapshot;
}
UEPtr<UContainer> ImportDatasetSnapshot(const QJsonObject& snapshot,UEPtr<UStorage> storage,UEPtr<UEnvironment> environment)
{
    CheckSnapshot(snapshot);QJsonArray required;
    for(const auto& name:Classes(snapshot["identity"].toObject()))required.append(name);
    if(snapshot["required_class_names"]!=QJsonValue(required))Fail("auxiliary_dependencies_mismatch","classes");
    Lease candidate{storage,LoadSnapshot(snapshot,storage,environment)};
    if(!dynamic_pointer_cast<NMSDK::NDatasetMatrix>(candidate.object))Fail("unsupported_auxiliary_graph","NDatasetMatrix required");
    return candidate.Release();
}

void PrepareModelArchive(const QJsonObject& archive,UEPtr<UStorage> storage,UEPtr<UEnvironment> environment,const QString& implementationId)
try
{
    if(!storage || !environment) Fail("invalid_archive_context", "storage/environment");
    const int version = archive["schema_version"].toInt();
    if(version!=1 && version!=2)Fail("unsupported_archive_schema","version");
    if(archive["schema_version"] != QJsonValue(version) || archive["kind"].toString() != "timelearner_model_archive" ||
       archive["encoding"].toString() != QString("native-xml-and-model-identity-v%1").arg(version) || archive["dynamic_checkpoint"] != QJsonValue(false))
        Fail("unsupported_archive_schema", "header");
    auto payload = archive; payload.remove("sha256");
    if(archive["sha256"].toString() != CanonicalModelDataSha256(payload)) Fail("archive_digest_mismatch", "payload");
    const auto metadata = archive["metadata"].toObject();
    if(implementationId.isEmpty() || metadata["implementation_id"].toString() != implementationId)
        Fail("implementation_identity_mismatch", "runtime");
    const auto graph = archive["graph"].toObject(); CheckSnapshot(graph);
    ValidateMetadata(metadata, graph["identity"].toObject());
    if(!archive["class_definitions"].isArray()) Fail("missing_class_definitions", "array required");
    std::set<QString> declared;
    const auto definitions = archive["class_definitions"].toArray();
    // Validate every document and the complete ordered dependency closure before
    // registering the first image or constructing the imported learned model.
    for(const auto& entry : definitions)
    {
        const auto definition = entry.toObject(); const auto name = RequiredString(definition,"name");
        if(declared.count(name)) Fail("duplicate_class_definition", name);
        auto snapshot = definition["snapshot"].toObject(); CheckSnapshot(snapshot);
        if(snapshot["identity"].toObject()["schema_version"] != QJsonValue(1))
            Fail("image_has_boundary_ports", name);
        const auto kind = RequiredString(definition,"kind");
        auto dependencies = Classes(snapshot["identity"].toObject()); dependencies.erase(name);
        if(kind == "derived_image")
        {
            const auto base = RequiredString(definition,"base_class");
            if(base == name || snapshot["component_class"].toString() != base) Fail("invalid_image_base", name);
            dependencies.insert(base);
        }
        else if(kind != "native_required" || !definition["base_class"].toString().isEmpty() || snapshot["component_class"].toString() != name)
            Fail("invalid_image_definition", name);
        QJsonArray expected; for(const auto& dependency : dependencies) expected.append(dependency);
        if(definition["dependencies"] != QJsonValue(expected)) Fail("invalid_class_dependencies", name);
        for(const auto& dependency : dependencies)
            if(!declared.count(dependency)) Fail("missing_or_unordered_class_dependency", dependency);
        declared.insert(name);
    }
    for(const auto& name : Classes(graph["identity"].toObject()))
        if(!declared.count(name)) Fail("missing_class_definition", name);
    for(const auto& entry : definitions)
    {
        const auto definition = entry.toObject(); const auto name = definition["name"].toString();
        const auto snapshot = definition["snapshot"].toObject();
        if(definition["kind"].toString() == "native_required" || storage->CheckClass(name.toStdString()))
        {
            auto source = Source(storage, name);
            if(source.kind != definition["kind"].toString() || source.base != definition["base_class"].toString())
                Fail("image_factory_mismatch", name);
            auto actual = ImageSnapshot(storage, environment, name, source);
            if(actual["identity"] != snapshot["identity"]) Fail("image_definition_collision", name);
        }
        else
        {
            Lease candidate{storage, LoadSnapshot(snapshot, storage, environment)};
            RegisterClassImage(storage, name.toStdString(), candidate.object);
        }
    }
}
catch(const UException& error){Fail("native_archive_prepare_failed",QString::fromStdString(error.what()));}

UEPtr<UContainer> ImportModelArchive(const QJsonObject& archive, UEPtr<UStorage> storage,
    UEPtr<UEnvironment> environment, const QString& implementationId, const ModelBoundarySources& boundarySources)
try
{
    PrepareModelArchive(archive,storage,environment,implementationId);
    if(archive["schema_version"]!=QJsonValue(boundarySources.empty()?1:2))Fail("unsupported_archive_schema","boundary ports");
    const auto graph=archive["graph"].toObject();BoundaryOutputs(graph["identity"].toObject(),boundarySources);
    Lease imported{storage,LoadSnapshot(graph, storage, environment,boundarySources),!boundarySources.empty()};
    if(!dynamic_pointer_cast<NMSDK::NPulseNeuron>(imported.object)) Fail("unsupported_archive_model", "NPulseNeuron graph required");
    return imported.Release();
}
catch(const UException& error) { Fail("native_archive_import_failed",QString::fromStdString(std::string(typeid(error).name())+": "+error.what())); }
}
