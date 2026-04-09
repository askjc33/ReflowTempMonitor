#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QStackedWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QCloseEvent>
#include <QSignalBlocker>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QSet>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QPointF>
#include <algorithm>

QT_CHARTS_USE_NAMESPACE

static const QString APP_VERSION = "v3.1.0-pos-traceability-fixed";
static const int ZONE_COUNT = 12;

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

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr) : QObject(parent)
    {
        connect(&serial_, &QSerialPort::readyRead, this, &SerialManager::onReadyRead);
        connect(&serial_, &QSerialPort::errorOccurred, this, &SerialManager::onErrorOccurred);
    }

    struct PortItem {
        QString portName;
        QString description;
        QString manufacturer;
        QString systemLocation;
    };

    void scanPorts()
    {
        QList<PortItem> ports;
        const auto infos = QSerialPortInfo::availablePorts();
        for (const auto &info : infos) {
            PortItem p;
            p.portName = info.portName();
            p.description = info.description();
            p.manufacturer = info.manufacturer();
            p.systemLocation = info.systemLocation();
            ports.append(p);
        }
        emit scanFinished(ports);
    }

    bool connectPort(const QString &portName, qint32 baudRate = QSerialPort::Baud115200)
    {
        if (serial_.isOpen()) {
            serial_.close();
        }

        serial_.setPortName(portName);
        serial_.setBaudRate(baudRate);
        serial_.setDataBits(QSerialPort::Data8);
        serial_.setParity(QSerialPort::NoParity);
        serial_.setStopBits(QSerialPort::OneStop);
        serial_.setFlowControl(QSerialPort::NoFlowControl);

        if (!serial_.open(QIODevice::ReadWrite)) {
            emit connectFailed(QString("打开串口失败：%1").arg(serial_.errorString()));
            return false;
        }

        rxBuffer_.clear();
        tempFrames_.clear();
        posFrames_.clear();
        emit connectSuccess(portName);
        return true;
    }

    void disconnectPort()
    {
        if (serial_.isOpen()) {
            serial_.close();
        }
        rxBuffer_.clear();
        tempFrames_.clear();
        posFrames_.clear();
        emit disconnected();
    }

    bool isConnected() const
    {
        return serial_.isOpen();
    }

signals:
    void scanFinished(const QList<SerialManager::PortItem> &ports);
    void connectSuccess(const QString &portName);
    void connectFailed(const QString &errorMsg);
    void disconnected();
    void notifyError(const QString &errorMsg);
    void batteryReceived(int value);

    void synchronizedFrameReceived(const QVector<double> &temps, const QStringList &positions);

private slots:
    void onReadyRead()
    {
        rxBuffer_.append(serial_.readAll());

        // 防止异常情况下缓冲区无限增长
        if (rxBuffer_.size() > 1024 * 1024) {
            rxBuffer_.clear();
            emit notifyError("串口接收缓冲区异常过大，已自动清空。");
            return;
        }

        while (true) {
            int idx = rxBuffer_.indexOf('\n');
            if (idx < 0) break;

            QByteArray line = rxBuffer_.left(idx);
            rxBuffer_.remove(0, idx + 1);

            QString text = QString::fromUtf8(line).trimmed();
            if (!text.isEmpty()) {
                parseLine(text);
            }
        }
    }

    void onErrorOccurred(QSerialPort::SerialPortError error)
    {
        if (error == QSerialPort::NoError) return;

        if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
            QString msg = serial_.errorString();
            if (serial_.isOpen()) {
                serial_.close();
            }
            emit notifyError(msg);
            emit disconnected();
        }
    }

