#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QtNetwork>

#define UUID_LEN 16
#define FRAGMENT_LEN 512

class udp_client : public QObject
{
    Q_OBJECT
public:
    explicit udp_client(QObject *parent = 0);
    udp_client(QString &addr, int p);
    ~udp_client();

    void set_server_address(QString &addr);
    void set_server_port(int p);

signals:

public slots:
    void send_data(QByteArray &data);

private:
    QUdpSocket *client;
    QHostAddress server;
    int port;

    void send_data_by_fragment(QByteArray &data);
    void generate_uuid(char * uuid, int len);
};

#endif // UDP_CLIENT_H
