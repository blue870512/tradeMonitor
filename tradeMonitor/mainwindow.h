#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include "opening.h"
#include "closing.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void closing_refresh();

private:
    void init();

    Ui::MainWindow *ui;
    QTabWidget *tab;
    opening *tab1;
    closing *tab2;
};

#endif // MAINWINDOW_H
