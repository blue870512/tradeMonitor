#include "common.h"
#include <QRegExp>

void encode(QString &s1, QString &s2)
{
    int i, k, m, n;
    char c, l1, l2, tmp[1024];
    tmp[0] = '\0';
    QByteArray ba = s1.toLatin1();
    char *mm = ba.data();
    QString s3;

    i=0;
    while(mm[i])
    {
        c = mm[i];
        k = 5*c - 2;

        m = k/97;
        n = k%97;

        l1 = ((k%15)+33);
        l2 = ((k%69)+58);

        sprintf(tmp, "%s%i%c%i%c", tmp, m, l1, n, l2);
        i++;
    }
    s3 = tmp;
    bitcode(s3, s2);
}

void decode(QString &s1, QString &s2)
{
    int i, j, k, m, n, pos1, pos2;
    char c, l1, l2, tmp1[10], tmp2[10], passwd[32];
    QString s3;

    bitcode(s1, s3);
    QByteArray ba = s3.toLatin1();
    char *mm = ba.data();

    memset(passwd, 0, 32);
    i = 0;
    j = 0;
    while(mm[i])
    {
        pos1 = 0;
        memset(tmp1, 0, 10);
        while(mm[i]>=48 && mm[i]<=57)
        {
            tmp1[pos1] = mm[i];
            pos1++;
            i++;
        }
        l1 = mm[i];
        i++;
        pos2 = 0;
        memset(tmp2, 0, 10);
        while(mm[i]>=48 && mm[i]<=57)
        {
            tmp2[pos2] = mm[i];
            pos2++;
            i++;
        }
        l2=mm[i];
        i++;

        m = atoi(tmp1);
        n = atoi(tmp2);
        k = m*97+n;

        if((((k%15)+33) != l1) || (((k%69)+58) != l2))
        {
            passwd[j] = '\n';
            j++;
        }
        else
        {
            c = ((k+2)/5);
            passwd[j] = c;
            j++;
        }
    }

    s2 = passwd;
}

void bitcode(QString &s1, QString &s2)
{
    int i;
    QByteArray ba = s1.toLatin1();
    char *mm = ba.data();
    char tmp[1024];

    memset(tmp, 0, 1024);

    i = 0;
    while(mm[i])
    {
        tmp[i] = (char) (mm[i]^((i%14)+1));
        i++;
    }

    s2 = tmp;
}

void xml_decode(QString &s1, QString &s2)
{
    s2 = s1.replace(QRegExp("&gt;"), ">");
    s2 = s2.replace(QRegExp("&lt;"), "<");
    s2 = s2.replace(QRegExp("\r"), " ");
}

void xml_encode(QString &s1, QString &s2)
{
    s2 = s1.replace(QRegExp(">"), "&gt;");
    s2 = s2.replace(QRegExp("<"), "&lt;");
}
