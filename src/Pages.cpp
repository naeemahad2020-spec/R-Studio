#include "Pages.h"
#include "Database.h"
#include "FFmpegLocator.h"
#include "Logger.h"
#include "SceneModel.h"
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QTimer>
#include <QVBoxLayout>

// ---------------- Live Studio ----------------
LivePage::LivePage(QWidget* p) : QWidget(p) {
    auto* v = new QVBoxLayout(this);
    v->addWidget(new QLabel("<h2>Live Studio</h2>"));
    auto* note = new QLabel("Preview of the primary screen. Scene composition, program "
                            "output and streaming are not implemented yet.");
    note->setWordWrap(true);
    v->addWidget(note);
    view_ = new QLabel;
    view_->setAlignment(Qt::AlignCenter);
    view_->setStyleSheet("background:#000;");
    view_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    view_->setMinimumSize(320, 180);
    v->addWidget(view_, 1);
    timer_ = new QTimer(this);
    timer_->setInterval(100);
    connect(timer_, &QTimer::timeout, this, &LivePage::grab);
}
void LivePage::showEvent(QShowEvent*) { timer_->start(); }
void LivePage::hideEvent(QHideEvent*) { timer_->stop(); }
void LivePage::grab() {
    QScreen* sc = QGuiApplication::primaryScreen();
    if (!sc) return;
    view_->setPixmap(sc->grabWindow(0).scaled(view_->size(), Qt::KeepAspectRatio,
                                              Qt::FastTransformation));
}

// ---------------- Scenes & Sources ----------------
ScenesPage::ScenesPage(QWidget* p) : QWidget(p) {
    auto* v = new QVBoxLayout(this);
    v->addWidget(new QLabel("<h2>Scenes & Sources</h2>"));
    auto* note = new QLabel("Scenes and sources are saved. Only 'Display Capture' exists so far; "
                            "other source types are not implemented yet (Stages 2-3).");
    note->setWordWrap(true);
    v->addWidget(note);

    auto* row = new QHBoxLayout;
    scenes_ = new QListWidget; sources_ = new QListWidget;
    auto* left = new QVBoxLayout; auto* right = new QVBoxLayout;
    left->addWidget(new QLabel("Scenes")); left->addWidget(scenes_);
    right->addWidget(new QLabel("Sources in selected scene")); right->addWidget(sources_);
    auto* addScene = new QPushButton("Add scene");
    auto* delScene = new QPushButton("Remove scene");
    auto* addSrc = new QPushButton("Add Display Capture");
    auto* delSrc = new QPushButton("Remove source");
    left->addWidget(addScene); left->addWidget(delScene);
    right->addWidget(addSrc); right->addWidget(delSrc);
    row->addLayout(left); row->addLayout(right);
    v->addLayout(row, 1);

    connect(scenes_, &QListWidget::currentRowChanged, this, [this] { reloadSources(); });
    connect(addScene, &QPushButton::clicked, this, [this] {
        bool ok = false;
        const QString n = QInputDialog::getText(this, "Add scene", "Scene name:",
                                                QLineEdit::Normal, "New scene", &ok).trimmed();
        if (ok && !n.isEmpty()) { SceneModel::addScene(n); reloadScenes(); }
    });
    connect(delScene, &QPushButton::clicked, this, [this] {
        if (scenes_->count() <= 1) {
            QMessageBox::information(this, "R STUDIO", "At least one scene must remain.");
            return;
        }
        const int id = currentSceneId();
        if (id >= 0) { SceneModel::removeScene(id); reloadScenes(); }
    });
    connect(addSrc, &QPushButton::clicked, this, [this] {
        const int id = currentSceneId();
        if (id >= 0) { SceneModel::addSource(id, "display", "Display Capture"); reloadSources(); }
    });
    connect(delSrc, &QPushButton::clicked, this, [this] {
        if (auto* it = sources_->currentItem()) {
            SceneModel::removeSource(it->data(Qt::UserRole).toInt());
            reloadSources();
        }
    });
    reloadScenes();
}
int ScenesPage::currentSceneId() const {
    auto* it = scenes_->currentItem();
    return it ? it->data(Qt::UserRole).toInt() : -1;
}
void ScenesPage::reloadScenes() {
    scenes_->clear();
    for (const auto& s : SceneModel::scenes()) {
        auto* it = new QListWidgetItem(s.name);
        it->setData(Qt::UserRole, s.id);
        scenes_->addItem(it);
    }
    if (scenes_->count() > 0) scenes_->setCurrentRow(0);
    reloadSources();
}
void ScenesPage::reloadSources() {
    sources_->clear();
    const int id = currentSceneId();
    if (id < 0) return;
    for (const auto& s : SceneModel::sources(id)) {
        auto* it = new QListWidgetItem(s.name + "  [" + s.type + "]");
        it->setData(Qt::UserRole, s.id);
        sources_->addItem(it);
    }
}

