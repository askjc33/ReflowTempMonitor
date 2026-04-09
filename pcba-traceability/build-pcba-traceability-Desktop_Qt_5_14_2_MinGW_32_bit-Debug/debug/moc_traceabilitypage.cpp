/****************************************************************************
** Meta object code from reading C++ file 'traceabilitypage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../pcba-traceability/traceabilitypage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'traceabilitypage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TraceabilityPage_t {
    QByteArrayData data[13];
    char stringdata0[190];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TraceabilityPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TraceabilityPage_t qt_meta_stringdata_TraceabilityPage = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TraceabilityPage"
QT_MOC_LITERAL(1, 17, 17), // "requestDisconnect"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 20), // "requestClearFinished"
QT_MOC_LITERAL(4, 57, 20), // "requestExportCurrent"
QT_MOC_LITERAL(5, 78, 7), // "barcode"
QT_MOC_LITERAL(6, 86, 22), // "selectedBarcodeChanged"
QT_MOC_LITERAL(7, 109, 18), // "onTableCellClicked"
QT_MOC_LITERAL(8, 128, 3), // "row"
QT_MOC_LITERAL(9, 132, 6), // "column"
QT_MOC_LITERAL(10, 139, 19), // "onDisconnectClicked"
QT_MOC_LITERAL(11, 159, 14), // "onClearClicked"
QT_MOC_LITERAL(12, 174, 15) // "onExportClicked"

    },
    "TraceabilityPage\0requestDisconnect\0\0"
    "requestClearFinished\0requestExportCurrent\0"
    "barcode\0selectedBarcodeChanged\0"
    "onTableCellClicked\0row\0column\0"
    "onDisconnectClicked\0onClearClicked\0"
    "onExportClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TraceabilityPage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    1,   56,    2, 0x06 /* Public */,
       6,    1,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   62,    2, 0x08 /* Private */,
      10,    0,   67,    2, 0x08 /* Private */,
      11,    0,   68,    2, 0x08 /* Private */,
      12,    0,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    9,
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
        case 0: _t->requestDisconnect(); break;
        case 1: _t->requestClearFinished(); break;
        case 2: _t->requestExportCurrent((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->selectedBarcodeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->onTableCellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->onDisconnectClicked(); break;
        case 6: _t->onClearClicked(); break;
        case 7: _t->onExportClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TraceabilityPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestDisconnect)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestClearFinished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::requestExportCurrent)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TraceabilityPage::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TraceabilityPage::selectedBarcodeChanged)) {
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
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void TraceabilityPage::requestDisconnect()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TraceabilityPage::requestClearFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TraceabilityPage::requestExportCurrent(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TraceabilityPage::selectedBarcodeChanged(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
