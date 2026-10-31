#include "MainWindow.h"
#include "Database.h"
#include "Logger.h"
#include "Pages.h"
#include "SceneModel.h"
#include "Theme.h"
#include <QApplication>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QVBoxLayout>

namespace {
struct Section { const char* name; int stage; const char* what; };
// "stage" = the master-prompt stage in which the feature will be built.
const Section kSections[] = {
    {"Home Dashboard", 1, "Overview of project status."},
    {"Live Studio", 2, "Preview/program output."},
    {"Scenes & Sources", 2, "Scenes, display/window/camera capture."},
    {"Audio Mixer", 3, "Mixer, meters, filters."},
    {"Stream Settings", 3, "Encoder, bitrate, resolution."},
    {"Multistream Manager", 6, "Multiple simultaneous destinations."},
    {"Social Media Destinations", 5, "Destination adapters (YouTube, Facebook, Twitch, RTMP...)."},
    {"Video Playlist", 7, "Playlists for automation."},
    {"24/7 Live Automation", 7, "Scheduled continuous broadcasting."},
    {"Recording Studio", 2, "Local recording."},
    {"Media Library", 3, "Media import and validation."},
    {"Graphics & Overlays", 8, "Logos, lower thirds, timers."},
    {"Virtual Camera", 8, "Virtual camera output."},
    {"Stream Health", 9, "Real dropped-frame/bitrate/CPU metrics."},
    {"Analytics", 9, "Official-API analytics only; otherwise 'Not available'."},
    {"Plugins", 9, "Plugin system with permission checks."},
    {"Settings & Profiles", 1, "Theme and profile storage (theme works now)."},
    {"Help & Diagnostics", 1, "Version info and log location."},
};
}

MainWindow::MainWindow() {
    setWindowTitle(QStringLiteral("R STUDIO"));
    resize(1200, 760);

    auto* central = new QWidget;
    auto* row = new QHBoxLayout(central);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(0);

    nav_ = new QListWidget;
    nav_->setFixedWidth(230);
    pages_ = new QStackedWidget;
    row->addWidget(nav_);
    row->addWidget(pages_, 1);

    for (const auto& s : kSections) {
        nav_->addItem(QString::fromUtf8(s.name));
        const QString secName = QString::fromUtf8(s.name);
        if (secName == "Live Studio") { pages_->addWidget(new LivePage); continue; }
        if (secName == "Scenes & Sources") { pages_->addWidget(new ScenesPage); continue; }
        if (secName == "Recording Studio") {
            auto* rp = new RecordingPage;
            connect(rp, &RecordingPage::recordingChanged, this, [this](bool on) {
                statusBar()->showMessage(on ? "Not streaming | RECORDING" : "Not streaming | Not recording");
            });
            pages_->addWidget(rp);
            continue;
        }
        auto* page = new QWidget;
        auto* v = new QVBoxLayout(page);
        auto* title = new QLabel(QString("<h2>%1</h2>").arg(s.name));
        auto* status = new QLabel(QString(
            "<b>Status:</b> %1<br>%2")
            .arg(s.stage == 1 ? "Basic version available"
                              : QString("Not implemented yet (planned for Stage %1)").arg(s.stage))
            .arg(s.what));
        status->setWordWrap(true);
        v->addWidget(title);
        v->addWidget(status);
        v->addStretch();

        if (QString(s.name) == "Settings & Profiles") {
            auto* btn = new QPushButton("Toggle dark / light theme");
            connect(btn, &QPushButton::clicked, this, [this] { applyTheme(!dark_); });
            v->insertWidget(2, btn);
        }
        if (QString(s.name) == "Help & Diagnostics") {
            v->insertWidget(2, new QLabel(QString("Version %1\nNo streaming, capture or "
                "recording features exist yet.").arg(RSTUDIO_VERSION)));
        }
        pages_->addWidget(page);
    }

    connect(nav_, &QListWidget::currentRowChanged, pages_, &QStackedWidget::setCurrentIndex);
    nav_->setCurrentRow(0);
    setCentralWidget(central);
    statusBar()->showMessage("Not streaming | Not recording");

    dark_ = Database::get("theme", "dark") == "dark";
    applyTheme(dark_);
    Logger::log("INFO", "Main window created");
}

void MainWindow::applyTheme(bool dark) {
    dark_ = dark;
    qApp->setStyleSheet(Theme::styleSheet(dark));
    Database::set("theme", dark ? "dark" : "light");
}

void MainWindow::closeEvent(QCloseEvent* e) {
    Logger::log("INFO", "Application closing");
    e->accept();
}
