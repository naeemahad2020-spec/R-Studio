#include "Logger.h"
#include <QtTest>

class TestLogger : public QObject {
    Q_OBJECT
private slots:
    void redactsRtmpKey() {
        QCOMPARE(Logger::redact("rtmp://a.rtmp.youtube.com/live2/abcd-1234-efgh"),
                 QString("rtmp://a.rtmp.youtube.com/live2/[REDACTED]"));
    }
    void redactsRtmpsKey() {
        QVERIFY(!Logger::redact("rtmps://live-api-s.facebook.com:443/rtmp/FB-123-xyz")
                     .contains("FB-123-xyz"));
    }
    void redactsKeyValue() {
        const QString out = Logger::redact("stream_key=SECRET123 password: hunter2");
        QVERIFY(!out.contains("SECRET123"));
        QVERIFY(!out.contains("hunter2"));
    }
    void leavesNormalTextAlone() {
        QCOMPARE(Logger::redact("Scene switched to Interview"),
                 QString("Scene switched to Interview"));
    }
};
QTEST_APPLESS_MAIN(TestLogger)
#include "test_logger.moc"
