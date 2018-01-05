#ifndef CLOSING_H
#define CLOSING_H

//盘后查询界面

#include <QMainWindow>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QMutex>
#include "detail_thread.h"

class closing : public QMainWindow
{
    Q_OBJECT
public:
    explicit closing(QWidget *parent = 0);
    ~closing();
    void init();

protected:
    void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void on_comb_db_index_currentIndexChanged(int index);
    void on_comb_sql_index_currentIndexChanged(int index);
    void on_btn_query_clicked();
    void detail_thread_stop();
    void detail_thread_connect_stop();

private:
    void init_size(int w, int h);
    void init_slot();

    void query();
    void show_table(const QList<QStringList> &v);

    QLabel *label_db;
    QTableWidget *table_result;
    QComboBox *comb_db_index;
    QComboBox *comb_sql_index;
    QLabel *label_sql;
    QPushButton *btn_query;

    int db_index;
    int sql_index;

    detail_thread* dt;
    QMutex m_tbl_detail_lock;

    int top;
    int botton;
    int left;
    int right;
};

#endif // CLOSING_H