private:
    void parseLine(const QString &line)
    {
        if (line.startsWith("BATT,", Qt::CaseInsensitive) ||
            line.startsWith("BATTERY,", Qt::CaseInsensitive)) {
            parseBattery(line);
            return;
        }

        if (line.startsWith("TEMP,", Qt::CaseInsensitive)) {
            parseTempFrame(line);
            return;
        }

        if (line.startsWith("POS,", Qt::CaseInsensitive)) {
            parsePosFrame(line);
            return;
        }
    }

    void parseBattery(const QString &line)
    {
        QStringList parts = line.split(',', Qt::KeepEmptyParts);
        if (parts.size() < 2) return;

        bool ok = false;
        int v = parts[1].trimmed().toInt(&ok);
        if (ok) emit batteryReceived(v);
    }

    void parseTempFrame(const QString &line)
    {
        QStringList parts = line.split(',', Qt::KeepEmptyParts);
        if (parts.size() != 2 + ZONE_COUNT) return;

        bool seqOk = false;
        int seq = parts[1].trimmed().toInt(&seqOk);
        if (!seqOk) return;

        QVector<double> temps;
        temps.reserve(ZONE_COUNT);

        for (int i = 0; i < ZONE_COUNT; ++i) {
            bool ok = false;
            double v = parts[2 + i].trimmed().toDouble(&ok);
            if (!ok) return;
            temps.append(v);
        }

        tempFrames_[seq] = temps;
        trySyncFrame(seq);
        cleanupOldFrames(seq);
    }

    void parsePosFrame(const QString &line)
    {
        QStringList parts = line.split(',', Qt::KeepEmptyParts);
        if (parts.size() != 2 + ZONE_COUNT) return;

        bool seqOk = false;
        int seq = parts[1].trimmed().toInt(&seqOk);
        if (!seqOk) return;

        QStringList pos;
        pos.reserve(ZONE_COUNT);
        for (int i = 0; i < ZONE_COUNT; ++i) {
            pos.append(parts[2 + i].trimmed());
        }

        posFrames_[seq] = pos;
        trySyncFrame(seq);
        cleanupOldFrames(seq);
    }

    void trySyncFrame(int seq)
    {
        if (!tempFrames_.contains(seq) || !posFrames_.contains(seq)) return;
        emit synchronizedFrameReceived(tempFrames_.take(seq), posFrames_.take(seq));
    }

    void cleanupOldFrames(int latestSeq)
    {
        // 清理过旧的缓存帧，避免 map/hash 长期膨胀
        QList<int> tempKeys = tempFrames_.keys();
        for (int k : tempKeys) {
            if (k < latestSeq - 50) {
                tempFrames_.remove(k);
            }
        }

        QList<int> posKeys = posFrames_.keys();
        for (int k : posKeys) {
            if (k < latestSeq - 50) {
                posFrames_.remove(k);
            }
        }

        // 双保险：极端情况下强制收缩
        if (tempFrames_.size() > 200) {
            tempFrames_.clear();
        }
        if (posFrames_.size() > 200) {
            posFrames_.clear();
        }
    }

private:
    QSerialPort serial_;
    QByteArray rxBuffer_;

    QHash<int, QVector<double>> tempFrames_;
    QHash<int, QStringList> posFrames_;
};

class BoardTraceManager : public QObject
{
    Q_OBJECT
public:
    explicit BoardTraceManager(QObject *parent = nullptr) : QObject(parent)
    {
        updateTimer_.setInterval(200);   // 节流：最多每 200ms 更新一次 UI
        updateTimer_.setSingleShot(true);
        connect(&updateTimer_, &QTimer::timeout, this, &BoardTraceManager::recordsUpdated);
    }

//    QList<BoardRecord> allRecords() const
//    {
//        QList<BoardRecord> list;
//        list.reserve(activeBoards_.size() + historyBoards_.size());

//        for (const auto &r : activeBoards_) list.append(r);
//        for (const auto &r : historyBoards_) list.append(r);

//        std::sort(list.begin(), list.end(), [](const BoardRecord &a, const BoardRecord &b) {
//            return a.enterTime > b.enterTime;
//        });
//        return list;
//    }
    QList<BoardRecord> allRecords() const
    {
        // 1：先拆分两组
        QList<BoardRecord> finishedList;  // 已完成（旧板）
        QList<BoardRecord> activeList;    // 过炉中（新板）

        for (const auto &r : activeBoards_)
            activeList.append(r);
        for (const auto &r : historyBoards_)
            finishedList.append(r);

        // 2：已完成 = 旧板在前，按【进入时间从旧到新】正序排列
        std::sort(finishedList.begin(), finishedList.end(), [](const BoardRecord &a, const BoardRecord &b) {
            return a.enterTime < b.enterTime;  // 旧 → 新
        });

        // 3：过炉中 = 按【进入时间从旧到新】正序排在后面
        std::sort(activeList.begin(), activeList.end(), [](const BoardRecord &a, const BoardRecord &b) {
            return a.enterTime < b.enterTime;  // 旧 → 新
        });

        // 4：最终顺序：旧板(完成) → 新板(过炉中)
        QList<BoardRecord> list;
        list.reserve(finishedList.size() + activeList.size());
        list.append(finishedList);
        list.append(activeList);

        return list;
    }

    bool getRecordByBarcode(const QString &barcode, BoardRecord &out) const
    {
        if (activeBoards_.contains(barcode)) {
            out = activeBoards_.value(barcode);
            return true;
        }

        for (const auto &r : historyBoards_) {
            if (r.barcode == barcode) {
                out = r;
                return true;
            }
        }
        return false;
    }

