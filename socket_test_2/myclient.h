#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QTcpSocket>

class myclient : public QObject
{
    Q_OBJECT
public:
    explicit myclient(const QString &strHost, int nPort, QObject *parent = 0);



signals:

public slots:
    void slotReadyRead ( );
    void slotError (QAbstractSocket::SocketError);
    void slotSendToServer( );
    void slotConnected ( );

private:
    QTcpSocket* m_pTcpSocket;
    quint16 m_nNextBlockSize;
};

#endif // MYCLIENT_H
