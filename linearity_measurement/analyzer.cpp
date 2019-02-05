#include "analyzer.h"
#include <unistd.h>



Analyzer::Analyzer(QObject *parent) : QObject(parent), ready(false), countQu(0)
{
    m_pTcpSocket = new QTcpSocket(this);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()), Qt::QueuedConnection);
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
    this, SLOT(slotError(QAbstractSocket::SocketError)), Qt::QueuedConnection);
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()), Qt::QueuedConnection);



    _timer = new QTimer( this );
    _timer->setInterval( 1000 );
    connect(_timer, SIGNAL(timeout()), this, SLOT(slot_sock_connect()));
    connect( this, SIGNAL(commit()), &wait_read, SLOT(quit()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), &wait_full, SLOT(quit()));
}

Analyzer::~Analyzer()
{
    _timer->stop();
    m_pTcpSocket->close();
    delete m_pTcpSocket;
    delete _timer;
}

//FUN

QString Analyzer::getSer()
{
    QString ser;
    QStringList l = info.split(',');
    ser = l[2].right(l[2].length()-1);
    return ser;
}

void Analyzer::sock_connect(const QString& ip, int por)
{
    adr = ip;
    port = por;
    slot_sock_connect();
}

bool Analyzer::isReady()
{
    if (ready == true) return true;
            else return false;
}

void Analyzer::setReady(bool x)
{
    ready = x;
}

void Analyzer::setType(bool x)
{
    type = x;
    adr = "127.0.0.1";

    if (type == true)
        port = 5025;
    else port = 5026;
}

void Analyzer::send_command(QString s)
{
    QByteArray arrBlock;
    const char *c_str2;

    arrBlock = s.toLocal8Bit();
    c_str2 = arrBlock.data();

    m_pTcpSocket->write(c_str2);
}

void Analyzer::set_anInf(analyzerInfo confg)
{
    anInf = confg;
}

void Analyzer::refresh_analyzator()
{
    QStringList commands;

    /**    настройка   **/
    if (type)
    {
        commands << "INIT:CONT ON\n"
                 << "CALC:PAR:COUN 4\n"
                 << "CALC:PAR1:DEF A\n"
                 << "CALC:PAR2:DEF B\n"
                 << "CALC:PAR3:DEF R1\n"
                 << "CALC:PAR4:DEF R2\n"
                 << "CALC:PAR1:SPOR "+anInf.gen.port+"\n"
                 << "CALC:PAR2:SPOR "+anInf.gen.port+"\n"
                 << "CALC:PAR3:SPOR "+anInf.gen.port+"\n"
                 << "CALC:PAR4:SPOR "+anInf.gen.port+"\n"
                 << "SENS:ROSC:SOUR INT\n" /** точно ли? **/
                 << "SENS:SWE:TYPE LIN\n"
                 << "TRIG:SOUR BUS\n"
                 << "SENS:FREQ:CENT "+anInf.gen.freqHz+"\n"
                 << "SENS:FREQ:SPAN 0\n"
                 << "SENS:SWE:POIN "+anInf.gen.points+"\n"
                 << "SENS:BWID "+anInf.gen.ifbwHz+"\n";
    }
    else
    {
        if (anInf.rcv.count_ports == "2")
        {
            commands << "INIT:CONT ON\n"
                     << "SERV:REC:CORR:STAT OFF\n" /** что это? **/
                     << "SERV:REC:LIN:STAT ON\n" /** что это? **/
                     << "CALC:PAR:COUN 4\n"
                     << "CALC:PAR1:DEF A\n"
                     << "CALC:PAR2:DEF B\n"
                     << "CALC:PAR3:DEF R1\n"
                     << "CALC:PAR4:DEF R2\n"
                     << "CALC:PAR1:SPOR "+anInf.rcv.port+"\n"
                     << "CALC:PAR2:SPOR "+anInf.rcv.port+"\n"
                     << "CALC:PAR3:SPOR "+anInf.rcv.port+"\n"
                     << "CALC:PAR4:SPOR "+anInf.rcv.port+"\n"
                     << "SENS:ROSC:SOUR EXP\n"
                     << "SENS:SWE:TYPE LIN\n"
                     << "TRIG:SOUR BUS\n"
                     << "SENS:FREQ:CENT "+anInf.rcv.freqHz+"\n"
                     << "SENS:FREQ:SPAN 0\n"
                     << "SENS:SWE:POIN "+anInf.rcv.points+"\n"
                     << "SENS:BWID "+anInf.rcv.ifbwHz+"\n"
                     << "SOUR:POW 0\n";
        }
        else
        {
            commands << "INIT:CONT ON\n"
                     << "SERV:REC:CORR:STAT OFF\n"
                     << "SENS:CORR:REC OFF\n" /** что это? **/
                     << "SERV:REC:LIN:STAT ON\n" /** что это? **/
                     << "CALC:PAR:COUN 8\n"
                     << "CALC:PAR1:DEF A\n"
                     << "CALC:PAR2:DEF B\n"
                     << "CALC:PAR3:DEF R1\n"
                     << "CALC:PAR4:DEF R2\n"
                     << "CALC:PAR5:DEF C\n"
                     << "CALC:PAR6:DEF D\n"
                     << "CALC:PAR7:DEF R3\n"
                     << "CALC:PAR8:DEF R4\n"
                     << "CALC:PAR1:SPOR 1\n"  /** почему 1? **/
                     << "CALC:PAR2:SPOR 1\n"
                     << "CALC:PAR3:SPOR 1\n"
                     << "CALC:PAR4:SPOR 1\n"
                     << "CALC:PAR5:SPOR 1\n"
                     << "CALC:PAR6:SPOR 1\n"
                     << "CALC:PAR7:SPOR 1\n"
                     << "CALC:PAR8:SPOR 1\n"
                     << "SENS:ROSC:SOUR EXP\n" /** точно ли? **/
                     << "SENS:SWE:TYPE LIN\n"
                     << "TRIG:SOUR BUS\n"
                     << "SENS:FREQ:CENT "+anInf.rcv.freqHz+"\n"
                     << "SENS:FREQ:SPAN 0\n"
                     << "SENS:SWE:POIN "+anInf.rcv.points+"\n"
                     << "SENS:BWID "+anInf.rcv.ifbwHz+"\n"
                     << "SOUR:POW 0\n";
        }

    }

    command = 0;
    for (int i = 0; i < commands.count(); ++i)
        send_command(commands[i]);
}

