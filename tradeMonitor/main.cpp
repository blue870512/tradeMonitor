#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    g_conf.init();

    MainWindow w;
    w.show();

    return a.exec();
}
