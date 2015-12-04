#ifndef FUNLIB_H
#define FUNLIB_H

#include <QtCore>

qint16 char2int16(char *);
quint16 CRC16(const char *, quint16 );
quint8 checksum(const QByteArray &);

#endif
