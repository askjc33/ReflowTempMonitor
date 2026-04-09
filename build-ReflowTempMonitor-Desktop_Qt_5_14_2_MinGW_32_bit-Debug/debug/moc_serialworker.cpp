/****************************************************************************
** Meta object code from reading C++ file 'serialworker.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../ReflowTempMonitor/serialworker.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
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
    QByteArrayData data[25];
    char stringdata0[259];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SerialWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SerialWorker_t qt_meta_stringdata_SerialWorker = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SerialWorker"
QT_MOC_LITERAL(1, 13, 19), // "temperatureReceived"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 5), // "pcbId"
QT_MOC_LITERAL(4, 40, 5), // "temp0"
QT_MOC_LITERAL(5, 46, 5), // "temp1"
QT_MOC_LITERAL(6, 52, 5), // "temp2"
QT_MOC_LITERAL(7, 58, 5), // "temp3"
QT_MOC_LITERAL(8, 64, 5), // "temp4"
QT_MOC_LITERAL(9, 70, 14), // "currentChamber"
QT_MOC_LITERAL(10, 85, 15), // "barcodeReceived"
QT_MOC_LITERAL(11, 101, 10), // "portOpened"
QT_MOC_LITERAL(12, 112, 7), // "success"
QT_MOC_LITERAL(13, 120, 3), // "msg"
QT_MOC_LITERAL(14, 124, 10), // "portClosed"
QT_MOC_LITERAL(15, 135, 13), // "errorOccurred"
QT_MOC_LITERAL(16, 149, 9), // "portError"
QT_MOC_LITERAL(17, 159, 8), // "openPort"
QT_MOC_LITERAL(18, 168, 8), // "portName"
QT_MOC_LITERAL(19, 177, 8), // "baudRate"
QT_MOC_LITERAL(20, 186, 9), // "closePort"
QT_MOC_LITERAL(21, 196, 11), // "onReadyRead"
QT_MOC_LITERAL(22, 208, 15), // "onErrorOccurred"
QT_MOC_LITERAL(23, 224, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(24, 253, 5) // "error"

    },
    "SerialWorker\0temperatureReceived\0\0"
    "pcbId\0temp0\0temp1\0temp2\0temp3\0temp4\0"
    "currentChamber\0barcodeReceived\0"
    "portOpened\0success\0msg\0portClosed\0"
    "errorOccurred\0portError\0openPort\0"
    "portName\0baudRate\0closePort\0onReadyRead\0"
    "onErrorOccurred\0QSerialPort::SerialPortError\0"
    "error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SerialWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    7,   64,    2, 0x06 /* Public */,
      10,    1,   79,    2, 0x06 /* Public */,
      11,    2,   82,    2, 0x06 /* Public */,
      14,    0,   87,    2, 0x06 /* Public */,
      15,    1,   88,    2, 0x06 /* Public */,
      16,    1,   91,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    2,   94,    2, 0x0a /* Public */,
      20,    0,   99,    2, 0x0a /* Public */,
      21,    0,  100,    2, 0x08 /* Private */,
      22,    1,  101,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Int,    3,    4,    5,    6,    7,    8,    9,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   12,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   13,
    QMetaType::Void, QMetaType::QString,   13,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   18,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 23,   24,

       0        // eod
};

void SerialWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SerialWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->temperatureReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 1: _t->barcodeReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->portOpened((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->portClosed(); break;
        case 4: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->portError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->openPort((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->closePort(); break;
        case 8: _t->onReadyRead(); break;
        case 9: _t->onErrorOccurred((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SerialWorker::*)(const QString & , float , float , float , float , float , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::temperatureReceived)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::barcodeReceived)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(bool , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::portOpened)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::portClosed)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::errorOccurred)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (SerialWorker::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SerialWorker::portError)) {
                *result = 5;
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
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void SerialWorker::temperatureReceived(const QString & _t1, float _t2, float _t3, float _t4, float _t5, float _t6, int _t7)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t6))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t7))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialWorker::barcodeReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SerialWorker::portOpened(bool _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SerialWorker::portClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SerialWorker::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void SerialWorker::portError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
