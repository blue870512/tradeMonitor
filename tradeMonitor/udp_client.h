#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QtNetwork>

class udp_client : public QObject
{
    Q_OBJECT
public:
    explicit udp_client(QObject *parent = 0);
    udp_client(QString &addr, int p);
    ~udp_client();

    void set_server_address(QString &addr);
    void set_server_port(int p);

    void send_data(QByteArray &data);

signals:

public slots:

private:
    QUdpSocket *client;
    QHostAddress server;
    int port;
};

#endif // UDP_CLIENT_H
