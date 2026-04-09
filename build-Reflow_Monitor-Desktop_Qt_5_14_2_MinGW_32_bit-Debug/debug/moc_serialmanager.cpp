/****************************************************************************
** Meta object code from reading C++ file 'serialmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../Reflow_Monitor/serialmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'serialmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SerialManager_t {
    QByteArrayData data[18];
    char stringdata0[221];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialManager_t qt_meta_stringdata_SerialManager = {
    {
QT_MOC_LITERAL(0, 0, 13), // "SerialManager"
QT_MOC_LITERAL(1, 14, 9), // "connected"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 12), // "disconnected"
QT_MOC_LITERAL(4, 38, 16), // "connectionFailed"
QT_MOC_LITERAL(5, 55, 3), // "msg"
QT_MOC_LITERAL(6, 59, 15), // "rawDataReceived"
QT_MOC_LITERAL(7, 75, 4), // "data"
QT_MOC_LITERAL(8, 80, 15), // "batteryReceived"
QT_MOC_LITERAL(9, 96, 5), // "value"
QT_MOC_LITERAL(10, 102, 20), // "temperaturesReceived"
QT_MOC_LITERAL(11, 123, 15), // "QVector<double>"
QT_MOC_LITERAL(12, 139, 6), // "values"
QT_MOC_LITERAL(13, 146, 13), // "errorOccurred"
QT_MOC_LITERAL(14, 160, 11), // "onReadyRead"
QT_MOC_LITERAL(15, 172, 13), // "onSerialError"
QT_MOC_LITERAL(16, 186, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(17, 215, 5) // "error"

    },
    "SerialManager\0connected\0\0disconnected\0"
    "connectionFailed\0msg\0rawDataReceived\0"
    "data\0batteryReceived\0value\0"
    "temperaturesReceived\0QVector<double>\0"
    "values\0errorOccurred\0onReadyRead\0"
    "onSerialError\0QSerialPort::SerialPortError\0"
    "error"
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
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    1,   61,    2, 0x06 /* Public */,
       6,    1,   64,    2, 0x06 /* Public */,
       8,    1,   67,    2, 0x06 /* Public */,
      10,    1,   70,    2, 0x06 /* Public */,
      13,    1,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    0,   76,    2, 0x08 /* Private */,
      15,    1,   77,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QByteArray,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16,   17,

       0        // eod
};

void SerialManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->connectionFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->rawDataReceived((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 4: _t->batteryReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->temperaturesReceived((*reinterpret_cast< const QVector<double>(*)>(_a[1]))); break;
        case 6: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onReadyRead(); break;
        case 8: _t->onSerialError((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
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
            using _t = void (SerialManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::connected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::disconnected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::connectionFailed)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QByteArray & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::rawDataReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::batteryReceived)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QVector<double> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::temperaturesReceived)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (SerialManager::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialManager::errorOccurred)) {
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
void SerialManager::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SerialManager::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SerialManager::connectionFailed(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialManager::rawDataReceived(const QByteArray & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SerialManager::batteryReceived(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialManager::temperaturesReceived(const QVector<double> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void SerialManager::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
