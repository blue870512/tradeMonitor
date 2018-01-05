#include "opening.h"
#include "settings.h"
#include "sqlsettings.h"
#include "export_file.h"
#include "global.h"
#include "udp_client.h"
#include <windows.h>
#include <QDebug>
#include <QResizeEvent>
#include <QHeaderView>
#include <QMessageBox>

opening::opening(QWidget *parent) : QMainWindow(parent)
{
    init_ui();

    tm = new tm_thread(this);

    n_timerID = -1;

    bl1 = 0.75;
    bl2 = 0.8;
    top = 30;
    botton = 5;
    left = 5;
    right = 5;
    m_dragging = 0;
    is_running = false;

    this->setMouseTracking(true);
    //this->ui->centralWidget->setMouseTracking(true);
    mainToolBar->setMouseTracking(true);
    tbl_detail->setMouseTracking(true);
    tbl_detail->setCursor(Qt::ArrowCursor);

    init();
    init_slot();

    for(int i=0; i<g_conf.lst_opening_sql.size(); i++)
    {
        tbl_info->add_item(g_conf.lst_opening_sql[i].intr);
        tbl_stat->add_item(g_conf.lst_opening_sql[i].intr);
    }
    for(int i=0; i<g_conf.lst_closing_sql.size(); i++)
    {
        tbl_info->add_item(g_conf.lst_closing_sql[i].intr);
    }
}

opening::~opening()
{
    if(tm->isRunning())
    {
        tm->terminate();
    }

    tm->close_oracle();
    if(n_timerID > 0)
    {
        killTimer(n_timerID);
        n_timerID = -1;
    }

    delete action_start;
    delete action_stop;
    delete action_settings;
    delete action_export;
    delete centralWidget;
    delete btn_ok;
    delete btn_error;
    delete mainToolBar;

    delete tbl_info;
    delete tbl_stat;
    delete tbl_detail;
    delete tm;
}

void opening::init()
{
    int w, h;
    w = this->width();
    h = this->height();
    init_table(w, h);
    //tm->init();
    n_interval = tm->get_interval();
}

void opening::init_ui()
{
    action_start = new QAction(this);
    action_start->setObjectName(QStringLiteral("action_start"));
    QIcon icon;
    icon.addFile(QStringLiteral(":/icon/start"), QSize(), QIcon::Normal, QIcon::Off);
    action_start->setIcon(icon);
    action_stop = new QAction(this);
    action_stop->setObjectName(QStringLiteral("action_stop"));
    action_stop->setEnabled(false);
    QIcon icon1;
    icon1.addFile(QStringLiteral(":/icon/stop"), QSize(), QIcon::Normal, QIcon::Off);
    action_stop->setIcon(icon1);
    action_settings = new QAction(this);
    action_settings->setObjectName(QStringLiteral("action_settings"));
    QIcon icon2;
    icon2.addFile(QStringLiteral(":/icon/set"), QSize(), QIcon::Normal, QIcon::Off);
    action_settings->setIcon(icon2);
    action_sqlsettings = new QAction(this);
    action_sqlsettings->setObjectName(QStringLiteral("action_sqlsettings"));
    QIcon icon3;
    icon3.addFile(QStringLiteral(":/icon/sqlset"), QSize(), QIcon::Normal, QIcon::Off);
    action_sqlsettings->setIcon(icon3);
    action_export = new QAction(this);
    action_export->setObjectName(QStringLiteral("action_export"));
    QIcon icon4;
    icon4.addFile(QStringLiteral(":/icon/export"), QSize(), QIcon::Normal, QIcon::Off);
    action_export->setIcon(icon4);
    centralWidget = new QWidget(this);
    centralWidget->setObjectName(QStringLiteral("centralWidget"));
    btn_ok = new QPushButton(this);
    btn_ok->setObjectName(QStringLiteral("btn_ok"));
    btn_ok->setGeometry(QRect(774, 430, 20, 20));
    btn_ok->setStyleSheet(QStringLiteral("background-image: url(:/icon/ok);"));
    btn_ok->setCheckable(true);
    btn_error = new QPushButton(this);
    btn_error->setObjectName(QStringLiteral("btn_error"));
    btn_error->setGeometry(QRect(774, 450, 20, 20));
    btn_error->setStyleSheet(QStringLiteral("background-image: url(:/icon/error);"));
    btn_error->setCheckable(true);
    mainToolBar = new QToolBar(this);
    mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
    mainToolBar->setEnabled(true);
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);

    mainToolBar->addAction(action_start);
    mainToolBar->addAction(action_stop);
    mainToolBar->addAction(action_settings);
    mainToolBar->addAction(action_sqlsettings);
    mainToolBar->addAction(action_export);

    this->setWindowTitle("opening");
    action_start->setText("开始");
