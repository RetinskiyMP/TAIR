#define _INT 0
#define _EXP 1

#include <QApplication>
#include <QTime>
#include <QDate>
#include "handler.h"

Handler::Handler(QObject *parent) : QObject(parent)
{
    mm = new memory_list();
    bd = new BD();
    connect(this,SIGNAL(delete_memory(QString)),bd,SLOT(slot_delete_memory(QString)));
    connect(mm,SIGNAL(ready_to_upload(QStringList)),this,SLOT(upload_coeff(QStringList)));
    connect(mm,SIGNAL(delete_from_mm(QStringList)),this,SLOT(slot_delete_from_mm(QStringList)));
}

void Handler::slot_delete_from_mm(QStringList list)
{
    if (!list.isEmpty()){
        for (int i = 0; i < list.count(); ++i)
        {
            emit delete_memory(list[i]);
        }
    }

    thread()->sleep(1);
    QStringList names = bd->getMMnames();
    emit ready_names(names);
}

void Handler::mmShow(QString id)
{
    if (!id.isEmpty()){
        QVector<QVector<complex<double>>> _dataComplexInMM;

        QStringList all = bd->getDataFromMM(id);
        QVector<QStringList> _all;
        QVector<double> powers;

        for (int i = 0; i < all.count(); ++i)
        {
            QString need_split = all[i];
            QStringList data = need_split.split(',');
            _all.append(data);
        }

        QString count = _all[5][0]; //колво точек
        int _count = count.toInt();
        //опр x
        for (int i = 0; i < _count; ++i){
            powers.append(_all[6][i].toDouble());
        }

        //опр компл значения
        for (int i = 7; i < all.count(); ++i){
            QString tmp = all[i];
            if(!tmp.isEmpty()){
                QVector<complex<double>> stb = strToComplexArray(all[i]);
                _dataComplexInMM.append(stb);}
        }

        QVector<QVector<double>> _dataInMM = channel::toData(_dataComplexInMM);


        emit deleteSeriesMM();

        for (int i = 0; i < _dataInMM[i].count(); ++i){
            QLineSeries * s = new QLineSeries();
            for (int j = 0; j < _dataInMM.count(); ++j){
                s->append(powers[j],_dataInMM[j][i]);
            }
            emit addSeriaMM(s);
        }
    }
}

void Handler::save(QString name)
{
    if (!name.isEmpty()) {
        QTime time;
        QDate date;

        QStringList result;

        for (int i = 0; i < 16; ++i){
            QString str;
            result.append(str);
        }

        result[0] = name;

        QStringList a = Analyzers[_EXP]->getInfo().split(',');
        QString _model, _ser;
        _model = a[1];
        _ser = a[2];

        _model = _model.right(_model.length()-1);
        _ser = _ser.right(_ser.length()-1);

        result[1] = _model;
        result[2] = _ser;
        result[3] = time.currentTime().toString("hh:mm:ss");
        result[4] = date.currentDate().toString("dd.MM.yy");
        result[5] = QString::number(pw.count());

        foreach (double var, pw.powerList()) {
            result[6]+=QString::number(var)+",";
        }


        for (int i = 0; i < _dataComplex.count(); ++i){
            for (int j = 0; j < _dataComplex[i].count(); j++){
                result[i+7]+=QString::number(_dataComplex[i][j].real())+","+QString::number(_dataComplex[i][j].imag())+",";
            }
        }

        for (int j = 6; j < 16; ++j)
        {
            QString str = result[j];
            result[j] = str.left(str.length()-1);
        }

        bd->addMM(result);
        emit error_list("Успешно сохранено.");
    }
}


