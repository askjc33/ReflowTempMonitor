#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>
#include "view.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ConnectionPage* connectionPage() const { return connPage_; }
    TraceabilityPage* tracePage() const { return tracePage_; }

signals:
    void statusMessageChanged(const QString &text);
    void connectionStatusChanged(const QString &text, bool connected);
    void batteryLevelChanged(int value);

public slots:
    void switchToConnectionPage();
    void switchToTracePage();
    void updateStatusMessage(const QString &text);
    void updateConnectionStatus(const QString &text, bool connected);
    void updateBatteryLevel(int v);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void setupMenuBar();
    void setupStatusBar();
    void updateClock();

    QStackedWidget *stacked_ = nullptr;
    ConnectionPage *connPage_ = nullptr;
    TraceabilityPage *tracePage_ = nullptr;

    QLabel *msgLabel_ = nullptr;
    QLabel *connLabel_ = nullptr;
    QLabel *batteryLabel_ = nullptr;
    QLabel *timeLabel_ = nullptr;
    QTimer *clockTimer_ = nullptr;
};

#endif // MAINWINDOW_H
