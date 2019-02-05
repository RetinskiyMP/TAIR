#include <QThread>

#include "gui.h"
#include "dialog.h"
#include "models.h"
#include "ui_gui.h"
#include "bd.h"

#include <QtCharts/QtCharts>
#include <QDate>
#include <QTime>
#include <QtCharts>
#include <QtWidgets>
#include <QRegExp>

GUI::GUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUI)
{
    ui->setupUi(this);
    error = new QLabel();
    ui->statusBar->addWidget( error );
    //иконочки
    QString addr = qApp->applicationDirPath() + QString("/ico");
    setWindowIcon(QIcon(addr+"/glavnaya"));
    ui->toolBox->setItemIcon(0,QIcon(addr+"/info"));
    ui->toolBox->setItemIcon(1,QIcon(addr+"/graph"));
    ui->toolBox->setItemIcon(2,QIcon(addr+"/mm"));
    ui->socket->setIcon(QIcon(addr+"/ip"));
    ui->bd->setIcon(QIcon(addr+"/bd"));


    conflict = 0;
    //график
    chart = new QChart();
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chart->legend()->setAlignment(Qt::AlignRight);
    ui->verticalLayout_2->addWidget(chartView);

    for (int i = 0; i < 16; ++i)
    {
        QLineSeries *s = new QLineSeries();
        series.append(s);
    }

    series[0]->setName("A");  series[4]->setName("C");
    series[1]->setName("B");  series[5]->setName("D");
    series[2]->setName("R1"); series[6]->setName("R3");
    series[3]->setName("R2"); series[7]->setName("R4");

    series[8]->setName("M_A");  series[12]->setName("M_C");
    series[9]->setName("M_B");  series[13]->setName("M_D");
    series[10]->setName("M_R1"); series[14]->setName("M_R3");
    series[11]->setName("M_R2"); series[15]->setName("M_R4");

    for (int i = 0; i < 16; ++i)
    {
        chart->addSeries(series[i]);
    }

    for (int i = 8; i < 16; i++)
    {
        series[i]->hide();
    }

    chart->createDefaultAxes();
    chart->axisY()->setRange(-0.4,0.4);
    chart->axisX()->setRange(-60,10);
    chart->axisX()->setReverse(true);
    qva = dynamic_cast<QValueAxis*>(chart->axisX());
    qva->setTickCount(10);

    //обработчик
    handler = new Handler();

    connect(handler,SIGNAL(d_connected(bool,QString)),this,SLOT(slot_d_connected(bool,QString)), Qt::QueuedConnection);
    connect(handler,SIGNAL(d_disconnected(bool)),this,SLOT(slot_d_disconnected(bool)), Qt::QueuedConnection);
    connect(handler, SIGNAL(error_model()),this,SLOT(slot_error_model()), Qt::QueuedConnection);
    connect(handler, SIGNAL(error_mm()),this,SLOT(slot_error_mm()), Qt::QueuedConnection);
    connect(handler,SIGNAL(setXrange(QString,QString)),SLOT(slot_setXrange(QString,QString)), Qt::QueuedConnection);
    connect(handler,SIGNAL(setProgress(int)),SLOT(slot_setProgress(int)), Qt::QueuedConnection);
    connect(handler,SIGNAL(setPoint(int,double,double)),this,SLOT(slot_setPoint(int,double,double)), Qt::QueuedConnection);
    connect(handler,SIGNAL(d_yes()),this,SLOT(slot_d_yes()), Qt::QueuedConnection);
    connect(handler,SIGNAL(ready_names(QStringList)),this,SLOT(slot_ready_names(QStringList)), Qt::QueuedConnection);
    connect(handler,SIGNAL(setPointList(int,QList<QPointF>)),this,SLOT(slot_setPointList(int,QList<QPointF>)), Qt::QueuedConnection);
    connect(handler,SIGNAL(delete_series(int,int)),this,SLOT(slot_delete_series(int,int)), Qt::QueuedConnection);
    connect(handler,SIGNAL(stop()),this,SLOT(on_pushButton_stop_clicked()), Qt::QueuedConnection);
    connect(handler,SIGNAL(error_list(QString)),this,SLOT(slot_error_list(QString)), Qt::QueuedConnection);

    connect(handler,SIGNAL(addSeriaMM(QLineSeries*)),this,SLOT(slot_addSeriaMM(QLineSeries*)), Qt::QueuedConnection);
    connect(handler,SIGNAL(deleteSeriesMM()),this,SLOT(slot_deleteSeriesMM()), Qt::QueuedConnection);

    connect(this,SIGNAL(delete_memory(QString)),handler,SIGNAL(delete_memory(QString)), Qt::QueuedConnection);
    connect(this,SIGNAL(repeat_start()),handler,SLOT(slot_start()), Qt::QueuedConnection);
    connect(ui->pushButton_start,SIGNAL(clicked(bool)),handler,SLOT(slot_start()), Qt::QueuedConnection);
    connect(ui->pushButton_pause,SIGNAL(clicked(bool)),handler,SLOT(slot_pause()), Qt::QueuedConnection);
    connect(ui->pushButton_stop,SIGNAL(clicked(bool)),handler,SLOT(slot_stop()), Qt::QueuedConnection);

    QThread *thread = new QThread(this);

    handler->moveToThread(thread);

    connect(this, SIGNAL(destroyed(QObject*)), thread, SLOT(quit()));
    connect(this, SIGNAL(destroyed(QObject*)), handler, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()),handler,SLOT(deleteLater()));

    //меню коннекты
    connect(ui->socket, SIGNAL(triggered(bool)),SLOT(slot_showAdrr()));
    connect(ui->bd, SIGNAL(triggered(bool)),SLOT(slot_showModels()));

    //маски
    QRegExp attenRegex( "[-]{0,1}[0-9]{0,3}[.]{0,1}[0-9]{0,3}" );
    QRegExpValidator *validator = new QRegExpValidator( attenRegex, this);
    ui->lineEdit->setValidator( validator );
    ui->lineEdit_2->setValidator( validator );

    //запуск в новом потоке
    thread->start();

    Dialog *window = new Dialog(handler);
    window->exec();
    delete window;
}

