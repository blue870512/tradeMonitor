#include "sqlsettings.h"
#include "ui_sqlsettings.h"
#include "global.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>

sqlsettings::sqlsettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sqlsettings)
{
    ui->setupUi(this);

    int i;
    for(i=0; i<g_conf.lst_opening_sql.size(); i++)
    {
        lst_sql.append(g_conf.lst_opening_sql[i]);
    }
    for(i=0; i<g_conf.lst_closing_sql.size(); i++)
    {
        lst_sql.append(g_conf.lst_closing_sql[i]);
    }
    sql_index = -1;

    init_ui();
    this->setMouseTracking(true);
}

sqlsettings::~sqlsettings()
{
    delete ui;
}

void sqlsettings::on_tbl_sql_clicked(const QModelIndex &index)
{
    sql_index = index.row();

    if(sql_index>=0 && sql_index <lst_sql.size())
    {
        ui->edit_sql_key->setText(lst_sql[sql_index].intr);
        ui->comb_sql_type->setCurrentIndex(lst_sql[sql_index].type);
        ui->edit_sql_value->setText(lst_sql[sql_index].sql);

        ui->btn_del->setEnabled(true);
        set_sql_edit_enable(true);
    }
    else
    {
        ui->edit_sql_key->setText("");
        ui->comb_sql_type->setCurrentIndex(0);
        ui->edit_sql_value->setText("");

        ui->btn_del->setEnabled(false);
        set_sql_edit_enable(false);
    }
}

void sqlsettings::on_btn_add_clicked()
{
    QTableWidget *table = ui->tbl_sql;
    QTableWidgetItem *item_key, *item_type, *item_value;
    int row = table->rowCount();
    sql_info sql;

    sql.intr = "";
    sql.type = 0;
    sql.sql = "";

    table->insertRow(row);
    lst_sql.insert(row, sql);
    sql_index = row;
    table->selectRow(row);

    item_key = new QTableWidgetItem;
    item_type = new QTableWidgetItem;
    item_value = new QTableWidgetItem;

    item_key->setText("");
    table->setItem(row, 0, item_key);

    item_type->setText("");
    table->setItem(row, 1, item_type);

    item_value->setText("");
    table->setItem(row, 2, item_value);

    ui->edit_sql_key->setText("");
    ui->comb_sql_type->setCurrentIndex(0);
    ui->edit_sql_value->setText("");

    set_sql_edit_enable(true);
}

void sqlsettings::on_btn_del_clicked()
{
    QTableWidget *table = ui->tbl_sql;

    if(sql_index>=0 && sql_index <lst_sql.size())
    {
        table->removeRow(sql_index);
        lst_sql.removeAt(sql_index);
        sql_index = -1;

        ui->edit_sql_key->setText("");
        ui->comb_sql_type->setCurrentIndex(0);
        ui->edit_sql_value->setText("");

        ui->btn_del->setEnabled(false);
        set_sql_edit_enable(false);
    }
}

