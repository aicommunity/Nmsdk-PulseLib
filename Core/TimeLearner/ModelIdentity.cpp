#include "ModelIdentity.h"
#include "ModelInspection.h"
#include "../NPulseNeuronCommon.h"
#include "../NPulseMembraneCommon.h"
#include "../NPulseNeuron.h"
#include "../NPulseChannel.h"
#include "../NPulseSynapse.h"
#include <QCryptographicHash>
#include <QJsonArray>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <functional>
#include <typeindex>

namespace NmsdkTools {
namespace {
using namespace RDK;
struct Node { UEPtr<UContainer> component; QString path; };

QString F64(double value)
{
    static_assert(sizeof(double) == sizeof(std::uint64_t) && std::numeric_limits<double>::is_iec559,
                  "Model identity requires IEEE754 binary64");
    if(!std::isfinite(value)) throw std::runtime_error("nonfinite_parameter");
    if(value == 0.) value = 0.; // canonicalize negative zero
    std::uint64_t bits; std::memcpy(&bits, &value, sizeof(bits));
    return QString::number(static_cast<qulonglong>(bits), 16).rightJustified(16, QLatin1Char('0'));
}

template<class T> QJsonValue Scalar(T value) { return QString::fromStdString(std::to_string(value)); }
template<> QJsonValue Scalar<double>(double value) { return F64(value); }
template<> QJsonValue Scalar<bool>(bool value) { return value; }
template<> QJsonValue Scalar<std::string>(std::string value)
{
    const auto text = QString::fromStdString(value);
    if(text.toUtf8().toStdString() != value) throw std::runtime_error("invalid_utf8_parameter");
    return text;
}

template<class T>
bool Simple(UIProperty* property, const char* kind, QJsonObject& result)
{
    auto typed = dynamic_cast<UVBaseDataProperty<T>*>(property);
    if(!typed) return false;
    result["type"] = kind; result["value"] = Scalar<T>(typed->GetData());
    return true;
}

template<class T>
bool Vector(UIProperty* property, const char* kind, QJsonObject& result)
{
    auto typed = dynamic_cast<UVBaseDataProperty<std::vector<T>>*>(property);
    if(!typed) return false;
    QJsonArray values;
    for(const auto& value : typed->GetData()) values.append(Scalar<T>(value));
    result["type"] = kind; result["value"] = values;
    return true;
}

template<class T>
bool Matrix(UIProperty* property, const char* kind, QJsonObject& result)
{
    auto typed = dynamic_cast<UVBaseDataProperty<MDMatrix<T>>*>(property);
    if(!typed) return false;
    const auto& matrix = typed->GetData();
    QJsonArray rows;
    for(int r = 0; r < matrix.GetRows(); ++r)
    {
        QJsonArray columns;
        for(int c = 0; c < matrix.GetCols(); ++c) columns.append(Scalar<T>(matrix(r,c)));
        rows.append(columns);
    }
    result["type"] = kind; result["rows"] = QString::number(matrix.GetRows());
    result["columns"] = QString::number(matrix.GetCols()); result["value"] = rows;
    return true;
}

QJsonObject Value(UIProperty* property)
{
    QJsonObject result;
    if(auto pair = dynamic_cast<UVBaseDataProperty<std::pair<std::string,std::string>>*>(property))
    {
        result["type"] = "utf8_pair";
        QJsonArray values; values.append(Scalar(pair->GetData().first)); values.append(Scalar(pair->GetData().second));
        result["value"] = values; return result;
    }
    if(Simple<bool>(property, "boolean", result) || Simple<double>(property, "binary64", result) ||
       Simple<int>(property, "signed_integer", result) || Simple<long>(property, "signed_integer", result) ||
       Simple<long long>(property, "signed_integer", result) ||
       Simple<unsigned int>(property, "unsigned_integer", result) ||
       Simple<unsigned long>(property, "unsigned_integer", result) ||
       Simple<unsigned long long>(property, "unsigned_integer", result) ||
       Simple<std::string>(property, "utf8", result) ||
       Vector<double>(property, "binary64_vector", result) || Vector<int>(property, "integer_vector", result) ||
       Vector<std::string>(property, "utf8_vector", result) ||
       Matrix<double>(property, "binary64_matrix", result) || Matrix<int>(property, "integer_matrix", result))
        return result;
    throw std::runtime_error("unsupported_parameter_type");
}

void Collect(UEPtr<UContainer> component, const QString& path, std::vector<Node>& nodes,
             std::set<const UContainer*>& seen)
{
    if(!component || !seen.insert(component.Get()).second) throw std::runtime_error("invalid_graph");
    nodes.push_back({component, path});
    std::vector<NameT> children; component->GetComponentsList(children);
    for(const auto& child : children)
        Collect(component->GetComponent(child, true), path + "." + QString::fromStdString(child), nodes, seen);
}

// Length-framed UTF8 and explicit value tags avoid Qt5/Qt6 JSON number-format
// differences, locale dependence, compiler typeid names and ambiguous joins.
void Frame(QByteArray& destination, const QByteArray& bytes)
{
    destination += QByteArray::number(bytes.size()); destination += ':'; destination += bytes;
}

void Canonical(QByteArray& bytes, const QJsonValue& value)
{
    if(value.isObject())
    {
        bytes += 'o'; const auto object = value.toObject();
        Frame(bytes, QByteArray::number(object.size()));
        for(auto i = object.begin(); i != object.end(); ++i)
        { Frame(bytes, i.key().toUtf8()); Canonical(bytes, i.value()); }
    }
    else if(value.isArray())
    {
        bytes += 'a'; const auto array = value.toArray(); Frame(bytes, QByteArray::number(array.size()));
        for(const auto& element : array) Canonical(bytes, element);
    }
    else if(value.isString()) { bytes += 's'; Frame(bytes, value.toString().toUtf8()); }
    else if(value.isBool()) bytes += value.toBool() ? 't' : 'f';
    else if(value.isDouble()) { bytes += 'd'; Frame(bytes, F64(value.toDouble()).toLatin1()); }
    else if(value.isNull()) bytes += 'n';
    else throw std::runtime_error("undefined_identity_value");
}

bool Excluded(const std::string& name)
{
    // These UContainer properties affect identity/presentation/diagnostics,
    // not the numerical model. MaxCalculationDuration is intentionally included:
    // its budget can skip child calculation and changes the effective protocol.
    return name == "Name" || name == "Id" || name == "Coord" ||
           name == "DebugSysEventsMask" || name == "CalculationDurationThreshold";
}

bool Sensitive(const std::string& name)
{
    const auto normalized = QString::fromStdString(name).toLower();
    return normalized == "password" || normalized == "apikey" || normalized == "api_key" ||
           normalized == "accesstoken" || normalized == "access_token" ||
           normalized == "secretkey" || normalized == "secret_key";
}

QString FirstDifference(const QJsonValue& a, const QJsonValue& b, const QString& path)
{
    if(a.type() != b.type()) return path;
    if(a.isObject())
    {
        const auto x = a.toObject(), y = b.toObject();
        for(auto i = x.begin(); i != x.end(); ++i)
        {
            if(!y.contains(i.key())) return path + "." + i.key();
            const auto difference = FirstDifference(i.value(), y[i.key()], path + "." + i.key());
            if(!difference.isEmpty()) return difference;
        }
        for(auto i = y.begin(); i != y.end(); ++i)
            if(!x.contains(i.key())) return path + "." + i.key();
    }
    else if(a.isArray())
    {
        const auto x = a.toArray(), y = b.toArray();
        if(x.size() != y.size()) return path + ".size";
        for(int i = 0; i < x.size(); ++i)
        {
            const auto difference = FirstDifference(x[i], y[i], path + "[" + QString::number(i) + "]");
            if(!difference.isEmpty()) return difference;
        }
    }
    else if(a != b) return path;
    return {};
}
}

QString CanonicalModelDataSha256(const QJsonValue& data)
{
    QByteArray canonical; Canonical(canonical, data);
    return QString::fromLatin1(QCryptographicHash::hash(canonical, QCryptographicHash::Sha256).toHex());
}

QJsonObject IdentifyModel(UEPtr<UContainer> model, const ModelBoundarySources& boundarySources)
{
    const int version = boundarySources.empty() ? 1 : 2;
    QJsonObject result; result["schema_version"] = version; result["kind"] = "semantic_model_identity";
    result["canonical_encoding"] = "length-framed-utf8-binary64-v1";
    QJsonArray diagnostics, serializedNodes, bindings, caches, ports;
    try
    {
        std::vector<Node> nodes; std::set<const UContainer*> seen;
        Collect(model, "$", nodes, seen);
        std::map<const UIProperty*, QString> propertyPaths;
        for(const auto& node : nodes)
            for(const auto& property : node.component->GetPropertiesList())
                propertyPaths[property.second.Property.Get()] = node.path + ":" + QString::fromStdString(property.first);
        std::map<QString, QJsonObject> orderedPorts;
        std::set<const UIProperty*> usedPorts;
        for(const auto& boundary : boundarySources)
        {
            const auto* source = boundary.first;
            const auto name = boundary.second;
            if(!source || name.trimmed().isEmpty() || orderedPorts.count(name) || propertyPaths.count(source) ||
               !(source->GetType() & ptOutput) || !dynamic_cast<const UIPropertyOutput*>(source) ||
               !dynamic_cast<const UVBaseDataProperty<MDMatrix<double>>*>(source))
                throw std::runtime_error("invalid_or_unsupported_boundary_source");
            orderedPorts[name] = QJsonObject{{"id", name}, {"type", "matrix_binary64"}};
        }
        for(const auto& port : orderedPorts) ports.append(port.second);
        for(const auto& node : nodes)
        {
            auto component = node.component;
            if(!component->IsReady() || !component->IsInit())
                diagnostics.append(node.path + ": model_not_ready_or_initialized");
            if(component->GetTimeStep() == 0) diagnostics.append(node.path + ": invalid_frequency");
            if(auto neuron = dynamic_cast<NMSDK::NPulseNeuronCommon*>(component.Get()))
            {
                std::set<const NMSDK::NPulseMembraneCommon*> expected, actual;
                for(int i = 0; i < component->GetNumComponents(); ++i)
                    if(auto membrane = dynamic_pointer_cast<NMSDK::NPulseMembraneCommon>(component->GetComponentByIndex(i)))
                        expected.insert(membrane.Get());
                for(const auto* membrane : neuron->Membranes) actual.insert(membrane);
                if(actual != expected || actual.size() != neuron->Membranes.size())
                    diagnostics.append(node.path + ": incomplete_or_duplicate_membrane_index");
            }
            if(auto membrane = dynamic_cast<NMSDK::NPulseMembraneCommon*>(component.Get()))
            {
                std::set<const NMSDK::NPulseChannelCommon*> expected, actual;
                for(int i = 0; i < component->GetNumComponents(); ++i)
                    if(auto channel = dynamic_pointer_cast<NMSDK::NPulseChannelCommon>(component->GetComponentByIndex(i)))
                        expected.insert(channel.Get());
                for(size_t i = 0; i < membrane->GetNumChannels(); ++i) actual.insert(membrane->GetChannel(i));
                if(actual != expected || actual.size() != membrane->GetNumChannels())
                    diagnostics.append(node.path + ": incomplete_or_duplicate_channel_index");
            }
            QJsonObject object, parameters;
            object["path"] = node.path;
            object["class"] = QString::fromStdString(component->GetCompClassName());
            object["frequency_hz"] = QString::number(component->GetTimeStep());
            for(const auto& name : component->GetPropertiesOrder())
            {
                auto property = component->FindProperty(name);
                if(!property) throw std::runtime_error("missing_registered_property");
                const QString path = node.path + ":" + QString::fromStdString(name);
                auto input = dynamic_cast<UIPropertyInput*>(property.Get());
                const bool connected = input && (input->IsConnected() || input->UsesExternalDataSource());
                if((property->GetType() & ptParameter) && !Excluded(name))
                {
                    try
                    {
                        if(Sensitive(name)) throw std::runtime_error("excluded_credential_property");
                        // Bound parameters get their value from the source;
                        // sampling that source's dynamic value is not model identity.
                        QJsonObject parameter;
                        parameter["mask"] = QString::number(property->GetType());
                        parameter["binding"] = connected;
                        if(!connected) parameter["data"] = Value(property.Get());
                        parameters[QString::fromStdString(name)] = parameter;
                    }
                    catch(const std::exception& error) { diagnostics.append(path + ": " + error.what()); }
                }
                if(connected)
                {
                    const auto sources = input->GetDataSources();
                    if(!input->CanInspectDataSources() || sources.empty())
                        diagnostics.append(path + ": uninspectable_binding");
                    for(size_t i = 0; i < sources.size(); ++i)
                    {
                        const auto found = propertyPaths.find(sources[i]);
                        QJsonObject binding;
                        binding["target"] = path; binding["slot"] = QString::number(qulonglong(i));
                        if(found == propertyPaths.end())
                        {
                            auto boundary = boundarySources.find(sources[i]);
                            if(boundary == boundarySources.end() || input->UsesExternalDataSource())
                            { diagnostics.append(path + ": external_or_null_binding"); continue; }
                            binding["source"] = boundary->second;
                            binding["kind"] = "stimulus_port";
                            usedPorts.insert(sources[i]);
                        }
                        else
                        {
                            binding["source"] = found->second;
                            binding["kind"] = input->UsesExternalDataSource() ? "external_data_source" : "property_pointer";
                        }
                        bindings.append(binding);
                    }
                }
            }
            object["parameters"] = parameters;
            QJsonArray children; std::vector<NameT> names; component->GetComponentsList(names);
            for(const auto& name : names) children.append(QString::fromStdString(name));
            object["child_order"] = children;
            QJsonArray aliases;
            if(auto net = dynamic_pointer_cast<UNet>(component))
                for(const auto& alias : net->GetPropertyAliases())
                {
                    QJsonObject entry;
                    entry["name"] = QString::fromStdString(alias.first);
                    entry["component"] = QString::fromStdString(alias.second.ComponentPath);
                    entry["property"] = QString::fromStdString(alias.second.PropertyName);
                    entry["type"] = QString::number(alias.second.PropertyType); aliases.append(entry);
                }
            object["aliases"] = aliases;
            serializedNodes.append(object);
        }
        // The inspector additionally checks cached neuron/membrane pointers.
        if(usedPorts.size() != boundarySources.size()) diagnostics.append("unused_boundary_source");
        // Typed parameters/bindings were already read above. Inspect caches and
        // graph structure without serializing unrelated dynamic state again.
        const auto readback = InspectModel(model, boundarySources,ModelInspectionDetail::Structure);
        if(!readback["valid"].toBool())
            for(const auto& diagnostic : readback["diagnostics"].toArray()) diagnostics.append(diagnostic);
        const QString root = QString::fromStdString(model->GetName());
        const auto relative = [&root](const QJsonValue& value) -> QJsonValue {
            if(value.isNull()) return value;
            const auto path = value.toString();
            if(path != root && !path.startsWith(root + ".")) throw std::runtime_error("cache_outside_model");
            return "$" + path.mid(root.size());
        };
        for(const auto& entry : readback["nodes"].toArray())
        {
            const auto node = entry.toObject(); QJsonObject cache;
            cache["path"] = relative(node["path"]);
            for(const auto& key : {"cached_ltzone", "cached_positive_generator", "cached_negative_generator"})
                if(node.contains(key)) cache[key] = relative(node[key]);
            for(const auto& key : {"cached_membranes", "cached_channels"})
            {
                QJsonArray paths;
                for(const auto& path : node[key].toArray()) paths.append(relative(path));
                cache[key] = paths;
            }
            caches.append(cache);
        }
    }
    catch(const std::exception& error) { diagnostics.append(QString::fromUtf8(error.what())); }
    QJsonObject semantic;
    semantic["schema_version"] = version; semantic["nodes"] = serializedNodes;
    if(version == 2) semantic["stimulus_ports"] = ports;
    semantic["bindings"] = bindings; semantic["caches"] = caches;
    result["model"] = semantic; result["diagnostics"] = diagnostics; result["valid"] = diagnostics.isEmpty();
    if(diagnostics.isEmpty())
    {
        result["sha256"] = CanonicalModelDataSha256(semantic);
    }
    return result;
}

namespace {
using Match=std::function<bool()>;
template<class T> Match ScalarMatch(UIProperty* property)
{
    if(auto typed=dynamic_cast<UVBaseDataProperty<T>*>(property))
        return [typed,saved=T(typed->GetData())]{return typed->GetData()==saved;};
    return {};
}
template<class T> Match MatrixMatch(UIProperty* property)
{
    auto typed=dynamic_cast<UVBaseDataProperty<MDMatrix<T>>*>(property);if(!typed)return {};
    const auto& matrix=typed->GetData();const auto rows=matrix.GetRows(),cols=matrix.GetCols();
    std::vector<T> saved;for(int r=0;r<rows;++r)for(int c=0;c<cols;++c)saved.push_back(matrix(r,c));
    return [typed,rows,cols,saved=std::move(saved)] {
        const auto& now=typed->GetData();if(now.GetRows()!=rows || now.GetCols()!=cols)return false;
        std::size_t i=0;for(int r=0;r<rows;++r)for(int c=0;c<cols;++c)if(now(r,c)!=saved[i++])return false;
        return true;
    };
}
Match ParameterMatch(UIProperty* property)
{
    // Same supported typed domain as Value above. Unknown types fail closed;
    // neither this path nor the full identity uses formatted Rdk numbers.
    Match result;
    if((result=ScalarMatch<bool>(property)) || (result=ScalarMatch<double>(property)) ||
       (result=ScalarMatch<int>(property)) || (result=ScalarMatch<long>(property)) ||
       (result=ScalarMatch<long long>(property)) || (result=ScalarMatch<unsigned int>(property)) ||
       (result=ScalarMatch<unsigned long>(property)) || (result=ScalarMatch<unsigned long long>(property)) ||
       (result=ScalarMatch<std::string>(property)) ||
       (result=ScalarMatch<std::pair<std::string,std::string>>(property)) ||
       (result=ScalarMatch<std::vector<double>>(property)) || (result=ScalarMatch<std::vector<int>>(property)) ||
       (result=ScalarMatch<std::vector<std::string>>(property)) ||
       (result=MatrixMatch<double>(property)) || (result=MatrixMatch<int>(property)))return result;
    throw std::runtime_error("unsupported_observer_parameter_type");
}
bool SameAliases(const UNet::PropertyAliasMapT& a,const UNet::PropertyAliasMapT& b)
{
    if(a.size()!=b.size())return false;auto x=a.begin(),y=b.begin();
    for(;x!=a.end();++x,++y)
        if(x->first!=y->first || x->second.ComponentPath!=y->second.ComponentPath ||
           x->second.PropertyName!=y->second.PropertyName || x->second.PropertyType!=y->second.PropertyType)return false;
    return true;
}
struct ObservedProperty {
    std::string name;UIProperty* property=nullptr;unsigned mask=0;
    std::type_index implementationType{typeid(void)};
    UIPropertyInput* input=nullptr;
    bool connected=false,external=false,inspectable=false;
    std::vector<const UIProperty*> sources;
    Match parameter;
};
struct ObservedNode {
    UContainer* component=nullptr;UModuleLifecycleObservation lifetime;
    std::string className;UTime frequency;
    std::vector<std::string> order,childNames;
    std::vector<UContainer*> children;
    std::vector<std::pair<std::string,UIProperty*>> registered;
    std::vector<ObservedProperty> properties;
    UNet* net=nullptr;UNet::PropertyAliasMapT aliases;
    NMSDK::NPulseNeuronCommon* neuron=nullptr;
    NMSDK::NPulseMembraneCommon* membrane=nullptr;
    NMSDK::NPulseNeuron* pulseNeuron=nullptr;
    std::vector<NMSDK::NPulseMembraneCommon*> membranes;
    std::vector<NMSDK::NPulseChannelCommon*> channels;
    UContainer *lt=nullptr,*positive=nullptr,*negative=nullptr;
    Match finitePhysics;
};
struct ObservedPort {
    const UIProperty* property=nullptr;UContainer* owner=nullptr;
    UModuleLifecycleObservation lifetime;std::string name;unsigned mask=0;
};
}

struct ModelRevisionObserver::Impl {
    UContainer* root=nullptr;ModelBoundarySources ports;
    std::vector<ObservedNode> nodes;std::vector<ObservedPort> boundaries;
    QString digest;std::size_t fullReads=0,cacheHits=0;
    void Clear(){root=nullptr;nodes.clear();boundaries.clear();ports.clear();digest.clear();}
    bool Unchanged(UEPtr<UContainer> model,const ModelBoundarySources& actualPorts) const
    {
        if(root!=model.Get() || ports!=actualPorts || nodes.empty())return false;
        // Do not dereference ANY stored node/property until all named owners'
        // lifetimes and memberships have been checked. UEPtr is a raw pointer.
        for(const auto& n:nodes)if(!n.lifetime.IsAlive() || !n.lifetime.IsCurrent())return false;
        for(const auto& b:boundaries)if(!b.lifetime.IsAlive() || !b.lifetime.IsCurrent())return false;
        for(const auto& n:nodes)
        {
            auto c=n.component;
            std::vector<std::string> names;c->GetComponentsList(names);
            if(names!=n.childNames)return false;
            for(std::size_t i=0;i<names.size();++i)if(c->GetComponent(names[i],true).Get()!=n.children[i])return false;
            if(!c->IsReady() || !c->IsInit() || c->GetTimeStep()!=n.frequency ||
                c->GetCompClassName()!=n.className || c->GetPropertiesOrder()!=n.order)return false;
            const auto& actual=c->GetPropertiesList();if(actual.size()!=n.registered.size())return false;
            auto i=actual.begin();for(const auto& p:n.registered)
            {if(i->first!=p.first || i->second.Property.Get()!=p.second)return false;++i;}
            // FindProperty also covers registered aliases, not only the map.
            for(const auto& p:n.properties)if(c->FindProperty(p.name).Get()!=p.property)return false;
        }
        for(const auto& b:boundaries)
        {
            const auto found=b.owner->GetPropertiesList().find(b.name);
            if(found==b.owner->GetPropertiesList().end() || found->second.Property.Get()!=b.property)return false;
        }
        for(const auto& n:nodes)
        {
            for(const auto& p:n.properties)
            {
                if(p.property->GetType()!=p.mask || std::type_index(typeid(*p.property))!=p.implementationType)return false;
                if(p.input && (p.input->UsesExternalDataSource()!=p.external ||
                    (p.input->IsConnected() || p.input->UsesExternalDataSource())!=p.connected ||
                    p.input->CanInspectDataSources()!=p.inspectable || p.input->GetDataSources()!=p.sources))return false;
                if(p.parameter && !p.parameter())return false;
            }
            if(n.net && !SameAliases(n.net->GetPropertyAliases(),n.aliases))return false;
            if(n.neuron && (n.neuron->Membranes!=n.membranes || n.neuron->GetLTZone()!=n.lt))return false;
            if(n.membrane)
            {
                if(n.membrane->GetNumChannels()!=n.channels.size())return false;
                for(std::size_t i=0;i<n.channels.size();++i)if(n.membrane->GetChannel(i)!=n.channels[i])return false;
            }
            if(n.pulseNeuron && (n.pulseNeuron->GetPosGenerator()!=n.positive ||
                n.pulseNeuron->GetNegGenerator()!=n.negative))return false;
            if(n.finitePhysics && !n.finitePhysics())return false;
        }
        for(const auto& b:boundaries)if(b.property->GetType()!=b.mask)return false;
        return true;
    }
    void Capture(UEPtr<UContainer> model,const ModelBoundarySources& actualPorts,const QString& hash)
    {
        Clear();std::vector<Node> collected;std::set<const UContainer*> seen;Collect(model,"$",collected,seen);
        for(const auto& entry:collected)
        {
            auto c=entry.component.Get();ObservedNode n;n.component=c;n.lifetime=c->ObserveLifecycle();
            n.className=c->GetCompClassName();n.frequency=c->GetTimeStep();n.order=c->GetPropertiesOrder();
            c->GetComponentsList(n.childNames);
            for(const auto& name:n.childNames)n.children.push_back(c->GetComponent(name,true).Get());
            for(const auto& p:c->GetPropertiesList())n.registered.push_back({p.first,p.second.Property.Get()});
            for(const auto& name:n.order)
            {
                ObservedProperty p;p.name=name;p.property=c->FindProperty(name).Get();p.mask=p.property->GetType();
                p.implementationType=std::type_index(typeid(*p.property));
                p.input=dynamic_cast<UIPropertyInput*>(p.property);
                if(p.input)
                {
                    p.external=p.input->UsesExternalDataSource();p.connected=p.input->IsConnected() || p.external;
                    p.inspectable=p.input->CanInspectDataSources();p.sources=p.input->GetDataSources();
                }
                if((p.mask & ptParameter) && !Excluded(name) && !p.connected)p.parameter=ParameterMatch(p.property);
                n.properties.push_back(std::move(p));
            }
            n.net=dynamic_cast<UNet*>(c);if(n.net)n.aliases=n.net->GetPropertyAliases();
            n.neuron=dynamic_cast<NMSDK::NPulseNeuronCommon*>(c);
            if(n.neuron){n.membranes=n.neuron->Membranes;n.lt=n.neuron->GetLTZone();}
            n.membrane=dynamic_cast<NMSDK::NPulseMembraneCommon*>(c);
            if(n.membrane)for(std::size_t i=0;i<n.membrane->GetNumChannels();++i)n.channels.push_back(n.membrane->GetChannel(i));
            n.pulseNeuron=dynamic_cast<NMSDK::NPulseNeuron*>(c);
            if(n.pulseNeuron){n.positive=n.pulseNeuron->GetPosGenerator();n.negative=n.pulseNeuron->GetNegGenerator();}
            if(auto channel=dynamic_cast<NMSDK::NPulseChannel*>(c))n.finitePhysics=[channel] {
                return std::isfinite(channel->Capacity.GetData()) && std::isfinite(channel->Resistance.GetData()) &&
                    std::isfinite(channel->RestingResistance.GetData()) && std::isfinite(channel->TimeConstant.GetData());};
            if(auto synapse=dynamic_cast<NMSDK::NPulseSynapse*>(c))n.finitePhysics=[synapse] {
                return std::isfinite(synapse->Resistance.GetData()) && std::isfinite(synapse->SecretionTC.GetData()) &&
                    std::isfinite(synapse->DissociationTC.GetData()) && std::isfinite(synapse->TypicalPulseDuration.GetData()) &&
                    std::isfinite(synapse->InhibitionCoeff.GetData());};
            nodes.push_back(std::move(n));
        }
        for(const auto& p:actualPorts)
        {
            auto owner=p.first->GetOwner();if(!owner)throw std::runtime_error("observer_port_has_no_owner");
            boundaries.push_back({p.first,owner,owner->ObserveLifecycle(),p.first->GetName(),p.first->GetType()});
        }
        root=model.Get();ports=actualPorts;digest=hash;
    }
};
ModelRevisionObserver::ModelRevisionObserver():implementation(new Impl){}
ModelRevisionObserver::~ModelRevisionObserver()=default;
void ModelRevisionObserver::Clear(){implementation->Clear();}
std::size_t ModelRevisionObserver::FullReads() const{return implementation->fullReads;}
std::size_t ModelRevisionObserver::CacheHits() const{return implementation->cacheHits;}
QString ModelRevisionObserver::Read(UEPtr<UContainer> model,const ModelBoundarySources& ports,bool forceFull)
{
    auto& state=*implementation;
    if(!forceFull && state.Unchanged(model,ports)){++state.cacheHits;return state.digest;}
    ++state.fullReads;
    // Guard a previously declared source that has been destroyed. Never read
    // its type/name through a dangling pointer while refreshing the snapshot.
    if(state.ports==ports)for(const auto& b:state.boundaries)
        if(!b.lifetime.IsAlive()){state.Clear();throw std::runtime_error("stale_model_boundary_source");}
    state.Clear();const auto identity=IdentifyModel(model,ports);
    if(!identity["valid"].toBool())throw std::runtime_error("invalid_model_revision");
    state.Capture(model,ports,identity["sha256"].toString());return state.digest;
}

QJsonObject VerifyModelIdentity(const QJsonObject& expected, UEPtr<UContainer> model,
                               const ModelBoundarySources& boundarySources)
{
    QJsonObject result;
    result["schema_version"] = 1; result["kind"] = "model_identity_verification";
    result["valid"] = false; result["match"] = false;
    const int version = boundarySources.empty() ? 1 : 2;
    if(expected["schema_version"] != QJsonValue(version) ||
       expected["kind"] != QJsonValue("semantic_model_identity") ||
       expected["canonical_encoding"] != QJsonValue("length-framed-utf8-binary64-v1") ||
       expected["valid"] != QJsonValue(true) || !expected["diagnostics"].isArray() ||
       !expected["diagnostics"].toArray().isEmpty() || !expected["model"].isObject() ||
       expected["model"].toObject()["schema_version"] != QJsonValue(version))
    {
        result["reason"] = "invalid_expected_identity"; return result;
    }
    const auto semantic = expected["model"].toObject();
    if(!semantic["nodes"].isArray() || semantic["nodes"].toArray().isEmpty() ||
       !semantic["bindings"].isArray() || !semantic["caches"].isArray())
    {
        result["reason"] = "invalid_expected_identity"; return result;
    }
    QByteArray canonical;
    try { Canonical(canonical, semantic); }
    catch(const std::exception&)
    { result["reason"] = "invalid_expected_identity"; return result; }
    const auto recomputed = QString::fromLatin1(QCryptographicHash::hash(canonical, QCryptographicHash::Sha256).toHex());
    if(expected["sha256"].toString() != recomputed)
    {
        result["reason"] = "expected_digest_mismatch"; return result;
    }
    const auto actual = IdentifyModel(model, boundarySources);
    if(!actual["valid"].toBool())
    {
        result["reason"] = "actual_identity_invalid"; result["diagnostics"] = actual["diagnostics"]; return result;
    }
    result["valid"] = true;
    result["expected_sha256"] = recomputed; result["actual_sha256"] = actual["sha256"];
    const auto difference = FirstDifference(semantic, actual["model"], "model");
    const bool match = difference.isEmpty() && expected["sha256"] == actual["sha256"];
    result["match"] = match; result["reason"] = match ? "identical" : "model_mismatch";
    if(!difference.isEmpty()) result["first_difference"] = difference;
    return result;
}
}