GUI::~GUI()
{
    delete qva;
    delete handler;
    delete ui;
    delete chart;
    for (int i = 0; i < series.count(); ++i)
        series[i]->deleteLater();
    series.clear();
}

void GUI::update_axes()
{
    chart->createDefaultAxes();
    chart->axisY()->setRange(ui->lineEdit_2->text().toDouble(),ui->lineEdit->text().toDouble());
    chart->axisX()->setRange(_maxx,_minx);
    chart->axisX()->setReverse(true);
    qva = dynamic_cast<QValueAxis*>(chart->axisX());
    qva->setTickCount(10);

    series[8]->setName("M_A");
    series[9]->setName("M_B");
    series[10]->setName("M_R1");
    series[11]->setName("M_R2");

    if (series.count() == 16)
    {
    series[12]->setName("M_C");
    series[13]->setName("M_D");
    series[14]->setName("M_R3");
    series[15]->setName("M_R4");
    }
}

void GUI::slot_addSeriaMM(QLineSeries* s)
{
    series.append(s);
    chart->addSeries(series.last());
    if (series.count() == 16 || series.count() == 12){
        update_axes();
    }
}

void GUI::slot_deleteSeriesMM()
{
    int id = 15;
    int st = 0;
    int sp = 8;
    if (series.count() == 12) {sp = 4; id = 11;}
    for (int i = st; i < sp; ++i)
    {
        chart->removeSeries(series[id-i]);
        series[id-i]->clear();
        series[id-i]->deleteLater();
        delete series[id-i];
        series.removeLast();
    }
}

void GUI::slot_error_list(QString err)
{
    error->setText(err);
}

void GUI::slot_ready_names(QStringList list)
{
    ui->mm_list->clear();
    ui->mm_list->addItems(list);
}

bool GUI::chechReady()
{
    if (readyA && readyB) return true;
    else
    {
        QMessageBox* pmbx = new QMessageBox("Ошибка",
                                            "Не подключены анализаторы\n",
                                            QMessageBox::Warning,
                                            0,
                                            QMessageBox::Ok, QMessageBox::Escape);

        pmbx->exec();
        delete pmbx;
        return false;
    }
}

void GUI::slot_d_connected(bool type,QString info)
{
    if (type)
    {
        ui->model->setText(info.left(info.length()-1));
        ui->socket1->setText("ON");
        readyA = true;
    }
    else
    {
        QString _model, _ser;

        QStringList list = info.split(',');
        _model = list[1];
        _ser = list[2];
        _model = _model.right(_model.length()-1);
        _ser = _ser.right(_ser.length()-1);

        ui->mm_name->setText(_model+"["+_ser+"]");

        ui->model2->setText(info.left(info.length()-1));
        ui->socket2->setText("ON");
        readyB = true;
    }
}

