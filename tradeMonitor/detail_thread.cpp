#include <QDebug>
#include <windows.h>
#include "detail_thread.h"
#include "global.h"

detail_thread::detail_thread(QObject *parent)
    :QThread(parent)
{
    db_index = -1;
    sql_index = -1;
    market_status = -1;
    p_lst_sql = NULL;
    receiver = NULL;
    oracle_connect_status = TM_CONNECT_INIT;

    connect(this,SIGNAL(task_end()),this,SLOT(stop()));
    //connect(this,SIGNAL(task_end2()),this->parent(),SLOT(detail_thread_stop()));
    //connect(this,SIGNAL(connect_end()),this->parent(),SLOT(detail_thread_connect_stop()));
}

detail_thread::~detail_thread()
{

}

void detail_thread::run()
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
        query_detail();
        m_lock.unlock();
        emit task_end();
    }
}

void detail_thread::init(int db_index, int sql_index, int flag)
{
    this->db_index = db_index;
    this->sql_index = sql_index;
    this->market_status = flag;
    if(market_status == TM_OPENING)
    {
        p_lst_sql = &g_conf.lst_opening_sql;
    }
    else if(market_status == TM_CLOSING)
    {
        p_lst_sql = &g_conf.lst_closing_sql;
    }
}

void detail_thread::reset()
{
    db_index = -1;
    sql_index = -1;
    market_status = -1;
    p_lst_sql = NULL;
    oracle_connect_status = TM_CONNECT_INIT;
}

bool detail_thread::connect_oracle()
{

    if(p_lst_sql == NULL)
    {
        return false;
    }

    if(sql_index<0 || sql_index>=p_lst_sql->size() || db_index<0 || db_index>=g_conf.lst_db.size())
    {
        return false;
    }

    db = g_conf.lst_db[db_index];
    QString str;
    if(market_status == TM_OPENING)
    {
        str = "db_" + db.get_host() +"_for_detail_opening";
    }
    else if(market_status == TM_CLOSING)
    {
        str = "db_" + db.get_host() +"_for_detail_closing";
    }
    else
    {
        str = "db_" + db.get_host() +"_for_detail";
    }
    db.set_str_conn_name(str);
    if(db.connect_oracle())
    {
        oracle_connect_status = TM_CONNECT;
        return true;
    }
    else
    {
        oracle_connect_status = TM_DISCONNECT;
        return false;
    }
}

void detail_thread::close_oracle()
{
    db.close_oracle();
    oracle_connect_status = TM_DISCONNECT;
}

void detail_thread::query_detail()
{
    if(p_lst_sql == NULL)
    {
        return;
    }

    if(sql_index<0 || sql_index>=p_lst_sql->size() || db_index<0 || db_index>=g_conf.lst_db.size())
    {
        return;
    }

    db.query_sql((*p_lst_sql)[sql_index].sql, lst_detail);
}

void detail_thread::stop()
{
    db.close_oracle();
    emit task_end2();
}

void detail_thread::set_receiver(QObject *p)
{
    receiver = p;
    connect(this,SIGNAL(task_end2()),receiver,SLOT(detail_thread_stop()));
    connect(this,SIGNAL(connect_end()),receiver,SLOT(detail_thread_connect_stop()));
}