    void clearFinished()
    {
        historyBoards_.clear();
        requestUiRefresh(true);
    }

signals:
    void recordsUpdated();

public slots:
    void onSynchronizedFrame(const QVector<double> &temps, const QStringList &positions)
    {
        if (temps.size() != ZONE_COUNT || positions.size() != ZONE_COUNT) return;

        const QDateTime now = QDateTime::currentDateTime();
        const qint64 nowMs = now.toMSecsSinceEpoch();

        QSet<QString> presentBoards;
        presentBoards.reserve(ZONE_COUNT);

        for (int zone = 0; zone < ZONE_COUNT; ++zone) {
            QString barcode = normalizeBarcode(positions[zone]);
            if (barcode.isEmpty()) continue;

            presentBoards.insert(barcode);

            if (!activeBoards_.contains(barcode)) {
                BoardRecord r;
                r.barcode = barcode;
                r.enterTime = now;
                r.status = "过炉中";
                r.startMs = nowMs;
                r.zoneTimes.resize(ZONE_COUNT);
                r.zoneTemps.resize(ZONE_COUNT);
                activeBoards_[barcode] = r;
            }

            BoardRecord &record = activeBoards_[barcode];
            double elapsed = (nowMs - record.startMs) / 1000.0;
            double temp = temps[zone];

            record.lastZone = zone + 1;
            record.missingFrames = 0;

            record.timeAxis.append(elapsed);
            record.fullTemps.append(temp);
            record.zoneAxis.append(zone + 1);

            record.zoneTimes[zone].append(elapsed);
            record.zoneTemps[zone].append(temp);
        }

        QList<QString> toFinish;

        for (auto it = activeBoards_.begin(); it != activeBoards_.end(); ++it) {
            if (!presentBoards.contains(it.key())) {
                it.value().missingFrames++;

                if (it.value().missingFrames >= finishMissingFrameThreshold_) {
                    BoardRecord finished = it.value();
                    finished.exitTime = now;
                    finished.status = "已完成";
                    finished.finished = true;
                    historyBoards_.prepend(finished);
                    toFinish.append(it.key());
                }
            }
        }

        for (const QString &key : toFinish) {
            activeBoards_.remove(key);
        }

        // 限制历史记录无限增长，防止长时间运行内存过大
        while (historyBoards_.size() > maxHistoryBoards_) {
            historyBoards_.removeLast();
        }

        requestUiRefresh(false);
    }

private:
    static QString normalizeBarcode(const QString &raw)
    {
        QString s = raw.trimmed();
        if (s.isEmpty()) return QString();
        if (s.compare("EMPTY", Qt::CaseInsensitive) == 0) return QString();
        if (s.compare("NULL", Qt::CaseInsensitive) == 0) return QString();
        if (s == "-") return QString();
        return s;
    }

    void requestUiRefresh(bool immediate)
    {
        if (immediate) {
            updateTimer_.stop();
            emit recordsUpdated();
            return;
        }

        if (!updateTimer_.isActive()) {
            updateTimer_.start();
        }
    }

private:
    QMap<QString, BoardRecord> activeBoards_;
    QList<BoardRecord> historyBoards_;
    const int finishMissingFrameThreshold_ = 5;
    const int maxHistoryBoards_ = 5000; // 长时间运行保护
    QTimer updateTimer_;
};

class ConnectionPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPage(SerialManager *serial, QWidget *parent = nullptr)
        : QWidget(parent), serial_(serial)
    {
        connect(serial_, &SerialManager::scanFinished, this, &ConnectionPage::onScanFinished);
        connect(serial_, &SerialManager::connectSuccess, this, &ConnectionPage::onConnectSuccess);
        connect(serial_, &SerialManager::connectFailed, this, &ConnectionPage::onConnectFailed);
        initUi();
    }

signals:
    void requestSwitchToTracePage();
    void requestSetStatusMessage(const QString &text);
    void requestSetConnectionStatus(const QString &text, bool connected);

