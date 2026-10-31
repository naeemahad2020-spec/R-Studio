#include "SceneModel.h"
#include <QSqlQuery>
#include <QVariant>

namespace SceneModel {
void ensureSchema() {
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS scenes(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL)");
    q.exec("CREATE TABLE IF NOT EXISTS sources(id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "scene_id INTEGER NOT NULL, type TEXT NOT NULL, name TEXT NOT NULL)");
    if (scenes().isEmpty()) addScene("Scene 1");
}
QList<SceneInfo> scenes() {
    QList<SceneInfo> out;
    QSqlQuery q("SELECT id, name FROM scenes ORDER BY id");
    while (q.next()) out.push_back({q.value(0).toInt(), q.value(1).toString()});
    return out;
}
int addScene(const QString& name) {
    QSqlQuery q;
    q.prepare("INSERT INTO scenes(name) VALUES(?)");
    q.addBindValue(name);
    return q.exec() ? q.lastInsertId().toInt() : -1;
}
void removeScene(int id) {
    QSqlQuery q;
    q.prepare("DELETE FROM sources WHERE scene_id = ?"); q.addBindValue(id); q.exec();
    q.prepare("DELETE FROM scenes WHERE id = ?"); q.addBindValue(id); q.exec();
}
QList<SourceInfo> sources(int sceneId) {
    QList<SourceInfo> out;
    QSqlQuery q;
    q.prepare("SELECT id, scene_id, type, name FROM sources WHERE scene_id = ? ORDER BY id");
    q.addBindValue(sceneId);
    if (q.exec())
        while (q.next())
            out.push_back({q.value(0).toInt(), q.value(1).toInt(),
                           q.value(2).toString(), q.value(3).toString()});
    return out;
}
int addSource(int sceneId, const QString& type, const QString& name) {
    QSqlQuery q;
    q.prepare("INSERT INTO sources(scene_id, type, name) VALUES(?,?,?)");
    q.addBindValue(sceneId); q.addBindValue(type); q.addBindValue(name);
    return q.exec() ? q.lastInsertId().toInt() : -1;
}
void removeSource(int id) {
    QSqlQuery q;
    q.prepare("DELETE FROM sources WHERE id = ?");
    q.addBindValue(id);
    q.exec();
}
}
