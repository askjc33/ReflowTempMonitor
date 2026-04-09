/****************************************************************************
** Meta object code from reading C++ file 'view.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../test2/view.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConnectionPage_t {
    QByteArrayData data[13];
    char stringdata0[160];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConnectionPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConnectionPage_t qt_meta_stringdata_ConnectionPage = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ConnectionPage"
QT_MOC_LITERAL(1, 15, 13), // "scanRequested"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 16), // "connectRequested"
QT_MOC_LITERAL(4, 47, 8), // "portName"
QT_MOC_LITERAL(5, 56, 14), // "onScanFinished"
QT_MOC_LITERAL(6, 71, 15), // "QList<PortItem>"
QT_MOC_LITERAL(7, 87, 5), // "ports"
QT_MOC_LITERAL(8, 93, 13), // "onItemClicked"
QT_MOC_LITERAL(9, 107, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(10, 124, 4), // "item"
QT_MOC_LITERAL(11, 129, 13), // "onScanClicked"
QT_MOC_LITERAL(12, 143, 16) // "onConnectClicked"

    },
    "ConnectionPage\0scanRequested\0\0"
    "connectRequested\0portName\0onScanFinished\0"
    "QList<PortItem>\0ports\0onItemClicked\0"
    "QListWidgetItem*\0item\0onScanClicked\0"
    "onConnectClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConnectionPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   48,    2, 0x0a /* Public */,
       8,    1,   51,    2, 0x08 /* Private */,
      11,    0,   54,    2, 0x08 /* Private */,
      12,    0,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ConnectionPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConnectionPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->scanRequested(); break;
        case 1: _t->connectRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onScanFinished((*reinterpret_cast< const QList<PortItem>(*)>(_a[1]))); break;
        case 3: _t->onItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->onScanClicked(); break;
        case 5: _t->onConnectClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConnectionPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::scanRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConnectionPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::connectRequested)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ConnectionPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_ConnectionPage.data,
    qt_meta_data_ConnectionPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ConnectionPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConnectionPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectionPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ConnectionPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ConnectionPage::scanRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConnectionPage::connectRequested(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_TraceabilityPage_t {
    QByteArrayData data[21];
    char stringdata0[286];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TraceabilityPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TraceabilityPage_t qt_meta_stringdata_TraceabilityPage = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TraceabilityPage"
QT_MOC_LITERAL(1, 17, 19), // "disconnectRequested"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 22), // "clearFinishedRequested"
QT_MOC_LITERAL(4, 61, 15), // "exportRequested"
QT_MOC_LITERAL(5, 77, 13), // "boardSelected"
QT_MOC_LITERAL(6, 91, 7), // "barcode"
QT_MOC_LITERAL(7, 99, 17), // "refreshRecordList"
QT_MOC_LITERAL(8, 117, 18), // "QList<BoardRecord>"
QT_MOC_LITERAL(9, 136, 7), // "records"
QT_MOC_LITERAL(10, 144, 15), // "showBoardRecord"
QT_MOC_LITERAL(11, 160, 11), // "BoardRecord"
QT_MOC_LITERAL(12, 172, 1), // "r"
QT_MOC_LITERAL(13, 174, 17), // "clearBoardDisplay"
QT_MOC_LITERAL(14, 192, 13), // "updateBattery"
QT_MOC_LITERAL(15, 206, 5), // "value"
QT_MOC_LITERAL(16, 212, 18), // "onTableCellClicked"
QT_MOC_LITERAL(17, 231, 3), // "row"
QT_MOC_LITERAL(18, 235, 15), // "onExportClicked"
QT_MOC_LITERAL(19, 251, 14), // "onClearClicked"
QT_MOC_LITERAL(20, 266, 19) // "onDisconnectClicked"

    },
    "TraceabilityPage\0disconnectRequested\0"
    "\0clearFinishedRequested\0exportRequested\0"
    "boardSelected\0barcode\0refreshRecordList\0"
    "QList<BoardRecord>\0records\0showBoardRecord\0"
    "BoardRecord\0r\0clearBoardDisplay\0"
    "updateBattery\0value\0onTableCellClicked\0"
    "row\0onExportClicked\0onClearClicked\0"
    "onDisconnectClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TraceabilityPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    0,   75,    2, 0x06 /* Public */,
       4,    0,   76,    2, 0x06 /* Public */,
       5,    1,   77,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   80,    2, 0x0a /* Public */,
      10,    1,   83,    2, 0x0a /* Public */,
      13,    0,   86,    2, 0x0a /* Public */,
      14,    1,   87,    2, 0x0a /* Public */,
      16,    2,   90,    2, 0x08 /* Private */,
      18,    0,   95,    2, 0x08 /* Private */,
      19,    0,   96,    2, 0x08 /* Private */,
      20,    0,   97,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   17,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TraceabilityPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TraceabilityPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->disconnectRequested(); break;
        case 1: _t->clearFinishedRequested(); break;
        case 2: _t->exportRequested(); break;
        case 3: _t->boardSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->refreshRecordList((*reinterpret_cast< const QList<BoardRecord>(*)>(_a[1]))); break;
        case 5: _t->showBoardRecord((*reinterpret_cast< const BoardRecord(*)>(_a[1]))); break;
        case 6: _t->clearBoardDisplay(); break;
        case 7: _t->updateBattery((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->onTableCellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->onExportClicked(); break;
        case 10: _t->onClearClicked(); break;
        case 11: _t->onDisconnectClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TraceabilityPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::disconnectRequested)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::clearFinishedRequested)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::exportRequested)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::boardSelected)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TraceabilityPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_TraceabilityPage.data,
    qt_meta_data_TraceabilityPage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TraceabilityPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TraceabilityPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TraceabilityPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TraceabilityPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void TraceabilityPage::disconnectRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TraceabilityPage::clearFinishedRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TraceabilityPage::exportRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TraceabilityPage::boardSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
