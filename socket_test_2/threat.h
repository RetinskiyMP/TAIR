#ifndef THREAT_H
#define THREAT_H

#include <QObject>
#include <QTcpSocket>


class Threat : public QObject
{
    Q_OBJECT
public:
    ~Threat();
    explicit Threat(const QString &strHost, int nPort, QObject *parent = 0);
    void send_command(QString);

signals:
    void ReadyGrahpSettings(const qint64 &, const qint64 &);
    void ReadyPointsOfGraph(double *);
    void ReadyKolAndX(qint64, double *);
    void Ready();
    void ReadyPointsOfGraphCicle(double *Y, qint16 n);

public slots:
    void slotAddGraph(const QString&);
    void slotSetSettings(const QString&, const QString&);
    void slotSetNPort(const QString&, const QString&);

    void slotReadyRead ();
    void slotError (QAbstractSocket::SocketError);
    void slotConnected ();
    void slotGetGrahpSettings();
    void slotGetPointOfGraph(const QString &);
    void slotSetConfigOfGraph(qint64, qint64, qint64);
    void slotSetKolPoint(qint64);

    void slotStartThread(qint16);
    void slotStopThread();

private:
    bool go;
    qint16 grf;

    void MaxAndMin();
    void SetPointsOfGraphCicle();
    void SetPointsOfGraph();
    void KolPointsAndXpoints();

    qint64 kol_point;
    double *X;

    QTcpSocket* m_pTcpSocket;
    QString sq;
    QString sp;
    qint16 commands;
};

#endif // THREAT_H