// ---------------- Recording Studio ----------------
RecordingPage::RecordingPage(QWidget* p) : QWidget(p) {
    engine_ = new RecordingEngine(this);
    auto* v = new QVBoxLayout(this);
    v->addWidget(new QLabel("<h2>Recording Studio</h2>"));
    auto* note = new QLabel("Records the whole desktop to an MKV file using ffmpeg. "
                            "No audio yet (Stage 3). Scenes are not composited into the "
                            "recording yet.");
    note->setWordWrap(true);
    v->addWidget(note);

    ffmpegLabel_ = new QLabel;
    auto* chooseFfmpeg = new QPushButton("Choose ffmpeg.exe...");
    auto* r1 = new QHBoxLayout; r1->addWidget(ffmpegLabel_, 1); r1->addWidget(chooseFfmpeg);
    v->addLayout(r1);

    folder_ = new QLineEdit(Database::get("rec_folder",
        QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/R STUDIO"));
    auto* chooseFolder = new QPushButton("Folder...");
    auto* r2 = new QHBoxLayout;
    r2->addWidget(new QLabel("Save to:")); r2->addWidget(folder_, 1); r2->addWidget(chooseFolder);
    v->addLayout(r2);

    encoder_ = new QComboBox; fps_ = new QComboBox; bitrate_ = new QSpinBox;
    fps_->addItems({"24", "30", "60"});
    fps_->setCurrentText(Database::get("rec_fps", "30"));
    bitrate_->setRange(1000, 50000); bitrate_->setSuffix(" kbps");
    bitrate_->setValue(Database::get("rec_bitrate", "6000").toInt());
    auto* r3 = new QHBoxLayout;
    r3->addWidget(new QLabel("Encoder:")); r3->addWidget(encoder_);
    r3->addWidget(new QLabel("FPS:")); r3->addWidget(fps_);
    r3->addWidget(new QLabel("Bitrate:")); r3->addWidget(bitrate_);
    r3->addStretch();
    v->addLayout(r3);

    button_ = new QPushButton("Start recording");
    status_ = new QLabel("Idle"); status_->setWordWrap(true);
    disk_ = new QLabel;
    v->addWidget(button_); v->addWidget(status_); v->addWidget(disk_);
    v->addStretch();

    connect(chooseFfmpeg, &QPushButton::clicked, this, [this] {
        const QString f = QFileDialog::getOpenFileName(this, "Select ffmpeg.exe", {},
                                                       "ffmpeg (ffmpeg.exe)");
        if (!f.isEmpty()) { Database::set("ffmpeg_path", f); detectEncoders(); }
    });
    connect(chooseFolder, &QPushButton::clicked, this, [this] {
        const QString d = QFileDialog::getExistingDirectory(this, "Recording folder",
                                                            folder_->text());
        if (!d.isEmpty()) folder_->setText(d);
    });
    connect(button_, &QPushButton::clicked, this, &RecordingPage::toggle);
    connect(engine_, &RecordingEngine::stateChanged, this,
            [this](RecordingEngine::State s, const QString& msg) {
        status_->setText(msg);
        button_->setEnabled(s != RecordingEngine::State::Stopping);
        const bool active = (s == RecordingEngine::State::Recording ||
                             s == RecordingEngine::State::Stopping);
        button_->setText(active ? "Stop recording" : "Start recording");
        emit recordingChanged(s == RecordingEngine::State::Recording);
    });
    auto* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, &RecordingPage::updateDisk);
    t->start(5000);
    updateDisk();
    detectEncoders();
}

void RecordingPage::detectEncoders() {
    const QString ff = findFfmpeg();
    encoder_->clear();
    if (ff.isEmpty()) {
        ffmpegLabel_->setText("ffmpeg.exe: NOT FOUND (put it next to RStudio.exe or choose it)");
        return;
    }
    ffmpegLabel_->setText("ffmpeg.exe: " + ff);
    auto* proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc] {
        const QStringList found = RecordingEngine::parseEncoders(
            QString::fromLocal8Bit(proc->readAllStandardOutput()));
        encoder_->addItems(found);
        if (found.isEmpty()) status_->setText("No supported H.264 encoder found in this ffmpeg.");
        proc->deleteLater();
    });
    proc->start(ff, {"-hide_banner", "-encoders"});
}

void RecordingPage::updateDisk() {
    const qint64 gb = QStorageInfo(folder_->text().isEmpty() ? "." : folder_->text())
                          .bytesAvailable() / (1024LL * 1024 * 1024);
    disk_->setText(QString("Free disk space: %1 GB%2").arg(gb)
                       .arg(gb < 5 ? "  -- LOW, recording may stop" : ""));
}

void RecordingPage::toggle() {
    if (engine_->state() == RecordingEngine::State::Recording) { engine_->stop(); return; }
    RecordingSettings s;
    s.ffmpegPath = findFfmpeg();
    s.encoder = encoder_->currentText();
    s.fps = fps_->currentText().toInt();
    s.bitrateKbps = bitrate_->value();
    s.outputPath = folder_->text() + "/RStudio_" +
                   QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".mkv";
    Database::set("rec_folder", folder_->text());
    Database::set("rec_fps", fps_->currentText());
    Database::set("rec_bitrate", QString::number(bitrate_->value()));
    QString err;
    if (!engine_->start(s, &err)) {
        status_->setText("Cannot start: " + err);
        Logger::log("ERROR", "Recording start refused: " + err);
    }
}
