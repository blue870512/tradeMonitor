#include "closing.h"
#include "global.h"
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>

closing::closing(QWidget *parent) : QMainWindow(parent)
{
    label_db = new QLabel(this);
    label_db->setObjectName(QStringLiteral("label_db"));
    label_db->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    table_result = new QTableWidget(this);
    table_result->setObjectName(QStringLiteral("table_result"));

    comb_db_index = new QComboBox(this);
    comb_db_index->setObjectName(QStringLiteral("comb_db_index"));

    comb_sql_index = new QComboBox(this);
    comb_sql_index->setObjectName(QStringLiteral("comb_sql_index"));

    label_sql = new QLabel(this);
    label_sql->setObjectName(QStringLiteral("label_sql"));
    label_sql->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    btn_query = new QPushButton(this);
    btn_query->setObjectName(QStringLiteral("btn_query"));


    label_db->setText("数据库 ");
    label_sql->setText("检测项目 ");
    btn_query->setText("查询");

    dt = new detail_thread(this);
    dt->set_receiver(this);

    db_index = -1;
    sql_index = -1;

    top = 5;
    botton = 5;
    left = 5;
    right = 5;

    init();
    init_slot();
}

closing::~closing()
{
    if(dt->isRunning())
    {
        dt->terminate();
        dt->wait();
    }
    delete label_db;
    delete table_result;
    delete comb_db_index;
    delete comb_sql_index;
    delete label_sql;
    delete btn_query;

    delete dt;
}

void closing::init()
{
    int w = this->width();
    int h = this->height();

    init_size(w, h);
    comb_db_index->clear();
    comb_sql_index->clear();

    int i;
    QString tmp;
    QStringList lst_db_intr;
    QStringList lst_sql_intr;

    for(i=0; i<g_conf.lst_db.size(); i++)
    {
        tmp = g_conf.lst_db[i].get_dbName() + "@" + g_conf.lst_db[i].get_host();
        lst_db_intr.append(tmp);
    }
    comb_db_index->addItems(lst_db_intr);
    if(lst_db_intr.size()>0)
    {
        db_index = 0;
    }

    for(i=0; i<g_conf.lst_closing_sql.size(); i++)
    {
        if(g_conf.lst_closing_sql[i].intr != "")
        {
            tmp = QString::number(i+1) + "." + g_conf.lst_closing_sql[i].intr;
            lst_sql_intr.append(tmp);
        }
        else
        {
            tmp = QString::number(i+1) + "." + g_conf.lst_closing_sql[i].sql;
            tmp = tmp.mid(0.50);
            lst_sql_intr.append(tmp);
        }
    }
    comb_sql_index->addItems(lst_sql_intr);
    if(lst_sql_intr.size()>0)
    {
        sql_index = 0;
    }
}

void closing::init_size(int w, int h)
{
    int hd = 20;

    int label_db_x = left;
    int label_db_y = top;
    int label_db_w = 50;
    int label_db_h = hd;

    int comb_db_index_x = label_db_x + label_db_w + 2;
    int comb_db_index_y = top;
    int comb_db_index_w = 160;
    int comb_db_index_h = hd;

    int label_sql_x = comb_db_index_x + comb_db_index_w + 2;
    int label_sql_y = top;
    int label_sql_w = 70;
    int label_sql_h = hd;

    int btn_query_w = 60;
    int btn_query_h = hd;
    int btn_query_x = w - right - btn_query_w;
    int btn_query_y = top;

    int comb_sql_index_x = label_sql_x + label_sql_w + 5;
    int comb_sql_index_y = top;
    int comb_sql_index_w = w - comb_sql_index_x - 5 - btn_query_w - right;
    int comb_sql_index_h = hd;

    int table_result_x = left;
    int table_result_y = top + hd + 4;
    int table_result_w = w - left - right;
    int table_result_h = h - top - hd - 8;

    label_db->setGeometry(QRect(label_db_x, label_db_y, label_db_w, label_db_h));
    table_result->setGeometry(QRect(table_result_x, table_result_y, table_result_w, table_result_h));
    comb_db_index->setGeometry(QRect(comb_db_index_x, comb_db_index_y, comb_db_index_w, comb_db_index_h));
    comb_sql_index->setGeometry(QRect(comb_sql_index_x, comb_sql_index_y, comb_sql_index_w, comb_sql_index_h));
    label_sql->setGeometry(QRect(label_sql_x, label_sql_y, label_sql_w, label_sql_h));
    btn_query->setGeometry(QRect(btn_query_x, btn_query_y, btn_query_w, btn_query_h));
}

void closing::init_slot()
{
    connect(comb_db_index,SIGNAL(currentIndexChanged(int)),this,SLOT(on_comb_db_index_currentIndexChanged(int)));
    connect(comb_sql_index,SIGNAL(currentIndexChanged(int)),this,SLOT(on_comb_sql_index_currentIndexChanged(int)));
    connect(btn_query,SIGNAL(clicked(bool)),this,SLOT(on_btn_query_clicked()));
}

void closing::resizeEvent(QResizeEvent *event)
{
    int w = event->size().width();
    int h = event->size().height();
    init_size(w, h);
}

void closing::on_comb_db_index_currentIndexChanged(int index)
{
    db_index = index;
}

void closing::on_comb_sql_index_currentIndexChanged(int index)
{
    sql_index = index;
}

void closing::on_btn_query_clicked()
{
    if(dt->isRunning())
    {
        //return;
        dt->terminate();
        dt->wait();
    }

    if((db_index<0 || db_index>=g_conf.lst_db.size()) ||
            (sql_index<0 || sql_index>= g_conf.lst_closing_sql.size()))
    {
        return;
    }

    dt->reset();
    dt->init(db_index, sql_index, TM_CLOSING);
    dt->start();
}

void closing::detail_thread_stop()
{
    if(m_tbl_detail_lock.tryLock() == false)
    {
        return;
    }
    if(dt->tryLock() == false)
    {
        m_tbl_detail_lock.unlock();
        return;
    }

    QList<QStringList>& lst_detail = dt->get_lst_detail();

    /*
     * QList<QStringList> v 第一行为行标题，其他行为数据
     */

    //tbl_detail->clearContents();
    table_result->clear();

    if(lst_detail.size() == 0)
    {
        table_result->setRowCount(0);
        table_result->setColumnCount(0);
        table_result->show();
        dt->unlock();
        m_tbl_detail_lock.unlock();
        return;
    }

    int i, j;

    table_result->setRowCount(lst_detail.size()-1);
    table_result->setColumnCount(lst_detail[0].size());

    QStringList headerLabels;
    for(j=0; j<lst_detail[0].size(); j++)
    {
        headerLabels << lst_detail[0][j];
    }
    table_result->setHorizontalHeaderLabels(headerLabels);

    //设置表格行标题的对齐方式
    table_result->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table_result->horizontalHeader()->setStretchLastSection(true);
    table_result->setSelectionBehavior(QAbstractItemView::SelectRows);

    for(i=1; i<lst_detail.size(); i++)
    {
        for(j=0; j<lst_detail[i].size(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(lst_detail[i][j]);
            table_result->setItem((i-1), j, item);
        }
    }

    table_result->resizeColumnsToContents();
    table_result->show();

    lst_detail.clear();

    dt->unlock();
    m_tbl_detail_lock.unlock();
}


void closing::detail_thread_connect_stop()
{
    if(dt->get_oracle_connect_status() != TM_CONNECT)
    {
        QMessageBox::warning(this, tr("警告"), tr("盘后：数据库连接不上！"));
        return;
    }

    dt->start();
}
