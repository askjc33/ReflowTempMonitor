#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QLabel;
class QTimer;
QT_END_NAMESPACE

#include "serial_manager.h"
#include "board_trace_manager.h"
#include "connection_page.h"
#include "traceability_page.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void switchToConnectionPage();
    void switchToTracePage();
    void updateClock();
    void setStatusMessage(const QString &text);
    void setConnectionStatus(const QString &text, bool connected);
    void setBatteryLevel(int v);
    void showAbout();

private:
    void initUi();
    void wireSignals();
    void setupMenu();
    void setupStatusBar();

private:
    SerialManager serial_;
    BoardTraceManager traceManager_;

    QStackedWidget *stacked_ = nullptr;
    ConnectionPage *connPage_ = nullptr;
    TraceabilityPage *tracePage_ = nullptr;

    QLabel *msgLabel_ = nullptr;
    QLabel *connLabel_ = nullptr;
    QLabel *batteryLabel_ = nullptr;
    QLabel *timeLabel_ = nullptr;
    QTimer *clockTimer_ = nullptr;
};
