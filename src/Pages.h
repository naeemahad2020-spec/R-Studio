#pragma once
#include "RecordingEngine.h"
#include <QWidget>
class QLabel; class QTimer; class QListWidget; class QComboBox;
class QLineEdit; class QSpinBox; class QPushButton;

class LivePage : public QWidget {
    Q_OBJECT
public:
    explicit LivePage(QWidget* parent = nullptr);
protected:
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;
private:
    void grab();
    QLabel* view_; QTimer* timer_;
};

class ScenesPage : public QWidget {
    Q_OBJECT
public:
    explicit ScenesPage(QWidget* parent = nullptr);
private:
    void reloadScenes();
    void reloadSources();
    int currentSceneId() const;
    QListWidget* scenes_; QListWidget* sources_;
};

class RecordingPage : public QWidget {
    Q_OBJECT
public:
    explicit RecordingPage(QWidget* parent = nullptr);
signals:
    void recordingChanged(bool active);
private:
    void detectEncoders();
    void updateDisk();
    void toggle();
    RecordingEngine* engine_;
    QLabel* ffmpegLabel_; QLabel* status_; QLabel* disk_;
    QLineEdit* folder_; QComboBox* encoder_; QComboBox* fps_;
    QSpinBox* bitrate_; QPushButton* button_;
};
