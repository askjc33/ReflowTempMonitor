/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../ReflowTempMonitor/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[28];
    char stringdata0[300];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 16), // "onConnectClicked"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12), // "onPortOpened"
QT_MOC_LITERAL(4, 42, 2), // "ok"
QT_MOC_LITERAL(5, 45, 3), // "msg"
QT_MOC_LITERAL(6, 49, 12), // "onPortClosed"
QT_MOC_LITERAL(7, 62, 11), // "onPortError"
QT_MOC_LITERAL(8, 74, 21), // "onTemperatureReceived"
QT_MOC_LITERAL(9, 96, 5), // "pcbId"
QT_MOC_LITERAL(10, 102, 2), // "t0"
QT_MOC_LITERAL(11, 105, 2), // "t1"
QT_MOC_LITERAL(12, 108, 2), // "t2"
QT_MOC_LITERAL(13, 111, 2), // "t3"
QT_MOC_LITERAL(14, 114, 2), // "t4"
QT_MOC_LITERAL(15, 117, 7), // "chamber"
QT_MOC_LITERAL(16, 125, 17), // "onBarcodeReceived"
QT_MOC_LITERAL(17, 143, 20), // "onPcbListItemClicked"
QT_MOC_LITERAL(18, 164, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(19, 181, 4), // "item"
QT_MOC_LITERAL(20, 186, 15), // "onPcbRegistered"
QT_MOC_LITERAL(21, 202, 2), // "id"
QT_MOC_LITERAL(22, 205, 16), // "onPcbDataUpdated"
QT_MOC_LITERAL(23, 222, 9), // "onPcbLeft"
QT_MOC_LITERAL(24, 232, 14), // "onRefreshTimer"
QT_MOC_LITERAL(25, 247, 17), // "onSimulateClicked"
QT_MOC_LITERAL(26, 265, 19), // "onManualScanClicked"
QT_MOC_LITERAL(27, 285, 14) // "onClearClicked"

    },
    "MainWindow\0onConnectClicked\0\0onPortOpened\0"
    "ok\0msg\0onPortClosed\0onPortError\0"
    "onTemperatureReceived\0pcbId\0t0\0t1\0t2\0"
    "t3\0t4\0chamber\0onBarcodeReceived\0"
    "onPcbListItemClicked\0QListWidgetItem*\0"
    "item\0onPcbRegistered\0id\0onPcbDataUpdated\0"
    "onPcbLeft\0onRefreshTimer\0onSimulateClicked\0"
    "onManualScanClicked\0onClearClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x08 /* Private */,
       3,    2,   85,    2, 0x08 /* Private */,
       6,    0,   90,    2, 0x08 /* Private */,
       7,    1,   91,    2, 0x08 /* Private */,
       8,    7,   94,    2, 0x08 /* Private */,
      16,    1,  109,    2, 0x08 /* Private */,
      17,    1,  112,    2, 0x08 /* Private */,
      20,    1,  115,    2, 0x08 /* Private */,
      22,    1,  118,    2, 0x08 /* Private */,
      23,    1,  121,    2, 0x08 /* Private */,
      24,    0,  124,    2, 0x08 /* Private */,
      25,    0,  125,    2, 0x08 /* Private */,
      26,    0,  126,    2, 0x08 /* Private */,
      27,    0,  127,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Int,    9,   10,   11,   12,   13,   14,   15,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void, QMetaType::QString,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onConnectClicked(); break;
        case 1: _t->onPortOpened((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 2: _t->onPortClosed(); break;
        case 3: _t->onPortError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onTemperatureReceived((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 5: _t->onBarcodeReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onPcbListItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 7: _t->onPcbRegistered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onPcbDataUpdated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onPcbLeft((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->onRefreshTimer(); break;
        case 11: _t->onSimulateClicked(); break;
        case 12: _t->onManualScanClicked(); break;
        case 13: _t->onClearClicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