void sqlsettings::on_btn_save_clicked()
{
    int i;

    QFile file(g_conf.str_sql_file);
    if(!file.open(QIODevice::WriteOnly | QFile::Text))
    {
        return;
    }

    g_conf.lst_closing_sql.clear();
    g_conf.lst_opening_sql.clear();

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("sqllist");

    for(i=0; i<lst_sql.size(); i++)
    {
        if(lst_sql[i].type == TM_OPENING)
        {
            g_conf.lst_opening_sql.append(lst_sql[i]);
        }
        else if(lst_sql[i].type == TM_CLOSING)
        {
            g_conf.lst_closing_sql.append(lst_sql[i]);
        }

        xmlWriter.writeStartElement("sql");
        xmlWriter.writeTextElement("key",lst_sql[i].intr);
        xmlWriter.writeTextElement("type",QString::number(lst_sql[i].type));
        xmlWriter.writeTextElement("value", lst_sql[i].sql);
        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();

    accept();
}

void sqlsettings::on_btn_cancel_clicked()
{
    close();
}

void sqlsettings::init_ui()
{
    QTableWidget *table = ui->tbl_sql;
    int row;
    QString tmp;

    table->setRowCount(lst_sql.size());
    table->setColumnCount(3);

    //设置表格行标题
    QStringList headerLabels;
    headerLabels << "说明" << "类型" << "详细";
    table->setHorizontalHeaderLabels(headerLabels);

    //设置表格行标题的对齐方式
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    //自动调整最后一列的宽度使它和表格的右边界对齐
    table->horizontalHeader()->setStretchLastSection(true);

    //设置表格的选择方式
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置编辑方式
    table->setEditTriggers(QAbstractItemView::DoubleClicked);

    for (row = 0; row < lst_sql.size(); ++row)
    {
        QTableWidgetItem *item_key, *item_type, *item_value;
        sql_info sql;

        item_key = new QTableWidgetItem;
        item_type = new QTableWidgetItem;
        item_value = new QTableWidgetItem;

        sql =  lst_sql[row];

        item_key->setText(sql.intr);
        table->setItem(row, 0, item_key);

        if(sql.type == 0)
        {
            item_type->setText(tr("盘前检测"));
        }
        else if(sql.type == 1)
        {
            item_type->setText(tr("盘中检测"));
        }
        else if(sql.type == 2)
        {
            item_type->setText(tr("盘后检测"));
        }
        else
        {
            item_type->setText(tr("未知类型"));
        }
        table->setItem(row, 1, item_type);

        if(sql.sql.size()>50)
        {
            tmp = sql.sql.mid(0,50);
        }
        else
        {
            tmp = sql.sql;
        }
        item_value->setText(tmp);
        table->setItem(row, 2, item_value);
    }

    table->resizeColumnsToContents();
    table->show();

    ui->comb_sql_type->addItem(tr("盘前检测"));
    ui->comb_sql_type->addItem(tr("盘中检测"));
    ui->comb_sql_type->addItem(tr("盘后检测"));
    ui->comb_sql_type->addItem(tr("未知类型"));
}

void sqlsettings::set_sql_edit_enable(bool b)
{
    ui->edit_sql_key->setEnabled(b);
    ui->comb_sql_type->setEnabled(b);
    ui->edit_sql_value->setEnabled(b);
}

void sqlsettings::on_edit_sql_key_textChanged()
{
    if(sql_index>=0 && sql_index <lst_sql.size())
    {
        lst_sql[sql_index].intr = ui->edit_sql_key->toPlainText();
    }
}

void sqlsettings::on_comb_sql_type_currentIndexChanged(int index)
{
    if(sql_index>=0 && sql_index <lst_sql.size())
    {
        lst_sql[sql_index].type = index;
    }
}

void sqlsettings::on_edit_sql_value_textChanged()
{
    if(sql_index>=0 && sql_index <lst_sql.size())
    {
        lst_sql[sql_index].sql = ui->edit_sql_value->toPlainText();
    }
}

void sqlsettings::mouseMoveEvent(QMouseEvent * event)
{
    QTableWidgetItem *item_key, *item_type, *item_value;
    QTableWidget *table = ui->tbl_sql;

    item_key = new QTableWidgetItem;
    item_type = new QTableWidgetItem;
    item_value = new QTableWidgetItem;

    item_key->setText(ui->edit_sql_key->toPlainText());
    table->setItem(sql_index, 0, item_key);

    if(ui->comb_sql_type->currentIndex() == 0)
    {
        item_type->setText(tr("盘前检测"));
    }
    else if(ui->comb_sql_type->currentIndex() == 1)
    {
        item_type->setText(tr("盘中检测"));
    }
    else if(ui->comb_sql_type->currentIndex() == 2)
    {
        item_type->setText(tr("盘后检测"));
    }
    else
    {
        item_type->setText(tr("未知类型"));
    }
    table->setItem(sql_index, 1, item_type);

    QString tmp = ui->edit_sql_value->toPlainText();
    if(tmp.size()>50)
    {
        tmp = tmp.mid(0,50);
    }
    item_value->setText(tmp);
    table->setItem(sql_index, 2, item_value);
}
