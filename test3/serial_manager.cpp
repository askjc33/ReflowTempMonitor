// 引入对应的头文件，包含SerialManager类的声明、宏定义（如ZONE_COUNT）等
#include "serial_manager.h"

// 构造函数：初始化SerialManager对象，指定父对象（符合Qt父子对象内存管理机制）
// parent：Qt父对象指针，用于内存自动回收
SerialManager::SerialManager(QObject *parent)
    : QObject(parent)  // 调用父类QObject的构造函数，传递父对象
{
    // 连接串口的readyRead信号：当串口有可读数据时，触发当前对象的onReadyRead槽函数
    connect(&serial_, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    // 连接串口的errorOccurred信号：当串口发生错误时，触发当前对象的onErrorOccurred槽函数
    // 信号参数为QSerialPort::SerialPortError枚举，携带错误类型
    connect(&serial_, SIGNAL(errorOccurred(QSerialPort::SerialPortError)),
            this, SLOT(onErrorOccurred(QSerialPort::SerialPortError)));
}

// 扫描所有可用的串口设备
void SerialManager::scanPorts()
{
    // 定义存储串口信息的列表，PortItem应为自定义结构体（包含串口名称、描述等）
    QList<PortItem> ports;
    // 获取系统中所有可用的串口信息列表
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();//枚举所有可用串口

    // 遍历每个串口信息，提取关键参数
    for (int i = 0; i < infos.size(); ++i) {
        // 获取第i个串口的信息（const引用避免拷贝）
        const QSerialPortInfo &info = infos.at(i);
        // 初始化单个串口信息对象
        PortItem p;
        // 串口名称（如"COM1"、"/dev/ttyUSB0"）
        p.portName = info.portName();
        // 串口描述信息（设备厂商的描述）
        p.description = info.description();
        // 串口制造商信息
        p.manufacturer = info.manufacturer();
        // 串口的系统路径（仅类Unix系统有效）
        p.systemLocation = info.systemLocation();
        // 将当前串口信息添加到列表
        ports.append(p);
    }
    // 发射扫描完成信号，将扫描到的串口列表传递给外部
    emit scanFinished(ports);
}

// 连接指定串口
// portName：串口名称（如"COM3"）
// baudRate：波特率（如9600、115200）
// 返回值：true=连接成功，false=连接失败
bool SerialManager::connectPort(const QString &portName, qint32 baudRate)
{
    // 如果串口已打开，先关闭（避免重复连接）
    if (serial_.isOpen()) {
        serial_.close();
    }

    // 配置串口参数
    serial_.setPortName(portName);      // 设置串口名称
    serial_.setBaudRate(baudRate);      // 设置波特率
    serial_.setDataBits(QSerialPort::Data8);    // 设置数据位：8位
    serial_.setParity(QSerialPort::NoParity);   // 设置校验位：无校验
    serial_.setStopBits(QSerialPort::OneStop);  // 设置停止位：1位
    serial_.setFlowControl(QSerialPort::NoFlowControl);  // 设置流控：无流控

    // 以读写模式打开串口
    if (!serial_.open(QIODevice::ReadWrite)) {
        // 打开失败：发射连接失败信号，携带错误信息（UTF8编码兼容中文）
        emit connectFailed(QString::fromUtf8("打开串口失败：%1").arg(serial_.errorString()));
        return false;
    }

    // 打开成功：清空接收缓冲区、温度帧缓存、位置帧缓存
    rxBuffer_.clear();
    tempFrames_.clear();
    posFrames_.clear();
    // 发射连接成功信号，携带串口名称
    emit connectSuccess(portName);
    return true;
}

// 断开当前连接的串口
void SerialManager::disconnectPort()
{
    // 如果串口已打开，关闭串口
    if (serial_.isOpen()) {
        serial_.close();
    }
    // 清空缓存，避免残留数据干扰下次连接
    rxBuffer_.clear();
    tempFrames_.clear();
    posFrames_.clear();
    // 发射断开连接信号，通知外部状态变更
    emit disconnected();
}

// 检查串口是否处于连接状态
// 返回值：true=已连接，false=未连接
bool SerialManager::isConnected() const
{
    // 直接返回串口的打开状态
    return serial_.isOpen();
}

// 串口可读数据的槽函数：处理串口接收到的数据
void SerialManager::onReadyRead()
{
    // 将串口接收到的所有数据追加到接收缓冲区（QByteArray支持二进制数据）
    rxBuffer_.append(serial_.readAll());

    // 缓冲区保护：如果缓冲区超过1MB，清空并发射错误提示（防止内存溢出）
    if (rxBuffer_.size() > 1024 * 1024) {
        rxBuffer_.clear();
        emit notifyError(QString::fromUtf8("串口接收缓冲区异常过大，已自动清空。"));
        return;
    }

    // 循环解析缓冲区中的数据：按换行符(\n)分割，保证每次处理一整行
    while (true) {
        // 查找换行符的位置，定位一行数据的结束位置
        int idx = rxBuffer_.indexOf('\n');   //保证“一次刚好一整行”数据
        // 没有找到换行符：说明数据不完整，退出循环等待后续数据
        if (idx < 0) {
            break;
        }

        // 提取从缓冲区起始到换行符的字节（完整一行数据）
        QByteArray line = rxBuffer_.left(idx);
        // 从缓冲区中移除已处理的行（包括换行符），避免重复解析
        rxBuffer_.remove(0, idx + 1);

        // 将二进制数据转换为UTF8编码的字符串，并去除首尾空白（空格、制表符、回车等）
        QString text = QString::fromUtf8(line).trimmed();
        // 非空行才进行解析（过滤空行/无效行）
        if (!text.isEmpty()) {
            parseLine(text);
        }
    }
}

// 串口错误处理槽函数：处理串口发生的错误
// error：串口错误类型（QSerialPort::SerialPortError枚举）
void SerialManager::onErrorOccurred(QSerialPort::SerialPortError error)
{
    // 无错误：直接返回
    if (error == QSerialPort::NoError) {
        return;
    }

    // 严重错误处理：资源错误（如设备被拔）、设备未找到
    if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
        // 获取错误描述信息
        QString msg = serial_.errorString();
        // 如果串口还处于打开状态，强制关闭
        if (serial_.isOpen()) {
            serial_.close();
        }
        // 发射错误提示信号
        emit notifyError(msg);
        // 发射断开连接信号，通知外部状态变更
        emit disconnected();
    }
}