private slots:
    void scan()
    {
        list_->clear();
        ports_.clear();
        selectedPort_.clear();
        connectBtn_->setEnabled(false);
        infoLabel_->setText("请先扫描串口设备");
        emit requestSetStatusMessage("正在扫描串口设备...");
        serial_->scanPorts();
    }

    void onScanFinished(const QList<SerialManager::PortItem> &ports)
    {
        ports_ = ports;
        if (ports.isEmpty()) {
            emit requestSetStatusMessage("未发现串口设备");
            return;
        }

        emit requestSetStatusMessage(QString("扫描完成，共发现 %1 个串口设备").arg(ports.size()));

        for (const auto &p : ports) {
            QString label = p.description.isEmpty() ? "未知设备" : p.description;
            auto *item = new QListWidgetItem(QString("%1 (%2)").arg(label, p.portName));
            item->setData(Qt::UserRole, p.portName);
            list_->addItem(item);
        }
    }

    void onItemClicked(QListWidgetItem *item)
    {
        selectedPort_ = item->data(Qt::UserRole).toString();
        connectBtn_->setEnabled(true);

        for (const auto &p : ports_) {
            if (p.portName == selectedPort_) {
                infoLabel_->setText(
                    QString("端口：%1\n描述：%2\n厂商：%3\n位置：%4")
                        .arg(p.portName)
                        .arg(p.description.isEmpty() ? "未知" : p.description)
                        .arg(p.manufacturer.isEmpty() ? "未知" : p.manufacturer)
                        .arg(p.systemLocation.isEmpty() ? "未知" : p.systemLocation)
                );
                break;
            }
        }
    }

    void doConnect()
    {
        if (selectedPort_.isEmpty()) return;
        emit requestSetStatusMessage("正在连接串口...");
        serial_->connectPort(selectedPort_, QSerialPort::Baud115200);
    }

    void onConnectSuccess(const QString &portName)
    {
        emit requestSetStatusMessage("串口已连接");
        emit requestSetConnectionStatus(QString("已连接 %1").arg(portName), true);
        emit requestSwitchToTracePage();
    }

    void onConnectFailed(const QString &msg)
    {
        QMessageBox::warning(this, "连接失败", msg);
        emit requestSetStatusMessage("连接失败");
    }

private:
    void initUi()
    {
        auto *layout = new QVBoxLayout(this);

        auto *title = new QLabel("串口连接");
        QFont f;
        f.setPointSize(12);
        f.setBold(true);
        title->setFont(f);
        layout->addWidget(title);

        auto *sep = new QFrame;
        sep->setFrameShape(QFrame::HLine);
        layout->addWidget(sep);

        auto *group = new QGroupBox("可用设备");
        auto *gLayout = new QVBoxLayout(group);

        list_ = new QListWidget;
        list_->setMinimumHeight(300);
        connect(list_, &QListWidget::itemClicked, this, &ConnectionPage::onItemClicked);
        gLayout->addWidget(list_);

        layout->addWidget(group);

        auto *infoGroup = new QGroupBox("设备信息");
        auto *infoLayout = new QVBoxLayout(infoGroup);
        infoLabel_ = new QLabel("请先扫描串口设备");
        infoLabel_->setWordWrap(true);
        infoLayout->addWidget(infoLabel_);
        layout->addWidget(infoGroup);

        auto *btnLayout = new QHBoxLayout;
        scanBtn_ = new QPushButton("扫描设备");
        connect(scanBtn_, &QPushButton::clicked, this, &ConnectionPage::scan);
        btnLayout->addWidget(scanBtn_);

        connectBtn_ = new QPushButton("连接");
        connectBtn_->setEnabled(false);
        connect(connectBtn_, &QPushButton::clicked, this, &ConnectionPage::doConnect);
        btnLayout->addWidget(connectBtn_);

        layout->addLayout(btnLayout);
    }

private:
    SerialManager *serial_;
    QList<SerialManager::PortItem> ports_;
    QString selectedPort_;

    QListWidget *list_ = nullptr;
    QLabel *infoLabel_ = nullptr;
    QPushButton *scanBtn_ = nullptr;
    QPushButton *connectBtn_ = nullptr;
};

