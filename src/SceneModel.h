#pragma once
#include <QList>
#include <QString>

struct SceneInfo { int id; QString name; };
struct SourceInfo { int id; int sceneId; QString type; QString name; };

// Scenes/sources persisted in SQLite. Only "display" sources exist so far.
namespace SceneModel {
void ensureSchema();
QList<SceneInfo> scenes();
int addScene(const QString& name);
void removeScene(int id);
QList<SourceInfo> sources(int sceneId);
int addSource(int sceneId, const QString& type, const QString& name);
void removeSource(int id);
}
