#ifndef EXPORT_FILE_H
#define EXPORT_FILE_H

#include <QDialog>

namespace Ui {
class export_file;
}

class export_file : public QDialog
{
    Q_OBJECT

public:
    explicit export_file(QWidget *parent = 0);
    ~export_file();

    const QString & get_file_name();
    int get_file_index();

private slots:
    void on_comb_file_currentIndexChanged(int index);
    void on_btn_file_clicked();
    void on_btn_save_clicked();
    void on_btn_cancel_clicked();
    void on_edit_file_textChanged(const QString &arg1);

private:
    void init();

    Ui::export_file *ui;

    QString file_name;
    QString file_path;
    int file_index;
};

#endif // EXPORT_FILE_H