class TraceabilityPage : public QWidget
{
    Q_OBJECT
public:
    explicit TraceabilityPage(SerialManager *serial, BoardTraceManager *manager, QWidget *parent = nullptr)
        : QWidget(parent), serial_(serial), manager_(manager)
    {
        connect(manager_, &BoardTraceManager::recordsUpdated, this, &TraceabilityPage::refreshView);
        connect(serial_, &SerialManager::batteryReceived, this, &TraceabilityPage::onBatteryReceived);
        connect(serial_, &SerialManager::notifyError, this, &TraceabilityPage::onSerialError);
        connect(serial_, &SerialManager::disconnected, this, &TraceabilityPage::onDisconnected);

        initUi();
        initChart();
    }

signals:
    void requestDisconnectAndBack();
    void requestSetStatusMessage(const QString &text);
    void requestSetConnectionStatus(const QString &text, bool connected);
    void requestSetBatteryLevel(int value);

private slots:
    void refreshView()
    {
        QList<BoardRecord> records = manager_->allRecords();

        table_->setUpdatesEnabled(false);
        {
            QSignalBlocker blocker(table_);
            table_->clearContents();
            table_->setRowCount(records.size());

            for (int row = 0; row < records.size(); ++row) {
                const BoardRecord &r = records[row];

                auto *itemBarcode = new QTableWidgetItem(r.barcode);
                itemBarcode->setData(Qt::UserRole, r.barcode);
                table_->setItem(row, 0, itemBarcode);

                table_->setItem(row, 1, new QTableWidgetItem(
                    r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "-"));

                table_->setItem(row, 2, new QTableWidgetItem(
                    r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "-"));

                table_->setItem(row, 3, new QTableWidgetItem(r.status));

                double duration = r.timeAxis.isEmpty() ? 0.0 : r.timeAxis.last();
                table_->setItem(row, 4, new QTableWidgetItem(QString::number(duration, 'f', 1)));

                table_->setItem(row, 5, new QTableWidgetItem(
                    r.lastZone > 0 ? QString::number(r.lastZone) : "-"));

//                table_->setItem(row, 6, new QTableWidgetItem(QString::number(r.timeAxis.size())));
            }
        }
        table_->setUpdatesEnabled(true);
        table_->viewport()->update();

        if (!selectedBarcode_.isEmpty()) {
            showBoard(selectedBarcode_);
            reselectRowByBarcode(selectedBarcode_);
        } else if (!records.isEmpty()) {
            selectedBarcode_ = records.first().barcode;
            showBoard(selectedBarcode_);
            reselectRowByBarcode(selectedBarcode_);
        } else {
            clearBoardDisplay();
        }
    }

    void onTableCellClicked(int row, int)
    {
        QTableWidgetItem *item = table_->item(row, 0);
        if (!item) return;
        selectedBarcode_ = item->data(Qt::UserRole).toString();
        showBoard(selectedBarcode_);
    }

    void onBatteryReceived(int value)
    {
        batteryLabel_->setText(QString("电量：%1%").arg(value));
        emit requestSetBatteryLevel(value);
    }

    void disconnectDevice()
    {
        serial_->disconnectPort();
    }

    void onSerialError(const QString &msg)
    {
        QMessageBox::warning(this, "串口错误", msg);
    }

    void onDisconnected()
    {
        emit requestSetStatusMessage("串口已断开");
        emit requestSetConnectionStatus("未连接", false);
        emit requestSetBatteryLevel(-1);
        selectedBarcode_.clear();
        clearBoardDisplay();
        table_->clearContents();
        table_->setRowCount(0);
        emit requestDisconnectAndBack();
    }

    void clearFinishedRecords()
    {
        manager_->clearFinished();
        emit requestSetStatusMessage("已清空完成记录");
    }

    void exportCurrentBoard()
    {
        if (selectedBarcode_.isEmpty()) {
            QMessageBox::information(this, "导出", "请先选择一块PCBA记录。");
            return;
        }

        BoardRecord r;
        if (!manager_->getRecordByBarcode(selectedBarcode_, r)) {
            QMessageBox::warning(this, "导出", "未找到当前板记录。");
            return;
        }

        QString path = QFileDialog::getSaveFileName(
            this,
            "导出当前板CSV",
            QString("%1.csv").arg(r.barcode),
            "CSV 文件 (*.csv)"
        );
        if (path.isEmpty()) return;

        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "导出失败", "无法写入文件。");
            return;
        }

        QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        out.setCodec("UTF-8");
#endif
        out << "Barcode," << r.barcode << "\n";
        out << "EnterTime," << (r.enterTime.isValid() ? r.enterTime.toString(Qt::ISODate) : "") << "\n";
        out << "ExitTime," << (r.exitTime.isValid() ? r.exitTime.toString(Qt::ISODate) : "") << "\n";
        out << "Status," << r.status << "\n\n";

        out << "ElapsedSec,Temp,Zone\n";
        const int n = std::min({r.timeAxis.size(), r.fullTemps.size(), r.zoneAxis.size()});
        for (int i = 0; i < n; ++i) {
            out << QString::number(r.timeAxis[i], 'f', 3) << ","
                << QString::number(r.fullTemps[i], 'f', 2) << ","
                << r.zoneAxis[i] << "\n";
        }

        file.close();
        QMessageBox::information(this, "导出成功", QString("已导出到：\n%1").arg(path));
    }

