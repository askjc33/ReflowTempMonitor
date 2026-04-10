#pragma once

#include <QWidget>
#include <QVector>

class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QGridLayout;
class ReflowSettings;

// ===== 新增：温区设置页面 =====

class ZoneSettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ZoneSettingsPage(ReflowSettings *settings, QWidget *parent = nullptr);

signals:
    void requestSetStatusMessage(const QString &text);

private slots:
    void applySettings();
    void resetDefaults();
    void syncFromSettings();

private:
    void initUi();

    ReflowSettings *settings_;
    QVector<QDoubleSpinBox*> lengthEditors_;
    QVector<QDoubleSpinBox*> thresholdEditors_;
    QLabel *summaryLabel_;
    QPushButton *applyBtn_;
    QPushButton *resetBtn_;
};
