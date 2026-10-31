#include "RecordingEngine.h"
#include <QtTest>

class TestRecording : public QObject {
    Q_OBJECT
private slots:
    void argsContainEncoderAndOutput() {
        RecordingSettings s;
        s.encoder = "h264_nvenc"; s.outputPath = "C:/x/out.mkv"; s.fps = 60; s.bitrateKbps = 8000;
        const QStringList a = RecordingEngine::buildArgs(s);
        QVERIFY(a.contains("gdigrab"));
        QCOMPARE(a[a.indexOf("-framerate") + 1], QString("60"));
        QCOMPARE(a[a.indexOf("-c:v") + 1], QString("h264_nvenc"));
        QCOMPARE(a[a.indexOf("-b:v") + 1], QString("8000k"));
        QCOMPARE(a.last(), QString("C:/x/out.mkv"));
        QVERIFY(!a.contains("-preset"));
    }
    void x264GetsPreset() {
        RecordingSettings s; s.encoder = "libx264"; s.outputPath = "o.mkv";
        QVERIFY(RecordingEngine::buildArgs(s).contains("-preset"));
    }
    void parsesEncoders() {
        const QString out = " V....D h264_nvenc  NVIDIA NVENC H.264\n V....D libx264  x264\n V....D mpeg4 x\n";
        QCOMPARE(RecordingEngine::parseEncoders(out), QStringList({"h264_nvenc", "libx264"}));
    }
    void emptyWhenNone() {
        QVERIFY(RecordingEngine::parseEncoders("V....D mpeg4").isEmpty());
    }
};
QTEST_APPLESS_MAIN(TestRecording)
#include "test_recording.moc"
