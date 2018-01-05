#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include "tm_thread.h"
#include "global.h"

tm_thread::tm_thread(QObject * parent)
    :QThread(parent)
{
    int i;
    for(i=0; i<TM_CLOSING; i++)
    {
        lst_total_count.append(0);
    }
    for(i=0; i<g_conf.lst_opening_sql.size(); i++)
    {
        lst_warn_count.append(0);
    }
    is_clearing_done = false;
    QDateTime time = QDateTime::currentDateTime();
    str_cur_date = time.toString("yyyyMMdd");
    oracle_connect_status = TM_CONNECT_INIT;

    connect(this,SIGNAL(task_end()),this->parent(),SLOT(tm_thread_stop()));
    connect(this,SIGNAL(connect_end()),this->parent(),SLOT(tm_thread_connect_stop()));
}

tm_thread::~tm_thread()
{
    close_oracle();
}

void tm_thread::run()
{
    if(oracle_connect_status != TM_CONNECT)
    {
        connect_oracle();
        emit connect_end();
    }
    else if(oracle_connect_status == TM_CONNECT)
    {
        if(m_lock.tryLock() == false)
        {
            return;
        }

        QDateTime time = QDateTime::currentDateTime();
        QString s = time.toString("hhmmss");
        QString d = time.toString("yyyyMMdd");

        if(str_cur_date != d)
        {
            init_stat_count();
            str_cur_date = d;
        }

        if(s>g_conf.str_open && s<=g_conf.str_close)
        {
            if(oracle_connect_status != TM_CONNECT)
            {
                connect_oracle();
            }
            if(is_clearing_done == false)
            {   
                is_clearing_done = true;
            }
            query(TM_OPENING);
        }
        else if(s>=g_conf.str_done)
        {
            if(is_clearing_done == true)
            {
                if(oracle_connect_status != TM_CONNECT)
                {
                    connect_oracle();
                }
                query(TM_CLOSING);
                is_clearing_done = false;
                close_oracle();
            }
            else
            {
                if(oracle_connect_status == TM_CONNECT)
                {
                    close_oracle();
                }
                idle();
            }
        }
        else
        {
            if(oracle_connect_status == TM_CONNECT)
            {
                close_oracle();
            }
            idle();
        }

        m_lock.unlock();
        emit task_end();
    }
}

bool tm_thread::connect_oracle()
{
    for(int i=0; i<g_conf.lst_db.size(); i++)
    {
        if(!g_conf.lst_db[i].connect_oracle())
        {
            oracle_connect_status = TM_DISCONNECT;
            return false;
        }
    }
    oracle_connect_status = TM_CONNECT;
    return true;
}

void tm_thread::close_oracle()
{
    for(int i=0; i<g_conf.lst_db.size(); i++)
    {
        g_conf.lst_db[i].close_oracle();
    }
    oracle_connect_status = TM_DISCONNECT;
}

int tm_thread::get_interval()
{
    return g_conf.n_interval;
}

void tm_thread::reset()
{
    int i;
    lst_total_count.clear();
    lst_warn_count.clear();

    for(i=0; i<TM_CLOSING; i++)
    {
        lst_total_count.append(0);
    }
    for(i=0; i<g_conf.lst_opening_sql.size(); i++)
    {
        lst_warn_count.append(0);
    }
    is_clearing_done = false;
    QDateTime time = QDateTime::currentDateTime();
    str_cur_date = time.toString("yyyyMMdd");

    oracle_connect_status = TM_CONNECT_INIT;
}

int tm_thread::query_sql(const QString &sql, int i)
{
    int n = 0;
    for(int j=0; j<g_conf.lst_db.size(); j++)
    {
        QList<QStandardItem*> lst_item;
        QStandardItem *item1, *item2, *item3, *item4, *item5;
        QDateTime time = QDateTime::currentDateTime();
        QString s = time.toString("[yyyy-MM-dd hh:mm:ss]");
        QString tmp;
        QString dbname = g_conf.lst_db[j].get_dbName() + "@" + g_conf.lst_db[j].get_host();
        QString stat;

        int ret = g_conf.lst_db[j].query_sql(sql);

        if(ret == 0)
        {
            tmp = QString::number(TM_OK) + s;
            stat = QString::number(TM_OK);
        }
        else if(ret == 1)
        {
            tmp = QString::number(TM_ERROR) + s;
            stat = QString::number(TM_ERROR);
            n++;
        }
        else
        {
            tmp = QString::number(TM_UNKNOW) + s;
            stat = QString::number(TM_UNKNOW);
            n++;
        }

        item1 = new QStandardItem(tmp);
        item2 = new QStandardItem(dbname);
        item3 = new QStandardItem(stat);
        item4 = new QStandardItem(QString::number(i));
        item5 = new QStandardItem(QString::number(j));

        lst_item.clear();
        lst_item.append(item1);
        lst_item.append(item2);
        lst_item.append(item3);
        lst_item.append(item4);
        lst_item.append(item5);
        lst_info.push_back(lst_item);
    }
    return n;
}

void tm_thread::query(int type)
{
    if(type == TM_OPENING)
    {
        opening_query();
    }
    else if(type == TM_CLOSING)
    {
        closing_query();
    }
}

void tm_thread::opening_query()
{
    int i;

    for(i=0; i<g_conf.lst_opening_sql.size(); i++)
    {
        lst_warn_count[i] += query_sql(g_conf.lst_opening_sql[i].sql, i);
    }
    lst_total_count[TM_OPENING-1]++;

    for(i=0; i<g_conf.lst_opening_sql.size(); i++)
    {
        QList<QStandardItem*> lst_item;
        QStandardItem *item1, *item2, *item3;

        item1 = new QStandardItem(QString::number(i));
        item2 = new QStandardItem(QString::number(lst_warn_count[i]));
        item3 = new QStandardItem(QString::number(lst_total_count[TM_OPENING-1]*g_conf.lst_db.size()));

        lst_item.clear();
        lst_item.push_back(item1);
        lst_item.append(item2);
        lst_item.append(item3);
        lst_stat.push_back(lst_item);
    }
}

void tm_thread::closing_query()
{
    int i;

    for(i=0; i<g_conf.lst_closing_sql.size(); i++)
    {
        query_sql(g_conf.lst_closing_sql[i].sql, (i+g_conf.lst_opening_sql.size()));
    }
    lst_total_count[TM_OPENING-1]++;
}

void tm_thread::idle()
{
    QList<QStandardItem*> lst_item;
    QStandardItem *item1, *item2, *item3, *item4, *item5;
    QDateTime time = QDateTime::currentDateTime();
    QString s = time.toString("[yyyy-MM-dd hh:mm:ss]");
    QString tmp;
    QString dbname = "none";
    QString stat;

    tmp = QString::number(TM_WARNING) + s;
    stat = QString::number(TM_WARNING);

    item1 = new QStandardItem(tmp);
    item2 = new QStandardItem(dbname);
    item3 = new QStandardItem(stat);
    item4 = new QStandardItem(QString::number(-1));
    item5 = new QStandardItem(QString::number(-1));

    lst_item.clear();
    lst_item.append(item1);
    lst_item.append(item2);
    lst_item.append(item3);
    lst_item.append(item4);
    lst_item.append(item5);
    lst_info.push_back(lst_item);

    g_log.write_log("idle");
}

void tm_thread::init_stat_count()
{
    int i;
    for(i=0; i<lst_total_count.size(); i++)
    {
        lst_total_count[i] = 0;
    }
    for(i=0; i<lst_warn_count.size(); i++)
    {
        lst_warn_count[i] = 0;
    }
}
