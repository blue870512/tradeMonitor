#include "tm_conf.h"
#include <QSettings>
#include <QFile>
#include <QDebug>

tm_conf::tm_conf()
{
    n_interval = 10;

    is_udp_send = true;
    udp_server_ip = "127.0.0.1";
    udp_server_port = 2001;
}


tm_conf::~tm_conf()
{

}

void tm_conf::init()
{
    get_ini();
    get_sql();
}

void tm_conf::get_ini()
{
    int i=0;
    QString strhost;
    QString strdbName;
    QString strusername;
    QString strpassword;
    QString strport;

    QSettings settings("tradeMonitor.ini", QSettings::IniFormat);

    lst_db.clear();

    while(i<100)
    {
        db_handle db;

        strhost = "DB" + QString::number(i) + "/host";
        strdbName = "DB" + QString::number(i) + "/dbName";
        strusername = "DB" + QString::number(i) + "/username";
        strpassword = "DB" + QString::number(i) + "/password";
        strport = "DB" + QString::number(i) + "/port";

        if(settings.contains(strhost))
        {
            db.set_host(settings.value(strhost).toString());
        }
        else
        {
            break;
        }
        if(settings.contains(strdbName))
        {
            db.set_dbName(settings.value(strdbName).toString());
        }
        else
        {
            db.set_dbName();
        }
        if(settings.contains(strusername))
        {
            db.set_username(settings.value(strusername).toString());
        }
        else
        {
            db.set_username();
        }
        if(settings.contains(strpassword))
        {
            QString tmp = settings.value(strpassword).toString();
            QString passwd;
            decode(tmp, passwd);
            db.set_password(passwd);
            //db.set_password(settings.value(strpassword).toString());
        }
        else
        {
            db.set_password();
        }
        if(settings.contains(strport))
        {
            db.set_port(settings.value(strport).toInt());
        }
        else
        {
            db.set_port();
        }

        db.set_index(i);

        lst_db.append(db);
        i++;
    }


    if(settings.contains("common/interval"))
    {
        n_interval = settings.value("common/interval").toInt();
    }
    else
    {
        n_interval = 5;
    }

    if(settings.contains("common/sqlfile"))
    {
        str_sql_file = settings.value("common/sqlfile").toString();
    }
    else
    {
        str_sql_file = "";
    }

    if(settings.contains("common/open"))
    {
        str_open = settings.value("common/open").toString();
    }
    else
    {
        str_open = "090000";
    }

    if(settings.contains("common/close"))
    {
        str_close = settings.value("common/close").toString();
    }
    else
    {
        str_close = "160000";
    }

    if(settings.contains("common/done"))
    {
        str_done = settings.value("common/done").toString();
    }
    else
    {
        str_done = "220000";
    }

    if(str_open.size() != 6)
    {
        str_open = "090000";
    }
    if(str_close.size() != 6)
    {
        str_close = "160000";
    }
    if(str_done.size() != 6)
    {
        str_done = "220000";
    }

    if(settings.contains("common/send"))
    {
        QString tmp = settings.value("common/send").toString();
        if(tmp == "true")
        {
            is_udp_send = true;
        }
        else
        {
            is_udp_send = false;
        }
    }
    else
    {
        is_udp_send = false;
    }

    if(settings.contains("common/serverip"))
    {
        udp_server_ip = settings.value("common/serverip").toString();
    }
    else
    {
        udp_server_ip = "";
    }

    if(settings.contains("common/serverport"))
    {
        udp_server_port = settings.value("common/serverport").toInt();
    }
    else
    {
        udp_server_port = -1;
    }
}

void tm_conf::get_sql()
{
    lst_opening_sql.clear();
    lst_closing_sql.clear();

    if(str_sql_file == "")
    {
        return;
    }

    QFile file(str_sql_file);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QDomDocument dom("WCM");
    QDomElement docElem;

    if (!dom.setContent(&file))
    {
        file.close();
        return;
    }

    docElem = dom.documentElement();
    listDom(docElem);

    file.close();
}

