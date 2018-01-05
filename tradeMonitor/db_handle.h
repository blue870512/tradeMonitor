#ifndef DB_HANDLE_H
#define DB_HANDLE_H

#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QStringList>
#include "common.h"
#include "tm_log.h"

class db_handle
{
public:
    explicit db_handle();
    db_handle(const db_handle &d);
    ~db_handle();
    db_handle & operator= (const db_handle& d);

    void set_host(const QString &host = "");
    void set_dbName(const QString &dbName = "");
    void set_username(const QString &username = "");
    void set_password(const QString &password = "");
    void set_port(const int &port = 1521);
    void set_db(const db_info& d);
    void set_index(const int i);
    void set_str_conn_name(QString &str);
    QString get_host();
    QString get_dbName();
    QString get_username();
    QString get_password();
    int get_port();

    bool connect_oracle();
    void close_oracle();
    int query_sql(const QString &sql);
    int query_sql(const QString &sql, QList<QStringList> &v);

private:
    db_info db;
    QSqlDatabase h;
    tm_log *log;
    QString str_conn_name;
    int index;
    bool is_add_database;

    void write_log(const QString &s = "");
    void write_log(const int &i);
};

#endif // DB_HANDLE_H
