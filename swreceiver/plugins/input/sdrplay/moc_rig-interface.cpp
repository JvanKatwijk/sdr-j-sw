/****************************************************************************
** Meta object code from reading C++ file 'rig-interface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../rig-interface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rig-interface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_rigInterface_t {
    QByteArrayData data[10];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_rigInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_rigInterface_t qt_meta_stringdata_rigInterface = {
    {
QT_MOC_LITERAL(0, 0, 12), // "rigInterface"
QT_MOC_LITERAL(1, 13, 16), // "set_ExtFrequency"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 9), // "set_ExtLO"
QT_MOC_LITERAL(4, 41, 10), // "set_lockLO"
QT_MOC_LITERAL(5, 52, 12), // "set_unlockLO"
QT_MOC_LITERAL(6, 65, 10), // "set_stopHW"
QT_MOC_LITERAL(7, 76, 11), // "set_startHW"
QT_MOC_LITERAL(8, 88, 14), // "set_changeRate"
QT_MOC_LITERAL(9, 103, 16) // "samplesAvailable"

    },
    "rigInterface\0set_ExtFrequency\0\0set_ExtLO\0"
    "set_lockLO\0set_unlockLO\0set_stopHW\0"
    "set_startHW\0set_changeRate\0samplesAvailable"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_rigInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       3,    1,   57,    2, 0x06 /* Public */,
       4,    0,   60,    2, 0x06 /* Public */,
       5,    0,   61,    2, 0x06 /* Public */,
       6,    0,   62,    2, 0x06 /* Public */,
       7,    0,   63,    2, 0x06 /* Public */,
       8,    1,   64,    2, 0x06 /* Public */,
       9,    1,   67,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void rigInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        rigInterface *_t = static_cast<rigInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->set_ExtFrequency((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->set_ExtLO((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->set_lockLO(); break;
        case 3: _t->set_unlockLO(); break;
        case 4: _t->set_stopHW(); break;
        case 5: _t->set_startHW(); break;
        case 6: _t->set_changeRate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->samplesAvailable((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (rigInterface::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_ExtFrequency)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_ExtLO)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_lockLO)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_unlockLO)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_stopHW)) {
                *result = 4;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_startHW)) {
                *result = 5;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::set_changeRate)) {
                *result = 6;
                return;
            }
        }
        {
            typedef void (rigInterface::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&rigInterface::samplesAvailable)) {
                *result = 7;
                return;
            }
        }
    }
}

const QMetaObject rigInterface::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_rigInterface.data,
      qt_meta_data_rigInterface,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *rigInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *rigInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_rigInterface.stringdata0))
        return static_cast<void*>(const_cast< rigInterface*>(this));
    return QThread::qt_metacast(_clname);
}

int rigInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void rigInterface::set_ExtFrequency(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void rigInterface::set_ExtLO(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void rigInterface::set_lockLO()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void rigInterface::set_unlockLO()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void rigInterface::set_stopHW()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}

// SIGNAL 5
void rigInterface::set_startHW()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void rigInterface::set_changeRate(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void rigInterface::samplesAvailable(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
