#include "udp_client.h"
#include <stdlib.h>
#include <time.h>

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
        if(data.size() > 2*FRAGMENT_LEN)
        {
            send_data_by_fragment(data);
        }
        else
        {
            client->writeDatagram(data.data(), data.size(), server, port);
        }
    }
}

void udp_client::send_data_by_fragment(QByteArray &data)
{
    int len = data.size();
    const char *d = data.data();
    char tmp[2*FRAGMENT_LEN];
    char uuid[UUID_LEN];
    unsigned short seqno = 0;
    int pos = 0;
    int send_len = 0;

    generate_uuid(uuid, UUID_LEN);

    while(len>0)
    {
        if(len>=FRAGMENT_LEN)
        {
            send_len = FRAGMENT_LEN;
        }
        else
        {
            send_len = len;
        }
        len -= send_len;

        memset(tmp, 0, 2*FRAGMENT_LEN);

        if(seqno == 0)
        {
            tmp[0] = 0x08;
        }
        else if(len == 0)
        {
            tmp[0] = 0x0a;
        }
        else
        {
            tmp[0] = 0x09;
        }

        memcpy(tmp+1, &seqno, 2);
        memcpy(tmp+3, uuid, UUID_LEN);
        memcpy(tmp+3+UUID_LEN, &d[pos], send_len);

        seqno++;
        pos += send_len;

        client->writeDatagram(tmp, send_len+3+UUID_LEN, server, port);
    }
}

void udp_client::generate_uuid(char *uuid, int len)
{
    const char *seed = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+=-";
    int seed_len = strlen(seed);

    srand((unsigned)time(NULL));
    for(int i=0; i<len; i++)
    {
        uuid[i] = seed[rand()%seed_len];
    }
}
