#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include <QVector>
#include <complex>
#include <data.h>

using namespace std;

class channel : public QObject
{
    Q_OBJECT
public:
    explicit channel(QObject *parent = 0);

    QVector<QVector<complex<double>>> getDataComplex();
    QVector<QVector<double>> getData() { return _results; }

    static QVector<QVector<double>> toData(QVector<QVector<complex<double>>>);

    QVector<double> updateData( const OneData &data );
    void setCountP(int);
    static complex<double> average( const QVector<complex<double>> &vec );

signals:

public slots:

private:
    void calculation(int);
    void calculation_phase(int,QVector<double>&);
    bool chech_count_activ_port(int);

    int _currentIndex;
    QVector<QVector<double>> _results; //данные, получ для расчета
    QVector<QVector<complex<double>>> _resultsComplex;
    QVector<OneData> _data;

    QVector<complex<double>> _genR;

    QVector<complex<double>> _recA;
    QVector<complex<double>> _recB;
    QVector<complex<double>> _recR1;
    QVector<complex<double>> _recR2;

    QVector<complex<double>> _recC;
    QVector<complex<double>> _recD;
    QVector<complex<double>> _recR3;
    QVector<complex<double>> _recR4;
};

#endif // CHANNEL_H
