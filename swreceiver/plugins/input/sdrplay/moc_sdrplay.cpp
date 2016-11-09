/****************************************************************************
** Meta object code from reading C++ file 'sdrplay.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "sdrplay.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sdrplay.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_sdrplay_t {
    QByteArrayData data[7];
    char stringdata0[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sdrplay_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sdrplay_t qt_meta_stringdata_sdrplay = {
    {
QT_MOC_LITERAL(0, 0, 7), // "sdrplay"
QT_MOC_LITERAL(1, 8, 15), // "setExternalGain"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 16), // "set_rateSelector"
QT_MOC_LITERAL(4, 42, 18), // "agcControl_toggled"
QT_MOC_LITERAL(5, 61, 14), // "set_ppmControl"
QT_MOC_LITERAL(6, 76, 12) // "set_debugBox"

    },
    "sdrplay\0setExternalGain\0\0set_rateSelector\0"
    "agcControl_toggled\0set_ppmControl\0"
    "set_debugBox"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sdrplay[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x08 /* Private */,
       3,    1,   42,    2, 0x08 /* Private */,
       4,    1,   45,    2, 0x08 /* Private */,
       5,    1,   48,    2, 0x08 /* Private */,
       6,    1,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void sdrplay::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        sdrplay *_t = static_cast<sdrplay *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setExternalGain((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->set_rateSelector((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->agcControl_toggled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->set_ppmControl((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->set_debugBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject sdrplay::staticMetaObject = {
    { &rigInterface::staticMetaObject, qt_meta_stringdata_sdrplay.data,
      qt_meta_data_sdrplay,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *sdrplay::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sdrplay::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_sdrplay.stringdata0))
        return static_cast<void*>(const_cast< sdrplay*>(this));
    if (!strcmp(_clname, "Ui_sdrplayWidget"))
        return static_cast< Ui_sdrplayWidget*>(const_cast< sdrplay*>(this));
    if (!strcmp(_clname, "sw radio/1.0"))
        return static_cast< rigInterface*>(const_cast< sdrplay*>(this));
    return rigInterface::qt_metacast(_clname);
}

int sdrplay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = rigInterface::qt_metacall(_c, _id, _a);
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

QT_PLUGIN_METADATA_SECTION const uint qt_section_alignment_dummy = 42;

#ifdef QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0x8c, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    'x',  0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x07, 0x00, 's',  'd',  'r',  'p',  'l',  'a', 
    'y',  0x00, 0x00, 0x00, 0x9b, 0x06, 0x00, 0x00,
    0x09, 0x00, 'c',  'l',  'a',  's',  's',  'N', 
    'a',  'm',  'e',  0x00, 0x07, 0x00, 's',  'd', 
    'r',  'p',  'l',  'a',  'y',  0x00, 0x00, 0x00,
    ':',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x11, 0x00, 0x00, 0x00, 0x05, 0x00, 'd',  'e', 
    'b',  'u',  'g',  0x00, 0x95, 0x0d, 0x00, 0x00,
    0x08, 0x00, 'M',  'e',  't',  'a',  'D',  'a', 
    't',  'a',  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, '\\', 0x00, 0x00, 0x00,
    '$',  0x00, 0x00, 0x00, 'P',  0x00, 0x00, 0x00,
    '@',  0x00, 0x00, 0x00
};

#else // QT_NO_DEBUG

QT_PLUGIN_METADATA_SECTION
static const unsigned char qt_pluginMetaData[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', ' ',
    'q',  'b',  'j',  's',  0x01, 0x00, 0x00, 0x00,
    0x8c, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00,
    'x',  0x00, 0x00, 0x00, 0x1b, 0x03, 0x00, 0x00,
    0x03, 0x00, 'I',  'I',  'D',  0x00, 0x00, 0x00,
    0x07, 0x00, 's',  'd',  'r',  'p',  'l',  'a', 
    'y',  0x00, 0x00, 0x00, 0x95, 0x06, 0x00, 0x00,
    0x08, 0x00, 'M',  'e',  't',  'a',  'D',  'a', 
    't',  'a',  0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1b, 0x0a, 0x00, 0x00, 0x09, 0x00, 'c',  'l', 
    'a',  's',  's',  'N',  'a',  'm',  'e',  0x00,
    0x07, 0x00, 's',  'd',  'r',  'p',  'l',  'a', 
    'y',  0x00, 0x00, 0x00, '1',  0x00, 0x00, 0x00,
    0x05, 0x00, 'd',  'e',  'b',  'u',  'g',  0x00,
    ':',  0xc0, 0xa0, 0x00, 0x07, 0x00, 'v',  'e', 
    'r',  's',  'i',  'o',  'n',  0x00, 0x00, 0x00,
    0x0c, 0x00, 0x00, 0x00, '$',  0x00, 0x00, 0x00,
    '@',  0x00, 0x00, 0x00, '\\', 0x00, 0x00, 0x00,
    'h',  0x00, 0x00, 0x00
};
#endif // QT_NO_DEBUG

QT_MOC_EXPORT_PLUGIN(sdrplay, sdrplay)

QT_END_MOC_NAMESPACE
