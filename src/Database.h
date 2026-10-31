#pragma once
#include <QString>

// Local SQLite store for settings/profiles. Never store stream keys here;
// credentials belong in Windows Credential Manager (Stage 4).
class Database {
public:
    static bool open(const QString& filePath);
    static QString get(const QString& key, const QString& fallback = {});
    static bool set(const QString& key, const QString& value);
};
