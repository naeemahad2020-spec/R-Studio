#pragma once
#include "Database.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QStandardPaths>

// Looks for ffmpeg.exe: user-chosen path, next to RStudio.exe, then PATH.
inline QString findFfmpeg() {
    const QString saved = Database::get("ffmpeg_path");
    if (!saved.isEmpty() && QFileInfo::exists(saved)) return saved;
    const QString local = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
    if (QFileInfo::exists(local)) return local;
    return QStandardPaths::findExecutable("ffmpeg");
}