#ifndef QT_NO_TOOLTIP
    action_start->setToolTip("开始");
#endif // QT_NO_TOOLTIP
    action_stop->setText("停止");
#ifndef QT_NO_TOOLTIP
    action_stop->setToolTip("停止");
#endif // QT_NO_TOOLTIP
    action_settings->setText("配置");
#ifndef QT_NO_TOOLTIP
    action_settings->setToolTip("配置");
#endif // QT_NO_TOOLTIP
    action_sqlsettings->setText("SQL配置");
#ifndef QT_NO_TOOLTIP
    action_sqlsettings->setToolTip("SQL配置");
#endif // QT_NO_TOOLTIP
    action_export->setText("导出");
#ifndef QT_NO_TOOLTIP
    action_export->setToolTip("导出");
#endif // QT_NO_TOOLTIP
    btn_ok->setText(QString());
    btn_error->setText(QString());

    mainToolBar->setIconSize(QSize(16,16));
    mainToolBar->setMovable(false);

    tbl_info = new info_table(this);
    tbl_stat = new status_table(this);
    tbl_detail = new QTableWidget(this);
}

void opening::init_table(int w, int h)
{
    int btn_x = w-right-20;
    int btn_y = top + bl1*(h-top-botton) + 2;//- mainToolBar->height();
    int btn_w = 20;
    int btn_h = 20;

    int tbl_info_x = left;
    int tbl_info_y = top + bl1*(h-top-botton) + 2;
    int tbl_info_w = w - left - right - btn_w;
    int tbl_info_h = (1.0-bl1)*(h-top-botton) - 2;

    int tbl_stat_x = left + bl2*(w-left-right) + 2;
    int tbl_stat_y = top;
    int tbl_stat_w = (1.0-bl2)*(w-left-right) - 2;
    int tbl_stat_h = bl1*(h-top-botton) - 2;
    if(tbl_stat_w > 140)
    {
        tbl_stat_x = w - 140 - right;
        tbl_stat_w = 140;
    }

    int tbl_detail_x = left;
    int tbl_detail_y = top;
    int tbl_detail_w = w-left-right-tbl_stat_w-4;
    int tbl_detail_h = bl1*(h-top-botton) - 2;

    tbl_info->setGeometry(QRect(tbl_info_x, tbl_info_y, tbl_info_w, tbl_info_h));
    tbl_stat->setGeometry(QRect(tbl_stat_x, tbl_stat_y, tbl_stat_w, tbl_stat_h));
    tbl_detail->setGeometry(QRect(tbl_detail_x, tbl_detail_y, tbl_detail_w, tbl_detail_h));

    btn_ok->setGeometry(QRect(btn_x, btn_y, btn_w, btn_h));
    btn_error->setGeometry(QRect(btn_x, btn_y+20, btn_w, btn_h));
}

void opening::init_slot()
{
    connect(btn_ok,SIGNAL(clicked(bool)),this,SLOT(on_btn_ok_clicked()));
    connect(btn_error,SIGNAL(clicked(bool)),this,SLOT(on_btn_error_clicked()));
    connect(action_start,SIGNAL(triggered(bool)),this,SLOT(on_action_start_triggered()));
    connect(action_stop,SIGNAL(triggered(bool)),this,SLOT(on_action_stop_triggered()));
    connect(action_settings,SIGNAL(triggered(bool)),this,SLOT(on_action_settings_triggered()));
    connect(action_sqlsettings,SIGNAL(triggered(bool)),this,SLOT(on_action_sqlsettings_triggered()));
    connect(action_export,SIGNAL(triggered(bool)),this,SLOT(on_action_export_triggered()));
    connect(this, SIGNAL(sqlsettings_changed()), this->parent(), SLOT(closing_refresh()));
}

