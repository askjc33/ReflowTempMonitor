// 引入头文件：当前类的声明头文件
#include "board_trace_manager.h"
// 引入算法库：用于后续的排序操作
#include <algorithm>

// 构造函数：初始化板子追踪管理器，父对象为parent
// 核心作用：根据同步后的温度+位置帧，维护所有板子的过炉状态
BoardTraceManager::BoardTraceManager(QObject *parent)
    : QObject(parent)  // 初始化父类QObject
{
    // 设置UI刷新定时器间隔为200ms：合并200ms内的多次更新，避免UI频繁刷新
    updateTimer_.setInterval(200);
    // 设置定时器为单次触发：触发一次后自动停止，需手动重新启动
    updateTimer_.setSingleShot(true);
    // 连接定时器超时信号到自定义的recordsUpdated信号
    // 定时器超时后，触发UI刷新通知
    connect(&updateTimer_, SIGNAL(timeout()), this, SIGNAL(recordsUpdated()));
}

// 成员函数：获取所有板子记录（已完成+过炉中），按进入时间排序
QList<BoardRecord> BoardTraceManager::allRecords() const
{
    // 定义两个临时列表：存放已完成的板子记录
    QList<BoardRecord> finishedList;
    // 存放正在过炉的板子记录
    QList<BoardRecord> activeList;

    // 遍历活跃板子（过炉中）的map容器，将值存入activeList
    QMap<QString, BoardRecord>::const_iterator it1;
    for (it1 = activeBoards_.constBegin(); it1 != activeBoards_.constEnd(); ++it1) {
        activeList.append(it1.value());
    }

    // 遍历历史板子（已完成）的列表容器，将值存入finishedList
    for (int i = 0; i < historyBoards_.size(); ++i) {
        finishedList.append(historyBoards_.at(i));
    }

    // 对已完成列表按进入时间升序排序：lambda表达式作为排序规则
    std::sort(finishedList.begin(), finishedList.end(),
              [](const BoardRecord &a, const BoardRecord &b) {
                  // 早进入的排在前面
                  return a.enterTime < b.enterTime;
              });

    // 对活跃列表按进入时间升序排序：同上
    std::sort(activeList.begin(), activeList.end(),
              [](const BoardRecord &a, const BoardRecord &b) {
                  return a.enterTime < b.enterTime;
              });

    // 合并两个列表：先已完成、后过炉中
    QList<BoardRecord> list;
    // 预分配内存：避免频繁扩容，提升性能
    list.reserve(finishedList.size() + activeList.size());
    list.append(finishedList);
    list.append(activeList);

    // 返回合并后的完整列表
    return list;
}

// 成员函数：根据条码查询板子记录，找到则存入out并返回true，否则返回false
bool BoardTraceManager::getRecordByBarcode(const QString &barcode, BoardRecord &out) const
{
    // 先查活跃板子（过炉中），命中则赋值并返回true
    if (activeBoards_.contains(barcode)) {
        out = activeBoards_.value(barcode);
        return true;
    }

    // 活跃列表未命中，遍历历史板子（已完成）列表
    for (int i = 0; i < historyBoards_.size(); ++i) {
        const BoardRecord &r = historyBoards_.at(i);
        // 条码匹配则赋值并返回true
        if (r.barcode == barcode) {
            out = r;
            return true;
        }
    }

    // 未找到匹配记录，返回false
    return false;
}

// 成员函数：清空所有已完成的板子记录
void BoardTraceManager::clearFinished()
{
    // 清空历史板子列表
    historyBoards_.clear();
    // 立即触发UI刷新（参数immediate=true）
    requestUiRefresh(true);
}

