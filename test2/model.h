#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QSet>

static const int ZONE_COUNT = 12;
static const QString APP_VERSION = "v3.1.0-pos-traceability-fixed";

// 数据结构：PCBA过炉记录
struct BoardRecord
{
    QString barcode;
    QDateTime enterTime;
    QDateTime exitTime;
    QString status;               // 过炉中 / 已完成

    qint64 startMs = 0;
    int lastZone = 0;
    int missingFrames = 0;
    bool finished = false;

    QVector<double> timeAxis;     // 完整过炉曲线时间轴
    QVector<double> fullTemps;    // 完整过炉曲线温度
    QVector<int> zoneAxis;        // 每个采样点来自哪个气室

    QVector<QVector<double>> zoneTimes; // 12个气室各自采样时间
    QVector<QVector<double>> zoneTemps; // 12个气室各自采样温度
};

// 数据管理类：负责记录的增删改查、排序、业务逻辑
class Model : public QObject
{
    Q_OBJECT
public:
    explicit Model(QObject *parent = nullptr);

    // 获取所有记录（已完成顶置，过炉中顺序排列）
    QList<BoardRecord> getAllRecords() const;
    // 根据条码获取单条记录
    bool getRecordByBarcode(const QString &barcode, BoardRecord &out) const;
    // 清空已完成记录
    void clearFinishedRecords();

signals:
    void recordsUpdated(); // 记录更新信号

public slots:
    // 处理同步帧数据（串口数据输入）
    void onSynchronizedFrame(const QVector<double> &temps, const QStringList &positions);

private:
    static QString normalizeBarcode(const QString &raw);
    void requestUiRefresh(bool immediate);

    QMap<QString, BoardRecord> activeBoards_;  // 正在过炉的板
    QList<BoardRecord> historyBoards_;         // 已完成的板
    QTimer updateTimer_;                       // UI刷新节流定时器
    const int finishMissingFrameThreshold_ = 5;
    const int maxHistoryBoards_ = 5000;
};

#endif // MODEL_H
