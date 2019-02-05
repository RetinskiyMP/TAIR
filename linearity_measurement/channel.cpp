#include "channel.h"
#include <QDebug>

channel::channel(QObject *parent) : QObject(parent)
{

}

complex<double> channel::average(const QVector<complex<double> > &vec)
{
    Q_ASSERT( vec.size() > 0 );
    complex<double> summ = complex<double>(0.0, 0.0);
    for( int i = 0 ; i < vec.size(); ++ i )
        summ += vec[ i ];
    return summ / double( vec.size() );
}

QVector<QVector<double>> channel::toData(QVector<QVector<complex<double>>> mass)
{
    QVector<QVector<double>> result;
    QVector<double> trace;
    complex<double> gen0, gen;
    complex<double> rcv0, rcv;

    for (int k = 0; k < mass[0].count(); k++){
        trace.clear();
        gen0 = mass[0][0];
        gen = mass[0][k];

        for (int i = 0; i < 4; ++i)
        {
            switch (i){
            case 0:  {
                rcv0 = mass[1][0];
                rcv  = mass[1][k];} break;
            case 1:  {
                rcv0 = mass[2][0];
                rcv  = mass[2][k]; } break;
            case 2: {
                rcv0 = mass[3][0];
                rcv  = mass[3][k]; } break;
            case 3: {
                rcv0 = mass[4][0];
                rcv  = mass[4][k]; } break;
            default: qWarning() << "-";
            }
            complex<double> re = (gen / gen0) / (rcv / rcv0);
            trace.append( 20.0 * log10( abs( re ) ));
        }

        if(mass.count() > 5)
        {
            for (int i = 0; i < 4; ++i)
            {
                switch (i){
                case 0:  {
                    rcv0 = mass[5][0];
                    rcv  = mass[5][k];} break;
                case 1:  {
                    rcv0 = mass[6][0];
                    rcv  = mass[6][k]; } break;
                case 2: {
                    rcv0 = mass[7][0];
                    rcv  = mass[7][k]; } break;
                case 3: {
                    rcv0 = mass[8][0];
                    rcv  = mass[8][k]; } break;
                default: qWarning() << "-";
                }
                complex<double> re = (gen / gen0) / (rcv / rcv0);
                trace.append( 20.0 * log10( abs( re ) ));
            }
        }
        result.append(trace);
        if (k == 1) result[0] = result[1];
    }
    return result;
}

QVector<QVector<complex<double>>> channel::getDataComplex()
{
    _resultsComplex.clear();
    _resultsComplex.append(_genR);
    _resultsComplex.append(_recA);
    _resultsComplex.append(_recB);
    _resultsComplex.append(_recR1);
    _resultsComplex.append(_recR2);

    if (chech_count_activ_port(0)){
    _resultsComplex.append(_recC);
    _resultsComplex.append(_recD);
    _resultsComplex.append(_recR3);
    _resultsComplex.append(_recR4);}

    return _resultsComplex;
}

QVector<double> channel::updateData(const OneData &data)
{
    int index = data.indexPoint;

    if (index == 0)
        _results.clear();

    _currentIndex = index;
    _data[index] = data;

    calculation(index);
    QVector<double> result;
    calculation_phase(index,result);
    _results.append(result);

    if (index==1) _results[0] = _results[1];

    return result;
}

void channel::calculation_phase(int index, QVector<double> &result)
{ 
//    if( index == 0 )
//    {
//        result.append( 20.0 * log10(abs( _genR[0])));
//        return;
//    }

    complex<double> gen0, gen;
    complex<double> rcv0, rcv;

    gen0 = _genR[0];
    gen = _genR[index];

    for (int i = 0; i < 4; ++i)
    {
        switch (i){
        case 0:  {
            rcv0 = _recA[0];
            rcv  = _recA[index];} break;
        case 1:  {
            rcv0 = _recB[0];
            rcv  = _recB[index]; } break;
        case 2: {
            rcv0 = _recR1[0];
            rcv  = _recR1[index]; } break;
        case 3: {
            rcv0 = _recR2[0];
            rcv  = _recR2[index]; } break;
        default: qWarning() << "-";
        }
        complex<double> re = (gen / gen0) / (rcv / rcv0);
        result.append( 20.0 * log10( abs( re ) ));
    }

    if(chech_count_activ_port(index))
    {
        for (int i = 0; i < 4; ++i)
        {
            switch (i){
            case 0:  {
                rcv0 = _recC[0];
                rcv  = _recC[index];} break;
            case 1:  {
                rcv0 = _recD[0];
                rcv  = _recD[index]; } break;
            case 2: {
                rcv0 = _recR3[0];
                rcv  = _recR3[index]; } break;
            case 3: {
                rcv0 = _recR4[0];
                rcv  = _recR4[index]; } break;
            default: qWarning() << "-";
            }
            complex<double> re = (gen / gen0) / (rcv / rcv0);
            result.append( 20.0 * log10( abs( re ) ));
        }
    }

}

void channel::calculation(int index)
{
    _genR[index] = average( _data[ index ].genR );

    _recA [index] = average( _data[ index ].rcv.A );
    _recB [index] = average( _data[ index ].rcv.B );
    _recR1[index] = average( _data[ index ].rcv.R1 );
    _recR2[index] = average( _data[ index ].rcv.R2 );

    if(chech_count_activ_port(index))
    {
        _recC [index] = average( _data[ index ].rcv.C );
        _recD [index] = average( _data[ index ].rcv.D );
        _recR3[index] = average( _data[ index ].rcv.R3 );
        _recR4[index] = average( _data[ index ].rcv.R4 );
    }
}

void channel::setCountP(int count)
{
    _data.resize(count);

    _genR.resize(count);

    _recA.resize (count);
    _recB.resize (count);
    _recR1.resize(count);
    _recR2.resize(count);

    _recC.resize (count);
    _recD.resize (count);
    _recR3.resize(count);
    _recR4.resize(count);
}

bool channel::chech_count_activ_port(int index)
{
    if(_data[index].rcv.C.count() != 0 &&
            _data[index].rcv.D.count() != 0 &&
            _data[index].rcv.R3.count() != 0 &&
            _data[index].rcv.R4.count() != 0) return true;
    else return false;
}



