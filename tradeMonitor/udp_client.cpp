#include "udp_client.h"

udp_client::udp_client(QObject *parent) : QObject(parent)
{
    client = new QUdpSocket();
    server = QHostAddress("127.0.0.1");
    port = -1;
}

udp_client::udp_client(QString &addr, int p)
{
    client = new QUdpSocket();
    server = QHostAddress(addr);
    port = p;
}

udp_client::~udp_client()
{
    if(client != NULL)
    {
        delete client;
    }
}

void udp_client::set_server_address(QString &addr)
{
    server = QHostAddress(addr);
}

void udp_client::set_server_port(int p)
{
    port = p;
}

void udp_client::send_data(QByteArray &data)
{
    if(port > 0)
    {
        client->writeDatagram(data.data(), data.size(), server, port);
    }
}
