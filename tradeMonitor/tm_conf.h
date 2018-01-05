#ifndef TM_CONF_H
#define TM_CONF_H

#include <QDomDocument>
#include "common.h"
#include "db_handle.h"

class tm_conf
{
public:
    tm_conf();
    ~tm_conf();

    void init();
    void get_ini();
    void get_sql();

    QList<db_handle> lst_db;
    QList<sql_info> lst_opening_sql;
    QList<sql_info> lst_closing_sql;
    int n_interval;
    QString str_sql_file;
    QString str_open;
    QString str_close;
    QString str_done;
    bool is_udp_send;
    QString udp_server_ip;
    int udp_server_port;

private:
    void listDom(QDomElement& docElem);
};

#endif // TM_CONF_H
