#include "StrictNativeJson.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <limits>
#include <set>
#include <stdexcept>

namespace NMSDK::TimeLearner {
namespace {
void Require(bool value,const char* reason){if(!value)throw std::invalid_argument(reason);}
// Qt validates syntax first. This second traversal preserves object-member
// multiplicity, which QJsonObject by itself would silently discard.
struct DuplicateKeys {
    const QByteArray& bytes;int position=0;
    void Space(){while(position<bytes.size() && (bytes[position]==' ' || bytes[position]=='\n' || bytes[position]=='\r' || bytes[position]=='\t'))++position;}
    QByteArray String(){const int start=position++;while(position<bytes.size()){const char c=bytes[position++];if(c=='\\')++position;else if(c=='"')break;}return bytes.mid(start,position-start);}
    void Value(unsigned depth=0)
    {
        Require(depth<=64,"native_configuration_depth_exceeded");Space();const char c=bytes.at(position);
        if(c=='{')
        {
            ++position;Space();std::set<QString> seen;if(bytes.at(position)=='}'){++position;return;}
            while(true)
            {
                Space();const auto key=QJsonDocument::fromJson(QByteArray("[")+String()+"]").array().at(0).toString();
                Require(seen.insert(key).second,"duplicate_native_configuration_key");Space();++position;Value(depth+1);Space();
                if(bytes.at(position++)=='}')break;
            }
        }
        else if(c=='[')
        {
            ++position;Space();if(bytes.at(position)==']'){++position;return;}
            while(true){Value(depth+1);Space();if(bytes.at(position++)==']')break;}
        }
        else if(c=='"')String();
        else while(position<bytes.size() && bytes[position]!=',' && bytes[position]!='}' && bytes[position]!=']')++position;
    }
};
}
QJsonObject ParseStrictNativeJsonObject(const std::string& text,std::size_t maximumBytes)
{
    Require(!text.empty() && text.size()<=maximumBytes && text.size()<=std::size_t(std::numeric_limits<int>::max()),"invalid_native_configuration_size");
    return ParseStrictNativeJsonBytes(QByteArray::fromRawData(text.data(),int(text.size())),maximumBytes);
}
QJsonObject ParseStrictNativeJsonBytes(const QByteArray& bytes,std::size_t maximumBytes)
{
    Require(!bytes.isEmpty() && std::size_t(bytes.size())<=maximumBytes &&
        std::size_t(bytes.size())<=std::size_t(std::numeric_limits<int>::max()),"invalid_native_configuration_size");
    QJsonParseError error;const auto document=QJsonDocument::fromJson(bytes,&error);
    Require(error.error==QJsonParseError::NoError && document.isObject(),"invalid_native_configuration_json");
    // Qt accepts a UTF-8 BOM. It must not let the duplicate-key traversal
    // mistake an otherwise valid object for a primitive and skip its members.
    DuplicateKeys scanner{bytes};if(bytes.startsWith("\xEF\xBB\xBF"))scanner.position=3;
    scanner.Value();scanner.Space();
    Require(scanner.position==bytes.size(),"native_configuration_json_trailing_data");
    return document.object();
}
}