// 核心入口函数：每收到一帧同步的温度+位置数据，执行一轮业务更新
void BoardTraceManager::onSynchronizedFrame(const QVector<double> &temps, const QStringList &positions)
{
    // 校验数据合法性：温度/位置数量需等于炉区数量（ZONE_COUNT），否则直接返回
    if (temps.size() != ZONE_COUNT || positions.size() != ZONE_COUNT) {
        return;
    }

    // 获取当前时间（QDateTime对象）
    const QDateTime now = QDateTime::currentDateTime();
    // 转换为毫秒级时间戳（从1970-01-01 00:00:00 UTC到现在的毫秒数）
    const qint64 nowMs = now.toMSecsSinceEpoch();

    // 定义集合：存放当前帧中出现的所有有效板子条码
    QSet<QString> presentBoards;

    // 遍历每个炉区（0~ZONE_COUNT-1）
    for (int zone = 0; zone < ZONE_COUNT; ++zone) {
        // 标准化条码：过滤空值、"empty"、"null"、"-"等无效值
        QString barcode = normalizeBarcode(positions.at(zone));
        // 条码无效则跳过当前炉区
        if (barcode.isEmpty()) {
            continue;
        }

        // 条码有效，加入当前帧存在的板子集合
        presentBoards.insert(barcode);
        // 检查是否是新板（活跃列表中无此条码）：首次出现则创建记录
        if (!activeBoards_.contains(barcode)) {
            BoardRecord r;               // 初始化空记录
            r.barcode = barcode;        // 赋值条码
            r.enterTime = now;          // 记录进入时间（当前帧时间）
            r.status = QString::fromUtf8("过炉中");  // 设置状态为“过炉中”
            r.startMs = nowMs;          // 记录进入时间戳（毫秒）
            r.zoneTimes.resize(ZONE_COUNT);  // 初始化每个炉区的时间轴数组
            r.zoneTemps.resize(ZONE_COUNT);  // 初始化每个炉区的温度数组
            activeBoards_[barcode] = r; // 将新记录加入活跃板子map
        }

        // 获取当前板子的活跃记录（引用，直接修改原数据）
        BoardRecord &record = activeBoards_[barcode];
        // 计算相对进板时间：当前时间 - 进板时间（转换为秒）
        double elapsed = (nowMs - record.startMs) / 1000.0;
        // 获取当前炉区的温度值
        double temp = temps.at(zone);

        // 更新板子最新所在炉区（+1：炉区从1开始显示，而非0）
        record.lastZone = zone + 1;
        // 重置丢失帧数：当前帧检测到板子，丢失帧计数清零
        record.missingFrames = 0;

        // 追加全局时间轴（相对进板时间）
        record.timeAxis.append(elapsed);
        // 追加全局温度数据
        record.fullTemps.append(temp);
        // 追加全局炉区轴（1~ZONE_COUNT）
        record.zoneAxis.append(zone + 1);

        // 追加当前炉区的时间轴数据
        record.zoneTimes[zone].append(elapsed);
        // 追加当前炉区的温度数据
        record.zoneTemps[zone].append(temp);
    }

    // 定义列表：存放需要标记为“已完成”的板子条码
    QList<QString> toFinish;

    // 遍历活跃板子列表，检查是否需要结束过炉流程
    QMap<QString, BoardRecord>::iterator it;
    for (it = activeBoards_.begin(); it != activeBoards_.end(); ++it) {
        // 当前帧中未检测到该板子（不在presentBoards集合中）
        if (!presentBoards.contains(it.key())) {
            // 丢失帧数+1
            it.value().missingFrames++;

            // 丢失帧数达到阈值：判定为板子已离开炉体，标记为完成
            if (it.value().missingFrames >= finishMissingFrameThreshold_) {
                BoardRecord finished = it.value();  // 拷贝当前记录
                finished.exitTime = now;            // 记录离开时间
                finished.status = QString::fromUtf8("已完成");  // 更新状态
                finished.finished = true;           // 标记为已完成
                historyBoards_.prepend(finished);   // 插入到历史列表头部（最新的在前面）
                toFinish.append(it.key());          // 加入待移除列表
            }
        }
    }

    // 从活跃列表中移除已完成的板子
    for (int i = 0; i < toFinish.size(); ++i) {
        activeBoards_.remove(toFinish.at(i));
    }

    // 限制历史记录最大数量：超过则移除最旧的（列表尾部）
    while (historyBoards_.size() > maxHistoryBoards_) {
        historyBoards_.removeLast();
    }

    // 请求UI刷新（非立即：合并200ms内的多次更新）
    requestUiRefresh(false);
}

// 辅助函数：标准化条码，过滤无效值
QString BoardTraceManager::normalizeBarcode(const QString &raw)
{
    // 去除首尾空格
    QString s = raw.trimmed();
    // 空字符串直接返回空
    if (s.isEmpty()) {
        return QString();
    }
    // 忽略大小写匹配"EMPTY"，返回空
    if (s.compare("EMPTY", Qt::CaseInsensitive) == 0) {
        return QString();
    }
    // 忽略大小写匹配"NULL"，返回空
    if (s.compare("NULL", Qt::CaseInsensitive) == 0) {
        return QString();
    }
    // 匹配"-"，返回空
    if (s == "-") {
        return QString();
    }
    // 有效条码，返回处理后的值
    return s;
}

// 辅助函数：请求UI刷新（控制刷新时机）
void BoardTraceManager::requestUiRefresh(bool immediate)
{
    // 立即刷新：停止定时器，直接触发刷新信号
    if (immediate) {
        updateTimer_.stop();
        emit recordsUpdated();
        return;
    }

    // 非立即刷新：定时器未激活时启动，200ms后触发刷新
    if (!updateTimer_.isActive()) {
        updateTimer_.start();
    }
}
