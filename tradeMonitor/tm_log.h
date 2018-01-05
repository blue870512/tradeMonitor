#ifndef TM_LOG_H
#define TM_LOG_H

#include <QObject>
#include <QMutex>

class tm_log : public QObject
{
    Q_OBJECT
public:
    explicit tm_log(QObject *parent = 0);
    void write_log(const QString &log = "");
    void write_log(const int &i);

signals:

public slots:

private:
    QMutex lock;
};

#endif // TM_LOG_H
