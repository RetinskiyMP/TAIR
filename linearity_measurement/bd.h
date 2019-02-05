#ifndef BD_H
#define BD_H

#include <QDebug>
#include <QDialog>
#include <QtSql>
#include <QMenu>

#include "QtSql/QSqlDatabase"
#include "QSqlQuery"

namespace Ui {
class BD;
}

class BD : public QDialog
{
    Q_OBJECT
signals:
    void d_update();
public:
    explicit BD(QWidget *parent = 0);
    ~BD();
    //для упр таблицой моделей
    void delete_data(QString);
    void add_data(QStringList);
    void update_data(QString, QStringList);
    //для исп в обработч.
    bool checkModel(QString);
    void addMM(QStringList);
    QStringList getMMnames();
    QStringList getConfig(QString);
    //для упр памятью
    QStringList getDataFromMM(QString);

public slots:
    void slot_delete_memory(QString);

private slots:
    void on_pushButton_4_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_accept_clicked();

private:
    Ui::BD *ui;

    QString addr;
    QSqlDatabase db;
    int rows;

    bool checkRow();
};

#endif // BD_H
