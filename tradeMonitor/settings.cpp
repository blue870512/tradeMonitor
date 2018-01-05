#include "settings.h"
#include "ui_settings.h"
#include "global.h"
#include <QSettings>
#include <QFileDialog>
#include <QTime>
#include <QDebug>
#include <QIntValidator>
#include <QtNetwork>

settings::settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    //ui->table_db_info->installEventFilter(this);
    ui->text_interval->setValidator(new QIntValidator(10, 1800, this));
    b_edit_passwd = false;
    init();
}

settings::~settings()
{
    delete ui;
}

void settings::init()
{
    int i;
    QTime time_tmp;

    for(i=0; i<g_conf.lst_db.size(); i++)
    {
        map_passwd[i] = g_conf.lst_db[i].get_password();
    }

    if(g_conf.str_open.size() != 6)
    {
        g_conf.str_open = "090000";
    }
    time_tmp.setHMS(g_conf.str_open.mid(0,2).toInt(), g_conf.str_open.mid(2,2).toInt(), g_conf.str_open.mid(4,2).toInt());
    ui->time_open->setTime(time_tmp);

    if(g_conf.str_close.size() != 6)
    {
        g_conf.str_close = "160000";
    }
    time_tmp.setHMS(g_conf.str_close.mid(0,2).toInt(), g_conf.str_close.mid(2,2).toInt(), g_conf.str_close.mid(4,2).toInt());
    ui->time_close->setTime(time_tmp);

    if(g_conf.str_done.size() != 6)
    {
        g_conf.str_done = "220000";
    }
    time_tmp.setHMS(g_conf.str_done.mid(0,2).toInt(), g_conf.str_done.mid(2,2).toInt(), g_conf.str_done.mid(4,2).toInt());
    ui->time_done->setTime(time_tmp);

    ui->text_interval->setText(QString::number(g_conf.n_interval));
    ui->text_sql_file->setText(g_conf.str_sql_file);

    ui->checkBox_send->setChecked(g_conf.is_udp_send);
    ui->edit_ip_addr->setText(g_conf.udp_server_ip);
    ui->edit_port->setText(QString::number(g_conf.udp_server_port));
    ui->edit_ip_addr->setEnabled(g_conf.is_udp_send);
    ui->edit_port->setEnabled(g_conf.is_udp_send);

    show_table();
}

void settings::show_table()
{
    show_db_info_table();
    //show_sql_info_table();
}

void settings::show_db_info_table()
{
    QTableWidget *table = ui->table_db_info;
    int row;

    table->setRowCount(g_conf.lst_db.size());
    table->setColumnCount(6);

    //设置表格行标题
    QStringList headerLabels;
    headerLabels << "host" << "dbName" << "username" << "password" << "port" << "status";
    table->setHorizontalHeaderLabels(headerLabels);

    //设置表格行标题的对齐方式
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    //设置行表题
    QStringList rowLabels;
    QString tmp;
    row = 0;
    while(row < g_conf.lst_db.size())
    {
        tmp = "DB" + QString::number(row);
        rowLabels << tmp;
        row++;
    }
    table->setVerticalHeaderLabels(rowLabels);

    //自动调整最后一列的宽度使它和表格的右边界对齐
    table->horizontalHeader()->setStretchLastSection(true);

    //设置表格的选择方式
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置编辑方式
    table->setEditTriggers(QAbstractItemView::DoubleClicked);

    for (row = 0; row < g_conf.lst_db.size(); ++row)
    {
        QTableWidgetItem *item_host, *item_dbName, *item_username, *item_password, *item_port, *item_status;

        item_host = new QTableWidgetItem;
        item_dbName = new QTableWidgetItem;
        item_username = new QTableWidgetItem;
        item_password = new QTableWidgetItem;
        item_port = new QTableWidgetItem;
        item_status = new QTableWidgetItem;

        item_host->setText(g_conf.lst_db[row].get_host());
        table->setItem(row, 0, item_host);

        item_dbName->setText(g_conf.lst_db[row].get_dbName());
        table->setItem(row, 1, item_dbName);

        item_username->setText(g_conf.lst_db[row].get_username());
        table->setItem(row, 2, item_username);

        item_password->setText("******");
        //item_password->setText(db.password);
        table->setItem(row, 3, item_password);

        item_port->setText(QString::number(g_conf.lst_db[row].get_port()));
        table->setItem(row, 4, item_port);

        item_status->setText("未知状态");
        table->setItem(row, 5, item_status);
    }

    table->resizeColumnsToContents();
    table->show();
}

