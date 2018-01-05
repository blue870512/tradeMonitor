#ifndef INFO_TABLE_H
#define INFO_TABLE_H

#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QPixmap>
#include <QStringList>
#include <QList>
#include "detail_thread.h"

class info_table;
class info_ItemDelegate;
class info_StandardItemModel;

class info_table : public QTableView
{
    Q_OBJECT
public:
    info_table(QWidget * parent=0);
    virtual ~info_table();

    void add_row(const QList<QStandardItem*> &items);
    void clear_item();
    void add_item(const QString &str);
    void show_all();
    void show_ok();
    void show_error();
    int get_row();
    int get_coloum();
    QStandardItem* get_item(int row, int column);
    QStandardItem* get_horizontalHeaderItem(int column);

    QList<QStringList> & get_lst_detail() {return dt->get_lst_detail();}
    int get_dt_oracle_connect_status() {return dt->get_oracle_connect_status();}
    void dt_lock() {dt->lock();}
    void dt_unlock() {dt->unlock();}
    bool dt_tryLock() {return dt->tryLock();}

protected:
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);

private:
    void refresh_scroll();

    info_ItemDelegate * delegate;
    info_StandardItemModel * model;
    detail_thread* dt;
    int show_flag;
};

class info_ItemDelegate:public QItemDelegate
{
    Q_OBJECT
public:
    info_ItemDelegate(QObject * parent=0);
    virtual ~ info_ItemDelegate();

    void paint(QPainter * painter,
        const QStyleOptionViewItem & option,
        const QModelIndex & index) const;
    /*
    bool editorEvent(QEvent * event,
        QAbstractItemModel * model,
        const QStyleOptionViewItem & option,
        const QModelIndex & index);
        */

private:
    /*
    void query_detail(int i, int j);
    void show_table(const QList<QStringList> &v);
    */

    QPixmap okPixmap;
    QPixmap errorPixmap;
    QPixmap warningPixmap;
};

class info_StandardItemModel:public QStandardItemModel
{
    Q_OBJECT
public:
    info_StandardItemModel(QObject * parent=0);
    virtual ~ info_StandardItemModel();

    QVariant data(const QModelIndex & index, int role=Qt::DisplayRole) const;
    /*
    QVariant headerData(int section,
        Qt::Orientation orientation,
        int role=Qt::DisplayRole) const;
        */
    void clear_item();
    void add_item(const QString &str);

private:
    QStringList info;
};

#endif // INFO_TABLE_H
