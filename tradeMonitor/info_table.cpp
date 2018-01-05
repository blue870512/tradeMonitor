#include "info_table.h"
#include "db_handle.h"
#include "common.h"
#include "mainwindow.h"
#include <QPainter>
#include <QHeaderView>
#include <QDateTime>
#include <QMouseEvent>
#include <QDebug>
#include <QTableWidget>
#include <QMetaType>
#include <windows.h>

info_table::info_table(QWidget * parent)
:QTableView(parent)
{
    delegate= new info_ItemDelegate(this);
    model= new info_StandardItemModel(this);
    dt = new detail_thread(this);
    dt->set_receiver(this->parent());
    show_flag = 0;
    model->setRowCount(0);
    model->setColumnCount(5);

    QStringList headerLabels;
    headerLabels << "时间" << "来源数据库" << "状态" << "说明";
    model->setHorizontalHeaderLabels(headerLabels);

    this->setModel(model);
    this->setItemDelegate(delegate);

    this->resizeColumnsToContents();
    this->setColumnWidth(0, 150);
    this->setColumnWidth(1, 140);
    this->setColumnWidth(2, 60);
    this->horizontalHeader()->setStretchLastSection(true);
    this->verticalHeader()->hide();
    this->resizeRowsToContents();
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setMouseTracking(true);
    this->setColumnHidden(4, true);

    qRegisterMetaType< QList<QPersistentModelIndex> >("QList<QPersistentModelIndex>");
    qRegisterMetaType< QAbstractItemModel::LayoutChangeHint >("QAbstractItemModel::LayoutChangeHint");
    qRegisterMetaType< QVector<int> >("QVector<int>");
    qRegisterMetaType< Qt::Orientation >("Qt::Orientation");
}

info_table::~info_table()
{
    if(dt->isRunning())
    {
        dt->terminate();
        dt->wait();
    }
    delete delegate;
    delete model;
    delete dt;
}

void info_table::add_row(const QList<QStandardItem*> &items)
{
    model->appendRow(items);
    // 限制总行数为10000
    if(model->rowCount() > 10000)
    {
        model->removeRow(0);
    }

    int stat = items[2]->text().toInt();
    if(show_flag == 1) // show ok
    {
        if(stat != TM_OK)
        {
            this->hideRow(model->rowCount()-1);
        }
    }
    else if(show_flag == 2) // show error
    {
        if(stat != TM_ERROR && stat != TM_UNKNOW)
        {
            this->hideRow(model->rowCount()-1);
        }
    }
    this->scrollToBottom();
}

void info_table::clear_item()
{
    model->clear_item();
}

void info_table::add_item(const QString &str)
{
    model->add_item(str);
}

/*
 * 刷新可显示行数，为了能够显示最底部
 * 不加refresh_scroll()，show_all()调用scrollToBottom()不会显示最底部
 */
void info_table::refresh_scroll()
{
    model->appendRow(NULL);
    model->removeRow(model->rowCount()-1);
}

void info_table::show_all()
{
    int i;
    show_flag = 0;

    for(i=0; i<model->rowCount(); i++)
    {
        this->showRow(i);
    }
    this->refresh_scroll();
    this->scrollToBottom();
}

void info_table::show_ok()
{
    int i;
    show_flag = 1;

    for(i=0; i<model->rowCount(); i++)
    {
        int stat = model->item(i,2)->text().toInt();
        if(stat == TM_OK)
        {
            this->showRow(i);
        }
        else
        {
            this->hideRow(i);
        }
    }
    this->refresh_scroll();
    this->scrollToBottom();
}

void info_table::show_error()
{
    int i;
    show_flag = 2;

    for(i=0; i<model->rowCount(); i++)
    {
        int stat = model->item(i,2)->text().toInt();
        if(stat == TM_ERROR || stat == TM_UNKNOW)
        {
            this->showRow(i);
        }
        else
        {
            this->hideRow(i);
        }
    }
    this->refresh_scroll();
    this->scrollToBottom();
}

int info_table::get_row()
{
    return model->rowCount();
}

int info_table::get_coloum()
{
    return model->columnCount();
}

QStandardItem * info_table::get_item(int row, int column)
{
    return model->item(row, column);
}

QStandardItem * info_table::get_horizontalHeaderItem(int column)
{
    return model->horizontalHeaderItem(column);
}

void info_table::mouseMoveEvent(QMouseEvent * /*event*/)
{
    this->setCursor(Qt::ArrowCursor);
}

void info_table::mousePressEvent(QMouseEvent *event)
{
    int row=this->rowAt(event->y());
    if(row>=0)
    {
        if(dt->isRunning())
        {
            return;
            //dt->wait();
        }
        this->selectRow(row);
        int i = ((QStandardItemModel*)model)->item(row, 3)->text().toInt();
        int j = ((QStandardItemModel*)model)->item(row, 4)->text().toInt();
        //detail_thread detail(this);
        dt->reset();
        dt->init(j, i, TM_OPENING);
        dt->connect_oracle();
        dt->start();
    }
}

info_ItemDelegate::info_ItemDelegate(QObject * parent)
:QItemDelegate(parent)
{
    okPixmap.load(":/icon/ok");
    errorPixmap.load(":/icon/error");
    warningPixmap.load(":/icon/warning");
}

info_ItemDelegate::~info_ItemDelegate()
{

}

void info_ItemDelegate::paint(QPainter * painter,
                           const QStyleOptionViewItem & option,
                           const QModelIndex & index) const
{
    if(index.column() == 0)
    {
        const QAbstractItemModel * model=index.model();
        QVariant var=model->data(index,Qt::DisplayRole);
        QString tmp = var.toString();
        int status = tmp.mid(0,1).toInt();


        const QPixmap & star=(status == TM_OK)?okPixmap:((status == TM_WARNING)?warningPixmap:errorPixmap);

        int width=star.width();
        int height=star.height();
        QRect rect=option.rect;
        int x=rect.x();
        int y=rect.y()+rect.height()/2-height/2;

        painter->drawPixmap(x,y,star);
        painter->drawText(x+width+2, rect.y()+18, tmp.mid(1));

        return;
    }

    QItemDelegate::paint(painter,option,index);
    return;
}

info_StandardItemModel::info_StandardItemModel(QObject * parent)
:QStandardItemModel(parent)
{
}

info_StandardItemModel::~info_StandardItemModel()
{

}

QVariant info_StandardItemModel::data(const QModelIndex & index, int role) const
{
    int column=index.column();

    if(role==Qt::DisplayRole)
    {
        //QList<QStandardItem*> lst_item;
        //lst_item = this->takeRow(row);
        const QStandardItem* item = this->itemFromIndex(index);

        switch(column)
        {
        case 0:
        case 1:
            return item->text();
        case 2:
            if(item->text().toInt() == TM_OK)
            {
                return tr("正常");
            }
            else if(item->text().toInt() == TM_ERROR)
            {
                return tr("异常");
            }
            else if(item->text().toInt() == TM_UNKNOW)
            {
                return tr("未知错误");
            }
            else if(item->text().toInt() == TM_WARNING)
            {
                return tr("警告");
            }
        case 3:
            if(item->text().toInt()<0 || item->text().toInt()>=info.size())
            {
                return tr("非交易时间");
            }
            else
            {
                return info[item->text().toInt()];
            }
        }
    }

    return QStandardItemModel::data(index,role);
}

void info_StandardItemModel::clear_item()
{
    info.clear();
}

void info_StandardItemModel::add_item(const QString &str)
{
    QString s = str;
    info.append(s);
}