void opening::resizeEvent(QResizeEvent *event)
{
    int w = event->size().width();
    int h = event->size().height();
    init_table(w, h);
}

void opening::mouseMoveEvent(QMouseEvent * event)
{
    int x, y, w, h, x1, y1;

    h = this->height();
    w = this->width();
    x = event->x();
    y = event->y();
    if(((1.0-bl2)*(w-left-right) - 2) > 140)
    {
        x1 = w - 142 - right;
    }
    else
    {
        x1 = left + bl2*(w-left-right);
    }
    y1 = top + bl1*(h-top-botton);

    if(m_dragging == 0)
    {
        if(y>top && y < y1-2 && x >= x1-2 && x <= x1+2)
        {
            this->setCursor(Qt::SplitHCursor);
        }
        else if(y >= y1-2 && y <= y1+2)
        {
            this->setCursor(Qt::SplitVCursor);
        }
        else
        {
            this->setCursor(Qt::ArrowCursor);
        }
    }
    else if(m_dragging == 1)
    {
        if(x>(w-right-50))
        {
            x = w-right-50;
        }
        bl2 = (double(x-left))/(double(w-left-right));
        init_table(w, h);
    }
    else if(m_dragging == 2)
    {
        if(y<(top+50))
        {
            y = top + 50;
        }
        else if(y>(h-botton-50))
        {
            y = h-botton-50;
        }
        bl1 = (double(y-top))/(double(h-top-botton));
        init_table(w, h);
    }
}

void opening::mousePressEvent(QMouseEvent *event)
{
    int x, y, w, h, x1, y1;

    h = this->height();
    w = this->width();
    x = event->x();
    y = event->y();
    if(((1.0-bl2)*(w-left-right) - 2) > 140)
    {
        x1 = w - 142 - right;
    }
    else
    {
        x1 = left + bl2*(w-left-right);
    }
    y1 = top + bl1*(h-top-botton);

    if(y>top && y < y1-2 && x >= x1-2 && x <= x1+2)
    {
        m_dragging = 1;
    }
    else if(y >= y1-2 && y <= y1+2)
    {
        m_dragging = 2;
    }
    else
    {
        m_dragging = 0;
    }
}

void opening::mouseReleaseEvent(QMouseEvent * /*event*/)
{
    m_dragging = 0;
}

void opening::timerEvent(QTimerEvent *event)
{
    if(n_timerID == event->timerId())
    {
        if(tm->isRunning())
        {
            return;
        }
        tm->start();
    }
}

void opening::on_action_start_triggered()
{
    is_running = true;
    start();
}

void opening::on_action_stop_triggered()
{
    is_running = false;
    stop();
}

void opening::on_action_settings_triggered()
{
    settings *s = new settings();
    s->setFixedSize(690, 416);
    s->setWindowFlags(windowFlags()&~Qt::MSWindowsFixedSizeDialogHint);
    if(s->exec() == QDialog::Accepted)
    {
        g_conf.get_sql();
        n_interval = g_conf.n_interval;
        tm->reset();
        tbl_stat->clear();
        tbl_info->clear_item();
        tbl_stat->clear_item();
        for(int i=0; i<g_conf.lst_opening_sql.size(); i++)
        {
            tbl_info->add_item(g_conf.lst_opening_sql[i].intr);
            tbl_stat->add_item(g_conf.lst_opening_sql[i].intr);
        }
        for(int i=0; i<g_conf.lst_closing_sql.size(); i++)
        {
            tbl_info->add_item(g_conf.lst_closing_sql[i].intr);
        }
        emit sqlsettings_changed();
    }
    delete s;
}

void opening::on_action_sqlsettings_triggered()
{
    sqlsettings *s = new sqlsettings();
    s->setFixedSize(711, 583);
    s->setWindowFlags(windowFlags()&~Qt::MSWindowsFixedSizeDialogHint);
    if(s->exec() == QDialog::Accepted)
    {
        tm->reset();
        tbl_stat->clear();
        tbl_info->clear_item();
        tbl_stat->clear_item();
        for(int i=0; i<g_conf.lst_opening_sql.size(); i++)
        {
            tbl_info->add_item(g_conf.lst_opening_sql[i].intr);
            tbl_stat->add_item(g_conf.lst_opening_sql[i].intr);
        }
        for(int i=0; i<g_conf.lst_closing_sql.size(); i++)
        {
            tbl_info->add_item(g_conf.lst_closing_sql[i].intr);
        }
        emit sqlsettings_changed();
    }
    delete s;
}

