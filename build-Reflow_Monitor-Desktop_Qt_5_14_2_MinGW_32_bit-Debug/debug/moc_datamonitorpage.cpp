/****************************************************************************
** Meta object code from reading C++ file 'datamonitorpage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Reflow_Monitor/datamonitorpage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datamonitorpage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DataMonitorPage_t {
    QByteArrayData data[16];
    char stringdata0[242];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DataMonitorPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DataMonitorPage_t qt_meta_stringdata_DataMonitorPage = {
    {
QT_MOC_LITERAL(0, 0, 15), // "DataMonitorPage"
QT_MOC_LITERAL(1, 16, 23), // "requestBackToConnection"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 15), // "requestGoPreset"
QT_MOC_LITERAL(4, 57, 15), // "startMonitoring"
QT_MOC_LITERAL(5, 73, 14), // "stopMonitoring"
QT_MOC_LITERAL(6, 88, 9), // "clearData"
QT_MOC_LITERAL(7, 98, 22), // "onTemperaturesReceived"
QT_MOC_LITERAL(8, 121, 15), // "QVector<double>"
QT_MOC_LITERAL(9, 137, 6), // "values"
QT_MOC_LITERAL(10, 144, 17), // "onBatteryReceived"
QT_MOC_LITERAL(11, 162, 5), // "value"
QT_MOC_LITERAL(12, 168, 14), // "onDisconnected"
QT_MOC_LITERAL(13, 183, 16), // "loadPresetCurves"
QT_MOC_LITERAL(14, 200, 24), // "syncThresholdsFromPreset"
QT_MOC_LITERAL(15, 225, 16) // "disconnectDevice"

    },
    "DataMonitorPage\0requestBackToConnection\0"
    "\0requestGoPreset\0startMonitoring\0"
    "stopMonitoring\0clearData\0"
    "onTemperaturesReceived\0QVector<double>\0"
    "values\0onBatteryReceived\0value\0"
    "onDisconnected\0loadPresetCurves\0"
    "syncThresholdsFromPreset\0disconnectDevice"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DataMonitorPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    0,   70,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    0,   73,    2, 0x08 /* Private */,
       7,    1,   74,    2, 0x08 /* Private */,
      10,    1,   77,    2, 0x08 /* Private */,
      12,    0,   80,    2, 0x08 /* Private */,
      13,    0,   81,    2, 0x08 /* Private */,
      14,    0,   82,    2, 0x08 /* Private */,
      15,    0,   83,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DataMonitorPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DataMonitorPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestBackToConnection(); break;
        case 1: _t->requestGoPreset(); break;
        case 2: _t->startMonitoring(); break;
        case 3: _t->stopMonitoring(); break;
        case 4: _t->clearData(); break;
        case 5: _t->onTemperaturesReceived((*reinterpret_cast< const QVector<double>(*)>(_a[1]))); break;
        case 6: _t->onBatteryReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->onDisconnected(); break;
        case 8: _t->loadPresetCurves(); break;
        case 9: _t->syncThresholdsFromPreset(); break;
        case 10: _t->disconnectDevice(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DataMonitorPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataMonitorPage::requestBackToConnection)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DataMonitorPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DataMonitorPage::requestGoPreset)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DataMonitorPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DataMonitorPage.data,
    qt_meta_data_DataMonitorPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DataMonitorPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataMonitorPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DataMonitorPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DataMonitorPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void DataMonitorPage::requestBackToConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DataMonitorPage::requestGoPreset()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