void Handler::do_operation(QString id, int index)
{
    //получить данные из бд об этой модельке
    //определить операцию
    //в currentIndex записать измененные
    if (!_start && !id.isEmpty()){
        QStringList MM = bd->getDataFromMM(id);
        QVector<QVector<double>> indexesInMM = dataFromMMtoThis(MM);

        if (MM[5] != QString::number(pw.count()) || currentIndex.isEmpty() || _data.isEmpty()) emit error_mm();
        else
        {
            if (indexesInMM[0].count() != currentIndex[0].count()) emit error_mm();
            else
            {
                emit delete_series(0,7);
                QVector<double> powers = pw.powerList();
                switch (index) {
                case 1:
                {
                    for (int i = 0; i < currentIndex.count(); i++)
                        for (int j = 0; j < currentIndex[i].count(); ++j){
                            currentIndex[i][j]=_data[i][j]+indexesInMM[i][j];
                            emit setPoint(j,powers[i],currentIndex[i][j]);
                        }
                }
                    break;
                case 2:
                {
                    for (int i = 0; i < currentIndex.count(); i++)
                        for (int j = 0; j < currentIndex[i].count(); ++j){
                            currentIndex[i][j]=_data[i][j]-indexesInMM[i][j];
                            emit setPoint(j,powers[i],currentIndex[i][j]);
                        }
                }
                    break;
                case 3:
                {
                    for (int i = 0; i < currentIndex.count(); i++)
                        for (int j = 0; j < currentIndex[i].count(); ++j){
                            currentIndex[i][j]=_data[i][j]/indexesInMM[i][j];
                            emit setPoint(j,powers[i],currentIndex[i][j]);
                        }
                }
                    break;
                case 4:
                {
                    for (int i = 0; i < currentIndex.count(); i++)
                        for (int j = 0; j < currentIndex[i].count(); ++j){
                            currentIndex[i][j]=_data[i][j]*indexesInMM[i][j];
                            emit setPoint(j,powers[i],currentIndex[i][j]);
                        }
                }
                    break;
                default:
                    break;
                }
            }
        }
    }
    else emit error_mm();
}

void Handler::set_default_data()
{
    if (!_data.isEmpty())
    {
    currentIndex = _data;
    emit delete_series(0, 7);
    QVector<double> powers = pw.powerList();
    for (int i = 0; i < _data.count(); i++)
        for (int j = 0; j < _data[i].count(); ++j){
            emit setPoint(j,powers[i],_data[i][j]);
        }
    }
}

QVector<QVector<double>> Handler::dataFromMMtoThis(QStringList all)
{
    QVector<QVector<complex<double>>> _dataComplexInMM;
    QVector<QStringList> _all;

    for (int i = 0; i < all.count(); ++i)
    {
        QString need_split = all[i];
        QStringList data = need_split.split(',');
        _all.append(data);
    }

    //опр компл значения
    for (int i = 7; i < all.count(); ++i){
        QString tmp = all[i];
        if(!tmp.isEmpty()){
            QVector<complex<double>> stb = strToComplexArray(all[i]);
            _dataComplexInMM.append(stb);}
    }

    QVector<QVector<double>> _dataInMM = channel::toData(_dataComplexInMM);
    return _dataInMM;
}

void Handler::first_config()
{
    qDebug() << this->thread();
    channel_ = new channel();

    Analyzer *INT = new Analyzer();
    Analyzer *EXP = new Analyzer();
    Analyzers.append(INT);
    Analyzers.append(EXP);

    Analyzers[_INT]->setType(true);
    Analyzers[_EXP]->setType(false);

    connect(this, SIGNAL(destroyed(QObject*)), Analyzers[_EXP], SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed(QObject*)), Analyzers[_INT], SLOT(deleteLater()));

    connect(Analyzers[_INT], SIGNAL(sigConnected(bool,QString)), this, SIGNAL(d_connected(bool,QString)), Qt::QueuedConnection);
    connect(Analyzers[_INT], SIGNAL(sigDisconnect(bool)), this, SIGNAL(d_disconnected(bool)), Qt::QueuedConnection);

    connect(Analyzers[_EXP], SIGNAL(sigConnected(bool,QString)), this, SIGNAL(d_connected(bool,QString)), Qt::QueuedConnection);
    connect(Analyzers[_EXP], SIGNAL(sigDisconnect(bool)), this, SIGNAL(d_disconnected(bool)), Qt::QueuedConnection);
    connect(Analyzers[_EXP], SIGNAL(sigConnected(bool,QString)), this, SLOT(slot_get_config(bool,QString)), Qt::QueuedConnection);
    connect(bd,SIGNAL(d_update()),Analyzers[1],SLOT(slot_sock_connect()));

    QStringList names = bd->getMMnames();
    emit ready_names(names);
}