void opening::on_action_export_triggered()
{
    export_file *e = new export_file();
    e->setFixedSize(362, 108);
    e->setWindowFlags(windowFlags()&~Qt::MSWindowsFixedSizeDialogHint);
    if(e->exec() == QDialog::Accepted)
    {
        const QString &file_name = e->get_file_name();
        int file_index = e->get_file_index();
        save_export_file(file_name, file_index);
    }
    delete e;
}

void opening::on_btn_ok_clicked()
{
    btn_error->setChecked(false);
    if(btn_ok->isChecked())
    {
        tbl_info->show_ok();
    }
    else
    {
        tbl_info->show_all();
    }
}

void opening::on_btn_error_clicked()
{
    btn_ok->setChecked(false);
    if(btn_error->isChecked())
    {
        tbl_info->show_error();
    }
    else
    {
        tbl_info->show_all();
    }
}

void opening::tm_thread_stop()
{
    if(m_tbl_info_lock.tryLock() == false)
    {
        return;
    }
    if(tm->tryLock() == false)
    {
        m_tbl_info_lock.unlock();
        return;
    }

    int i;
    QList<QList<QStandardItem*> > &lst_info = tm->get_lst_info();
    QList<QList<QStandardItem*> > &lst_stat = tm->get_lst_stat();

    udp_client client;
    QByteArray data;
    if(g_conf.is_udp_send)
    {
        client.set_server_address(g_conf.udp_server_ip);
        client.set_server_port(g_conf.udp_server_port);
    }

    for(i=0; i<lst_info.size(); i++)
    {
        tbl_info->add_row(lst_info[i]);

        int db_index = lst_info[i][4]->text().toInt();
        if(db_index>=0 && g_conf.is_udp_send)
        {
            QJsonObject json, json1;
            QString system = "JZJY_" + QString::number(db_index+1);
            json.insert("system", system);
            json.insert("service", "tradeMonitor");
            json.insert("timestamp", lst_info[i][0]->text().mid(2, 19));
            json.insert("type", "status");
            if(lst_info[i][3]->text().toInt()<0 || lst_info[i][3]->text().toInt()>=g_conf.lst_opening_sql.size())
            {
                json.insert("name", tr("非交易时间"));
            }
            else
            {
                json.insert("name", g_conf.lst_opening_sql[lst_info[i][3]->text().toInt()].intr);
            }

            json1.insert("status", lst_info[i][2]->text().toInt());
            json1.insert("db_srouce", lst_info[i][1]->text());
            json1.insert("msg", "");

            json.insert("info", json1);

            QJsonDocument document;
            document.setObject(json);
            data =document.toJson(QJsonDocument::Compact);
            //data = tmp.toUtf8();
            client.send_data(data);
        }
    }
    lst_info.clear();

    for(i=0; i<lst_stat.size(); i++)
    {
        tbl_stat->set_item(i, 0, lst_stat[i][0]);
        tbl_stat->set_item(i, 1, lst_stat[i][1]);
        tbl_stat->set_item(i, 2, lst_stat[i][2]);
    }
    lst_stat.clear();

    tm->unlock();
    m_tbl_info_lock.unlock();
}

void opening::tm_thread_connect_stop()
{
    if(tm->get_oracle_connect_status() != TM_CONNECT)
    {
        QMessageBox::warning(this, tr("警告"), tr("盘中：数据库连接不上！"));
        is_running = false;

        action_start->setEnabled(true);
        action_stop->setEnabled(false);
        action_settings->setEnabled(true);
        action_sqlsettings->setEnabled(true);
        action_export->setEnabled(true);

        return;
    }
    tm->start();
    n_timerID = startTimer(n_interval*1000);

    action_start->setEnabled(false);
    action_stop->setEnabled(true);
    action_settings->setEnabled(false);
    action_sqlsettings->setEnabled(false);
    action_export->setEnabled(true);
}

