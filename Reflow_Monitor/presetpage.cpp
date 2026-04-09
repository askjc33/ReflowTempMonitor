#include "presetpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QFrame>
#include <QFont>

PresetPage::PresetPage(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel("温度预设设置");
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    title->setFont(font);
    layout->addWidget(title);

    QFrame *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    layout->addWidget(sep);

    QGroupBox *presetGroup = new QGroupBox("5通道温度预设值 (°C)");
    QVBoxLayout *presetLayout = new QVBoxLayout(presetGroup);

    for (int i = 0; i < NUM_CHANNELS; ++i) {
        QHBoxLayout *row = new QHBoxLayout;
        row->addWidget(new QLabel(QString("通道%1:").arg(i + 1)));
        QVector<QDoubleSpinBox*> rowInputs;
                for (int j = 0; j < NUM_PRESETS; ++j) {
                    QDoubleSpinBox *spin = new QDoubleSpinBox;
                    spin->setRange(-100.0, 500.0);
                    spin->setDecimals(1);
                    spin->setValue(25.0 + j * 5);
                    spin->setMinimumWidth(60);
                    row->addWidget(spin);
                    rowInputs.append(spin);
                }
                m_presetInputs.append(rowInputs);
                presetLayout->addLayout(row);
            }
            layout->addWidget(presetGroup);

            QGroupBox *thrGroup = new QGroupBox("12个气室温度阈值 (°C)");
            QHBoxLayout *thrLayout = new QHBoxLayout(thrGroup);
            thrLayout->addWidget(new QLabel("炉温阈值:"));
            for (int j = 0; j < NUM_PRESETS; ++j) {
                QDoubleSpinBox *spin = new QDoubleSpinBox;
                spin->setRange(-100.0, 500.0);
                spin->setDecimals(1);
                spin->setValue(150.0 + j * 5);
                spin->setMinimumWidth(60);
                thrLayout->addWidget(spin);
                m_thresholdInputs.append(spin);
            }
            layout->addWidget(thrGroup);

            QHBoxLayout *btnLayout = new QHBoxLayout;
            btnLayout->addStretch();
            QPushButton *saveBtn = new QPushButton("保存预设");
            QPushButton *resetBtn = new QPushButton("重置为默认");
            btnLayout->addWidget(saveBtn);
            btnLayout->addWidget(resetBtn);
            layout->addLayout(btnLayout);

            connect(saveBtn, &QPushButton::clicked, this, &PresetPage::savePresets);
            connect(resetBtn, &QPushButton::clicked, this, &PresetPage::resetPresets);
        }

QVector<QVector<double>> PresetPage::presetData() const
{
    QVector<QVector<double>> data;
    for (const auto &rowInputs : m_presetInputs) {
        QVector<double> row;
        for (auto *spin : rowInputs) row.append(spin->value());
        data.append(row);
    }
    return data;
}

QVector<double> PresetPage::thresholdData() const
{
    QVector<double> data;
    for (auto *spin : m_thresholdInputs) data.append(spin->value());
    return data;
}

void PresetPage::savePresets()
{
    QMessageBox::information(this, "保存成功", "温度预设值和温区阈值已保存。");
}

void PresetPage::resetPresets()
{
    for (int i = 0; i < NUM_CHANNELS; ++i) {
        for (int j = 0; j < NUM_PRESETS; ++j) {
            m_presetInputs[i][j]->setValue(25.0 + j * 5);
        }
    }
    for (int j = 0; j < NUM_PRESETS; ++j) {
        m_thresholdInputs[j]->setValue(150.0 + j * 5);
    }
    QMessageBox::information(this, "重置成功", "已重置为默认预设值和温区阈值。");
}