void Handler::getFullNames(bool l)
{
    QStringList names = bd->getMMnames();

    QStringList list;
    if(l)
    {
        QString info = Analyzers[_EXP]->getInfo();
        QStringList _info = info.split(',');
        QString mod = _info[1].right(_info[1].length()-1);
        QString ser = _info[2].right(_info[2].length()-1);

        foreach (QString s, names) {
            QString tmp;
            tmp = s.left(s.indexOf("]")+1);
            if (tmp == mod+"["+ser+"]") list.append(s);
        }
        emit ready_names(list);
    }
    else
    {
        emit ready_names(names);
    }
}

void Handler::upload_coeff(QStringList list)
{
    if (list.count() != 0 && Analyzers[_EXP]->getReady()){
    QVector<QVector<double>> all = calcAVG(list);

    for( int i = 0 ; i < all.size(); ++ i )
        Analyzers[_EXP]->send_command(QString("SERV:REC%1:LIN:DATA %2\n").arg(i+1).arg(vectorToString(all[i])));

//    int STB = Analyzers[_EXP]->send_qcommand("*STB?\n").trimmed().toInt();

//    if( STB != 0 )
//        emit error_list("Что-то не так.");
//        qDebug() << STB;
//    else
//        qDebug() << STB;
//       emit error_list("Завершено.");
    }
}

QVector<QVector<double>> Handler::calcAVG(QStringList list)
{
    QString ser; //записывает серийный номер первой же записи из памяти и сравнивает с другими
    QString count; //записывает кол-во измерений первой записи и сравнивает с другими

    QVector<QVector<complex<double>>> allinMM; //[столбец][значения] суммарный вектор.

    for (int i = 0; i < list.count(); ++i){
        QVector<QVector<complex<double>>> _dataComplexInMM; //[столбец][значения]

        QStringList all = bd->getDataFromMM(list[i]);
        QVector<QStringList> _all;

        for (int i = 0; i < all.count(); ++i)
        {
            QString need_split = all[i];
            QStringList data = need_split.split(',');
            _all.append(data);
        }

        //сохранение и последующая проверка
        if (i == 0){
            ser = _all[2][0];
            if (ser != Analyzers[_EXP]->getSer()){
                emit error_list("Записи не от этого анализатора");
                break;
            }
            count = _all[5][0];
        }
        else {
            if (_all[2][0] != ser || _all[5][0] != count){
                emit error_list("Не все записи от одного анализатора");
                break;
            }
        }
        //получение данных из бд
        for (int i = 7; i < all.count(); ++i){
            QString tmp = all[i];
            if(!tmp.isEmpty()){
                QVector<complex<double>> stb = strToComplexArray(all[i]);
                _dataComplexInMM.append(stb);}
        }

        if (i == 0) allinMM = _dataComplexInMM;

        //суммирование
        if (list.count() != 1 && i != 0)
        {
            for (int j = 0; j < _dataComplexInMM.count(); ++j){
                for (int k = 0; k < _dataComplexInMM[j].count(); ++k){
                    allinMM[j][k] += _dataComplexInMM[j][k];
                }
            }
        }
        _dataComplexInMM.clear();
    }

    /** рассчет сред **/
    if (list.count() != 1)
    {
        for (int i = 0; i < allinMM.count(); ++i){
            for (int j = 0; j < allinMM[i].count(); ++j){
                allinMM[i][j] /= list.count();
            }
        }
    }

    QVector<QVector<complex<double>>> avgRcvAll;

    for (int i = 1; i < allinMM.count(); ++i)
        avgRcvAll.append(allinMM[i]);

    QVector<QVector<double>> all;
    calcCoefficients(allinMM[0], avgRcvAll, &all);
    return all;

}

