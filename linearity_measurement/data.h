#ifndef DATA_H
#define DATA_H

#include <QVector>
#include <complex>
#include <QString>

struct analyzerInfo
{
    struct {
    QString          port;
    QString          freqHz;
    QString          points;
    QString          ifbwHz;

    QString          delay; //rcv
    QString          count_ports; //rcv
    } gen, rcv;
};

struct powerSweep
{
    double start() const { return _start; }
    double stop()  const { return _stop; }
    double count() const { return _count; }
    void  set( QString start, QString stop, QString count  )
    {
        _start = start.toDouble();
        _stop  = stop.toDouble();
        _count = count.toDouble();
        computePowerList();
    }
    QVector<double> powerList() const { return _powerList; }
private:
    void computePowerList() {
        _powerList.resize( _count );
        size_t i = 0;
        double step = (_start - _stop) / double(_count-1);
        for( double pw = _start; pw >= _stop; pw -= step )
            _powerList[ i++ ] = pw;

        while( i < _count )
            _powerList[ i ++ ] = _stop;
    }
    double _start;
    double _stop;
    size_t _count;
    QVector<double> _powerList;
};

struct OneData {
    size_t   indexPoint;
    double   referencePower;

    QVector<std::complex<double>> genR;

    struct Meas {
        QVector<std::complex<double>> A;
        QVector<std::complex<double>> B;
        QVector<std::complex<double>> R1;
        QVector<std::complex<double>> R2;

        // for 4 ports
        QVector<std::complex<double>> C;
        QVector<std::complex<double>> D;
        QVector<std::complex<double>> R3;
        QVector<std::complex<double>> R4;
        std::pair<double,double> minmax() const
        {
            if( R1.isEmpty() )
                return std::make_pair(0, 0);

            double min = 20.0 * log10( abs( R1.first() ) );
            double max = min;
            for( int i = 1 ; i < R1.size(); ++i )
            {
                double r = 20.0 * log10( abs( R1.at( i ) ) );
                if( min > r ) min = r;
                if( max < r ) max = r;
            }

            return std::make_pair(min, max);
        }
    } rcv; // receiver;
};

#endif // DATA_H
