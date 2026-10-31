#include "Database.h"
#include <QSqlDatabase>
#include <QSqlQuery>

bool Database::open(const QString& filePath) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    if (!db.open()) return false;
    QSqlQuery q;
    return q.exec("CREATE TABLE IF NOT EXISTS settings(k TEXT PRIMARY KEY, v TEXT)");
}

QString Database::get(const QString& key, const QString& fallback) {
    QSqlQuery q;
    q.prepare("SELECT v FROM settings WHERE k = ?");
    q.addBindValue(key);
    if (q.exec() && q.next()) return q.value(0).toString();
    return fallback;
}

bool Database::set(const QString& key, const QString& value) {
    QSqlQuery q;
    q.prepare("INSERT INTO settings(k, v) VALUES(?, ?) "
              "ON CONFLICT(k) DO UPDATE SET v = excluded.v");
    q.addBindValue(key);
    q.addBindValue(value);
    return q.exec();
}