void opening::detail_thread_stop()
{
    if(m_tbl_detail_lock.tryLock() == false)
    {
        return;
    }
    if(tbl_info->dt_tryLock() == false)
    {
        m_tbl_detail_lock.unlock();
        return;
    }

    QList<QStringList>& lst_detail = tbl_info->get_lst_detail();

    /*
     * QList<QStringList> v 第一行为行标题，其他行为数据
     */

    //tbl_detail->clearContents();
    tbl_detail->clear();

    if(lst_detail.size() == 0)
    {
        tbl_detail->setRowCount(0);
        tbl_detail->setColumnCount(0);
        tbl_detail->show();
        tbl_info->dt_unlock();
        m_tbl_detail_lock.unlock();
        return;
    }

    int i, j;

    tbl_detail->setRowCount(lst_detail.size()-1);
    tbl_detail->setColumnCount(lst_detail[0].size());

    QStringList headerLabels;
    for(j=0; j<lst_detail[0].size(); j++)
    {
        headerLabels << lst_detail[0][j];
    }
    tbl_detail->setHorizontalHeaderLabels(headerLabels);

    //设置表格行标题的对齐方式
    tbl_detail->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tbl_detail->horizontalHeader()->setStretchLastSection(true);
    tbl_detail->setSelectionBehavior(QAbstractItemView::SelectRows);

    for(i=1; i<lst_detail.size(); i++)
    {
        for(j=0; j<lst_detail[i].size(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem;
            item->setText(lst_detail[i][j]);
            tbl_detail->setItem((i-1), j, item);
        }
    }

    tbl_detail->resizeColumnsToContents();
    tbl_detail->show();

    lst_detail.clear();

    tbl_info->dt_unlock();
    m_tbl_detail_lock.unlock();
}

void opening::detail_thread_connect_stop()
{
    if(tbl_info->get_dt_oracle_connect_status() != TM_CONNECT)
    {
        detail_thread_stop();
    }
}

void opening::start()
{
    if(g_conf.lst_db.size() == 0)
    {
        QMessageBox::warning(this, tr("警告"), tr("没有配置数据库！"));
        return;
    }
    if(g_conf.lst_opening_sql.size() == 0)
    {
        QMessageBox::warning(this, tr("警告"), tr("没有配置SQL！"));
        return;
    }
    if(tm->get_oracle_connect_status() == TM_CONNECT)
    {
        tm->close_oracle();
    }
    if(n_timerID>0)
    {
        killTimer(n_timerID);
        n_timerID = -1;
    }

    action_start->setEnabled(false);
    action_stop->setEnabled(false);
    action_settings->setEnabled(false);
    action_sqlsettings->setEnabled(false);
    action_export->setEnabled(false);

    tm->start();
}

void opening::stop()
{
    if(tm->isRunning())
    {
        tm->terminate();
        tm->wait();
    }

    tm->close_oracle();
    if(n_timerID > 0)
    {
        killTimer(n_timerID);
        n_timerID = -1;
    }

    action_start->setEnabled(true);
    action_stop->setEnabled(false);
    action_settings->setEnabled(true);
    action_sqlsettings->setEnabled(true);
    //action_export->setEnabled(true);
}

void opening::save_export_file(const QString &file_name, int file_index)
{
    if(file_name == "")
    {
        return;
    }

    switch (file_index) {
    case 0:
        m_tbl_info_lock.lock();
        save_tbl_info(file_name);
        m_tbl_info_lock.unlock();
        break;
    case 1:
        m_tbl_info_lock.lock();
        save_tbl_stat(file_name);
        m_tbl_info_lock.unlock();
        break;
    case 2:
        m_tbl_detail_lock.lock();
        save_tbl_detail(file_name);
        m_tbl_detail_lock.unlock();
        break;
    default:
        break;
    }
}

void opening::save_tbl_info(const QString &file_name)
{
    int row = tbl_info->get_row();
    int colunm = 4;
    QString tmp;
    int i;
    QStandardItem* item;

    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    //写BOM，默认无BOM，用Excel打开会有乱码
    QDataStream bom(&file);
    unsigned char c1 = 0xef;
    unsigned char c2 = 0xbb;
    unsigned char c3 = 0xbf;
    bom << c1 << c2 << c3;

    QTextStream out(&file);

    tmp = "";
    for(i=0; i<colunm; i++)
    {
        item = tbl_info->get_horizontalHeaderItem(i);
        tmp += item->text();
        tmp += ",";
    }
    tmp = tmp.left(tmp.size()-1);
    out << tmp << endl;
    out.flush();

    for(i=0; i<row; i++)
    {
        tmp = "";

        item = tbl_info->get_item(i, 0);
        tmp += item->text().mid(1);
        tmp += ",";

        item = tbl_info->get_item(i, 1);
        tmp += item->text();
        tmp += ",";

        item = tbl_info->get_item(i, 2);
        if(item->text().toInt() == TM_OK)
        {
            tmp += tr("正常,");
        }
        else if(item->text().toInt() == TM_ERROR)
        {
            tmp += tr("异常,");
        }
        else if(item->text().toInt() == TM_UNKNOW)
        {
            tmp += tr("未知错误,");
        }
        else if(item->text().toInt() == TM_WARNING)
        {
            tmp += tr("警告,");
        }

        item = tbl_info->get_item(i, 3);
        if(item->text().toInt()>=0 && item->text().toInt()<g_conf.lst_opening_sql.size())
        {
            tmp += g_conf.lst_opening_sql[item->text().toInt()].intr;
        }
        else if(item->text().toInt()>=g_conf.lst_opening_sql.size() &&
                item->text().toInt()<(g_conf.lst_opening_sql.size()+g_conf.lst_closing_sql.size()))
        {
            tmp += g_conf.lst_closing_sql[item->text().toInt()-g_conf.lst_opening_sql.size()].intr;
        }
        else
        {
            tmp += tr("非交易时间");
        }

        out << tmp << endl;
        out.flush();
    }

    file.close();
}

void opening::save_tbl_stat(const QString &file_name)
{
    int row = tbl_stat->get_row();
    int colunm = 3;
    QString tmp;
    int i;
    QStandardItem* item;

    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    //写BOM，默认无BOM，用Excel打开会有乱码
    QDataStream bom(&file);
    unsigned char c1 = 0xef;
    unsigned char c2 = 0xbb;
    unsigned char c3 = 0xbf;
    bom << c1 << c2 << c3;

    QTextStream out(&file);

    tmp = "";
    for(i=0; i<colunm; i++)
    {
        item = tbl_stat->get_horizontalHeaderItem(i);
        tmp += item->text();
        tmp += ",";
    }
    tmp = tmp.left(tmp.size()-1);
    out << tmp << endl;
    out.flush();

    for(i=0; i<row; i++)
    {
        tmp = "";

        item = tbl_stat->get_item(i, 0);
        if(item->text().toInt()<0 || item->text().toInt()>=g_conf.lst_opening_sql.size())
        {
            tmp += "未知";
        }
        else
        {
            tmp += g_conf.lst_opening_sql[item->text().toInt()].intr;
        }
        tmp += ",";

        item = tbl_stat->get_item(i, 1);
        tmp += item->text();
        tmp += ",";

        item = tbl_stat->get_item(i, 2);
        tmp += item->text();

        out << tmp << endl;
        out.flush();
    }

    file.close();
}

void opening::save_tbl_detail(const QString &file_name)
{
    int row = tbl_detail->rowCount();
    int colunm = tbl_detail->columnCount();
    QString tmp;
    int i, j;
    QTableWidgetItem* item;

    QFile file(file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    //写BOM，默认无BOM，用Excel打开会有乱码
    QDataStream bom(&file);
    unsigned char c1 = 0xef;
    unsigned char c2 = 0xbb;
    unsigned char c3 = 0xbf;
    bom << c1 << c2 << c3;

    QTextStream out(&file);

    tmp = "";
    for(j=0; j<colunm; j++)
    {
        item = tbl_detail->horizontalHeaderItem(j);
        tmp += item->text();
        tmp += ",";
    }
    tmp = tmp.left(tmp.size()-1);
    out << tmp << endl;
    out.flush();

    for(i=0; i<row; i++)
    {
        tmp = "";
        for(j=0; j<colunm; j++)
        {
            item = tbl_detail->item(i, j);
            tmp += item->text();
            tmp += ",";
        }

        tmp = tmp.left(tmp.size()-1);
        out << tmp << endl;
        out.flush();
    }

    file.close();
}