void tm_conf::listDom(QDomElement& docElem)
{
    QDomNode node = docElem.firstChild();
    QString s1, s2;
    int i;

    //if(node.toElement().isNull())
    while(!node.isNull())
    {
        if(node.toElement().tagName() == "sql")
        {
            sql_info sql;
            QMap<int, QString> key_index;
            QDomNode child = node.firstChild();
            while(!child.isNull())
            {
                if(child.toElement().tagName() == "note")
                {
                    sql.intr = child.toElement().text();
                }
                else if(child.toElement().tagName() == "type")
                {
                    sql.type = child.toElement().text().toInt();
                }
                else if(child.toElement().tagName() == "value")
                {
                    s1 = child.toElement().text();
                    xml_decode(s1, s2);
                    sql.sql = s2;
                }
                else if(child.toElement().tagName() == "keyindex")
                {
                    s1 = child.toElement().text();
                    QStringList tmp_list = s1.split(',');
                    for(i=0; i<tmp_list.size(); i++)
                    {
                        QStringList tmp_list2 = tmp_list[i].split(':');
                        if(tmp_list2.size() == 1 && is_digit(tmp_list2[0]))
                        {
                            key_index[tmp_list2[0].toInt()] = "";
                        }
                        else if(tmp_list2.size() == 2 && is_digit(tmp_list2[1]))
                        {
                            key_index[tmp_list2[1].toInt()] = tmp_list2[0];
                        }
                    }
                }
                child = child.nextSibling();
            }
            if(sql.type == TM_OPENING)
            {
                lst_opening_sql.append(sql);
                lst_opening_key_index.append(key_index);
            }
            else if(sql.type == TM_CLOSING)
            {
                lst_closing_sql.append(sql);
            }
            node = node.nextSibling();
        }
    }
}

bool tm_conf::is_digit(QString s)
{
    if(QString::number(s.toInt()) == s)
    {
        return true;
    }

    return false;
}

/*
void tm_conf::get_sql()
{
    sql_info sql;
    lst_sql.clear();
    //lst_warn_count.clear();

    if(str_sql_file == "")
    {
        return;
    }

    QFile file(str_sql_file);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);
    QString line;
    QString tmp;

    tmp = "";
    sql.sql = "";
    sql.intr = "";
    sql.type = -1;

    while(!out.atEnd())
    {
        line = out.readLine();
        line = line.simplified();

        if(line == "")
        {
            tmp = tmp.simplified();
            sql.sql = tmp;
            if(sql.sql != "" && sql.type != -1)
            {
                lst_sql.append(sql);
                //lst_warn_count.append(0);

                tmp = "";
                sql.sql = "";
                sql.intr = "";
                sql.type = -1;
            }
            else if(sql.sql != "")
            {
                tmp = "";
                sql.sql = "";
            }
        }
        else if(line.size()<2)
        {
            tmp += line;
            tmp += " ";
        }
        else if(line.mid(0,2) == "--")
        {
            sql.intr = line.mid(2);
        }
        else if(line.mid(0,5) == "type=")
        {
            sql.type = line.mid(5).toInt();
        }
        else if(line.mid(0,6) == "type =")
        {
            sql.type = line.mid(6).toInt();
        }
        else if(line.mid(0,4) == "sql=")
        {
            tmp += line.mid(4);
            tmp += " ";
        }
        else if(line.mid(0,5) == "sql =")
        {
            tmp += line.mid(5);
            tmp += " ";
        }
        else
        {
            tmp += line;
            tmp += " ";
        }
    }

    tmp = tmp.simplified();
    if(tmp != "")
    {
        sql.sql = tmp;
        if(sql.sql != "" && sql.type != -1)
        {
            lst_sql.append(sql);
            //lst_warn_count.append(0);

            tmp = "";
            sql.sql = "";
            sql.intr = "";
            sql.type = -1;
        }
    }

    file.close();
}
*/
