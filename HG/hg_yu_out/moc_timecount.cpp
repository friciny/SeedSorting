/****************************************************************************
** Meta object code from reading C++ file 'timecount.h'
**
** Created: Mon Dec 28 10:29:47 2015
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../hg_yu/timecount.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timecount.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Timecount[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Timecount[] = {
    "Timecount\0\0count()\0"
};

const QMetaObject Timecount::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Timecount,
      qt_meta_data_Timecount, 0 }
};

const QMetaObject *Timecount::metaObject() const
{
    return &staticMetaObject;
}

void *Timecount::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Timecount))
        return static_cast<void*>(const_cast< Timecount*>(this));
    return QThread::qt_metacast(_clname);
}

int Timecount::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: count(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
