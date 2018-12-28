#include "threat.h"
#include <QString>

Threat::~Threat()
{
    delete[] X;
    m_pTcpSocket->close();
    delete m_pTcpSocket;
}

Threat::Threat(const QString &strHost, int nPort, QObject *parent) : QObject(parent), go(0)
{
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
    this, SLOT(slotError(QAbstractSocket::SocketError))
    );

    sq = "SENSe1:SWEep:POINts?;:SENSe1:FREQuency:DATA?\n";
    send_command(sq);

    commands = 3;
}


//СЛОТЫ ПОД СИГНАЛЫ СОКЕТА
void Threat::slotConnected()
{
    qDebug() << "Поток обработчика: " << this->thread();
    emit Ready();
}

void Threat::slotReadyRead()
{
    sp = "";

    while(m_pTcpSocket->bytesAvailable())
        {
            sp += m_pTcpSocket->readAll();
        }

    switch (commands) {
    case 1:
        MaxAndMin();
        break;
    case 2:
    {
        if (go == false) SetPointsOfGraph();
        else SetPointsOfGraphCicle();
    }
        break;
    case 3:
        KolPointsAndXpoints();
        break;
    default:
        break;
    }
}

void Threat::slotError(QAbstractSocket::SocketError)
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

    qDebug() << strError;
}

//УСТАНОВКА НОВЫХ ПАРАМЕТРОВ ГРАФИКА
void Threat::slotSetConfigOfGraph(qint64 min, qint64 max, qint64 pch)
{
    if (min != 0) sq = "SENSe1:FREQuency:STARt "+QString::number(min)+"\n";
    if (max != 0) sq = "SENSe1:FREQuency:STOP "+QString::number(max)+"\n";
    if (pch != 0) sq = "SENSe1:BANDwidth "+QString::number(pch)+"\n";

    send_command(sq);
}

void Threat::slotSetKolPoint(qint64 k)
{
    sq = "SENSe1:SWEep:POINts "+QString::number(k)+"\n";
    send_command(sq);

    sq = "SENSe1:SWEep:POINts?;:SENSe1:FREQuency:DATA?\n";
    send_command(sq);

    commands = 3;
}

//ЗАПРОС НАЧАЛА И КОНЦА СТИМУЛА
void Threat::slotGetGrahpSettings()
{
    sq = "SENSe1:FREQuency:STARt?\n";
    send_command(sq);

    sq = "SENSe1:FREQuency:STOP?\n";
    send_command(sq);

    commands = 1;
}
//ОТПРАВКА НАЧАЛА И КОНЦА СТИМУЛА ИНТЕРФЕЙСУ
void Threat::MaxAndMin()
{
    QString max = "", min = "";
    qint64 qmax, qmin;

    for (int i = 0; i < 17; ++i)
    {
        min += sp[i];
    }
    for (int i = 18; i < sp.length()-1; ++i)
    {
        max += sp[i];
    }

    qmax = max.toDouble();
    qmin = min.toDouble();

    emit ReadyGrahpSettings(qmin,qmax);
}
//ЗАПРОС ЗНАЧЕНИЙ ОСИ Y ОПРЕДЛЕННОГО ГРАФИКА
void Threat::slotGetPointOfGraph(const QString &num)
{
    sq = "CALCulate1:PARameter"+num+":SELect\n";
    send_command(sq);

    sq = "CALCulate1:FORMat MLOG\n";
    send_command(sq);

    sq = "CALCulate1:TRACe"+num+":DATA:FDATa?\n";
    send_command(sq);


    commands = 2;
}
//ОТПРАВКА ЗНАЧЕНИЙ ОСИ Y ИНТЕРФЕЙСУ
void Threat::SetPointsOfGraph()
{
    QString val = "";
    qint16 num = 1;
    qint64 index = 0;

    double *Y = new double[kol_point];
    QStringList shortList = sp.split(',');
            for (auto shortStr : shortList)
            {
                if (num % 2 != 0)
                {
                    val = shortStr;
                    Y[index] = val.toDouble();
                    num++;
                    index++;
                }
                else
                {
                    num++;
                }
            };

    emit ReadyPointsOfGraph(Y);
}
//ПОЛУЧЕНИЕ ТОЧЕК ОСИ Х И КОЛ-ВО ТОЧЕК
void Threat::KolPointsAndXpoints()
{
    QString v = "";
    int num = 0;;

    for (int i = 0; i < sp.length(); ++i)
    {
        if (sp[i] == ';')
        {
            num = i;
            break;
        }
    }

    for (int i = 0; i < num; ++i)
    {
        v += sp[i];
    }

    kol_point = v.toDouble();

    //массив из координат оси х

    int index = 0;
    v = "";
    X = new double[kol_point];

    for (int i = num+1; i < sp.length()-1; ++i)
    {
        v += sp[i];
    }

    QStringList shortList = v.split(',');
            for (auto shortStr : shortList)
            {
                X[index] = shortStr.toDouble();
                index++;
            }

    emit ReadyKolAndX(kol_point, X);
}
//УСТАНОВЛЕНИЕ КОЛ-ВА ГРАФИКОВ
void Threat::slotAddGraph(const QString &s)
{
    sq = "CALCulate1:PARameter:COUNt "+s+"\n";
    send_command(sq);
}
//УСТАНОВКА ПАРМЕТРОВ
void Threat::slotSetSettings(const QString &a, const QString&b)
{
    sq = "CALCulate1:PARameter"+a+":DEFine "+b+"\n";
    send_command(sq);
}
//УСТАНОВКА ПОРТА
void Threat::slotSetNPort(const QString &a, const QString &b)
{
    sq = "CALCulate1:PARameter"+a+":SPORt "+b+"\n";
    send_command(sq);
}

//ОТПРАВКА КОММАНД
void Threat::send_command(QString q)
{
    QByteArray arrBlock;
    const char *c_str2;

    arrBlock = q.toLocal8Bit();
    c_str2 = arrBlock.data();

    m_pTcpSocket->write(c_str2);
}
//======================================================================================================
//НАЧАЛО ЦИКЛА
void Threat::slotStartThread(qint16 n)
{
    if (n == 0) emit ReadyPointsOfGraphCicle(nullptr,-1);
    else
    {
        go = true;
        slotGetPointOfGraph(QString::number(n));
        grf = n;
    }
}


void Threat::SetPointsOfGraphCicle()
{
    QString val = "";
    qint16 num = 1;
    qint64 index = 0;

    double *Y = new double[kol_point];
    QStringList shortList = sp.split(',');
            for (auto shortStr : shortList)
            {
                if (num % 2 != 0)
                {
                    val = shortStr;
                    Y[index] = val.toDouble();
                    num++;
                    index++;
                }
                else
                {
                    num++;
                }
            };

    emit ReadyPointsOfGraphCicle(Y,grf-1);
}

void Threat::slotStopThread()
{
    go = false;
}


