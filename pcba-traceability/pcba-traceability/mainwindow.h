#pragma once

#include <QMainWindow>

class QStackedWidget;
class QLabel;
class QTimer;
class ConnectionPage;
class TraceabilityPage;
class QCloseEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    ConnectionPage *connectionPage() const;
    TraceabilityPage *tracePage() const;

public slots:
    void switchToConnectionPage();
    void switchToTracePage();
    void setStatusMessage(const QString &text);
    void setConnectionStatus(const QString &text, bool connected);
    void setBatteryLevel(int v);

signals:
    void windowClosing();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateClock();
    void showAbout();

private:
    void initUi();
    void setupMenu();
    void setupStatusBar();

private:
    QStackedWidget *stacked_ = nullptr;
    ConnectionPage *connPage_ = nullptr;
    TraceabilityPage *tracePage_ = nullptr;

    QLabel *msgLabel_ = nullptr;
    QLabel *connLabel_ = nullptr;
    QLabel *batteryLabel_ = nullptr;
    QLabel *timeLabel_ = nullptr;
    QTimer *clockTimer_ = nullptr;
};