// 解析单行串口数据：按前缀分流到不同的解析函数（协议路由）
// line：去除空白后的单行字符串
void SerialManager::parseLine(const QString &line)  //协议解析，按前缀分流
{
    // 电池数据前缀：BATT/ BATTERY（不区分大小写）
    if (line.startsWith("BATT,", Qt::CaseInsensitive) ||
        line.startsWith("BATTERY,", Qt::CaseInsensitive)) {
        parseBattery(line);
        return;
    }

    // 温度帧前缀：TEMP（不区分大小写）
    if (line.startsWith("TEMP,", Qt::CaseInsensitive)) {
        parseTempFrame(line);
        return;
    }

    // 位置帧前缀：POS（不区分大小写）
    if (line.startsWith("POS,", Qt::CaseInsensitive)) {
        parsePosFrame(line);
        return;
    }
    // 注：无匹配前缀的行会被忽略，可根据需求补充日志/错误提示
}

// 解析电池数据行
// line：格式示例 "BATT,95" 或 "BATTERY,100"
void SerialManager::parseBattery(const QString &line)
{
    // 按逗号分割字符串，保留空项（防止分割后丢失字段）
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    // 字段数不足2个（前缀+数值）：无效数据，返回
    if (parts.size() < 2) {
        return;
    }

    // 提取第二个字段（电池数值），转换为整数
    bool ok = false;
    int v = parts.at(1).trimmed().toInt(&ok);
    // 转换成功：发射电池数据信号，传递数值
    if (ok) {
        emit batteryReceived(v);
    }
}

