#ifndef MODELS_H
#define MODELS_H

#include <QDialog>
#include "handler.h"

namespace Ui {
class models;
}

class models : public QDialog
{
    Q_OBJECT

public:
    explicit models(Handler *,QWidget *parent = 0);
    ~models();

signals:
    void sig_change_gen(QList<QString>);
    void sig_change_exp(QList<QString>);

private slots:
    void on_pushButton_clicked();

private:
    Handler *handler;
    Ui::models *ui;
};

#endif // MODELS_H
