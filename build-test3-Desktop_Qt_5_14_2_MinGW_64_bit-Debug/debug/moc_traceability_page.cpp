/****************************************************************************
** Meta object code from reading C++ file 'traceability_page.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../test3/traceability_page.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'traceability_page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TraceabilityPage_t {
    QByteArrayData data[20];
    char stringdata0[304];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TraceabilityPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TraceabilityPage_t qt_meta_stringdata_TraceabilityPage = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TraceabilityPage"
QT_MOC_LITERAL(1, 17, 23), // "requestSetStatusMessage"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 4), // "text"
QT_MOC_LITERAL(4, 47, 26), // "requestSetConnectionStatus"
QT_MOC_LITERAL(5, 74, 9), // "connected"
QT_MOC_LITERAL(6, 84, 22), // "requestSetBatteryLevel"
QT_MOC_LITERAL(7, 107, 5), // "value"
QT_MOC_LITERAL(8, 113, 24), // "requestDisconnectAndBack"
QT_MOC_LITERAL(9, 138, 11), // "refreshView"
QT_MOC_LITERAL(10, 150, 18), // "onTableCellClicked"
QT_MOC_LITERAL(11, 169, 3), // "row"
QT_MOC_LITERAL(12, 173, 17), // "onBatteryReceived"
QT_MOC_LITERAL(13, 191, 16), // "disconnectDevice"
QT_MOC_LITERAL(14, 208, 13), // "onSerialError"
QT_MOC_LITERAL(15, 222, 3), // "msg"
QT_MOC_LITERAL(16, 226, 14), // "onDisconnected"
QT_MOC_LITERAL(17, 241, 20), // "clearFinishedRecords"
QT_MOC_LITERAL(18, 262, 18), // "exportCurrentBoard"
QT_MOC_LITERAL(19, 281, 22) // "refreshChartReferences"

    },
    "TraceabilityPage\0requestSetStatusMessage\0"
    "\0text\0requestSetConnectionStatus\0"
    "connected\0requestSetBatteryLevel\0value\0"
    "requestDisconnectAndBack\0refreshView\0"
    "onTableCellClicked\0row\0onBatteryReceived\0"
    "disconnectDevice\0onSerialError\0msg\0"
    "onDisconnected\0clearFinishedRecords\0"
    "exportCurrentBoard\0refreshChartReferences"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TraceabilityPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       4,    2,   82,    2, 0x06 /* Public */,
       6,    1,   87,    2, 0x06 /* Public */,
       8,    0,   90,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   91,    2, 0x08 /* Private */,
      10,    2,   92,    2, 0x08 /* Private */,
      12,    1,   97,    2, 0x08 /* Private */,
      13,    0,  100,    2, 0x08 /* Private */,
      14,    1,  101,    2, 0x08 /* Private */,
      16,    0,  104,    2, 0x08 /* Private */,
      17,    0,  105,    2, 0x08 /* Private */,
      18,    0,  106,    2, 0x08 /* Private */,
      19,    0,  107,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   11,    2,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
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
        case 0: _t->requestSetStatusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->requestSetConnectionStatus((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: _t->requestSetBatteryLevel((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->requestDisconnectAndBack(); break;
        case 4: _t->refreshView(); break;
        case 5: _t->onTableCellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->onBatteryReceived((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->disconnectDevice(); break;
        case 8: _t->onSerialError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->onDisconnected(); break;
        case 10: _t->clearFinishedRecords(); break;
        case 11: _t->exportCurrentBoard(); break;
        case 12: _t->refreshChartReferences(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TraceabilityPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestSetStatusMessage)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestSetConnectionStatus)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestSetBatteryLevel)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestDisconnectAndBack)) {
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void TraceabilityPage::requestSetStatusMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TraceabilityPage::requestSetConnectionStatus(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TraceabilityPage::requestSetBatteryLevel(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TraceabilityPage::requestDisconnectAndBack()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
