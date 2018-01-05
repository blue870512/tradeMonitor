#ifndef OPENING_H
#define OPENING_H

//盘中查询界面

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include <QStatusBar>
#include <QMutex>
#include "info_table.h"
#include "status_table.h"
#include "db_handle.h"
#include "tm_thread.h"

class opening : public QMainWindow
{
    Q_OBJECT
public:
    explicit opening(QWidget *parent = 0);
    ~opening();

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void timerEvent( QTimerEvent *event );

private slots:
    void on_action_start_triggered();
    void on_action_stop_triggered();
    void on_action_settings_triggered();
    void on_action_sqlsettings_triggered();
    void on_action_export_triggered();
    void on_btn_ok_clicked();
    void on_btn_error_clicked();

    void tm_thread_stop();
    void tm_thread_connect_stop();
    void detail_thread_stop();
    void detail_thread_connect_stop();

signals:
    void sqlsettings_changed();

private:
    void init();
    void init_ui();
    void init_table(int w, int h);
    void init_slot();

    void start();
    void stop();

    void save_export_file(const QString &file_name, int file_index);
    void save_tbl_info(const QString &file_name);
    void save_tbl_stat(const QString &file_name);
    void save_tbl_detail(const QString &file_name);

    QAction *action_start;
    QAction *action_stop;
    QAction *action_settings;
    QAction *action_sqlsettings;
    QAction *action_export;
    QWidget *centralWidget;
    QPushButton *btn_ok;
    QPushButton *btn_error;
    QToolBar *mainToolBar;

    info_table *tbl_info;
    status_table *tbl_stat;
    QTableWidget *tbl_detail;
    tm_thread *tm;

    QMutex m_tbl_info_lock;
    QMutex m_tbl_detail_lock;

    int n_interval;
    int n_timerID;

    double bl1, bl2;
    int top;
    int botton;
    int left;
    int right;
    int m_dragging;
    bool is_running;
};

#endif // OPENING_H
