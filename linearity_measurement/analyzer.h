#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QEventLoop>

#include "data.h"

class Analyzer : public QObject
{
    Q_OBJECT
public:
    explicit Analyzer(QObject *parent = 0);
    ~Analyzer();

    void sock_connect(const QString &, int);
    void send_command(QString);
    QString send_qcommand(QString);

    bool isReady();
    void setReady(bool);
    void setType(bool);
    bool getReady();

    void set_anInf(analyzerInfo);
    void refresh_analyzator();
    QString getInfo();
    QString getSer();

signals:
    void sigReady();
    void sigConnected(bool, QString);
    void sigDisconnect(bool);
    void sigReadyInfo(QString);
    void commit();

public slots:
    void slotReadyRead();
    void slotError (QAbstractSocket::SocketError);
    void slotConnected ();
    void slot_sock_connect();

private:
    QTcpSocket       *m_pTcpSocket;
    QString          adr;
    int              port;

    bool             ready;
    bool             type;//1-generator, 0-device
    int              command;

    QString          info;
    QString          Qu;
    QString          Ans;

    int              countQu;
    QStringList      ListAns;
    QEventLoop       wait_read;
    QEventLoop       wait_full;

    QTimer           *_timer;

    analyzerInfo     anInf;
};

#endif // ANALYZER_H
