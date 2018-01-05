#ifndef STATUS_TABLE_H
#define STATUS_TABLE_H

#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QPixmap>
#include <QStringList>

class status_table;
class status_ItemDelegate;
class status_StandardItemModel;

class status_table : public QTableView
{
    Q_OBJECT
public:
    status_table(QWidget * parent=0);
    virtual ~status_table();

    void set_item(int row, int column, QStandardItem *aitem);
    void clear_item();
    void add_item(const QString &str);

    int get_row();
    int get_coloum();
    QStandardItem* get_item(int row, int column);
    QStandardItem* get_horizontalHeaderItem(int column);
    void clear();

protected:
    void mouseMoveEvent(QMouseEvent * event);

private:
    status_ItemDelegate * delegate;
    status_StandardItemModel * model;
};

class status_ItemDelegate:public QItemDelegate
{
    Q_OBJECT
public:
    status_ItemDelegate(QObject * parent=0);
    virtual ~ status_ItemDelegate();

    void paint(QPainter * painter,
        const QStyleOptionViewItem & option,
        const QModelIndex & index) const;

};

class status_StandardItemModel:public QStandardItemModel
{
    Q_OBJECT
public:
    status_StandardItemModel(QObject * parent=0);
    virtual ~ status_StandardItemModel();

    QVariant data(const QModelIndex & index, int role=Qt::DisplayRole) const;
    void add_item(const QString &str);
    void clear_item();

private:
    QStringList info;
};

#endif // STATUS_TABLE_H