void Handler::calcCoefficients( const QVector<complex<double>> &srcGenR,
                              const QVector< QVector<complex<double>> > &srcAll,
                              QVector<QVector<double> > *all )
{
    all->resize( 2 * anInf.rcv.count_ports.toInt() );
    for( int i = 0 ; i < all->size(); ++ i )
        (*all)[ i ].resize(  pw.count()*2 );

    for( size_t i = 0, j = 0; i < pw.count(); ++ i, j += 2 )
    {
        for( int l = 0 ; l < all->size(); ++ l )
            (*all)[ l ][ j ] = ( abs( srcAll[ l ][ i ] ) );

        complex<double> d = srcGenR[ i ] / srcGenR[ 0 ];

        for( int l = 0 ; l < all->size(); ++ l )
            (*all)[ l ][ j+1 ] = ( abs( d / ( srcAll[ l ][ i ] / srcAll[ l ][ 0 ] ) ) );
    }

    for( int l = 0 ; l < all->size(); ++ l )
        (*all)[ l ][ 1 ] = 1.0;
}

void Handler::slot_newAdr(QString adr1, int port1, QString adr2, int port2)
{
    if (Analyzers.count() == 0)
        first_config();

    Analyzers[_INT]->setReady(false);
    Analyzers[_EXP]->setReady(false);

    Analyzers[_INT]->sock_connect(adr1,port1);
    Analyzers[_EXP]->sock_connect(adr2,port2);
}

void Handler::bd_show()
{
    bd->exec();
}

void Handler::mm_show()
{
    QVector<QStringList> result;

    QStringList names = bd->getMMnames();
    for (int i = 0; i < names.count(); ++i)
    {
        QStringList data = bd->getDataFromMM(names[i]);
        result.append(data);
    }
    mm->setInfo(result);
    mm->exec();
}

void Handler::slot_get_config(bool type, QString info)
{
    if (type){};
    QStringList list = info.split(',');
    QString model = list[1];
    model = model.right(model.length()-1);

    if (bd->checkModel(model))
    {
        emit d_yes();
        QStringList config = bd->getConfig(model);
        pw.set(config[0], config[1], config[2]);
        emit setXrange(config[1],config[0]);

        anInf.gen.port = config[3];
        anInf.gen.freqHz = config[4];
        anInf.gen.points = config[5];
        anInf.gen.ifbwHz = config[6];

        anInf.rcv.count_ports = config[7];
        anInf.rcv.port = config[8];
        anInf.rcv.freqHz = config[9];
        anInf.rcv.points = config[10];
        anInf.rcv.ifbwHz = config[11];
        anInf.rcv.delay = config[12];

        Analyzers[_INT]->set_anInf(anInf);
        Analyzers[_EXP]->set_anInf(anInf);
    }
    else
    {
        emit error_model();
    }
}

QVector<std::complex<double> > Handler::strToComplexArray(const QString &str)
{
    QStringList split = str.split(',');

    QVector<std::complex<double>> result( split.size() / 2 );
    for( int i = 0, j = 0; i < split.size(); ++ j, i += 2 )
        result[ j ] = std::complex<double>( split[ i ].toDouble(), split[ i+1 ].toDouble() );
    return result;
}

