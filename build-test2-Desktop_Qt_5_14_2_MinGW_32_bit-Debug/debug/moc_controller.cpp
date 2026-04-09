/****************************************************************************
** Meta object code from reading C++ file 'controller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../test2/controller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Controller_t {
    QByteArrayData data[16];
    char stringdata0[235];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Controller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Controller_t qt_meta_stringdata_Controller = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Controller"
QT_MOC_LITERAL(1, 11, 22), // "onSerialConnectSuccess"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 8), // "portName"
QT_MOC_LITERAL(4, 44, 21), // "onSerialConnectFailed"
QT_MOC_LITERAL(5, 66, 3), // "msg"
QT_MOC_LITERAL(6, 70, 20), // "onSerialDisconnected"
QT_MOC_LITERAL(7, 91, 13), // "onSerialError"
QT_MOC_LITERAL(8, 105, 17), // "onBatteryReceived"
QT_MOC_LITERAL(9, 123, 5), // "value"
QT_MOC_LITERAL(10, 129, 16), // "onRecordsUpdated"
QT_MOC_LITERAL(11, 146, 15), // "onBoardSelected"
QT_MOC_LITERAL(12, 162, 7), // "barcode"
QT_MOC_LITERAL(13, 170, 17), // "onExportRequested"
QT_MOC_LITERAL(14, 188, 24), // "onClearFinishedRequested"
QT_MOC_LITERAL(15, 213, 21) // "onDisconnectRequested"

    },
    "Controller\0onSerialConnectSuccess\0\0"
    "portName\0onSerialConnectFailed\0msg\0"
    "onSerialDisconnected\0onSerialError\0"
    "onBatteryReceived\0value\0onRecordsUpdated\0"
    "onBoardSelected\0barcode\0onExportRequested\0"
    "onClearFinishedRequested\0onDisconnectRequested"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Controller[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x08 /* Private */,
       4,    1,   67,    2, 0x08 /* Private */,
       6,    0,   70,    2, 0x08 /* Private */,
       7,    1,   71,    2, 0x08 /* Private */,
       8,    1,   74,    2, 0x08 /* Private */,
      10,    0,   77,    2, 0x08 /* Private */,
      11,    1,   78,    2, 0x08 /* Private */,
      13,    0,   81,    2, 0x08 /* Private */,
      14,    0,   82,    2, 0x08 /* Private */,
      15,    0,   83,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Controller *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onSerialConnectSuccess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->onSerialConnectFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onSerialDisconnected(); break;
        case 3: _t->onSerialError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onBatteryReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onRecordsUpdated(); break;
        case 6: _t->onBoardSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onExportRequested(); break;
        case 8: _t->onClearFinishedRequested(); break;
        case 9: _t->onDisconnectRequested(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Controller::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Controller.data,
    qt_meta_data_Controller,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
