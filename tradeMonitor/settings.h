#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QList>
#include <QMap>
#include <QTableWidgetItem>
#include "common.h"

namespace Ui {
class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();

private slots:
    void on_btn_OK_clicked();
    void on_btn_Cancel_clicked();
    void on_btn_sql_file_clicked();
    void on_btn_add_db_clicked();
    void on_btn_del_db_clicked();
    void on_table_db_info_cellClicked(int row, int column);
    void on_table_db_info_itemChanged(QTableWidgetItem *item);
    void on_table_db_info_doubleClicked(const QModelIndex &index);
    void on_btn_test_db_clicked();
    void on_checkBox_send_clicked();

protected:
    //bool eventFilter(QObject *watched, QEvent *event);

private:
    void init();
    void show_table();
    void show_db_info_table();

    Ui::settings *ui;
    QMap<int, QString> map_passwd;
    bool b_edit_passwd;
};

#endif // SETTINGS_H
