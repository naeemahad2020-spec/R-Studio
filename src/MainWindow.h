#pragma once
#include <QMainWindow>
class QListWidget;
class QStackedWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
protected:
    void closeEvent(QCloseEvent* e) override;
private:
    void applyTheme(bool dark);
    QListWidget* nav_;
    QStackedWidget* pages_;
    bool dark_ = true;
};
