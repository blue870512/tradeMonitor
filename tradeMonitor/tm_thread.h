#ifndef TM_THREAD_H
#define TM_THREAD_H

//盘中查询线程

#include <QThread>
#include <QTableWidget>
#include <QList>
#include <QMutex>
#include "info_table.h"
#include "status_table.h"
#include "db_handle.h"

class tm_thread : public QThread
{
    Q_OBJECT
public:
    explicit tm_thread(QObject *parent = 0);
    ~tm_thread();
    //void init();

    bool connect_oracle();
    void close_oracle();

    int get_interval();
    QList<QList<QStandardItem*> > &get_lst_info() {return lst_info;}
    QList<QList<QStandardItem*> > &get_lst_stat() {return lst_stat;}
    int get_oracle_connect_status() {return oracle_connect_status;}

    void lock() {m_lock.lock();}
    void unlock() {m_lock.unlock();}
    bool tryLock() {return m_lock.tryLock();}

    void reset();

private Q_SLOTS:
    //void stop();

protected:
    void run();

signals:
    void connect_end();
    void task_end();

private:
    int query_sql(const QString &sql, int i);
    void query(int type);
    void opening_query();
    void closing_query();
    void idle();
    void init_stat_count();

    QMutex m_lock;

    QList<int> lst_total_count;
    QList<int> lst_warn_count;
    QString str_cur_date;
    bool is_clearing_done;
    int oracle_connect_status;

    QList<QList<QStandardItem*> > lst_info;
    QList<QList<QStandardItem*> > lst_stat;
};

#endif // TM_THREAD_H