void GUI::slot_d_disconnected(bool type)
{
    if (type)
    {
        ui->model->setText("");
        ui->socket1->setText("OFF");
        readyA = false;
    }
    else
    {
        ui->model2->setText("");
        ui->socket2->setText("OFF");
        readyB = false;
    }
}


void GUI::slot_showAdrr()
{
    Dialog *window = new Dialog(handler);
    window->exec();
    delete window;
}

void GUI::slot_error_model()
{
    QMessageBox* pmbx = new QMessageBox("Ошибка",
                                        "Модель приемника не найдена в БД.\n",
                                        QMessageBox::Warning,
                                        0,
                                        QMessageBox::Ok, QMessageBox::Escape);

    pmbx->exec();
    delete pmbx;
    ui->label_bd->setText("Нет");
}

void GUI::slot_error_mm()
{
    QMessageBox* pmbx = new QMessageBox("Ошибка",
                                        "Нету текущих данных или конфигурация записи в БД не соответствует текущим конфигурациям измерения.\n",
                                        QMessageBox::Warning,
                                        0,
                                        QMessageBox::Ok, QMessageBox::Escape);

    pmbx->exec();
    delete pmbx;
}

void GUI::slot_showModels()
{
    handler->bd_show();
}

void GUI::slot_d_yes()
{
    ui->label_bd->setText("Да");
}

void GUI::slot_setXrange(QString start, QString stop)
{
    _maxx = start;
    _minx = stop;
    chart->axisX()->setRange(start,stop);
}

void GUI::slot_setPoint(int id, double x, double y)
{
    series[id]->append(x,y);
}

void GUI::slot_delete_series(int start, int count)
{
    for (int i = start; i <= count; ++i)
    {
        series[i]->clear();
    }
}

void GUI::slot_setPointList(int id, QList<QPointF> list)
{
    series[id]->clear();
    series[id]->append(list);
}

void GUI::slot_setProgress(int val)
{
    if (ui->pushButton_stop->isEnabled())
    {
        ui->progressBar->setValue(val);
        if (val == 100)
        {
            ui->save->setEnabled(true);
            ui->menu->setEnabled(true);
            ui->pushButton_pause->setText("||");
            ui->pushButton_start->setEnabled(true);
            ui->pushButton_pause->setEnabled(false);
            ui->pushButton_stop->setEnabled(false);

            if (autoSave && autoSaveCount > 1)
            {
                ui->autoSaveCount->setValue(autoSaveCount-1);
                on_pushButton_clicked();
                on_pushButton_stop_clicked();
                on_pushButton_start_clicked();
                emit repeat_start();
            }
            else
            {
                if (autoSave && autoSaveCount == 1)
                {
                    ui->autoSaveCount->setValue(autoSaveCount-1);
                    on_pushButton_clicked();
                }
                ui->autoSave->setChecked(false);
            }
        }
    }
}

void GUI::on_lineEdit_editingFinished()
{
    chart->axisY()->setMax(ui->lineEdit->text().toDouble());
}

void GUI::on_lineEdit_2_editingFinished()
{
    chart->axisY()->setMin(ui->lineEdit_2->text().toDouble());
}

void GUI::on_pushButton_pause_clicked()
{
    if (ui->pushButton_pause->text() == "||")
        ui->pushButton_pause->setText("...");
    else ui->pushButton_pause->setText("||");
}

void GUI::on_pushButton_start_clicked()
{
    if (chechReady())
    {
        ui->save->setEnabled(false);

        for (int i = 0; i < 8; ++i)
            series[i]->clear();

        ui->menu->setEnabled(false);
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_pause->setEnabled(true);
        ui->pushButton_stop->setEnabled(true);
    }
}

void GUI::on_pushButton_stop_clicked()
{
    for (int i = 0; i < 8; ++i)
        series[i]->clear();

    ui->menu->setEnabled(true);
    ui->pushButton_pause->setText("||");
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_pause->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);
    ui->progressBar->setValue(0);
}


/**                                                 MEMORY                                             **/
/**                                                                                                    **/


void GUI::on_pushButton_clicked()
{
    QString _id="";
    QString _name = ui->mm_name->text();

    while (ui->mm_list->findText(_name+_id) != -1){
        _id="."+QString::number(conflict);
        conflict++;
    }

    ui->mm_list->addItem(_name+_id);
    handler->save(ui->mm_list->itemText(ui->mm_list->count()-1));
}

