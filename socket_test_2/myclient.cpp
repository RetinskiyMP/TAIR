#include "myclient.h"
#include <QMessageBox>
#include <QDebug>

myclient::myclient(const QString & strHost, int nPort, QObject *parent) : QObject(parent), m_nNextBlockSize(0)
{
    m_pTcpSocket = new QTcpSocket(this);

    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
    this, SLOT(slotError(QAbstractSocket::SocketError)));

}

/*QTcpSocket myclient::get_socket()
{
    return this->m_pTcpSocket;
}*/

void myclient::slotConnected()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_3);
    qDebug() << "connected";

}

void myclient::slotReadyRead()
{

}

void myclient::slotSendToServer()
{

}

void myclient::slotError(QAbstractSocket::SocketError)
{

}

