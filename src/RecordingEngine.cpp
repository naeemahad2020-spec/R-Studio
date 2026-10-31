#include "RecordingEngine.h"
#include "Logger.h"
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QTimer>

RecordingEngine::RecordingEngine(QObject* parent) : QObject(parent) {
    proc_ = new QProcess(this);
    killTimer_ = new QTimer(this);
    killTimer_->setSingleShot(true);
    connect(killTimer_, &QTimer::timeout, this, [this] {
        Logger::log("WARN", "ffmpeg did not stop in time; killing");
        proc_->kill();
    });
    connect(proc_, &QProcess::readyReadStandardError, this, [this] {
        lastErr_ += QString::fromLocal8Bit(proc_->readAllStandardError());
        if (lastErr_.size() > 4000) lastErr_ = lastErr_.right(4000);
    });
    connect(proc_, &QProcess::finished, this,
            [this](int code, QProcess::ExitStatus) { onFinished(code); });
}

QStringList RecordingEngine::buildArgs(const RecordingSettings& s) {
    QStringList a{"-hide_banner", "-y", "-f", "gdigrab",
                  "-framerate", QString::number(s.fps), "-i", "desktop",
                  "-c:v", s.encoder,
                  "-b:v", QString::number(s.bitrateKbps) + "k",
                  "-pix_fmt", "yuv420p"};
    if (s.encoder == "libx264") a << "-preset" << "veryfast";
    a << s.outputPath;
    return a;
}

QStringList RecordingEngine::parseEncoders(const QString& out) {
    static const QStringList preferred{"h264_nvenc", "h264_amf", "h264_qsv", "libx264"};
    QStringList found;
    for (const QString& name : preferred) {
        QRegularExpression re("\\b" + QRegularExpression::escape(name) + "\\b");
        if (re.match(out).hasMatch()) found << name;
    }
    return found;
}

bool RecordingEngine::start(const RecordingSettings& s, QString* error) {
    auto fail = [&](const QString& m) { if (error) *error = m; return false; };
    if (state_ == State::Recording || state_ == State::Stopping)
        return fail("Recording is already running.");
    if (s.ffmpegPath.isEmpty() || !QFileInfo::exists(s.ffmpegPath))
        return fail("ffmpeg.exe was not found. Choose its location first.");
    if (s.encoder.isEmpty()) return fail("No encoder selected.");

    const QString dir = QFileInfo(s.outputPath).absolutePath();
    if (!QDir().mkpath(dir)) return fail("Cannot create the recording folder.");
    if (QStorageInfo(dir).bytesAvailable() < 1024LL * 1024 * 1024)
        return fail("Less than 1 GB of free disk space. Recording refused.");

    lastErr_.clear();
    outputPath_ = s.outputPath;
    proc_->setProgram(s.ffmpegPath);
    proc_->setArguments(buildArgs(s));
    proc_->start();
    if (!proc_->waitForStarted(3000)) return fail("ffmpeg failed to start.");

    state_ = State::Recording;
    Logger::log("INFO", "Recording started: " + outputPath_);
    emit stateChanged(state_, "Recording to " + outputPath_);
    return true;
}

void RecordingEngine::stop() {
    if (state_ != State::Recording) return;
    state_ = State::Stopping;
    emit stateChanged(state_, "Stopping and finalizing file...");
    proc_->write("q");          // ffmpeg's graceful-quit command
    killTimer_->start(10000);   // force-kill only if it hangs
}

void RecordingEngine::onFinished(int exitCode) {
    killTimer_->stop();
    const bool wasStopping = (state_ == State::Stopping);
    const QFileInfo fi(outputPath_);
    const bool fileOk = fi.exists() && fi.size() > 0;
    if ((wasStopping || exitCode == 0) && fileOk) {
        state_ = State::Idle;
        Logger::log("INFO", "Recording saved: " + outputPath_);
        emit stateChanged(state_, QString("Saved: %1 (%2 MB)")
                                      .arg(outputPath_).arg(fi.size() / 1024 / 1024));
    } else {
        state_ = State::Failed;
        const QString tail = Logger::redact(lastErr_.right(500));
        Logger::log("ERROR", "Recording failed: " + tail);
        emit stateChanged(state_, "Recording failed. " + tail);
    }
}
