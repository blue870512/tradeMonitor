#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "global.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tab = new QTabWidget(this);
    tab1 = new opening(this);
    tab2 = new closing(this);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tab;
}

void MainWindow::init()
{
    int w = this->width();
    int h = this->height();

    tab->setObjectName(QStringLiteral("tab"));
    tab->setEnabled(true);
    tab->setGeometry(QRect(5, 5, w-10, h-10));
    tab->setLayoutDirection(Qt::LeftToRight);
    tab->setTabPosition(QTabWidget::West);
    tab1->setObjectName(QStringLiteral("tab1"));
    tab->addTab(tab1, QString());
    tab2->setObjectName(QStringLiteral("tab2"));
    tab->addTab(tab2, QString());

    tab->setTabText(tab->indexOf(tab1), QApplication::translate("MainWindow", "盘中", Q_NULLPTR));
    tab->setTabText(tab->indexOf(tab2), QApplication::translate("MainWindow", "盘后", Q_NULLPTR));

    QDateTime time = QDateTime::currentDateTime();
    QString str_curtime = time.toString("hhmmss");

    if(str_curtime < g_conf.str_close)
    {
        tab->setCurrentIndex(0);
    }
    else
    {
        tab->setCurrentIndex(1);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    int w = event->size().width();
    int h = event->size().height();
    tab->setGeometry(QRect(5, 5, w-10, h-10));
}

void MainWindow::closing_refresh()
{
    if(tab2)
    {
        tab2->init();
    }
}
