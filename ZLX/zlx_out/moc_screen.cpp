/****************************************************************************
** Meta object code from reading C++ file 'screen.h'
**
** Created: Fri Nov 20 16:17:00 2015
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../zlx_yu/screen.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'screen.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Screen[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

       0        // eod
};

static const char qt_meta_stringdata_Screen[] = {
    "Screen\0"
};

const QMetaObject Screen::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_Screen,
      qt_meta_data_Screen, 0 }
};

const QMetaObject *Screen::metaObject() const
{
    return &staticMetaObject;
}

void *Screen::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Screen))
        return static_cast<void*>(const_cast< Screen*>(this));
    return QThread::qt_metacast(_clname);
}

int Screen::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
