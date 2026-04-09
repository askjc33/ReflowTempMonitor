#include "pcbdatamanager.h"

PcbDataManager::PcbDataManager(QObject *parent)
    : QObject(parent)
{}

void PcbDataManager::registerPcb(const QString &id)
{
    if (m_records.contains(id)) return;

    PcbRecord rec;
    rec.id            = id;
    rec.currentChamber = -1;
    rec.enterTime     = QDateTime::currentDateTime();
    rec.isActive      = true;
    m_records[id]     = rec;

    emit pcbRegistered(id);
}

void PcbDataManager::updatePcbTemperature(const QString &id,
                                           const float chamberTemps[5],
                                           int currentChamber)
{
    if (!m_records.contains(id)) {
        registerPcb(id);  // 容错：未扫码但收到数据时自动注册
    }

    PcbRecord &rec = m_records[id];
    rec.currentChamber = currentChamber;

    TempSample sample;
    sample.timestamp      = QDateTime::currentMSecsSinceEpoch();
    sample.currentChamber = currentChamber;
    for (int i = 0; i < 5; ++i)
        sample.temps[i] = chamberTemps[i];

    rec.samples.append(sample);

    emit pcbDataUpdated(id);
}

QStringList PcbDataManager::getAllIds() const
{
    return m_records.keys();
}

const PcbRecord* PcbDataManager::getPcbRecord(const QString &id) const
{
    auto it = m_records.find(id);
    if (it == m_records.end()) return nullptr;
    return &it.value();
}

void PcbDataManager::markPcbLeft(const QString &id)
{
    if (m_records.contains(id)) {
        m_records[id].isActive      = false;
        m_records[id].currentChamber = -1;
        emit pcbLeft(id);
    }
}

void PcbDataManager::clearAll()
{
    m_records.clear();
}
