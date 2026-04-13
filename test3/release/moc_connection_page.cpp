/****************************************************************************
** Meta object code from reading C++ file 'connection_page.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../connection_page.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connection_page.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ConnectionPage_t {
    QByteArrayData data[19];
    char stringdata0[256];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ConnectionPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ConnectionPage_t qt_meta_stringdata_ConnectionPage = {
    {
QT_MOC_LITERAL(0, 0, 14), // "ConnectionPage"
QT_MOC_LITERAL(1, 15, 24), // "requestSwitchToTracePage"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 23), // "requestSetStatusMessage"
QT_MOC_LITERAL(4, 65, 4), // "text"
QT_MOC_LITERAL(5, 70, 26), // "requestSetConnectionStatus"
QT_MOC_LITERAL(6, 97, 9), // "connected"
QT_MOC_LITERAL(7, 107, 4), // "scan"
QT_MOC_LITERAL(8, 112, 14), // "onScanFinished"
QT_MOC_LITERAL(9, 127, 30), // "QList<SerialManager::PortItem>"
QT_MOC_LITERAL(10, 158, 5), // "ports"
QT_MOC_LITERAL(11, 164, 13), // "onItemClicked"
QT_MOC_LITERAL(12, 178, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(13, 195, 4), // "item"
QT_MOC_LITERAL(14, 200, 9), // "doConnect"
QT_MOC_LITERAL(15, 210, 16), // "onConnectSuccess"
QT_MOC_LITERAL(16, 227, 8), // "portName"
QT_MOC_LITERAL(17, 236, 15), // "onConnectFailed"
QT_MOC_LITERAL(18, 252, 3) // "msg"

    },
    "ConnectionPage\0requestSwitchToTracePage\0"
    "\0requestSetStatusMessage\0text\0"
    "requestSetConnectionStatus\0connected\0"
    "scan\0onScanFinished\0QList<SerialManager::PortItem>\0"
    "ports\0onItemClicked\0QListWidgetItem*\0"
    "item\0doConnect\0onConnectSuccess\0"
    "portName\0onConnectFailed\0msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ConnectionPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    1,   60,    2, 0x06 /* Public */,
       5,    2,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   68,    2, 0x08 /* Private */,
       8,    1,   69,    2, 0x08 /* Private */,
      11,    1,   72,    2, 0x08 /* Private */,
      14,    0,   75,    2, 0x08 /* Private */,
      15,    1,   76,    2, 0x08 /* Private */,
      17,    1,   79,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   18,

       0        // eod
};

void ConnectionPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ConnectionPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->requestSwitchToTracePage(); break;
        case 1: _t->requestSetStatusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->requestSetConnectionStatus((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->scan(); break;
        case 4: _t->onScanFinished((*reinterpret_cast< const QList<SerialManager::PortItem>(*)>(_a[1]))); break;
        case 5: _t->onItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->doConnect(); break;
        case 7: _t->onConnectSuccess((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->onConnectFailed((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ConnectionPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::requestSwitchToTracePage)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ConnectionPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::requestSetStatusMessage)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ConnectionPage::*)(const QString & , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ConnectionPage::requestSetConnectionStatus)) {
                *result = 2;
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
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ConnectionPage::requestSwitchToTracePage()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConnectionPage::requestSetStatusMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ConnectionPage::requestSetConnectionStatus(const QString & _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
