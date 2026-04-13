/****************************************************************************
** Meta object code from reading C++ file 'serial_manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../serial_manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serial_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialManager_t {
    QByteArrayData data[21];
    char stringdata0[280];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialManager_t qt_meta_stringdata_SerialManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "SerialManager"
QT_MOC_LITERAL(1, 14, 12), // "scanFinished"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 30), // "QList<SerialManager::PortItem>"
QT_MOC_LITERAL(4, 59, 5), // "ports"
QT_MOC_LITERAL(5, 65, 14), // "connectSuccess"
QT_MOC_LITERAL(6, 80, 8), // "portName"
QT_MOC_LITERAL(7, 89, 13), // "connectFailed"
QT_MOC_LITERAL(8, 103, 8), // "errorMsg"
QT_MOC_LITERAL(9, 112, 12), // "disconnected"
QT_MOC_LITERAL(10, 125, 11), // "notifyError"
QT_MOC_LITERAL(11, 137, 15), // "batteryReceived"
QT_MOC_LITERAL(12, 153, 5), // "value"
QT_MOC_LITERAL(13, 159, 25), // "synchronizedFrameReceived"
QT_MOC_LITERAL(14, 185, 15), // "QVector<double>"
QT_MOC_LITERAL(15, 201, 5), // "temps"
QT_MOC_LITERAL(16, 207, 9), // "positions"
QT_MOC_LITERAL(17, 217, 11), // "onReadyRead"
QT_MOC_LITERAL(18, 229, 15), // "onErrorOccurred"
QT_MOC_LITERAL(19, 245, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(20, 274, 5) // "error"

    },
    "SerialManager\0scanFinished\0\0"
    "QList<SerialManager::PortItem>\0ports\0"
    "connectSuccess\0portName\0connectFailed\0"
    "errorMsg\0disconnected\0notifyError\0"
    "batteryReceived\0value\0synchronizedFrameReceived\0"
    "QVector<double>\0temps\0positions\0"
    "onReadyRead\0onErrorOccurred\0"
    "QSerialPort::SerialPortError\0error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       5,    1,   62,    2, 0x06 /* Public */,
       7,    1,   65,    2, 0x06 /* Public */,
       9,    0,   68,    2, 0x06 /* Public */,
      10,    1,   69,    2, 0x06 /* Public */,
      11,    1,   72,    2, 0x06 /* Public */,
      13,    2,   75,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    0,   80,    2, 0x08 /* Private */,
      18,    1,   81,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, 0x80000000 | 14, QMetaType::QStringList,   15,   16,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 19,   20,

       0        // eod
};

void SerialManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scanFinished((*reinterpret_cast< const QList<SerialManager::PortItem>(*)>(_a[1]))); break;
        case 1: _t->connectSuccess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->connectFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->disconnected(); break;
        case 4: _t->notifyError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->batteryReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->synchronizedFrameReceived((*reinterpret_cast< const QVector<double>(*)>(_a[1])),(*reinterpret_cast< const QStringList(*)>(_a[2]))); break;
        case 7: _t->onReadyRead(); break;
        case 8: _t->onErrorOccurred((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
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
            using _t = void (SerialManager::*)(const QList<SerialManager::PortItem> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::scanFinished)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::connectSuccess)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::connectFailed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::disconnected)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::notifyError)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::batteryReceived)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QVector<double> & , const QStringList & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::synchronizedFrameReceived)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SerialManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_SerialManager.data,
    qt_meta_data_SerialManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SerialManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SerialManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SerialManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SerialManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void SerialManager::scanFinished(const QList<SerialManager::PortItem> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialManager::connectSuccess(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialManager::connectFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialManager::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SerialManager::notifyError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialManager::batteryReceived(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SerialManager::synchronizedFrameReceived(const QVector<double> & _t1, const QStringList & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
