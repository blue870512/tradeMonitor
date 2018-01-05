#include "export_file.h"
#include "ui_export_file.h"
#include <QFileDialog>
#include <QDir>
#include <QDateTime>

export_file::export_file(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::export_file)
{
    file_name = "";
    file_path = "";
    file_index = 0;

    ui->setupUi(this);

    init();
}

export_file::~export_file()
{
    delete ui;
}

void export_file::init()
{
    //创建export目录
    QDir dir1;
    QString path1=dir1.currentPath();
    QString path2 = path1 + "/export/";
    QDir dir2(path2);
    if(!dir2.exists())
    {
        dir2.mkdir(path2);
    }

    file_path = path2;

    QStringList lst_file;

    lst_file.append(tr("状态信息"));
    lst_file.append(tr("统计信息"));
    lst_file.append(tr("细节信息"));

    ui->comb_file->addItems(lst_file);

    QDateTime time = QDateTime::currentDateTime();
    QString s = time.toString("yyyyMMddhhmmss");
    file_name = path2 + "状态信息_" + s + ".csv";
    ui->edit_file->setText(file_name);
}

const QString &export_file::get_file_name()
{
    return file_name;
}

int export_file::get_file_index()
{
    return file_index;
}

void export_file::on_comb_file_currentIndexChanged(int index)
{
    file_index = index;
    QString tmp;

    if(index == 0)
    {
        tmp = "状态信息_";
    }
    else if(index == 1)
    {
        tmp = "统计信息_";
    }
    else if(index == 2)
    {
        tmp = "细节信息_";
    }

    QDateTime time = QDateTime::currentDateTime();
    QString s = time.toString("yyyyMMddhhmmss");
    file_name = file_path + tmp + s + ".csv";
    ui->edit_file->setText(file_name);
}

void export_file::on_btn_file_clicked()
{
    QFileDialog *fd = new QFileDialog(this, "CSV file", "./export/", "*.csv");
    if(fd->exec()==QFileDialog::Accepted)
    {
        QString file=fd->selectedFiles()[0];
        if(file.mid(file.size()-4) != ".csv")
        {
            file += ".csv";
        }
        ui->edit_file->setText(file);
        file_name = file;
    }
}

void export_file::on_btn_save_clicked()
{
    accept();
}

void export_file::on_btn_cancel_clicked()
{
    close();
}

void export_file::on_edit_file_textChanged(const QString &arg1)
{
    file_name = arg1;
}
