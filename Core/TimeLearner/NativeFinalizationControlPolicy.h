#pragma once
#include "NativeFinalizationBoundary.h"
#include <QCryptographicHash>

namespace NMSDK::TimeLearner {
// Only the measurement/final threshold mirrors may change during finalization.
// All other public controls and their declared sources remain owned by the
// original request. This policy never writes into an external provider.
template<class Owner> void CheckNativeFinalizationControls(const Owner& owner,const QJsonObject& boundary)
{
    if(boundary.isEmpty())return;
    auto expected=boundary["controls"].toObject();auto current=ReadNativeFinalizationControls(owner);
    if(expected["parameters"].toObject()["SyncCapacitanceTrim"].toInt()!=0)
        throw std::logic_error("native_finalization_c_trim_not_yet_supported");
    if(!expected["parameters"].toObject()["IsNeedToTrain"].toBool())
        throw std::logic_error("native_finalization_requires_active_request");
    if(!owner.IsNeedToTrain.GetData())throw std::logic_error("native_finalization_external_stop");
    auto parameters=expected["parameters"].toObject(),actual=current["parameters"].toObject();
    for(const auto* name:{"UseFixedLTZThreshold","FixedLTZThreshold","LTZThreshold"})
    {parameters.remove(name);actual.remove(name);}
    if(actual!=parameters || current["bindings"]!=expected["bindings"])
        throw std::logic_error("native_finalization_controls_changed");
}

inline QJsonObject NativeFinalizationArchiveSemantics(QJsonObject archive)
{
    archive.remove("sha256");
    const auto snapshot=[](QJsonObject value) {
        const auto hash=[](const QString& text) {
            return QString::fromLatin1(QCryptographicHash::hash(text.toUtf8(),QCryptographicHash::Sha256).toHex());
        };
        auto xml=value["native_xml"].toString();
        if(value["native_xml_sha256"]!=QJsonValue(hash(xml)))throw std::logic_error("native_finalization_xml_digest_mismatch");
        if(value["component_class"]==QJsonValue("NPMembraneBio")) {
            xml.replace("<InputFeedbackSignal Type=\"MDMatrix&lt;d&gt;\" Rows=\"0\" Cols=\"0\" PType=\"266\" IoType=\"17\"></InputFeedbackSignal>",
                "<InputFeedbackSignal Type=\"MDMatrix&lt;d&gt;\" Rows=\"1\" Cols=\"1\" PType=\"266\" IoType=\"17\">0</InputFeedbackSignal>");
            value["native_xml"]=xml;value["native_xml_sha256"]=hash(xml);
        }
        return value;
    };
    archive["graph"]=snapshot(archive["graph"].toObject());
    auto definitions=archive["class_definitions"].toArray();
    for(int i=0;i<definitions.size();++i) {
        auto value=definitions[i].toObject();value["snapshot"]=snapshot(value["snapshot"].toObject());definitions[i]=value;
    }
    archive["class_definitions"]=definitions;return archive;
}

template<class Owner> void ValidateNativeFinalizationEntry(Owner& owner,const NativeFinalizationBoundary& boundary,
    const std::string& implementation)
{
    if(boundary.DiagnosticOnly())throw std::logic_error("native_finalization_requires_completed_boundary");
    if(boundary.Document()["implementation_id"].toString().toStdString()!=implementation)
        throw std::logic_error("native_finalization_implementation_mismatch");
    CheckNativeFinalizationControls(owner,boundary.Document());
    const auto fresh=CaptureNativeFinalizationBoundary(owner,implementation);
    auto expected=boundary.Solo(),actual=fresh.Solo();
    if(NativeFinalizationArchiveSemantics(expected["neuron"].toObject())!=
       NativeFinalizationArchiveSemantics(actual["neuron"].toObject()))
        throw std::logic_error("native_finalization_source_model_changed");
    // No tick is legal between capture and entry. Only the measured empty/zero
    // image feedback cache transition is normalized; all other XML stays exact.
    expected.remove("neuron");actual.remove("neuron");expected.remove("sha256");actual.remove("sha256");
    if(actual!=expected)throw std::logic_error("native_finalization_source_changed");
}

inline QJsonObject NativeFinalizationSourceSolo(QJsonObject package)
{
    if(package["kind"]==QJsonValue("timelearner_calibrated_recognition_state"))
        package=package["source_candidate"].toObject();
    if(package["kind"]==QJsonValue("timelearner_recognition_threshold_candidate"))
        package=package["source_recognition"].toObject();
    return package["source_solo"].toObject();
}
}
