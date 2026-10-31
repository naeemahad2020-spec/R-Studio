#pragma once
#include <QString>

// File logger that redacts stream keys/secrets BEFORE anything is written.
class Logger {
public:
    static void init(const QString& filePath);
    static void log(const QString& level, const QString& message);
    static QString redact(const QString& text);
};
