#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QVector>
#include <ActiveQt/ActiveQt>
#include <QtCharts/QtCharts>

#include <analyzer.h>
#include "bd.h"
#include "data.h"
#include "channel.h"
#include "memory_list.h"

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(QObject *parent = 0);

    ~Handler();
    static QVector<std::complex<double>> strToComplexArray( const QString &str );
    template <typename T>
    static QString vectorToString(const QVector<T> &vector)
    {
        QString result = "";
        for( int i = 0; i < vector.size() - 1; ++ i )
            result += QString("%1,").arg( vector.at( i ) );
        result += QString("%1").arg( vector.last() );
        return result;
    }

    void bd_show();
    void mm_show();
    void first_config();
    bool check_status();
    void save(QString);
    void mmShow(QString);
    void do_operation(QString, int);
    void update_graph();
    void set_default_data();
    void getFullNames(bool);

signals:
    void start_config(QString);
    void d_connected(bool, QString);
    void d_disconnected(bool);
    void error_model();
    void error_list(QString);
    void error_mm();
    void setXrange(QString,QString);
    void setProgress(int);
    void setPoint(int,double,double);
    void d_yes();
    void ready_names(QStringList);
    void setPointList(int,QList<QPointF>);
    void delete_memory(QString);
    void delete_series(int,int);
    void stop();

    void addSeriaMM(QLineSeries*);
    void deleteSeriesMM();

public slots:
    void slot_newAdr(QString,int,QString,int);
    void slot_get_config(bool, QString);
    void slot_start();
    void slot_pause();
    void slot_stop();
    void slot_delete_from_mm(QStringList);

     void upload_coeff(QStringList);

private:
    QVector<QVector<double>> calcAVG(QStringList);
    void calcCoefficients( const QVector<complex<double>> &srcGenR,
                                  const QVector< QVector<complex<double>> > &srcAll,
                                  QVector< QVector<double> > *all );

    QVector<QVector<double>> dataFromMMtoThis(QStringList);
    QVector<QVector<double>> currentIndex;
    QVector<QVector<double>> _data;
    QVector<QVector<complex<double>>> _dataComplex; //[номер трасы][значение Y]

    QVector<Analyzer *>      Analyzers; //id 0 - gen, id 1 - exp;
    BD                       *bd;
    channel                  *channel_;
    memory_list              *mm;

    bool                     _start;
    bool                     _pause;
    powerSweep               pw;
    analyzerInfo             anInf;
};

#endif // HANDLER_H
