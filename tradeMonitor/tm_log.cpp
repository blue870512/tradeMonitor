#include "tm_log.h"
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>

tm_log::tm_log(QObject *parent) : QObject(parent)
{
    QDir dir1;
    QString path1=dir1.currentPath();
    QString path2 = path1 + "/log/";
    QDir dir2(path2);
    if(!dir2.exists())
    {
        dir2.mkdir(path2);
    }
}

void tm_log::write_log(const QString &log)
{
    lock.lock();

    QDateTime time = QDateTime::currentDateTime();
    QString s = time.toString("yyyyMMdd");
    QString tmp = time.toString("[yyyy-MM-dd hh:mm:ss]");

    s = "./log/log_" + s + ".txt";
    tmp = tmp + " " + log;

    QFile file(s);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        return;
    }
    QTextStream out(&file);

    out << tmp << endl;
    out.flush();

    file.close();

    lock.unlock();
}

void tm_log::write_log(const int &i)
{
    QString s = QString::number(i);
    write_log(s);
}
