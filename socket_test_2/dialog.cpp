#include "dialog.h"
#include "ui_dialog.h"
#include "threat.h"

#include <QDebug>
#include <QtWidgets>
#include <QString>
#include <QDataStream>
#include <QtCharts/QtCharts>
#include <vector>
#include <QThread>
#include <QRegExp>

QT_CHARTS_USE_NAMESPACE


dialog::dialog(Threat *obj, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dialog), numbers_graphs(0)
{
    ui->setupUi(this);

    QThread *thread = new QThread(this);
    obj->moveToThread(thread);

    connect(this, SIGNAL(destroyed(QObject*)), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    chart = new QChart();
    chart->legend()->hide();

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->horizontalLayout_2->addWidget(chartView);


    for (int i = 0; i < 16; ++i)
    {
        QLineSeries *s = new QLineSeries();
        series.append(s);
    }

    for (int i = 0; i < 16; ++i)
    {
        chart->addSeries(series[i]);
    }

    connect(this,SIGNAL(AddGraph(const QString&)),
            obj,SLOT(slotAddGraph(const QString&)));


    connect(this,SIGNAL(SetSettings(const QString&, const QString&)),
            obj,SLOT(slotSetSettings(const QString&, const QString&)));

    connect(this,SIGNAL(SetNPort(const QString &, const QString &)),
            obj,SLOT(slotSetNPort(const QString &, const QString &)));

    connect(this,SIGNAL(GetGrahpSettings()),
            obj,SLOT(slotGetGrahpSettings()));

    connect(obj,SIGNAL(ReadyGrahpSettings(const qint64 &, const qint64 &)),
            this,SLOT(slotRearyGraphSettings(const qint64 &, const qint64 &)));

    connect(this,SIGNAL(GetPointOfGraph(const QString &)),
            obj,SLOT(slotGetPointOfGraph(const QString &)));

    connect(obj, SIGNAL(ReadyPointsOfGraph(double*)),
            this, SLOT(slotReadyPointsOfGraph(double*)));

    connect(obj, SIGNAL(ReadyKolAndX(qint64,double*)),
            this, SLOT(slotReadyKolAndX(qint64,double*)));

    connect(obj,SIGNAL(Ready()),
            this,SLOT(slotReady()));

    connect(this,SIGNAL(SetConfigOfGraph(qint64,qint64,qint64)),
            obj, SLOT(slotSetConfigOfGraph(qint64,qint64,qint64)));

    connect(this,SIGNAL(SetKolPoint(qint64)),
            obj, SLOT(slotSetKolPoint(qint64)));

    connect(this,SIGNAL(StartThread(qint16)),
            obj,SLOT(slotStartThread(qint16)));

    connect (obj, SIGNAL(ReadyPointsOfGraphCicle(double*,qint16)),
             this, SLOT(slotReadyPointsOfGraphCicle(double*,qint16)));

    connect (this, SIGNAL(StopThread()),
             obj, SLOT(slotStopThread()));

    thread->start();
}

void dialog::slotReady()
{
    qDebug() << "Поток gui: " << this->thread();
    emit GetGrahpSettings();
}

void dialog::slotRearyGraphSettings(const qint64 &min, const qint64 &max)
{
    chart->createDefaultAxes();
    chart->axisX()->setMax(max);
    chart->axisX()->setMin(min);       
    chart->axisY()->setMax(0);
    chart->axisY()->setMin(-100);
    chart->axisX()->setTitleText("Частота (Гц)");
    chart->axisY()->setTitleText("Амплитуда (дБ)");

    QValueAxis *s = dynamic_cast<QValueAxis*>(chart->axisX());
    s->setLabelFormat("%.3g");
    s->setTickCount(10);

    st_min = min;
    st_max = max;

    curr_max = max;
    curr_min = min;

    ui->lineEdit->setText(QString::number(min));
    ui->lineEdit_2->setText(QString::number(max));
    ui->comboBox_4->setCurrentIndex(8);
}

dialog::~dialog()
{
    delete ui;
    delete[] X;
}



void dialog::on_b_sendtoserver_clicked()
{

    if (ui->radioButton->isChecked())
    {
        emit SetSettings(ui->comboBox_3->currentText(),ui->comboBox->currentText());
    }
    if (ui->radioButton_2->isChecked())
    {
        emit SetNPort(ui->comboBox_3->currentText(),ui->spinBox->text());
        emit SetSettings(ui->comboBox_3->currentText(),ui->comboBox_2->currentText());
    }

    emit GetPointOfGraph(ui->comboBox_3->currentText());
}

void dialog::on_radioButton_clicked()
{
    ui->comboBox_2->setEnabled(false);
    ui->comboBox->setEnabled(true);
    ui->spinBox->setEnabled(false);
}

void dialog::on_radioButton_2_clicked()
{
    ui->comboBox->setEnabled(false);
    ui->comboBox_2->setEnabled(true);
    ui->spinBox->setEnabled(true);
}

void dialog::on_b_sendtoserver_2_clicked()
{
    ui->comboBox_3->setEnabled(true);
    ui->radioButton->setEnabled(true);
    ui->radioButton_2->setEnabled(true);
    ui->b_sendtoserver->setEnabled(true);
    ui->b_sendtoserver_3->setEnabled(true);

    if (numbers_graphs < 16)
    {
        numbers_graphs++;
        ui->comboBox_3->addItem(QString::number(numbers_graphs));
        emit AddGraph(QString::number(numbers_graphs));
    }
    else
    {
        qDebug() << "MaxCount";
    }

    ui->comboBox_3->setCurrentIndex(numbers_graphs-1);
    emit GetPointOfGraph(ui->comboBox_3->currentText());
}

void dialog::on_b_sendtoserver_3_clicked()
{
    if (numbers_graphs > 0)
    {
        series[numbers_graphs-1]->clear();
        ui->comboBox_3->removeItem(numbers_graphs-1);
        numbers_graphs--;
        emit AddGraph(QString::number(numbers_graphs));
    }
    else
    {
        qDebug() << "MinCount";
        ui->b_sendtoserver_3->setEnabled(false);
    }
}


void dialog::slotReadyPointsOfGraph(double *Y)
{
    qint64 s = ui->comboBox_3->currentText().toDouble();
    series[s-1]->clear();

    for (int i = 0; i < kol; ++i)
    {
        series[s-1]->append(X[i],Y[i]);
    }
    delete[] Y;
}

void dialog::slotReadyKolAndX(qint64 k, double *mas)
{
    kol = k;
    X = new double[k];

    for (int i = 0; i < kol; ++i)
    {
        X[i] = mas[i];
    }
    ui->lineEdit_3->setText(QString::number(kol));
}

bool dialog::check_str(QString tmp)
{
    bool norm = true;

    if (tmp.length() != 0 && tmp[0] != '0')
    {

    if (tmp.indexOf(QRegExp("[^\\d,k,M,G]")) != -1)
        norm = false;
    else
    {
        if (tmp.indexOf(QRegExp("(k|M|G)")) != -1)
        {
            if (tmp.indexOf(QRegExp("(k|M|G)")) != tmp.length()-1 || tmp.indexOf(QRegExp("(k|M|G)")) == 0)
                    norm = false;
        }
    }

    }
    else
        norm = false;

    return norm;
}


//НАСТРОЙКА ПАРАМЕТРОВ ГРАФИКА
void dialog::on_lineEdit_editingFinished()
{
    QString tmp = "";
    qint64 localmin;
    qint16 sl = 0;

    tmp = ui->lineEdit->text();
    bool norm = check_str(tmp);

    if (norm == false) ui->lineEdit->setText(QString::number(curr_min));
    else
    {
        if (tmp[tmp.length()-1] == 'k') sl = 3;
        if (tmp[tmp.length()-1] == 'M') sl = 6;
        if (tmp[tmp.length()-1] == 'G') sl = 9;
        if (sl > 1) tmp = tmp.left(tmp.length()-1);

        localmin = tmp.toDouble()*pow(10,sl);

        if (localmin < ui->lineEdit_2->text().toDouble() && localmin >= st_min)
        {
            emit SetConfigOfGraph(localmin,0,0);
            curr_min = localmin;
            chart->axisX()->setMin(localmin);
        }
        else
            ui->lineEdit->setText(QString::number(curr_min));
    }
}

void dialog::on_lineEdit_2_editingFinished()
{
    QString tmp = "";
    qint64 localmax;
    qint16 sl = 0;

    tmp = ui->lineEdit_2->text();
    bool norm = check_str(tmp);

    if (norm == false) ui->lineEdit_2->setText(QString::number(curr_max));
    else
    {
        if (tmp[tmp.length()-1] == 'k') sl = 3;
        if (tmp[tmp.length()-1] == 'M') sl = 6;
        if (tmp[tmp.length()-1] == 'G') sl = 9;
        if (sl > 1) tmp = tmp.left(tmp.length()-1);

        localmax = tmp.toDouble()*pow(10,sl);

        if (localmax > ui->lineEdit->text().toDouble() && localmax <= st_max)
        {
            emit SetConfigOfGraph(0,localmax,0);
            curr_max = localmax;
            chart->axisX()->setMax(localmax);
        }
        else
            ui->lineEdit_2->setText(QString::number(curr_max));
    }
}



void dialog::on_comboBox_4_currentIndexChanged(const QString &t)
{
    qint16 sl = 0;
    QString tmp = t;
    qint64 localpch;

    if (tmp[tmp.length()-1] == 'k') sl = 3;
    if (tmp[tmp.length()-1] == 'M') sl = 6;
    if (tmp[tmp.length()-1] == 'G') sl = 9;
    if (sl > 1) tmp = tmp.left(tmp.length()-1);

    localpch = tmp.toDouble()*pow(10,sl);

    emit SetConfigOfGraph(0,0,localpch);
}

void dialog::on_lineEdit_3_editingFinished()
{
    qint64 current_kol = ui->lineEdit_3->text().toDouble();
    if (current_kol < 1 || current_kol > 500000)
        ui->lineEdit_3->setText(QString::number(kol));
    else
    {
        kol = current_kol;
        emit SetKolPoint(kol);
        delete[] X;
    }
}

//===================================================================================================
void dialog::on_pushButton_clicked()
{
    go = true;
    ui->lineEdit_3->setEnabled(false);
    emit StartThread(numbers_graphs);
}


void dialog::slotReadyPointsOfGraphCicle(double *Y, qint16 n)
{
    if (n == -1)
    {
        if (go == true)
            emit StartThread(numbers_graphs);
    }
    else
    {
    series[n]->clear();

    for (int i = 0; i < kol; ++i)
    {
        series[n]->append(X[i],Y[i]);
    }
    delete[] Y;

    emit StartThread(n);
    }
}

void dialog::on_pushButton_2_clicked()
{
    go = false;
    ui->lineEdit_3->setEnabled(true);
    emit StopThread();
}
