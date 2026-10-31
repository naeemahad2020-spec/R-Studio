#pragma once
#include <QObject>
#include <QProcess>
#include <QStringList>
class QTimer;

struct RecordingSettings {
    QString ffmpegPath;
    QString encoder;     // h264_nvenc | h264_amf | h264_qsv | libx264
    QString outputPath;  // .mkv recommended (survives crashes)
    int fps = 30;
    int bitrateKbps = 6000;
};

// Records the desktop using an external ffmpeg process (gdigrab).
// Audio is NOT included yet (Stage 3).
class RecordingEngine : public QObject {
    Q_OBJECT
public:
    enum class State { Idle, Recording, Stopping, Failed };
    explicit RecordingEngine(QObject* parent = nullptr);

    static QStringList buildArgs(const RecordingSettings& s);
    static QStringList parseEncoders(const QString& ffmpegEncodersOutput);

    bool start(const RecordingSettings& s, QString* error);
    void stop();
    State state() const { return state_; }

signals:
    void stateChanged(RecordingEngine::State state, const QString& message);

private:
    void onFinished(int exitCode);
    QProcess* proc_;
    QTimer* killTimer_;
    State state_ = State::Idle;
    QString outputPath_;
    QString lastErr_;
};
