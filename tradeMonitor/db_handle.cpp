#include <QFile>
#include <QTextStream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTime>
#include <QDebug>
#include "db_handle.h"
#include "common.h"
#include "global.h"

db_handle::db_handle()
{
    db.host = "";
    db.dbName = "";
    db.username = "";
    db.password = "";
    db.port = 0;
    str_conn_name = "";
    index = -1;

    log = &g_log;
    is_add_database = false;
}

db_handle::db_handle(const db_handle &d)
{
    db.host = d.db.host;
    db.dbName = d.db.dbName;
    db.username = d.db.username;
    db.password = d.db.password;
    db.port = d.db.port;
    str_conn_name = d.str_conn_name;
    index = d.index;

    log = &g_log;
    is_add_database = false;
}

db_handle::~db_handle()
{
    close_oracle();
}

db_handle & db_handle::operator =(const db_handle &d)
{
    db.host = d.db.host;
    db.dbName = d.db.dbName;
    db.username = d.db.username;
    db.password = d.db.password;
    db.port = d.db.port;
    str_conn_name = d.str_conn_name;
    index = d.index;

    log = &g_log;
    is_add_database = false;

    return *this;
}

void db_handle::set_host(const QString &host)
{
    db.host = host;
}

void db_handle::set_dbName(const QString &dbName)
{
    db.dbName = dbName;
}

void db_handle::set_username(const QString &username)
{
    if(username.size()>32)
    {
        db.username = username.mid(0,32);
    }
    else
    {
        db.username = username;
    }
}

void db_handle::set_password(const QString &password)
{
    db.password = password;
}

void db_handle::set_port(const int &port)
{
    db.port = port;
}

void db_handle::set_db(const db_info& d)
{
    db.host = d.host;
    db.dbName = d.dbName;
    db.username = d.username;
    db.password = d.password;
    db.port = d.port;

    is_add_database = false;
}

QString db_handle::get_host()
{
    return db.host;
}

QString db_handle::get_dbName()
{
    return db.dbName;
}

QString db_handle::get_username()
{
    return db.username;
}

QString db_handle::get_password()
{
    return db.password;
}

int db_handle::get_port()
{
    return db.port;
}

void db_handle::set_index(const int i)
{
    index = i;
}

void db_handle::set_str_conn_name(QString &str)
{
    str_conn_name = str;
}

bool db_handle::connect_oracle()
{
    if(h.isOpen())
    {
        h.close();
    }

    if(is_add_database)
    {
        if(str_conn_name != "")
        {
            h = QSqlDatabase();
            h.removeDatabase(str_conn_name);
        }
        is_add_database = false;
    }

    if(str_conn_name == "")
    {
        if(index>=0)
        {
            str_conn_name = "DB" + QString::number(index);
        }
        else
        {
            QTime time;
            time= QTime::currentTime();
            qsrand(time.msec()+time.second()*1000);
            int xxx=qrand();
            str_conn_name = "random" + QString::number(xxx);
        }
    }

    h = QSqlDatabase::addDatabase("QOCI", str_conn_name);
    h.setPort(db.port);
    h.setHostName(db.host);
    h.setDatabaseName(db.dbName);
    h.setUserName(db.username);
    h.setPassword(db.password);

    is_add_database = true;

    if (!h.open())
    {
        return false;
    }

    return true;
}

void db_handle::close_oracle()
{
    if(h.isOpen())
    {
        h.close();
    }

    if(is_add_database)
    {
        if(str_conn_name != "")
        {
            h = QSqlDatabase();
            h.removeDatabase(str_conn_name);
        }
        is_add_database = false;
    }
}

int db_handle::query_sql(const QString &sql)
{
    QString tmp;
    if(!h.isOpen())
    {
        tmp = str_conn_name + " is not connected!";
        write_log(tmp);
        return -1;
    }

    QSqlQuery query(h);
    if(!query.exec(sql))
    {
        tmp = str_conn_name + ": sql[" + sql + "] is not correct!";
        write_log(tmp);
        return -1;
    }
    int columnNum = query.record().count();
    int i, j;

    query.last();
    /*
    if(!query.last())
    {
        tmp = "db" + QString::number(index) + ": sql["+ sql +"], last() return false!";
        write_log(tmp);
        return -1;
    }
    */

    int count = query.at()+1;
    if(count == 0)
    {
        return 0;
    }

    tmp = "";
    for(i=0; i<columnNum; i++)
    {
        tmp += query.record().fieldName(i);
        tmp += ",";
    }
    tmp = tmp.left(tmp.size()-1);
    write_log(tmp);

    query.first();
    /*
    if(!query.first())
    {
        tmp = "db" + QString::number(index) + ": sql["+ sql +"], first() return false!";
        write_log(tmp);
        return -1;
    }
    */
    i = 0;

    do
    {
        tmp = "";
        for(j=0; j<columnNum; j++)
        {
            if(query.value(j).type() == QVariant::Double)
            {
                int n = query.value(j).toInt();
                if(query.value(j).toString() == QString::number(n))
                {
                    tmp += QString::number(n);
                }
                else
                {
                    double d = query.value(j).toDouble();
                    tmp += QString::number(d, 'f', 2);
                }
                //qDebug() << "double" << tmp;
            }
            else
            {
                tmp += query.value(j).toString();
            }
            //tmp += query.value(j).toString();
            tmp += ",";
        }
        tmp = tmp.left(tmp.size()-1);
        write_log(tmp);
        i++;
    }while (query.next() && i<100); //只记录前100

    return 1;
}

/*
 * 将返回结果记录在QList<QStringList> &v中
 */
int db_handle::query_sql(const QString &sql, QList<QStringList> &v)
{
    QString tmp;
    QStringList lst_tmp;
    v.clear();

    if(!h.isOpen())
    {
        tmp = str_conn_name + " is not connected!";
        lst_tmp.append(tmp);
        v.append(lst_tmp);
        write_log(tmp);
        return -1;
    }

    QSqlQuery query(h);
    if(!query.exec(sql))
    {
        tmp = str_conn_name + ": sql[" + sql + "] is not correct!";
        lst_tmp.append(tmp);
        v.append(lst_tmp);
        write_log(tmp);
        return -1;
    }
    int columnNum = query.record().count();
    int i;

    for(i=0; i<columnNum; i++)
    {
        tmp = query.record().fieldName(i);
        lst_tmp.append(tmp);
    }
    v.append(lst_tmp);
    lst_tmp.clear();

    while (query.next())
    {
        for(i=0; i<columnNum; i++)
        {
            if(query.value(i).type() == QVariant::Double)
            {
                int n = query.value(i).toInt();
                if(query.value(i).toString() == QString::number(n))
                {
                    tmp = QString::number(n);
                }
                else
                {
                    double d = query.value(i).toDouble();
                    tmp = QString::number(d, 'f', 2);
                }
            }
            else
            {
                tmp = query.value(i).toString();
            }
            lst_tmp.append(tmp);
        }
        v.append(lst_tmp);
        lst_tmp.clear();
    }

    return 1;
}

void db_handle::write_log(const QString &s)
{
    if(log != NULL)
    {
        log->write_log(s);
    }
}

void db_handle::write_log(const int &i)
{
    if(log != NULL)
    {
        log->write_log(i);
    }
}
