#pragma once
#include <QByteArray>
#include <QJsonObject>
#include <cstddef>
#include <string>

namespace NMSDK::TimeLearner {
// Strict object parsing, including duplicate/escaped keys and byte/depth limits.
// The returned JSON owns its data. Neither entry point retains caller buffers.
QJsonObject ParseStrictNativeJsonObject(const std::string& text,std::size_t maximumBytes=16*1024*1024);
// Direct file-byte path: no full QByteArray -> string -> QByteArray copies.
QJsonObject ParseStrictNativeJsonBytes(const QByteArray& bytes,std::size_t maximumBytes=16*1024*1024);
}
