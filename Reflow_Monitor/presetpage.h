#ifndef PRESETPAGE_H
#define PRESETPAGE_H

#include <QWidget>
#include <QVector>

class QDoubleSpinBox;

class PresetPage : public QWidget
{
    Q_OBJECT
public:
    explicit PresetPage(QWidget *parent = nullptr);

    QVector<QVector<double>> presetData() const;
    QVector<double> thresholdData() const;

private slots:
    void savePresets();
    void resetPresets();

private:
    static const int NUM_CHANNELS = 5;
    static const int NUM_PRESETS = 12;

    QVector<QVector<QDoubleSpinBox*>> m_presetInputs;
    QVector<QDoubleSpinBox*> m_thresholdInputs;
};

#endif // PRESETPAGE_H
