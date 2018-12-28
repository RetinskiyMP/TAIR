#ifndef DIALOG_H
#define DIALOG_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QVector>
#include <QtCharts/QtCharts>
#include "threat.h"


namespace Ui {
class dialog;
}

class dialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit dialog(Threat *obj,QWidget *parent = 0);
    ~dialog();

private:
    Ui::dialog *ui;
    qint16 numbers_graphs;
    QVector<QLineSeries*> series;
    QChart *chart;

    bool go;

    double *X;
    qint64 kol;

    qint64 st_max;
    qint64 st_min;

    qint64 curr_max;
    qint64 curr_min;

    bool check_str(QString);

signals:
    void AddGraph(const QString&);
    void SetSettings(const QString&, const QString&);
    void SetNPort(const QString &, const QString &);
    void GetGrahpSettings();
    void GetPointOfGraph(const QString &);
    void SetConfigOfGraph(qint64, qint64, qint64);
    void SetKolPoint(qint64);
    void StartThread(qint16);
    void StopThread();

public slots:
    void slotRearyGraphSettings(const qint64 &min, const qint64 &max);
    void slotReadyPointsOfGraph(double *);
    void slotReadyKolAndX(qint64, double *);
    void slotReady();
    void slotReadyPointsOfGraphCicle(double *Y, qint16 n);

private slots:
    void on_b_sendtoserver_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_b_sendtoserver_2_clicked();
    void on_b_sendtoserver_3_clicked();
    void on_lineEdit_editingFinished();
    void on_lineEdit_2_editingFinished();

    void on_comboBox_4_currentIndexChanged(const QString &arg1);
    void on_lineEdit_3_editingFinished();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // DIALOG_H
