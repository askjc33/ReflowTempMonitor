/****************************************************************************
** Meta object code from reading C++ file 'connectionpage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../pcba-traceability/connectionpage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connectionpage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConnectionPage_t {
    QByteArrayData data[10];
    char stringdata0[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConnectionPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConnectionPage_t qt_meta_stringdata_ConnectionPage = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ConnectionPage"
QT_MOC_LITERAL(1, 15, 16), // "requestScanPorts"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 18), // "requestConnectPort"
QT_MOC_LITERAL(4, 52, 8), // "portName"
QT_MOC_LITERAL(5, 61, 13), // "onItemClicked"
QT_MOC_LITERAL(6, 75, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 92, 4), // "item"
QT_MOC_LITERAL(8, 97, 13), // "onScanClicked"
QT_MOC_LITERAL(9, 111, 16) // "onConnectClicked"

    },
    "ConnectionPage\0requestScanPorts\0\0"
    "requestConnectPort\0portName\0onItemClicked\0"
    "QListWidgetItem*\0item\0onScanClicked\0"
    "onConnectClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConnectionPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    1,   40,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   43,    2, 0x08 /* Private */,
       8,    0,   46,    2, 0x08 /* Private */,
       9,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
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
        case 0: _t->requestScanPorts(); break;
        case 1: _t->requestConnectPort((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->onScanClicked(); break;
        case 4: _t->onConnectClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConnectionPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::requestScanPorts)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConnectionPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::requestConnectPort)) {
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
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ConnectionPage::requestScanPorts()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConnectionPage::requestConnectPort(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