void GUI::on_checkBox_mm_clicked(bool checked)
{
    if (checked)
    {
        ui->ypr->setEnabled(true);
        ui->checkBox->setEnabled(true);
        on_mm_operations_currentIndexChanged(ui->mm_operations->currentIndex());
    }
    else
    {
        handler->set_default_data();
        ui->checkBox->setChecked(false);
        on_checkBox_clicked(false);
        ui->checkBox->setEnabled(false);
        ui->ypr->setEnabled(false);
        mmShow_setEnabled(false);
    }
}

void GUI::mmShow_setEnabled(bool val)
{
    if (val)
    {
        for (int i = 8; i < series.count(); ++i)
        {
            series[i]->show();
        }
        handler->mmShow(ui->mm_list->currentText());
    }
    else
    {
        for (int i = 8; i < series.count(); i++)
            series[i]->clear();

        for (int i = 8; i < series.count(); ++i)
        {
            series[i]->hide();
        }
    }
}

void GUI::on_mm_list_activated()
{
    if (ui->mm_operations->currentIndex() != 0) mmShow_setEnabled(false);
    on_mm_operations_currentIndexChanged(ui->mm_operations->currentIndex());
}

void GUI::on_mm_operations_currentIndexChanged(int index)
{
    if (index != 0) mmShow_setEnabled(false);

    switch (index) {
    case 0:{
        handler->set_default_data();
        mmShow_setEnabled(true);}
        break;
    case 5:
    {
        handler->set_default_data();
        QMessageBox* pmbx = new QMessageBox("Подтверждение",
                                            "Удалить сохранение?\n",
                                            QMessageBox::Warning,
                                            QMessageBox::Yes,
                                            QMessageBox::No, QMessageBox::Escape);

        pmbx->setButtonText(QMessageBox::Yes,"Да");
        pmbx->setButtonText(QMessageBox::No, "Нет");

        int n = pmbx->exec();
        delete pmbx;

        if (n == 3)
        {
            emit delete_memory(ui->mm_list->currentText());
            ui->mm_list->removeItem(ui->mm_list->currentIndex());
        }
    }
        break;
    default:
        handler->do_operation(ui->mm_list->currentText(),index);
        break;
    }
}


void GUI::on_checkBox_clicked(bool checked)
{
    if (checked)
    {
        for (int i = 0; i < 8; ++i)
        {
            series[i]->hide();
        }
    }
    else
    {
        for (int i = 0; i < 8; ++i)
        {
            series[i]->show();
        }
    }
}


void GUI::on_autoscale_clicked()
{
    double max = 0.001, min = -0.001;
    int start;
    int stop;

    if (!ui->checkBox_mm->isChecked()){
        start = 0;
        stop = 8;
    }
    else
    {
        if(!ui->checkBox->isChecked()){
            start = 0;
            stop = series.count();
        }
        else
        {
            start = 8;
            stop = series.count();
        }
    }


    for (int i = start; i < stop; ++i)
    {
        QLineSeries* ser = series[i];
        if (ser->count() > 1)
        {
            for (int j = 0; j < ser->count(); ++j)
            {
                if (ser->at(j).y())
                {
                    if (ser->at(j).y() < min) min = ser->at(j).y();
                    if (ser->at(j).y() > max) max = ser->at(j).y();
                }
            }
        }
    }

    double delta = ( max - min ) * 0.05;
    chart->axisY()->setRange(min - delta,max + delta);
    ui->lineEdit->setText(QString::number(max+delta));
    ui->lineEdit_2->setText(QString::number(min-delta));
}

void GUI::on_filter_clicked(bool checked)
{
        handler->getFullNames(checked);
}

void GUI::on_upload_coeff_clicked()
{
    QStringList list;
    for (int i = 0; i < ui->mm_list->count(); ++i)
    {
        QString tmp = ui->mm_list->itemText(i);
        list.append(tmp);
    }
    handler->upload_coeff(list);
}

void GUI::on_choose_coeff_clicked()
{
    handler->mm_show();
}

void GUI::on_autoSave_clicked(bool checked)
{
    if (checked)
    {
        autoSave = true;
    }
    else
    {
        autoSave = false;
    }
}

void GUI::on_autoSaveCount_valueChanged(int arg1)
{
    autoSaveCount = arg1;
}