private:
    void initUi()
    {
        auto *layout = new QVBoxLayout(this);

        auto *top = new QHBoxLayout;

        auto *title = new QLabel("PCBA 过炉追溯");
        QFont titleFont;
        titleFont.setPointSize(12);
        titleFont.setBold(true);
        title->setFont(titleFont);
        top->addWidget(title);

        top->addStretch();

        batteryLabel_ = new QLabel("电量：--");
        top->addWidget(batteryLabel_);

        auto *exportBtn = new QPushButton("导出当前板CSV");
        connect(exportBtn, &QPushButton::clicked, this, &TraceabilityPage::exportCurrentBoard);
        top->addWidget(exportBtn);

        auto *clearBtn = new QPushButton("清空完成记录");
        connect(clearBtn, &QPushButton::clicked, this, &TraceabilityPage::clearFinishedRecords);
        top->addWidget(clearBtn);

        auto *disconnectBtn = new QPushButton("断开连接");
        connect(disconnectBtn, &QPushButton::clicked, this, &TraceabilityPage::disconnectDevice);
        top->addWidget(disconnectBtn);

        layout->addLayout(top);

        auto *sep = new QFrame;
        sep->setFrameShape(QFrame::HLine);
        layout->addWidget(sep);

        auto *infoGroup = new QGroupBox("当前选中板信息");
        auto *infoLayout = new QGridLayout(infoGroup);

        barcodeValue_ = new QLabel("--");
        statusValue_ = new QLabel("--");
        enterValue_ = new QLabel("--");
        exitValue_ = new QLabel("--");
        durationValue_ = new QLabel("--");
        zoneValue_ = new QLabel("--");

        infoLayout->addWidget(new QLabel("条码："), 0, 0);
        infoLayout->addWidget(barcodeValue_, 0, 1);
        infoLayout->addWidget(new QLabel("状态："), 0, 2);
        infoLayout->addWidget(statusValue_, 0, 3);
        infoLayout->addWidget(new QLabel("当前/最后气室："), 0, 4);
        infoLayout->addWidget(zoneValue_, 0, 5);

        infoLayout->addWidget(new QLabel("进板时间："), 1, 0);
        infoLayout->addWidget(enterValue_, 1, 1);
        infoLayout->addWidget(new QLabel("出板时间："), 1, 2);
        infoLayout->addWidget(exitValue_, 1, 3);
        infoLayout->addWidget(new QLabel("过炉时长："), 1, 4);
        infoLayout->addWidget(durationValue_, 1, 5);

        layout->addWidget(infoGroup);

        auto *chartGroup = new QGroupBox("温度曲线");
        auto *chartLayout = new QVBoxLayout(chartGroup);

        chartView_ = new QChartView;
        chartView_->setRenderHint(QPainter::Antialiasing);
        chartView_->setMinimumHeight(420);
        chartLayout->addWidget(chartView_);

        layout->addWidget(chartGroup, 1);

        auto *tableGroup = new QGroupBox("过炉记录列表");
        auto *tableLayout = new QVBoxLayout(tableGroup);

        table_ = new QTableWidget;
        table_->setColumnCount(6);  // 从 7 改成 6
        table_->setHorizontalHeaderLabels(
            QStringList() << "条码" << "进板时间" << "出板时间" << "状态" << "时长(s)" << "最后气室"); // 删掉了“点数”
        table_->horizontalHeader()->setStretchLastSection(true);

        // ========== 在这里设置列宽 ==========
        table_->setColumnWidth(0, 140);  // 条码
        table_->setColumnWidth(1, 180);  // 进板时间
        table_->setColumnWidth(2, 180);  // 出板时间
        table_->setColumnWidth(3, 90);   // 状态
        table_->setColumnWidth(4, 90);   // 时长

        table_->setSelectionBehavior(QAbstractItemView::SelectRows);
        table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table_->setAlternatingRowColors(true);
        table_->setSelectionMode(QAbstractItemView::SingleSelection);
        connect(table_, &QTableWidget::cellClicked, this, &TraceabilityPage::onTableCellClicked);

        tableLayout->addWidget(table_);
        layout->addWidget(tableGroup, 1);
    }

    void initChart()
    {
        chart_ = new QChart;
        chart_->legend()->setVisible(false);
        chart_->setTitle("选中 PCBA 的完整过炉温度曲线");

        series_ = new QLineSeries;
        QPen pen(QColor("#d32f2f"));
        pen.setWidth(2);
        series_->setPen(pen);

        chart_->addSeries(series_);

        axisX_ = new QValueAxis;
        axisY_ = new QValueAxis;

        axisX_->setTitleText("过炉时间 (s)");
        axisY_->setTitleText("温度 (°C)");
        axisX_->setRange(0, totalProcessTimeSec_);
        axisY_->setRange(0, 260);

        chart_->addAxis(axisX_, Qt::AlignBottom);
        chart_->addAxis(axisY_, Qt::AlignLeft);
        series_->attachAxis(axisX_);
        series_->attachAxis(axisY_);

        addZoneReferenceLines();
        chartView_->setChart(chart_);
    }

    void addZoneReferenceLines()
    {
        const double step = totalProcessTimeSec_ / double(ZONE_COUNT);

        for (int i = 0; i <= ZONE_COUNT; ++i) {
            auto *line = new QLineSeries;
            QPen pen(QColor("#888888"));
            pen.setStyle(Qt::DashLine);
            pen.setWidth(1);
            line->setPen(pen);

            double x = i * step;
            line->append(x, 0);
            line->append(x, 260);

            chart_->addSeries(line);
            line->attachAxis(axisX_);
            line->attachAxis(axisY_);
            zoneRefLines_.append(line);
        }
    }

    void showBoard(const QString &barcode)
    {
        BoardRecord r;
        if (!manager_->getRecordByBarcode(barcode, r)) {
            clearBoardDisplay();
            return;
        }

        barcodeValue_->setText(r.barcode);
        statusValue_->setText(r.status);
        enterValue_->setText(r.enterTime.isValid() ? r.enterTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
        exitValue_->setText(r.exitTime.isValid() ? r.exitTime.toString("yyyy-MM-dd HH:mm:ss") : "--");
        durationValue_->setText(r.timeAxis.isEmpty() ? "--" : QString("%1 s").arg(r.timeAxis.last(), 0, 'f', 1));
        zoneValue_->setText(r.lastZone > 0 ? QString::number(r.lastZone) : "--");

        QVector<QPointF> points;
        points.reserve(std::min(r.timeAxis.size(), r.fullTemps.size()));
        const int n = std::min(r.timeAxis.size(), r.fullTemps.size());
        for (int i = 0; i < n; ++i) {
            points.append(QPointF(r.timeAxis[i], r.fullTemps[i]));
        }

        series_->replace(points);

        double xMax = totalProcessTimeSec_;
        if (!r.timeAxis.isEmpty()) {
            xMax = std::max(totalProcessTimeSec_, r.timeAxis.last() + 5.0);
        }
        axisX_->setRange(0, xMax);

        emit requestSetStatusMessage(QString("已显示板 %1 的温度曲线").arg(r.barcode));
    }

    void clearBoardDisplay()
    {
        barcodeValue_->setText("--");
        statusValue_->setText("--");
        enterValue_->setText("--");
        exitValue_->setText("--");
        durationValue_->setText("--");
        zoneValue_->setText("--");
        if (series_) {
            QVector<QPointF> empty;
            series_->replace(empty);
        }
        axisX_->setRange(0, totalProcessTimeSec_);
    }

    void reselectRowByBarcode(const QString &barcode)
    {
        for (int row = 0; row < table_->rowCount(); ++row) {
            QTableWidgetItem *item = table_->item(row, 0);
            if (item && item->data(Qt::UserRole).toString() == barcode) {
                table_->selectRow(row);
                return;
            }
        }
    }

private:
    SerialManager *serial_;
    BoardTraceManager *manager_;

    QLabel *batteryLabel_ = nullptr;

    QLabel *barcodeValue_ = nullptr;
    QLabel *statusValue_ = nullptr;
    QLabel *enterValue_ = nullptr;
    QLabel *exitValue_ = nullptr;
    QLabel *durationValue_ = nullptr;
    QLabel *zoneValue_ = nullptr;

    QChartView *chartView_ = nullptr;
    QChart *chart_ = nullptr;
    QLineSeries *series_ = nullptr;
    QValueAxis *axisX_ = nullptr;
    QValueAxis *axisY_ = nullptr;
    QVector<QLineSeries*> zoneRefLines_;

    QTableWidget *table_ = nullptr;
    QString selectedBarcode_;

    const double totalProcessTimeSec_ = 120.0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        initUi();
        setupMenu();
        setupStatusBar();
        wireSignals();
    }

