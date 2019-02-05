#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QThread>
#include <QVector>
#include <QtCharts/QtCharts>
#include <handler.h>

namespace Ui {
class GUI;
}

class GUI : public QMainWindow
{
    Q_OBJECT

signals:
    void delete_memory(QString);
    void repeat_start();

public:
    explicit GUI(QWidget *parent = 0);
    void autoscale();
    ~GUI();

public slots:
    void slot_d_connected(bool,QString);
    void slot_d_disconnected(bool);
    void slot_error_model();
    void slot_setXrange(QString,QString);
    void slot_setProgress(int);
    void slot_showAdrr();
    void slot_showModels();
    void slot_setPoint(int,double,double);
    void slot_setPointList(int,QList<QPointF>);
    void slot_d_yes();
    void slot_ready_names(QStringList);
    void slot_error_mm();
    void slot_error_list(QString);
    void slot_delete_series(int,int);

    void slot_addSeriaMM(QLineSeries*);
    void slot_deleteSeriesMM();


private slots:
    void on_lineEdit_editingFinished();
    void on_lineEdit_2_editingFinished();
    void on_pushButton_pause_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_clicked();
    void on_checkBox_mm_clicked(bool checked);
    void on_mm_operations_currentIndexChanged(int index);
    void on_mm_list_activated();
    void on_checkBox_clicked(bool checked);
    void on_autoscale_clicked();
    void on_filter_clicked(bool checked);
    void on_upload_coeff_clicked();
    void on_choose_coeff_clicked();
    void on_autoSave_clicked(bool checked);
    void on_autoSaveCount_valueChanged(int arg1);



private:
    void mmShow_setEnabled(bool);
    void update_axes();

    QString _maxx;
    QString _minx;

    QValueAxis *qva;
    QLabel *error;

    Ui::GUI *ui;
    Handler* handler;
    int conflict;

    QChart     *chart;
    QVector<QLineSeries *> series;

    int autoSaveCount;
    bool autoSave;

    bool readyA;
    bool readyB;
    bool chechReady();
};

#endif // GUI_H
