#ifndef DETAIL_THREAD_H
#define DETAIL_THREAD_H

#include <QThread>
#include <QTableWidget>
#include <QMutex>
#include "db_handle.h"

class detail_thread: public QThread
{
    Q_OBJECT
public:
    explicit detail_thread(QObject *parent = 0);
    ~detail_thread();

    void init(int db_index, int sql_index, int flag);
    void reset();
    bool connect_oracle();
    void close_oracle();

    void set_receiver(QObject *p);

    QList<QStringList> & get_lst_detail() {return lst_detail;}
    int get_oracle_connect_status() {return oracle_connect_status;}
    void lock() {m_lock.lock();}
    void unlock() {m_lock.unlock();}
    bool tryLock() {return m_lock.tryLock();}


private Q_SLOTS:
    void stop();

protected:
    void run();

signals:
    void task_end();
    void task_end2();
    void connect_end();

private:
    void query_detail();
    //void show_table();

    QMutex m_lock;

    int db_index;
    int sql_index;
    int market_status;
    int oracle_connect_status;
    db_handle db;
    QObject *receiver;

    QList<QStringList> lst_detail;
    QList<sql_info> *p_lst_sql;
};

#endif // DETAIL_THREAD_H
