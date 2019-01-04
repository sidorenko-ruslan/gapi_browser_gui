#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore/QString>

enum class CommandType {
    Goto,
    ExecuteScript,
    CreatePdf,
    Unknown
};

struct ClientCommand {
    explicit ClientCommand(const QString& _type, const QString& _data): data(_data) {
        if (_type == "goto") {
            type = CommandType::Goto;
        }
        else if (_type == "script") {
            type = CommandType::ExecuteScript;
        }
        else if (_type == "pdf") {
            type = CommandType::CreatePdf;
        }
        else {
            type = CommandType::Unknown;
        }
    }
    CommandType type;
    QString data;
};

#endif // GLOBAL_H
