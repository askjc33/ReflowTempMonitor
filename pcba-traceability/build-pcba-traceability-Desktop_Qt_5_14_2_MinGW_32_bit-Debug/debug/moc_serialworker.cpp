/****************************************************************************
** Meta object code from reading C++ file 'serialworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../pcba-traceability/serialworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialWorker_t {
    QByteArrayData data[24];
    char stringdata0[294];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialWorker_t qt_meta_stringdata_SerialWorker = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SerialWorker"
QT_MOC_LITERAL(1, 13, 12), // "scanFinished"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 15), // "QList<PortItem>"
QT_MOC_LITERAL(4, 43, 5), // "ports"
QT_MOC_LITERAL(5, 49, 14), // "connectSuccess"
QT_MOC_LITERAL(6, 64, 8), // "portName"
QT_MOC_LITERAL(7, 73, 13), // "connectFailed"
QT_MOC_LITERAL(8, 87, 8), // "errorMsg"
QT_MOC_LITERAL(9, 96, 12), // "disconnected"
QT_MOC_LITERAL(10, 109, 11), // "notifyError"
QT_MOC_LITERAL(11, 121, 15), // "batteryReceived"
QT_MOC_LITERAL(12, 137, 5), // "value"
QT_MOC_LITERAL(13, 143, 25), // "synchronizedFrameReceived"
QT_MOC_LITERAL(14, 169, 9), // "SyncFrame"
QT_MOC_LITERAL(15, 179, 5), // "frame"
QT_MOC_LITERAL(16, 185, 9), // "scanPorts"
QT_MOC_LITERAL(17, 195, 11), // "connectPort"
QT_MOC_LITERAL(18, 207, 8), // "baudRate"
QT_MOC_LITERAL(19, 216, 14), // "disconnectPort"
QT_MOC_LITERAL(20, 231, 11), // "onReadyRead"
QT_MOC_LITERAL(21, 243, 15), // "onErrorOccurred"
QT_MOC_LITERAL(22, 259, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(23, 288, 5) // "error"

    },
    "SerialWorker\0scanFinished\0\0QList<PortItem>\0"
    "ports\0connectSuccess\0portName\0"
    "connectFailed\0errorMsg\0disconnected\0"
    "notifyError\0batteryReceived\0value\0"
    "synchronizedFrameReceived\0SyncFrame\0"
    "frame\0scanPorts\0connectPort\0baudRate\0"
    "disconnectPort\0onReadyRead\0onErrorOccurred\0"
    "QSerialPort::SerialPortError\0error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       5,    1,   82,    2, 0x06 /* Public */,
       7,    1,   85,    2, 0x06 /* Public */,
       9,    0,   88,    2, 0x06 /* Public */,
      10,    1,   89,    2, 0x06 /* Public */,
      11,    1,   92,    2, 0x06 /* Public */,
      13,    1,   95,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      16,    0,   98,    2, 0x0a /* Public */,
      17,    2,   99,    2, 0x0a /* Public */,
      17,    1,  104,    2, 0x2a /* Public | MethodCloned */,
      19,    0,  107,    2, 0x0a /* Public */,
      20,    0,  108,    2, 0x08 /* Private */,
      21,    1,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, 0x80000000 | 14,   15,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    6,   18,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 22,   23,

       0        // eod
};

void SerialWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scanFinished((*reinterpret_cast< const QList<PortItem>(*)>(_a[1]))); break;
        case 1: _t->connectSuccess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->connectFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->disconnected(); break;
        case 4: _t->notifyError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->batteryReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->synchronizedFrameReceived((*reinterpret_cast< const SyncFrame(*)>(_a[1]))); break;
        case 7: _t->scanPorts(); break;
        case 8: _t->connectPort((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< qint32(*)>(_a[2]))); break;
        case 9: _t->connectPort((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->disconnectPort(); break;
        case 11: _t->onReadyRead(); break;
        case 12: _t->onErrorOccurred((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<PortItem> >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< SyncFrame >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SerialWorker::*)(const QList<PortItem> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::scanFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::connectSuccess)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::connectFailed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::disconnected)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::notifyError)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::batteryReceived)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const SyncFrame & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::synchronizedFrameReceived)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SerialWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SerialWorker.data,
    qt_meta_data_SerialWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SerialWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SerialWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void SerialWorker::scanFinished(const QList<PortItem> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialWorker::connectSuccess(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialWorker::connectFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialWorker::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SerialWorker::notifyError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialWorker::batteryReceived(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SerialWorker::synchronizedFrameReceived(const SyncFrame & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