void Handler::slot_start()
{   
    if (Analyzers[_INT]->getReady() && Analyzers[_EXP]->getReady()){

    _data.clear();
    currentIndex.clear();
    _dataComplex.clear();

    _start = true;
    _pause = false;

    Analyzers[_INT]->refresh_analyzator();
    Analyzers[_EXP]->refresh_analyzator();

    QVector<double> powers = pw.powerList();
    double procent = 100.0/powers.size();
    double progress = 0;
    for( int i = 0 ; i < powers.size(); ++ i )
    {

        double _pw = powers[ i ];

        if( !check_status() )  return;

        Analyzers[_INT]->send_command("SOUR:POW "+QString::number(_pw)+"\n");
        Analyzers[_INT]->send_command("TRIG:SING\n");
        Analyzers[_INT]->send_qcommand("*OPC?\n");

        if( !check_status() ) return;

        QString str1R;
        if(anInf.gen.port == "1")
           str1R = Analyzers[_INT]->send_qcommand("CALC:TRAC3:DATA:SDAT?\n");
        else
           str1R = Analyzers[_INT]->send_qcommand("CALC:TRAC4:DATA:SDAT?\n");

        if( !check_status() ) return;

        Analyzers[_EXP]->send_command("TRIG:SING\n");
        Analyzers[_EXP]->send_qcommand("*OPC?\n");

        QThread::msleep(anInf.rcv.delay.toULong());

        if( !check_status() ) return;

        QString str2A  = Analyzers[_EXP]->send_qcommand("CALC:TRAC1:DATA:SDAT?\n");
        QString str2B  = Analyzers[_EXP]->send_qcommand("CALC:TRAC2:DATA:SDAT?\n");
        QString str2R1 = Analyzers[_EXP]->send_qcommand("CALC:TRAC3:DATA:SDAT?\n");
        QString str2R2 = Analyzers[_EXP]->send_qcommand("CALC:TRAC4:DATA:SDAT?\n");

        OneData data;

        data.indexPoint     = size_t( i );
        data.referencePower = _pw;

        data.genR   = strToComplexArray(str1R );
        data.rcv.A  = strToComplexArray(str2A );
        data.rcv.B  = strToComplexArray(str2B );
        data.rcv.R1 = strToComplexArray(str2R1);
        data.rcv.R2 = strToComplexArray(str2R2);


        if(anInf.rcv.count_ports == "4")
        {
            QString str2C  = Analyzers[_EXP]->send_qcommand("CALC:TRAC5:DATA:SDAT?\n");
            QString str2D  = Analyzers[_EXP]->send_qcommand("CALC:TRAC6:DATA:SDAT?\n");
            QString str2R3 = Analyzers[_EXP]->send_qcommand("CALC:TRAC7:DATA:SDAT?\n");
            QString str2R4 = Analyzers[_EXP]->send_qcommand("CALC:TRAC8:DATA:SDAT?\n");

            data.rcv.C  = strToComplexArray(str2C );
            data.rcv.D  = strToComplexArray(str2D );
            data.rcv.R3 = strToComplexArray(str2R3);
            data.rcv.R4 = strToComplexArray(str2R4);
        }

        channel_->setCountP(pw.count());

        //отправка в GUI
        int id = 0;
        foreach (double val, channel_->updateData(data)) {
            emit setPoint(id,_pw,val);
            id++;
        };

        progress+=procent;
        emit setProgress(progress-1);
    }

    _start = false;
    _pause = false;
    _data = channel_->getData();
    currentIndex = channel_->getData();
    _dataComplex = channel_->getDataComplex();
    emit setProgress(100);
    }
}

void Handler::slot_pause()
{
    if (_pause == false) _pause = true;
            else _pause = false;
}

void Handler::slot_stop()
{
    _pause = false;
    _start = false;
}

bool Handler::check_status()
{
    if (Analyzers[_INT]->getReady() == false || Analyzers[_EXP]->getReady() == false)
    {
        _start = false;
    }

    forever{
        if( _pause )
            QThread::msleep(200);
        else break;
        QApplication::processEvents();
    }

    if (_start) return true;
    else {
        emit stop();
        return false;
    }


    return false;
}

Handler::~Handler()
{
    _start = false;
    _pause = false;

    delete bd;
    delete channel_;
}