protected:
    void closeEvent(QCloseEvent *event) override
    {
        serial_.disconnectPort();
        QMainWindow::closeEvent(event);
    }

private slots:
    void switchToConnectionPage()
    {
        stacked_->setCurrentWidget(connPage_);
    }

    void switchToTracePage()
    {
        stacked_->setCurrentWidget(tracePage_);
    }

    void updateClock()
    {
        timeLabel_->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    }

    void setStatusMessage(const QString &text)
    {
        msgLabel_->setText(text);
    }

    void setConnectionStatus(const QString &text, bool connected)
    {
        connLabel_->setText(QString("● %1").arg(text));
        connLabel_->setStyleSheet(connected ? "color:green;font-weight:bold;" : "color:black;");
    }

    void setBatteryLevel(int v)
    {
        if (v < 0) {
            batteryLabel_->setText("电量：--");
            batteryLabel_->setStyleSheet("");
        } else {
            batteryLabel_->setText(QString("电量：%1%").arg(v));
            batteryLabel_->setStyleSheet(v <= 20 ? "color:red;" : "");
        }
    }

    void showAbout()
    {
        QMessageBox::about(
            this,
            "关于",
            QString("<b>PCBA 多板过炉追溯系统</b><br>"
                    "版本：%1<br><br>"
                    "协议：TEMP/POS 同步帧<br>"
                    "支持多板同时在炉、逐板温度曲线追溯。<br>"
                    "本版本已修复长时间运行后界面未响应问题。")
                .arg(APP_VERSION)
        );
    }

