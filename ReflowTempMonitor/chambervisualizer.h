#pragma once
#include <QWidget>

// 顶部气室位置可视化控件：5个气室横排，高亮当前所在气室
class ChamberVisualizer : public QWidget
{
    Q_OBJECT
public:
    explicit ChamberVisualizer(QWidget *parent = nullptr);

    // -1 = 不在任何气室
    void setCurrentChamber(int chamber);
    void setTemperatures(const float temps[5]);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int   m_current;      // 当前气室 0~4, -1=无
    float m_temps[5];     // 各气室温度
};