void settings::on_btn_OK_clicked()
{
    QTableWidget *table = ui->table_db_info;
    QTableWidgetItem *item_host, *item_dbName, *item_username, *item_port;
    QString tmp;
    int row;
    db_handle db;

    g_conf.lst_db.clear();

    for(row=0; row<table->rowCount(); row++)
    {
        item_host = table->item(row, 0);
        item_dbName = table->item(row, 1);
        item_username = table->item(row, 2);
        //item_password = table->item(row, 3);
        item_port = table->item(row, 4);

        if(item_host->text() != "" && item_dbName->text() != ""
                && item_username->text() != "")
        {
            db.set_host(item_host->text());
            db.set_dbName(item_dbName->text());
            db.set_username(item_username->text());
            db.set_password(map_passwd[row]);//item_password->text();
            db.set_port(item_port->text().toInt());
            db.set_index(row);

            g_conf.lst_db.append(db);
        }
    }

    QSettings settings("tradeMonitor.ini", QSettings::IniFormat);
    settings.clear();

    for(row=0; row<g_conf.lst_db.size(); row++)
    {
        db = g_conf.lst_db[row];
        tmp = "DB" + QString::number(row);
        QString passwd;
        QString passwd_tmp = db.get_password();
        encode(passwd_tmp, passwd);

        settings.beginGroup(tmp);
        settings.setValue("host", db.get_host());
        settings.setValue("dbName", db.get_dbName());
        settings.setValue("username", db.get_username());
        settings.setValue("password", passwd);
        settings.setValue("port", db.get_port());
        settings.endGroup();
    }

    g_conf.n_interval = ui->text_interval->text().toInt();
    g_conf.str_sql_file = ui->text_sql_file->text();
    g_conf.str_open = ui->time_open->time().toString("hhmmss");
    g_conf.str_close = ui->time_close->time().toString("hhmmss");
    g_conf.str_done = ui->time_done->time().toString("hhmmss");

    if(g_conf.n_interval < 10)
    {
        g_conf.n_interval = 10;
    }
    if(g_conf.n_interval > 1800)
    {
        g_conf.n_interval = 1800;
    }
    if(g_conf.n_interval == 0)
    {
        g_conf.n_interval = 60;
    }

    if(g_conf.str_close < g_conf.str_open)
    {
        g_conf.str_close = g_conf.str_open;
    }
    if(g_conf.str_done < g_conf.str_close)
    {
        g_conf.str_done = g_conf.str_close;
    }

    if(g_conf.str_open > "090000")
    {
        g_conf.str_open = "090000";
    }
    if(g_conf.str_close > "180000")
    {
        g_conf.str_close = "180000";
    }
    if(g_conf.str_done > "230000")
    {
        g_conf.str_done = "230000";
    }

    g_conf.is_udp_send = ui->checkBox_send->isChecked();
    g_conf.udp_server_ip = ui->edit_ip_addr->text();
    g_conf.udp_server_port = ui->edit_port->text().toInt();
    QHostAddress test;
    if(!test.setAddress(g_conf.udp_server_ip))
    {
        g_conf.udp_server_ip = "127.0.0.1";
    }

    settings.beginGroup("common");
    settings.setValue("interval", g_conf.n_interval);
    settings.setValue("sqlfile", g_conf.str_sql_file);
    settings.setValue("open", g_conf.str_open);
    settings.setValue("close", g_conf.str_close);
    settings.setValue("done", g_conf.str_done);
    settings.setValue("send", g_conf.is_udp_send);
    settings.setValue("serverip", g_conf.udp_server_ip);
    settings.setValue("serverport", g_conf.udp_server_port);
    settings.endGroup();

    accept();
}