// 解析温度帧数据行
// line：格式示例 "TEMP,1,25.5,30.2,..."（1=序列号，后续为各区域温度）
void SerialManager::parseTempFrame(const QString &line)
{
    // 按逗号分割字符串，保留空项
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    // 字段数校验：前缀+序列号+ZONE_COUNT个温度值（ZONE_COUNT为区域数量宏定义）
    if (parts.size() != 2 + ZONE_COUNT) {
        return;
    }

    // 提取序列号（第二个字段），转换为整数
    bool seqOk = false;
    int seq = parts.at(1).trimmed().toInt(&seqOk);
    // 序列号转换失败：无效数据，返回
    if (!seqOk) {
        return;
    }

    // 初始化温度数组，预分配内存（提升性能）
    QVector<double> temps;
    temps.reserve(ZONE_COUNT);

    // 遍历所有温度字段（从第三个字段开始）
    for (int i = 0; i < ZONE_COUNT; ++i) {
        // 转换为浮点型温度值
        bool ok = false;
        double v = parts.at(2 + i).trimmed().toDouble(&ok);
        // 任意一个温度值转换失败：整帧无效，返回
        if (!ok) {
            return;
        }
        // 转换成功：添加到温度数组
        temps.append(v);
    }

    // 将温度帧存入哈希表（键=序列号，值=温度数组），用于后续同步
    tempFrames_[seq] = temps;
    // 尝试同步温度帧和位置帧（同序列号才会触发同步）
    trySyncFrame(seq);
    // 清理过期的帧数据，防止哈希表过大
    cleanupOldFrames(seq);
}

// 解析位置帧数据行
// line：格式示例 "POS,1,A,B,C,..."（1=序列号，后续为各区域位置标识）
void SerialManager::parsePosFrame(const QString &line)
{
    // 按逗号分割字符串，保留空项
    QStringList parts = line.split(',', Qt::KeepEmptyParts);
    // 字段数校验：前缀+序列号+ZONE_COUNT个位置值
    if (parts.size() != 2 + ZONE_COUNT) {
        return;
    }

    // 提取序列号（第二个字段），转换为整数
    bool seqOk = false;
    int seq = parts.at(1).trimmed().toInt(&seqOk);
    // 序列号转换失败：无效数据，返回
    if (!seqOk) {
        return;
    }

    // 初始化位置列表
    QStringList pos;
    // 遍历所有位置字段（从第三个字段开始）
    for (int i = 0; i < ZONE_COUNT; ++i) {
        // 提取位置字符串（去除空白），添加到列表
        pos.append(parts.at(2 + i).trimmed());
    }

    // 将位置帧存入哈希表（键=序列号，值=位置列表），用于后续同步
    posFrames_[seq] = pos;
    // 尝试同步温度帧和位置帧
    trySyncFrame(seq);
    // 清理过期的帧数据
    cleanupOldFrames(seq);
}

// 尝试同步同序列号的温度帧和位置帧
// seq：需要同步的序列号
void SerialManager::trySyncFrame(int seq)
{
    // 检查哈希表中是否同时存在该序列号的温度帧和位置帧
    if (!tempFrames_.contains(seq) || !posFrames_.contains(seq)) {
        return;
    }

    // 同步成功：发射同步帧信号，传递温度和位置数据
    // take(seq)：取出并从哈希表中移除该序列号的帧（避免重复发射）
    emit synchronizedFrameReceived(tempFrames_.take(seq), posFrames_.take(seq));
}

// 清理过期的帧数据：防止哈希表无限增长
// latestSeq：最新接收到的帧序列号（作为基准判断过期）
void SerialManager::cleanupOldFrames(int latestSeq)  //清理哈希表，只保留最近50个seq，超过200条就整体清空兜底
{
    // 清理温度帧哈希表：移除比最新序列号小50以上的帧
    QList<int> tempKeys = tempFrames_.keys();
    for (int i = 0; i < tempKeys.size(); ++i) {
        int k = tempKeys.at(i);
        if (k < latestSeq - 50) {
            tempFrames_.remove(k);
        }
    }

    // 清理位置帧哈希表：移除比最新序列号小50以上的帧
    QList<int> posKeys = posFrames_.keys();
    for (int i = 0; i < posKeys.size(); ++i) {
        int k = posKeys.at(i);
        if (k < latestSeq - 50) {
            posFrames_.remove(k);
        }
    }

    // 兜底保护：如果温度帧数量超过200，整体清空（防止极端情况内存泄漏）
    if (tempFrames_.size() > 200) {
        tempFrames_.clear();
    }

    // 兜底保护：如果位置帧数量超过200，整体清空
    if (posFrames_.size() > 200) {
        posFrames_.clear();
    }
}
