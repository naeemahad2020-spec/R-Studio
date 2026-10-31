#include "Logger.h"
#include <QFile>
#include <QMutex>
#include <QDateTime>
#include <QTextStream>
#include <QRegularExpression>

static QMutex g_mutex;
static QString g_path;

void Logger::init(const QString& filePath) { g_path = filePath; }

QString Logger::redact(const QString& text) {
    static const QRegularExpression url(
        R"((rtmps?://[^/\s]+/[^/\s]+/)[^\s?&]+)",
        QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression kv(
        R"(((?:stream[_-]?key|key|token|password|secret)\s*[=:]\s*)\S+)",
        QRegularExpression::CaseInsensitiveOption);
    QString out = text;
    out.replace(url, QStringLiteral("\\1[REDACTED]"));
    out.replace(kv, QStringLiteral("\\1[REDACTED]"));
    return out;
}

void Logger::log(const QString& level, const QString& message) {
    QMutexLocker lock(&g_mutex);
    if (g_path.isEmpty()) return;
    QFile f(g_path);
    if (!f.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream(&f) << QDateTime::currentDateTime().toString(Qt::ISODate)
                    << " [" << level << "] " << redact(message) << "\n";
}
