#include "status_table.h"

#include<QPainter>
#include <QHeaderView>

status_table::status_table(QWidget * parent)
:QTableView(parent)
{
    delegate=new status_ItemDelegate;
    model=new status_StandardItemModel;
    model->setRowCount(0);
    model->setColumnCount(3);

    QStringList headerLabels;
    headerLabels << "标号" << "异常数" << "总数";
    model->setHorizontalHeaderLabels(headerLabels);

    this->setModel(model);
    this->setItemDelegate(delegate);

    this->resizeColumnsToContents();
    this->setColumnWidth(0, 40);
    this->setColumnWidth(1, 50);
    this->horizontalHeader()->setStretchLastSection(true);
    this->verticalHeader()->hide();
    this->resizeRowsToContents();
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setMouseTracking(true);
}

status_table::~status_table()
{
    delete delegate;
    delete model;
}

void status_table::set_item(int row, int column, QStandardItem *aitem)
{
    model->setItem(row, column, aitem);
}

void status_table::clear_item()
{
    model->clear_item();
}

void status_table::add_item(const QString &str)
{
    model->add_item(str);
}

int status_table::get_row()
{
    return model->rowCount();
}

int status_table::get_coloum()
{
    return model->columnCount();
}

QStandardItem * status_table::get_item(int row, int column)
{
    return model->item(row, column);
}

QStandardItem * status_table::get_horizontalHeaderItem(int column)
{
    return model->horizontalHeaderItem(column);
}

void status_table::clear()
{
    //this->clearSpans();
    model->removeRows(0, model->rowCount());
}

void status_table::mouseMoveEvent(QMouseEvent * event)
{
    this->setCursor(Qt::ArrowCursor);
}

status_ItemDelegate::status_ItemDelegate(QObject * parent)
:QItemDelegate(parent)
{
}

status_ItemDelegate::~status_ItemDelegate()
{

}

void status_ItemDelegate::paint(QPainter * painter,
                           const QStyleOptionViewItem & option,
                           const QModelIndex & index) const
{
    QItemDelegate::paint(painter,option,index);
    return;
}

status_StandardItemModel::status_StandardItemModel(QObject * parent)
:QStandardItemModel(parent)
{
}

status_StandardItemModel::~status_StandardItemModel()
{

}

QVariant status_StandardItemModel::data(const QModelIndex & index, int role) const
{
    int column=index.column();
    const QStandardItem* item = this->itemFromIndex(index);

    if(role==Qt::DisplayRole)
    {
        switch(column)
        {
        case 0:
            return (item->text().toInt()+1);
        case 1:
        case 2:
            return item->text().toInt();
        }
    }

    if(role==Qt::ToolTipRole && column==0)
    {
        if(item->text().toInt()<0 || item->text().toInt()>=info.size())
        {
            return tr("未知");
        }
        else
        {
            return info[(item->text().toInt())];
        }
    }

    return QStandardItemModel::data(index,role);
}

void status_StandardItemModel::add_item(const QString &str)
{
    QString s = str;
    info.append(s);
}

void status_StandardItemModel::clear_item()
{
    info.clear();
}