void settings::on_btn_Cancel_clicked()
{
    close();
}

void settings::on_btn_sql_file_clicked()
{
    QFileDialog *fd = new QFileDialog(this, "XML file", ".", "*.xml");
    if(fd->exec()==QFileDialog::Accepted)
    {
        QString file=fd->selectedFiles()[0];
        if(file.mid(file.size()-4) != ".xml")
        {
            file += ".xml";
        }
        ui->text_sql_file->setText(file);
    }
}

void settings::on_btn_add_db_clicked()
{
    QTableWidget *table = ui->table_db_info;
    QTableWidgetItem *item_host, *item_dbName, *item_username, *item_password, *item_port, *item_status;
    int row_count = table->rowCount();
    QString tmp = "DB" + QString::number(row_count);
    QTableWidgetItem *vHeader = new QTableWidgetItem(tmp);

    table->insertRow(row_count);
    table->setVerticalHeaderItem(row_count, vHeader);

    item_host = new QTableWidgetItem;
    item_dbName = new QTableWidgetItem;
    item_username = new QTableWidgetItem;
    item_password = new QTableWidgetItem;
    item_port = new QTableWidgetItem;
    item_status = new QTableWidgetItem;

    item_host->setText("");
    table->setItem(row_count, 0, item_host);

    item_dbName->setText("");
    table->setItem(row_count, 1, item_dbName);

    item_username->setText("");
    table->setItem(row_count, 2, item_username);

    item_password->setText("");
    table->setItem(row_count, 3, item_password);

    item_port->setText("");
    table->setItem(row_count, 4, item_port);

    item_status->setText("未知状态");
    table->setItem(row_count, 5, item_status);
}

void settings::on_btn_del_db_clicked()
{
    QTableWidget *table = ui->table_db_info;
    int row_count = table->currentRow();

    //qDebug() << row_count;
    table->removeRow(row_count);
}

void settings::on_table_db_info_cellClicked(int /*row*/, int /*column*/)
{
    ui->btn_del_db->setEnabled(true);
}

void settings::on_table_db_info_itemChanged(QTableWidgetItem *item)
{
    if(b_edit_passwd)
    {
        int row = item->row();
        int column = item->column();

        if(column == 3)
        {
            map_passwd[row] = item->text();
        }
    }
}

void settings::on_table_db_info_doubleClicked(const QModelIndex &index)
{
    //int row = index.row();
    int column = index.column();

    if(column == 3)
    {
        b_edit_passwd = true;
    }
    else
    {
        b_edit_passwd = false;
    }
}

void settings::on_btn_test_db_clicked()
{
    int i;
    db_handle db;
    QString conn_name;
    QTableWidgetItem *item_status;
    QTableWidget *table = ui->table_db_info;

    for(i=0; i<g_conf.lst_db.size(); i++)
    {
        db = g_conf.lst_db[i];
        conn_name = "db_" + db.get_host() + "_for_test";
        db.set_str_conn_name(conn_name);
        item_status = new QTableWidgetItem;
        if(db.connect_oracle())
        {
            item_status->setText("可连接");
            table->setItem(i, 5, item_status);
            db.close_oracle();
        }
        else
        {
            item_status->setText("不可连接");
            table->setItem(i, 5, item_status);
        }
    }
}

void settings::on_checkBox_send_clicked()
{
    ui->edit_ip_addr->setEnabled(ui->checkBox_send->isChecked());
    ui->edit_port->setEnabled(ui->checkBox_send->isChecked());
    /*
    if(ui->checkBox_send->checkState() == Qt::Checked)
    {
        ui->edit_ip_addr->setEnabled(true);
        ui->edit_port->setEnabled(true);
    }
    else
    {
        ui->edit_ip_addr->setEnabled(false);
        ui->edit_port->setEnabled(false);
    }
    */
}