QString Analyzer::getInfo()
{
    QString ans;
    ans = info.left(info.length()-1);
    return ans;
}

QString Analyzer::send_qcommand(QString s)
{
    if (!wait_read.isRunning())
    {
        QString result;
        command = 1;

        int this_count;
        countQu++;
        this_count = countQu;

        send_command(s);

        if (ListAns.length() == 0)
            wait_read.exec();

        countQu--;
        result = ListAns[this_count - 1];

        if (countQu == 0) ListAns.clear();

        return result;
    }

    return "waiting";
}

bool Analyzer::getReady()
{
    if (ready == true) return true;
            else return false;
}

//SLOTS

void Analyzer::slot_sock_connect()
{
        if (m_pTcpSocket->isOpen())
        {
            m_pTcpSocket->abort();
        }

        m_pTcpSocket->connectToHost(adr, port);

        command = 0;
        Qu = "*IDN?\n";
        send_command(Qu);
}

void Analyzer::slotConnected()
{
    _timer->stop();
    qDebug() << this << "connected";
    ready = true;
}

void Analyzer::slotReadyRead()
{
    Ans += m_pTcpSocket->readAll();

    switch (command) {
    case 0:
    {
        info = Ans;
        emit sigConnected(type, info);
        Ans = "";
        break;
    }
    case 1:
    {
        if (Ans[Ans.length()-1] != '\n')
        {
            wait_full.exec();
        }
        else
        {
            if (Ans.indexOf('\n') != Ans.length()-1)
            {
                QStringList ans = Ans.split('\n');
                foreach (QString val, ans) {
                    ListAns.append(val);
                }
            }
            else
            {
                ListAns.append(Ans);
            }
            emit commit();
            Ans = "";
        }
        break;
    }
    default:
        break;
    }
}

void Analyzer::slotError(QAbstractSocket::SocketError)
{
    QString strError =
    "Error: " + ( QAbstractSocket::HostNotFoundError ?
    "The host was not found." :
     QAbstractSocket::RemoteHostClosedError ?
    "The remote host is closed." :
     QAbstractSocket::ConnectionRefusedError ?
    "The connection was refused." :
    QString(m_pTcpSocket->errorString())
    );

    if (type == true)
        qDebug() << "GEN_" << strError;
    else
        qDebug() << "EXP_" << strError;

    ready = false;
    emit sigDisconnect(type);
    _timer->start();
}
