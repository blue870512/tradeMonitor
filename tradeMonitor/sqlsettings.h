#ifndef SQLSETTINGS_H
#define SQLSETTINGS_H

#include "common.h"
#include <QDialog>
#include <QList>
#include <QMouseEvent>

namespace Ui {
class sqlsettings;
}

class sqlsettings : public QDialog
{
    Q_OBJECT

public:
    explicit sqlsettings(QWidget *parent = 0);
    ~sqlsettings();

protected:
    void mouseMoveEvent(QMouseEvent * event);

private slots:
    void on_tbl_sql_clicked(const QModelIndex &index);
    void on_btn_add_clicked();
    void on_btn_del_clicked();
    void on_btn_save_clicked();
    void on_btn_cancel_clicked();
    void on_edit_sql_key_textChanged();
    void on_comb_sql_type_currentIndexChanged(int index);
    void on_edit_sql_value_textChanged();

private:
    void init_ui();
    void set_sql_edit_enable(bool b);

    Ui::sqlsettings *ui;
    QList<sql_info> lst_sql;
    int sql_index;
};

#endif // SQLSETTINGS_H