private:
    void initUi()
    {
        setWindowTitle("PCBA 多板过炉追溯系统");
        resize(1400, 900);

        stacked_ = new QStackedWidget(this);

        connPage_ = new ConnectionPage(&serial_);
        tracePage_ = new TraceabilityPage(&serial_, &traceManager_);

        stacked_->addWidget(connPage_);
        stacked_->addWidget(tracePage_);
        setCentralWidget(stacked_);
    }

    void wireSignals()
    {
        connect(connPage_, &ConnectionPage::requestSwitchToTracePage, this, &MainWindow::switchToTracePage);
        connect(connPage_, &ConnectionPage::requestSetStatusMessage, this, &MainWindow::setStatusMessage);
        connect(connPage_, &ConnectionPage::requestSetConnectionStatus, this, &MainWindow::setConnectionStatus);

        connect(tracePage_, &TraceabilityPage::requestDisconnectAndBack, this, &MainWindow::switchToConnectionPage);
        connect(tracePage_, &TraceabilityPage::requestSetStatusMessage, this, &MainWindow::setStatusMessage);
        connect(tracePage_, &TraceabilityPage::requestSetConnectionStatus, this, &MainWindow::setConnectionStatus);
        connect(tracePage_, &TraceabilityPage::requestSetBatteryLevel, this, &MainWindow::setBatteryLevel);

        connect(&serial_, &SerialManager::synchronizedFrameReceived,
                &traceManager_, &BoardTraceManager::onSynchronizedFrame);
    }

    void setupMenu()
    {
        QMenu *fileMenu = menuBar()->addMenu("文件(&F)");

        QAction *quitAct = new QAction("退出(&Q)", this);
        quitAct->setShortcut(QKeySequence("Ctrl+Q"));
        connect(quitAct, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(quitAct);

        QMenu *viewMenu = menuBar()->addMenu("视图(&V)");

        QAction *connAct = new QAction("连接页(&C)", this);
        connect(connAct, &QAction::triggered, this, &MainWindow::switchToConnectionPage);
        viewMenu->addAction(connAct);

        QAction *traceAct = new QAction("追溯页(&T)", this);
        connect(traceAct, &QAction::triggered, this, &MainWindow::switchToTracePage);
        viewMenu->addAction(traceAct);

        QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");

        QAction *aboutAct = new QAction("关于(&A)", this);
        connect(aboutAct, &QAction::triggered, this, &MainWindow::showAbout);
        helpMenu->addAction(aboutAct);
    }

    void setupStatusBar()
    {
        statusBar()->setSizeGripEnabled(true);

        msgLabel_ = new QLabel("就绪");
        statusBar()->addWidget(msgLabel_, 1);

        statusBar()->addPermanentWidget(new QLabel("│"));

        connLabel_ = new QLabel("● 未连接");
        statusBar()->addPermanentWidget(connLabel_);

        statusBar()->addPermanentWidget(new QLabel("│"));

        batteryLabel_ = new QLabel("电量：--");
        statusBar()->addPermanentWidget(batteryLabel_);

        statusBar()->addPermanentWidget(new QLabel("│"));

        QLabel *verLabel = new QLabel(APP_VERSION);
        statusBar()->addPermanentWidget(verLabel);

        statusBar()->addPermanentWidget(new QLabel("│"));

        timeLabel_ = new QLabel;
        statusBar()->addPermanentWidget(timeLabel_);

        clockTimer_ = new QTimer(this);
        connect(clockTimer_, &QTimer::timeout, this, &MainWindow::updateClock);
        clockTimer_->start(1000);
        updateClock();
    }

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

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
