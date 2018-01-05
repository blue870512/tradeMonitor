#ifndef COMMON_H
#define COMMON_H

#include <QString>

typedef struct _db_info
{
    QString host;
    QString dbName;
    QString username;
    QString password;
    int port;
}db_info;

typedef struct _sql_info
{
    QString sql;
    QString intr;
    int type;
}sql_info;

typedef struct _sql_stat_info
{
    QString sql;
    QString intr;
    int type;
    int count;
}sql_stat_info;

enum status_level
{
    TM_OK = 0,
    TM_WARNING,
    TM_ERROR,
    TM_UNKNOW,
};

enum market_status
{
    TM_PRE_OPENING = 0,
    TM_OPENING,
    TM_CLOSING,
};

enum connect_status
{
    TM_CONNECT_INIT = 0,
    TM_DISCONNECT,
    TM_CONNECT,
};

void encode(QString &s1, QString &s2);
void decode(QString &s1, QString &s2);
void bitcode(QString &s1, QString &s2);
void xml_decode(QString &s1, QString &s2);
void xml_encode(QString &s1, QString &s2);
#endif // COMMON_H
